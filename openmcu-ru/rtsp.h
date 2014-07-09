
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

    virtual void CleanUpOnCallEnd();
    virtual void LeaveMCU();
    void ProcessShutdown(CallEndReason reason = EndedByRemoteUser);

  protected:

    RtspStates rtsp_state;

    int SendSetup(int pt);
    int SendPlay();
    int SendOptions();
    int SendTeardown();
    int SendDescribe();
    int SendRequest(int fd, char *request, PString method_name);

    int OnResponseReceived(const msg_t *msg);
    int OnResponseDescribe(const msg_t *msg);
    int OnResponseSetup(const msg_t *msg);
    int OnResponsePlay(const msg_t *msg);

    int cseq;
    int rtsp_terminating;
    PString rtsp_session_str;

    int socket_fd;
    int CreateSocket();

    PThread *rtsp_thread;
    PDECLARE_NOTIFIER(PThread, MCURtspConnection, RtspListener);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_RTSP_H
