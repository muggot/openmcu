
#include "precompile.h"

#ifndef _MCU_FILEMEMBERS_H
#define _MCU_FILEMEMBERS_H

#include "utils.h"
#include "conference.h"
#include "mcu_rtp.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceSoundCardMember : public ConferenceMember
{
  PCLASSINFO(ConferenceSoundCardMember, ConferenceMember);
  public:
    ConferenceSoundCardMember(Conference * _conference);
    ~ConferenceSoundCardMember();

    virtual PString GetName() const
    { return "sound card listener"; }

    virtual BOOL IsVisible() const
    { return FALSE; }

    void OnReceivedUserInputIndication(const PString & str)
    { cout << "Received user input indication " << str << endl; }

    PDECLARE_NOTIFIER(PThread, ConferenceSoundCardMember, Thread);

  protected:
    BOOL running;
    PSoundChannel soundDevice;
    PThread * thread;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceFileMember : public ConferenceMember
{
  PCLASSINFO(ConferenceFileMember, ConferenceMember);
  public:
    ConferenceFileMember(Conference * conference, const PFilePath & _fn, PFile::OpenMode mode);

    typedef std::deque<PFilePath> FilenameList;
    ConferenceFileMember(Conference * conference, const FilenameList & _fns, PFile::OpenMode mode);

    ~ConferenceFileMember();

    virtual PString GetName() const
    { return PString(mode == PFile::ReadOnly ? "file player" : "file recorder") & currentFilename; }

    virtual BOOL IsVisible() const
    { return FALSE; }

    void OnReceivedUserInputIndication(const PString & str)
    { cout << "Received user input indication " << str << endl; }

    PDECLARE_NOTIFIER(PThread, ConferenceFileMember, ReadThread);

  protected:
    void Construct();
    BOOL QueueNext();

    FilenameList filenames;
    PFilePath currentFilename;
    PFile::OpenMode mode;

    PThread * thread;
    OpalWAVFile file;
    BOOL running;

    PString roomName;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceCacheMember : public ConferenceMember
{
  PCLASSINFO(ConferenceCacheMember, ConferenceMember);
  public:
    ConferenceCacheMember(Conference * conference, unsigned videoMixerNumber, const OpalMediaFormat & _format, const PString & cacheName);
    ~ConferenceCacheMember();

    virtual void Close();

    virtual PString GetName() const
    { return "cache"; }

    virtual BOOL IsVisible() const
    { return FALSE; }

    void OnReceivedUserInputIndication(const PString & str)
    { cout << "Received user input indication " << str << endl; }

    PString GetMediaFormat()
    { return format; }

    int GetStatus()
    { return status; }

    PString GetCacheName() const
    { return cacheName; }

    int GetCacheUsersNumber() const
    { return (cache ? cache->GetUsersNumber() : 0); }

    const H323Codec * GetCodec() const
    { return codec; }

    bool IsAudio()
    { return isAudio; }

    PDECLARE_NOTIFIER(PThread, ConferenceCacheMember, CacheThread);

  protected:
    OpalMediaFormat format;
    PString cacheName;
    PString roomName;
    int status;

    bool isAudio;

    H323Codec * codec;
    CacheRTP *cache;

    BOOL running;
    PThread * thread;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferencePipeMember : public ConferenceMember
{
  PCLASSINFO(ConferencePipeMember, ConferenceMember);

  public:
    ConferencePipeMember(Conference * conference);
    ~ConferencePipeMember();

    virtual void Close();

    virtual PString GetName() const
    { return PString("file recorder"); }

    virtual PString GetFormat()
    { return GetName(); }

    void OnReceivedUserInputIndication(const PString & str)
    { cout << "Received user input indication " << str << endl; }

    PDECLARE_NOTIFIER(PThread, ConferencePipeMember, AudioThread);
    PDECLARE_NOTIFIER(PThread, ConferencePipeMember, VideoThread);

  protected:
    PString roomName;
    PString trace_section;
    BOOL running;
    PThread * audio_thread;
    PThread * video_thread;
    PString audioPipeName, videoPipeName;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceFilePlayer : public ConferenceFileMember
{
  PCLASSINFO(ConferenceFilePlayer, ConferenceFileMember);
  public:
    ConferenceFilePlayer();

    void Unlisten();

    PDECLARE_NOTIFIER(PThread, ConferenceFilePlayer, Thread);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_FILEMEMBERS_H
