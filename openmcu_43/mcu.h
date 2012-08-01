
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

#endif

	static int defaultRoomCount;

  protected:
    ConferenceManager * manager;
    OpenMCUH323EndPoint * endpoint;
    long GetCodec(const PString & codecname);

    PString    defaultRoomName;
    PFilePath  logFilename;
    int        roomTimeLimit;

#if OPENMCU_VIDEO
    BOOL forceScreenSplit;
#endif
};


void BeginPage (PStringStream &html, char *ptitle, char *title, char *quotekey);
void EndPage (PStringStream &html, PString copyr);


#endif // _OpenMCU_MCU_H

// End of File ///////////////////////////////////////////////////////////////
