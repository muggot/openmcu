/*
 * xmpp_roster.h
 *
 * Extensible Messaging and Presence Protocol (XMPP) IM
 * Roster management classes
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
 * $Log: xmpp_roster.h,v $
 * Revision 1.2  2004/05/09 07:23:46  rjongbloed
 * More work on XMPP, thanks Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.1  2004/04/26 01:51:57  rjongbloed
 * More implementation of XMPP, thanks a lot to Federico Pinna & Reitek S.p.A.
 *
 *
 */

#ifndef _XMPP_ROSTER
#define _XMPP_ROSTER

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptclib/xmpp_c2s.h>

#if P_EXPAT

///////////////////////////////////////////////////////

namespace XMPP
{
  class Roster : public PObject
  {
    PCLASSINFO(Roster, PObject);
  public:

    enum ItemType { // Subscription type
      None,
      To,
      From,
      Both,
      Unknown = 999
    };

    class Item : public PObject
    {
      PCLASSINFO(Item, PObject);
      PDICTIONARY(PresenceInfo, PString, Presence);

    public:
      Item(PXMLElement * item = 0);
      Item(PXMLElement& item);
      Item(const JID& jid, ItemType type, const PString& group, const PString& name = PString::Empty());

      const JID&          GetJID() const        { return m_JID; }
      ItemType            GetType() const       { return m_Type; }
      const PString&      GetName() const       { return m_Name; }
      const PStringSet&   GetGroups() const     { return m_Groups; }
      const PresenceInfo& GetPresence() const   { return m_Presence; }

      virtual void  SetJID(const JID& jid, BOOL dirty = TRUE)
                                                { m_JID = jid; if (dirty) SetDirty(); }
      virtual void  SetType(ItemType type, BOOL dirty = TRUE)
                                                { m_Type = type; if (dirty) SetDirty(); }
      virtual void  SetName(const PString& name, BOOL dirty = TRUE) 
                                                { m_Name = name; if (dirty) SetDirty(); }

      virtual void  AddGroup(const PString& group, BOOL dirty = TRUE);
      virtual void  RemoveGroup(const PString& group, BOOL dirty = TRUE);

      virtual void  SetPresence(const Presence& p);

      void SetDirty(BOOL b = TRUE) { m_IsDirty = b; }

      /** This operator will set the dirty flag
       */
      Item & operator=(
        const PXMLElement& item
      );

      virtual PXMLElement * AsXML(PXMLElement * parent) const;

    protected:
      BareJID     m_JID;
      ItemType    m_Type;
      PString     m_Name;
      PStringSet  m_Groups;

      // The item's presence state: for each resource (the key to the dictionary) a
      // a presence stanza if kept.
      PDictionary<PString, Presence> m_Presence;

      BOOL        m_IsDirty; // item modified locally, server needs to be updated
    };
    PLIST(ItemList, Item);

  public:
    Roster(XMPP::C2S::StreamHandler * handler = 0);
    ~Roster();

    const ItemList& GetItems() const    { return m_Items; }

    virtual Item * FindItem(const PString& jid);

    virtual BOOL SetItem(Item * item, BOOL localOnly = FALSE);
    virtual BOOL RemoveItem(const PString& jid, BOOL localOnly = FALSE);
    virtual BOOL RemoveItem(Item * item, BOOL localOnly = FALSE);

    virtual void  Attach(XMPP::C2S::StreamHandler * handler);
    virtual void  Detach();
    virtual void  Refresh(BOOL sendPresence = TRUE);

    virtual PNotifierList& ItemChangedHandlers()    { return m_ItemChangedHandlers; }
    virtual PNotifierList& RosterChangedHandlers()  { return m_RosterChangedHandlers; }

  protected:
    PDECLARE_NOTIFIER(XMPP::C2S::StreamHandler, Roster, OnSessionEstablished);
    PDECLARE_NOTIFIER(XMPP::C2S::StreamHandler, Roster, OnSessionReleased);
    PDECLARE_NOTIFIER(XMPP::Presence, Roster, OnPresence);
    PDECLARE_NOTIFIER(XMPP::IQ, Roster, OnIQ);

    ItemList m_Items;
    XMPP::C2S::StreamHandler * m_Handler;
    PNotifierList m_ItemChangedHandlers;
    PNotifierList m_RosterChangedHandlers;
  };

} // namespace XMPP


#endif  // P_EXPAT

#endif  // _XMPP_ROSTER

// End of File ///////////////////////////////////////////////////////////////

