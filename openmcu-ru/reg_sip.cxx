/*
 * reg_sip.cxx
 *
 * Copyright (C) 2014-2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
 * Copyright (C) 2014 Konstantin Yeliseyev, OpenMCU-ru, All Rights Reserved
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

int Registrar::OnReceivedSipRegister(const msg_t *msg)
{
  PTRACE(1, trace_section << "OnReceivedSipRegister");

  PWaitAndSignal m(mutex);

  sip_t *sip = sip_object(msg);
  MCUURL_SIP url(msg, DIRECTION_INBOUND);
  PString username = url.GetUserName();
  msg_t *msg_reply = nta_msg_create(sep->GetAgent(), 0);

  RegistrarAccount *raccount = NULL;

  raccount = FindAccountWithLock(ACCOUNT_TYPE_SIP, username);
  if(!raccount && sip_allow_unauth_reg)
    raccount = InsertAccountWithLock(ACCOUNT_TYPE_SIP, username);

  int response_code = SipPolicyCheck(msg, msg_reply, raccount, NULL);
  if(response_code)
    goto return_response;

  {
    raccount->registered = TRUE;
    raccount->start_time = PTime();
    // update account data
    raccount->host = url.GetHostName();
    raccount->domain = url.GetDomainName();
    raccount->port = atoi(url.GetPort());
    raccount->transport = url.GetTransport();
    raccount->display_name = url.GetDisplayName();
    raccount->remote_application = url.GetRemoteApplication();
    // save register message
    raccount->SetRegisterMsg(msg);
    // expires
    raccount->expires = sip_reg_max_expires;
    if(sip->sip_expires)
      raccount->expires = sip->sip_expires->ex_delta;
    if(raccount->expires != 0)
    {
      if(raccount->expires < sip_reg_min_expires)
        raccount->expires = sip_reg_min_expires;
      if(raccount->expires > sip_reg_max_expires)
        raccount->expires = sip_reg_max_expires;
    }
    // add headers
    sip_contact_t *sip_contact = sip_contact_create(sep->GetHome(), URL_STRING_MAKE((const char *)raccount->GetUrl()), NULL);
    sip_add_tl(msg_reply, sip_object(msg_reply),
		   SIPTAG_CONTACT(sip_contact),
		   SIPTAG_EXPIRES_STR((const char*)PString(raccount->expires)),
  		   SIPTAG_EVENT_STR("registration"),
  		   SIPTAG_ALLOW_EVENTS_STR("presence"),
                   TAG_END());
    response_code = 200;
    goto return_response;
  }

  return_response:
    if(raccount) raccount->Unlock();
    if(response_code == 0)
      return 0;
    else
      return sep->SipReqReply(msg, msg_reply, response_code);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipMessage(msg_t *msg)
{
  PTRACE(1, trace_section << "OnReceivedSipMessage");

  sip_t *sip = sip_object(msg);
  if(sip->sip_payload == NULL)
    return sep->SipReqReply(msg, NULL, 415); // SIP_415_UNSUPPORTED_MEDIA

  if(sip->sip_content_type == NULL || sip->sip_content_type->c_type == NULL || PString(sip->sip_content_type->c_type) != "text/plain")
    return sep->SipReqReply(msg, NULL, 200); // SIP_200_OK

  MCUURL_SIP url(msg, DIRECTION_INBOUND);
  PString username_in = url.GetUserName();
  PString username_out = url.GetLocalUserName();

  if(username_in == username_out)
    return sep->SipReqReply(msg, NULL, 406); // SIP_406_NOT_ACCEPTABLE

  msg_t *msg_reply = nta_msg_create(sep->GetAgent(), 0);

  int response_code = 0;
  RegistrarAccount *raccount_in = NULL;
  RegistrarAccount *raccount_out = NULL;
  RegistrarConnection *rconn = NULL;

  rconn = FindRegConnWithLock(ACCOUNT_TYPE_SIP, username_in);
  if(rconn && rconn->state == CONN_MCU_ESTABLISHED)
  {
    MCUH323Connection *conn = ep->FindConnectionWithLock(rconn->callToken_in);
    if(conn)
    {
      conn->OnUserInputString(sip->sip_payload->pl_data);
      conn->Unlock();
    }
    response_code = 200;
    goto return_response;
  }

  raccount_in = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_in);
  raccount_out = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_out);
  response_code = SipPolicyCheck(msg, msg_reply, raccount_in, raccount_out);
  if(response_code)
    goto return_response;

  {
    if(raccount_out->account_type == ACCOUNT_TYPE_SIP)
      SipSendMessage(raccount_in, raccount_out, PString(sip->sip_payload->pl_data));
    response_code = 200;
//  else if(raccount_out->account_type == ACCOUNT_TYPE_H323)
//    H323SendMessage(raccount_out, PString(sip->sip_payload->pl_data));
  }

  return_response:
    if(rconn) rconn->Unlock();
    if(raccount_in) raccount_in->Unlock();
    if(raccount_out) raccount_out->Unlock();
    if(response_code == 0)
      return 0;
    else
      return sep->SipReqReply(msg, msg_reply, response_code);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipInvite(const msg_t *msg)
{
  PTRACE(1, trace_section << "OnReceivedSipInvite");

  PWaitAndSignal m(mutex);

  PString callToken = GetSipCallToken(msg);
  if(HasRegConn(callToken))
    return 1;

  MCUURL_SIP url(msg, DIRECTION_INBOUND);
  PString username_in = url.GetUserName();
  PString username_out = url.GetLocalUserName();

  if(username_in == username_out)
    return sep->SipReqReply(msg, NULL, 406); // SIP_406_NOT_ACCEPTABLE

  msg_t *msg_reply = nta_msg_create(sep->GetAgent(), 0);

  // default response
  int response_code = 403; // SIP_403_FORBIDDEN

  RegistrarAccount *raccount_in = NULL;
  RegistrarAccount *raccount_out = NULL;
  RegistrarConnection *rconn = NULL;

  raccount_in = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_in);

  if(allow_internal_calls)
    raccount_out = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_out);

  if(!raccount_out)
  {
    ConferenceManager *manager = OpenMCU::Current().GetConferenceManager();
    if(!manager->CheckJoinConference(username_out))
    {
      response_code = 403; // SIP_403_FORBIDDEN
      goto return_response;
    }
  }

  if((!raccount_in && !raccount_out && sip_allow_unauth_mcu_calls) ||
     (!raccount_in && raccount_out && sip_allow_unauth_internal_calls))
  {
    raccount_in = InsertAccountWithLock(ACCOUNT_TYPE_SIP, username_in);
  }

  response_code = SipPolicyCheck(msg, msg_reply, raccount_in, raccount_out);
  if(response_code)
    goto return_response;

  // update account data ???
  if(!raccount_in->registered)
  {
    raccount_in->host = url.GetHostName();
    raccount_in->domain = url.GetDomainName();
    raccount_in->port = atoi(url.GetPort());
    raccount_in->transport = url.GetTransport();
    raccount_in->display_name = url.GetDisplayName();
    raccount_in->remote_application = url.GetRemoteApplication();
  }

  {
    // redirect
    if(raccount_out && raccount_out->account_type == ACCOUNT_TYPE_SIP && raccount_in->sip_call_processing != "full" && raccount_out->sip_call_processing != "full")
    {
      sep->SipReqReply(msg, NULL, 100);
      sep->SipReqReply(msg, NULL, 180);
      // add headers
      sip_contact_t *sip_contact = sip_contact_create(sep->GetHome(), URL_STRING_MAKE((const char *)raccount_out->GetUrl()), NULL);
      sip_add_tl(msg_reply, sip_object(msg_reply),
		   SIPTAG_CONTACT(sip_contact),
                   TAG_END());
      response_code = 302;
      goto return_response;
    }

    // create MCU sip connection
    MCUSipConnection *sCon = MCUSipConnection::CreateConnection(DIRECTION_INBOUND, callToken, msg);
    if(sCon == NULL)
    {
      response_code = 500; // SIP_500_INTERNAL_SERVER_ERROR
      goto return_response;
    }

    // create registrar connection
    rconn = InsertRegConnWithLock(callToken, username_in, username_out);
    // save msg
    rconn->SetInviteMsg(msg);

     // MCU call if !raccount_out
    if(!raccount_out)
    {
      rconn->account_type_in = raccount_in->account_type;
      rconn->state = CONN_MCU_WAIT;
      response_code = -1; // MCU call
      goto return_response;
    }
    else
    {
      rconn->roomname = MCU_INTERNAL_CALL_PREFIX + OpalGloballyUniqueID().AsString();
      rconn->state = CONN_WAIT;
      response_code = 180; // SIP_180_RINGING
      goto return_response;
    }
  }

  return_response:
    if(raccount_in) raccount_in->Unlock();
    if(raccount_out) raccount_out->Unlock();
    if(rconn) rconn->Unlock();
    if(response_code == 0)
      return 0;
    else if(response_code == -1)
      sep->CreateIncomingConnection(msg); // MCU call
    else
      sep->SipReqReply(msg, msg_reply, response_code);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipSubscribe(msg_t *msg)
{
  PTRACE(1, trace_section << "OnReceivedSipSubscribe");

  PWaitAndSignal m(mutex);

  sip_t *sip = sip_object(msg);
  if(!sip->sip_event || (sip->sip_event && PString(sip->sip_event->o_type) != "presence"))
    return sep->SipReqReply(msg, NULL, 406); // SIP_406_NOT_ACCEPTABLE

  MCUURL_SIP url(msg, DIRECTION_INBOUND);
  PString username_in = url.GetUserName();
  PString username_out = url.GetLocalUserName();
  PString username_pair = username_in+"@"+username_out;

  if(username_in == username_out)
    return sep->SipReqReply(msg, NULL, 406); // SIP_406_NOT_ACCEPTABLE

  msg_t *msg_reply = nta_msg_create(sep->GetAgent(), 0);

  RegistrarAccount *raccount_in = NULL;
  RegistrarSubscription *rsub = NULL;

  raccount_in = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_in);

  int response_code = SipPolicyCheck(msg, msg_reply, raccount_in, NULL);
  if(response_code)
    goto return_response;

  {
    rsub = FindSubWithLock(username_pair);
    if(!rsub)
      rsub = InsertSubWithLock(username_in, username_out);
    rsub->state = SUB_STATE_CLOSED;
    rsub->ruri_str = url.GetUrl();
    rsub->contact_str = "sip:"+PString(sip->sip_to->a_url->url_user)+"@"+PString(sip->sip_to->a_url->url_host);
    // expires
    if(sip->sip_expires)
      rsub->expires = sip->sip_expires->ex_delta;
    else
      rsub->expires = 600;
    // create to_tag for reply
    msg_header_add_param(msg_home(msg), (msg_common_t *)sip->sip_to, nta_agent_newtag(GetHome(), "tag=%s", GetAgent()));
    // save subscribe message
    rsub->SetSubMsg(msg);
    // add headers
    sip_contact_t *sip_contact = sip_contact_create(sep->GetHome(), URL_STRING_MAKE((const char *)rsub->contact_str), NULL);
    sip_add_tl(msg_reply, sip_object(msg_reply),
		   SIPTAG_CONTACT(sip_contact),
		   SIPTAG_EXPIRES_STR((const char*)PString(rsub->expires)),
                   TAG_END());
    response_code = 202; // SIP_202_ACCEPTED
    goto return_response;
  }

  return_response:
    if(raccount_in) raccount_in->Unlock();
    if(rsub) rsub->Unlock();
    if(response_code == 0)
      return 0;
    else
      return sep->SipReqReply(msg, msg_reply, response_code);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipOptionsResponse(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);
  PString username = sip->sip_to->a_url->url_user;
  RegistrarAccount *raccount = FindAccountWithLock(ACCOUNT_TYPE_SIP, username);
  if(raccount)
  {
    raccount->keep_alive_time_response = PTime();
    raccount->Unlock();
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::SipPolicyCheck(const msg_t *msg, msg_t *msg_reply, RegistrarAccount *raccount_in, RegistrarAccount *raccount_out)
{
  PTRACE(1, trace_section << "SipPolicyCheck");
  sip_t *sip = sip_object(msg);

  if(!raccount_in)
    return 403; // SIP_403_FORBIDDEN

  int request = sip->sip_request->rq_method;
  PString method_name = sip->sip_request->rq_method_name;

  if(sip->sip_authorization)
  {
    PString response = msg_params_find(sip->sip_authorization->au_params, "response=");
    response.Replace("\"","",TRUE,0);
    HTTPAuth auth_copy(raccount_in->auth);
    auth_copy.method = method_name;
    auth_copy.uri = msg_params_find(sip->sip_authorization->au_params, "uri=");
    PString auth_response = auth_copy.MakeResponse();
    if(auth_response == response)
      return 0;
  }
  else if(sip->sip_proxy_authorization)
  {
    PString response = msg_params_find(sip->sip_proxy_authorization->au_params, "response=");
    response.Replace("\"","",TRUE,0);
    HTTPAuth auth_copy(raccount_in->auth);
    auth_copy.method = method_name;
    auth_copy.uri = msg_params_find(sip->sip_proxy_authorization->au_params, "uri=");
    PString auth_response = auth_copy.MakeResponse();
    if(auth_response == response)
      return 0;
  }

  if(request == sip_method_register)
  {
    if(!raccount_in->is_saved_account && !sip_allow_unauth_reg)
      return 403; // SIP_403_FORBIDDEN
    if(sip_allow_unauth_reg || raccount_in->auth.password == "")
      return 0;
    // add headers
    sip_authorization_t *sip_www_auth = sip_authorization_make(msg_home(msg_reply), raccount_in->auth.MakeAuthenticateStr());
    sip_add_tl(msg_reply, sip_object(msg_reply),
                   SIPTAG_WWW_AUTHENTICATE(sip_www_auth),
                   TAG_END());
    return 401; // SIP_401_UNAUTHORIZED
  }
  if(request == sip_method_invite)
  {
    if(!raccount_out && !raccount_in->is_saved_account && !sip_allow_unauth_mcu_calls)
      return 403; // SIP_403_FORBIDDEN
    if(!raccount_out && sip_allow_unauth_mcu_calls)
      return 0;
    if(raccount_out && raccount_out->host == "")
      return 404; // SIP_404_NOT_FOUND
    if(raccount_out && !raccount_in->is_saved_account && !sip_allow_unauth_internal_calls)
      return 403; // SIP_403_FORBIDDEN
    if(raccount_out && sip_allow_unauth_internal_calls)
      return 0;
    if(raccount_in->auth.password == "")
      return 0;
    // add headers
    sip_authorization_t *sip_proxy_auth = sip_authorization_make(msg_home(msg_reply), raccount_in->auth.MakeAuthenticateStr());
    sip_add_tl(msg_reply, sip_object(msg_reply),
                   SIPTAG_PROXY_AUTHENTICATE(sip_proxy_auth),
                   TAG_END());
    return 407; // SIP_407_PROXY_AUTH_REQUIRED
  }
  if(request == sip_method_message)
  {
    if(!raccount_out || raccount_out->host == "")
      return 404; // SIP_404_NOT_FOUND
    if(!raccount_in->is_saved_account && !sip_allow_unauth_reg)
      return 403; // SIP_403_FORBIDDEN
    if(sip_allow_unauth_reg || raccount_in->auth.password == "")
      return 0;
    // add headers
    sip_authorization_t *sip_proxy_auth = sip_authorization_make(msg_home(msg_reply), raccount_in->auth.MakeAuthenticateStr());
    sip_add_tl(msg_reply, sip_object(msg_reply),
                   SIPTAG_PROXY_AUTHENTICATE(sip_proxy_auth),
                   TAG_END());
    return 407; // SIP_407_PROXY_AUTH_REQUIRED
  }
  if(request == sip_method_subscribe)
  {
    if(!raccount_in->is_saved_account && !sip_allow_unauth_reg)
      return 403; // SIP_403_FORBIDDEN
    if(sip_allow_unauth_reg || raccount_in->auth.password == "")
      return 0;
    // add headers
    sip_authorization_t *sip_proxy_auth = sip_authorization_make(msg_home(msg_reply), raccount_in->auth.MakeAuthenticateStr());
    sip_add_tl(msg_reply, sip_object(msg_reply),
                   SIPTAG_PROXY_AUTHENTICATE(sip_proxy_auth),
                   TAG_END());
    return 407; // SIP_407_PROXY_AUTH_REQUIRED
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::SipSendNotify(RegistrarSubscription *rsub)
{
  PTRACE(1, trace_section << "SipSendNotify");

  msg_t *msg_sub = rsub->GetSubMsgCopy();
  sip_t *sip_sub = sip_object(msg_sub);
  if(sip_sub == NULL)
    return FALSE;

  PString basic;
  if(rsub->state == SUB_STATE_OPEN)
    basic = "open";
  else
    basic = "closed";

  PString state_rpid; // http://tools.ietf.org/search/rfc4480
  if(rsub->state == SUB_STATE_BUSY)
    state_rpid = "on-the-phone";

  PString sip_payload_str = "<?xml version='1.0' encoding='UTF-8'?>"
      "<presence xmlns='urn:ietf:params:xml:ns:pidf'"
//      " xmlns:ep='urn:ietf:params:xml:ns:pidf:status:rpid-status'"
//      " xmlns:et='urn:ietf:params:xml:ns:pidf:rpid-tuple'"
//      " xmlns:ci='urn:ietf:params:xml:ns:pidf:cipid'"
      " xmlns:dm='urn:ietf:params:xml:ns:pidf:data-model'"
      " xmlns:rpid='urn:ietf:params:xml:ns:pidf:rpid'"
      " entity='"+rsub->contact_str+"'>"
      "<tuple id='sg89ae'>"
        "<status>"
          "<basic>"+basic+"</basic>"
//          "<st:state>"+state+"</st:state>"
//          "<ep:activities><ep:activity>"+state+"</ep:activity></ep:activities>"
        "</status>"
      "</tuple>";
//      "<ci:display-name></ci:display-name>"

  if(state_rpid != "")
    sip_payload_str +=
      "<dm:person id='sg89aep'>"
        "<rpid:activities><rpid:"+state_rpid+"/></rpid:activities>"
//        "<dm:note>Idle</dm:note>"
      "</dm:person>";

  sip_payload_str += "</presence>";

  // cseq increment for incoming sub request
  sip_cseq_t *sip_cseq = sip_cseq_create(GetHome(), rsub->cseq++, SIP_METHOD_NOTIFY);

  sip_request_t *sip_rq = sip_request_create(GetHome(), SIP_METHOD_NOTIFY, URL_STRING_MAKE((const char *)rsub->ruri_str), NULL);
  sip_route_t* sip_route = sip_route_reverse(GetHome(), sip_sub->sip_record_route);

  sip_contact_t *sip_contact = sip_contact_create(sep->GetHome(), URL_STRING_MAKE((const char *)rsub->contact_str), NULL);

  msg_t *msg_req = nta_msg_create(GetAgent(), 0);
  sip_add_tl(msg_req, sip_object(msg_req),
			SIPTAG_FROM(sip_sub->sip_to),
			SIPTAG_TO(sip_sub->sip_from),
			SIPTAG_CONTACT(sip_contact),
			SIPTAG_ROUTE(sip_route),
 			SIPTAG_REQUEST(sip_rq),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip_sub->sip_call_id),
			SIPTAG_EVENT_STR("presence"),
                        SIPTAG_CONTENT_TYPE_STR("application/pidf+xml"),
                        SIPTAG_PAYLOAD_STR((const char*)sip_payload_str),
                        SIPTAG_SUBSCRIPTION_STATE_STR("active"), // active;expires=xxx
			SIPTAG_MAX_FORWARDS_STR(SIP_MAX_FORWARDS),
			SIPTAG_SERVER_STR((const char*)(SIP_USER_AGENT)),
			TAG_END());

  msg_destroy(msg_sub);
  sep->SendRequest(msg_req);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::SipSendMessage(RegistrarAccount *raccount_in, RegistrarAccount *raccount_out, const PString & message)
{
  PTRACE(1, trace_section << "SipSendMessage");

  if(raccount_out->host == "" || raccount_out->port == 0)
    return FALSE;

  PString url_from = "sip:"+raccount_in->username+"@"+raccount_out->domain;
  sip_addr_t *sip_from = sip_from_create(GetHome(), URL_STRING_MAKE((const char *)url_from));
  sip_from_tag(GetHome(), sip_from, nta_agent_newtag(GetHome(), "tag=%s", GetAgent()));

  PString url_to = "sip:"+raccount_out->username+"@"+raccount_out->domain;
  sip_addr_t *sip_to = sip_to_create(GetHome(), URL_STRING_MAKE((const char *)url_to));

  sip_cseq_t *sip_cseq = sip_cseq_create(GetHome(), 1, SIP_METHOD_MESSAGE);
  sip_request_t *sip_rq = sip_request_create(GetHome(), SIP_METHOD_MESSAGE, URL_STRING_MAKE((const char *)raccount_out->GetUrl()), NULL);
  sip_call_id_t* sip_call_id = sip_call_id_create(GetHome(), NULL);

  sip_route_t* sip_route = NULL;
  if(raccount_out->registered)
  {
    msg_t *msg_reg = raccount_out->GetRegisterMsgCopy();
    sip_t *sip_reg = sip_object(msg_reg);
    if(sip_reg)
      sip_route = sip_route_reverse(GetHome(), sip_reg->sip_record_route);
    msg_destroy(msg_reg);
  }

  msg_t *msg_req = nta_msg_create(GetAgent(), 0);
  sip_add_tl(msg_req, sip_object(msg_req),
			SIPTAG_FROM(sip_from),
			SIPTAG_TO(sip_to),
			//SIPTAG_CONTACT(sip_contact),
			SIPTAG_ROUTE(sip_route),
 			SIPTAG_REQUEST(sip_rq),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip_call_id),
			SIPTAG_CONTENT_TYPE_STR("text/plain"),
                        SIPTAG_PAYLOAD_STR((const char*)message),
			SIPTAG_MAX_FORWARDS_STR(SIP_MAX_FORWARDS),
			SIPTAG_SERVER_STR((const char*)(SIP_USER_AGENT)),
			TAG_END());

  sep->SendRequest(msg_req);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::SipSendPing(RegistrarAccount *raccount)
{
  if(raccount->host == "" || raccount->port == 0)
    return FALSE;

  PString url_from = "sip:keepalive@"+raccount->domain;
  sip_addr_t *sip_from = sip_from_create(GetHome(), URL_STRING_MAKE((const char *)url_from));
  sip_from_tag(GetHome(), sip_from, nta_agent_newtag(GetHome(), "tag=%s", GetAgent()));

  PString url_to = "sip:"+raccount->username+"@"+raccount->domain;
  sip_addr_t *sip_to = sip_to_create(GetHome(), URL_STRING_MAKE((const char *)url_to));

  sip_cseq_t *sip_cseq = sip_cseq_create(GetHome(), su_randint(0, 0x7fffffff), SIP_METHOD_OPTIONS);
  sip_request_t *sip_rq = sip_request_create(GetHome(), SIP_METHOD_OPTIONS, URL_STRING_MAKE((const char *)raccount->GetUrl()), NULL);
  sip_call_id_t* sip_call_id = sip_call_id_make(GetHome(), PGloballyUniqueID().AsString());

  sip_route_t* sip_route = NULL;
  if(raccount->registered)
  {
    msg_t *msg_reg = raccount->GetRegisterMsgCopy();
    sip_t *sip = sip_object(msg_reg);
    if(sip && sip->sip_record_route)
      sip_route = sip_route_reverse(GetHome(), sip->sip_record_route);
    msg_destroy(msg_reg);
  }

  msg_t *msg_req = nta_msg_create(GetAgent(), 0);
  sip_add_tl(msg_req, sip_object(msg_req),
			SIPTAG_FROM(sip_from),
			SIPTAG_TO(sip_to),
			SIPTAG_ROUTE(sip_route),
 			SIPTAG_REQUEST(sip_rq),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip_call_id),
                        TAG_END());

  sep->SendRequest(msg_req);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
