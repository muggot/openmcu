/*
 * main.h
 *
 * PWLib application header file for XMPP Console
 *
 * Copyright 2004 Reitek S.p.A.
 *
 * Copied by Derek Smithies, 1)removed all the wxwidget stuff.
 *                           2)turned into a console application.
 *
 * $Log: main.h,v $
 * Revision 1.2  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.1  2005/08/04 03:29:12  dereksmithies
 * Initial release of a console application to test XMPP messages between running applications.
 * Thanks to Indranet Technologies ltd for sponsoring the work.
 * Thanks to Federico Pinna & Reitek S.p.A for the inital work of getting XMPP going.
 *
 *
 *
 *
 *
 *
 * Commit log from old XMPP Test application.
 * Revision 1.2  2004/05/09 07:23:49  rjongbloed
 * More work on XMPP, thanks Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.1  2004/04/26 01:51:58  rjongbloed
 * More implementation of XMPP, thanks a lot to Federico Pinna & Reitek S.p.A.
 *
 */

#ifndef _XMPPConsole_MAIN_H
#define _XMPPConsole_MAIN_H

#include <ptlib.h>
#include <ptlib/pprocess.h>
#include <ptlib/notifier_ext.h>
#include <ptclib/xmpp_c2s.h>
#include <ptclib/xmpp_roster.h>
#include <ptclib/xmpp_muc.h>


class XMPPFrameBase : public PObject
{
  PCLASSINFO(XMPPFrameBase, PObject);
  PDECLARE_SMART_NOTIFIEE;
protected:
  XMPPFrameBase() { PCREATE_SMART_NOTIFIEE; }
  PDECLARE_SMART_NOTIFIER(XMPP::C2S::StreamHandler, XMPPFrameBase, OnSessionEstablished) = 0;
  PDECLARE_SMART_NOTIFIER(XMPP::C2S::StreamHandler, XMPPFrameBase, OnSessionReleased) = 0;
  PDECLARE_SMART_NOTIFIER(XMPP::Message, XMPPFrameBase, OnMessage) = 0;
  PDECLARE_SMART_NOTIFIER(XMPP::Message, XMPPFrameBase, OnError) = 0;
  PDECLARE_SMART_NOTIFIER(XMPP::Roster, XMPPFrameBase, OnRosterChanged) = 0;
  PDECLARE_SMART_NOTIFIER(XMPP::Presence,  XMPPFrameBase, OnPresence) = 0;
  PDECLARE_SMART_NOTIFIER(XMPP::IQ,  XMPPFrameBase, OnIQ) = 0;
};


class XMPPFrame : public XMPPFrameBase
{
public:
  XMPPFrame();
  ~XMPPFrame();

  void ConnectNow() { OnConnect(); }
  void DisconnectNow() { OnQuit(); }

  PDECLARE_NOTIFIER(PTimer, XMPPFrame, OnReadyForUse);
  PTimer onReadyForUseTimer;

  BOOL    LocalPartyIsEmpty() { return localParty.IsEmpty(); }
  BOOL    OtherPartyIsEmpty() { return otherParty.IsEmpty(); }
  PString GetOtherParty() { return otherParty; }
  PString GetLocalParty() { return localParty; }
  BOOL    IsConnected() {  return isReadyForUse; }

  BOOL Send(XMPP::Stanza * stanza) { return m_Client->Send(stanza); }

  void OnConnect();

  // pwlib events
  virtual void OnDisconnect();
  virtual void OnError(XMPP::Message &, INT);
  virtual void OnMessage(XMPP::Message&, INT);
  virtual void OnPresence(XMPP::Presence& pdu, INT);
  virtual void OnQuit();
  virtual void OnIQ(XMPP::IQ& pdu, INT);
  virtual void OnRosterChanged(XMPP::Roster&, INT);
  virtual void OnSessionEstablished(XMPP::C2S::StreamHandler&, INT);
  virtual void OnSessionReleased(XMPP::C2S::StreamHandler&, INT);
  
  void ReportRoster();

  PStringArray & GetAvailableNodes();

private:
  XMPP::Roster * m_Roster;
  XMPP::C2S::StreamHandler * m_Client;

  PStringArray availableNodes;
  PMutex       availableLock;

  PString otherParty;
  PString localParty;
  BOOL    isReadyForUse;
};


class XMPPConsole :public PProcess
{
  PCLASSINFO(XMPPConsole, PProcess);

public:
  XMPPConsole();

  void Main();

  PString GetPassword() { return password; }

  PString GetUserName() { return userName; }

  PString GetServer()   { return server; }

  static XMPPConsole & Current() { return (XMPPConsole &)PProcess::Current(); }

protected:

  PString password;
  PString userName;
  PString server;

};


class UserInterface: public PThread
{
  PCLASSINFO(UserInterface, PThread);
   
public:
  UserInterface(XMPPFrame & _frame)
    : PThread(1000, NoAutoDeleteThread),  frame(_frame)
    { Resume(); }
   
  void Main();
     
 protected:

  void ProcessDirectedMessage(PString & message);

  void SendThisMessageTo(PString & _message, PString subject, PString dest);

  void SendThisMessage(PString & _message);

  void ReportAvailableNodes();

  XMPPFrame &frame;

  PTime startTime;
};


#endif  // _XMPPConsole_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
