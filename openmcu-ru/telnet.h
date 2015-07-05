
#include "precompile.h"

#ifndef _MCU_TELNET_H
#define _MCU_TELNET_H

#include "sockets.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

const unsigned char TEL_ECHO       = 1;
const unsigned char TEL_SGA        = 3;   // suppress go ahead
const unsigned char TEL_BACKSPACE  = 8;
const unsigned char TEL_LF         = 10;
const unsigned char TEL_CR         = 13;
const unsigned char TEL_ESC        = 27;
const unsigned char TEL_NAWS       = 31;
const unsigned char TEL_SPACE      = 32;
const unsigned char TEL_LINEMODE   = 34;
const unsigned char TEL_DELETE     = 127;
const unsigned char TEL_IP         = 244; // suspend, interrupt or abort the process
const unsigned char TEL_WILL       = 251;
const unsigned char TEL_WONT       = 252;
const unsigned char TEL_DO         = 253;
const unsigned char TEL_DONT       = 254;
const unsigned char TEL_IAC        = 255;

const PString TEL_WELCOME = "OpenMCU-ru/"+PString(MAJOR_VERSION)+"."+PString(MINOR_VERSION)+"."+PString(BUILD_NUMBER)+" "+"Copyright (c) 2015 by OpenMCU-ru Team\r\n"
                            +"git revision: "
#ifdef GIT_REVISION
                            +MCU_STRINGIFY(GIT_REVISION)
#endif
                            +"\r\n";


////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUTelnetServer;

class MCUTelnetSession
{
    MCUTelnetSession(MCUTelnetServer *_server, MCUSocket *socket);

  public:
    ~MCUTelnetSession();

    static MCUTelnetSession *Create(MCUTelnetServer *_server, MCUSocket *socket);
    void Close();

  protected:
    PString trace_section;
    PString databuf;
    PString echobuf;
    PString cur_path;
    PTime start_time;

    BOOL opt_echo;

    HTTPAuth auth;
    int state;

    PString username_recv;
    PString password_recv;

    BOOL Send(const char *buffer);
    BOOL Sendf(const char *format, ...);
    BOOL SendEcho();

    BOOL ProcessState(const PString & data);
    BOOL OnReceivedIAC(const PString & data, int & i);
    BOOL OnReceivedMotion(const PString & data, int & i);
    BOOL OnReceivedData(const PString & data);

    static int OnReceived_wrap(void *context, MCUSocket *socket, PString data)
    { return ((MCUTelnetSession *)context)->OnReceived(socket, data); }
    int OnReceived(MCUSocket *socket, PString data);

    MCUTelnetServer *server;
    MCUListener *listener;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUTelnetServer
{
  public:
    MCUTelnetServer();
    ~MCUTelnetServer();

    void StartListeners();
    void RemoveListeners();
    void AddListener(const PString & address);

    MCUTelnetSessionList & GetSessionList()
    { return sessionList; }

  protected:

    PString trace_section;

    static int OnReceived_wrap(void *context, MCUSocket *socket, PString data)
    { return ((MCUTelnetServer *)context)->OnReceived(socket, data); }
    int OnReceived(MCUSocket *socket, PString data);

    MCUListenerList listenerList;
    MCUTelnetSessionList sessionList;

    PMutex listenerListMutex;
    PMutex sessionListMutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_TELNET_H
