
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

MCURtspConnection::MCURtspConnection(MCUSipEndPoint *_sep, MCUH323EndPoint *_ep, PString _callToken)
  :MCUSipConnection(_sep, _ep, _callToken)
{
  connectionState = NoConnectionActive;
  callToken = _callToken;
  trace_section = "RTSP Connection "+callToken+": ";
  scap = -1;
  vcap = -1;
  connectedTime = PTime();
  audio_rtp_port = video_rtp_port = 0;

  rtsp_state = RTSP_NONE;
  rtsp_terminating = 0;
  cseq = 1;
  rtsp_thread = NULL;

  MCUTRACE(1, trace_section << "constructor");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCURtspConnection::~MCURtspConnection()
{
  MCUTRACE(1, trace_section << "destructor");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::ProcessShutdown(CallEndReason reason)
{
  PTRACE(1, trace_section << "shutdown connection");
  callEndReason = reason;
  // leave conference and delete connection
  MCUH323Connection::LeaveMCU();
  return 1;
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
  requestedRoom = room;
  ruri_str = _ruri_str;
  remotePartyAddress = ruri_str;

  ruri_str.Replace("rtsp:","http:",TRUE,0);
  MCUURL url(ruri_str);
  remotePartyName = remoteName = url.GetPathStr();
  username = url.GetUserName();
  password = url.GetPassword();

  PString rtsp_port = url.GetPort();
  if(rtsp_port == "80") rtsp_port = "554";
  ruri_str = "rtsp://"+url.GetHostName()+":"+rtsp_port+url.GetPathStr();

  display_name = GetEndpointParamFromUrl("Display name", ruri_str);
  if(display_name != "")
    remoteName = remotePartyName = display_name;


  if(CreateSocket() == 0)
    return 0;

  if(SendDescribe() == 0)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendOptions()
{
  char request[1024];
  snprintf(request, 1024,
  	   "OPTIONS %s RTSP/1.0\r\n"
	   "CSeq: %d OPTIONS\r\n"
	   "User-Agent: %s\r\n",
	   (const char *)ruri_str, cseq++, (const char *)(SIP_USER_AGENT));
  strcat(request,"\r\n");

  if(SendRequest(socket_fd, request) == 0)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendPlay()
{
  char request[1024];
  snprintf(request, 1024,
  	   "PLAY %s RTSP/1.0\r\n"
	   "CSeq: %d PLAY\r\n"
           "Session: %s\r\n"
           "Range: npt=0.000-\r\n"
	   "User-Agent: %s\r\n",
	   (const char *)ruri_str, cseq++, (const char *)rtsp_session_str, (const char *)(SIP_USER_AGENT));
  strcat(request,"\r\n");

  if(SendRequest(socket_fd, request) == 0)
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

  char request[1024];
  snprintf(request, 1024,
  	   "SETUP %s RTSP/1.0\r\n"
	   "CSeq: %d SETUP\r\n"
	   "%s"
           "Transport: RTP/AVP/UDP;unicast;client_port=%d-%d\r\n"
	   "User-Agent: %s\r\n",
	   (const char *)control, cseq++, (const char *)session_header, rtp_port, rtp_port+1, (const char *)(SIP_USER_AGENT));
  strcat(request,"\r\n");

  if(SendRequest(socket_fd, request) == 0)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendTeardown()
{
  char request[1024];
  snprintf(request, 1024,
  	   "TEARDOWN %s RTSP/1.0\r\n"
	   "CSeq: %d TEARDOWN\r\n"
	   "Session: %s\r\n"
	   "User-Agent: %s\r\n",
	   (const char *)ruri_str, cseq++, (const char *)rtsp_session_str, (const char *)(SIP_USER_AGENT));
  strcat(request,"\r\n");

  if(SendRequest(socket_fd, request) == 0)
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendDescribe()
{
  char request[1024];
  snprintf(request,1024,
  	   "DESCRIBE %s RTSP/1.0\r\n"
	   "CSeq: %d DESCRIBE\r\n"
	   "Accept: application/sdp\r\n"
	   "User-Agent: %s\r\n",
	   (const char *)ruri_str, cseq++, (const char *)(SIP_USER_AGENT));
  strcat(request,"\r\n");

  if(SendRequest(socket_fd, request) == 0)
     return 0;

  rtsp_state = RTSP_DESCRIBE;
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnPlayResponse(msg_t *msg)
{
  // set endpoint member name
  SetMemberName();
  // override requested room from registrar
  SetRequestedRoom();
  // join conference
  OnEstablished();
  if(!conference || !conferenceMember || !conferenceMember->IsJoined())
    return 600;

  DeleteTempSockets();
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

int MCURtspConnection::OnSetupResponse(msg_t *msg)
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
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::OnDescribeResponse(msg_t *msg)
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

  CreateTempSockets();

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

int MCURtspConnection::OnReceived(msg_t *msg)
{
  sip_t *sip = sip_object(msg);

  int status = 0;
  PString rq_method_name, cs_method_name;
  if(sip->sip_status)  status = sip->sip_status->st_status;
  if(sip->sip_request) rq_method_name = sip->sip_request->rq_method_name;
  if(sip->sip_cseq)    cs_method_name = sip->sip_cseq->cs_method_name;

  if(status == 0)
    return 0;

  if(cs_method_name == "DESCRIBE" && rtsp_state == RTSP_DESCRIBE)
  {
    if(status == 200)
    {
      int response_code = OnDescribeResponse(msg);
      if(response_code)
      {
        PTRACE(1, trace_section << "error " << response_code);
        ProcessShutdown();
        return 0;
      }
      return 0;
    }
    //else if(status == 401)
    //{
    //  return 0;
    //}
  }
  if(cs_method_name == "SETUP" && (rtsp_state == RTSP_SETUP_AUDIO || rtsp_state == RTSP_SETUP_VIDEO))
  {
    if(status == 200)
    {
      OnSetupResponse(msg);
      return 0;
    }
  }
  if(cs_method_name == "PLAY" && rtsp_state == RTSP_PLAY)
  {
    if(status == 200)
    {
      int response_code = OnPlayResponse(msg);
      if(response_code)
      {
        PTRACE(1, trace_section << "error " << response_code);
        ProcessShutdown();
        return 0;
      }
      return 0;
    }
  }

  PTRACE(1, trace_section << "error");
  ProcessShutdown();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::SendRequest(int socket_fd, char *request)
{
  int len = strlen(request);
  if(send(socket_fd, request, len, 0) == -1)
  {
    // If failed connection
    if(errno != EAGAIN)
    {
      MCUTRACE(1, trace_section << "error sending request " << errno);
      ProcessShutdown();
    }
    return 0;
  }

  MCUTRACE(1, trace_section << "send " << len << " bytes\n" << request);
  return len;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCURtspConnection::RecvData(int socket_fd, char *buffer, int buffer_size)
{
  // Read into buffer
  int len = recv(socket_fd, buffer, buffer_size, 0);

  // if error or closed
  if(len <= 0)
  {
    // If failed connection
    if((errno != EAGAIN && errno != EWOULDBLOCK) || !len)
    {
      MCUTRACE(1, trace_section << "error receiving data [" << len << "," << errno << "]." << strerror(errno));
      ProcessShutdown();
    }
    // exit
    return 0;
  }
  // Finalize as string
  buffer[len] = 0;

  MCUTRACE(1, trace_section << "recv " << len << " bytes\n" << buffer);
  return len;
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

  // Set socket non-blocking
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
    MCUTRACE(1, trace_section << "Failed connect to socket " << ip << ":" << port);
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
  int buffer_size = 16383; // One less for finall \0
  int buffer_len = 0;

  msg_t *msg = NULL;

  while(rtsp_terminating == 0)
  {
    // Read into buffer
    if((buffer_len = RecvData(socket_fd, buffer, buffer_size)) == 0)
      continue;

    msg = msg_make(sip_default_mclass(), 0, (void const *)buffer, buffer_len);
    sip_t *sip = sip_object(msg);
    if(!sip)
    {
      MCUTRACE(1, trace_section << "Failed parse message");
      msg_destroy(msg);
      msg = NULL;
      continue;
    }
    if(sip && sip->sip_content_length && sip->sip_content_length->l_length != 0)
    {
      // Read payload
      if((buffer_len = RecvData(socket_fd, buffer, buffer_size)) == 0)
      {
        msg_destroy(msg);
        msg = NULL;
        continue;
      }
      sip_add_tl(msg, sip_object(msg),
                  SIPTAG_PAYLOAD_STR(buffer),
                  TAG_END());
    }
    OnReceived(msg);
    msg_destroy(msg);
    msg = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

