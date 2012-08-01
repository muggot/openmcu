/*
 * xmpp_muc.h
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
 * $Log: xmpp_muc.h,v $
 * Revision 1.4  2007/04/10 05:08:46  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.3  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.2  2005/08/04 03:19:07  dereksmithies
 * Add xmpp_muc (XMPP multi user conference) to the compile process for unix.
 * Correct compile errors under unix.
 *
 * Revision 1.1  2004/05/09 07:23:46  rjongbloed
 * More work on XMPP, thanks Federico Pinna and Reitek S.p.A.
 *
 *
 */

#ifndef _XMPP_MUC
#define _XMPP_MUC

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptclib/xmpp_c2s.h>

#if P_EXPAT

///////////////////////////////////////////////////////

namespace XMPP
{
  namespace MUC
  {
    extern const PString & NamespaceTag();

    class User : public PObject
    {
      PCLASSINFO(User, PObject);
    public:
      User();
      ~User();

      static const PString & NamespaceTag();

      enum Role {
        None,
        Moderator,
        Participant,
        Visitor,
        Unknown = 999
      };

      enum Affiliation {
        None_a,
        Owner,
        Admin,
        Member,
        Outcast,
        Unknown_a = 999
      };

      PString       m_Nick;
      Role          m_Role;
      Affiliation   m_Affiliation;

      Comparison Compare(const PObject & obj) const;
    };
    PSORTED_LIST(Users, User);

    class Room : public PObject
    {
      PCLASSINFO(Room, PObject);
      PDECLARE_SMART_NOTIFIEE;
    public:
      Room(C2S::StreamHandler * handler,  ///< The C2S stream handler
           const JID& jid,                ///< The room's jid
           const PString& nick);          ///< Our user in the room

      const User&   GetUser() const         { return m_User; }
      const Users&  GetOtherUsers() const   { return m_OtherUsers; }

      virtual BOOL  Enter();
      virtual BOOL  Leave();
      virtual BOOL  SendMessage(const PString& msg);
      virtual BOOL  SendMessage(Message& msg);

      // Event methods
      virtual void  OnMessage(Message& msg);
      virtual void  OnRoomJoined();
      virtual void  OnRoomLeft();
      virtual void  OnUserAdded(User& user);
      virtual void  OnUserRemoved(User& user);
      virtual void  OnUserChanged(User& user);

    protected:
      PDECLARE_SMART_NOTIFIER(C2S::StreamHandler, Room, OnSessionReleased);
      PDECLARE_SMART_NOTIFIER(Message, Room, OnMessage);
      PDECLARE_SMART_NOTIFIER(Presence, Room, OnPresence);

      C2S::StreamHandler *  m_Handler;
      BareJID               m_RoomJID;
      User                  m_User;
      Users                 m_OtherUsers;

      PNotifierList         m_MessageHandlers;
      PNotifierList         m_RoomJoinedHandlers;
      PNotifierList         m_RoomLeftHandlers;
      PNotifierList         m_UserAddedHandlers;
      PNotifierList         m_UserRemovedHandlers;
      PNotifierList         m_UserChangedHandlers;
    };

  }  // namespace MUC
} // namespace XMPP


#endif  // P_EXPAT

#endif  // _XMPP_MUC

// End of File ///////////////////////////////////////////////////////////////



