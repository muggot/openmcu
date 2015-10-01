
#include "precompile.h"

#ifndef _MCU_CONFERENCE_H
#define _MCU_CONFERENCE_H

#include "utils.h"
#include "video.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

enum MemberTypes
{
  MEMBER_TYPE_NONE       = 0,
  MEMBER_TYPE_CONN       = 2,
  MEMBER_TYPE_STREAM     = 4,
  //
  MEMBER_TYPE_GSYSTEM    = 1, // MEMBER_TYPE_PIPE|MEMBER_TYPE_CACHE|MEMBER_TYPE_RECORDER
  MEMBER_TYPE_PIPE       = 1,
  MEMBER_TYPE_CACHE      = 3,
  MEMBER_TYPE_RECORDER   = 7,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class AudioResampler
{
    AudioResampler(int _srcSampleRate, int _srcChannels, int _dstSampleRate, int _dstChannels);
    BOOL Initialise();

  public:
    static AudioResampler * Create(int _srcSampleRate, int _srcChannels, int _dstSampleRate, int _dstChannels);
    ~AudioResampler();

    void Resample(const BYTE * src, int srcBytes, BYTE * dst, int dstBytes);
    static void InternalResample(const BYTE * src, int srcBytes, int srcSampleRate, int srcChannels, BYTE * dst, int dstBytes, int dstSampleRate, int dstChannels);

  protected:
    int srcSampleRate;
    int srcChannels;
    int dstSampleRate;
    int dstChannels;

#if USE_SWRESAMPLE
    struct SwrContext * swrc;
#elif USE_AVRESAMPLE
    struct AVAudioResampleContext * swrc;
#elif USE_LIBSAMPLERATE
    struct SRC_STATE_tag * swrc;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class AudioBuffer
{
  public:
    AudioBuffer(int _sampleRate, int _channels);
    ~AudioBuffer();

    int GetSampleRate() const
    { return sampleRate; }

    int GetChannels() const
    { return channels; }

    BYTE * GetPointer()
    { return buffer.GetPointer(); }

    int GetSize()
    { return bufferSize; }

    int GetTimeSize()
    { return bufferTimeSize; }

  protected:
    int sampleRate;
    int channels;

    int bufferTimeSize;
    int bufferSize;
    MCUBuffer buffer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceConnection : public PObject {
  PCLASSINFO(ConferenceConnection, PObject);
  public:
    ConferenceConnection(ConferenceMemberId _id)
      : id(_id)
    { }

    ~ConferenceConnection()
    { }

    ConferenceMemberId GetID() const
    { return id; }

    virtual PString GetName() const
    { return (long)id; }

  protected:
    ConferenceMemberId id;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceAudioConnection : public ConferenceConnection
{
  PCLASSINFO(ConferenceAudioConnection, ConferenceConnection);

  public:
    ConferenceAudioConnection(ConferenceMemberId _id, int _sampleRate = 8000, int _channels = 1);
    ~ConferenceAudioConnection();

    virtual void WriteAudio(const uint64_t & srcTimestamp, const BYTE * data, int amount);
    virtual void ReadAudio(const uint64_t & dstTimestamp, BYTE * data, int amount, int dstSampleRate, int dstChannels);

    AudioBuffer * GetBuffer(int _dstSampleRate, int _dstChannels);

    int GetSampleRate() const
    { return sampleRate; }

    int GetChannels() const
    { return channels; }

    static void Mix(const BYTE * src, BYTE * dst, int count);

  protected:
    int sampleRate;
    int channels;
    int maxFrameTime;
    int timeIndex;           // current position ms
    uint64_t startTimestamp; // us

    typedef std::map<long, AudioResampler *> AudioResamplerListType;
    AudioResamplerListType audioResamplerList;

    typedef MCUSharedList<AudioBuffer> MCUAudioBufferList;
    MCUAudioBufferList audioBufferList;
    // mutex только для добавления буфера в список
    PMutex audioBufferListMutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceMember : public PObject
{
  PCLASSINFO(ConferenceMember, PObject);
  public:

    /**
      * create a new conference member.
      * that can used to identify this member unambiguously
      */
    ConferenceMember(Conference * conference);

    /**
      * destroy the conference member
      */
    ~ConferenceMember();

    void ChannelStateUpdate(unsigned bit, BOOL state);

    void Unlock();

    /**
      * used to pre-emptively close a members connection
      */
    virtual void Close();

    /**
      * If this returns TRUE, the conference member will be visible in all publically displayed
      * conference lists. It will always be visible in the console displays
      */
    void SetVisible(BOOL enable)
    {
      if(memberType == MEMBER_TYPE_CONN)
        visible = enable;
    }

    BOOL IsVisible() const
    { return visible; }

    BOOL IsOnline() const
    { return (callToken != ""); }

    BOOL IsSystem() const
    { return (memberType & MEMBER_TYPE_GSYSTEM); }

    /**
      * return the conference member ID
      */
    ConferenceMemberId GetID() const
    { return id; }

    virtual PTime GetStartTime() const
    { return startTime; }

    /**
      * return the conference this member belongs to
      */
    Conference * GetConference()
    { return conference; }

    /**
     * This is called when the conference member want to send a user input indication to the the conference.
     * By default, this routines calls OnReceivedUserInputIndication for all of the other conference members
     */
    virtual void SendUserInputIndication(const PString & str) {}

    /**
     * this virtual function is called when the conference sends a user input indication to the endpoint
     * the conference
     */
    virtual void OnReceivedUserInputIndication(const PString & str)
    { }

    /**
      *  Called when the conference member want to send audio data to the cofnerence
      */
    virtual void WriteAudio(const uint64_t & timestamp, const void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels);

    void WriteAudioAutoGainControl(const short * pcm, unsigned samplesPerFrame, unsigned codecChannels, unsigned sampleRate, unsigned level, float* currVolCoef, unsigned* signalLevel, float kManual);

    /**
      *  Called when the conference member wants to read a block of audio from the conference
      *  By default, this calls ReadMemberAudio on the conference
      */
    virtual void ReadAudio(const uint64_t & timestamp, void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels);

    void ReadAudioGainControl(void * buffer, int amount);

#if MCU_VIDEO
    /**
      *  Called when the conference member wants to send video data to the conference
      */
    virtual void WriteVideo(const void * buffer, int width, int height);

    /**
      *  Called when a conference member wants to read a block of video from the conference
      *  By default, this calls ReadMemberVideo on the conference
      */
    virtual void ReadVideo(void * buffer, int width, int height, PINDEX & amount);

    /**
      * called when another conference member wants to write a video frame to this endpoint
      * this will only be called when the conference is not "use same video for all members"
      */
    virtual void OnExternalSendVideo(ConferenceMemberId id, const void * buffer, int width, int height);

    /**
      * called to when a new video source added
      */
    virtual BOOL AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr);

    /**
      * called to when a new video source removed
      */
    virtual void RemoveVideoSource(ConferenceMemberId id, ConferenceMember & mbr);

    double GetVideoTxFrameRate() const
    { 
      if (totalVideoFramesSent == 0) 
        return 0.0; 
      else 
        return totalVideoFramesSent * 1000.0 / ((PTime() - firstFrameSendTime).GetMilliSeconds()); }

    PString GetVideoRxFrameSize() const
    {
      PStringStream res;
      res << rxFrameWidth << "x" << rxFrameHeight;
      return res;
    }

    double GetVideoRxFrameRate() const
    { 
      if (totalVideoFramesReceived == 0)
        return 0.0; 
      else 
        return totalVideoFramesReceived * 1000.0 / ((PTime() - firstFrameReceiveTime).GetMilliSeconds()); 
    }

    unsigned GetVideoMixerNumber()
    {
      return videoMixerNumber;
    }
    void SetVideoMixerNumber(unsigned newVideoMixerNumber)
    {
      videoMixerNumber=newVideoMixerNumber;
    }
#endif

    /*
     * used to output monitor information for the member
     */
    virtual PString GetMonitorInfo(const PString & hdr);

    void SetJoined(BOOL isJoinedNow)
    { memberIsJoined = isJoinedNow; }

    inline BOOL IsJoined() const
    { return memberIsJoined; }

    virtual void SetName(PString newName) {}

    inline virtual PString GetName() const
    { return name; }

    virtual PString GetNameID() const
    { return nameID; }

    virtual PString GetCallToken() const
    { return callToken; }

    virtual void SetCallToken(const PString & token)
    { callToken = token; }

    inline MemberTypes GetType()
    { return memberType; }

    BOOL IsMCU()
    { return isMCU; }

    virtual void SetFreezeVideo(BOOL) const
    { }

    virtual unsigned GetAudioLevel() const
    { return audioLevel;  }

    void ResetCounters()
    {
      totalVideoFramesSent = 0;
      firstFrameSendTime = -1;
      totalVideoFramesReceived = 0;
      firstFrameReceiveTime = -1;
      rxFrameWidth = 0;
      rxFrameHeight = 0;
      channelMask = 0;
    }

    void Dial();
    void Dial(int _autoDial);
    void ClearDial();
    void SetAutoDial(BOOL enable);

    PMutex & GetDialMutex()
    { return dialMutex; }

    BOOL autoDial;
    PString dialToken;
    PMutex dialMutex;

    unsigned muteMask;
    unsigned channelMask;
    BOOL disableVAD;
    BOOL chosenVan; // allways visible, but can change place on frame ("priority")
    int vad;
    unsigned long audioCounter;
    unsigned audioLevelIndicator;
    unsigned previousAudioLevel;

    MCUSimpleVideoMixer * videoMixer;

    float kManualGain, kOutputGain;
    int kManualGainDB, kOutputGainDB;

    // functions H323Connection_ConferenceMember
    virtual void SetChannelPauses(unsigned mask)
    {
      PTRACE(2,"Conference\tIncorrent call of SetChannelPauses() with mask " << mask);
    };
    virtual void UnsetChannelPauses(unsigned mask)
    {
      PTRACE(2,"Conference\tIncorrent call of UnsetChannelPauses() with mask " << mask);
    };
    virtual void SetChannelState(unsigned newMask)
    {
      PTRACE(2,"Conference\tSetChannelState() with mask " << newMask);
      muteMask = newMask;
    };

    void SendRoomControl(int state);
    MCUJSON * AsJSON(int state = 1);

#if MCU_VIDEO
    int resizerRule; //0=cut, 1=stripes
    PTime firstFrameSendTime;
#endif

  protected:
    unsigned videoMixerNumber;
    Conference * conference;
    ConferenceMemberId id;
    BOOL memberIsJoined;
    PTime startTime;
    unsigned audioLevel;
    PString callToken;
    MemberTypes memberType;
    PString name;
    PString nameID;
    float currVolCoef;
    BOOL visible;
    BOOL isMCU;
    unsigned write_audio_time_microseconds;
    unsigned write_audio_average_level;
    unsigned write_audio_write_counter;

#if MCU_VIDEO
    PINDEX totalVideoFramesSent;

    PTime firstFrameReceiveTime;
    PINDEX totalVideoFramesReceived;

    int rxFrameWidth, rxFrameHeight;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Conference : public PObject
{
  PCLASSINFO(Conference, PObject);
  public:

    Conference(ConferenceManager & manager,
                                  long _listID,
        const OpalGloballyUniqueID & _guid, // H.323 m_conferenceID ???
                   const PString & _number,
                     const PString & _name
#if MCU_VIDEO
                  ,MCUSimpleVideoMixer * _videoMixer = NULL
#endif
                   );

    ~Conference();

    void Unlock();

    ConferenceManager & GetManager()
    { return manager; }

    /**
      * add the specified member to the conference
      */
    BOOL AddMember(ConferenceMember * member, BOOL addToList = TRUE);
    MCUMemberList::shared_iterator AddMemberToList(ConferenceMember * member, BOOL addToList = TRUE);

    /**
     * remove the specifed member from the conference.
     * Note that this function does not actually delete the conference member
     * as sometimes a conference member needs to remove itself from a conference
     * 
     * @return if TRUE, the conference is now empty
     */
    BOOL RemoveMember(ConferenceMember * member, BOOL removeFromList = TRUE);

    MCUMemberList & GetMemberList()
    { return memberList; }

    PMutex & GetMemberListMutex()
    { return memberListMutex; }

    int GetMemberCount()
    { return memberList.GetSize(); }

    int GetOnlineMemberCount() const
    { return onlineMemberCount; }

    int GetVisibleMemberCount() const
    { return visibleMemberCount; }

    virtual PString GetName() const
    { return name; }

    virtual PString GetNumber() const
    { return number; }

    OpalGloballyUniqueID GetGUID() const
    { return guid; }

    long GetID() const
    { return listID; }

    virtual BOOL IsVisible() const
    { return TRUE; }

    virtual BOOL IsMuteUnvisible() const
    { return muteUnvisible; }

    virtual void SetMuteUnvisible(BOOL set)
    { muteUnvisible = set; }

    virtual PString IsModerated() const
    {
      if(forceScreenSplit == FALSE)
        return "0";
     PString yes="+";
     PString no="-";
     if(!moderated) return no; else return yes;
    }

    virtual void SetModerated(BOOL set)
    { moderated = set; }

    PTime GetStartTime() const
    { return startTime; }

    PINDEX GetMaxMemberCount() const
    { return maxMemberCount; }

    MCUVideoMixerList & GetVideoMixerList()
    { return videoMixerList; }

    virtual void ReadMemberAudio(ConferenceMember * member, const uint64_t & timestamp, void * buffer, int amount, int sampleRate, int channels);

    virtual void WriteMemberAudio(ConferenceMember * member, const uint64_t & timestamp, const void * buffer, int amount, int sampleRate, int channels);

    virtual void WriteMemberAudioLevel(ConferenceMember * member, int audioLevel, int tint);

#if MCU_VIDEO
    virtual void ReadMemberVideo(ConferenceMember * member, void * buffer, int width, int height, PINDEX & amount);

    virtual BOOL WriteMemberVideo(ConferenceMember * member, const void * buffer, int width, int height);

    virtual BOOL UseSameVideoForAllMembers()
    { return videoMixerList.GetSize() > 0; }

    virtual void FreezeVideo(ConferenceMemberId id);
    virtual BOOL PutChosenVan();
#endif

    void HandleFeatureAccessCode(ConferenceMember & member, PString fac);

    void UpdateVideoMixOptions(ConferenceMember * member);
    
    unsigned short int VAdelay;
    unsigned short int VAtimeout;
    unsigned short int VAlevel;
    unsigned short int echoLevel;

    PString membersConf;
    PString confTpl;
    virtual PString SaveTemplate(PString tplName);
    virtual void LoadTemplate(PString tpl);
    virtual PString GetTemplateList();
    virtual PString GetSelectedTemplateName();
    virtual PString ExtractTemplate(PString tplName);
    virtual void TemplateInsertAndRewrite(PString tplName, PString tpl);
    virtual void SetLastUsedTemplate(PString tplName);
    virtual void DeleteTemplate(PString tplName);
    virtual BOOL RewriteMembersConf();

    ConferenceRecorder * conferenceRecorder;
    ConferenceMember * pipeMember;

    BOOL GetForceScreenSplit() { return forceScreenSplit; }

    BOOL RecorderCheckSpace();
    BOOL StartRecorder();
    BOOL StopRecorder();

    BOOL stopping;
    BOOL lockedTemplate;

    int dialCountdown;

  protected:
    ConferenceManager & manager;

    PString trace_section;

    // memberListMutex - используется при добавлении/удалении участника
    // предотвращает создание в списке двух одноименных участников
    PMutex memberListMutex;

    MCUMemberList memberList;

    void RemoveAudioConnection(ConferenceMember * member);
    MCUAudioConnectionList audioConnectionList;

    MCUVideoMixerList videoMixerList;

    PINDEX onlineMemberCount;
    PINDEX visibleMemberCount;
    PINDEX maxMemberCount;

    long listID;
    OpalGloballyUniqueID guid;

    PString number;
    PString name;
    PTime startTime;
    BOOL moderated;
    BOOL muteUnvisible;
    int vidmembernum;
    PMutex membersConfMutex;
    BOOL forceScreenSplit;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceMonitor : public PThread
{
  PCLASSINFO(ConferenceMonitor, PThread);
  public:
    ConferenceMonitor(ConferenceManager & _manager)
      : PThread(10000, NoAutoDeleteThread), manager(_manager)
    { Resume(); }

    void Main();
    BOOL running;

  protected:
    int Perform(Conference * conference);
    ConferenceManager & manager;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceManager : public PObject
{
  PCLASSINFO(ConferenceManager, PObject);
  public:
    ConferenceManager();
    ~ConferenceManager();

    Conference * FindConferenceWithLock(Conference * c);
    Conference * FindConferenceWithLock(const OpalGloballyUniqueID & conferenceID);
    Conference * FindConferenceWithLock(long id);
    Conference * FindConferenceWithLock(const PString & room);

    BOOL CheckJoinConference(const PString & number);
    Conference * MakeConferenceWithLock(const PString & number, PString name = "", BOOL ignoreRestriction = FALSE);

    ConferenceMember * FindMemberWithLock(const PString & roomName, const PString & memberName);
    ConferenceMember * FindMemberWithLock(Conference * conference, const PString & memberName);
    ConferenceMember * FindMemberWithLock(const PString & roomName, long id);
    ConferenceMember * FindMemberWithLock(Conference * conference, long id);
    ConferenceMember * FindMemberSimilarWithLock(const PString & roomName, const PString & memberName);
    ConferenceMember * FindMemberSimilarWithLock(Conference * conference, const PString & memberName);

    MCUSimpleVideoMixer * FindVideoMixerWithLock(const PString & room, long number);
    MCUSimpleVideoMixer * FindVideoMixerWithLock(Conference * conference, long number);
    MCUSimpleVideoMixer * GetVideoMixerWithLock(const PString & room);
    MCUSimpleVideoMixer * GetVideoMixerWithLock(Conference * conference);
    MCUSimpleVideoMixer * GetVideoMixerWithLock(Conference * conference, int & number);
    int AddVideoMixer(Conference * conference);
    int DeleteVideoMixer(Conference * conference, int number);

    /**
      * return true if a conference with the specified ID exists
      */
    BOOL HasConference(const OpalGloballyUniqueID & conferenceID, PString & room);
    BOOL HasConference(const OpalGloballyUniqueID & conferenceID)
    { PString r; return HasConference(conferenceID, r); }

    /**
      * return true if a conference with the specified number exists
      */
    BOOL HasConference(const PString & number, OpalGloballyUniqueID & conferenceID);
    BOOL HasConference(const PString & number)
    { OpalGloballyUniqueID i; return HasConference(number, i); }

    //
    void RemoveConference(const PString & room);

    MCUConferenceList & GetConferenceList()
    { return conferenceList; }

    virtual void OnCreateConference(Conference *);

    virtual void OnDestroyConference(Conference *);

    PINDEX GetMaxConferenceCount() const
    { return maxConferenceCount; }

    void ClearConferenceList();

  protected:
    virtual Conference * CreateConference(long _id, const OpalGloballyUniqueID & _guid, const PString & _number, const PString & _name);

    MCUConferenceList conferenceList;

    // conferenceListMutex - используется при создании конференции
    // предотвращает создание в списке двух одноименных конференций
    PMutex conferenceListMutex;

    PINDEX maxConferenceCount;
    ConferenceMonitor * monitor;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  // _MCU_CONFERENCE_H

