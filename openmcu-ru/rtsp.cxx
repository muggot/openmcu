
#include <ptlib.h>

#include "mcu.h"

#ifdef _WIN32
  //fcntl.h
# define FD_CLOEXEC     1       /* posix */
# define F_DUPFD        0       /* Duplicate fildes */
# define F_GETFD        1       /* Get fildes flags (close on exec) */
# define F_SETFD        2       /* Set fildes flags (close on exec) */
# define F_GETFL        3       /* Get file flags */
# define F_SETFL        4       /* Set file flags */
# define O_NONBLOCK     0x4000
  inline int fcntl (int, int, ...) {return -1;}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL PreParseMsg(PString & msg_str)
{
  if(msg_str.Find("Cseq:") != P_MAX_INDEX)
    msg_str.Replace("Cseq:","CSeq:",TRUE,0);

  if(msg_str.Find("CSeq:") == P_MAX_INDEX)
  {
    MCUTRACE(1, "RTSP pre-parse: CSeq header not found");
    return FALSE;
  }

  // create not empty CSeq method name
  PString method_name = "EMPTY";
  PString cseq;
  for(PINDEX i = msg_str.Find("CSeq:")+6; i < msg_str.GetLength(); i++)
  {
    if(msg_str[i] == ' ' || msg_str[i] == '\r')
      break;
    cseq += msg_str[i];
  }
  if(cseq == "")
  {
    MCUTRACE(1, "RTSP pre-parse: CSeq number not found");
    return FALSE;
  }
  msg_str.Replace("CSeq: "+cseq, "CSeq: "+cseq+" "+method_name, TRUE, 0);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

msg_t * ParseMsg(PString & msg_str)
{
  if(PreParseMsg(msg_str) == FALSE)
    return NULL;

  msg_t *msg = msg_make(sip_default_mclass(), 0, (const void *)(const char *)msg_str, msg_str.GetLength());
  sip_t *sip = sip_object(msg);
  if(sip == NULL || sip->sip_cseq == NULL)
  {
    MCUTRACE(1, "RTSP parse: failed parse rtsp message");
    goto error;
  }
  if(sip->sip_request == NULL && sip->sip_status == NULL)
  {
    MCUTRACE(1, "RTSP parse: failed parse rtsp message");
    goto error;
  }

  return msg;

  error:
    msg_destroy(msg);
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCURtspConnection::MCURtspConnection(MCUSipEndPoint *_sep, MCUH323EndPoint *_ep, PString _callToken)
  :MCUSipConnection(_sep, _ep, _callToken)
{
  connectionType = CONNECTION_TYPE_RTSP;
  trace_section = "RTSP Connection "+callToken+": ";
  remoteApplication = "RTSP terminal";

  rtsp_state = RTSP_NONE;
  cseq = 1;
  listener = NULL;

  // create local capability list
  CreateLocalSipCaps();

  PTRACE(1, trace_section << "constructor");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCURtspConnection::~MCURtspConnection()
{
  PTRACE(1, trace_section << "destructor");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspConnection::ProcessShutdown(CallEndReason reason)
{
  if(connectionState == ShuttingDownConnection)
  {
    MCUTRACE(1, trace_section << "shutdown connection process is already running");
    return;
  }
  connectionState = ShuttingDownConnection;

  MCUTRACE(1, trace_section << "shutdown connection");
  callEndReason = reason;
  // leave conference and delete connection
  MCUH323Connection::LeaveMCU();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspConnection::LeaveMCU()
{
  if(connectionState == ShuttingDownConnection)
  {
    MCUTRACE(1, trace_section << "shutdown connection process is already running");
    return;
  }
  connectionState = ShuttingDownConnection;

  PTRACE(1, trace_section << "LeaveMCU");
  MCUH323Connection::LeaveMCU();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspConnection::CleanUpOnCallEnd()
{
  PTRACE(1, trace_section << "CleanUpOnCallEnd reason: " << callEndReason);

  connectionState = ShuttingDownConnection;

  if(direction == DIRECTION_OUTBOUND && callEndReason == EndedByLocalUser && rtsp_state == RTSP_PLAYING)
    SendTeardown();

  if(listener)
    delete listener;

  StopTransmitChannels();
  StopReceiveChannels();
  DeleteChannels();
  videoReceiveCodecName = videoTransmitCodecName = "none";
  videoReceiveCodec = NULL;
  videoTransmitCodec = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCURtspConnection::Connect(PString room, PString address)
{
  direction = DIRECTION_OUTBOUND;

  // requested room
  requestedRoom = room;

  // rtsp address (remote)
  address.Replace("rtsp:","http:",TRUE,0);
  MCUURL url(address);
  PString rtsp_port = url.GetPort();
  if(rtsp_port == "80") rtsp_port = "554";
  ruri_str = "rtsp://"+url.GetHostName()+":"+rtsp_port+url.GetPathStr();
  remotePartyAddress = ruri_str;

  // listener address
  PString listener_address = "tcp:"+url.GetHostName()+":"+rtsp_port;
  trace_section.Replace(":"," ("+listener_address+"):",TRUE,0);

  // detect local_ip, nat_ip and create rtp sessions
  if(CreateDefaultRTPSessions() == FALSE)
    return FALSE;

  // display name
  remoteName = GetEndpointParam(DisplayNameKey, url.GetPathStr());
  remotePartyName = remoteName;

  // auth
  auth_username = GetEndpointParam(UserNameKey, url.GetUserName());
  auth_password = GetEndpointParam(PasswordKey, url.GetPassword());

  // create listener
  listener = MCUListener::Create(listener_address, OnReceived_wrap, this);
  if(listener == NULL)
    return FALSE;

  // start connection
  if(SendDescribe() == 0)
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCURtspConnection::Connect(PString address, int socket_fd, const msg_t *msg)
{
  direction = DIRECTION_INBOUND;

  // remote address proto:host:port
  ruri_str = address;

  sip_t *sip = sip_object(msg);

  // rtsp address (local)
  luri_str = url_as_string(msg_home(msg), sip->sip_request->rq_url);
  MCUURL lurl(luri_str);
  rtsp_path = lurl.GetPath()[0];

  // used in GetEndpointParam
  remotePartyAddress = "RTSP Server "+rtsp_path;

  // requested room
  requestedRoom = GetEndpointParam(RoomNameKey);

  // detect local_ip, nat_ip and create rtp sessions
  if(CreateDefaultRTPSessions() == FALSE)
    return FALSE;

  // random session string
  rtsp_session_str = PString(random());

  // create remote caps, for RTP channels
  for(SipCapMapType::iterator it = LocalSipCaps.begin(); it != LocalSipCaps.end(); ++it)
  {
    SipCapability *local_sc = it->second;
    RemoteSipCaps.insert(SipCapMapType::value_type(it->first, new SipCapability(*local_sc)));
  }

  // auth
  auth_username = GetEndpointParam(UserNameKey);
  auth_password = GetEndpointParam(PasswordKey);
  if(auth_password != "")
  {
    auth_type = AUTH_WWW;
    auth_scheme = "Digest";
    auth_realm = "HONEYNET-MCU";
    auth_nonce = PGloballyUniqueID().AsString();
  }

  // set remote application
  if(sip->sip_user_agent)
    remoteApplication = sip->sip_user_agent->g_string;
  else if(sip->sip_server)
    remoteApplication = sip->sip_server->g_string;

  // setup codecs
  PString audio_codec = GetEndpointParam(AudioCodecKey);
  PString video_codec = GetEndpointParam(VideoCodecKey);
  PString video_resolution = GetEndpointParam(VideoResolutionKey, "352x288");
  unsigned video_bandwidth = GetEndpointParam(BandwidthFromKey, 256);
  unsigned video_frame_rate = GetEndpointParam(FrameRateFromKey, 10);

  unsigned video_width = video_resolution.Tokenise("x")[0].AsInteger();
  unsigned video_height = video_resolution.Tokenise("x")[1].AsInteger();

  // setup audio capability
  SipCapability *audio_sc = FindSipCap(RemoteSipCaps, audio_codec);
  if(audio_sc)
  {
    audio_sc->cap = H323Capability::Create(audio_codec);
    if(audio_sc->cap == NULL)
    {
      MCUTRACE(1, trace_section << "not found audio codec " << audio_codec);
      return FALSE;
    }
    if(audio_sc->payload == -1)
      audio_sc->payload = 96;
    scap = audio_sc->payload;
  }
  // setup video capability
  SipCapability *video_sc = FindSipCap(RemoteSipCaps, video_codec);
  if(video_sc)
  {
    video_sc->cap = H323Capability::Create(video_codec);
    if(video_sc->cap == NULL)
    {
      MCUTRACE(1, trace_section << "not found video codec " << video_codec);
      return FALSE;
    }
    if(video_sc->payload == -1)
      video_sc->payload = 97;
    vcap = video_sc->payload;
    video_sc->video_width = video_width;
    video_sc->video_height = video_height;
    video_sc->video_frame_rate = video_frame_rate;
    video_sc->bandwidth = video_bandwidth;
    //sc->fmtp = "profile-level-id=1;";
    video_sc->fmtp = "";
    OpalMediaFormat & wf = video_sc->cap->GetWritableMediaFormat();
    SetFormatParams(wf, video_sc->video_width, video_sc->video_height, video_sc->video_frame_rate, video_sc->bandwidth);
    //int keyint = ((video_frame_rate+4)*2/10)*10;
    //wf.SetOptionInteger(OPTION_TX_KEY_FRAME_PERIOD, keyint);
  }

  if(audio_sc == NULL && video_sc == NULL)
  {
    MCUTRACE(1, trace_section << "cannot create connection without codecs, audio: " << audio_codec << " video: " << video_codec);
    return FALSE;
  }

  // create listener
  listener = MCUListener::Create(socket_fd, ruri_str, OnReceived_wrap, this);
  if(listener == NULL)
    return FALSE;

  // start connection
  OnRequestReceived(msg);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspConnection::CreateLocalSipCaps()
{
  LocalSipCaps.clear();
  for(SipCapMapType::iterator it = sep->GetBaseSipCaps().begin(); it != sep->GetBaseSipCaps().end(); ++it)
  {
    SipCapability *base_sc = it->second;
    if(SkipCapability(base_sc->capname, connectionType))
      continue;
    LocalSipCaps.insert(SipCapMapType::value_type(it->first, new SipCapability(*base_sc)));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendOptions()
{
  char buffer[1024];
  snprintf(buffer, 1024,
  	   "OPTIONS %s RTSP/1.0\r\n"
	   "CSeq: %d %s\r\n"
	   , (const char *)ruri_str, cseq++, (const char *)METHOD_OPTIONS);

  AddHeaders(buffer, METHOD_OPTIONS);
  if(SendRequest(buffer) == 0)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendPlay()
{
  char buffer[1024];
  snprintf(buffer, 1024,
  	   "PLAY %s RTSP/1.0\r\n"
	   "CSeq: %d %s\r\n"
           "Session: %s\r\n"
           "Range: npt=0.000-\r\n"
	   , (const char *)ruri_str, cseq++, (const char *)METHOD_PLAY, (const char *)rtsp_session_str);

  AddHeaders(buffer, METHOD_PLAY);
  if(SendRequest(buffer) == 0)
    return 0;

  rtsp_state = RTSP_PLAY;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendSetup(int pt)
{
  SipCapability *sc = FindSipCap(RemoteSipCaps, pt);
  if(sc->attr.GetAt("control") == NULL)
  {
    MCUTRACE(1, trace_section << "capability attribute \"control\" not found");
    ProcessShutdown();
    return 0;
  }
  PString control = sc->attr("control");
  if(control.Left(4) != "rtsp")
    control = ruri_str+"/"+control;

  unsigned rtp_port = 0;
  if(pt == scap)
    rtp_port = audio_rtp_port;
  else
    rtp_port = video_rtp_port;

  PString session_header;
  if(rtsp_session_str != "")
    session_header = "Session: "+rtsp_session_str+"\r\n";

  char buffer[1024];
  snprintf(buffer, 1024,
  	   "SETUP %s RTSP/1.0\r\n"
	   "CSeq: %d %s\r\n"
	   "%s"
           "Transport: RTP/AVP/UDP;unicast;client_port=%d-%d\r\n"
	   , (const char *)control, cseq++, (const char *)METHOD_SETUP, (const char *)session_header, rtp_port, rtp_port+1);

  AddHeaders(buffer, METHOD_SETUP);
  if(SendRequest(buffer) == 0)
    return 0;

  if(pt == scap)
    rtsp_state = RTSP_SETUP_AUDIO;
  else
    rtsp_state = RTSP_SETUP_VIDEO;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendTeardown()
{
  char buffer[1024];
  snprintf(buffer, 1024,
  	   "TEARDOWN %s RTSP/1.0\r\n"
	   "CSeq: %d %s\r\n"
	   "Session: %s\r\n"
	   , (const char *)ruri_str, cseq++, (const char *)METHOD_TEARDOWN, (const char *)rtsp_session_str);

  AddHeaders(buffer, METHOD_TEARDOWN);
  if(SendRequest(buffer) == 0)
    return 0;

  rtsp_state = RTSP_TEARDOWN;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendDescribe()
{
  char buffer[1024];
  snprintf(buffer,1024,
  	   "DESCRIBE %s RTSP/1.0\r\n"
	   "CSeq: %d %s\r\n"
	   "Accept: application/sdp\r\n"
	   , (const char *)ruri_str, cseq++, (const char *)METHOD_DESCRIBE);

  AddHeaders(buffer, METHOD_DESCRIBE);
  if(SendRequest(buffer) == 0)
     return 0;

  rtsp_state = RTSP_DESCRIBE;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnResponsePlay(const msg_t *msg)
{
  // set endpoint member name
  SetMemberName();
  // override requested room from registrar
  SetRequestedRoom();
  // join conference
  OnEstablished();
  if(!conference || !conferenceMember || !conferenceMember->IsJoined())
  {
    MCUTRACE(1, trace_section << "error");
    return 0;
  }

  // create and start channels
  CreateMediaChannel(scap, 0);
  CreateMediaChannel(vcap, 0);
  StartReceiveChannels();

  // is connected
  connectionState = EstablishedConnection;

  if(scap > 0)
  {
    SipCapability *sc = FindSipCap(RemoteSipCaps, scap);
    MCUTRACE(1, trace_section << "audio " << sc->capname << " " << sc->remote_ip << ":" << sc->remote_port);
  }
  if(vcap > 0)
  {
    SipCapability *sc = FindSipCap(RemoteSipCaps, vcap);
    MCUTRACE(1, trace_section << "video " << sc->capname << " " << sc->remote_ip << ":" << sc->remote_port);
  }

  rtsp_state = RTSP_PLAYING;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnResponseSetup(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);

  PString transport_str;
  for(sip_unknown_t *sip_un = sip->sip_unknown; sip_un != NULL; sip_un = sip_un->un_next)
  {
    if(PString(sip_un->un_name) == "Session")
      rtsp_session_str = PString(sip_un->un_value).Tokenise(";")[0];
    if(PString(sip_un->un_name) == "Transport")
      transport_str = sip_un->un_value;
  }

  SipCapability *sc = NULL;
  if(rtsp_state == RTSP_SETUP_AUDIO)
    sc = FindSipCap(RemoteSipCaps, scap);
  else
    sc = FindSipCap(RemoteSipCaps, vcap);

  if(ParseTransportStr(sc, transport_str) == FALSE)
  {
    MCUTRACE(1, trace_section << "failed parse transport header");
    return 0;
  }

  if(rtsp_state == RTSP_SETUP_AUDIO && vcap > 0)
  {
    SendSetup(vcap);
    return 1;
  }

  SendPlay();
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnResponseDescribe(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);

  if(!sip->sip_payload || !sip->sip_payload->pl_data)
  {
    MCUTRACE(1, trace_section << "error");
    return 0;
  }

  PString sdp_str = sip->sip_payload->pl_data;
  int response_code = ProcessSDP(LocalSipCaps, RemoteSipCaps, sdp_str);
  if(response_code)
  {
    MCUTRACE(1, trace_section << "error");
    return 0;
  }

  // set remote application
  if(sip->sip_user_agent)
    remoteApplication = sip->sip_user_agent->g_string;
  else if(sip->sip_server)
    remoteApplication = sip->sip_server->g_string;

  if(scap >= 0)
    SendSetup(scap);
  else
    SendSetup(vcap);

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnRequestOptions(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);

  char buffer[1024];
  snprintf(buffer, 1024,
  	   "RTSP/1.0 200 OK\r\n"
	   "CSeq: %d %s\r\n"
	   "Date: %s\r\n"
	   "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY\r\n"
	   , sip->sip_cseq->cs_seq, sip->sip_request->rq_method_name, (const char *)PTime().AsString());

  AddHeaders(buffer);
  if(SendRequest(buffer) == 0)
     return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnRequestDescribe(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);

  char buffer_sdp[1024];
  snprintf(buffer_sdp, 1024,
           "v=0\r\n"
           "o=- 15516361289475271524 15516361289475271524 IN IP4 HONEYNET-MCU\r\n"
           "s=Unnamed\r\n"
           "i=N/A\r\n"
           "c=IN IP4 0.0.0.0\r\n"
           "t=0 0\r\n"
           "a=recvonly\r\n"
           "a=type:unicast\r\n"
           "a=charset:UTF-8\r\n"
           "a=control:%s\r\n"
           , (const char *)luri_str);

  if(scap >= 0)
  {
    SipCapability *sc = FindSipCap(RemoteSipCaps, scap);
    if(sc)
    {
      snprintf(buffer_sdp + strlen(buffer_sdp), 1024,
           "m=audio 0 RTP/AVP %d\r\n"
           "a=rtpmap:%d %s/%d%s\r\n"
           "a=control:%s\r\n"
           , sc->payload, sc->payload, (const char *)sc->format.ToUpper(), sc->clock, (const char *)(sc->params == "" ? "" : "/"+sc->params)
           , (const char *)(luri_str+"/audio"));
    }
  }

  if(vcap >= 0)
  {
    SipCapability *sc = FindSipCap(RemoteSipCaps, vcap);
    if(sc)
    {
      snprintf(buffer_sdp + strlen(buffer_sdp), 1024,
           "m=video 0 RTP/AVP %d\r\n"
           "b=AS:%d\r\n"
           "a=rtpmap:%d %s/90000\r\n"
           "a=fmtp:%d %s\r\n"
           "a=control:%s\r\n"
           , sc->payload, sc->bandwidth, sc->payload, (const char *)sc->format.ToUpper()
           , sc->payload, (const char *)sc->fmtp
           , (const char *)(luri_str+"/video"));
    }
  }

  char buffer[2048];
  snprintf(buffer, 2048,
  	   "RTSP/1.0 200 OK\r\n"
	   "CSeq: %d %s\r\n"
	   "Date: %s\r\n"
	   "Content-Type: application/sdp\r\n"
	   "Cache-Control: no-cache\r\n"
           "Content-Length: %d\r\n"
	   , sip->sip_cseq->cs_seq, sip->sip_request->rq_method_name, (const char *)PTime().AsString(), strlen(buffer_sdp)+2);

  AddHeaders(buffer);
  strcat(buffer, buffer_sdp);
  strcat(buffer, "\r\n");

  if(SendRequest(buffer) == 0)
     return 0;

  rtsp_state = RTSP_DESCRIBE;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCURtspConnection::ParseTransportStr(SipCapability *sc, PString & transport_str)
{
  if(sc == NULL)
  {
    MCUTRACE(1, trace_section << "error");
    return FALSE;
  }

  //RTP/AVP/UDP;unicast;source=192.168.1.1;client_port=5002-5003;server_port=52069-52070;ssrc=C7F3A123;mode=play
  //RTP/AVP;unicast;client_port=55986-55987
  MCUStringDictionary transport_dict(transport_str);
  PString local_ports, remote_ports;

  if(direction == DIRECTION_INBOUND)
  {
    sc->remote_ip = MCUURL(ruri_str).GetHostName();
    remote_ports = transport_dict("client_port");
    sc->remote_port = remote_ports.Tokenise("-")[0].AsInteger();
  } else {
    sc->remote_ip = transport_dict("source");
    if(sc->remote_ip == "")
      sc->remote_ip = MCUURL(ruri_str).GetHostName();
    remote_ports = transport_dict("server_port");
    sc->remote_port = remote_ports.Tokenise("-")[0].AsInteger();
  }

  if(sc->remote_ip == "" || sc->remote_ip == "0.0.0.0" || sc->remote_port == 0)
  {
    MCUTRACE(1, trace_section << "missing remote ip or remote port");
    return FALSE;
  }

  if(direction == DIRECTION_INBOUND)
  {
    if(sc->media == MEDIA_TYPE_AUDIO)
    {
      if(audio_rtp_port == 0)
      {
        MCUTRACE(1, trace_section << "error");
        return FALSE;
      }
      local_ports = PString(audio_rtp_port)+"-"+PString(audio_rtp_port+1);
    }
    else if(sc->media == MEDIA_TYPE_VIDEO)
    {
      if(video_rtp_port == 0)
      {
        MCUTRACE(1, trace_section << "error");
        return FALSE;
      }
      local_ports = PString(video_rtp_port)+"-"+PString(video_rtp_port+1);
    }
    transport_dict.Append("source", nat_ip);
    transport_dict.Append("server_port", local_ports);
    transport_str = transport_dict.AsString();
    transport_str.Replace("=;",";",TRUE,0);
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnRequestSetup(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);
  SipCapability *sc = NULL;

  PString setup_media;
  MCUURL url(url_as_string(msg_home(msg), sip->sip_request->rq_url));
  if(url.GetPath().GetSize() == 1)
    setup_media = url.GetPath()[0];
  else if(url.GetPath().GetSize() > 1)
    setup_media = url.GetPath()[1];
  else
  {
    MCUTRACE(1, trace_section << "incorrect path " << url.GetPathStr());
    return 0;
  }

  if(setup_media == "audio")
    sc = FindSipCap(RemoteSipCaps, scap);
  else if(setup_media == "video")
    sc = FindSipCap(RemoteSipCaps, vcap);
  else
  {
    MCUTRACE(1, trace_section << "unknown media " << setup_media);
    return 0;
  }

  PString transport_str;
  for(sip_unknown_t *sip_un = sip->sip_unknown; sip_un != NULL; sip_un = sip_un->un_next)
  {
    if(PString(sip_un->un_name) == "Transport")
      transport_str = sip_un->un_value;
  }

  if(ParseTransportStr(sc, transport_str) == FALSE)
  {
    MCUTRACE(1, trace_section << "failed parse transport header");
    return 0;
  }

  char buffer[1024];
  snprintf(buffer, 1024,
  	   "RTSP/1.0 200 OK\r\n"
	   "CSeq: %d %s\r\n"
	   "Date: %s\r\n"
	   "Session: %s\r\n"
	   "Transport: %s\r\n"
	   , sip->sip_cseq->cs_seq, sip->sip_request->rq_method_name, (const char *)PTime().AsString(), (const char *)rtsp_session_str, (const char *)transport_str);

  AddHeaders(buffer);
  if(SendRequest(buffer) == 0)
     return 0;

  rtsp_state = RTSP_SETUP;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnRequestPlay(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);

  // creating conference if needed
  MCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();
  ConferenceManager & manager = ((MCUH323EndPoint &)ep).GetConferenceManager();
  conference = manager.MakeAndLockConference(requestedRoom);
  conferenceMember = new ConferenceStreamMember(conference, GetCallToken(), "RTSP "+rtsp_path+" ("+ruri_str+")");
  manager.UnlockConference();

  // start rtp channels
  CreateMediaChannel(scap, 1);
  CreateMediaChannel(vcap, 1);
  StartTransmitChannels();

  // is connected
  connectionState = EstablishedConnection;

  char buffer[1024];
  snprintf(buffer, 1024,
  	   "RTSP/1.0 200 OK\r\n"
	   "CSeq: %d %s\r\n"
	   "Date: %s\r\n"
	   "Session: %s\r\n"
	   "Range: npt=0.000-\r\n"
	   , sip->sip_cseq->cs_seq, sip->sip_request->rq_method_name, (const char *)PTime().AsString(), (const char *)rtsp_session_str);

  AddHeaders(buffer);
  if(SendRequest(buffer) == 0)
     return 0;

  rtsp_state = RTSP_PLAYING;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnRequestTeardown(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);

  char buffer[1024];
  snprintf(buffer, 1024,
  	   "RTSP/1.0 200 OK\r\n"
	   "CSeq: %d %s\r\n"
	   "Date: %s\r\n"
	   "Session: %s\r\n"
	   , sip->sip_cseq->cs_seq, sip->sip_request->rq_method_name, (const char *)PTime().AsString(), (const char *)rtsp_session_str);

  AddHeaders(buffer);
  SendRequest(buffer);

  rtsp_state = RTSP_TEARDOWN;

  ProcessShutdown(EndedByRemoteUser);
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspConnection::AddHeaders(char *buffer, PString method_name)
{
  if(direction == DIRECTION_OUTBOUND && auth_type != AUTH_NONE && method_name != METHOD_OPTIONS)
  {
    PString auth_str = sep->MakeAuthStr(auth_username, auth_password, ruri_str, method_name, auth_scheme, auth_realm, auth_nonce);
    strcat(buffer, (const char *)PString("Authorization: "+auth_str+"\r\n"));
  }

  if(direction == DIRECTION_OUTBOUND)
    strcat(buffer, (const char *)PString("User-Agent: "+SIP_USER_AGENT+"\r\n"));
  else
    strcat(buffer, (const char *)PString("Server: "+SIP_USER_AGENT+"\r\n"));

  strcat(buffer, "\r\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendRequest(char *buffer)
{
  if(listener->Send(buffer) == FALSE)
  {
    ProcessShutdown();
    return 0;
  }

  MCUTRACE(1, trace_section << "send " << strlen(buffer) << " bytes\n" << buffer);
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnResponseReceived(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);
  int response_code = -1;

  int status = sip->sip_status->st_status;

  if(rtsp_state == RTSP_DESCRIBE)
  {
    if(status == 200)
    {
      response_code = OnResponseDescribe(msg);
    }
    else if(status == 401)
    {
      if(auth_type != AUTH_NONE || auth_username == "" || auth_password == "")
      {
        MCUTRACE(1, trace_section << "error");
        ProcessShutdown();
        return 0;
      }
      if(sep->ParseAuthMsg(msg, auth_type, auth_scheme, auth_realm, auth_nonce) == FALSE)
      {
        MCUTRACE(1, trace_section << "error");
        ProcessShutdown();
        return 0;
      }
      SendDescribe();
      return 0;
    }
  }
  else if(rtsp_state == RTSP_SETUP_AUDIO || rtsp_state == RTSP_SETUP_VIDEO)
  {
    if(status == 200)
    {
      response_code = OnResponseSetup(msg);
    }
  }
  else if(rtsp_state == RTSP_PLAY)
  {
    if(status == 200)
    {
      response_code = OnResponsePlay(msg);
    }
  }
  else if(rtsp_state == RTSP_TEARDOWN)
  {
    return 0;
  }

  if(response_code != 1)
  {
    if(response_code == -1)
    {
      MCUTRACE(1, trace_section << "unknown response " << status << ", state " << rtsp_state);
    }

    MCUTRACE(1, trace_section << "error processing response " << status << ", error " << response_code << ", state " << rtsp_state);
    ProcessShutdown();
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCURtspConnection::RtspCheckAuth(const msg_t *msg)
{
  if(auth_type == AUTH_NONE)
    return TRUE;

  sip_t *sip = sip_object(msg);
  if(sip->sip_authorization == NULL)
  {
    PString auth_str = auth_scheme+" realm=\""+auth_realm+"\",nonce=\""+auth_nonce+"\",algorithm=MD5";
    char buffer[1024];
    snprintf(buffer, 1024,
  	   "RTSP/1.0 401 Unauthorized\r\n"
	   "CSeq: %d %s\r\n"
	   "Date: %s\r\n"
	   "WWW-Authenticate: %s\r\n"
	   , sip->sip_cseq->cs_seq, sip->sip_request->rq_method_name, (const char *)PTime().AsString(), (const char *)auth_str);
    AddHeaders(buffer);
    SendRequest(buffer);
    return FALSE;
  }
  else
  {
    PString method_name = sip->sip_request->rq_method_name;
    PString username = msg_params_find(sip->sip_authorization->au_params, "username=");
    PString response = msg_params_find(sip->sip_authorization->au_params, "response=");
    PString uri = msg_params_find(sip->sip_authorization->au_params, "uri=");

    PString auth_str = sep->MakeAuthStr(auth_username, auth_password, uri, method_name, auth_scheme, auth_realm, auth_nonce);
    MCUStringDictionary dict(auth_str, ", ", "=");
    PString auth_response = dict("response");
    if(auth_response != response)
    {
      char buffer[1024];
      snprintf(buffer, 1024,
  	   "RTSP/1.0 403 Forbidden\r\n"
	   "CSeq: %d %s\r\n"
	   "Date: %s\r\n"
	   , sip->sip_cseq->cs_seq, sip->sip_request->rq_method_name, (const char *)PTime().AsString());
      AddHeaders(buffer);
      SendRequest(buffer);

      MCUTRACE(1, trace_section << "authorization failure");
      ProcessShutdown();
      return FALSE;
    }
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnRequestReceived(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);
  int response_code = -1;

  PString method_name = sip->sip_request->rq_method_name;

  if(method_name != METHOD_OPTIONS && RtspCheckAuth(msg) == FALSE)
  {
    return 0;
  }

  if(method_name == METHOD_OPTIONS)
  {
    response_code = OnRequestOptions(msg);
  }
  if(method_name == METHOD_DESCRIBE && rtsp_state == RTSP_NONE)
  {
    response_code = OnRequestDescribe(msg);
  }
  if(method_name == METHOD_SETUP && (rtsp_state == RTSP_DESCRIBE || rtsp_state == RTSP_SETUP))
  {
    response_code = OnRequestSetup(msg);
  }
  if(method_name == METHOD_PLAY && rtsp_state == RTSP_SETUP)
  {
    response_code = OnRequestPlay(msg);
  }
  if(method_name == METHOD_TEARDOWN)
  {
    response_code = OnRequestTeardown(msg);
  }

  if(response_code != 1)
  {
    if(response_code == -1)
    {
      MCUTRACE(1, trace_section << "unknown request " << method_name << ", state " << rtsp_state);
    }

    MCUTRACE(1, trace_section << "error processing request " << method_name << ", error " << response_code << ", state " << rtsp_state);
    ProcessShutdown();
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnReceived(int socket_fd, PString address, PString data)
{
  msg_t *msg = NULL;
  sip_t *sip = NULL;

  if(socket_fd == -1)
  {
    MCUTRACE(1, trace_section << "connection closed by remote user");
    goto error;
  }

  MCUTRACE(1, trace_section << "recv from " << address << " "  << data.GetLength() << " bytes\n" << data);

  msg = ParseMsg(data);
  if(msg == NULL)
  {
    MCUTRACE(1, trace_section << "failed parse message");
    goto error;
  }

  sip = sip_object(msg);
  if(sip->sip_content_length && sip->sip_content_length->l_length != 0 && sip->sip_payload == NULL)
  {
    MCUTRACE(1, trace_section << "failed parse message, empty payload");
    goto error;
  }

  if(sip->sip_status)
    OnResponseReceived(msg);

  if(sip->sip_request)
    OnRequestReceived(msg);

  msg_destroy(msg);
  return 1;

  error:
    msg_destroy(msg);
    ProcessShutdown(EndedByRemoteUser);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCURtspServer::MCURtspServer(MCUH323EndPoint *_ep, MCUSipEndPoint *_sep)
  :ep(_ep), sep(_sep)
{
  trace_section = "RTSP server: ";
  InitListeners();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCURtspServer::~MCURtspServer()
{
  PWaitAndSignal m(mutex);
  ClearListeners();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspServer::InitListeners()
{
  PWaitAndSignal m(mutex);
  ClearListeners();

  MCUConfig cfg("RTSP Parameters");
  if(cfg.GetBoolean(EnableKey, TRUE) == FALSE)
    return;

  PStringArray list = cfg.GetString("Listener", "0.0.0.0:1554").Tokenise(",");
  for(PINDEX i = 0; i < list.GetSize(); ++i)
  {
    if(list[i] == "") continue;
    AddListener(list[i]);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspServer::AddListener(PString address)
{
  address.Replace(" ","",TRUE,0);
  if(address.Find("tcp:") == P_MAX_INDEX)
    address = "tcp:"+address;

  MCUURL url(address);
  PString socket_host = url.GetHostName();
  unsigned socket_port = url.GetPort().AsInteger();
  if(socket_host != "0.0.0.0" && PIPSocket::Address(socket_host).IsValid() == FALSE)
  {
    MCUTRACE(1, trace_section << "incorrect listener host " << socket_host);
    return;
  }
  if(socket_host != "0.0.0.0" && PIPSocket::IsLocalHost(socket_host) == FALSE)
  {
    MCUTRACE(1, trace_section << "incorrect listener host " << socket_host << ", this is not a local address");
    return;
  }
  if(socket_port == 0)
  {
    MCUTRACE(1, trace_section << "incorrect listener port " << socket_port);
    return;
  }

  PWaitAndSignal m(mutex);
  MCUListener *listener = MCUListener::Create(address, OnReceived_wrap, this);
  if(listener)
    Listeners.insert(ListenersMapType::value_type(address, listener));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspServer::RemoveListener(PString address)
{
  address.Replace(" ","",TRUE,0);
  if(address.Find("tcp:") == P_MAX_INDEX)
    address = "tcp:"+address;

  PWaitAndSignal m(mutex);
  for(ListenersMapType::iterator it = Listeners.begin(); it != Listeners.end(); ++it)
  {
    if(address == it->first)
    {
      delete it->second;
      Listeners.erase(it);
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspServer::ClearListeners()
{
  PWaitAndSignal m(mutex);
  for(ListenersMapType::iterator it = Listeners.begin(); it != Listeners.end(); )
  {
    delete it->second;
    Listeners.erase(it++);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspServer::OnReceived(int socket_fd, PString address, PString data)
{
  MCUTRACE(1, trace_section << "read from " << address << " "  << data.GetLength() << " bytes\n" << data);

  msg_t *msg = ParseMsg(data);
  if(CreateConnection(address, socket_fd, msg) == FALSE)
  {
    msg_destroy(msg);
    return 0;
  }

  msg_destroy(msg);
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCURtspServer::CreateConnection(PString address, int socket_fd, const msg_t *msg)
{
  if(ep->HasConnection(address))
  {
    MCUTRACE(1, trace_section << address << " connection already exists ");
    SendResponse(socket_fd, address, msg, "454 Session Not Found");
    return FALSE;
  }

  if(msg == NULL)
  {
    MCUTRACE(1, trace_section << address << " failed parse message ");
    SendResponse(socket_fd, address, msg, "400 Bad Request");
    return FALSE;
  }

  sip_t *sip = sip_object(msg);
  if(sip->sip_request == NULL || sip->sip_cseq == NULL)
  {
    MCUTRACE(1, trace_section << address << " missing headers ");
    SendResponse(socket_fd, address, msg, "400 Bad Request");
    return FALSE;
  }

  PString agent;
  if(sip->sip_user_agent)
    agent = sip->sip_user_agent->g_string;
  if(agent.Find("RealMedia") != P_MAX_INDEX)
  {
    MCUTRACE(1, trace_section << address << " RealRTSP is not supported ");
    SendResponse(socket_fd, address, msg, "505 RTSP Version not supported");
    return FALSE;
  }

  PString method_name = sip->sip_request->rq_method_name;
  if(method_name != METHOD_OPTIONS && method_name != METHOD_DESCRIBE)
  {
    MCUTRACE(1, trace_section << address << " incorrect method " << method_name);
    SendResponse(socket_fd, address, msg, "455 Method Not Valid in This State");
    return FALSE;
  }

  PString luri_str = url_as_string(msg_home(msg), sip->sip_request->rq_url);
  MCUURL lurl(luri_str);
  PString path = lurl.GetPath()[0];
  if(path == "" || MCUConfig("RTSP Server "+path).GetBoolean(EnableKey) == FALSE)
  {
    MCUTRACE(1, trace_section << address << " server not found " << path);
    SendResponse(socket_fd, address, msg, "404 Not Found");
    return FALSE;
  }

  PString callToken = address;
  MCURtspConnection *rCon = new MCURtspConnection(sep, ep, callToken);
  if(rCon->Connect(address, socket_fd, msg) == FALSE)
  {
    rCon->LeaveMCU();
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspServer::SendResponse(int socket_fd, const PString & address, const msg_t *msg, const PString & status_str)
{
  char buffer[1024];
  snprintf(buffer, 1024,
  	   "RTSP/1.0 %s\r\n"
	   "Date: %s\r\n"
	   "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY\r\n"
	   "\r\n"
	   , (const char *)status_str, (const char *)PTime().AsString());

  MCUTRACE(1, trace_section << "send to " << address << " " << strlen(buffer) << " bytes\n" << buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUListener::MCUListener(PString address, mcu_listener_cb *_callback, void *_callback_context)
{
  running = FALSE;

  callback = _callback;
  callback_context = _callback_context;
  tcp_thread = NULL;
  socket_timeout_sec = 0;
  socket_timeout_usec = 250000;
  handler_count = 0;

  socket_address = address;
  socket_address.Replace(" ","",TRUE,0);
  if(socket_address.Find("tcp:") == P_MAX_INDEX)
    socket_address = "tcp:"+socket_address;

  MCUURL url(socket_address);
  socket_host = url.GetHostName();
  socket_port = url.GetPort().AsInteger();
  if(socket_host == "")
    socket_host = "0.0.0.0";

  trace_section = "MCU listener ("+socket_address+"): ";

  if(socket_host != "0.0.0.0" && PIPSocket::Address(socket_host).IsValid() == FALSE)
  {
    MCUTRACE(1, trace_section << "incorrect socket host " << socket_host);
    return;
  }
  if(socket_port == 0)
  {
    MCUTRACE(1, trace_section << "incorrect socket port " << socket_port);
    return;
  }

  if(socket_host == "0.0.0.0" || PIPSocket::IsLocalHost(socket_host) == TRUE)
  {
    listener_type = TCP_LISTENER_SERVER;
    if(CreateTCPServer() == FALSE)
      return;
  } else {
    listener_type = TCP_LISTENER_CLIENT;
    if(CreateTCPClient() == FALSE)
      return;
  }

  tcp_thread = PThread::Create(PCREATE_NOTIFIER(TCPListener), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "mcu_listener:%0x");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUListener::MCUListener(int client_fd, PString address, mcu_listener_cb *_callback, void *_callback_context)
{
  running = FALSE;

  callback = _callback;
  callback_context = _callback_context;
  tcp_thread = NULL;
  socket_timeout_sec = 0;
  socket_timeout_usec = 250000;
  handler_count = 0;

  socket_address = address;
  if(socket_address.Find("tcp:") == P_MAX_INDEX)
    socket_address = "tcp:"+socket_address;
  trace_section = "MCU listener ("+socket_address+"): ";

  address.Replace(" ","",TRUE,0);
  socket_host = address.Tokenise(":")[1];
  socket_port = address.Tokenise(":")[2].AsInteger();

  listener_type = TCP_LISTENER_CLIENT;
  socket_fd = client_fd;

  tcp_thread = PThread::Create(PCREATE_NOTIFIER(TCPListener), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "mcu_listener:%0x");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUListener::~MCUListener()
{
  running = FALSE;
  if(tcp_thread)
  {
    tcp_thread->WaitForTermination(10000);
    delete tcp_thread;
  }

  // close cocket
  close(socket_fd);

  // wait until all handler threads terminated
  while(handler_count > 0)
    PThread::Sleep(100);

  MCUTRACE(1, trace_section << "close");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUListener *MCUListener::Create(PString address, mcu_listener_cb *callback, void *callback_context)
{
  MCUListener *list = new MCUListener(address, callback, callback_context);
  PThread::Sleep(100);
  if(list->IsRunning() == TRUE)
  {
    MCUTRACE(1, "MCU listener ("+address+"): " << "create");
  } else {
    MCUTRACE(1, "MCU listener ("+address+"): " << "cannot create listener");
    delete list;
    list = NULL;
  }

  return list;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUListener *MCUListener::Create(int client_fd, PString address, mcu_listener_cb *callback, void *callback_context)
{
  MCUListener *list = new MCUListener(client_fd, address, callback, callback_context);
  PThread::Sleep(100);
  if(list->IsRunning() == TRUE)
  {
    MCUTRACE(1, "MCU listener ("+address+"): " << "create");
  } else {
    MCUTRACE(1, "MCU listener ("+address+"): " << "cannot create listener");
    delete list;
    list = NULL;
  }

  return list;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUListener::GetSocketAddress(int fd, PString & address)
{
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  if(getpeername(fd, (sockaddr *)&addr, &addr_len) == -1)
    return FALSE;

  unsigned port = ntohs(addr.sin_port);
  char ip[INET_ADDRSTRLEN];
  if(inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip)) == NULL)
    return FALSE;

  int type;
  socklen_t type_len = sizeof(int);
  if(getsockopt(fd, SOL_SOCKET, SO_TYPE, &type, &type_len) == -1)
    return FALSE;

  PString transport;
  if(type == SOCK_STREAM)
    transport = "tcp";
  else
    transport = "udp";

  address = transport+":"+PString(ip)+":"+PString(port);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUListener::Send(int fd, char *buffer)
{
  int len = strlen(buffer);
  if(send(fd, buffer, len, 0) == -1)
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUListener::Send(char *buffer)
{
  if(Send(socket_fd, buffer) == FALSE)
  {
    MCUTRACE(1, trace_section << "sending error " << errno << " " << strerror(errno));
    return FALSE;
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUListener::ReadData(int fd, char *buffer, int buffer_size)
{
  // read into buffer
  int len = read(fd, buffer, buffer_size);
  // if error or closed
  if(len <= 0)
    return len;

  // Finalize as string
  buffer[len] = 0;
  // return len
  return len;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUListener::RecvData(int fd, char *buffer, int buffer_size)
{
  // read into buffer
  int len = recv(fd, buffer, buffer_size, 0);
  // if error or closed
  if(len <= 0)
    return len;

  // Finalize as string
  buffer[len] = 0;
  // return len
  return len;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUListener::ReadSerialData(int fd, PString & data)
{
  char buffer[16384];
  int buffer_size = 16383; // one less for finall \0

  while(MCUListener::ReadData(fd, buffer, buffer_size) > 0)
    data += buffer;

  if(data.GetLength() == 0)
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUListener::RecvSerialData(int fd, PString & data)
{
  char buffer[16384];
  int buffer_size = 16383; // one less for finall \0

  while(MCUListener::RecvData(fd, buffer, buffer_size) > 0)
    data += buffer;

  if(data.GetLength() == 0)
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUListener::TestSocket(int fd)
{
  if(send(fd, NULL, 0, MSG_NOSIGNAL) == -1)
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUListener::TCPConnectionHandler(PThread &, INT client_fd)
{
  handler_count++;
  PString address, data;

  if(MCUListener::GetSocketAddress(client_fd, address) == FALSE)
    goto error;

  if(MCUListener::ReadSerialData(client_fd, data) == FALSE)
    goto error;

  if(callback(callback_context, client_fd, address, data) == 0)
    goto error;

  handler_count--;
  return;

  error:
    MCUTRACE(1, "MCU Listener: close connection " << address);
    close(client_fd);
    handler_count--;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUListener::CreateTCPServer()
{
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

  // create socket
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_fd == -1)
  {
    MCUTRACE(1, trace_section << "create socket error " << errno << " " << strerror(errno));
    return FALSE;
  }

  // set socket non-blocking
  int flags = fcntl(socket_fd, F_GETFD);
  fcntl(socket_fd, F_SETFD, flags | O_NONBLOCK);

  // recv timeout
  struct timeval tv;
  tv.tv_sec = socket_timeout_sec;
  tv.tv_usec = socket_timeout_usec;
  if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof(tv)) == -1)
  {
    MCUTRACE(1, trace_section << "setsockopt error " << errno << " " << strerror(errno));
    return FALSE;
  }

  // allows other sockets to bind() to this port, unless there is an active listening socket bound to the port already
  int reuse = 1;
  if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
  {
    MCUTRACE(1, trace_section << "setsockopt error " << errno << " " << strerror(errno));
    return FALSE;
  }

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, socket_host, &addr.sin_addr);
  addr.sin_port = htons(socket_port);

  if(bind(socket_fd, (sockaddr *)&addr, addr_len) == -1)
  {
    MCUTRACE(1, trace_section << "socket bind error " << errno << " " << strerror(errno));
    return FALSE;
  }

  if(listen(socket_fd, SOMAXCONN) == -1)
  {
    MCUTRACE(1, trace_section << "socket listen error " << errno << " " << strerror(errno));
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUListener::CreateTCPClient()
{
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

  // create socket
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_fd == -1)
  {
    MCUTRACE(1, trace_section << "create socket error " << errno << " " << strerror(errno));
    return FALSE;
  }

  // set socket non-blocking
  int flags = fcntl(socket_fd, F_GETFD);
  fcntl(socket_fd, F_SETFD, flags | O_NONBLOCK);

  // recv timeout
  struct timeval tv;
  tv.tv_sec = socket_timeout_sec;
  tv.tv_usec = socket_timeout_usec;
  if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof(tv)) == -1)
  {
    MCUTRACE(1, trace_section << "setsockopt error " << errno << " " << strerror(errno));
    return FALSE;
  }

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, socket_host, &addr.sin_addr);
  addr.sin_port = htons(socket_port);

  // Connect
  if(connect(socket_fd, (const sockaddr *)&addr, addr_len) == -1)
  {
    MCUTRACE(1, trace_section << "failed connect to socket " << socket_host << ":" << socket_port);
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUListener::TCPListener(PThread &, INT)
{
  signal(SIGPIPE, SIG_IGN);

  struct sockaddr_in addr_client;
  socklen_t addr_client_len = sizeof(addr_client);

  running = TRUE;
  if(listener_type == TCP_LISTENER_SERVER)
  {
    while(running)
    {
      int client_fd = accept(socket_fd, (struct sockaddr *)&addr_client, &addr_client_len);
      if(client_fd < 0)
        continue;

      PThread::Create(PCREATE_NOTIFIER(TCPConnectionHandler), client_fd, PThread::AutoDeleteThread, PThread::NormalPriority, "tcp_connection_handler:%0x");
    }
  }
  else if(listener_type == TCP_LISTENER_CLIENT)
  {
    while(running)
    {
      PString data;
      RecvSerialData(socket_fd, data);

      if(errno != EAGAIN)
      {
        MCUTRACE(1, trace_section << "error " << errno << " " << strerror(errno));
        callback(callback_context, -1, "", "");
        break;
      }

      if(data.GetLength() == 0)
        continue;

      callback(callback_context, socket_fd, socket_address, data);
    }
  }
  running = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
