
#include <ptlib.h>

#include "version.h"
#include "mcu.h"
#include "h323.h"
#include "html.h"

#define new PNEW

///////////////////////////////////////////////////////////////
// This really isn't the default count only a counter
// for sending aliases and prefixes to the gatekeeper
int OpenMCU::defaultRoomCount = 5;

VideoMixConfigurator OpenMCU::vmcfg;

OpenMCU::OpenMCU()
  : OpenMCUProcessAncestor(ProductInfo)
{
#ifndef _WIN32
  signal(SIGPIPE, SIG_IGN); // PTCPSocket caused SIGPIPE on browser disconnect time to time
#endif
  endpoint          = NULL;
  sipendpoint       = NULL;
  registrar         = NULL;
  currentLogLevel   = -1;
  currentTraceLevel = -1;
  traceFileRotated  = FALSE;
}

void OpenMCU::Main()
{
  Suspend();
}

BOOL OpenMCU::OnStart()
{
#if PTLIB_VER == PTLIB_VERSION_INT(2,0,1)
  char ** argv=PXGetArgv();
  executableFile = argv[0];
  PDirectory exeDir = executableFile.GetDirectory();
  exeDir.Change();
#endif

  SetConfigurationPath(CONFIG_PATH);

  httpNameSpace.AddResource(new PHTTPDirectory("data", "data"));
  httpNameSpace.AddResource(new PServiceHTTPDirectory("html", "html"));

  manager  = CreateConferenceManager();
  endpoint = CreateEndPoint(*manager);
  sipendpoint = new MCUSipEndPoint(endpoint);
  registrar = new Registrar(endpoint, sipendpoint);

  return PHTTPServiceProcess::OnStart();
}

void OpenMCU::OnStop()
{

#if PTLIB_VER < PTLIB_VERSION_INT(2,10,0)
  PHTTPServiceProcess::OnStop();
#endif

  // clear conference monitor events, autodelete and etc.
  manager->ClearMonitorEvents();
  // clear all conference and leave connections
  manager->ClearConferenceList();

  delete endpoint;
  endpoint = NULL;

  delete manager;
  manager = NULL;

  sipendpoint->terminating = 1;
  sipendpoint->WaitForTermination(10000);
  delete sipendpoint;
  sipendpoint = NULL;

  registrar->terminating = 1;
  registrar->WaitForTermination(10000);
  delete registrar;
  registrar = NULL;

#ifndef _WIN32
  CommonDestruct(); // save config
#endif
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

  vmcfg.go(vmcfg.bfw,vmcfg.bfh);

#if PTRACING
  int TraceLevel=cfg.GetInteger(TraceLevelKey, DEFAULT_TRACE_LEVEL);
  int LogLevel=cfg.GetInteger(LogLevelKey, DEFAULT_LOG_LEVEL);
  if(currentLogLevel != LogLevel)
  {
    SetLogLevel((PSystemLog::Level)LogLevel);
    currentLogLevel = LogLevel;
  }
  PINDEX rotationLevel = cfg.GetInteger(TraceRotateKey, 0);
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

    PTrace::Initialise(TraceLevel, PString(SERVER_LOGS) + PATH_SEPARATOR + "trace.txt");
    PTrace::SetOptions(PTrace::FileAndLine);
    currentTraceLevel = TraceLevel;
  }

#  ifdef GIT_REVISION
#    define _QUOTE_MACRO_VALUE1(x) #x
#    define _QUOTE_MACRO_VALUE(x) _QUOTE_MACRO_VALUE1(x)
  PTRACE(1,"OpenMCU-ru git revision " << _QUOTE_MACRO_VALUE(GIT_REVISION));
#    undef _QUOTE_MACRO_VALUE
#    undef _QUOTE_MACRO_VALUE1
#  endif
#endif //if PTRACING

#ifdef __VERSION__
  PTRACE(1,"OpenMCU-ru GCC version " <<__VERSION__);
#endif

// default log file name
#ifdef SERVER_LOGS
  {
    PString lfn = cfg.GetString(CallLogFilenameKey, DefaultCallLogFilename);
    if(lfn.Find(PATH_SEPARATOR) == P_MAX_INDEX) logFilename = PString(SERVER_LOGS)+PATH_SEPARATOR+lfn;
    else logFilename = lfn;
  }
#else
  logFilename = cfg.GetString(CallLogFilenameKey, DefaultCallLogFilename);
#endif
  copyWebLogToLog = cfg.GetBoolean("Copy web log to call log", FALSE);
  // Buffered events
  httpBuffer=cfg.GetInteger(HttpLinkEventBufferKey, 100);
  httpBufferedEvents.SetSize(httpBuffer);
  httpBufferIndex=0; httpBufferComplete=0;

#if MCU_VIDEO
  endpoint->enableVideo = cfg.GetBoolean("Enable video", TRUE);
  endpoint->videoRate = MCUConfig("Video").GetInteger("Video frame rate", DefaultVideoFrameRate);
  endpoint->videoTxQuality = cfg.GetInteger("Video quality", DefaultVideoQuality);
#if USE_LIBYUV
  scaleFilter=libyuv::LIBYUV_FILTER;
#endif
#endif

  // allow/disallow self-invite:
  allowLoopbackCalls = cfg.GetBoolean(AllowLoopbackCallsKey, FALSE);

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

  // get default "room" (conference) name
  defaultRoomName = cfg.GetString(DefaultRoomKey, DefaultRoom);

  { // video recorder setup
    vr_ffmpegPath  = cfg.GetString( RecorderFfmpegPathKey,  DefaultFfmpegPath);
    vr_ffmpegOpts  = cfg.GetString( RecorderFfmpegOptsKey,  DefaultFfmpegOptions);
    vr_ffmpegDir   = cfg.GetString( RecorderFfmpegDirKey,   DefaultRecordingDirectory);
    vr_framewidth  = cfg.GetInteger(RecorderFrameWidthKey,  DefaultRecorderFrameWidth);
    vr_frameheight = cfg.GetInteger(RecorderFrameHeightKey, DefaultRecorderFrameHeight);
    vr_framerate   = cfg.GetInteger(RecorderFrameRateKey,   DefaultRecorderFrameRate);
    vr_sampleRate  = cfg.GetInteger(RecorderSampleRateKey,  DefaultRecorderSampleRate);
    vr_audioChans  = cfg.GetInteger(RecorderAudioChansKey,  DefaultRecorderAudioChans);
    vr_minimumSpaceMiB = 1024;
    PString opts = vr_ffmpegOpts;
    PStringStream frameSize; frameSize << vr_framewidth << "x" << vr_frameheight;
    PStringStream frameRate; frameRate << vr_framerate;
    PStringStream outFile; outFile << vr_ffmpegDir << PATH_SEPARATOR << "%o";
    opts.Replace("%F",frameSize,TRUE,0);
    opts.Replace("%R",frameRate,TRUE,0);
    opts.Replace("%S",PString(vr_sampleRate),TRUE,0);
    opts.Replace("%C",PString(vr_audioChans),TRUE,0);
    opts.Replace("%O",outFile,TRUE,0);
    PStringStream tmp;
    tmp << vr_ffmpegPath << " " << opts;
    ffmpegCall=tmp;
  }

  // get WAV file played to a user when they enter a conference
  connectingWAVFile = cfg.GetString(ConnectingWAVFileKey, DefaultConnectingWAVFile);

  // get WAV file played to a conference when a new user enters
  enteringWAVFile = cfg.GetString(EnteringWAVFileKey, DefaultEnteringWAVFile);

  // get WAV file played to a conference when a new user enters
  leavingWAVFile = cfg.GetString(LeavingWAVFileKey, DefaultLeavingWAVFile);

  // Create the config page - general
  GeneralPConfigPage * rsrc = new GeneralPConfigPage(*this, "Parameters", "Parameters", authSettings);
  OnCreateConfigPage(cfg, *rsrc);
  httpNameSpace.AddResource(rsrc, PHTTPSpace::Overwrite);

  // Create the config page - conference parameters
  httpNameSpace.AddResource(new ConferencePConfigPage(*this, "ConferenceParameters", "Conference Parameters", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - registrar parameters
  httpNameSpace.AddResource(new RegistrarPConfigPage(*this, "RegistrarParameters", "Registrar Parameters", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - managing users
  httpNameSpace.AddResource(new ManagingUsersPConfigPage(*this, "ManagingUsers", "Managing Users", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - managing groups
  httpNameSpace.AddResource(new ManagingGroupsPConfigPage(*this, "ManagingGroups", "Managing Groups", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - control codes
  httpNameSpace.AddResource(new ControlCodesPConfigPage(*this, "ControlCodes", "Control Codes", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - room codes
  //httpNameSpace.AddResource(new RoomCodesPConfigPage(*this, "RoomCodes", "Room Codes", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - h323
  httpNameSpace.AddResource(new H323PConfigPage(*this, "H323Parameters", "H323 Parameters", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - sip
  httpNameSpace.AddResource(new SIPPConfigPage(*this, "SIPParameters", "SIP Parameters", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - h323 endpoints
  httpNameSpace.AddResource(new H323EndpointsPConfigPage(*this, "H323EndpointsParameters", "H323 Endpoints", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - sip endpoints
  httpNameSpace.AddResource(new SipEndpointsPConfigPage(*this, "SipEndpointsParameters", "SIP Endpoints", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - video
#if MCU_VIDEO
  httpNameSpace.AddResource(new VideoPConfigPage(*this, "VideoParameters", "Video", authSettings), PHTTPSpace::Overwrite);
#endif

  // Create the config page - record
  //httpNameSpace.AddResource(new RecordPConfigPage(*this, "RecordParameters", "Parameters", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - sip room acccess
  httpNameSpace.AddResource(new RoomAccessSIPPConfigPage(*this, "RoomAccess", "RoomAccess", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - sip proxy servers
  httpNameSpace.AddResource(new ProxySIPPConfigPage(*this, "ProxyServers", "ProxyServers", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - receive sound codecs
  httpNameSpace.AddResource(new CodecsPConfigPage(*this, "ReceiveSoundCodecs", "RECEIVE_SOUND", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - transmit sound codecs
  httpNameSpace.AddResource(new CodecsPConfigPage(*this, "TransmitSoundCodecs", "TRANSMIT_SOUND", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - receive video codecs
  httpNameSpace.AddResource(new CodecsPConfigPage(*this, "ReceiveVideoCodecs", "RECEIVE_VIDEO", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - transmit video codecs
  httpNameSpace.AddResource(new CodecsPConfigPage(*this, "TransmitVideoCodecs", "TRANSMIT_VIDEO", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - sip sound codecs
  httpNameSpace.AddResource(new CodecsPConfigPage(*this, "SipSoundCodecs", "TRANSMIT_SOUND", authSettings), PHTTPSpace::Overwrite);

  // Create the config page - sip video codecs
  httpNameSpace.AddResource(new CodecsPConfigPage(*this, "SipVideoCodecs", "TRANSMIT_VIDEO", authSettings), PHTTPSpace::Overwrite);

  // Create the status page
  httpNameSpace.AddResource(new MainStatusPage(*this, authConference), PHTTPSpace::Overwrite);

  // Create invite conference page
  httpNameSpace.AddResource(new InvitePage(*this, authConference), PHTTPSpace::Overwrite);

  // Create room selection page
  httpNameSpace.AddResource(new SelectRoomPage(*this, authConference), PHTTPSpace::Overwrite);

  // Create video recording directory browser page:
  httpNameSpace.AddResource(new RecordsBrowserPage(*this, authConference), PHTTPSpace::Overwrite);

#if USE_LIBJPEG
  // Create JPEG frame via HTTP
  httpNameSpace.AddResource(new JpegFrameHTTP(*this, authConference), PHTTPSpace::Overwrite);
#endif

  httpNameSpace.AddResource(new InteractiveHTTP(*this, authConference), PHTTPSpace::Overwrite);

  // Add log file links
/*
  if (!systemLogFileName && (systemLogFileName != "-")) {
    httpNameSpace.AddResource(new PHTTPFile("logfile.txt", systemLogFileName, authority));
    httpNameSpace.AddResource(new PHTTPTailFile("tail_logfile", systemLogFileName, authority));
  }
*/  
  httpNameSpace.AddResource(new WelcomePage(*this, authConference), PHTTPSpace::Overwrite);

  // create monitoring page
  PString monitorText =
#  ifdef GIT_REVISION
#    define _QUOTE_MACRO_VALUE1(x) #x
#    define _QUOTE_MACRO_VALUE(x) _QUOTE_MACRO_VALUE1(x)
                        (PString("OpenMCU-ru REVISION ") + _QUOTE_MACRO_VALUE(GIT_REVISION) +"\n\n") +
#    undef _QUOTE_MACRO_VALUE
#    undef _QUOTE_MACRO_VALUE1
#  endif
                        "<!--#equival monitorinfo-->"
                        "<!--#equival mcuinfo-->";
  httpNameSpace.AddResource(new PServiceHTTPString("monitor.txt", monitorText, "text/plain", authConference), PHTTPSpace::Overwrite);

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
  WEBSERVER_LINK_MIME("text/javascript"          , "locale_ru.js");
  WEBSERVER_LINK_MIME("text/javascript"          , "locale_en.js");
  WEBSERVER_LINK_MIME("text/css"                 , "main.css");
  WEBSERVER_LINK_MIME("image/gif"                , "i15_getNoVideo.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "vad_vad.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "vad_disable.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "vad_chosenvan.gif");
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
  WEBSERVER_LINK_MIME("image/png"                , "s15_ch.png");
  WEBSERVER_LINK_MIME("image/vnd.microsoft.icon" , "mcu.ico");
  WEBSERVER_LINK_MIME("image/vnd.microsoft.icon" , "mcu.gif");
#if USE_LIBJPEG
  WEBSERVER_LINK_MIME("image/jpeg"               , "logo.jpeg");
#endif
  WEBSERVER_LINK_MIME("image/png"                , "i16_close_gray.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_close_red.png");
  WEBSERVER_LINK_MIME("image/png"                , "i32_lock.png");
  WEBSERVER_LINK_MIME("image/png"                , "i32_lockopen.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_status_gray.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_status_green.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_status_blue.png");
  WEBSERVER_LINK_MIME("image/png"                , "i16_status_red.png");

  for(PINDEX i=-1; i<rotationLevel; i++)
  {
    PString s="trace";
    if(i>=0) s+=PString(i);
    s+=".txt";
    WEBSERVER_LINK_LOGS("application/octet-stream", s);
  }

  // set up the HTTP port for listening & start the first HTTP thread
  if (ListenForHTTP((WORD)cfg.GetInteger(HttpPortKey, DefaultHTTPPort)))
    PSYSTEMLOG(Info, "Opened master socket for HTTP: " << httpListeningSocket->GetPort());
  else {
    PSYSTEMLOG(Fatal, "Cannot run without HTTP port: " << httpListeningSocket->GetErrorText());
    return FALSE;
  }

  PStringList sect = cfg.GetSections();
  // auto create
  PString sectionPrefix = "Conference ";
  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    if(sect[i].Left(sectionPrefix.GetLength()) == sectionPrefix)
    {
      PString roomname = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());
      if(GetConferenceParam(roomname, RoomAutoCreateKey, FALSE) && !GetConferenceParam(roomname, RoomAutoDeleteEmptyKey, FALSE))
      {
        manager->MakeAndLockConference(roomname);
        manager->UnlockConference();
      }
    }
  }

  // restart threads
  registrar->Resume();
  registrar->restart = 1;
  sipendpoint->Resume();
  sipendpoint->restart = 1;


  PSYSTEMLOG(Info, "Service " << GetName() << ' ' << initMsg);
  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::ManagerRefreshAddressBook()
{
  // refresh Address Book
  PWaitAndSignal m(manager->GetConferenceListMutex());
  ConferenceListType & conferenceList = manager->GetConferenceList();
  for(ConferenceListType::iterator r = conferenceList.begin(); r != conferenceList.end(); ++r)
    if(r->second) r->second->RefreshAddressBook();
}

//////////////////////////////////////////////////////////////////////////////////////////////

void OpenMCU::OnConfigChanged()
{
}

PString OpenMCU::GetNewRoomNumber()
{
  static PAtomicInteger number(100);
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
    if(!logFile.Open(logFilename, PFile::ReadWrite))
    {
      PTRACE(1,"OpenMCU-ru\tCan not open log file: " << logFilename << "\n" << msg << flush);
      logMutex.Signal();
      return;
    }
    if(!logFile.SetPosition(0, PFile::End))
    {
      PTRACE(1,"OpenMCU-ru\tCan not change log position, log file name: " << logFilename << "\n" << msg << flush);
      logFile.Close();
      logMutex.Signal();
      return;
    }
  }

  if(!logFile.WriteLine(msg))
  {
    PTRACE(1,"OpenMCU-ru\tCan not write to log file: " << logFilename << "\n" << msg << flush);
  }
  logFile.Close();
  logMutex.Signal();
}

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

ConferenceManager * OpenMCU::CreateConferenceManager()
{
  return new ConferenceManager();
}

MCUH323EndPoint * OpenMCU::CreateEndPoint(ConferenceManager & manager)
{
  return new MCUH323EndPoint(manager);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

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
  while(running) PThread::Sleep(100);
  PTRACE(1,"EVRT\tStopping external recording thread" << flush);
#ifdef _WIN32
  fputs("q\r\n",recordState);
#else
  kill(recordPid, SIGINT);
#endif
  PThread::Sleep(200);
#ifdef _WIN32
  _pclose(recordState);
#endif
  PThread::Terminate();
}

// End of File ///////////////////////////////////////////////////////////////

