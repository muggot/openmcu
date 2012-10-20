#include <map>
#include <ptlib.h>
#include <time.h>
#include "config.h"
#include "h323.h"
#include <sofia-sip/su.h>
#include <sofia-sip/nta.h>
#include <sofia-sip/nta_stateless.h>
#include <sofia-sip/stun_tag.h>
#include <sofia-sip/sip_status.h>
#include <sofia-sip/sip_header.h>

class OpenMCUSipConnection;

class SipKey
{
 public:
 int addr; // remote ip
 int port; // remote port
 PString sid; // sdp session-id
};

class CmpSipKey
{
 public:
 bool operator()(SipKey k1, SipKey k2)
 {
  if(k1.addr < k2.addr) return true;
  else if(k1.addr == k2.addr)
  {
   if(k1.port < k2.port) return true;
   else if(k1.port == k2.port)
   {
    if(k1.sid < k2.sid) return true;
   }
  }
  return false;
 }
};

typedef std::map<SipKey, OpenMCUSipConnection *, CmpSipKey> SipConnectionMapType;

class OpenMCUSipEndPoint : public PThread
{
  public:
   OpenMCUSipEndPoint(OpenMCUH323EndPoint *_ep):
     PThread(10000,NoAutoDeleteThread,NormalPriority,"SIP Listener:%0x"),
     ep(_ep)
    {
     terminating = 0;
     Resume();
    }
   ~OpenMCUSipEndPoint()
   {
   }
   void Main();
   static int /*__attribute__((cdecl))*/ ProcessSipEventWrap_cb(nta_agent_magic_t *context,
                                             nta_agent_t *agent,
                                             msg_t *msg,
                                             sip_t *sip)
   {
    return ((OpenMCUSipEndPoint *)context)->ProcessSipEvent_cb(agent, msg, sip); 
   }

//    H323Connection * connectionToDelete = connectionsActive.RemoveAt(token);

   int terminating;
          
  protected:
   void MainLoop();
   int ProcessSipEvent_cb(nta_agent_t *agent,
                         msg_t *msg,
                         sip_t *sip);
   OpenMCUH323EndPoint *ep;
   su_home_t home;
   su_root_t *root;
   nta_agent_t *agent;
   SipConnectionMapType sipConnMap; // map of sip connections
};

class SipCapability
{
 public:
 SipCapability(int _pt = 0, int _m = 0, int _d = 0, int _p = 0, int _bw = 0):
  payload(_pt), media(_m), dir(_d), port(_p), bandwidth(_bw)
  { clock = 0; cnum = 0; cap = NULL; inpChan = NULL; outChan = NULL; }
  
 void Print();

 int payload; // payload type
 int media; // media type 0 - audio, 1 - video, 2 - other
 int dir; // direction 1 - recvonly, 2 - sendonly, 3 -sendrecv
 int port; // remote rtp port number
 int lport; // local rtp port
 int bandwidth;
 int clock; // rtp clock
 int cnum; // channels numbers
 PString format; // codec name
 PString parm; // parameters string
 PString h323; // found h323 capability name
 H323Capability *cap;
 H323_RTPChannel *inpChan;
 H323_RTPChannel *outChan;
};

typedef std::map<int, SipCapability *> SipCapMapType;

class OpenMCUSipConnection : public OpenMCUH323Connection
{
 public:
  OpenMCUSipConnection(OpenMCUSipEndPoint *_sep, OpenMCUH323EndPoint *_ep):
      sep(_sep), OpenMCUH323Connection(*_ep, 0, NULL)
      {
       remoteName = "hz";
       remotePartyName = "hz1";
       remoteApplication = "PCS-";
       requestedRoom = "room101";
       sdp_seq = 0;
       sdp_id = time(NULL);
       noInpTimeout = 0;
       inpBytes = 0;
      }
  int ProcessInviteEvent(sip_t *sip);
  void StartTransmitChannels();
  void StartReceiveChannels();
  void StartChannel(int pt, int dir);
  void CreateLogicalChannels();
  int CreateVideoChannel(int pt, int dir);
  int CreateAudioChannel(int pt, int dir);
  RTP_UDP *CreateRTPSession(int pt, SipCapability *sc);
  void FindCapability_H263(SipCapability &c,PStringArray &keys, const char * _H323Name, const char * _SIPName);
  void SelectCapability_H263(SipCapability &c,PStringArray &tvCaps);
  void SelectCapability_H263p(SipCapability &c,PStringArray &tvCaps);
  void SelectCapability_H264(SipCapability &c,PStringArray &tvCaps);
  void SipReply200(nta_agent_t *agent, msg_t *msg);
  void StopChannel(int pt, int dir);
  void StopTransmitChannels();
  void StopReceiveChannels();
  void DeleteMediaChannels(int pt);
  void DeleteChannels();
  PString sdp_msg;
  void CleanUpOnCallEnd();
//  virtual BOOL ClearCall(
//      CallEndReason reason = EndedByLocalUser  ///< Reason for call clearing
//    ) { return TRUE; }
  virtual BOOL WriteSignalPDU(
      H323SignalPDU & pdu       ///< PDU to write.
    ) { return TRUE; }
  int noInpTimeout;
  int inpBytes;

 protected:
 OpenMCUSipEndPoint *sep;
 PString sdp_s;
 PIntArray sipCapsId; // array of declared payload types
 SipCapMapType sipCaps; // map of sip capabilities
 int scap; // selected audio capability payload type
// PString sCapH323Name; // H323 audio capability format name
 int vcap; // selected video capability payload type
// PString vCapH323Name; // H323 video capability format name
 PString localIP; // from sip invite message
 PString remoteIP; // from sip invite message
 unsigned int sdp_seq;
 unsigned int sdp_id;
};

 
