
#include "precompile.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUURL_SIP::MCUURL_SIP(const msg_t *msg, Directions dir)
  : MCUURL()
{
  sip_t *sip = sip_object(msg);
  su_home_t *home = msg_home(msg);
  if(sip == NULL || home == NULL)
    return;

  sip_addr_t *sip_from;
  sip_addr_t *sip_to;
  if(dir == DIRECTION_INBOUND)
  {
    sip_from = sip->sip_from;
    sip_to = sip->sip_to;
  }
  else
  {
    sip_from = sip->sip_to;
    sip_to = sip->sip_from;
  }

  // username
  if(sip_from->a_url->url_user && PString(sip_from->a_url->url_user) != "")
    username = sip_from->a_url->url_user;
  else if(sip->sip_contact && sip->sip_contact->m_url && PString(sip->sip_contact->m_url->url_user) != "")
    username = sip->sip_contact->m_url->url_user;
  username.Replace("sip:","",TRUE,0);
  username.Replace("\"","",TRUE,0);
  username = PURL::UntranslateString(username, PURL::QueryTranslation);

  // display name
  if(sip_from->a_display && PString(sip_from->a_display) != "")
    display_name = sip_from->a_display;
  else if(sip->sip_contact && sip->sip_contact->m_display && PString(sip->sip_contact->m_display) != "")
    display_name = sip->sip_contact->m_display;
  else
    display_name = username;
  display_name.Replace("sip:","",TRUE,0);
  display_name.Replace("\"","",TRUE,0);
  display_name = PURL::UntranslateString(display_name, PURL::QueryTranslation);

  // domain
  domain_name = sip_from->a_url->url_host;

  // remote application
  if(sip->sip_user_agent && sip->sip_user_agent->g_string)
    remote_application = sip->sip_user_agent->g_string;
  else if(sip->sip_server && sip->sip_server->g_string)
    remote_application = sip->sip_server->g_string;

  // get hostname,port,transport from su_addrinfo_t
  // not available after msg_dup or msg_copy
  su_addrinfo_t *addrinfo = msg_addrinfo((msg_t *)msg);
  if(addrinfo->ai_addr)
  {
    // hostname
    char ip[INET_ADDRSTRLEN] = {0};
    int ret = getnameinfo(addrinfo->ai_addr, addrinfo->ai_addrlen, ip, (socklen_t)sizeof(ip), NULL, 0, NI_NUMERICHOST);
    if(ret == 0)
      hostname = ip;
    // port
    port = ntohs(((struct sockaddr_in *)addrinfo->ai_addr)->sin_port);
    // transport
    if(addrinfo->ai_protocol == TPPROTO_UDP)      transport = "udp";
    else if(addrinfo->ai_protocol == TPPROTO_TCP) transport = "tcp";
    //else if(addrinfo->ai_protocol == TPPROTO_TLS) transport = "tls";
    else                                          transport = "*";
  }

  if(transport == "tcp")
  {
    if(PString(sip->sip_via->v_protocol).Find("TLS") != P_MAX_INDEX)
      transport = "tls";
    if(sip->sip_request && sip->sip_via->v_port)
      port = PString(sip->sip_via->v_port).AsInteger();
  }

  // url_party
  url_scheme = "sip";
  url_party = url_scheme+":"+username+"@"+hostname;
  if(port != 0)
    url_party += ":"+PString(port);
  if(transport != "" && transport != "*")
    url_party += ";transport="+transport;

  // local_username
  if(dir == DIRECTION_INBOUND && sip->sip_request && sip->sip_request->rq_url->url_user && PString(sip->sip_request->rq_url->url_user) != "")
    local_username = sip->sip_request->rq_url->url_user;
  else if(sip_to->a_url->url_user && PString(sip_to->a_url->url_user) != "")
    local_username = sip_to->a_url->url_user;
  local_username.Replace("sip:","",TRUE,0);
  local_username.Replace("\"","",TRUE,0);
  local_username = PURL::UntranslateString(local_username, PURL::QueryTranslation);

  // local_url
  /*
  if(OpenMCU::Current().GetSipEndpoint()->GetLocalSipAddress(local_url, msg) == TRUE)
  {
    MCUURL lurl(local_url);
    lurl.SetUserName(local_username);
    local_url = lurl.GetUrl();
  }
  */
  if(dir == DIRECTION_INBOUND && sip->sip_request)
  {
    PString local_hostname = sip->sip_request->rq_url->url_host;
    PString local_port = sip->sip_request->rq_url->url_port;
    if(local_port == "")
      local_port = "5060";
    local_url = url_scheme+":"+local_username+"@"+local_hostname+":"+local_port;
    if(transport != "" && transport != "*")
      local_url += ";transport="+transport;
  }
  else
    local_url = "*";

  PTRACE(1, "MCUURL_SIP url: " << url_party << " local_url: " << local_url);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ProxyAccount::SetStatus(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);
  if(sip)
  {
    status = sip->sip_status->st_status;
    status_phrase = sip->sip_status->st_phrase;
    if(sip->sip_expires)
    {
      if(sip->sip_expires->ex_delta != 0)
      {
        proxy_expires = sip->sip_expires->ex_delta;
      }
      else
      {
        status = 0;
        status_phrase = "Registration canceled";
      }
    }
  }
  PString event = "<font color=blue>"+roomname+" - "+username+"@"+domain+" status: "+status_phrase+"</font>";
  OpenMCU::Current().HttpWriteEvent(event);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString logMsgBuf;
void MCUSipLoggerFunc(void *logarg, char const *fmt, va_list ap)
{
  if(fmt == NULL)
    return;

#ifdef _WIN32
  PString trace = pvsprintf(fmt, ap);
  if(trace.IsEmpty()) return;
#else
  char *data = NULL;
  int ret = vasprintf(&data, fmt, ap);
  if(ret == -1 || data == NULL) return;
  PString trace = (const char *)data;
  free(data);
#endif
  if(trace.GetLength() < 2) return;
  if(trace == "   ") return;
  if(trace.Right(1) == "\n" && trace.Right(2) != "-\n") trace = trace.Left(trace.GetLength()-1);

  if(trace.Left(4) == "send")
  {
    logMsgBuf = "\n   "+trace;
    return;
  }
  else if(trace.Left(4) == "recv")
  {
    logMsgBuf = "\n   "+trace;
    return;
  }
  else if(trace.Find("---") != P_MAX_INDEX)
  {
    if(logMsgBuf.IsEmpty())
      return;
    logMsgBuf = logMsgBuf+trace;
    PRegularExpression RegEx("cseq: [0-9]* (options|info|publish|subscribe|notify)", PRegularExpression::Extended|PRegularExpression::IgnoreCase);
    if(logMsgBuf.FindRegEx(RegEx) == P_MAX_INDEX)
      PTRACE(1, logMsgBuf);
    else
      PTRACE(6, logMsgBuf);
    MCUTRACE(0, logMsgBuf);
    logMsgBuf = "";
    return;
  }
  if(logMsgBuf.IsEmpty())
  {
    MCUTRACE(1, trace);
  }
  else if(trace.Find("\n") == P_MAX_INDEX && !logMsgBuf.IsEmpty())
  {
    logMsgBuf = logMsgBuf+trace+"\n";
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GetSipCapabilityParams(PString capname, PString & name, int & pt, int & rate, PString & fmtp, PString & params)
{
  if(capname == "")
    return FALSE;

  MCUCapability *cap = MCUCapability::Create(capname);
  if(!cap)
    return FALSE;

  const OpalMediaFormat & mf = cap->GetMediaFormat();
  rate = mf.GetTimeUnits()*1000;
  // only for audio use the default fmtp
  if(fmtp == "" && rate != 90000)
  {
    for(PINDEX i = 0; i < mf.GetOptionCount(); i++)
    {
      if(mf.GetOption(i).GetFMTPName() != "" && mf.GetOption(i).GetFMTPDefault() != mf.GetOption(i).AsString())
        fmtp += mf.GetOption(i).GetFMTPName()+"="+mf.GetOption(i).AsString()+";";
    }
    params = mf.GetOptionInteger("Encoder Channels");
    if(params.AsInteger() < 2) params = "";
  }
  delete cap;

  name = capname.ToLower();
  if(name.Find("ulaw") != P_MAX_INDEX)         { name = "pcmu"; }
  else if(name.Find("alaw") != P_MAX_INDEX)    { name = "pcma"; }
  else if(name.Find("722.1") != P_MAX_INDEX)   { name = "g7221"; }
  else if(name.Find("722.2") != P_MAX_INDEX)   { name = "amr-wb"; }
  else if(name.Find("723") != P_MAX_INDEX)     { name = "g723"; }
  else if(name.Find("726-16") != P_MAX_INDEX)  { name = "g726-16"; }
  else if(name.Find("726-24") != P_MAX_INDEX)  { name = "g726-24"; }
  else if(name.Find("726-32") != P_MAX_INDEX)  { name = "g726-32"; }
  else if(name.Find("726-40") != P_MAX_INDEX)  { name = "g726-40"; }
  else if(name.Find("729-") != P_MAX_INDEX)    return FALSE;
  else if(name.Find("729a") != P_MAX_INDEX)    { name = "g729"; fmtp = "annexb=no;"; }
  else if(name.Find("h.263p") != P_MAX_INDEX)  { name = "h263-1998"; }
  else if(name.Find("mp4v-es") != P_MAX_INDEX) { name = "mp4v-es"; }
  else
  {
    name.Replace("{sw}","",TRUE,0);
    name.Replace(".","",TRUE,0);
    name = name.Left(name.Find("-")).Left(name.Find("_"));
  }

  if     (name == "pcmu")   { pt = 0; }
  else if(name == "pcma")   { pt = 8; }
  else if(name == "g723")   { pt = 4; }
  else if(name == "g722")   { pt = 9; rate = 8000; }
  else if(name == "g728")   { pt = 15; }
  else if(name == "g729")   { pt = 18; }
  else if(name == "h261")   { pt = 31; }
  else if(name == "h263")   { pt = 34; }
  else                      { pt = -1; }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CreateSipCaps(SipCapMapType & SipCaps, PString audio_section, PString video_section)
{
  MCUConfig cfg;
  PStringList keys;

  cfg = MCUConfig(audio_section);
  keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    if(keys[i].Right(4) == "fmtp" || keys[i].Right(7) == "payload") continue;
    if(cfg.GetBoolean(keys[i]) == FALSE) continue;

    PString capname = keys[i];
    if(capname.Right(4) != "{sw}") capname += "{sw}";
    PString fmtp = cfg.GetString(capname+"_fmtp");
    PString local_fmtp = cfg.GetString(capname+"_local_fmtp");

    SipCapability *sc = new SipCapability(capname);
    if(sc->format == "") { delete sc; sc = NULL; continue; }

    sc->media = MEDIA_TYPE_AUDIO;
    if(fmtp != "") sc->fmtp = fmtp;
    if(local_fmtp != "") sc->local_fmtp = local_fmtp;
    SipCaps.insert(SipCapMapType::value_type(SipCaps.size(), sc));
  }

  cfg = MCUConfig(video_section);
  keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    if(keys[i].Right(4) == "fmtp" || keys[i].Right(7) == "payload") continue;
    if(cfg.GetBoolean(keys[i]) == FALSE) continue;

    PString capname = keys[i];
    if(capname.Right(4) != "{sw}") capname += "{sw}";

    SipCapability *sc = new SipCapability(capname);
    if(sc->format == "") { delete sc; sc = NULL; continue; }

    sc->media = MEDIA_TYPE_VIDEO;
    SipCaps.insert(SipCapMapType::value_type(SipCaps.size(), sc));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString GetSipCallToken(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);
  PString callToken = PString(sip->sip_call_id->i_id);
  return callToken;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SipCapability * FindSipCap(SipCapMapType & SipCapMap, MediaTypes mtype, int payload)
{
  if(payload < 0) return NULL;
  for(SipCapMapType::iterator it = SipCapMap.begin(); it != SipCapMap.end(); it++)
  {
    if(it->second->media == mtype && it->second->payload == payload)
      return it->second;
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SipCapability * FindSipCap(SipCapMapType & SipCapMap, MediaTypes mtype, PString capname)
{
  for(SipCapMapType::iterator it = SipCapMap.begin(); it != SipCapMap.end(); it++)
  {
    if(it->second->media == mtype && (it->second->capname == capname || it->second->capname == capname+"{sw}"))
      return it->second;
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CheckPreferSipCap(SipCapMapType & SipCapMap, SipCapability *sc)
{
  for(SipCapMapType::iterator it = SipCapMap.begin(); it != SipCapMap.end(); it++)
  {
    if(it->second->preferred_cap && it->second->format == sc->format)
    {
      sc->cap = MCUCapability::Create(it->second->capname);
      if(sc->cap) sc->preferred_cap = TRUE;
      return;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ClearSipCaps(SipCapMapType & SipCaps)
{
  for(SipCapMapType::iterator it = SipCaps.begin(); it != SipCaps.end(); )
  {
    delete it->second;
    it->second = NULL;
    SipCaps.erase(it++);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SipCapability::Print()
{
 cout << "\r\n\r\n";
 cout << "Name: " << capname << " Format: " << format << " Payload: " << payload << " Mode: " << mode << "\r\n";
 cout << "Remote IP: " << remote_ip << " Remote port: " << remote_port << "\r\n";
 cout << "Secure: " << secure_type << "\r\n";
 cout << "Clock: " << clock << " Bandwidth: " << bandwidth << "\r\n";
 cout << "Parameters: " << fmtp << "\r\n\r\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString HTTPAuth::MakeResponse()
{
  if(scheme == "Basic")
  {
    return PBase64::Encode(username+":"+password);
  }
  else if(scheme == "Digest")
  {
    auth_response_t ar[1] = {{ sizeof(ar) }};
    auth_hexmd5_t ha1, hresponse;
    ar->ar_username = username;
    ar->ar_realm = realm;
    ar->ar_nonce = nonce;
    ar->ar_uri = uri;
    //auth_digest_ha1(ha1, username, realm, password);
    auth_digest_a1(ar, ha1, password);
    auth_digest_response(ar, hresponse, ha1, method, NULL, 0);
    return hresponse;
  }
  return "";
}

PString HTTPAuth::MakeAuthorizationStr()
{
  if(scheme == "Basic")
  {
    return scheme+" "+MakeResponse();
  }
  else if(scheme == "Digest")
  {
    return scheme
           +" username=\""+username+"\""
           +",realm="+realm
           +",nonce="+nonce
           +",response=\""+MakeResponse()+"\""
           +",uri=\""+uri+"\""
           +",algorithm="+algorithm;
  }
  return "";
}

PString HTTPAuth::MakeAuthenticateStr()
{
  if(scheme == "Basic")
    return scheme+" realm=\""+realm+"\"";
  else if(scheme == "Digest")
    return scheme+" realm=\""+realm+"\",nonce=\""+nonce+"\",algorithm="+algorithm;
  return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSipConnection::MCUSipConnection(MCUH323EndPoint *_ep, const PString & _callToken)
  :MCUH323Connection(*_ep, 0, NULL)
{
  sep = OpenMCU::Current().GetSipEndpoint();
  connectionState = NoConnectionActive;
  connectionType = CONNECTION_TYPE_SIP;
  direction = DIRECTION_INBOUND;
  callToken = _callToken;
  trace_section = "SIP Connection "+callToken+": ";

  remoteUserName = "";
  remoteDisplayName = "";
  remoteApplication = "SIP terminal";
  requestedRoom = OpenMCU::Current().GetDefaultRoomName();
  connectedTime = PTime();

  audio_rtp_port = 0;
  video_rtp_port = 0;
  rtp_proto = "RTP";
  stun = NULL;

  c_sip_msg = NULL;
  leg = NULL;
  orq_invite = NULL;

  scap = -1;
  vcap = -1;

  MCUTRACE(1, trace_section << "constructor");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSipConnection::~MCUSipConnection()
{
  ClearSipCaps(LocalSipCaps);
  ClearSipCaps(RemoteSipCaps);

  if(stun != NULL)
    delete stun;

  MCUTRACE(1, trace_section << "destructor");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipConnection::Init(Directions _direction, const msg_t *msg)
{
  direction = _direction;

  su_home_t *home = msg_home(msg);
  sip_t *sip = sip_object(msg);
  sip_addr_t *sip_from = NULL;
  sip_addr_t *sip_to = NULL;
  if(direction == DIRECTION_INBOUND)
  {
    MCUURL_SIP url(msg, DIRECTION_INBOUND);
    ruri_str = url.GetUrl();
    remotePartyAddress = ruri_str;
    contact_str = url.GetLocalUrl();
    sip_from = sip->sip_to;
    sip_to = sip->sip_from;
  }
  else if(direction == DIRECTION_OUTBOUND)
  {
    ruri_str = url_as_string(home, sip->sip_request->rq_url);
    remotePartyAddress = ruri_str;
    contact_str = url_as_string(home, sip->sip_contact->m_url);
    sip_from = sip->sip_from;
    sip_to = sip->sip_to;
  }

  // detect local_ip, nat_ip and create rtp sessions
  if(CreateDefaultRTPSessions() == FALSE)
    return FALSE;

  // create local capability list
  CreateLocalSipCaps();

  MCUURL contact_url(contact_str);
  MCUURL remote_url(ruri_str);

  // requested room
  requestedRoom = contact_url.GetUserName();
  if(requestedRoom == "")
    requestedRoom = OpenMCU::Current().GetDefaultRoomName();

  // contact
  contact_url.SetUserName(requestedRoom);
  contact_url.SetHostName(nat_ip);
  if(nat_port != "")
    contact_url.SetPort(nat_port.AsInteger());
  contact_url.SetTransport(remote_url.GetTransport());
  contact_str = contact_url.GetUrl();

  // create call leg
  leg = nta_leg_tcreate(sep->GetAgent(), wrap_invite_request_cb, (nta_leg_magic_t *)this,
                        SIPTAG_FROM(sip_from),
                        SIPTAG_TO(sip_to),
			SIPTAG_CALL_ID(sip->sip_call_id),
                        SIPTAG_SERVER_STR((const char*)(SIP_USER_AGENT)),
                        TAG_END());

  if(direction == DIRECTION_INBOUND)
  {
    // save the request message
    c_sip_msg = msg_dup(msg);
    msg_addr_copy(c_sip_msg, msg);
    // waiting ACK signal
    connectionState = AwaitingSignalConnect;
  }
  else if(direction == DIRECTION_OUTBOUND)
  {
  }

  // endpoint parameters
  rtp_proto = GetEndpointParam(RtpProtoKey, "RTP");
  remote_bw = GetEndpointParam(BandwidthToKey, "0").AsInteger();

  MCUTRACE(1, trace_section << "contact: " << contact_str << " ruri: " << ruri_str);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSipConnection * MCUSipConnection::CreateConnection(Directions _direction, const PString & _callToken, const msg_t *msg)
{
  MCUH323EndPoint *ep = &OpenMCU::Current().GetEndpoint();
  MCUSipConnection *conn = new MCUSipConnection(ep, _callToken);
  if(!conn->Init(_direction, msg))
  {
    delete conn;
    conn = NULL;
  }

  if(conn != NULL)
    conn->OnCreated();

  return conn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipConnection::ClearCall(H323Connection::CallEndReason reason, const PString & event)
{
  callEndReasonEvent = event;
  return ClearCall(reason);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipConnection::ClearCall(H323Connection::CallEndReason reason)
{
  if(PThread::Current() != sep)
  {
    PString *cmd = new PString("clear_call:"+callToken+","+PString(reason));
    MCUTRACE(1, trace_section << "added to sip queue: " << *cmd);
    sep->GetSipQueue().Push(cmd);
    return TRUE;
  }

  if(reason == EndedByLocalUser)
  {
    if(connectionState == EstablishedConnection)
    {
      SendRequest(SIP_METHOD_BYE);
    }
    else if(connectionState == AwaitingSignalConnect)
    {
      if(direction == DIRECTION_INBOUND)
        ReqReply(c_sip_msg, SIP_603_DECLINE);
      else if(direction == DIRECTION_OUTBOUND)
        nta_outgoing_cancel(orq_invite);
    }
  }
  return MCUH323Connection::ClearCall(reason);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::CleanUpOnCallEnd()
{
  PTRACE(1, trace_section << "CleanUpOnCallEnd reason: " << callEndReason);

  connectionState = ShuttingDownConnection;

  connMutex.Wait();
  if(leg) nta_leg_destroy(leg);
  if(orq_invite) nta_outgoing_destroy(orq_invite);
  if(c_sip_msg) msg_destroy(c_sip_msg);

  connMutex.Signal();
  DeleteMediaChannels();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipConnection::DetermineLocalIp(const PString & _address)
{
  PString address = _address;
  if(address == "")
    address = ruri_str;

  PString remote_host = MCUURL(address).GetHostName();
  PString remote_port = MCUURL(address).GetPort();
  if(MCUSocket::GetFromIP(local_ip, remote_host, remote_port) == FALSE)
  {
    MCUTRACE(1, trace_section << "unable to determine the local IP address for " << address);
    return FALSE;
  }

  MCUTRACE(1, trace_section << "found local IP address \"" << local_ip << "\"");
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipConnection::DetermineNAT()
{
  // NAT router IP
  PString nat_addr = GetEndpointParam(NATRouterIPKey);
  nat_ip = nat_addr.Tokenise(":")[0];
  nat_port = nat_addr.Tokenise(":")[1];
  if(nat_ip != "" && PIPSocket::Address(nat_ip).IsValid() == FALSE)
  {
    MCUTRACE(1, trace_section << "incorrect NAT router IP " << nat_ip);
    return FALSE;
  }

#ifdef P_STUN
  // STUN server
  if(nat_ip == "")
  {
    PString address = GetEndpointParam(NATStunServerKey);
    if(address != "")
    {
      stun = sep->GetPreferedStun(address);
      PIPSocket::Address extAddress;
      if(stun == NULL || stun->GetExternalAddress(extAddress) == FALSE)
      {
        MCUTRACE(1, trace_section << "STUN server error");
        return FALSE;
      }
      nat_ip = extAddress.AsString();
    }
  }
#endif

  // remoteIsNAT ???
  remoteIsNAT = FALSE;

  MCUTRACE(1, trace_section << "found NAT IP address \"" << nat_ip << "\"");
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipConnection::CreateDefaultRTPSessions()
{
  // local ip
  if(DetermineLocalIp() == FALSE)
    return FALSE;

  // NAT
  if(DetermineNAT() == FALSE)
    return FALSE;

  // if empty nat_ip set as local_ip
  if(nat_ip == "")
    nat_ip = local_ip;

  MCUSIP_RTP_UDP *session = NULL;

  session = CreateRTPSession(MEDIA_TYPE_AUDIO);
  session->DecrementReference();

  session = CreateRTPSession(MEDIA_TYPE_VIDEO);
  session->DecrementReference();

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSIP_RTP_UDP *MCUSipConnection::CreateRTPSession(MediaTypes media)
{
  int id = (media == MEDIA_TYPE_AUDIO) ? RTP_Session::DefaultAudioSessionID : RTP_Session::DefaultVideoSessionID;
  MCUSIP_RTP_UDP *session = (MCUSIP_RTP_UDP *)(rtpSessions.UseSession(id));
  if(session == NULL)
  {
    session = new MCUSIP_RTP_UDP(
#ifdef H323_RTP_AGGREGATE
                useRTPAggregation ? ep.GetRTPAggregator() : NULL,
#endif
                id, remoteIsNAT);

    session->SetUserData(new RTP_UserData());

    // add session to RTP_SessionManager()
    rtpSessions.AddSession(session);

    PIPSocket::Address localIP(local_ip);
    session->Open(localIP, ep.GetRtpIpPortBase(), ep.GetRtpIpPortMax(), ep.GetRtpIpTypeofService(), *this, (PNatMethod *)stun, NULL);

    if(media == MEDIA_TYPE_AUDIO)
      audio_rtp_port = session->GetLocalDataPort();
    else if(media == MEDIA_TYPE_VIDEO)
      video_rtp_port = session->GetLocalDataPort();

    MCUTRACE(1, trace_section << "create " << ((media == MEDIA_TYPE_AUDIO) ? "audio" : "video") <<
                " RTP session, port=" << ((media == MEDIA_TYPE_AUDIO) ? audio_rtp_port : video_rtp_port));
  }
  return session;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSIP_RTP_UDP *MCUSipConnection::CreateRTPSession(SipCapability *sc)
{
  MCUSIP_RTP_UDP *session = CreateRTPSession(sc->media);

  // remoteIsNAT ???
  PIPSocket::Address remoteIP(sc->remote_ip);
  session->SetRemoteSocketInfo(remoteIP,sc->remote_port,TRUE);

  return session;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipConnection::CreateMediaChannel(MediaTypes mtype, int pt, int rtp_dir)
{
  if(pt < 0) return TRUE;
  SipCapability *sc = FindSipCap(RemoteSipCaps, mtype, pt);
  if(!sc) return FALSE;

  MCUCapability * cap = sc->cap;
  if(!cap) return FALSE;

  if(PIPSocket::Address(sc->remote_ip).IsValid() == FALSE || sc->remote_port == 0)
    return FALSE;

  PTRACE(1, trace_section << "create " << ((rtp_dir == 0) ? "receive" : "transmit") << " channel " << pt);

  // При исходящем вызове payload случайный, терминал может указать в ответе другой payload.
  // Для исходящего потока использовать payload полученный от терминала, для входящего исходный payload.
  if(direction == DIRECTION_OUTBOUND && rtp_dir == 0)
  {
    SipCapability *local_sc = FindSipCap(LocalSipCaps, mtype, sc->capname);
    if(local_sc && local_sc->payload > 0 && local_sc->payload != pt)
    {
      pt = local_sc->payload;
      MCUTRACE(1, trace_section << "change payload type " << sc->capname << " " << sc->payload << "->" << local_sc->payload);
    }
  }

  MCUSIP_RTP_UDP *session = CreateRTPSession(sc);
  if(session == NULL)
    return FALSE;

  if(sc->secure_type == SECURE_TYPE_ZRTP)
  {
    // master zrtp session
    if(sc->media == MEDIA_TYPE_AUDIO)
    {
      session->SetMaster(TRUE);
      session->SetConnection(this);
    }
    session->CreateZRTP();
  }
  if(sc->secure_type == SECURE_TYPE_DTLS_SRTP)
  {
    //session->CreateDTLS(rtp_dir, sc->dtls_fp_type, sc->dtls_fp);
  }
  if(sc->secure_type == SECURE_TYPE_SRTP)
  {
    if(sc->srtp_local_key == "")
    {
      sc->srtp_local_type = sc->srtp_remote_type;
      if(sc->srtp_local_type == AES_CM_128_HMAC_SHA1_80)
      {
        if(sc->media == MEDIA_TYPE_AUDIO)
        {
          if(key_audio80 != "") sc->srtp_local_key = key_audio80;
          else                  sc->srtp_local_key = key_audio80 = srtp_get_random_keysalt();
        }
        else if(sc->media == MEDIA_TYPE_VIDEO)
        {
          if(key_video80 != "") sc->srtp_local_key = key_video80;
          else                  sc->srtp_local_key = key_video80 = srtp_get_random_keysalt();
        }
      }
      else if(sc->srtp_local_type == AES_CM_128_HMAC_SHA1_32)
      {
        if(sc->media == MEDIA_TYPE_AUDIO)
        {
          if(key_audio32 != "") sc->srtp_local_key = key_audio32;
          else                  sc->srtp_local_key = key_audio32 = srtp_get_random_keysalt();
        }
        else if(sc->media == MEDIA_TYPE_VIDEO)
        {
          if(key_video32 != "") sc->srtp_local_key = key_video32;
          else                  sc->srtp_local_key = key_video32 = srtp_get_random_keysalt();
        }
      }
    }
    if(rtp_dir == 0) session->CreateSRTP(rtp_dir, sc->srtp_remote_type, sc->srtp_remote_key);
    else             session->CreateSRTP(rtp_dir, sc->srtp_local_type, sc->srtp_local_key);
  }

  MCUSIP_RTPChannel *channel =
    new MCUSIP_RTPChannel(*this, *cap, ((rtp_dir == 0) ? H323Channel::IsReceiver : H323Channel::IsTransmitter), *session);

  if(pt >= RTP_DataFrame::DynamicBase && pt <= RTP_DataFrame::MaxPayloadType)
    channel->SetDynamicRTPPayloadType(pt);

  if(rtp_dir == 0)
    sc->inpChan = channel;
  else
    sc->outChan = channel;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::CreateMediaChannels()
{
  CreateMediaChannel(MEDIA_TYPE_AUDIO, scap, 0);
  CreateMediaChannel(MEDIA_TYPE_AUDIO, scap, 1);
  CreateMediaChannel(MEDIA_TYPE_VIDEO, vcap, 0);
  CreateMediaChannel(MEDIA_TYPE_VIDEO, vcap, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::StartMediaChannel(MediaTypes mtype, int pt, int dir)
{
  if(pt < 0) return;
  SipCapability *sc = FindSipCap(RemoteSipCaps, mtype, pt);
  if(!sc) return;
  PTRACE(1, trace_section << "start " << ((dir == 0) ? "receive" : "transmit") << " channel " << pt);
  if(dir == 0 && (sc->mode&2) && sc->inpChan && !sc->inpChan->IsRunning()) sc->inpChan->Start();
  if(dir == 1 && (sc->mode&1) && sc->outChan && !sc->outChan->IsRunning()) sc->outChan->Start();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::StartMediaChannels()
{
  StartMediaChannel(MEDIA_TYPE_AUDIO, scap, 0);
  StartMediaChannel(MEDIA_TYPE_AUDIO, scap, 1);
  StartMediaChannel(MEDIA_TYPE_VIDEO, vcap, 0);
  StartMediaChannel(MEDIA_TYPE_VIDEO, vcap, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::DeleteMediaChannel(MediaTypes mtype, int pt, int dir)
{
  if(pt < 0) return;
  SipCapability *sc = FindSipCap(RemoteSipCaps, mtype, pt);
  if(!sc) return;
  PTRACE(1, trace_section << "delete " << ((dir == 0) ? "receive" : "transmit") << " channel " << pt);
  if(dir==0 && sc->inpChan) { sc->inpChan->CleanUpOnTermination(); delete sc->inpChan; sc->inpChan = NULL; }
  if(dir==1 && sc->outChan) { sc->outChan->CleanUpOnTermination(); delete sc->outChan; sc->outChan = NULL; }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::DeleteMediaChannels()
{
  DeleteMediaChannel(MEDIA_TYPE_AUDIO, scap, 0);
  DeleteMediaChannel(MEDIA_TYPE_AUDIO, scap, 1);
  DeleteMediaChannel(MEDIA_TYPE_VIDEO, vcap, 0);
  DeleteMediaChannel(MEDIA_TYPE_VIDEO, vcap, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::CreateLocalSipCaps()
{
  PString audio_capname = GetEndpointParam(AudioCodecKey);
  if(audio_capname != "" && audio_capname.Right(4) != "{sw}")
    audio_capname += "{sw}";
  PString video_capname = GetEndpointParam(VideoCodecKey, false);
  PString video_resolution = GetEndpointParam(VideoResolutionKey, false);
  if(video_capname == "")
  {
    video_capname = GetEndpointParam(VideoCodecKey);
    video_resolution = GetEndpointParam(VideoResolutionKey);
  }
  if(video_capname != "" && video_capname.Right(4) != "{sw}")
    video_capname += "{sw}";
  PString video_fmtp = GetEndpointParam(VideoFmtpKey);
  unsigned frame_rate = GetEndpointParam(FrameRateFromKey, "0").AsInteger();
  unsigned bandwidth = GetEndpointParam(BandwidthFromKey, "0").AsInteger();

  LocalSipCaps.clear();
  for(SipCapMapType::iterator it = sep->GetBaseSipCaps().begin(); it != sep->GetBaseSipCaps().end(); it++)
  {
    SipCapability *base_sc = it->second;

    if(SkipCapability(base_sc->capname, connectionType))
      continue;

    if(base_sc->media == MEDIA_TYPE_AUDIO && audio_capname != "" && audio_capname != base_sc->capname)
      continue;
    if(base_sc->media == MEDIA_TYPE_VIDEO && video_capname != "" && video_capname != base_sc->capname)
      continue;

    // create new SipCapability()
    SipCapability *local_sc = new SipCapability(*base_sc);

    if(base_sc->media == MEDIA_TYPE_AUDIO && audio_capname != "")
    {
      local_sc->preferred_cap = TRUE;
    }
    if(base_sc->media == MEDIA_TYPE_VIDEO)
    {
      if(video_capname != "")
      {
        local_sc->preferred_cap = TRUE;
        if(video_resolution != "")
        {
          local_sc->video_width = video_resolution.Tokenise("x")[0].AsInteger();
          local_sc->video_height = video_resolution.Tokenise("x")[1].AsInteger();
        }
        if(video_fmtp != "")
          local_sc->fmtp = video_fmtp;
      }
      local_sc->video_frame_rate = frame_rate;
      local_sc->bandwidth = bandwidth;
    }

    LocalSipCaps.insert(SipCapMapType::value_type(LocalSipCaps.size(), local_sc));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::CreateSdpOk()
{
  SipCapMapType SipCaps;
  if(scap >= 0)
  {
    SipCapability *remote_sc = FindSipCap(RemoteSipCaps, MEDIA_TYPE_AUDIO, scap);
    if(remote_sc)
    {
      SipCapability *sc = new SipCapability(*remote_sc);
      SipCapability *local_sc = FindSipCap(LocalSipCaps, MEDIA_TYPE_AUDIO, sc->capname);
      if(local_sc)
      {
        // send back the received fmtp
        if(local_sc->fmtp != "") sc->fmtp = local_sc->fmtp;
      }
      SipCaps.insert(SipCapMapType::value_type(SipCaps.size(), sc));
    }
  }
  if(vcap >= 0)
  {
    SipCapability *remote_sc = FindSipCap(RemoteSipCaps, MEDIA_TYPE_VIDEO, vcap);
    if(remote_sc)
    {
      SipCapability *sc = new SipCapability(*remote_sc);
      SipCapability *local_sc = FindSipCap(LocalSipCaps, MEDIA_TYPE_VIDEO, sc->capname);
      if(local_sc)
      {
        // send back the received fmtp
        if(local_sc->fmtp != "") sc->fmtp = local_sc->fmtp;
      }
      SipCaps.insert(SipCapMapType::value_type(SipCaps.size(), sc));
    }
  }
  sdp_ok_str = CreateSdpStr(SipCaps);
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::CreateSdpInvite()
{
  SipCapMapType SipCaps = LocalSipCaps;
  sdp_invite_str = CreateSdpStr(SipCaps);
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sdp_rtpmap_t * MCUSipConnection::CreateSdpRtpmap(su_home_t *sess_home, SipCapability *sc)
{
  sdp_rtpmap_t *rm = (sdp_rtpmap_t *)su_salloc(sess_home, sizeof(*rm));
  rm->rm_predef = 0;
  rm->rm_pt = sc->payload;
  rm->rm_encoding = PStringToChar(sc->format.ToUpper());
  rm->rm_rate = sc->clock;
  if(sc->fmtp != "") rm->rm_fmtp = PStringToChar(sc->fmtp);
  if(sc->params != "") rm->rm_params = PStringToChar(sc->params);
  return rm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sdp_media_t * MCUSipConnection::CreateSdpMedia(SipCapMapType & LocalCaps, su_home_t *sess_home, sdp_media_e m_type, sdp_proto_e m_proto)
{
  sdp_media_t *m = (sdp_media_t *)su_salloc(sess_home, sizeof(*m));
  m->m_type = m_type;
  m->m_proto = m_proto;

  if(m_type == sdp_media_audio)
    m->m_port = audio_rtp_port;
  else if(m_type == sdp_media_video)
    m->m_port = video_rtp_port;

  int dyn_pt = 96;
  sdp_rtpmap_t *rm = NULL; // rtpmap iterator
  for(SipCapMapType::iterator it = LocalCaps.begin(); it != LocalCaps.end(); it++)
  {
    SipCapability *sc = it->second;

    if(m_type == sdp_media_audio && sc->media != MEDIA_TYPE_AUDIO)
      continue;
    else if(m_type == sdp_media_video && sc->media != MEDIA_TYPE_VIDEO)
      continue;

    if(m->m_mode == 0)
      m->m_mode = sc->mode;

    // check payload number
    for(SipCapMapType::iterator it = LocalCaps.begin(); it != LocalCaps.end(); it++)
    {
      if(it->second->payload < 0)
        continue;
      if(dyn_pt == it->second->payload)
        dyn_pt++;
      if(it->second->format == sc->format && it->second->clock == sc->clock && it->second->fmtp == sc->fmtp && it->second->params == sc->params)
        sc->payload = it->second->payload;
    }
    if(sc->payload < 0)
      sc->payload = dyn_pt++;
    if(sc->payload > 127)
      continue;

    sdp_rtpmap_t *rm_new = CreateSdpRtpmap(sess_home, sc);
    if(!rm_new)
      continue;

    // do not duplicate capability
    sdp_rtpmap_t *rm_find = sdp_rtpmap_find_matching(m->m_rtpmaps, rm_new);
    if(rm_find)
    {
      if(sc->fmtp == "")
        continue;
      else if(!rm_find->rm_fmtp)
      {
        rm_find->rm_fmtp = PStringToChar(sc->fmtp);
        continue;
      }
    }

    // srtp
    if(direction == DIRECTION_INBOUND)
    {
      if(sc->secure_type == SECURE_TYPE_SRTP && m->m_proto != sdp_proto_srtp)
      {
        m->m_proto = sdp_proto_srtp;
        if(sc->srtp_local_type != "" && sc->srtp_local_key != "")
        {
          sdp_attribute_t *a = NULL;
          a = m->m_attributes = CreateSdpAttr(sess_home, "crypto", "1 "+sc->srtp_local_type+" inline:"+sc->srtp_local_key);
          //a = a->a_next = CreateSdpAttr(sess_home, "encryption", "optional");
        }
      }
    }

    if(!m->m_rtpmaps)
      rm = m->m_rtpmaps = rm_new;
    else
      rm = rm->rm_next = rm_new;
  }
  if(!m->m_rtpmaps)
    return NULL;
  return m;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sdp_attribute_t * MCUSipConnection::CreateSdpAttr(su_home_t *sess_home, PString m_name, PString m_value)
{
  sdp_attribute_t *a = (sdp_attribute_t *)su_salloc(sess_home, sizeof(*a));
  a->a_name = PStringToChar(m_name);
  a->a_value = PStringToChar(m_value);
  return a;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString MCUSipConnection::CreateSdpStr(SipCapMapType & LocalCaps)
{
  // create sdp session
  su_home_t *sess_home = su_home_create();
  sdp_session_t *sess = (sdp_session_t *)su_salloc(sess_home, sizeof(*sess));
  sdp_origin_t *o = (sdp_origin_t *)su_salloc(sess_home, sizeof(*o));
  sdp_time_t *t = (sdp_time_t *)su_salloc(sess_home, sizeof(*t));
  sdp_connection_t *c = (sdp_connection_t *)su_salloc(sess_home, sizeof(*c));
  sdp_bandwidth_t *b = (sdp_bandwidth_t *)su_salloc(sess_home, sizeof(*b));

  sess->sdp_origin = o;
  sess->sdp_time = t;		/* zero time is fine for SIP */
  sess->sdp_connection = c;
  sess->sdp_subject = "Talk";

  o->o_username = PStringToChar(requestedRoom);
  o->o_id = rand();
  o->o_version = 1;
  o->o_address = c;

  c->c_nettype = sdp_net_in;
  c->c_addrtype = sdp_addr_ip4;
  c->c_address = PStringToChar(nat_ip);

  if(remote_bw)
  {
    b->b_modifier = sdp_bw_as;
    b->b_value = remote_bw;
    sess->sdp_bandwidths = b;
  }

  // create default RTP media
  sdp_media_t *m_audio_rtp = CreateSdpMedia(LocalCaps, sess_home, sdp_media_audio, sdp_proto_rtp);
  sdp_media_t *m_video_rtp = CreateSdpMedia(LocalCaps, sess_home, sdp_media_video, sdp_proto_rtp);
  sdp_media_t *m_audio_srtp = NULL;
  sdp_media_t *m_video_srtp = NULL;
  sdp_media_t *m = NULL;

  if(direction == DIRECTION_OUTBOUND)
  {
    // check proto
    if(rtp_proto == "SRTP")
    {
      m_audio_srtp = m_audio_rtp;
      m_audio_rtp = NULL;
      m_video_srtp = m_video_rtp;
      m_video_rtp = NULL;
    }
    if(rtp_proto == "SRTP/RTP")
    {
      if(m_audio_rtp) m_audio_srtp = sdp_media_dup(sess_home, m_audio_rtp, sess);
      if(m_video_rtp) m_video_srtp = sdp_media_dup(sess_home, m_video_rtp, sess);
    }
    // init SRTP
    if(m_audio_srtp)
    {
      m_audio_srtp->m_proto = sdp_proto_srtp;
      key_audio80 = srtp_get_random_keysalt();
      key_audio32 = srtp_get_random_keysalt();
      sdp_attribute_t *a = NULL;
      a = m_audio_srtp->m_attributes = CreateSdpAttr(sess_home, "crypto", "1 AES_CM_128_HMAC_SHA1_80 inline:"+key_audio80);
      a = a->a_next = CreateSdpAttr(sess_home, "crypto", "2 AES_CM_128_HMAC_SHA1_32 inline:"+key_audio32);
      //a = a->a_next = CreateSdpAttr(sess_home, "encryption", "optional");
    }
    if(m_video_srtp)
    {
      m_video_srtp->m_proto = sdp_proto_srtp;
      key_video80 = srtp_get_random_keysalt();
      key_video32 = srtp_get_random_keysalt();
      sdp_attribute_t *a = NULL;
      a = m_video_srtp->m_attributes = CreateSdpAttr(sess_home, "crypto", "1 AES_CM_128_HMAC_SHA1_80 inline:"+key_video80);
      a = a->a_next = CreateSdpAttr(sess_home, "crypto", "2 AES_CM_128_HMAC_SHA1_32 inline:"+key_video32);
      //a = a->a_next = CreateSdpAttr(sess_home, "encryption", "optional");
    }
  }

  // add media to session
  if(m_audio_srtp)
  {
    if(sess->sdp_media) m = m->m_next = m_audio_srtp;
    else                m = sess->sdp_media = m_audio_srtp;
  }
  if(m_audio_rtp)
  {
    if(sess->sdp_media) m = m->m_next = m_audio_rtp;
    else                m = sess->sdp_media = m_audio_rtp;
  }
  if(m_video_srtp)
  {
    if(sess->sdp_media) m = m->m_next = m_video_srtp;
    else                m = sess->sdp_media = m_video_srtp;
  }
  if(m_video_rtp)
  {
    if(sess->sdp_media) m = m->m_next = m_video_rtp;
    else                m = sess->sdp_media = m_video_rtp;
  }

  char buffer[16384];
  sdp_printer_t *printer = sdp_print(sess_home, sess, buffer, sizeof(buffer), 0);
  sdp_printer_free(printer);

  su_home_check(sess_home);
  su_home_unref(sess_home);

  return buffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_H261(SipCapMapType & LocalCaps, SipCapability *sc)
{
  PStringArray keys = sc->fmtp.Tokenise(";");
  PString mpiname;

  if(!sc->cap)
  {
    if(FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, "H.261{sw}"))
      sc->cap = MCUCapability::Create("H.261{sw}");
    if(!sc->cap)
      return;
  }

  SipCapability *local_sc = FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, sc->cap->GetFormatName());
  if(local_sc->video_width) sc->video_width = local_sc->video_width;
  if(local_sc->video_height) sc->video_height = local_sc->video_height;
  if(local_sc->video_frame_rate) sc->video_frame_rate = local_sc->video_frame_rate;
  if(local_sc->bandwidth) sc->bandwidth = local_sc->bandwidth;

  if(sc->video_width == 0 && sc->video_height == 0)
  {
    for(int kn=0; kn<keys.GetSize(); kn++)
    {
      if(keys[kn].Find("QCIF=")==0 || keys[kn].Find("CIF=")==0)
      {
        mpiname = keys[kn].Tokenise("=")[0];
        GetParamsH263(mpiname, sc->video_width, sc->video_height);
        break;
      }
    }
  }

  if(sc->cap)
  {
    OpalMediaFormat & wf = sc->cap->GetWritableMediaFormat();
    SetFormatParams(wf, sc->video_width, sc->video_height, sc->video_frame_rate, sc->bandwidth);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_H263(SipCapMapType & LocalCaps, SipCapability *sc)
{
  PStringArray keys = sc->fmtp.Tokenise(";");
  int f = 0; // annex f
  PString mpiname;

  for(int kn=0; kn<keys.GetSize(); kn++)
  { if(keys[kn] == "F=1") { sc->fmtp = "F=1;"; f=1; break; } }

  if(!sc->cap)
  {
    if(FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, "H.263{sw}"))
      sc->cap = MCUCapability::Create("H.263{sw}");
    if(!sc->cap)
      return;
  }

  SipCapability *local_sc = FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, sc->cap->GetFormatName());
  if(local_sc->video_width) sc->video_width = local_sc->video_width;
  if(local_sc->video_height) sc->video_height = local_sc->video_height;
  if(local_sc->video_frame_rate) sc->video_frame_rate = local_sc->video_frame_rate;
  if(local_sc->bandwidth) sc->bandwidth = local_sc->bandwidth;

  if(sc->video_width == 0 && sc->video_height == 0)
  {
    for(int kn=0; kn<keys.GetSize(); kn++)
    {
      if(keys[kn].Find("SQCIF=")==0 || keys[kn].Find("QCIF=")==0 || keys[kn].Find("CIF=")==0 || keys[kn].Find("CIF4=")==0 || keys[kn].Find("CIF16=")==0)
      {
        mpiname = keys[kn].Tokenise("=")[0];
        GetParamsH263(mpiname, sc->video_width, sc->video_height);
        break;
      }
    }
  }

  if(sc->cap)
  {
    OpalMediaFormat & wf = sc->cap->GetWritableMediaFormat();
    SetFormatParams(wf, sc->video_width, sc->video_height, sc->video_frame_rate, sc->bandwidth);
    if(f) wf.SetOptionBoolean("_advancedPrediction", f);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_H263p(SipCapMapType & LocalCaps, SipCapability *sc)
{
  PStringArray keys = sc->fmtp.Tokenise(";");
  int f = 0, d = 0, e = 0, g = 0; // annexes
  PString mpiname;

  for(int kn=0; kn<keys.GetSize(); kn++)
  {
    if(keys[kn] == "F=1") { sc->fmtp += "F=1;"; f=1; }
    else if(keys[kn] == "D=1") { sc->fmtp += "D=1;"; d=1; }
    else if(keys[kn] == "E=1") { sc->fmtp += "E=1;"; e=1; }
    else if(keys[kn] == "G=1") { sc->fmtp += "G=1;"; g=1; }
  }

  if(!sc->cap)
  {
    if(FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, "H.263p{sw}"))
      sc->cap = MCUCapability::Create("H.263p{sw}");
    if(!sc->cap)
      return;
  }

  SipCapability *local_sc = FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, sc->cap->GetFormatName());
  if(local_sc->video_width) sc->video_width = local_sc->video_width;
  if(local_sc->video_height) sc->video_height = local_sc->video_height;
  if(local_sc->video_frame_rate) sc->video_frame_rate = local_sc->video_frame_rate;
  if(local_sc->bandwidth) sc->bandwidth = local_sc->bandwidth;

  if(sc->video_width == 0 && sc->video_height == 0)
  {
    for(int kn=0; kn<keys.GetSize(); kn++)
    {
      if(keys[kn].Find("SQCIF=")==0 || keys[kn].Find("QCIF=")==0 || keys[kn].Find("CIF=")==0 || keys[kn].Find("CIF4=")==0 || keys[kn].Find("CIF16=")==0)
      {
        mpiname = keys[kn].Tokenise("=")[0];
        GetParamsH263(mpiname, sc->video_width, sc->video_height);
        break;
      }
    }
  }

  if(sc->cap)
  {
    OpalMediaFormat & wf = sc->cap->GetWritableMediaFormat();
    SetFormatParams(wf, sc->video_width, sc->video_height, sc->video_frame_rate, sc->bandwidth);
    if(f) wf.SetOptionBoolean("_advancedPrediction", f);
    if(d) wf.SetOptionBoolean("_unrestrictedVector", d);
    if(e) wf.SetOptionBoolean("_arithmeticCoding", e);
    if(g) wf.SetOptionBoolean("_pbFrames", g);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_H264(SipCapMapType & LocalCaps, SipCapability *sc)
{
  PStringArray keys = sc->fmtp.Tokenise(";");
  unsigned profile = 0, level = 0, level_h241 = 0, max_fs = 0, max_mbps = 0, max_br = 0;
  PString sprop;

  for(int kn = 0; kn < keys.GetSize(); kn++)
  {
    if(keys[kn].Find("sprop-parameter-sets=") == 0) { sprop = keys[kn].Right(keys[kn].GetLength() - PString("sprop-parameter-sets=").GetLength()); }
  }

  if(!sc->cap)
  {
    if(FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, "H.264{sw}"))
      sc->cap = MCUCapability::Create("H.264{sw}");
    if(!sc->cap)
      return;
  }

  SipCapability *local_sc = FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, sc->cap->GetFormatName());
  if(local_sc->video_width) sc->video_width = local_sc->video_width;
  if(local_sc->video_height) sc->video_height = local_sc->video_height;
  if(local_sc->video_frame_rate) sc->video_frame_rate = local_sc->video_frame_rate;
  if(local_sc->bandwidth) sc->bandwidth = local_sc->bandwidth;

  if(sc->video_width == 0 && sc->video_height == 0)
  {
    for(int kn = 0; kn < keys.GetSize(); kn++)
    {
      if(keys[kn].Find("profile-level-id=") == 0) { int p = (keys[kn].Tokenise("=")[1]).AsInteger(16); profile = (p>>16); level = (p&255); }
      else if(keys[kn].Find("max-mbps=") == 0)    { max_mbps = (keys[kn].Tokenise("=")[1]).AsInteger(); }
      else if(keys[kn].Find("max-fs=") == 0)      { max_fs = (keys[kn].Tokenise("=")[1]).AsInteger(); }
      else if(keys[kn].Find("max-br=") == 0)      { max_br = (keys[kn].Tokenise("=")[1]).AsInteger(); }
    }
    if(level == 0) level = 12; // default level
    GetParamsH264(level, level_h241, max_fs, max_mbps, max_br, sc->video_width, sc->video_height);
  }

  if(sc->cap)
  {
    OpalMediaFormat & wf = sc->cap->GetWritableMediaFormat();
    SetFormatParams(wf, sc->video_width, sc->video_height, sc->video_frame_rate, sc->bandwidth);
    if(sprop != "") wf.SetOptionString("sprop-parameter-sets", sprop);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_VP8(SipCapMapType & LocalCaps, SipCapability *sc)
{
  PStringArray keys = sc->fmtp.Tokenise(";");

  if(!sc->cap)
  {
    if(FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, "VP8{sw}"))
      sc->cap = MCUCapability::Create("VP8{sw}");
    if(!sc->cap)
      return;
  }

  SipCapability *local_sc = FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, sc->cap->GetFormatName());
  if(local_sc->video_width) sc->video_width = local_sc->video_width;
  if(local_sc->video_height) sc->video_height = local_sc->video_height;
  if(local_sc->video_frame_rate) sc->video_frame_rate = local_sc->video_frame_rate;
  if(local_sc->bandwidth) sc->bandwidth = local_sc->bandwidth;

  if(sc->video_width == 0 && sc->video_height == 0)
  {
    for(int kn = 0; kn < keys.GetSize(); kn++)
    {
      if(keys[kn].Find("width=") == 0)       sc->video_width = (keys[kn].Tokenise("=")[1]).AsInteger();
      else if(keys[kn].Find("height=") == 0) sc->video_height = (keys[kn].Tokenise("=")[1]).AsInteger();
    }
  }

  if(sc->cap)
  {
    OpalMediaFormat & wf = sc->cap->GetWritableMediaFormat();
    SetFormatParams(wf, sc->video_width, sc->video_height, sc->video_frame_rate, sc->bandwidth);
    if(remoteApplication.ToLower().Find("linphone") != P_MAX_INDEX) wf.SetOptionEnum("Picture ID Size", 0);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_MPEG4(SipCapMapType & LocalCaps, SipCapability *sc)
{
  PStringArray keys = sc->fmtp.Tokenise(";");
  unsigned profile_level = 0, profile = 0, level = 0, max_fs = 0;
  PString config;

  for(int kn = 0; kn < keys.GetSize(); kn++)
  {
    if(keys[kn].Find("config=") == 0) { config = keys[kn].Tokenise("=")[1]; }
  }

  if(!sc->cap)
  {
    if(FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, "MP4V-ES{sw}"))
      sc->cap = MCUCapability::Create("MP4V-ES{sw}");
    if(!sc->cap)
      return;
  }

  SipCapability *local_sc = FindSipCap(LocalCaps, MEDIA_TYPE_VIDEO, sc->cap->GetFormatName());
  if(local_sc->video_width) sc->video_width = local_sc->video_width;
  if(local_sc->video_height) sc->video_height = local_sc->video_height;
  if(local_sc->video_frame_rate) sc->video_frame_rate = local_sc->video_frame_rate;
  if(local_sc->bandwidth) sc->bandwidth = local_sc->bandwidth;

  if(sc->video_width == 0 && sc->video_height == 0)
  {
    for(int kn = 0; kn < keys.GetSize(); kn++)
    {
      if(keys[kn].Find("profile-level-id=") == 0) { profile_level = keys[kn].Tokenise("=")[1].AsInteger(); }
    }
    if(profile_level == 0) profile_level = 3;
    GetParamsMpeg4(profile_level, profile, level, max_fs, sc->video_width, sc->video_height);
  }

  if(sc->cap)
  {
    OpalMediaFormat & wf = sc->cap->GetWritableMediaFormat();
    SetFormatParams(wf, sc->video_width, sc->video_height, sc->video_frame_rate, sc->bandwidth);
    if(config != "") wf.SetOptionString("config", config);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_SPEEX(SipCapMapType & LocalCaps, SipCapability *sc)
{
  PString capname;
  if(sc->clock == 8000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "Speex_8K{sw}"))        capname = "Speex_8K{sw}";
  else if(sc->clock == 16000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "Speex_16K{sw}")) capname = "Speex_16K{sw}";
  else if(sc->clock == 32000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "Speex_32K{sw}")) capname = "Speex_32K{sw}";
  else return;

  PString fmtp = sc->fmtp;
  // replace fmtp from codec parameters
  SipCapability *local_sc = FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, capname);
  if(local_sc && local_sc->local_fmtp != "")
    fmtp = local_sc->local_fmtp;

  int vbr = -1;
  int mode = -1;

  PStringArray keys = fmtp.Tokenise(";");
  for(int kn = 0; kn < keys.GetSize(); kn++)
  {
    if(keys[kn].Find("vbr=") == 0)
    {
      if(strcmp(keys[kn].Tokenise("=")[1], "on") == 0)
        vbr = 1;
      else if(strcmp(keys[kn].Tokenise("=")[1], "off") == 0)
        vbr = 0;
    }
    else if(keys[kn].Find("mode=") == 0)
      mode = (keys[kn].Tokenise("=")[1]).AsInteger();
  }

  sc->cap = MCUCapability::Create(capname);
  if(sc->cap)
  {
    OpalMediaFormat & wf = sc->cap->GetWritableMediaFormat();
    if (vbr > -1) wf.SetOptionEnum("vbr", vbr);
    if (mode > -1) wf.SetOptionInteger("mode", mode);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_OPUS(SipCapMapType & LocalCaps, SipCapability *sc)
{
  PString capname;
  if(sc->clock == 8000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "OPUS_8K{sw}"))
    capname = "OPUS_8K{sw}";
  else if(sc->clock == 16000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "OPUS_16K{sw}"))
    capname = "OPUS_16K{sw}";
  else if(sc->clock == 48000 && sc->params.AsInteger() <= 1 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "OPUS_48K{sw}"))
    capname = "OPUS_48K{sw}";
  else if(sc->clock == 48000 && sc->params.AsInteger() == 2 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "OPUS_48K2{sw}"))
    capname = "OPUS_48K2{sw}";
  else
    return;

  PString fmtp = sc->fmtp;
  // replace fmtp from codec parameters
  SipCapability *local_sc = FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, capname);
  if(local_sc && local_sc->local_fmtp != "")
    fmtp = local_sc->local_fmtp;

  int cbr = -1;
  int maxaveragebitrate = -1;
  int useinbandfec = -1;
  int usedtx = -1;

  PStringArray keys = fmtp.Tokenise(";");
  for(int kn = 0; kn < keys.GetSize(); kn++)
  {
    if(keys[kn].Find("cbr=") == 0)
      cbr = (keys[kn].Tokenise("=")[1]).AsInteger();
    else if(keys[kn].Find("maxaveragebitrate=") == 0)
      maxaveragebitrate = (keys[kn].Tokenise("=")[1]).AsInteger();
    else if(keys[kn].Find("useinbandfec=") == 0)
      useinbandfec = (keys[kn].Tokenise("=")[1]).AsInteger();
    else if(keys[kn].Find("usedtx=") == 0)
      usedtx = (keys[kn].Tokenise("=")[1]).AsInteger();
  }

  sc->cap = MCUCapability::Create(capname);
  if(sc->cap)
  {
    OpalMediaFormat & wf = sc->cap->GetWritableMediaFormat();
    if (cbr > -1) wf.SetOptionInteger("cbr", cbr);
    if (maxaveragebitrate > -1) wf.SetOptionInteger("maxaveragebitrate", maxaveragebitrate);
    if (useinbandfec > -1) wf.SetOptionInteger("useinbandfec", useinbandfec);
    if (usedtx > -1) wf.SetOptionInteger("usedtx", usedtx);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_G7221(SipCapMapType & LocalCaps, SipCapability *sc)
{
  int bitrate = 0;
  PStringArray keys = sc->fmtp.Tokenise(";");
  for(int kn = 0; kn < keys.GetSize(); kn++)
  {
    if(keys[kn].Find("bitrate=") == 0)
      bitrate = (keys[kn].Tokenise("=")[1]).AsInteger();
  }

  PString capname;
  if(sc->clock == 16000 && bitrate == 24000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.722.1-24K{sw}"))
    capname = "G.722.1-24K{sw}";
  else if(sc->clock == 16000 && bitrate == 32000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.722.1-32K{sw}"))
    capname = "G.722.1-32K{sw}";
  else if(sc->clock == 32000 && bitrate == 24000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.722.1C-24K{sw}"))
    capname = "G.722.1C-24K{sw}";
  else if(sc->clock == 32000 && bitrate == 32000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.722.1C-32K{sw}"))
    capname = "G.722.1C-32K{sw}";
  else if(sc->clock == 32000 && bitrate == 48000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.722.1C-48K{sw}"))
    capname = "G.722.1C-48K{sw}";
  else
    return;

  SipCapability *local_sc = FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, capname);
  local_sc->fmtp = sc->fmtp;
  local_sc->local_fmtp = sc->fmtp;
  sc->cap = MCUCapability::Create(capname);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_G7222(SipCapMapType & LocalCaps, SipCapability *sc)
{
  PString capname;
  if(sc->clock == 16000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.722.2{sw}"))
    capname = "G.722.2{sw}";
  else
    return;

  int octet_align = -1;
  PStringArray keys = sc->fmtp.Tokenise(";");
  for(int kn = 0; kn < keys.GetSize(); kn++)
  {
    if(keys[kn].Find("octet-align=") == 0)
      octet_align = (keys[kn].Tokenise("=")[1]).AsInteger();
  }

  sc->cap = MCUCapability::Create(capname);
  if(sc->cap)
  {
    OpalMediaFormat & wf = sc->cap->GetWritableMediaFormat();
    if(octet_align > -1) wf.SetOptionInteger("Octet Aligned", octet_align);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_AC3(SipCapMapType & LocalCaps, SipCapability *sc)
{
  PString capname;
  if(sc->clock == 48000 && sc->params.AsInteger() <= 1 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "AC3_48K{sw}"))
    capname = "AC3_48K{sw}";
  else if(sc->clock == 48000 && sc->params.AsInteger() == 2 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "AC3_48K2{sw}"))
    capname = "AC3_48K2{sw}";
  else
    return;

  sc->cap = MCUCapability::Create(capname);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sdp_parser_t *MCUSipConnection::SdpParser(PString sdp_str)
{
  // RTP/SAVPF parse
  BOOL found_savpf = FALSE;
  if(sdp_str.Find("RTP/SAVPF") != P_MAX_INDEX)
  {
    sdp_str.Replace("RTP/SAVPF","RTP/SAVP",TRUE,0);
    found_savpf = TRUE;
  }
  sdp_parser_t *parser = sdp_parse(sep->GetHome(), sdp_str, sdp_str.GetLength(), 0);
  if(found_savpf)
  {
    sdp_session_t *sess = sdp_session(parser);
    for(sdp_session_t *sdp = sess; sdp != NULL; sdp = sdp->sdp_next)
    {
      for(sdp_media_t *m = sdp->sdp_media; m != NULL; m = m->m_next)
      {
        if(PString(m->m_proto_name) == "RTP/SAVP")
        {
          sdp_attribute_t *a = sdp_attribute_find(m->m_attributes, "fingerprint");
          if(a)
          {
            m->m_proto = sdp_proto_x;
            m->m_proto_name = "RTP/SAVPF";
          }
        }
      }
    }
  }
  return parser;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ProcessSDP(SipCapMapType & LocalCaps, SipCapMapType & RemoteCaps, PString & sdp_str)
{
  PTRACE(1, trace_section << "ProcessSDP");
  sdp_parser_t *parser = SdpParser(sdp_str);
  sdp_session_t *sess = sdp_session(parser);
  if(!sess)
  {
    PTRACE(1, trace_section << "SDP parsing error: " << sdp_parsing_error(parser));
    return 415;
  }
  // -2 if c= line is missing. -1 if some SDP line is missing. (c=, o=, s=, t=)
  if(sdp_sanity_check(parser) != 0)
  {
    PTRACE(1, trace_section << "SDP parsing error: sanity check");
    return 415;
  }

  RemoteCaps.clear();
  for(sdp_session_t *sdp = sess; sdp != NULL; sdp = sdp->sdp_next)
  {
    if(!sdp->sdp_origin)
    { PTRACE(1, trace_section << "SDP parsing error: sdp origin line is missing"); continue; }

    for(sdp_media_t *m = sdp->sdp_media; m != NULL; m = m->m_next)
    {
      PString m_proto_name = PString(m->m_proto_name);
      if(rtp_proto == "RTP" && m_proto_name != "RTP/AVP")
        continue;
      else if(rtp_proto == "ZRTP" && m_proto_name != "RTP/AVP")
        continue;
      else if(rtp_proto == "SRTP" && m_proto_name != "RTP/SAVP")
        continue;
      else if(rtp_proto == "SRTP/RTP" && m_proto_name != "RTP/SAVP" && m_proto_name != "RTP/AVP")
        continue;
      else if(rtp_proto == "DTLS-SRTP" && m_proto_name != "RTP/SAVPF")
        continue;

      PString address;
      if(m->m_connections && m->m_connections->c_addrtype == sdp_addr_ip4 && m->m_connections->c_address)
      {
        address = m->m_connections->c_address;
        //PTRACE(6, trace_section << "SDP parsing info: media connection address: " << address);
      }
      if(!PIPSocket::Address(address).IsValid() && sdp->sdp_connection && sdp->sdp_connection->c_addrtype == sdp_addr_ip4 && sdp->sdp_connection->c_address)
      {
        address = sdp->sdp_connection->c_address;
        //PTRACE(6, trace_section << "SDP parsing info: sdp connection address: " << address);
      }
      if(!PIPSocket::Address(address).IsValid() && sdp->sdp_origin->o_address && sdp->sdp_origin->o_address->c_addrtype == sdp_addr_ip4 && sdp->sdp_origin->o_address->c_address)
      {
        address = sdp->sdp_origin->o_address->c_address;
        //PTRACE(6, trace_section << "SDP parsing info: sdp origin address: " << address);
      }
      if(!PIPSocket::Address(address).IsValid())
      {
        PTRACE(1, trace_section << "SDP parsing warning: incorrect or missing address");
      }

      int remote_port = 0;
      if(m->m_port) remote_port = m->m_port;
      else
      { PTRACE(1, trace_section << "SDP parsing warning: missing port"); }

      MediaTypes media_type = MEDIA_TYPE_UNKNOWN;
      if(m->m_type == sdp_media_audio)      media_type = MEDIA_TYPE_AUDIO;
      else if(m->m_type == sdp_media_video) media_type = MEDIA_TYPE_VIDEO;
      else
      { PTRACE(1, trace_section << "SDP parsing error: unknown media type, skip media"); continue; }

      int mode = 3; // inactive, recvonly, sendonly, sendrecv
      if(m->m_mode == 1)      mode = 2;
      else if(m->m_mode == 2) mode = 1;
      else if(m->m_mode == 3) mode = 3;

      int bw = 0;
      if(m->m_bandwidths)
      {
        bw = m->m_bandwidths->b_value;
        if(PString(m->m_bandwidths->b_modifier_name) != "AS") bw = bw/1000;
      }
      else if(sdp->sdp_bandwidths)
      {
        bw = sdp->sdp_bandwidths->b_value;
        if(PString(sdp->sdp_bandwidths->b_modifier_name) != "AS") bw = bw/1000;
      }
      if(bw && bw < MCU_MIN_BIT_RATE/1000) bw = MCU_MIN_BIT_RATE/1000;
      if(bw && bw > MCU_MAX_BIT_RATE/1000) bw = MCU_MAX_BIT_RATE/1000;

      SipSecureTypes secure_type = SECURE_TYPE_NONE;
      PString srtp_type, srtp_key, srtp_param, zrtp_hash, dtls_fp, dtls_fp_type;

      if(m_proto_name == "RTP/AVP" && rtp_proto == "ZRTP")
      {
        secure_type = SECURE_TYPE_ZRTP;
        sdp_attribute_t *a = sdp_attribute_find(m->m_attributes, "zrtp-hash");
        if(a && a->a_value)
          zrtp_hash = PString(a->a_value);
      }
      if(m_proto_name == "RTP/SAVP")
      {
        sdp_attribute_t *a = sdp_attribute_find(m->m_attributes, "crypto");
        if(!a || !a->a_value)
        {
          PTRACE(1, trace_section << "SDP parsing warning: RTP/SAVP crypto attribute is not present, skip media");
          continue;
        }
        secure_type = SECURE_TYPE_SRTP;
        PStringArray attr = PString(a->a_value).Tokenise(" ", FALSE);
        srtp_type = attr[1];
        if(srtp_type != AES_CM_128_HMAC_SHA1_32 && srtp_type != AES_CM_128_HMAC_SHA1_80)
          continue;
        if(attr[2].Find("inline:") == P_MAX_INDEX)
          continue;
        PString inl = attr[2].Right(attr[2].GetLength()-7);
        PINDEX pos_param = inl.Find("|");
        if(pos_param == P_MAX_INDEX)
        {
          srtp_key = inl;
        } else {
          srtp_key = inl.Left(pos_param);
          srtp_param = inl.Right(inl.GetLength()-pos_param);
        }
      }
      if(m_proto_name == "RTP/SAVPF")
      {
        sdp_attribute_t *a = sdp_attribute_find(m->m_attributes, "fingerprint");
        if(!a || !a->a_value)
        {
          PTRACE(1, trace_section << "SDP parsing warning: RTP/SAVPF fingerprint attribute is not present, skip media");
          continue;
        }
        secure_type = SECURE_TYPE_DTLS_SRTP;
        dtls_fp_type = a->a_name;
        dtls_fp = a->a_value;
      }

      for(sdp_rtpmap_t *rm = m->m_rtpmaps; rm != NULL; rm = rm->rm_next)
      {
        if(FindSipCap(RemoteCaps, media_type, rm->rm_pt)) continue;
        SipCapability *sc = new SipCapability();
        sc->payload = rm->rm_pt;
        sc->media = media_type;
        sc->mode = mode;
        if(rm->rm_encoding) sc->format = PString(rm->rm_encoding).ToLower();
        sc->clock = rm->rm_rate;
        sc->bandwidth = bw;
        if(rm->rm_fmtp) { sc->fmtp = rm->rm_fmtp; sc->fmtp.Replace(" ","",TRUE,0); }
        if(rm->rm_params) sc->params = rm->rm_params;
        //
        sc->remote_ip = address;
        sc->remote_port = remote_port;
        //
        if(sdp->sdp_origin->o_username) sdp_o_username = sdp->sdp_origin->o_username;
        if(sdp->sdp_origin->o_id)       sdp_o_id = sdp->sdp_origin->o_id;
        if(sdp->sdp_origin->o_version)  sdp_o_ver = sdp->sdp_origin->o_version;
        // secure
        sc->secure_type = secure_type;
        sc->srtp_remote_type = srtp_type;
        sc->srtp_remote_key = srtp_key;
        sc->srtp_remote_param = srtp_param;
        sc->zrtp_hash = zrtp_hash;
        sc->dtls_fp_type = dtls_fp_type;
        sc->dtls_fp = dtls_fp;
        // attributes
        for(sdp_attribute_t *a = m->m_attributes; a != NULL; a = a->a_next)
        {
          sc->attr.SetAt(a->a_name, a->a_value);
        }
        RemoteCaps.insert(SipCapMapType::value_type(RemoteCaps.size(), sc));
      }
    }
  }
  sdp_parser_free(parser);

  MergeSipCaps(LocalCaps, RemoteCaps);

  if(scap < 0 && vcap < 0)
  {
    PTRACE(1, trace_section << "SDP parsing error: compatible codecs not found");
    return 415;
  }

  if(scap >= 0)
  {
    SipCapability *sc = FindSipCap(RemoteCaps, MEDIA_TYPE_AUDIO, scap);
    if(sc) sc->Print();
  }
  if(vcap >= 0)
  {
    SipCapability *sc = FindSipCap(RemoteCaps, MEDIA_TYPE_VIDEO, vcap);
    if(sc) sc->Print();
  }

  return 0;
}

BOOL MCUSipConnection::MergeSipCaps(SipCapMapType & LocalCaps, SipCapMapType & RemoteCaps)
{
  scap = -1; vcap = -1;
  for(SipCapMapType::iterator it = RemoteCaps.begin(); it != RemoteCaps.end(); it++)
  {
    SipCapability *remote_sc = it->second;
    if(remote_sc->media == MEDIA_TYPE_AUDIO)
    {
      if(scap >= 0) continue;
      // PCMU
      if((remote_sc->format == "pcmu" || remote_sc->payload == 0) && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.711-uLaw-64k{sw}"))
      { remote_sc->capname = "G.711-uLaw-64k{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // PCMA
      else if((remote_sc->format == "pcma" || remote_sc->payload == 8) && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.711-ALaw-64k{sw}"))
      { remote_sc->capname = "G.711-ALaw-64k{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // G.722
      else if(remote_sc->format == "g722" && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.722-64k{sw}"))
      { remote_sc->capname = "G.722-64k{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // G.723.1
      else if(remote_sc->format == "g723" && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.7231-6.3k[e]{sw}"))
      { remote_sc->capname = "G.7231-6.3k[e]{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // G.726-16
      else if(remote_sc->format == "g726-16" && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.726-16k{sw}"))
      { remote_sc->capname = "G.726-16k{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // G.726-24
      else if(remote_sc->format == "g726-24" && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.726-24k{sw}"))
      { remote_sc->capname = "G.726-24k{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // G.726-32
      else if(remote_sc->format == "g726-32" && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.726-32k{sw}"))
      { remote_sc->capname = "G.726-32k{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // G.726-40
      else if(remote_sc->format == "g726-40" && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.726-40k{sw}"))
      { remote_sc->capname = "G.726-40k{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // G.728
      else if(remote_sc->format == "g728" && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.728-16k[e]"))
      { remote_sc->capname = "G.728-16k[e]"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // G.729A
      else if(remote_sc->format == "g729" && remote_sc->fmtp == "annexb=no;" && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "G.729A-8k[e]{sw}"))
      { remote_sc->capname = "G.729A-8k[e]{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // iLBC-13k3
      else if(remote_sc->format == "ilbc" && remote_sc->fmtp == "mode=30;" && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "iLBC-13k3{sw}"))
      { remote_sc->capname = "iLBC-13k3{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // iLBC-15k2
      else if(remote_sc->format == "ilbc" && remote_sc->fmtp == "mode=20;" && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "iLBC-15k2{sw}"))
      { remote_sc->capname = "iLBC-15k2{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // SILK 16000
      else if(remote_sc->format == "silk" && remote_sc->clock == 16000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "SILK_B40{sw}"))
      { remote_sc->capname = "SILK_B40{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // SILK 24000
      else if(remote_sc->format == "silk" && remote_sc->clock == 24000 && FindSipCap(LocalCaps, MEDIA_TYPE_AUDIO, "SILK_B40_24K{sw}"))
      { remote_sc->capname = "SILK_B40_24K{sw}"; remote_sc->cap = MCUCapability::Create(remote_sc->capname); }
      // SPEEX
      else if(remote_sc->format == "speex")
      { SelectCapability_SPEEX(LocalCaps, remote_sc); }
      // OPUS
      else if(remote_sc->format == "opus")
      { SelectCapability_OPUS(LocalCaps, remote_sc); }
      // G.722.1
      else if(remote_sc->format == "g7221")
      { SelectCapability_G7221(LocalCaps, remote_sc); }
      // G.722.2
      else if(remote_sc->format == "amr-wb" && remote_sc->clock == 16000)
      { SelectCapability_G7222(LocalCaps, remote_sc); }
      // AC3
      else if(remote_sc->format == "ac3")
      { SelectCapability_AC3(LocalCaps, remote_sc); }
    }
    else if(remote_sc->media == MEDIA_TYPE_VIDEO)
    {
      if(vcap >= 0) continue;
      // preferred capability
      CheckPreferSipCap(LocalCaps, remote_sc);
      //
      if(remote_sc->format == "h261") SelectCapability_H261(LocalCaps, remote_sc);
      else if(remote_sc->format == "h263") SelectCapability_H263(LocalCaps, remote_sc);
      else if(remote_sc->format == "h263-1998") SelectCapability_H263p(LocalCaps, remote_sc);
      else if(remote_sc->format == "h264") SelectCapability_H264(LocalCaps, remote_sc);
      else if(remote_sc->format == "mp4v-es") SelectCapability_MPEG4(LocalCaps, remote_sc);
      else if(remote_sc->format == "vp8") SelectCapability_VP8(LocalCaps, remote_sc);
    }
    if(remote_sc->cap)
    {
      remote_sc->capname = remote_sc->cap->GetFormatName();
      if(remote_sc->media == MEDIA_TYPE_AUDIO)      scap = remote_sc->payload;
      else if(remote_sc->media == MEDIA_TYPE_VIDEO) vcap = remote_sc->payload;
    }
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ProcessConnect()
{
  PTRACE(1, trace_section << "ProcessConnect");
  sip_t *sip = sip_object(c_sip_msg);

  if(direction == DIRECTION_INBOUND)
  {
    ProxyAccount *proxy = sep->FindProxyAccount(sip->sip_to->a_url->url_user, sip->sip_from->a_url->url_host);
    if(proxy) requestedRoom = proxy->roomname;
  }

  MCUURL_SIP url(c_sip_msg, direction);
  remoteUserName = url.GetUserName();
  remoteDisplayName = url.GetDisplayName();
  remoteApplication = url.GetRemoteApplication();

  // set endpoint member name
  SetMemberName();
  // join conference
  JoinConference(requestedRoom);
  if(!conferenceMember || !conferenceMember->IsJoined())
    return 600;
  //
  OnEstablished();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ProcessInvite(const msg_t *msg)
{
  PTRACE(1, trace_section << "ProcessInviteEvent");

  msg_destroy(c_sip_msg);
  c_sip_msg = msg_dup(msg);
  msg_addr_copy(c_sip_msg, msg);
  sip_t *sip = sip_object(c_sip_msg);
  int response_code = 0;

  // empty payload header for invite
  if(!sip->sip_payload || !sip->sip_payload->pl_data)
    return 415; // SIP_415_UNSUPPORTED_MEDIA

  PString sdp_str = sip->sip_payload->pl_data;
  response_code = ProcessSDP(LocalSipCaps, RemoteSipCaps, sdp_str);
  if(response_code)
    return response_code;

  response_code = ProcessConnect();
  if(response_code)
    return response_code;

  if(direction == DIRECTION_INBOUND)
  {
    // create logical channels
    CreateMediaChannels();
    // add to_tag in sip_to header
    // added here because CANCEL request does not contain to_tag
    if(!sip->sip_to->a_tag)
      msg_header_add_param(msg_home(c_sip_msg), (msg_common_t *)sip->sip_to, nta_agent_newtag(msg_home(c_sip_msg), "tag=%s", sep->GetAgent()));
    if(!nta_leg_get_tag(leg))
      nta_leg_tag(leg, sip->sip_to->a_tag);
    // create sdp for OK
    CreateSdpOk();
    ReqReply(c_sip_msg, SIP_200_OK);
  }
  else if(direction == DIRECTION_OUTBOUND)
  {
    // create logical channels
    CreateMediaChannels();
    // for incoming connection start channels after ACK
    StartMediaChannels();
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ProcessReInvite(const msg_t *msg)
{
  PTRACE(1, trace_section << "ProcessReInvite");

  msg_destroy(c_sip_msg);
  c_sip_msg = msg_dup(msg);
  msg_addr_copy(c_sip_msg, msg);
  sip_t *sip = sip_object(c_sip_msg);

  // empty payload header for invite
  if(!sip->sip_payload || !sip->sip_payload->pl_data)
    return 415; // SIP_415_UNSUPPORTED_MEDIA

  PString sdp_str = sip->sip_payload->pl_data;
  SipCapMapType SipCaps;

  int cur_scap = scap;
  int cur_vcap = vcap;

  int response_code = ProcessSDP(LocalSipCaps, SipCaps, sdp_str);
  if(response_code)
    return response_code;

  int sflag = 1; // 0 - no changes
  if(scap >= 0 && cur_scap >= 0)
  {
    SipCapability *cur_sc = FindSipCap(RemoteSipCaps, MEDIA_TYPE_AUDIO, cur_scap);
    SipCapability *new_sc = FindSipCap(SipCaps , MEDIA_TYPE_AUDIO, scap);
    sflag = new_sc->CmpSipCaps(*cur_sc);
  }
  else if(scap < 0 && cur_scap < 0)
  {
    sflag = 0;
  }
  if(sflag && cur_scap >= 0)
  {
    DeleteMediaChannel(MEDIA_TYPE_AUDIO, cur_scap, 0);
    DeleteMediaChannel(MEDIA_TYPE_AUDIO, cur_scap, 1);
  }

  int vflag = 1; // 0 - no changes
  if(vcap >= 0 && cur_vcap >= 0)
  {
    SipCapability *cur_sc = FindSipCap(RemoteSipCaps, MEDIA_TYPE_VIDEO, cur_vcap);
    SipCapability *new_sc = FindSipCap(SipCaps, MEDIA_TYPE_VIDEO, vcap);
    vflag = new_sc->CmpSipCaps(*cur_sc);
  }
  else if(vcap < 0 && cur_vcap < 0)
  {
    vflag = 0;
  }
  if(vflag && cur_vcap >= 0)
  {
    DeleteMediaChannel(MEDIA_TYPE_VIDEO, cur_vcap, 0);
    DeleteMediaChannel(MEDIA_TYPE_VIDEO, cur_vcap, 1);
  }

  if(!sflag && !vflag) // nothing changed
  {
    // sending old sdp
    return 0;
  }

  // update remote sip caps
  ClearSipCaps(RemoteSipCaps);
  RemoteSipCaps = SipCaps;

  if(sflag && scap >= 0)
  {
    CreateMediaChannel(MEDIA_TYPE_AUDIO, scap, 0);
    CreateMediaChannel(MEDIA_TYPE_AUDIO, scap, 1);
  }
  if(vflag && vcap >= 0)
  {
    CreateMediaChannel(MEDIA_TYPE_VIDEO, vcap, 0);
    CreateMediaChannel(MEDIA_TYPE_VIDEO, vcap, 1);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ProcessAck()
{
  PTRACE(1, trace_section << "ProcessACK");
  if(!c_sip_msg)
    return 0;

  if(direction != DIRECTION_INBOUND)
    return 0;

  if(connectionState != AwaitingSignalConnect)
    return 0;

  // for incoming connection start channels after ACK
  StartMediaChannels();

  // is connected
  connectionState = EstablishedConnection;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::OnReceivedVFU()
{
  if(!CheckVFU())
    return;

  PWaitAndSignal m(channelsMutex);
  if(videoTransmitChannel)
    videoTransmitChannel->OnFastUpdatePicture();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::OnReceivedDTMF(PString sdp)
{
  if(conference == NULL)
    return;

  PStringArray dataArray = sdp.Lines();
  for(PINDEX i = 0; i < dataArray.GetSize(); i++)
  {
    if(dataArray[i].Find("Signal=") != P_MAX_INDEX)
    {
      PString signal = dataArray[i].Tokenise("=")[1];
      PString signalTypes = "1234567890*#ABCD";
      if(signal.GetLength() == 1 && signalTypes.Find(signal) != P_MAX_INDEX)
        OnUserInputString(signal);
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ProcessInfo(const msg_t *msg)
{
  PTRACE(1, trace_section << "OnReceivedInfo");
  sip_t *sip = sip_object(msg);
  if(!sip->sip_payload || !sip->sip_payload->pl_data || !sip->sip_content_type)
    return 0;

  PString type = PString(sip->sip_content_type->c_type);
  PString data = PString(sip->sip_payload->pl_data);
  if(type.Find("application/media_control") != P_MAX_INDEX && data.Find("picture_fast_update") != P_MAX_INDEX)
    OnReceivedVFU();
  else if (type.Find("application/dtmf-relay") != P_MAX_INDEX)
    OnReceivedDTMF(data);
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SendRequest(sip_method_t method, const char *method_name, const char *payload)
{
  if(connectionType != CONNECTION_TYPE_SIP)
    return;

  if(PThread::Current() != sep)
  {
    PString *cmd = new PString("connection_send_request:"+callToken+","+PString(method)+","+PString(method_name)+","+PString(payload));
    PTRACE(1, trace_section << "added to sip queue: " << *cmd);
    sep->GetSipQueue().Push(cmd);
    return;
  }

  PTRACE(1, trace_section << "SendRequest: " << method_name);

  int stateless = 1;
  nta_response_f *callback = NULL;
  nta_outgoing_magic_t *magic = NULL;
  const char *via_branch_key = NULL;
  sip_call_id_t *sip_call_id = NULL;
  sip_cseq_t *sip_cseq = NULL;
  sip_route_t* sip_route = NULL;
  sip_contact_t *sip_contact = NULL;
  sip_content_type_t *sip_content = NULL;
  sip_payload_t *sip_payload = NULL;
  sip_date_t *sip_date = NULL;
  sip_authorization_t *sip_auth = NULL;
  sip_authorization_t *sip_proxy_auth = NULL;

  url_string_t *route_ruri = (url_string_t *)(const char *)ruri_str;
  url_string_t *request_ruri = route_ruri;
  sip_contact = sip_contact_create(sep->GetHome(), (url_string_t *)(const char *)contact_str, NULL);

  sip_t *c_sip = sip_object(c_sip_msg);
  if(c_sip)
  {
    // in-dialog requests should always use URI from contact header
    request_ruri = (url_string_t *)c_sip->sip_contact->m_url;
    // route
    if(direction == DIRECTION_INBOUND)
      sip_route = c_sip->sip_record_route;
    else
      sip_route = sip_route_reverse(sep->GetHome(), c_sip->sip_record_route);
    // ACK branch and cseq from invite request
    if(method == sip_method_ack)
    {
      via_branch_key = c_sip->sip_via->v_branch;
      sip_cseq = sip_cseq_create(sep->GetHome(), c_sip->sip_cseq->cs_seq, method, method_name);
    }
  }
  if(method == sip_method_invite)
  {
    callback = wrap_invite_response_cb;
    magic = (nta_outgoing_magic_t *)this;
    stateless = 0;
    sip_content = sip_content_type_make(sep->GetHome(), "application/sdp");
    sip_payload = sip_payload_format(sep->GetHome(), (const char *)sdp_invite_str);
  }
  if(method == sip_method_info)
  {
    sip_content = sip_content_type_make(sep->GetHome(), "application/media_control+xml");
    sip_payload = sip_payload_format(sep->GetHome(), "<media_control><vc_primitive><to_encoder><picture_fast_update/></to_encoder></vc_primitive></media_control>");
  }
  if(method == sip_method_message)
  {
    sip_call_id = sip_call_id_create(sep->GetHome(), NULL);
    sip_content = sip_content_type_make(sep->GetHome(), "text/plain");
    sip_payload = sip_payload_format(sep->GetHome(), payload);
    sip_date = sip_date_create(sep->GetHome(), sip_now());
  }

  if(auth.type != HTTPAuth::AUTH_NONE)
  {
    HTTPAuth auth_copy(auth);
    auth_copy.method = method_name;
    PString auth_str = auth_copy.MakeAuthorizationStr();
    if(auth.type == HTTPAuth::AUTH_WWW)
      sip_auth = sip_authorization_make(sep->GetHome(), auth_str);
    else if(auth.type == HTTPAuth::AUTH_PROXY)
      sip_proxy_auth = sip_proxy_authorization_make(sep->GetHome(), auth_str);
  }

  nta_outgoing_t *a_orq = nta_outgoing_tcreate(leg, callback, magic,
                        route_ruri,
                        method, method_name,
                        request_ruri,
   		        NTATAG_STATELESS(stateless),
   		        SIPTAG_CALL_ID(sip_call_id),
			SIPTAG_CSEQ(sip_cseq),
                        NTATAG_BRANCH_KEY(via_branch_key),
			SIPTAG_ROUTE(sip_route),
                        SIPTAG_CONTACT(sip_contact),
                        SIPTAG_CONTENT_TYPE(sip_content),
                        SIPTAG_PAYLOAD(sip_payload),
                        SIPTAG_DATE(sip_date),
			SIPTAG_AUTHORIZATION(sip_auth),
			SIPTAG_PROXY_AUTHORIZATION(sip_proxy_auth),
			SIPTAG_MAX_FORWARDS_STR(SIP_MAX_FORWARDS),
			SIPTAG_SERVER_STR((const char*)(SIP_USER_AGENT)),
                        TAG_END());
  if(a_orq == NULL)
  {
    PTRACE(1, trace_section << "error");
    ClearCall();
    return;
  }
  if(stateless == 1)
  {
    nta_outgoing_destroy(a_orq);
    a_orq = NULL;
  }
  if(method == sip_method_invite)
  {
    if(orq_invite)
      nta_outgoing_destroy(orq_invite);
    orq_invite = a_orq;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SendInvite()
{
  // outgoing Invite direction outbound
  direction = DIRECTION_OUTBOUND;
  // waiting OK signal
  connectionState = AwaitingSignalConnect;
  // create sdp for invite
  CreateSdpInvite();

  SendRequest(SIP_METHOD_INVITE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SendLogicalChannelMiscCommand(H323Channel & channel, unsigned command)
{
  if(command == H245_MiscellaneousCommand_type::e_videoFastUpdatePicture)
  {
    PTime now;
    if(now < vfuSendTime + PTimeInterval(1000))
      return;
    vfuSendTime = now;

    SendRequest(SIP_METHOD_INFO);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SendUserInput(const PString & value)
{
  if(connectionType != CONNECTION_TYPE_SIP)
    return;
  PTRACE(6, trace_section << "SendUserInput");
  SendRequest(SIP_METHOD_MESSAGE, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ReqReply(const msg_t *msg, unsigned status, const char *status_phrase)
{
  sip_t *sip = sip_object(msg);

  if(sip->sip_request->rq_method == sip_method_invite && status == 200)
  {
    sip_contact_t *sip_contact = sip_contact_create(sep->GetHome(), (url_string_t *)(const char *)contact_str, NULL);
    nta_incoming_t *irq = nta_incoming_find(sep->GetAgent(), sip, sip->sip_via);
    if(irq)
    {
      nta_incoming_treply(irq, status, status_phrase,
                   SIPTAG_CONTACT(sip_contact),
                   SIPTAG_CONTENT_TYPE_STR("application/sdp"),
                   SIPTAG_PAYLOAD_STR((const char*)sdp_ok_str),
                   SIPTAG_SERVER_STR((const char*)(SIP_USER_AGENT)),
                   TAG_END());
      return 0;
    }
    else
    {
      msg_t *msg_reply = nta_msg_create(sep->GetAgent(), 0);
      sip_add_tl(msg_reply, sip_object(msg_reply),
                   SIPTAG_CONTACT(sip_contact),
                   SIPTAG_CONTENT_TYPE_STR("application/sdp"),
                   SIPTAG_PAYLOAD_STR((const char*)sdp_ok_str),
                   TAG_END());
      sep->SipReqReply(msg, msg_reply, status, status_phrase);
      return 0;
    }
  }
  sep->SipReqReply(msg, NULL, status, status_phrase);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::invite_request_cb(nta_leg_t *leg, nta_incoming_t *irq, const sip_t *sip)
{
  PWaitAndSignal m(connMutex);

  msg_t *msg = nta_incoming_getrequest(irq);
  int request = sip->sip_request->rq_method;

  if(request == sip_method_invite)
  {
    nta_incoming_treply(irq, SIP_100_TRYING,
                        TAG_END());
    // incoming re-Invite direction inbound
    direction = DIRECTION_INBOUND;
    // waiting ACK signal
    connectionState = AwaitingSignalConnect;
    // re-Invite processing
    int response_code = ProcessReInvite(msg);
    if(response_code)
    {
      PTRACE(1, trace_section << "error " << response_code);
      ClearCall();
      return response_code;
    }
    // create sdp for OK
    CreateSdpOk();
    ReqReply(msg, SIP_200_OK);
    return 200;
  }
  if(request == sip_method_ack)
  {
    ProcessAck();
    //sep->SipReqReply(msg, SIP_200_OK);
  }
  if(request == sip_method_bye)
  {
    ClearCall(EndedByRemoteUser);
  }
  if(request == sip_method_cancel)
  {
    ClearCall(EndedByRemoteUser);
  }
  if(request == sip_method_info)
  {
    ProcessInfo(msg);
  }

  // nta: timer J fired, terminate 200 response
  return 200;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::invite_response_cb(nta_outgoing_t *orq, const sip_t *sip)
{
  PWaitAndSignal m(connMutex);

  // note that the message is not copied
  msg_t *msg = nta_outgoing_getresponse(orq);
  // note that the request message is not copied
  msg_t *msg_orq = nta_outgoing_getrequest(orq);
  if(!msg || !msg_orq) return 0;

  int status = 0, request = 0;
  if(sip->sip_status)  status = sip->sip_status->st_status;
  if(sip->sip_cseq)    request = sip->sip_cseq->cs_method;

  // destroy a request object
  // marks it as disposable, the object is freed after a timeout.
  //if(status >= 200)
  //{
  //  nta_outgoing_destroy(orq);
  //  orq_invite = NULL;
  //}
  // destroy it at end of the connection

  if(status == 200)
  {
    // repeated OK
    if(connectionState == EstablishedConnection)
    {
      SendRequest(SIP_METHOD_ACK);
      return 0;
    }
    // add rtag to call leg from response
    if(!nta_leg_get_rtag(leg))
      nta_leg_rtag(leg, sip->sip_to->a_tag);
    // processing OK
    int response_code = 0;
    if(!conference)
      response_code = ProcessInvite(msg);
    else
      response_code = ProcessReInvite(msg);
    if(response_code)
    {
      PTRACE(1, trace_section << "error " << response_code);
      ClearCall();
      return 0;
    }
    // is connected
    connectionState = EstablishedConnection;
    // send ACK
    SendRequest(SIP_METHOD_ACK);
    return 0;
  }
  if(status == 401 || status == 407)
  {
    if(auth.username == "" || auth.password == "")
    {
      PTRACE(1, trace_section << "error");
      ClearCall(EndedByRemoteUser);
      return 0;
    }
    if(sep->ParseAuthMsg(msg, auth) == FALSE)
    {
      PTRACE(1, trace_section << "error");
      ClearCall(EndedByRemoteUser);
      return 0;
    }
    SendInvite();
    return 0;
  }
  if(status > 299)
  {
    PStringStream event;
    event << sip->sip_status->st_status << " " << sip->sip_status->st_phrase;
    ClearCall(EndedByRemoteUser, event);
    return 0;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString MCUSipEndPoint::GetRoomAccess(const sip_t *sip)
{
    PTRACE(1, trace_section << "GetRoomAccess");
    BOOL inRoom = FALSE;
    PString via = sip->sip_via->v_host;
    PString userName = sip->sip_from->a_url->url_user;
    PString hostName = sip->sip_from->a_url->url_host;
    PString roomName;
    PString userName_, hostName_, via_, access;
    PString defaultAccess = MCUConfig("RoomAccess").GetString("*", "ALLOW").Tokenise(",")[0].ToUpper();

    ProxyAccount *proxy = FindProxyAccount(sip->sip_to->a_url->url_user, sip->sip_from->a_url->url_host);
    if(proxy)
      roomName = proxy->roomname;
    else
      roomName = sip->sip_to->a_url->url_user;

    PStringToString data = MCUConfig("RoomAccess").GetAllKeyValues();
    if(roomName != "*")
      access = data(roomName).Tokenise(",")[0].ToUpper();
    PStringArray accessList = data(roomName).Tokenise(",")[1].Tokenise(" ");
    for(int i=0; accessList[i] != NULL; i++)
    {
      userName_ = accessList[i].Tokenise("@")[0];
      hostName_ = accessList[i].Tokenise("@")[1];
      via_ = accessList[i].Tokenise("@")[2];
      if((userName_ == "" && hostName_ == "" && via_ == via) ||
         (userName_ == "" && hostName_ == hostName && via_ == "") ||
         (userName_ == "" && hostName_ == hostName && via_ == via) ||
         (userName_ == userName && hostName_ == "" && via_ == "") ||
         (userName_ == userName && hostName_ == "" && via_ == via) ||
         (userName_ == userName && hostName_ == hostName && via_ == "") ||
         (userName_ == userName && hostName_ == hostName && via_ == via)
        )
      {
        inRoom = TRUE;
        break;
      }
    }

    if(inRoom && access == "ALLOW")
      access = "ALLOW";
    else if(inRoom && access == "DENY")
      access = "DENY";
    else if(inRoom == FALSE && access == "ALLOW")
      access = "DENY";
    else if(inRoom == FALSE && access == "DENY")
      access = "ALLOW";
    else
      access = defaultAccess;

    PTRACE(1, trace_section << " " << access << " access to room \"" << roomName << "\", from=" << userName+"@"+hostName << ", via=" << via);
    return access;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipEndPoint::SipMakeCall(PString from, PString to, PString & callToken)
{
    PTRACE(1, trace_section << "SipMakeCall from: " << from << " to: " << to);

    MCUURL url_to(to);
    if(url_to.GetHostName() == "")
      return FALSE;
    PString remote_user = url_to.GetUserName();
    PString remote_host = url_to.GetHostName();
    PString remote_domain = remote_host;
    PString remote_port = url_to.GetPort();
    PString remote_transport = url_to.GetTransport();

    if(from.Left(4) != "sip:") from = "sip:"+from;
    if(from.Find("@") == P_MAX_INDEX) from = from+"@";
    MCUURL url_from(from);
    PString local_user = url_from.GetUserName();
    PString local_domain = url_from.GetHostName();
    PString local_dname = url_from.GetDisplayName();
    PString local_ip;
    PString auth_username;
    PString auth_password;

    for(ProxyAccountMapType::iterator it=ProxyAccountMap.begin(); it!=ProxyAccountMap.end(); it++)
    {
      ProxyAccount *proxy = it->second;
      if((proxy->domain == remote_domain || proxy->host == remote_host) && proxy->roomname == local_user && proxy->enable == 1)
      {
        local_user = proxy->username;
        local_domain = proxy->domain;
        local_dname = proxy->roomname;
        remote_domain = proxy->domain;
        remote_host = proxy->host;
        remote_port = proxy->port;
        auth_username = proxy->username;
        auth_password = proxy->password;
        break;
      }
    }

    // override, if using the default port and transport
    if(remote_port == "5060")
      remote_port = GetSectionParamFromUrl(PortKey, "sip:"+remote_user+"@"+remote_domain, remote_port);
    if(remote_transport == "*")
      remote_transport = GetSectionParamFromUrl(TransportKey, "sip:"+remote_user+"@"+remote_domain, remote_transport);
    if(remote_transport == "tls" && remote_port == "5060")
      remote_port = "5061";

    // ruri
    PString ruri_str = "sip:"+remote_user+"@"+remote_host+":"+remote_port+";transport="+remote_transport;

    // local SIP address
    PString contact_str;
    if(GetLocalSipAddress(contact_str, ruri_str) == FALSE)
      return FALSE;
    MCUURL lurl(contact_str);
    local_ip = lurl.GetHostName();
    lurl.SetUserName(local_user);
    contact_str = lurl.GetUrl();

    if(local_domain == "")
      local_domain = local_ip;
    if(local_dname == "")
      local_dname = local_user;

    sip_addr_t *sip_from = sip_from_create(&home, (url_string_t *)(const char *)("sip:"+local_user+"@"+local_domain));
    sip_from_tag(&home, sip_from, nta_agent_newtag(&home, "tag=%s", agent));

    sip_addr_t *sip_to = sip_to_create(&home, (url_string_t *)(const char *)("sip:"+remote_user+"@"+remote_domain));

    sip_contact_t *sip_contact = sip_contact_create(&home, (url_string_t *)(const char *)contact_str, NULL);

    sip_request_t *sip_rq = sip_request_create(&home, SIP_METHOD_INVITE, (url_string_t *)(const char *)ruri_str, NULL);

    // check callToken data
    PString call_id_str;
    if(callToken == "")
      call_id_str = PGloballyUniqueID().AsString();
    else
      call_id_str = callToken;
    sip_call_id_t* sip_call_id = sip_call_id_make(&home, call_id_str);

    msg_t *msg = nta_msg_create(agent, 0);
    sip_t *sip = sip_object(msg);
    sip_add_tl(msg, sip,
		SIPTAG_REQUEST(sip_rq),
		SIPTAG_FROM(sip_from),
		SIPTAG_TO(sip_to),
		SIPTAG_CALL_ID(sip_call_id),
		SIPTAG_CONTACT(sip_contact),
                TAG_END());

    // existing connection, possibly there is a bug
    callToken = GetSipCallToken(msg);
    if(ep->HasConnection(callToken))
    {
      msg_destroy(msg);
      return FALSE;
    }

    // create connection
    MCUSipConnection *sCon = MCUSipConnection::CreateConnection(DIRECTION_OUTBOUND, callToken, msg);
    msg_destroy(msg);

    if(sCon == NULL)
      return FALSE;

    sCon = (MCUSipConnection *)ep->FindConnectionWithLock(callToken);
    if(sCon == NULL)
      return FALSE;
    sCon->auth.username = auth_username;
    sCon->auth.password = auth_password;
    sCon->SendInvite();
    sCon->Unlock();

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::SipRegister(ProxyAccount *proxy, BOOL enable)
{
    PTRACE(1, trace_section << "SipRegister");

    // ruri
    PString ruri_str = "sip:"+proxy->username+"@"+proxy->host+":"+proxy->port;

    // local SIP address
    PString contact_str;
    if(GetLocalSipAddress(contact_str, ruri_str) == FALSE)
      return 0;
    MCUURL lurl(contact_str);
    lurl.SetUserName(proxy->username);
    contact_str = lurl.AsString();

    sip_addr_t *sip_from = sip_from_create(&home, (url_string_t *)(const char *)
	("sip:"+proxy->username+"@"+proxy->domain));
    sip_from_tag(&home, sip_from, nta_agent_newtag(&home, "tag=%s", agent));
    sip_from->a_display = proxy->roomname;

    sip_addr_t *sip_to = sip_to_create(&home, (url_string_t *)(const char *)
	("sip:"+proxy->username+"@"+proxy->domain));

    sip_contact_t *sip_contact = sip_contact_create(&home, (url_string_t *)(const char *)contact_str, NULL);
    sip_contact->m_display = proxy->roomname;

    sip_request_t *sip_rq = sip_request_create(&home, SIP_METHOD_REGISTER, (url_string_t *)(const char *)ruri_str, NULL);
    sip_cseq_t *sip_cseq = sip_cseq_create(&home, proxy->cseq++, SIP_METHOD_REGISTER);

    sip_call_id_t* sip_call_id = NULL;
    if(proxy->call_id == "" || !enable) // If the same Call-ID is used between register and unregister, asterisk keeps returning 401 with a new challenge
      sip_call_id = sip_call_id_create(&home, "0");
    else
      sip_call_id = sip_call_id_make(&home, proxy->call_id);
    proxy->call_id = sip_call_id->i_id;

    PString expires = "0";
    if(enable)
      expires = proxy->default_expires;

    msg_t *sip_msg = nta_msg_create(agent, 0);
    nta_outgoing_t *a_orq = nta_outgoing_mcreate(agent, wrap_response_cb1, (nta_outgoing_magic_t *)this,
      			(url_string_t *)(const char *)ruri_str,
			sip_msg,
			SIPTAG_REQUEST(sip_rq),
			SIPTAG_FROM(sip_from),
			SIPTAG_TO(sip_to),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip_call_id),
			SIPTAG_CONTACT(sip_contact),
			SIPTAG_EXPIRES_STR((const char*)expires),
			SIPTAG_MAX_FORWARDS_STR(SIP_MAX_FORWARDS),
			SIPTAG_SERVER_STR((const char*)(SIP_USER_AGENT)),
			TAG_END());
    if(a_orq == NULL)
      return 0;
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipEndPoint::ParseAuthMsg(const msg_t *msg, HTTPAuth & auth)
{
  sip_t *sip = sip_object(msg);

  int status = 0;
  if(sip->sip_status) status = sip->sip_status->st_status;

  if(status == 401 && sip->sip_www_authenticate && sip->sip_www_authenticate->au_scheme && sip->sip_www_authenticate->au_params)
  {
    auth.type = HTTPAuth::AUTH_WWW;
    auth.scheme = sip->sip_www_authenticate->au_scheme;
    auth.realm = msg_params_find(sip->sip_www_authenticate->au_params, "realm=");
    auth.nonce = msg_params_find(sip->sip_www_authenticate->au_params, "nonce=");
    auth.uri = "sip:"+PString(auth.realm); auth.uri.Replace("\"","",true,0);
    if(auth.scheme == "" || auth.realm == "")
      return FALSE;
  }
  else if(status == 407 && sip->sip_proxy_authenticate && sip->sip_proxy_authenticate->au_scheme && sip->sip_proxy_authenticate->au_params)
  {
    auth.type = HTTPAuth::AUTH_PROXY;
    auth.scheme = sip->sip_proxy_authenticate->au_scheme;
    auth.realm = msg_params_find(sip->sip_proxy_authenticate->au_params, "realm=");
    auth.nonce = msg_params_find(sip->sip_proxy_authenticate->au_params, "nonce=");
    auth.uri = "sip:"+PString(auth.realm); auth.uri.Replace("\"","",true,0);
    if(auth.scheme == "" || auth.realm == "")
      return FALSE;
  }
  else
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipEndPoint::MakeMsgAuth(msg_t *msg_orq, const msg_t *msg)
{
  sip_t *sip = sip_object(msg);
  sip_t *sip_orq = sip_object(msg_orq);

  int status = 0, request = 0;
  if(sip->sip_status)  status = sip->sip_status->st_status;
  if(sip->sip_cseq)    request = sip->sip_cseq->cs_method;

  // check authorization attempts
  if(sip_orq->sip_authorization || sip_orq->sip_proxy_authorization)
    return FALSE;

  ProxyAccount *proxy = FindProxyAccount(sip_orq->sip_from->a_url->url_user, sip_orq->sip_from->a_url->url_host);
  if(!proxy)
    return FALSE;

  // remove via header
  msg_header_remove(msg_orq, (msg_pub_t *)sip_orq, (msg_header_t *)sip_orq->sip_via);

  // cseq increment
  int cseq = sip_orq->sip_cseq->cs_seq+1;
  if(request == sip_method_register)
    cseq = proxy->cseq++;
  sip_cseq_t *sip_cseq = sip_cseq_create(&home, cseq, sip_orq->sip_cseq->cs_method, sip_orq->sip_cseq->cs_method_name);
  msg_header_replace(msg_orq, (msg_pub_t *)sip_orq, (msg_header_t *)sip_orq->sip_cseq, (msg_header_t *)sip_cseq);

  HTTPAuth auth;
  if(ParseAuthMsg(msg, auth) == FALSE)
    return FALSE;

  auth.username = proxy->username;
  auth.password = proxy->password;
  auth.method = sip->sip_cseq->cs_method_name;
  PString auth_str = auth.MakeAuthorizationStr();

  // add headers
  if(auth.type == HTTPAuth::AUTH_WWW)
  {
    sip_authorization_t *sip_auth = sip_authorization_make(&home, auth_str);
    msg_header_insert(msg_orq, (msg_pub_t *)sip_orq, (msg_header_t *)sip_auth);
  }
  else if(auth.type == HTTPAuth::AUTH_PROXY)
  {
    sip_authorization_t *sip_proxy_auth = sip_proxy_authorization_make(&home, auth_str);
    msg_header_insert(msg_orq, (msg_pub_t *)sip_orq, (msg_header_t *)sip_proxy_auth);
  }
  else
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::response_cb1(nta_outgoing_t *orq, const sip_t *sip)
{
  msg_t *msg = nta_outgoing_getresponse(orq);
  msg_t *msg_orq = nta_outgoing_getrequest(orq);
  sip_t *sip_orq = sip_object(msg_orq);
  if(!msg || !msg_orq) return 0;

  int status = 0, request = 0;
  if(sip->sip_status)  status = sip->sip_status->st_status;
  if(sip->sip_cseq)    request = sip->sip_cseq->cs_method;

  if(request == sip_method_register)
  {
    ProxyAccount *proxy = FindProxyAccount(sip->sip_from->a_url->url_user, sip->sip_from->a_url->url_host);
    if(!proxy)
    {
      nta_outgoing_destroy(orq);
      return 0;
    }

    proxy->SetStatus(msg);

    if(status != 401 && status != 407)
    {
      nta_outgoing_destroy(orq);
      return 0;
    }
  }
  if(status == 401 || status == 407)
  {
    // add authorization header
    if(MakeMsgAuth(msg_orq, msg) == FALSE)
    {
      nta_outgoing_destroy(orq);
      return 0;
    }
    nta_outgoing_mcreate(agent, wrap_response_cb1, (nta_outgoing_magic_t *)this,
 			 (url_string_t *)sip_orq->sip_request->rq_url,
			 msg_orq,
          		 TAG_END());
    nta_outgoing_destroy(orq);
    return 0;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::SendAckBye(const msg_t *msg)
{
  PTRACE(1, trace_section << "SendAckBye");
  sip_t *sip = sip_object(msg);
  if(!sip || !sip->sip_status) return 0;

  MCUURL_SIP url(msg, DIRECTION_OUTBOUND);
  PString ruri_str = url.GetUrl();
  sip_route_t* sip_route = sip_route_reverse(&home, sip->sip_record_route);

  msg_t *msg_req = nta_msg_create(agent, 0);
  sip_t *sip_req = sip_object(msg_req);
  sip_add_tl(msg_req, sip_req,
			NTATAG_STATELESS(1),
			SIPTAG_ROUTE(sip_route),
			SIPTAG_FROM(sip->sip_from),
			SIPTAG_TO(sip->sip_to),
			SIPTAG_CALL_ID(sip->sip_call_id),
			SIPTAG_MAX_FORWARDS_STR(SIP_MAX_FORWARDS),
			TAG_END());

  // ACK
  sip_request_t *sip_rq_ack = sip_request_create(&home, SIP_METHOD_ACK, (url_string_t *)(const char *)ruri_str, NULL);
  sip_cseq_t *sip_cseq_ack = sip_cseq_create(&home, sip->sip_cseq->cs_seq, SIP_METHOD_ACK);
  msg_t *msg_ack = msg_dup(msg_req);
  nta_outgoing_t *orq_ack = nta_outgoing_mcreate(agent, NULL, NULL,
			(url_string_t *)(const char *)ruri_str,
			msg_ack,
			NTATAG_BRANCH_KEY(sip->sip_via->v_branch),
 			SIPTAG_REQUEST(sip_rq_ack),
			SIPTAG_CSEQ(sip_cseq_ack),
			TAG_END());
  if(orq_ack)
    nta_outgoing_destroy(orq_ack);

  // BYE
  if(sip->sip_cseq->cs_method == sip_method_invite && sip->sip_status->st_status == 200)
  {
    sip_request_t *sip_rq_bye = sip_request_create(&home, SIP_METHOD_BYE, (url_string_t *)(const char *)ruri_str, NULL);
    sip_cseq_t *sip_cseq_bye = sip_cseq_create(&home, 0x7fffffff, SIP_METHOD_BYE);
    msg_t *msg_bye = msg_dup(msg_req);
    nta_outgoing_t *orq_bye = nta_outgoing_mcreate(agent, NULL, NULL,
			(url_string_t *)(const char *)ruri_str,
			msg_bye,
 			SIPTAG_REQUEST(sip_rq_bye),
			SIPTAG_CSEQ(sip_cseq_bye),
			TAG_END());
    if(orq_bye)
      nta_outgoing_destroy(orq_bye);
  }

  msg_destroy(msg_req);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::SendRequest(msg_t *msg)
{
  if(PThread::Current() != this)
  {
    if(sipMsgQueue.Push(msg))
      return 1;
    return 0;
  }

  sip_t *sip = sip_object(msg);
  if(sip == NULL || sip->sip_request == NULL)
  {
    msg_destroy(msg);
    return 0;
  }

  PString url_str = url_as_string(&home, sip->sip_request->rq_url);
  int ret = nta_msg_tsend(agent,
                        msg,
                        (const url_string_t *)(const char *)url_str,
			NTATAG_STATELESS(1),
			TAG_END());
  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::SipReqReply(const msg_t *msg, msg_t *msg_reply, unsigned status, const char *status_phrase)
{
  PTRACE(1, trace_section << "SipReqReply");
  sip_t *sip = sip_object(msg);
  if(sip == NULL) return 0;

  if(status_phrase == NULL) status_phrase = sip_status_phrase(status);
  if(status_phrase == NULL) return 0;

  const char *allow_str = NULL;
  if(status == 200 && sip->sip_request->rq_method == sip_method_register)
  {
    allow_str = SIP_ALLOW_METHODS_REGISTER;
  }
  if(status == 200 && sip->sip_request->rq_method == sip_method_options)
  {
    allow_str = SIP_ALLOW_METHODS_OPTIONS;
  }

  msg_t *msg_req = msg_dup(msg);
  if(msg_reply == NULL)
    msg_reply = nta_msg_create(agent, 0);
  nta_msg_mreply(agent, msg_reply, sip_object(msg_reply), status, status_phrase, msg_req,
  		   SIPTAG_ALLOW_STR(allow_str),
                   SIPTAG_SERVER_STR((const char*)(SIP_USER_AGENT)),
                   TAG_END());
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::CreateIncomingConnection(const msg_t *msg)
{
  PTRACE(1, trace_section << "CreateIncomingConnection");
  //sip_t *sip = sip_object(msg);
  //if(GetRoomAccess(sip) == "DENY")
    //return SipReqReply(msg, NULL, SIP_403_FORBIDDEN);

  //nta_leg_t *leg = nta_leg_by_call_id(agent, sip->sip_call_id->i_id);
  //MCUSipConnection *sCon = (MCUSipConnection *)nta_leg_magic(leg, NULL);
  PString callToken = GetSipCallToken(msg);
  MCUSipConnection *sCon = (MCUSipConnection *)ep->FindConnectionWithLock(callToken);
  if(!sCon)
    return SipReqReply(msg, NULL, SIP_500_INTERNAL_SERVER_ERROR);

  int response_code = sCon->ProcessInvite(msg);
  if(response_code)
  {
    SipReqReply(msg, NULL, response_code);
    sCon->ClearCall();
  }
  sCon->Unlock();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::ProcessSipEvent_cb(nta_agent_t *agent, msg_t *msg, sip_t *sip)
{
  if(terminating == 1)
    return 0;

  int request = 0, status = 0, cseq = 0;
  if(sip->sip_request) request = sip->sip_request->rq_method;
  if(sip->sip_status)  status = sip->sip_status->st_status;
  if(sip->sip_cseq)    cseq = sip->sip_cseq->cs_method;

  Registrar *registrar = OpenMCU::Current().GetRegistrar();

  // repeated responses 200/603 outside call leg
  if(cseq == sip_method_invite && (status == 200 || status == 603))
    return 0; // SendAckBye(msg);

  // keep alive
  if(cseq == sip_method_options && status)
    return registrar->OnReceivedSipOptionsResponse(msg);

  // processing requests only
  if(!sip->sip_request)
    return 0;

  // wrong RequestURI
  //PString ruri_str = MCUURL_SIP(msg).GetUrl();
  //PString stun_server = GetSectionParamFromUrl(NATStunServerKey, ruri_str);
  //PString nat_ip = GetSectionParamFromUrl(NATRouterIPKey, ruri_str);
  //PString local_ip = sip->sip_request->rq_url->url_host;
  //if(stun_server == "" && ((nat_ip != "" && nat_ip != local_ip) || (nat_ip == "" && PIPSocket::IsLocalHost(local_ip) == FALSE)))
  //  return SipReqReply(msg, NULL, SIP_400_BAD_REQUEST);

  if(request == sip_method_invite)
  {
    SipReqReply(msg, NULL, SIP_100_TRYING);
    // empty payload header for invite
    if(!sip->sip_payload || !sip->sip_payload->pl_data)
      return SipReqReply(msg, NULL, SIP_415_UNSUPPORTED_MEDIA);
    // reinvite request outside call leg
    if(sip->sip_to->a_tag)
      return SipReqReply(msg, NULL, SIP_481_NO_TRANSACTION);
    // existing connection, repeated INVITE, possibly there is a bug
    PString callToken = GetSipCallToken(msg);
    if(ep->HasConnection(callToken))
      return 0;
    // redirect to the registrar
    return registrar->OnReceivedSipInvite(msg);
  }
  if(request == sip_method_cancel)
  {
    return SipReqReply(msg, NULL, 481); // SIP_481_NO_TRANSACTION
  }

  if(request == sip_method_bye)
  {
    // bye outside call leg
    return SipReqReply(msg, NULL, 481); // SIP_481_NO_TRANSACTION
  }
  if(request == sip_method_register)
  {
    return registrar->OnReceivedSipRegister(msg);
  }
  if(request == sip_method_message)
  {
    return registrar->OnReceivedSipMessage(msg);
  }
  if(request == sip_method_subscribe)
  {
    return registrar->OnReceivedSipSubscribe(msg);
  }
  if(request == sip_method_ack)
  {
    return SipReqReply(msg, NULL, 200);
  }
  if(request == sip_method_notify)
  {
    return SipReqReply(msg, NULL, 200);
  }
  if(request == sip_method_info)
  {
    return SipReqReply(msg, NULL, 200);
  }
  if(request == sip_method_options)
  {
    return SipReqReply(msg, NULL, 200);
  }
  if(request == sip_method_publish)
  {
    return SipReqReply(msg, NULL, 200);
  }
  if(request != 0)
  {
    return SipReqReply(msg, NULL, 501); // SIP_501_NOT_IMPLEMENTED
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::ProcessProxyAccount()
{
  PWaitAndSignal m(sipMutex);

  for(ProxyAccountMapType::iterator it=ProxyAccountMap.begin(); it!=ProxyAccountMap.end(); it++)
  {
    ProxyAccount *proxy = it->second;
    PTime now;
    if(proxy->enable && now > proxy->start_time + PTimeInterval(proxy->proxy_expires*1000))
    {
      proxy->status = 0;
      proxy->status_phrase = "Registration ...";
      proxy->start_time = now;
      proxy->proxy_expires = proxy->default_expires;
      proxy->SetStatus(NULL);
      SipRegister(proxy, TRUE);
    }
    if(!proxy->enable && proxy->status == 200)
    {
      proxy->status = 0;
      proxy->status_phrase = "Unregister ...";
      proxy->start_time = PTime(0);
      proxy->SetStatus(NULL);
      SipRegister(proxy, FALSE);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ProxyAccount * MCUSipEndPoint::FindProxyAccount(PString username, PString domain)
{
  PWaitAndSignal m(sipMutex);

  for(ProxyAccountMapType::iterator it = ProxyAccountMap.begin(); it != ProxyAccountMap.end(); ++it)
  {
    ProxyAccount *proxy = it->second;
    if(username == proxy->username && (domain == proxy->domain || domain == proxy->host))
      return proxy;
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::ClearProxyAccounts()
{
  sipMutex.Wait();
  for(ProxyAccountMapType::iterator it = ProxyAccountMap.begin(); it != ProxyAccountMap.end(); ++it)
  {
    if(it->second->status == 200)
      SipRegister(it->second, FALSE);
  }
  sipMutex.Signal();

  su_root_sleep(root, 500);

  sipMutex.Wait();
  for(ProxyAccountMapType::iterator it = ProxyAccountMap.begin(); it != ProxyAccountMap.end(); )
  {
    delete it->second;
    ProxyAccountMap.erase(it++);
  }
  sipMutex.Signal();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::InitProxyAccounts()
{
  PWaitAndSignal m(sipMutex);

  PString sectionPrefix = "SIP Proxy Account ";
  PStringList sect = MCUConfig("Parameters").GetSectionsPrefix(sectionPrefix);
  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    MCUConfig scfg(sect[i]);
    PString name = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());
    name.Replace("sip:","",TRUE,0);
    if(name.Find("@") == P_MAX_INDEX) name = name+"@";
    name = "sip:"+name;
    PString username = MCUURL(name).GetUserName();
    PString domain = MCUURL(name).GetHostName();
    BOOL enable = scfg.GetBoolean(EnableKey);
    PString roomname = scfg.GetString(RoomNameKey);
    PString address = scfg.GetString(AddressKey);
    if(address.Left(4) != "sip:") address = "sip:"+address;
    if(address.Find("@") == P_MAX_INDEX) address.Replace("sip:","sip:@",TRUE,0);
    PString host = MCUURL(address).GetHostName();
    PString port = MCUURL(address).GetPort();
    PString transport = MCUURL(address).GetTransport();
    PString password = PHTTPPasswordField::Decrypt(scfg.GetString(PasswordKey));
    unsigned expires = scfg.GetInteger(ExpiresKey);

    if(username == "") continue;
    if(domain == "") domain = host;
    if(domain == "") continue;

    ProxyAccount *proxy = FindProxyAccount(username, domain);
    if(proxy && proxy->enable)
    {
      proxy->enable = enable;
      continue;
    }
    if(!proxy)
    {
      proxy = new ProxyAccount();
      proxy->username = username;
      proxy->domain = domain;
      ProxyAccountMap.insert(ProxyAccountMapType::value_type(proxy->username+"@"+proxy->domain, proxy));
    }
    proxy->username = username;
    proxy->domain = domain;
    proxy->enable = enable;
    proxy->roomname = roomname;
    proxy->host = host;
    proxy->port = port;
    proxy->transport = transport;
    proxy->password = password;
    proxy->default_expires = expires;
    proxy->proxy_expires = expires;
    //
    proxy->start_time = PTime(0);
    proxy->call_id = "";
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::CreateBaseSipCaps()
{
  PWaitAndSignal m(sipMutex);
  ClearSipCaps(BaseSipCaps);
  CreateSipCaps(BaseSipCaps, "SIP Audio", "SIP Video");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PSTUNClient * MCUSipEndPoint::CreateStun(PString address)
{
  if(address == "")
    return NULL;

  PSTUNClient *stun = new PSTUNClient(address, ep->GetUDPPortBase(), ep->GetUDPPortMax(), ep->GetRtpIpPortBase(), ep->GetRtpIpPortMax());
  stun->SetTimeout(800);
  stun->SetRetries(2);
  if(stun->IsAvailable() == FALSE)
  {
    MCUTRACE(1, "MCUSIP\t" << "failed create STUN client, server \"" << address << "\" not available");
    delete stun;
    return NULL;
  }

  MCUTRACE(1, "MCUSIP\t" << "create STUN client, server \"" << address << "\" " << stun->GetServer());
  return stun;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PSTUNClient * MCUSipEndPoint::GetPreferedStun(PString address)
{
  PWaitAndSignal m(sipMutex);

  PString stun_name;
  PSTUNClient *stun = NULL;
  for(StunMapType::iterator it = StunMap.begin(); it != StunMap.end(); ++it)
  {
    if(address == "auto")
    {
      stun_name = it->first;
      stun = it->second;
      break;
    }
    else if(address == it->first)
    {
      stun_name = it->first;
      stun = it->second;
      break;
    }
  }

  if(stun != NULL)
  {
    MCUTRACE(1, "MCUSIP\t" << "found STUN server \"" << stun_name << "\" " << stun->GetServer());
    // return clone
    return new PSTUNClient(*stun);
  }

  if(address != "" && address != "auto")
  {
    // create new stun
    return CreateStun(address);
  }

  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::InitStunList()
{
  PWaitAndSignal m(sipMutex);
  ClearStunList();

  PStringArray stun_list = MCUConfig("SIP Parameters").GetString(NATStunListKey).Tokenise(",");
  for(PINDEX i = 0; i < stun_list.GetSize(); i++)
  {
    PString address = stun_list[i];
    if(StunMap.find(address) != StunMap.end())
      continue;

    PSTUNClient *stun = CreateStun(address);
    if(stun != NULL)
      StunMap.insert(StunMapType::value_type(address, stun));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::ClearStunList()
{
  PWaitAndSignal m(sipMutex);
  for(StunMapType::iterator it = StunMap.begin(); it != StunMap.end(); )
  {
    delete it->second;
    StunMap.erase(it++);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipEndPoint::HasListener(PString addr)
{
  if(addr.Left(4) != "sip:") addr = "sip:"+addr;
  if(addr.Find("@") == P_MAX_INDEX) addr.Replace("sip:","sip:@",TRUE,0);
  MCUURL url(addr);
  PString host = url.GetHostName();
  PString port = url.GetPort();

  PWaitAndSignal m(sipMutex);
  for(tport_t *tp = nta_agent_tports(agent); tp != NULL; tp = tport_next(tp))
  {
    tp_name_t const *tp_name = tport_name(tp);
    //PString proto = tp_name->tpn_proto;
    if(host == (PString)tp_name->tpn_host && port == (PString)tp_name->tpn_port)
    {
      PTRACE(1, trace_section << "tport found: " << host << ":" << port);
      return TRUE;
    }
  }

  PTRACE(1, trace_section << "tport not found: " << host << ":" << port);
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::RemoveListeners()
{
  PWaitAndSignal m(sipMutex);
  nta_agent_close_tports(agent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::StartListeners()
{
  PWaitAndSignal m(sipMutex);

  RemoveListeners();
  for(PINDEX i = 0; i < sipListenerArray.GetSize(); i++)
  {
    PString listener = sipListenerArray[i];
    MCUURL url(listener);
    PString transport = url.GetTransport();
    if(transport != "tls")
      nta_agent_add_tport(agent, URL_STRING_MAKE((const char*)listener), TAG_NULL());

    if(transport == "*" || transport == "tls")
    {
      PString certificatePath = PString(SYS_SSL_DIR);
      unsigned port = atoi(url.GetPort());
      if(transport == "*")
        port++;
      listener = "sips:"+url.GetHostName()+":"+PString(port)+";transport=tls";
      nta_agent_add_tport(agent, URL_STRING_MAKE((const char*)listener),
                          TPTAG_CERTIFICATE((const char*)certificatePath),
                          //TPTAG_TLS_VERSION(0),
                          //TPTAG_TLS_VERIFY_DATE(0),
                          //TPTAG_TLS_VERIFY_DEPTH(0),
                          //TPTAG_TLS_VERIFY_PEER(TPTLS_VERIFY_NONE),
                          //TPTAG_TLS_VERIFY_POLICY(TPTLS_VERIFY_NONE),
                          //TPTAG_TLS_VERIFY_POLICY(TPTLS_VERIFY_ALL),
                          //TPTAG_TLS_VERIFY_POLICY(TPTLS_VERIFY_SUBJECTS_ALL),
                          //TPTAG_X509_SUBJECT(0),
                          //TPTAG_TLS_VERIFY_SUBJECTS(NULL),
                          TAG_NULL());
    }
  }

  for(tport_t *tp = nta_agent_tports(agent); tp != NULL; tp = tport_next(tp))
  {
    tp_name_t const *tp_name = tport_name(tp);
    PTRACE(1, trace_section << "Listener start " << tp_name->tpn_host << ":" << tp_name->tpn_port << ";transport=" << tp_name->tpn_proto);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipEndPoint::GetLocalSipAddress(PString & local_addr, const PString & ruri_str)
{
  PWaitAndSignal m(sipMutex);

  tp_name_t tpn[1] = {{ NULL }};
  tport_t *tp = NULL;
  const tp_name_t *tp_name = NULL;

  if(tport_name_by_url(&home, tpn, (url_string_t *)(const char *)ruri_str) == -1)
    goto error;

  tp = tport_primary_by_name(nta_agent_tports(agent), tpn);
  if(tp == NULL)
    goto error;

  tp_name = tport_name(tp);
  if(tp_name == NULL)
    goto error;

  local_addr = "sip:@"+(PString)tp_name->tpn_host+":"+(PString)tp_name->tpn_port+";transport="+(PString)tp_name->tpn_proto;
  MCUTRACE(1, "MCUSIP found local SIP address " << local_addr << " for remote " << ruri_str);
  return TRUE;

  error:
    MCUTRACE(1, "MCUSIP not found local SIP address for remote " << ruri_str);
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipEndPoint::GetLocalSipAddress(PString & local_addr, const msg_t *msg)
{
  PWaitAndSignal m(sipMutex);
  for(tport_t *tp = nta_agent_tports(agent); tp != NULL; tp = tport_next(tp))
  {
    tport_t *msg_tp = tport_delivered_by(tp, msg);
    if(msg_tp)
    {
      tp_name_t const *tp_name = tport_name(msg_tp);
      local_addr = "sip:"+(PString)tp_name->tpn_host+":"+(PString)tp_name->tpn_port+";transport="+(PString)tp_name->tpn_proto;
      MCUTRACE(1, "MCUSIP found local SIP address " << local_addr << " from msg " << msg);
      return TRUE;
    }
  }

  MCUTRACE(1, "MCUSIP not found local SIP address from msg " << msg);
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::ProcessSipQueue()
{
  for(;;)
  {
    PString *cmd = sipQueue.Pop();
    if(cmd == NULL)
      break;
    PINDEX pos = cmd->Find(":");
    PString request = cmd->Left(pos);
    PString data = cmd->Right(cmd->GetLength()-pos-1);
    if(request == "invite")
    {
      PStringArray dataArray = data.Tokenise(",");
      PString from = dataArray[0];
      PString to = dataArray[1];
      PString call_id = dataArray[2];
      SipMakeCall(from, to, call_id);
    }
    else if(request == "clear_call")
    {
      PStringArray dataArray = data.Tokenise(",");
      PString callToken = dataArray[0];
      PString reason = dataArray[1];
      MCUSipConnection *sCon = (MCUSipConnection *)ep->FindConnectionWithLock(callToken);
      if(sCon)
      {
        sCon->ClearCall((H323Connection::CallEndReason)reason.AsInteger());
        sCon->Unlock();
      }
    }
    else if(request == "connection_send_request")
    {
      //connection_send_request:method,method_name,payload
      PStringArray dataArray = data.Tokenise(",");
      PString callToken = dataArray[0];
      PString method = dataArray[1];
      PString method_name = dataArray[2];
      PString payload = dataArray[3];
      MCUSipConnection *sCon = (MCUSipConnection *)ep->FindConnectionWithLock(callToken);
      if(sCon)
      {
        sCon->SendRequest((sip_method_t)method.AsInteger(), method_name, payload);
        sCon->Unlock();
      }
    }
    delete cmd;
  }
  for(;;)
  {
    msg_t *msg = sipMsgQueue.Pop();
    if(msg == NULL)
      break;
    SendRequest(msg);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::MainLoop()
{
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
      init_proxy_accounts = 1;
      init_caps = 1;
      init_stun = 1;
    }
    if(init_config)
    {
      init_config = 0;
      StartListeners();
      init_stun = 1;
    }
    if(init_proxy_accounts)
    {
      init_proxy_accounts = 0;
      InitProxyAccounts();
    }
    if(init_caps)
    {
      init_caps = 0;
      CreateBaseSipCaps();
    }
    if(init_stun)
    {
      init_stun = 0;
      InitStunList();
    }
    ProcessSipQueue();
    ProcessProxyAccount();
    su_root_sleep(root,500);
    PTRACE(9, trace_section << "SIP Down to sleep");
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::Terminating()
{
  PWaitAndSignal m(sipMutex);
  ProcessSipQueue();
  ClearProxyAccounts();
  ClearSipCaps(BaseSipCaps);
  ClearStunList();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::Main()
{
  su_init();
  su_home_init(&home);
  root = su_root_create(NULL);
  if(root == NULL) return;

  su_log_set_level(NULL, 9);
  setenv("TPORT_LOG", "1", 1);
  su_log_redirect(NULL, MCUSipLoggerFunc, NULL);

  agent = nta_agent_create(root, (url_string_t *)-1, ProcessSipEventWrap_cb, (nta_agent_magic_t *)this,
                           NTATAG_UDP_MTU(64000),
                           TAG_NULL());

  nta_agent_set_params(agent,
                           NTATAG_SIP_T1(1000), // Initial retransmission interval (in milliseconds)
                           //NTATAG_SIP_T2(1000), // Maximum retransmission interval (in milliseconds)
                           NTATAG_SIP_T1X64(3000), // Transaction timeout (defaults to T1 * 64)
                           //NTATAG_TIMER_C(2999), // two invite requests ???
                           //NTATAG_UA(1), // If true, NTA acts as User Agent Server or Client by default
                           TAG_NULL());

  sip_rtp_init();
  if(agent != NULL)
  {
    MainLoop();
    nta_agent_destroy(agent);
  }
  sip_rtp_shutdown();

  su_root_destroy(root);
  root = NULL;
  su_home_deinit(&home);
  su_deinit();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

