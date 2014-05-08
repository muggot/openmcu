
#include <ptlib.h>
#include <ptclib/guid.h>

#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

H323Connection::AnswerCallResponse Registrar::OnIncomingMsg(PString remoteAccount, PString & requestedRoom, PString callToken, OpalGloballyUniqueID callIdentifier)
{
  PTRACE(1, "Registrar\tOnIncomingCallH323");
  MCUURL url(remoteAccount);
  if(url.GetUserName() == "" || url.GetHostName() == "")
    return H323Connection::AnswerCallDenied;

  if(FindRegConn(callToken))
    return H323Connection::AnswerCallDenied;

  PString username_in = url.GetUserName();
  PString host_in = url.GetHostName();
  PString username_out = requestedRoom;
  //PString host_out = "";

  H323Connection::AnswerCallResponse response = H323Connection::AnswerCallDenied; // default response

  RegistrarAccount *regAccount_in = NULL;
  RegistrarAccount *regAccount_out = NULL;
  RegistrarConnection *regConn = NULL;

  regAccount_in = FindAccountWithLock(ACCOUNT_TYPE_H323, username_in);

  if(allow_internal_calls)
  {
    regAccount_out = FindAccountWithLock(ACCOUNT_TYPE_UNKNOWN, username_out);
    if(regAccount_out && !regAccount_out->enable && !regAccount_out->registered)
    {
      response = H323Connection::AnswerCallDenied;
      goto return_response;
    }
  }
  if((!regAccount_in && h323_allow_unreg_mcu_calls && !regAccount_out) ||
     (!regAccount_in && h323_allow_unreg_internal_calls && regAccount_out))
  {
    // create temp acount with registered status
    regAccount_in = InsertAccountWithLock(ACCOUNT_TYPE_H323, username_in, host_in);
    regAccount_in->registered = TRUE;
    regAccount_in->start_time = PTime();
    regAccount_in->expires = 60;
  }
  if(!regAccount_in)
  {
    response = H323Connection::AnswerCallDenied;
    goto return_response;
  }

  if((!regAccount_out && !h323_allow_unreg_mcu_calls) || (regAccount_out && !h323_allow_unreg_internal_calls))
  {
    if(regAccount_in->h323CallIdentifier != callIdentifier)
    {
      response = H323Connection::AnswerCallDenied;
      goto return_response;
    }
  }

  regConn = InsertRegConnWithLock(callToken, username_in, username_out);
  // MCU call if !regAccount_out
  if(!regAccount_out)
  {
    regConn->roomname = requestedRoom;
    regConn->state = CONN_MCU_WAIT;
    response = H323Connection::AnswerCallNow;
  } else {
    regConn->roomname = internal_room_prefix + OpalGloballyUniqueID().AsString();
    requestedRoom = regConn->roomname;
    regConn->state = CONN_WAIT;
    response = H323Connection::AnswerCallPending;
  }

  return_response:
    if(regAccount_in) regAccount_in->Unlock();
    if(regAccount_out) regAccount_out->Unlock();
    if(regConn) regConn->Unlock();
    return response;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323GatekeeperRequest::Response RegistrarGk::OnRegistration(H323GatekeeperRRQ & info)
{
  PWaitAndSignal m(mutex);

  H323GatekeeperRequest::Response response = H323GatekeeperServer::OnRegistration(info);
  if(response != H323GatekeeperRequest::Confirm)
    return response;

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

  unsigned expires = GetTimeToLive();
  if(info.rrq.HasOptionalField(H225_RegistrationRequest::e_timeToLive) && expires > info.rrq.m_timeToLive)
    expires = info.rrq.m_timeToLive;

  if(info.rrq.HasOptionalField(H225_RegistrationRequest::e_terminalAlias))
  {
    for(PINDEX i = 0; i < info.rrq.m_terminalAlias.GetSize(); i++)
    {
      PString username = H323GetAliasAddressString(info.rrq.m_terminalAlias[i]);
      RegistrarAccount *regAccount = registrar->FindAccountWithLock(ACCOUNT_TYPE_H323, username);
      if(!regAccount && requireH235) // ??? или добавить все алиасы?
        continue;
      if(!regAccount)
        regAccount = registrar->InsertAccountWithLock(ACCOUNT_TYPE_H323, username, "");
      regAccount->registered = TRUE;
      regAccount->display_name = username;
      regAccount->start_time = PTime();
      regAccount->expires = expires;
      regAccount->host = host.AsString();
      if(port != 0)
        regAccount->port = port;
      regAccount->Unlock();
    }
  }
  return response;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323GatekeeperRequest::Response RegistrarGk::OnUnregistration(H323GatekeeperURQ & info)
{
  H323GatekeeperRequest::Response response = H323GatekeeperServer::OnUnregistration(info);
  mutex.Wait();
  if(info.urq.HasOptionalField(H225_UnregistrationRequest::e_endpointAlias))
  {
    for(PINDEX i = 0; i < info.urq.m_endpointAlias.GetSize(); i++)
    {
      PString alias = H323GetAliasAddressString(info.urq.m_endpointAlias[i]);
      RegistrarAccount *regAccount = registrar->FindAccountWithLock(ACCOUNT_TYPE_H323, alias);
      if(regAccount)
      {
        regAccount->registered = FALSE;
        regAccount->Unlock();
      }
    }
  }
  mutex.Signal();
  return response;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL RegistrarGk::AdmissionPolicyCheck(H323GatekeeperARQ & info)
{
  PWaitAndSignal wait(mutex);

  BOOL answerCall = info.arq.m_answerCall;
  //if (arq.m_answerCall ? canOnlyAnswerRegisteredEP : canOnlyCallRegisteredEP) {

  OpalGloballyUniqueID id = info.arq.m_callIdentifier.m_guid;
  if(id == NULL)
  {
    //PTRACE(1, "Registrar H.323\tNo call identifier provided in ARQ!");
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
    //PTRACE(1, "Registrar H.323\tARQ rejected, not allowed to answer call");
    info.SetRejectReason(H225_AdmissionRejectReason::e_securityDenial);
  }
  return accept;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString RegistrarGk::GetAdmissionSrcUsername(H323GatekeeperARQ & info)
{
  PWaitAndSignal m(mutex);
  PString username;
  username = H323GetAliasAddressE164(info.arq.m_srcInfo);
  if(username == "")
  {
    if(info.arq.m_srcInfo.GetSize() == 1)
      username = H323GetAliasAddressString(info.arq.m_srcInfo[0]);
    else if(info.arq.m_srcInfo.GetSize() > 1)
      username = H323GetAliasAddressString(info.arq.m_srcInfo[1]);
  }
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
  {
    username = H323GetAliasAddressE164(info.arq.m_destinationInfo);
    if(username == "")
    {
      if(info.arq.m_destinationInfo.GetSize() == 1)
        username = H323GetAliasAddressString(info.arq.m_destinationInfo[0]);
      else if(info.arq.m_destinationInfo.GetSize() > 1)
        username = H323GetAliasAddressString(info.arq.m_destinationInfo[1]);
    }
  }
  return username;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323GatekeeperRequest::Response RegistrarGk::OnAdmission(H323GatekeeperARQ & info)
{

  PWaitAndSignal m(mutex);

  // security
  if(AdmissionPolicyCheck(info) == FALSE)
    return H323GatekeeperRequest::Reject;

  PString              srcUsername;
  H323TransportAddress srcHost;
  PString              dstUsername;
  H323TransportAddress dstHost;
  BOOL                 h323ToH323 = FALSE;

  srcUsername = GetAdmissionSrcUsername(info);
  if(!info.IsBehindNAT() && info.arq.HasOptionalField(H225_AdmissionRequest::e_srcCallSignalAddress))
    srcHost = info.arq.m_srcCallSignalAddress;
  else
    srcHost = info.GetReplyAddress();

  dstUsername = GetAdmissionDstUsername(info);
  if(info.arq.HasOptionalField(H225_AdmissionRequest::e_destCallSignalAddress))
    dstHost = info.arq.m_destCallSignalAddress;

  if(srcUsername == dstUsername)
  {
    info.SetRejectReason(H225_AdmissionRejectReason::e_undefinedReason);
    return H323GatekeeperRequest::Reject;
  }

  if(srcUsername != "" && dstUsername != "")
  {
    if(registrar->FindAccount(ACCOUNT_TYPE_H323, srcUsername) && registrar->FindAccount(ACCOUNT_TYPE_H323, dstUsername))
      h323ToH323 = TRUE;
    //PString processing_type = FreeMCU::Current().GetEndpointParamFromUrl("Processing", "");
    PString processing_type = "transcoding";
    if(h323ToH323 && processing_type == "forwarding")
      return H323GatekeeperServer::OnAdmission(info);
  }

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

  // set call identifier for security check incoming call
  RegistrarAccount *regAccount = NULL;
  if(srcUsername != "")
  {
    regAccount = registrar->FindAccountWithLock(ACCOUNT_TYPE_H323, srcUsername);
    if(regAccount)
    {
      regAccount->h323CallIdentifier = info.arq.m_callIdentifier.m_guid;
      regAccount->Unlock();
    }
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

RegistrarGk::~RegistrarGk()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RegistrarGk::RegistrarGk(MCUH323EndPoint *ep, Registrar *_registrar)
  : H323GatekeeperServer(*ep), registrar(_registrar)
{
  requireH235 = TRUE;

  totalBandwidth = UINT_MAX;      // Unlimited total bandwidth
  usedBandwidth = 0;              // None used so far
  defaultBandwidth = 2560;        // Enough for bidirectional G.711 and 64k H.261
  maximumBandwidth = 200000;      // 10baseX LAN bandwidth
  defaultTimeToLive = 3600;       // One hour, zero disables
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
  //requireH235 = FALSE;
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
