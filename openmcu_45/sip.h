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
#include <sofia-sip/sip_util.h>
#include <sofia-sip/su_log.h>

class OpenMCUSipConnection;

class SipKey
{
 public:
 SipKey() { addr = port = 0; }
 int addr; // remote ip
 int port; // remote port
 PString sid; // sdp session-id
};

class CmpSipKey
{
 public:
#ifdef _WIN32
 bool operator()(SipKey k1, SipKey k2) const
#else
 bool operator()(SipKey k1, SipKey k2)
#endif
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

   int ProcessH323toSipQueue(const SipKey &key, OpenMCUSipConnection *sCon);
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
 int CmpSipCaps(SipCapability &c)
 {
  if(payload != c.payload) return 1;
  if(media != c.media) return 1;
  if(dir != c.dir) return 1;
  if(port != c.port) return 1;
  if(bandwidth != c.bandwidth) return 1;
  if(clock != c.clock) return 1;
  if(cnum != c.cnum) return 1;
  if(format != c.format) return 1;
  if(parm != c.parm) return 1;
  sdp = c.sdp;
  inpChan = c.inpChan;
  outChan = c.outChan;
  return 0;
 }

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
 PString sdp; // sdp section
 H323Capability *cap;
 H323_RTPChannel *inpChan;
 H323_RTPChannel *outChan;
};

typedef std::map<int, SipCapability *> SipCapMapType;


class H323toSipQueue
{
 public:
  H323toSipQueue() : 
   rp(0), wp(0), qsize(100) 
   { for(int i=0; i<100; i++) queue[i] = NULL; }
  ~H323toSipQueue() { for (int i=0; i<100; i++) if(queue[i]) delete queue[i]; } 
  BOOL Push(PString *cmd)
  {
   int tp = wp;
   if(queue[wp] != NULL) 
   {
    PTRACE(1, "MCUSIP\tH323toSipQueue full, " << *cmd << "message lost");
    return FALSE;
   }
   wp = (wp+1)%qsize; queue[tp] = cmd;
   return TRUE;
  }
  PString *Pop()
  {
   if(queue[rp] == NULL) return NULL;
   PString *cmd = queue[rp];
   queue[rp] = NULL; rp++;
   return cmd;
  }
 protected:
  int rp;
  int wp;
  int qsize;
  PString *queue[100];
};

class OpenMCUSipConnection : public OpenMCUH323Connection
{
 public:
  OpenMCUSipConnection(OpenMCUSipEndPoint *_sep, OpenMCUH323EndPoint *_ep):
      OpenMCUH323Connection(*_ep, 0, NULL), sep(_sep)
      {
       remoteName = "";
       remotePartyName = "";
       remoteApplication = "SIP terminal";
       requestedRoom = "room101";
       sdp_seq = 0;
       sdp_id = time(NULL);
       noInpTimeout = 0;
       inpBytes = 0;
       bandwidth = 0;
       connectedTime = PTime();
       sip_msg = NULL;
      }
  ~OpenMCUSipConnection()
  {
   PTRACE(1, "OpenMCUHSipConnection\tDestructor called");
   if(sip_msg) msg_destroy(sip_msg);
  }
      
  int ProcessInviteEvent(sip_t *sip);
  int ProcessReInviteEvent(sip_t *sip);
  int ProcessSDP(PStringArray &sdp_sa, PIntArray &par, SipCapMapType &caps, int reinvite);
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
  void SipProcessACK(nta_agent_t *agent, msg_t *msg);
  void StopChannel(int pt, int dir);
  void StopTransmitChannels();
  void StopReceiveChannels();
  void DeleteMediaChannels(int pt);
  void DeleteChannels();
  PString sdp_msg;
  void CleanUpOnCallEnd();
  void LeaveConference();
  void LeaveConference(BOOL remove);
//  virtual BOOL ClearCall(
//      CallEndReason reason = EndedByLocalUser  ///< Reason for call clearing
//    ) { return TRUE; }
  virtual BOOL WriteSignalPDU(
      H323SignalPDU & pdu       ///< PDU to write.
    ) { return TRUE; }
  int SendBYE(nta_agent_t *agent);
  int noInpTimeout;
  int inpBytes;
 H323toSipQueue cmdQueue;

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
 int bandwidth;
 msg_t *sip_msg;
 PString sess_id;
 PString sess_ver;
 PString sess_username;
};

 
