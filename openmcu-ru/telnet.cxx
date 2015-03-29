
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
      MCUSharedList<MCUTelnetSession> & sessionList = server->GetSessionList();
      MCUSharedList<MCUTelnetSession>::shared_iterator it = sessionList.Find(session);
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUTelnetServer::StartListeners()
{
  PWaitAndSignal m(telnetMutex);
  RemoveListeners();
  AddListener("0.0.0.0:1423");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUTelnetServer::RemoveListeners()
{
  PWaitAndSignal m(telnetMutex);
  for(ListenerList::shared_iterator it = listenerList.begin(); it != listenerList.end(); ++it)
  {
    MCUListener *listener = *it;
    if(listenerList.Erase(it))
      delete listener;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUTelnetServer::AddListener(PString address)
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

  PWaitAndSignal m(telnetMutex);

  MCUListener *listener = MCUListener::Create(MCU_LISTENER_TCP_SERVER, socket_host, socket_port, OnReceived_wrap, this);
  if(listener)
    listenerList.Insert(listener, (long)listener, address);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUTelnetServer::OnReceived(MCUSocket *socket, PString data)
{
  PWaitAndSignal m(telnetMutex);

  MCUSharedList<MCUTelnetSession>::shared_iterator it = sessionList.Find(socket->GetAddress());
  if(it != sessionList.end())
    return 0;

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
  auth.username = "admin";
  auth.password = "admin";
  cur_path = "# ";

  opt_echo = FALSE;

  // create listener
  listener = MCUListener::Create(MCU_LISTENER_TCP_CLIENT, socket, OnReceived_wrap, this);

  ProcessState("");

  MCUTRACE(1, trace_section << "create");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUTelnetSession::~MCUTelnetSession()
{
  if(listener)
    delete listener;
  listener = NULL;
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

  for(PINDEX i = 0; i < data.GetLength(); ++i)
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
          if(!Send("%c%c%c", TEL_BACKSPACE, TEL_SPACE, TEL_BACKSPACE))
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
      if(!Send("%c%c%c", TEL_IAC, TEL_DO, TEL_ECHO))
        return FALSE;
      state = 2;
    case(2):
      if(!Send("%s", "Login: "))
        return FALSE;
      state = 3;
      return TRUE;
    case(3):
      username_recv = data;
      if(!opt_echo && !Send("%c%c%c", TEL_IAC, TEL_WILL, TEL_ECHO))
        return FALSE;
      if(!Send("%s", "Password: "))
        return FALSE;
      state = 4;
      return TRUE;
    case(4):
      password_recv = data;
      if(!opt_echo && !Send("%c%c%c", TEL_IAC, TEL_WONT, TEL_ECHO))
        return FALSE;
      if(!Send("%s", "\r\n"))
        return FALSE;
      if(username_recv != auth.username || password_recv != auth.password)
      {
        if(!Send("%s", "Login incorrect\r\n\r\n"))
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

  if(!Send("%s%s", (const char *)TEL_WELCOME, (const char *)cur_path))
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUTelnetSession::OnReceivedData(const PString & data)
{
  MCUTRACE(1, trace_section << "recv " << databuf.GetLength() << " bytes\n" << databuf);

  if(!SendEcho())
    return FALSE;

  if(state)
    return ProcessState(data);

  //////////////////////////////////////////////////

  //////////////////////////////////////////////////

  if(!Send("%s", (const char *)cur_path))
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUTelnetSession::SendEcho()
{
  if(opt_echo && echobuf.GetLength() != 0)
  {
    if(state != 4 && !Send("%s", (const char *)echobuf))
      return FALSE;
    else
      echobuf = "";
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUTelnetSession::Send(const char* format, ...)
{
  char buffer[65536];
  int buffer_size = 65535;
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, buffer_size, format, args);
  va_end(args);

  if(listener->Send(buffer) == FALSE)
  {
    Close();
    return FALSE;
  }

  MCUTRACE(1, trace_section << "send " << strlen(buffer) << " bytes\n" << buffer);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

