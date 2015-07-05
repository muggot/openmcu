
#include "precompile.h"

#ifndef _MCU_H323_H
#define _MCU_H323_H

#include "utils.h"
#include "mcu_caps.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

PString H323GetAliasUserName(const H225_ArrayOf_AliasAddress & aliases);
PString H323GetAliasDisplayName(const H225_ArrayOf_AliasAddress & aliases);

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUH323EndPoint : public H323EndPoint
{
  PCLASSINFO(MCUH323EndPoint, H323EndPoint);

  public:
    MCUH323EndPoint(ConferenceManager & conferenceManager);
    ~MCUH323EndPoint();

    PString IncomingConferenceRequest(H323Connection & connection, const H323SignalPDU & setupPDU, unsigned & videoMixerNumber);

    // overrides from H323EndPoint
    virtual H323Connection * CreateConnection(unsigned callReference,void * userData,H323Transport * transport,H323SignalPDU * setupPDU);
    virtual void TranslateTCPAddress(PIPSocket::Address &localAddr, const PIPSocket::Address &remoteAddr);

    MCUH323Connection * FindConnectionWithoutLock(const PString & token);
    MCUH323Connection * FindConnectionWithLock(const PString & token);
    virtual BOOL HasConnection(const PString & token);

    virtual BOOL ClearCall(const PString & token, H323Connection::CallEndReason reason = H323Connection::EndedByLocalUser);
    virtual BOOL ClearCallSynchronous(const PString & token, H323Connection::CallEndReason reason, PSyncPoint * sync);
    virtual void ClearAllCalls(H323Connection::CallEndReason reason = H323Connection::EndedByLocalUser, BOOL wait = TRUE);

    virtual void CleanUpConnections();

    virtual void OnConnectionCleared(H323Connection & connection, const PString & token);
    BOOL OnConnectionCreated(MCUH323Connection * conn);

    virtual BOOL OnConnectionForwarded(H323Connection & connection, const PString & forwardParty, const H323SignalPDU & pdu);

    // new functions
    void Initialise(PConfig & cfg);
    void InitialiseCapability();

    BOOL HasListener(PString host, PString port);

    PString GetRoomStatus(const PString & block);
    PString GetRoomStatusJS();
    PString GetRoomStatusJSStart();
    MCUJSON* GetVideoMixerConfiguration(MCUVideoMixer * mixer, int number);
    MCUJSON* GetMemberDataJS(ConferenceMember * member);
    PString GetConferenceOptsJavascript(Conference & c);
    PString GetMemberListOptsJavascript(Conference & conference);
    PString GetAddressBookOptsJavascript();
    int SetMemberVideoMixer(Conference & conference, ConferenceMember * victim, int newVideoMixer);
    PString GetRoomList(const PString & block);
    PString SetRoomParams(const PStringToString & data);
    void UnmoderateConference(Conference & conference);
    PString RoomCtrlPage(const PString room);
    PString GetMonitorText();

    ConferenceManager & GetConferenceManager()
    { return conferenceManager; }

    virtual H323Gatekeeper * CreateGatekeeper(H323Transport * transport);

    PString GetGatekeeperHostName();
    PString Invite(PString room, PString memberName);

    void AddCapabilitiesMCU();
    PINDEX AddCapabilitiesMCU(PINDEX descriptorNum, PINDEX simultaneous, const char **caps);

    char **rsCaps;
    char **tsCaps;
    char **rvCaps;
    char **tvCaps;
    char **cvCaps;
    char *listCaps;

#if MCU_VIDEO
    unsigned GetVideoTxQuality() const
    { return videoTxQuality; }

    unsigned GetVideoFrameRate() const
    { return videoFrameRate; }

    BOOL enableVideo;
    unsigned videoFrameRate;
    unsigned videoTxQuality;
#endif

    MCUConnectionList & GetConnectionList()
    { return connectionList; }

    MCUConnectionList & GetConnectionDeleteList()
    { return connectionDeleteList; }

  protected:

    virtual H323Connection * InternalMakeCall(const PString & trasferFromToken, const PString & callIdentity, unsigned capabilityLevel, const PString & remoteParty, H323Transport * transport, PString & newToken, void * userData);

    BOOL behind_masq;
    PIPSocket::Address *masqAddressPtr;
    PString nat_lag_ip;

    PString trace_section;

    ConferenceManager & conferenceManager;
    ConnectionMonitor * connectionMonitor;
    GatekeeperMonitor * gatekeeperMonitor;

    PMutex connectionListMutex;
    MCUConnectionList connectionList;
    MCUConnectionList connectionDeleteList;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class H323CallThread : public PThread
{
  PCLASSINFO(H323CallThread, PThread)
  public:
    H323CallThread(H323EndPoint & endpoint, H323Connection & c, H323Transport & t, const PString & a, const H323TransportAddress & addr)
      : PThread(endpoint.GetSignallingThreadStackSize(), NoAutoDeleteThread, NormalPriority, "H225 Caller:%0x"),
        connection(c), transport(t), alias(a), address(addr)
    {
#ifdef H323_SIGNAL_AGGREGATE
      useAggregator = endpoint.GetSignallingAggregator() != NULL;
      if(!useAggregator)
#endif
        transport.AttachThread(this);
      Resume();
    }

  protected:
    void Main();
    H323Connection     & connection;
    H323Transport      & transport;
    PString              alias;
    H323TransportAddress address;
#ifdef H323_SIGNAL_AGGREGATE
    BOOL                 useAggregator;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class OutgoingAudio : public PChannel
{
  PCLASSINFO(OutgoingAudio, PChannel);

  public:
    OutgoingAudio(MCUH323Connection & conn, unsigned int sr, unsigned _channels);

    BOOL Read(void * buffer, PINDEX amount);
    BOOL Close();

  protected:
    void CreateSilence(void * buffer, PINDEX amount);

    MCUH323Connection & conn;

    unsigned int sampleRate;
    unsigned channels; //1=mono, 2=stereo

    MCUDelay delay;
    PMutex audioChanMutex;
};

////////////////////////////////////////////////////

class IncomingAudio : public PChannel
{
  PCLASSINFO(IncomingAudio, PChannel);

  public:
    IncomingAudio(MCUH323Connection & conn, unsigned int _sampleRate, unsigned _channels);

    BOOL Write(const void * buffer, PINDEX amount);
    BOOL Close();

  protected:
    MCUH323Connection & conn;

    unsigned int sampleRate;
    unsigned channels; //1=mono, 2=stereo

    MCUDelay delay;
    PMutex audioChanMutex;
};

////////////////////////////////////////////////////

class NotifyH245Thread : public PThread
{
  public:
    NotifyH245Thread(Conference & conference, BOOL _join, ConferenceMember * _memberToIgnore);
    void Main();

  protected:
    BOOL join;
    ConferenceMember * memberToIgnore;
    PStringArray tokens;
    int mcuNumber;
    int terminalIdToSend;
};

class MCUH323Connection : public H323Connection
{
  PCLASSINFO(MCUH323Connection, H323Connection);

  public:
    MCUH323Connection(MCUH323EndPoint &, unsigned, void *);
    ~MCUH323Connection();

    PMutex & GetConnectionMutex()
    { return connMutex; }

    PMutex & GetChannelsMutex()
    { return channelsMutex; }

    BOOL Lock()
    {
      MCUConnectionList & connectionList = ep.GetConnectionList();
      MCUConnectionList::shared_iterator it = connectionList.Find(callToken);
      if(it != connectionList.end())
      {
        it.GetCapturedObject();
        return TRUE;
      }
      return FALSE;
    }

    void Unlock()
    {
      MCUConnectionList & connectionList = ep.GetConnectionList();
      connectionList.Release((long)this);
    }

    virtual ConferenceMember * GetConferenceMember()
    { return conferenceMember; }

    virtual Conference * GetConference()
    { return conference; }

    BOOL IsAwaitingSignalConnect()
    { return connectionState == AwaitingSignalConnect; };

    BOOL IsShuttingDown()
    { return connectionState == ShuttingDownConnection; };

    BOOL IsClearing()
    { return clearing; };

    BOOL SetClearing()
    { return sync_bool_compare_and_swap(&clearing, false, true); };

    virtual void SendLogicalChannelMiscCommand(H323Channel & channel, unsigned command);
    virtual void SendLogicalChannelMiscIndication(H323Channel & channel, unsigned command);

    // overrides from H323Connection
    virtual BOOL OnH245_MiscellaneousCommand(const H245_MiscellaneousCommand & pdu /* Received PDU */ );
    virtual BOOL OnH245_MiscellaneousIndication(const H245_MiscellaneousIndication & pdu);

    virtual void AttachSignalChannel(const PString & token, H323Transport * channel, BOOL answeringCall);
    virtual BOOL OpenAudioChannel(BOOL, unsigned, H323AudioCodec & codec);

    virtual BOOL OnStartLogicalChannel(H323Channel & channel);
    virtual void OnClosedLogicalChannel(const H323Channel & channel);

#if MCU_VIDEO
    virtual BOOL OpenVideoChannel(BOOL isEncoding, H323VideoCodec & codec);
#endif

    virtual void OnCreated();
    virtual BOOL ClearCall(CallEndReason reason = EndedByLocalUser);
    virtual BOOL ClearCall(CallEndReason reason, const PString & event);
    virtual void CleanUpOnCallEnd();
    virtual void OnEstablished();
    virtual void OnCleared();
    virtual void SetCallEndReason(CallEndReason reason, PSyncPoint * sync = NULL);

    virtual AnswerCallResponse OnAnswerCall(const PString &, const H323SignalPDU &, H323SignalPDU &);

    virtual void OnUserInputString(const PString & value);
    virtual CallEndReason SendSignalSetup(const PString & alias, const H323TransportAddress & address);
    virtual BOOL OnReceivedSignalSetup(const H323SignalPDU & setupPDU);
    virtual BOOL OnReceivedCallProceeding(const H323SignalPDU & proceedingPDU);
    virtual BOOL OnReceivedSignalConnect(const H323SignalPDU & pdu);

    virtual BOOL OnH245Request(const H323ControlPDU & pdu);
    virtual void SelectDefaultLogicalChannel(unsigned sessionID);
    H323Capability * SelectRemoteCapability(H323Capabilities & capabilities, unsigned sessionID, char **capsName);

    virtual BOOL OnReceivedCapabilitySet(const H323Capabilities & remoteCaps, const H245_MultiplexCapability * muxCap, H245_TerminalCapabilitySetReject & /*rejectPDU*/);
    virtual void OnSendCapabilitySet(H245_TerminalCapabilitySet & /*pdu*/);
    virtual BOOL StartControlNegotiations(BOOL renegotiate = FALSE);
    virtual void OnSetLocalCapabilities();

    virtual void SendUserInput(const PString & value);

    virtual BOOL OnIncomingAudio(const uint64_t & timestamp, const void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels);
    virtual BOOL OnOutgoingAudio(const uint64_t & timestamp, void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels);

    void SetRemoteName(const H323SignalPDU & pdu);
    void SetMemberName();

    PString GetRemoteUserName();

    PString GetRemoteDisplayName()
    { return remoteDisplayName; }

    virtual PString GetMemberName() const
    { return memberName; }

    void SetEndpointDefaultVideoParams(H323VideoCodec & codec);

    virtual void SetupCacheConnection(PString & format,Conference * conf, ConferenceMember * memb);

    virtual RTP_Session * UseSession(
      unsigned sessionID,
      const H245_TransportAddress & pdu,
      H323Channel::Directions dir,
      RTP_QOS * rtpqos = NULL
    );
    virtual H323Channel * CreateRealTimeLogicalChannel(
      const H323Capability & capability, ///< Capability creating channel
      H323Channel::Directions dir,       ///< Direction of channel
      unsigned sessionID,                ///< Session ID for RTP channel
      const H245_H2250LogicalChannelParameters * param,
                                         ///< Parameters for channel
      RTP_QOS * rtpqos = NULL            ///< QoS for RTP
    );

    PString GetEndpointParam(PString param, bool asterisk = true);
    PString GetEndpointParam(PString param, PString defaultValue, bool asterisk = true);

#if MCU_VIDEO
    virtual BOOL OnIncomingVideo(const void * buffer, int width, int height);
    virtual BOOL OnOutgoingVideo(void * buffer, int width, int height, PINDEX & amount);
    virtual void RestartGrabber();
    unsigned videoMixerNumber;
#endif

    PString GetVideoTransmitCodecName() const { return videoTransmitCodecName; }
    PString GetVideoReceiveCodecName() const  { return videoReceiveCodecName; }
    PString GetAudioTransmitCodecName() const { return audioTransmitCodecName; }
    PString GetAudioReceiveCodecName() const  { return audioReceiveCodecName; }

    MCU_RTPChannel * GetAudioReceiveChannel() const { return audioReceiveChannel; }
    MCU_RTPChannel * GetVideoReceiveChannel() const { return videoReceiveChannel; }
    MCU_RTPChannel * GetAudioTransmitChannel() const { return audioTransmitChannel; }
    MCU_RTPChannel * GetVideoTransmitChannel() const { return videoTransmitChannel; }

    // Valid states for the welcome procedure. Note that new states may
    //  be added because the procedure can be customized by subclassing.
    enum WelcomeStates {
      // Reserved, never switch to this state
      NotStartedYet,

      // First state, by default jumps to PlayingConnecting
      PlayingWelcome,

      // Plays connecting wave file, then jumps to CompleteConnection
      PlayingConnecting,

      // Jumping here causes the connection to be joined to requestedRoom
      CompleteConnection,

      // The system jumps here if the join to the conference fails
      JoinFailed,

      // The system jumps here when the member is disconnected
      ConferenceEnded,

      // Use this as the starting value for custom sates in subclasses
      CustomWelcome_FirstUsableValue
    };

    virtual void LogCall(const BOOL accepted = TRUE);

    // Use this function to change the current state. This call fires
    //  the OnWelcomeStateChanged callback immediately.
    void ChangeWelcomeState(int newState);

    const PString & GetRequestedRoom() const { return requestedRoom; };
    void SetRequestedRoom(PString room) { requestedRoom = room; };

    MCUConnectionTypes GetConnectionType() const
    { return connectionType; }

    int rtpInputTimeout;
    int rtpInputLostInterval;
    int rtpInputBytes;

  protected:

    virtual void InternalEstablishedConnectionCheck();

    virtual void JoinConference(const PString & room);
    virtual void SetRequestedRoom();

#if MCU_VIDEO
    virtual BOOL InitGrabber(PVideoInputDevice  * grabber, int frameWidth, int frameHeight, int frameRate);
#endif

    MCUH323EndPoint & ep;

    // Name of the room to join when the welcome procedure ends.
    //  This is usually initialized when the call is answered,
    //  before the welcome procedure begins.
    PString requestedRoom;

    // Room the connection is joined to. It is NULL before the
    //  welcome procedure ends, or after the member is disconnected
    //  from the conference.
    Conference * conference;

    // Object used to treat the connection as a conference member.
    //  It is NULL before the connection is joined to the conference,
    //  or after the member is disconnected from the conference.
    ConferenceMember * conferenceMember;

    // This is the current state of the welcome procedure. It is not
    //  declared using type WelcomeStates to allow the use of custom
    //  enumerated values.
    int welcomeState;

    // Call this function during the OnWelcomeStateChanged callback
    //  to play a new wave file during the welcome procedure. If
    //  useTheFile is FALSE, fileToPlay is empty or the file does
    //  not exist, the currently playing wave file is stopped and
    //  the OnWelcomeWaveEnded callback will be called anyway (as
    //  if an empty wave file has been played).
    void PlayWelcomeFile(BOOL useTheFile, PFilePath & fileToPlay);

    // Called whenever the current state changes. Use welcomeState
    //  to know the current (new) state.
    virtual void OnWelcomeStateChanged();

    // Called by OnOutgoingAudio to process the current welcome
    //  state, even if the member is already joined to a conference.
    virtual void OnWelcomeProcessing();

    // Called when the currently playing wave file has ended. This
    //  is not called if the state changed before the wave file
    //  stopped. Use welcomeState to know the current state.
    virtual void OnWelcomeWaveEnded();

    PString remoteUserName;
    PString remoteDisplayName;
    PString memberName;

    MCUConnectionTypes connectionType;

    // sync_bool_compare_and_swap
    sync_bool volatile clearing;

    // Wave file played during the welcome procedure.
    OpalWAVFile playFile;

    // optional record file used during the welcome procedure
    OpalWAVFile recordFile;
    unsigned int recordSilenceCount;
    unsigned int recordSilenceThreshold;
    unsigned int recordDuration;
    unsigned int recordLimit;

    //PAdaptiveDelay playDelay;

    // True if the state has not changed since the wave file started.
    BOOL wavePlayingInSameState;

    BOOL isMCU;

    PString audioTransmitCodecName;
    PString audioReceiveCodecName;
    PString videoTransmitCodecName;
    PString videoReceiveCodecName;

    MCU_RTPChannel *audioReceiveChannel;
    MCU_RTPChannel *videoReceiveChannel;
    MCU_RTPChannel *audioTransmitChannel;
    MCU_RTPChannel *videoTransmitChannel;

    BOOL CheckVFU();
    PTime vfuSendTime;             // время отправки запроса от MCU
    PTime vfuBeginTime;            // время первого запроса за интервал
    unsigned int vfuInterval;      // interval sec
    unsigned int vfuLimit;         // limit requests for interval
    unsigned int vfuCount;         // count requests for interval
    unsigned int vfuTotalCount;    // count total requests

    Directions direction;
    PString trace_section;

    PString callEndReasonEvent;

    PString dtmfBuffer;

#if MCU_VIDEO
    MCUPVideoInputDevice * videoGrabber;
    MCUPVideoOutputDevice * videoDisplay;
#endif

    PMutex connMutex;
    PMutex channelsMutex;
};

////////////////////////////////////////////////////

class MCUConnection_ConferenceMember : public ConferenceMember
{
  PCLASSINFO(MCUConnection_ConferenceMember, ConferenceMember);
  public:
    MCUConnection_ConferenceMember(Conference * _conference, const PString & _memberName, const PString & _callToken, BOOL _isMCU = FALSE);
    ~MCUConnection_ConferenceMember();

    virtual void SetName(PString newName)
    {
      MCUURL url(newName);
      name = url.GetMemberName();
      nameID = url.GetMemberNameId();
    }

    virtual void SetFreezeVideo(BOOL) const;

    virtual PString GetMonitorInfo(const PString & hdr);

    virtual void SendUserInputIndication(const PString & str);

    virtual void OnReceivedUserInputIndication(const PString & str)
    {
      PTRACE(2, "h323.cxx\tFCWL from OnReceivedUserInputIndication:" << callToken);
      MCUH323Connection * conn = ep.FindConnectionWithLock(callToken);
      if (conn != NULL)
      {
        conn->SendUserInput(str); 
        conn->Unlock();
      }
    }

    virtual void SetChannelPauses(unsigned mask);
    virtual void UnsetChannelPauses(unsigned mask);
    virtual void SetChannelState(unsigned newMask);

  protected:
    PStringStream iISequence; // input indication sequence
    MCUH323EndPoint & ep;
};

////////////////////////////////////////////////////

#if MCU_VIDEO

class MCUPVideoInputDevice : public PVideoInputDevice
{
 PCLASSINFO(MCUPVideoInputDevice, PVideoInputDevice);
 public:
  /** Create a new MCU video input device.
   */
    MCUPVideoInputDevice(MCUH323Connection & conn);

    /**Open the device given the device name.
      */
    BOOL Open(
      const PString & title,        /// Device name to open
      BOOL startImmediate = TRUE    /// Immediately start device
    );

    /**Determine of the device is currently open.
      */
    BOOL IsOpen() ;

    /**Close the device.
      */
    BOOL Close();

    /**Start the video device I/O.
      */
    BOOL Start();

    /**Stop the video device I/O capture.
      */
    BOOL Stop();

    /**Determine if the video device I/O capture is in progress.
      */
    BOOL IsCapturing();

    /**Get a list of all of the drivers available.
      */
    static PStringList GetInputDeviceNames();

#if PTLIB_MAJOR == 2 && PTLIB_MINOR > 0
    virtual PStringArray GetDeviceNames() const
      { return GetInputDeviceNames(); }
#else
    virtual PStringList GetDeviceNames() const
      { return GetInputDeviceNames(); }
#endif

    /**Get the maximum frame size in bytes.

       Note a particular device may be able to provide variable length
       frames (eg motion JPEG) so will be the maximum size of all frames.
      */
    virtual PINDEX GetMaxFrameBytes();

    /**Grab a frame. 

       There will be a delay in returning, as specified by frame rate.
      */
    virtual BOOL GetFrameData(
      BYTE * buffer,                 /// Buffer to receive frame
      PINDEX * bytesReturned = NULL  /// Optional bytes returned.
    );

    /**Grab a frame.

       Do not delay according to the current frame rate.
      */
    virtual BOOL GetFrameDataNoDelay(
      BYTE * buffer,                 /// Buffer to receive frame
      PINDEX * bytesReturned = NULL  /// OPtional bytes returned.
    );


    /**Generate a static image, containing a constant field of grey.
     */
    void GrabBlankImage(BYTE *resFrame);

    /**Set the video format to be used.

       Default behaviour sets the value of the videoFormat variable and then
       returns the IsOpen() status.
    */
    virtual BOOL SetVideoFormat(
      VideoFormat videoFormat   /// New video format
    );

    /**Get the number of video channels available on the device.

       Default behaviour returns 1.
    */
    virtual int GetNumChannels() ;

    /**Set the video channel to be used on the device. 
    */
    virtual BOOL SetChannel(
         int channelNumber  /// New channel number for device.
    );
    
    /**Set the colour format to be used.

       Default behaviour sets the value of the colourFormat variable and then
       returns the IsOpen() status.
    */
    virtual BOOL SetColourFormat(
      const PString & colourFormat   // New colour format for device.
    );
    
    /**Set the video frame rate to be used on the device.

       Default behaviour sets the value of the frameRate variable and then
       return the IsOpen() status.
    */
    virtual BOOL SetFrameRate(
      unsigned rate  /// Frames per second
    );
         
    /**Get the minimum & maximum size of a frame on the device.

       Default behaviour returns the value 1 to UINT_MAX for both and returns
       FALSE.
    */
    virtual BOOL GetFrameSizeLimits(
      unsigned & minWidth,   /// Variable to receive minimum width
      unsigned & minHeight,  /// Variable to receive minimum height
      unsigned & maxWidth,   /// Variable to receive maximum width
      unsigned & maxHeight   /// Variable to receive maximum height
    ) ;

    /**Set the frame size to be used.

       Default behaviour sets the frameWidth and frameHeight variables and
       returns the IsOpen() status.
    */
    virtual BOOL SetFrameSize(
      unsigned width,   /// New width of frame
      unsigned height   /// New height of frame
    );
         
    void ClearMapping() { return ; }

    /**Try all known video formats & see which ones are accepted by the video driver
     */
    virtual BOOL TestAllFormats()
      { return TRUE; }
      
    void Restart() { grabDelay.Restart(); }

  protected:
    MCUH323Connection & mcuConnection;
    unsigned grabCount;
    PINDEX   videoFrameSize;
    PINDEX   scanLineWidth;
    PAdaptiveDelay grabDelay;
};


class MCUPVideoOutputDevice : public PVideoOutputDevice
{
  PCLASSINFO(MCUPVideoOutputDevice, PVideoOutputDevice);

  public:
    /** Create a new video output device.
     */
    MCUPVideoOutputDevice(MCUH323Connection & _mcuConnection);

    /**Get a list of all of the drivers available.
      */
#if PTLIB_MAJOR == 2 && PTLIB_MINOR > 0
    static PStringArray GetOutputDeviceNames();
    virtual PStringArray GetDeviceNames() const
      { return GetOutputDeviceNames(); }
#else
    static PStringList GetOutputDeviceNames();
    virtual PStringList GetDeviceNames() const
      { return GetOutputDeviceNames(); }
#endif

    /**Open the device given the device name.
      */
    virtual BOOL Open(
      const PString & deviceName,   /// Device name to open
      BOOL startImmediate = TRUE    /// Immediately start device
    );

    /**Start the video device I/O.
      */
    BOOL Start();

    /**Stop the video device I/O capture.
      */
    BOOL Stop();

    /**Close the device.
      */
    virtual BOOL Close();

    /**Determine if the device is currently open.
      */
    virtual BOOL IsOpen();

    /**Get the maximum frame size in bytes.

       Note a particular device may be able to provide variable length
       frames (eg motion JPEG) so will be the maximum size of all frames.
      */
    virtual PINDEX GetMaxFrameBytes();

    /**Set a section of the output frame buffer.
      */
    virtual BOOL SetFrameData(
      unsigned x,
      unsigned y,
      unsigned width,
      unsigned height,
      const BYTE * data,
      BOOL endFrame = TRUE
    );

    /**Indicate frame may be displayed.
      */
    virtual BOOL EndFrame();

  protected:  
    MCUH323Connection & mcuConnection;
};

#endif // MCU_VIDEO

////////////////////////////////////////////////////

class ConnectionMonitor : public PThread
{
  PCLASSINFO(ConnectionMonitor, PThread);
  public:
    ConnectionMonitor(MCUH323EndPoint & _ep)
      : PThread(10000, NoAutoDeleteThread), ep(_ep)
    {
      Resume();
    }

    void Main();
    BOOL running;

    void AddConnection(const PString & callToken);
    void RemoveConnection(const PString & callToken);

  protected:
    int Perform(MCUH323Connection * conn);
    int RTPTimeoutMonitor(MCUH323Connection * conn);

    MCUH323EndPoint & ep;
    MCUConnectionList monitorList;
};

////////////////////////////////////////////////////

class GatekeeperMonitor : public PThread
{
  PCLASSINFO(GatekeeperMonitor, PThread);
  public:
    GatekeeperMonitor(MCUH323EndPoint & _ep, PString & _mode, PTimeInterval _interval)
      : PThread(10000, NoAutoDeleteThread), ep(_ep)
    {
      mode = _mode;
      retryInterval = _interval;
      terminate = FALSE;
      Resume();
    }

    void Main();
    BOOL terminate;

  protected:
    PTime nextRetryTime;
    PTimeInterval retryInterval;
    MCUH323EndPoint & ep;
    PString mode;
};

////////////////////////////////////////////////////

#endif //_MCU_H323_H

