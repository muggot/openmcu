/*
 * vidinput_v4l2.h
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2000 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library (V4L plugin).
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): Derek Smithies (derek@indranet.co.nz)
 *                 Mark Cooke (mpc@star.sr.bham.ac.uk)
 *                 Nicola Orru' <nigu@itadinanta.it>
 *
 * $Log: vidinput_v4l2.h,v $
 * Revision 1.9  2007/04/27 17:34:45  dsandras
 * Applied patch from Luc Saillard to fix things after the latest change
 * which broke all drivers. Thanks Luc <luc saillard org>.
 *
 * Revision 1.8  2007/02/19 22:26:15  dsandras
 * Fixed V4L2 OpenSolaris support thanks to Elaine Xiong <elaine xiong sun
 * com> (Ekiga report #407820). Thanks !
 *
 * Revision 1.7  2006/11/01 17:55:37  dsandras
 * Applied patch from Brian Lu <brian lu sun com> to fix V4L2 on OpenSolaris.
 *
 * Revision 1.6  2006/03/12 11:16:19  dsandras
 * Added multi-buffering support to V4L2 thanks to Luc Saillard. Thanks!
 *
 * Revision 1.5  2006/01/09 18:22:42  dsandras
 * Use memset before some ioctl() to make valgrind happy.
 * Create a common function to set and get control information.
 * Fix range values return by the driver.
 * Fix setting value to be in the range (>>16 is unsigned).
 * Add support for YUY2.
 * Patch from Luc Saillard <luc _AT___ saillard.org>. Many thanks!
 *
 * Revision 1.4  2005/08/09 09:08:10  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.3.4.2  2005/07/24 09:01:49  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.3.4.1  2005/07/17 11:30:42  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.3  2004/11/07 22:48:47  dominance
 * fixed copyright of v4l2 plugin. Last commit's credits go to Nicola Orru' <nigu@itadinanta.it> ...
 *
 */
#ifndef _PVIDEOIOV4L2
#define _PVIDEOIOV4L2


#include <sys/mman.h>
#include <sys/time.h>

#include <ptlib.h>
#include <ptlib/videoio.h>
#include <ptlib/vconvert.h>
#include <ptclib/delaychan.h>

#ifdef SOLARIS
#include <sys/videodev2.h>
#else
#include <linux/videodev.h>
#endif

#ifndef V4L2_PIX_FMT_SBGGR8
#define V4L2_PIX_FMT_SBGGR8  v4l2_fourcc('B','A','8','1') /*  8  BGBG.. GRGR.. */
#endif

class PVideoInputDevice_V4L2: public PVideoInputDevice
{

  PCLASSINFO(PVideoInputDevice_V4L2, PVideoInputDevice);
      

public:
  PVideoInputDevice_V4L2();
  ~PVideoInputDevice_V4L2();
  
  void ReadDeviceDirectory (PDirectory, POrdinalToString &);

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

  int GetControlCommon(unsigned int control, int *value);
  BOOL SetControlCommon(unsigned int control, int newValue);

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

  BOOL SetMapping();

  struct v4l2_capability videoCapability;
  struct v4l2_streamparm videoStreamParm;
  BOOL   canRead;
  BOOL   canStream;
  BOOL   canSelect;
  BOOL   canSetFrameRate;
  BOOL   isMapped;
#define NUM_VIDBUF 4
  BYTE * videoBuffer[NUM_VIDBUF];
  uint   videoBufferCount;
  uint   currentvideoBuffer;

  int    videoFd;
  int    frameBytes;
  BOOL   started;
  PAdaptiveDelay m_pacing;
};

#endif
