
#ifndef _MCU_SIP_H
#define _MCU_SIP_H

#include "config.h"

#include <map>
#include <ptlib.h>
#include <time.h>
#include <sofia-sip/su.h>
#include <sofia-sip/nta.h>
#include <sofia-sip/nta_stateless.h>
#include <sofia-sip/stun_tag.h>
#include <sofia-sip/sip_status.h>
#include <sofia-sip/sip_header.h>
#include <sofia-sip/sip_util.h>
#include <sofia-sip/su_log.h>
#include <sofia-sip/auth_digest.h>
#include <sofia-sip/sofia_features.h>
#include <sofia-sip/nta_tport.h>
#include <sofia-sip/tport.h>
#include <sofia-sip/sdp.h>
#include <sofia-sip/msg_addr.h>
#include <sofia-sip/sres_sip.h>

#include "h323.h"
#include "mcu_rtp.h"
#include "util.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#define SIP_USER_AGENT       OpenMCU::Current().GetName()+"/"+OpenMCU::Current().GetVersion()+" ("+SOFIA_SIP_NAME_VERSION+")"
#define SIP_MAX_FORWARDS     "70"
#define SIP_ALLOW_METHODS_REGISTER    "SUBSCRIBE"
#define SIP_ALLOW_METHODS_OPTIONS     "INVITE,BYE,ACK,CANCEL,OPTIONS,SUBSCRIBE,MESSAGE,INFO"

PString GetFromIp(PString toAddr, PString toPort);

enum SipSecureTypes
{
  SECURE_TYPE_NONE,
  SECURE_TYPE_ZRTP,
  SECURE_TYPE_SRTP,
  SECURE_TYPE_DTLS_SRTP
};

enum Direction
{
  DIRECTION_INBOUND,
  DIRECTION_OUTBOUND
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUSipEndPoint;
class MCUSipConnection;

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUURL_SIP : public MCUURL
{
  public:
    MCUURL_SIP(const msg_t *msg, Direction dir = DIRECTION_INBOUND);
    const PString & GetRemoteApplication() const { return remote_application; }
    const PString & GetDomainName() const { return domain_name; }
  protected:
    PString domain_name;
    PString remote_application;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ProxyAccount
{
  public:
    ProxyAccount()
    {
      start_time = PTime(0);
      cseq = 100;
      status = 0;
    }
    void SetStatus(const sip_t *sip);
    PString roomname;
    PString username;
    PString domain;
    PString host;
    PString port;
    PString transport;
    PString password;
    unsigned expires;
    BOOL enable;

    PTime start_time;
    int status;
    PString status_phrase;

    PString sip_www_str;
    PString sip_proxy_str;
    PString call_id;
    unsigned cseq;
};
typedef std::map<PString, ProxyAccount *> ProxyAccountMapType;

////////////////////////////////////////////////////////////////////////////////////////////////////

class SipCapability
{
  public:
    SipCapability(int _pt = 0, int _m = 0, int _mm = 0, int _p = 0, int _bw = 0)
      : payload(_pt), media(_m), mode(_mm), port(_p), bandwidth(_bw)
    {
      clock = 0;
      cnum = 0;
      cap = NULL;
      inpChan = NULL;
      outChan = NULL;
      secure_type = SECURE_TYPE_NONE;
    }
    void Print();
    int CmpSipCaps(SipCapability &c)
    {
      if(payload != c.payload) return 1;
      if(media != c.media) return 1;
      if(mode != c.mode) return 1;
      if(port != c.port) return 1;
      if(bandwidth != c.bandwidth) return 1;
      if(clock != c.clock) return 1;
      if(cnum != c.cnum) return 1;
      if(format != c.format) return 1;
      if(parm != c.parm) return 1;
      if(secure_type != c.secure_type) return 1;
      if(srtp_remote_type != c.srtp_remote_type) return 1;
      if(srtp_remote_key != c.srtp_remote_key) return 1;
      if(srtp_remote_param != c.srtp_remote_param) return 1;
      sdp = c.sdp;
      inpChan = c.inpChan;
      outChan = c.outChan;
      return 0;
    }

    int payload; // payload type
    int media; // media type 0 - audio, 1 - video, 2 - other
    int mode; // 0-inactive, 1-recvonly, 2-sendonly, 3-sendrecv
    int port; // remote rtp port number
    int lport; // local rtp port
    int bandwidth;
    int clock; // rtp clock
    int cnum; // channels numbers
    PString format; // codec name
    PString parm; // parameters string
    PString h323; // found h323 capability name
    PString sdp; // sdp section
    H323Capability *cap;
    H323_RTPChannel *inpChan;
    H323_RTPChannel *outChan;

    PString remote_ip;
    PString sess_id;
    PString sess_ver;
    PString sess_username;

    SipSecureTypes secure_type;
    PString srtp_remote_type;
    PString srtp_remote_key;
    PString srtp_remote_param;
    PString srtp_local_type;
    PString srtp_local_key;
    PString srtp_local_param;
    PString zrtp_hash;
    PString dtls_fp;
    PString dtls_fp_type;
};

typedef std::map<int, SipCapability *> SipCapMapType;

////////////////////////////////////////////////////////////////////////////////////////////////////

class H323toSipQueue
{
  public:
    H323toSipQueue() { }
    ~H323toSipQueue() { }
    BOOL Push(PString *cmd)
    {
      PThread::Sleep(10);
      if(!cmd) return FALSE;
      PWaitAndSignal m(mutex);
      if(queue.GetSize() > 100) return FALSE;
      if(queue.GetStringsIndex(*cmd) != P_MAX_INDEX) return FALSE;
      queue.InsertAt(0, cmd);
      return TRUE;
    }
    PString *Pop()
    {
      PThread::Sleep(10);
      PWaitAndSignal m(mutex);
      PString *cmd = (PString *)queue.GetAt(0);
      if(cmd)
      {
        cmd = new PString(*cmd);
        queue.RemoveAt(0);
      }
      return cmd;
    }

  protected:
    PStringArray queue;
    PMutex mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUSipConnection : public MCUH323Connection
{
  public:
    MCUSipConnection(MCUSipEndPoint *_sep, MCUH323EndPoint *_ep, PString _callToken)
      :MCUH323Connection(*_ep, 0, NULL), sep(_sep)
      {
       callToken = _callToken;
       OnCreated();
       remoteName = "";
       remotePartyName = "";
       remoteApplication = "SIP terminal";
       requestedRoom = "room101";
       scap = -1;
       vcap = -1;
       sdp_seq = 0;
       sdp_id = (unsigned int)time(NULL);
       noInpTimeout = 0;
       inpBytes = 0;
       connectedTime = PTime();
       c_sip_msg = NULL;
       cseqNum = 100;
       direction = DIRECTION_INBOUND;
       bandwidth_to = 0;
       aDataSocket = aControlSocket = vDataSocket = vControlSocket = NULL; // temp rtp sockets
       audio_rtp_port = video_rtp_port = 0;
      }
    ~MCUSipConnection()
    {
      MCUTRACE(1, "MCUSipConnection destructor, callToken: "+callToken+" remotePartyAddress: "+remotePartyAddress);
      if(c_sip_msg) msg_destroy(c_sip_msg);
      DeleteTempSockets();
    }

    BOOL CreateTempSockets(PString local_ip);
    void DeleteTempSockets()
    {
      if(aDataSocket) { aDataSocket->Close(); delete aDataSocket; aDataSocket = NULL; }
      if(aControlSocket) { aControlSocket->Close(); delete aControlSocket; aControlSocket = NULL; }
      if(vDataSocket) { vDataSocket->Close(); delete vDataSocket; vDataSocket = NULL; }
      if(vControlSocket) { vControlSocket->Close(); delete vControlSocket; vControlSocket = NULL; }
    }

    int ProcessInviteEvent();
    int ProcessReInviteEvent();
    int ProcessSDP(PString & sdp_txt, SipCapMapType & caps, int reinvite);
    sdp_parser_t *SdpParser(PString sdp_txt);
    void StartTransmitChannels();
    void StartReceiveChannels();
    void StartChannel(int pt, int dir);
    void CreateLogicalChannels();
    int CreateVideoChannel(int pt, int dir);
    int CreateAudioChannel(int pt, int dir);
    SipRTP_UDP *CreateRTPSession(int pt, SipCapability *sc);
    void FindCapability_H263(SipCapability &c,PStringArray &keys, const char * _H323Name, const char * _SIPName);
    void SelectCapability_H261(SipCapability &c,PStringArray &tvCaps);
    void SelectCapability_H263(SipCapability &c,PStringArray &tvCaps);
    void SelectCapability_H263p(SipCapability &c,PStringArray &tvCaps);
    void SelectCapability_H264(SipCapability &c,PStringArray &tvCaps);
    void SelectCapability_VP8(SipCapability &c,PStringArray &tvCaps);
    void SelectCapability_SPEEX(SipCapability &c,PStringArray &tsCaps);
    void SelectCapability_OPUS(SipCapability &c,PStringArray &tsCaps);
    void StopChannel(int pt, int dir);
    void StopTransmitChannels();
    void StopReceiveChannels();
    void DeleteMediaChannels(int pt);
    void DeleteChannels();
    void CleanUpOnCallEnd();
    void LeaveMCU();
    void LeaveMCU(BOOL remove);
    virtual void SendLogicalChannelMiscCommand(H323Channel & channel, unsigned command);
    int SendBYE();
    int SendACK();
    int SendVFU();
    void ReceivedVFU();
    int SendRequest(sip_method_t method, const char *method_name, msg_t *sip_msg);
    void ReceivedDTMF(PString payload);
    BOOL HadAnsweredCall() { return (direction=DIRECTION_INBOUND); }

    virtual BOOL WriteSignalPDU(H323SignalPDU & pdu) { return TRUE; }
    int noInpTimeout;
    int inpBytes;

    Direction direction;
    PString local_user, local_ip;
    PString ruri_str;
    PString contact_str;
    msg_t *c_sip_msg;
    int cseqNum;

    PString sdp_invite_str;
    PString sdp_ok_str;

    // preffered endpoints parameters
    unsigned bandwidth_to;
    PString pref_audio_cap, pref_video_cap;

    PString key_audio80;
    PString key_audio32;
    PString key_video80;
    PString key_video32;
    PString rtp_proto;

    // temp rtp sockets for outgoing invite
    PUDPSocket *aDataSocket, *aControlSocket;
    PUDPSocket *vDataSocket, *vControlSocket;

    unsigned audio_rtp_port, video_rtp_port;

  protected:
    SipCapMapType SipCapMap;
    void InsertSipCap(SipCapability *sc);
    SipCapability *FindSipCap(int payload);

    MCUSipEndPoint *sep;
    PString sdp_s; // sdp for SIP_200_OK
    int scap; // selected audio capability payload type
    int vcap; // selected video capability payload type
    unsigned int sdp_seq;
    unsigned int sdp_id;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUSipEndPoint : public PThread
{
  public:
    MCUSipEndPoint(MCUH323EndPoint *_ep)
      :PThread(10000,NoAutoDeleteThread,NormalPriority,"SIP Listener:%0x"), ep(_ep)
    {
      restart = 1;
      terminating = 0;
    }
    ~MCUSipEndPoint()
    {
    }
    void Main();
    void Initialise();

    int terminating;
    int restart;

    H323toSipQueue SipQueue;
    void ProcessSipQueue();
    void ProcessProxyAccount();

    PStringArray sipListenerArray;
    BOOL FindListener(PString addr);

    nta_agent_t *GetAgent() { return agent; };
    su_home_t *GetHome() { return &home; };

    nta_outgoing_t * SipMakeCall(PString from, PString to, PString & call_id);
    int CreateIncomingConnection(const msg_t *msg);
    int CreateOutgoingConnection(const msg_t *msg);
    int SipReqReply(const msg_t *msg, unsigned status, const char *auth_str = "", const char *contact_str = "", const char *content_str = "", const char *payload_str = "");

    BOOL MakeProxyAuth(ProxyAccount *proxy, const sip_t *sip);
    PString MakeAuthStr(PString username, PString password, PString uri, const char *method, const char *scheme, const char *realm, const char *nonce);

    ProxyAccountMapType ProxyAccountMap;
    void InsertProxyAccount(ProxyAccount *proxy);
    ProxyAccount *FindProxyAccount(PString account);

  protected:
    void MainLoop();
    MCUH323EndPoint *ep;
    su_home_t home;
    su_root_t *root;
    nta_agent_t *agent;

    MCUSipConnection * FindConnectionWithLock(const PString & callToken)
    { return (MCUSipConnection *)ep->FindConnectionWithLock(callToken); }

    MCUSipConnection * FindConnectionWithoutLock(const PString & callToken)
    { return (MCUSipConnection *)ep->FindConnectionWithoutLock(callToken); }

    static int /*__attribute__((cdecl))*/ ProcessSipEventWrap_cb(nta_agent_magic_t *context, nta_agent_t *agent, msg_t *msg, sip_t *sip)
    { return ((MCUSipEndPoint *)context)->ProcessSipEvent_cb(agent, msg, sip); }
    int ProcessSipEvent_cb(nta_agent_t *agent, msg_t *msg, sip_t *sip);

    static int nta_response_cb1_wrap(nta_outgoing_magic_t *context, nta_outgoing_t *orq, const sip_t *sip)
    { return ((MCUSipEndPoint *)context)->nta_response_cb1(orq, sip); }
    int nta_response_cb1(nta_outgoing_t *orq, const sip_t *sip);

    static int ProcessSipEventWrap_request1(nta_leg_magic_t *context, nta_leg_t *leg, nta_incoming_t *irq, const sip_t *sip)
    { return ((MCUSipEndPoint *)context)->ProcessSipEvent_request1(leg, irq, sip); }
    int ProcessSipEvent_request1(nta_leg_t *leg, nta_incoming_t *irq, const sip_t *sip);

    PString CreateSdpInvite(MCUSipConnection *sCon, PString local_url, PString remote_url);
    BOOL GetCapabilityParams(PString & capname, unsigned & pt, PString & name, unsigned & rate, PString & fmtp);
    sdp_rtpmap_t *CreateSdpRtpmap(su_home_t *sess_home, PString & capname, unsigned & dyn_pt);
    sdp_media_t *CreateSdpMedia(su_home_t *sess_home, PStringArray & caps, sdp_media_e m_type, sdp_proto_e m_proto, unsigned m_port, unsigned & dyn_pt);
    sdp_attribute_t *CreateSdpAttr(su_home_t *sess_home, PString m_name, PString m_value);
    char * SdpText(PString text);

    int SipRegister(ProxyAccount *, BOOL enable = TRUE);
    PString GetRoomAccess(const sip_t *sip);

    void InitProxyAccounts();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_SIP_H
