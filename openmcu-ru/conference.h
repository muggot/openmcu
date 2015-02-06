
#ifndef _MCU_CONFERENCE_H
#define _MCU_CONFERENCE_H

#include <ptlib/sound.h>
#include <ptclib/delaychan.h>

#include <set>
#include <map>

#include "config.h"
#include "utils.h"
#include "video.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

enum MemberTypes
{
  MEMBER_TYPE_NONE       = 0,
  MEMBER_TYPE_MCU        = 2,
  //
  MEMBER_TYPE_GSYSTEM    = 1, // MEMBER_TYPE_PIPE|MEMBER_TYPE_CACHE|MEMBER_TYPE_RECORDER|MEMBER_TYPE_STREAM
  MEMBER_TYPE_PIPE       = 1,
  MEMBER_TYPE_CACHE      = 3,
  MEMBER_TYPE_RECORDER   = 7,
  MEMBER_TYPE_STREAM     = 15,
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
    ConferenceConnection(long _id)
      : id(_id)
    { }

    ~ConferenceConnection()
    { }

    long GetID() const
    { return id; }

    virtual PString GetName() const
    { return id; }

  protected:
    long id;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceAudioConnection : public ConferenceConnection
{
  PCLASSINFO(ConferenceAudioConnection, ConferenceConnection);

  public:
    ConferenceAudioConnection(long _id, int _sampleRate = 8000, int _channels = 1);
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

class ConferenceProfile : public PObject
{
  PCLASSINFO(ConferenceProfile, PObject);

  public:
    ConferenceProfile(long _listID, const PString & _name, Conference * _conference, ConferenceMember * member);

    ~ConferenceProfile()
    { }

    void Unlock();

    long GetID()
    { return listID; }

    const PString & GetName() const
    { return name; }

    const PString & GetNameID() const
    { return nameID; }

    const PString & GetNameHTML() const
    { return nameHTML; }

    ConferenceMemberId GetID() const
    { return (ConferenceMemberId)this; }

    ConferenceMember * GetMember()
    { return member; }

  protected:
    long listID;
    PString name;
    PString nameHTML;
    PString nameID;

    Conference *conference;
    ConferenceMember *member;
    PMutex mutex;
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

    void ChannelBrowserStateUpdate(BYTE bitMask, BOOL bitState);

    void Unlock();

    /**
      * used to pre-emptively close a members connection
      */
    virtual void Close()
    { }

    /**
      * If this returns TRUE, the conference member will be visible in all publically displayed
      * conference lists. It will always be visible in the console displays
      */
    virtual BOOL IsVisible() const
    { return TRUE; }

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
    virtual void WriteVideo(const void * buffer, int width, int height, PINDEX amount);

    /**
      *  Called when a conference member wants to read a block of video from the conference
      *  By default, this calls ReadMemberVideo on the conference
      */
    virtual void ReadVideo(void * buffer, int width, int height, PINDEX & amount);

    /**
      * called when another conference member wants to write a video frame to this endpoint
      * this will only be called when the conference is not "use same video for all members"
      */
    virtual void OnExternalSendVideo(ConferenceMemberId id, const void * buffer, int width, int height, PINDEX amount);

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

    BOOL IsJoined() const
    { return memberIsJoined; }

    virtual void SetName() {}
    virtual void SetName(PString newName) {}

    virtual PString GetName() const
    { return name; }

    virtual PString GetNameID() const
    { return nameID; }

    PString GetNameHTML() const
    {
      PString _name = name;
      _name.Replace("&","&amp;",TRUE,0);
      _name.Replace("\"","&quot;",TRUE,0);
      return _name;
    }

    virtual PString GetCallToken() const
    { return callToken; }

    virtual MemberTypes GetType()
    { return memberType; }

    virtual void SetFreezeVideo(BOOL) const
    { }

    virtual unsigned GetAudioLevel() const
    { return audioLevel;  }

    BOOL autoDial;
    unsigned muteMask;
    BOOL disableVAD;
    BOOL chosenVan; // allways visible, but can change place on frame, used in 5+1 layout
    int vad;
    unsigned long audioCounter;
    unsigned audioLevelIndicator;
    unsigned previousAudioLevel;
    BYTE channelCheck;

    MCUSimpleVideoMixer * videoMixer;

    float kManualGain, kOutputGain;
    int kManualGainDB, kOutputGainDB;

    // functions H323Connection_ConferenceMember
    virtual void SetChannelPauses(unsigned mask) {};
    virtual void UnsetChannelPauses(unsigned mask) {};


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

#if MCU_VIDEO
    PTime firstFrameSendTime;
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

    void RefreshAddressBook();

    /**
      * add the specified member to the conference
      */
    BOOL AddMember(ConferenceMember * member);

    /**
     * remove the specifed member from the conference.
     * Note that this function does not actually delete the conference member
     * as sometimes a conference member needs to remove itself from a conference
     * 
     * @return if TRUE, the conference is now empty
     */
    BOOL RemoveMember(ConferenceMember * member);

    MCUMemberList & GetMemberList()
    { return memberList; }

    MCUProfileList & GetProfileList()
    { return profileList; }

    void AddMemberToList(const PString & name, ConferenceMember *member);
    void RemoveMemberFromList(const PString & name, ConferenceMember *member);

    int GetMemberCount() const
    { return memberCount; }

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

    virtual BOOL WriteMemberVideo(ConferenceMember * member, const void * buffer, int width, int height, PINDEX amount);

    virtual BOOL UseSameVideoForAllMembers()
    { return videoMixerList.GetCurrentSize() > 0; }

    virtual void FreezeVideo(ConferenceMemberId id);
    virtual BOOL PutChosenVan();
#endif

    void HandleFeatureAccessCode(ConferenceMember & member, PString fac);

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
    virtual void PullMemberOptionsFromTemplate(ConferenceMember * member, PString tpl);
    virtual void TemplateInsertAndRewrite(PString tplName, PString tpl);
    virtual void SetLastUsedTemplate(PString tplName);
    virtual void DeleteTemplate(PString tplName);
    virtual BOOL RewriteMembersConf();
    virtual void OnConnectionClean(const PString & remotePartyName, const PString & remotePartyAddress);

    ConferenceRecorder * conferenceRecorder;
    ConferenceMember * pipeMember;

    BOOL GetForceScreenSplit() { return forceScreenSplit; }

    BOOL RecorderCheckSpace();
    BOOL StartRecorder();
    BOOL StopRecorder();

    BOOL stopping;
    BOOL lockedTemplate;

  protected:
    ConferenceManager & manager;

    PString trace_section;

    // memberListMutex - используется при добавлении/удалении участника
    // предотвращает создание в списке двух одноименных участников
    PMutex memberListMutex;

    MCUMemberList memberList;
    MCUProfileList profileList;

    void RemoveAudioConnection(ConferenceMember * member);
    MCUAudioConnectionList audioConnectionList;

    MCUVideoMixerList videoMixerList;

    PINDEX memberCount;
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

    ConferenceProfile * FindProfileWithLock(const PString & roomName, const PString & memberName);
    ConferenceProfile * FindProfileWithLock(Conference * conference, const PString & memberName);

    ConferenceMember * FindMemberWithLock(const PString & roomName, const PString & memberName);
    ConferenceMember * FindMemberWithLock(Conference * conference, const PString & memberName);
    ConferenceMember * FindMemberWithoutLock(Conference * conference, const PString & memberName);

    ConferenceMember * FindMemberNameIDWithLock(const PString & roomName, const PString & memberName);
    ConferenceMember * FindMemberNameIDWithLock(Conference * conference, const PString & memberName);

    ConferenceMember * FindMemberWithLock(const PString & roomName, long id);
    ConferenceMember * FindMemberWithLock(Conference * conference, long id);

    MCUSimpleVideoMixer * FindVideoMixerWithLock(const PString & room, long number);
    MCUSimpleVideoMixer * FindVideoMixerWithLock(Conference * conference, long number);
    MCUSimpleVideoMixer * GetVideoMixerWithLock(const PString & room);
    MCUSimpleVideoMixer * GetVideoMixerWithLock(Conference * conference);
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

    void ManagerRefreshAddressBook();

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

