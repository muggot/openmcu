
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipRegister(const msg_t *msg)
{
  PTRACE(1, "Registrar\tOnReceivedSipRegister");
  sip_t *sip = sip_object(msg);

  MCUURL_SIP url(msg, DIRECTION_INBOUND);
  PString username = url.GetUserName();
  msg_t *msg_reply = nta_msg_create(sep->GetAgent(), 0);

  RegistrarAccount *regAccount = NULL;
  PWaitAndSignal m(mutex);

  regAccount = FindAccountWithLock(ACCOUNT_TYPE_SIP, username);
  if(!regAccount && !sip_require_password)
    regAccount = InsertAccountWithLock(ACCOUNT_TYPE_SIP, username);

  int response_code = SipPolicyCheck(msg, msg_reply, regAccount, NULL);
  if(response_code)
    goto return_response;

  {
    regAccount->registered = TRUE;
    regAccount->start_time = PTime();
    // update account data
    regAccount->host = url.GetHostName();
    regAccount->domain = url.GetDomainName();
    regAccount->port = atoi(url.GetPort());
    regAccount->transport = url.GetTransport();
    if(regAccount->display_name == "")
      regAccount->display_name = url.GetDisplayName();
    regAccount->remote_application = url.GetRemoteApplication();
    regAccount->contact_str = regAccount->GetUrl();
    // save register message
    msg_destroy(regAccount->msg_reg);
    regAccount->msg_reg = msg_dup(msg);
    msg_addr_copy(regAccount->msg_reg, msg);
    // expires
    regAccount->expires = sip_reg_max_expires;
    if(sip->sip_expires)
      regAccount->expires = sip->sip_expires->ex_delta;
    if(regAccount->expires != 0)
    {
      if(regAccount->expires < sip_reg_min_expires)
        regAccount->expires = sip_reg_min_expires;
      if(regAccount->expires > sip_reg_max_expires)
        regAccount->expires = sip_reg_max_expires;
    }
    // add headers
    sip_add_tl(msg_reply, sip_object(msg_reply),
		   SIPTAG_CONTACT_STR(regAccount->contact_str),
		   SIPTAG_EXPIRES_STR(PString(regAccount->expires)),
  		   SIPTAG_EVENT_STR("registration"),
  		   SIPTAG_ALLOW_EVENTS_STR("presence"),
                   TAG_END());
    response_code = 200;
    goto return_response;
  }
  return_response:
    if(regAccount) regAccount->Unlock();
    if(response_code == 0)
      return 0;
    else
      return sep->SipReqReply(msg, msg_reply, response_code);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipMessage(msg_t *msg)
{
  PTRACE(1, "Registrar\tOnReceivedSipMessage");
  sip_t *sip = sip_object(msg);

  if(sip->sip_payload == NULL)
    return sep->SipReqReply(msg, NULL, 415); // SIP_415_UNSUPPORTED_MEDIA

  PString username_in = sip->sip_from->a_url->url_user;
  PString username_out = sip->sip_to->a_url->url_user;
  msg_t *msg_reply = nta_msg_create(sep->GetAgent(), 0);

  RegistrarAccount *regAccount_in = NULL;
  RegistrarAccount *regAccount_out = NULL;
  PWaitAndSignal m(mutex);

  regAccount_in = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_in);
  regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_out);

  int response_code = SipPolicyCheck(msg, msg_reply, regAccount_in, regAccount_out);
  if(response_code)
    goto return_response;

  {
    if(regAccount_out->account_type == ACCOUNT_TYPE_SIP)
      SipSendMessage(regAccount_in, regAccount_out, PString(sip->sip_payload->pl_data));
    response_code = 200;
//  else if(regAccount_out->account_type == ACCOUNT_TYPE_H323)
//    H323SendMessage(regAccount_out, PString(sip->sip_payload->pl_data));
  }

  return_response:
    if(regAccount_in) regAccount_in->Unlock();
    if(regAccount_out) regAccount_out->Unlock();
    if(response_code == 0)
      return 0;
    else
      return sep->SipReqReply(msg, msg_reply, response_code);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipInvite(const msg_t *msg)
{
  PTRACE(1, "Registrar\tOnReceivedSipInvite");
  sip_t *sip = sip_object(msg);

  PString callToken = GetSipCallToken(msg);
  if(FindRegConn(callToken))
    return 1;

  MCUURL_SIP url(msg, DIRECTION_INBOUND);
  PString username_in = url.GetUserName();
  PString username_out = sip->sip_to->a_url->url_user;
  msg_t *msg_reply = nta_msg_create(sep->GetAgent(), 0);

  if(username_in == username_out)
    sep->SipReqReply(msg, NULL, 486); // SIP_486_BUSY_HERE

  RegistrarAccount *regAccount_in = NULL;
  RegistrarAccount *regAccount_out = NULL;
  RegistrarConnection *regConn = NULL;
  PWaitAndSignal m(mutex);

  regAccount_in = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_in);
  if(allow_internal_calls)
    regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_out);

  if((!regAccount_in && sip_allow_unauth_mcu_calls && !regAccount_out) || (!regAccount_in && sip_allow_unauth_internal_calls && regAccount_out))
    regAccount_in = InsertAccountWithLock(ACCOUNT_TYPE_SIP, username_in);

  int response_code = SipPolicyCheck(msg, msg_reply, regAccount_in, regAccount_out);
  if(response_code)
    goto return_response;

  // update account data ???
  if(!regAccount_in->registered)
  {
    regAccount_in->host = url.GetHostName();
    regAccount_in->domain = url.GetDomainName();
    regAccount_in->port = atoi(url.GetPort());
    regAccount_in->transport = url.GetTransport();
    if(regAccount_in->display_name == "")
      regAccount_in->display_name = url.GetDisplayName();
    regAccount_in->remote_application = url.GetRemoteApplication();
    regAccount_in->contact_str = regAccount_in->GetUrl();
  }

  {
    // redirect
    if(regAccount_out && regAccount_out->account_type == ACCOUNT_TYPE_SIP && regAccount_in->sip_call_processing != "full" && regAccount_out->sip_call_processing != "full")
    {
      sep->SipReqReply(msg, NULL, 100);
      sep->SipReqReply(msg, NULL, 180);
      // add headers
      sip_add_tl(msg_reply, sip_object(msg_reply),
		   SIPTAG_CONTACT_STR(regAccount_out->GetUrl()),
                   TAG_END());
      response_code = 302;
      goto return_response;
    }

    // create MCU sip connection
    new MCUSipConnection(sep, ep, DIRECTION_INBOUND, callToken, msg);

    // create registrar connection
    regConn = InsertRegConnWithLock(callToken, username_in, username_out);
    msg_destroy(regConn->msg_invite);
    regConn->msg_invite = msg_dup(msg);
    msg_addr_copy(regConn->msg_invite, msg);

     // MCU call if !regAccount_out
    if(!regAccount_out)
    {
      regConn->account_type_in = regAccount_in->account_type;
      regConn->state = CONN_MCU_WAIT;
      response_code = -1; // MCU call
      goto return_response;
    }
    else
    {
      regConn->roomname = MCU_INTERNAL_CALL_PREFIX + OpalGloballyUniqueID().AsString();
      regConn->state = CONN_WAIT;
      response_code = 180; // SIP_180_RINGING
      goto return_response;
    }
  }

  return_response:
    if(regAccount_in) regAccount_in->Unlock();
    if(regAccount_out) regAccount_out->Unlock();
    if(regConn) regConn->Unlock();
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
  PTRACE(1, "Registrar\tOnReceivedSipSubscribe");
  sip_t *sip = sip_object(msg);

  if(!sip->sip_event || (sip->sip_event && PString(sip->sip_event->o_type) != "presence"))
    return sep->SipReqReply(msg, NULL, 406); // SIP_406_NOT_ACCEPTABLE

  MCUURL_SIP url(msg, DIRECTION_INBOUND);
  PString username_in = url.GetUserName();
  PString username_out = sip->sip_to->a_url->url_user;
  PString username_pair = username_in+"@"+username_out;
  msg_t *msg_reply = nta_msg_create(sep->GetAgent(), 0);

  RegistrarAccount *regAccount_in = NULL;
  Subscription *subAccount = NULL;
  PWaitAndSignal m(mutex);

  regAccount_in = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_in);

  int response_code = SipPolicyCheck(msg, msg_reply, regAccount_in, NULL);
  if(response_code)
    goto return_response;

  {
    subAccount = FindSubWithLock(username_pair);
    if(!subAccount)
      subAccount = InsertSubWithLock(this, username_in, username_out);
    subAccount->state = SUB_STATE_CLOSED;
    subAccount->ruri_str = url.GetUrl();
    subAccount->contact_str = "sip:"+PString(sip->sip_to->a_url->url_user)+"@"+PString(sip->sip_to->a_url->url_host);
    // expires
    if(sip->sip_expires)
      subAccount->expires = sip->sip_expires->ex_delta;
    else
      subAccount->expires = 600;
    // save subscribe message
    // create to_tag for reply
    msg_header_add_param(msg_home(msg), (msg_common_t *)sip->sip_to, nta_agent_newtag(GetHome(), "tag=%s", GetAgent()));
    msg_destroy(subAccount->msg_sub);
    subAccount->msg_sub = msg_dup(msg);
    msg_addr_copy(subAccount->msg_sub, msg);
    // add headers
    sip_add_tl(msg_reply, sip_object(msg_reply),
		   SIPTAG_CONTACT_STR(subAccount->contact_str),
		   SIPTAG_EXPIRES_STR(PString(subAccount->expires)),
                   TAG_END());
    response_code = 202; // SIP_202_ACCEPTED
    goto return_response;
  }

  return_response:
    if(regAccount_in) regAccount_in->Unlock();
    if(subAccount) subAccount->Unlock();
    if(response_code == 0)
      return 0;
    else
      return sep->SipReqReply(msg, msg_reply, response_code);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::SipPolicyCheck(const msg_t *msg, msg_t *msg_reply, RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out)
{
  PTRACE(1, "Registrar\tSipPolicyCheck");
  sip_t *sip = sip_object(msg);

  if(!regAccount_in)
    return 403; // SIP_403_FORBIDDEN

  int request = sip->sip_request->rq_method;
  PString method_name = sip->sip_request->rq_method_name;

  if(sip->sip_authorization)
  {
    PString reg_response = msg_params_find(sip->sip_authorization->au_params, "response=");
    PString reg_uri = msg_params_find(sip->sip_authorization->au_params, "uri=");

    PString sip_auth_str = sep->MakeAuthStr(regAccount_in->username, regAccount_in->password, reg_uri, method_name, regAccount_in->scheme, regAccount_in->domain, regAccount_in->nonce);
    sip_authorization_t *sip_auth = sip_authorization_make(msg_home(msg), sip_auth_str);
    regAccount_in->www_response = msg_params_find(sip_auth->au_params, "response=");
    if(regAccount_in->www_response == reg_response)
      return 0;
  }
  else if(sip->sip_proxy_authorization)
  {
    PString reg_response = msg_params_find(sip->sip_proxy_authorization->au_params, "response=");
    PString reg_uri = msg_params_find(sip->sip_proxy_authorization->au_params, "uri=");

    PString sip_auth_str = sep->MakeAuthStr(regAccount_in->username, regAccount_in->password, reg_uri, method_name, regAccount_in->scheme, regAccount_in->domain, regAccount_in->nonce);
    sip_authorization_t *sip_auth = sip_authorization_make(msg_home(msg), sip_auth_str);
    regAccount_in->proxy_response = msg_params_find(sip_auth->au_params, "response=");
    if(regAccount_in->proxy_response == reg_response)
      return 0;
  }

  if(request == sip_method_register)
  {
    if(!regAccount_in->enable && sip_require_password)
      return 403; // SIP_403_FORBIDDEN
    if(!sip_require_password)
      return 0;
    if(regAccount_in->password == "")
      return 0;
    // add headers
    sip_authorization_t *sip_www_auth = sip_authorization_make(msg_home(msg_reply), regAccount_in->GetAuthStr());
    sip_add_tl(msg_reply, sip_object(msg_reply),
                   SIPTAG_WWW_AUTHENTICATE(sip_www_auth),
                   TAG_END());
    return 401; // SIP_401_UNAUTHORIZED
  }
  if(request == sip_method_invite)
  {
    if(regAccount_out && regAccount_out->host == "")
      return 404; // SIP_404_NOT_FOUND
    if(regAccount_out && sip_allow_unauth_internal_calls)
      return 0;
    if(!regAccount_out && sip_allow_unauth_mcu_calls)
      return 0;
    if(regAccount_in->password == "")
      return 0;
    // add headers
    sip_authorization_t *sip_proxy_auth = sip_authorization_make(msg_home(msg_reply), regAccount_in->GetAuthStr());
    sip_add_tl(msg_reply, sip_object(msg_reply),
                   SIPTAG_PROXY_AUTHENTICATE(sip_proxy_auth),
                   TAG_END());
    return 407; // SIP_407_PROXY_AUTH_REQUIRED
  }
  if(request == sip_method_message)
  {
    if(!regAccount_out || (regAccount_out && !regAccount_out->registered) || (regAccount_out && regAccount_out->host == ""))
      return 404; // SIP_404_NOT_FOUND
    if(regAccount_in->password == "")
      return 0;
    // add headers
    sip_authorization_t *sip_proxy_auth = sip_authorization_make(msg_home(msg_reply), regAccount_in->GetAuthStr());
    sip_add_tl(msg_reply, sip_object(msg_reply),
                   SIPTAG_PROXY_AUTHENTICATE(sip_proxy_auth),
                   TAG_END());
    return 407; // SIP_407_PROXY_AUTH_REQUIRED
  }
  if(request == sip_method_subscribe)
  {
    if(regAccount_in->password == "")
      return 0;
    // add headers
    sip_authorization_t *sip_proxy_auth = sip_authorization_make(msg_home(msg_reply), regAccount_in->GetAuthStr());
    sip_add_tl(msg_reply, sip_object(msg_reply),
                   SIPTAG_PROXY_AUTHENTICATE(sip_proxy_auth),
                   TAG_END());
    return 407; // SIP_407_PROXY_AUTH_REQUIRED
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::SipSendNotify(msg_t *msg_sub, Subscription *subAccount)
{
  PTRACE(1, "MCUSIP\tSipSendNotify");
  sip_t *sip_sub = sip_object(msg_sub);

  PString basic;
  if(subAccount->state == SUB_STATE_OPEN)
    basic = "open";
  else
    basic = "closed";

  PString state_rpid; // http://tools.ietf.org/search/rfc4480
  if(subAccount->state == SUB_STATE_BUSY)
    state_rpid = "on-the-phone";

  PString sip_payload_str = "<?xml version='1.0' encoding='UTF-8'?>"
      "<presence xmlns='urn:ietf:params:xml:ns:pidf'"
//      " xmlns:ep='urn:ietf:params:xml:ns:pidf:status:rpid-status'"
//      " xmlns:et='urn:ietf:params:xml:ns:pidf:rpid-tuple'"
//      " xmlns:ci='urn:ietf:params:xml:ns:pidf:cipid'"
      " xmlns:dm='urn:ietf:params:xml:ns:pidf:data-model'"
      " xmlns:rpid='urn:ietf:params:xml:ns:pidf:rpid'"
      " entity='"+subAccount->contact_str+"'>"
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

  url_string_t *ruri = (url_string_t *)(const char *)subAccount->ruri_str;

  // cseq increment for incoming sub request
  sip_cseq_t *sip_cseq = sip_cseq_create(GetHome(), subAccount->cseq++, SIP_METHOD_NOTIFY);
  msg_header_insert(msg_sub, (msg_pub_t *)sip_sub, (msg_header_t *)sip_cseq);

  sip_request_t *sip_rq = sip_request_create(GetHome(), SIP_METHOD_NOTIFY, ruri, NULL);
  sip_route_t* sip_route = sip_route_reverse(GetHome(), sip_sub->sip_record_route);

  msg_t *msg_req = nta_msg_create(GetAgent(), 0);
  nta_outgoing_mcreate(GetAgent(), NULL, NULL,
			ruri,
			msg_req,
			NTATAG_STATELESS(1),
			SIPTAG_FROM(sip_sub->sip_to),
			SIPTAG_TO(sip_sub->sip_from),
			SIPTAG_CONTACT_STR(subAccount->contact_str),
			SIPTAG_ROUTE(sip_route),
 			SIPTAG_REQUEST(sip_rq),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip_sub->sip_call_id),
			SIPTAG_EVENT_STR("presence"),
                        SIPTAG_CONTENT_TYPE_STR("application/pidf+xml"),
                        SIPTAG_PAYLOAD_STR(sip_payload_str),
                        SIPTAG_SUBSCRIPTION_STATE_STR("active"), // active;expires=xxx
			SIPTAG_MAX_FORWARDS_STR(SIP_MAX_FORWARDS),
			SIPTAG_SERVER_STR(SIP_USER_AGENT),
			TAG_END());
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::SipSendMessage(RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out, PString message)
{
  PTRACE(1, "MCUSIP\tServerSendMessage");
  sip_t *sip_reg_out = sip_object(regAccount_out->msg_reg);

  // do not use GetUrl() directly, "nta outgoing create: invalid URI"
  PString ruri_str = regAccount_out->GetUrl();
  url_string_t *ruri = (url_string_t *)(const char *)ruri_str;

  PString url_from = "sip:"+regAccount_in->username+"@"+regAccount_out->domain;
  sip_addr_t *sip_from = sip_from_create(GetHome(), (url_string_t *)(const char *)url_from);
  sip_from_tag(GetHome(), sip_from, nta_agent_newtag(GetHome(), "tag=%s", GetAgent()));

  PString url_to = "sip:"+regAccount_out->username+"@"+regAccount_out->domain;
  sip_addr_t *sip_to = sip_to_create(GetHome(), (url_string_t *)(const char *)url_to);

  sip_cseq_t *sip_cseq = sip_cseq_create(GetHome(), 1, SIP_METHOD_MESSAGE);
  sip_request_t *sip_rq = sip_request_create(GetHome(), SIP_METHOD_MESSAGE, ruri, NULL);
  sip_call_id_t* sip_call_id = sip_call_id_create(GetHome(), "");

  sip_route_t* sip_route = NULL;
  if(sip_reg_out)
    sip_route = sip_route_reverse(GetHome(), sip_reg_out->sip_record_route);

  msg_t *msg_req = nta_msg_create(GetAgent(), 0);
  nta_outgoing_mcreate(GetAgent(), NULL, NULL,
			ruri,
			msg_req,
			NTATAG_STATELESS(1),
			SIPTAG_FROM(sip_from),
			SIPTAG_TO(sip_to),
			//SIPTAG_CONTACT(sip_contact),
			SIPTAG_ROUTE(sip_route),
 			SIPTAG_REQUEST(sip_rq),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip_call_id),
			SIPTAG_CONTENT_TYPE_STR("text/plain"),
                        SIPTAG_PAYLOAD_STR(message),
			SIPTAG_MAX_FORWARDS_STR(SIP_MAX_FORWARDS),
			SIPTAG_SERVER_STR(SIP_USER_AGENT),
			TAG_END());
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
