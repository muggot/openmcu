
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

BOOL MCUSocket::GetFromIP(PString & local_ip, PString remote_host, PString remote_port)
{
  PTRACE(1, "GetFromIP host:" << remote_host << " port:" << remote_port);
  if(remote_host == "" || remote_port == "")
    return FALSE;

  int err = 0;
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock == -1)
  {
    PTRACE(1, "GetFromIP error " << errno << " " << strerror(errno));
    return FALSE;
  }

  struct addrinfo serv;
  struct addrinfo *res = NULL;
  memset((void *)&serv, 0, sizeof(serv));
  serv.ai_family = AF_INET;
  serv.ai_socktype = SOCK_DGRAM;
  err = getaddrinfo((const char *)remote_host, (const char *)remote_port, &serv, &res);
  if(err != 0 || res == NULL)
  {
    PTRACE(1, "GetFromIP error " << errno << " " << strerror(errno));
    return FALSE;
  }

  err = connect(sock, res->ai_addr, res->ai_addrlen);
  if(err == -1)
  {
    PTRACE(1, "GetFromIP error " << errno << " " << strerror(errno));
    return FALSE;
  }

  sockaddr_in name;
  socklen_t namelen = sizeof(name);
  err = getsockname(sock, (sockaddr*) &name, &namelen);
  if(err == -1)
  {
    PTRACE(1, "GetFromIP error " << errno << " " << strerror(errno));
    return FALSE;
  }

#ifndef _WIN32
  char buffer[16] = {0};
  inet_ntop(AF_INET, (const void *)&name.sin_addr, buffer, 16);
  close(sock);
  local_ip = buffer;
#else
  local_ip = PIPSocket::Address(name.sin_addr);
  closesocket(sock);
#endif

  PTRACE(1, "GetFromIP ip: " << local_ip);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::GetHostIP(PString & ip, PString host, PString port)
{
  PTRACE(1, "GetHostIP host:" << host << " port:" << port);
  if(host == "")
    return FALSE;

  int err = 0;
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock == -1)
  {
    PTRACE(1, "GetHostIP error " << errno << " " << strerror(errno));
    return FALSE;
  }

  struct addrinfo serv;
  struct addrinfo *res=NULL;
  memset((void *)&serv, 0, sizeof(serv));
  serv.ai_family = AF_INET;
  serv.ai_socktype = SOCK_DGRAM;
  err = getaddrinfo((const char *)host, (const char *)port, &serv, &res);
  if(err != 0 || res == NULL)
  {
    PTRACE(1, "GetHostIP error " << errno << " " << strerror(errno));
    return FALSE;
  }

  char buffer[80] = {0};
  err = getnameinfo(res->ai_addr, res->ai_addrlen, buffer, (socklen_t)sizeof(buffer), NULL, 0, NI_NUMERICHOST);
  if(err != 0)
  {
    PTRACE(1, "GetHostIP error " << errno << " " << strerror(errno));
    return FALSE;
  }

  ip = buffer;
  PTRACE(1, "GetHostIP ip: " << ip);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSocket::MCUSocket(int fd, int proto, PString host, int port)
{
  socket_fd = fd;
  socket_proto = proto;
  socket_host = host;
  socket_port = port;

  socket_timeout_sec = 0;
  socket_timeout_usec = 250000;

  if(proto == SOCK_STREAM)
  {
    socket_address += "tcp:";
    if(socket_host == "0.0.0.0" || PIPSocket::IsLocalHost(socket_host) == TRUE)
      socket_type = TCP_SERVER;
    else
      socket_type = TCP_CLIENT;
  }
  else
    socket_address += "udp:";

  socket_address += socket_host+":"+PString(socket_port);
  trace_section = "MCU socket ("+socket_address+"): ";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSocket::~MCUSocket()
{
  // close cocket
  close(socket_fd);

  MCUTRACE(1, trace_section << "close");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSocket * MCUSocket::Create(int proto, PString host, int port)
{
  if(host == "")
    host = "0.0.0.0";

  if(host != "0.0.0.0" && MCUSocket::GetHostIP(host, host) == FALSE)
  {
    MCUTRACE(1, "MCUSocket incorrect host " << host);
    return NULL;
  }
  if(port == 0)
  {
    MCUTRACE(1, "MCUSocket incorrect port " << port);
    return NULL;
  }

  MCUSocket *socket = new MCUSocket(-1, proto, host, port);
  if(socket->GetType() == MCUSocket::TCP_SERVER && socket->Listen())
    return socket;
  else if(socket->GetType() == MCUSocket::TCP_CLIENT && socket->Connect())
    return socket;

  delete socket;
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSocket * MCUSocket::Create(int fd)
{
  int socket_proto;
  PString socket_host;
  int socket_port;
  if(MCUSocket::GetSocketAddress(fd, socket_proto, socket_host, socket_port) == FALSE)
    return NULL;

  return new MCUSocket(fd, socket_proto, socket_host, socket_port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::Open()
{
  // create socket
  socket_fd = socket(AF_INET, socket_proto, 0);
  if(socket_fd == -1)
  {
    MCUTRACE(1, trace_section << "create error " << errno << " " << strerror(errno));
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

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::Connect()
{
  if(!Open())
    return FALSE;

  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, socket_host, &addr.sin_addr);
  addr.sin_port = htons(socket_port);

  // Connect
  if(connect(socket_fd, (const sockaddr *)&addr, addr_len) == -1)
  {
    MCUTRACE(1, trace_section << "connect error " << socket_host << ":" << socket_port);
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::Listen()
{
  if(!Open())
    return FALSE;

  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, socket_host, &addr.sin_addr);
  addr.sin_port = htons(socket_port);

  // allows other sockets to bind() to this port, unless there is an active listening socket bound to the port already
  int reuse = 1;
  if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
  {
    MCUTRACE(1, trace_section << "setsockopt error " << errno << " " << strerror(errno));
    return FALSE;
  }

  if(::bind(socket_fd, (sockaddr *)&addr, addr_len) == -1)
  {
    MCUTRACE(1, trace_section << "bind error " << errno << " " << strerror(errno));
    return FALSE;
  }

  if(listen(socket_fd, SOMAXCONN) == -1)
  {
    MCUTRACE(1, trace_section << "listen error " << errno << " " << strerror(errno));
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSocket * MCUSocket::Accept()
{
  struct sockaddr_in addr_client;
  socklen_t addr_client_len = sizeof(addr_client);
  int fd = accept(socket_fd, (struct sockaddr *)&addr_client, &addr_client_len);
  if(fd < 0)
    return NULL;

  return MCUSocket::Create(fd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::TestSocket(int fd)
{
  if(send(fd, NULL, 0, MSG_NOSIGNAL) == -1)
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::GetSocketAddress(int fd, int & proto, PString & host, int & port)
{
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  if(getpeername(fd, (sockaddr *)&addr, &addr_len) == -1)
    return FALSE;

  char ip[INET_ADDRSTRLEN];
  if(inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip)) == NULL)
    return FALSE;

  host = ip;
  port = ntohs(addr.sin_port);

  socklen_t proto_len = sizeof(int);
  if(getsockopt(fd, SOL_SOCKET, SO_TYPE, &proto, &proto_len) == -1)
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::SendData(char *buffer)
{
  int len = strlen(buffer);
  if(send(socket_fd, buffer, len, 0) == -1)
  {
    MCUTRACE(1, trace_section << "send error: " << errno << " " << strerror(errno));
    return FALSE;
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::RecvData(PString & data)
{
  char buffer[16384];
  // one less for finall \0
  int buffer_size = 16383;
  int len;

  while(1)
  {
    len = recv(socket_fd, buffer, buffer_size, 0);
    int error = errno;
    if(len > 0)
    {
      buffer[len] = 0;
      data += buffer;
      if(data.GetLength() >= 65535)
        return TRUE;
    }
    else if(len < 0)
    {
      switch(error)
      {
        case(EINTR):
        case(EAGAIN):
          return TRUE;
        default:
          MCUTRACE(1, trace_section << "recv error " << error << " " << strerror(error));
          return FALSE;
      }
    }
    else
    {
      // If an end-of-file condition is received or the connection is closed
      MCUTRACE(1, trace_section << "recv error, connection is closed");
      return FALSE;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::ReadData(PString & data)
{
  char buffer[16384];
  // one less for finall \0
  int buffer_size = 16383;
  int len;

  while(1)
  {
    len = read(socket_fd, buffer, buffer_size);
    int error = errno;
    if(len > 0)
    {
      buffer[len] = 0;
      data += buffer;
      if(data.GetLength() >= 65535)
        return TRUE;
    }
    else if(len < 0)
    {
      switch(error)
      {
        case(EINTR):
        case(EAGAIN):
          return TRUE;
        default:
          MCUTRACE(1, trace_section << "read error " << error << " " << strerror(error));
          return FALSE;
      }
    }
    else
    {
      // If an end-of-file condition is received or the connection is closed
      MCUTRACE(1, trace_section << "read error, connection is closed");
      return FALSE;
    }
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUListenerHandler::Main()
{
  PString data;

  if(socket->ReadData(data) == FALSE || data.GetLength() == 0)
    goto error;

  if(callback(callback_context, socket, data) == 0)
    goto error;

  (*handler_count)--;
  return;

  error:
    delete socket;
    socket = NULL;
    (*handler_count)--;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUListener::MCUListener(MCUSocket *_socket, mcu_listener_cb *_callback, void *_callback_context)
{
  running = FALSE;

  callback = _callback;
  callback_context = _callback_context;
  tcp_thread = NULL;
  handler_count = 0;

  socket = _socket;
  trace_section = "MCU listener ("+socket->GetAddress()+"): ";

  tcp_thread = PThread::Create(PCREATE_NOTIFIER(ListenerThread), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "mcu_listener:%0x");
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
  delete socket;
  socket = NULL;

  // wait until all handler threads terminated
  while(handler_count > 0)
    MCUTime::Sleep(100);

  MCUTRACE(1, trace_section << "close");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUListener * MCUListener::Create(int proto, const PString & host, int port, mcu_listener_cb *callback, void *callback_context)
{
  MCUSocket *socket = MCUSocket::Create(proto, host, port);
  if(socket == NULL)
  {
    MCUTRACE(1, "MCU listener cannot create listener");
    return NULL;
  }
  return MCUListener::Create(socket, callback, callback_context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUListener * MCUListener::Create(MCUSocket *socket, mcu_listener_cb *callback, void *callback_context)
{
  MCUTRACE(1, "MCU listener ("+socket->GetAddress()+"): " << "create");
  MCUListener *list = new MCUListener(socket, callback, callback_context);
  return list;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUListener::Send(char *buffer)
{
  return socket->SendData(buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUListener::ListenerThread(PThread &, INT)
{
  signal(SIGPIPE, SIG_IGN);

  running = TRUE;
  if(socket->GetType() == MCUSocket::TCP_SERVER)
  {
    while(running)
    {
      MCUSocket *client_socket = socket->Accept();
      if(client_socket == NULL)
        continue;

      handler_count++;
      new MCUListenerHandler(callback, callback_context, client_socket, &handler_count);
    }
  }
  else if(socket->GetType() == MCUSocket::TCP_CLIENT)
  {
    while(running)
    {
      PString data;
      if(socket->RecvData(data) == FALSE)
      {
        MCUTRACE(1, trace_section << "error");
        callback(callback_context, NULL, "");
        break;
      }

      if(data.GetLength() == 0)
        continue;

      callback(callback_context, socket, data);
    }
  }
  running = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
