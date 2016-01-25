/*
 * sockets.cxx
 *
 * Copyright (C) 2014-2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
 * Copyright (C) 2015 Konstantin Yeliseyev, OpenMCU-ru, All Rights Reserved
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
 *                  Konstantin Yeliseyev (kay27@bk.ru)
 *
 */


#include "precompile.h"
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

// XP compatibility:
int inet_pton_xp(int af, const char *src, void *dst)
{
  struct sockaddr_storage ss;
  int size = sizeof(ss);
  char src_copy[INET6_ADDRSTRLEN+1];

  ZeroMemory(&ss, sizeof(ss));
  /* stupid non-const API */
  strncpy (src_copy, src, INET6_ADDRSTRLEN+1);
  src_copy[INET6_ADDRSTRLEN] = 0;

  if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
    switch(af) {
      case AF_INET:
    *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
    return 1;
      case AF_INET6:
    *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
    return 1;
    }
  }
  return 0;
}

const char *inet_ntop_xp(int af, const void *src, char *dst, socklen_t size)
{
  struct sockaddr_storage ss;
  unsigned long s = size;

  ZeroMemory(&ss, sizeof(ss));
  ss.ss_family = af;

  switch(af) {
    case AF_INET:
      ((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
      break;
    case AF_INET6:
      ((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
      break;
    default:
      return NULL;
  }
  /* cannot direclty use &size because of strict aliasing rules */
  return (WSAAddressToString((struct sockaddr *)&ss, sizeof(ss), NULL, dst, &s) == 0)?
          dst : NULL;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::GetFromIP(PString & ip, const PString & host, const PString & port)
{
  PTRACE(1, "MCUSocket GetFromIP host:" << host << " port:" << port);
  if(host == "" || port == "")
    return FALSE;

  int err = 0;
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock == -1)
  {
    PTRACE(1, "MCUSocket error " << errno << " " << strerror(errno));
    return FALSE;
  }

  struct addrinfo serv;
  struct addrinfo *res = NULL;
  memset((void *)&serv, 0, sizeof(serv));
  serv.ai_family = AF_INET;
  serv.ai_socktype = SOCK_DGRAM;
  err = getaddrinfo((const char *)host, (const char *)port, &serv, &res);
  if(err != 0 || res == NULL)
  {
    PTRACE(1, "MCUSocket error " << errno << " " << strerror(errno));
    return FALSE;
  }

  err = connect(sock, res->ai_addr, res->ai_addrlen);
  if(err == -1)
  {
    PTRACE(1, "MCUSocket error " << errno << " " << strerror(errno));
    return FALSE;
  }

  sockaddr_in name;
  socklen_t namelen = sizeof(name);
  err = getsockname(sock, (sockaddr*) &name, &namelen);
  if(err == -1)
  {
    PTRACE(1, "MCUSocket error " << errno << " " << strerror(errno));
    return FALSE;
  }

#ifndef _WIN32
  char buffer[16] = {0};
  inet_ntop(AF_INET, (const void *)&name.sin_addr, buffer, 16);
  close(sock);
  ip = buffer;
#else
  ip = PIPSocket::Address(name.sin_addr).AsString();
  closesocket(sock);
#endif

  PTRACE(1, "MCUSocket GetFromIP ip: " << ip);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::GetHostIP(PString & ip, const PString & host, const PString & port)
{
  PTRACE(1, "MCUSocket GetHostIP host:" << host << " port:" << port);

  if(host == "")
    return FALSE;

  if(MCUSocket::IsValidHost(host))
  {
    ip = host;
    PTRACE(1, "MCUSocket GetHostIP ip: " << ip);
    return TRUE;
  }

  int err = 0;
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock == -1)
  {
    PTRACE(1, "MCUSocket error " << errno << " " << strerror(errno));
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
    PTRACE(1, "MCUSocket error " << errno << " " << strerror(errno));
    return FALSE;
  }

  char buffer[80] = {0};
  err = getnameinfo(res->ai_addr, res->ai_addrlen, buffer, (socklen_t)sizeof(buffer), NULL, 0, NI_NUMERICHOST);
  if(err != 0)
  {
    PTRACE(1, "MCUSocket error " << errno << " " << strerror(errno));
    return FALSE;
  }

  ip = buffer;
  PTRACE(1, "MCUSocket GetHostIP ip: " << ip);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::IsValidHost(const PString & host)
{
  if(host == "*" || host == "0.0.0.0")
    return TRUE;
  return PIPSocket::Address(host).IsValid();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::IsLocalHost(const PString & host)
{
  if(host == "*" || host == "0.0.0.0")
    return TRUE;
  return PIPSocket::IsLocalHost(host);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSocket::MCUSocket(int fd, int proto, const PString & host, int port)
{
  socket_fd = fd;
  socket_proto = proto;
  socket_host = host;
  socket_port = port;

  if(socket_host == "*")
    socket_host = "0.0.0.0";

  socket_timeout_sec = 0;
  socket_timeout_usec = 250000;

  if(socket_proto == SOCK_STREAM)
    socket_address += "tcp:";
  else
    socket_address += "udp:";

  socket_address += socket_host+":"+PString(socket_port);
  trace_section = "MCU socket ("+socket_address+"): ";
  MCUTRACE(1, trace_section << "create");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSocket::~MCUSocket()
{
  // close cocket
  if(socket_fd!=-1)
#   ifdef _WIN32
      closesocket(socket_fd);
#   else
      close(socket_fd);
#   endif

  MCUTRACE(1, trace_section << "close");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSocket * MCUSocket::Create(int proto, PString host, int port)
{
  if(!MCUSocket::IsValidHost(host) && !MCUSocket::GetHostIP(host, host))
  {
    MCUTRACE(1, "MCUSocket incorrect host " << host);
    return NULL;
  }
  if(port < 1 || port > 65535)
  {
    MCUTRACE(1, "MCUSocket incorrect port " << port);
    return NULL;
  }

  return new MCUSocket(-1, proto, host, port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSocket * MCUSocket::Create(int fd)
{
  int proto;
  PString host;
  int port;
  if(MCUSocket::GetSocketAddress(fd, proto, host, port) == FALSE)
    return NULL;

  return new MCUSocket(fd, proto, host, port);
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
  int flags;

# ifndef _WIN32
    bzero(&addr, sizeof(addr));
# else
    memset(&addr, 0, sizeof(addr));
# endif
  addr.sin_family = AF_INET;
# ifdef _WIN32
  inet_pton_xp(AF_INET, socket_host, &addr.sin_addr);
# else
  inet_pton(AF_INET, socket_host, &addr.sin_addr);
# endif
  addr.sin_port = htons(socket_port);

  // set socket non-blocking
  flags = fcntl(socket_fd, F_GETFL);
  fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

  if(connect(socket_fd, (const sockaddr *)&addr, addr_len) == -1)
  {
    if(errno == EINPROGRESS)
    {
      struct timeval tv;
      fd_set fdset;
      tv.tv_sec = 3;
      tv.tv_usec = 0;
      FD_ZERO(&fdset);
      FD_SET(socket_fd, &fdset);
      if(select(socket_fd+1, NULL, &fdset, NULL, &tv) > 0)
      {
        int opt;
        socklen_t opt_len = sizeof(int);
        getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (char *)(&opt), &opt_len);
        if(opt)
        {
          MCUTRACE(1, trace_section << "connect error " << opt << " " << strerror(opt));
          return FALSE;
        }
      } else {
        MCUTRACE(1, trace_section << "connect timeout " << errno << " " << strerror(errno));
        return FALSE;
      }
    }
    else
    {
      MCUTRACE(1, trace_section << "connect error " << errno << " " << strerror(errno));
      return FALSE;
    }
  }

  // set socket blocking
  flags = fcntl(socket_fd, F_GETFL);
  fcntl(socket_fd, F_SETFL, flags & (~O_NONBLOCK));

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::Listen()
{
  if(!Open())
    return FALSE;

  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

# ifndef _WIN32
    bzero(&addr, sizeof(addr));
# else
    memset(&addr, 0, sizeof(addr));
# endif
  addr.sin_family = AF_INET;
# ifdef _WIN32
  inet_pton_xp(AF_INET, socket_host, &addr.sin_addr);
# else
  inet_pton(AF_INET, socket_host, &addr.sin_addr);
# endif
  addr.sin_port = htons(socket_port);

  // allows other sockets to bind() to this port, unless there is an active listening socket bound to the port already
  int reuse = 1;
  if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)(&reuse), sizeof(int)) == -1)
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
# ifdef _WIN32
  if(inet_ntop_xp(AF_INET, &addr.sin_addr, ip, sizeof(ip)) == NULL)
    return FALSE;
# else
  if(inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip)) == NULL)
    return FALSE;
# endif

  host = ip;
  port = ntohs(addr.sin_port);

  socklen_t proto_len = sizeof(int);
  if(getsockopt(fd, SOL_SOCKET, SO_TYPE, (char *)(&proto), &proto_len) == -1)
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSocket::SendData(const char *buffer)
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
  if(callback(callback_context, socket, "") == 0)
    goto error;

  (*handler_count)--;
  return;

  error:
    delete socket;
    socket = NULL;
    (*handler_count)--;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUListener::MCUListener(MCUListenerType type, MCUSocket *_socket, mcu_listener_cb *_callback, void *_callback_context)
{
  running = FALSE;

  listener_type = type;
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
#   ifdef _WIN32
      if(!tcp_thread->WaitForTermination(2000))
        tcp_thread->Terminate(); // because socket->Accept() blocks
#   else
      tcp_thread->WaitForTermination(10000);
#   endif

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

MCUListener * MCUListener::Create(MCUListenerType type, const PString & host, int port, mcu_listener_cb *callback, void *callback_context)
{
  int proto;
  if(type == MCU_LISTENER_TCP_SERVER || type == MCU_LISTENER_TCP_CLIENT)
    proto = SOCK_STREAM;
  else
    proto = SOCK_DGRAM;

  if(type == MCU_LISTENER_TCP_SERVER && !MCUSocket::IsLocalHost(host))
  {
    MCUTRACE(1, "MCUListener incorrect host " << host << ", this is not a local address");
    return NULL;
  }

  MCUSocket *socket = MCUSocket::Create(proto, host, port);
  if(socket == NULL)
  {
    MCUTRACE(1, "MCU listener cannot create socket");
    return NULL;
  }
  if(type == MCU_LISTENER_TCP_SERVER && !socket->Listen())
  {
    MCUTRACE(1, "MCU listener cannot create socket");
    delete socket;
    return NULL;
  }
  if(type == MCU_LISTENER_TCP_CLIENT && !socket->Connect())
  {
    MCUTRACE(1, "MCU listener cannot create socket");
    delete socket;
    return NULL;
  }

  return MCUListener::Create(type, socket, callback, callback_context);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUListener * MCUListener::Create(MCUListenerType type, MCUSocket *socket, mcu_listener_cb *callback, void *callback_context)
{
  MCUTRACE(1, "MCU listener ("+socket->GetAddress()+"): " << "create");
  MCUListener *list = new MCUListener(type, socket, callback, callback_context);
  return list;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUListener::Send(const char *buffer)
{
  return socket->SendData(buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUListener::ListenerThread(PThread &, INT)
{
#ifndef _WIN32
  signal(SIGPIPE, SIG_IGN);
#endif

  running = TRUE;
  if(listener_type == MCU_LISTENER_TCP_SERVER)
  {
    while(running)
    {
      MCUSocket *client_socket = socket->Accept();

      if(!running) return; // ??

      if(client_socket == NULL)
        continue;

      handler_count++;
      new MCUListenerHandler(callback, callback_context, client_socket, &handler_count);
    }
  }
  else if(listener_type == MCU_LISTENER_TCP_CLIENT)
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
