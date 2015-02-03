
#ifndef _MCU_SOCKET_H
#define _MCU_SOCKET_H

#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/socket.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUSocket
{
  public:
    MCUSocket(int fd, int proto, PString host, int port);
    ~MCUSocket();

    enum SocketType
    {
      NONE = 0,
      TCP_CLIENT,
      TCP_SERVER
    };

    static MCUSocket * Create(int proto, PString host, int port);
    static MCUSocket * Create(int fd);

    MCUSocket * Accept();

    BOOL SendData(char *buffer);
    BOOL RecvData(PString & data);
    BOOL ReadData(PString & data);

    static BOOL TestSocket(int fd);
    static BOOL GetSocketAddress(int fd, int & proto, PString & host, int & port);

    static BOOL GetFromIP(PString & local_ip, PString remote_host, PString remote_port);
    static BOOL GetHostIP(PString & ip, PString host, PString port = "");

    PString GetAddress()
    { return socket_address; }

    PString GetHost()
    { return socket_host; }

    PString GetPort()
    { return socket_port; }

    int GetProto()
    { return socket_proto; }

    SocketType GetType()
    { return socket_type; }

    int GetSocket()
    { return socket_fd; }

  protected:
    BOOL Open();
    BOOL Connect();
    BOOL Listen();

    PString trace_section;

    SocketType socket_type;
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
    MCUListener(MCUSocket *_socket, mcu_listener_cb *callback, void *callback_context);
    ~MCUListener();

    static MCUListener * Create(int proto, const PString & host, int port, mcu_listener_cb *callback, void *callback_context);
    static MCUListener * Create(MCUSocket *socket, mcu_listener_cb *callback, void *callback_context);

    BOOL Send(char *buffer);

    BOOL IsRunning()
    { return running; }

    PString GetSocketAddress()
    { return socket->GetAddress(); }

    PString GetSocketHost()
    { return socket->GetHost(); }

    PString GetSocketPort()
    { return socket->GetPort(); }

  protected:
    BOOL running;
    PString trace_section;

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
