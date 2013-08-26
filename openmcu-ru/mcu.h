
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

    OpenMCUH323EndPoint & GetEndpoint()
    { return *endpoint; }

    int GetRoomTimeLimit() const
    { return roomTimeLimit; }

    int GetHttpBuffer() const { return httpBuffer; }

    virtual void HttpWrite_(PString evt) {
      PWaitAndSignal m(httpBufferMutex);
      httpBufferedEvents[httpBufferIndex]=evt; httpBufferIndex++;
      if(httpBufferIndex>=httpBuffer){ httpBufferIndex=0; httpBufferComplete=1; }
    }
    virtual void HttpWriteEvent(PString evt) {
      PStringStream evt0; PTime now;
      evt0 << now.AsString("h:mm:ss. ", PTime::Local) << evt << "<br>\n";
      HttpWrite_(evt0);
    }
    virtual void HttpWriteEventRoom(PString evt, PString room){
      PStringStream evt0; PTime now;
      evt0 << room << "\t" << now.AsString("h:mm:ss. ", PTime::Local) << evt << "<br>\n";
      HttpWrite_(evt0);
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
       << PHTML::HotLink(copyrightHomePage)
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

  protected:
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

    PFilePath  logFilename;
    int        roomTimeLimit;

#if OPENMCU_VIDEO
    BOOL forceScreenSplit;
#if USE_LIBYUV
    libyuv::FilterMode scaleFilter;
#endif
#endif
};


void BeginPage (PStringStream &html, const char *ptitle, const char *title, const char *quotekey);
void EndPage (PStringStream &html, PString copyr);

PString ErrorPage( //maybe ptlib could provide pages like this? for future: dig http server part
  PString        ip,            // "192.168.1.1"
  unsigned short port,          // 1420
  unsigned       errorCode,     // 403
  PString        errorText,     // "Forbidden"
  PString        title,         // "Page you tried to access is forbidden, lol"
  PString        description    // detailed: "blablablablablabla \n blablablablablabla"
);

#ifndef _WIN32
static pid_t popen2(const char *command, int *infp = NULL, int *outfp = NULL)
{
    int read = 0, write = 1;
    int p_stdin[2], p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
      return -1;

    pid = fork();

    if (pid < 0)
      return pid;
    else if (pid == 0)
    {
      close(p_stdin[write]);
      dup2(p_stdin[read], read);
      close(p_stdout[read]);
      dup2(p_stdout[write], write);

      std::string cmd = command;
      execl("/bin/sh", "/bin/sh", "-c", cmd.c_str(), "", NULL);
      perror("execl");
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
