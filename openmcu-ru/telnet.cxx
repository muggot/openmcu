/*
 * telnet.cxx
 *
 * Copyright (C) 2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
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
 *
 */

#include "precompile.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUTelnetSessionCleaner : public PThread
{
  public:
    MCUTelnetSessionCleaner(MCUTelnetServer *_server, MCUTelnetSession *_session)
      : PThread(10000, AutoDeleteThread, NormalPriority, "MCU Telnet Session Cleaner")
    {
      server = _server;
      session = _session;
      Resume();
    }

    void Main()
    {
      MCUTelnetSessionList & sessionList = server->GetSessionList();
      MCUTelnetSessionList::shared_iterator it = sessionList.Find(session);
      if(it != sessionList.end())
      {
        if(sessionList.Erase(it))
        {
          delete session;
          session = NULL;
        }
      }
    }

  protected:
    MCUTelnetServer *server;
    MCUTelnetSession *session;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUTelnetServer::MCUTelnetServer()
{
  trace_section = "Telnet server: ";
  StartListeners();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUTelnetServer::~MCUTelnetServer()
{
  RemoveListeners();
  for(MCUTelnetSessionList::shared_iterator it = sessionList.begin(); it != sessionList.end(); ++it)
  {
    MCUTelnetSession *session = *it;
    if(sessionList.Erase(it))
      delete session;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUTelnetServer::StartListeners()
{
  PWaitAndSignal m(listenerListMutex);

  MCUConfig cfg("Telnet Server");
  if(cfg.GetBoolean(EnableKey, TRUE) == FALSE)
  {
    RemoveListeners();
    return;
  }
  PStringArray listenerArray = cfg.GetString(TelnetListenerKey, TelnetDefaultListener).Tokenise(",");

  // delete listeners
  for(MCUListenerList::shared_iterator it = listenerList.begin(); it != listenerList.end(); ++it)
  {
    if(listenerArray.GetStringsIndex(it.GetName()) == P_MAX_INDEX)
    {
      MCUListener *listener = *it;
      if(listenerList.Erase(it))
        delete listener;
    }
  }
  // add listeners
  for(int i = 0; i < listenerArray.GetSize(); i++)
    AddListener(listenerArray[i]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUTelnetServer::RemoveListeners()
{
  PWaitAndSignal m(listenerListMutex);
  for(MCUListenerList::shared_iterator it = listenerList.begin(); it != listenerList.end(); ++it)
  {
    MCUListener *listener = *it;
    if(listenerList.Erase(it))
      delete listener;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUTelnetServer::AddListener(const PString & address)
{
  PWaitAndSignal m(listenerListMutex);

  if(listenerList.Find(address) != listenerList.end())
    return;

  MCUURL url(address);
  PString socket_host = url.GetHostName();
  unsigned socket_port = url.GetPort().AsInteger();

  MCUListener *listener = MCUListener::Create(MCU_LISTENER_TCP_SERVER, socket_host, socket_port, OnReceived_wrap, this);
  if(listener)
    listenerList.Insert(listener, (long)listener, address);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUTelnetServer::OnReceived(MCUSocket *socket, PString data)
{
  PWaitAndSignal m(sessionListMutex);

  MCUTelnetSessionList::shared_iterator it = sessionList.Find(socket->GetAddress());
  if(it != sessionList.end())
  {
    MCUTRACE(1, trace_section << "error");
    return 0;
  }

  MCUTelnetSession *session = MCUTelnetSession::Create(this, socket);
  if(session == NULL)
    return 0;
  sessionList.Insert(session, (long)session, socket->GetAddress());
  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUTelnetSession::MCUTelnetSession(MCUTelnetServer *_server, MCUSocket *socket)
{
  server = _server;
  trace_section = "Telnet Session "+socket->GetAddress()+": ";

  state = 1;
  cur_path = "# ";
  opt_echo = FALSE;

  MCUConfig cfg("Telnet Server");
  auth.username = cfg.GetString(UserNameKey, "admin");
  auth.password = cfg.GetString(PasswordKey);

  // create listener
  listener = MCUListener::Create(MCU_LISTENER_TCP_CLIENT, socket, OnReceived_wrap, this);

  MCUTRACE(1, trace_section << "create");
  ProcessState("");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUTelnetSession::~MCUTelnetSession()
{
  if(listener)
    delete listener;
  listener = NULL;
  MCUTRACE(1, trace_section << "close");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUTelnetSession * MCUTelnetSession::Create(MCUTelnetServer *_server, MCUSocket *socket)
{
  MCUTelnetSession *session = new MCUTelnetSession(_server, socket);
  return session;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUTelnetSession::Close()
{
  new MCUTelnetSessionCleaner(server, this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUTelnetSession::OnReceived(MCUSocket *socket, PString data)
{
  if(socket == NULL)
  {
    MCUTRACE(1, trace_section << "connection closed by remote user");
    Close();
    return 0;
  }

  for(int i = 0; i < data.GetLength(); ++i)
  {
    switch((unsigned char)data[i])
    {
      case(TEL_LF):
        break;
      case(TEL_CR):
        echobuf += "\r\n";
        OnReceivedData(databuf);
        databuf = "";
        break;
      case(TEL_BACKSPACE):
        if(databuf.GetLength() > 0)
        {
          databuf.Delete(databuf.GetLength()-1, 1);
          if(!Sendf("%c%c%c", TEL_BACKSPACE, TEL_SPACE, TEL_BACKSPACE))
            return 0;
        }
        break;
      case(TEL_DELETE):
      case(TEL_ESC):
        OnReceivedMotion(data, i);
        break;
      case(TEL_IAC):
        OnReceivedIAC(data, i);
        break;
      case(TEL_SGA):
        Close();
        return 1;
        break;
      default:
        databuf += data[i];
        echobuf += data[i];
        break;
    }
  }

  if(!SendEcho())
    return 0;

  return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUTelnetSession::OnReceivedIAC(const PString & data, int & i)
{
  unsigned char c1 = data[++i];
  unsigned char c2 = 0;
  switch(c1)
  {
    case(TEL_IP):
      Close();
      break;
    case(TEL_WILL):
      c2 = data[++i];
      if(c2 == TEL_ECHO)
        opt_echo = TRUE;
      break;
    default:
      ++i;
      break;
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUTelnetSession::OnReceivedMotion(const PString & data, int & i)
{
  unsigned char c1 = data[++i];
  if(c1 == 91) // "["
  {
    if(data[i+2] == 126)
      i += 2;
    else
      i += 1;
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUTelnetSession::ProcessState(const PString & data)
{
  switch(state)
  {
    case(1):
      if(!Sendf("%c%c%c", TEL_IAC, TEL_DO, TEL_ECHO))
        return FALSE;
      state = 2;
    case(2):
      if(!Send("Login: "))
        return FALSE;
      state = 3;
      return TRUE;
    case(3):
      username_recv = data;
      if(!opt_echo && !Sendf("%c%c%c", TEL_IAC, TEL_WILL, TEL_ECHO))
        return FALSE;
      if(!Send("Password: "))
        return FALSE;
      state = 4;
      return TRUE;
    case(4):
      password_recv = data;
      if(!opt_echo && !Sendf("%c%c%c", TEL_IAC, TEL_WONT, TEL_ECHO))
        return FALSE;
      if(!Send("\r\n"))
        return FALSE;
      if(username_recv != auth.username || password_recv != auth.password)
      {
        if(!Send("Login incorrect\r\n\r\n"))
          return FALSE;
        auth.attempts++;
        if(auth.attempts == 3)
        {
          Close();
          return FALSE;
        }
        state = 2;
        return ProcessState("");
      }
      state = 0;
      break;
  }

  if(!Sendf("%s%s", (const char *)TEL_WELCOME, (const char *)cur_path))
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUTelnetSession::OnReceivedData(const PString & data)
{
  MCUTRACE(6, trace_section << "recv " << databuf.GetLength() << " bytes\n" << databuf);

  if(!SendEcho())
    return FALSE;

  if(state)
    return ProcessState(data);

  PString rdata;
  if(!OpenMCU::Current().OTFControl(data, rdata))
    rdata += "error!";

  rdata += "\r\n";
  rdata += cur_path;

  if(!Send((const char *)rdata))
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUTelnetSession::SendEcho()
{
  if(opt_echo && echobuf.GetLength() != 0)
  {
    if(state != 4 && !Send((const char *)echobuf))
      return FALSE;
    else
      echobuf = "";
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUTelnetSession::Send(const char *buffer)
{
  if(listener->Send(buffer) == FALSE)
  {
    Close();
    return FALSE;
  }

  MCUTRACE(6, trace_section << "send " << strlen(buffer) << " bytes\n" << buffer);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUTelnetSession::Sendf(const char *format, ...)
{
  char buffer[65536];
  int buffer_size = 65535;
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, buffer_size, format, args);
  va_end(args);
  return Send(buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

