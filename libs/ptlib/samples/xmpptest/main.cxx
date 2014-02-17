/*
 * main.cxx
 *
 * PWLib application source file for XMPPTest
 *
 * Main program entry point.
 *
 * Copyright 2004 Reitek S.p.A.
 *
 * $Log: main.cxx,v $
 * Revision 1.4  2007/05/10 00:41:20  rjongbloed
 * Updated build for wxWidgets 2.8.x
 *
 * Revision 1.3  2006/07/23 03:39:49  dereksmithies
 * Modify main.cxx so it compiles under linux.
 * Add Makefile so it can be compiled under linux.
 *
 * Revision 1.2  2004/05/09 07:23:49  rjongbloed
 * More work on XMPP, thanks Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.1  2004/04/26 01:51:58  rjongbloed
 * More implementation of XMPP, thanks a lot to Federico Pinna & Reitek S.p.A.
 *
 *
 */

#include "main.h"


#if !P_EXPAT
#error Must have XML support for this application
#endif

///////////////////////////////////////////////////////////////////////////////
#include "ConnectDialog.h"

class ConnectDialog : public _ConnectDialog
{
public:
  ConnectDialog(wxWindow *parent)
    : _ConnectDialog(parent) { }

  void      SetJID(const PString& jid);
  void      SetPwd(const PString& pwd);
  void      SetRememberPwd(bool b = true);

  XMPP::JID GetJID() const;
  PString   GetPwd() const;
  bool      GetRememberPwd() const;
};

void ConnectDialog::SetJID(const PString& jid)
{ if (m_JID) m_JID->SetValue((const char *)jid); }

void ConnectDialog::SetPwd(const PString& pwd)
{ if (m_Pwd) m_Pwd->SetValue((const char *)pwd); }

void ConnectDialog::SetRememberPwd(bool b)
{ if (m_RememberPwd) m_RememberPwd->SetValue(b); }

XMPP::JID ConnectDialog::GetJID() const
{ 
    PString answer;
    
    if (m_JID != NULL)
	answer = XMPP::JID(PString((const char *)m_JID->GetValue()));
    
    return answer;
}
/** Was return m_JID != NULL ?  XMPP::JID(PString((const char *)m_JID->GetValue())) : PString::Empty(); } */

PString ConnectDialog::GetPwd() const
{ 
    PString answer;

    if (m_Pwd != NULL)
	answer = (const char *)m_Pwd->GetValue();
    
    return answer;
}

bool ConnectDialog::GetRememberPwd() const
{ return m_RememberPwd != NULL ? m_RememberPwd->GetValue() : false; }

///////////////////////////////////////////////////////////////////////////////

/*
BEGIN_EVENT_TABLE(XMPPFrame, wxFrame)
  EVT_SIZE(XMPPFrame::OnSize)
  EVT_MENU(XMPPFrame::MENU_FILE_CONNECT, XMPPFrame::OnConnect)
  EVT_MENU(XMPPFrame::MENU_FILE_DISCONNECT, XMPPFrame::OnDisconnect)
  EVT_MENU(XMPPFrame::MENU_FILE_QUIT, XMPPFrame::OnQuit)
END_EVENT_TABLE()
*/

XMPPFrame::XMPPFrame()
  : MainFrame(NULL),
    m_Roster(new XMPP::Roster), m_Client(NULL)
{
  m_Roster->RosterChangedHandlers().Add(new PCREATE_SMART_NOTIFIER(OnRosterChanged));
  Show(TRUE);
}


XMPPFrame::~XMPPFrame()
{
}


void XMPPFrame::OnConnect(wxCommandEvent& WXUNUSED(event))
{
  ConnectDialog * dlg = new ConnectDialog(this);

  if (m_Client != NULL) {
    dlg->SetJID(m_Client->GetJID());
    // and the password...
  }

  if (dlg->ShowModal() == wxID_CANCEL)
    return;

  XMPP::JID jid = dlg->GetJID();
  PString pwd = dlg->GetPwd();
  dlg->Destroy(); 
  
  
  if (m_Client != NULL) {
    m_Client->Stop();
    if (!m_Client->IsTerminated())
      m_Client->WaitForTermination();
    delete m_Client;
  }

  m_Client = new XMPP::C2S::StreamHandler(jid, pwd);
  m_Client->SessionEstablishedHandlers().Add(new PCREATE_SMART_NOTIFIER(OnSessionEstablished));
  m_Client->SessionReleasedHandlers().Add(new PCREATE_SMART_NOTIFIER(OnSessionReleased));
  m_Client->MessageHandlers().Add(new PCREATE_SMART_NOTIFIER(OnMessage));
  SetStatusText("Connecting...", 0);
  m_Client->Start();
}


void XMPPFrame::OnDisconnect(wxCommandEvent& WXUNUSED(event))
{
  if (m_Client == NULL)
    return;
  
  m_Client->Stop();
  if (!m_Client->IsTerminated())
    m_Client->WaitForTermination();
  delete m_Client;
  m_Client = NULL;
}


void XMPPFrame::OnQuit(wxCommandEvent& event)
{
  OnDisconnect(event);
  Close(TRUE);
}


void XMPPFrame::OnSessionEstablished(XMPP::C2S::StreamHandler& client, INT)
{
  SetStatusText("Connected", 0);

  m_Roster->Attach(m_Client);
}


void XMPPFrame::OnSessionReleased(XMPP::C2S::StreamHandler& client, INT)
{
  SetStatusText("Disconnected", 0);
  m_Roster->Detach();
}


void XMPPFrame::OnMessage(XMPP::Message& msg, INT)
{
  // If it's valid and it's not in-band data
  if (msg.GetElement("data") == NULL) {
    wxMessageDialog dialog(this, (const char *)msg.GetBody(), (const char *)msg.GetFrom(), wxOK);
    dialog.ShowModal();
  }
}

void XMPPFrame::OnRosterChanged(XMPP::Roster&, INT)
{
  // rebuild the tree
  m_RosterTree->DeleteAllItems();
  wxTreeItemId rootID = m_RosterTree->AddRoot(_T("root"));

  PDictionary<PString, POrdinalKey> groups;

  const XMPP::Roster::ItemList& l = m_Roster->GetItems();
  
  for (PINDEX i = 0, imax = l.GetSize() ; i < imax ; i++) {
    const XMPP::Roster::Item& item = l[i];
    const PStringSet& s = item.GetGroups();

    for (PINDEX j = 0, jmax = s.GetSize() ; j < jmax ; j++) {

      const PString& key = s.GetKeyAt(j);
      wxTreeItemId g_id;

      if (!groups.Contains(key)) {
        g_id = m_RosterTree->AppendItem(rootID, (const char *)key);
        groups.SetAt(key, new POrdinalKey(g_id));
      }

      g_id = (void *)(PINDEX)groups[key];
      wxTreeItemId i_id = m_RosterTree->AppendItem(g_id, (const char *)item.GetName());
      m_RosterTree->Expand(g_id);

      const XMPP::Roster::Item::PresenceInfo& pres = item.GetPresence();

      for (PINDEX k = 0, kmax = pres.GetSize() ; k < kmax ; k++) {
        PString res = pres.GetKeyAt(k);

        if (pres[res].GetType() == XMPP::Presence::Available) {
          PString show;
          pres[res].GetShow(&show);
          res += " - " + show;
          m_RosterTree->AppendItem(i_id, (const char *)res);
        }
      }
      m_RosterTree->Expand(i_id);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_APP(XMPPTest)

XMPPTest::XMPPTest()
  : PProcess("Reitek S.p.A.", "XMPPTest", 1, 0, AlphaCode, 1)
{
}


bool XMPPTest::OnInit()
{
#if PTRACING
  PTrace::Initialise(10, "jabber.log", PTrace::Blocks | PTrace::DateAndTime | PTrace::Thread | PTrace::FileAndLine);
#endif

  // Create the main frame window
  SetTopWindow(new XMPPFrame());
  return true;
}

// End of File ///////////////////////////////////////////////////////////////
