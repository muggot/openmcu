/*
 * main.cxx
 *
 * PWLib application source file for XMPP Console
 *
 * Main program entry point.
 *
 * Copyright 2004 Reitek S.p.A.
 *
 * Copied by Derek Smithies, 1)removed all the wxwidget stuff.
 *                           2)turned into a console application.
 *
 * $Log: main.cxx,v $
 * Revision 1.1  2005/08/04 03:29:12  dereksmithies
 * Initial release of a console application to test XMPP messages between running applications.
 * Thanks to Indranet Technologies ltd for sponsoring the work.
 * Thanks to Federico Pinna & Reitek S.p.A for the inital work of getting XMPP going.
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
 *
 */

#include "main.h"
#include "version.h"

#if !P_EXPAT
#error Must have XML support for this application
#endif



///////////////////////////////////////////////////////////////////////////////


XMPPFrame::XMPPFrame()
  : m_Roster(new XMPP::Roster), m_Client(NULL)
{
  m_Roster->RosterChangedHandlers().Add(new PCREATE_SMART_NOTIFIER(OnRosterChanged));
  isReadyForUse = FALSE;
}


XMPPFrame::~XMPPFrame()
{
}


void XMPPFrame::OnConnect()
{
  //  cout << "On Connect is running " << endl;

  m_Client = new XMPP::C2S::StreamHandler(XMPPConsole::Current().GetServer(), XMPPConsole::Current().GetPassword());
  m_Client->SessionEstablishedHandlers().Add(new PCREATE_SMART_NOTIFIER(OnSessionEstablished));
  m_Client->SessionReleasedHandlers().Add   (new PCREATE_SMART_NOTIFIER(OnSessionReleased));
  m_Client->MessageHandlers().Add           (new PCREATE_SMART_NOTIFIER(OnMessage));
  m_Client->ErrorHandlers().Add             (new PCREATE_SMART_NOTIFIER(OnError));
  m_Client->PresenceHandlers().Add          (new PCREATE_SMART_NOTIFIER(OnPresence));
  m_Client->IQHandlers().Add                (new PCREATE_SMART_NOTIFIER(OnIQ));
  m_Client->Start();
}


void XMPPFrame::OnDisconnect()
{
  if (m_Client == NULL)
    return;
  
  m_Client->Stop();
  m_Client->WaitForTermination();
  delete m_Client;
  m_Client = NULL;
}

void XMPPFrame::OnError(XMPP::Message & msg, INT)
{
  cout << "There has been an error with a XMPP Message " << endl;
  cout << "The XMPP Message was " << msg << endl;
}

void XMPPFrame::OnQuit()
{
  // cout << "On Quit called, so end of program " << endl;
  OnDisconnect();
}

void XMPPFrame::OnIQ(XMPP::IQ& pdu, INT)
{
  // cout << "On IQ for " << pdu << endl;
}

void XMPPFrame::OnPresence(XMPP::Presence& pdu, INT)
{
  otherParty = pdu.GetFrom();
  localParty = pdu.GetTo();
  //cout << "To   " << pdu.GetTo()   << endl;
  //cout << "From " << pdu.GetFrom() << endl;
  //cout << "ID   " << pdu.GetID()   << endl;
  PString message;
  pdu.GetType(&message);
  //cout << "PresenceType is " << message << endl << endl;

  XMPP::Presence::PresenceType type = pdu.GetType();
  if (type ==  XMPP::Presence::Available) {
    //cout << " ADD available node " << pdu.GetFrom() << endl;
    PWaitAndSignal m(availableLock);
    availableNodes.AppendString(pdu.GetFrom());
    //cout << "Another party is  " << pdu.GetFrom() << endl;
  } else {
    //cout << " Remove available node " << pdu.GetFrom() << endl;
    PWaitAndSignal m(availableLock);
    PINDEX loc = availableNodes.GetStringsIndex(pdu.GetFrom());
    if (loc != P_MAX_INDEX)
      availableNodes.RemoveAt(loc);
  }    

#if 0
  cout << "Valid presences is " << pdu.IsValid() << endl;
  cout << "Priority is        " << pdu.GetPriority() << endl;
  pdu.GetShow(&message);
  cout << "ShowType is     " << message << endl;

#endif
}

void XMPPFrame::OnSessionEstablished(XMPP::C2S::StreamHandler& client, INT)
{
  cout << "On Session Established." << endl;

  m_Roster->Attach(m_Client);

  onReadyForUseTimer =  PTimeInterval(1000);  //1 second.
  onReadyForUseTimer.SetNotifier(PCREATE_NOTIFIER(OnReadyForUse));
}

void XMPPFrame::OnReadyForUse(PTimer &, INT)
{
  //  cout << "Called 1 second after session established " << endl;

  isReadyForUse = TRUE;
}

void XMPPFrame::OnSessionReleased(XMPP::C2S::StreamHandler& client, INT)
{
  cout << "Disconnected" << endl;
  m_Roster->Detach();
}


void XMPPFrame::OnMessage(XMPP::Message& msg, INT)
{
  // If it's valid and it's not in-band data
  // cout << "On message called, " << msg << endl;
  if (msg.GetElement("data") == NULL) 
    cout << " Message received. Subject:" << msg.GetSubject() << "    body:" << msg.GetBody() << "  from: " << msg.GetFrom() << endl;
}

void XMPPFrame::OnRosterChanged(XMPP::Roster& rost, INT)
{
  //  cout << " ON Roster changed " << endl;
  //cout << endl;

  ReportRoster();
}

void XMPPFrame::ReportRoster()
{
  if (m_Roster == NULL) {
    cout << "Roster is empty. Nothing to report" << endl;
    return;
  }

  PDictionary<PString, POrdinalKey> groups;
  const XMPP::Roster::ItemList& l = m_Roster->GetItems();
  //  cout << " Roster contains " << l.GetSize() << " items" << endl;

  for (PINDEX i = 0, imax = l.GetSize() ; i < imax ; i++) {
    const XMPP::Roster::Item& item = l[i];
    const PStringSet& s = item.GetGroups();
    //  cout << " groups within element " << (i + 1) << " has size " << s.GetSize() << endl;

    for (PINDEX j = 0, jmax = s.GetSize() ; j < jmax ; j++) {

      const PString& key = s.GetKeyAt(j);
      cout << key << endl;
      if (!groups.Contains(key)) {
	//  g_id = m_RosterTree->AppendItem(rootID, (const char *)key);
        // groups.SetAt(key, new POrdinalKey(g_id));
      }

      // g_id = (PINDEX)groups[key];
      // wxTreeItemId i_id = m_RosterTree->AppendItem(g_id, (const char *)item.GetName());
      //m_RosterTree->Expand(g_id);

      const XMPP::Roster::Item::PresenceInfo& pres = item.GetPresence();

      for (PINDEX k = 0, kmax = pres.GetSize() ; k < kmax ; k++) {
        PString res = pres.GetKeyAt(k);
	cout << "res is " << res << endl;

        if (pres[res].GetType() == XMPP::Presence::Available) {
          PString show;
          pres[res].GetShow(&show);
          res += " - " + show;
	  cout << res << endl;
        }
      }
    }
  }
}

  
PStringArray & XMPPFrame::GetAvailableNodes() 
{
  PWaitAndSignal m(availableLock); 
  
  return availableNodes; 
}    
  

///////////////////////////////////////////////////////////////////////////////

PCREATE_PROCESS(XMPPConsole);

XMPPConsole::XMPPConsole()
  : PProcess("Reitek S.p.A & Derek Smithies code factory.", "XMPPConsole", 
             MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER)
{
}

void XMPPConsole::Main()
{
  PConfigArgs args(GetArguments());
 
  args.Parse(
             "p-password:" 
             "u-user:"     
	     "j-jabber:"   
             "h-help."
	     "v-version."
#if PTRACING
             "o-output:"   
             "t-trace."    
#endif
#ifdef PMEMORY_CHECK
             "-setallocationbreakpoint:"
#endif       
	     , FALSE);
  
  #if PMEMORY_CHECK
  if (args.HasOption("setallocationbreakpoint"))
    PMemoryHeap::SetAllocationBreakpoint(args.GetOptionString("setallocationbreakpoint").AsInteger());
#endif

  if (args.HasOption('h')) {
    cout << "Usage : " << GetName() << " [options] \n"
      "\n"
      "\nOptions:\n"
      "  -h --help               : Display this help message.\n"
      "  -p --password pwd       : Password for jabber identity\n"
      "  -u --user     name      : Set name of the local jabber identity (defaults to login name)\n"
      "  -j --jabber   server    : Jabber server to talk to\n"
      "  -v --version            : report current version info\n"
#if PTRACING || PMEMORY_CHECK
      "\nDebug options:\n"
#endif
#if PTRACING
      "  -t --trace              : Enable trace, use multiple times for more detail\n"
      "  -o --output             : File for trace output, default is stderr\n"
#endif
#ifdef PMEMORY_CHECK
      "  --setallocationbreakpoint n : Enable breakpoint on memory allocation n\n"
#endif
	 << endl << endl;
    return;
  }

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
         PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif

  if (args.HasOption('v')) {
    cout << endl
	 << "Product Name: " <<  (const char *)GetName() << endl
	 << "Manufacturer: " <<  (const char *)GetManufacturer() << endl
	 << "Version     : " <<  (const char *)GetVersion(TRUE) << endl
	 << "System      : " <<  (const char *)GetOSName() << " - "
	 << (const char *)GetOSHardware() << " - "
	 << (const char *)GetOSVersion()  << endl
	 << endl;
    return;
  }


  if (args.HasOption("password")) 
    password = args.GetOptionString("password");

  userName = GetUserName();
  if (args.HasOption('u')) 
    userName = args.GetOptionString('u');

  if (args.HasOption('j')) 
    server = args.GetOptionString('j');

  XMPPFrame frame;
  
  frame.ConnectNow();

  
   

  UserInterface ui (frame);
  ui.WaitForTermination();
  frame.DisconnectNow();

#if PTRACING
  if (args.GetOptionCount('t') > 0) {
    PTrace::ClearOptions(0);
    PTrace::SetLevel(0);
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////

void UserInterface::Main()
{
  PConsoleChannel console(PConsoleChannel::StandardInput);
  cout << "This program will listen for incoming jabber messages, and send messages on request " << endl;
 
  PStringStream help;
  help << "Press : "                                                                                                     << endl
       << "         D i subject  message    Send a message (with specified subject) to member i in known list of nodes " << endl
       << "         H or ?                  help on this interface"                                                      << endl
       << "         R                       report nodes known to be present "                                           << endl
       << "         S msg                   send a message to the room "                                                 << endl
       << "         T                       time elapsed"                                                                << endl
       << "         X or Q                  exit "                                                                       << endl;
  
  cout << endl << help;
 
  console.SetReadTimeout(P_MAX_INDEX);
  for (;;) {
    char ch = console.ReadChar();
 
    switch (tolower(ch)) {

    case 't' :
      cout << "\nElapsed time is " << (PTime() - startTime) << " (Hours:mins:seconds.millseconds)" << endl;
      cout << "Command ? " << flush;
      break;
    case 'r':
      ReportAvailableNodes();
      cout << "Command ? " << flush;
      break;      
    case 'x' :
    case 'q' :
      cout << "Exiting." << endl;
      return;
      break;
    case 'd' :
      {
	PString message;
	console >> message;
	ProcessDirectedMessage(message);
	cout << "Command ? " << flush;
      }
      break;
    case 's' :
      {
	PString message;
	console >> message;
	SendThisMessage(message);
	cout << "Command ? " << flush;
      }
      break;
    case '?' :
    case 'h' :
      cout << help << endl;
      cout << "Command ? " << flush;
    default:
      break;
    } // end switch
  } // end for
}

void UserInterface::ReportAvailableNodes()
{
  PStringArray str = frame.GetAvailableNodes();

  if (str.GetSize() == 0) {
    cout << "Noone else is available for the current  JID" << endl;
    return;
  }

  for (PINDEX i = 0; i < str.GetSize(); i++) {
    cout << "Node " << (i + 1) << " of " << str.GetSize() << " is " << str[i] << endl;
  }
}

void UserInterface::SendThisMessage(PString & _message)
{
  if (!frame.IsConnected()) {
    cout << "Error, cannot send " << _message << " as not connected yet" << endl;
    return;
  }

  if (frame.OtherPartyIsEmpty()) {
    cout << "Error, cannot send " << _message << " as no one else to send to on this jabber server" << endl;
    return;
  }
  
  SendThisMessageTo(_message, "msg", frame.GetOtherParty());
}

void UserInterface::SendThisMessageTo(PString & _message, PString subject, PString dest)
{
  PString message;
  message = _message.Trim();
  //  cout << "Now we send the message \"" << message << "\" to " << dest << endl;

  XMPP::Message *msg = new XMPP::Message();
  msg->SetType("normal");
  msg->SetSubject(subject);
  msg->SetBody(message);
  msg->SetTo(dest);
  msg->SetFrom(frame.GetLocalParty());
  msg->SetID(msg->GenerateID());
  
  if (frame.Send(msg)) {
    //  cout << "Sent message just fine" << endl;
  } else {
    //  cout << "Failed in sending of the message. Sorry. " << endl;
    //  msg->SetFrom(frame.    
  }
}

void UserInterface::ProcessDirectedMessage(PString & message)
{
  PStringArray str = frame.GetAvailableNodes();

  if (str.GetSize() == 0) {
    cout << "Noone else is available to write to.. Sorry." << endl;
    return;
  }

  PStringArray bits = message.Tokenise(" ", FALSE);
  if (bits.GetSize() < 3) {
    cout << "a directed message consists of a node number to call, subject, and a message" << endl;
    return;
  }

  PINDEX node = bits[0].AsInteger();
  if ((node < 1) || (node > str.GetSize())) {
    cout << "Sorry. The selected node number " << node << " is out of the range of 1.." << str.GetSize() << endl;
    return;
  }

  PString subject = bits[1];

  PINDEX loc = message.Find(subject);
  PString send = message.Mid(loc + subject.GetLength());
  send = send.Trim();

  //  cout << " send \"" << send << "\" to node " << node << " or " << str[node - 1] << endl;

  SendThisMessageTo(send, subject, str[node - 1]);
}


// End of File ///////////////////////////////////////////////////////////////
