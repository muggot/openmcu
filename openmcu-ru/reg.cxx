/*
 * reg.cxx
 *
 * Copyright (C) 2014-2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
 * Copyright (C) 2014-2015 Konstantin Yeliseyev, OpenMCU-ru, All Rights Reserved
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
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

PString RegistrarAccount::GetUrl()
{
  PString url;
  if(account_type == ACCOUNT_TYPE_SIP)
  {
    url = "sip:"+username+"@"+host;
    if(port != 0)
      url += ":"+PString(port);
    if(transport != "" && transport != "*")
      url += ";transport="+transport;
  }
  else if(account_type == ACCOUNT_TYPE_H323)
  {
    url = "h323:"+username+"@"+host;
    if(port != 0)
      url += ":"+PString(port);
  }
  else if(account_type == ACCOUNT_TYPE_RTSP)
  {
    if(username.Left(7) != "rtsp://")
      url += "rtsp://";
    url += username;
  }
  return url;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString AbookAccount::GetUrl()
{
  PString url;
  if(account_type == ACCOUNT_TYPE_SIP)
  {
    url = "sip:"+username+"@"+host;
    if(port != 0)
      url += ":"+PString(port);
    if(transport != "" && transport != "*")
      url += ";transport="+transport;
  }
  else if(account_type == ACCOUNT_TYPE_H323)
  {
    url = "h323:"+username+"@"+host;
    if(port != 0)
      url += ":"+PString(port);
  }
  else if(account_type == ACCOUNT_TYPE_RTSP)
  {
    if(username.Left(7) != "rtsp://")
      url += "rtsp://";
    url += username;
  }
  return url;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void RegistrarAccount::Unlock()
{
  registrar->GetAccountList().Release(id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void AbookAccount::SaveConfig()
{
  MCUConfig scfg("Address book "+username);
  if(!is_abook)
    scfg.DeleteSection();
  else
  {
    scfg.SetString("Scheme", GetSchemeFromAccountType(account_type));
    scfg.SetString(HostKey, host);
    scfg.SetString(DisplayNameKey, display_name);
    if(account_type == ACCOUNT_TYPE_H323)
    {
      if(port != 1720)
        scfg.SetInteger(PortKey, port);
    }
    else if(account_type == ACCOUNT_TYPE_RTSP)
    {
    }
    else if(account_type == ACCOUNT_TYPE_SIP)
    {
      if(port != 5060)
        scfg.SetInteger(PortKey, port);
      if(transport != "*")
        scfg.SetString(TransportKey, transport);
    }
  }

  MCUTRACE(6, "Address book: " << GetUrl() << " save config");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString AbookAccount::AsJsArray(int state)
{
  MCUJSON *json = AsJSON(state);
  std::string str;
  json->ToString(str, false, false);
  delete json;
  return str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * AbookAccount::AsJSON(int state)
{
  PString memberName = display_name+" ["+GetUrl()+"]";
  PString memberNameID = MCUURL(memberName).GetMemberNameId();
  MCUJSON *json = new MCUJSON(MCUJSON::JSON_ARRAY);
  json->Insert("state", state);
  json->Insert("memberNameID", memberNameID);
  json->Insert("memberName", memberName);
  json->Insert("is_abook", is_abook);
  json->Insert("remote_application", remote_application);
  json->Insert("reg_state", reg_state);
  json->Insert("reg_info", reg_info);
  json->Insert("conn_state", conn_state);
  json->Insert("conn_info", conn_info);
  json->Insert("ping_state", ping_state);
  json->Insert("ping_info", ping_info);
  json->Insert("is_account", is_account);
  json->Insert("is_saved_account", is_saved_account);
  return json;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void AbookAccount::SendRoomControl(int state)
{
  PStringStream msg;
  msg << "abook_change(" << AsJsArray(state) << ");";
  ConferenceManager *cm = OpenMCU::Current().GetConferenceManager();
  MCUConferenceList & conferenceList = cm->GetConferenceList();
  for(MCUConferenceList::shared_iterator r = conferenceList.begin(); r != conferenceList.end(); ++r)
    OpenMCU::Current().HttpWriteCmdRoom(msg, r->GetNumber());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void RegistrarConnection::Unlock()
{
  registrar->GetConnectionList().Release(id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void RegistrarSubscription::Unlock()
{
  registrar->GetSubscriptionList().Release(id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RegistrarAccount * Registrar::InsertAccountWithLock(RegAccountTypes account_type, const PString & username)
{
  long id = accountList.GetNextID();
  RegistrarAccount *raccount = new RegistrarAccount(this, id, account_type, username);
  PString name = PString(raccount->account_type)+":"+raccount->username;
  MCURegistrarAccountList::shared_iterator it = accountList.Insert(raccount, id, name);
  return it.GetCapturedObject();
}

RegistrarAccount * Registrar::FindAccountWithLock(RegAccountTypes account_type, const PString & username)
{
  for(MCURegistrarAccountList::shared_iterator it = accountList.begin(); it != accountList.end(); ++it)
  {
    if(it->username == username && (account_type == ACCOUNT_TYPE_UNKNOWN || account_type == it->account_type))
    {
      return it.GetCapturedObject();
    }
  }
  return NULL;
}

PString Registrar::FindAccountNameFromH323Id(const PString & h323id)
{
  for(MCURegistrarAccountList::shared_iterator it = accountList.begin(); it != accountList.end(); ++it)
  {
    if(it->h323id == h323id)
      return it->username;
  }
  return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RegistrarSubscription * Registrar::InsertSubWithLock(const PString & username_in, const PString & username_out)
{
  long id = accountList.GetNextID();
  RegistrarSubscription *rsub = new RegistrarSubscription(this, id, username_in, username_out);
  MCURegistrarSubscriptionList::shared_iterator it = subscriptionList.Insert(rsub, id, rsub->username_pair);
  return it.GetCapturedObject();
}

RegistrarSubscription *Registrar::FindSubWithLock(const PString & username_pair)
{
  MCURegistrarSubscriptionList::shared_iterator it = subscriptionList.Find(username_pair);
  if(it != subscriptionList.end())
  {
    return it.GetCapturedObject();
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RegistrarConnection * Registrar::InsertRegConnWithLock(const PString & callToken, const PString & username_in, const PString & username_out)
{
  long id = connectionList.GetNextID();
  RegistrarConnection *rconn = new RegistrarConnection(this, id, callToken, username_in, username_out);
  MCURegistrarConnectionList::shared_iterator it = connectionList.Insert(rconn, id, callToken);
  return it.GetCapturedObject();
}

RegistrarConnection * Registrar::FindRegConnWithLock(const PString & callToken)
{
  for(MCURegistrarConnectionList::shared_iterator it = connectionList.begin(); it != connectionList.end(); ++it)
  {
    if(it->callToken_in == callToken || it->callToken_out == callToken)
    {
      return it.GetCapturedObject();
    }
  }
  return NULL;
}

RegistrarConnection * Registrar::FindRegConnWithLock(RegAccountTypes account_type, const PString & username)
{
  for(MCURegistrarConnectionList::shared_iterator it = connectionList.begin(); it != connectionList.end(); ++it)
  {
    if((it->account_type_in == account_type && it->username_in == username) ||
       (it->account_type_out == account_type && it->username_out == username))
      return it.GetCapturedObject();
  }
  return NULL;
}

bool Registrar::HasRegConn(const PString & callToken)
{
  MCURegistrarConnectionList::shared_iterator it = connectionList.Find(callToken);
  if(it != connectionList.end())
    return true;
  return false;
}

bool Registrar::HasRegConn(RegAccountTypes account_type, const PString & username)
{
  for(MCURegistrarConnectionList::shared_iterator it = connectionList.begin(); it != connectionList.end(); ++it)
  {
    if((it->account_type_in == account_type && it->username_in == username) ||
       (it->account_type_out == account_type && it->username_out == username))
      return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ConnectionCreated(const PString & callToken)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ConnectionEstablished(const PString & callToken)
{
  PString *cmd = new PString("established:"+callToken);
  regQueue.Push(cmd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ConnectionCleared(const PString & callToken)
{
  PString *cmd = new PString("cleared:"+callToken);
  regQueue.Push(cmd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::SetRequestedRoom(const PString & callToken, PString & requestedRoom)
{
  RegistrarConnection *rconn = FindRegConnWithLock(callToken);
  if(rconn)
  {
    if(rconn->roomname != "")
      requestedRoom = rconn->roomname;
    rconn->Unlock();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::AddAbookAccount(const PString & address)
{
  MCUURL url(address);
  RegAccountTypes account_type;
  if(url.GetScheme() == "h323")
    account_type = ACCOUNT_TYPE_H323;
  else if(url.GetScheme() == "rtsp")
  {
    account_type = ACCOUNT_TYPE_RTSP;
    url.SetUserName(url.GetUrl());
  }
  else if(url.GetScheme() == "sip")
    account_type = ACCOUNT_TYPE_SIP;
  else
    return FALSE;

  if(url.GetUserName() == "")
    return FALSE;

  PWaitAndSignal m(mutex);

  PString key = PString(account_type)+":"+url.GetUserName();
  MCUAbookList::shared_iterator it = abookList.Find(key);
  if(it != abookList.end())
  {
    if(!it->is_abook)
    {
      it->is_abook = true;
      it->SaveConfig();
      it->SendRoomControl();
    }
    return TRUE;
  }

  AbookAccount *ab = new AbookAccount();
  ab->is_abook = true;
  ab->account_type = account_type;
  ab->username = url.GetUserName();
  ab->host = url.GetHostName();
  ab->port = url.GetPort().AsInteger();
  ab->transport = url.GetTransport();
  ab->display_name = url.GetDisplayName();
  ab->SaveConfig();
  ab->SendRoomControl();

  long id = abookList.GetNextID();
  abookList.Insert(ab, id, key);

  MCUTRACE(6, "Address book: " << address << " insert");
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::RemoveAbookAccount(const PString & address)
{
  MCUURL url(address);
  RegAccountTypes account_type;
  if(url.GetScheme() == "h323")
    account_type = ACCOUNT_TYPE_H323;
  else if(url.GetScheme() == "rtsp")
  {
    account_type = ACCOUNT_TYPE_RTSP;
    url.SetUserName(url.GetUrl());
  }
  else if(url.GetScheme() == "sip")
    account_type = ACCOUNT_TYPE_SIP;
  else
    return FALSE;

  if(url.GetUserName() == "")
    return FALSE;

  PWaitAndSignal m(mutex);

  PString key = PString(account_type)+":"+url.GetUserName();
  MCUAbookList::shared_iterator it = abookList.Find(key);
  if(it == abookList.end())
    return FALSE;

  AbookAccount *ab = *it;
  ab->is_abook = false;
  ab->SaveConfig();
  ab->SendRoomControl(2);
  if(abookList.Erase(it))
    delete ab;

  MCUTRACE(6, "Address book: " << address << " remove");
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::MakeCall(const PString & room, const PString & to, PString & callToken)
{
  // the default protocol H.323
  // correct formats - proto:username, proto:username@, proto:ip, proto:@ip
  // wrong formats - proto:@username, proto:ip@

  PWaitAndSignal m(mutex);

  MCUURL url(to);
  PString address = url.GetUrl();

  RegAccountTypes account_type = ACCOUNT_TYPE_UNKNOWN;
  if(url.GetScheme() == "sip")
    account_type = ACCOUNT_TYPE_SIP;
  else if(url.GetScheme() == "h323")
    account_type = ACCOUNT_TYPE_H323;
  else if(url.GetScheme() == "rtsp")
  {
    account_type = ACCOUNT_TYPE_RTSP;
    url.SetUserName(url.GetUrl());
  }
  else
    return FALSE;

  PString username_out;
  if(url.GetUserName() != "")
    username_out = url.GetUserName();
  else if(url.GetHostName() != "")
    username_out = url.GetHostName();
  else
    return FALSE;

  RegistrarAccount *raccount_out = NULL;
  RegistrarConnection *rconn = NULL;

  raccount_out = FindAccountWithLock(account_type, username_out);
  if(raccount_out)
  {
    // update address from account
    address = raccount_out->GetUrl();
    raccount_out->Unlock();
    // update url
    url = MCUURL(address);
    PTRACE(1, "Registrar MakeCall: found the account, changed address " << address);
  } else {
    raccount_out = InsertAccountWithLock(account_type, username_out);
    raccount_out->host = url.GetHostName();
    raccount_out->port = url.GetPort().AsInteger();
    raccount_out->transport = url.GetTransport();
    raccount_out->display_name = url.GetDisplayName();
    PTRACE(1, "Registrar MakeCall: create new account " << raccount_out->GetUrl());
    raccount_out->Unlock();
  }

  if(account_type == ACCOUNT_TYPE_SIP)
  {
    callToken = PGloballyUniqueID().AsString();
    PString *cmd = new PString("invite:"+room+","+address+","+callToken);
    sep->GetSipQueue().Push(cmd);
  }
  else if(account_type == ACCOUNT_TYPE_H323)
  {
    // Звонок по IP адресу
    if(url.GetHostName() == "")
    {
      address = url.GetUserName();
      PTRACE(1, "Registrar MakeCall: changed address " << address);
    }
    // gatekeeper
    H323Transport * transport = NULL;
    if(ep->GetGatekeeper())
    {
      PString gk_host = ep->GetGatekeeperHostName();
      if(url.GetHostName() == "" || gk_host == url.GetHostName())
      {
        address = url.GetUserName();
        PTRACE(1, "Registrar MakeCall: changed address " << address);
      }
      else
      {
        H323TransportAddress taddr(url.GetHostName()+":"+url.GetPort());
        transport = taddr.CreateTransport(*ep);
        transport->SetRemoteAddress(taddr);
        PTRACE(1, "Registrar MakeCall: use transport " << taddr);
      }
    }
    void *userData = new PString(room);
    if(!ep->MakeCall(address, transport, callToken, userData))
      callToken = "";
  }
  else if(account_type == ACCOUNT_TYPE_RTSP)
  {
    callToken = PGloballyUniqueID().AsString();
    PString *cmd = new PString("invite:"+room+","+address+","+callToken);
    regQueue.Push(cmd);
  }

  if(callToken != "")
  {
    rconn = InsertRegConnWithLock(callToken, room, username_out);
    rconn->account_type_out = account_type;
    rconn->callToken_out = callToken;
    rconn->roomname = room;
    rconn->state = CONN_MCU_WAIT;
    rconn->Unlock();
  }

  if(callToken == "")
    return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::InternalMakeCall(RegistrarConnection *rconn, const PString & username_in, const PString & username_out)
{
  RegistrarAccount *raccount_in = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_in);
  RegistrarAccount *raccount_out = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_out);
  BOOL ret = InternalMakeCall(rconn, raccount_in, raccount_out);

  if(raccount_in) raccount_in->Unlock();
  if(raccount_out) raccount_out->Unlock();
  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::InternalMakeCall(RegistrarConnection *rconn, RegistrarAccount *raccount_in, RegistrarAccount *raccount_out)
{
  if(!rconn || !raccount_in || !raccount_out)
    return FALSE;

  rconn->account_type_in = raccount_in->account_type;
  rconn->account_type_out = raccount_out->account_type;

  PString address = raccount_out->GetUrl();

  if(raccount_out->account_type == ACCOUNT_TYPE_SIP)
  {
    PString callToken = PGloballyUniqueID().AsString();
    PString *cmd = new PString("invite:"+rconn->username_in+","+address+","+callToken);
    sep->GetSipQueue().Push(cmd);
    rconn->callToken_out = callToken;
    return TRUE;
  }
  else if(raccount_out->account_type == ACCOUNT_TYPE_H323)
  {
    PString callToken_out;
    void *userData = new PString(rconn->username_in);
    ep->MakeCall(address, callToken_out, userData);
    if(callToken_out != "")
    {
      rconn->callToken_out = callToken_out;
      return TRUE;
    }
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::IncomingCallAccept(RegistrarConnection *rconn)
{
  if(rconn->account_type_in == ACCOUNT_TYPE_SIP)
  {
    msg_t *msg = rconn->GetInviteMsgCopy();
    sep->CreateIncomingConnection(msg);
    msg_destroy(msg);
  }
  else if(rconn->account_type_in == ACCOUNT_TYPE_H323)
  {
    MCUH323Connection *conn = ep->FindConnectionWithLock(rconn->callToken_in);
    if(conn)
    {
      conn->AnsweringCall(H323Connection::AnswerCallNow);
      conn->Unlock();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::IncomingCallCancel(RegistrarConnection *rconn)
{
  if(rconn->callToken_in != "")
    Leave(rconn->account_type_in, rconn->callToken_in);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::OutgoingCallCancel(RegistrarConnection *rconn)
{
  if(rconn->callToken_out != "")
    Leave(rconn->account_type_out, rconn->callToken_out);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::Leave(int account_type, const PString & callToken)
{
  MCUH323Connection *conn = NULL;
  if(account_type == ACCOUNT_TYPE_SIP)
    conn = (MCUSipConnection *)ep->FindConnectionWithLock(callToken);
  else if(account_type == ACCOUNT_TYPE_H323)
    conn = ep->FindConnectionWithLock(callToken);
  if(conn)
  {
    conn->ClearCall();
    conn->Unlock();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::AccountThread(PThread &, INT)
{
  while(!terminating)
  {
    MCUTime::Sleep(100);
    if(terminating)
      break;

    PTime now;
    for(MCURegistrarAccountList::shared_iterator it = accountList.begin(); it != accountList.end(); ++it)
    {
      RegistrarAccount *raccount = *it;
      // expires
      if(raccount->registered)
      {
        if(now > raccount->start_time + PTimeInterval(raccount->expires*1000))
          raccount->registered = FALSE;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::ConnectionThread(PThread &, INT)
{
  while(!terminating)
  {
    MCUTime::Sleep(100);
    if(terminating)
      break;

    PTime now;
    for(MCURegistrarConnectionList::shared_iterator it = connectionList.begin(); it != connectionList.end(); ++it)
    {
      RegistrarConnection *rconn = *it;
      // remove empty connection
      if(rconn->state == CONN_IDLE)
      {
        if(connectionList.Erase(it))
          delete rconn;
        continue;
      }
      // MCU call answer limit
      if(rconn->state == CONN_MCU_WAIT)
      {
        if(now > rconn->start_time + PTimeInterval(rconn->accept_timeout*1000))
        {
          OutgoingCallCancel(rconn);
          rconn->state = CONN_END;
        }
      }
      // internal call answer limit
      if(rconn->state == CONN_WAIT)
      {
        if(now > rconn->start_time + PTimeInterval(rconn->accept_timeout*1000))
        {
          IncomingCallCancel(rconn);
          OutgoingCallCancel(rconn);
          rconn->state = CONN_END;
        }
      }
      // make internal call
      if(rconn->state == CONN_WAIT)
      {
        if(rconn->callToken_out == "")
        {
          if(!InternalMakeCall(rconn, rconn->username_in, rconn->username_out))
          {
            rconn->state = CONN_CANCEL_IN;
          }
        }
      }
      // accept incoming
      if(rconn->state == CONN_ACCEPT_IN)
      {
        IncomingCallAccept(rconn);
        rconn->state = CONN_ESTABLISHED;
      }
      // cancel incoming
      if(rconn->state == CONN_CANCEL_IN)
      {
        IncomingCallCancel(rconn);
        rconn->state = CONN_END;
      }
      // cancel outgoing
      if(rconn->state == CONN_CANCEL_OUT)
      {
        OutgoingCallCancel(rconn);
        rconn->state = CONN_END;
      }
      // leave incoming
      if(rconn->state == CONN_LEAVE_IN)
      {
        Leave(rconn->account_type_in, rconn->callToken_in);
        rconn->state = CONN_END;
      }
      // leave outgoing
      if(rconn->state == CONN_LEAVE_OUT)
      {
        Leave(rconn->account_type_out, rconn->callToken_out);
        rconn->state = CONN_END;
      }
      // internal call end
      if(rconn->state == CONN_END)
      {
        rconn->state = CONN_IDLE;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::BookThread(PThread &, INT)
{
  while(!terminating)
  {
    MCUTime::Sleep(1000);
    if(terminating)
      break;

    for(MCURegistrarAccountList::shared_iterator r = accountList.begin(); r != accountList.end(); ++r)
    {
      RegistrarAccount *raccount = *r;

      PString key = PString(raccount->account_type)+":"+raccount->username;
      MCUAbookList::shared_iterator s = abookList.Find(key);
      if(s != abookList.end())
        continue;

      AbookAccount *ab = new AbookAccount();
      ab->is_abook = false;
      ab->is_account = true;
      ab->is_saved_account = raccount->is_saved_account;
      ab->account_type = raccount->account_type;
      ab->username = raccount->username;
      ab->host = raccount->host;
      ab->port = raccount->port;
      ab->transport = raccount->transport;
      if(raccount->display_name_saved != "")
        ab->display_name = raccount->display_name_saved;
      else
        ab->display_name = raccount->display_name;
      ab->remote_application = raccount->remote_application;
      ab->SendRoomControl();

      long id = abookList.GetNextID();
      abookList.Insert(ab, id, key);
    }

    AbookAccount oldab;
    for(MCUAbookList::shared_iterator r = abookList.begin(); r != abookList.end(); ++r)
    {
      AbookAccount *ab = *r;
      oldab.Clear();
      oldab.Set(*ab);

      MCURegistrarAccountList::shared_iterator s = accountList.Find(PString(r->account_type)+":"+r->username);
      if(s == accountList.end())
      {
        ab->is_account = false;
        ab->is_saved_account = false;
        if(oldab.AsJsArray() != ab->AsJsArray())
          ab->SendRoomControl();
        continue;
      }
      RegistrarAccount *raccount = *s;

      ab->is_account = true;
      ab->is_saved_account = raccount->is_saved_account;
      ab->host = raccount->host;
      ab->port = raccount->port;
      ab->transport = raccount->transport;
      if(raccount->display_name_saved != "")
        ab->display_name = raccount->display_name_saved;
      else
        ab->display_name = raccount->display_name;
      ab->remote_application = raccount->remote_application;
      ab->reg_state = 0;
      ab->reg_info = "";
      ab->conn_state = 0;
      ab->conn_info = "";
      ab->ping_state = 0;
      ab->ping_info = "";

      if(raccount->is_saved_account)
        ab->reg_state = 1;
      if(raccount->registered)
        ab->reg_state = 2;
      if(ab->reg_state != 0 && raccount->start_time != PTime(0))
        ab->reg_info = raccount->start_time.AsString("hh:mm:ss dd.MM.yyyy");

      RegistrarConnection *rconn = FindRegConnWithLock(raccount->account_type, raccount->username);
      if(rconn)
      {
        if(rconn->state == CONN_WAIT || rconn->state == CONN_MCU_WAIT)
          ab->conn_state = 1;
        else if(rconn->state == CONN_ESTABLISHED || rconn->state == CONN_MCU_ESTABLISHED)
        {
          ab->conn_state = 2;
          ab->conn_info = rconn->start_time.AsString("hh:mm:ss dd.MM.yyyy");
        }
        rconn->Unlock();
      }

      if(raccount->keep_alive_enable)
      {
        if(raccount->keep_alive_time_response > raccount->keep_alive_time_request-PTimeInterval(raccount->keep_alive_interval*1000-2000))
          ab->ping_state = 1;
        else
          ab->ping_state = 2;
        ab->ping_info = raccount->keep_alive_time_response.AsString("hh:mm:ss dd.MM.yyyy");
      }

      if(ab->AsJsArray() != oldab.AsJsArray())
        ab->SendRoomControl();

    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::SubscriptionThread(PThread &, INT)
{
  while(!terminating)
  {
    MCUTime::Sleep(1000);
    if(terminating)
      break;

    PTime now;
    for(MCURegistrarSubscriptionList::shared_iterator it = subscriptionList.begin(); it != subscriptionList.end(); ++it)
    {
      RegistrarSubscription *rsub = *it;
      RegSubscriptionStates state_new;
      if(now > rsub->start_time + PTimeInterval(rsub->expires*1000))
      {
        if(subscriptionList.Erase(it))
          delete rsub;
        continue;
      }

      RegistrarAccount *raccount_out = FindAccountWithLock(ACCOUNT_TYPE_SIP, rsub->username_out);
      if(!raccount_out)
        raccount_out = FindAccountWithLock(ACCOUNT_TYPE_H323, rsub->username_out);

      if(raccount_out && raccount_out->registered)
      {
        state_new = SUB_STATE_OPEN;
        if(HasRegConn(raccount_out->account_type, raccount_out->username))
          state_new = SUB_STATE_BUSY;
      } else {
        state_new = SUB_STATE_CLOSED;
      }

      // send notify
      if(rsub->state != state_new)
      {
        rsub->state = state_new;
        SipSendNotify(rsub);
      }
      if(raccount_out) raccount_out->Unlock();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::AliveThread(PThread &, INT)
{
  while(!terminating)
  {
    MCUTime::Sleep(1000);
    if(terminating)
      break;

    PTime now;
    for(MCURegistrarAccountList::shared_iterator it = accountList.begin(); it != accountList.end(); ++it)
    {
      RegistrarAccount *raccount = *it;
      if(raccount->keep_alive_enable && now > raccount->keep_alive_time_request+PTimeInterval(raccount->keep_alive_interval*1000))
      {
        if(raccount->account_type == ACCOUNT_TYPE_H323)
        {
          raccount->keep_alive_time_request = PTime();
          if(raccount->host == "" || raccount->port == 0)
            continue;
          H323TransportTCP transport(OpenMCU::Current().GetEndpoint());
          if(!transport.SetRemoteAddress(raccount->host+":"+PString(raccount->port)))
            continue;
          PBYTEArray rawData;
          if(transport.Connect() && transport.WritePDU(rawData))
            raccount->keep_alive_time_response = raccount->keep_alive_time_request;
        }
        else if(raccount->account_type == ACCOUNT_TYPE_SIP)
        {
          raccount->keep_alive_time_request = now;
          SipSendPing(raccount);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::QueueThread(PThread &, INT)
{
  while(!terminating)
  {
    MCUTime::Sleep(100);
    if(terminating)
      break;

    while(1)
    {
      PString *cmd = regQueue.Pop();
      if(cmd == NULL)
        break;
      if(cmd->Left(7) == "invite:")
        QueueInvite(cmd->Right(cmd->GetLength()-7));
      else if(cmd->Left(12) == "established:")
        QueueEstablished(cmd->Right(cmd->GetLength()-12));
      else if(cmd->Left(8) == "cleared:")
        QueueCleared(cmd->Right(cmd->GetLength()-8));
      delete cmd;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::QueueInvite(const PString & data)
{
  PString from = data.Tokenise(",")[0];
  PString address = data.Tokenise(",")[1];
  PString callToken = data.Tokenise(",")[2];
  MCUURL url(address);
  if(url.GetScheme() == "h323")
  {
  }
  else if(url.GetScheme() == "rtsp")
  {
    MCURtspServer *rtsp = OpenMCU::Current().GetRtspServer();
    rtsp->CreateConnection(from, address, callToken);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::QueueEstablished(const PString & data)
{
  PString callToken = data;
  RegistrarConnection *rconn = FindRegConnWithLock(callToken);
  if(rconn == NULL)
    return;

  // update account data ???
  RegistrarAccount *raccount = NULL;
  if(rconn->callToken_out == callToken)
    raccount = FindAccountWithLock(rconn->account_type_out, rconn->username_out);
  if(raccount)
  {
    if(!raccount->registered)
    {
      MCUH323Connection *conn = ep->FindConnectionWithLock(callToken);
      if(conn)
      {
        MCUURL url(conn->GetMemberName());
        raccount->host = url.GetHostName();
        raccount->port = url.GetPort().AsInteger();
        raccount->transport = url.GetTransport();
        raccount->display_name = url.GetDisplayName();
        raccount->remote_application = conn->GetRemoteApplication();
        conn->Unlock();
      }
    }
    raccount->Unlock();
  }

  if(rconn->state == CONN_MCU_WAIT)
    rconn->state = CONN_MCU_ESTABLISHED;
  if(rconn->state == CONN_WAIT && rconn->callToken_out == callToken)
    rconn->state = CONN_ACCEPT_IN;
  rconn->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::QueueCleared(const PString & data)
{
  PString callToken = data;
  RegistrarConnection *rconn = FindRegConnWithLock(callToken);
  if(rconn == NULL)
    return;
  if(rconn->state == CONN_MCU_WAIT || rconn->state == CONN_MCU_ESTABLISHED)
    rconn->state = CONN_IDLE;
  if(rconn->state == CONN_WAIT && rconn->callToken_out == callToken)
    rconn->state = CONN_CANCEL_IN;
  if(rconn->state == CONN_WAIT && rconn->callToken_in == callToken)
    rconn->state = CONN_CANCEL_OUT;
  if(rconn->state == CONN_ESTABLISHED && rconn->callToken_out == callToken)
    rconn->state = CONN_LEAVE_IN;
  if(rconn->state == CONN_ESTABLISHED && rconn->callToken_in == callToken)
    rconn->state = CONN_LEAVE_OUT;
  if(rconn->state == CONN_ACCEPT_IN && rconn->callToken_out == callToken)
    rconn->state = CONN_LEAVE_IN;
  if(rconn->state == CONN_ACCEPT_IN && rconn->callToken_in == callToken)
    rconn->state = CONN_LEAVE_OUT;
  rconn->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::InitConfig()
{
  MCUConfig cfg("Registrar Parameters");

  // general parameters
  registrar_domain = cfg.GetString("Registrar domain", PRODUCT_NAME_TEXT);
  allow_internal_calls = cfg.GetBoolean("Allow internal calls", TRUE);

  enable_gatekeeper = cfg.GetBoolean("H.323 gatekeeper enable", TRUE);
  if(enable_gatekeeper && registrarGk == NULL)
  {
    PIPSocket::Address address("*");
    WORD port = 1719;
    registrarGk = new RegistrarGk(ep, this);
    PString mcuName = OpenMCU::Current().GetName();
    registrarGk->SetGatekeeperIdentifier(mcuName);
    registrarGk->CreateListener(new H323TransportUDP(*ep, address, port, 0));
  }
  if(!enable_gatekeeper && registrarGk != NULL)
  {
    delete registrarGk;
    registrarGk = NULL;
  }

  // SIP parameters
  sip_allow_unauth_reg = cfg.GetBoolean("SIP allow registration without authentication", TRUE);
  sip_allow_unauth_mcu_calls = cfg.GetBoolean("SIP allow MCU calls without authentication", TRUE);
  sip_allow_unauth_internal_calls = cfg.GetBoolean("SIP allow internal calls without authentication", TRUE);
  sip_reg_min_expires = cfg.GetInteger("SIP registrar minimum expiration", 60);
  sip_reg_max_expires = cfg.GetInteger("SIP registrar maximum expiration", 600);

  // H.323 parameters
  h323_allow_unauth_reg = cfg.GetBoolean("H.323 allow registration without authentication", TRUE);
  h323_allow_unreg_mcu_calls = cfg.GetBoolean("H.323 allow MCU calls without registration", TRUE);
  h323_allow_unreg_internal_calls = cfg.GetBoolean("H.323 allow internal calls without registration", TRUE);
  h323_min_time_to_live = cfg.GetInteger("H.323 gatekeeper minimum Time To Live", 60);
  h323_max_time_to_live = cfg.GetInteger("H.323 gatekeeper maximum Time To Live", 600);
  BOOL h323_allow_duplicate_aliases = cfg.GetBoolean("H.323 allow duplicate aliases", FALSE);
  if(registrarGk)
  {
    registrarGk->SetRequireH235(!h323_allow_unauth_reg);
    registrarGk->SetMinTimeToLive(h323_min_time_to_live);
    registrarGk->SetMaxTimeToLive(h323_max_time_to_live);
    registrarGk->SetTimeToLive(h323_max_time_to_live);
    registrarGk->SetAllowDuplicateAlias(h323_allow_duplicate_aliases);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::InitAbook()
{
  if(abookList.GetSize() > 0)
    return;

  PStringList sect = MCUConfig().GetSections();
  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    if(sect[i].Find("Address book ") != 0)
      continue;

    MCUConfig scfg(sect[i]);
    PString username = sect[i].Right(sect[i].GetLength()-PString("Address book ").GetLength());
    PString host = scfg.GetString(HostKey);
    RegAccountTypes account_type = GetAccountTypeFromScheme(scfg.GetString("Scheme"));

    if(username == "" || account_type == ACCOUNT_TYPE_UNKNOWN)
      continue;

    AbookAccount *ab = new AbookAccount();
    ab->is_abook = true;
    ab->account_type = account_type;
    ab->username = username;
    ab->host = host;
    ab->port = scfg.GetInteger(PortKey);
    ab->transport = scfg.GetString(TransportKey);
    ab->display_name = scfg.GetString(DisplayNameKey);

    PString key = PString(ab->account_type)+":"+ab->username;
    long id = abookList.GetNextID();
    abookList.Insert(ab, id, key);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::InitAccounts()
{
  PWaitAndSignal m(mutex);

  MCUConfig cfg("Registrar Parameters");

  PString h323SectionPrefix = "H323 Endpoint ";
  PString rtspSectionPrefix = "RTSP Endpoint ";
  PString sipSectionPrefix = "SIP Endpoint ";

  PStringToString h323Passwords;

  for(MCURegistrarAccountList::shared_iterator it = accountList.begin(); it != accountList.end(); ++it)
  {
    RegistrarAccount *raccount = *it;
    PString sectionPrefix;
    if(raccount->account_type == ACCOUNT_TYPE_H323)
      sectionPrefix = h323SectionPrefix;
    else if(raccount->account_type == ACCOUNT_TYPE_RTSP)
      sectionPrefix = rtspSectionPrefix;
    else if(raccount->account_type == ACCOUNT_TYPE_SIP)
      sectionPrefix = sipSectionPrefix;

    if(MCUConfig::HasSection(sectionPrefix+raccount->username))
      raccount->is_saved_account = TRUE;
    else
    {
      raccount->is_saved_account = FALSE;
      raccount->display_name_saved = "";
      raccount->auth.password = "";
      raccount->sip_call_processing = MCUConfig(sectionPrefix+"*").GetString("SIP call processing", "redirect");
      raccount->h323_call_processing = MCUConfig(sectionPrefix+"*").GetString("H.323 call processing", "direct");
      raccount->keep_alive_enable = FALSE;
      if((raccount->account_type == ACCOUNT_TYPE_SIP && !sip_allow_unauth_reg) || (raccount->account_type == ACCOUNT_TYPE_H323 && !h323_allow_unauth_reg))
        raccount->registered = FALSE;
    }
  }

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

    if(username == "*" || username == "" || username == "empty")
      continue;

    unsigned port = scfg.GetInteger(PortKey);
    if(port == 0) port = gcfg.GetInteger(PortKey);

    RegistrarAccount *raccount = FindAccountWithLock(account_type, username);
    if(!raccount)
      raccount = InsertAccountWithLock(account_type, username);
    if(!raccount)
      continue;
    raccount->is_saved_account = TRUE;
    raccount->host = scfg.GetString(HostKey);
    if(port != 0)
      raccount->port = port;
    raccount->transport = scfg.GetString(TransportKey);
    raccount->domain = registrar_domain;
    raccount->auth.password = scfg.GetString(PasswordKey);
    raccount->display_name_saved = scfg.GetString(DisplayNameKey);
    // keep alive
    raccount->keep_alive_interval = scfg.GetString(PingIntervalKey).AsInteger();
    if(raccount->keep_alive_interval == 0)
      raccount->keep_alive_interval = gcfg.GetString(PingIntervalKey).AsInteger();
    if(raccount->keep_alive_interval != 0)
      raccount->keep_alive_enable = TRUE;
    else
      raccount->keep_alive_enable = FALSE;
    // sip call processing
    raccount->sip_call_processing = scfg.GetString("SIP call processing");
    if(raccount->sip_call_processing == "")
      raccount->sip_call_processing = gcfg.GetString("SIP call processing", "redirect");
    // h323 call processing
    raccount->h323_call_processing = scfg.GetString("H.323 call processing");
    if(raccount->h323_call_processing == "")
      raccount->h323_call_processing = gcfg.GetString("H.323 call processing", "direct");
    // password
    if(account_type == ACCOUNT_TYPE_H323)
      h323Passwords.Insert(PString(username), new PString(raccount->auth.password));
    raccount->Unlock();
  }
  // set gatekeeper parameters
  if(registrarGk)
    registrarGk->SetPasswords(h323Passwords);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::Terminating()
{
  // delete gatekeeper
  if(registrarGk)
  {
    delete registrarGk;
    registrarGk = NULL;
  }

  // stop alive refresh
  if(aliveThread)
  {
    PTRACE(5, trace_section << "Waiting for termination alive thread: " << aliveThread->GetThreadName());
    aliveThread->WaitForTermination();
    delete aliveThread;
    aliveThread = NULL;
  }

  // stop abook refresh
  if(bookThread)
  {
    PTRACE(5, trace_section << "Waiting for termination book thread: " << bookThread->GetThreadName());
    bookThread->WaitForTermination();
    delete bookThread;
    bookThread = NULL;
  }
  for(MCUAbookList::shared_iterator it = abookList.begin(); it != abookList.end(); ++it)
  {
    AbookAccount *acc = *it;
    if(abookList.Erase(it))
      delete acc;
  }

  // stop subscriptions refresh
  if(subscriptionThread)
  {
    PTRACE(5, trace_section << "Waiting for termination book thread: " << connectionThread->GetThreadName());
    subscriptionThread->WaitForTermination();
    delete subscriptionThread;
    subscriptionThread = NULL;
  }

  // stop queue thread
  if(queueThread)
  {
    PTRACE(5, trace_section << "Waiting for termination queue thread: " << queueThread->GetThreadName());
    queueThread->WaitForTermination();
    delete queueThread;
    queueThread = NULL;
  }

  // stop accounts refresh
  if(accountThread)
  {
    PTRACE(5, trace_section << "Waiting for termination accounts thread: " << accountThread->GetThreadName());
    accountThread->WaitForTermination();
    delete accountThread;
    accountThread = NULL;
  }

  // stop connections refresh
  if(connectionThread)
  {
    PTRACE(5, trace_section << "Waiting for termination book thread: " << connectionThread->GetThreadName());
    connectionThread->WaitForTermination();
    delete connectionThread;
    connectionThread = NULL;
  }

  for(MCURegistrarAccountList::shared_iterator it = accountList.begin(); it != accountList.end(); ++it)
  {
    RegistrarAccount *raccount = *it;
    if(accountList.Erase(it))
      delete raccount;
  }
  for(MCURegistrarConnectionList::shared_iterator it = connectionList.begin(); it != connectionList.end(); ++it)
  {
    RegistrarConnection *rconn = *it;
    if(connectionList.Erase(it))
      delete rconn;
  }
  for(MCURegistrarSubscriptionList::shared_iterator it = subscriptionList.begin(); it != subscriptionList.end(); ++it)
  {
    RegistrarSubscription *rsub = *it;
    if(subscriptionList.Erase(it))
      delete rsub;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::MainLoop()
{
  InitAbook();
  while(1)
  {
    MCUTime::Sleep(100);
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
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Registrar::Main()
{
  accountThread = PThread::Create(PCREATE_NOTIFIER(AccountThread), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "registrar account:%0x");
  connectionThread = PThread::Create(PCREATE_NOTIFIER(ConnectionThread), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "registrar connection:%0x");
  subscriptionThread = PThread::Create(PCREATE_NOTIFIER(SubscriptionThread), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "registrar subscription:%0x");
  queueThread = PThread::Create(PCREATE_NOTIFIER(QueueThread), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "registrar queue:%0x");
  aliveThread = PThread::Create(PCREATE_NOTIFIER(AliveThread), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "registrar alive:%0x");
  bookThread = PThread::Create(PCREATE_NOTIFIER(BookThread), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "registrar book:%0x");

  MainLoop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
