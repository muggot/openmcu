
#include <ptlib.h>

#include "version.h"
#include "mcu.h"
#include "h323.h"

const WORD DefaultHTTPPort = 1420;

extern PHTTPServiceProcess::Info ProductInfo;

static const char LogLevelKey[]           = "Log Level";
static const char UserNameKey[]           = "Username";
static const char PasswordKey[]           = "Password";
static const char HttpPortKey[]           = "HTTP Port";

static const char CallLogFilenameKey[]    = "Call log filename";

#if P_SSL
static const char HTTPCertificateFileKey[]  = "HTTP Certificate";
#endif
static const char DefaultRoomKey[]          = "Default room";
static const char DefaultRoomTimeLimitKey[] = "Room time limit";

static const char DefaultCallLogFilename[] = "mcu_log.txt"; 
static const char DefaultRoom[]            = "room101";

#if OPENMCU_VIDEO
static const char ForceSplitVideoKey[]   = "Force split screen video";
#endif

#define new PNEW


///////////////////////////////////////////////////////////////

class MainStatusPage : public PServiceHTTPString
{
 // PCLASSINFO(MainStatusPage, PServiceHTTPString);

  public:
    MainStatusPage(OpenMCU & app, PHTTPAuthority & auth);
    
    virtual BOOL Post(
      PHTTPRequest & request,
      const PStringToString &,
      PHTML & msg
    );
  
  private:
    OpenMCU & app;
};

class InvitePage : public PServiceHTTPString
{
 // PCLASSINFO(InvitePage, PServiceHTTPString);

  public:
    InvitePage(OpenMCU & app, PHTTPAuthority & auth);

    virtual BOOL Post(
      PHTTPRequest & request,       // Information on this request.
      const PStringToString & data, // Variables in the POST data.
      PHTML & replyMessage          // Reply message for post.
    );
  
  private:
    OpenMCU & app;
};

class SelectRoomPage : public PServiceHTTPString
{
 // PCLASSINFO(MainStatusPage, PServiceHTTPString);

  public:
    SelectRoomPage(OpenMCU & app, PHTTPAuthority & auth);
    
    virtual BOOL Post(
      PHTTPRequest & request,
      const PStringToString &,
      PHTML & msg
    );
  
  private:
    OpenMCU & app;
};


///////////////////////////////////////////////////////////////
// This really isn't the default count only a counter
// for sending aliases and prefixes to the gatekeeper
int OpenMCU::defaultRoomCount = 5;

VideoMixConfigurator OpenMCU::vmcfg;

OpenMCU::OpenMCU()
  : OpenMCUProcessAncestor(ProductInfo)
{
  endpoint = NULL;
}

void OpenMCU::Main()
{
  Suspend();
}

BOOL OpenMCU::OnStart()
{
  // change to the default directory to the one containing the executable
  PDirectory exeDir = GetFile().GetDirectory();

#if defined(_WIN32) && defined(_DEBUG)
  // Special check to aid in using DevStudio for debugging.
  if (exeDir.Find("\\Debug\\") != P_MAX_INDEX)
    exeDir = exeDir.GetParent();
#endif
  exeDir.Change();

  httpNameSpace.AddResource(new PHTTPDirectory("data", "data"));
  httpNameSpace.AddResource(new PServiceHTTPDirectory("html", "html"));

  manager  = CreateConferenceManager();
  endpoint = CreateEndPoint(*manager);

  return PHTTPServiceProcess::OnStart();
}

void OpenMCU::OnStop()
{
  delete endpoint;
  endpoint = NULL;

  delete manager;
  manager = NULL;

  PHTTPServiceProcess::OnStop();
}

void OpenMCU::OnControl()
{
  // This function get called when the Control menu item is selected in the
  // tray icon mode of the service.
  PStringStream url;
  url << "http://";

  PString host = PIPSocket::GetHostName();
  PIPSocket::Address addr;
  if (PIPSocket::GetHostAddress(host, addr))
    url << host;
  else
    url << "localhost";

  url << ':' << DefaultHTTPPort;

  PURL::OpenBrowser(url);
}

BOOL OpenMCU::Initialise(const char * initMsg)
{
  PConfig cfg("Parameters");

  // Set log level as early as possible
//  SetLogLevel((PSystemLog::Level)cfg.GetInteger(LogLevelKey, GetLogLevel()));
#if PTRACING

    SetLogLevel(PSystemLog::Debug6);
    PTrace::Initialise(6,"trace.txt");

/*  if (GetLogLevel() >= PSystemLog::Warning)
    PTrace::SetLevel(GetLogLevel()-PSystemLog::Warning);
  else
    PTrace::SetLevel(0);
  PTrace::ClearOptions(PTrace::Timestamp);
  PTrace::SetOptions(PTrace::DateAndTime); */
#endif

  // Get the HTTP basic authentication info
  PString adminUserName = cfg.GetString(UserNameKey);
  PString adminPassword = PHTTPPasswordField::Decrypt(cfg.GetString(PasswordKey));

  PHTTPSimpleAuth authority(GetName(), adminUserName, adminPassword);

  // Create the parameters URL page, and start adding fields to it
  PConfigPage * rsrc = new PConfigPage(*this, "Parameters", "Parameters", authority);

  // HTTP authentication username/password
  rsrc->Add(new PHTTPStringField(UserNameKey, 25, adminUserName));
  rsrc->Add(new PHTTPPasswordField(PasswordKey, 25, adminPassword));

  // Log level for messages
  rsrc->Add(new PHTTPIntegerField(LogLevelKey,
                                  PSystemLog::Fatal, PSystemLog::NumLogLevels-1,
                                  GetLogLevel(),
                                  "1=Fatal only, 2=Errors, 3=Warnings, 4=Info, 5=Debug"));

#if P_SSL
  // SSL certificate file.
  PString certificateFile = cfg.GetString(HTTPCertificateFileKey, "server.pem");
  rsrc->Add(new PHTTPStringField(HTTPCertificateFileKey, 25, certificateFile));
  if (!SetServerCertificate(certificateFile, TRUE)) {
    PSYSTEMLOG(Fatal, "MCU\tCould not load certificate \"" << certificateFile << '"');
    return FALSE;
  }
#endif

  // HTTP Port number to use.
  WORD httpPort = (WORD)cfg.GetInteger(HttpPortKey, DefaultHTTPPort);
  rsrc->Add(new PHTTPIntegerField(HttpPortKey, 1, 32767, httpPort));

  endpoint->Initialise(cfg, rsrc);

  // get default "room" (conference) name
  defaultRoomName = cfg.GetString(DefaultRoomKey, DefaultRoom);
  rsrc->Add(new PHTTPStringField(DefaultRoomKey, 25, defaultRoomName));

  // get conference time limit 
  roomTimeLimit = cfg.GetInteger(DefaultRoomTimeLimitKey, 0);
  rsrc->Add(new PHTTPIntegerField(DefaultRoomTimeLimitKey, 0, 10800, roomTimeLimit));

  OnCreateConfigPage(cfg, *rsrc);

  // default log file name
  logFilename = cfg.GetString(CallLogFilenameKey, DefaultCallLogFilename);
  rsrc->Add(new PHTTPStringField(CallLogFilenameKey, 50, logFilename));

#if OPENMCU_VIDEO
  forceScreenSplit = cfg.GetBoolean(ForceSplitVideoKey, FALSE);
  rsrc->Add(new PHTTPBooleanField(ForceSplitVideoKey, forceScreenSplit));
#endif

  // Finished the resource to add, generate HTML for it and add to name space
  PServiceHTML html("System Parameters");
  rsrc->BuildHTML(html);
  httpNameSpace.AddResource(rsrc, PHTTPSpace::Overwrite);

  // Create the status page
  httpNameSpace.AddResource(new MainStatusPage(*this, authority), PHTTPSpace::Overwrite);

  // Create invite conference page
  httpNameSpace.AddResource(new InvitePage(*this, authority), PHTTPSpace::Overwrite);

  // Create room selection page
  httpNameSpace.AddResource(new SelectRoomPage(*this, authority), PHTTPSpace::Overwrite);

  // Add log file links
  if (!systemLogFileName && (systemLogFileName != "-")) {
    httpNameSpace.AddResource(new PHTTPFile("logfile.txt", systemLogFileName, authority));
    httpNameSpace.AddResource(new PHTTPTailFile("tail_logfile", systemLogFileName, authority));
  }

  //  create the home page
  static const char welcomeHtml[] = "welcome.html";
  if (PFile::Exists(welcomeHtml))
    httpNameSpace.AddResource(new PServiceHTTPFile(welcomeHtml, TRUE), PHTTPSpace::Overwrite);
  else {
    PHTML html;
    PStringStream shtml;
         BeginPage(shtml,"OpenMCU Home","OpenMCU Home","$WELCOME$");
/*
    if (!systemLogFileName && systemLogFileName != "-")
      html << PHTML::HotLink("logfile.txt") << "Full Log File" << PHTML::HotLink()
           << PHTML::BreakLine()
           << PHTML::HotLink("tail_logfile") << "Tail Log File" << PHTML::HotLink()
           << PHTML::Paragraph();
*/
         EndPage(shtml,GetCopyrightText());
    html = shtml;
    httpNameSpace.AddResource(new PServiceHTTPString("welcome.html", html), PHTTPSpace::Overwrite);
  }

  // create monitoring page
  PString monitorText = "<!--#equival monitorinfo-->"
                        "<!--#equival mcuinfo-->";
  httpNameSpace.AddResource(new PServiceHTTPString("monitor.txt", monitorText, "text/plain", authority), PHTTPSpace::Overwrite);


  // set up the HTTP port for listening & start the first HTTP thread
  if (ListenForHTTP(httpPort))
    PSYSTEMLOG(Info, "Opened master socket for HTTP: " << httpListeningSocket->GetPort());
  else {
    PSYSTEMLOG(Fatal, "Cannot run without HTTP port: " << httpListeningSocket->GetErrorText());
    return FALSE;
  }

  PSYSTEMLOG(Info, "Service " << GetName() << ' ' << initMsg);
  return TRUE;
}

PCREATE_SERVICE_MACRO(mcuinfo,P_EMPTY,P_EMPTY)
{
  return OpenMCU::Current().GetEndpoint().GetMonitorText();
}

void OpenMCU::OnConfigChanged()
{
}

PString OpenMCU::GetNewRoomNumber()
{
  static PAtomicInteger number = 100;
  return PString(PString::Unsigned, ++number);
}

void OpenMCU::LogMessage(const PString & str)
{
  static PMutex logMutex;
  static PTextFile logFile;

  PTime now;
  PString msg = now.AsString("dd/MM/yyyy") & str;
  logMutex.Wait();

  if (!logFile.IsOpen()) {
    logFile.Open(logFilename, PFile::ReadWrite);
    logFile.SetPosition(0, PFile::End);
  }

  logFile.WriteLine(msg);
  logFile.Close();
  logMutex.Signal();
}

ConferenceManager * OpenMCU::CreateConferenceManager()
{
  return new ConferenceManager();
}

OpenMCUH323EndPoint * OpenMCU::CreateEndPoint(ConferenceManager & manager)
{
  return new OpenMCUH323EndPoint(manager);
}

//////////////////////////////////////////////////////////////////////////////////////////////

PCREATE_SERVICE_MACRO_BLOCK(RoomStatus,P_EMPTY,P_EMPTY,block)
{
  return OpenMCU::Current().GetEndpoint().GetRoomStatus(block);
}

MainStatusPage::MainStatusPage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Status", "", "text/html; charset=windows-1251", auth),
    app(_app)
{
  PStringStream html;
  

  html << "<meta http-equiv=\"Refresh\" content=\"30\">\n";
  BeginPage(html,"Status Page","Status Page","$STATUS$");
  html << "<p>"
       << "<table class=\"table table-striped table-bordered table-condensed\">"
       << "<tr>"
       << "<th>"
       << "Room Name"
       << "<th>"
       << "Room Members"

       << "<!--#macrostart RoomStatus-->"
         << "<tr>"
         << "<td>"
         << "<!--#status RoomName-->"
         << "<td>"
         << "<!--#status RoomMembers-->"
       << "<!--#macroend RoomStatus-->"

       << "</table>"

       << "<p>";
       
         EndPage(html,app.GetCopyrightText());
  string = html;
}


BOOL MainStatusPage::Post(PHTTPRequest & request,
                          const PStringToString & data,
                          PHTML & msg)
{
  return TRUE;
}


///////////////////////////////////////////////////////////////

InvitePage::InvitePage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Invite", "", "text/html; charset=windows-1251", auth),
    app(_app)
{
  PStringStream html;
  

  BeginPage(html,"Invite User","Invite User","$INVITE$");

     html << "<p>"

    << "<form method=\"POST\" class=\"well form-inline\">"
    << "<input type=\"text\" class=\"input-small\" placeholder=\"Room Name\" name=\"room\" > "
    << "<input type=\"text\" class=\"input-small\" placeholder=\"Address\" name=\"address\">"
    << "&nbsp;&nbsp;<input type=\"submit\" class=\"btn\" value=\"Invite\">"
    << "</form>";

         EndPage(html,app.GetCopyrightText());

  string = html;
}


BOOL InvitePage::Post(PHTTPRequest & request,
                          const PStringToString & data,
                          PHTML & msg)
{
  PString room    = data("room");
  PString address = data("address");
  PStringStream html;

  if (room.IsEmpty() || address.IsEmpty()) {
    BeginPage(html,"Invite Failed","Invite Failed","$INVITE_F$");
    html << "<div class=\"alert alert-error\"><b>Insufficient information to perform INVITE</b></div>";
    EndPage(html,OpenMCU::Current().GetCopyrightText()); msg = html;
    return TRUE;
  }

  OpenMCUH323EndPoint & ep = app.GetEndpoint();
  BOOL created = ep.OutgoingConferenceRequest(room);

  if (!created) {
    BeginPage(html,"Invite Failed","Invite Failed","$INVITE_F$");
    html << "<div class=\"alert\"><b>Cannot create</b> room " << room;
    html << "</div>";
    EndPage(html,OpenMCU::Current().GetCopyrightText()); msg = html;
    return TRUE;
  }

  PString h323Token;
  PString * userData = new PString(room);
  if (ep.MakeCall(address, h323Token, userData) == NULL) {
    BeginPage(html,"Invite Failed","Invite Failed","$INVITE_F$");
    html << "<div class=\"alert\"><b>Cannot create make call to</b> " << address;
    html << "</div>";
    EndPage(html,OpenMCU::Current().GetCopyrightText()); msg = html;
    ep.GetConferenceManager().RemoveConference(room);
    return TRUE;
  }

  BeginPage(html,"Invite Succeeded","Invite Succeeded","$INVITE_S$");
  html << "<div class=\"alert alert-success\">Inviting " << address << " to room " << room;
  html << "</div>";

  html << "<p><h3>Invite another:</h3>"
    << "<form method=\"POST\" class=\"well form-inline\">"
    << "<input type=\"text\" class=\"input-small\" name=\"room\" placeholder=\"" << room << "\" > "
    << "<input type=\"text\" class=\"input-small\" name=\"address\" placeholder=\"address\">"
    << "&nbsp;&nbsp;&nbsp;<input type=\"submit\" class=\"btn\" value=\"Invite\">"
    << "</form>";

  EndPage(html,OpenMCU::Current().GetCopyrightText()); msg = html;

  return TRUE;
}

PCREATE_SERVICE_MACRO_BLOCK(RoomList,P_EMPTY,P_EMPTY,block)
{
  return OpenMCU::Current().GetEndpoint().GetRoomList(block);
}

SelectRoomPage::SelectRoomPage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Select", "", "text/html; charset=windows-1251", auth),
    app(_app)
{
  PStringStream html;

  BeginPage(html,"Select Room","Select Room","$SELECT$");
  html << "<p>"
       << "<form method=\"POST\" class=\"well form-inline\">"
       << "<!--#macrostart RoomList-->"
         << "<!--#status List-->"
       << "<!--#macroend RoomList-->"
       << "&nbsp;";
  EndPage(html,app.GetCopyrightText());

  string = html;
}


BOOL SelectRoomPage::Post(PHTTPRequest & request,
                          const PStringToString & data,
                          PHTML & msg)
{

  
  msg << OpenMCU::Current().GetEndpoint().SetRoomParams(data);
  return TRUE;
}


// End of File ///////////////////////////////////////////////////////////////
