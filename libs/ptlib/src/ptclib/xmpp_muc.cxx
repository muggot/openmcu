/*
 * xmpp_muc.cxx
 *
 * Extensible Messaging and Presence Protocol (XMPP)
 * JEP-0045 Multi-User Chat
 *
 * Portable Windows Library
 *
 * Copyright (c) 2004 Reitek S.p.A.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: xmpp_muc.cxx,v $
 * Revision 1.4  2007/09/08 11:34:29  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.3  2007/04/10 05:08:48  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.2  2005/08/04 03:19:08  dereksmithies
 * Add xmpp_muc (XMPP multi user conference) to the compile process for unix.
 * Correct compile errors under unix.
 *
 * Revision 1.1  2004/05/09 07:23:50  rjongbloed
 * More work on XMPP, thanks Federico Pinna and Reitek S.p.A.
 *
 *
 */

#ifdef __GNUC__
#pragma implementation "xmpp_muc.h"
#endif

#include <ptlib.h>
#include <ptclib/xmpp_muc.h>

#define new PNEW


#if P_EXPAT

XMPP::MUC::User::User()
{

}

XMPP::MUC::User::~User()
{

}


PObject::Comparison XMPP::MUC::User::Compare(const PObject & obj) const
{
  if (PIsDescendant(&obj, XMPP::MUC::User))
    return m_Nick.Compare(((const XMPP::MUC::User&)obj).m_Nick);
  else if (PIsDescendant(&obj, PString))
    return m_Nick.Compare((const PString&)obj);

  PAssertAlways(PInvalidCast);
  return PObject::LessThan;
}

///////////////////////////////////////////////////////

const PString & XMPP::MUC::NamespaceTag() { static PString s = "http://jabber.org/protocol/muc"; return s; }
const PString & XMPP::MUC::User::NamespaceTag() { static PString s = "http://jabber.org/protocol/muc#user"; return s; }

XMPP::MUC::Room::Room(C2S::StreamHandler * handler, const JID& jid, const PString& nick)
  : m_Handler(handler), m_RoomJID(jid)
{
  PCREATE_SMART_NOTIFIEE;

  if (PAssertNULL(m_Handler) == NULL)
    return;

  m_User.m_Nick = nick;
  m_User.m_Role = XMPP::MUC::User::None;
  m_User.m_Affiliation = XMPP::MUC::User::None_a;

  m_RoomJID.SetResource(PString::Empty());
  m_Handler->SessionReleasedHandlers().Add(new PCREATE_SMART_NOTIFIER(OnSessionReleased));
  m_Handler->PresenceHandlers().Add(new PCREATE_SMART_NOTIFIER(OnPresence));
  m_Handler->MessageSenderHandlers(m_RoomJID).Add(new PCREATE_SMART_NOTIFIER(OnMessage));
}


BOOL XMPP::MUC::Room::Enter()
{
  if (PAssertNULL(m_Handler) == NULL)
    return FALSE;

  JID ourUser(m_RoomJID);
  ourUser.SetResource(m_User.m_Nick);

  XMPP::Presence pre;
  pre.SetTo(ourUser);
  pre.SetStatus("Available");
  pre.SetPriority(0);

  PXMLElement * x = new PXMLElement(NULL, "x");
  x->SetAttribute(XMPP::NamespaceTag(), XMPP::MUC::NamespaceTag());

  pre.AddElement(x);

  return m_Handler->Write(pre);
}


BOOL XMPP::MUC::Room::Leave()
{
  if (PAssertNULL(m_Handler) == NULL)
    return FALSE;

  XMPP::Presence pre;
  pre.SetTo(m_RoomJID);
  pre.SetType(XMPP::Presence::Unavailable);

  return m_Handler->Write(pre);
}


BOOL XMPP::MUC::Room::SendMessage(const PString& msg)
{
  XMPP::Message _msg;
  _msg.SetBody(msg);

  return SendMessage(_msg);
}


BOOL XMPP::MUC::Room::SendMessage(Message& msg)
{
  if (PAssertNULL(m_Handler) == NULL)
    return FALSE;

  msg.SetTo(m_RoomJID);
  msg.SetType(XMPP::Message::GroupChat);

  return m_Handler->Write(msg);
}


void XMPP::MUC::Room::OnMessage(Message& msg)
{ m_MessageHandlers.Fire(msg); }

void XMPP::MUC::Room::OnRoomJoined()
{ m_RoomJoinedHandlers.Fire(*this); }


void XMPP::MUC::Room::OnRoomLeft()
{ m_RoomLeftHandlers.Fire(*this); }


void XMPP::MUC::Room::OnUserAdded(User& user)
{ m_UserAddedHandlers.Fire(user); }


void XMPP::MUC::Room::OnUserRemoved(User& user)
{ m_UserRemovedHandlers.Fire(user); }


void XMPP::MUC::Room::OnUserChanged(User& user)
{ m_UserChangedHandlers.Fire(user); }


void XMPP::MUC::Room::OnSessionReleased(C2S::StreamHandler&, INT)
{
  m_User.m_Role = XMPP::MUC::User::None;
  m_User.m_Affiliation = XMPP::MUC::User::None_a;
  OnRoomLeft();
}


void XMPP::MUC::Room::OnMessage(XMPP::Message& msg, INT)
{
  OnMessage(msg);
}


void XMPP::MUC::Room::OnPresence(XMPP::Presence& msg, INT)
{
  JID from = msg.GetFrom();
  PString res = from.GetResource();

  if (m_RoomJID != from) // It's not about this room
    return;

  XMPP::MUC::User::Role role = User::Unknown;
  XMPP::MUC::User::Affiliation affiliation = User::Unknown_a;

  PXMLElement * x = msg.GetElement("x");

  if (x != NULL && x->GetAttribute(XMPP::NamespaceTag()) == XMPP::MUC::User::NamespaceTag()) {
    PXMLElement * item = x->GetElement("item");

    if (item != NULL) {
      PString attr = item->GetAttribute("role");
      if (attr *= "none")
        role = XMPP::MUC::User::None;
      else if (attr *= "moderator")
        role = XMPP::MUC::User::Moderator;
      else if (attr *= "participant")
        role = XMPP::MUC::User::Participant;
      else if (attr *= "visitor")
        role = XMPP::MUC::User::Visitor;

      attr = item->GetAttribute("affiliation");
      if (attr *= "none")
        affiliation = XMPP::MUC::User::None_a;
      else if (attr *= "owner")
        affiliation = XMPP::MUC::User::Owner;
      else if (attr *= "admin")
        affiliation = XMPP::MUC::User::Admin;
      else if (attr *= "member")
        affiliation = XMPP::MUC::User::Member;
      else if (attr *= "outcast")
        affiliation = XMPP::MUC::User::Outcast;
    }
  }

  if (res == m_User.m_Nick) { // is this about us?
    if (msg.GetType() == XMPP::Presence::Unavailable) {
      OnRoomLeft();
      m_User.m_Role = XMPP::MUC::User::None;
      m_User.m_Affiliation = XMPP::MUC::User::None_a;
    }
    else if (m_User.m_Role == XMPP::MUC::User::None) {
      m_User.m_Role = role;
      m_User.m_Affiliation = affiliation;
      OnRoomJoined();
    }
    else {
      m_User.m_Role = role;
      m_User.m_Affiliation = affiliation;
      // TODO: raise a "LocalUserChanged" event...
    }
  }
  else {
    XMPP::MUC::User user;
    user.m_Nick = res;
    user.m_Role = role;
    user.m_Affiliation = affiliation;

    XMPP::MUC::User * puser;

    PINDEX i = m_OtherUsers.GetValuesIndex(user);

    if (i != P_MAX_INDEX) { // known user?
      if (msg.GetType() == XMPP::Presence::Unavailable) {
        OnUserRemoved(user);
        m_OtherUsers.RemoveAt(i);
      }
      else {
        puser = (XMPP::MUC::User *)m_OtherUsers.GetAt(i);
        puser->m_Role = role;
        puser->m_Affiliation = affiliation;
        OnUserChanged(user);
      }
    }
    else { // new user!
      puser = new XMPP::MUC::User;
      puser->m_Role = role;
      puser->m_Affiliation = affiliation;
      m_OtherUsers.Append(puser);
      OnUserAdded(user);
    }
  }
}

#endif // P_EXPAT

// End of File ///////////////////////////////////////////////////////////////



