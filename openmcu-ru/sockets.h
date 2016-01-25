/*
 * sockets.h
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

#ifndef _MCU_SOCKET_H
#define _MCU_SOCKET_H

////////////////////////////////////////////////////////////////////////////////////////////////////

enum MCUListenerType
{
  NONE = 0,
  MCU_LISTENER_TCP_CLIENT,
  MCU_LISTENER_TCP_SERVER
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUSocket
{
  public:
    MCUSocket(int fd, int proto, const PString & host, int port);
    ~MCUSocket();

    static MCUSocket * Create(int proto, PString host, int port);
    static MCUSocket * Create(int fd);

    BOOL Open();
    BOOL Connect();
    BOOL Listen();

    MCUSocket * Accept();

    BOOL SendData(const char *buffer);

    BOOL RecvData(PString & data);
    BOOL ReadData(PString & data);

    static BOOL TestSocket(int fd);
    static BOOL GetSocketAddress(int fd, int & proto, PString & host, int & port);

    static BOOL GetFromIP(PString & ip, const PString & host, const PString & port);
    static BOOL GetHostIP(PString & ip, const PString & host, const PString & port = "");

    static BOOL IsValidHost(const PString & host);
    static BOOL IsLocalHost(const PString & host);

    PString GetAddress()
    { return socket_address; }

    PString GetHost()
    { return socket_host; }

    PString GetPort()
    { return socket_port; }

    int GetProto()
    { return socket_proto; }

    int GetSocket()
    { return socket_fd; }

  protected:
    PString trace_section;

    PString socket_address;
    PString socket_host;
    int socket_port;
    int socket_proto;
    int socket_timeout_sec;
    int socket_timeout_usec;

    int socket_fd;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef int mcu_listener_cb(void *callback_context, MCUSocket *socket, PString data);

class MCUListener
{
  public:
    MCUListener(MCUListenerType type, MCUSocket *_socket, mcu_listener_cb *callback, void *callback_context);
    ~MCUListener();

    static MCUListener * Create(MCUListenerType type, const PString & host, int port, mcu_listener_cb *callback, void *callback_context);
    static MCUListener * Create(MCUListenerType type, MCUSocket *socket, mcu_listener_cb *callback, void *callback_context);

    BOOL Send(const char *buffer);

    BOOL IsRunning()
    { return running; }

    PString GetSocketAddress()
    { return socket->GetAddress(); }

    PString GetSocketHost()
    { return socket->GetHost(); }

    PString GetSocketPort()
    { return socket->GetPort(); }

    MCUListenerType GetType()
    { return listener_type; }

  protected:
    BOOL running;
    PString trace_section;

    MCUListenerType listener_type;
    MCUSocket *socket;

    mcu_listener_cb *callback;
    void *callback_context;

    // handler threads count
    int handler_count;

    PThread *tcp_thread;
    PDECLARE_NOTIFIER(PThread, MCUListener, ListenerThread);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUListenerHandler : public PThread
{
  public:
    MCUListenerHandler(mcu_listener_cb *_callback, void *_callback_context, MCUSocket *_socket, int *_handler_count)
      : PThread(10000, AutoDeleteThread, NormalPriority, "tcp_connection_handler:%0x")
    {
      callback = _callback;
      callback_context = _callback_context;
      socket = _socket;
      handler_count = _handler_count;
      Resume();
    }

  protected:
    void Main();
    mcu_listener_cb *callback;
    void *callback_context;
    int *handler_count;
    MCUSocket *socket;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_SOCKET_H
