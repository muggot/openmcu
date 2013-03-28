
#ifndef _OpenMCU_MCU_H
#define _OpenMCU_MCU_H

#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include <ptlib.h>
#include <ptclib/httpsvc.h>

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

    virtual BOOL GetConnectingWAVFile(PFilePath & fn) const
    { return FALSE; }

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

	static int defaultRoomCount;

  int vr_framewidth, vr_frameheight, vr_framerate; // video recorder values

  protected:
    ConferenceManager * manager;
    OpenMCUH323EndPoint * endpoint;
    OpenMCUSipEndPoint * sipendpoint;
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


#endif // _OpenMCU_MCU_H

// End of File ///////////////////////////////////////////////////////////////
