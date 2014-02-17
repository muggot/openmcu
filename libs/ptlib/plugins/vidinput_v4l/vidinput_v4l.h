
#include <sys/mman.h>
#include <sys/time.h>

#include <ptlib.h>
#include <ptlib/videoio.h>
#include <ptlib/vconvert.h>
#include <ptclib/delaychan.h>

#include <linux/videodev.h>

class PVideoInputDevice_V4L: public PVideoInputDevice
{

public:
  PVideoInputDevice_V4L();
  ~PVideoInputDevice_V4L();

  static PStringList GetInputDeviceNames();

  PStringList GetDeviceNames() const
  { return GetInputDeviceNames(); }

  BOOL Open(const PString &deviceName, BOOL startImmediate);

  BOOL IsOpen();

  BOOL Close();

  BOOL Start();
  BOOL Stop();

  BOOL IsCapturing();

  PINDEX GetMaxFrameBytes();

  BOOL GetFrameData(BYTE*, PINDEX*);
  BOOL GetFrameDataNoDelay(BYTE*, PINDEX*);

  BOOL GetFrameSizeLimits(unsigned int&, unsigned int&,
			  unsigned int&, unsigned int&);

  BOOL TestAllFormats();

  BOOL SetFrameSize(unsigned int, unsigned int);
  BOOL SetFrameRate(unsigned int);
  BOOL VerifyHardwareFrameSize(unsigned int, unsigned int);

  BOOL GetParameters(int*, int*, int*, int*, int*);

  BOOL SetColourFormat(const PString&);

  int GetContrast();
  BOOL SetContrast(unsigned int);
  int GetBrightness();
  BOOL SetBrightness(unsigned int);
  int GetWhiteness();
  BOOL SetWhiteness(unsigned int);
  int GetColour();
  BOOL SetColour(unsigned int);
  int GetHue();
  BOOL SetHue(unsigned int);

  BOOL SetVideoChannelFormat(int, PVideoDevice::VideoFormat);
  BOOL SetVideoFormat(PVideoDevice::VideoFormat);
  int GetNumChannels();
  BOOL SetChannel(int);

  BOOL NormalReadProcess(BYTE*, PINDEX*);

  void ClearMapping();
  BOOL RefreshCapabilities();

  PAdaptiveDelay m_pacing;

  int    videoFd;
  struct video_capability videoCapability;
  int    canMap;  // -1 = don't know, 0 = no, 1 = yes
  int    colourFormatCode;
  PINDEX hint_index;
  BYTE *videoBuffer;
  PINDEX frameBytes;
  
  BOOL   pendingSync[2];
  
  int    currentFrame;
  struct video_mbuf frame;
  struct video_mmap frameBuffer[2];
  
};
