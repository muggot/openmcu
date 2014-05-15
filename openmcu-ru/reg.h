
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
      //PTRACE(1, "RegistrarConnection Destructor: " << username_in << "->" << username_out);
      //cout << "RegistrarConnection Destructor: " << username_in << "->" << username_out << "\n";
      if(orq_invite_out) { nta_outgoing_destroy(orq_invite_out); orq_invite_out = NULL; }
      if(msg_invite) { msg_destroy(msg_invite); msg_invite = NULL; }
    }
    void Init()
    {
      account_type_in = ACCOUNT_TYPE_UNKNOWN;
      account_type_out = ACCOUNT_TYPE_UNKNOWN;
      msg_invite = NULL;
      orq_invite_out = NULL;
      state = CONN_IDLE;
      start_time = PTime();
      accept_timeout = 20; // 20 sec connection accept timeout
    }

    void Lock()      { mutex.Wait(); }
    void Unlock()    { mutex.Signal(); }
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
    nta_outgoing_t *orq_invite_out;

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
      if(orq_sub_out) { nta_outgoing_destroy(orq_sub_out); orq_sub_out = NULL; }
      if(leg_sub_out) { nta_leg_destroy(leg_sub_out); leg_sub_out = NULL; }
      if(msg_sub) { msg_destroy(msg_sub); msg_sub = NULL; }
    }
    void Init()
    {
      state = state_new = SUB_STATE_CLOSED;
      msg_sub = NULL;
      leg_sub_out = NULL;
      orq_sub_out = NULL;
      start_time = PTime();
      expires = 0;
    }
    void Reset()
    {
      if(orq_sub_out) { nta_outgoing_destroy(orq_sub_out); orq_sub_out = NULL; }
      if(leg_sub_out) { nta_leg_destroy(leg_sub_out); leg_sub_out = NULL; }
      state = state_new = SUB_STATE_CLOSED;
    }

    void Lock()      { mutex.Wait(); }
    void Unlock()    { mutex.Signal(); }
    PMutex & GetMutex() { return mutex; }

    PString username_in;
    PString username_out;
    PString username_pair;

    PTime start_time;
    time_t expires;

    RegSubscriptionStates state;
    RegSubscriptionStates state_new;

    msg_t *msg_sub;
    nta_leg_t *leg_sub_out;
    nta_outgoing_t *orq_sub_out;
    int LegOutCreate(RegistrarAccount *regAccount_out);

    static int wrap_sub_request_out_cb(nta_leg_magic_t *context, nta_leg_t *leg, nta_incoming_t *irq, const sip_t *sip)
    { return ((Subscription *)context)->sub_request_out_cb(leg, irq, sip); }
    int sub_request_out_cb(nta_leg_t *leg, nta_incoming_t *irq, const sip_t *sip);

    static int wrap_sub_response_out_cb(nta_outgoing_magic_t *context, nta_outgoing_t *orq, const sip_t *sip)
    { return ((Subscription *)context)->sub_response_out_cb(orq, sip); }
    int sub_response_out_cb(nta_outgoing_t *orq, const sip_t *sip);

  protected:
    PMutex mutex;
    Registrar *registrar;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RegistrarAccount
{
  public:
    RegistrarAccount() { Init(); }
    RegistrarAccount(RegAccountTypes _account_type, PString _username, PString _host)
    {
      username = _username;
      host = _host;
      domain = _host;
      account_type = _account_type;
      Init();
    }
    ~RegistrarAccount()
    {
      //PTRACE(1, "RegistrarAccount Destructor: " << username+"@"+host);
      //cout << "RegistrarAccount Destructor: " << username+"@"+host << "\n";
      if(msg_reg) { msg_destroy(msg_reg); msg_reg = NULL; }
    }
    void Init()
    {
      scheme = "Digest";
      algorithm = "MD5";
      expires = 0;
      msg_reg = NULL;
      if(account_type == ACCOUNT_TYPE_SIP)
        port = 5060;
      else if(account_type == ACCOUNT_TYPE_H323)
        port = 1720;
      enable = FALSE;
      registered = FALSE;
      abook = FALSE;
    }
    void Reset()
    {
      //if(msg_reg) { msg_destroy(msg_reg); msg_reg = NULL; }
    }
    PString GetUrl()
    {
      PString url;
      if(account_type == ACCOUNT_TYPE_SIP)
        url = "sip:"+username+"@"+host;
      else if(account_type == ACCOUNT_TYPE_H323)
        url = "h323:"+username+"@"+host;
      if(port != 0)
        url += ":"+PString(port);
      return url;
    }

    void Lock()      { mutex.Wait(); }
    void Unlock()    { mutex.Signal(); }
    PMutex & GetMutex() { return mutex; }

    PString username;
    PString domain;
    PString host;
    unsigned port;
    PString password;
    PString scheme, nonce, algorithm;
    PString www_response, proxy_response;
    PString display_name;

    PTime start_time;
    time_t expires;

    OpalGloballyUniqueID h323CallIdentifier;

    PString GetAuthStr();

    BOOL enable;
    BOOL registered;
    BOOL abook;

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
    BOOL IsGatekeeperRouted() const { return isGatekeeperRouted; }

  protected:
    PString GetAdmissionSrcUsername(H323GatekeeperARQ & info);
    PString GetAdmissionDstUsername(H323GatekeeperARQ & info);
    BOOL AdmissionPolicyCheck(H323GatekeeperARQ & info);

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
      terminating = 0;
      restart = 0;
      gk = NULL;
    }
    ~Registrar();

    int terminating;
    int restart;

    const PString & GetRegistrarDomain() const { return registrar_domain; };
    const PString & GetInternalRoomPrefix() const { return internal_room_prefix; };
    PStringArray GetAddressBook();
    void RefreshAddressBook();

    void ConnectionCreated(const PString & callToken);
    void ConnectionEstablished(const PString & callToken);
    void ConnectionCleared(const PString & callToken);
    void SetRequestedRoom(const PString & callToken, PString & requestedRoom);

    BOOL MakeCall(PString room, PString to, PString & callToken);

    int OnIncomingMsg(msg_t *msg);
    H323Connection::AnswerCallResponse OnIncomingMsg(PString memberName, PString & requestedRoom, PString callToken, OpalGloballyUniqueID callIdentifier);

    nta_agent_t *GetAgent() { return sep->GetAgent(); };
    su_home_t *GetHome() { return sep->GetHome(); };
    MCUSipEndPoint *GetSep() { return sep; };

    int SipSendNotify(msg_t *msg_sub, int state);
    int SipReqReply(const msg_t *msg, unsigned method, const char *method_name, PString sip_auth_str);
    int SipReqReply(const msg_t *msg, unsigned method, PString sip_auth_str);

    RegistrarAccount * InsertAccountWithLock(RegAccountTypes account_type, PString username, PString host);
    RegistrarAccount * InsertAccount(RegistrarAccount *regAccount);
    RegistrarAccount * InsertAccount(RegAccountTypes account_type, PString username, PString host);
    RegistrarAccount * FindAccountWithLock(RegAccountTypes account_type, PString username);
    RegistrarAccount * FindAccount(RegAccountTypes account_type, PString username);

  protected:
    void InitConfig();
    void InitTerminals();
    void Main();
    void MainLoop();
    MCUH323EndPoint *ep;
    MCUSipEndPoint *sep;
    RegistrarGk *gk;

    PString registrar_domain;
    PString internal_room_prefix;

    BOOL allow_internal_calls;
    BOOL sip_require_password;
    BOOL sip_allow_unauth_mcu_calls;
    BOOL sip_allow_unauth_internal_calls;
    BOOL enable_gatekeeper;
    BOOL h323_require_h235;
    BOOL h323_allow_unreg_mcu_calls;
    BOOL h323_allow_unreg_internal_calls;
    unsigned h323_time_to_live;

    BOOL MakeCall(RegistrarConnection *regConn, PString & username_in, PString & username_out);
    BOOL MakeCall(RegistrarConnection *regConn, RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out);
    PString GetAccountAddress(RegistrarAccount *regAccount);

    int SipSendMessage(RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out, PString message);
    //int H323SendMessage(RegistrarAccount *regAccount_out, PString message);

    int SipForwardMessage(msg_t *msg);

    // internal function
    int OnReceivedSipRegister(const msg_t *msg);
    int OnReceivedSipInvite(const msg_t *msg);
    int OnReceivedSipSubscribe(msg_t *msg);
    int OnReceivedSipMessage(msg_t *msg);
    void SubscriptionProcess();
    int SipPolicyCheck(const msg_t *msg, RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out);

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

    void Lock()      { mutex.Wait(); }
    void Unlock()    { mutex.Signal(); }
    PMutex & GetMutex() { return mutex; }
    PMutex mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_REGISTRAR_H
