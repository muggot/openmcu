
#include <ptlib.h>
#include <ptlib/sound.h>
#include <ptlib/socket.h>

//#if !P_USE_INLINES
//#include <ptlib/contain.inl>
//#endif

#ifdef P_SOLARIS
#include <sys/soundcard.h>
#endif

#ifdef P_LINUX
#include <sys/soundcard.h>
#endif

#ifdef P_FREEBSD
#if P_FREEBSD >= 500000
#include <sys/soundcard.h>
#else
#include <machine/soundcard.h>
#endif
#endif

#if defined(P_OPENBSD) || defined(P_NETBSD)
#include <soundcard.h>
#endif

class SoundHandleEntry : public PObject {

  PCLASSINFO(SoundHandleEntry, PObject)

  public:
    SoundHandleEntry();

    int handle;
    int direction;

    unsigned numChannels;
    unsigned sampleRate;
    unsigned bitsPerSample;
    unsigned fragmentValue;
    BOOL isInitialised;
    unsigned resampleRate;
};

class PSoundChannelOSS: public PSoundChannel
{
 public:
    PSoundChannelOSS();
    void Construct();
    PSoundChannelOSS(const PString &device,
                     PSoundChannel::Directions dir,
                     unsigned numChannels,
                     unsigned sampleRate,
		     unsigned bitsPerSample);
    ~PSoundChannelOSS();
    static PStringArray GetDeviceNames(PSoundChannel::Directions = Player);
    static PString GetDefaultDevice(PSoundChannel::Directions);
    BOOL Open(const PString & _device,
              Directions _dir,
              unsigned _numChannels,
              unsigned _sampleRate,
              unsigned _bitsPerSample);
    BOOL Setup();
    BOOL Close();
    BOOL IsOpen() const;
    BOOL Write(const void * buf, PINDEX len);
    BOOL Read(void * buf, PINDEX len);
    BOOL SetFormat(unsigned numChannels,
                   unsigned sampleRate,
                   unsigned bitsPerSample);
    unsigned GetChannels() const;
    unsigned GetSampleRate() const;
    unsigned GetSampleSize() const;
    BOOL SetBuffers(PINDEX size, PINDEX count);
    BOOL GetBuffers(PINDEX & size, PINDEX & count);
    BOOL PlaySound(const PSound & sound, BOOL wait);
    BOOL PlayFile(const PFilePath & filename, BOOL wait);
    BOOL HasPlayCompleted();
    BOOL WaitForPlayCompletion();
    BOOL RecordSound(PSound & sound);
    BOOL RecordFile(const PFilePath & filename);
    BOOL StartRecording();
    BOOL IsRecordBufferFull();
    BOOL AreAllRecordBuffersFull();
    BOOL WaitForRecordBufferFull();
    BOOL WaitForAllRecordBuffersFull();
    BOOL Abort();
    BOOL SetVolume(unsigned newVal);
    BOOL GetVolume(unsigned &devVol);

  protected:
    unsigned mNumChannels;
    unsigned mSampleRate;
    unsigned mBitsPerSample;
    unsigned actualSampleRate;
    Directions direction;
    PString device;
    BOOL isInitialised;
    unsigned resampleRate;
};
