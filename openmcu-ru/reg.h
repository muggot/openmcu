
#ifndef _MCU_REGISTRAR_H
#define _MCU_REGISTRAR_H

#include <sofia-sip/nta.h>
#include <sofia-sip/sip_header.h>

#include "h323.h"
#include "gkserver.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

enum RegAccountTypes
{
  ACCOUNT_TYPE_UNKNOWN,
  ACCOUNT_TYPE_SIP,
  ACCOUNT_TYPE_H323
};

enum RegConnectionStates
{
  CONN_IDLE,
  CONN_MCU_WAIT,
  CONN_MCU_ESTABLISHED,
  CONN_WAIT,
  CONN_ACCEPT_IN,
  CONN_ESTABLISHED,
  CONN_CANCEL_IN,
  CONN_CANCEL_OUT,
  CONN_LEAVE_IN,
  CONN_LEAVE_OUT,
  CONN_END
};

enum RegSubscriptionStates
{
  SUB_STATE_CLOSED,
  SUB_STATE_OPEN,
  SUB_STATE_BUSY
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Registrar;
class RegistrarAccount;
class MCUSipEndPoint;

////////////////////////////////////////////////////////////////////////////////////////////////////

class RegistrarConnection
{
  public:
    RegistrarConnection()
    {
      Init();
    }
    RegistrarConnection(PString callToken, PString _username_in, PString _username_out)
    {
      callToken_in = callToken;
      username_in = _username_in;
      username_out = _username_out;
      Init();
    }
    ~RegistrarConnection()
    {
      if(msg_invite) { msg_destroy(msg_invite); msg_invite = NULL; }
    }
    void Init()
    {
      account_type_in = ACCOUNT_TYPE_UNKNOWN;
      account_type_out = ACCOUNT_TYPE_UNKNOWN;
      msg_invite = NULL;
      state = CONN_IDLE;
      start_time = PTime();
      accept_timeout = 20; // 20 sec connection accept timeout
    }

    void Lock()      { mutex.Wait(); }
    void Unlock()    { mutex.Signal(); }
    int TryLock()    { if(!mutex.Wait(20)) return 0; return 1; }
    PMutex & GetMutex() { return mutex; }

    PString username_in;
    PString username_out;
    PString roomname;

    PString callToken_in;
    PString callToken_out;

    RegAccountTypes account_type_in;
    RegAccountTypes account_type_out;

    RegConnectionStates state;

    msg_t *msg_invite;

    time_t accept_timeout;
    PTime start_time;

  protected:
    PMutex mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Subscription
{
  public:
    Subscription(Registrar *_registrar)
      :registrar(_registrar)
    {
      Init();
    }
    Subscription(Registrar *_registrar, PString _username_in, PString _username_out)
      :registrar(_registrar)
    {
      username_in = _username_in;
      username_out = _username_out;
      username_pair = username_in+"@"+username_out;
      Init();
    }
    ~Subscription()
    {
      if(msg_sub) { msg_destroy(msg_sub); msg_sub = NULL; }
    }
    void Init()
    {
      state = SUB_STATE_CLOSED;
      msg_sub = NULL;
      start_time = PTime();
      expires = 0;
      cseq = 100;
    }

    void Lock()      { mutex.Wait(); }
    void Unlock()    { mutex.Signal(); }
    int TryLock()    { if(!mutex.Wait(20)) return 0; return 1; }
    PMutex & GetMutex() { return mutex; }

    PString username_in;
    PString username_out;
    PString username_pair;
    PString ruri_str;
    PString contact_str;
    int cseq;

    PTime start_time;
    time_t expires;

    RegSubscriptionStates state;

    msg_t *msg_sub;

  protected:
    PMutex mutex;
    Registrar *registrar;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RegistrarAccount
{
  public:
    RegistrarAccount() { Init(); }
    RegistrarAccount(RegAccountTypes _account_type, PString _username)
    {
      account_type = _account_type;
      username = _username;
      Init();
    }
    ~RegistrarAccount()
    {
      if(msg_reg) { msg_destroy(msg_reg); msg_reg = NULL; }
    }
    void Init()
    {
      domain = "openmcu-ru";
      port = 0;
      scheme = "Digest";
      algorithm = "MD5";
      expires = 0;
      enable = FALSE;
      registered = FALSE;
      abook_enable = FALSE;
      keep_alive_enable = FALSE;
      keep_alive_interval = 0;
      keep_alive_time_request = PTime(3600);
      keep_alive_time_response = PTime(0);
      msg_reg = NULL;
    }
    PString GetUrl()
    {
      PString url;
      if(account_type == ACCOUNT_TYPE_SIP)
      {
        url = "sip:"+username+"@"+host;
        if(port != 0 && port != 5060)
          url += ":"+PString(port);
        if(transport != "" && transport != "*")
          url += ";transport="+transport;
      }
      else if(account_type == ACCOUNT_TYPE_H323)
      {
        url = "h323:"+username+"@"+host;
        if(port != 0 && port != 1720)
          url += ":"+PString(port);
      }
      return url;
    }
    PString GetContact()
    {
      return contact_str;
    }

    void Lock()      { mutex.Wait(); }
    void Unlock()    { mutex.Signal(); }
    int TryLock()    { if(!mutex.Wait(20)) return 0; return 1; }
    PMutex & GetMutex() { return mutex; }

    PString GetAuthStr();

    PString display_name;
    PString username;
    PString host;
    PString domain;
    unsigned port;
    PString transport;
    PString password;

    PString contact_str;
    PString remote_application;

    PString sip_call_processing;
    PString h323_call_processing;

    PString scheme, nonce, algorithm;
    PString www_response, proxy_response;

    PTime start_time;
    time_t expires;

    BOOL keep_alive_enable;
    time_t keep_alive_interval;
    PTime keep_alive_time_request;
    PTime keep_alive_time_response;

    OpalGloballyUniqueID h323CallIdentifier; // h323 incoming call

    BOOL enable;
    BOOL registered;
    BOOL abook_enable;

    RegAccountTypes account_type;

    msg_t *msg_reg;

  protected:
    PMutex mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RegistrarGk : public H323GatekeeperServer
{
  PCLASSINFO(RegistrarGk, H323GatekeeperServer);

  public:
    RegistrarGk(MCUH323EndPoint *ep, Registrar *_registrar);
    ~RegistrarGk();

    void SetRequireH235(BOOL _requireH235) { requireH235 = _requireH235; }
    void SetPasswords(PStringToString _passwords) { passwords = _passwords; }
    void SetMinTimeToLive(int _minTimeToLive) { minTimeToLive = _minTimeToLive; }
    void SetMaxTimeToLive(int _maxTimeToLive) { maxTimeToLive = _maxTimeToLive; }

    BOOL IsGatekeeperRouted() const { return isGatekeeperRouted; }

  protected:

    PString GetAdmissionSrcUsername(H323GatekeeperARQ & info);
    PString GetAdmissionDstUsername(H323GatekeeperARQ & info);
    BOOL AdmissionPolicyCheck(H323GatekeeperARQ & info);

    H323GatekeeperRequest::Response OnAdmissionMCU(H323GatekeeperARQ & info);
    H323GatekeeperRequest::Response OnAdmissionDirect(H323GatekeeperARQ & info, PString dstUsername, H323TransportAddress dstHost);

//    virtual H323GatekeeperRequest::Response OnDiscovery(H323GatekeeperGRQ & request);
    virtual H323GatekeeperRequest::Response OnRegistration(H323GatekeeperRRQ & request);
    virtual H323GatekeeperRequest::Response OnUnregistration(H323GatekeeperURQ & request);
    virtual H323GatekeeperRequest::Response OnInfoResponse(H323GatekeeperIRR & request);
//    virtual void AddEndPoint(H323RegisteredEndPoint * ep); // Add a new registered endpoint to the server database
//    virtual BOOL RemoveEndPoint(H323RegisteredEndPoint * ep); // Remove a registered endpoint from the server database
//    virtual H323RegisteredEndPoint * CreateRegisteredEndPoint(H323GatekeeperRRQ & request); // Create a new registered endpoint object
//    virtual PString CreateEndPointIdentifier(); // Create a new unique identifier for the registered endpoint
    virtual H323GatekeeperRequest::Response OnAdmission(H323GatekeeperARQ & request);
    virtual H323GatekeeperRequest::Response OnDisengage(H323GatekeeperDRQ & request);
    virtual unsigned AllocateBandwidth(unsigned newBandwidth, unsigned oldBandwidth = 0);
    // Create a new call object
//    virtual H323GatekeeperCall * CreateCall(const OpalGloballyUniqueID & callIdentifier, H323GatekeeperCall::Direction direction);
    /** Called whenever a new call is started */
//    virtual void AddCall(H323GatekeeperCall *) { }

    unsigned minTimeToLive;
    unsigned maxTimeToLive;

    Registrar *registrar;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Registrar : public PThread
{
  public:
    Registrar(MCUH323EndPoint *_ep, MCUSipEndPoint *_sep):
      PThread(10000,NoAutoDeleteThread,NormalPriority,"Registrar:%0x"),
      ep(_ep), sep(_sep)
    {
      restart = 1;
      terminating = 0;
      init_config = 0;
      init_accounts = 0;
      gk = NULL;
    }
    int terminating;
    int restart;
    int init_config;
    int init_accounts;

    const PString & GetRegistrarDomain() const { return registrar_domain; };

    void RefreshAccountStatusList();
    PStringArray GetAccountList();

    void ConnectionCreated(const PString & callToken);
    void ConnectionEstablished(const PString & callToken);
    void ConnectionCleared(const PString & callToken);
    void SetRequestedRoom(const PString & callToken, PString & requestedRoom);

    BOOL MakeCall(PString room, PString to, PString & callToken);

    int OnReceivedSipRegister(const msg_t *msg);
    int OnReceivedSipInvite(const msg_t *msg);
    int OnReceivedSipSubscribe(msg_t *msg);
    int OnReceivedSipMessage(msg_t *msg);
    int OnReceivedSipOptionsResponse(const msg_t *msg);
    H323Connection::AnswerCallResponse OnReceivedH323Invite(MCUH323Connection *conn);

    nta_agent_t *GetAgent() { return sep->GetAgent(); };
    su_home_t *GetHome() { return sep->GetHome(); };
    MCUSipEndPoint *GetSep() { return sep; };

    RegistrarAccount * InsertAccountWithLock(RegAccountTypes account_type, PString username);
    RegistrarAccount * InsertAccount(RegistrarAccount *regAccount);
    RegistrarAccount * InsertAccount(RegAccountTypes account_type, PString username);
    RegistrarAccount * FindAccountWithLock(RegAccountTypes account_type, PString username);
    RegistrarAccount * FindAccount(RegAccountTypes account_type, PString username);

    void Lock()      { mutex.Wait(); }
    void Unlock()    { mutex.Signal(); }
    PMutex & GetMutex() { return mutex; }

  protected:
    void Main();
    void MainLoop();
    void Terminating();
    void InitConfig();
    void InitAccounts();
    MCUH323EndPoint *ep;
    MCUSipEndPoint *sep;
    RegistrarGk *gk;

    PString registrar_domain;

    BOOL allow_internal_calls;
    BOOL sip_require_password;
    BOOL sip_allow_unauth_mcu_calls;
    BOOL sip_allow_unauth_internal_calls;
    BOOL enable_gatekeeper;
    BOOL h323_require_h235;
    BOOL h323_allow_unreg_mcu_calls;
    BOOL h323_allow_unreg_internal_calls;
    int sip_reg_min_expires;
    int sip_reg_max_expires;
    int h323_min_time_to_live;
    int h323_max_time_to_live;

    BOOL MakeCall(RegistrarConnection *regConn, PString & username_in, PString & username_out);
    BOOL MakeCall(RegistrarConnection *regConn, RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out);

    int SipSendNotify(msg_t *msg_sub, Subscription *subAccount);
    int SipSendMessage(RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out, PString message);
    int SipSendPing(RegistrarAccount *regAccount);

    void ProcessAccountList();
    void ProcessSubscriptionList();
    void ProcessConnectionList();
    void ProcessKeepAlive();

    int SipPolicyCheck(const msg_t *msg, msg_t *msg_reply, RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out);

    // internal call process function
    void Leave(int account_type, PString callToken);
    void IncomingCallAccept(RegistrarConnection *regConn);
    void IncomingCallCancel(RegistrarConnection *regConn);
    void OutgoingCallCancel(RegistrarConnection *regConn);

    Subscription * InsertSubWithLock(Registrar *_registrar, PString username_in, PString username_out);
    Subscription * InsertSub(Registrar *_registrar, PString username_in, PString username_out);
    Subscription * InsertSub(Subscription *subAccount);
    Subscription * FindSubWithLock(PString username_pair);
    Subscription * FindSub(PString username_pair);

    RegistrarConnection * InsertRegConnWithLock(PString callToken, PString username_in, PString username_out);
    RegistrarConnection * InsertRegConn(PString callToken, PString username_in, PString username_out);
    RegistrarConnection * InsertRegConn(RegistrarConnection *regConn);
    RegistrarConnection * FindRegConnWithLock(PString callToken);
    RegistrarConnection * FindRegConn(PString callToken);
    RegistrarConnection * FindRegConnUsername(PString username);

    typedef std::map<PString /* username */, RegistrarAccount *> AccountMapType;
    AccountMapType AccountMap;

    typedef std::map<PString /* username */, Subscription *> SubscriptionMapType;
    SubscriptionMapType SubscriptionMap;

    typedef std::map<PString /* callToken */, RegistrarConnection *> RegConnMapType;
    RegConnMapType RegConnMap;
    RegConnMapType RegConnMapCopy;

    PStringArray account_status_list;

    PMutex mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_REGISTRAR_H
