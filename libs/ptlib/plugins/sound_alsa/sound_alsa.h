#include <ptlib.h>
#include <ptlib/sound.h>

#define ALSA_PCM_NEW_HW_PARAMS_API 1
#include <alsa/asoundlib.h>

class PAudioDelay : public PObject
{
  PCLASSINFO(PAudioDelay, PObject);

  public:
    PAudioDelay();
    BOOL Delay(int time);
    void Restart();
    int  GetError();

  protected:
    PTime  previousTime;
    BOOL   firstTime;
    int    error;
};

#define MIN_HEADROOM    30
#define MAX_HEADROOM    60

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
};

#define LOOPBACK_BUFFER_SIZE 5000
#define BYTESINBUF ((startptr<endptr)?(endptr-startptr):(LOOPBACK_BUFFER_SIZE+endptr-startptr))



class PSoundChannelALSA : public PSoundChannel {
 public:
  PSoundChannelALSA();
  void Construct();
  PSoundChannelALSA(const PString &device,
		   PSoundChannel::Directions dir,
		   unsigned numChannels,
		   unsigned sampleRate,
		   unsigned bitsPerSample);
  ~PSoundChannelALSA();
  static PStringArray GetDeviceNames(PSoundChannel::Directions);
  static PString GetDefaultDevice(PSoundChannel::Directions);
  BOOL Open(const PString & _device,
       Directions _dir,
       unsigned _numChannels,
       unsigned _sampleRate,
       unsigned _bitsPerSample);
  BOOL Setup();
  BOOL Close();
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
  BOOL SetVolume (unsigned);
  BOOL GetVolume (unsigned &);
  BOOL IsOpen() const;

 private:

  static void UpdateDictionary(PSoundChannel::Directions);
  BOOL Volume (BOOL, unsigned, unsigned &);
  PSoundChannel::Directions direction;
  PString device;
  unsigned mNumChannels;
  unsigned mSampleRate;
  unsigned mBitsPerSample;
  BOOL isInitialised;

  snd_pcm_t *os_handle; /* Handle, different from the PChannel handle */
  int card_nr;

  PMutex device_mutex;

  /**number of 30 (or 20) ms long sound intervals stored by ALSA. Typically, 2.*/
  PINDEX storedPeriods;

  /**Total number of bytes of audio stored by ALSA.  Typically, 2*480 or 960.*/
  PINDEX storedSize;

  /** Number of bytes in a ALSA frame. a frame may only be 4ms long*/
  int frameBytes; 
};
