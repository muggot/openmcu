
#include <ptlib.h>

#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ConnectionCreated(const PString & callToken)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ConnectionEstablished(const PString & callToken)
{
  RegistrarConnection *regConn = FindRegConnWithLock(callToken);
  if(!regConn)
    return;

  if(regConn->state == CONN_MCU_WAIT)
    regConn->state = CONN_MCU_ESTABLISHED;

  if(regConn->state == CONN_WAIT && regConn->callToken_out == callToken)
    regConn->state = CONN_ACCEPT_IN;

  regConn->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ConnectionCleared(const PString & callToken)
{
  RegistrarConnection *regConn = FindRegConnWithLock(callToken);
  if(!regConn)
    return;

  if(regConn->state == CONN_MCU_WAIT || regConn->state == CONN_MCU_ESTABLISHED)
    regConn->state = CONN_IDLE;

  if(regConn->state == CONN_WAIT && regConn->callToken_out == callToken)
    regConn->state = CONN_CANCEL_IN;

  if(regConn->state == CONN_WAIT && regConn->callToken_in == callToken)
    regConn->state = CONN_CANCEL_OUT;

  if(regConn->state == CONN_ESTABLISHED && regConn->callToken_out == callToken)
    regConn->state = CONN_LEAVE_IN;

  if(regConn->state == CONN_ESTABLISHED && regConn->callToken_in == callToken)
    regConn->state = CONN_LEAVE_OUT;

  if(regConn->state == CONN_ACCEPT_IN && regConn->callToken_out == callToken)
    regConn->state = CONN_LEAVE_IN;

  if(regConn->state == CONN_ACCEPT_IN && regConn->callToken_in == callToken)
    regConn->state = CONN_LEAVE_OUT;

  regConn->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::SetRequestedRoom(const PString & callToken, PString & requestedRoom)
{
  RegistrarConnection *regConn = FindRegConnWithLock(callToken);
  if(regConn)
  {
    if(regConn->roomname != "")
      requestedRoom = regConn->roomname;
    regConn->Unlock();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::MakeCall(PString room, PString address, PString & callToken)
{
  // the default protocol H.323
  // correct formats - proto:username, proto:username@, proto:ip, proto:@ip
  // wrong formats - proto:@username, proto:ip@

  MCUURL url(address);

  RegAccountTypes account_type = ACCOUNT_TYPE_UNKNOWN;
  if(url.GetScheme() == "sip")
    account_type = ACCOUNT_TYPE_SIP;
  else if(url.GetScheme() == "h323")
    account_type = ACCOUNT_TYPE_H323;
  else if(url.GetScheme() == "rtsp")
    account_type = ACCOUNT_TYPE_RTSP;
  else
    return FALSE;

  PString username_out;
  if(url.GetUserName() != "")
    username_out = url.GetUserName();
  else if(url.GetHostName() != "")
    username_out = url.GetHostName();
  else
    return FALSE;

  RegistrarAccount *regAccount_out = NULL;
  RegistrarConnection *regConn = NULL;
  PWaitAndSignal m(mutex);

  regAccount_out = FindAccountWithLock(account_type, username_out);
  if(regAccount_out)
  {
    // update address from account
    address = regAccount_out->GetUrl();
    regAccount_out->Unlock();
    // update url
    url = MCUURL(address);
  }
  // initial username_out, can be empty
  username_out = url.GetUserName();

  if(account_type == ACCOUNT_TYPE_SIP)
  {
    PString call_id_str = PGloballyUniqueID().AsString();
    PString *cmd = new PString("invite:"+room+","+address+","+call_id_str);
    sep->SipQueue.Push(cmd);
    callToken = call_id_str;
  }
  else if(account_type == ACCOUNT_TYPE_H323)
  {
    H323Transport * transport = NULL;
    if(ep->GetGatekeeper())
    {
      PString gk_host = ep->GetGatekeeperHostName();
      if(url.GetHostName() == "" || gk_host == url.GetHostName())
      {
        address = url.GetUserName();
        PTRACE(1, "Found gatekeeper, change address " << url.GetUrl() << " -> " << address);
      }
      else
      {
        H323TransportAddress taddr(url.GetHostName()+":"+url.GetPort());
        transport = taddr.CreateTransport(*ep);
        transport->SetRemoteAddress(taddr);
      }
    }
    void *userData = new PString(room);
    ep->MakeCall(address, transport, callToken, userData);
  }
  else if(account_type == ACCOUNT_TYPE_RTSP)
  {
    PString callToken = PGloballyUniqueID().AsString();
    MCURtspConnection *rCon = new MCURtspConnection(sep, ep, callToken);
    if(rCon->Connect(room, address) == 0)
    {
      rCon->LeaveMCU();
      return FALSE;
    }
  }

  if(callToken != "")
  {
    regConn = InsertRegConnWithLock(callToken, room, username_out);
    regConn->account_type_out = account_type;
    regConn->callToken_out = callToken;
    regConn->roomname = room;
    regConn->state = CONN_MCU_WAIT;
    regConn->Unlock();
  }

  if(callToken == "")
    return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::MakeCall(RegistrarConnection *regConn, PString & username_in, PString & username_out)
{
  RegistrarAccount *regAccount_in = NULL;
  RegistrarAccount *regAccount_out = NULL;
  PWaitAndSignal m(mutex);

  regAccount_in = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_in);
  regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_out);
  BOOL ret = MakeCall(regConn, regAccount_in, regAccount_out);

  if(regAccount_in) regAccount_in->Unlock();
  if(regAccount_out) regAccount_out->Unlock();
  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::MakeCall(RegistrarConnection *regConn, RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out)
{
  if(!regConn || !regAccount_in || !regAccount_out)
    return FALSE;

  regConn->account_type_in = regAccount_in->account_type;
  regConn->account_type_out = regAccount_out->account_type;

  PString address = regAccount_out->GetUrl();

  if(regAccount_out->account_type == ACCOUNT_TYPE_SIP)
  {
    PString callToken;
    if(sep->SipMakeCall(regConn->username_in, address, callToken))
    {
      regConn->callToken_out = callToken;
      return TRUE;
    }
  }
  else if(regAccount_out->account_type == ACCOUNT_TYPE_H323)
  {
    PString callToken_out;
    void *userData = new PString(regConn->username_in);
    ep->MakeCall(address, callToken_out, userData);
    if(callToken_out != "")
    {
      regConn->callToken_out = callToken_out;
      return TRUE;
    }
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString RegistrarAccount::GetAuthStr()
{
  if(nonce == "")
    nonce = PGloballyUniqueID().AsString();
  PString sip_auth_str = scheme+" "+"realm=\""+domain+"\",nonce=\""+nonce+"\",algorithm="+algorithm;
  return sip_auth_str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RegistrarAccount * Registrar::InsertAccountWithLock(RegAccountTypes account_type, PString username)
{
  RegistrarAccount *regAccount = new RegistrarAccount(account_type, username);
  regAccount->Lock();
  return InsertAccount(regAccount);
}
RegistrarAccount * Registrar::InsertAccount(RegistrarAccount *regAccount)
{
  PWaitAndSignal m(mutex);
  AccountMap.insert(AccountMapType::value_type(PString(regAccount->account_type)+":"+regAccount->username, regAccount));
  return regAccount;
}
RegistrarAccount * Registrar::InsertAccount(RegAccountTypes account_type, PString username)
{
  RegistrarAccount *regAccount = new RegistrarAccount(account_type, username);
  return InsertAccount(regAccount);
}
RegistrarAccount * Registrar::FindAccountWithLock(RegAccountTypes account_type, PString username)
{
  PWaitAndSignal m(mutex);
  RegistrarAccount *regAccount = FindAccount(account_type, username);
  if(regAccount)
    regAccount->Lock();
  return regAccount;
}
RegistrarAccount * Registrar::FindAccount(RegAccountTypes account_type, PString username)
{
  RegistrarAccount *regAccount = NULL;
  PWaitAndSignal m(mutex);
  for(AccountMapType::iterator it=AccountMap.begin(); it!=AccountMap.end(); ++it)
  {
    if(it->second->username == username && (account_type == ACCOUNT_TYPE_UNKNOWN || account_type == it->second->account_type))
    {
      regAccount = it->second;
      break;
    }
  }
  return regAccount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Subscription * Registrar::InsertSubWithLock(Registrar *_registrar, PString username_in, PString username_out)
{
  Subscription *subAccount = new Subscription(_registrar, username_in, username_out);
  subAccount->Lock();
  return InsertSub(subAccount);
}
Subscription * Registrar::InsertSub(Subscription *subAccount)
{
  PWaitAndSignal m(mutex);
  SubscriptionMap.insert(SubscriptionMapType::value_type(subAccount->username_pair, subAccount));
  return subAccount;
}
Subscription * Registrar::InsertSub(Registrar *_registrar, PString username_in, PString username_out)
{
  Subscription *subAccount = new Subscription(_registrar, username_in, username_out);
  return InsertSub(subAccount);
}
Subscription *Registrar::FindSubWithLock(PString username_pair)
{
  PWaitAndSignal m(mutex);
  Subscription *subAccount = FindSub(username_pair);
  if(subAccount)
    subAccount->Lock();
  return subAccount;
}
Subscription *Registrar::FindSub(PString username_pair)
{
  Subscription *subAccount = NULL;
  PWaitAndSignal m(mutex);
  SubscriptionMapType::iterator it = SubscriptionMap.find(username_pair);
  if(it != SubscriptionMap.end())
    subAccount = it->second;
  return subAccount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RegistrarConnection * Registrar::InsertRegConnWithLock(PString callToken, PString username_in, PString username_out)
{
  RegistrarConnection *regConn = new RegistrarConnection(callToken, username_in, username_out);
  regConn->Lock();
  return InsertRegConn(regConn);
}
RegistrarConnection * Registrar::InsertRegConn(RegistrarConnection *regConn)
{
  PWaitAndSignal m(mutex);
  RegConnMap.insert(RegConnMapType::value_type(regConn->callToken_in, regConn));
  return regConn;
}
RegistrarConnection * Registrar::InsertRegConn(PString callToken, PString username_in, PString username_out)
{
  RegistrarConnection *regConn = new RegistrarConnection(callToken, username_in, username_out);
  return InsertRegConn(regConn);
}
RegistrarConnection *Registrar::FindRegConnWithLock(PString callToken)
{
  PWaitAndSignal m(mutex);
  RegistrarConnection *regConn = FindRegConn(callToken);
  if(regConn)
    regConn->Lock();
  return regConn;
}
RegistrarConnection *Registrar::FindRegConn(PString callToken)
{
  RegistrarConnection *regConn = NULL;
  PWaitAndSignal m(mutex);
  for(RegConnMapType::iterator it=RegConnMap.begin(); it!=RegConnMap.end(); ++it)
  {
    if(it->second->callToken_in == callToken || it->second->callToken_out == callToken)
    {
      regConn = it->second;
      break;
    }
  }
  return regConn;
}
RegistrarConnection *Registrar::FindRegConnUsername(PString username)
{
  RegistrarConnection *regConn = NULL;
  PWaitAndSignal m(mutex);
  for(RegConnMapType::iterator it=RegConnMap.begin(); it!=RegConnMap.end(); ++it)
  {
    if(it->second->username_in == username || it->second->username_out == username)
    {
      regConn = it->second;
      break;
    }
  }
  return regConn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::IncomingCallAccept(RegistrarConnection *regConn)
{
  if(regConn->account_type_in == ACCOUNT_TYPE_SIP)
  {
    sep->CreateIncomingConnection(regConn->msg_invite);
  }
  else if(regConn->account_type_in == ACCOUNT_TYPE_H323)
  {
    MCUH323Connection *conn = (MCUH323Connection *)ep->FindConnectionWithLock(regConn->callToken_in);
    if(conn)
    {
      conn->AnsweringCall(H323Connection::AnswerCallNow);
      conn->Unlock();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::IncomingCallCancel(RegistrarConnection *regConn)
{
  if(regConn->callToken_in != "")
    Leave(regConn->account_type_in, regConn->callToken_in);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::OutgoingCallCancel(RegistrarConnection *regConn)
{
  if(regConn->callToken_out != "")
    Leave(regConn->account_type_out, regConn->callToken_out);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::Leave(int account_type, PString callToken)
{
  MCUH323Connection *conn = NULL;
  if(account_type == ACCOUNT_TYPE_SIP)
    conn = (MCUSipConnection *)ep->FindConnectionWithLock(callToken);
  else if(account_type == ACCOUNT_TYPE_H323)
    conn = (MCUH323Connection *)ep->FindConnectionWithLock(callToken);
  if(conn)
  {
    conn->LeaveMCU();
    conn->Unlock();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ProcessSubscriptionList()
{
  PWaitAndSignal m(mutex);
  PTime now;

  for(SubscriptionMapType::iterator it=SubscriptionMap.begin(); it!=SubscriptionMap.end(); )
  {
    Subscription *subAccount = it->second;
    if(!subAccount->TryLock())
      continue;

    RegSubscriptionStates state_new;
    if(now > subAccount->start_time + PTimeInterval(subAccount->expires*1000))
    {
      SubscriptionMap.erase(it++);
      delete subAccount;
      continue;
    } else {
      ++it;
    }

    RegistrarAccount *regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_SIP, subAccount->username_out);
    if(!regAccount_out)
      regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_H323, subAccount->username_out);

    if(regAccount_out && regAccount_out->registered)
    {
      state_new = SUB_STATE_OPEN;
      RegistrarConnection *regConn = FindRegConnUsername(subAccount->username_out);
      if(regConn)
        state_new = SUB_STATE_BUSY;
    } else {
      state_new = SUB_STATE_CLOSED;
    }

    // send notify
    if(subAccount->state != state_new)
    {
      subAccount->state = state_new;
      if(subAccount->msg_sub) // SIP
        SipSendNotify(subAccount->msg_sub, subAccount);
    }
    if(subAccount) subAccount->Unlock();
    if(regAccount_out) regAccount_out->Unlock();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ProcessKeepAlive()
{
  PWaitAndSignal m(mutex);
  PTime now;

  for(AccountMapType::iterator it = AccountMap.begin(); it != AccountMap.end(); ++it)
  {
    RegistrarAccount *regAccount = it->second;
    if(regAccount->account_type != ACCOUNT_TYPE_SIP)
      continue;
    if(!regAccount->TryLock())
      continue;
    if(regAccount->keep_alive_enable && now > regAccount->keep_alive_time_request+PTimeInterval(regAccount->keep_alive_interval*1000))
    {
      regAccount->keep_alive_time_request = now;
      SipSendPing(regAccount);
    }
    regAccount->Unlock();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ProcessAccountList()
{
  PWaitAndSignal m(mutex);
  PTime now;

  for(AccountMapType::iterator it = AccountMap.begin(); it != AccountMap.end(); ++it)
  {
    RegistrarAccount *regAccount = it->second;
    if(!regAccount->TryLock())
      continue;
    // expires
    if(regAccount->registered)
    {
      if(now > regAccount->start_time + PTimeInterval(regAccount->expires*1000))
        regAccount->registered = FALSE;
    }
    regAccount->Unlock();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ProcessConnectionList()
{
  PWaitAndSignal m(mutex);
  PTime now;

  for(RegConnMapType::iterator it = RegConnMap.begin(); it != RegConnMap.end(); )
  {
    RegistrarConnection *regConn = it->second;
    if(!regConn->TryLock())
      continue;
    // remove empty connection
    if(regConn->state == CONN_IDLE)
    {
      RegConnMap.erase(it++);
      delete regConn;
      continue;
    } else {
      ++it;
    }
    // MCU call answer limit
    if(regConn->state == CONN_MCU_WAIT)
    {
      if(now > regConn->start_time + PTimeInterval(regConn->accept_timeout*1000))
      {
        OutgoingCallCancel(regConn);
        regConn->state = CONN_END;
      }
    }
    // internal call answer limit
    if(regConn->state == CONN_WAIT)
    {
      if(now > regConn->start_time + PTimeInterval(regConn->accept_timeout*1000))
      {
        IncomingCallCancel(regConn);
        OutgoingCallCancel(regConn);
        regConn->state = CONN_END;
      }
    }
    // make internal call
    if(regConn->state == CONN_WAIT)
    {
      if(regConn->callToken_out == "")
      {
        if(!MakeCall(regConn, regConn->username_in, regConn->username_out))
        {
          regConn->state = CONN_CANCEL_IN;
        }
      }
    }
    // accept incoming
    if(regConn->state == CONN_ACCEPT_IN)
    {
      IncomingCallAccept(regConn);
      regConn->state = CONN_ESTABLISHED;
    }
    // cancel incoming
    if(regConn->state == CONN_CANCEL_IN)
    {
      IncomingCallCancel(regConn);
      regConn->state = CONN_END;
    }
    // cancel outgoing
    if(regConn->state == CONN_CANCEL_OUT)
    {
      OutgoingCallCancel(regConn);
      regConn->state = CONN_END;
    }
    // leave incoming
    if(regConn->state == CONN_LEAVE_IN)
    {
      Leave(regConn->account_type_in, regConn->callToken_in);
      regConn->state = CONN_END;
    }
    // leave outgoing
    if(regConn->state == CONN_LEAVE_OUT)
    {
      Leave(regConn->account_type_out, regConn->callToken_out);
      regConn->state = CONN_END;
    }
    // internal call end
    if(regConn->state == CONN_END)
    {
      regConn->state = CONN_IDLE;
    }
    regConn->Unlock();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::RefreshAccountStatusList()
{
  PWaitAndSignal m(mutex);
  PStringArray list;

  for(AccountMapType::iterator it=AccountMap.begin(); it!=AccountMap.end(); ++it)
  {
    RegistrarAccount *regAccount = it->second;
    int reg_state = 0;
    int conn_state = 0;
    int ping_state = 0;

    if(regAccount->registered)
      reg_state = 2;
    else if(regAccount->enable)
      reg_state = 1;

    RegistrarConnection *regConn = FindRegConnUsername(regAccount->username);
    if(regConn)
    {
     if(regConn->state == CONN_WAIT || regConn->state == CONN_MCU_WAIT)
       conn_state = 1;
     else if(regConn->state == CONN_ESTABLISHED || regConn->state == CONN_MCU_ESTABLISHED)
       conn_state = 2;
    }

    if(regAccount->keep_alive_enable)
    {
      if(regAccount->keep_alive_time_response > regAccount->keep_alive_time_request-PTimeInterval(regAccount->keep_alive_interval*1000-2000))
        ping_state = 1;
      else
        ping_state = 2;
    }

    PString remote_application = regAccount->remote_application;
    PString reg_info;
    PString conn_info;
    PString ping_info;
    if(reg_state == 2)
      reg_info = regAccount->start_time.AsString("hh:mm:ss dd.MM.yyyy");
    if(conn_state == 2)
      conn_info = regConn->start_time.AsString("hh:mm:ss dd.MM.yyyy");
    if(ping_state == 2)
      ping_info = regAccount->keep_alive_time_response.AsString("hh:mm:ss dd.MM.yyyy");

    list.AppendString(regAccount->display_name+" ["+regAccount->GetUrl()+"],"+
                      PString(regAccount->abook_enable)+","+
                      remote_application+","+
                      PString(reg_state)+","+
                      reg_info+","+
                      PString(conn_state)+","+
                      conn_info+","+
                      PString(ping_state)+","+
                      ping_info
                     );
  }
  if(account_status_list != list)
  {
    account_status_list = list;
    OpenMCU::Current().ManagerRefreshAddressBook();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PStringArray Registrar::GetAccountList()
{
  return account_status_list;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::MainLoop()
{
  PTime time_sub;
  PTime time_abook;
  PTime time_account;
  PTime time_conn;
  PTime time_ping;
  while(1)
  {
    if(terminating)
    {
      Terminating();
      return;
    }
    if(restart)
    {
      restart = 0;
      init_config = 1;
      init_accounts = 1;
    }
    if(init_config)
    {
      init_config = 0;
      InitConfig();
    }
    if(init_accounts)
    {
      init_accounts = 0;
      InitAccounts();
    }
    //
    PTime now;
    // accounts
    if((now-time_account).GetMilliSeconds() > 1000)
    {
      time_account = now;
      ProcessAccountList();
    }
    // connections
    if((now-time_conn).GetMilliSeconds() > 250)
    {
      time_conn = now;
      ProcessConnectionList();
    }
    // subscriptions
    if((now-time_sub).GetMilliSeconds() > 1000)
    {
      time_sub = now;
      ProcessSubscriptionList();
    }
    // address book
    if((now-time_abook).GetMilliSeconds() > 1000)
    {
      time_abook = now;
      RefreshAccountStatusList();
    }
    // keep alive
    if((now-time_ping).GetMilliSeconds() > 1000)
    {
      time_ping = now;
      ProcessKeepAlive();
    }

    int wait = 100 - PTimeInterval(PTime() - now).GetMilliSeconds();
    if(wait < 30) wait = 30;
    PThread::Sleep(wait);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::InitConfig()
{
  MCUConfig cfg("Registrar Parameters");

  // general parameters
  registrar_domain = cfg.GetString("Registrar domain", "openmcu-ru");
  allow_internal_calls = cfg.GetBoolean("Allow internal calls", TRUE);

  enable_gatekeeper = cfg.GetBoolean("H.323 gatekeeper enable", TRUE);
  if(enable_gatekeeper && registrarGk == NULL)
  {
    PIPSocket::Address address("*");
    WORD port = 1719;
    registrarGk = new RegistrarGk(ep, this);
    PString mcuName = OpenMCU::Current().GetName();
    registrarGk->SetGatekeeperIdentifier(mcuName);
    registrarGk->CreateListener(new H323TransportUDP(*ep, address, port, NULL));
  }
  if(!enable_gatekeeper && registrarGk != NULL)
  {
    delete registrarGk;
    registrarGk = NULL;
  }

  // SIP parameters
  sip_require_password = cfg.GetBoolean("SIP registrar required password authorization", FALSE);
  sip_allow_unauth_mcu_calls = cfg.GetBoolean("SIP allow unauthorized MCU calls", TRUE);
  sip_allow_unauth_internal_calls = cfg.GetBoolean("SIP allow unauthorized internal calls", TRUE);
  sip_reg_min_expires = cfg.GetInteger("SIP registrar minimum expiration", 60);
  sip_reg_max_expires = cfg.GetInteger("SIP registrar maximum expiration", 600);

  // H.323 parameters
  h323_require_h235 = cfg.GetBoolean("H.323 gatekeeper required password authorization", FALSE);
  h323_allow_unreg_mcu_calls = cfg.GetBoolean("H.323 allow unregistered MCU calls", TRUE);
  h323_allow_unreg_internal_calls = cfg.GetBoolean("H.323 allow unregistered internal calls", TRUE);
  h323_min_time_to_live = cfg.GetInteger("H.323 gatekeeper minimum Time To Live", 60);
  h323_max_time_to_live = cfg.GetInteger("H.323 gatekeeper maximum Time To Live", 600);
  if(registrarGk)
  {
    registrarGk->SetRequireH235(h323_require_h235);
    registrarGk->SetMinTimeToLive(h323_min_time_to_live);
    registrarGk->SetMaxTimeToLive(h323_max_time_to_live);
    registrarGk->SetTimeToLive(h323_max_time_to_live);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::InitAccounts()
{
  MCUConfig cfg("Registrar Parameters");

  PString h323SectionPrefix = "H323 Endpoint ";
  PString rtspSectionPrefix = "RTSP Endpoint ";
  PString sipSectionPrefix = "SIP Endpoint ";

  PStringToString h323Passwords;

  Lock();
  for(AccountMapType::iterator reg = AccountMap.begin(); reg != AccountMap.end(); ++reg)
  {
    RegistrarAccount *regAccount = reg->second;
    if(regAccount->account_type == ACCOUNT_TYPE_SIP)
    {
      regAccount->enable = MCUConfig(sipSectionPrefix+regAccount->username).GetBoolean("Registrar", FALSE);
      if(!regAccount->enable && sip_require_password)
        regAccount->registered = FALSE;
    }
    if(regAccount->account_type == ACCOUNT_TYPE_H323)
    {
      regAccount->enable = MCUConfig(h323SectionPrefix+regAccount->username).GetBoolean("Registrar", FALSE);
      if(!regAccount->enable && h323_require_h235)
        regAccount->registered = FALSE;
    }
  }
  Unlock();

  PStringList sect = cfg.GetSections();
  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    RegAccountTypes account_type = ACCOUNT_TYPE_UNKNOWN;
    PString username;
    MCUConfig scfg(sect[i]);
    MCUConfig gcfg;
    if(sect[i].Left(sipSectionPrefix.GetLength()) == sipSectionPrefix)
    {
      account_type = ACCOUNT_TYPE_SIP;
      username = sect[i].Right(sect[i].GetLength()-sipSectionPrefix.GetLength());
      gcfg = MCUConfig(sipSectionPrefix+"*");
    }
    else if(sect[i].Left(h323SectionPrefix.GetLength()) == h323SectionPrefix)
    {
      account_type = ACCOUNT_TYPE_H323;
      username = sect[i].Right(sect[i].GetLength()-h323SectionPrefix.GetLength());
      gcfg = MCUConfig(h323SectionPrefix+"*");
    }
    else if(sect[i].Left(h323SectionPrefix.GetLength()) == rtspSectionPrefix)
    {
      account_type = ACCOUNT_TYPE_RTSP;
      username = sect[i].Right(sect[i].GetLength()-rtspSectionPrefix.GetLength());
      gcfg = MCUConfig(rtspSectionPrefix+"*");
    }

    if(username == "*" || username == "")
      continue;

    unsigned port = scfg.GetInteger("Port");
    if(port == 0) port = gcfg.GetInteger("Port");

    RegistrarAccount *regAccount = FindAccountWithLock(account_type, username);
    if(!regAccount)
      regAccount = InsertAccountWithLock(account_type, username);
    regAccount->enable = scfg.GetBoolean("Registrar", FALSE);
    regAccount->abook_enable = scfg.GetBoolean("Address book", FALSE);
    regAccount->host = scfg.GetString("Host");
    if(port != 0)
      regAccount->port = port;
    regAccount->domain = registrar_domain;
    regAccount->password = scfg.GetString("Password");
    regAccount->display_name = scfg.GetString("Display name");
    if(account_type == ACCOUNT_TYPE_SIP)
    {
      regAccount->keep_alive_interval = scfg.GetString("Ping interval").AsInteger();
      if(regAccount->keep_alive_interval == 0)
        regAccount->keep_alive_interval = gcfg.GetString("Ping interval").AsInteger();
      if(regAccount->keep_alive_interval != 0)
        regAccount->keep_alive_enable = TRUE;
      else
        regAccount->keep_alive_enable = FALSE;

      PString sip_call_processing = scfg.GetString("SIP call processing");
      if(sip_call_processing == "")
        sip_call_processing = gcfg.GetString("SIP call processing", "redirect");
      regAccount->sip_call_processing = sip_call_processing;
    }
    else if(account_type == ACCOUNT_TYPE_H323)
    {
      PString h323_call_processing = scfg.GetString("H.323 call processing");
      if(h323_call_processing == "")
        h323_call_processing = gcfg.GetString("H.323 call processing", "direct");
      regAccount->h323_call_processing = h323_call_processing;

      h323Passwords.Insert(PString(username), new PString(regAccount->password));
    }
    else if(account_type == ACCOUNT_TYPE_RTSP)
    {
      regAccount->abook_enable = TRUE;
    }
    regAccount->Unlock();
  }
  // set gatekeeper parameters
  if(registrarGk) registrarGk->SetPasswords(h323Passwords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::Terminating()
{
  PWaitAndSignal m(mutex);
  if(registrarGk)
  {
    delete registrarGk;
    registrarGk = NULL;
  }
  for(SubscriptionMapType::iterator it = SubscriptionMap.begin(); it != SubscriptionMap.end(); )
  {
    Subscription *subAccount = it->second;
    subAccount->Lock();
    SubscriptionMap.erase(it++);
    delete subAccount;
    subAccount = NULL;
  }
  for(RegConnMapType::iterator it = RegConnMap.begin(); it != RegConnMap.end(); )
  {
    RegistrarConnection *regConn = it->second;
    regConn->Lock();
    RegConnMap.erase(it++);
    delete regConn;
    regConn = NULL;
  }
  for(AccountMapType::iterator it = AccountMap.begin(); it != AccountMap.end(); )
  {
    RegistrarAccount *regAccount = it->second;
    regAccount->Lock();
    AccountMap.erase(it++);
    delete regAccount;
    regAccount = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::Main()
{
  MainLoop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
