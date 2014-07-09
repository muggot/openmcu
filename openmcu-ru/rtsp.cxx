
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

static struct sockaddr *GetIPAddr(const char *ip, int port, int isIPv6, int *size, int *PF)
{
  struct sockaddr *sendAddr;

  /* If it is ipv6 */
  if(isIPv6)
  {
    /* Set size*/
    *size = sizeof(struct sockaddr_in6);
    /*Create address */
    sendAddr = (struct sockaddr *)malloc(*size);
    /* empty addres */
    memset(sendAddr,0,*size);
    /* Set PF */
    *PF = PF_INET6;
    /*Set family */
    ((struct sockaddr_in6 *)sendAddr)->sin6_family = AF_INET6;
    /* Set Address */
    inet_pton(AF_INET6,ip,&((struct sockaddr_in6*)sendAddr)->sin6_addr);
    /* Set port */
    ((struct sockaddr_in6 *)sendAddr)->sin6_port = htons(port);
  } else {
    /* Set size*/
    *size = sizeof(struct sockaddr_in);
    /*Create address */
    sendAddr = (struct sockaddr *)malloc(*size);
    /* empty addres */
    memset(sendAddr,0,*size);
    /*Set family */
    ((struct sockaddr_in*)sendAddr)->sin_family = AF_INET;
    /* Set PF */
    *PF = PF_INET;
    /* Set Address */
    ((struct sockaddr_in*)sendAddr)->sin_addr.s_addr = inet_addr(ip);
    /* Set port */
    ((struct sockaddr_in *)sendAddr)->sin_port = htons(port);
  }
  return sendAddr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int RecvData(int socket_fd, char *buffer, int buffer_size)
{
  // read into buffer
  int len = recv(socket_fd, buffer, buffer_size, 0);

  // if error or closed
  if(len <= 0)
    return 0;

  buffer[len] = 0; // Finalize as string
  return len;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int SendData(int socket_fd, char *buffer)
{
  int len = strlen(buffer);
  if(send(socket_fd, buffer, len, 0) == -1)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL PreParseMsg(PString & msg_str)
{
  if(msg_str.Find("Cseq:") != P_MAX_INDEX)
    msg_str.Replace("Cseq:","CSeq:",TRUE,0);

  if(msg_str.Find("CSeq:") == P_MAX_INDEX)
    return FALSE;

  PString name;
  if(msg_str.Find("OPTIONS") != P_MAX_INDEX)
    name = "OPTIONS";
  else if(msg_str.Find("DESCRIBE") != P_MAX_INDEX)
    name = "DESCRIBE";
  else if(msg_str.Find("SETUP") != P_MAX_INDEX)
    name = "SETUP";
  else if(msg_str.Find("PLAY") != P_MAX_INDEX)
    name = "PLAY";
  else if(msg_str.Find("TEARDOWN") != P_MAX_INDEX)
    name = "TEARDOWN";
  else
    return FALSE;

  PString cseq;
  for(PINDEX i = msg_str.Find("CSeq:")+6; i < msg_str.GetLength(); i++)
  {
    if(msg_str[i] == ' ' || msg_str[i] == '\r')
      break;
    cseq += msg_str[i];
  }
  if(cseq == "")
    return FALSE;

  if(msg_str.Find("CSeq: "+cseq+" "+name) == P_MAX_INDEX)
    msg_str.Replace("CSeq: "+cseq,"CSeq: "+cseq+" "+name,TRUE,0);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

msg_t * ParseMsg(PString & msg_str)
{
  if(PreParseMsg(msg_str) == FALSE)
    return NULL;

  msg_t *msg = msg_make(sip_default_mclass(), 0, (const void *)(const char *)msg_str, msg_str.GetLength());
  sip_t *sip = sip_object(msg);
  if(sip == NULL || sip->sip_cseq == NULL || sip->sip_cseq->cs_method_name == NULL)
  {
    msg_destroy(msg);
    return NULL;
  }
  return msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCURtspConnection::MCURtspConnection(MCUSipEndPoint *_sep, MCUH323EndPoint *_ep, PString _callToken)
  :MCUSipConnection(_sep, _ep, _callToken)
{
  trace_section = "RTSP Connection "+callToken+": ";
  remoteApplication = "RTSP terminal";

  rtsp_state = RTSP_NONE;
  rtsp_terminating = 0;
  cseq = 1;
  rtsp_thread = NULL;

  // create local capability list
  CreateLocalSipCaps();

  // create rtp sessions
  CreateDefaultRTPSessions();

  MCUTRACE(1, trace_section << "constructor");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCURtspConnection::~MCURtspConnection()
{
  MCUTRACE(1, trace_section << "destructor");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspConnection::ProcessShutdown(CallEndReason reason)
{
  PTRACE(1, trace_section << "shutdown connection");
  callEndReason = reason;
  // leave conference and delete connection
  MCUH323Connection::LeaveMCU();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspConnection::LeaveMCU()
{
  PTRACE(1, trace_section << "LeaveMCU");
  MCUH323Connection::LeaveMCU();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspConnection::CleanUpOnCallEnd()
{
  PTRACE(1, trace_section << "CleanUpOnCallEnd reason: " << callEndReason);

  if(connectionState == EstablishedConnection)
    SendTeardown();

  if(rtsp_thread)
  {
    rtsp_terminating = 1;
    rtsp_thread->WaitForTermination();
    delete rtsp_thread;
    rtsp_thread = NULL;
  }
  close(socket_fd);

  connectionState = ShuttingDownConnection;

  StopTransmitChannels();
  StopReceiveChannels();
  DeleteChannels();
  videoReceiveCodecName = videoTransmitCodecName = "none";
  videoReceiveCodec = NULL;
  videoTransmitCodec = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::Connect(PString room, PString _ruri_str)
{
  PTRACE(1, trace_section << "Connect room: " << room << " ruri: " << _ruri_str);

  direction = DIRECTION_OUTBOUND;
  requestedRoom = room;

  ruri_str = _ruri_str;
  remotePartyAddress = ruri_str;

  ruri_str.Replace("rtsp:","http:",TRUE,0);
  MCUURL url(ruri_str);

  PString rtsp_port = url.GetPort();
  if(rtsp_port == "80") rtsp_port = "554";
  ruri_str = "rtsp://"+url.GetHostName()+":"+rtsp_port+url.GetPathStr();

  display_name = GetSectionParamFromUrl(DisplayNameKey, _ruri_str, url.GetPathStr());
  remotePartyName = remoteName = display_name;

  auth_username = GetSectionParamFromUrl(UserNameKey, _ruri_str, url.GetUserName());
  auth_password = GetSectionParamFromUrl(PasswordKey, _ruri_str, url.GetPassword());

  if(CreateSocket() == 0)
    return 0;

  if(SendDescribe() == 0)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendOptions()
{
  char buffer[1024];
  snprintf(buffer, 1024,
  	   "OPTIONS %s RTSP/1.0\r\n"
	   "CSeq: %d OPTIONS\r\n",
	   (const char *)ruri_str, cseq++);

  if(SendRequest(socket_fd, buffer, "OPTIONS") == 0)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendPlay()
{
  char buffer[1024];
  snprintf(buffer, 1024,
  	   "PLAY %s RTSP/1.0\r\n"
	   "CSeq: %d PLAY\r\n"
           "Session: %s\r\n"
           "Range: npt=0.000-\r\n",
	   (const char *)ruri_str, cseq++, (const char *)rtsp_session_str);

  if(SendRequest(socket_fd, buffer, "PLAY") == 0)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendSetup(int pt)
{
  SipCapability *sc = FindSipCap(RemoteSipCaps, pt);
  if(sc->attr.GetAt("control") == NULL)
  {
    PTRACE(1, trace_section << "capability attribute \"control\" not found");
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
	   "CSeq: %d SETUP\r\n"
	   "%s"
           "Transport: RTP/AVP/UDP;unicast;client_port=%d-%d\r\n",
	   (const char *)control, cseq++, (const char *)session_header, rtp_port, rtp_port+1);

  if(SendRequest(socket_fd, buffer, "SETUP") == 0)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendTeardown()
{
  char buffer[1024];
  snprintf(buffer, 1024,
  	   "TEARDOWN %s RTSP/1.0\r\n"
	   "CSeq: %d TEARDOWN\r\n"
	   "Session: %s\r\n",
	   (const char *)ruri_str, cseq++, (const char *)rtsp_session_str);

  if(SendRequest(socket_fd, buffer, "TEARDOWN") == 0)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendDescribe()
{
  char buffer[1024];
  snprintf(buffer,1024,
  	   "DESCRIBE %s RTSP/1.0\r\n"
	   "CSeq: %d DESCRIBE\r\n"
	   "Accept: application/sdp\r\n",
	   (const char *)ruri_str, cseq++);

  if(SendRequest(socket_fd, buffer, "DESCRIBE") == 0)
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
    return 600;

  // create and start channels
  CreateLogicalChannels();
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
  return 0;
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

  PStringArray ta = transport_str.Tokenise(";");
  //RTP/AVP/UDP;unicast;source=192.168.1.1;client_port=5002-5003;server_port=52069-52070;ssrc=C7F3A123;mode=play
  for(PINDEX i = 0; i < ta.GetSize(); i++)
  {
    if(ta[i].Left(7) == "source=")
    {
      sc->remote_ip = ta[i].Right(ta[i].GetLength()-7);
    }
    if(ta[i].Left(12) == "server_port=")
    {
      PString ports = ta[i].Right(ta[i].GetLength()-12);
      sc->remote_port = ports.Tokenise("-")[0].AsInteger();
    }
  }

  if(sc->remote_ip == "" || sc->remote_ip == "0.0.0.0")
    sc->remote_ip = MCUURL(ruri_str).GetHostName();

  if(rtsp_session_str == "" || sc->remote_port == 0)
  {
    PTRACE(1, trace_section << "missing session string or remote port");
    ProcessShutdown();
    return 0;
  }

  if(rtsp_state == RTSP_SETUP_AUDIO && vcap > 0)
  {
    rtsp_state = RTSP_SETUP_VIDEO;
    SendSetup(vcap);
    return 0;
  }

  SendPlay();
  rtsp_state = RTSP_PLAY;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnResponseDescribe(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);

  if(!sip->sip_payload || !sip->sip_payload->pl_data)
    return 415;

  PString sdp_str = sip->sip_payload->pl_data;
  int response_code = ProcessSDP(LocalSipCaps, RemoteSipCaps, sdp_str);
  if(response_code)
    return response_code;

  // set remote application
  if(sip->sip_user_agent)
    remoteApplication = sip->sip_user_agent->g_string;
  else if(sip->sip_server)
    remoteApplication = sip->sip_server->g_string;

  if(scap >= 0)
  {
    rtsp_state = RTSP_SETUP_AUDIO;
    SendSetup(scap);
  }
  else
  {
    rtsp_state = RTSP_SETUP_VIDEO;
    SendSetup(vcap);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnResponseReceived(const msg_t *msg)
{
  sip_t *sip = sip_object(msg);

  int status = sip->sip_status->st_status;
  PString cs_method_name = sip->sip_cseq->cs_method_name;

  if(cs_method_name == "DESCRIBE" && rtsp_state == RTSP_DESCRIBE)
  {
    if(status == 200)
    {
      int response_code = OnResponseDescribe(msg);
      if(response_code)
      {
        PTRACE(1, trace_section << "error " << response_code);
        ProcessShutdown();
        return 0;
      }
      return 0;
    }
    else if(status == 401)
    {
      if(auth_type != AUTH_NONE || auth_username == "" || auth_password == "")
      {
        PTRACE(1, trace_section << "error");
        ProcessShutdown();
        return 0;
      }
      if(sep->ParseAuthMsg(msg, auth_type, auth_scheme, auth_realm, auth_nonce) == FALSE)
      {
        PTRACE(1, trace_section << "error");
        ProcessShutdown();
        return 0;
      }
      SendDescribe();
      return 0;
    }
  }
  if(cs_method_name == "SETUP" && (rtsp_state == RTSP_SETUP_AUDIO || rtsp_state == RTSP_SETUP_VIDEO))
  {
    if(status == 200)
    {
      OnResponseSetup(msg);
      return 0;
    }
  }
  if(cs_method_name == "PLAY" && rtsp_state == RTSP_PLAY)
  {
    if(status == 200)
    {
      int response_code = OnResponsePlay(msg);
      if(response_code)
      {
        PTRACE(1, trace_section << "error " << response_code);
        ProcessShutdown();
        return 0;
      }
      return 0;
    }
  }

  PTRACE(1, trace_section << "unknown response " << cs_method_name << ", state " << rtsp_state);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendRequest(int socket_fd, char *buffer, PString method_name)
{
  if(auth_type != AUTH_NONE && method_name != "OPTIONS" && method_name != "RESPONSE")
  {
    PString auth_str = sep->MakeAuthStr(auth_username, auth_password, ruri_str, method_name, auth_scheme, auth_realm, auth_nonce);
    strcat(buffer, (const char *)PString("Authorization: "+auth_str+"\r\n"));
  }
  if(method_name != "SDP")
  {
    strcat(buffer, (const char *)PString("User-Agent: "+SIP_USER_AGENT+"\r\n"));
  }
  strcat(buffer, "\r\n");

  if(SendData(socket_fd, buffer) == 0)
  {
    MCUTRACE(1, trace_section << "error sending " << errno);
    ProcessShutdown();
    return 0;
  }

  MCUTRACE(1, trace_section << "send " << strlen(buffer) << " bytes\n" << buffer);
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::CreateSocket()
{
  struct sockaddr * sendAddr;
  int size;
  int PF;
  int isIPv6 = 0;

  MCUURL url(ruri_str);
  PString ip = url.GetHostName();
  unsigned port = url.GetPort().AsInteger();

  // Get send address
  sendAddr = GetIPAddr(ip, port, isIPv6, &size, &PF);

  // open socket
  socket_fd = socket(PF, SOCK_STREAM,0);

  // set socket non-blocking
  int flags = fcntl(socket_fd, F_GETFD);
  fcntl(socket_fd, F_SETFD,flags | O_NONBLOCK);

  // recv timeout 1 sec
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv))
  {
     perror("setsockopt");
     free(sendAddr);
     return 0;
  }

  // Connect
  if(connect(socket_fd, sendAddr, size) < 0)
  {
    MCUTRACE(1, trace_section << "failed connect to socket " << ip << ":" << port);
    free(sendAddr);
    return 0;
  }

  free(sendAddr);

  rtsp_thread = PThread::Create(PCREATE_NOTIFIER(RtspListener), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "rtsp_listener:%0x");

  rtsp_state = RTSP_CONNECT;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCURtspConnection::RtspListener(PThread &, INT)
{
  char buffer[16384];
  int buffer_size = 16383; // one less for finall \0
  PString msg_str;

  while(rtsp_terminating == 0)
  {
    // Read into buffer
    if(RecvData(socket_fd, buffer, buffer_size) == 0)
      continue;

    msg_str = buffer;
    MCUTRACE(1, trace_section << "recv " << msg_str.GetLength() << " bytes\n" << msg_str);

    msg_t *msg = ParseMsg(msg_str);
    sip_t *sip = sip_object(msg);
    if(msg == NULL || sip == NULL)
    {
      MCUTRACE(1, trace_section << "failed parse message");
      continue;
    }
    if(sip && sip->sip_content_length && sip->sip_content_length->l_length != 0)
    {
      // Read payload
      if(RecvData(socket_fd, buffer, buffer_size) == 0)
      {
        msg_destroy(msg);
        msg = NULL;
        continue;
      }
      msg_str = buffer;
      MCUTRACE(1, trace_section << "recv " << msg_str.GetLength() << " bytes\n" << msg_str);
      sip_add_tl(msg, sip_object(msg),
                  SIPTAG_PAYLOAD_STR((const char *)msg_str),
                  TAG_END());
    }

    if(sip->sip_status != NULL)
      OnResponseReceived(msg);

    msg_destroy(msg);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

