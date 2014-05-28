
#include <ptlib.h>
#include <sys/types.h>
#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  define setenv(n,v,f) _putenv(n "=" v)
#else
#  include <sys/socket.h>
#endif

#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

PString GetFromIp(PString toAddr, PString toPort)
{
    PTRACE(1, "MCUSIP\tGetFromIp address:" << toAddr << " port:" << toPort);
    if(toAddr == "" || toPort == "") return "";
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1) return "";

    struct addrinfo serv;
    struct addrinfo *res=NULL;
    memset((void *)&serv, 0, sizeof(serv));
    serv.ai_family = AF_INET;
    serv.ai_socktype=SOCK_DGRAM;
    int err=getaddrinfo((const char *)toAddr, (const char *)toPort, &serv, &res);
    if (err != 0 || res == NULL) return "";

    err = connect(sock, res->ai_addr, res->ai_addrlen);
    if(err == -1) return "";

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (sockaddr*) &name, &namelen);
    if(err == -1) return "";

#ifndef _WIN32
    char buffer[16];
    inet_ntop(AF_INET, (const void *)&name.sin_addr, buffer, 16);
    close(sock);
    return (PString)buffer;
#else
    PString buffer0 = PIPSocket::Address(name.sin_addr);
    closesocket(sock);
    return buffer0;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUURL_SIP::MCUURL_SIP(const msg_t *msg, Direction dir)
  : MCUURL()
{
  sip_t *sip = sip_object(msg);
  su_home_t *home = msg_home(msg);
  if(sip == NULL || home == NULL)
    return;

  sip_addr_t *sip_addr;
  if(dir == DIRECTION_INBOUND) sip_addr = sip->sip_from;
  else                         sip_addr = sip->sip_to;

  // display name
  if(sip_addr->a_display && PString(sip_addr->a_display) != "")
    display_name = sip_addr->a_display;
  else if(sip->sip_contact && sip->sip_contact->m_display && PString(sip->sip_contact->m_display) != "")
    display_name = sip->sip_contact->m_display;
  else
    display_name = sip_addr->a_url->url_user;
  display_name.Replace("\"","",TRUE,0);
  display_name = PURL::UntranslateString(display_name, PURL::QueryTranslation);

  // username
  if(sip->sip_contact && sip->sip_contact->m_url && PString(sip->sip_contact->m_url->url_user) != "")
    username = sip->sip_contact->m_url->url_user;
  else if(sip_addr->a_url->url_user && PString(sip_addr->a_url->url_user) != "")
    username = sip_addr->a_url->url_user;
  username.Replace("\"","",TRUE,0);
  username = PURL::UntranslateString(username, PURL::QueryTranslation);

  // domain
  domain_name = sip_addr->a_url->url_host;

  // remote application
  if(sip->sip_user_agent && sip->sip_user_agent->g_string)
    remote_application = sip->sip_user_agent->g_string;
  else if(sip->sip_server && sip->sip_server->g_string)
    remote_application = sip->sip_server->g_string;

  // get hostname,port,transport from su_addrinfo_t
/*
  su_addrinfo_t *addrinfo = msg_addrinfo((msg_t *)msg);
  if(addrinfo->ai_addr)
  {
    char ip[80] = {0};
    getnameinfo(addrinfo->ai_addr, addrinfo->ai_addrlen, ip, (socklen_t)sizeof(ip), NULL, 0, NI_NUMERICHOST);
    hostname = ip;
    port = ntohs(((struct sockaddr_in *)addrinfo->ai_addr)->sin_port);
    if(addrinfo->ai_protocol == TPPROTO_UDP)
      transport = "udp";
    else if(addrinfo->ai_protocol == TPPROTO_TCP)
      transport = "tcp";
    else if(addrinfo->ai_protocol == TPPROTO_TLS)
      transport = "tls";
  }
*/

  // hostname
  if(hostname == "")
  {
    if(sip->sip_request && sip->sip_via && PString(sip->sip_via->v_host) != "0.0.0.0")
      hostname = sip->sip_via->v_host;
    //if(sip->sip_contact && sip->sip_contact->m_url && PString(sip->sip_contact->m_url->url_host) != "")
    //  hostname = sip->sip_contact->m_url->url_host;
    else
      hostname = sip_addr->a_url->url_host;
  }

  // port
  if(port == 0)
  {
    if(sip->sip_request && sip->sip_via && sip->sip_via->v_port)
      port = atoi(sip->sip_via->v_port);
    else if(sip_addr->a_url->url_port)
      port = atoi(sip_addr->a_url->url_port);
    //else if(sip->sip_contact && sip->sip_contact->m_url->url_port)
    //  port = sip->sip_contact->m_url->url_port;
  }

  // transport
  if(transport == "")
  {
    if(sip->sip_via && sip->sip_via->v_protocol && PString(sip->sip_via->v_protocol).Find("UDP") != P_MAX_INDEX)
      transport = "udp";
    else if(sip->sip_via && sip->sip_via->v_protocol && PString(sip->sip_via->v_protocol).Find("TCP") != P_MAX_INDEX)
      transport = "tcp";
  }

  url_scheme = "sip";
  url_party = url_scheme+":"+username+"@"+hostname;
  if(port != 0)
    url_party += ":"+PString(port);
  if(transport != "" && transport != "*")
    url_party += ";transport="+transport;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ProxyAccount::SetStatus(const sip_t *sip)
{
  status = sip->sip_status->st_status;
  status_phrase = sip->sip_status->st_phrase;
  if(sip->sip_expires && sip->sip_expires->ex_delta == 0)
  {
    status = 0;
    status_phrase = "Registration canceled";
  }
  PString msg = "<font color=blue>"+roomname+" - "+username+"@"+domain+" status: "+status_phrase+"</font>";
  OpenMCU::Current().HttpWriteEvent(msg);
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
#endif
  if(trace == "" || trace == "   ")
    return;
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

BOOL GetSipCapabilityParams(PString capname, PString & name, int & pt, int & rate, PString & fmtp)
{
  if(capname == "")
    return FALSE;

  H323Capability *cap = H323Capability::Create(capname);
  if(!cap)
    return FALSE;

  name = capname.ToLower();
  if(name.Find("ulaw") != P_MAX_INDEX)         { name = "pcmu"; }
  else if(name.Find("alaw") != P_MAX_INDEX)    { name = "pcma"; }
  else if(name.Find("723") != P_MAX_INDEX)     { name = "g723"; }
  else if(name.Find("726-16") != P_MAX_INDEX)  { name = "g726-16"; }
  else if(name.Find("726-24") != P_MAX_INDEX)  { name = "g726-24"; }
  else if(name.Find("726-32") != P_MAX_INDEX)  { name = "g726-32"; }
  else if(name.Find("726-40") != P_MAX_INDEX)  { name = "g726-40"; }
  else if(name.Find("729-") != P_MAX_INDEX)    return FALSE;
  else if(name.Find("729a") != P_MAX_INDEX)    { name = "g729"; fmtp = "annexb=no;"; }
  else if(name.Find("h.263p") != P_MAX_INDEX)  { name = "h263-1998"; }
  else
  {
    name.Replace(".","",TRUE,0);
    name = name.Left(name.Find("-")).Left(name.Find("_"));
  }

  const OpalMediaFormat & mf = cap->GetMediaFormat();
  rate = mf.GetTimeUnits()*1000;

  // only for audio use the default fmtp
  if(fmtp == "" && rate != 90000)
  {
    for (PINDEX i = 0; i < mf.GetOptionCount(); i++)
    {
      if(mf.GetOption(i).GetFMTPName() != "" && mf.GetOption(i).GetFMTPDefault() != mf.GetOption(i).AsString())
        fmtp += mf.GetOption(i).GetFMTPName()+"="+mf.GetOption(i).AsString()+";";
    }
  }

  if     (name == "pcmu")   { pt = 0; }
  else if(name == "pcma")   { pt = 8; }
  else if(name == "g723")   { pt = 4; }
  else if(name == "g722")   { pt = 9; }
  else if(name == "g728")   { pt = 15; }
  else if(name == "g729")   { pt = 18; }
  else if(name == "h261")   { pt = 31; }
  else if(name == "h263")   { pt = 34; }
  else                      { pt = 128; }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SipCapability * FindSipCap(SipCapMapType & SipCapMap, int payload)
{
  if(payload < 0) return NULL;
  for(SipCapMapType::iterator it = SipCapMap.begin(); it != SipCapMap.end(); it++)
  {
    if(it->second->payload == payload)
      return it->second;
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SipCapability * FindSipCap(SipCapMapType & SipCapMap, PString capname)
{
  for(SipCapMapType::iterator it = SipCapMap.begin(); it != SipCapMap.end(); it++)
  {
    if(it->second->capname == capname)
      return it->second;
  }
  return NULL;
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

MCUSipConnection::MCUSipConnection(MCUSipEndPoint *_sep, MCUH323EndPoint *_ep, Direction _direction, PString _callToken)
  :MCUH323Connection(*_ep, 0, NULL), sep(_sep)
{
  connectionState = NoConnectionActive;
  direction = _direction;
  callToken = _callToken;
  remoteName = "";
  remotePartyName = "";
  remoteApplication = "SIP terminal";
  requestedRoom = "room101";
  scap = -1;
  vcap = -1;
  connectedTime = PTime();
  cseq_num = 100;
  c_sip_msg = NULL;

  MCUTRACE(1, "MCUSipConnection constructor, callToken: "+callToken+" contact: "+contact_str+" ruri: "+ruri_str);
  OnCreated();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSipConnection::MCUSipConnection(MCUSipEndPoint *_sep, MCUH323EndPoint *_ep, Direction _direction, PString _callToken, const msg_t *msg)
  :MCUH323Connection(*_ep, 0, NULL), sep(_sep)
{
  connectionState = NoConnectionActive;
  direction = _direction;
  callToken = _callToken;
  remoteName = "";
  remotePartyName = "";
  remoteApplication = "SIP terminal";
  scap = -1;
  vcap = -1;
  connectedTime = PTime();
  cseq_num = 100;
  aDataSocket = aControlSocket = vDataSocket = vControlSocket = NULL;
  audio_rtp_port = video_rtp_port = 0;
  c_sip_msg = NULL;

  su_home_t *home = msg_home(msg);
  sip_t *sip = sip_object(msg);
  if(direction == DIRECTION_INBOUND)
  {
    ruri_str = MCUURL_SIP(msg).GetUrl();
    contact_str = url_as_string(home, sip->sip_request->rq_url);
  }
  else if(direction == DIRECTION_OUTBOUND)
  {
    ruri_str = url_as_string(home, sip->sip_request->rq_url);
    contact_str = url_as_string(home, sip->sip_contact->m_url);
  }

  MCUURL url(contact_str);
  local_user = url.GetUserName();
  local_ip = url.GetHostName();
  if(local_user == "") local_user = OpenMCU::Current().GetDefaultRoomName();
  if(local_user == "") local_user = "room101";
  UpdateLocalContact();

  requestedRoom = local_user;

  // endpoint parameters
  pref_audio_cap = GetEndpointParamFromUrl("Audio codec", ruri_str);
  if(pref_audio_cap != "" && pref_audio_cap.Right(4) != "{sw}")
    pref_audio_cap += "{sw}";
  pref_video_cap = GetEndpointParamFromUrl("Video codec", ruri_str);
  rtp_proto = GetEndpointParamFromUrl("RTP proto", ruri_str, "RTP");
  remote_bw = GetEndpointParamFromUrl("Bandwidth to MCU", ruri_str, 0);
  display_name = GetEndpointParamFromUrl("Display name", ruri_str);

  if(direction == DIRECTION_INBOUND)
  {
    // waiting ACK signal
    connectionState = AwaitingSignalConnect;
  }
  else if(direction == DIRECTION_OUTBOUND)
  {
    // waiting OK signal
    connectionState = AwaitingSignalConnect;
    // create temp rtp sockets
    CreateTempSockets();
    // create sdp for invite
    CreateSdpInvite();
  }

  // add to the list of connections
  OnCreated();
  MCUTRACE(1, "MCUSipConnection constructor, callToken: "+callToken+" contact: "+contact_str+" ruri: "+ruri_str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::UpdateLocalContact()
{
  contact_str = "sip:"+local_user+"@"+local_ip;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipConnection::CreateTempSockets()
{
  unsigned localDataPort = OpenMCU::Current().GetEndpoint().GetRtpIpPortPair();
  PQoS * dataQos = NULL;
  PQoS * ctrlQos = NULL;
  aDataSocket = new PUDPSocket(dataQos);
  aControlSocket = new PUDPSocket(ctrlQos);
  vDataSocket = new PUDPSocket(dataQos);
  vControlSocket = new PUDPSocket(ctrlQos);
  while(!aDataSocket->Listen(local_ip, 1, localDataPort) || !aControlSocket->Listen(local_ip, 1, localDataPort+1))
  {
    aDataSocket->Close();
    aControlSocket->Close();
    localDataPort = OpenMCU::Current().GetEndpoint().GetRtpIpPortPair();
  }
  audio_rtp_port = localDataPort;
  localDataPort = OpenMCU::Current().GetEndpoint().GetRtpIpPortPair();
  while(!vDataSocket->Listen(local_ip, 1, localDataPort) || !vControlSocket->Listen(local_ip, 1, localDataPort+1))
  {
    vDataSocket->Close();
    vControlSocket->Close();
    localDataPort = OpenMCU::Current().GetEndpoint().GetRtpIpPortPair();
  }
  video_rtp_port = localDataPort;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::DeleteTempSockets()
{
  if(aDataSocket) { aDataSocket->Close(); delete aDataSocket; aDataSocket = NULL; }
  if(aControlSocket) { aControlSocket->Close(); delete aControlSocket; aControlSocket = NULL; }
  if(vDataSocket) { vDataSocket->Close(); delete vDataSocket; vDataSocket = NULL; }
  if(vControlSocket) { vControlSocket->Close(); delete vControlSocket; vControlSocket = NULL; }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::RefreshLocalSipCaps()
{
  LocalSipCaps.clear();
  for(SipCapMapType::iterator it = sep->GetBaseSipCaps().begin(); it != sep->GetBaseSipCaps().end(); it++)
  {
    SipCapability *base_sc = it->second;

    if(base_sc->media == 0 && pref_audio_cap != "" && pref_audio_cap != base_sc->capname)
      continue;
    if(base_sc->media == 1 && pref_video_cap != "" && pref_video_cap != base_sc->capname)
      continue;

    SipCapability *local_sc = new SipCapability(*base_sc);
    LocalSipCaps.insert(SipCapMapType::value_type(LocalSipCaps.size(), local_sc));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::CreateSdpOk()
{
  SipCapMapType SipCaps;
  if(scap >= 0)
  {
    SipCapability *remote_sc = FindSipCap(RemoteSipCaps, scap);
    if(remote_sc)
    {
      SipCapability *sc = new SipCapability(*remote_sc);
      SipCapability *local_sc = FindSipCap(LocalSipCaps, sc->capname);
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
    SipCapability *remote_sc = FindSipCap(RemoteSipCaps, vcap);
    if(remote_sc)
    {
      SipCapability *sc = new SipCapability(*remote_sc);
      SipCapability *local_sc = FindSipCap(LocalSipCaps, sc->capname);
      if(local_sc)
      {
        // send back the received fmtp
        if(local_sc->fmtp != "") sc->fmtp = local_sc->fmtp;
      }
      SipCaps.insert(SipCapMapType::value_type(SipCaps.size(), sc));
    }
  }
  LocalSipCaps = SipCaps;
  sdp_ok_str = CreateSdpStr();
  RefreshLocalSipCaps(); // update from base
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::CreateSdpInvite()
{
  RefreshLocalSipCaps(); // update from base
  sdp_invite_str = CreateSdpStr();
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sdp_rtpmap_t * MCUSipConnection::CreateSdpRtpmap(su_home_t *sess_home, SipCapability *sc)
{
  sdp_rtpmap_t *rm = (sdp_rtpmap_t *)su_salloc(sess_home, sizeof(*rm));
  rm->rm_predef = 0;
  rm->rm_pt = sc->payload;
  rm->rm_encoding = PStringToChar(sc->format);
  rm->rm_rate = sc->clock;
  if(sc->fmtp != "") rm->rm_fmtp = PStringToChar(sc->fmtp);
  if(sc->params != "") rm->rm_params = PStringToChar(sc->params);
  return rm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sdp_media_t * MCUSipConnection::CreateSdpMedia(su_home_t *sess_home, sdp_media_e m_type, sdp_proto_e m_proto)
{
  sdp_media_t *m = (sdp_media_t *)su_salloc(sess_home, sizeof(*m));
  m->m_type = m_type;
  m->m_proto = m_proto;

  if(m_type == sdp_media_audio)
    m->m_port = audio_rtp_port;
  else if(m_type == sdp_media_video)
    m->m_port = video_rtp_port;

  sdp_rtpmap_t *rm = NULL; // rtpmap iterator
  for(SipCapMapType::iterator it = LocalSipCaps.begin(); it != LocalSipCaps.end(); it++)
  {
    SipCapability *sc = it->second;

    if(m_type == sdp_media_audio && sc->media != 0)
      continue;
    else if(m_type == sdp_media_video && sc->media != 1)
      continue;

    if(m->m_mode == 0)
      m->m_mode = sc->mode;

    sdp_rtpmap_t *rm_new = CreateSdpRtpmap(sess_home, sc);
    if(!rm_new)
      continue;

    // do not duplicate capability
    sdp_rtpmap_t *rm_find = sdp_rtpmap_find_matching(m->m_rtpmaps, rm_new);
    if(rm_find)
    {
      if(sc->fmtp == "")         { continue; }
      else if(!rm_find->rm_fmtp) { rm_find->rm_fmtp = PStringToChar(sc->fmtp); continue; }
    }

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

PString MCUSipConnection::CreateSdpStr()
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

  o->o_username = PStringToChar(local_user);
  o->o_id = rand();
  o->o_version = 1;
  o->o_address = c;

  c->c_nettype = sdp_net_in;
  c->c_addrtype = sdp_addr_ip4;
  c->c_address = PStringToChar(local_ip);

  if(remote_bw)
  {
    b->b_modifier = sdp_bw_as;
    b->b_value = remote_bw;
    sess->sdp_bandwidths = b;
  }

  // create default RTP media
  sdp_media_t *m_audio_rtp = CreateSdpMedia(sess_home, sdp_media_audio, sdp_proto_rtp);
  sdp_media_t *m_video_rtp = CreateSdpMedia(sess_home, sdp_media_video, sdp_proto_rtp);
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

SipRTP_UDP *MCUSipConnection::CreateRTPSession(SipCapability *sc)
{
  int id = (!sc->media)?RTP_Session::DefaultAudioSessionID:RTP_Session::DefaultVideoSessionID;
  SipRTP_UDP *session = (SipRTP_UDP *)(rtpSessions.UseSession(id));
  if(session == NULL)
  {
    session = new SipRTP_UDP(
#ifdef H323_RTP_AGGREGATE
                useRTPAggregation ? endpoint.GetRTPAggregator() : NULL,
#endif
                id, remoteIsNAT);
    rtpSessions.AddSession(session);
    PIPSocket::Address lIP(local_ip);
    PIPSocket::Address rIP(sc->remote_ip);
    unsigned port_base = endpoint.GetRtpIpPortBase();
    unsigned port_max = endpoint.GetRtpIpPortMax();
    if(sc->media == 0 && audio_rtp_port)
    {
      port_base = port_max = audio_rtp_port;
      audio_rtp_port = 0;
    }
    else if(sc->media == 1 && video_rtp_port)
    {
      port_base = port_max = video_rtp_port;
      video_rtp_port = 0;
    }
    session->Open(lIP,port_base,port_max,endpoint.GetRtpIpTypeofService(),*this,NULL,NULL);
    session->SetRemoteSocketInfo(rIP,sc->remote_port,TRUE);
    if(sc->media == 0)      audio_rtp_port = session->GetLocalDataPort();
    else if(sc->media == 1) video_rtp_port = session->GetLocalDataPort();
  }
  return session;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::CreateMediaChannel(int pt, int dir)
{
  if(pt < 0) return 0;
  SipCapability *sc = FindSipCap(RemoteSipCaps, pt);
  if(!sc) return 0;

  H323Capability * cap = sc->cap;
  if(!cap) return 0;

  if(sc->remote_ip == "" || sc->remote_port == 0)
    return 0;

  SipRTP_UDP *session = NULL;
  if(sc->secure_type == SECURE_TYPE_NONE)
  {
    session = CreateRTPSession(sc);
  }
  else if(sc->secure_type == SECURE_TYPE_ZRTP)
  {
    session = CreateRTPSession(sc);
    // master zrtp session
    if(sc->media == 0)
    {
      session->SetMaster(TRUE);
      session->SetConnection(this);
    }
    session->CreateZRTP();
  }
  else if(sc->secure_type == SECURE_TYPE_DTLS_SRTP)
  {
    session = CreateRTPSession(sc);
    //session->CreateDTLS(dir, sc->dtls_fp_type, sc->dtls_fp);
  }
  else if(sc->secure_type == SECURE_TYPE_SRTP)
  {
    if(sc->srtp_local_key == "")
    {
      sc->srtp_local_type = sc->srtp_remote_type;
      if(sc->srtp_local_type == AES_CM_128_HMAC_SHA1_80)
      {
        if(sc->media == 0)
        {
          if(key_audio80 != "") sc->srtp_local_key = key_audio80;
          else                  sc->srtp_local_key = key_audio80 = srtp_get_random_keysalt();
        }
        else if(sc->media == 1)
        {
          if(key_video80 != "") sc->srtp_local_key = key_video80;
          else                  sc->srtp_local_key = key_video80 = srtp_get_random_keysalt();
        }
      }
      else if(sc->srtp_local_type == AES_CM_128_HMAC_SHA1_32)
      {
        if(sc->media == 0)
        {
          if(key_audio32 != "") sc->srtp_local_key = key_audio32;
          else                  sc->srtp_local_key = key_audio32 = srtp_get_random_keysalt();
        }
        else if(sc->media == 1)
        {
          if(key_video32 != "") sc->srtp_local_key = key_video32;
          else                  sc->srtp_local_key = key_video32 = srtp_get_random_keysalt();
        }
      }
    }
    session = CreateRTPSession(sc);
    if(dir == 0) session->CreateSRTP(dir, sc->srtp_remote_type, sc->srtp_remote_key);
    else         session->CreateSRTP(dir, sc->srtp_local_type, sc->srtp_local_key);
  }

  if(session == NULL)
    return 0;

  SipRTPChannel *channel =
    new SipRTPChannel(*this, *cap, (!dir)?H323Channel::IsReceiver:H323Channel::IsTransmitter, *session);

  if(pt >= RTP_DataFrame::DynamicBase && pt <= RTP_DataFrame::MaxPayloadType)
    channel->SetDynamicRTPPayloadType(pt);
  if(!dir) sc->inpChan = channel; else sc->outChan = channel;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::CreateLogicalChannels()
{
  CreateMediaChannel(scap, 0);
  CreateMediaChannel(scap, 1);
  CreateMediaChannel(vcap, 0);
  CreateMediaChannel(vcap, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::StartChannel(int pt, int dir)
{
  if(pt < 0) return;
  SipCapability *sc = FindSipCap(RemoteSipCaps, pt);
  if(!sc) return;
  if(dir == 0 && (sc->mode&2) && sc->inpChan && !sc->inpChan->IsRunning()) sc->inpChan->Start();
  if(dir == 1 && (sc->mode&1) && sc->outChan && !sc->outChan->IsRunning()) sc->outChan->Start();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::StartReceiveChannels()
{
  StartChannel(scap,0);
  StartChannel(vcap,0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::StartTransmitChannels()
{
  StartChannel(scap,1);
  StartChannel(vcap,1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::StopChannel(int pt, int dir)
{
  if(pt < 0) return;
  SipCapability *sc = FindSipCap(RemoteSipCaps, pt);
  if(!sc) return;
  if(dir==0 && sc->inpChan) sc->inpChan->CleanUpOnTermination();
  if(dir==1 && sc->outChan) sc->outChan->CleanUpOnTermination();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::StopTransmitChannels()
{
  StopChannel(scap,1);
  StopChannel(vcap,1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::StopReceiveChannels()
{
  StopChannel(scap,0);
  StopChannel(vcap,0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::DeleteMediaChannels(int pt)
{
  if(pt < 0) return;
  SipCapability *sc = FindSipCap(RemoteSipCaps, pt);
  if(!sc) return;
  if(sc->inpChan) { delete sc->inpChan; sc->inpChan = NULL; }
  if(sc->outChan) { delete sc->outChan; sc->outChan = NULL; }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::DeleteChannels()
{
  DeleteMediaChannels(scap);
  DeleteMediaChannels(vcap);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::CleanUpOnCallEnd()
{
  PTRACE(1, "MCUSIP\tCleanUpOnCallEnd");
  StopTransmitChannels();
  StopReceiveChannels();
  DeleteChannels();
  videoReceiveCodecName = videoTransmitCodecName = "none";
  videoReceiveCodec = NULL;
  videoTransmitCodec = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::ReceivedVFU()
{
  if(!CheckVFU())
    return;

  if(vcap < 0) return;
  SipCapability *sc = FindSipCap(RemoteSipCaps, vcap);
  if(sc && sc->outChan)
  {
    H323VideoCodec *vcodec = (H323VideoCodec*)sc->outChan->GetCodec();
    if(vcodec)
      vcodec->OnFastUpdatePicture();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SendLogicalChannelMiscCommand(H323Channel & channel, unsigned command)
{
  if(command == H245_MiscellaneousCommand_type::e_videoFastUpdatePicture)
  {
    PTime now;
    if(now < vfuLastTimeSend + PTimeInterval(1000))
      return;
    vfuLastTimeSend = now;

    PString *cmd = new PString("fast_update:"+callToken);
    sep->SipQueue.Push(cmd);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::ReceivedDTMF(PString sdp)
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

void MCUSipConnection::FindCapability_H263(SipCapability &sc, PStringArray &keys, const char * _H323Name, const char * _SIPName)
{
  PString H323Name(_H323Name);
  PString SIPName(_SIPName);
  for(int kn=0; kn<keys.GetSize(); kn++)
  {
    if(keys[kn].Find(SIPName + "=")==0)
    {
      sc.cap = H323Capability::Create(H323Name);
      if(sc.cap == NULL) return;
      OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat(); 
      int mpi = (keys[kn].Mid(SIPName.GetLength()+1)).AsInteger();
      wf.SetOptionInteger(SIPName + " MPI",mpi);
      return;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_H261(SipCapability & sc)
{
  if(pref_video_cap != "" && pref_video_cap.Left(5) == "H.261")
  {
    sc.cap = H323Capability::Create(pref_video_cap);
  }
  else if(pref_video_cap == "")
  {
    PStringArray keys = sc.fmtp.Tokenise(";");
    if(!sc.cap && FindSipCap(LocalSipCaps, "H.261-CIF{sw}"))
      FindCapability_H263(sc,keys,"H.261-CIF{sw}","CIF");
    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263-CIF{sw}"))
      FindCapability_H263(sc,keys,"H.261-QCIF{sw}","QCIF");
  }
  if(sc.cap)
  {
    OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
    if(sc.bandwidth) wf.SetOptionInteger("Max Bit Rate", sc.bandwidth*1000);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_H263(SipCapability & sc)
{
  int f=0; // annex f

  if(pref_video_cap != "" && pref_video_cap.Left(6) == "H.263-")
  {
    sc.cap = H323Capability::Create(pref_video_cap);
  }
  else if(pref_video_cap == "")
  {
    PStringArray keys = sc.fmtp.Tokenise(";");
    for(int kn=0; kn<keys.GetSize(); kn++)
    { if(keys[kn] == "F=1") { sc.fmtp = "F=1;"; f=1; break; } }

    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263-16CIF{sw}"))
      FindCapability_H263(sc,keys,"H.263-16CIF{sw}","CIF16");
    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263-4CIF{sw}"))
      FindCapability_H263(sc,keys,"H.263-4CIF{sw}","CIF4");
    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263-CIF{sw}"))
      FindCapability_H263(sc,keys,"H.263-CIF{sw}","CIF");
    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263-QCIF{sw}"))
      FindCapability_H263(sc,keys,"H.263-QCIF{sw}","QCIF");
    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263-SQCIF{sw}"))
      FindCapability_H263(sc,keys,"H.263-SQCIF{sw}","SQCIF");
  }
  if(sc.cap)
  {
    OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
    if(f) wf.SetOptionBoolean("_advancedPrediction", f);
    if(sc.bandwidth) wf.SetOptionInteger("Max Bit Rate", sc.bandwidth*1000);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_H263p(SipCapability & sc)
{
  int f=0,d=0,e=0,g=0; // annexes

  if(pref_video_cap != "" && pref_video_cap.Left(6) == "H.263p")
  {
    sc.cap = H323Capability::Create(pref_video_cap);
  }
  else if(pref_video_cap == "")
  {
    PStringArray keys = sc.fmtp.Tokenise(";");
    for(int kn=0; kn<keys.GetSize(); kn++)
    {
      if(keys[kn] == "F=1") { sc.fmtp += "F=1;"; f=1; }
      else if(keys[kn] == "D=1") { sc.fmtp += "D=1;"; d=1; }
      else if(keys[kn] == "E=1") { sc.fmtp += "E=1;"; e=1; }
      else if(keys[kn] == "G=1") { sc.fmtp += "G=1;"; g=1; }
    }

    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263p-16CIF{sw}"))
      FindCapability_H263(sc,keys,"H.263p-16CIF{sw}","CIF16");
    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263p-4CIF{sw}"))
      FindCapability_H263(sc,keys,"H.263p-4CIF{sw}","CIF4");
    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263p-CIF{sw}"))
      FindCapability_H263(sc,keys,"H.263p-CIF{sw}","CIF");
    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263p-QCIF{sw}"))
      FindCapability_H263(sc,keys,"H.263p-QCIF{sw}","QCIF");
    if(!sc.cap && FindSipCap(LocalSipCaps, "H.263p-SQCIF{sw}"))
      FindCapability_H263(sc,keys,"H.263p-SQCIF{sw}","SQCIF");
  }
  if(sc.cap)
  {
    OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
    if(f) wf.SetOptionBoolean("_advancedPrediction", f);
    if(d) wf.SetOptionBoolean("_unrestrictedVector", d);
    if(e) wf.SetOptionBoolean("_arithmeticCoding", e);
    if(g) wf.SetOptionBoolean("_pbFrames", g);
    if(sc.bandwidth) wf.SetOptionInteger("Max Bit Rate", sc.bandwidth*1000);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const struct h241_to_x264_level {
    int h241;
    int idc;
} h241_to_x264_levels[]=
{
    { 15, 9 },
    { 19,10 },
    { 22,11 },
    { 29,12 },
    { 36,13 },
    { 43,20 },
    { 50,21 },
    { 57,22 },
    { 64,30 },
    { 71,31 },
    { 78,32 },
    { 85,40 },
    { 92,41 },
    { 99,42 },
    { 106,50},
    { 113,51},
    { 0 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_H264(SipCapability & sc)
{
  int profile = 0, level = 0;
  int max_mbps = 0, max_fs = 0, max_br = 0;

  if(pref_video_cap != "" && pref_video_cap.Left(5) == "H.264")
  {
    sc.cap = H323Capability::Create(pref_video_cap);
  }
  else if(pref_video_cap == "")
  {
    PStringArray keys = sc.fmtp.Tokenise(";");
    for(int kn = 0; kn < keys.GetSize(); kn++)
    {
      if(keys[kn].Find("profile-level-id=") == 0) { int p = (keys[kn].Tokenise("=")[1]).AsInteger(16); profile = (p>>16); level = (p&255); }
      else if(keys[kn].Find("max-mbps=") == 0)    { max_mbps = (keys[kn].Tokenise("=")[1]).AsInteger(); }
      else if(keys[kn].Find("max-fs=") == 0)      { max_fs = (keys[kn].Tokenise("=")[1]).AsInteger(); }
      else if(keys[kn].Find("max-br=") == 0)      { max_br = (keys[kn].Tokenise("=")[1]).AsInteger(); }
    }
    // if(profile == 0 || level == 0) return;
    if(level == 0)
    {
      PTRACE(2,"SIP_CONNECTION\tH.264 level will set to " << OpenMCU::Current().h264DefaultLevelForSip);
      level = OpenMCU::Current().h264DefaultLevelForSip;
    }
    int l = 0;
    while(h241_to_x264_levels[l].idc != 0)
    {
      if(level == h241_to_x264_levels[l].idc) { level = h241_to_x264_levels[l].h241; break; }
      l++;
    }
    profile = 64;

    int cl = 0;
    for(SipCapMapType::iterator it = LocalSipCaps.begin(); it != LocalSipCaps.end(); it++)
    {
      if(it->second->capname.Find("H.264")==0)
      {
        H323Capability *cap = H323Capability::Create(it->second->capname);
        if(cap != NULL)
        {
          const OpalMediaFormat & mf = cap->GetMediaFormat();
          int flevel = mf.GetOptionInteger("Generic Parameter 42");
          if(flevel > cl && flevel <= level)
          { cl = flevel; if(sc.cap) delete sc.cap; sc.cap = cap; }
          else { delete cap; }
          if(flevel == level) break;
        }
      }
    }
  }
  if(sc.cap)
  {
    OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
    if(level) wf.SetOptionInteger("Generic Parameter 42", level);
    if(max_mbps) wf.SetOptionInteger("Generic Parameter 3", max_mbps);
    if(max_fs) wf.SetOptionInteger("Generic Parameter 4", max_fs);
    if(max_br) wf.SetOptionInteger("Generic Parameter 6", max_br);
    if(sc.bandwidth) wf.SetOptionInteger("Max Bit Rate", sc.bandwidth*1000);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_VP8(SipCapability & sc)
{
  int width = 0, height = 0;

  if(pref_video_cap != "" && pref_video_cap.Left(3) == "VP8")
  {
    sc.cap = H323Capability::Create(pref_video_cap);
  }
  else if(pref_video_cap == "")
  {
    PStringArray keys = sc.fmtp.Tokenise(";");
    for(int kn = 0; kn < keys.GetSize(); kn++)
    {
      if(keys[kn].Find("width=") == 0)
        width = (keys[kn].Tokenise("=")[1]).AsInteger();
      else if(keys[kn].Find("height=") == 0)
        height = (keys[kn].Tokenise("=")[1]).AsInteger();
    }
    if(!sc.cap && width && height)
    {
      for(SipCapMapType::iterator it = LocalSipCaps.begin(); it != LocalSipCaps.end(); it++)
      {
        if(it->second->capname.Left(3) == "VP8")
        {
          sc.cap = H323Capability::Create(it->second->capname);
          if(sc.cap)
          {
            const OpalMediaFormat & mf = sc.cap->GetMediaFormat();
            if(width == mf.GetOptionInteger("Frame Width") && height == mf.GetOptionInteger("Frame Height"))
              break;
            else
            { delete sc.cap; sc.cap = NULL; }
          }
        }
      }
    }
    if(!sc.cap)
    {
      sc.cap = H323Capability::Create("VP8-CIF{sw}");
    }
  }
  if(sc.cap)
  {
    OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
    if(width) wf.SetOptionInteger("Frame Width", width);
    if(height) wf.SetOptionInteger("Frame Height", height);
    if(remoteApplication.ToLower().Find("linphone") != P_MAX_INDEX) wf.SetOptionEnum("Picture ID Size", 0);
    if(sc.bandwidth) wf.SetOptionInteger("Max Bit Rate", sc.bandwidth*1000);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_SPEEX(SipCapability & sc)
{
  PString capname;
  if(sc.clock == 8000 && FindSipCap(LocalSipCaps, "Speex_8K{sw}"))        capname = "Speex_8K{sw}";
  else if(sc.clock == 16000 && FindSipCap(LocalSipCaps, "Speex_16K{sw}")) capname = "Speex_16K{sw}";
  else if(sc.clock == 32000 && FindSipCap(LocalSipCaps, "Speex_32K{sw}")) capname = "Speex_32K{sw}";
  else return;

  PString fmtp = sc.fmtp;
  // replace fmtp from codec parameters
  SipCapability *local_sc = FindSipCap(LocalSipCaps, capname);
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

  if(!sc.cap)
  {
    sc.cap = H323Capability::Create(capname);
  }
  if(sc.cap)
  {
    OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
    if (vbr > -1) wf.SetOptionEnum("vbr", vbr);
    if (mode > -1) wf.SetOptionInteger("mode", mode);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::SelectCapability_OPUS(SipCapability & sc)
{
  PString capname;
  if(sc.clock == 8000 && FindSipCap(LocalSipCaps, "OPUS_8K{sw}"))
    capname = "OPUS_8K{sw}";
  else if(sc.clock == 16000 && FindSipCap(LocalSipCaps, "OPUS_16K{sw}"))
    capname = "OPUS_16K{sw}";
  else if(sc.clock == 48000 && sc.params != "2" && FindSipCap(LocalSipCaps, "OPUS_48K{sw}"))
    capname = "OPUS_48K{sw}";
  else if(sc.clock == 48000 && sc.params == "2" && FindSipCap(LocalSipCaps, "OPUS_48K2{sw}"))
    capname = "OPUS_48K2{sw}";
  else
    return;

  PString fmtp = sc.fmtp;
  // replace fmtp from codec parameters
  SipCapability *local_sc = FindSipCap(LocalSipCaps, capname);
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

  if(!sc.cap)
  {
    sc.cap = H323Capability::Create(capname);
  }
  if(sc.cap)
  {
    OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
    if (cbr > -1) wf.SetOptionInteger("cbr", cbr);
    if (maxaveragebitrate > -1) wf.SetOptionInteger("maxaveragebitrate", maxaveragebitrate);
    if (useinbandfec > -1) wf.SetOptionInteger("useinbandfec", useinbandfec);
    if (usedtx > -1) wf.SetOptionInteger("usedtx", usedtx);
  }
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

int MCUSipConnection::ProcessSDP(PString & sdp_str, SipCapMapType & RemoteCaps)
{
  PTRACE(1, "MCUSIP\tProcessSDP");
  sdp_parser_t *parser = SdpParser(sdp_str);
  sdp_session_t *sess = sdp_session(parser);
  if(!sess)
  {
    PTRACE(1, "MCUSIP\tSDP parsing error: " << sdp_parsing_error(parser));
    return 0;
  }
  // -2 if c= line is missing. -1 if some SDP line is missing. (c=, o=, s=, t=)
  if(sdp_sanity_check(parser) != 0)
  {
    PTRACE(1, "MCUSIP\tSDP parsing error: sanity check");
    return 0;
  }

  RemoteCaps.clear();
  for(sdp_session_t *sdp = sess; sdp != NULL; sdp = sdp->sdp_next)
  {
    if(!sdp->sdp_origin)
    { PTRACE(1, "MCUSIP\tSDP parsing error: sdp origin line is missing"); continue; }

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
        address = m->m_connections->c_address;
      else if(sdp->sdp_connection && sdp->sdp_connection->c_addrtype == sdp_addr_ip4 && sdp->sdp_connection->c_address)
        address = sdp->sdp_connection->c_address;
      else
      { PTRACE(1, "MCUSIP\tSDP parsing error: incorrect or missing connection line, skip media"); continue; }

      int remote_port;
      if(m->m_port) remote_port = m->m_port;
      else
      { PTRACE(1, "MCUSIP\tSDP parsing error: missing port, skip media"); continue; }

      int media;
      if(m->m_type == sdp_media_audio)      media = 0;
      else if(m->m_type == sdp_media_video) media = 1;
      else
      { PTRACE(1, "MCUSIP\tSDP parsing error: unknown media type, skip media"); continue; }

      int mode = 3; // inactive, recvonly, sendonly, sendrecv
      if(m->m_mode == 1)      mode = 2;
      else if(m->m_mode == 2) mode = 1;
      else if(m->m_mode == 3) mode = 3;

      int bw = 0;
      if(m->m_bandwidths)          bw = m->m_bandwidths->b_value/1000;
      else if(sdp->sdp_bandwidths) bw = sdp->sdp_bandwidths->b_value;

      SipSecureTypes secure_type = SECURE_TYPE_NONE;
      PString srtp_type, srtp_key, srtp_param, zrtp_hash, dtls_fp, dtls_fp_type;
      if(rtp_proto == "ZRTP")
        secure_type = SECURE_TYPE_ZRTP;

      if(m_proto_name == "RTP/AVP" && secure_type == SECURE_TYPE_ZRTP)
      {
        sdp_attribute_t *a = sdp_attribute_find(m->m_attributes, "zrtp-hash");
        if(a && a->a_value)
          zrtp_hash = PString(a->a_value);
      }
      if(m_proto_name == "RTP/SAVP")
      {
        sdp_attribute_t *a = sdp_attribute_find(m->m_attributes, "crypto");
        if(!a || !a->a_value)
        {
          PTRACE(1, "MCUSIP\tSDP parsing warning: RTP/SAVP crypto attribute is not present, skip media");
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
          PTRACE(1, "MCUSIP\tSDP parsing warning: RTP/SAVPF fingerprint attribute is not present, skip media");
          continue;
        }
        secure_type = SECURE_TYPE_DTLS_SRTP;
        dtls_fp_type = a->a_name;
        dtls_fp = a->a_value;
      }

      for(sdp_rtpmap_t *rm = m->m_rtpmaps; rm != NULL; rm = rm->rm_next)
      {
        if(FindSipCap(RemoteCaps, rm->rm_pt)) continue;
        SipCapability *sc = new SipCapability();
        sc->payload = rm->rm_pt;
        sc->media = media;
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
        //
        RemoteCaps.insert(SipCapMapType::value_type(RemoteCaps.size(), sc));
      }
    }
  }
  sdp_parser_free(parser);

  RefreshLocalSipCaps();
  MergeSipCaps(LocalSipCaps, RemoteCaps);

  if(scap < 0 && vcap < 0)
  {
    PTRACE(1, "MCUSIP\tSDP parsing error: compatible codecs not found");
    return 0;
  }

  if(scap != -1)
  {
    SipCapability *sc = FindSipCap(RemoteCaps, scap);
    if(sc) sc->Print();
  }
  if(vcap != -1)
  {
    SipCapability *sc = FindSipCap(RemoteCaps, vcap);
    if(sc) sc->Print();
  }

  return 1;
}

BOOL MCUSipConnection::MergeSipCaps(SipCapMapType & LocalCaps, SipCapMapType & RemoteCaps)
{
  scap = -1; vcap = -1;
  for(SipCapMapType::iterator it = RemoteCaps.begin(); it != RemoteCaps.end(); it++)
  {
    SipCapability *remote_sc = it->second;
    if(remote_sc->media == 0)
    {
      if(scap >= 0) continue;
      // PCMU
      if((remote_sc->format == "pcmu" || remote_sc->payload == 0) && FindSipCap(LocalCaps, "G.711-uLaw-64k{sw}"))
      { remote_sc->capname = "G.711-uLaw-64k{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // PCMA
      else if((remote_sc->format == "pcma" || remote_sc->payload == 8) && FindSipCap(LocalCaps, "G.711-ALaw-64k{sw}"))
      { remote_sc->capname = "G.711-ALaw-64k{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // G.722
      else if(remote_sc->format == "g722" && FindSipCap(LocalCaps, "G.722-64k{sw}"))
      { remote_sc->capname = "G.722-64k{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // G.723.1
      else if(remote_sc->format == "g723" && FindSipCap(LocalCaps, "G.7231-6.3k[e]{sw}"))
      { remote_sc->capname = "G.7231-6.3k[e]{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // G.726-16
      else if(remote_sc->format == "g726-16" && FindSipCap(LocalCaps, "G.726-16k{sw}"))
      { remote_sc->capname = "G.726-16k{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // G.726-24
      else if(remote_sc->format == "g726-24" && FindSipCap(LocalCaps, "G.726-24k{sw}"))
      { remote_sc->capname = "G.726-24k{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // G.726-32
      else if(remote_sc->format == "g726-32" && FindSipCap(LocalCaps, "G.726-32k{sw}"))
      { remote_sc->capname = "G.726-32k{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // G.726-40
      else if(remote_sc->format == "g726-40" && FindSipCap(LocalCaps, "G.726-40k{sw}"))
      { remote_sc->capname = "G.726-40k{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // G.728
      else if(remote_sc->format == "g728" && FindSipCap(LocalCaps, "G.728-16k[e]"))
      { remote_sc->capname = "G.728-16k[e]"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // G.729A
      else if(remote_sc->format == "g729" && remote_sc->fmtp == "annexb=no;" && FindSipCap(LocalCaps, "G.729A-8k[e]{sw}"))
      { remote_sc->capname = "G.729A-8k[e]{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // iLBC-13k3
      else if(remote_sc->format == "ilbc" && remote_sc->fmtp == "mode=30;" && FindSipCap(LocalCaps, "iLBC-13k3{sw}"))
      { remote_sc->capname = "iLBC-13k3{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // iLBC-15k2
      else if(remote_sc->format == "ilbc" && remote_sc->fmtp == "mode=20;" && FindSipCap(LocalCaps, "iLBC-15k2{sw}"))
      { remote_sc->capname = "iLBC-15k2{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // SILK 16000
      else if(remote_sc->format == "silk" && remote_sc->clock == 16000 && FindSipCap(LocalCaps, "SILK_B40{sw}"))
      { remote_sc->capname = "SILK_B40{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // SILK 24000
      else if(remote_sc->format == "silk" && remote_sc->clock == 24000 && FindSipCap(LocalCaps, "SILK_B40_24K{sw}"))
      { remote_sc->capname = "SILK_B40_24K{sw}"; remote_sc->cap = H323Capability::Create(remote_sc->capname); }
      // SPEEX
      else if(remote_sc->format == "speex")
      { SelectCapability_SPEEX(*remote_sc); }
      // OPUS
      else if(remote_sc->format == "opus")
      { SelectCapability_OPUS(*remote_sc); }
    }
    else if(remote_sc->media == 1)
    {
      if(vcap >= 0) continue;
      if(remote_sc->format == "h261") SelectCapability_H261(*remote_sc);
      else if(remote_sc->format == "h263") SelectCapability_H263(*remote_sc);
      else if(remote_sc->format == "h263-1998") SelectCapability_H263p(*remote_sc);
      else if(remote_sc->format == "h264") SelectCapability_H264(*remote_sc);
      else if(remote_sc->format == "vp8") SelectCapability_VP8(*remote_sc);
    }
    if(remote_sc->cap)
    {
      if(remote_sc->media == 0)      scap = remote_sc->payload;
      else if(remote_sc->media == 1) vcap = remote_sc->payload;
      remote_sc->capname = remote_sc->cap->GetFormatName();
    }
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ProcessACK(const msg_t *msg)
{
  PTRACE(1, "MCUSIP\tProcessACK");
  sip_t *sip = sip_object(msg);
  // replace to_tag
  sip_t *c_sip = sip_object(c_sip_msg);
  msg_header_insert(c_sip_msg, (msg_pub_t *)c_sip, (msg_header_t *)sip->sip_to);

  // for incoming connection start channels after ACK
  StartReceiveChannels(); // start receive logical channels
  StartTransmitChannels(); // start transmit logical channels

  // is connected
  connectionState = EstablishedConnection;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ProcessInviteEvent(const msg_t *msg)
{
  PTRACE(1, "MCUSIP\tProcessInviteEvent");

  msg_destroy(c_sip_msg);
  c_sip_msg = msg_dup(msg);
  sip_t *sip = sip_object(c_sip_msg);

  // save cseq
  cseq_num = sip->sip_cseq->cs_seq;

  PString sdp_str = sip->sip_payload->pl_data;
  if(!ProcessSDP(sdp_str, RemoteSipCaps))
    return 415; // SIP_415_UNSUPPORTED_MEDIA

  if(direction == DIRECTION_INBOUND)
  {
    ProxyAccount *proxy = sep->FindProxyAccount((PString)sip->sip_to->a_url->url_user+"@"+(PString)sip->sip_from->a_url->url_host);
    if(proxy) requestedRoom = proxy->roomname;
  }

  MCUURL_SIP url(c_sip_msg, direction);
  remoteName = url.GetDisplayName();
  remotePartyName = remoteName;
  remotePartyAddress = url.GetUrl();
  remoteApplication = url.GetRemoteApplication();
  if(display_name != "")
    remoteName = remotePartyName = display_name;

  // set endpoint member name
  SetMemberName();
  // override requested room from registrar
  SetRequestedRoom();
  // join conference
  OnEstablished();
  if(!conference || !conferenceMember || (conferenceMember && !conferenceMember->IsJoined()))
    return 600;

  if(direction == DIRECTION_INBOUND)
  {
    // create logical channels
    CreateLogicalChannels();
    // create sdp for OK
    CreateSdpOk();
  }
  else if(direction == DIRECTION_OUTBOUND)
  {
    DeleteTempSockets();
    // create logical channels
    CreateLogicalChannels();
    // for incoming connection start channels after ACK
    StartReceiveChannels(); // start receive logical channels
    StartTransmitChannels(); // start transmit logical channels
    // is connected
    connectionState = EstablishedConnection;
    SendACK();
  }

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ProcessReInviteEvent(const msg_t *msg)
{
  PTRACE(1, "MCUSIP\tProcessReInviteEvent");

  // re-Invite always direction inbound
  direction = DIRECTION_INBOUND;
  // waiting ACK signal
  connectionState = AwaitingSignalConnect;

  msg_destroy(c_sip_msg);
  c_sip_msg = msg_dup(msg);
  sip_t *sip = sip_object(c_sip_msg);

  // update cseq
  cseq_num = sip->sip_cseq->cs_seq;

  PString sdp_str = sip->sip_payload->pl_data;
  SipCapMapType SipCaps;

  int cur_scap = scap;
  int cur_vcap = vcap;

  if(!ProcessSDP(sdp_str, SipCaps))
    return 415; // SIP_415_UNSUPPORTED_MEDIA

  int sflag = 1; // 0 - no changes
  if(scap >= 0 && cur_scap >= 0)
  {
    SipCapability *cur_sc = FindSipCap(RemoteSipCaps, cur_scap);
    SipCapability *new_sc = FindSipCap(SipCaps , scap);
    sflag = new_sc->CmpSipCaps(*cur_sc);
  }
  else if(scap < 0 && cur_scap < 0)
  {
    sflag = 0;
  }
  if(sflag && cur_scap >= 0)
  {
    StopChannel(cur_scap,1);
    StopChannel(cur_scap,0);
    DeleteMediaChannels(cur_scap);
  }

  int vflag = 1; // 0 - no changes
  if(vcap >= 0 && cur_vcap >= 0)
  {
    SipCapability *cur_sc = FindSipCap(RemoteSipCaps, cur_vcap);
    SipCapability *new_sc = FindSipCap(SipCaps , vcap);
    vflag = new_sc->CmpSipCaps(*cur_sc);
  }
  else if(vcap < 0 && cur_vcap < 0)
  {
    vflag = 0;
  }
  if(vflag && cur_vcap >= 0)
  {
    StopChannel(cur_vcap,1);
    StopChannel(cur_vcap,0);
    DeleteMediaChannels(cur_vcap);
  }

  if(!sflag && !vflag) // nothing changed
  {
    // sending old sdp
    return 1;
  }

  // update remote sip caps
  RemoteSipCaps = SipCaps;

  if(sflag && scap >= 0)
  {
    CreateMediaChannel(scap,0);
    CreateMediaChannel(scap,1);
  }
  if(vflag && vcap >= 0)
  {
    CreateMediaChannel(vcap,0);
    CreateMediaChannel(vcap,1);
  }

  // create sdp for OK
  if(direction == DIRECTION_INBOUND)
    CreateSdpOk();

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::ProcessInfo(const msg_t *msg)
{
  PTRACE(1, "MCUSIP\tOnReceivedInfo");
  sip_t *sip = sip_object(c_sip_msg);
  if(!sip->sip_payload || !sip->sip_payload->pl_data || !sip->sip_content_type)
    return 0;

  PString type = PString(sip->sip_content_type->c_type);
  PString data = PString(sip->sip_payload->pl_data);
  if(type.Find("application/media_control") != P_MAX_INDEX && data.Find("picture_fast_update") != P_MAX_INDEX)
    ReceivedVFU();
  else if (type.Find("application/dtmf-relay") != P_MAX_INDEX)
    ReceivedDTMF(data);
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::SendRequest(sip_method_t method, const char *method_name, msg_t *req_msg=NULL)
{
  PTRACE(1, "MCUSIP\tSendRequest");
  sip_t *sip = sip_object(c_sip_msg);
  su_home_t *home = msg_home(c_sip_msg);

  // Send request
  sip_addr_t *sip_from, *sip_to;
  if(direction == DIRECTION_INBOUND)
  {
    sip_from = sip->sip_to;
    sip_to = sip->sip_from;
  } else {
    sip_from = sip->sip_from;
    sip_to = sip->sip_to;
  }

  int _cseq_num;
  if(method == sip_method_ack) _cseq_num = sip->sip_cseq->cs_seq;
  else _cseq_num = ++cseq_num;

  url_string_t *ruri = (url_string_t *)(const char *)ruri_str;

  sip_request_t *sip_rq = sip_request_create(home, method, method_name, ruri, NULL);
  sip_cseq_t *sip_cseq = sip_cseq_create(home, _cseq_num, method, method_name);
  sip_route_t* sip_route = sip_route_reverse(home, sip->sip_record_route);

  if(req_msg == NULL)
    req_msg = nta_msg_create(sep->GetAgent(), 0);
  nta_outgoing_t *a_orq = nta_outgoing_mcreate(sep->GetAgent(), NULL, NULL,
			ruri,
			req_msg,
			NTATAG_STATELESS(1),
 			SIPTAG_REQUEST(sip_rq),
			SIPTAG_ROUTE(sip_route),
                        SIPTAG_CONTACT_STR(contact_str),
			SIPTAG_FROM(sip_from),
			SIPTAG_TO(sip_to),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip->sip_call_id),
			SIPTAG_MAX_FORWARDS_STR(SIP_MAX_FORWARDS),
			SIPTAG_SERVER_STR(SIP_USER_AGENT),
			TAG_END());
  if(a_orq == NULL)
    return 0;
  nta_outgoing_destroy(a_orq);
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::SendBYE()
{
  PTRACE(1, "MCUSIP\tSendBYE");
  return SendRequest(SIP_METHOD_BYE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::SendVFU()
{
  PTRACE(1, "MCUSIP\tSendFastUpdatePicture");
  msg_t *msg_req = nta_msg_create(sep->GetAgent(), 0);
  sip_payload_t *sip_payload = sip_payload_format(msg_home(msg_req),
      "<media_control><vc_primitive><to_encoder><picture_fast_update/></to_encoder></vc_primitive></media_control>");
  sip_add_tl(msg_req, NULL,
                        SIPTAG_CONTENT_TYPE_STR("application/media_control+xml"),
                        SIPTAG_PAYLOAD(sip_payload),
                        TAG_END());
  return SendRequest(SIP_METHOD_INFO, msg_req);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnection::SendACK()
{
  PTRACE(1, "MCUSIP\tSendACK");
  return SendRequest(SIP_METHOD_ACK);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::LeaveMCU()
{
  PString *bye = new PString("bye:"+callToken);
  sep->SipQueue.Push(bye);
  LeaveMCU(FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnection::LeaveMCU(BOOL remove)
{
  PTRACE(1, "MCUSIP\tLeave " << callToken << " remove=" << remove);
  if(remove == FALSE) return;
  MCUH323Connection::LeaveMCU();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString MCUSipEndPoint::GetRoomAccess(const sip_t *sip)
{
    PTRACE(1, "MCUSIP\tGetRoomAccess");
    BOOL inRoom = FALSE;
    PString via = sip->sip_via->v_host;
    PString userName = sip->sip_from->a_url->url_user;
    PString hostName = sip->sip_from->a_url->url_host;
    PString roomName;
    PString userName_, hostName_, via_, access;
    PString defaultAccess = MCUConfig("RoomAccess").GetString("*", "ALLOW").Tokenise(",")[0].ToUpper();

    ProxyAccount *proxy = FindProxyAccount((PString)sip->sip_to->a_url->url_user+"@"+(PString)sip->sip_from->a_url->url_host);
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

    PTRACE(1, "MCUSIP\t"<< access << " access to room \"" << roomName << "\", from=" << userName+"@"+hostName << ", via=" << via);
    return access;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

nta_outgoing_t * MCUSipEndPoint::SipMakeCall(PString from, PString to, PString & call_id)
{
    PTRACE(1, "MCUSIP\tSipMakeCall from:" << from << " to:" << to);

    MCUURL url_to(to);
    if(url_to.GetHostName() == "") return NULL;
    PString remote_user = url_to.GetUserName();
    PString remote_domain = url_to.GetHostName();
    PString remote_host = remote_domain;
    PString remote_port = url_to.GetPort();
    PString remote_transport = url_to.GetSipProto();

    if(from.Left(4) != "sip:") from = "sip:"+from;
    if(from.Find("@") == P_MAX_INDEX) from = from+"@";
    MCUURL url_from(from);
    PString local_user = url_from.GetUserName();
    PString local_domain = url_from.GetHostName();
    PString local_dname = url_from.GetDisplayName();
    PString local_ip;

    for(ProxyAccountMapType::iterator it=ProxyAccountMap.begin(); it!=ProxyAccountMap.end(); it++)
    {
      ProxyAccount *proxy = it->second;
      if((proxy->domain == remote_domain || proxy->host == remote_host) && proxy->roomname == local_user && proxy->enable == 1)
      {
        local_user = proxy->username;
        local_domain = proxy->domain;
        local_dname = proxy->roomname;
        remote_host = proxy->host;
        remote_port = proxy->port;
        break;
      }
    }

    remote_port = GetEndpointParamFromUrl("Port", "sip:"+remote_user+"@"+remote_domain, remote_port);
    remote_transport = GetEndpointParamFromUrl("Transport", "sip:"+remote_user+"@"+remote_domain, remote_transport);
    if(remote_transport == "transport=tls" && remote_port == "5060")
      remote_port = "5061";

    local_ip = GetFromIp(remote_host, remote_port);
    if(local_ip == "" || FindListener(local_ip) == FALSE)
      return NULL;

    if(local_domain == "")
      local_domain = local_ip;
    if(local_dname == "")
      local_dname = local_user;

    // ruri
    PString ruri_str = "sip:"+remote_user+"@"+remote_host+":"+remote_port+";transport="+remote_transport;
    url_string_t *ruri = (url_string_t *)(const char *)ruri_str;

    sip_addr_t *sip_from = sip_from_create(&home, (url_string_t *)(const char *)("sip:"+local_user+"@"+local_domain));
    sip_from_tag(&home, sip_from, nta_agent_newtag(&home, "tag=%s", agent));

    sip_addr_t *sip_to = sip_to_create(&home, (url_string_t *)(const char *)("sip:"+remote_user+"@"+remote_domain));

    PString contact_str = "sip:"+local_user+"@"+local_ip;
    sip_contact_t *sip_contact = sip_contact_create(&home, (url_string_t *)(const char *)contact_str, NULL);

    sip_request_t *sip_rq = sip_request_create(&home, SIP_METHOD_INVITE, ruri, NULL);
    sip_cseq_t *sip_cseq = sip_cseq_create(&home, 100, SIP_METHOD_INVITE);

    sip_call_id_t* sip_call_id = NULL;
    if(call_id != "")
    {
      sip_call_id = sip_call_id_make(&home, call_id);
    } else {
      sip_call_id = sip_call_id_create(&home, "1");
      call_id = sip_call_id->i_id;
    }

    msg_t *msg = nta_msg_create(agent, 0);
    sip_t *sip = sip_object(msg);
    sip_add_tl(msg, sip,
		SIPTAG_REQUEST(sip_rq),
		SIPTAG_FROM(sip_from),
		SIPTAG_TO(sip_to),
		SIPTAG_CSEQ(sip_cseq),
		SIPTAG_CALL_ID(sip_call_id),
		SIPTAG_CONTACT(sip_contact),
                TAG_END());

    // create connection
    PString callToken = "sip:"+PString(sip_to->a_url->url_user)+":"+PString(sip_call_id->i_id);
    MCUSipConnection *sCon = FindConnectionWithoutLock(callToken);
    if(!sCon)
    {
      sCon = new MCUSipConnection(this, ep, DIRECTION_OUTBOUND, callToken, msg);
    }

    sip_payload_t *sip_payload = sip_payload_make(&home, (const char *)sCon->sdp_invite_str);

    nta_outgoing_t *orq = nta_outgoing_mcreate(agent, nta_response_cb1_wrap, (nta_outgoing_magic_t *)this,
			ruri,
			msg,
			SIPTAG_CONTENT_TYPE_STR("application/sdp"),
			SIPTAG_PAYLOAD(sip_payload),
			SIPTAG_MAX_FORWARDS_STR(SIP_MAX_FORWARDS),
			SIPTAG_SERVER_STR(SIP_USER_AGENT),
			TAG_END());
    if(orq == NULL)
      sCon->LeaveMCU(TRUE);

    return orq;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::SipRegister(ProxyAccount *proxy, BOOL enable)
{
    PTRACE(1, "MCUSIP\tSipRegister");

    PString local_ip = GetFromIp(proxy->host, proxy->port);
    if(local_ip == "" || FindListener(local_ip) == FALSE)
      return 0;

    // ruri
    PString ruri_str = "sip:"+proxy->username+"@"+proxy->host+":"+proxy->port;
    url_string_t *ruri = (url_string_t *)(const char *)ruri_str;

    sip_addr_t *sip_from = sip_from_create(&home, (url_string_t *)(const char *)
	("sip:"+proxy->username+"@"+proxy->domain));
    sip_from_tag(&home, sip_from, nta_agent_newtag(&home, "tag=%s", agent));
    sip_from->a_display = proxy->roomname;

    sip_addr_t *sip_to = sip_to_create(&home, (url_string_t *)(const char *)
	("sip:"+proxy->username+"@"+proxy->domain));

    sip_contact_t *sip_contact = sip_contact_create(&home, (url_string_t *)(const char *)
	("sip:"+proxy->username+"@"+local_ip), NULL);
    sip_contact->m_display = proxy->roomname;

    sip_request_t *sip_rq = sip_request_create(&home, SIP_METHOD_REGISTER, ruri, NULL);
    sip_cseq_t *sip_cseq = sip_cseq_create(&home, proxy->cseq++, SIP_METHOD_REGISTER);

    sip_call_id_t* sip_call_id = NULL;
    if(proxy->call_id == "" || !enable) // If the same Call-ID is used between register and unregister, asterisk keeps returning 401 with a new challenge
      sip_call_id = sip_call_id_create(&home, "0");
    else
      sip_call_id = sip_call_id_make(&home, proxy->call_id);

    PString expires = "0";
    if(enable)
      expires = proxy->expires;

    msg_t *sip_msg = nta_msg_create(agent, 0);
    nta_outgoing_t *a_orq = nta_outgoing_mcreate(agent, nta_response_cb1_wrap, (nta_outgoing_magic_t *)this,
      			ruri,
			sip_msg,
			SIPTAG_REQUEST(sip_rq),
			SIPTAG_FROM(sip_from),
			SIPTAG_TO(sip_to),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip_call_id),
			SIPTAG_CONTACT(sip_contact),
			SIPTAG_EXPIRES_STR((const char*)expires),
			SIPTAG_MAX_FORWARDS_STR(SIP_MAX_FORWARDS),
			SIPTAG_SERVER_STR(SIP_USER_AGENT),
			TAG_END());
    if(a_orq == NULL)
      return 0;
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipEndPoint::MakeProxyAuth(ProxyAccount *proxy, const sip_t *sip)
{
  PTRACE(1, "MCUSIP\tMakeProxyAuth");

  const char *realm = NULL;
  const char *nonce = NULL;
  const char *scheme = NULL;
  if(sip->sip_www_authenticate && sip->sip_www_authenticate->au_scheme && sip->sip_www_authenticate->au_params)
  {
    realm = msg_params_find(sip->sip_www_authenticate->au_params, "realm=");
    nonce = msg_params_find(sip->sip_www_authenticate->au_params, "nonce=");
    scheme = sip->sip_www_authenticate->au_scheme;
  }
  else if(sip->sip_proxy_authenticate && sip->sip_proxy_authenticate->au_scheme && sip->sip_proxy_authenticate->au_params)
  {
    realm = msg_params_find(sip->sip_proxy_authenticate->au_params, "realm=");
    nonce = msg_params_find(sip->sip_proxy_authenticate->au_params, "nonce=");
    scheme = sip->sip_proxy_authenticate->au_scheme;
  }
  else
    return FALSE;

  if(scheme == NULL || realm == NULL || nonce == NULL)
    return FALSE;

  PString uri = "sip:"+PString(realm); uri.Replace("\"","",true,0);
  PString sip_auth_str = MakeAuthStr(proxy->username, proxy->password, uri, sip->sip_cseq->cs_method_name, scheme, realm, nonce);

  if(sip->sip_www_authenticate)
    proxy->sip_www_str = sip_auth_str;
  else if(sip->sip_proxy_authenticate)
    proxy->sip_proxy_str = sip_auth_str;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString MCUSipEndPoint::MakeAuthStr(PString username, PString password, PString uri, const char *method, const char *scheme, const char *realm, const char *nonce)
{
    PTRACE(1, "MCUSIP\tMakeAuthStr");
    const char *sip_auth_str=NULL;

    auth_response_t ar[1] = {{ sizeof(ar) }};
    auth_hexmd5_t ha1, hresponse;
    ar->ar_username = username;
    ar->ar_realm = realm;
    ar->ar_nonce = nonce;
    ar->ar_uri = uri;
    //auth_digest_ha1(ha1, username, realm, password);
    auth_digest_a1(ar, ha1, password);
    auth_digest_response(ar, hresponse, ha1, method, NULL, 0);
    sip_auth_str = su_sprintf(&home, "%s %s\"%s\", %s%s, %s%s, %s\"%s\", %s\"%s\", %s",
			    scheme,
			    "username=", ar->ar_username,
			    "realm=", realm,
			    "nonce=", nonce,
			    "response=", hresponse,
			    "uri=", ar->ar_uri,
			    "algorithm=MD5");
    return sip_auth_str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::nta_response_cb1(nta_outgoing_t *orq, const sip_t *sip)
{
  msg_t *msg = nta_outgoing_getresponse(orq);
  msg_t *msg_orq = nta_outgoing_getrequest(orq);
  sip_t *sip_orq = sip_object(msg_orq);
  if(!msg || !msg_orq) return 0;

  int status = 0, request = 0;
  if(sip->sip_status)  status = sip->sip_status->st_status;
  if(sip->sip_cseq)    request = sip->sip_cseq->cs_method;
  PString callToken = "sip:"+PString(sip->sip_to->a_url->url_user)+":"+PString(sip->sip_call_id->i_id);

  if(request == sip_method_register)
  {
    ProxyAccount *proxy = FindProxyAccount((PString)sip->sip_from->a_url->url_user+"@"+(PString)sip->sip_from->a_url->url_host);
    if(!proxy)
    {
      nta_outgoing_destroy(orq);
      return 0;
    }

    proxy->SetStatus(sip);

    if(status != 401 && status != 407)
    {
      nta_outgoing_destroy(orq);
      return 0;
    }
  }
  if(request == sip_method_invite)
  {
    if(status == 200)
    {
      MCUSipConnection *sCon = FindConnectionWithoutLock(callToken);
      if(sCon)
        CreateOutgoingConnection(msg);
      nta_outgoing_destroy(orq);
      return 0;
    }
    if(status > 299 && status != 401 && status != 407)
    {
      MCUSipConnection *sCon = FindConnectionWithoutLock(callToken);
      if(sCon)
        sCon->LeaveMCU(TRUE);
      nta_outgoing_destroy(orq);
      return 0;
    }
  }

  if(status == 401 || status == 407)
  {
    // check authorization attempts
    if(sip_orq->sip_authorization || sip_orq->sip_proxy_authorization)
    {
      nta_outgoing_destroy(orq);
      return 0;
    }

    ProxyAccount *proxy = FindProxyAccount((PString)sip->sip_from->a_url->url_user+"@"+(PString)sip->sip_from->a_url->url_host);
    if(!proxy)
    {
      nta_outgoing_destroy(orq);
      return 0;
    }

    // remove via header
    msg_header_remove(msg_orq, (msg_pub_t *)sip_orq, (msg_header_t *)sip_orq->sip_via);

    // cseq increment
    int cseq = sip_orq->sip_cseq->cs_seq+1;
    if(request == sip_method_register)
      cseq = proxy->cseq++;
    sip_cseq_t *sip_cseq = sip_cseq_create(&home, cseq, sip_orq->sip_cseq->cs_method, sip_orq->sip_cseq->cs_method_name);
    msg_header_replace(msg_orq, (msg_pub_t *)sip_orq, (msg_header_t *)sip_orq->sip_cseq, (msg_header_t *)sip_cseq);

    // call id
    if(request == sip_method_register)
      proxy->call_id = sip_orq->sip_call_id->i_id;

    // authorization
    MakeProxyAuth(proxy, sip);
    if(status == 401)
    {
      sip_authorization_t *sip_auth = sip_authorization_make(&home, proxy->sip_www_str);
      msg_header_insert(msg_orq, (msg_pub_t *)sip_orq, (msg_header_t *)sip_auth);
    }
    else if(status == 407)
    {
      sip_authorization_t *sip_proxy_auth = sip_proxy_authorization_make(&home, proxy->sip_proxy_str);
      msg_header_insert(msg_orq, (msg_pub_t *)sip_orq, (msg_header_t *)sip_proxy_auth);
    }

    nta_outgoing_mcreate(agent, nta_response_cb1_wrap, (nta_outgoing_magic_t *)this,
 			 (url_string_t *)sip_orq->sip_to->a_url,
			 msg_orq,
          		 TAG_END());
    nta_outgoing_destroy(orq);
    return 0;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::SipReqReply(const msg_t *msg, unsigned status, const char *auth_str, const char *contact_str, const char *content_str, const char *payload_str)
{
  PTRACE(1, "MCUSipEndPoint\tSipReqReply");
  sip_t *sip = sip_object(msg);

  const char *status_phrase = sip_status_phrase(status);
  if(status_phrase == NULL) return 0;

  sip_authorization_t *sip_www_auth = NULL;
  sip_authorization_t *sip_proxy_auth = NULL;
  if(auth_str)
  {
    if(status == 401)      sip_www_auth = sip_authorization_make(&home, auth_str);
    else if(status == 407) sip_proxy_auth = sip_authorization_make(&home, auth_str);
  }

  const char *event_str = NULL;
  const char *allow_events_str = NULL;
  const char *allow_str = NULL;
  sip_expires_t *sip_expires = NULL;
  if(status == 200 && sip->sip_request->rq_method == sip_method_register)
  {
    event_str = "registration";
    allow_events_str = "presence";
    allow_str = SIP_ALLOW_METHODS_REGISTER;
    sip_expires = sip->sip_expires;
  }
  if(status == 200 && sip->sip_request->rq_method == sip_method_options)
  {
    allow_str = SIP_ALLOW_METHODS_OPTIONS;
  }
  if(status == 200 && sip->sip_request->rq_method == sip_method_subscribe)
  {
    sip_expires = sip->sip_expires;
  }

  sip_contact_t *sip_contact = NULL;
  if(contact_str)
    sip_contact = sip_contact_make(&home, contact_str);

  sip_content_type_t *sip_content = NULL;
  if(content_str)
    sip_content = sip_content_type_make(&home, content_str);

  sip_payload_t *sip_payload = NULL;
  if(payload_str)
    sip_payload = sip_payload_make(&home, payload_str);

  msg_t *msg_reply = msg_dup(msg);
  nta_msg_treply(agent, msg_reply, status, status_phrase,
                   SIPTAG_CONTACT(sip_contact),
                   SIPTAG_CONTENT_TYPE(sip_content),
                   SIPTAG_PAYLOAD(sip_payload),
  		   SIPTAG_EVENT_STR(event_str),
  		   SIPTAG_ALLOW_EVENTS_STR(allow_events_str),
  		   SIPTAG_ALLOW_STR(allow_str),
		   SIPTAG_EXPIRES(sip_expires),
  		   SIPTAG_WWW_AUTHENTICATE(sip_www_auth),
		   SIPTAG_PROXY_AUTHENTICATE(sip_proxy_auth),
                   SIPTAG_SERVER_STR(SIP_USER_AGENT),
                   TAG_END());
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::ProcessSipEvent_request1(nta_leg_t *leg, nta_incoming_t *irq, const sip_t *sip)
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::CreateOutgoingConnection(const msg_t *msg)
{
  PTRACE(1, "MCUSIP\tCreateOutgoingConnection");
  sip_t *sip = sip_object(msg);

  if(sip->sip_payload==NULL || sip->sip_payload->pl_data==NULL)
    return 0;

  PString callToken = "sip:"+PString(sip->sip_to->a_url->url_user)+":"+PString(sip->sip_call_id->i_id);

  MCUSipConnection *sCon = FindConnectionWithoutLock(callToken);
  if(!sCon || sCon->IsConnected()) // repeated OK
    return 0;

  int ret = sCon->ProcessInviteEvent(msg);
  if(ret != 1)
  {
    sCon->SendBYE();
    sCon->LeaveMCU(TRUE); // leave conference and delete connection
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::CreateIncomingConnection(const msg_t *msg)
{
  PTRACE(1, "MCUSIP\tCreateIncomingConnection");
  sip_t *sip = sip_object(msg);

  if(GetRoomAccess(sip) == "DENY")
    return SipReqReply(msg, 403); // SIP_403_FORBIDDEN

  PString callToken = "sip:"+PString(sip->sip_from->a_url->url_user)+":"+PString(sip->sip_call_id->i_id);

  MCUSipConnection *sCon = FindConnectionWithoutLock(callToken);
  if(!sCon)
    return 0;

  if(sCon->IsConnected())  // connection already exist, process reinvite
  {
    PTRACE(1, "MCUSIP\tSIP REINVITE");
    int ret = sCon->ProcessReInviteEvent(msg);
    if(ret != 1)
      return SipReqReply(msg, ret);
    SipReqReply(msg, 200, NULL, sCon->contact_str, "application/sdp", sCon->sdp_ok_str);
    return 0;
  }

  PTRACE(1, "MCUSIP\tSIP INVITE");

  int ret = sCon->ProcessInviteEvent(msg);
  if(ret != 1)
  {
    SipReqReply(msg, ret);
    sCon->LeaveMCU(TRUE); // leave conference and delete connection
    return 0;
  }
  SipReqReply(msg, 200, NULL, sCon->contact_str, "application/sdp", sCon->sdp_ok_str);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::ProcessSipEvent_cb(nta_agent_t *agent, msg_t *msg, sip_t *sip)
{
  if(terminating == 1)
    return 0;

  Registrar *registrar = OpenMCU::Current().GetRegistrar();
  int request = 0, status = 0, cseq = 0;
  if(sip->sip_request) request = sip->sip_request->rq_method;
  if(sip->sip_status)  status = sip->sip_status->st_status;
  if(sip->sip_cseq)    cseq = sip->sip_cseq->cs_method;

  // wrong RequestURI
  if(sip->sip_request && !FindListener(sip->sip_request->rq_url->url_host))
  {
    return SipReqReply(msg, 400); // SIP_400_BAD_REQUEST
  }
  // empty payload header for invite
  if(request == sip_method_invite && (!sip->sip_payload || !sip->sip_payload->pl_data))
  {
    return SipReqReply(msg, 415); // SIP_415_UNSUPPORTED_MEDIA
  }

  PString callToken;
  if(request) callToken = "sip:"+PString(sip->sip_from->a_url->url_user)+":"+PString(sip->sip_call_id->i_id);
  else        callToken = "sip:"+PString(sip->sip_to->a_url->url_user)+":"+PString(sip->sip_call_id->i_id);
  MCUSipConnection *sCon = FindConnectionWithoutLock(callToken);

  // repeated INVITE, waiting ACK signal
  if(request == sip_method_invite && sCon && sCon->IsAwaitingSignalConnect())
  {
    return 0;
  }
  // repeated OK, must be processed in the invite callback
  if(cseq == sip_method_invite && status == 200)
  {
    if(sCon) sCon->SendACK();
    return 0;
  }

  if(request == sip_method_register)
  {
    return registrar->OnReceivedSipRegister(msg);
  }
  if(request == sip_method_invite)
  {
    if(!sCon || !sCon->IsConnected())
    {
      return registrar->OnReceivedSipInvite(msg);
    }
    return CreateIncomingConnection(msg);
  }
  if(request == sip_method_ack)
  {
    if(!sCon)
      return SipReqReply(msg, 200); // SIP_481_NO_TRANSACTION
    sCon->ProcessACK(msg);
    return 0;
  }
  if(request == sip_method_bye)
  {
    if(!sCon)
      return SipReqReply(msg, 200);
    sCon->LeaveMCU(TRUE); // leave conference and delete connection
    return SipReqReply(msg, 200);
  }
  if(request == sip_method_cancel)
  {
    if(!sCon)
      return SipReqReply(msg, 200);
    sCon->LeaveMCU(TRUE); // leave conference and delete connection
    return SipReqReply(msg, 200);
  }
  if(request == sip_method_info)
  {
    if(!sCon)
      return SipReqReply(msg, 200);
    sCon->ProcessInfo(msg);
    return SipReqReply(msg, 200);
  }
  if(request == sip_method_message)
  {
    return registrar->OnReceivedSipMessage(msg);
  }
  if(request == sip_method_subscribe)
  {
    return registrar->OnReceivedSipSubscribe(msg);
  }
  //if(request == sip_method_notify)
  //{
  //  return SipReqReply(msg, 481); // SIP_481_NO_TRANSACTION
  //}
  if(request == sip_method_options)
  {
    return SipReqReply(msg, 200);
  }
  if(request == sip_method_publish)
  {
    return SipReqReply(msg, 200);
  }
  if(request != 0)
  {
    return SipReqReply(msg, 501); // SIP_501_NOT_IMPLEMENTED
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::ProcessProxyAccount()
{
  ProxyAccountMapType::iterator it;
  for(it=ProxyAccountMap.begin(); it!=ProxyAccountMap.end(); it++)
  {
    ProxyAccount *proxy = it->second;
    PTime now;
    if(proxy->enable && now > proxy->start_time + PTimeInterval(proxy->expires*1000))
    {
      proxy->status = 0;
      proxy->start_time = now;
      SipRegister(proxy);
    }
    if(!proxy->enable && proxy->status == 200)
    {
      proxy->status = 0;
      proxy->start_time = PTime(0);
      SipRegister(proxy, FALSE);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::InsertProxyAccount(ProxyAccount *proxy)
{
  ProxyAccountMap.insert(ProxyAccountMapType::value_type(proxy->username+"@"+proxy->domain, proxy));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ProxyAccount * MCUSipEndPoint::FindProxyAccount(PString account)
{
  ProxyAccountMapType::iterator it = ProxyAccountMap.find(account);
  if(it != ProxyAccountMap.end())
    return it->second;
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::InitProxyAccounts()
{
  PString sectionPrefix = "SIP Proxy Account ";
  PStringList sect = MCUConfig("Parameters").GetSectionsPrefix(sectionPrefix);
  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    MCUConfig scfg(sect[i]);
    PString name = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());
    PString host = MCUURL("sip:@"+scfg.GetString("Host")).GetHostName();
    PString port = MCUURL("sip:@"+scfg.GetString("Host")).GetPort();
    PString transport = MCUURL("sip:@"+scfg.GetString("Host")).GetSipProto();
    PString username = MCUURL("sip:"+scfg.GetString("Account")).GetUserName();
    PString domain = MCUURL("sip:"+scfg.GetString("Account")).GetHostName();
    PString password = PHTTPPasswordField::Decrypt(scfg.GetString("Password"));
    unsigned expires = scfg.GetInteger("Expires");
    BOOL enable = scfg.GetBoolean("Register");

    ProxyAccount *proxy = FindProxyAccount(username+"@"+domain);
    if(!proxy)
    {
      proxy = new ProxyAccount();
      proxy->username = username;
      proxy->domain = domain;
      InsertProxyAccount(proxy);
    }
    if(proxy->enable)
    {
      proxy->enable = enable;
      continue;
    }
    proxy->roomname = name;
    proxy->username = username;
    proxy->domain = domain;
    proxy->host = host;
    proxy->port = port;
    proxy->transport = transport;
    proxy->password = password;
    proxy->expires = expires;
    proxy->enable = enable;
    //
    proxy->start_time = PTime(0);
    proxy->sip_www_str = "";
    proxy->sip_proxy_str = "";
    proxy->call_id = "";
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::CreateBaseSipCaps()
{
  int dyn_pt = 96;
  PStringArray tmp_caps;
  PStringList keys;

  BaseSipCaps.clear();

  keys = MCUConfig("SIP Audio").GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    if(dyn_pt >= 127) continue;
    if(keys[i].Right(4) == "fmtp") continue;
    if(MCUConfig("SIP Audio").GetBoolean(keys[i]) == FALSE) continue;

    PString capname = keys[i];
    if(capname.Right(4) != "{sw}") capname += "{sw}";
    PString fmtp = MCUConfig("SIP Audio").GetString(capname+"_fmtp");
    PString local_fmtp = MCUConfig("SIP Audio").GetString(capname+"_local_fmtp");

    SipCapability *sc = new SipCapability(capname);
    if(sc->format == "") { delete sc; sc = NULL; continue; }

    // duplication check
    if(sc->payload == 128)
    {
      if(tmp_caps.GetStringsIndex(sc->format+PString(sc->clock)+sc->fmtp) == P_MAX_INDEX)
        sc->payload = dyn_pt++;
      else
        sc->payload = dyn_pt;
    }
    tmp_caps.AppendString(sc->format+PString(sc->clock)+sc->fmtp);

    sc->media = 0;
    if(fmtp != "") sc->fmtp = fmtp;
    if(local_fmtp != "") sc->local_fmtp = local_fmtp;
    if(capname == "OPUS_48K2") sc->params = "2";
    BaseSipCaps.insert(SipCapMapType::value_type(BaseSipCaps.size(), sc));
  }

  keys = MCUConfig("SIP Video").GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    if(dyn_pt >= 127) continue;
    if(keys[i].Right(4) == "fmtp") continue;
    if(MCUConfig("SIP Video").GetBoolean(keys[i]) == FALSE) continue;

    PString capname = keys[i];
    if(capname.Right(4) != "{sw}") capname += "{sw}";
    PString fmtp = MCUConfig("SIP Video").GetString(capname+"_fmtp");

    SipCapability *sc = new SipCapability(capname);
    if(sc->format == "") { delete sc; sc = NULL; continue; }

    // duplication check
    if(sc->payload == 128)
    {
      if(tmp_caps.GetStringsIndex(sc->format+PString(sc->clock)+sc->fmtp) == P_MAX_INDEX)
        sc->payload = dyn_pt++;
      else
        sc->payload = dyn_pt;
    }
    tmp_caps.AppendString(sc->format+PString(sc->clock)+sc->fmtp);

    sc->media = 1;
    if(fmtp != "") sc->fmtp = fmtp;
    BaseSipCaps.insert(SipCapMapType::value_type(BaseSipCaps.size(), sc));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::Initialise()
{
  nta_agent_close_tports(agent);
  for(PINDEX i = 0; i < sipListenerArray.GetSize(); i++)
  {
    PString listener = sipListenerArray[i];
    MCUURL url(listener);
    PString transport = url.GetSipProto();
    if(transport != "tls")
      nta_agent_add_tport(agent, URL_STRING_MAKE((const char*)listener), TAG_NULL());
/*
    if(transport == "*" || transport == "tls")
    {
      PString certificatePath = PString(SYS_SSL_DIR);
      unsigned port = atoi(url.GetPort());
      if(port == 5060) port++;
      PString tlsListener = "sips:"+url.GetHostName()+":"+PString(port)+";transport=tls";
      nta_agent_add_tport(agent, URL_STRING_MAKE((const char*)tlsListener),
                          //TPTAG_TLS_VERSION(0),
                          TPTAG_CERTIFICATE(certificatePath),
                          TPTAG_TLS_VERIFY_DATE(0),
                          TPTAG_TLS_VERIFY_DEPTH(0),
                          TPTAG_TLS_VERIFY_PEER(TPTLS_VERIFY_NONE),
                          TPTAG_TLS_VERIFY_POLICY(TPTLS_VERIFY_NONE),
                          //TPTAG_TLS_VERIFY_POLICY(TPTLS_VERIFY_ALL),
                          //TPTAG_TLS_VERIFY_POLICY(TPTLS_VERIFY_SUBJECTS_ALL),
                          //TPTAG_X509_SUBJECT(0),
                          //TPTAG_TLS_VERIFY_SUBJECTS(NULL),
                          TAG_NULL());
    }
*/
  }
  InitProxyAccounts();
  CreateBaseSipCaps();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipEndPoint::FindListener(PString addr)
{
  if(agent == NULL) return FALSE;
  if(addr.Left(4) != "sip:") addr = "sip:"+addr;
  MCUURL url(addr);
  for(tport_t *tp = nta_agent_tports(agent); tp != NULL; tp = tport_next(tp))
  {
    tp_name_t const *tp_name = tport_name(tp);
    PString host = tp_name->tpn_host;
    PString port = tp_name->tpn_port;
    PString proto = tp_name->tpn_proto;
    if(host == url.GetHostName() && port == url.GetPort() && (proto == url.GetSipProto() || url.GetSipProto() == "*"))
      return TRUE;
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::ProcessSipQueue()
{
  PString *cmd = SipQueue.Pop();
  while(cmd != NULL)
  {
    if(cmd->Left(4) == "bye:")
    {
      PString callToken = cmd->Right(cmd->GetLength()-4);
      MCUSipConnection *sCon = FindConnectionWithoutLock(callToken);
      if(sCon)
      {
        if(sCon->IsConnected()) sCon->SendBYE();
        sCon->StopTransmitChannels();
        sCon->StopReceiveChannels();
        sCon->DeleteChannels();
        sCon->LeaveMCU(TRUE); // leave conference and delete connection
        PTRACE(1, "MCUSIP\tSIP BYE sent\n");
      }
    }
    if(cmd->Left(7) == "invite:")
    {
      PString data = cmd->Right(cmd->GetLength()-7);
      PString from = data.Tokenise(",")[0];
      PString to = data.Tokenise(",")[1];
      PString call_id = data.Tokenise(",")[2];
      SipMakeCall(from, to, call_id);
    }
    if(cmd->Left(12) == "fast_update:")
    {
      PString callToken = cmd->Right(cmd->GetLength()-12);
      MCUSipConnection *sCon = FindConnectionWithoutLock(callToken);
      if(sCon)
      {
        sCon->SendVFU();
      }
    }
    delete cmd;
    cmd = SipQueue.Pop();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::MainLoop()
{
  while(1)
  {
    if(restart)
    {
      restart = 0;
      Initialise();
    }
    if(terminating)
    {
      for(ProxyAccountMapType::iterator it = ProxyAccountMap.begin(); it != ProxyAccountMap.end(); )
      {
        if(it->second->status == 200)
          SipRegister(it->second, FALSE);
        ProxyAccountMap.erase(it++);
      }
      su_root_sleep(root,500);
      return;
    }
    ProcessSipQueue();
    ProcessProxyAccount();
    su_root_sleep(root,500);
    PTRACE(9, "MCUSIP\tSIP Down to sleep");
  }
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
