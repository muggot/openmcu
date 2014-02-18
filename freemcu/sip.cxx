
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

BOOL MCUSipConnnection::CreateTempSockets(PString localIP)
{
  unsigned localDataPort = FreeMCU::Current().GetEndpoint().GetRtpIpPortPair();
  PQoS * dataQos = NULL;
  PQoS * ctrlQos = NULL;
  aDataSocket = new PUDPSocket(dataQos);
  aControlSocket = new PUDPSocket(ctrlQos);
  vDataSocket = new PUDPSocket(dataQos);
  vControlSocket = new PUDPSocket(ctrlQos);
  while(!aDataSocket->Listen(localIP, 1, localDataPort) || !aControlSocket->Listen(localIP, 1, localDataPort+1))
  {
    aDataSocket->Close();
    aControlSocket->Close();
    localDataPort = FreeMCU::Current().GetEndpoint().GetRtpIpPortPair();
  }
  audio_rtp_port = localDataPort;
  localDataPort = FreeMCU::Current().GetEndpoint().GetRtpIpPortPair();
  while(!vDataSocket->Listen(localIP, 1, localDataPort) || !vControlSocket->Listen(localIP, 1, localDataPort+1))
  {
    vDataSocket->Close();
    vControlSocket->Close();
    localDataPort = FreeMCU::Current().GetEndpoint().GetRtpIpPortPair();
  }
  video_rtp_port = localDataPort;
  return TRUE;
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
  cout << trace;
  trace.Replace("   ","",TRUE,0);
  if(trace.IsEmpty()) return;

  if(trace.Left(4) == "send")
  {
    logMsgBuf = "MCUSIP\tSend SIP message:\n";
    logMsgBuf = logMsgBuf+trace;
    return;
  }
  else if(trace.Left(4) == "recv")
  {
    logMsgBuf = "MCUSIP\tReceived SIP message:\n";
    logMsgBuf = logMsgBuf+trace;
    return;
  }
  else if(trace.Find("---") != P_MAX_INDEX)
  {
    if(logMsgBuf.IsEmpty()) return;
    logMsgBuf = logMsgBuf+trace;
    PRegularExpression RegEx("cseq: [0-9]* (options|info|publish|subscribe|notify)", PRegularExpression::Extended|PRegularExpression::IgnoreCase);
    if(logMsgBuf.FindRegEx(RegEx) == P_MAX_INDEX) PTRACE(1, logMsgBuf);
    else PTRACE(6, logMsgBuf);
    logMsgBuf = "";
    return;
  }
  if(logMsgBuf.IsEmpty())
  {
    trace.Replace("\n","",TRUE,0);
    PTRACE(6, trace);
  }
  else if(trace.Find("\n") == P_MAX_INDEX && !logMsgBuf.IsEmpty())
  {
    logMsgBuf = logMsgBuf+trace+"\n";
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSipEndPoint::GetCapabilityParams(PString & capname, unsigned & pt, PString & name, unsigned & rate, PString & fmtp)
{
  if(capname == "")
    return FALSE;

  H323Capability *cap = H323Capability::Create(capname);
  if(cap == NULL && capname.Find("{sw}") == P_MAX_INDEX)
    cap = H323Capability::Create(capname+"{sw}");
  if(!cap)
    return FALSE;

  const OpalMediaFormat & mf = cap->GetMediaFormat();

  name = PString(cap->GetFormatName()).ToLower();
  if(name.Find("ulaw") != P_MAX_INDEX) name = "pcmu";
  if(name.Find("alaw") != P_MAX_INDEX) name = "pcma";
  if(name.Find("723") != P_MAX_INDEX) name = "g723";
  name = name.Left(PString(cap->GetFormatName()).Find("-")).Left(name.Find("_"));
  name.Replace(".","",TRUE,0);
  name.Replace("{sw}","",TRUE,0);
  if(name.Find("h263p") != P_MAX_INDEX) name.Replace("h263p","h263-1998",TRUE,0);
  if(mf.GetOptionInteger("Encoder Channels") == 2 || mf.GetOptionInteger("Decoder Channels") == 2)
    return FALSE;

  rate = mf.GetTimeUnits()*1000;

  if(MCUConfig("CODEC_OPTIONS").HasKey(cap->GetFormatName()))
  {
    fmtp = MCUConfig("CODEC_OPTIONS").GetString(cap->GetFormatName());
  } else {
    for (PINDEX j = 0; j < mf.GetOptionCount(); j++)
      if(mf.GetOption(j).GetFMTPName() != "" && mf.GetOption(j).GetFMTPDefault() != mf.GetOption(j).AsString())
        fmtp += mf.GetOption(j).GetFMTPName()+"="+mf.GetOption(j).AsString()+";";
  }
  if(rate == 90000) // video
    fmtp = "";

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

sdp_rtpmap_t * MCUSipEndPoint::CreateSdpRtpmap(su_home_t *sess_home, PString & capname, unsigned & dyn_pt)
{
  PString name, fmtp;
  unsigned pt, rate;
  if(!GetCapabilityParams(capname, pt, name, rate, fmtp))
    return NULL;

  if(pt == 128) { pt = dyn_pt; dyn_pt++; }

  sdp_rtpmap_t *rm = (sdp_rtpmap_t *)su_salloc(sess_home, sizeof(*rm));

  rm->rm_predef = 0;
  rm->rm_pt = pt;
  rm->rm_encoding = SdpText(name);
  rm->rm_rate = rate;
  if(fmtp != "") rm->rm_fmtp = SdpText(fmtp);
  return rm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

char * MCUSipEndPoint::SdpText(PString text)
{
  if(text.GetLength() == 0)
    return NULL;
  char *data = (char *)malloc((text.GetLength()+1) * sizeof(char));
  strcpy(data, text);
  return data;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sdp_media_t * MCUSipEndPoint::CreateSdpMedia(su_home_t *sess_home, PStringArray & caps, sdp_media_e m_type, sdp_proto_e m_proto, unsigned m_port, unsigned & dyn_pt)
{
  sdp_media_t *m = (sdp_media_t *)su_salloc(sess_home, sizeof(*m));
  m->m_type = m_type;
  m->m_proto = m_proto;
  m->m_port = m_port;
  m->m_mode = 3; // sendrecv

  sdp_rtpmap_t *rm = NULL; // capability iterator
  for(PINDEX i = 0; i < caps.GetSize(); i++)
  {
    sdp_rtpmap_t *rm_new = CreateSdpRtpmap(sess_home, caps[i], dyn_pt);
    if(!rm_new)
      continue;

    // do not duplicate video capability
    if(m_type == sdp_media_video && sdp_rtpmap_find_matching(m->m_rtpmaps, rm_new))
    {
      dyn_pt--;
      continue;
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

sdp_attribute_t * MCUSipEndPoint::CreateSdpAttr(su_home_t *sess_home, PString m_name, PString m_value)
{
  sdp_attribute_t *a = (sdp_attribute_t *)su_salloc(sess_home, sizeof(*a));
  a->a_name = SdpText(m_name);
  a->a_value = SdpText(m_value);
  return a;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString MCUSipEndPoint::CreateSdpInvite(MCUSipConnnection *sCon, PString local_url, PString remote_url)
{
  /*
    get endpoint param
  */
  MCUURL url(local_url);
  PString local_user = url.GetUserName();
  PString local_host = url.GetHostName();

  PString prefAudioCap = FreeMCU::Current().GetEndpointParamFromUrl("Audio codec", remote_url);
  PString prefVideoCap = FreeMCU::Current().GetEndpointParamFromUrl("Video codec", remote_url);

  unsigned bandwidth = atoi(FreeMCU::Current().GetEndpointParamFromUrl("Preferred bandwidth to MCU", remote_url));

  PString pref_rtp_proto = FreeMCU::Current().GetEndpointParamFromUrl("RTP proto", remote_url);
  if(pref_rtp_proto == "") pref_rtp_proto = "RTP";
  sCon->rtp_proto = pref_rtp_proto;

  /*
     create temp sockets
  */
  sCon->CreateTempSockets(local_host);

  /*
     create and check capabilities
  */
  PStringArray tsCaps, tvCaps;
  unsigned dyn_pt = 96;
  for(PINDEX i = 0; FreeMCU::Current().GetEndpoint().tsCaps[i]!=NULL; i++)
  {
    PString capname = FreeMCU::Current().GetEndpoint().tsCaps[i];
    if(prefAudioCap != "" && prefAudioCap != capname) continue;
    tsCaps.AppendString(capname);
  }
  for(PINDEX i = 0; FreeMCU::Current().GetEndpoint().tvCaps[i]!=NULL; i++)
  {
    PString capname = FreeMCU::Current().GetEndpoint().tvCaps[i];
    if(prefVideoCap != "" && prefVideoCap != capname) continue;
    tvCaps.AppendString(capname);
  }
  if(tsCaps.GetSize() == 0 && tvCaps.GetSize() == 0)
    return "";

  /*
    create sdp session
  */
  su_home_t *sess_home = su_home_create();
  sdp_session_t *sess = (sdp_session_t *)su_salloc(sess_home, sizeof(*sess));
  sdp_origin_t *o = (sdp_origin_t *)su_salloc(sess_home, sizeof(*o));
  sdp_time_t *t = (sdp_time_t *)su_salloc(sess_home, sizeof(*t));
  sdp_connection_t *c = (sdp_connection_t *)su_salloc(sess_home, sizeof(*c));
  sdp_bandwidth_t *b = (sdp_bandwidth_t *)su_salloc(sess_home, sizeof(*b));

  sess->sdp_origin = o;
  sess->sdp_time = t;		/* zero time is fine for SIP */
  sess->sdp_connection = c;
  sess->sdp_bandwidths = b;
  sess->sdp_subject = "Talk";

  o->o_username = SdpText(local_user);
  o->o_id = rand();
  o->o_version = 1;
  o->o_address = c;

  c->c_nettype = sdp_net_in;
  c->c_addrtype = sdp_addr_ip4;
  c->c_address = SdpText(local_host);

  if(bandwidth)
  {
    b->b_modifier = sdp_bw_as;
    b->b_value = bandwidth;
  }

  // create default RTP media
  sdp_media_t *m_audio_rtp = CreateSdpMedia(sess_home, tsCaps, sdp_media_audio, sdp_proto_rtp, sCon->audio_rtp_port, dyn_pt);
  sdp_media_t *m_video_rtp = CreateSdpMedia(sess_home, tvCaps, sdp_media_video, sdp_proto_rtp, sCon->video_rtp_port, dyn_pt);
  sdp_media_t *m_audio_srtp = NULL;
  sdp_media_t *m_video_srtp = NULL;
  sdp_media_t *m = NULL;

  // check proto
  if(pref_rtp_proto == "SRTP")
  {
    m_audio_srtp = m_audio_rtp;
    m_audio_rtp = NULL;
    m_video_srtp = m_video_rtp;
    m_video_rtp = NULL;
  }
  if(pref_rtp_proto == "SRTP/RTP")
  {
    if(m_audio_rtp) m_audio_srtp = sdp_media_dup(sess_home, m_audio_rtp, sess);
    if(m_video_rtp) m_video_srtp = sdp_media_dup(sess_home, m_video_rtp, sess);
  }

  // init SRTP
  if(m_audio_srtp)
  {
    m_audio_srtp->m_proto = sdp_proto_srtp;
    sCon->key_audio80 = srtp_get_random_keysalt();
    sCon->key_audio32 = srtp_get_random_keysalt();
    sdp_attribute_t *a = NULL;
    a = m_audio_srtp->m_attributes = CreateSdpAttr(sess_home, "crypto", "1 AES_CM_128_HMAC_SHA1_80 inline:"+sCon->key_audio80);
    a = a->a_next = CreateSdpAttr(sess_home, "crypto", "2 AES_CM_128_HMAC_SHA1_32 inline:"+sCon->key_audio32);
    //a = a->a_next = CreateSdpAttr(sess_home, "encryption", "optional");
  }
  if(m_video_srtp)
  {
    m_video_srtp->m_proto = sdp_proto_srtp;
    sCon->key_video80 = srtp_get_random_keysalt();
    sCon->key_video32 = srtp_get_random_keysalt();
    sdp_attribute_t *a = NULL;
    a = m_video_srtp->m_attributes = CreateSdpAttr(sess_home, "crypto", "1 AES_CM_128_HMAC_SHA1_80 inline:"+sCon->key_video80);
    a = a->a_next = CreateSdpAttr(sess_home, "crypto", "2 AES_CM_128_HMAC_SHA1_32 inline:"+sCon->key_video32);
    //a = a->a_next = CreateSdpAttr(sess_home, "encryption", "optional");
  }

  // add media to session
  if(m_audio_srtp)
  {
    if(sess->sdp_media)
      m = m->m_next = m_audio_srtp;
    else
      m = sess->sdp_media = m_audio_srtp;
  }
  if(m_audio_rtp)
  {
    if(sess->sdp_media)
      m = m->m_next = m_audio_rtp;
    else
      m = sess->sdp_media = m_audio_rtp;
  }
  if(m_video_srtp)
  {
    if(sess->sdp_media)
      m = m->m_next = m_video_srtp;
    else
      m = sess->sdp_media = m_video_srtp;
  }
  if(m_video_rtp)
  {
    if(sess->sdp_media)
      m = m->m_next = m_video_rtp;
    else
      m = sess->sdp_media = m_video_rtp;
  }

  char buffer[16384];
  sdp_printer_t *printer = sdp_print(sess_home, sess, buffer, sizeof(buffer), 0);
  sdp_printer_free(printer);

  su_home_check(sess_home);
  su_home_unref(sess_home);

  sCon->invite_sdp_txt = buffer;
  return sCon->invite_sdp_txt;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString MCUSipEndPoint::CreateRuriStr(const msg_t *msg, int direction)
{
  // 0=incoming, 1=outgoing
  PTRACE(1, "MCUSIP\tCreateRuriStr");
  sip_t *sip = sip_object(msg);
  su_home_t *home = msg_home(msg);
  if(sip == NULL || home == NULL) return "";

  sip_addr_t *sip_to;
  if(direction == 0)
    sip_to = sip_from_dup(home, sip->sip_from);
  else
    sip_to = sip_to_dup(home, sip->sip_to);

  PString ruri = "sip:";

  PString user;
  if(sip_to->a_url->url_user)
    user = sip_to->a_url->url_user;
  if(user == "" && sip->sip_contact && sip->sip_contact->m_url)
    user = sip->sip_contact->m_url->url_user;
  ruri += user;

  if(strcmp(sip->sip_via->v_host, "0.0.0.0") != 0 && sip->sip_request)
    ruri = ruri+"@"+PString(sip->sip_via->v_host);
  else
    ruri = ruri+"@"+PString(sip_to->a_url->url_host);

  if(sip->sip_via->v_port && sip->sip_request)
    ruri = ruri+":"+sip->sip_via->v_port;
  else if(sip_to->a_url->url_port)
    ruri = ruri+":"+sip_to->a_url->url_port;
//  else if(sip->sip_contact && sip->sip_contact->m_url->url_port)
//    ruri = ruri+":"+sip->sip_contact->m_url->url_port;

  if(PString(sip->sip_via->v_protocol).Find("UDP") != P_MAX_INDEX)
    ruri += ";transport=udp";
  else if(PString(sip->sip_via->v_protocol).Find("TCP") != P_MAX_INDEX)
    ruri += ";transport=tcp";
  return ruri;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SipRTP_UDP *MCUSipConnnection::CreateRTPSession(int pt, SipCapability *sc)
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
    unsigned port_base, port_max;
    if(direction == 1 && !sc->media && audio_rtp_port)
    {
      port_base = port_max = audio_rtp_port;
      audio_rtp_port = 0;
    } else if(direction == 1 && sc->media && video_rtp_port) {
      port_base = port_max = video_rtp_port;
      video_rtp_port = 0;
    } else {
      port_base = endpoint.GetRtpIpPortBase();
      port_max = endpoint.GetRtpIpPortMax();
    }
    session->Open(lIP,port_base,port_max,endpoint.GetRtpIpTypeofService(),*this,NULL,NULL);
    session->SetRemoteSocketInfo(rIP,sc->port,TRUE);
    sc->lport = session->GetLocalDataPort();

    sc->sdp = PString("m=") + ((!sc->media)?"audio ":"video ");
    if(sc->secure_type == SECURE_TYPE_SRTP)
    {
      sc->sdp += PString(sc->lport)+" RTP/SAVP "+PString(pt)+"\r\n";
      sc->sdp += "a=crypto:1 "+sc->srtp_local_type+" inline:"+sc->srtp_local_key;
      if(sc->srtp_local_param != "")
        sc->sdp += sc->srtp_local_param;
      sc->sdp += "\r\n";
    } else {
      sc->sdp += PString(sc->lport)+" RTP/AVP "+PString(pt)+"\r\n";
    }
    if(sc->mode == 0) sc->sdp = sc->sdp + "a=inactive\r\n";
    else if(sc->mode == 1) sc->sdp = sc->sdp + "a=sendonly\r\n";
    else if(sc->mode == 2) sc->sdp = sc->sdp + "a=recvonly\r\n";
    else if(sc->mode == 3) sc->sdp = sc->sdp + "a=sendrecv\r\n";
    if(pt != 0 && pt != 8)
    {
      sc->sdp = sc->sdp + "a=rtpmap:" + PString(pt) + " " + sc->format + "/" + PString(sc->clock);
      if(sc->cnum) sc->sdp = sc->sdp + "/" + PString(sc->cnum);
      sc->sdp = sc->sdp + "\r\n";
      if(sc->cap)
      {
        PString fmtp = "";
        if(MCUConfig("CODEC_OPTIONS").HasKey(sc->h323))
        {
          fmtp = MCUConfig("CODEC_OPTIONS").GetString(sc->h323);
        } else if(pref_video_cap != "" ) {
          const OpalMediaFormat & mf = sc->cap->GetMediaFormat();
          for (PINDEX j = 0; j < mf.GetOptionCount(); j++)
            if(mf.GetOption(j).GetFMTPName() != "" && mf.GetOption(j).GetFMTPDefault() != mf.GetOption(j).AsString())
              fmtp += mf.GetOption(j).GetFMTPName()+"="+mf.GetOption(j).AsString()+";";
        } else {
          fmtp = sc->parm; // send incoming(from the client) ftmp
        }
        if(fmtp != "")
          sc->sdp = sc->sdp + "a=fmtp:" + PString(pt) + " " + fmtp + "\r\n";
      }
    }
    sdp_msg += sc->sdp;
  }
  return session;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnnection::CreateAudioChannel(int pt, int dir)
{
  SipCapMapType::iterator it = sipCaps.find(pt);
  if(it == sipCaps.end()) return 0;

  SipCapability *sc = it->second;
  H323Capability * cap = sc->cap;
  if(cap == NULL) return 0;

  SipRTP_UDP *session = NULL;
  if(sc->secure_type == SECURE_TYPE_NONE)
  {
    session = CreateRTPSession(pt, sc);
  }
  else if(sc->secure_type == SECURE_TYPE_ZRTP)
  {
    session = CreateRTPSession(pt, sc);
    session->SetMaster(TRUE);
    session->SetConnection(this);
    session->CreateZRTP();
  }
  else if(sc->secure_type == SECURE_TYPE_DTLS_SRTP)
  {
    session = CreateRTPSession(pt, sc);
    //session->CreateDTLS(dir, sc->dtls_fp_type, sc->dtls_fp);
  }
  else if(sc->secure_type == SECURE_TYPE_SRTP)
  {
    if(sc->srtp_local_key == "")
    {
      sc->srtp_local_type = sc->srtp_remote_type;
      if(sc->srtp_local_type == AES_CM_128_HMAC_SHA1_80)
      {
        if(key_audio80 != "")
          sc->srtp_local_key = key_audio80;
        else
          sc->srtp_local_key = key_audio80 = srtp_get_random_keysalt();
      }
      else if(sc->srtp_local_type == AES_CM_128_HMAC_SHA1_32)
      {
        if(key_audio32 != "")
          sc->srtp_local_key = key_audio32;
        else
          sc->srtp_local_key = key_audio32 = srtp_get_random_keysalt();
      }
    }
    session = CreateRTPSession(pt, sc);
    if(dir == 0) // incoming
      session->CreateSRTP(dir, sc->srtp_remote_type, sc->srtp_remote_key);
    else
      session->CreateSRTP(dir, sc->srtp_local_type, sc->srtp_local_key);
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

int MCUSipConnnection::CreateVideoChannel(int pt, int dir)
{
  SipCapMapType::iterator it = sipCaps.find(pt);
  if(it == sipCaps.end()) return 0;

  SipCapability *sc = it->second;
  H323Capability * cap = sc->cap;
  if(cap == NULL) return 0;

  SipRTP_UDP *session = NULL;
  if(sc->secure_type == SECURE_TYPE_NONE)
  {
    session = CreateRTPSession(pt, sc);
  }
  else if(sc->secure_type == SECURE_TYPE_ZRTP)
  {
    session = CreateRTPSession(pt, sc);
    session->CreateZRTP();
  }
  else if(sc->secure_type == SECURE_TYPE_SRTP)
  {
    if(sc->srtp_local_key == "")
    {
      sc->srtp_local_type = sc->srtp_remote_type;
      if(sc->srtp_local_type == AES_CM_128_HMAC_SHA1_80)
      {
        if(key_video80 != "")
          sc->srtp_local_key = key_video80;
        else
          sc->srtp_local_key = key_video80 = srtp_get_random_keysalt();
      }
      else if(sc->srtp_local_type == AES_CM_128_HMAC_SHA1_32)
      {
        if(key_video32 != "")
          sc->srtp_local_key = key_video32;
        else
          sc->srtp_local_key = key_video32 = srtp_get_random_keysalt();
      }
    }
    session = CreateRTPSession(pt, sc);
    if(dir == 0) // incoming
      session->CreateSRTP(dir, sc->srtp_remote_type, sc->srtp_remote_key);
    else
      session->CreateSRTP(dir, sc->srtp_local_type, sc->srtp_local_key);
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

void MCUSipConnnection::CreateLogicalChannels()
{
  if(scap >= 0) // audio capability is set
  {
    CreateAudioChannel(scap,0);
    CreateAudioChannel(scap,1);
  }
  if(vcap >= 0) // video capability is set
  {
    CreateVideoChannel(vcap,0);
    CreateVideoChannel(vcap,1);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::StartChannel(int pt, int dir)
{
  if(pt < 0) return;
  SipCapMapType::iterator cir = sipCaps.find(pt);
  if(cir == sipCaps.end()) return;
  if(dir == 0 && (cir->second->mode&2) && cir->second->inpChan && !cir->second->inpChan->IsRunning()) cir->second->inpChan->Start();
  if(dir == 1 && (cir->second->mode&1) && cir->second->outChan && !cir->second->outChan->IsRunning()) cir->second->outChan->Start();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::StartReceiveChannels()
{
  StartChannel(scap,0);
  StartChannel(vcap,0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::StartTransmitChannels()
{
  StartChannel(scap,1);
  StartChannel(vcap,1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::StopChannel(int pt, int dir)
{
  if(pt < 0) return;
  SipCapMapType::iterator cir = sipCaps.find(pt);
  if(cir == sipCaps.end()) return;
  if(dir==0 && cir->second->inpChan) cir->second->inpChan->CleanUpOnTermination();
  if(dir==1 && cir->second->outChan) cir->second->outChan->CleanUpOnTermination();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::StopTransmitChannels()
{
  StopChannel(scap,1);
  StopChannel(vcap,1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::StopReceiveChannels()
{
  StopChannel(scap,0);
  StopChannel(vcap,0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::DeleteMediaChannels(int pt)
{
  if(pt<0) return;
  SipCapMapType::iterator cir = sipCaps.find(pt);
  if(cir == sipCaps.end()) return;
  if(cir->second->inpChan) { delete cir->second->inpChan; cir->second->inpChan = NULL; }
  if(cir->second->outChan) { delete cir->second->outChan; cir->second->outChan = NULL; }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::DeleteChannels()
{
  DeleteMediaChannels(scap);
  DeleteMediaChannels(vcap);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::CleanUpOnCallEnd()
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

void MCUSipConnnection::FastUpdatePicture()
{
  SipCapMapType::iterator cir = sipCaps.find(vcap);
  if(cir != sipCaps.end() && cir->second->outChan != NULL)
  {
    H323VideoCodec *vcodec = (H323VideoCodec*)cir->second->outChan->GetCodec();
    if(vcodec != NULL)
      vcodec->OnFastUpdatePicture();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::SendLogicalChannelMiscCommand(H323Channel & channel, unsigned commandIdentifier)
{
  if(commandIdentifier == H245_MiscellaneousCommand_type::e_videoFastUpdatePicture)
    SendFastUpdatePicture();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::ReceiveDTMF(PString sdp)
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

void SipCapability::Print()
{
 cout << "\r\n";
 cout << "Format: " << format << " Payload: " << " Mode: " << mode << "\r\n";
 cout << "Remote IP: " << remote_ip << " Port: " << port << "\r\n";
 cout << "Secure: " << secure_type << "\r\n";
 cout << "Clock: " << clock << " Bandwidth: " << bandwidth << "\r\n";
 cout << "Parameters: " << parm << "\r\n\r\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::FindCapability_H263(SipCapability &sc,PStringArray &keys, const char * _H323Name, const char * _SIPName)
{
 PString H323Name(_H323Name);
 PString SIPName(_SIPName);
 for(int kn=0; kn<keys.GetSize(); kn++) 
 { 
  if(keys[kn].Find(SIPName + "=")==0)
   { 
    sc.cap = H323Capability::Create(H323Name);
    if(sc.cap == NULL) return;
    vcap = sc.payload; sc.h323 = H323Name; sc.parm += keys[kn]; 
    OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat(); 
    int mpi = (keys[kn].Mid(SIPName.GetLength()+1)).AsInteger();
    wf.SetOptionInteger(SIPName + " MPI",mpi);
    return;
   }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::SelectCapability_H261(SipCapability &sc,PStringArray &tvCaps)
{
 //int f=0; // annex f
 PStringArray keys = sc.parm.Tokenise(";");
 sc.parm = "";
 //for(int kn=0; kn<keys.GetSize(); kn++) 
 // { if(keys[kn] == "F=1") { c.parm = "F=1;"; f=1; break; } }

 if(tvCaps.GetStringsIndex("H.261-CIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.261-CIF{sw}","CIF");
 if(tvCaps.GetStringsIndex("H.263-CIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.261-QCIF{sw}","QCIF");

 if(sc.cap)
 {
  OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
  //wf.SetOptionBoolean("_advancedPrediction",f);
  if(sc.bandwidth) wf.SetOptionInteger("Max Bit Rate",sc.bandwidth*1000);
 }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::SelectCapability_H263(SipCapability &sc,PStringArray &tvCaps)
{
 int f=0; // annex f
 PStringArray keys = sc.parm.Tokenise(";");
 sc.parm = "";
 for(int kn=0; kn<keys.GetSize(); kn++) 
  { if(keys[kn] == "F=1") { sc.parm = "F=1;"; f=1; break; } }

 if(tvCaps.GetStringsIndex("H.263-16CIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.263-16CIF{sw}","CIF16");
 if(tvCaps.GetStringsIndex("H.263-4CIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.263-4CIF{sw}","CIF4");
 if(tvCaps.GetStringsIndex("H.263-CIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.263-CIF{sw}","CIF");
 if(tvCaps.GetStringsIndex("H.263-QCIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.263-QCIF{sw}","QCIF");
 if(tvCaps.GetStringsIndex("H.263-SQCIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.263-SQCIF{sw}","SQCIF");

 if(sc.cap)
 {
  OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
  wf.SetOptionBoolean("_advancedPrediction",f);
  if(sc.bandwidth) wf.SetOptionInteger("Max Bit Rate",sc.bandwidth*1000);
 }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::SelectCapability_H263p(SipCapability &sc,PStringArray &tvCaps)
{
 int f=0,d=0,e=0,g=0; // annexes
 PStringArray keys = sc.parm.Tokenise(";");
 sc.parm = "";
 for(int kn=0; kn<keys.GetSize(); kn++) 
 { 
  if(keys[kn] == "F=1") { sc.parm += "F=1;"; f=1; } 
  else if(keys[kn] == "D=1") { sc.parm += "D=1;"; d=1; } 
  else if(keys[kn] == "E=1") { sc.parm += "E=1;"; e=1; } 
  else if(keys[kn] == "G=1") { sc.parm += "G=1;"; g=1; } 
 }

 if(tvCaps.GetStringsIndex("H.263p-16CIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.263p-16CIF{sw}","CIF16");
 if(tvCaps.GetStringsIndex("H.263p-4CIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.263p-4CIF{sw}","CIF4");
 if(tvCaps.GetStringsIndex("H.263p-CIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.263p-CIF{sw}","CIF");
 if(tvCaps.GetStringsIndex("H.263p-QCIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.263p-QCIF{sw}","QCIF");
 if(tvCaps.GetStringsIndex("H.263p-SQCIF{sw}")!=P_MAX_INDEX && sc.cap == NULL)
  FindCapability_H263(sc,keys,"H.263p-SQCIF{sw}","SQCIF");

 if(sc.cap)
 {
  OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
  wf.SetOptionBoolean("_advancedPrediction",f);
  wf.SetOptionBoolean("_unrestrictedVector",d);
  wf.SetOptionBoolean("_arithmeticCoding",e);
  wf.SetOptionBoolean("_pbFrames",g);
  if(sc.bandwidth) wf.SetOptionInteger("Max Bit Rate",sc.bandwidth*1000);
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

void MCUSipConnnection::SelectCapability_H264(SipCapability &sc,PStringArray &tvCaps)
{
 int profile = 0, level = 0;
 int max_mbps = 0, max_fs = 0, max_br = 0;
 PStringArray keys = sc.parm.Tokenise(";");
 for(int kn = 0; kn < keys.GetSize(); kn++) 
 { 
  if(keys[kn].Find("profile-level-id=") == 0) 
  { 
   int p = (keys[kn].Tokenise("=")[1]).AsInteger(16);
   profile = (p>>16); level = (p&255);
  } 
  else if(keys[kn].Find("max-mbps=") == 0) 
  { 
   max_mbps = (keys[kn].Tokenise("=")[1]).AsInteger();
  } 
  else if(keys[kn].Find("max-fs=") == 0) 
  { 
   max_fs = (keys[kn].Tokenise("=")[1]).AsInteger();
  } 
  else if(keys[kn].Find("max-br=") == 0) 
  { 
   max_br = (keys[kn].Tokenise("=")[1]).AsInteger();
  } 
 }
 cout << "profile " << profile << " level " << level << "\n";
// if(profile == 0 || level == 0) return;
 if(level == 0)
 {
   PTRACE(2,"SIP_CONNECTION\tH.264 level will set to " << FreeMCU::Current().h264DefaultLevelForSip);
   level = FreeMCU::Current().h264DefaultLevelForSip;
 }
 int l = 0;
 while(h241_to_x264_levels[l].idc != 0)
 {
  if(level == h241_to_x264_levels[l].idc) { level = h241_to_x264_levels[l].h241; break; }
  l++;
 }
 profile = 64;

 cout << "profile " << profile << " level " << level << "\n";
 int cl = 0;
 for(int cn = 0; cn < tvCaps.GetSize(); cn++)
 {
  if(tvCaps[cn].Find("H.264")==0)
  {
   H323Capability *cap = H323Capability::Create(tvCaps[cn]);
   if(cap != NULL)
   {
    const OpalMediaFormat & mf = cap->GetMediaFormat(); 
    int flevel = mf.GetOptionInteger("Generic Parameter 42");
    cout << "flevel" << flevel << "\n";
    if(flevel > cl && flevel <= level) 
     { cl = flevel; if(sc.cap) delete sc.cap; sc.cap = cap; sc.h323 = tvCaps[cn]; }
    else { delete cap; }
    if(flevel == level) break; 
   }
  }
 }

 if(sc.cap)
 {
  OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
  if(sc.bandwidth) wf.SetOptionInteger("Max Bit Rate",sc.bandwidth*1000);
  wf.SetOptionInteger("Generic Parameter 42",level);
  wf.SetOptionInteger("Generic Parameter 3",max_mbps);
  wf.SetOptionInteger("Generic Parameter 4",max_fs);
  wf.SetOptionInteger("Generic Parameter 6",max_br);
  vcap = sc.payload;
 }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::SelectCapability_VP8(SipCapability &sc,PStringArray &tvCaps)
{
 int width = 0, height = 0;
 PStringArray keys = sc.parm.Tokenise(";");
 for(int kn = 0; kn < keys.GetSize(); kn++)
 {
  if(keys[kn].Find("width=") == 0)
   width = (keys[kn].Tokenise("=")[1]).AsInteger();
  else if(keys[kn].Find("height=") == 0)
   height = (keys[kn].Tokenise("=")[1]).AsInteger();
 }

 PString H323Name;

 if(!sc.cap && width && height)
 {
  for(int cn = 0; cn < tvCaps.GetSize(); cn++)
  {
   if(tvCaps[cn].Find("VP8")==0)
   {
    H323Name = tvCaps[cn];
    sc.cap = H323Capability::Create(H323Name);
    if(sc.cap)
    {
     const OpalMediaFormat & mf = sc.cap->GetMediaFormat();
     if(width == mf.GetOptionInteger("Frame Width") && height == mf.GetOptionInteger("Frame Height"))
      break;
     else
      sc.cap=NULL;
    }
   }
  }
  if(!sc.cap && tvCaps.GetStringsIndex("VP8-CIF{sw}") != P_MAX_INDEX)
  {
   H323Name = "VP8-CIF{sw}";
   sc.cap = H323Capability::Create(H323Name);
   if(sc.cap)
   {
    OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
    wf.SetOptionInteger("Frame Width", width);
    wf.SetOptionInteger("Frame Height", height);
   }
  }
 }

 if(!sc.cap && tvCaps.GetStringsIndex("VP8-CIF{sw}") != P_MAX_INDEX)
 {
  H323Name = "VP8-CIF{sw}";
  sc.cap = H323Capability::Create(H323Name);
 }

 if(sc.cap)
 {
  vcap = sc.payload;
  if(H323Name != "") sc.h323 = H323Name;
  OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
  if(remoteApplication.ToLower().Find("linphone") == 0) wf.SetOptionEnum("Picture ID Size", 0);
  if(sc.bandwidth) wf.SetOptionInteger("Max Bit Rate",sc.bandwidth*1000);
 }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::SelectCapability_SPEEX(SipCapability &sc,PStringArray &tsCaps)
{
  PString H323Name;
  if(sc.clock == 8000) H323Name = "Speex_8K{sw}";
  else if(sc.clock == 16000) H323Name = "Speex_16K{sw}";
  else if(sc.clock == 32000) H323Name = "Speex_32K{sw}";

  int vbr = -1;
  int mode = -1;

  sc.parm.Replace(" ","",TRUE,0);
  PStringArray keys = sc.parm.Tokenise(";");
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

  if(sc.cap) sc.cap = NULL;
  sc.cap = H323Capability::Create(H323Name);
  if(sc.cap)
  {
    scap = sc.payload;
    sc.h323 = H323Name;
    OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
    if (vbr > -1) wf.SetOptionEnum("vbr", vbr);
    if (mode > -1) wf.SetOptionInteger("mode", mode);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::SelectCapability_OPUS(SipCapability &sc,PStringArray &tsCaps)
{
  PString H323Name;
  if(sc.clock == 8000) H323Name = "OPUS_8K{sw}";
  else if(sc.clock == 16000) H323Name = "OPUS_16K{sw}";
  else if(sc.clock == 48000) H323Name = "OPUS_48K{sw}";

  int cbr = -1;
  int maxaveragebitrate = -1;
  int useinbandfec = -1;
  int usedtx = -1;

  sc.parm.Replace(" ","",TRUE,0);
  PStringArray keys = sc.parm.Tokenise(";");
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

  if(sc.cap) sc.cap = NULL;
  sc.cap = H323Capability::Create(H323Name);
  if(sc.cap)
  {
   scap = sc.payload;
   sc.h323 = H323Name;
   OpalMediaFormat & wf = sc.cap->GetWritableMediaFormat();
   if (cbr > -1) wf.SetOptionInteger("cbr", cbr);
   if (maxaveragebitrate > -1) wf.SetOptionInteger("maxaveragebitrate", maxaveragebitrate);
   if (useinbandfec > -1) wf.SetOptionInteger("useinbandfec", useinbandfec);
   if (usedtx > -1) wf.SetOptionInteger("usedtx", usedtx);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

sdp_parser_t *MCUSipConnnection::SdpParser(PString sdp_txt)
{
  // RTP/SAVPF parse
  BOOL found_savpf = FALSE;
  if(sdp_txt.Find("RTP/SAVPF") != P_MAX_INDEX)
  {
    sdp_txt.Replace("RTP/SAVPF","RTP/SAVP",TRUE,0);
    found_savpf = TRUE;
  }
  sdp_parser_t *parser = sdp_parse(sep->GetHome(), sdp_txt, sdp_txt.GetLength(), 0);
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

int MCUSipConnnection::ProcessSDP(PString & sdp_txt, SipCapMapType & sipCaps, int reinvite)
{
  PTRACE(1, "MCUSIP\tProcessSDP");
  sdp_parser_t *parser = SdpParser(sdp_txt);
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

  for(sdp_session_t *sdp = sess; sdp != NULL; sdp = sdp->sdp_next)
  {
    if(!sdp->sdp_origin || !sdp->sdp_connection)
    { PTRACE(1, "MCUSIP\tSDP parsing warning: origin or connection line is missing"); continue; }
    if(sess->sdp_connection->c_addrtype != sdp_addr_ip4)
    { PTRACE(1, "MCUSIP\tSDP parsing warning: unknown connection address type"); continue; }
    if(!sess->sdp_connection->c_address)
    { PTRACE(1, "MCUSIP\tSDP parsing warning: empty connection address"); continue; }

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

      int port;
      if(m->m_port) port = m->m_port;
      else continue;

      int media;
      if(m->m_type == sdp_media_audio)      media = 0;
      else if(m->m_type == sdp_media_video) media = 1;
      else continue;


      int mode = 3; // inactive, recvonly, sendonly, sendrecv
      if(m->m_mode == 1)      mode = 2;
      else if(m->m_mode == 2) mode = 1;
      else if(m->m_mode == 3) mode = 3;

      int bw = 0;
      if(m->m_bandwidths) bw = m->m_bandwidths->b_value;

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
        int payload = rm->rm_pt;
        SipCapMapType::iterator it = sipCaps.find(payload);
        if(it != sipCaps.end()) continue;
        SipCapability *sc = new SipCapability(payload, media, mode, port, bw);
        if(rm->rm_encoding) sc->format = rm->rm_encoding;
        if(rm->rm_rate) sc->clock = rm->rm_rate;
        if(rm->rm_fmtp) sc->parm = rm->rm_fmtp;
        //sc->cnum
        if(sdp->sdp_connection->c_address) sc->remote_ip = sdp->sdp_connection->c_address;
        if(sdp->sdp_bandwidths)            sc->bandwidth = sdp->sdp_bandwidths->b_value;
        if(sdp->sdp_origin->o_username)    sc->sess_username = sdp->sdp_origin->o_username;
        if(sdp->sdp_origin->o_id)          sc->sess_id = sdp->sdp_origin->o_id;
        if(sdp->sdp_origin->o_version)     sc->sess_ver = sdp->sdp_origin->o_version;
        // secure
        sc->secure_type = secure_type;
        sc->srtp_remote_type = srtp_type;
        sc->srtp_remote_key = srtp_key;
        sc->srtp_remote_param = srtp_param;
        sc->zrtp_hash = zrtp_hash;
        sc->dtls_fp_type = dtls_fp_type;
        sc->dtls_fp = dtls_fp;
        //
        sipCaps.insert(SipCapMapType::value_type(payload, sc));
      }
    }
  }
  sdp_parser_free(parser);

  PStringArray tsCaps, tvCaps;
  int cn = 0; while(endpoint.tsCaps[cn]!=NULL) { tsCaps.AppendString(endpoint.tsCaps[cn]); cn++; }
  cn = 0; while(endpoint.tvCaps[cn]!=NULL) { tvCaps.AppendString(endpoint.tvCaps[cn]); cn++; }

  scap = -1; vcap = -1;
  for(SipCapMapType::iterator it=sipCaps.begin(); it!=sipCaps.end(); it++)
  {
    SipCapability &c = it->second[0];
    if(c.media == 0)
    {
      if(scap >= 0) continue;
      // PCMU
      if((c.format.ToLower() == "pcmu" || c.payload == 0) && tsCaps.GetStringsIndex("G.711-uLaw-64k")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "G.711-uLaw-64k{sw}"; c.cap = H323Capability::Create(c.h323); }
      // PCMA
      else if((c.format.ToLower() == "pcma" || c.payload == 8) && tsCaps.GetStringsIndex("G.711-ALaw-64k")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "G.711-ALaw-64k{sw}"; c.cap = H323Capability::Create(c.h323); }
      // G.722
      else if(c.format.ToLower() == "g722" && tsCaps.GetStringsIndex("G.722-64k{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "G.722-64k{sw}"; c.cap = H323Capability::Create(c.h323); }
      // G.723.1
      else if(c.format.ToLower() == "g723" && tsCaps.GetStringsIndex("G.7231-6.3k[e]{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "G.7231-6.3k[e]{sw}"; c.cap = H323Capability::Create(c.h323); }
      // G.726-16
      else if(c.format.ToLower() == "g726-16" && tsCaps.GetStringsIndex("G.726-16k{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "G.726-16k{sw}"; c.cap = H323Capability::Create(c.h323); }
      // G.726-24
      else if(c.format.ToLower() == "g726-24" && tsCaps.GetStringsIndex("G.726-24k{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "G.726-24k{sw}"; c.cap = H323Capability::Create(c.h323); }
      // G.726-32
      else if(c.format.ToLower() == "g726-32" && tsCaps.GetStringsIndex("G.726-32k{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "G.726-32k{sw}"; c.cap = H323Capability::Create(c.h323); }
      // G.726-40
      else if(c.format.ToLower() == "g726-40" && tsCaps.GetStringsIndex("G.726-40k{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "G.726-40k{sw}"; c.cap = H323Capability::Create(c.h323); }
      // G.728
      else if(c.format.ToLower() == "g728" && tsCaps.GetStringsIndex("G.728-16k[e]")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "G.728-16k[e]"; c.cap = H323Capability::Create(c.h323); }
      // G.729A
      else if(c.format.ToLower() == "g729" && tsCaps.GetStringsIndex("G.729A-8k[e]{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "G.729A-8k[e]{sw}"; c.cap = H323Capability::Create(c.h323); }
      // iLBC-13k3
      else if(c.format.ToLower() == "ilbc" && c.parm == "mode=30;" && tsCaps.GetStringsIndex("iLBC-13k3{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "iLBC-13k3{sw}"; c.cap = H323Capability::Create(c.h323); }
      // iLBC-15k2
      else if(c.format.ToLower() == "ilbc" && c.parm == "mode=20;" && tsCaps.GetStringsIndex("iLBC-15k2{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "iLBC-15k2{sw}"; c.cap = H323Capability::Create(c.h323); }
      // SILK 16000
      else if(c.format.ToLower() == "silk" && c.clock == 16000 && tsCaps.GetStringsIndex("SILK_B40{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "SILK_B40{sw}"; c.cap = H323Capability::Create(c.h323); }
      // SILK 24000
      else if(c.format.ToLower() == "silk" && c.clock == 24000 && tsCaps.GetStringsIndex("SILK_B40_24K{sw}")!=P_MAX_INDEX)
      { scap = c.payload; c.h323 = "SILK_B40_24K{sw}"; c.cap = H323Capability::Create(c.h323); }
      // SPEEX
      else if(c.format.ToLower() == "speex")
        SelectCapability_SPEEX(c,tsCaps);
      // OPUS
      else if(c.format.ToLower() == "opus")
        SelectCapability_OPUS(c,tsCaps);

      if(pref_audio_cap != "" && pref_audio_cap != c.h323)
        scap = -1;
    }
    else if(c.media == 1)
    {
      if(vcap >= 0) continue;
      if(pref_video_cap == "")
      {
        if(c.format.ToLower() == "h261") SelectCapability_H261(c,tvCaps);
        else if(c.format.ToLower() == "h263") SelectCapability_H263(c,tvCaps);
        else if(c.format.ToLower() == "h263-1998") SelectCapability_H263p(c,tvCaps);
        else if(c.format.ToLower() == "h264") SelectCapability_H264(c,tvCaps);
        else if(c.format.ToLower() == "vp8") SelectCapability_VP8(c,tvCaps);
      } else if(tvCaps.GetStringsIndex(pref_video_cap) != P_MAX_INDEX) {
        if((c.format.ToLower() == "h261" && pref_video_cap.ToLower().Find("h.261") != P_MAX_INDEX) ||
           (c.format.ToLower() == "h263" && pref_video_cap.ToLower().Find("h.263-") != P_MAX_INDEX) ||
           (c.format.ToLower() == "h263-1998" && pref_video_cap.ToLower().Find("h.263p") != P_MAX_INDEX))
        {
          H323Capability *cap = H323Capability::Create(pref_video_cap);
          if(cap)
          {
            c.cap = cap;
            c.h323 = pref_video_cap;
            vcap = c.payload;
            PString SIPName = pref_video_cap.Tokenise("-")[1].Tokenise("{sw}")[0];
            OpalMediaFormat & wf = c.cap->GetWritableMediaFormat(); 
            int mpi = 1;
            wf.SetOptionInteger(SIPName + " MPI",mpi);
            c.parm += SIPName+"="+PString(mpi);
            if(c.bandwidth) wf.SetOptionInteger("Max Bit Rate",c.bandwidth*1000);
          }
        }
        else if(c.format.ToLower() == "h264" && pref_video_cap.ToLower().Find("h.264") != P_MAX_INDEX)
        {
          H323Capability *cap = H323Capability::Create(pref_video_cap);
          if(cap)
          {
            c.cap = cap;
            c.h323 = pref_video_cap;
            c.parm = "";
            vcap = c.payload;
            OpalMediaFormat & wf = c.cap->GetWritableMediaFormat(); 
            if(c.bandwidth) wf.SetOptionInteger("Max Bit Rate",c.bandwidth*1000);
          }
        }
        else if(c.format.ToLower() == "vp8" && pref_video_cap.ToLower().Find("vp8") != P_MAX_INDEX)
        {
          H323Capability *cap = H323Capability::Create(pref_video_cap);
          if(cap)
          {
            c.cap = cap;
            c.h323 = pref_video_cap;
            c.parm = "";
            SelectCapability_VP8(c,tvCaps);
          }
        }
      }
    }
  }

  if(scap < 0 && vcap < 0)
  {
    PTRACE(1, "MCUSIP\tSDP parsing error: compatible codecs not found");
    return 0;
  }

  if(scap != -1)
  {
    SipCapMapType::iterator it = sipCaps.find(scap);
    if(it != sipCaps.end()) it->second->Print();
  }
  if(vcap != -1)
  {
    SipCapMapType::iterator it = sipCaps.find(vcap);
    if(it != sipCaps.end()) it->second->Print();
  }

  sdp_msg = "v=0\r\no=";
  sdp_msg = sdp_msg + FreeMCU::Current().GetName() + " ";
  sdp_seq++;
  sdp_msg = sdp_msg + PString(sdp_id) + " ";
  sdp_msg = sdp_msg + PString(sdp_seq);
  sdp_msg = sdp_msg + " IN IP4 ";
  sdp_msg = sdp_msg + local_ip + "\r\n";
  sdp_msg = sdp_msg + "s="+FreeMCU::Current().GetName()+"\r\n";
  sdp_msg = sdp_msg + "c=IN IP4 ";
  sdp_msg = sdp_msg + local_ip + "\r\n";
  if(bandwidth_to > 64) sdp_msg = sdp_msg + "b=AS:" + PString(bandwidth_to) + "\r\n";
  sdp_msg = sdp_msg + "t=0 0\r\n";
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnnection::CreateSipData()
{
  PTRACE(1, "MCUSIP\tCreateSipData");
  su_home_t *home = msg_home(c_sip_msg);
  sip_t *sip = sip_object(c_sip_msg);
  if(sip == NULL)
    return 0;

  if(direction == 0) //incoming
  {
    ProxyAccount *proxy = sep->FindProxyAccount((PString)sip->sip_to->a_url->url_user+"@"+(PString)sip->sip_from->a_url->url_host);
    if(proxy)
    {
      contact_t = sip_contact_create(home, (url_string_t *)(const char *)("sip:"+proxy->username+"@"+proxy->local_ip), NULL);
      local_ip = proxy->local_ip;
      roomname = proxy->roomname;
    } else {
      PString local_port = "5060";
      if(sip->sip_to->a_url->url_port) local_port = sip->sip_to->a_url->url_port;
      contact_t = sip_contact_create(home, (url_string_t *)(const char *)
	  ("sip:"+(PString)sip->sip_to->a_url->url_user+"@"+(PString)sip->sip_to->a_url->url_host+":"+local_port), NULL);
      local_ip = sip->sip_to->a_url->url_host;
      roomname = sip->sip_to->a_url->url_user;
    }
    PString url = sip_header_as_string(home, (sip_header_t const *)sip->sip_from);
    bandwidth_to = atoi(FreeMCU::Current().GetEndpointParamFromUrl("Preferred bandwidth to MCU", url));
  } else { // outgoing
    ProxyAccount *proxy = sep->FindProxyAccount((PString)sip->sip_from->a_url->url_user+"@"+(PString)sip->sip_from->a_url->url_host);
    if(proxy)
    {
      contact_t = sip_contact_create(home, (url_string_t *)(const char *)("sip:"+proxy->username+"@"+proxy->local_ip), NULL);
      local_ip = proxy->local_ip;
      roomname = proxy->roomname;
    } else {
      PString local_port = "5060";
      if(sip->sip_from->a_url->url_port) local_port = sip->sip_from->a_url->url_port;
      contact_t = sip_contact_create(home, (url_string_t *)(const char *)
	  ("sip:"+(PString)sip->sip_from->a_url->url_user+"@"+(PString)sip->sip_from->a_url->url_host+":"+local_port), NULL);
      local_ip = sip->sip_from->a_url->url_host;
      roomname = sip->sip_from->a_url->url_user;
    }
    PString url = sip_header_as_string(home, (sip_header_t const *)sip->sip_to);
    bandwidth_to = atoi(FreeMCU::Current().GetEndpointParamFromUrl("Preferred bandwidth to MCU", url));
  }
  cseqNum = sip->sip_cseq->cs_seq+1;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnnection::ProcessInviteEvent()
{
  PTRACE(1, "MCUSIP\tProcessInviteEvent");
  su_home_t *home = msg_home(c_sip_msg);
  sip_t *sip = sip_object(c_sip_msg);
  if(sip == NULL || sip->sip_payload == NULL || sip->sip_payload->pl_data == NULL)
    return 415; // SIP_415_UNSUPPORTED_MEDIA

  sdp_s = sip->sip_payload->pl_data;
  PStringArray sdp_sa = sdp_s.Lines();

  if(CreateSipData() != 1)
    return 500; // SIP_500_INTERNAL_SERVER_ERROR

  sip_addr_t *remote_addr_t;
  if(direction == 0) remote_addr_t = sip_from_dup(home, sip->sip_from);
  else remote_addr_t = sip_to_dup(home, sip->sip_to);

  if(sip->sip_contact && sip->sip_contact->m_display && strcmp(sip->sip_contact->m_display, "") != 0)
    remoteName = sip->sip_contact->m_display;
  else if(remote_addr_t->a_display && strcmp(remote_addr_t->a_display, "") != 0)
    remoteName = remote_addr_t->a_display;
  else
    remoteName = remote_addr_t->a_url->url_user;

  remoteName.Replace("\"","",TRUE,0);
  remoteName = PURL::UntranslateString(remoteName, PURL::QueryTranslation);
  remotePartyName = remoteName;

  remotePartyAddress = sep->CreateRuriStr(c_sip_msg, direction);
  remotePartyAddress = PURL::UntranslateString(remotePartyAddress, PURL::QueryTranslation);

  if(sip->sip_user_agent && sip->sip_user_agent->g_string)
    remoteApplication = sip->sip_user_agent->g_string;
  else if(sip->sip_server && sip->sip_server->g_string)
    remoteApplication = sip->sip_server->g_string;

  // endpoint display name override
  PString override_name = GetEndpointParam("Display name override");
  if(override_name != "")
  {
    PTRACE(1, "MCUSipConnnection\tSet endpoint display name override: " << override_name);
    remotePartyName = override_name;
    remoteName = override_name;
  }

  // set endpoint member name
  SetMemberName();

  // endpoint rtp proto
  if(rtp_proto == "") // for incoming, outgoing set in CreateSdpInvite
  {
    rtp_proto = GetEndpointParam("RTP proto");
    if(rtp_proto == "") rtp_proto = "RTP";
  }

  // endpoint custom capability
  pref_audio_cap = GetEndpointParam("Audio codec");
  pref_video_cap = GetEndpointParam("Video codec");
  if(pref_audio_cap != "" && pref_audio_cap.Find("{sw}") == P_MAX_INDEX)
    pref_audio_cap += "{sw}";
  if(pref_audio_cap != "") { PTRACE(1, "MCUSipConnnection\tSet endpoint custom audio: " << pref_audio_cap); }
  if(pref_video_cap != "") { PTRACE(1, "MCUSipConnnection\tSet endpoint custom video: " << pref_video_cap); }

  PString sdp_txt = sip->sip_payload->pl_data;
  if(!ProcessSDP(sdp_txt, sipCaps, 0))
    return 415; // SIP_415_UNSUPPORTED_MEDIA

  // join conference
  requestedRoom = roomname;
  connectionState = EstablishedConnection;
  OnEstablished();
  if(!conference || !conferenceMember || (conferenceMember && !conferenceMember->IsJoined()))
    return 600;

  // create logical channels
  CreateLogicalChannels();
//  if(direction == 1) // for incoming connection start channels after ACK
//  {
    StartReceiveChannels(); // start receive logical channels
    StartTransmitChannels(); // start transmit logical channels
//  }

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnnection::ProcessReInviteEvent()
{
  PTRACE(1, "MCUSIP\tProcessReInviteEvent");
  sip_t *sip = sip_object(c_sip_msg);
  if(sip == NULL || sip->sip_payload == NULL || sip->sip_payload->pl_data == NULL)
    return 415; // SIP_415_UNSUPPORTED_MEDIA

  PString sdp_txt = sip->sip_payload->pl_data;
  SipCapMapType new_caps;

  int cur_scap = scap;
  int cur_vcap = vcap;
  PString cur_sdp_msg = sdp_msg;

  if(!ProcessSDP(sdp_txt, new_caps, 1))
    return 415; // SIP_415_UNSUPPORTED_MEDIA

  int sflag = 1; // 0 - no changes
  //cout << "scap: " << scap << " cur_scap: " << cur_scap << "\n";
  if(scap >= 0 && cur_scap >= 0)
  {
    SipCapMapType::iterator cir = sipCaps.find(cur_scap);
    if(cir == sipCaps.end()) return 500; // SIP_500_INTERNAL_SERVER_ERROR
    SipCapability *cur_sc = cir->second;
    cir = new_caps.find(scap);
    SipCapability *new_sc = cir->second;
    sflag = new_sc->CmpSipCaps(*cur_sc);
    if(!sflag) sdp_msg += new_sc->sdp;
  }
  else if(scap < 0 && cur_scap < 0) sflag = 0;
  if(sflag && cur_scap>=0)
  {
    StopChannel(cur_scap,1);
    StopChannel(cur_scap,0);
    DeleteMediaChannels(cur_scap);
  }

  int vflag = 1; // 0 - no changes
  if(vcap >= 0 && cur_vcap >= 0)
  {
    SipCapMapType::iterator cir = sipCaps.find(cur_vcap);
    if(cir == sipCaps.end()) return 500; // SIP_500_INTERNAL_SERVER_ERROR
    SipCapability *cur_sc = cir->second;
    cir = new_caps.find(vcap);
    SipCapability *new_sc = cir->second;
    vflag = new_sc->CmpSipCaps(*cur_sc);
    if(!vflag) sdp_msg += new_sc->sdp;
  }
  else if(vcap < 0 && cur_vcap < 0)
  {
    vflag = 0;
  }

  if(vflag && cur_vcap>=0)
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

  sipCaps = new_caps;

  if(sflag && scap>=0)
  {
    CreateAudioChannel(scap,0);
    CreateAudioChannel(scap,1);
  }

  if(vflag && vcap>=0)
  {
    CreateVideoChannel(vcap,0);
    CreateVideoChannel(vcap,1);
  }
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnnection::SendRequest(sip_method_t method, const char *method_name, msg_t *req_msg=NULL)
{
  PTRACE(1, "MCUSIP\tSendRequest");
  sip_t *sip = sip_object(c_sip_msg);
  su_home_t *home = msg_home(c_sip_msg);
  if(!sip || !home) return 0;

  // Send request
  sip_addr_t *sip_from, *sip_to;
  if(direction == 0)
  {
    sip_from = sip_to_dup(home, sip->sip_to);
    sip_to = sip_from_dup(home, sip->sip_from);
  } else {
    sip_from = sip_from_dup(home, sip->sip_from);
    sip_to = sip_to_dup(home, sip->sip_to);
  }
  PString ruri_str = sep->CreateRuriStr(c_sip_msg, direction);
  url_string_t *ruri = (url_string_t *)(const char *)ruri_str;

  sip_request_t *sip_rq = sip_request_create(home, method, method_name, ruri, NULL);
  sip_cseq_t *sip_cseq = sip_cseq_create(home, cseqNum++, method, method_name);
  sip_route_t* sip_route = sip_route_reverse(home, sip->sip_record_route);

  if(req_msg == NULL)
    req_msg = nta_msg_create(sep->GetAgent(), 0);
  nta_outgoing_t *a_orq = nta_outgoing_mcreate(sep->GetAgent(), NULL, NULL,
			ruri,
			req_msg,
			NTATAG_STATELESS(1),
 			SIPTAG_REQUEST(sip_rq),
			SIPTAG_ROUTE(sip_route),
                        SIPTAG_CONTACT(contact_t),
			SIPTAG_FROM(sip_from),
			SIPTAG_TO(sip_to),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip->sip_call_id),
			SIPTAG_SERVER_STR((const char*)(MCUSIP_USER_AGENT_STR)),
			TAG_END());
  if(a_orq == NULL)
    return 0;
  nta_outgoing_destroy(a_orq);
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnnection::SendBYE()
{
  PTRACE(1, "MCUSIP\tSendBYE");
  return SendRequest(SIP_METHOD_BYE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipConnnection::SendFastUpdatePicture()
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

void MCUSipConnnection::LeaveMCU()
{
  PString *bye = new PString("bye:"+callToken);
  sep->SipQueue.Push(bye);
  LeaveMCU(FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipConnnection::LeaveMCU(BOOL remove)
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
    if(agent == NULL) return NULL;

    MCUURL url_to(to);
    if(url_to.GetHostName() == "") return NULL;
    PString remote_user = url_to.GetUserName();
    PString remote_domain = url_to.GetHostName();
    PString remote_host = remote_domain;
    PString remote_port = url_to.GetPort();
    PString remote_transport = "transport="+url_to.GetSipProto();

    if(from.Left(4) != "sip:") from = "sip:"+from;
    if(from.Find("@") == P_MAX_INDEX) from = from+"@";
    MCUURL url_from(from);
    PString local_user = url_from.GetUserName();
    PString local_domain = url_from.GetHostName();
    PString local_dname = url_from.GetDisplayName();
    PString local_ip;
    ProxyAccount *proxy = NULL;
    ProxyAccountMapType::iterator it;
    for(it=ProxyAccountMap.begin(); it!=ProxyAccountMap.end(); it++)
    {
      proxy = it->second;
      if((proxy->domain == remote_domain || proxy->host == remote_host) && proxy->roomname == local_user && proxy->enable == 1)
      {
        local_user = proxy->username;
        local_domain = proxy->domain;
        local_dname = proxy->roomname;
        local_ip = proxy->local_ip;
        remote_host = proxy->host;
        remote_port = proxy->port;
        break;
      } else {
        proxy = NULL;
      }
    }

    PString ep_port = FreeMCU::Current().GetEndpointParamFromUrl("Port", "sip:"+remote_user+"@"+remote_domain);
    if(ep_port != "") remote_port = ep_port;

    PString ep_transport = FreeMCU::Current().GetEndpointParamFromUrl("Transport", "sip:"+remote_user+"@"+remote_domain);
    if(ep_transport != "") remote_transport = "transport="+ep_transport;
    if(remote_transport == "transport=tls" && remote_port == "5060")
      remote_port = "5061";

    if(local_ip == "")
    {
      local_ip = GetFromIp(remote_host, remote_port);
      if(local_ip == "") return NULL;
    }

    if(local_domain == "")
      local_domain = local_ip;
    if(local_dname == "")
      local_dname = local_user;
    if(FindListener(local_ip) == FALSE)
      return NULL;

    // ruri
    PString ruri_str = "sip:"+remote_user+"@"+remote_host+":"+remote_port+";"+remote_transport;
    url_string_t *ruri = (url_string_t *)(const char *)ruri_str;

    sip_addr_t *sip_from = sip_from_create(&home, (url_string_t *)(const char *)
	("sip:"+local_user+"@"+local_domain));
    sip_from->a_display = local_dname;
    sip_from_tag(&home, sip_from, nta_agent_newtag(&home, "tag=%s", agent));

    sip_addr_t *sip_to = sip_to_create(&home, (url_string_t *)(const char *)
	("sip:"+remote_user+"@"+remote_domain));

    sip_contact_t *sip_contact = sip_contact_create(&home, (url_string_t *)(const char *)
	("sip:"+local_user+"@"+local_ip), NULL);
    sip_contact->m_display = local_dname;

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

    // temporarily invite data
    PString callToken = "sip:"+PString(sip_to->a_url->url_user)+":"+PString(sip_call_id->i_id);
    MCUSipConnnection *sCon = FindConnectionWithoutLock(callToken);
    if(!sCon)
    {
      sCon = new MCUSipConnnection(this, ep, callToken);
      sCon->direction = 1;
      PString local_url = "sip:"+local_user+"@"+local_ip;
      PString remote_url = "sip:"+remote_user+"@"+remote_domain;
      CreateSdpInvite(sCon, local_url, remote_url);
    }

    sip_payload_t *sip_payload = sip_payload_make(&home, (const char *)sCon->invite_sdp_txt);
    sip_authorization_t *sip_proxy_auth = NULL;
    if(proxy && proxy->sip_proxy_str != "")
      sip_proxy_auth = sip_proxy_authorization_make(&home, proxy->sip_proxy_str);

    msg_t *sip_msg = nta_msg_create(agent, 0);
    nta_outgoing_t *orq = nta_outgoing_mcreate(agent, nta_response_cb1_wrap, (nta_outgoing_magic_t *)this,
			ruri,
			sip_msg,
			SIPTAG_REQUEST(sip_rq),
			SIPTAG_FROM(sip_from),
			SIPTAG_TO(sip_to),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip_call_id),
			SIPTAG_CONTACT(sip_contact),
			SIPTAG_PROXY_AUTHORIZATION(sip_proxy_auth),
			SIPTAG_PAYLOAD(sip_payload),
			SIPTAG_CONTENT_TYPE_STR("application/sdp"),
			SIPTAG_SERVER_STR(MCUSIP_USER_AGENT_STR),
			TAG_END());
    if(orq == NULL)
      sCon->LeaveMCU(TRUE);

    return orq;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::SipRegister(ProxyAccount *proxy, int unregister = 0)
{
    PTRACE(1, "MCUSIP\tSipRegister");
    if(agent == NULL)
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
	("sip:"+proxy->username+"@"+proxy->local_ip), NULL);
    sip_contact->m_display = proxy->roomname;

    sip_request_t *sip_rq = sip_request_create(&home, SIP_METHOD_REGISTER, ruri, NULL);
    sip_cseq_t *sip_cseq = sip_cseq_create(&home, 100, SIP_METHOD_REGISTER);

    sip_call_id_t* sip_call_id = NULL;
    if(proxy->call_id != "")
    {
      sip_call_id = sip_call_id_make(&home, proxy->call_id);
      proxy->call_id = "";
    } else {
      sip_call_id = sip_call_id_create(&home, "0");
    }

    PString expires;
    if(unregister)
      expires = "0";
    else
      expires = proxy->expires;

    sip_authorization_t *sip_auth=NULL;
    if(proxy->sip_www_str != "")
      sip_auth = sip_authorization_make(&home, proxy->sip_www_str);

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
			SIPTAG_AUTHORIZATION(sip_auth),
			SIPTAG_EXPIRES_STR((const char*)expires),
			SIPTAG_ALLOW_STR("INVITE, ACK, BYE, CANCEL, OPTIONS, SUBSCRIBE, INFO"),
			SIPTAG_SERVER_STR((const char*)(MCUSIP_USER_AGENT_STR)),
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
  if(sip->sip_status == NULL) return 0;
  msg_t *msg = nta_outgoing_getresponse(orq);
  if(msg == NULL) return 0;

  int status = 0, cseq = 0;
  if(sip->sip_status)  status = sip->sip_status->st_status;
  if(sip->sip_cseq)    cseq = sip->sip_cseq->cs_method;
  PString callToken = "sip:"+PString(sip->sip_to->a_url->url_user)+":"+PString(sip->sip_call_id->i_id);

  if(cseq == sip_method_register && (status == 401 || status == 407))
  {
    ProxyAccount *proxy = FindProxyAccount((PString)sip->sip_from->a_url->url_user+"@"+(PString)sip->sip_from->a_url->url_host);
    if(!proxy)
      return 0;
    MakeProxyAuth(proxy, sip);

    proxy->call_id = sip->sip_call_id->i_id;
    SipRegister(proxy);
    nta_outgoing_destroy(orq);
    return 0;
  }
  if(cseq == sip_method_invite && (status == 401 || status == 407))
  {
    ProxyAccount *proxy = FindProxyAccount((PString)sip->sip_from->a_url->url_user+"@"+(PString)sip->sip_from->a_url->url_host);
    if(!proxy)
      return 0;
    MakeProxyAuth(proxy, sip);

    MCUSipConnnection *sCon = FindConnectionWithoutLock(callToken);
    if(!sCon) // connection not exist
      return 0;

    sip_addr_t *sip_to = sip_to_create(&home, (url_string_t *)sip->sip_to->a_url);
    sip_contact_t *sip_contact = sip_contact_create(&home, (url_string_t *)(const char *)
  	  ("sip:"+proxy->username+"@"+proxy->local_ip), NULL);
    sip_contact->m_display = proxy->roomname;

    sip_request_t *sip_rq = sip_request_create(&home, sip->sip_cseq->cs_method,
		                               sip->sip_cseq->cs_method_name, (url_string_t *)sip_to->a_url, NULL);
    sip_cseq_t *sip_cseq = sip_cseq_create(&home, sip->sip_cseq->cs_seq+1,
			                       sip->sip_cseq->cs_method, sip->sip_cseq->cs_method_name);

    // temp invite data
    sip_payload_t *sip_payload = sip_payload_make(&home, (const char *)sCon->invite_sdp_txt);

    sip_authorization_t *sip_auth=NULL, *sip_proxy_auth=NULL;
    if(status == 401)
      sip_auth = sip_authorization_make(&home, proxy->sip_www_str);
    else if(status == 407)
      sip_proxy_auth = sip_proxy_authorization_make(&home, proxy->sip_proxy_str);

    msg_t *sip_msg = nta_msg_create(agent, 0);
    nta_outgoing_t *a_orq = nta_outgoing_mcreate(agent, nta_response_cb1_wrap, (nta_outgoing_magic_t *)this,
			(url_string_t *)sip->sip_to->a_url,
			sip_msg,
			SIPTAG_REQUEST(sip_rq),
			SIPTAG_FROM(sip->sip_from),
			SIPTAG_TO(sip_to),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip->sip_call_id),
			SIPTAG_CONTACT(sip_contact),
			SIPTAG_AUTHORIZATION(sip_auth),
			SIPTAG_PROXY_AUTHORIZATION(sip_proxy_auth),
			SIPTAG_PAYLOAD(sip_payload),
			SIPTAG_CONTENT_TYPE_STR("application/sdp"),
			SIPTAG_ALLOW_STR("INVITE, ACK, BYE, CANCEL, OPTIONS, SUBSCRIBE, INFO"),
			SIPTAG_SERVER_STR((const char*)(MCUSIP_USER_AGENT_STR)),
			TAG_END());
    if(a_orq == NULL)
      sCon->LeaveMCU(TRUE);

    nta_outgoing_destroy(orq);
    return 0;
  }
  if(cseq == sip_method_register && status == 200)
  {
    ProxyAccount *proxy = FindProxyAccount((PString)sip->sip_from->a_url->url_user+"@"+(PString)sip->sip_from->a_url->url_host);
    if(!proxy)
      return 0;
    proxy->registered = TRUE;
  }
  if(cseq == sip_method_invite && status == 200)
  {
    SendACK(msg);

    CreateOutgoingConnection(msg);

    nta_outgoing_destroy(orq);
    return 0;
  }
  if(status >= 300)
  {
    MCUSipConnnection *sCon = FindConnectionWithoutLock(callToken);
    if(sCon)
    {
      sCon->LeaveMCU(TRUE);
    }
    nta_outgoing_destroy(orq);
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::SendACK(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);
  if(sip == NULL) return 0;

  PString ruri_str = CreateRuriStr(msg, 1);
  url_string_t *ruri = (url_string_t *)(const char *)ruri_str;

  sip_request_t *sip_rq = sip_request_create(&home, SIP_METHOD_ACK, ruri, NULL);
  sip_cseq_t *sip_cseq = sip_cseq_create(&home, sip->sip_cseq->cs_seq, SIP_METHOD_ACK);
  sip_route_t* sip_route = sip_route_reverse(&home, sip->sip_record_route);
  msg_t *sip_msg = nta_msg_create(agent, 0);
  nta_outgoing_mcreate(agent, NULL, NULL,
			ruri,
			sip_msg,
			NTATAG_STATELESS(1),
			SIPTAG_REQUEST(sip_rq),
			SIPTAG_ROUTE(sip_route),
			SIPTAG_FROM(sip->sip_from),
			SIPTAG_TO(sip->sip_to),
			SIPTAG_CSEQ(sip_cseq),
			SIPTAG_CALL_ID(sip->sip_call_id),
			SIPTAG_SERVER_STR((const char*)(MCUSIP_USER_AGENT_STR)),
			TAG_END());
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::ReqReply(const msg_t *msg, unsigned method, const char *method_name, MCUSipConnnection *sCon)
{
  PTRACE(1, "MCUSIP\tReqReply");
  sip_t *sip = sip_object(msg);
  if(sip == NULL || sip->sip_request == NULL) return 0;

  if(method_name == NULL)
    method_name = sip_status_phrase(method);

  sip_contact_t *sip_contact = NULL;
  const char *content_str = NULL;
  sip_payload_t *sip_payload = NULL;
  if(sCon != NULL)
  {
    content_str = "application/sdp";
    sip_payload = sip_payload_make(&home, (const char *)sCon->sdp_msg);
    sip_contact = sCon->contact_t;
  }

  msg_t *msg_reply = msg_dup(msg);
  nta_msg_treply(agent, msg_reply, method, method_name,
                   SIPTAG_CONTACT(sip_contact),
                   SIPTAG_CONTENT_TYPE_STR(content_str),
                   SIPTAG_PAYLOAD(sip_payload),
                   SIPTAG_ALLOW_STR("INVITE, ACK, BYE, CANCEL, OPTIONS, INFO"),
                   SIPTAG_SERVER_STR((const char*)(MCUSIP_USER_AGENT_STR)),
                   TAG_END());
  return 0;
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
  if(!sip) return 0;

  if(sip->sip_payload==NULL || sip->sip_payload->pl_data==NULL)
    return 0;

  PString callToken = "sip:"+PString(sip->sip_to->a_url->url_user)+":"+PString(sip->sip_call_id->i_id);

  MCUSipConnnection *sCon = FindConnectionWithoutLock(callToken);
  if(!sCon || (sCon && sCon->IsConnected())) // repeated OK
    return 0;

  sCon->c_sip_msg = msg_dup(msg);
  sCon->DeleteTempSockets();

  int ret = sCon->ProcessInviteEvent();
  if(ret != 1)
  {
    sCon->LeaveMCU(TRUE); // leave conference and delete connection
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::CreateIncomingConnection(const msg_t *msg)
{
  PTRACE(1, "MCUSIP\tCreateIncomingConnection");
  sip_t *sip = sip_object(msg);
  if(sip == NULL) return 0;

  if(sip->sip_payload==NULL || (sip->sip_payload!=NULL && sip->sip_payload->pl_data==NULL))
    return ReqReply(msg, SIP_415_UNSUPPORTED_MEDIA);

  if(GetRoomAccess(sip) == "DENY")
    return ReqReply(msg, SIP_403_FORBIDDEN);

  PString callToken = "sip:"+PString(sip->sip_from->a_url->url_user)+":"+PString(sip->sip_call_id->i_id);

  MCUSipConnnection *sCon = FindConnectionWithoutLock(callToken);
  if(!sCon)
    return 0;

  if(sCon->IsConnected())  // connection already exist, process reinvite
  {
    PTRACE(1, "MCUSIP\tSIP REINVITE");
    sCon->direction = 0;
    sCon->c_sip_msg = msg_dup(msg);
    int ret = sCon->ProcessReInviteEvent();
    if(ret != 1)
      return ReqReply(msg, ret);
    ReqReply(msg, SIP_200_OK, sCon);
    sCon->StartReceiveChannels(); // start receive logical channels
    sCon->StartTransmitChannels(); // start transmit logical channels
    return 0;
  }

  PTRACE(1, "MCUSIP\tSIP INVITE");

  int ret = sCon->ProcessInviteEvent();
  if(ret != 1)
  {
    ReqReply(msg, ret);
    sCon->LeaveMCU(TRUE); // leave conference and delete connection
    return 0;
  }
  ReqReply(msg, SIP_200_OK, sCon);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUSipEndPoint::ProcessSipEvent_cb(nta_agent_t *agent, msg_t *msg, sip_t *sip)
{
  if(terminating || restart)
    return 0;

  int request = 0, status = 0, cseq = 0;
  if(sip->sip_request) request = sip->sip_request->rq_method;
  if(sip->sip_status)  status = sip->sip_status->st_status;
  if(sip->sip_cseq)    cseq = sip->sip_cseq->cs_method;

  PString callToken = "sip:"+PString(sip->sip_from->a_url->url_user)+":"+PString(sip->sip_call_id->i_id);
  MCUSipConnnection *sCon = FindConnectionWithoutLock(callToken);

  Registrar *registrar = FreeMCU::Current().GetRegistrar();
  // add new incoming connection
  if(!sCon && request == sip_method_invite)
  {
    sCon = new MCUSipConnnection(this, ep, callToken);
    sCon->direction = 0;
    sCon->c_sip_msg = msg_dup(msg);
    sCon = NULL;
  }
  if(!sCon || (sCon && !sCon->IsEstablished()))
  {
    if(registrar->OnIncomingMsg(msg))
      return 0;
  }
/*
  // add new incoming connection
  if(!sCon && request == sip_method_invite)
  {
    sCon = new MCUSipConnnection(this, ep, callToken);
    sCon->direction = 0;
    sCon->c_sip_msg = msg_dup(msg);
    InsertSipConn(callToken, sCon);
  }
*/
  // repeated OK, must be processed in the invite callback
  if(cseq == sip_method_invite && status == 200)
  {
    SendACK(msg);
    return 0;
  }

  if(request == sip_method_invite)
  {
    return CreateIncomingConnection(msg);
  }
  if(request == sip_method_ack)
  {
    if(!sCon)
      return ReqReply(msg, SIP_200_OK); // SIP_481_NO_TRANSACTION
    PTRACE(1, "MCUSIP\tNew SIP ACK accepted");
    // replace to_tag
    sip_t *c_sip = sip_object(sCon->c_sip_msg);
    msg_header_insert(sCon->c_sip_msg, (msg_pub_t *)c_sip, (msg_header_t *)sip->sip_to);
    //sCon->StartReceiveChannels(); // start receive logical channels
    //sCon->StartTransmitChannels(); // start transmit logical channels
    return 0;
  }
  if(request == sip_method_bye)
  {
    if(!sCon)
      return ReqReply(msg, SIP_200_OK);
    sCon->LeaveMCU(TRUE); // leave conference and delete connection
    return ReqReply(msg, SIP_200_OK);
  }
  if(request == sip_method_cancel)
  {
    if(!sCon)
      return ReqReply(msg, SIP_200_OK);
    sCon->LeaveMCU(TRUE); // leave conference and delete connection
    return ReqReply(msg, SIP_200_OK);
  }
  if(request == sip_method_info)
  {
    if(!sCon)
      return ReqReply(msg, SIP_200_OK);
    if(sip->sip_payload && sip->sip_payload->pl_data && sip->sip_content_type)
    {
      PString type = PString(sip->sip_content_type->c_type);
      PString data = PString(sip->sip_payload->pl_data);
      if(type.Find("application/media_control") != P_MAX_INDEX && data.Find("to_encoder") != P_MAX_INDEX && data.Find("picture_fast_update") != P_MAX_INDEX)
        sCon->FastUpdatePicture();
      else if (type.Find("application/dtmf-relay") != P_MAX_INDEX)
        sCon->ReceiveDTMF(data);
      return ReqReply(msg, SIP_200_OK);
    }
    return ReqReply(msg, SIP_501_NOT_IMPLEMENTED);
  }
  if(request == sip_method_options)
  {
    return ReqReply(msg, SIP_200_OK);
  }
  if(request != 0)
  {
    return ReqReply(msg, SIP_501_NOT_IMPLEMENTED);
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
    if(proxy->enable == 1 && proxy->timeout >= atoi(proxy->expires)*2)
    {
      SipRegister(proxy);
      proxy->timeout = 0;
    }
    if(proxy->enable == 0 && proxy->registered)
    {
      proxy->registered = FALSE;
    }
    proxy->timeout++;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSipEndPoint::InitProxyAccounts()
{
  for(ProxyAccountMapType::iterator it = ProxyAccountMap.begin(); it != ProxyAccountMap.end(); )
  {
    if(it->second->enable && it->second->registered)
      SipRegister(it->second, 1);
    ProxyAccountMap.erase(it++);
  }

  PStringList keys = MCUConfig("ProxyServers").GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString tmp = MCUConfig("ProxyServers").GetString(keys[i]);
    ProxyAccount *proxy = new ProxyAccount();
    // room
    proxy->roomname = keys[i];
    // register
    if(tmp.Tokenise(",")[0] == "TRUE") proxy->enable = 1;
    else proxy->enable = 0;
    // host
    proxy->host = tmp.Tokenise(",")[1].Tokenise(":")[0];
    // port
    proxy->port = tmp.Tokenise(",")[1].Tokenise(":")[1];
    if(proxy->port == "") proxy->port = "5060";
    // username
    proxy->username = tmp.Tokenise(",")[2].Tokenise("@")[0];
    // domain
    proxy->domain = tmp.Tokenise(",")[2].Tokenise("@")[1];
    // password
    proxy->password = PHTTPPasswordField::Decrypt(tmp.Tokenise(",")[3]);
    // expires
    proxy->expires = tmp.Tokenise(",")[4];
    proxy->timeout = atoi(proxy->expires)*2;
    if(atoi(proxy->expires) < 60) proxy->expires = "60";
    if(atoi(proxy->expires) > 3600) proxy->expires = "3600";
    // host & port check
    if(proxy->domain == "") proxy->domain = proxy->host;
    if(proxy->host == "") proxy->host = proxy->domain;
    //
    if(proxy->enable == 0 || proxy->domain == "" || proxy->username == "")
      continue;
    proxy->local_ip = GetFromIp(proxy->host, proxy->port);
    if(proxy->local_ip == "" || FindListener(proxy->local_ip) == FALSE)
      continue;
    InsertProxyAccount(proxy);
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
      MCUSipConnnection *sCon = FindConnectionWithoutLock(callToken);
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
        if(it->second->enable) SipRegister(it->second, 1);
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
