#ifndef _PVIDEOIOBSDCAPTURE

#define _PVIDEOIOBSDCAPTURE

#ifdef __GNUC__   
#pragma interface
#endif

#include <sys/mman.h>

#include <ptlib.h>
#include <ptlib/videoio.h>
#include <ptlib/vconvert.h>
#include <ptclib/delaychan.h>

#if defined(P_FREEBSD)
#include <sys/param.h>
# if __FreeBSD_version >= 502100
#include <dev/bktr/ioctl_meteor.h>
# else
#include <machine/ioctl_meteor.h>
# endif
#endif

#if defined(P_OPENBSD) || defined(P_NETBSD)
#if P_OPENBSD >= 200105
#include <dev/ic/bt8xx.h>
#elif P_NETBSD >= 105000000
#include <dev/ic/bt8xx.h>
#else
#include <i386/ioctl_meteor.h>
#endif
#endif

#if !P_USE_INLINES
#include <ptlib/contain.inl>
#endif


class PVideoInputDevice_BSDCAPTURE : public PVideoInputDevice
{

  PCLASSINFO(PVideoInputDevice_BSDCAPTURE, PVideoInputDevice);

public:
  PVideoInputDevice_BSDCAPTURE();
  ~PVideoInputDevice_BSDCAPTURE();

  BOOL Open(
    const PString &deviceName,
    BOOL startImmediate = TRUE
  );

  BOOL IsOpen();

  BOOL Close();

  BOOL Start();
  BOOL Stop();

  BOOL IsCapturing();

  static PStringList GetInputDeviceNames();

  PStringList GetDeviceNames() const
  { return GetInputDeviceNames(); }

  PINDEX GetMaxFrameBytes();

//  BOOL GetFrame(
//    PBYTEArray & frame
//  );
  BOOL GetFrameData(
    BYTE * buffer,
    PINDEX * bytesReturned = NULL
  );
  BOOL GetFrameDataNoDelay(
    BYTE * buffer,
    PINDEX * bytesReturned = NULL
  );

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
//  int GetWhiteness();
//  BOOL SetWhiteness(unsigned int);
//  int GetColour();
//  BOOL SetColour(unsigned int);
  int GetHue();
  BOOL SetHue(unsigned int);

//  BOOL SetVideoChannelFormat(int, PVideoDevice::VideoFormat);
  BOOL SetVideoFormat(PVideoDevice::VideoFormat);
  int GetNumChannels();
  BOOL SetChannel(int);

  BOOL NormalReadProcess(BYTE*, PINDEX*);

  void ClearMapping();

  struct video_capability
  {
      int channels;   /* Num channels */
      int maxwidth;   /* Supported width */
      int maxheight;  /* And height */
      int minwidth;   /* Supported width */
      int minheight;  /* And height */
  };

  int    videoFd;
  struct video_capability videoCapability;
  int    canMap;  // -1 = don't know, 0 = no, 1 = yes
  BYTE * videoBuffer;
  PINDEX frameBytes;
  int    mmap_size;
  PAdaptiveDelay m_pacing;
 
};

#endif
