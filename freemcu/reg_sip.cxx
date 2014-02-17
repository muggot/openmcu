
#include <ptlib.h>
#include <ptclib/guid.h>

#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnIncomingMsg(msg_t *msg)
{
  PTRACE(1, "Registrar\tOnIncomingMessage");
  if(sep->terminating)
    return TRUE;

  sip_t *sip = sip_object(msg);
  if(sip->sip_cseq == NULL)
  {
    SipReqReply(msg, SIP_400_BAD_REQUEST);
    return TRUE;
  }

  PString username = sip->sip_from->a_url->url_user;
  PString username_out = sip->sip_to->a_url->url_user;
  BOOL sipToSip = FALSE;
  if(username != username_out && FindAccount(ACCOUNT_TYPE_SIP, username) && FindAccount(ACCOUNT_TYPE_SIP, username_out))
    sipToSip = TRUE;

  int request = 0, status = 0, cseq = 0;
  if(sip->sip_request) request = sip->sip_request->rq_method;
  if(sip->sip_status)  status = sip->sip_status->st_status;
  if(sip->sip_cseq)    cseq = sip->sip_cseq->cs_method;

  // register
  if(request == sip_method_register)
  {
    OnReceivedSipRegister(msg);
    return TRUE;
  }
  // publish
  if(request == sip_method_publish)
  {
    SipReqReply(msg, SIP_200_OK);
    return TRUE;
  }
  // options
  if(request == sip_method_options)
  {
    SipReqReply(msg, SIP_200_OK);
    return TRUE;
  }
  //
  if(username == username_out)
  {
    if(request == sip_method_invite)
      SipReqReply(msg, SIP_486_BUSY_HERE);
    else
      SipReqReply(msg, SIP_200_OK);
    return TRUE;
  }

  // subscribe dialog, notify receive in callback function
  if(request == sip_method_subscribe)
  {
    OnReceivedSipSubscribe(msg);
    return TRUE;
  }
  // notify only receive in subscribe callback function
//  if(request == sip_method_notify)
//  {
//    SipReqReply(msg, SIP_481_NO_TRANSACTION);
//    return TRUE;
//  }
//  if(request == sip_method_subscribe || request == sip_method_notify || cseq == sip_method_notify || cseq == sip_method_subscribe)
//    return SipForwardMessage(msg);

  // message dialog
  if(request == sip_method_message)
  {
    OnReceivedSipMessage(msg);
    return TRUE;
  }

  // forwarding
  //PString processing_type = FreeMCU::Current().GetEndpointParamFromUrl("Processing", "");
  PString processing_type = "transcoding";
  if((sipToSip && processing_type == "forwarding") &&
     (cseq == sip_method_invite || cseq == sip_method_cancel || cseq == sip_method_bye || cseq == sip_method_ack))
  {
    SipForwardMessage(msg);
    return TRUE;
  }

  // invite
  if(request == sip_method_invite)
  {
    OnReceivedSipInvite(msg);
    return TRUE;
  }

  if(request == sip_method_refer ||
     request == sip_method_update)
  {
    SipReqReply(msg, SIP_501_NOT_IMPLEMENTED);
    return TRUE;
  }

  return FALSE;
// request == "REGISTER"
// request == "PUBLISH"
// request == "SUBSCRIBE"
// request == "NOTIFY"
// request == "UPDATE"
// request == "REFER"
// request == "MESSAGE"
}


////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipRegister(const msg_t *msg)
{
  PTRACE(1, "Registrar\tOnReceivedSipRegister");
  sip_t *sip = sip_object(msg);

  PString username = sip->sip_from->a_url->url_user;
  PString domain = sip->sip_from->a_url->url_host;
  PString host = GetSipFromHost(msg, 0);

  unsigned response_code = 501; // default SIP_501_NOT_IMPLEMENTED
  PString sip_auth_str;

  RegistrarAccount *regAccount = FindAccountWithLock(ACCOUNT_TYPE_SIP, username);
  if(!regAccount && !sip_require_password)
  {
    regAccount = InsertAccountWithLock(ACCOUNT_TYPE_SIP, username, host);
    regAccount->domain = domain;
  }
  if(!regAccount)
  {
    response_code = 403; // SIP_403_FORBIDDEN
    goto return_response;
  }
  //regAccount->Reset();

  if(!SipPolicyCheck(msg, regAccount, NULL))
  {
    sip_auth_str = regAccount->GetAuthStr();
    response_code = 401; // SIP_401_UNAUTHORIZED
    goto return_response;
  }

  {
    regAccount->registered = TRUE;
    regAccount->domain = domain;
    regAccount->start_time = PTime();
    if(sip->sip_expires)
      regAccount->expires = sip->sip_expires->ex_delta;
    else
      regAccount->expires = 600;
    msg_destroy(regAccount->msg_reg);
    regAccount->msg_reg = msg_dup(msg);
    response_code = 200; // SIP_200_OK
    goto return_response;
  }
  return_response:
    if(regAccount) regAccount->Unlock();
    return SipReqReply(msg, response_code, sip_auth_str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipMessage(msg_t *msg)
{
  PTRACE(1, "Registrar\tOnReceivedSipMessage");
  sip_t *sip = sip_object(msg);

  if(sip->sip_payload == NULL)
    return SipReqReply(msg, SIP_415_UNSUPPORTED_MEDIA);

  PString username_in = sip->sip_from->a_url->url_user;
  PString username_out = sip->sip_to->a_url->url_user;

  RegistrarAccount *regAccount_in = NULL;
  RegistrarAccount *regAccount_out = NULL;

  unsigned response_code = 200; // default SIP_200_OK
  PString sip_auth_str;

  regAccount_in = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_in);
  if(!regAccount_in)
  {
    response_code = 403; // SIP_403_FORBIDDEN
    goto return_response;
  }

  regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_out);
  if(!regAccount_out)
  {
    response_code = 404; // SIP_404_NOT_FOUND
    goto return_response;
  }

  // auth
  if(!SipPolicyCheck(msg, regAccount_in, NULL))
  {
    sip_auth_str = regAccount_in->GetAuthStr();
    response_code = 401; // SIP_401_UNAUTHORIZED
    goto return_response;
  }

  if(regAccount_out->account_type == ACCOUNT_TYPE_SIP)
    SipSendMessage(regAccount_in, regAccount_out, PString(sip->sip_payload->pl_data));
//  else if(regAccount_out->account_type == ACCOUNT_TYPE_H323)
//    H323SendMessage(regAccount_out, PString(sip->sip_payload->pl_data));

  return_response:
    if(regAccount_in) regAccount_in->Unlock();
    if(regAccount_out) regAccount_out->Unlock();
    return SipReqReply(msg, response_code, sip_auth_str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::SipSendMessage(RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out, PString message)
{
  PTRACE(1, "MCUSIP\tServerSendMessage");
  sip_t *sip_reg_out = sip_object(regAccount_out->msg_reg);

  PString ruri_str;
  if(regAccount_out->registered)
    ruri_str = sep->CreateRuriStr(regAccount_out->msg_reg, 0);
  else if(regAccount_out->host != "")
    ruri_str = regAccount_out->GetUrl();
  else
    return FALSE;
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
			SIPTAG_SERVER_STR(MCUSIP_USER_AGENT_STR),
			TAG_END());
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipInvite(const msg_t *msg)
{
  PTRACE(1, "Registrar\tOnReceivedSipInvite");
  sip_t *sip = sip_object(msg);

  if(sip->sip_payload == NULL)
    return SipReqReply(msg, SIP_415_UNSUPPORTED_MEDIA);

  PString callToken = "sip:"+PString(sip->sip_from->a_url->url_user)+":"+PString(sip->sip_call_id->i_id);

  if(FindRegConn(callToken))
    return 0;

  PString username_in = sip->sip_from->a_url->url_user;
  PString host_in = GetSipFromHost(msg, 0);
  PString username_out = sip->sip_to->a_url->url_user;

  int response_code = 404; // default SIP_404_NOT_FOUND
  PString sip_auth_str;

  RegistrarAccount *regAccount_in = NULL;
  RegistrarAccount *regAccount_out = NULL;
  RegistrarConnection *regConn = NULL;

  regAccount_in = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_in);

  if(allow_internal_calls)
  {
    regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_out);
    //if(regAccount_out && !regAccount_out->enable && !regAccount_out->registered)
    //{
    //  response_code = 404; // SIP_404_NOT_FOUND
    //  goto return_response;
    //}
  }

  if((!regAccount_in && sip_allow_unauth_mcu_calls && !regAccount_out) ||
     (!regAccount_in && sip_allow_unauth_internal_calls && regAccount_out))
  {
    // create temp acount with registered status
    regAccount_in = InsertAccountWithLock(ACCOUNT_TYPE_SIP, username_in, host_in);
    regAccount_in->registered = TRUE;
    regAccount_in->start_time = PTime();
    regAccount_in->expires = 60;
  }
  if(!regAccount_in)
  {
    response_code = 403; // SIP_403_FORBIDDEN
    goto return_response;
  }

  // auth
  if(!SipPolicyCheck(msg, regAccount_in, regAccount_out))
  {
    sip_auth_str = regAccount_in->GetAuthStr();
    response_code = 407; // SIP_407_PROXY_AUTH_REQUIRED
    goto return_response;
  }

  regConn = InsertRegConnWithLock(callToken, username_in, username_out);
  regConn->roomname = internal_room_prefix + OpalGloballyUniqueID().AsString();
  msg_destroy(regConn->msg_invite);
  regConn->msg_invite = msg_dup(msg);

  // MCU call if !regAccount_out
  if(!regAccount_out)
  {
    regConn->state = CONN_MCU_WAIT;
    response_code = 0; // MCU call
  } else {
    regConn->state = CONN_WAIT;
    response_code = 100; // SIP_100_TRYING
  }

  return_response:
    if(regAccount_in) regAccount_in->Unlock();
    if(regAccount_out) regAccount_out->Unlock();
    if(regConn) regConn->Unlock();
    if(response_code == -1)
      return 0;
    else if(response_code == 0)
      sep->CreateIncomingConnection(msg); // MCU call
    else
      SipReqReply(msg, response_code, sip_auth_str);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::OnReceivedSipSubscribe(msg_t *msg)
{
  PTRACE(1, "Registrar\tOnReceivedSipSubscribe");
  sip_t *sip = sip_object(msg);
  if(!sip->sip_expires) return 0;

  RegistrarAccount *regAccount_in = NULL;
  Subscription *subAccount = NULL;

  unsigned response_code = 202; // SIP_202_ACCEPTED
  PString sip_auth_str;

  if(sip->sip_event && PString(sip->sip_event->o_type) == "presence")
  {
    PString username_in = sip->sip_from->a_url->url_user;
    PString username_out = sip->sip_to->a_url->url_user;
    PString username_pair = username_in+"@"+username_out;

    regAccount_in = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_in);
    if(!regAccount_in)
    {
      response_code = 403; // SIP_403_FORBIDDEN
      goto return_response;
    }

    // auth
    if(!SipPolicyCheck(msg, regAccount_in, NULL))
    {
      sip_auth_str = regAccount_in->GetAuthStr();
      response_code = 407; // SIP_407_PROXY_AUTH_REQUIRED
      goto return_response;
    }

    subAccount = FindSubWithLock(username_pair);
    if(subAccount)
      subAccount->Reset();
    else
      subAccount = InsertSubWithLock(this, username_in, username_out);

    if(sip->sip_expires)
      subAccount->expires = sip->sip_expires->ex_delta;
    else
      subAccount->expires = 600;
    // create to_tag for reply
    msg_header_add_param(msg_home(msg), (msg_common_t *)sip->sip_to, nta_agent_newtag(GetHome(), "tag=%s", GetAgent()));
    //
    msg_destroy(subAccount->msg_sub);
    subAccount->msg_sub = msg_dup(msg);
    //
    response_code = 202; // SIP_202_ACCEPTED
    goto return_response;
  }
  return_response:
    if(regAccount_in) regAccount_in->Unlock();
    if(subAccount) subAccount->Unlock();
    return SipReqReply(msg, response_code, sip_auth_str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::SipSendNotify(msg_t *msg_sub, int state)
{
  PTRACE(1, "MCUSIP\tSipSendNotify");
  sip_t *sip_sub = sip_object(msg_sub);
  if(sip_sub == NULL) return 0;

  PString sip_contact_str = "sip:"+PString(sip_sub->sip_to->a_url->url_user)+"@"+PString(sip_sub->sip_to->a_url->url_host);

  PString basic;
  if(state == SUB_STATE_OPEN)
    basic = "open";
  else
    basic = "closed";

  PString state_rpid; // http://tools.ietf.org/search/rfc4480
  if(state == SUB_STATE_BUSY)
    state_rpid = "on-the-phone";

  PString sip_payload_str = "<?xml version='1.0' encoding='UTF-8'?>"
      "<presence xmlns='urn:ietf:params:xml:ns:pidf'"
//      " xmlns:ep='urn:ietf:params:xml:ns:pidf:status:rpid-status'"
//      " xmlns:et='urn:ietf:params:xml:ns:pidf:rpid-tuple'"
//      " xmlns:ci='urn:ietf:params:xml:ns:pidf:cipid'"
      " xmlns:dm='urn:ietf:params:xml:ns:pidf:data-model'"
      " xmlns:rpid='urn:ietf:params:xml:ns:pidf:rpid'"
      " entity='"+sip_contact_str+"'>"
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

  int direction = 0;
  PString ruri_str = sep->CreateRuriStr(msg_sub, direction);
  url_string_t *ruri = (url_string_t *)(const char *)ruri_str;

  sip_contact_t *sip_contact = sip_contact_create(GetHome(), (url_string_t *)(const char *)sip_contact_str, NULL);

  // cseq increment for incoming sub request
  sip_cseq_t *sip_cseq = sip_cseq_create(GetHome(), sip_sub->sip_cseq->cs_seq+1, SIP_METHOD_NOTIFY);
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
			SIPTAG_CONTACT(sip_contact),
			SIPTAG_ROUTE(sip_route),
 			SIPTAG_REQUEST(sip_rq),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip_sub->sip_call_id),
			SIPTAG_EVENT_STR("presence"),
                        SIPTAG_CONTENT_TYPE_STR("application/pidf+xml"),
                        SIPTAG_PAYLOAD_STR(sip_payload_str),
                        SIPTAG_SUBSCRIPTION_STATE_STR("active"), // active;expires=xxx
			SIPTAG_SERVER_STR(MCUSIP_USER_AGENT_STR),
			TAG_END());
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Registrar::SipPolicyCheck(const msg_t *msg, RegistrarAccount *regAccount_in, RegistrarAccount *regAccount_out)
{
  PTRACE(1, "Registrar\tSipPolicyCheck");
  sip_t *sip = sip_object(msg);

  if(!sip->sip_request)
    return TRUE;
  PString request_name = sip->sip_request->rq_method_name;
  int request = sip->sip_request->rq_method;

  if(regAccount_in->password == "")
  {
    return TRUE;
  }
  if(request == sip_method_register && !sip_require_password)
  {
    return TRUE;
  }
  if(request == sip_method_invite)
  {
    if(!regAccount_out && sip_allow_unauth_mcu_calls)
      return TRUE;
    if(regAccount_out && sip_allow_unauth_internal_calls)
      return TRUE;
  }
  if(request == sip_method_cancel || request == sip_method_ack)
  {
    return TRUE;
  }

  if(sip->sip_authorization)
  {
    PString reg_response = msg_params_find(sip->sip_authorization->au_params, "response=");
    PString reg_uri = msg_params_find(sip->sip_authorization->au_params, "uri=");

    PString sip_auth_str = sep->MakeAuthStr(regAccount_in->username, regAccount_in->password, reg_uri, request_name, regAccount_in->scheme, regAccount_in->domain, regAccount_in->nonce);
    sip_authorization_t *sip_auth = sip_authorization_make(msg_home(msg), sip_auth_str);
    regAccount_in->www_response = msg_params_find(sip_auth->au_params, "response=");
    if(regAccount_in->www_response == reg_response)
      return TRUE;
  }
  else if(sip->sip_proxy_authorization)
  {
    PString reg_response = msg_params_find(sip->sip_proxy_authorization->au_params, "response=");
    PString reg_uri = msg_params_find(sip->sip_proxy_authorization->au_params, "uri=");

    PString sip_auth_str = sep->MakeAuthStr(regAccount_in->username, regAccount_in->password, reg_uri, request_name, regAccount_in->scheme, regAccount_in->domain, regAccount_in->nonce);
    sip_authorization_t *sip_auth = sip_authorization_make(msg_home(msg), sip_auth_str);
    regAccount_in->proxy_response = msg_params_find(sip_auth->au_params, "response=");
    if(regAccount_in->proxy_response == reg_response)
      return TRUE;
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::SipReqReply(const msg_t *msg, unsigned method, PString sip_auth_str)
{
  const char *method_name = NULL;
  method_name = sip_status_phrase(method);
  if(method_name == NULL)
    return 0;
  return SipReqReply(msg, method, method_name, sip_auth_str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::SipReqReply(const msg_t *msg, unsigned method, const char *method_name, PString sip_auth_str)
{
  //PTRACE(1, "Registrar\tSipReqReply");
  sip_t *sip = sip_object(msg);
  if(sip == NULL || sip->sip_request == NULL) return 0;

  sip_authorization_t *sip_www_auth=NULL, *sip_proxy_auth=NULL;
  if(sip_auth_str != "")
  {
    if(method == 401)
      sip_www_auth = sip_authorization_make(sep->GetHome(), sip_auth_str);
    else if(method == 407)
      sip_proxy_auth = sip_authorization_make(sep->GetHome(), sip_auth_str);
  }

  const char *event = NULL;
  const char *allow_events = NULL;
  sip_contact_t *sip_contact = NULL;
  if(sip->sip_request->rq_method == sip_method_register)
  {
    event = "registration";
    allow_events = "presence";
    sip_contact = sip->sip_contact;
  }
  PString allow = "SUBSCRIBE, INVITE, ACK, BYE, CANCEL, OPTIONS, INFO";

  sip_expires_t *sip_expires = NULL;
  if(sip->sip_request->rq_method == sip_method_register ||
     sip->sip_request->rq_method == sip_method_subscribe ||
     sip->sip_request->rq_method == sip_method_publish)
    sip_expires = sip->sip_expires;

  msg_t *msg_reply = msg_dup(msg);
  nta_msg_treply(sep->GetAgent(), msg_reply, method, method_name,
                   SIPTAG_CONTACT(sip_contact),
		   SIPTAG_EXPIRES(sip_expires),
  		   SIPTAG_WWW_AUTHENTICATE(sip_www_auth),
		   SIPTAG_PROXY_AUTHENTICATE(sip_proxy_auth),
  		   SIPTAG_EVENT_STR(event),
  		   SIPTAG_ALLOW_EVENTS_STR(allow_events),
  		   SIPTAG_ALLOW_STR(allow),
                   //SIPTAG_CONTENT_TYPE_STR(content_str),
                   //SIPTAG_PAYLOAD(sip_payload),
                   SIPTAG_SERVER_STR((const char*)(MCUSIP_USER_AGENT_STR)),
                   TAG_END());
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Registrar::SipForwardMessage(msg_t *msg)
{
  sip_t *sip = sip_object(msg);

  PString username_in = sip->sip_from->a_url->url_user;
  PString username_out = sip->sip_to->a_url->url_user;

  unsigned response_code = 404; // default SIP_404_NOT_FOUND
  PString sip_auth_str;

  RegistrarAccount *regAccount_in = NULL;
  RegistrarAccount *regAccount_out = NULL;

  regAccount_in = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_in);
  if(!regAccount_in)
  {
    response_code = 403; // SIP_403_FORBIDDEN
    goto return_response;
  }

  regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_SIP, username_out);
  if(!regAccount_out) // || (regAccount_out && !regAccount_in->registered))
  {
    response_code = 404; // SIP_404_NOT_FOUND
    goto return_response;
  }

  // authorization
  if(!SipPolicyCheck(msg, regAccount_in, regAccount_out))
  {
    sip_auth_str = regAccount_in->GetAuthStr();
    response_code = 407; // SIP_407_PROXY_AUTH_REQUIRED
    goto return_response;
  }

  {
    PString ruri_str;
    if(regAccount_out->registered)
      ruri_str = sep->CreateRuriStr(regAccount_out->msg_reg, 0);
    else if(regAccount_out->host != "")
      ruri_str = regAccount_out->GetUrl();
    else
      goto return_response;
    url_string_t *ruri = (url_string_t *)(const char *)ruri_str;
    // create reply message
    msg_t *msg_reply = msg_dup(msg);
    sip_t *c_sip = sip_object(msg_reply);
    // remove authorization headers
    if(c_sip->sip_authorization)
      msg_header_remove(msg_reply, (msg_pub_t *)c_sip, (msg_header_t *)c_sip->sip_authorization);
    if(c_sip->sip_proxy_authorization)
      msg_header_remove(msg_reply, (msg_pub_t *)c_sip, (msg_header_t *)c_sip->sip_proxy_authorization);
/*
    // add route header
    if(processing_type == "route signaling")
    {
      PString route_url = "sip:"+PString(sip->sip_to->a_url->url_host);
      if(sip->sip_to->a_url->url_port)
        route_url += sip->sip_to->a_url->url_port;
      sip_record_route_t* sip_record_route = sip_record_route_create(GetHome(), url_make(GetHome(), route_url), NULL);
      msg_header_insert(msg_reply, (msg_pub_t *)c_sip, (msg_header_t *)sip_record_route);
    }
*/
    //
    nta_msg_tsend(sep->GetAgent(),
                  msg_reply,
                  ruri,
                  SIPTAG_SERVER_STR(MCUSIP_USER_AGENT_STR),
                  TAG_END());
    response_code = 0; // EMPTY
    goto return_response;
  }

  return_response:
    if(regAccount_in) regAccount_in->Unlock();
    if(regAccount_out) regAccount_out->Unlock();
    if(response_code != 0)
      return SipReqReply(msg, response_code, sip_auth_str);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString Registrar::GetSipFromHost(const msg_t *msg, int direction)
{
  // 0=incoming, 1=outgoing
  PTRACE(1, "Registrar\tGetFromHost");
  sip_t *sip = sip_object(msg);
  su_home_t *home = msg_home(msg);

  sip_addr_t *sip_from;
  if(direction == 0)
    sip_from = sip_from_dup(home, sip->sip_from);
  else
    sip_from = sip_to_dup(home, sip->sip_to);

  PString host;
  if(PString(sip->sip_via->v_host) != "0.0.0.0" && sip->sip_request)
    host = sip->sip_via->v_host;
//  if(sip->sip_contact && sip->sip_contact->m_url)
//    host = sip->sip_contact->m_url->url_host;
  else
    host = sip_from->a_url->url_host;

  return host;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Subscription::LegOutCreate(RegistrarAccount *regAccount_out)
{
  sip_t *sip = sip_object(msg_sub);
  if(sip == NULL) return 0;

  PString username_in = sip->sip_from->a_url->url_user;
  PString username_out = sip->sip_to->a_url->url_user;
  PString domain_out = regAccount_out->domain;

  PString ruri_str;
  if(regAccount_out->registered)
    ruri_str = registrar->GetSep()->CreateRuriStr(regAccount_out->msg_reg, 0);
  else if(regAccount_out->host != "")
    ruri_str = regAccount_out->GetUrl();
  else
    return 0;
  url_string_t *ruri = (url_string_t *)(const char *)ruri_str;

  sip_addr_t *sip_from = sip_from_create(registrar->GetHome(), (url_string_t *)(const char *)
      ("sip:"+username_in+"@"+domain_out));
  sip_from_tag(registrar->GetHome(), sip_from, nta_agent_newtag(registrar->GetHome(), "tag=%s", registrar->GetAgent()));
  sip_addr_t *sip_to = sip_from_create(registrar->GetHome(), (url_string_t *)(const char *)
      ("sip:"+username_out+"@"+domain_out));
  sip_contact_t *sip_contact = sip_contact_create(registrar->GetHome(), (url_string_t *)(const char *)
      ("sip:"+username_in+"@"+domain_out), NULL);

  sip_call_id_t* sip_call_id = sip_call_id_create(registrar->GetHome(), "");

  leg_sub_out = nta_leg_tcreate(registrar->GetAgent(), wrap_sub_request_out_cb, (nta_leg_magic_t *)this,
                        SIPTAG_FROM(sip_from),
                        SIPTAG_TO(sip_to),
			SIPTAG_CALL_ID(sip_call_id),
                        SIPTAG_SERVER_STR(MCUSIP_USER_AGENT_STR),
                        TAG_END());
  if(leg_sub_out == NULL)
    return 0;
  orq_sub_out = nta_outgoing_tcreate(leg_sub_out, wrap_sub_response_out_cb, (nta_outgoing_magic_t *)this,
			ruri,
			SIP_METHOD_SUBSCRIBE,
			ruri,
                        SIPTAG_CONTACT(sip_contact),
                        SIPTAG_CSEQ(sip->sip_cseq),
    		        SIPTAG_EVENT_STR("presence"),
    		        SIPTAG_ACCEPT_STR("application/pidf+xml"),
    		        SIPTAG_EXPIRES(sip->sip_expires),
    		        //SIPTAG_ALLOW_STR("INVITE,ACK,OPTIONS,BYE,CANCEL,SUBSCRIBE,NOTIFY,REFER,MESSAGE,INFO,PING,PRACK"),
			SIPTAG_SERVER_STR(MCUSIP_USER_AGENT_STR),
			TAG_END());
  return 1;
}

int Subscription::sub_request_out_cb(nta_leg_t *leg, nta_incoming_t *irq, const sip_t *sip_not)
{
  msg_t *msg_not = nta_incoming_getrequest(irq);
  if(sip_not->sip_request && sip_not->sip_request->rq_method == sip_method_notify)
  {
    PString pay_not = sip_not->sip_payload->pl_data;
    if(pay_not.Find(">busy<") != P_MAX_INDEX || pay_not.Find("on-the-phone") != P_MAX_INDEX)
      state_new = SUB_STATE_BUSY;
    else if(pay_not.Find(">unreachable<") != P_MAX_INDEX)
      state_new = SUB_STATE_CLOSED;
    else if(pay_not.Find(">close<") != P_MAX_INDEX || pay_not.Find(">closed<") != P_MAX_INDEX)
      state_new = SUB_STATE_CLOSED;
    else
      state_new = SUB_STATE_OPEN;
  }
  registrar->SipReqReply(msg_not, SIP_200_OK);
  return 200; // nta: timer J fired, terminate 200 response
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int Subscription::sub_response_out_cb(nta_outgoing_t *orq, const sip_t *sip)
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
