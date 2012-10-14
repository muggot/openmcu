
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

  protected:
    ConferenceManager * manager;
    OpenMCUH323EndPoint * endpoint;
    OpenMCUSipEndPoint * sipendpoint;
    long GetCodec(const PString & codecname);

    PString    defaultRoomName;
    BOOL       allowLoopbackCalls;
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


#endif // _OpenMCU_MCU_H

// End of File ///////////////////////////////////////////////////////////////
