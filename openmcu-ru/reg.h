/*
 * reg.h
 *
 * Copyright (C) 2014-2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
 * Copyright (C) 2015 Konstantin Yeliseyev, OpenMCU-ru, All Rights Reserved
 *
 * The Initial Developer of the Original Code is Andrey Burbovskiy (andrewb@yandex.ru), All Rights Reserved
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * Contributor(s):  Andrey Burbovskiy (andrewb@yandex.ru)
 *                  Konstantin Yeliseyev (kay27@bk.ru)
 *
 */

#include "precompile.h"

#ifndef _MCU_REGISTRAR_H
#define _MCU_REGISTRAR_H

////////////////////////////////////////////////////////////////////////////////////////////////////

enum RegAccountTypes
{
  ACCOUNT_TYPE_UNKNOWN,
  ACCOUNT_TYPE_SIP,
  ACCOUNT_TYPE_H323,
  ACCOUNT_TYPE_RTSP
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

inline RegAccountTypes GetAccountTypeFromScheme(const PString & scheme)
{
  if(scheme == "h323")
    return ACCOUNT_TYPE_H323;
  else if(scheme == "rtsp")
    return ACCOUNT_TYPE_RTSP;
  else if(scheme == "sip")
    return ACCOUNT_TYPE_SIP;
  return ACCOUNT_TYPE_UNKNOWN;
}
inline PString GetSchemeFromAccountType(RegAccountTypes type)
{
  PString scheme;
  if(type == ACCOUNT_TYPE_H323)
    return "h323";
  else if(type == ACCOUNT_TYPE_RTSP)
    return "rtsp";
  else if(type == ACCOUNT_TYPE_SIP)
    return "sip";
  return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

class Registrar;
class RegistrarAccount;
class MCUSipEndPoint;

////////////////////////////////////////////////////////////////////////////////////////////////////

class RegistrarAccount
{
  public:
    RegistrarAccount(Registrar *_registrar, long _id, RegAccountTypes _account_type, PString _username)
    {
      registrar = _registrar;
      id = _id;
      account_type = _account_type;
      username = _username;
      auth.username = username;
      host = "";
      if(account_type == ACCOUNT_TYPE_H323)
        port = 1720;
      else if(account_type == ACCOUNT_TYPE_SIP)
        port = 5060;
      else
        port = 0;
      domain = PRODUCT_NAME_TEXT;
      expires = 0;
      is_saved_account = FALSE;
      registered = FALSE;
      keep_alive_enable = FALSE;
      keep_alive_interval = 0;
      keep_alive_time_request = PTime(3600);
      keep_alive_time_response = PTime(0);
      msg_reg = NULL;
      start_time = PTime(0);
    }

    ~RegistrarAccount()
    {
      if(msg_reg)
      {
        msg_destroy(msg_reg);
        msg_reg = NULL;
      }
    }

    PTimedMutex & GetMutex()
    { return account_mutex; }

    void Unlock();

    long GetID() const
    { return id; }

    PString GetUrl();

    void SetRegisterMsg(const msg_t * msg)
    {
      PWaitAndSignal m(account_mutex);
      msg_destroy(msg_reg);
      msg_reg = msg_dup(msg);
      msg_addr_copy(msg_reg, msg);
    }

    msg_t * GetRegisterMsgCopy()
    {
      if(!msg_reg)
        return NULL;
      PWaitAndSignal m(account_mutex);
      msg_t *msg = msg_dup(msg_reg);
      msg_addr_copy(msg, msg_reg);
      return msg;
    }

    PString display_name;
    PString display_name_saved;
    PString username;
    PString host;
    PString domain;
    unsigned port;
    PString transport;
    PString h323id;

    PString remote_application;

    PString sip_call_processing;
    PString h323_call_processing;

    HTTPAuth auth;

    PTime start_time;
    time_t expires;

    BOOL keep_alive_enable;
    time_t keep_alive_interval;
    PTime keep_alive_time_request;
    PTime keep_alive_time_response;

    OpalGloballyUniqueID h323CallIdentifier; // h323 incoming call

    BOOL is_saved_account;
    BOOL registered;

    RegAccountTypes account_type;

  protected:
    long id;
    msg_t *msg_reg;

    PTimedMutex account_mutex;
    Registrar *registrar;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class AbookAccount
{
  public:
    AbookAccount()
    {
      Clear();
    }
    AbookAccount(RegAccountTypes _account_type, PString _username)
    {
      Clear();
      account_type = _account_type;
      username = _username;
      host = "";
      if(account_type == ACCOUNT_TYPE_H323)
        port = 1720;
      else if(account_type == ACCOUNT_TYPE_SIP)
        port = 5060;
      else
        port = 0;
    }

    void Clear()
    {
      is_abook = false;
      is_account = false;
      is_saved_account = false;
      account_type = ACCOUNT_TYPE_UNKNOWN;
      username = "";
      host = "";
      port = 0;
      transport = "";
      display_name = "";
      remote_application = "";
      reg_state = 0;
      reg_info = "";
      conn_state = 0;
      conn_info = "";
      ping_state = 0;
      ping_info = "";
    }

    void Set(const AbookAccount & ab)
    {
      is_abook = ab.is_abook;
      is_account = ab.is_account;
      is_saved_account = ab.is_saved_account;
      account_type = ab.account_type;
      username = ab.username;
      host = ab.host;
      port = ab.port;
      transport = ab.transport;
      display_name = ab.display_name;
      remote_application = ab.remote_application;
      reg_state = ab.reg_state;
      reg_info = ab.reg_info;
      conn_state = ab.conn_state;
      conn_info = ab.conn_info;
      ping_state = ab.ping_state;
      ping_info = ab.ping_info;
    }

    PString AsJsArray(int state = 0);
    MCUJSON * AsJSON(int state = 0);
    void SendRoomControl(int state = 0);
    void SaveConfig();
    PString GetUrl();

    bool is_abook;
    bool is_account;
    bool is_saved_account;
    RegAccountTypes account_type;
    PString username;
    PString host;
    unsigned port;
    PString transport;
    PString display_name;
    PString remote_application;

    int reg_state;
    PString reg_info;
    int conn_state;
    PString conn_info;
    int ping_state;
    PString ping_info;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RegistrarConnection
{
  public:
    RegistrarConnection(Registrar *_registrar, long _id, PString callToken, PString _username_in, PString _username_out)
    {
      registrar = _registrar;
      id = _id;
      callToken_in = callToken;
      username_in = _username_in;
      username_out = _username_out;
      account_type_in = ACCOUNT_TYPE_UNKNOWN;
      account_type_out = ACCOUNT_TYPE_UNKNOWN;
      msg_invite = NULL;
      state = CONN_IDLE;
      start_time = PTime();
      accept_timeout = 20; // 20 sec connection accept timeout
    }

    ~RegistrarConnection()
    {
      if(msg_invite)
      {
        msg_destroy(msg_invite);
        msg_invite = NULL;
      }
    }

    PTimedMutex & GetMutex()
    { return conn_mutex; }

    void Unlock();

    long GetID() const
    { return id; }

    void SetInviteMsg(const msg_t * msg)
    {
      PWaitAndSignal m(conn_mutex);
      msg_destroy(msg_invite);
      msg_invite = msg_dup(msg);
      msg_addr_copy(msg_invite, msg);
    }

    msg_t * GetInviteMsgCopy()
    {
      if(!msg_invite)
        return NULL;
      PWaitAndSignal m(conn_mutex);
      msg_t *msg = msg_dup(msg_invite);
      msg_addr_copy(msg, msg_invite);
      return msg;
    }

    PString username_in;
    PString username_out;
    PString roomname;

    PString callToken_in;
    PString callToken_out;

    RegAccountTypes account_type_in;
    RegAccountTypes account_type_out;

    RegConnectionStates state;

    time_t accept_timeout;
    PTime start_time;

  protected:
    long id;
    msg_t *msg_invite;

    PTimedMutex conn_mutex;
    Registrar *registrar;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RegistrarSubscription
{
  public:
    RegistrarSubscription(Registrar *_registrar, long _id, PString _username_in, PString _username_out)
      :registrar(_registrar)
    {
      registrar = _registrar;
      id = _id;
      username_in = _username_in;
      username_out = _username_out;
      username_pair = username_in+"@"+username_out;
      state = SUB_STATE_CLOSED;
      msg_sub = NULL;
      start_time = PTime();
      expires = 0;
      cseq = 100;
    }

    ~RegistrarSubscription()
    {
      if(msg_sub)
      {
        msg_destroy(msg_sub);
        msg_sub = NULL;
      }
    }

    PTimedMutex & GetMutex()
    { return sub_mutex; }

    void Unlock();

    long GetID() const
    { return id; }

    void SetSubMsg(const msg_t * msg)
    {
      PWaitAndSignal m(sub_mutex);
      msg_destroy(msg_sub);
      msg_sub = msg_dup(msg);
      msg_addr_copy(msg_sub, msg);
    }

    msg_t * GetSubMsgCopy()
    {
      if(!msg_sub)
        return NULL;
      PWaitAndSignal m(sub_mutex);
      msg_t *msg = msg_dup(msg_sub);
      msg_addr_copy(msg, msg_sub);
      return msg;
    }

    PString username_in;
    PString username_out;
    PString username_pair;
    PString ruri_str;
    PString contact_str;
    int cseq;

    PTime start_time;
    time_t expires;

    RegSubscriptionStates state;

  protected:
    long id;
    msg_t *msg_sub;

    PTimedMutex sub_mutex;
    Registrar *registrar;
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

    virtual H323Transactor * CreateListener(H323Transport * transport);

    BOOL IsGatekeeperRouted() const { return isGatekeeperRouted; }

    void SetAllowDuplicateAlias(BOOL enable)
    { canHaveDuplicateAlias = enable; }

    void SendUnregister(const PString & alias, int reason = H225_UnregRequestReason::e_maintenance);

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

    PString trace_section;

    H323Transactor *gkListener;
    Registrar *registrar;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUH323Gatekeeper : public H323Gatekeeper
{
  PCLASSINFO(MCUH323Gatekeeper, H323Gatekeeper);

  public:
    MCUH323Gatekeeper(H323EndPoint & endpoint, H323Transport * transport)
      : H323Gatekeeper(endpoint, transport)
    { }

    void AddIgnoreConnection(unsigned ref, MCUH323Connection *conn)
    {
      ignoreList.Insert(conn, (long)ref);
    }

    void RemoveIgnoreConnection(unsigned ref)
    { ignoreList.Erase((long)ref); }

    virtual BOOL MakeRequest(Request & request)
    {
      H323TransactionPDU & pdu = request.requestPDU;
      if(pdu.GetPDU().GetTag() == H323RasPDU::e_admissionRequest)
      {
        H225_AdmissionRequest arq((H225_AdmissionRequest &)request.requestPDU.GetChoice().GetObject());
        MCUConnectionList::shared_iterator it = ignoreList.Find((long)arq.m_callReferenceValue);
        if(it != ignoreList.end())
          return TRUE;

      }
      else if(pdu.GetPDU().GetTag() == H323RasPDU::e_disengageRequest)
      {
        H225_DisengageRequest arq((H225_DisengageRequest &)request.requestPDU.GetChoice().GetObject());
        MCUConnectionList::shared_iterator it = ignoreList.Find((long)arq.m_callReferenceValue);
        if(it != ignoreList.end())
          return TRUE;
      }

      return H323Gatekeeper::MakeRequest(request);
    }
    //InfoRequestResponse(*this, pdu.m_h323_uu_pdu, TRUE);

  protected:
    MCUConnectionList ignoreList;
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
      registrarGk = NULL;
      trace_section = "Registrar: ";
    }

    void SetTerminating()
    { terminating = 1; }

    void SetInitConfig()
    { init_config = 1; }

    void SetInitAccounts()
    { init_accounts = 1; }

    BOOL AddAbookAccount(const PString & address);
    BOOL RemoveAbookAccount(const PString & address);

    const PString & GetRegistrarDomain() const { return registrar_domain; };

    void RefreshAccountStatusList();

    void ConnectionCreated(const PString & callToken);
    void ConnectionEstablished(const PString & callToken);
    void ConnectionCleared(const PString & callToken);
    void SetRequestedRoom(const PString & callToken, PString & requestedRoom);

    BOOL MakeCall(const PString & room, const PString & to, PString & callToken);

    int OnReceivedSipRegister(const msg_t *msg);
    int OnReceivedSipInvite(const msg_t *msg);
    int OnReceivedSipSubscribe(msg_t *msg);
    int OnReceivedSipMessage(msg_t *msg);
    int OnReceivedSipOptionsResponse(const msg_t *msg);
    H323Connection::AnswerCallResponse OnReceivedH323Invite(MCUH323Connection *conn);

    nta_agent_t *GetAgent() { return sep->GetAgent(); };
    su_home_t *GetHome() { return sep->GetHome(); };
    MCUSipEndPoint *GetSep() { return sep; };

    RegistrarAccount * InsertAccountWithLock(RegAccountTypes account_type, const PString & username);
    RegistrarAccount * FindAccountWithLock(RegAccountTypes account_type, const PString & username);
    PString FindAccountNameFromH323Id(const PString & id);

    MCUQueuePString & GetQueue()
    { return regQueue; }

    MCURegistrarAccountList & GetAccountList()
    { return accountList; }

    MCURegistrarSubscriptionList & GetSubscriptionList()
    { return subscriptionList; }

    MCURegistrarConnectionList & GetConnectionList()
    { return connectionList; }

    MCUAbookList & GetAbookList()
    { return abookList; }

  protected:
    void Main();
    void MainLoop();

    void Terminating();
    void InitConfig();
    void InitAccounts();
    void InitAbook();

    MCUH323EndPoint *ep;
    MCUSipEndPoint *sep;
    RegistrarGk *registrarGk;

    PString registrar_domain;

    int restart;
    int terminating;
    int init_config;
    int init_accounts;

    PString trace_section;

    BOOL allow_internal_calls;
    BOOL sip_allow_unauth_reg;
    BOOL sip_allow_unauth_mcu_calls;
    BOOL sip_allow_unauth_internal_calls;
    BOOL enable_gatekeeper;
    BOOL h323_allow_unauth_reg;
    BOOL h323_allow_unreg_mcu_calls;
    BOOL h323_allow_unreg_internal_calls;
    int sip_reg_min_expires;
    int sip_reg_max_expires;
    int h323_min_time_to_live;
    int h323_max_time_to_live;

    BOOL InternalMakeCall(RegistrarConnection *rconn, const PString & username_in, const PString & username_out);
    BOOL InternalMakeCall(RegistrarConnection *rconn, RegistrarAccount *raccount_in, RegistrarAccount *raccount_out);

    BOOL SipSendNotify(RegistrarSubscription *rsub);
    BOOL SipSendMessage(RegistrarAccount *raccount_in, RegistrarAccount *raccount_out, const PString & message);
    BOOL SipSendPing(RegistrarAccount *raccount);

    void ProcessAccountList();
    void ProcessSubscriptionList();
    void ProcessConnectionList();
    void ProcessKeepAlive();

    int SipPolicyCheck(const msg_t *msg, msg_t *msg_reply, RegistrarAccount *raccount_in, RegistrarAccount *raccount_out);

    // internal call process function
    void Leave(int account_type, const PString & callToken);
    void IncomingCallAccept(RegistrarConnection *rconn);
    void IncomingCallCancel(RegistrarConnection *rconn);
    void OutgoingCallCancel(RegistrarConnection *rconn);

    RegistrarSubscription * InsertSubWithLock(const PString & username_in, const PString & username_out);
    RegistrarSubscription * FindSubWithLock(const PString & username_pair);

    RegistrarConnection * InsertRegConnWithLock(const PString & callToken, const PString & username_in, const PString & username_out);
    RegistrarConnection * FindRegConnWithLock(const PString & callToken);
    RegistrarConnection * FindRegConnWithLock(RegAccountTypes account_type, const PString & username);
    bool HasRegConn(const PString & callToken);
    bool HasRegConn(RegAccountTypes account_type, const PString & username);

    PDECLARE_NOTIFIER(PThread, Registrar, AccountThread);
    PThread * accountThread;

    PDECLARE_NOTIFIER(PThread, Registrar, ConnectionThread);
    PThread * connectionThread;

    PDECLARE_NOTIFIER(PThread, Registrar, SubscriptionThread);
    PThread * subscriptionThread;

    PDECLARE_NOTIFIER(PThread, Registrar, QueueThread);
    PThread *queueThread;
    MCUQueuePString regQueue;

    PDECLARE_NOTIFIER(PThread, Registrar, BookThread);
    PThread * bookThread;

    PDECLARE_NOTIFIER(PThread, Registrar, AliveThread);
    PThread * aliveThread;

    void QueueInvite(const PString & data);
    void QueueCleared(const PString & data);
    void QueueEstablished(const PString & data);

    MCURegistrarAccountList accountList;
    MCURegistrarSubscriptionList subscriptionList;
    MCURegistrarConnectionList connectionList;
    MCUAbookList abookList;

    // mutex - используется в функциях OnReceived, MakeCall
    // предотвращает создание в списках двух одноименных объектов
    PMutex mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_REGISTRAR_H
