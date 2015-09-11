
#include "precompile.h"
#include "mcu.h"
#include "html.h"

#define new PNEW

///////////////////////////////////////////////////////////////

VideoMixConfigurator OpenMCU::vmcfg;

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
void MCUSignalHandler(int sig)
{
  PProcess & process = PProcess::Current();
  switch(sig)
  {
    case SIGINT:
      MCUTRACE(0, "OpenMCU-ru received signal " << sig << " SIGINT");
      process.Terminate();
    case SIGTERM:
      MCUTRACE(0, "OpenMCU-ru received signal " << sig << " SIGTERM");
      process.Terminate();
    case SIGPIPE:
      MCUTRACE(0, "OpenMCU-ru received signal " << sig << " SIGPIPE");
    default:
      return;
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

OpenMCUPreInit::OpenMCUPreInit()
{
  //setenv("PWLIB_TRACE_STARTUP", "6", 1);
  //setenv("PWLIB_TRACE_LEVEL", "6", 1);
  //setenv("PWLIB_TRACE_FILE", PString(SERVER_LOGS) + PATH_SEPARATOR + "trace_startup.txt", 1);
  setenv("PWLIBPLUGINDIR", MCU_PLUGIN_DIR, 1);
  pluginCodecManager = new MCUPluginCodecManager();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

OpenMCU::OpenMCU()
  : OpenMCUPreInit(), OpenMCUProcessAncestor(ProductInfo)
{
#ifndef _WIN32
  // new versions ptlib hang on signal SIGINT
  signal(SIGINT, &MCUSignalHandler);
  signal(SIGTERM, &MCUSignalHandler);
  // PTCPSocket caused SIGPIPE on browser disconnect time to time
  signal(SIGPIPE, &MCUSignalHandler);
#endif
  endpoint          = NULL;
  sipendpoint       = NULL;
  rtspServer        = NULL;
  registrar         = NULL;
  currentLogLevel   = -1;
  currentTraceLevel = -1;
  traceFileRotated  = FALSE;

  videoResizeDeltaTSCSum=0;
  videoResizeDeltaTSCCounter=0;
  videoResizeDeltaTSCReportTime=0;

  httpBufferIndex = 0;
  httpBufferComplete = 0;

  uniqueMemberID = 1000;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::Main()
{
  Suspend();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpenMCU::OnStart()
{
#if MCU_PTLIB_VERSION == MCU_PTLIB_VERSION_INT(2,0,1)
  char ** argv=PXGetArgv();
  executableFile = argv[0];
  PDirectory exeDir = executableFile.GetDirectory();
  exeDir.Change();
#endif

  SetConfigurationPath(CONFIG_PATH);
  trace_section = "OpenMCU-ru ";
  InitialiseTrace();
  PrintOnStartInfo();

  vmcfg.go(vmcfg.bfw,vmcfg.bfh);

  MCUPluginCodecManager::PopulateMediaFormats();

  manager  = new ConferenceManager();
  endpoint = new MCUH323EndPoint(*manager);
  sipendpoint = new MCUSipEndPoint(endpoint);
  registrar = new Registrar(endpoint, sipendpoint);
  rtspServer = new MCURtspServer(endpoint, sipendpoint);
  telnetServer = new MCUTelnetServer();

  //
  httpNameSpace.AddResource(new PHTTPDirectory("data", "data"));
  httpNameSpace.AddResource(new PServiceHTTPDirectory("html", "html"));
  // Initialise()
  BOOL ret = PHTTPServiceProcess::OnStart();

  // start threads
  sipendpoint->Resume();
  registrar->Resume();

  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::OnStop()
{
  // shutdown http listener
  MCUHTTPListenerShutdown();

  // telnet
  delete telnetServer;

  // close endpoints listeners
  rtspServer->RemoveListeners();
  //sipendpoint->RemoveListeners();
  endpoint->RemoveListener(NULL);

  // clear all calls
  manager->ClearConferenceList();
  //endpoint->ClearAllCalls();

  // delete rtsp endpoint
  delete rtspServer;

  // stop sip
  sipendpoint->SetTerminating();
  sipendpoint->WaitForTermination(10000);

  // stop registrar
  registrar->SetTerminating();
  registrar->WaitForTermination(10000);

  // delete h323 endpoint
  delete endpoint;
  endpoint = NULL;

  // delete sip
  delete sipendpoint;
  sipendpoint = NULL;

  // delete registrar
  delete registrar;
  registrar = NULL;

  delete manager;
  manager = NULL;

#ifndef _WIN32
  CommonDestruct(); // save config
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::OnConfigChanged()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpenMCU::Initialise(const char * initMsg)
{
  PDirectory exeDir = executableFile.GetDirectory();
  exeDir.Change();

#ifdef _WIN32
  if(PXGetArgc()>1)
  {
    char ** argv=PXGetArgv();
    PString arg1=argv[1];
    if(arg1 *= "debug") cout.rdbuf(PError.rdbuf());
  }
#endif

  MCUConfig cfg("Parameters");

  serverId = cfg.GetString(ServerIdKey, OpenMCU::Current().GetName() + " v" + OpenMCU::Current().GetVersion());

  InitialiseTrace();

#ifdef SERVER_LOGS
  // default log file name
  logFilename = cfg.GetString(CallLogFilenameKey, DefaultCallLogFilename);
  if(logFilename.Find(PATH_SEPARATOR) == P_MAX_INDEX)
  {
    logFilename = PString(SERVER_LOGS) + PATH_SEPARATOR + logFilename;
    cfg.SetString(CallLogFilenameKey, logFilename);
  }
#endif
  copyWebLogToLog = cfg.GetBoolean("Copy web log to call log", FALSE);

  // Buffered events
  httpBuffer=cfg.GetInteger(HttpLinkEventBufferKey, 100);
  httpBufferedEvents.SetSize(httpBuffer);

#if MCU_VIDEO
  endpoint->enableVideo = cfg.GetBoolean("Enable video", TRUE);
  endpoint->videoFrameRate = MCUConfig("Video").GetInteger("Video frame rate", DefaultVideoFrameRate);
  endpoint->videoTxQuality = cfg.GetInteger("Video quality", DefaultVideoQuality);

#if USE_LIBYUV
  SetScaleFilterType(libyuv::LIBYUV_FILTER+1);
#else
  SetScaleFilterType(SCALE_FILTER);
#endif
#endif

#if P_SSL
  // Secure HTTP
  BOOL enableSSL = cfg.GetBoolean(HTTPSecureKey, FALSE);
  disableSSL = !enableSSL;
  if(enableSSL)
  {
    PString certificateFile = cfg.GetString(HTTPCertificateFileKey, DefaultHTTPCertificateFile);
    if (!SetServerCertificate(certificateFile, TRUE)) {
      PSYSTEMLOG(Fatal, "MCU\tCould not load certificate \"" << certificateFile << '"');
      return FALSE;
    }
  }
#endif

  // auto dial delay
  PString addtmp=cfg.GetString(AutoDialDelayKey, 1);
  if(addtmp=="X") autoDialDelay = 999999;
  else autoDialDelay = addtmp.AsInteger();

  // get default "room" (conference) name
  defaultRoomName = cfg.GetString(DefaultRoomKey, DefaultRoom);

  // video recorder setup
  vr_ffmpegDir   = cfg.GetString( RecorderFfmpegDirKey,   DefaultRecordingDirectory);
  vr_minimumSpaceMiB = 1024;

  // get WAV file played to a user when they enter a conference
  connectingWAVFile = cfg.GetString(ConnectingWAVFileKey, DefaultConnectingWAVFile);

  // get WAV file played to a conference when a new user enters
  enteringWAVFile = cfg.GetString(EnteringWAVFileKey, DefaultEnteringWAVFile);

  // get WAV file played to a conference when a new user enters
  leavingWAVFile = cfg.GetString(LeavingWAVFileKey, DefaultLeavingWAVFile);

  CreateHTTPResource("Parameters");
  CreateHTTPResource("ConferenceParameters");
  CreateHTTPResource("TelnetServer");
  CreateHTTPResource("ExportParameters");
  CreateHTTPResource("RegistrarParameters");
  CreateHTTPResource("ManagingUsers");
  CreateHTTPResource("ManagingGroups");
  CreateHTTPResource("ControlCodes");
  CreateHTTPResource("H323Parameters");
  CreateHTTPResource("SIPParameters");
  CreateHTTPResource("H323EndpointsParameters");
  CreateHTTPResource("SipEndpointsParameters");
  CreateHTTPResource("RtspParameters");
  CreateHTTPResource("RtspServers");
  CreateHTTPResource("RtspEndpoints");
  CreateHTTPResource("VideoParameters");
  CreateHTTPResource("ProxyServers");
  CreateHTTPResource("ReceiveSoundCodecs");
  CreateHTTPResource("TransmitSoundCodecs");
  CreateHTTPResource("ReceiveVideoCodecs");
  CreateHTTPResource("TransmitVideoCodecs");
  CreateHTTPResource("SipSoundCodecs");
  CreateHTTPResource("SipVideoCodecs");

  CreateHTTPResource("Status");
  CreateHTTPResource("Invite");
  CreateHTTPResource("Select");
  CreateHTTPResource("Records");
  CreateHTTPResource("Jpeg");
  CreateHTTPResource("Comm");

  CreateHTTPResource("welcome.html");
  CreateHTTPResource("monitor.txt");

  // adding web server links (eg. images):
#ifdef SYS_RESOURCE_DIR
#  define WEBSERVER_LINK(r1) httpNameSpace.AddResource(new PHTTPFile(r1, PString(SYS_RESOURCE_DIR) + PATH_SEPARATOR + r1), PHTTPSpace::Overwrite)
#  define WEBSERVER_LINK_MIME(mt1,r1) httpNameSpace.AddResource(new PHTTPFile(r1, PString(SYS_RESOURCE_DIR) + PATH_SEPARATOR + r1, mt1), PHTTPSpace::Overwrite)
#  define WEBSERVER_LINK_MIME_CFG(mt1,r1) httpNameSpace.AddResource(new PHTTPFile(r1, PString(SYS_CONFIG_DIR) + PATH_SEPARATOR + r1, mt1), PHTTPSpace::Overwrite)
#else
#  define WEBSERVER_LINK(r1) httpNameSpace.AddResource(new PHTTPFile(r1), PHTTPSpace::Overwrite)
#  define WEBSERVER_LINK_MIME(mt1,r1) httpNameSpace.AddResource(new PHTTPFile(r1, r1, mt1), PHTTPSpace::Overwrite)
#  define WEBSERVER_LINK_MIME_CFG(mt1,r1) httpNameSpace.AddResource(new PHTTPFile(r1, r1, mt1), PHTTPSpace::Overwrite)
#endif
#define WEBSERVER_LINK_LOGS(mt1,r1) httpNameSpace.AddResource(new PHTTPFile(r1, PString(SERVER_LOGS) + PATH_SEPARATOR + r1, mt1), PHTTPSpace::Overwrite)
  WEBSERVER_LINK_MIME("text/javascript"          , "control.js");
  WEBSERVER_LINK_MIME("text/javascript"          , "status.js");
  WEBSERVER_LINK_MIME("text/javascript"          , "local_en.js");
  WEBSERVER_LINK_MIME("text/javascript"          , "local_fr.js");
  WEBSERVER_LINK_MIME("text/javascript"          , "local_jp.js");
  WEBSERVER_LINK_MIME("text/javascript"          , "local_pt.js");
  WEBSERVER_LINK_MIME("text/javascript"          , "local_ru.js");
  WEBSERVER_LINK_MIME("text/javascript"          , "local_uk.js");
  WEBSERVER_LINK_MIME("text/css"                 , "main.css");
  WEBSERVER_LINK_MIME("image/gif"                , "i15_inv.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "launched.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i20_close.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i20_vad.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i20_vad2.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i20_static.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i20_plus.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i24_shuff.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i24_left.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i24_right.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i24_mix.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i24_clr.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i24_revert.gif");
  WEBSERVER_LINK_MIME("image/png"                , "logo_text.png");
  WEBSERVER_LINK_MIME("image/png"                , "menu_left.png");
  WEBSERVER_LINK_MIME("image/png"                , "s32_ch.png");
  WEBSERVER_LINK_MIME("image/vnd.microsoft.icon" , "mcu.ico");
  WEBSERVER_LINK_MIME("image/vnd.microsoft.icon" , "mcu.gif");
  WEBSERVER_LINK_MIME("image/png"                , "i16_close_gray.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_close_red.png");
  WEBSERVER_LINK_MIME("image/png"                , "i32_lock.png");
  WEBSERVER_LINK_MIME("image/png"                , "i32_lockopen.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_status_gray.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_status_green.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_status_blue.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_status_red.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_minus.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_plus.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_save.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_abook_plus.png");

  WEBSERVER_LINK_MIME_CFG("image/bmp"            , "logo.bmp");
  WEBSERVER_LINK_MIME_CFG("image/jpeg"           , "logo.jpeg");
  WEBSERVER_LINK_MIME_CFG("image/png"            , "logo.png");
  WEBSERVER_LINK_MIME_CFG("image/gif"            , "logo.gif");

  // determine logo filename
  GetLogoFilename();

  for(PINDEX i=-1; i<rotationLevel; i++)
  {
    PString s="trace";
    if(i>=0) s+=PString(i);
    s+=".txt";
    WEBSERVER_LINK_LOGS("application/octet-stream", s);
  }

  // set up the HTTP port for listening & start the first HTTP thread
  PString ip = cfg.GetString(HttpIPKey, "0.0.0.0");
  WORD port = cfg.GetInteger(HttpPortKey, DefaultHTTPPort);
  if(MCUHTTPListenerCreate(ip, port))
  {
    PSYSTEMLOG(Info, "Opened master socket for HTTP: " << ip << ":" << port);
    PTRACE(0, trace_section << "Opened master socket for HTTP: " << ip << ":" << port);
  } else {
    PSYSTEMLOG(Fatal, "Failed open master socket: " <<  ip << ":" << port << " " << httpListeningSocket->GetErrorText());
    PTRACE(0, trace_section << "Failed open master socket: " <<  ip << ":" << port << " " << httpListeningSocket->GetErrorText());
    return FALSE;
  }

  // auto create
  PStringList sect = cfg.GetSections();
  PString sectionPrefix = "Conference ";
  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    if(sect[i].Left(sectionPrefix.GetLength()) == sectionPrefix)
    {
      PString roomname = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());
      if(GetConferenceParam(roomname, RoomAutoCreateKey, FALSE) && !GetConferenceParam(roomname, RoomAutoDeleteEmptyKey, FALSE))
      {
        Conference *conference = manager->MakeConferenceWithLock(roomname, "", TRUE);
        if(conference)
          conference->Unlock();
      }
    }
  }

  PSYSTEMLOG(Info, "Service " << GetName() << ' ' << initMsg);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::CreateHTTPResource(const PString & name)
{
  // Get the HTTP authentication info
  MCUConfig("Managing Groups").SetString("administrator", "");
  MCUConfig("Managing Groups").SetString("conference manager", "");

  PHTTPMultiSimpAuth authSettings(GetName());
  PHTTPMultiSimpAuth authConference(GetName());
  PStringList keysUsers = MCUConfig("Managing Users").GetKeys();
  for(PINDEX i = 0; i < keysUsers.GetSize(); i++)
  {
    PStringArray params = MCUConfig("Managing Users").GetString(keysUsers[i]).Tokenise(",");
    if(params.GetSize() < 2) continue;
    if(params[1] == "administrator")
    {
      authSettings.AddUser(keysUsers[i], PHTTPPasswordField::Decrypt(params[0]));
      authConference.AddUser(keysUsers[i], PHTTPPasswordField::Decrypt(params[0]));
    }
    if(params[1] == "conference manager")
    {
      authConference.AddUser(keysUsers[i], PHTTPPasswordField::Decrypt(params[0]));
    }
  }

  if(name == "Parameters")
    httpNameSpace.AddResource(new GeneralPConfigPage(*this, name, "Parameters", authSettings), PHTTPSpace::Overwrite);
  else if(name == "ConferenceParameters")
    httpNameSpace.AddResource(new ConferencePConfigPage(*this, name, "Conference Parameters", authSettings), PHTTPSpace::Overwrite);
  else if(name == "ExportParameters")
    httpNameSpace.AddResource(new ExportPConfigPage(*this, name, "Export Parameters", authSettings), PHTTPSpace::Overwrite);
  else if(name == "RegistrarParameters")
    httpNameSpace.AddResource(new RegistrarPConfigPage(*this, name, "Registrar Parameters", authSettings), PHTTPSpace::Overwrite);
  else if(name == "ManagingUsers")
    httpNameSpace.AddResource(new ManagingUsersPConfigPage(*this, name, "Managing Users", authSettings), PHTTPSpace::Overwrite);
  else if(name == "ManagingGroups")
    httpNameSpace.AddResource(new ManagingGroupsPConfigPage(*this, name, "Managing Groups", authSettings), PHTTPSpace::Overwrite);
  else if(name == "ControlCodes")
    httpNameSpace.AddResource(new ControlCodesPConfigPage(*this, name, "Control Codes", authSettings), PHTTPSpace::Overwrite);
  //else if(name == "RoomCodes")
  //  httpNameSpace.AddResource(new RoomCodesPConfigPage(*this, name, "Room Codes", authSettings), PHTTPSpace::Overwrite);
  else if(name == "TelnetServer")
    httpNameSpace.AddResource(new TelnetServerPConfigPage(*this, name, "Telnet Server", authSettings), PHTTPSpace::Overwrite);
  else if(name == "H323Parameters")
    httpNameSpace.AddResource(new H323PConfigPage(*this, name, "H323 Parameters", authSettings), PHTTPSpace::Overwrite);
  else if(name == "SIPParameters")
    httpNameSpace.AddResource(new SIPPConfigPage(*this, name, "SIP Parameters", authSettings), PHTTPSpace::Overwrite);
  else if(name == "H323EndpointsParameters")
    httpNameSpace.AddResource(new H323EndpointsPConfigPage(*this, name, "H323 Endpoints", authSettings), PHTTPSpace::Overwrite);
  else if(name == "SipEndpointsParameters")
    httpNameSpace.AddResource(new SipEndpointsPConfigPage(*this, name, "SIP Endpoints", authSettings), PHTTPSpace::Overwrite);
  else if(name == "RtspParameters")
    httpNameSpace.AddResource(new RtspPConfigPage(*this, name, "RTSP Parameters", authSettings), PHTTPSpace::Overwrite);
  else if(name == "RtspServers")
    httpNameSpace.AddResource(new RtspServersPConfigPage(*this, name, "RTSP Servers", authSettings), PHTTPSpace::Overwrite);
  else if(name == "RtspEndpoints")
    httpNameSpace.AddResource(new RtspEndpointsPConfigPage(*this, name, "RTSP Endpoints", authSettings), PHTTPSpace::Overwrite);
  else if(name == "VideoParameters")
    httpNameSpace.AddResource(new VideoPConfigPage(*this, name, "Video", authSettings), PHTTPSpace::Overwrite);
  //else if(name == "RoomAccess")
  //  httpNameSpace.AddResource(new RoomAccessSIPPConfigPage(*this, name, "RoomAccess", authSettings), PHTTPSpace::Overwrite);
  else if(name == "ProxyServers")
    httpNameSpace.AddResource(new ProxySIPPConfigPage(*this, name, "ProxyServers", authSettings), PHTTPSpace::Overwrite);
  else if(name == "ReceiveSoundCodecs")
    httpNameSpace.AddResource(new CodecsPConfigPage(*this, name, "RECEIVE_SOUND", authSettings), PHTTPSpace::Overwrite);
  else if(name == "TransmitSoundCodecs")
    httpNameSpace.AddResource(new CodecsPConfigPage(*this, name, "TRANSMIT_SOUND", authSettings), PHTTPSpace::Overwrite);
  else if(name == "ReceiveVideoCodecs")
    httpNameSpace.AddResource(new CodecsPConfigPage(*this, name, "RECEIVE_VIDEO", authSettings), PHTTPSpace::Overwrite);
  else if(name == "TransmitVideoCodecs")
    httpNameSpace.AddResource(new CodecsPConfigPage(*this, name, "TRANSMIT_VIDEO", authSettings), PHTTPSpace::Overwrite);
  else if(name == "SipSoundCodecs")
    httpNameSpace.AddResource(new SIPCodecsPConfigPage(*this, name, "SIP Audio", authSettings), PHTTPSpace::Overwrite);
  else if(name == "SipVideoCodecs")
    httpNameSpace.AddResource(new SIPCodecsPConfigPage(*this, name, "SIP Video", authSettings), PHTTPSpace::Overwrite);

  else if(name == "Status")
    httpNameSpace.AddResource(new MainStatusPage(*this, authConference), PHTTPSpace::Overwrite);
  else if(name == "Invite")
    httpNameSpace.AddResource(new InvitePage(*this, authConference), PHTTPSpace::Overwrite);
  else if(name == "Select")
    httpNameSpace.AddResource(new SelectRoomPage(*this, authConference), PHTTPSpace::Overwrite);
  else if(name == "Records")
    httpNameSpace.AddResource(new RecordsBrowserPage(*this, authConference), PHTTPSpace::Overwrite);
  else if(name == "Jpeg")
    httpNameSpace.AddResource(new JpegFrameHTTP(*this, authConference), PHTTPSpace::Overwrite);
  else if(name == "Comm")
    httpNameSpace.AddResource(new InteractiveHTTP(*this, authConference), PHTTPSpace::Overwrite);

  else if(name == "welcome.html")
    httpNameSpace.AddResource(new WelcomePage(*this, authConference), PHTTPSpace::Overwrite);
  else if(name == "monitor.txt")
  {
    PString monitorText =
#ifdef GIT_REVISION
                        (PString("OpenMCU-ru REVISION ") + MCU_STRINGIFY(GIT_REVISION) +"\n\n") +
#endif
                        "<!--#equival monitorinfo-->"
                        "<!--#equival mcuinfo-->";
    httpNameSpace.AddResource(new PServiceHTTPString("monitor.txt", monitorText, "text/plain", authConference), PHTTPSpace::Overwrite);
  }

  // Add log file links
/*
  if (!systemLogFileName && (systemLogFileName != "-")) {
    httpNameSpace.AddResource(new PHTTPFile("logfile.txt", systemLogFileName, authority));
    httpNameSpace.AddResource(new PHTTPTailFile("tail_logfile", systemLogFileName, authority));
  }
*/

}

////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::InitialiseTrace()
{
#if PTRACING
  MCUConfig cfg("Parameters");
  int TraceLevel = cfg.GetInteger(TraceLevelKey, DEFAULT_TRACE_LEVEL);
  int LogLevel = cfg.GetInteger(LogLevelKey, DEFAULT_LOG_LEVEL);
  if(currentLogLevel != LogLevel)
  {
    SetLogLevel((PSystemLog::Level)LogLevel);
    currentLogLevel = LogLevel;
  }
  rotationLevel = cfg.GetInteger(TraceRotateKey, 0);
  if(currentTraceLevel != TraceLevel)
  {
    if(!traceFileRotated)
    {
      if(rotationLevel != 0)
      {
        PString
          pfx = PString(SERVER_LOGS) + PATH_SEPARATOR + "trace",
          sfx = ".txt";
        PFile::Remove(pfx + PString(rotationLevel-1) + sfx, TRUE);
        for (PINDEX i=rotationLevel-1; i>0; i--) PFile::Move(pfx + PString(i-1) + sfx, pfx + PString(i) + sfx, TRUE);
        PFile::Move(pfx + sfx, pfx + "0" + sfx, TRUE);
      }
      traceFileRotated = TRUE;
    }

    PTrace::Initialise(TraceLevel, PString(SERVER_LOGS) + PATH_SEPARATOR + "trace.txt", PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
    // PTRACE_BLOCK gkserver.cxx
    if(TraceLevel >= 3)
      PTrace::SetOptions(PTrace::Blocks);

    currentTraceLevel = TraceLevel;
    PTRACE(0, trace_section << "Trace Level " << currentTraceLevel);
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::LogMessage(const PString & str)
{
  static PMutex logMutex;
  static PTextFile logFile;

  PTime now;
  PString msg = now.AsString("dd/MM/yyyy") & str;
  logMutex.Wait();

  if (!logFile.IsOpen()) {
    if(!logFile.Open(logFilename, PFile::ReadWrite))
    {
      PTRACE(1, trace_section << "Can not open log file: " << logFilename << "\n" << msg << flush);
      logMutex.Signal();
      return;
    }
    if(!logFile.SetPosition(0, PFile::End))
    {
      PTRACE(1, trace_section << "Can not change log position, log file name: " << logFilename << "\n" << msg << flush);
      logFile.Close();
      logMutex.Signal();
      return;
    }
  }

  if(!logFile.WriteLine(msg))
  {
    PTRACE(1, trace_section << "Can not write to log file: " << logFilename << "\n" << msg << flush);
  }
  logFile.Close();
  logMutex.Signal();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::LogMessageHTML(PString str)
{
  PString str2, roomName;
  PINDEX tabPos=str.Find("\t");
  if(tabPos!=P_MAX_INDEX)
  {
    roomName=str.Left(tabPos);
    str=str.Mid(tabPos+1,P_MAX_INDEX);
  }
  if(str.GetLength()>8)
  {
    if(str[1]==':') str=PString("0")+str;
    if(!roomName.IsEmpty()) str=str.Left(8)+" "+roomName+str.Mid(9,P_MAX_INDEX);
  }
  BOOL tag=FALSE;
  for (PINDEX i=0; i< str.GetLength(); i++)
  if(str[i]=='<') tag=TRUE;
  else if(str[i]=='>') tag=FALSE;
  else if(!tag) str2+=str[i];
  LogMessage(str2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpenMCU::MCUHTTPListenerCreate(const PString & ip, unsigned port)
{
  if(httpListeningSocket)
  {
    PString curAddress = ((PTCPSocket *)httpListeningSocket)->GetLocalAddress();
    if(curAddress == ip + ":" + PString(port))
      return TRUE;
  }

  PSocket::Reusability reuse = PSocket::CanReuseAddress;
  PINDEX stackSize = 0x4000;

  MCUHTTPListenerDelete();

  PIPSocket::Address address(ip);
  PTCPSocket *listener = new PTCPSocket(port);

  if(!listener->Listen(address, 5, 0, reuse))
  {
    PTRACE(0, trace_section << "Listen on address " << ip << ":" << port << " failed: " << listener->GetErrorText());
    return FALSE;
  }

  httpListeningSocket = PAssertNULL(listener);

  if(stackSize > 1000)
    new PHTTPServiceThread(stackSize, *this);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::MCUHTTPListenerShutdown()
{
  if(httpListeningSocket == NULL)
    return;

  if(!httpListeningSocket->IsOpen())
    return;

  PTRACE(0, trace_section << "Closing listener socket");
  httpListeningSocket->Close();

  httpThreadsMutex.Wait();
  int threshold = 0;
  for(PINDEX i = 0; i < httpThreads.GetSize(); ++i)
  {
    if(&httpThreads[i] != PThread::Current())
      httpThreads[i].Close();
    else
      threshold = 1;
  }
  httpThreadsMutex.Signal();

  while(httpThreads.GetSize() > threshold)
    Sleep(10);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::MCUHTTPListenerDelete()
{
  MCUHTTPListenerShutdown();
  delete httpListeningSocket;
  httpListeningSocket = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::PrintOnStartInfo()
{
#ifdef GIT_REVISION
  PTRACE(0, trace_section << "git revision: " << MCU_STRINGIFY(GIT_REVISION));
#endif
#ifdef __VERSION__
  PTRACE(0, trace_section << "GCC " << __VERSION__);
#endif
#ifdef PTLIB_VERSION
  PTRACE(0, trace_section << "PTLib " << PTLIB_VERSION);
#endif
#ifdef OPENH323_VERSION
  PTRACE(0, trace_section << "H323Plus " << OPENH323_VERSION);
#endif
#ifdef SOFIA_SIP_VERSION
  PTRACE(0, trace_section << "Sofia-SIP " << SOFIA_SIP_VERSION);
#endif
#ifdef LIBYUV_VERSION
  PTRACE(0, trace_section << "libyuv " << LIBYUV_VERSION);
#endif
#ifdef LIBJPEG_TURBO_VERSION
  PTRACE(0, trace_section << "libjpeg " << MCU_STRINGIFY(LIBJPEG_TURBO_VERSION));
#endif

#ifdef LIBAVCODEC_VERSION
  PTRACE(0, trace_section << "libavcodec " << MCU_STRINGIFY(LIBAVCODEC_VERSION));
#endif
#ifdef LIBAVUTIL_VERSION
  PTRACE(0, trace_section << "libavutil " << MCU_STRINGIFY(LIBAVUTIL_VERSION));
#endif
#ifdef LIBAVFORMAT_VERSION
  PTRACE(0, trace_section << "libavformat " << MCU_STRINGIFY(LIBAVFORMAT_VERSION));
#endif
#ifdef LIBSWSCALE_VERSION
  PTRACE(0, trace_section << "libswscale " << MCU_STRINGIFY(LIBSWSCALE_VERSION));
#endif

#ifdef LIBSWRESAMPLE_VERSION_INT
  PTRACE(0, trace_section << "libswresample " << LIBSWRESAMPLE_VERSION_MAJOR << "." << LIBSWRESAMPLE_VERSION_MINOR << "." << LIBSWRESAMPLE_VERSION_MICRO);
#endif
#ifdef LIBAVRESAMPLE_VERSION
  PTRACE(0, trace_section << "libavresample " << MCU_STRINGIFY(LIBAVRESAMPLE_VERSION));
#endif
#ifdef SAMPLERATE_H
  PTRACE(0, trace_section << src_get_version());
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpenMCU::OTFControl(const PString & data, PString & rdata)
{
  PStringToString command;
  command.SetAt("v", "0");
  PStringArray tokens = data.Tokenise(" ");
  if(tokens[0] == "?")
  {
    rdata = "room 'name' ...\r\n"
            "show ...\r\n"
            ;
    return TRUE;
  }
  if(tokens[0] == "room")
  {
    if(tokens[1] == "?" || tokens.GetSize() < 3)
    {
      rdata = "room 'name' ...\r\n"
              "            create\r\n"
              "            delete\r\n"
              "            dial 'id'\r\n"
              "            invite 'address'\r\n"
              "            drop 'id'\r\n"
              "            show members\r\n"
              "            start_recorder\r\n"
              "            stop_recorder\r\n"
              ;
      return TRUE;
    }
    command.SetAt("room", tokens[1]);
    if(tokens[2] == "create")
    {
      command.SetAt("action", OTFC_ROOM_CREATE);
      return OTFControl(command, rdata);
    }
    if(tokens[2] == "delete")
    {
      command.SetAt("action", OTFC_ROOM_DELETE);
      return OTFControl(command, rdata);
    }
    if(tokens[2] == "dial")
    {
      command.SetAt("action", OTFC_DIAL);
      command.SetAt("v", tokens[3]);
      return OTFControl(command, rdata);
    }
    if(tokens[2] == "invite")
    {
      command.SetAt("action", OTFC_INVITE);
      command.SetAt("v", tokens[3]);
      return OTFControl(command, rdata);
    }
    if(tokens[2] == "drop")
    {
      command.SetAt("action", OTFC_DROP_MEMBER);
      command.SetAt("v", PString(tokens[3]));
      return OTFControl(command, rdata);
    }
    if(tokens[2] == "show")
    {
      if(tokens[3] == "members")
      {
        command.SetAt("action", OTFC_ROOM_SHOW_MEMBERS);
        return OTFControl(command, rdata);
      }
      return FALSE;
    }
    if(tokens[2] == "start_recorder")
    {
      command.SetAt("action", OTFC_VIDEO_RECORDER_START);
      return OTFControl(command, rdata);
    }
    if(tokens[2] == "stop_recorder")
    {
      command.SetAt("action", OTFC_VIDEO_RECORDER_STOP);
      return OTFControl(command, rdata);
    }
  }
  if(tokens[0] == "show")
  {
    if(tokens[1] == "?")
    {
      rdata = "show ...\r\n"
              "     registrar ...\r\n"
              "               accounts\r\n"
              ;
      return TRUE;
    }
    if(tokens[1] == "registrar")
    {
      if(tokens[2] == "accounts")
      {
        command.SetAt("action", OTFC_SHOW_REGISTRAR_ACCOUNTS);
        return OTFControl(command, rdata);
      }
      return FALSE;
    }
    return FALSE;
  }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpenMCU::OTFControl(const PStringToString & data, PString & rdata)
{
  PWaitAndSignal m(otfcMutex);

  MCUTRACE(6,"OTFControl:\n" << data);

  if(!data.Contains("action"))
    return FALSE;
  int action = data("action").AsInteger();

  if(!data.Contains("v"))
    return FALSE;
  PString value = data("v");
  long v = value.AsInteger();

  BOOL otfc_web = data.Contains("otfc");

  if(action == OTFC_ADD_TO_ABOOK)
  {
    registrar->AddAbookAccount(value);
    return TRUE;
  }
  if(action == OTFC_REMOVE_FROM_ABOOK)
  {
    registrar->RemoveAbookAccount(value);
    return TRUE;
  }
  if(action == OTFC_SHOW_REGISTRAR_ACCOUNTS)
  {
    MCUJSON json(MCUJSON::JSON_ARRAY);
    MCUAbookList & abookList = registrar->GetAbookList();
    for(MCUAbookList::shared_iterator it = abookList.begin(); it != abookList.end(); ++it)
    {
      if(it->is_account)
        json.Insert(it->AsJSON());
    }
    std::string str;
    if(otfc_web)
      json.ToString(str, false, false);
    else
      json.ToString(str, true, true);
    rdata = str;
    return TRUE;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // room
  //////////////////////////////////////////////////////////////////////////////////////////////////

  if(!data.Contains("room"))
    return FALSE;
  PString room = data("room");

  if(action == OTFC_ROOM_CREATE)
  {
    Conference * conference = manager->MakeConferenceWithLock(room, "", TRUE);
    if(conference)
      conference->Unlock();
    return TRUE;
  }
  if(action == OTFC_ROOM_DELETE)
  {
    manager->RemoveConference(room);
    return TRUE;
  }
  if(action == OTFC_YUV_FILTER_MODE)
  {
    PString filterName = SetScaleFilterType(v);
    HttpWriteEventRoom("filter: "+filterName, room);
    PStringStream cmd;
    cmd << "conf[0][10]=" << v;
    HttpWriteCmdRoom(cmd,room);
    HttpWriteCmdRoom("top_panel()",room);
    HttpWriteCmdRoom("alive()",room);
    return TRUE;
  }

  MCUConferenceList & conferenceList = manager->GetConferenceList();
  MCUConferenceList::shared_iterator cit = conferenceList.Find(room);
  if(cit == conferenceList.end())
    return FALSE;
  Conference *conference = *cit;

  if(action == OTFC_ROOM_SHOW_MEMBERS)
  {
    MCUJSON json(MCUJSON::JSON_ARRAY);
    MCUMemberList & memberList = conference->GetMemberList();
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
      json.Insert(it->AsJSON());
    std::string str;
    json.ToString(str, true, true);
    rdata = str;
    return TRUE;
  }
  if(action == OTFC_REFRESH_VIDEO_MIXERS)
  {
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("mixrfr()",room);
    return TRUE;
  }
  if(action == OTFC_VIDEO_RECORDER_START)
  {
    if(conference->StartRecorder())
    {
      return TRUE;
    }
    return FALSE;
  }
  if(action == OTFC_VIDEO_RECORDER_STOP)
  {
    conference->StopRecorder();
    return TRUE;
  }
  if(action == OTFC_TEMPLATE_RECALL)
  {
    HttpWriteCmdRoom("alive()",room);

    if(conference->IsModerated()=="-")
    {
      conference->SetModerated(TRUE);
      MCUSimpleVideoMixer * mixer = manager->GetVideoMixerWithLock(conference);
      if(mixer)
      {
        mixer->SetForceScreenSplit(TRUE);
        mixer->Unlock();
      }
      conference->PutChosenVan();
      HttpWriteEventRoom("<span style='background-color:#bfb'>Operator took the control</span>",room);
      HttpWriteCmdRoom("r_moder()",room);
    }

    conference->confTpl = conference->ExtractTemplate(value);
    conference->LoadTemplate(conference->confTpl);
    conference->SetLastUsedTemplate(value);
    PStringStream msg;
    msg << endpoint->GetMemberListOptsJavascript(*conference) << "\n"
        << "p." << endpoint->GetConferenceOptsJavascript(*conference) << "\n"
        << "p.tl=Array" << conference->GetTemplateList() << "\n"
        << "p.seltpl=\"" << conference->GetSelectedTemplateName() << "\"\n"
        << "p.build_page()";
    HttpWriteCmdRoom(msg,room);
    return TRUE;
  }
  if(action == OTFC_SAVE_TEMPLATE)
  {
    HttpWriteCmdRoom("alive()",room);
    PString templateName=value.Trim();
    if(templateName=="") return FALSE;
    if(templateName.Right(1) == "*") templateName=templateName.Left(templateName.GetLength()-1).RightTrim();
    if(templateName=="") return FALSE;
    conference->confTpl = conference->SaveTemplate(templateName);
    conference->TemplateInsertAndRewrite(templateName, conference->confTpl);
    conference->LoadTemplate(conference->confTpl);
    PStringStream msg;
    msg << endpoint->GetMemberListOptsJavascript(*conference) << "\n"
        << "p." << endpoint->GetConferenceOptsJavascript(*conference) << "\n"
        << "p.tl=Array" << conference->GetTemplateList() << "\n"
        << "p.seltpl=\"" << conference->GetSelectedTemplateName() << "\"\n"
        << "p.build_page()";
      HttpWriteCmdRoom(msg,room);
    return TRUE;
  }
  if(action == OTFC_DELETE_TEMPLATE)
  {
    HttpWriteCmdRoom("alive()",room);
    PString templateName=value.Trim();
    if(templateName=="") return FALSE;
    if(templateName.Right(1) == "*") return FALSE;
    conference->DeleteTemplate(templateName);
    conference->LoadTemplate("");
    conference->SetLastUsedTemplate("");
    PStringStream msg;
    msg << endpoint->GetMemberListOptsJavascript(*conference) << "\n"
        << "p." << endpoint->GetConferenceOptsJavascript(*conference) << "\n"
        << "p.tl=Array" << conference->GetTemplateList() << "\n"
        << "p.seltpl=\"" << conference->GetSelectedTemplateName() << "\"\n"
        << "p.build_page()";
      HttpWriteCmdRoom(msg,room);
    return TRUE;
  }
  if(action == OTFC_TOGGLE_TPL_LOCK)
  {
    PString templateName=value.Trim();
    if((templateName.IsEmpty())||(templateName.Right(1) == "*")) if(!conference->lockedTemplate) return FALSE;
    conference->lockedTemplate = !conference->lockedTemplate;
    if(conference->lockedTemplate) HttpWriteCmdRoom("tpllck(1)",room);
    else HttpWriteCmdRoom("tpllck(0)",room);
    return TRUE;
  }
  if(action == OTFC_INVITE || action == OTFC_ADD_AND_INVITE)
  {
    PString memberName = value;
    ConferenceMember *member = manager->FindMemberSimilarWithLock(conference, memberName);
    if(member == NULL)
    {
      member = new MCUConnection_ConferenceMember(conference, memberName, "");
      MCUMemberList::shared_iterator it = conference->AddMemberToList(member);
      if(it == conference->GetMemberList().end())
      {
        delete member;
        member = NULL;
      }
      else
        member = it.GetCapturedObject();
    }
    if(member)
    {
      member->Dial();
      member->Unlock();
      return TRUE;
    }
    return FALSE;
  }
  if(action == OTFC_REMOVE_OFFLINE_MEMBER)
  {
    PWaitAndSignal m(conference->GetMemberListMutex());
    MCUMemberList & memberList = conference->GetMemberList();
    MCUMemberList::shared_iterator it = memberList.Find(value);
    if(it != memberList.end())
    {
      ConferenceMember *member = *it;
      if(!member->IsSystem() && !member->IsOnline())
      {
        member->Close();
        if(memberList.Erase(it))
          delete member;
      }
    }
    PStringStream msg;
    msg << endpoint->GetMemberListOptsJavascript(*conference) << "\n"
        << "p.members_refresh()";
    HttpWriteCmdRoom(msg,room);
    return TRUE;
  }
  if(action == OTFC_DROP_ALL_ACTIVE_MEMBERS)
  {
    PWaitAndSignal m(conference->GetMemberListMutex());
    MCUMemberList & memberList = conference->GetMemberList();
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
    {
      ConferenceMember * member = *it;
      if(!member->IsSystem())
      {
        member->SetAutoDial(FALSE);
        if(member->IsOnline())
          member->Close();
      }
    }
    HttpWriteEventRoom("Active members dropped by operator",room);
    HttpWriteCmdRoom("drop_all()",room);
    return TRUE;
  }
  if(action == OTFC_MUTE_ALL || action == OTFC_UNMUTE_ALL)
  {
    MCUMemberList & memberList = conference->GetMemberList();
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
    {
      ConferenceMember * member = *it;
      if(member->IsSystem())
        continue;
      if(action == OTFC_MUTE_ALL) member->SetChannelPauses  (1);
      else                        member->UnsetChannelPauses(1);
    }
    return TRUE;
  }
  if(action == OTFC_INVITE_ALL_INACT_MMBRS)
  {
    PWaitAndSignal m(conference->GetMemberListMutex());
    MCUMemberList & memberList = conference->GetMemberList();
    BOOL dial=(v==1);
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
    {
      ConferenceMember * member = *it;
      if(!member->IsSystem())
      {
        if(member->IsOnline())
          member->SetAutoDial(dial);
        else
          member->Dial(dial);
      }
    }
    return TRUE;
  }
  if(action == OTFC_REMOVE_ALL_INACT_MMBRS)
  {
    PWaitAndSignal m(conference->GetMemberListMutex());
    MCUMemberList & memberList = conference->GetMemberList();
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
    {
      ConferenceMember *member = *it;
      if(!member->IsSystem() && !member->IsOnline())
      {
        member->Close();
        if(memberList.Erase(it))
          delete member;
      }
    }
    HttpWriteEventRoom("Offline members removed by operator",room);
    HttpWriteCmdRoom("remove_all()",room);
    return TRUE;
  }
  if(action == OTFC_TAKE_CONTROL)
  {
    if(conference->IsModerated()=="-")
    {
      conference->SetModerated(TRUE);
      MCUSimpleVideoMixer * mixer = manager->GetVideoMixerWithLock(conference);
      if(mixer)
      {
        mixer->SetForceScreenSplit(TRUE);
        mixer->Unlock();
      }
      conference->PutChosenVan();
      HttpWriteEventRoom("<span style='background-color:#bfb'>Operator took the control</span>",room);
      HttpWriteCmdRoom("r_moder()",room);
    }
    return TRUE;
  }
  if(action == OTFC_DECONTROL)
  {
    if(conference->IsModerated()=="+")
    {
      conference->SetModerated(FALSE);
      MCUSimpleVideoMixer * mixer = manager->GetVideoMixerWithLock(conference);
      if(mixer)
      {
        mixer->SetForceScreenSplit(FALSE);
        mixer->Unlock();
      }
      endpoint->UnmoderateConference(*conference);
      HttpWriteEventRoom("<span style='background-color:#acf'>Operator resigned</span>",room);
      HttpWriteCmdRoom("r_unmoder()",room);
      HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
      HttpWriteCmdRoom("build_page()",room);
    }
    return TRUE;
  }
  if(action == OTFC_ADD_VIDEO_MIXER)
  {
    if(conference->GetVideoMixerList().GetSize() == 0)
      return FALSE;
    if(conference->IsModerated()=="+")
    {
      unsigned n = manager->AddVideoMixer(conference);
      PStringStream msg; msg << "Video mixer " << n << " added";
      HttpWriteEventRoom(msg,room);
      HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
      HttpWriteCmdRoom("mmw=-1;p.build_page()",room);
      return TRUE;
    }
    return FALSE;
  }
  if(action == OTFC_DELETE_VIDEO_MIXER)
  {
    if(conference->GetVideoMixerList().GetSize() == 0)
      return FALSE;
    if(conference->IsModerated()=="+")
    {
      unsigned n_old = conference->GetVideoMixerList().GetSize();
      unsigned n = manager->DeleteVideoMixer(conference, v);
      if(n_old != n)
      {
        PStringStream msg; msg << "Video mixer " << v << " removed";
        HttpWriteEventRoom(msg,room);
        HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
        HttpWriteCmdRoom("mmw=-1;p.build_page()",room);
        return TRUE;
      }
    }
    return FALSE;
  }
  if(action == OTFC_SET_VIDEO_MIXER_LAYOUT)
  {
    long option = data("o").AsInteger();
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, option);
    if(mixer == NULL)
      return FALSE;
    mixer->MyChangeLayout(v);
    mixer->Unlock();
    conference->PutChosenVan();
    conference->FreezeVideo(0);
    if(conference->GetVideoMixerList().GetSize() != 0)
      HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    else
      HttpWriteCmdRoom(endpoint->GetMemberListOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_REMOVE_VMP)
  {
    unsigned pos = data("o").AsInteger();
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, v);
    if(mixer == NULL)
      return FALSE;
    mixer->MyRemoveVideoSource(pos,TRUE);
    mixer->Unlock();
    conference->FreezeVideo(0);
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_REMOVE_VMP_MEMBER)
  {
    unsigned pos = data("o").AsInteger();
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, v);
    if(mixer == NULL)
      return FALSE;
    ConferenceMemberId id=mixer->GetPositionId(pos);
    int type=-1; if(id) type=mixer->GetPositionType(id);
    if((type==2)||(type==3))
    {
      if((unsigned long)id<100) //empty VAD pos, operator probably wants to remove it completely
        mixer->MyRemoveVideoSource(pos,TRUE);
      else
        mixer->MyRemoveVideoSource(pos,FALSE);
    }
    else mixer->MyRemoveVideoSource(pos,TRUE);
    mixer->Unlock();
    conference->FreezeVideo(0);
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_MIXER_ARRANGE_VMP)
  {
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, v);
    if(mixer == NULL)
      return FALSE;
    MCUMemberList & memberList = conference->GetMemberList();
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
    {
      ConferenceMember * member = *it;
      if(member->IsVisible())
      {
        if(mixer->AddVideoSourceToLayout(member->GetID(), *(member)))
           member->SetFreezeVideo(FALSE);
        else
          break;
      }
    }
    mixer->Unlock();
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_MIXER_CLEAR)
  {
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, v);
    if(mixer == NULL)
      return FALSE;
    mixer->MyRemoveAllVideoSource();
    mixer->Unlock();
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_MIXER_SHUFFLE_VMP)
  {
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, v);
    if(mixer == NULL)
      return FALSE;
    mixer->Shuffle();
    mixer->Unlock();
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_MIXER_SCROLL_LEFT)
  {
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, v);
    if(mixer == NULL)
      return FALSE;
    mixer->Scroll(TRUE);
    mixer->Unlock();
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_MIXER_SCROLL_RIGHT)
  {
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, v);
    if(mixer == NULL)
      return FALSE;
    mixer->Scroll(FALSE);
    mixer->Unlock();
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_MIXER_REVERT)
  {
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, v);
    if(mixer == NULL)
      return FALSE;
    mixer->Revert();
    mixer->Unlock();
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_GLOBAL_MUTE)
  {
    if(data("v") == "true") v = 1;
    if(data("v") == "false") v = 0;
    conference->SetMuteUnvisible((BOOL)v);
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_SET_VAD_VALUES)
  {
    conference->VAlevel   = (unsigned short int) v;
    conference->VAdelay   = (unsigned short int) (data("o").AsInteger());
    conference->VAtimeout = (unsigned short int) (data("o2").AsInteger());
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_MOVE_VMP)
  {
    if(!data.Contains("o2"))
      return FALSE;
    long option2 = data("o2").AsInteger();
    MCUSimpleVideoMixer *mixer1 = manager->FindVideoMixerWithLock(conference, v);
    if(mixer1 == NULL)
      return FALSE;
    MCUSimpleVideoMixer *mixer2 = manager->FindVideoMixerWithLock(conference, option2);
    if(mixer2 == NULL)
    {
      mixer1->Unlock();
      return FALSE;
    }
    int pos1 = data("o").AsInteger(); int pos2 = data("o3").AsInteger();
    if(mixer1 == mixer2)
    {
      mixer1->Exchange(pos1,pos2);
    } else {
      ConferenceMemberId id = mixer1->GetPositionId(pos1); if(((long)id<100)&&((long)id>=0)) id=0;
      ConferenceMemberId id2 = mixer2->GetPositionId(pos2); if(((long)id2<100)&&((long)id2>=0)) id2=0;
      ConferenceMember *member1 = NULL;
      ConferenceMember *member2 = NULL;
      if(id) member1 = manager->FindMemberWithLock(conference, id);
      if(id2) member2 = manager->FindMemberWithLock(conference, id2);
      mixer2->PositionSetup(pos2, 1001, member1);
      mixer1->PositionSetup(pos1, 1001, member2);
      if(member1) member1->Unlock();
      if(member2) member2->Unlock();
    }
    mixer1->Unlock();
    mixer2->Unlock();
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if(action == OTFC_VAD_CLICK)
  {
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, v);
    if(mixer == NULL)
      return FALSE;
    unsigned pos = data("o").AsInteger();
    int type = data("o2").AsInteger();
    if((type<1)||(type>3)) type=2;
    long id = (long)mixer->GetPositionId(pos);
//    if((type==1)&&(id>=0)&&(id<100)) //static but no member
    if((id>=0)&&(id<100)) //just no member (and we want to add him for some reason)
    {
      BOOL setup = FALSE;
      MCUMemberList & memberList = conference->GetMemberList();
      for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
      {
        ConferenceMember * member = *it;
        if(member->IsVisible() && member->IsOnline())
        {
          if(mixer->VMPFind(member->GetID()) == mixer->vmpList.end() && ((type==1)||(!member->disableVAD)))
          {
            mixer->PositionSetup(pos, type, member);
            member->SetFreezeVideo(FALSE);
            setup = TRUE;
            break;
          }
        }
      }
      if(setup == FALSE)
      {
        if(type==1) type++;
        mixer->PositionSetup(pos,type,NULL);
      }
    }
//    else if((id>=0)&&(id<100))
//      mixer->PositionSetup(pos,type,NULL);
    else
      mixer->SetPositionType(pos,type);
    mixer->Unlock();
    conference->FreezeVideo(0);
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }

  MCUMemberList & memberList = conference->GetMemberList();
  MCUMemberList::shared_iterator mit = memberList.Find(v);
  if(mit == memberList.end())
    return FALSE;
  ConferenceMember * member = *mit;
  PStringStream cmd;

  if(action == OTFC_CHANGE_RESIZER_RULE)
  {
    member->resizerRule=(!member->resizerRule)&1;
    cmd << "rszspr(" << member->GetID() << ", " << member->resizerRule << ")";
    HttpWriteCmdRoom(cmd,room);
    conference->UpdateVideoMixOptions(member);
    return TRUE;
  }

  if(action == OTFC_DIAL)
  {
    member->Dial(!member->autoDial);
    cmd << "dspr(" << (long)member->GetID() << ",'" << member->IsOnline() << member->autoDial << "')";
    HttpWriteCmdRoom(cmd,room);
    return TRUE;
  }
  if(action == OTFC_SET_VMP_STATIC )
  {
    if(member->IsSystem())
      return FALSE;
    long n = data("o").AsInteger();
    MCUSimpleVideoMixer *mixer = manager->FindVideoMixerWithLock(conference, n);
    if(mixer == NULL)
      return FALSE;
    int pos = data("o2").AsInteger();
    mixer->PositionSetup(pos, 1001, member);
    mixer->Unlock();
    member->SetFreezeVideo(FALSE);
    HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
    HttpWriteCmdRoom("build_page()",room);
    return TRUE;
  }
  if( action == OTFC_AUDIO_GAIN_LEVEL_SET )
  {
    int n=data("o").AsInteger();
    if(n<0) n=0;
    if(n>80) n=80;
    member->kManualGainDB=n-20;
    member->kManualGain=(float)pow(10.0,((float)member->kManualGainDB)/20.0);
    cmd << "setagl(" << v << "," << member->kManualGainDB << ")";
    HttpWriteCmdRoom(cmd,room);
    SaveParameterByURL("Input Gain", MCUURL(member->GetName()).GetUrl(), member->kManualGainDB);
    return TRUE;
  }
  if( action == OTFC_OUTPUT_GAIN_SET )
  {
    int n=data("o").AsInteger();
    if(n<0) n=0;
    if(n>80) n=80;
    member->kOutputGainDB=n-20;
    member->kOutputGain=(float)pow(10.0,((float)member->kOutputGainDB)/20.0);
    cmd << "setogl(" << v << "," << member->kOutputGainDB << ")";
    HttpWriteCmdRoom(cmd,room);
    SaveParameterByURL("Output Gain", MCUURL(member->GetName()).GetUrl(), member->kOutputGainDB);
    return TRUE;
  }
  if(action == OTFC_MUTE)
  {
    if(!member->IsSystem())
      member->SetChannelPauses(data("o").AsInteger());
    return TRUE;
  }
  if(action == OTFC_UNMUTE)
  {
    if(!member->IsSystem())
      member->UnsetChannelPauses(data("o").AsInteger());
    return TRUE;
  }
  if(action == OTFC_REMOVE_FROM_VIDEOMIXERS)
  {
    if(conference->IsModerated()=="+")
    {
      MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
      if(videoMixerList.GetSize() != 0)
      {
        for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
        {
          MCUSimpleVideoMixer *mixer = it.GetObject();
          ConferenceMemberId id = member->GetID();
          int oldPos = mixer->GetPositionNum(id);
          if(oldPos != -1)
            mixer->MyRemoveVideoSource(oldPos, (mixer->GetPositionType(id) & 2) != 2);
        }
      }
      else // classic MCU mode
      {
        MCUMemberList & memberList = conference->GetMemberList();
        for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
        {
          ConferenceMember * member = *it;
          MCUVideoMixer * mixer = member->videoMixer;
          ConferenceMemberId id = member->GetID();
          int oldPos = mixer->GetPositionNum(id);
          if(oldPos != -1) mixer->MyRemoveVideoSource(oldPos, (mixer->GetPositionType(id) & 2) != 2);
        }
      }
      if(!member->IsSystem())
        member->SetFreezeVideo(TRUE);
      HttpWriteCmdRoom(endpoint->GetConferenceOptsJavascript(*conference),room);
      HttpWriteCmdRoom("build_page()",room);
      return TRUE;
    }
    return TRUE;
  }
  if(action == OTFC_DROP_MEMBER )
  {
    member->SetAutoDial(FALSE);
    if(member->IsOnline())
      member->Close();
    return TRUE;
  }
  if(action == OTFC_VAD_NORMAL)
  {
    member->disableVAD=FALSE;
    member->chosenVan=FALSE;
    conference->PutChosenVan();
    cmd << "ivad(" << v << ",0)";
    HttpWriteCmdRoom(cmd,room);
    return TRUE;
  }
  if(action == OTFC_VAD_CHOSEN_VAN)
  {
    member->disableVAD=FALSE;
    member->chosenVan=TRUE;
    conference->PutChosenVan();
    conference->FreezeVideo(member->GetID());
    cmd << "ivad(" << v << ",1)";
    HttpWriteCmdRoom(cmd,room);
    return TRUE;
  }
  if(action == OTFC_VAD_DISABLE_VAD)
  {
    member->disableVAD=TRUE;
    member->chosenVan=FALSE;
    conference->PutChosenVan();
#if 1 // DISABLING VAD WILL CAUSE MEMBER REMOVAL FROM VAD POSITIONS
    {
      ConferenceMemberId id = member->GetID();
      MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
      if(videoMixerList.GetSize() != 0)
      {
        for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
        {
          MCUSimpleVideoMixer *mixer = it.GetObject();
          int type = mixer->GetPositionType(id);
          if(type == 2 || type == 3)
            mixer->MyRemoveVideoSourceById(id, FALSE);
        }
      }
      else
      {
        MCUMemberList & memberList = conference->GetMemberList();
        for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
        {
          ConferenceMember * member = *it;
          MCUVideoMixer * mixer = member->videoMixer;
          int type = mixer->GetPositionType(id);
          if(type<2 || type>3) continue; //-1:not found, 1:static, 2&3:VAD
          mixer->MyRemoveVideoSourceById(id, FALSE);
        }
      }
      conference->FreezeVideo(id);
    }
#endif
    cmd << "ivad(" << v << ",2)";
    HttpWriteCmdRoom(cmd,room);
    return TRUE;
  }
  if(action == OTFC_SET_MEMBER_VIDEO_MIXER)
  {
    int option = data("o").AsInteger();
    int newMixerNumber = endpoint->SetMemberVideoMixer(*conference, member, option);
    if(newMixerNumber == -1)
      return FALSE;
    return TRUE;
  }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/*
#ifndef _WIN32
static pid_t popen2(const char *command, int *infp = NULL, int *outfp = NULL)
{
    int read = 0, write = 1;
    int p_stdin[2], p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
      return -1;

    signal(SIGCHLD, SIG_IGN);
    pid = fork();

    if (pid < 0)
      return pid;
    else if (pid == 0)
    {
      close(p_stdin[write]);
      dup2(p_stdin[read], read);
      close(p_stdout[read]);
      dup2(p_stdout[write], write);

      PStringArray pargv = PString(command).Tokenise(" ");
      PINDEX argc=pargv.GetSize();
      char *argv[argc+1];
      for(int i = 0; i< argc; i++) argv[i] = (char*)(const char*)pargv[i];
      argv[argc]=NULL;
      execv(OpenMCU::Current().vr_ffmpegPath, argv);
      perror("execv");
      exit(1);
    }

    if (infp == NULL)
      close(p_stdin[write]);
    else
      *infp = p_stdin[write];

    if (outfp == NULL)
      close(p_stdout[read]);
    else
      *outfp = p_stdout[read];

    return pid;
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

ExternalVideoRecorderThread::ExternalVideoRecorderThread(PString roomName)
  : PThread(1000, AutoDeleteThread)
{
  running=FALSE;
  PStringStream t; t << roomName << "__" // fileName format: room101__2013-0516-1058270__704x576x10
    << PTime().AsString("yyyy-MMdd-hhmmssu", PTime::Local) << "__"
    << OpenMCU::Current().vr_framewidth << "x"
    << OpenMCU::Current().vr_frameheight << "x"
    << OpenMCU::Current().vr_framerate;
  fileName = t;
  t = OpenMCU::Current().ffmpegCall;
  t.Replace("%o",fileName,TRUE,0);
  PString audio, video;
#ifdef _WIN32
  audio = "\\\\.\\pipe\\sound_" + roomName;
  video = "\\\\.\\pipe\\video_" + roomName;
#else
#  ifdef SYS_PIPE_DIR
  audio = PString(SYS_PIPE_DIR)+"/sound." + roomName;
  video = PString(SYS_PIPE_DIR)+"/video." + roomName;
#  else
  audio = "sound." + roomName;
  video = "video." + roomName;
#  endif
#endif
  t.Replace("%A",audio,TRUE,0);
  t.Replace("%V",video,TRUE,0);
#ifdef _WIN32
  recordState=_popen(t, "w");
  PTRACE(1,"EVRT\tStarting new external recording thread, popen result: " << recordState << ", CL: " << t);
#else
  recordPid = popen2(t);
  PTRACE(1,"EVRT\tStarting new external recording thread, pid: " << recordPid << ", CL: " << t);
#endif
#ifdef _WIN32
  if(recordState) {running=TRUE; Resume(); }
#else
  if(recordPid > 0) {running=TRUE; Resume(); }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ExternalVideoRecorderThread::Main()
{
  while(running) MCUTime::Sleep(100);
  PTRACE(1,"EVRT\tStopping external recording thread" << flush);
#ifdef _WIN32
  fputs("q\r\n",recordState);
#else
  kill(recordPid, SIGINT);
#endif
  MCUTime::Sleep(200);
#ifdef _WIN32
  _pclose(recordState);
#endif
  PThread::Terminate();
}
*/
// End of File ///////////////////////////////////////////////////////////////

