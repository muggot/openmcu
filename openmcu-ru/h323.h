
#ifndef _OpenMCU_H323_H
#define _OpenMCU_H323_H

#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include <ptlib.h>
#include <ptlib/videoio.h>
#include <opalwavfile.h>

/*
#if USE_SWRESAMPLE
extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/audioconvert.h>
};
#endif //USE_SWRESAMPLE
*/

#include "config.h"

// don't forget to add the same into quote.txt:
#define OTFC_UNMUTE                    0
#define OTFC_MUTE                      1
#define OTFC_MUTE_ALL                  2
#define OTFC_REMOVE_FROM_VIDEOMIXERS   3
#define OTFC_DROP_MEMBER               7
#define OTFC_VAD_NORMAL                8
#define OTFC_VAD_CHOSEN_VAN            9
#define OTFC_VAD_DISABLE_VAD          10
#define OTFC_REMOVE_VMP               11
#define OTFC_MOVE_VMP                 12
#define OTFC_SET_VMP_STATIC           13
#define OTFC_VAD_CLICK                14
#define OTFC_MIXER_ARRANGE_VMP        15
#define OTFC_MIXER_SCROLL_LEFT        16
#define OTFC_MIXER_SHUFFLE_VMP        17
#define OTFC_MIXER_SCROLL_RIGHT       18
#define OTFC_MIXER_CLEAR              19
#define OTFC_MIXER_REVERT             20
#define OTFC_GLOBAL_MUTE              21
#define OTFC_SET_VAD_VALUES           22
#define OTFC_TEMPLATE_RECALL          23
#define OTFC_SAVE_TEMPLATE            24
#define OTFC_DELETE_TEMPLATE          25
#define OTFC_INVITE                   32
#define OTFC_REMOVE_OFFLINE_MEMBER    33
#define OTFC_DROP_ALL_ACTIVE_MEMBERS  64
#define OTFC_INVITE_ALL_INACT_MMBRS   65
#define OTFC_REMOVE_ALL_INACT_MMBRS   66
#define OTFC_SAVE_MEMBERS_CONF        67
#define OTFC_YUV_FILTER_MODE          68
#define OTFC_TAKE_CONTROL             69
#define OTFC_DECONTROL                70
#define OTFC_ADD_VIDEO_MIXER          71
#define OTFC_DELETE_VIDEO_MIXER       72
#define OTFC_SET_VIDEO_MIXER_LAYOUT   73
#define OTFC_SET_MEMBER_VIDEO_MIXER   74
#define OTFC_VIDEO_RECORDER_START     75
#define OTFC_VIDEO_RECORDER_STOP      76

class OpenMCUH323EndPoint : public H323EndPoint
{
  PCLASSINFO(OpenMCUH323EndPoint, H323EndPoint);

  public:
    OpenMCUH323EndPoint(ConferenceManager & conferenceManager);
    ~OpenMCUH323EndPoint();
    
    PString IncomingConferenceRequest(H323Connection & connection, 
                                      const H323SignalPDU & setupPDU,
                                      unsigned & videoMixerNumber);

    BOOL OutgoingConferenceRequest(const PString & room);

    // overrides from H323EndPoint
    virtual H323Connection * CreateConnection(unsigned callReference,void * userData,H323Transport * transport,H323SignalPDU * setupPDU);
    virtual void TranslateTCPAddress(PIPSocket::Address &localAddr, const PIPSocket::Address &remoteAddr);
    H323Connection * FindConnectionWithoutLock(
      const PString & token     ///< Token to identify connection
    )
    {
     return FindConnectionWithoutLocks(token);
    }

    BOOL behind_masq;
    PIPSocket::Address *masqAddressPtr;
    PString nat_lag_ip;

    // new functions
    void Initialise(PConfig & cfg, PConfigPage * rsrc);

    PString GetRoomStatus(const PString & block);
    PString GetRoomStatusJS();
    PString GetMemberList(Conference & conference, ConferenceMemberId id);
    BOOL MemberExist(Conference & conference, ConferenceMemberId id);
    PString GetMemberListOpts(Conference & conference);
    PString GetConferenceOptsJavascript(Conference & c);
    PString GetMemberListOptsJavascript(Conference & conference);
    void SetMemberListOpts(Conference & conference, const PStringToString & data);
    BOOL SetMemberVideoMixer(Conference & conference, ConferenceMember * victim, unsigned newVideoMixer);
    ConferenceMember * GetConferenceMemberById(Conference * conference, long id);
    PString OTFControl(const PString room, const PStringToString & data);
    void OfflineMembersManager(Conference & conference,const PStringToString & data);
    PString GetRoomList(const PString & block);
    PString SetRoomParams(const PStringToString & data);
    void UnmoderateConference(Conference & conference);
    PString RoomCtrlPage(const PString room, BOOL ctrl, int n, Conference & conference, ConferenceMemberId *idp);
    PString GetMonitorText();
    PString GetUsername(ConferenceMemberId id);
    void OnIncomingSipConnection(PString &token, H323Connection &connection);

    ConferenceManager & GetConferenceManager()
    { return conferenceManager; }

#if OPENMCU_VIDEO
    unsigned GetVideoTxQuality() const
    { return videoTxQuality; }

    unsigned GetVideoFrameRate() const
    { return videoRate; }

    BOOL enableVideo;
    unsigned videoRate;
    unsigned videoTxQuality;
#endif

//    void CleanUpConnections(){ H323Endpoint::CleanUpConnections(); };

  protected:
    PString gkAlias;
    PStringList PrefixList;
    PStringList AliasList;


  protected:
    ConferenceManager & conferenceManager;
};

////////////////////////////////////////////////////

class OpenMCUH323Connection;

class OutgoingAudio : public PChannel
{
  PCLASSINFO(OutgoingAudio, PChannel);

  public:
    OutgoingAudio(H323EndPoint & ep, OpenMCUH323Connection & conn, unsigned int sr, unsigned _channels);
    
    BOOL Read(void * buffer, PINDEX amount);
    BOOL Close();

  protected:
    void CreateSilence(void * buffer, PINDEX amount);

    H323EndPoint & ep;
    OpenMCUH323Connection & conn;

    unsigned int sampleRate;
    unsigned channels; //1=mono, 2=stereo
/*
#if USE_SWRESAMPLE
    struct SwrContext *swrc;
#else
    BOOL swrc;
#endif
    PShortArray swr_buf;
*/
    PAdaptiveDelay delay;
    PMutex audioChanMutex;
};

////////////////////////////////////////////////////

class IncomingAudio : public PChannel
{
  PCLASSINFO(IncomingAudio, PChannel);

  public:
    IncomingAudio(H323EndPoint & ep, OpenMCUH323Connection & conn, unsigned int _sampleRate, unsigned _channels);

    BOOL Write(const void * buffer, PINDEX amount);
    BOOL Close();

    unsigned int sampleRate;
    unsigned channels; //1=mono, 2=stereo

  protected:
    H323EndPoint & ep;
    OpenMCUH323Connection & conn;

//    unsigned int sampleRate;
/*
#if USE_SWRESAMPLE
    struct SwrContext *swrc;
#else
    BOOL swrc;
#endif
    PShortArray swr_buf;
*/
    PMutex audioChanMutex;
    PAdaptiveDelay delay;
    unsigned modulo;
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

class H323Connection_ConferenceMember;
class PVideoInputDevice_OpenMCU;
class PVideoOutputDevice_OpenMCU;

class OpenMCUH323Connection : public H323Connection
{
  PCLASSINFO(OpenMCUH323Connection, H323Connection);

  public:
    OpenMCUH323Connection(OpenMCUH323EndPoint &, unsigned, void *);
    ~OpenMCUH323Connection();

    virtual void LeaveConference();
    virtual H323Connection_ConferenceMember * GetConferenceMember()
    {
     cout << "GetConferenceMember " << conferenceMember << "\n";
     return conferenceMember;
    }

    // overrides from H323Connection
    virtual BOOL OpenAudioChannel(BOOL, unsigned,   H323AudioCodec & codec);
#if OPENMCU_VIDEO
    virtual BOOL OpenVideoChannel(BOOL isEncoding, H323VideoCodec & codec);
    virtual void OpenVideoCache(H323VideoCodec & srcCodec);
    virtual void OnClosedLogicalChannel(const H323Channel & channel);
#endif
		virtual void CleanUpOnCallEnd();
    virtual void OnEstablished();
    virtual AnswerCallResponse OnAnswerCall(const PString &, const H323SignalPDU &, H323SignalPDU &);
    virtual void OnUserInputString(const PString & value);
    virtual BOOL OnReceivedSignalSetup(const H323SignalPDU & setupPDU);
    virtual BOOL OnReceivedCallProceeding(const H323SignalPDU & proceedingPDU);

    virtual BOOL OnIncomingAudio(const void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels);
    virtual BOOL OnOutgoingAudio(void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels);
    virtual PString GetAudioTransmitCodecName() const { return audioTransmitCodecName; }
    virtual PString GetAudioReceiveCodecName() const  { return audioReceiveCodecName; }
    virtual PString GetRemoteName() const             { return remoteName; }

    virtual void SetupCacheConnection(PString & format,Conference * conf, ConferenceMember * memb);

    void SetEndpointDefaultVideoParams(OpalMediaFormat & mf);
    void SetEndpointPrefVideoParams(OpalMediaFormat & mf, PString uri, PString section);
    PString GetEndpointParam(PString param, PString uri, PString section);

#if OPENMCU_VIDEO
    virtual BOOL OnIncomingVideo(const void * buffer, int width, int height, PINDEX amount);
    virtual BOOL OnOutgoingVideo(void * buffer, int width, int height, PINDEX & amount);
    virtual PString GetVideoTransmitCodecName() const { return videoTransmitCodecName; }
    virtual void SetVideoTransmitCodecName(PString newVideoTransmitCodecName) { videoTransmitCodecName=newVideoTransmitCodecName; }
    virtual PString GetVideoReceiveCodecName() const  { return videoReceiveCodecName; }
    virtual BOOL GetPreMediaFrame(void * buffer, int width, int height, PINDEX & amount);
    virtual H323VideoCodec * GetVideoReceiveCodec() const  { return videoReceiveCodec; }
    virtual H323VideoCodec * GetVideoTransmitCodec() const  { return videoTransmitCodec; }
    virtual void RestartGrabber();
    unsigned videoMixerNumber;
#endif

    // Room the connection is joined to. It is NULL before the
    //  welcome procedure ends, or after the member is disconnected
    //  from the conference.
    Conference * conference;

    PMutex connMutex;

    PMutex & GetMutex() { return connMutex; }

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

  protected:

    virtual void JoinConference(const PString & room);

#if OPENMCU_VIDEO
    virtual BOOL InitGrabber(PVideoInputDevice  * grabber, int frameWidth, int frameHeight, int frameRate);
#endif

//    PMutex connMutex;
    OpenMCUH323EndPoint & ep;

    // Name of the room to join when the welcome procedure ends.
    //  This is usually initialized when the call is answered,
    //  before the welcome procedure begins.
    PString requestedRoom;

    // Room the connection is joined to. It is NULL before the
    //  welcome procedure ends, or after the member is disconnected
    //  from the conference.
//    Conference * conference;

    // Object used to treat the connection as a conference member.
    //  It is NULL before the connection is joined to the conference,
    //  or after the member is disconnected from the conference.
    H323Connection_ConferenceMember * conferenceMember;

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

    PString remoteName;

    // Wave file played during the welcome procedure.
    OpalWAVFile playFile;

    // optional record file used during the welcome procedure
    OpalWAVFile recordFile;
    unsigned int recordSilenceCount;
    unsigned int recordSilenceThreshold;
    unsigned int recordDuration;
    unsigned int recordLimit;

/*
    void StartRecording(
      const PFilePath & filename, 
      unsigned int recordLimit = 5, 
      unsigned int recordSilenceThreshold = 1
    );
    virtual void OnFinishRecording();
*/
    //PAdaptiveDelay playDelay;

    // True if the state has not changed since the wave file started.
    BOOL wavePlayingInSameState;

    PString audioTransmitCodecName;
    PString audioReceiveCodecName;

    BOOL isMCU;

#if OPENMCU_VIDEO
    PVideoInputDevice_OpenMCU * videoGrabber;
    PVideoOutputDevice_OpenMCU * videoDisplay;
    PString videoTransmitCodecName;
    PString videoReceiveCodecName;
    H323VideoCodec *videoReceiveCodec;
    H323VideoCodec *videoTransmitCodec;
#endif
};

////////////////////////////////////////////////////

class H323Connection_ConferenceConnection : public ConferenceConnection
{
  PCLASSINFO(H323Connection_ConferenceConnection, ConferenceConnection);
  public:
    H323Connection_ConferenceConnection(void * id)
      : ConferenceConnection(id)
    { conn = (OpenMCUH323Connection *)id; }

    virtual PString GetName() const
    { return conn->GetCallToken(); }

  protected:
    OpenMCUH323Connection * conn;
};

class H323Connection_ConferenceMember : public ConferenceMember
{
  PCLASSINFO(H323Connection_ConferenceMember, ConferenceMember);
  public:
    H323Connection_ConferenceMember(Conference * _conference, OpenMCUH323EndPoint & _ep, const PString & _h323Token, ConferenceMemberId _id, BOOL isMCU);
    ~H323Connection_ConferenceMember();

    virtual ConferenceConnection * CreateConnection()
    { 
//     if(id==this) return NULL;
//      PTRACE(2, "h323.h\tFCWL from CreateConnection:" << h323Token);
     H323Connection * conn = (H323Connection *)id;
//      H323Connection * conn = ep.FindConnectionWithLock(h323Token);
      if (conn == NULL)
        return NULL;

      H323Connection_ConferenceConnection * confConn = new H323Connection_ConferenceConnection(conn); 
//      conn->Unlock();
      return confConn;
    }

    virtual void Close();

    virtual PString GetH323Token() const
    { return h323Token; }

    virtual PString GetTitle() const;

    virtual void SetName();
    virtual void SetName(PString newName) { name = newName; }

    virtual void SetFreezeVideo(BOOL) const;

    PString GetMonitorInfo(const PString & hdr);

    virtual void SendUserInputIndication(const PString & str);

    void OnReceivedUserInputIndication(const PString & str)
    { 
      PTRACE(2, "h323.cxx\tFCWL from OnReceivedUserInputIndication:" << h323Token);
      H323Connection * conn = ep.FindConnectionWithLock(h323Token);
      if (conn != NULL)
      {
        conn->SendUserInput(str); 
        conn->Unlock();
      }
    }

  protected:
    PStringStream iISequence; // input indication sequence
    OpenMCUH323EndPoint & ep;
    PString h323Token;
};

////////////////////////////////////////////////////

#if OPENMCU_VIDEO

#include <ptlib/vconvert.h>

class PVideoInputDevice_OpenMCU : public PVideoInputDevice
{
 PCLASSINFO(PVideoInputDevice_OpenMCU, PVideoInputDevice);
 public:
  /** Create a new MCU video input device.
   */
    PVideoInputDevice_OpenMCU(OpenMCUH323Connection & conn);

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

    virtual PStringList GetDeviceNames() const
      { return GetInputDeviceNames(); }

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
    OpenMCUH323Connection & mcuConnection;
    unsigned grabCount;
    PINDEX   videoFrameSize;
    PINDEX   scanLineWidth;
    PAdaptiveDelay grabDelay;
};


class PVideoOutputDevice_OpenMCU : public PVideoOutputDevice
{
  PCLASSINFO(PVideoOutputDevice_OpenMCU, PVideoOutputDevice);

  public:
    /** Create a new video output device.
     */
    PVideoOutputDevice_OpenMCU(OpenMCUH323Connection & _mcuConnection);

    /**Get a list of all of the drivers available.
      */
    static PStringList GetOutputDeviceNames();

    virtual PStringList GetDeviceNames() const
      { return GetOutputDeviceNames(); }

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
    OpenMCUH323Connection & mcuConnection;
};

#endif // OPENMCU_VIDEO

#endif //_OpenMCU_H323_H

