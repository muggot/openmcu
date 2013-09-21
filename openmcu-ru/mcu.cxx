
#include <ptlib.h>

#include "version.h"
#include "mcu.h"
#include "h323.h"
#include "html.h"

#if USE_LIBJPEG
extern "C" {
#include <jpeglib.h>
}
#endif

#define new PNEW

///////////////////////////////////////////////////////////////

#if USE_LIBJPEG
class JpegFrameHTTP : public PServiceHTTPString
{
  public:
    JpegFrameHTTP(OpenMCU & app, PHTTPAuthority & auth);
    BOOL OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo);
    PMutex mutex;
  private:
    OpenMCU & app;
};
#endif

class InteractiveHTTP : public PServiceHTTPString
{
  public:
    InteractiveHTTP(OpenMCU & app, PHTTPAuthority & auth);
    BOOL OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo);
  private:
    OpenMCU & app;
};

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
  public:
    SelectRoomPage(OpenMCU & app, PHTTPAuthority & auth);
    
    BOOL OnGET(
      PHTTPServer & server,
      const PURL &url,
      const PMIMEInfo & info,
      const PHTTPConnectionInfo & connectInfo
    );

    virtual BOOL Post(
      PHTTPRequest & request,
      const PStringToString &,
      PHTML & msg
    );
  
  private:
    OpenMCU & app;
};

class WelcomePage : public PServiceHTTPString
{
  public:
    WelcomePage(OpenMCU & app, PHTTPAuthority & auth);
    BOOL OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo);
  
  private:
    OpenMCU & app;
};

class RecordsBrowserPage : public PServiceHTTPString
{
  public:
    RecordsBrowserPage(OpenMCU & app, PHTTPAuthority & auth);
    BOOL OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo);
  
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
  sipendpoint = NULL;
}

void OpenMCU::Main()
{
  Suspend();
}

BOOL OpenMCU::OnStart()
{
  char ** argv=PXGetArgv();
  executableFile = argv[0];
  PDirectory exeDir = executableFile.GetDirectory();
  exeDir.Change();

  SetConfigurationPath(CONFIG_PATH);

  httpNameSpace.AddResource(new PHTTPDirectory("data", "data"));
  httpNameSpace.AddResource(new PServiceHTTPDirectory("html", "html"));

  manager  = CreateConferenceManager();
  endpoint = CreateEndPoint(*manager);
  sipendpoint = new OpenMCUSipEndPoint(endpoint);

  return PHTTPServiceProcess::OnStart();
}

void OpenMCU::OnStop()
{
  sipendpoint->terminating = 1;
  sipendpoint->WaitForTermination(10000);
  delete sipendpoint;
  sipendpoint = NULL;

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
  SetLogLevel((PSystemLog::Level)LogLevel);
#  ifdef SERVER_LOGS
#    ifdef _WIN32
  PTrace::Initialise(TraceLevel,PString(SERVER_LOGS)+"\\trace.txt");
#    else
  PTrace::Initialise(TraceLevel,PString(SERVER_LOGS)+"/trace.txt");
#    endif
#  else
  PTrace::Initialise(TraceLevel,"trace.txt");
#  endif

  PTrace::SetOptions(PTrace::FileAndLine);

#  ifdef GIT_REVISION
#    define _QUOTE_MACRO_VALUE1(x) #x
#    define _QUOTE_MACRO_VALUE(x) _QUOTE_MACRO_VALUE1(x)
  PTRACE(1,"OpenMCU\tREVISION " << _QUOTE_MACRO_VALUE(GIT_REVISION));
#    undef _QUOTE_MACRO_VALUE
#    undef _QUOTE_MACRO_VALUE1
#  endif
#endif //if PTRACING

// default log file name
#ifdef SERVER_LOGS
#  ifdef _WIN32
  { PString lfn = cfg.GetString(CallLogFilenameKey, DefaultCallLogFilename);
    if(lfn.Find("\\")==P_MAX_INDEX) logFilename = PString(SERVER_LOGS)+"\\"+lfn; else logFilename = lfn;
  }
#  else
  { PString lfn = cfg.GetString(CallLogFilenameKey, DefaultCallLogFilename);
    if(lfn.Find("/")==P_MAX_INDEX) logFilename = PString(SERVER_LOGS)+"/"+lfn; else logFilename = lfn;
  }
#  endif
#else
  logFilename = cfg.GetString(CallLogFilenameKey, DefaultCallLogFilename);
#endif

  // Buffered events
  httpBuffer=cfg.GetInteger(HttpLinkEventBufferKey, 100);
  httpBufferedEvents.SetSize(httpBuffer);
  httpBufferIndex=0; httpBufferComplete=0;

#if OPENMCU_VIDEO
  endpoint->enableVideo = cfg.GetBoolean("Enable video", TRUE);
  endpoint->videoRate = cfg.GetInteger("Video frame rate", DefaultVideoFrameRate);
  endpoint->videoTxQuality = cfg.GetInteger("Video quality", DefaultVideoQuality);
  forceScreenSplit = cfg.GetBoolean(ForceSplitVideoKey, TRUE);

  h264DefaultLevelForSip = cfg.GetString(H264LevelForSIPKey, "9").AsInteger();
  if(h264DefaultLevelForSip < 9) h264DefaultLevelForSip=9;
  else if(h264DefaultLevelForSip>13 && h264DefaultLevelForSip<20) h264DefaultLevelForSip=13;
  else if(h264DefaultLevelForSip>22 && h264DefaultLevelForSip<30) h264DefaultLevelForSip=22;
  else if(h264DefaultLevelForSip>32 && h264DefaultLevelForSip<40) h264DefaultLevelForSip=32;
  else if(h264DefaultLevelForSip>42 && h264DefaultLevelForSip<50) h264DefaultLevelForSip=42;
  else if(h264DefaultLevelForSip>51) h264DefaultLevelForSip=51;
#if USE_LIBYUV
  scaleFilter=libyuv::LIBYUV_FILTER;
#endif
#endif

  // recall last template after room created
  recallRoomTemplate = cfg.GetBoolean(RecallLastTemplateKey, FALSE);

  // get conference time limit
  roomTimeLimit = cfg.GetInteger(DefaultRoomTimeLimitKey, 0);

  // allow/disallow self-invite:
  allowLoopbackCalls = cfg.GetBoolean(AllowLoopbackCallsKey, FALSE);

  // Get the HTTP basic authentication info
  PString adminUserName = cfg.GetString(UserNameKey);
  PString adminPassword = PHTTPPasswordField::Decrypt(cfg.GetString(PasswordKey));
  PHTTPSimpleAuth authority(GetName(), adminUserName, adminPassword);

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
    PString opts = vr_ffmpegOpts;
    PStringStream frameSize; frameSize << vr_framewidth << "x" << vr_frameheight;
    PStringStream frameRate; frameRate << vr_framerate;
#ifdef _WIN32
    PStringStream outFile; outFile << vr_ffmpegDir << "\\%o";
#else
    PStringStream outFile; outFile << vr_ffmpegDir << "/%o";
#endif
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
  GeneralPConfigPage * rsrc = new GeneralPConfigPage(*this, "Parameters", "Parameters", authority);
  OnCreateConfigPage(cfg, *rsrc);
  httpNameSpace.AddResource(rsrc, PHTTPSpace::Overwrite);

  // Create the config page - h323
  httpNameSpace.AddResource(new H323PConfigPage(*this, "H323Parameters", "Parameters", authority), PHTTPSpace::Overwrite);

  // Create the config page - sip
  httpNameSpace.AddResource(new SIPPConfigPage(*this, "SIPParameters", "Parameters", authority), PHTTPSpace::Overwrite);

  // Create the config page - sip room acccess
  httpNameSpace.AddResource(new SectionPConfigPage(*this, "RoomAccess", "RoomAccess", authority), PHTTPSpace::Overwrite);

  // Create the config page - sip proxy servers
  httpNameSpace.AddResource(new SectionPConfigPage(*this, "ProxyServers", "ProxyServers", authority), PHTTPSpace::Overwrite);

  // Create the config page - receive sound codecs
  httpNameSpace.AddResource(new CodecsPConfigPage(*this, "ReceiveSoundCodecs", "RECEIVE_SOUND", authority), PHTTPSpace::Overwrite);

  // Create the config page - transmit sound codecs
  httpNameSpace.AddResource(new CodecsPConfigPage(*this, "TransmitSoundCodecs", "TRANSMIT_SOUND", authority), PHTTPSpace::Overwrite);

  // Create the config page - receive video codecs
  httpNameSpace.AddResource(new CodecsPConfigPage(*this, "ReceiveVideoCodecs", "RECEIVE_VIDEO", authority), PHTTPSpace::Overwrite);

  // Create the config page - transmit video codecs
  httpNameSpace.AddResource(new CodecsPConfigPage(*this, "TransmitVideoCodecs", "TRANSMIT_VIDEO", authority), PHTTPSpace::Overwrite);

  // Create the status page
  httpNameSpace.AddResource(new MainStatusPage(*this, authority), PHTTPSpace::Overwrite);

  // Create invite conference page
  httpNameSpace.AddResource(new InvitePage(*this, authority), PHTTPSpace::Overwrite);

  // Create room selection page
  httpNameSpace.AddResource(new SelectRoomPage(*this, authority), PHTTPSpace::Overwrite);

  // Create video recording directory browser page:
  httpNameSpace.AddResource(new RecordsBrowserPage(*this, authority), PHTTPSpace::Overwrite);

#if USE_LIBJPEG
  // Create JPEG frame via HTTP
  httpNameSpace.AddResource(new JpegFrameHTTP(*this, authority), PHTTPSpace::Overwrite);
#endif

  httpNameSpace.AddResource(new InteractiveHTTP(*this, authority), PHTTPSpace::Overwrite);

  // Add log file links
/*
  if (!systemLogFileName && (systemLogFileName != "-")) {
    httpNameSpace.AddResource(new PHTTPFile("logfile.txt", systemLogFileName, authority));
    httpNameSpace.AddResource(new PHTTPTailFile("tail_logfile", systemLogFileName, authority));
  }
*/  
  httpNameSpace.AddResource(new WelcomePage(*this, authority), PHTTPSpace::Overwrite);

  // create monitoring page
  PString monitorText =
#  ifdef GIT_REVISION
#    define _QUOTE_MACRO_VALUE1(x) #x
#    define _QUOTE_MACRO_VALUE(x) _QUOTE_MACRO_VALUE1(x)
                        (PString("OpenMCU REVISION ") + _QUOTE_MACRO_VALUE(GIT_REVISION) +"\n\n") +
#    undef _QUOTE_MACRO_VALUE
#    undef _QUOTE_MACRO_VALUE1
#  endif
                        "<!--#equival monitorinfo-->"
                        "<!--#equival mcuinfo-->";
  httpNameSpace.AddResource(new PServiceHTTPString("monitor.txt", monitorText, "text/plain", authority), PHTTPSpace::Overwrite);

  // adding web server links (eg. images):
#ifdef SYS_RESOURCE_DIR
#  ifdef _WIN32
#    define WEBSERVER_LINK(r1) httpNameSpace.AddResource(new PHTTPFile(r1, PString(SYS_RESOURCE_DIR) + "\\" + r1), PHTTPSpace::Overwrite)
#    define WEBSERVER_LINK_MIME(mt1,r1) httpNameSpace.AddResource(new PHTTPFile(r1, PString(SYS_RESOURCE_DIR) + "\\" + r1, mt1), PHTTPSpace::Overwrite)
#  else
#    define WEBSERVER_LINK(r1) httpNameSpace.AddResource(new PHTTPFile(r1, PString(SYS_RESOURCE_DIR) + "/" + r1), PHTTPSpace::Overwrite)
#    define WEBSERVER_LINK_MIME(mt1,r1) httpNameSpace.AddResource(new PHTTPFile(r1, PString(SYS_RESOURCE_DIR) + "/" + r1, mt1), PHTTPSpace::Overwrite)
#  endif
#else
#  define WEBSERVER_LINK(r1) httpNameSpace.AddResource(new PHTTPFile(r1), PHTTPSpace::Overwrite)
#  define WEBSERVER_LINK_MIME(mt1,r1) httpNameSpace.AddResource(new PHTTPFile(r1, r1, mt1), PHTTPSpace::Overwrite)
#endif
  WEBSERVER_LINK_MIME("text/javascript"          , "control.js");
  WEBSERVER_LINK_MIME("text/javascript"          , "locale_ru.js");
  WEBSERVER_LINK_MIME("text/javascript"          , "locale_en.js");
  WEBSERVER_LINK_MIME("image/gif"                , "i15_mic_on.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i15_mic_off.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "openmcu.ru_drop_Abdylas_Tynyshov.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "openmcu.ru_vad_vad.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "openmcu.ru_vad_disable.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "openmcu.ru_vad_chosenvan.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "i15_inv.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "openmcu.ru_launched_Ypf.gif");
  WEBSERVER_LINK_MIME("image/gif"                , "openmcu.ru_remove.gif");
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
  WEBSERVER_LINK_MIME("image/vnd.microsoft.icon" , "openmcu.ico");
  WEBSERVER_LINK_MIME("image/x-windows-bmp"      , "openmcu.ru_logo_text.bmp");

  // set up the HTTP port for listening & start the first HTTP thread
  if (ListenForHTTP((WORD)cfg.GetInteger(HttpPortKey, DefaultHTTPPort)))
    PSYSTEMLOG(Info, "Opened master socket for HTTP: " << httpListeningSocket->GetPort());
  else {
    PSYSTEMLOG(Fatal, "Cannot run without HTTP port: " << httpListeningSocket->GetErrorText());
    return FALSE;
  }

  if(cfg.GetBoolean(CreateEmptyRoomKey, FALSE))
  { GetEndpoint().GetConferenceManager().MakeAndLockConference(defaultRoomName);
    GetEndpoint().GetConferenceManager().UnlockConference();
  }

  PSYSTEMLOG(Info, "Service " << GetName() << ' ' << initMsg);
  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////

PCREATE_SERVICE_MACRO(mcuinfo,P_EMPTY,P_EMPTY)
{
  return OpenMCU::Current().GetEndpoint().GetMonitorText();
}

PCREATE_SERVICE_MACRO_BLOCK(RoomStatus,P_EMPTY,P_EMPTY,block)
{
  return OpenMCU::Current().GetEndpoint().GetRoomStatus(block);
}

//////////////////////////////////////////////////////////////////////////////////////////////

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
    if(!logFile.Open(logFilename, PFile::ReadWrite))
    {
      PTRACE(1,"OpenMCU\tCan not open log file: " << logFilename << "\n" << msg << flush);
      logMutex.Signal();
      return;
    }
    if(!logFile.SetPosition(0, PFile::End))
    {
      PTRACE(1,"OpenMCU\tCan not change log position, log file name: " << logFilename << "\n" << msg << flush);
      logFile.Close();
      logMutex.Signal();
      return;
    }
  }

  if(!logFile.WriteLine(msg))
  {
    PTRACE(1,"OpenMCU\tCan not write to log file: " << logFilename << "\n" << msg << flush);
  }
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

MainStatusPage::MainStatusPage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Status", "", "text/html; charset=utf-8", auth),
    app(_app)
{
  PStringStream html;
  
  html << "<meta http-equiv=\"Refresh\" content=\"30\">\n";
  BeginPage(html,"Connections","window.l_connections","window.l_info_connections");
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
       
         EndPage(html,OpenMCU::Current().GetHtmlCopyright());
  string = html;
}


BOOL MainStatusPage::Post(PHTTPRequest & request,
                          const PStringToString & data,
                          PHTML & msg)
{
  return TRUE;
}

///////////////////////////////////////////////////////////////


#if USE_LIBJPEG

MCUVideoMixer* jpegMixer;

void jpeg_init_destination(j_compress_ptr cinfo){
  if(jpegMixer->myjpeg.GetSize()<32768)jpegMixer->myjpeg.SetSize(32768);
  cinfo->dest->next_output_byte=&jpegMixer->myjpeg[0];
  cinfo->dest->free_in_buffer=jpegMixer->myjpeg.GetSize();
}

boolean jpeg_empty_output_buffer(j_compress_ptr cinfo){
  PINDEX oldsize=jpegMixer->myjpeg.GetSize();
  jpegMixer->myjpeg.SetSize(oldsize+16384);
  cinfo->dest->next_output_byte = &jpegMixer->myjpeg[oldsize];
  cinfo->dest->free_in_buffer = jpegMixer->myjpeg.GetSize() - oldsize;
  return true;
}

void jpeg_term_destination(j_compress_ptr cinfo){
  jpegMixer->jpegSize=jpegMixer->myjpeg.GetSize() - cinfo->dest->free_in_buffer;
  jpegMixer->jpegTime=(long)time(0);
}

JpegFrameHTTP::JpegFrameHTTP(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Jpeg", "", "image/jpeg", auth),
    app(_app)
{
}

BOOL JpegFrameHTTP::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
  if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
  delete req;

  PString request=url.AsString();
  PINDEX q;
  if((q=request.Find("?"))==P_MAX_INDEX) return FALSE;

  request=request.Mid(q+1,P_MAX_INDEX);
  PStringToString data;
  PURL::SplitQueryVars(request,data);

  PString room=data("room"); if (room.GetLength()==0) return FALSE;

  int width=atoi(data("w"));
  int height=atoi(data("h"));

  unsigned requestedMixer=0;
  if(data.Contains("mixer")) requestedMixer=(unsigned)data("mixer").AsInteger();

  const unsigned long t1=(unsigned long)time(0);

//  PWaitAndSignal m(mutex); // no more required: the following mutex will do the same:
  app.GetEndpoint().GetConferenceManager().GetConferenceListMutex().Wait();

  ConferenceListType & conferenceList = app.GetEndpoint().GetConferenceManager().GetConferenceList();
  for(ConferenceListType::iterator r = conferenceList.begin(); r != conferenceList.end(); ++r)
  {
    Conference & conference = *(r->second);
    if(conference.GetNumber()==room)
    {
      if(conference.videoMixerList==NULL){ app.GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal(); return FALSE; }
      jpegMixer=conference.VMLFind(requestedMixer);
      if(jpegMixer==NULL) { app.GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal(); return FALSE; }
      if(t1-(jpegMixer->jpegTime)>1)
      {
        if(width<1||height<1||width>2048||height>2048)
        { width=OpenMCU::vmcfg.vmconf[jpegMixer->GetPositionSet()].splitcfg.mockup_width;
          height=OpenMCU::vmcfg.vmconf[jpegMixer->GetPositionSet()].splitcfg.mockup_height;
        }
        struct jpeg_compress_struct cinfo; struct jpeg_error_mgr jerr;
        JSAMPROW row_pointer[1];
        int row_stride;
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);
        cinfo.image_width = width;
        cinfo.image_height = height;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;

        PINDEX amount=width*height*3/2;
        unsigned char *videoData=new unsigned char[amount];

        ((MCUSimpleVideoMixer*)jpegMixer)->ReadMixedFrame((void*)videoData,width,height,amount);
        PColourConverter * converter = PColourConverter::Create("YUV420P", "RGB24", width, height);
        converter->SetDstFrameSize(width, height);
        unsigned char * bitmap = new unsigned char[width*height*3];
        converter->Convert(videoData,bitmap);
        delete converter;
        delete videoData;

        jpeg_set_defaults(&cinfo);
        cinfo.dest = new jpeg_destination_mgr;
        cinfo.dest->init_destination = &jpeg_init_destination;
        cinfo.dest->empty_output_buffer = &jpeg_empty_output_buffer;
        cinfo.dest->term_destination = &jpeg_term_destination;
        jpeg_start_compress(&cinfo,TRUE);
        row_stride = cinfo.image_width * 3;
        while (cinfo.next_scanline < cinfo.image_height)
        { row_pointer[0] = (JSAMPLE *) & bitmap [cinfo.next_scanline * row_stride];
          (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }
        jpeg_finish_compress(&cinfo);
        delete bitmap; delete cinfo.dest; cinfo.dest=NULL;
        jpeg_destroy_compress(&cinfo);
        jpegMixer->jpegTime=t1;
      }

      PTime now;
      PStringStream message;
      message << "HTTP/1.1 200 OK\r\n"
              << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
              << "Server: OpenMCU.ru\r\n"
              << "MIME-Version: 1.0\r\n"
              << "Cache-Control: no-cache, must-revalidate\r\n"
              << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
              << "Content-Type: image/jpeg\r\n"
              << "Content-Length: " << jpegMixer->jpegSize << "\r\n"
              << "Connection: Close\r\n"
              << "\r\n";  //that's the last time we need to type \r\n instead of just \n

      server.Write((const char*)message,message.GetLength());
      server.Write(jpegMixer->myjpeg.GetPointer(),jpegMixer->jpegSize);

      app.GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal();
      server.flush();
      return TRUE;
    }
  }
  app.GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal();
  return FALSE;
}
#endif //#if USE_LIBJPEG

InteractiveHTTP::InteractiveHTTP(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Comm", "", "text/html; charset=utf-8", auth),
    app(_app)
{
}

BOOL InteractiveHTTP::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
  if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
  delete req;

  PString request=url.AsString();
  PINDEX q;
  PStringToString data;

  if((q=request.Find("?"))!=P_MAX_INDEX)
  {
    request=request.Mid(q+1,P_MAX_INDEX);
    PURL::SplitQueryVars(request,data);
  }

  PString room=data("room");

  PStringStream message;
  PTime now;
  int idx;

  message << "HTTP/1.1 200 OK\r\n"
          << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
          << "Server: OpenMCU.ru\r\n"
          << "MIME-Version: 1.0\r\n"
          << "Cache-Control: no-cache, must-revalidate\r\n"
          << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
          << "Content-Type: text/html;charset=utf-8\r\n"
          << "Connection: Close\r\n"
          << "\r\n";  //that's the last time we need to type \r\n instead of just \n
  server.Write((const char*)message,message.GetLength());
  server.flush();

  message="<html><body style='font-size:9px;font-family:Verdana,Arial;padding:0px;margin:1px;color:#000'><script>p=parent</script>\n";
  message << OpenMCU::Current().HttpStartEventReading(idx,room);

//PTRACE(1,"!!!!!\tsha1('123')=" << PMessageDigestSHA1::Encode("123")); // sha1 works!! I'll try with websocket in future

  if(room!="")
  {
    OpenMCU::Current().GetEndpoint().GetConferenceManager().GetConferenceListMutex().Wait();
    ConferenceListType & conferenceList = OpenMCU::Current().GetEndpoint().GetConferenceManager().GetConferenceList();
    ConferenceListType::iterator r;
    for (r = conferenceList.begin(); r != conferenceList.end(); ++r) if(r->second->GetNumber() == room) break;
    if(r != conferenceList.end() ) {
      Conference & conference = *(r->second);
      message << "<script>p.splitdata=Array(";
      for (unsigned i=0;i<OpenMCU::vmcfg.vmconfs;i++)
      {
        PString split=OpenMCU::vmcfg.vmconf[i].splitcfg.Id;
        split.Replace("\"","\\x22",TRUE,0);
        if(i!=0) message << ",";
        message << "\"" << split << "\"";
      }
      message << ");\n"
        << "p." << OpenMCU::Current().GetEndpoint().GetMemberListOptsJavascript(conference) << "\n"
        << "p." << OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(conference) << "\n"
        << "p.tl=Array" << conference.GetTemplateList() << "\n"
        << "p.seltpl=\"" << conference.GetSelectedTemplateName() << "\"\n"
        << "p.build_page();\n"
        << "</script>\n";
    }
    OpenMCU::Current().GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal();
  }

  while(server.Write((const char*)message,message.GetLength())) {
    server.flush();
    int count=0;
    message = OpenMCU::Current().HttpGetEvents(idx,room);
    while (message.GetLength()==0 && count < 20){
      count++;
      PThread::Sleep(100);
      message = OpenMCU::Current().HttpGetEvents(idx,room);
    }
    if(message.Find("<script>")==P_MAX_INDEX) message << "<script>p.alive()</script>\n";
  }
  return FALSE;
}

InvitePage::InvitePage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Invite", "", "text/html; charset=utf-8", auth),
    app(_app)
{
  PStringStream html;

  BeginPage(html,"Invite","window.l_invite","window.l_info_invite");

  html << "<p>"
    << "<form method=\"POST\" class=\"well form-inline\">"
    << "<input type=\"text\" class=\"input-small\" placeholder=\"" << app.GetDefaultRoomName() << "\" name=\"room\" value=\"" << app.GetDefaultRoomName() << "\"> "
    << "<input type=\"text\" class=\"input-large\" placeholder=\"Address\" name=\"address\"><script language='javascript'><!--\ndocument.forms[0].address.focus(); //--></script>"
    << "&nbsp;&nbsp;<input type=\"submit\" class=\"btn\" value=\"Invite\">"
    << "</form>";

  EndPage(html,OpenMCU::Current().GetHtmlCopyright());

  string = html;
}


BOOL InvitePage::Post(PHTTPRequest & request,
                          const PStringToString & data,
                          PHTML & msg)
{
  PString room    = data("room");
  PString address = data("address");
  PStringStream html;

  PStringStream html_invite;
  html_invite << "<p>"
    << "<form method=\"POST\" class=\"well form-inline\">"
    << "<input type=\"text\" class=\"input-small\" placeholder=\"" << room << "\" name=\"room\" value=\"" << room << "\"> "
    << "<input type=\"text\" class=\"input-large\" placeholder=\"Address\" name=\"address\"><script language='javascript'><!--\ndocument.forms[0].address.focus(); //--></script>"
    << "&nbsp;&nbsp;<input type=\"submit\" class=\"btn\" value=\"Invite\">"
    << "</form>";

  if (room.IsEmpty() || address.IsEmpty()) {
    BeginPage(html,"Invite failed","window.l_invite_f","window.l_info_invite_f");
    html << html_invite;
    EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;
    return TRUE;
  }

  OpenMCUH323EndPoint & ep = app.GetEndpoint();
  BOOL created = ep.OutgoingConferenceRequest(room);

  if (!created) {
    BeginPage(html,"Invite failed","window.l_invite_f","window.l_info_invite_f");
    html << html_invite;
    EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;
    return TRUE;
  }

  if(address.Find("sip:") == 0) {
    OpenMCU::Current().sipendpoint->SipMakeCall(room, address);
  } else {
    PString h323Token;
    PString * userData = new PString(room);
    if (ep.MakeCall(address, h323Token, userData) == NULL) {
      BeginPage(html,"Invite failed","window.l_invite_f","window.l_info_invite_f");
      EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;
      ep.GetConferenceManager().RemoveConference(room);
      return TRUE;
    }
  }

  BeginPage(html,"Invite succeeded","window.l_invite_s","window.l_info_invite_s");
  html << html_invite;
  EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;

  return TRUE;
}

PCREATE_SERVICE_MACRO_BLOCK(RoomList,P_EMPTY,P_EMPTY,block)
{
  return OpenMCU::Current().GetEndpoint().GetRoomList(block);
}

SelectRoomPage::SelectRoomPage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Select", "", "text/html; charset=utf-8", auth),
    app(_app)
{
}

BOOL SelectRoomPage::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  { PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
    if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
    delete req;
  }

  PStringToString data;
  { PString request=url.AsString(); PINDEX q;
    if((q=request.Find("?"))!=P_MAX_INDEX) { request=request.Mid(q+1,P_MAX_INDEX); PURL::SplitQueryVars(request,data); }
  }

  PStringStream cmdResult; // will empty if no action performed
  if(data.Contains("action"))
  {
    PString action=data("action");
    PString room=data("room");
    if(action == "create" && (!room.IsEmpty()))
    {
      ConferenceManager & cm = app.GetEndpoint().GetConferenceManager();
      cmdResult << "Creating &laquo;" << room << "&raquo;: ";
      if(cm.MakeAndLockConference(room) != NULL) cmdResult << "created"; else cmdResult << "failed";
      cm.UnlockConference();
    }
    else if(action == "delete" && (!room.IsEmpty()))
    {
      ConferenceManager & cm = app.GetEndpoint().GetConferenceManager();
      if(cm.HasConference(room))
      {
        Conference * conference = cm.MakeAndLockConference(room); // find & get locked
        if(conference != NULL)
        { cmdResult << "Closing &laquo;" << room << "&raquo;: ";

//          conference->GetMutex().Wait();
//          conference->GetMemberNameList().clear();
/*          Conference::MemberList & memberList = conference->GetMemberList();
          Conference::MemberList::iterator r;
          for(r=memberList.begin();r!=memberList.end();++r) if(r->second->GetName() == "file recorder")
          { delete dynamic_cast<ConferenceFileMember *>(r->second); break; }
*/
/*

          while (r != memberList.end())
          {
            ConferenceMember * member = r->second;
            if(member != NULL)
            { PString memberName=member->GetName();
              BOOL cache = (memberName=="cache"),
                recorder = (memberName=="file recorder");
              BOOL peer = !(cache || recorder);
              cmdResult << "-" << memberName << "; "; PTRACE(5,"MCU\tClosing " << memberName << flush);
  //            if(peer) (dynamic_cast<H323Connection_ConferenceMember *>(r->second))->Close();
              if(recorder)
              { ConferenceFileMember * fileMember = dynamic_cast<ConferenceFileMember *>(r->second);
//                memberList.erase(r);
PTRACE(1,"1" << flush);
                delete fileMember;
PTRACE(1,"2" << flush);
                if(memberList.size() == 0) break;
PTRACE(1,"3" << flush);
                r = memberList.begin();
PTRACE(1,"4" << flush);
                continue;
              }
            }
            r++;
          }
//          conference->GetMemberList().clear();

          conference->GetMutex().Signal();
*/
          cm.RemoveConference(conference->GetID());
          cmdResult << "done";
          cm.UnlockConference();
        }
      }
    }
  }

  OpenMCUH323EndPoint & ep=app.GetEndpoint();

  PStringStream html;
  BeginPage(html,"Rooms","window.l_rooms","window.l_info_rooms");
  if(!cmdResult.IsEmpty()) html << cmdResult;

  html
    << "<form method=\"post\"><input name='room' id='room' type=hidden>"
    << "<table class=\"table table-striped table-bordered table-condensed\">"

    << "<tr>"
    << "<td colspan='6'><input class='btn btn-large' name='newroom' id='newroom' value='room102' /><input type='button' class='btn btn-large btn-info' id='l_select_create' onclick=\"location.href='?action=create&room='+encodeURIComponent(document.getElementById('newroom').value);\"></td>"
    << "</tr>"

    << "<tr>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_enter);</script><br></th>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_moderated);</script><br></th>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_visible);</script><br></th>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_unvisible);</script><br></th>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_duration);</script><br></th>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_delete);</script><br></th>"
    << "</tr>"
  ;
  
  { PWaitAndSignal m(ep.GetConferenceManager().GetConferenceListMutex());
    ConferenceListType & conferenceList = ep.GetConferenceManager().GetConferenceList();

    ConferenceListType::iterator r;
    for (r = conferenceList.begin(); r != conferenceList.end(); ++r)
    {
      BOOL controlled = TRUE;
      Conference & conference = *(r->second);
      PString roomNumber = conference.GetNumber();
#if ENABLE_TEST_ROOMS
      controlled &= (!(roomNumber.Left(8)=="testroom")) ;
#endif
#if ENABLE_ECHO_MIXER
      controlled &= (!(roomNumber.Left(4)*="echo"));
#endif
      BOOL moderated=FALSE; PString charModerated = "-";
      if(controlled) { charModerated = conference.IsModerated(); moderated=(charModerated=="+"); }
      PINDEX   visibleMemberCount = conference.GetVisibleMemberCount();
      PINDEX unvisibleMemberCount = conference.GetMemberCount() - visibleMemberCount;

      PString roomButton = "<span class=\"btn btn-large btn-";
      if(moderated) roomButton+="success";
      else if(controlled) roomButton+="primary";
      else roomButton+="inverse";
      roomButton += "\"";
      if(controlled) roomButton+=" onclick='document.getElementById(\"room\").value=\""
        + roomNumber + "\";document.forms[0].submit();'";
      roomButton += ">" + roomNumber + "</span>";

      html << "<tr>"
        << "<td>" << roomButton                            << "</td>"
        << "<td>" << moderated                             << "</td>"
        << "<td>" << visibleMemberCount                    << "</td>"
        << "<td>" << unvisibleMemberCount                  << "</td>"
        << "<td>" << (PTime() - conference.GetStartTime()) << "</td>"
        << "<td><span class=\"btn btn-large btn-danger\" onclick=\"if(confirm('Вы уверены? Are you sure?')){location.href='?action=delete&room=" << PURL::TranslateString(roomNumber,PURL::QueryTranslation) << "';}\">X</span></td>"
        << "</tr>";
    }
  }

  html << "</table></form>";

  EndPage(html,OpenMCU::Current().GetHtmlCopyright());
  { PStringStream message; PTime now; message
      << "HTTP/1.1 200 OK\r\n"
      << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
      << "Server: OpenMCU.ru\r\n"
      << "MIME-Version: 1.0\r\n"
      << "Cache-Control: no-cache, must-revalidate\r\n"
      << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
      << "Content-Type: text/html;charset=utf-8\r\n"
      << "Content-Length: " << html.GetLength() << "\r\n"
      << "Connection: Close\r\n"
      << "\r\n";  //that's the last time we need to type \r\n instead of just \n
    server.Write((const char*)message,message.GetLength());
  }
  server.Write((const char*)html,html.GetLength());
  server.flush();
  return TRUE;
}

BOOL SelectRoomPage::Post(PHTTPRequest & request,
                          const PStringToString & data,
                          PHTML & msg)
{
  if(OpenMCU::Current().GetForceScreenSplit())
  {
    msg << OpenMCU::Current().GetEndpoint().SetRoomParams(data);
  }
  else msg << ErrorPage(request.localAddr.AsString(),request.localPort,423,"Locked","Room Control feature is locked","To unlock the page: click &laquo;<a href='/Parameters'>Parameters</a>&raquo;, check &laquo;Force split screen video and enable Room Control feature&raquo; and accept.<br/><br/>");
  return TRUE;
}

WelcomePage::WelcomePage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("welcome.html", "", "text/html; charset=utf-8", auth),
    app(_app)
{}

BOOL WelcomePage::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  PString peerAddr  = "N/A",
          localAddr = "127.0.0.1";
  WORD    localPort = 80;
  { PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
    if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
    if(req->origin != 0) peerAddr=req->origin.AsString();
    if(req->localAddr != 0) localAddr=req->localAddr.AsString();
    if(req->localPort != 0) localPort=req->localPort;
    delete req;
  }
  PStringToString data;
  { PString request=url.AsString(); PINDEX q;
    if((q=request.Find("?"))!=P_MAX_INDEX) { request=request.Mid(q+1,P_MAX_INDEX); PURL::SplitQueryVars(request,data); }
  }
  PStringStream shtml;
  BeginPage(shtml,"OpenMCU-ru","window.l_welcome","window.l_info_welcome");

  PString timeFormat = "yyyyMMdd hhmmss z";
  PTime now;
  PTimeInterval upTime = now - OpenMCU::Current().GetStartTime();

  shtml << "<br><b>Monitor Text (<span style='cursor:pointer;text-decoration:underline' onclick='javascript:{if(document.selection){var range=document.body.createTextRange();range.moveToElementText(document.getElementById(\"monitorTextId\"));range.select();}else if(window.getSelection){var range=document.createRange();range.selectNode(document.getElementById(\"monitorTextId\"));window.getSelection().addRange(range);}}'>select all</span>)</b><div style='padding:5px;border:1px dotted #595;width:100%;height:auto;max-height:300px;overflow:auto'><pre style='margin:0px;padding:0px' id='monitorTextId'>"
#  ifdef GIT_REVISION
#    define _QUOTE_MACRO_VALUE1(x) #x
#    define _QUOTE_MACRO_VALUE(x) _QUOTE_MACRO_VALUE1(x)
        << "OpenMCU REVISION " << _QUOTE_MACRO_VALUE(GIT_REVISION) << "\n"
#    undef _QUOTE_MACRO_VALUE
#    undef _QUOTE_MACRO_VALUE1
#  endif

              << "Program: "          << OpenMCU::Current().GetProductName() << "\n"
              << "Version: "          << OpenMCU::Current().GetVersion(TRUE) << "\n"
              << "Manufacturer: "     << OpenMCU::Current().GetManufacturer() << "\n"
              << "OS: "               << OpenMCU::Current().GetOSClass() << " " << OpenMCU::Current().GetOSName() << "\n"
              << "OS Version: "       << OpenMCU::Current().GetOSVersion() << "\n"
              << "Hardware: "         << OpenMCU::Current().GetOSHardware() << "\n"
              << "Compilation date: " << OpenMCU::Current().GetCompilationDate().AsString(timeFormat, PTime::GMT) << "\n"
              << "Start Date: "       << OpenMCU::Current().GetStartTime().AsString(timeFormat, PTime::GMT) << "\n"
              << "Current Date: "     << now.AsString(timeFormat, PTime::GMT) << "\n"
              << "Up time: "          << upTime << "\n"
              << "Peer Addr: "        << peerAddr << "\n"
              << "Local Host: "       << PIPSocket::GetHostName() << "\n"
              << "Local Addr: "       << localAddr << "\n"
              << "Local Port: "       << localPort << "\n"


        << app.GetEndpoint().GetMonitorText() << "</pre></div>";
  EndPage(shtml,OpenMCU::Current().GetHtmlCopyright());
  { PStringStream message; PTime now; message
      << "HTTP/1.1 200 OK\r\n"
      << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
      << "Server: OpenMCU.ru\r\n"
      << "MIME-Version: 1.0\r\n"
      << "Cache-Control: no-cache, must-revalidate\r\n"
      << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
      << "Content-Type: text/html;charset=utf-8\r\n"
      << "Content-Length: " << shtml.GetLength() << "\r\n"
      << "Connection: Close\r\n"
      << "\r\n";  //that's the last time we need to type \r\n instead of just \n
    server.Write((const char*)message,message.GetLength());
  }
  server.Write((const char*)shtml,shtml.GetLength());
  server.flush();
  return TRUE;
}


RecordsBrowserPage::RecordsBrowserPage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Records", "", "text/html; charset=utf-8", auth),
    app(_app)
{}

BOOL RecordsBrowserPage::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  { PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
    if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
    delete req;
  }
  PStringToString data;
  { PString request=url.AsString(); PINDEX q;
    if((q=request.Find("?"))!=P_MAX_INDEX) { request=request.Mid(q+1,P_MAX_INDEX); PURL::SplitQueryVars(request,data); }
  }
  if(data.Contains("getfile")) // just download
  {
#ifdef _WIN32
    PString filePathStr = OpenMCU::Current().vr_ffmpegDir + "\\" + data("getfile");
#else
    PString filePathStr = OpenMCU::Current().vr_ffmpegDir + "/" + data("getfile");
#endif
    if(!PFile::Exists(filePathStr))
    { PHTTPRequest * request = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server);
      request->entityBody = connectInfo.GetEntityBody();
      PStringStream msg; msg << ErrorPage(request->localAddr.AsString(), request->localPort, 404, "Not Found", "The file cannot be found",
        "The requested URL <a href=\"" + data("getfile") + "\">" + filePathStr + "</a> was not found on this server.<br/><br/>");
      delete request;
      PStringStream message; PTime now; message
        << "HTTP/1.1 404 Not Found\r\n"
        << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
        << "Server: OpenMCU.ru\r\n"
        << "MIME-Version: 1.0\r\n"
        << "Cache-Control: no-cache, must-revalidate\r\n"
        << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
        << "Content-Type: text/html;charset=utf-8\r\n"
        << "Content-Length: " << msg.GetLength() << "\r\n"
        << "Connection: Close\r\n"
        << "\r\n";  //that's the last time we need to type \r\n instead of just \n
      message << msg; server.Write((const char*)message,message.GetLength()); server.flush();
      return FALSE;
    }
    FILE *f=fopen(filePathStr,"rb"); if(f==NULL)
    { PHTTPRequest * request = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server);
      request->entityBody = connectInfo.GetEntityBody();
      PStringStream msg; msg << ErrorPage(request->localAddr.AsString(), request->localPort, 403, "Forbidden", "The file cannot be downloaded",
        "You don't have permission to access <a href=\"" + data("getfile") + "\">" + filePathStr + "</a> on this server.<br/><br/>");
      delete request;
      PStringStream message; PTime now; message
        << "HTTP/1.1 403 Forbidden\r\n"
        << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
        << "Server: OpenMCU.ru\r\n"
        << "MIME-Version: 1.0\r\n"
        << "Cache-Control: no-cache, must-revalidate\r\n"
        << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
        << "Content-Type: text/html;charset=utf-8\r\n"
        << "Content-Length: " << msg.GetLength() << "\r\n"
        << "Connection: Close\r\n"
        << "\r\n";  //that's the last time we need to type \r\n instead of just \n
      message << msg; server.Write((const char*)message,message.GetLength()); server.flush();
      return FALSE;
    }
    fseek(f, 0, SEEK_END); size_t fileSize = ftell(f); rewind(f);

    PStringStream message; PTime now; message
      << "HTTP/1.1 200 OK\r\n"
      << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
      << "Server: OpenMCU.ru\r\n"
      << "MIME-Version: 1.0\r\n"
      << "Cache-Control: no-cache, must-revalidate\r\n"
      << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
      << "Content-Type: application/octet-stream\r\n"
      << "Content-Disposition: attachment; filename=\"" << PURL::TranslateString(data("getfile"),PURL::QueryTranslation) << "\"\r\n"
      << "Content-Transfer-Encoding: binary\r\n"
      << "Content-Length: " << fileSize << "\r\n"
      << "Connection: Close\r\n"
      << "\r\n";
    PTimeInterval oldTimeout=server.GetWriteTimeout();
    server.SetWriteTimeout(150000);
    if(!server.Write((const char*)message, message.GetLength())) {fclose(f); return FALSE;}
    server.flush();

    char *buffer = (char*)malloc(PMIN(65536, fileSize));
    size_t p=0, result;
    while (p<fileSize)
    { size_t blockSize = PMIN(65536, fileSize-p);
      result=fread(buffer, 1, blockSize, f);
      if(blockSize != result)
      { PTRACE(1,"mcu.cxx\tFile read error: " << p << "/" << fileSize << ", filename: " << filePathStr);
        fclose(f); free(buffer);
        return FALSE;
      }
      if(!server.Write((const char*)buffer, result))
      { PTRACE(1,"mcu.cxx\tServer write error: " << p << "/" << fileSize << ", filename: " << filePathStr);
        fclose(f); free(buffer);
        return FALSE;
      }
      server.flush();
      p+=result;
    }
    fclose(f); free(buffer);
    server.SetWriteTimeout(oldTimeout);
    return TRUE;
  }

  PString dir=OpenMCU::Current().vr_ffmpegDir;
  BOOL isDir=PFile::Exists(dir); if(isDir)
  { PFileInfo info;
    PFile::GetInfo(PFilePath(dir), info);
    isDir=((info.type & 6) != 0);
  }
#ifdef _WIN32
  PString dir0(dir);
  if(dir0.Right(1)!="\\") dir0+='\\'; else dir=dir.Right(dir.GetLength()-1);
  dir0+='*';
  WIN32_FIND_DATA d;
  HANDLE f = FindFirstFileA(dir0, &d); if (f==INVALID_HANDLE_VALUE)
#else
  if(dir.Right(1)=="/") dir=dir.Right(dir.GetLength()-1);
  DIR *d; if(isDir) isDir=((d=opendir(dir)) != NULL); if(!isDir)
#endif
  { PHTTPRequest * request = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server);
    request->entityBody = connectInfo.GetEntityBody();
    PStringStream msg; msg << ErrorPage(request->localAddr.AsString(), request->localPort, 404, "Not Found", "The file cannot be found",
      "The requested URL <a href=\"Records\">/Records</a> was not found on this server.<br/><br/>");
    delete request;
    PStringStream message; PTime now; message
      << "HTTP/1.1 404 Not Found\r\n"
      << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
      << "Server: OpenMCU.ru\r\n"
      << "MIME-Version: 1.0\r\n"
      << "Cache-Control: no-cache, must-revalidate\r\n"
      << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
      << "Content-Type: text/html;charset=utf-8\r\n"
      << "Content-Length: " << msg.GetLength() << "\r\n"
      << "Connection: Close\r\n"
      << "\r\n";  //that's the last time we need to type \r\n instead of just \n
    message << msg; server.Write((const char*)message,message.GetLength()); server.flush();
    return FALSE;
  }

// directory d is now opened
  PStringArray fileList;

#ifdef _WIN32
  do if(strcmp(".", d.cFileName) && strcmp("..", d.cFileName))
  {
    FILE *f=fopen(dir + '\\' + d.cFileName,"rb");
    if(f!=NULL)
    {
      fseek(f, 0, SEEK_END);
      PStringStream r; r << d.cFileName << "," << ftell(f);
      fileList.AppendString(r);
      fclose(f);
    }
  }
  while(FindNextFileA(f, &d) != 0 || GetLastError() != ERROR_NO_MORE_FILES);
  FindClose(f);
#else
  struct dirent *e;
  while((e = readdir(d))) if(strcmp(".", e->d_name) && strcmp("..", e->d_name))
  {
    FILE *f=fopen(dir + '/' + e->d_name,"rb");
    if(f!=NULL)
    {
      fseek(f, 0, SEEK_END);
      PStringStream r; r << e->d_name << "," << ftell(f);
      fileList.AppendString(r);
      fclose(f);
    }
  }
  closedir(d);
#endif

  PINDEX sortMode=0;
  if(data.Contains("sort")) {sortMode = data("sort").AsInteger(); if(sortMode<0 || sortMode>7) sortMode=0;}
  PStringStream shtml;
  BeginPage(shtml,"Records","window.l_records","window.l_info_records");


  shtml << "<h1>" << dir << "</h1>";
  if(fileList.GetSize()==0) shtml << "The direcory does not contain records at the moment."; else
  {
    shtml << "<table style='border:2px solid #82b8e3'><tr><th style='border:2px solid #82b8e3;padding:2px;background-color:#144a59;color:#afc'>N</th>"
      << "<th style='border:2px solid #82b8e3;padding:2px;background-color:#144a59'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=0)?'0':'1') << "'>Date/Time</a></th>"
      << "<th style='border:2px solid #82b8e3;padding:2px;background-color:#144a59'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=2)?'2':'3') << "'>Room</a></th>"
      << "<th style='border:2px solid #82b8e3;padding:2px;background-color:#144a59'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=4)?'4':'5') << "'>Resolution</th>"
      << "<th style='border:2px solid #82b8e3;padding:2px;background-color:#144a59'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=6)?'6':'7') << "'>File Size</th></tr>";

    for(PINDEX i=0;i<fileList.GetSize()-1; i++)
    { PString s=fileList[i]; PINDEX pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
      { PString roomName1 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
        pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
        { PString dateTime1 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
          PString videoResolution1; pos1=s.Find('.'); PINDEX pos2=s.Find(',');
          if(pos1!=P_MAX_INDEX) videoResolution1=s.Left(pos1); else videoResolution1=s.Left(pos2);
          PINDEX fileSize1 = s.Mid(pos2+1,P_MAX_INDEX).AsInteger();
          for(PINDEX j=i+1;j<fileList.GetSize(); j++)
          { PString s=fileList[j]; pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
            { PString roomName2 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
              pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
              { PString dateTime2 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
                PString videoResolution2; pos1=s.Find('.'); PINDEX pos2=s.Find(',');
                if(pos1!=P_MAX_INDEX) videoResolution2=s.Left(pos1); else videoResolution2=s.Left(pos2);
                PINDEX fileSize2 = s.Mid(pos2+1,P_MAX_INDEX).AsInteger();

                if((sortMode==0) && (dateTime2 < dateTime1)) {}
                else if((sortMode==1) && (dateTime2 >= dateTime1)) {}
                else if((sortMode==2) && (roomName2 >= roomName1)) {}
                else if((sortMode==3) && (roomName2 <= roomName1)) {}
                else if((sortMode==4) && (videoResolution2 <= videoResolution1)) {}
                else if((sortMode==5) && (videoResolution2 >= videoResolution1)) {}
                else if((sortMode==6) && (fileSize2 <= fileSize1)) {}
                else if((sortMode==7) && (fileSize2 >= fileSize1)) {}
                else { PString s=fileList[i]; fileList[i]=fileList[j]; fileList[j]=s; } // or just swap PString pointers?

              }
            }
          }
        }
      }
    }

    for(PINDEX i=0;i<fileList.GetSize(); i++)
    { PString s0=fileList[i]; PINDEX pos1=s0.Find("__"); if(pos1!=P_MAX_INDEX)
      { PString roomName = s0.Left(pos1); PString s=s0.Mid(pos1+2,P_MAX_INDEX);
        pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
        { PString dateTime = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
          PString videoResolution; pos1=s.Find('.'); PINDEX pos2=s.Find(',');
          if(pos1!=P_MAX_INDEX) videoResolution=s.Left(pos1); else videoResolution=s.Left(pos2);
          PINDEX fileSize = s.Mid(pos2+1,P_MAX_INDEX).AsInteger();
          shtml << "<tr>"
            << "<td style='border:2px solid #82b8e3;padding:2px;background-color:#add0ed'><a href='/Records?getfile=" << s0.Left(s0.Find(',')) << "' download>" << (i+1) << "</a></td>"
            << "<td style='border:2px solid #82b8e3;padding:2px;background-color:#add0ed'>" << dateTime.Mid(7,2) << '.' << dateTime.Mid(5,2) << '.' << dateTime.Mid(0,4) << ' ' << dateTime.Mid(10,2) << ':' << dateTime.Mid(12,2) << "</td>"
            << "<td style='border:2px solid #82b8e3;padding:2px;background-color:#add0ed'>" << roomName << "</td>"
            << "<td style='border:2px solid #82b8e3;padding:2px;background-color:#add0ed'>" << videoResolution << "</td>"
            << "<td style='border:2px solid #82b8e3;padding:2px;background-color:#add0ed;text-align:right'><a href='/Records?getfile=" << s0.Left(s0.Find(',')) << "' download>" << fileSize << "</a></td>"
            << "</tr>";
        }
      }
    }
    shtml << "</table>";

  } // if(fileList.GetSize()==0) ... else {

  EndPage(shtml,OpenMCU::Current().GetHtmlCopyright());
  { PStringStream message; PTime now; message
      << "HTTP/1.1 200 OK\r\n"
      << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
      << "Server: OpenMCU.ru\r\n"
      << "MIME-Version: 1.0\r\n"
      << "Cache-Control: no-cache, must-revalidate\r\n"
      << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
      << "Content-Type: text/html;charset=utf-8\r\n"
      << "Content-Length: " << shtml.GetLength() << "\r\n"
      << "Connection: Close\r\n"
      << "\r\n";  //that's the last time we need to type \r\n instead of just \n
    server.Write((const char*)message,message.GetLength());
  }
  server.Write((const char*)shtml,shtml.GetLength());
  server.flush();
  return TRUE;
}


// End of File ///////////////////////////////////////////////////////////////
