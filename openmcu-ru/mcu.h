
#ifndef _MCU_MCU_H
#define _MCU_MCU_H

#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include <ptlib.h>
#include <ptclib/httpsvc.h>
#include <ptclib/guid.h>

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
#include <h323rtp.h>
#include <h323pdu.h>
#include <h245.h>

#include "conference.h"
#include "filemembers.h"
#include "h323.h"
#include "sip.h"
#include "rtsp.h"
#include "reg.h"
#include "util.h"

#if P_SSL
#include <ptclib/shttpsvc.h>
typedef PSecureHTTPServiceProcess OpenMCUProcessAncestor;
#else
#include <ptclib/httpsvc.h>
typedef PHTTPServiceProcess OpenMCUProcessAncestor;
#endif

extern PHTTPServiceProcess::Info ProductInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////

static const PString MCU_INTERNAL_CALL_PREFIX("INTERNAL_CALL_");

////////////////////////////////////////////////////////////////////////////////////////////////////

const WORD DefaultHTTPPort = 1420;

static const char LogLevelKey[]           = "Log Level";
static const char TraceLevelKey[]         = "Trace level";
static const char TraceRotateKey[]        = "Rotate trace files at startup";
static const char UserNameKey[]           = "Username";
static const char PasswordKey[]           = "Password";
static const char HttpPortKey[]           = "HTTP Port";
static const char HttpLinkEventBufferKey[]= "Room control event buffer size";

static const char CallLogFilenameKey[]    = "Call log filename";

#if P_SSL
static const char HTTPSecureKey[]           = "Enable HTTP secure";
static const char HTTPCertificateFileKey[]  = "HTTP Certificate";
static PString DefaultHTTPCertificateFile   = PString(SYS_SSL_DIR)+PString(PATH_SEPARATOR)+"http.pem";
#endif
static const char DefaultRoomKey[]          = "Default room";

static const char DefaultCallLogFilename[] = "mcu_log.txt"; 
static const char DefaultRoom[]            = "room101";
static const char AllowLoopbackCallsKey[]  = "Allow loopback calls";

static const char SipListenerKey[]         = "SIP Listener";

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

// room parameters
static const char ForceSplitVideoKey[]          = "Force split screen video";
static const char RoomAutoCreateKey[]           = "Auto create";
static const char RoomAutoDeleteEmptyKey[]      = "Auto delete empty";
static const char RoomAutoRecordNotEmptyKey[]   = "Auto record not empty";
static const char RoomAllowRecordKey[]          = "Allow record";
static const char RoomRecallLastTemplateKey[]   = "Recall last template";
static const char RoomTimeLimitKey[]            = "Room time limit";
static const char LockTemplateKey[]             = "Template locks conference by default";

static const char ReceivedVFUDelayKey[]         = "Received VFU delay";
static const char SendVFUDelayKey[]             = "Send VFU delay";

static const char H264LevelForSIPKey[]        = "H.264 Default Level for SIP";

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
static const char DefaultProtocolKey[]    = "Default protocol for outgoing calls";

static const char RejectDuplicateNameKey[] = "Reject duplicate name";

static const PTEACypher::Key CypherKey = {
  {
    103,  60, 222,  17, 128, 157,  31, 137,
    133,  64,  82, 148,  94, 136,   4, 209
  }
};

static PString MCUControlCodes = "NO ACTION,"
                                 "new action";

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

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUH323EndPoint;
class OpenMCUMonitor;

////////////////////////////////////////////////////////////////////////////////////////////////////

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
    virtual MCUH323EndPoint * CreateEndPoint(ConferenceManager & manager);

    virtual void OnCreateConfigPage(PConfig & /*cfg*/, PConfigPage & /*page*/)
    { }

    PString GetDefaultRoomName() const { return defaultRoomName; }
    BOOL AreLoopbackCallsAllowed() const { return allowLoopbackCalls; }
    PString GetNewRoomNumber();
    void LogMessage(const PString & str);
    void LogMessageHTML(PString str);

    MCUH323EndPoint & GetEndpoint()
    { return *endpoint; }

    MCUSipEndPoint * GetSipEndpoint()
    { return sipendpoint; }

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

#if MCU_VIDEO
    static VideoMixConfigurator vmcfg;

    virtual MCUVideoMixer * CreateVideoMixer(BOOL forceScreenSplit)
    { return new MCUSimpleVideoMixer(forceScreenSplit); }

    virtual void RemovePreMediaFrame()
    { }

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
    unsigned   vr_sampleRate, vr_audioChans, vr_minimumSpaceMiB;

    int        h264DefaultLevelForSip;

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

    Registrar *GetRegistrar() { return registrar; };

    void ManagerRefreshAddressBook();

  protected:
    int        currentLogLevel, currentTraceLevel;
    PFilePath executableFile;
    ConferenceManager * manager;
    MCUH323EndPoint * endpoint;
    long GetCodec(const PString & codecname);

    MCUSipEndPoint * sipendpoint;
    Registrar *registrar;

    PString    defaultRoomName;
    BOOL       allowLoopbackCalls;
    BOOL       traceFileRotated;

    int        httpBuffer, httpBufferIndex;
    PStringArray httpBufferedEvents;
    PMutex     httpBufferMutex;
    BOOL       httpBufferComplete;

#if MCU_VIDEO
#if USE_LIBYUV
    libyuv::FilterMode scaleFilter;
#endif
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ExternalVideoRecorderThread : public PThread
{
  PCLASSINFO(ExternalVideoRecorderThread, PThread);
  public:
    ExternalVideoRecorderThread(PString roomName);

    void Main();
    BOOL running;
    PString fileName; // to replace %o
#ifdef _WIN32
    FILE *recordState;
#else
    pid_t recordPid;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_MCU_H
