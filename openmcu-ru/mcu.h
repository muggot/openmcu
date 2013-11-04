
#ifndef _OpenMCU_MCU_H
#define _OpenMCU_MCU_H

#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include <ptlib.h>
#include <ptclib/httpsvc.h>

#ifdef _WIN32
#include <h323pluginmgr.h>
#endif

#ifdef __FreeBSD__
#  include <signal.h>
#endif

#include "config.h"

#include <map>
#include <h323.h>
#include <h323ep.h>
#include <h323pdu.h>
#include <h245.h>

#include "conference.h"
#include "filemembers.h"
#include "h323.h"
#include "sip.h"

#if P_SSL
#include <ptclib/shttpsvc.h>
typedef PSecureHTTPServiceProcess OpenMCUProcessAncestor;
#else
#include <ptclib/httpsvc.h>
typedef PHTTPServiceProcess OpenMCUProcessAncestor;
#endif

extern PHTTPServiceProcess::Info ProductInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////

const WORD DefaultHTTPPort = 1420;

static const char LogLevelKey[]           = "Log Level";
static const char TraceLevelKey[]         = "Trace level";
static const char UserNameKey[]           = "Username";
static const char PasswordKey[]           = "Password";
static const char HttpPortKey[]           = "HTTP Port";
static const char HttpLinkEventBufferKey[]= "Room control event buffer size";

static const char CallLogFilenameKey[]    = "Call log filename";

#if P_SSL
static const char HTTPCertificateFileKey[]  = "HTTP Certificate";
#endif
static const char DefaultRoomKey[]          = "Default room";
static const char DefaultRoomTimeLimitKey[] = "Room time limit";

static const char DefaultCallLogFilename[] = "mcu_log.txt"; 
static const char DefaultRoom[]            = "room101";
static const char CreateEmptyRoomKey[]     = "Auto create empty room";
static const char RecallLastTemplateKey[]  = "Auto recall last template";
static const char AllowLoopbackCallsKey[]  = "Allow loopback calls";

static const char SipListenerKey[]         = "SIP Listener";

#if OPENMCU_VIDEO
const unsigned int DefaultVideoFrameRate = 10;
const unsigned int DefaultVideoQuality   = 10;

static const char RecorderFfmpegPathKey[]  = "Path to ffmpeg";
static const char RecorderFfmpegOptsKey[]  = "Ffmpeg options";
static const char RecorderFfmpegDirKey[]   = "Video Recorder directory";
static const char RecorderFrameWidthKey[]  = "Video Recorder frame width";
static const char RecorderFrameHeightKey[] = "Video Recorder frame height";
static const char RecorderFrameRateKey[]   = "Video Recorder frame rate";
static const char RecorderSampleRateKey[]  = "Video Recorder sound rate";
static const char RecorderAudioChansKey[]  = "Video Recorder sound channels";
#ifdef _WIN32
static const char DefaultFfmpegPath[]         = "ffmpeg.exe";
#else
static const char DefaultFfmpegPath[]         = FFMPEG_PATH;
#endif
static const char DefaultFfmpegOptions[]      = "-y -f s16le -ac %C -ar %S -i %A -f rawvideo -r %R -s %F -i %V -f asf -acodec pcm_s16le -ac %C -vcodec msmpeg4v2 %O.asf";
#ifdef RECORDS_DIR
static const char DefaultRecordingDirectory[] = RECORDS_DIR;
#else
static const char DefaultRecordingDirectory[] = "records";
#endif
static const int  DefaultRecorderFrameWidth   = 704;
static const int  DefaultRecorderFrameHeight  = 576;
static const int  DefaultRecorderFrameRate    = 10;
static const int  DefaultRecorderSampleRate   = 16000;
static const int  DefaultRecorderAudioChans   = 1;

static const char ForceSplitVideoKey[]   = "Force split screen video (enables Room Control page)";

static const char H264LevelForSIPKey[]        = "H.264 Default Level for SIP";
#endif

static PString DefaultConnectingWAVFile = PString(SYS_RESOURCE_DIR)+"/connecting.wav";
static PString DefaultEnteringWAVFile   = PString(SYS_RESOURCE_DIR)+"/entering.wav";
static PString DefaultLeavingWAVFile    = PString(SYS_RESOURCE_DIR)+"/leaving.wav";

static const char ConnectingWAVFileKey[]  = "Connecting WAV File";
static const char EnteringWAVFileKey[]    = "Entering WAV File";
static const char LeavingWAVFileKey[]     = "Leaving WAV File";

static const char InterfaceKey[]          = "H.323 Listener";
static const char LocalUserNameKey[]      = "Local User Name";
static const char GatekeeperUserNameKey[] = "Gatekeeper Username";
static const char GatekeeperAliasKey[]    = "Gatekeeper Room Names";
static const char GatekeeperPasswordKey[] = "Gatekeeper Password";
static const char GatekeeperPrefixesKey[] = "Gatekeeper Prefixes";
static const char GatekeeperModeKey[]     = "Gatekeeper Mode";
static const char GatekeeperKey[]         = "Gatekeeper";
static const char DisableCodecsKey[]      = "Disable codecs - deprecated, use capability.conf instead!";
static const char NATRouterIPKey[]        = "NAT Router IP";
static const char NATTreatAsGlobalKey[]   = "Treat as global for NAT";
static const char DisableFastStartKey[]   = "Disable Fast-Start";
static const char DisableH245TunnelingKey[]="Disable H.245 Tunneling";
static const char RTPPortBaseKey[]        = "RTP Base Port";
static const char RTPPortMaxKey[]         = "RTP Max Port";

static const char RejectDuplicateNameKey[] = "Reject duplicate name";
static const char SIPReInviteKey[]         = "SIP ReInvite (pause)";

// endpoints options
static PStringArray h323EndpointOptionsOrder = PString(
                                        "Display name override,"
                                        "Preferred frame rate from MCU,"
                                        "Preferred bandwidth from MCU").Tokenise(",");
static PStringArray sipEndpointOptionsOrder = PString(
                                        "Display name override,"
                                        "Preferred frame rate from MCU,"
                                        "Preferred bandwidth from MCU,"
                                        "Preferred bandwidth to MCU").Tokenise(",");

static const PTEACypher::Key CypherKey = {
  {
    103,  60, 222,  17, 128, 157,  31, 137,
    133,  64,  82, 148,  94, 136,   4, 209
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUConfig: public PConfig
{
 public:
   MCUConfig(const PString & section)
    : PConfig(CONFIG_PATH, section){};
};

////////////////////////////////////////////////////////////////////////////////////////////////////

// All this silly stuff to get the plugins to load 
// because windows is stoopid and the pluginloader never gets instanced.
// This is required for ALL MFC based applications looking to load plugins!

#ifdef _WIN32
class PluginLoaderStartup2 : public PProcessStartup
{
  PCLASSINFO(PluginLoaderStartup2, PProcessStartup);
  public:
    void OnStartup()
    { 
      // load the actual DLLs, which will also load the system plugins
      PStringArray dirs = PPluginManager::GetPluginDirs();
      PPluginManager & mgr = PPluginManager::GetPluginManager();
      PINDEX i;
      for (i = 0; i < dirs.GetSize(); i++) 
        mgr.LoadPluginDirectory(dirs[i]);

      // now load the plugin module managers
      PFactory<PPluginModuleManager>::KeyList_T keyList = PFactory<PPluginModuleManager>::GetKeyList();
      PFactory<PPluginModuleManager>::KeyList_T::const_iterator r;
      for (r = keyList.begin(); r != keyList.end(); ++r) {
        PPluginModuleManager * mgr = PFactory<PPluginModuleManager>::CreateInstance(*r);
        if (mgr == NULL) {
          PTRACE(1, "PLUGIN\tCannot create manager for plugins of type " << *r);
        } else {
          PTRACE(3, "PLUGIN\tCreated manager for plugins of type " << *r);
          managers.push_back(mgr);
        }
      }
    }

    void OnShutdown()
    {
      while (managers.begin() != managers.end()) {
        std::vector<PPluginModuleManager *>::iterator r = managers.begin();
        PPluginModuleManager * mgr = *r;
        managers.erase(r);
        mgr->OnShutdown();
      }
    }

  protected:
    std::vector<PPluginModuleManager *> managers;
};
#endif //_WIN32

class OpenMCUH323EndPoint;
class OpenMCUMonitor;

class OpenMCU : public OpenMCUProcessAncestor
{
  PCLASSINFO(OpenMCU, OpenMCUProcessAncestor)

  public:
    OpenMCU();
    void Main();
    BOOL OnStart();
    void OnStop();
    void OnControl();
    void OnConfigChanged();
    BOOL Initialise(const char * initMsg);

    static OpenMCU & Current() { return (OpenMCU &)PProcess::Current(); }

    virtual ConferenceManager * CreateConferenceManager();
    virtual OpenMCUH323EndPoint * CreateEndPoint(ConferenceManager & manager);

    virtual void OnCreateConfigPage(PConfig & /*cfg*/, PConfigPage & /*page*/)
    { }

    PString GetDefaultRoomName() const { return defaultRoomName; }
    BOOL IsLoopbackCallsAllowed() const { return allowLoopbackCalls; }
    PString GetNewRoomNumber();
    void LogMessage(const PString & str);
    void LogMessageHTML(PString str);

    OpenMCUH323EndPoint & GetEndpoint()
    { return *endpoint; }

    int GetRoomTimeLimit() const
    { return roomTimeLimit; }

    int GetHttpBuffer() const { return httpBuffer; }

    virtual void HttpWrite_(PString evt) {
      httpBufferMutex.Wait();
      httpBufferedEvents[httpBufferIndex]=evt; httpBufferIndex++;
      if(httpBufferIndex>=httpBuffer){ httpBufferIndex=0; httpBufferComplete=1; }
      httpBufferMutex.Signal();
    }
    virtual void HttpWriteEvent(PString evt) {
      PString evt0; PTime now;
      evt0 += now.AsString("h:mm:ss. ", PTime::Local) + evt;
      HttpWrite_(evt0+"<br>\n");
      if(copyWebLogToLog) LogMessageHTML(evt0);
    }
    virtual void HttpWriteEventRoom(PString evt, PString room){
      PString evt0; PTime now;
      evt0 += room + "\t" + now.AsString("h:mm:ss. ", PTime::Local) + evt;
      HttpWrite_(evt0+"<br>\n");
      if(copyWebLogToLog) LogMessageHTML(evt0);
    }
    virtual void HttpWriteCmdRoom(PString evt, PString room){
      PStringStream evt0;
      evt0 << room << "\t<script>p." << evt << "</script>\n";
      HttpWrite_(evt0);
    }
    virtual void HttpWriteCmd(PString evt){
      PStringStream evt0;
      evt0 << "<script>p." << evt << "</script>\n";
      HttpWrite_(evt0);
    }
    virtual PString HttpGetEvents(int &idx, PString room){
      PStringStream result;
      while (idx!=httpBufferIndex){
        PINDEX pos=httpBufferedEvents[idx].Find("\t",0);
        if(pos==P_MAX_INDEX)result << httpBufferedEvents[idx];
        else if(room=="")result << httpBufferedEvents[idx].Mid(pos+1,P_MAX_INDEX);
        else if(httpBufferedEvents[idx].Left(pos)==room) result << httpBufferedEvents[idx].Mid(pos+1,P_MAX_INDEX);
        idx++;
        if(idx>=httpBuffer)idx=0;
      }
      return result;
    }
    virtual PString HttpStartEventReading(int &idx, PString room){
      PStringStream result;
      if(httpBufferComplete){idx=httpBufferIndex+1; if(idx>httpBuffer)idx=0;} else idx=0;
      while (idx!=httpBufferIndex){
        if(httpBufferedEvents[idx].Find("<script>",0)==P_MAX_INDEX){
          PINDEX pos=httpBufferedEvents[idx].Find("\t",0);
          if(pos==P_MAX_INDEX)result << httpBufferedEvents[idx];
          else if(room=="")result << httpBufferedEvents[idx].Mid(pos+1,P_MAX_INDEX);
          else if(httpBufferedEvents[idx].Left(pos)==room) result << httpBufferedEvents[idx].Mid(pos+1,P_MAX_INDEX);
        }
        idx++;
        if(idx>=httpBuffer)idx=0;
      }
      return result;
    }
    PString GetHtmlCopyright()
    {
      PHTML html(PHTML::InBody);
      html << "Copyright &copy;"
       << compilationDate.AsString("yyyy") << " by "
       << PHTML::HotLink(copyrightHomePage + "\" target=\"_blank\"")
       << copyrightHolder;
      return html;
    }

#if OPENMCU_VIDEO
    static VideoMixConfigurator vmcfg;
    BOOL GetForceScreenSplit() const
    { return forceScreenSplit; }

    virtual MCUVideoMixer * CreateVideoMixer()
    { return new MCUSimpleVideoMixer(forceScreenSplit); }

    virtual BOOL GetPreMediaFrame(void * buffer, int width, int height, PINDEX & amount)
    { return FALSE; }

    virtual BOOL GetEmptyMediaFrame(void * buffer, int width, int height, PINDEX & amount)
    { return GetPreMediaFrame(buffer, width, height, amount); }

#if USE_LIBYUV
    virtual libyuv::FilterMode GetScaleFilter(){ return scaleFilter; }
    virtual void SetScaleFilter(libyuv::FilterMode newScaleFilter){ scaleFilter=newScaleFilter; }
#endif

#endif

#ifdef _WIN32
    // This is to get the plugins to load in MFC applications
    static void LoadPluginMgr() { plugmgr = new H323PluginCodecManager(); }
    static void RemovePluginMgr() { delete plugmgr; }
    static H323PluginCodecManager * plugmgr;
    static PluginLoaderStartup2 pluginLoader;
#endif

    static int defaultRoomCount;

    // video recorder
    PString    vr_ffmpegPath, vr_ffmpegOpts, vr_ffmpegDir;
    PString    ffmpegCall;
    int        vr_framewidth, vr_frameheight, vr_framerate;
    unsigned   vr_sampleRate, vr_audioChans;

    PString    sipListener;

    BOOL       recallRoomTemplate;

    int        h264DefaultLevelForSip;

    OpenMCUSipEndPoint * sipendpoint;

    PFilePath GetLeavingWAVFile() const
    { return leavingWAVFile; }

    PFilePath GetEnteringWAVFile() const
    { return enteringWAVFile; }

    BOOL GetConnectingWAVFile(PFilePath & fn) const
    { fn = connectingWAVFile; return TRUE; }

    PFilePath connectingWAVFile;
    PFilePath enteringWAVFile;
    PFilePath leavingWAVFile;

    PFilePath  logFilename;
    BOOL       copyWebLogToLog;

  protected:
    int        currentLogLevel, currentTraceLevel;
    PFilePath executableFile;
    ConferenceManager * manager;
    OpenMCUH323EndPoint * endpoint;
    long GetCodec(const PString & codecname);

    PString    defaultRoomName;
    BOOL       allowLoopbackCalls;

    int        httpBuffer, httpBufferIndex;
    PStringArray httpBufferedEvents;
    PMutex     httpBufferMutex;
    BOOL       httpBufferComplete;

    int        roomTimeLimit;

#if OPENMCU_VIDEO
    BOOL forceScreenSplit;
#if USE_LIBYUV
    libyuv::FilterMode scaleFilter;
#endif
#endif
};

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

class ExternalVideoRecorderThread : public PThread
{ PCLASSINFO(ExternalVideoRecorderThread, PThread);
  public:
    PString roomName; // the only thing we need to identify the room
    PString fileName; // to replace %o
#ifdef _WIN32
    FILE *recordState;
#else
    pid_t recordPid;
#endif
    BOOL running;
    ExternalVideoRecorderThread(PString _roomName)
    : PThread(1000, AutoDeleteThread),
      roomName(_roomName)
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
    void Main()
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
};

#endif // _OpenMCU_MCU_H

// End of File ///////////////////////////////////////////////////////////////
