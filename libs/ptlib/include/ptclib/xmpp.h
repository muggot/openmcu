/*
 * xmpp.h
 *
 * Extensible Messaging and Presence Protocol (XMPP) Core
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
 * $Log: xmpp.h,v $
 * Revision 1.6  2007/04/10 05:08:46  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.5  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.4  2004/11/11 07:34:50  csoutheren
 * Added #include <ptlib.h>
 *
 * Revision 1.3  2004/05/09 07:23:46  rjongbloed
 * More work on XMPP, thanks Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.2  2004/04/26 01:51:57  rjongbloed
 * More implementation of XMPP, thanks a lot to Federico Pinna & Reitek S.p.A.
 *
 * Revision 1.1  2004/04/22 12:31:00  rjongbloed
 * Added PNotifier extensions and XMPP (Jabber) support,
 *   thanks to Federico Pinna and Reitek S.p.A.
 *
 *
 */

#ifndef _XMPP
#define _XMPP

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>

#if P_EXPAT

#include <ptclib/pxml.h>
#include <ptlib/notifier_ext.h>


///////////////////////////////////////////////////////

namespace XMPP
{
  /** Various constant strings
   */
  extern const PString & LanguageTag();
  extern const PString & NamespaceTag();
  extern const PString & MessageStanzaTag();
  extern const PString & PresenceStanzaTag();
  extern const PString & IQStanzaTag();
  extern const PString & IQQueryTag();

  class JID : public PObject
  {
    PCLASSINFO(JID, PObject);

  public:
    JID(const char * jid = 0);
    JID(const PString& jid);
    JID(const PString& user, const PString& server, const PString& resource = PString::Empty());

    virtual Comparison Compare(
      const PObject & obj   ///< Object to compare against.
      ) const;

    JID& operator=(
      const PString & jid  ///< New JID to assign.
      );

    operator const PString&() const;

    virtual PObject * Clone() const { return new JID(m_JID); }

    PString   GetUser() const         { return m_User; }
    PString   GetServer() const       { return m_Server; }

    virtual PString GetResource() const { return m_Resource; }

    virtual void SetUser(const PString& user);
    virtual void SetServer(const PString& server);
    virtual void SetResource(const PString& resource);

    virtual BOOL IsBare() const       { return m_Resource.IsEmpty(); }
    virtual void PrintOn(ostream & strm) const;

  protected:
    virtual void ParseJID(const PString& jid);
    virtual void BuildJID() const;

    PString   m_User;
    PString   m_Server;
    PString   m_Resource;

    mutable PString m_JID;
    mutable BOOL    m_IsDirty;
  };

  // A bare jid is a jid with no resource
  class BareJID : public JID
  {
    PCLASSINFO(BareJID, JID);

  public:
    BareJID(const char * jid = 0) : JID(jid) { }
    BareJID(const PString& jid) : JID(jid) { }
    BareJID(const PString& user, const PString& server, const PString& resource = PString::Empty())
      : JID(user, server, resource) { }

    virtual Comparison Compare(
      const PObject & obj   ///< Object to compare against.
      ) const;

    BareJID& operator=(
      const PString & jid  ///< New JID to assign.
      );

    virtual PObject * Clone() const { return new BareJID(m_JID); }
    virtual PString GetResource() const { return PString::Empty(); }
    virtual void SetResource(const PString&) { }
    virtual BOOL IsBare() const { return TRUE; }
  };

  /** This interface is the base class of each XMPP transport class

  Derived classes might include an XMPP TCP transport as well as
  classes to handle XMPP incapsulated in SIP messages.
  */
  class Transport : public PIndirectChannel
  {
    PCLASSINFO(Transport, PIndirectChannel);

  public:
    virtual BOOL Open() = 0;
    virtual BOOL Close() = 0;
  };


/** This class represents a XMPP stream, i.e. a XML message exchange
    between XMPP entities
 */
  class Stream : public PIndirectChannel
  {
    PCLASSINFO(Stream, PIndirectChannel);

  public:
    Stream(Transport * transport = 0);
    ~Stream();

    virtual BOOL        OnOpen()            { return m_OpenHandlers.Fire(*this); }
    PNotifierList&      OpenHandlers()      { return m_OpenHandlers; }

    virtual BOOL        Close();
    virtual void        OnClose()           { m_CloseHandlers.Fire(*this); }
    PNotifierList&      CloseHandlers()     { return m_CloseHandlers; }

    virtual BOOL        Write(const void * buf, PINDEX len);
    virtual BOOL        Write(const PString& data);
    virtual BOOL        Write(const PXML& pdu);

    /** Read a XMPP stanza from the stream
    */
    virtual PXML *      Read();

    /** Reset the parser. The will delete and re-instantiate the
    XML stream parser.
    */
    virtual void        Reset();
    PXMLStreamParser *  GetParser()     { return m_Parser; }

  protected:
    PXMLStreamParser *  m_Parser;
    PNotifierList       m_OpenHandlers;
    PNotifierList       m_CloseHandlers;
  };


  class BaseStreamHandler : public PThread
  {
    PCLASSINFO(BaseStreamHandler, PThread);

  public:
    BaseStreamHandler();
    ~BaseStreamHandler();

    virtual BOOL        Start(Transport * transport = 0);
    virtual BOOL        Stop(const PString& error = PString::Empty());

    void                SetAutoReconnect(BOOL b = TRUE, long timeout = 1000);

    PNotifierList&      ElementHandlers()   { return m_ElementHandlers; }
    Stream *            GetStream()         { return m_Stream; }

    virtual BOOL        Write(const void * buf, PINDEX len);
    virtual BOOL        Write(const PString& data);
    virtual BOOL        Write(const PXML& pdu);
    virtual void        OnElement(PXML& pdu);

    virtual void        Main();

  protected:
    PDECLARE_NOTIFIER(Stream, BaseStreamHandler, OnOpen);
    PDECLARE_NOTIFIER(Stream, BaseStreamHandler, OnClose);

    Stream *        m_Stream;
    BOOL            m_AutoReconnect;
    PTimeInterval   m_ReconnectTimeout;

    PNotifierList   m_ElementHandlers;
  };


  /** XMPP stanzas: the following classes represent the three
    stanzas (PDUs) defined by the xmpp protocol
   */

  class Stanza : public PXML
  {
    PCLASSINFO(Stanza, PXML)

  public:
    /** Various constant strings
    */
    static const PString & IDTag();
    static const PString & FromTag();
    static const PString & ToTag();

    virtual BOOL IsValid() const = 0;

    virtual PString GetID() const;
    virtual PString GetFrom() const;
    virtual PString GetTo() const;

    virtual void SetID(const PString& id);
    virtual void SetFrom(const PString& from);
    virtual void SetTo(const PString& to);

    virtual PXMLElement * GetElement(const PString& name, PINDEX i = 0);
    virtual void AddElement(PXMLElement * elem);

    static PString GenerateID();
  };

  PLIST(StanzaList, Stanza);


  class Message : public Stanza
  {
    PCLASSINFO(Message, Stanza)

  public:
    enum MessageType {
      Normal,
      Chat,
      Error,
      GroupChat,
      HeadLine,
      Unknown = 999
    };

    /** Various constant strings
    */
    static const PString & TypeTag();
    static const PString & SubjectTag();
    static const PString & BodyTag();
    static const PString & ThreadTag();

    /** Construct a new empty message
    */
    Message();

    /** Construct a message from a (received) xml PDU.
    The root of the pdu MUST be a message stanza.
    NOTE: the root of the pdu is cloned.
    */
    Message(PXML& pdu);
    Message(PXML * pdu);

    virtual BOOL IsValid() const;
    static BOOL IsValid(const PXML * pdu);

    virtual MessageType GetType(PString * typeName = 0) const;
    virtual PString     GetLanguage() const;

    /** Get the subject for the specified language. The default subject (if any)
    is returned in case no language is specified or a matching one cannot be
    found
    */
    virtual PString GetSubject(const PString& lang = PString::Empty());
    virtual PString GetBody(const PString& lang = PString::Empty());
    virtual PString GetThread();

    virtual PXMLElement * GetSubjectElement(const PString& lang = PString::Empty());
    virtual PXMLElement * GetBodyElement(const PString& lang = PString::Empty());

    virtual void SetType(MessageType type);
    virtual void SetType(const PString& type); // custom, possibly non standard, type
    virtual void SetLanguage(const PString& lang);

    virtual void SetSubject(const PString& subj, const PString& lang = PString::Empty());
    virtual void SetBody(const PString& body, const PString& lang = PString::Empty());
    virtual void SetThread(const PString& thrd);
  };


  class Presence : public Stanza
  {
    PCLASSINFO(Presence, Stanza)

  public:
    enum PresenceType {
      Available,
      Unavailable,
      Subscribe,
      Subscribed,
      Unsubscribe,
      Unsubscribed,
      Probe,
      Error,
      Unknown = 999
    };

    enum ShowType {
      Online,
      Away,
      Chat,
      DND,
      XA,
      Other = 999
    };

    /** Various constant strings
    */
    static const PString & TypeTag();
    static const PString & ShowTag();
    static const PString & StatusTag();
    static const PString & PriorityTag();

    /** Construct a new empty presence
    */
    Presence();

    /** Construct a presence from a (received) xml PDU.
    The root of the pdu MUST be a presence stanza.
    NOTE: the root of the pdu is cloned.
    */
    Presence(PXML& pdu);
    Presence(PXML * pdu);

    virtual BOOL IsValid() const;
    static BOOL IsValid(const PXML * pdu);

    virtual PresenceType GetType(PString * typeName = 0) const;
    virtual ShowType     GetShow(PString * showName = 0) const;
    virtual BYTE         GetPriority() const;

    /** Get the status for the specified language. The default status (if any)
    is returned in case no language is specified or a matching one cannot be
    found
    */
    virtual PString GetStatus(const PString& lang = PString::Empty());
    virtual PXMLElement * GetStatusElement(const PString& lang = PString::Empty());

    virtual void SetType(PresenceType type);
    virtual void SetType(const PString& type); // custom, possibly non standard, type
    virtual void SetShow(ShowType show);
    virtual void SetShow(const PString& show); // custom, possibly non standard, type
    virtual void SetPriority(BYTE priority);

    virtual void SetStatus(const PString& status, const PString& lang = PString::Empty());
  };


  class IQ : public Stanza
  {
    PCLASSINFO(IQ, Stanza)

  public:
    enum IQType {
      Get,
      Set,
      Result,
      Error,
      Unknown = 999
    };

    /** Various constant strings
    */
    static const PString & TypeTag();

    IQ(IQType type, PXMLElement * body = 0);
    IQ(PXML& pdu);
    IQ(PXML * pdu);
    ~IQ();

    virtual BOOL IsValid() const;
    static BOOL IsValid(const PXML * pdu);

    /** This method signals that the message was taken care of
    If the stream handler, after firing all the notifiers finds
    that an iq set/get pdu has not being processed, it returns
    an error to the sender
    */
    void SetProcessed()             { m_Processed = TRUE; }
    BOOL HasBeenProcessed() const   { return m_Processed; }

    virtual IQType        GetType(PString * typeName = 0) const;
    virtual PXMLElement * GetBody();

    virtual void SetType(IQType type);
    virtual void SetType(const PString& type); // custom, possibly non standard, type
    virtual void SetBody(PXMLElement * body);

    // If the this message is response, returns a pointer to the
    // original set/get message
    virtual IQ *  GetOriginalMessage() const      { return m_OriginalIQ; }
    virtual void  SetOriginalMessage(IQ * iq);

    /** Creates a new response iq for this message (that must
    be of the set/get type!)
    */
    virtual IQ *  BuildResult() const;

    /** Creates an error response for this message
    */
    virtual IQ *  BuildError(const PString& type, const PString& code) const;

    virtual PNotifierList GetResponseHandlers()   { return m_ResponseHandlers; }

  protected:
    BOOL            m_Processed;
    IQ *            m_OriginalIQ;
    PNotifierList   m_ResponseHandlers;
  };
  /** JEP-0030 Service Discovery classes
   */
  namespace Disco
  {
    class Item : public PObject
    {
      PCLASSINFO(Item, PObject);
    public:
      Item(PXMLElement * item);
      Item(const PString& jid, const PString& node = PString::Empty());

      const JID&      GetJID() const    { return m_JID; }
      const PString&  GetNode() const   { return m_Node; }

      PXMLElement *   AsXML(PXMLElement * parent) const;

    protected:
      const JID     m_JID;
      const PString m_Node;
    };

    PDECLARE_LIST(ItemList, Item)
    public:
      ItemList(PXMLElement * list);
      PXMLElement * AsXML(PXMLElement * parent) const;
    };

    class Identity : public PObject
    {
      PCLASSINFO(Identity, PObject);
    public:
      Identity(PXMLElement * identity);
      Identity(const PString& category, const PString& type, const PString& name);

      const PString&  GetCategory() const   { return m_Category; }
      const PString&  GetType() const       { return m_Type; }
      const PString&  GetName() const       { return m_Name; }

      PXMLElement *   AsXML(PXMLElement * parent) const;

    protected:
      const PString m_Category;
      const PString m_Type;
      const PString m_Name;
    };

    PDECLARE_LIST(IdentityList, Identity)
    public:
      IdentityList(PXMLElement * list);
      PXMLElement * AsXML(PXMLElement * parent) const;
    };

    class Info : public PObject
    {
      PCLASSINFO(Info, PObject);
    public:
      Info(PXMLElement * info);
      
      IdentityList&   GetIdentities() { return m_Identities; }
      PStringSet&     GetFeatures()   { return m_Features; }

      PXMLElement *   AsXML(PXMLElement * parent) const;

    protected:
      IdentityList  m_Identities;
      PStringSet    m_Features;
    };
  } // namespace Disco

}; // class XMPP


#endif  // P_EXPAT

#endif  // _XMPP

// End of File ///////////////////////////////////////////////////////////////


