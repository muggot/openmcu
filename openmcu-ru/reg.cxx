
#include <ptlib.h>
#include <ptclib/guid.h>

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

BOOL Registrar::MakeCall(PString roomname, PString to, PString & callToken)
{
  PString username_out;
  PString address;
  RegAccountTypes account_type = ACCOUNT_TYPE_UNKNOWN;

  RegistrarAccount *regAccount_out = NULL;
  RegistrarConnection *regConn = NULL;

  if(to.Find("@") == P_MAX_INDEX)
  {
    username_out = to;
  } else {
    MCUURL urlTo(to);
    if(urlTo.GetUserName() != "")
      username_out = urlTo.GetUserName();
    else
      username_out = urlTo.GetHostName();
    if(urlTo.GetScheme() == "sip")
      account_type = ACCOUNT_TYPE_SIP;
    else if(urlTo.GetScheme() == "h323")
      account_type = ACCOUNT_TYPE_H323;
  }
  regAccount_out = FindAccountWithLock(account_type, username_out);
  if(regAccount_out)
  {
    address = GetAccountAddress(regAccount_out);
    username_out = regAccount_out->username;
    regAccount_out->Unlock();
  } else { // MCU call if !regAccount_out
    username_out = address;
    address = MCUURL(to).GetUrl();
  }
  MCUURL url(address);
  if(url.GetScheme() == "sip")
  {
    PGloballyUniqueID id;
    callToken = id.AsString();
    PString *cmd = new PString("invite:"+roomname+","+address+","+callToken);
    sep->SipQueue.Push(cmd);
    callToken = "sip:"+url.GetUserName()+":"+callToken;
  }
  else if(url.GetScheme() == "h323")
  {
    if(url.GetHostName() == "")
      address = url.GetUserName();
    void *userData = new PString(roomname);
    ep->MakeCall(address, callToken, userData);
  }

  if(callToken != "")
  {
    regConn = InsertRegConnWithLock(callToken, roomname, username_out);
    regConn->state = CONN_MCU_WAIT;
    regConn->roomname = roomname;
    regConn->callToken_out = callToken;
    regConn->Unlock();
  }

  if(callToken == "")
    return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::MakeCall(RegistrarConnection *regConn, PString & username_in, PString & username_out)
{
  RegistrarAccount *regAccount_in = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_in);
  RegistrarAccount *regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_out);
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

  PString address = GetAccountAddress(regAccount_out);
  if(address == "")
    return FALSE;

  if(regAccount_out->account_type == ACCOUNT_TYPE_SIP)
  {
    PString call_id;
    nta_outgoing_t *orq = sep->SipMakeCall(regConn->username_in, address, call_id);
    if(orq)
    {
      regConn->orq_invite_out = orq;
      regConn->callToken_out = "sip:"+regAccount_out->username+":"+call_id;
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

PString Registrar::GetAccountAddress(RegistrarAccount *regAccount)
{
  PString address;
  if(regAccount->account_type == ACCOUNT_TYPE_SIP)
  {
    if(regAccount->registered)
      address = MCUURL_SIP(regAccount->msg_reg, 0).GetUrl();
    else if(regAccount->host != "")
      address = regAccount->GetUrl();
    else
      return "";
  }
  else if(regAccount->account_type == ACCOUNT_TYPE_H323)
  {
    address = regAccount->GetUrl();
  }
  return address;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::SubscriptionProcess()
{
  PTime now;
  for(SubscriptionMapType::iterator it=SubscriptionMap.begin(); it!=SubscriptionMap.end(); )
  {
    Subscription *subAccount = it->second;
    subAccount->Lock();
    if(now > subAccount->start_time + PTimeInterval(subAccount->expires*1000))
    {
      SubscriptionMap.erase(it++);
      delete subAccount;
      continue;
    } else {
      ++it;
    }

    RegistrarAccount *regAccount_out = NULL;
    regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_SIP, subAccount->username_out);
    if(!regAccount_out)
      regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_H323, subAccount->username_out);

    if(regAccount_out && regAccount_out->registered)
    {
      subAccount->state_new = SUB_STATE_OPEN;
      RegistrarConnection *regConn = FindRegConnUsername(subAccount->username_out);
      if(regConn)
        subAccount->state_new = SUB_STATE_BUSY;
    } else {
      subAccount->state_new = SUB_STATE_CLOSED;
    }
    // send notify
    if(subAccount->state != subAccount->state_new)
    {
      subAccount->state = subAccount->state_new;
      if(subAccount->msg_sub) // SIP
        SipSendNotify(subAccount->msg_sub, subAccount->state);
    }
    if(subAccount) subAccount->Unlock();
    if(regAccount_out) regAccount_out->Unlock();
  }
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

RegistrarAccount * Registrar::InsertAccountWithLock(RegAccountTypes account_type, PString username, PString host)
{
  RegistrarAccount *regAccount = new RegistrarAccount(account_type, username, host);
  regAccount->Lock();
  return InsertAccount(regAccount);
}
RegistrarAccount * Registrar::InsertAccount(RegistrarAccount *regAccount)
{
  PWaitAndSignal m(mutex);
  AccountMap.insert(AccountMapType::value_type(regAccount->username, regAccount));
  return regAccount;
}
RegistrarAccount * Registrar::InsertAccount(RegAccountTypes account_type, PString username, PString host)
{
  RegistrarAccount *regAccount = new RegistrarAccount(account_type, username, host);
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
  if(regConn->account_type_in == ACCOUNT_TYPE_SIP)
  {
    SipReqReply(regConn->msg_invite, SIP_603_DECLINE);
  }
  Leave(regConn->account_type_in, regConn->callToken_in);
/*
  else if(regConn->account_type_in == ACCOUNT_TYPE_H323)
  {
    MCUH323Connection *conn = (MCUH323Connection *)ep->FindConnectionWithLock(regConn->callToken_in);
    if(conn)
    {
      //conn->AnsweringCall(H323Connection::AnswerCallDenied);
      conn->LeaveMCU();
      conn->Unlock();
    }
  }
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::OutgoingCallCancel(RegistrarConnection *regConn)
{
  if(regConn->orq_invite_out)
  {
    nta_outgoing_cancel(regConn->orq_invite_out);
  }
  if(regConn->callToken_out != "")
  {
    Leave(regConn->account_type_out, regConn->callToken_out);
  }
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

void Registrar::RefreshAddressBook()
{
  PStringArray list;
  PWaitAndSignal m(mutex);
  for(AccountMapType::iterator it=AccountMap.begin(); it!=AccountMap.end(); ++it)
  {
    RegistrarAccount *regAccount = it->second;
    int state = 0;
    RegistrarConnection *regConn = FindRegConnUsername(regAccount->username);
    if(regConn)
    {
     if(regConn->state == CONN_WAIT || regConn->state == CONN_MCU_WAIT)
      state = 1;
     else if(regConn->state == CONN_ESTABLISHED || regConn->state == CONN_MCU_ESTABLISHED)
      state = 2;
    }
    list.AppendString(regAccount->display_name+" ["+regAccount->GetUrl()+"],"+PString(regAccount->enable)+","+PString(regAccount->registered)+","+PString(state));
  }
  if(account_status_list != list)
  {
    account_status_list = list;
    OpenMCU::Current().ManagerRefreshAddressBook();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PStringArray Registrar::GetAddressBook()
{
  return account_status_list;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::MainLoop()
{
  while(1)
  {
    if(restart)
    {
      restart = 0;
      InitConfig();
      InitTerminals();
    }
    if(terminating)
    {
      return;
    }
    mutex.Wait();
    // Subscribtion
    SubscriptionProcess();
    RefreshAddressBook();
    PTime now;
    // RegistrarAccount
    for(AccountMapType::iterator it=AccountMap.begin(); it!=AccountMap.end(); ++it)
    {
      RegistrarAccount *regAccount = it->second;
      regAccount->Lock();
      // registrar
      if(regAccount->registered)
      {
        if(now > regAccount->start_time + PTimeInterval(regAccount->expires*1000))
          regAccount->registered = FALSE;
      }
      regAccount->Unlock();
    }
    // RegistrarConnection
    for(RegConnMapType::iterator it = RegConnMap.begin(); it != RegConnMap.end(); )
    {
      RegistrarConnection *regConn = it->second;
      regConn->Lock();
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
        if(!ep->HasConnection(regConn->callToken_in))
        {
          if(now > regConn->start_time + PTimeInterval(regConn->accept_timeout*1000))
            regConn->state = CONN_IDLE;
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
      // internal call waiting
      if(regConn->state == CONN_WAIT)
      {
        if(now > regConn->start_time + PTimeInterval(regConn->accept_timeout*1000))
        {
          IncomingCallCancel(regConn);
          OutgoingCallCancel(regConn);
          regConn->state = CONN_END;
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
    mutex.Signal();
    PThread::Sleep(500);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::InitConfig()
{
  internal_room_prefix = "INTERNAL_CALL_";

  MCUConfig cfg("Registrar Parameters");
  registrar_domain = cfg.GetString("Registrar domain", "openmcu-ru");

  // general parameters
  allow_internal_calls = cfg.GetBoolean("Allow internal calls", TRUE);

  // SIP parameters
  sip_require_password = cfg.GetBoolean("SIP proxy required password authorization", FALSE);
  sip_allow_unauth_mcu_calls = cfg.GetBoolean("SIP allow unauthorized MCU calls", TRUE);
  sip_allow_unauth_internal_calls = cfg.GetBoolean("SIP allow unauthorized internal calls", TRUE);

  // H.323 parameters
  h323_require_h235 = cfg.GetBoolean("H.323 gatekeeper required password authorization", FALSE);
  h323_allow_unreg_mcu_calls = cfg.GetBoolean("H.323 allow unregistered MCU calls", TRUE);
  h323_allow_unreg_internal_calls = cfg.GetBoolean("H.323 allow unregistered internal calls", TRUE);
  if(gk) gk->SetRequireH235(h323_require_h235);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::InitTerminals()
{
  MCUConfig cfg("Registrar Parameters");

  PString sipSectionPrefix = "SIP Endpoint ";
  PString h323SectionPrefix = "H323 Endpoint ";

  PStringToString h323Passwords;

  Lock();

  for(AccountMapType::iterator reg = AccountMap.begin(); reg != AccountMap.end(); ++reg)
  {
    RegistrarAccount *regAccount = reg->second;
    regAccount->Lock();
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
    regAccount->Unlock();
  }
  Unlock();

  PStringList sect = cfg.GetSections();
  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    RegAccountTypes account_type;
    if(sect[i].Left(sipSectionPrefix.GetLength()) == sipSectionPrefix)
      account_type = ACCOUNT_TYPE_SIP;
    else if(sect[i].Left(h323SectionPrefix.GetLength()) == h323SectionPrefix)
      account_type = ACCOUNT_TYPE_H323;
    else
      continue;

    PString username;
    if(account_type == ACCOUNT_TYPE_SIP)
      username = sect[i].Right(sect[i].GetLength()-sipSectionPrefix.GetLength());
    else if(account_type == ACCOUNT_TYPE_H323)
      username = sect[i].Right(sect[i].GetLength()-h323SectionPrefix.GetLength());
    if(username == "*" || username == "") continue;

    PConfig scfg(sect[i]);
    PString host = scfg.GetString("Host");
    unsigned port = scfg.GetInteger("Port");

    RegistrarAccount *regAccount = FindAccountWithLock(account_type, username);
    if(!regAccount)
      regAccount = InsertAccountWithLock(account_type, username, host);
    regAccount->enable = scfg.GetBoolean("Registrar", FALSE);
    regAccount->abook = scfg.GetBoolean("Address book", FALSE);
    regAccount->host = host;
    if(port != 0)
      regAccount->port = port;
    regAccount->domain = registrar_domain;
    regAccount->password = scfg.GetString("Password");
    regAccount->display_name = scfg.GetString("Display name override");
    if(account_type == ACCOUNT_TYPE_H323)
      h323Passwords.Insert(PString(username), new PString(regAccount->password));
    regAccount->Unlock();
  }
  // set gatekeeper parameters
  if(gk) gk->SetPasswords(h323Passwords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Registrar::~Registrar()
{
  Lock();
  if(gk)
  {
    delete gk;
    gk = NULL;
  }
  for(AccountMapType::iterator it = AccountMap.begin(); it != AccountMap.end(); )
  {
    RegistrarAccount *regAccount = it->second;
    regAccount->Lock();
    AccountMap.erase(it++);
    delete regAccount;
    regAccount = NULL;
  }
  for(RegConnMapType::iterator it = RegConnMap.begin(); it != RegConnMap.end(); )
  {
    RegistrarConnection *regConn = it->second;
    regConn->Lock();
    RegConnMap.erase(it++);
    delete regConn;
    regConn = NULL;
  }
  for(SubscriptionMapType::iterator it = SubscriptionMap.begin(); it != SubscriptionMap.end(); )
  {
    Subscription *subAccount = it->second;
    subAccount->Lock();
    SubscriptionMap.erase(it++);
    delete subAccount;
    subAccount = NULL;
  }
  Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::Main()
{
  PThread::Sleep(1000);

  nta_agent_set_params(sep->GetAgent(),
                       NTATAG_SIP_T1(1000), // Initial retransmission interval (in milliseconds)
                       //NTATAG_SIP_T2(1000), // Maximum retransmission interval (in milliseconds)
                       NTATAG_SIP_T1X64(3000), // Transaction timeout (defaults to T1 * 64)
                       //NTATAG_TIMER_C(2999), // two invite requests ???
                       //NTATAG_UA(1), // If true, NTA acts as User Agent Server or Client by default
                       TAG_NULL());

  enable_gatekeeper = TRUE;
  if(enable_gatekeeper)
  {
    PIPSocket::Address address("*");
    WORD port = 1719;
    gk = new RegistrarGk(ep, this);
    PString mcuName = OpenMCU::Current().GetName();
    gk->SetGatekeeperIdentifier(mcuName);
    H323Transactor *gkListener = gk->CreateListener(new H323TransportUDP(*ep, address, port, NULL));
    gkListener->StartChannel();
  }

  InitConfig();
  InitTerminals();

  MCUTRACE(0, "Registrar is initialized.");
  MainLoop();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
