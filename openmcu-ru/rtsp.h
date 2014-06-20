
#ifndef _MCU_RTSP_H
#define _MCU_RTSP_H

enum RtspStates
{
  RTSP_NONE,
  RTSP_CONNECT,
  RTSP_DESCRIBE,
  RTSP_SETUP_AUDIO,
  RTSP_SETUP_VIDEO,
  RTSP_PLAY,
  RTSP_PLAYING,
  RTSP_RELEASED
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCURtspConnection : public MCUSipConnection
{
  public:
    MCURtspConnection(MCUSipEndPoint *_sep, MCUH323EndPoint *_ep, PString _callToken);
    ~MCURtspConnection();

    int Connect(PString room, PString _ruri_str);
    int ProcessShutdown(CallEndReason reason = EndedByRemoteUser);
    void CleanUpOnCallEnd();
    void LeaveMCU();

  protected:

    RtspStates rtsp_state;

    int SendSetup(int pt);
    int SendPlay();
    int SendOptions();
    int SendTeardown();
    int SendDescribe();
    int SendRequest(int fd, char *request, PString method_name);
    int RecvData(int fd, char *buffer, int bufferSize);

    int OnReceived(msg_t *msg);
    int OnDescribeResponse(msg_t *msg);
    int OnSetupResponse(msg_t *msg);
    int OnPlayResponse(msg_t *msg);

    int socket_fd;
    int cseq;
    int rtsp_terminating;
    PString rtsp_session_str;

    int CreateSocket();
    PThread *rtsp_thread;
    PDECLARE_NOTIFIER(PThread, MCURtspConnection, RtspListener);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_RTSP_H
