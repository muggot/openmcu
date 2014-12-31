
#ifndef _MCU_RTSP_H
#define _MCU_RTSP_H

////////////////////////////////////////////////////////////////////////////////////////////////////

static const PString METHOD_OPTIONS    = "OPTIONS";
static const PString METHOD_DESCRIBE   = "DESCRIBE";
static const PString METHOD_SETUP      = "SETUP";
static const PString METHOD_PLAY       = "PLAY";
static const PString METHOD_TEARDOWN   = "TEARDOWN";

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceStreamMember;
class MCUListener;

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCURtspConnection : public MCUSipConnection
{
  friend class MCURtspServer;

  public:
    virtual void CleanUpOnCallEnd();
    virtual void LeaveMCU();

  protected:

    enum RtspStates
    {
      RTSP_NONE = 0,
      RTSP_CONNECT,
      RTSP_DESCRIBE,
      RTSP_SETUP,
      RTSP_SETUP_AUDIO,
      RTSP_SETUP_VIDEO,
      RTSP_PLAY,
      RTSP_PLAYING,
      RTSP_TEARDOWN
    };
    RtspStates rtsp_state;

    MCURtspConnection(MCUSipEndPoint *_sep, MCUH323EndPoint *_ep, PString _callToken);
    ~MCURtspConnection();

    BOOL Connect(PString room, PString address);
    BOOL Connect(PString address, int socket_fd, const msg_t *msg);

    void ProcessShutdown(CallEndReason reason = EndedByLocalUser);

    void CreateLocalSipCaps();
    BOOL CreateInboundCaps();

    BOOL SendSetup(int pt);
    BOOL SendPlay();
    BOOL SendOptions();
    BOOL SendTeardown();
    BOOL SendDescribe();

    BOOL OnResponseReceived(const msg_t *msg);
    BOOL OnResponseDescribe(const msg_t *msg);
    BOOL OnResponseSetup(const msg_t *msg);
    BOOL OnResponsePlay(const msg_t *msg);

    BOOL OnRequestReceived(const msg_t *msg);
    BOOL OnRequestDescribe(const msg_t *msg);
    BOOL OnRequestSetup(const msg_t *msg);
    BOOL OnRequestPlay(const msg_t *msg);
    BOOL OnRequestTeardown(const msg_t *msg);
    BOOL OnRequestOptions(const msg_t *msg);

    BOOL RtspCheckAuth(const msg_t *msg);
    BOOL ParseTransportStr(SipCapability *sc, PString & transport_str);
    void AddHeaders(char *buffer, PString method_name="");
    BOOL SendRequest(char *buffer);

    int cseq;
    PString rtsp_session_str;
    PString rtsp_path;
    PString luri_str;

    static int OnReceived_wrap(void *context, int socket_fd, PString address, PString data)
    { return ((MCURtspConnection *)context)->OnReceived(socket_fd, address, data); }
    int OnReceived(int socket_fd, PString address, PString data);

    MCUListener *listener;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCURtspServer
{
  public:
    MCURtspServer(MCUH323EndPoint *ep, MCUSipEndPoint *sep);
    ~MCURtspServer();

    bool CreateConnection(const PString & room, const PString & address, const PString & callToken);

    void InitListeners();

  protected:
    void AddListener(PString address);
    void RemoveListener(PString address);
    void ClearListeners();

    BOOL CreateConnection(PString address, int socket_fd, const msg_t *msg);
    void SendResponse(int socket_fd, const PString & address, const msg_t *msg, const PString & status_str);

    PString trace_section;

    static int OnReceived_wrap(void *context, int socket_fd, PString address, PString data)
    { return ((MCURtspServer *)context)->OnReceived(socket_fd, address, data); }
    int OnReceived(int socket_fd, PString address, PString data);

    typedef std::map<PString /* address */, MCUListener *> ListenersMapType;
    ListenersMapType Listeners;

    MCUH323EndPoint *ep;
    MCUSipEndPoint *sep;

    PMutex rtspMutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef int mcu_listener_cb(void *callback_context, int socket_fd, PString address, PString data);

class MCUListener
{
  public:
    MCUListener(PString address, mcu_listener_cb *callback, void *callback_context);
    MCUListener(int socket_fd, PString address, mcu_listener_cb *callback, void *callback_context);
    ~MCUListener();

    enum ListenerType
    {
      TCP_LISTENER_CLIENT,
      TCP_LISTENER_SERVER
    };

    static MCUListener * Create(PString address, mcu_listener_cb *callback, void *callback_context);
    static MCUListener * Create(int client_fd, PString address, mcu_listener_cb *callback, void *callback_context);

    BOOL Send(char *buffer);

    static BOOL Send(int fd, char *buffer);
    static int ReadData(int fd, char *buffer, int buffer_size);
    static int RecvData(int fd, char *buffer, int buffer_size);
    static BOOL ReadSerialData(int fd, PString & data);
    static BOOL RecvSerialData(int fd, PString & data);
    static BOOL TestSocket(int fd);
    static BOOL GetSocketAddress(int fd, PString & address);

    BOOL IsRunning()
    { return running; }

    PString GetAddress()
    { return socket_address; }

    ListenerType GetType()
    { return listener_type; }

    int GetSocket()
    { return socket_fd; }

  protected:
    BOOL CreateTCPServer();
    BOOL CreateTCPClient();

    BOOL running;
    PString trace_section;

    ListenerType listener_type;
    PString socket_address;

    PString socket_host;
    unsigned socket_port;
    unsigned socket_timeout_sec;
    unsigned socket_timeout_usec;

    int socket_fd;
    mcu_listener_cb *callback;
    void *callback_context;

    int handler_count; // handler threads count
    PDECLARE_NOTIFIER(PThread, MCUListener, TCPConnectionHandler);

    PThread *tcp_thread;
    PDECLARE_NOTIFIER(PThread, MCUListener, TCPListener);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceStreamMember : public ConferenceMember
{
  PCLASSINFO(ConferenceStreamMember, ConferenceMember);

  public:
    ConferenceStreamMember(Conference *_conference, MCUH323EndPoint & ep, const PString & _callToken, const PString & _name)
      : ConferenceMember(_conference, (void *)this), ep(ep)
    {
      conference = _conference;
      callToken = _callToken;
      name = _name;
      conference->AddMember(this);
    }
    ~ConferenceStreamMember()
    {
    }

    virtual void Close()
    {
      MCUH323Connection * conn = (MCUH323Connection *)ep.FindConnectionWithLock(callToken);
      if(conn != NULL)
      {
        conn->LeaveMCU();
        conn->Unlock();
      }
    }

    virtual ConferenceConnection * CreateConnection()
    { return new ConferenceConnection(this); }

    virtual PString GetName() const
    { return "stream "+name; }

    virtual MemberTypes GetType()
    { return MEMBER_TYPE_STREAM; }

    virtual BOOL IsVisible() const
    { return FALSE; }

  protected:
    MCUH323EndPoint & ep;

};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_RTSP_H
