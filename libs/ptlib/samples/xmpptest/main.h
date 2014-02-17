/*
 * main.h
 *
 * PWLib application header file for XMPPTest
 *
 * Copyright 2004 Reitek S.p.A.
 *
 * $Log: main.h,v $
 * Revision 1.4  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.3  2005/08/08 08:15:59  rjongbloed
 * Fixed precompiled header usage
 *
 * Revision 1.2  2004/05/09 07:23:49  rjongbloed
 * More work on XMPP, thanks Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.1  2004/04/26 01:51:58  rjongbloed
 * More implementation of XMPP, thanks a lot to Federico Pinna & Reitek S.p.A.
 *
 */

#ifndef _XMPPTest_MAIN_H
#define _XMPPTest_MAIN_H

#include <ptlib.h>
#include <ptlib/pprocess.h>
#include <ptlib/notifier_ext.h>
#include <ptclib/xmpp_c2s.h>
#include <ptclib/xmpp_roster.h>
#include <ptclib/xmpp_muc.h>

#include <wx/wx.h>

#include "MainFrame.h"

class XMPPFrameBase : public PObject
{
  PCLASSINFO(XMPPFrameBase, PObject);
  PDECLARE_SMART_NOTIFIEE;
protected:
  XMPPFrameBase() { PCREATE_SMART_NOTIFIEE; }
  PDECLARE_SMART_NOTIFIER(XMPP::C2S::StreamHandler, XMPPFrameBase, OnSessionEstablished) = 0;
  PDECLARE_SMART_NOTIFIER(XMPP::C2S::StreamHandler, XMPPFrameBase, OnSessionReleased) = 0;
  PDECLARE_SMART_NOTIFIER(XMPP::Message, XMPPFrameBase, OnMessage) = 0;
  PDECLARE_SMART_NOTIFIER(XMPP::Roster, XMPPFrameBase, OnRosterChanged) = 0;
};


class XMPPFrame : public MainFrame, public XMPPFrameBase
{
//  DECLARE_EVENT_TABLE()
public:
  XMPPFrame();
  ~XMPPFrame();

protected:
  // wxWidgets events
  virtual void OnConnect(wxCommandEvent& event);
  virtual void OnDisconnect(wxCommandEvent& event);
  virtual void OnQuit(wxCommandEvent& event);

  // pwlib events
  virtual void OnSessionEstablished(XMPP::C2S::StreamHandler&, INT);
  virtual void OnSessionReleased(XMPP::C2S::StreamHandler&, INT);
  virtual void OnMessage(XMPP::Message&, INT);
  virtual void OnRosterChanged(XMPP::Roster&, INT);

private:
  XMPP::Roster * m_Roster;
  XMPP::C2S::StreamHandler * m_Client;
};


class XMPPTest : public wxApp, public PProcess
{
  PCLASSINFO(XMPPTest, PProcess);

public:
  XMPPTest();

  void Main() { }

protected:
  // Initialise wxWidgets
  virtual bool OnInit();
};

#endif  // _XMPPTest_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
