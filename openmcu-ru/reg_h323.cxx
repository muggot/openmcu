/*
 * reg_h323.cxx
 *
 * Copyright (C) 2014-2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
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
 *
 */

#include "precompile.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

H323Connection::AnswerCallResponse Registrar::OnReceivedH323Invite(MCUH323Connection *conn)
{
  PTRACE(1, trace_section << "OnReceivedH323Invite");

  PWaitAndSignal m(mutex);

  MCUURL url(conn->GetMemberName());
  if(url.GetUserName() == "" || url.GetHostName() == "")
    return H323Connection::AnswerCallDenied;

  if(HasRegConn(conn->GetCallToken()))
    return H323Connection::AnswerCallDenied;

  PString username_in = url.GetUserName();
  PString username_out = conn->GetRequestedRoom();

  if(username_in == username_out)
  {
    PTRACE(1, trace_section << "error " << username_in);
    return H323Connection::AnswerCallDenied;
  }

  // default response
  H323Connection::AnswerCallResponse response = H323Connection::AnswerCallDenied;

  RegistrarAccount *raccount_in = NULL;
  RegistrarAccount *raccount_out = NULL;
  RegistrarConnection *rconn = NULL;

  raccount_in = FindAccountWithLock(ACCOUNT_TYPE_H323, username_in);

  if(allow_internal_calls)
    raccount_out = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_out);

  if(!raccount_out)
  {
    ConferenceManager *manager = OpenMCU::Current().GetConferenceManager();
    if(!manager->CheckJoinConference(username_out))
    {
      response = H323Connection::AnswerCallDenied;
      goto return_response;
    }
  }

  if((!raccount_in && !raccount_out && h323_allow_unreg_mcu_calls) ||
     (!raccount_in && raccount_out && h323_allow_unreg_internal_calls))
  {
    raccount_in = InsertAccountWithLock(ACCOUNT_TYPE_H323, username_in);
  }
  if(!raccount_in)
  {
    response = H323Connection::AnswerCallDenied;
    goto return_response;
  }

  if((!raccount_out && !h323_allow_unreg_mcu_calls) || (raccount_out && !h323_allow_unreg_internal_calls))
  {
    if(raccount_in->h323CallIdentifier != conn->GetCallIdentifier())
    {
      response = H323Connection::AnswerCallDenied;
      goto return_response;
    }
  }

  // update account data ???
  if(!raccount_in->registered)
  {
    raccount_in->host = url.GetHostName();
    raccount_in->domain = raccount_in->host;
    raccount_in->display_name = url.GetDisplayName();
    raccount_in->remote_application = conn->GetRemoteApplication();
  }

  rconn = InsertRegConnWithLock(conn->GetCallToken(), username_in, username_out);

  // MCU call if !raccount_out
  if(!raccount_out)
  {
    rconn->account_type_in = raccount_in->account_type;
    rconn->state = CONN_MCU_WAIT;
    response = H323Connection::AnswerCallNow;
  } else {
    rconn->roomname = MCU_INTERNAL_CALL_PREFIX + OpalGloballyUniqueID().AsString();
    rconn->state = CONN_WAIT;
    response = H323Connection::AnswerCallPending;
  }

  return_response:
    if(raccount_in) raccount_in->Unlock();
    if(raccount_out) raccount_out->Unlock();
    if(rconn) rconn->Unlock();
    return response;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323GatekeeperRequest::Response RegistrarGk::OnRegistration(H323GatekeeperRRQ & info)
{
  PWaitAndSignal m(mutex);

  H323GatekeeperRequest::Response response = H323GatekeeperServer::OnRegistration(info);
  if(response != H323GatekeeperRequest::Confirm)
  {
    PTRACE(1, trace_section << "registration failed");
    return response;
  }

  PIPSocket::Address host;
  WORD port = 0;

  H323TransportAddress srcAddress;
  if(!info.IsBehindNAT() && info.rrq.m_callSignalAddress.GetSize())
  {
    srcAddress = H323TransportAddress(info.rrq.m_callSignalAddress[0]);
    srcAddress.GetIpAndPort(host, port); // need get only port
  }
  srcAddress = info.GetReplyAddress();
  srcAddress.GetIpAddress(host);

  if(!info.rcf.HasOptionalField(H225_RegistrationRequest::e_timeToLive))
  {
    info.rcf.IncludeOptionalField(H225_RegistrationRequest::e_timeToLive);
    info.rcf.m_timeToLive = defaultTimeToLive;
  }
  if(info.rcf.m_timeToLive < minTimeToLive)
    info.rcf.m_timeToLive = minTimeToLive;
  if(info.rcf.m_timeToLive > maxTimeToLive)
    info.rcf.m_timeToLive = maxTimeToLive;
  unsigned expires = info.rcf.m_timeToLive;

  PString h323id = info.rcf.m_endpointIdentifier.GetValue();
//  PString remote_application = H323GetApplicationInfo(info.rrq.m_endpointVendor);
  PString remote_application = info.rrq.m_endpointVendor.m_productId.AsString()+" "+
                               info.rrq.m_endpointVendor.m_versionId.AsString();

  PString username;
  PString display_name;
  if(info.rrq.HasOptionalField(H225_RegistrationRequest::e_terminalAlias))
  {
    username = H323GetAliasUserName(info.rrq.m_terminalAlias);
    display_name = H323GetAliasDisplayName(info.rrq.m_terminalAlias);
    if(remote_application.Find("MyPhone") != P_MAX_INDEX || remote_application.Find("Polycom ViaVideo Release 8.0") != P_MAX_INDEX)
    {
      username = convert_ucs2_to_utf8(username);
      display_name = convert_ucs2_to_utf8(display_name);
    }
    if(remote_application.Find("RealPresence") != P_MAX_INDEX)
    {
      username = PWORDArrayToPString(username.AsUCS2());
      display_name = PWORDArrayToPString(display_name.AsUCS2());
    }
  }
  if(username == "")
  {
    PTRACE(1, trace_section << "check already endpoint registered " << h323id);
    username = registrar->FindAccountNameFromH323Id(h323id);
  }
  if(username == "")
  {
    PTRACE(1, trace_section << "unable to determine username");
    return H323GatekeeperRequest::Reject;
  }

  // check account
  RegistrarAccount *raccount = registrar->FindAccountWithLock(ACCOUNT_TYPE_H323, username);
  if(!raccount && !requireH235)
    raccount = registrar->InsertAccountWithLock(ACCOUNT_TYPE_H323, username);

  if(!raccount || (raccount && !raccount->is_saved_account && requireH235))
  {
    PTRACE(1, trace_section << "registration failed");
    return H323GatekeeperRequest::Reject;
  }

  // update account data
  raccount->host = host.AsString();
  raccount->domain = raccount->host;
  if(port != 0)
    raccount->port = port;
  raccount->display_name = display_name;
  raccount->remote_application = remote_application;
  raccount->h323id = h323id;

  // regsiter TTL
  raccount->registered = TRUE;
  raccount->start_time = PTime();
  raccount->expires = expires;

  raccount->Unlock();

  PTRACE(1, trace_section << "endpoint registered, username " << username << ", id " << h323id);
  return response;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323GatekeeperRequest::Response RegistrarGk::OnUnregistration(H323GatekeeperURQ & info)
{
  H323GatekeeperRequest::Response response = H323GatekeeperServer::OnUnregistration(info);

  PWaitAndSignal m(mutex);

  if(info.urq.HasOptionalField(H225_UnregistrationRequest::e_endpointAlias))
  {
    for(PINDEX i = 0; i < info.urq.m_endpointAlias.GetSize(); i++)
    {
      PString alias = H323GetAliasAddressString(info.urq.m_endpointAlias[i]);
      RegistrarAccount *raccount = registrar->FindAccountWithLock(ACCOUNT_TYPE_H323, alias);
      if(raccount)
      {
        raccount->registered = FALSE;
        raccount->Unlock();
      }
    }
  }

  return response;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL RegistrarGk::AdmissionPolicyCheck(H323GatekeeperARQ & info)
{
  PWaitAndSignal m(mutex);

  BOOL answerCall = info.arq.m_answerCall;
  //if (arq.m_answerCall ? canOnlyAnswerRegisteredEP : canOnlyCallRegisteredEP) {

  OpalGloballyUniqueID id = info.arq.m_callIdentifier.m_guid;
  if(id == NULL)
  {
    //PTRACE(1, trace_section << "No call identifier provided in ARQ!");
    info.SetRejectReason(H225_AdmissionRejectReason::e_undefinedReason);
    return FALSE;
  }

  BOOL accept = FALSE;
  if(answerCall)
  {
    accept = TRUE;
    // call from MCU
    /*
    if(info.arq.HasOptionalField(H225_AdmissionRequest::e_srcCallSignalAddress))
    {
      PString srcHost = H323TransportAddress(info.arq.m_srcCallSignalAddress).GetHostName();
      H323TransportAddressArray taa = ownerEndPoint.GetInterfaceAddresses(TRUE, NULL);
      for(PINDEX i = 0; i < taa.GetSize(); i++)
        if(taa[i].GetHostName() == srcHost) { accept = TRUE; break; }
    }
    */
  } else {
    for(PINDEX i = 0; i < info.arq.m_srcInfo.GetSize(); i++)
    {
      PSafePtr<H323RegisteredEndPoint> ep = FindEndPointByAliasAddress(info.arq.m_srcInfo[i]);
      if(ep != NULL) { accept = TRUE; break; }
    }
    if(!accept)
    {
      PString srcNumber = H323GetAliasAddressE164(info.arq.m_srcInfo);
      PSafePtr<H323RegisteredEndPoint> ep = FindEndPointByAliasString(srcNumber);
      if(ep != NULL) accept = TRUE;
    }
  }
  if(!accept)
  {
    //PTRACE(1, trace_section << "ARQ rejected, not allowed to answer call");
    info.SetRejectReason(H225_AdmissionRejectReason::e_securityDenial);
  }
  return accept;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString RegistrarGk::GetAdmissionSrcUsername(H323GatekeeperARQ & info)
{
  PWaitAndSignal m(mutex);
  PString username;
  username = H323GetAliasUserName(info.arq.m_srcInfo);
  // myphone BUG
  PINDEX pos = username.FindLast(" [");
  if(pos != P_MAX_INDEX) username = username.Left(pos);
  // remove host from alias
  username = username.Tokenise("@")[0];
  return username;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString RegistrarGk::GetAdmissionDstUsername(H323GatekeeperARQ & info)
{
  PWaitAndSignal m(mutex);
  PString username;
  if(info.arq.HasOptionalField(H225_AdmissionRequest::e_destinationInfo))
    username = H323GetAliasUserName(info.arq.m_destinationInfo);
  return username;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323GatekeeperRequest::Response RegistrarGk::OnAdmission(H323GatekeeperARQ & info)
{

  PWaitAndSignal m(mutex);

  if(AdmissionPolicyCheck(info) == FALSE)
    return H323GatekeeperRequest::Reject;

  BOOL direct = FALSE;
  H323TransportAddress dstHost;
  PString srcUsername = GetAdmissionSrcUsername(info);
  PString dstUsername = GetAdmissionDstUsername(info);
  if(srcUsername == dstUsername)
  {
    info.SetRejectReason(H225_AdmissionRejectReason::e_undefinedReason);
    return H323GatekeeperRequest::Reject;
  }

  RegistrarAccount *raccount_in = registrar->FindAccountWithLock(ACCOUNT_TYPE_H323, srcUsername);
  RegistrarAccount *raccount_out = NULL;

  // set call identifier for security check incoming call
  if(raccount_in)
    raccount_in->h323CallIdentifier = info.arq.m_callIdentifier.m_guid;

  // direct
  if(dstUsername == "" && info.arq.HasOptionalField(H225_AdmissionRequest::e_destCallSignalAddress))
  {
    dstHost = info.arq.m_destCallSignalAddress;
    direct = TRUE;
  }
  else if(raccount_in && dstUsername != "")
  {
    //
    PString dstPrefix = dstUsername;
    if(dstUsername.Find("*") != P_MAX_INDEX)
    {
      dstPrefix = dstPrefix.Left(dstPrefix.Find("*"));
      dstUsername = dstUsername.Right(dstUsername.GetLength()-dstPrefix.GetLength()-1);
    }
    //
    //raccount_out = registrar->FindAccountWithLock(ACCOUNT_TYPE_H323, dstUsername);
    raccount_out = registrar->FindAccountWithLock(ACCOUNT_TYPE_H323, dstPrefix);
    if(raccount_in && raccount_out && raccount_out->account_type == ACCOUNT_TYPE_H323 &&
       raccount_out->host != "" && raccount_out->port != 0 &&
       raccount_in->h323_call_processing != "full" && raccount_out->h323_call_processing != "full"
      )
    {
      dstHost = H323TransportAddress(raccount_out->host, raccount_out->port);
      direct = TRUE;
    }
  }

  if(raccount_in) raccount_in->Unlock();
  if(raccount_out) raccount_out->Unlock();

  if(direct)
    return OnAdmissionDirect(info, dstUsername, dstHost);
  return OnAdmissionMCU(info);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323GatekeeperRequest::Response RegistrarGk::OnAdmissionDirect(H323GatekeeperARQ & info, PString dstUsername, H323TransportAddress dstHost)
{
  // for registered endpoint use default
  H323GatekeeperRequest::Response ret = H323GatekeeperServer::OnAdmission(info);
  if(ret == H323GatekeeperRequest::Confirm)
    return ret;

  // append destination aliases
  if(info.arq.HasOptionalField(H225_AdmissionRequest::e_destinationInfo))
  {
    PStringArray dstAliases(dstUsername);
    H323SetAliasAddresses(dstAliases, info.acf.m_destinationInfo);
    if(info.acf.m_destinationInfo.GetSize())
      info.acf.IncludeOptionalField(H225_AdmissionConfirm::e_destinationInfo);
  }

  // append destination address
  H323TransportAddressArray taa;
  taa.AppendString(dstHost);
  taa[0].SetPDU(info.acf.m_destCallSignalAddress);

  // use bandwidth of request
  unsigned requestedBandwidth = info.arq.m_bandWidth;
  info.acf.m_bandWidth = requestedBandwidth;

  return H323GatekeeperRequest::Confirm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323GatekeeperRequest::Response RegistrarGk::OnAdmissionMCU(H323GatekeeperARQ & info)
{
  PString srcUsername = GetAdmissionSrcUsername(info);
  PString dstUsername = GetAdmissionDstUsername(info);

  // append destination aliases
  if(info.arq.HasOptionalField(H225_AdmissionRequest::e_destinationInfo))
  {
    PStringArray dstAliases;
    for(PINDEX i = 0; i < info.arq.m_destinationInfo.GetSize(); i++)
    {
      PString alias = H323GetAliasAddressString(info.arq.m_destinationInfo[i]);
      alias = alias.Tokenise("@")[0]; // remove host from alias
      if(alias != "" && dstAliases.GetValuesIndex(alias) == P_MAX_INDEX)
        dstAliases += alias;
    }
    PString dstNumber = H323GetAliasAddressE164(info.arq.m_destinationInfo);
    if(dstNumber != "")
      dstAliases += dstNumber;
    if(dstAliases.GetSize())
    {
      H323SetAliasAddresses(dstAliases, info.acf.m_destinationInfo);
      if(info.acf.m_destinationInfo.GetSize())
        info.acf.IncludeOptionalField(H225_AdmissionConfirm::e_destinationInfo);
    }
  }

  // append destination address
  H323TransportAddressArray taa = ownerEndPoint.GetInterfaceAddresses(TRUE, NULL);
  taa[0].SetPDU(info.acf.m_destCallSignalAddress);

  // use bandwidth of request
  unsigned requestedBandwidth = info.arq.m_bandWidth;
  info.acf.m_bandWidth = requestedBandwidth;

  // If ep can do UUIE's then use that to expidite getting some statistics
  if (info.arq.m_willSupplyUUIEs) {
    info.acf.m_uuiesRequested.m_alerting = TRUE;
    info.acf.m_uuiesRequested.m_connect = TRUE;
    //setup = FALSE
    //callProceeding = FALSE
    //information = FALSE
    //releaseComplete = FALSE
    //facility = FALSE
    //progress = FALSE
    //empty = FALSE
    //status = FALSE
    //statusInquiry = FALSE
    //setupAcknowledge = FALSE
    //notify = FALSE
  }

  return H323GatekeeperRequest::Confirm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323GatekeeperRequest::Response RegistrarGk::OnDisengage(H323GatekeeperDRQ & request)
{
  //return H323GatekeeperRequest::Confirm;
  return H323GatekeeperServer::OnDisengage(request);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323GatekeeperRequest::Response RegistrarGk::OnInfoResponse(H323GatekeeperIRR & request)
{
  return H323GatekeeperServer::OnInfoResponse(request);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned RegistrarGk::AllocateBandwidth(unsigned newBandwidth, unsigned oldBandwidth)
{
  return newBandwidth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void RegistrarGk::SendUnregister(const PString & alias, int reason)
{
  PSafePtr<H323RegisteredEndPoint> rep = FindEndPointByAliasString(alias);
  if(rep)
    rep->Unregister(reason);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323Transactor * RegistrarGk::CreateListener(H323Transport * transport)
{
  gkListener = H323GatekeeperServer::CreateListener(transport);
  if(gkListener)
    gkListener->StartChannel();

  return gkListener;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RegistrarGk::~RegistrarGk()
{
  MCURegistrarAccountList & accountList = registrar->GetAccountList();
  for(MCURegistrarAccountList::shared_iterator it = accountList.begin(); it != accountList.end(); ++it)
  {
    RegistrarAccount *raccount = *it;
    if(raccount->account_type == ACCOUNT_TYPE_H323 && raccount->registered)
    {
      SendUnregister(raccount->username);
      raccount->registered = FALSE;
    }
  }

  if(gkListener)
  {
    delete gkListener;
    gkListener = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RegistrarGk::RegistrarGk(MCUH323EndPoint *ep, Registrar *_registrar)
  : H323GatekeeperServer(*ep), registrar(_registrar)
{
  gkListener = NULL;
  trace_section = "Registrar gk: ";

  requireH235 = TRUE;
  minTimeToLive = 60;
  maxTimeToLive = 600;
  defaultTimeToLive = 600;        // zero disables

  totalBandwidth = UINT_MAX;      // Unlimited total bandwidth
  usedBandwidth = 0;              // None used so far
  defaultBandwidth = 2560;        // Enough for bidirectional G.711 and 64k H.261
  maximumBandwidth = 200000;      // 10baseX LAN bandwidth
  defaultInfoResponseRate = 60;   // One minute, zero disables
  overwriteOnSameSignalAddress = TRUE;
  canHaveDuplicateAlias = FALSE;
  canHaveDuplicatePrefix = FALSE;
  canOnlyCallRegisteredEP = FALSE;
  canOnlyAnswerRegisteredEP = FALSE;
  answerCallPreGrantedARQ = FALSE;
  makeCallPreGrantedARQ = FALSE;
  isGatekeeperRouted = FALSE;
  aliasCanBeHostName = TRUE;
  disengageOnHearbeatFail = TRUE;

  identifierBase = time(NULL);
  nextIdentifier = 1;

  peakRegistrations = 0;
  totalRegistrations = 0;
  rejectedRegistrations = 0;
  peakCalls = 0;
  totalCalls = 0;
  rejectedCalls = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
