/*
 * vidinput_v4l2.cxx
 *
 * Classes to support streaming video input (grabbing) and output.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
 * Copyright (c) 2003 March Networks
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
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * First V4L2 capture code written by March Networks 
 * (http://www.marchnetworks.com) 
 *
 * This code is based on the Video4Linux 1 code.
 *
 * Contributor(s): Guilhem Tardy (gtardy@salyens.com)
 *  Nicola Orru' <nigu@itadinanta.it>
 *
 * $Log: vidinput_v4l2.cxx,v $
 * Revision 1.24  2007/06/02 12:21:42  dsandras
 * Fixed problems with the sa7134 card and with newer kernels thanks to
 * Michael Smith <msmith cbnco com>. Many thanks !
 *
 * Revision 1.23  2007/04/27 17:34:45  dsandras
 * Applied patch from Luc Saillard to fix things after the latest change
 * which broke all drivers. Thanks Luc <luc saillard org>.
 *
 * Revision 1.22  2007/02/19 22:26:14  dsandras
 * Fixed V4L2 OpenSolaris support thanks to Elaine Xiong <elaine xiong sun
 * com> (Ekiga report #407820). Thanks !
 *
 * Revision 1.21  2007/01/03 22:35:50  dsandras
 * Fixed possible race condition while detecting available devices. (#376078, #328753).
 *
 * Revision 1.20  2006/11/28 21:06:12  dsandras
 * Added a few missing mutexes in order to prevent collection
 * corruption when the update is called from different threads.
 * Hopefully fixes Ekiga report #376078.
 *
 * Revision 1.19  2006/11/01 17:55:37  dsandras
 * Applied patch from Brian Lu <brian lu sun com> to fix V4L2 on OpenSolaris.
 *
 * Revision 1.18  2006/05/06 15:29:38  dsandras
 * Applied patch from Martin Rubli <martin rubli logitech com> to fix framerate
 * and computation issues. Thanks a lot!
 *
 * Revision 1.17  2006/03/12 11:16:19  dsandras
 * Added multi-buffering support to V4L2 thanks to Luc Saillard. Thanks!
 *
 * Revision 1.16  2006/03/12 11:09:51  dsandras
 * Applied patch from Luc Saillard to fix problems with MJPEG. Thanks!
 *
 * Revision 1.15  2006/03/06 19:09:31  dsandras
 * Applied patch from Luc Saillard <luc saillard org> to fix YUV422, which
 * is in fact YUYV.
 *
 * Revision 1.14  2006/02/06 22:23:08  dsandras
 * Added UYVY palette thanks to Luc Saillard <luc saillard org>. Thanks!
 *
 * Revision 1.13  2006/01/29 22:46:41  csoutheren
 * Added support for cameras that return MJPEG streams
 * Thanks to Luc Saillard and Damien Sandras
 *
 * Revision 1.12  2006/01/21 13:59:50  dsandras
 * Added BGR colour format thanks to Luc Saillard <luc saillard org>. Thanks!
 *
 * Revision 1.11  2006/01/17 22:28:26  dsandras
 * Another patch from Luc Saillard <luc saillard org> to fix V4L2 support when
 * opening/closing the device several times in a row. Thanks a lot!!!!!
 *
 * Revision 1.10  2006/01/09 18:22:42  dsandras
 * Use memset before some ioctl() to make valgrind happy.
 * Create a common function to set and get control information.
 * Fix range values return by the driver.
 * Fix setting value to be in the range (>>16 is unsigned).
 * Add support for YUY2.
 * Patch from Luc Saillard <luc _AT___ saillard.org>. Many thanks!
 *
 * Revision 1.9  2006/01/07 16:10:21  dsandras
 * More changes from Luc Saillard. Thanks!
 *
 * Revision 1.8  2006/01/05 19:21:37  dsandras
 * Applied patch from Luc Saillard <luc _____AT_ saillard.org>. Many thanks!
 *
 * Revision 1.7  2005/12/21 21:31:39  dsandras
 * Fixed build with gcc 4.1.
 *
 * Revision 1.6  2005/11/30 12:47:39  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.5  2005/08/09 09:08:10  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.4.4.2  2005/07/24 09:01:48  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.4.4.1  2005/07/17 11:30:42  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.4  2004/11/07 22:48:47  dominance
 * fixed copyright of v4l2 plugin. Last commit's credits go to Nicola Orru' <nigu@itadinanta.it> ...
 *
 * Revision 1.3  2004/11/07 21:34:21  dominance
 * v4l2 patch to add verbose device names detection.
 *
 * Revision 1.2  2004/10/27 09:22:59  dsandras
 * Added patch from Nicola Orru' to make things work better.
 *
 * Revision 1.1  2004/09/21 12:54:23  dsandras
 * Added initial port to the new pwlib API/V4L2 API for the video4linux 2 code of Guilhem Tardy. Thanks!
 *
 * Revision 1.0  2003/03/03 12:27:00  guilhem
 * First build.
 *
 */

#pragma implementation "vidinput_v4l2.h"

#include "vidinput_v4l2.h"
#include <sys/utsname.h>

PCREATE_VIDINPUT_PLUGIN(V4L2);

#include "vidinput_names.h" 

class V4L2Names : public V4LXNames
{

  PCLASSINFO(V4L2Names, V4LXNames);

public:
  
  V4L2Names() { kernelVersion=KUNKNOWN; };

  virtual void Update ();
  
protected:
  
  virtual PString BuildUserFriendly(PString devname);

  enum KernelVersionEnum {
    K2_4, 
    K2_6,
    KUNKNOWN,
  } kernelVersion;

};


PMutex creationMutex;
static 
V4L2Names & GetNames()
{
  PWaitAndSignal m(creationMutex);
  static V4L2Names names;
  names.Update();
  return names;
}

///////////////////////////////////////////////////////////////////////////////
// PVideoInputDevice_V4L2

PVideoInputDevice_V4L2::PVideoInputDevice_V4L2()
{
  videoFd = -1;
  canRead = FALSE;
  canStream = FALSE;
  canSelect = FALSE;
  canSetFrameRate = FALSE;
  isMapped = FALSE;
  started = FALSE;
}

PVideoInputDevice_V4L2::~PVideoInputDevice_V4L2()
{
  Close();
}


#ifndef V4L2_PIX_FMT_H263
#define V4L2_PIX_FMT_H263       v4l2_fourcc('H','2','6','3')
#endif


static struct {
  const char * colourFormat;
#ifdef SOLARIS
  uint32_t code;
#else
  __u32 code;
#endif
} colourFormatTab[] = {
    { "Grey", V4L2_PIX_FMT_GREY },   //Entries in this table correspond
    { "RGB32", V4L2_PIX_FMT_RGB32 }, //(line by line) to those in the 
    { "BGR32", V4L2_PIX_FMT_BGR32 }, //PVideoDevice ColourFormat table.
    { "RGB24", V4L2_PIX_FMT_RGB24 }, 
    { "BGR24", V4L2_PIX_FMT_BGR24 },
    { "RGB565", V4L2_PIX_FMT_RGB565 },
    { "RGB555", V4L2_PIX_FMT_RGB555 },
    { "YUV411", V4L2_PIX_FMT_Y41P },
    { "YUV411P", V4L2_PIX_FMT_YUV411P },
    { "YUV420", V4L2_PIX_FMT_NV21 },
    { "YUV420P", V4L2_PIX_FMT_YUV420 },
    { "YUV422", V4L2_PIX_FMT_YUYV },   /* Note: YUV422 is for compatibility */
    { "YUV422P", V4L2_PIX_FMT_YUV422P },
    { "YUY2", V4L2_PIX_FMT_YUYV },
    { "JPEG", V4L2_PIX_FMT_JPEG },
    { "H263", V4L2_PIX_FMT_H263 },
    { "SBGGR8", V4L2_PIX_FMT_SBGGR8 },
    { "MJPEG", V4L2_PIX_FMT_MJPEG},
    { "UYVY422", V4L2_PIX_FMT_UYVY}
};


BOOL PVideoInputDevice_V4L2::Open(const PString & devName, BOOL startImmediate)
{
  struct utsname buf;
  PString version;
  
  uname (&buf);

  if (buf.release)
    version = PString (buf.release);

  PTRACE(1,"PVidInDev\tOpen()\tvideoFd:" << videoFd);
  Close();

  PString name = GetNames().GetDeviceName(devName);
  PTRACE(1,"PVidInDev\tOpen()\tdevName:" << name << "  videoFd:" << videoFd);
  
  videoFd = ::open((const char *)name, O_RDWR);
  if (videoFd < 0) {
    PTRACE(1,"PVidInDev\topen failed : " << ::strerror(errno));
    return FALSE;
  }
  
  PTRACE(6,"PVidInDev\topen, fd=" << videoFd);
  deviceName=name;

  // Don't share the camera device with subprocesses - they could cause
  // EBUSY errors on VIDIOC_STREAMON if the parent tries to close and reopen
  // the camera while the child is still running.
  ::fcntl(videoFd, F_SETFD, FD_CLOEXEC);

  // get the device capabilities
  if (::ioctl(videoFd, VIDIOC_QUERYCAP, &videoCapability) < 0) {
    PTRACE(1,"PVidInDev\tQUERYCAP failed : " << ::strerror(errno));
    ::close (videoFd);
    videoFd = -1;
    return FALSE;
  }
    
  canRead = videoCapability.capabilities & V4L2_CAP_READWRITE;
  canStream = videoCapability.capabilities & V4L2_CAP_STREAMING;
  canSelect = videoCapability.capabilities & V4L2_CAP_ASYNCIO;

  // set height and width
  frameHeight = QCIFHeight;
  frameWidth  = QCIFWidth;


  // get the capture parameters
  videoStreamParm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (::ioctl(videoFd, VIDIOC_G_PARM, &videoStreamParm) < 0)  {

    PTRACE(1,"PVidInDev\tG_PARM failed : " << ::strerror(errno));
    canSetFrameRate = FALSE;

  } else {

    canSetFrameRate = videoStreamParm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME;
    if (canSetFrameRate)
      PVideoDevice::SetFrameRate (videoStreamParm.parm.capture.timeperframe.denominator / videoStreamParm.parm.capture.timeperframe.numerator);
  }
  
  return TRUE;
}


BOOL PVideoInputDevice_V4L2::IsOpen() 
{
  return videoFd >= 0;
}


BOOL PVideoInputDevice_V4L2::Close()
{
  PTRACE(1,"PVidInDev\tClose()\tvideoFd:" << videoFd << "  started:" << started);
  if (!IsOpen())
    return FALSE;

  Stop();
  ClearMapping();
  ::close(videoFd);

  PTRACE(6,"PVidInDev\tclose, fd=" << videoFd);

  videoFd = -1;
  canRead = FALSE;
  canStream = FALSE;
  canSelect = FALSE;
  canSetFrameRate = FALSE;
  isMapped = FALSE;

  PTRACE(1,"PVidInDev\tClose()\tvideoFd:" << videoFd << "  started:" << started);
  return TRUE;
}


BOOL PVideoInputDevice_V4L2::Start()
{
  // automatically set mapping
  if (!isMapped && !SetMapping()) {
    ClearMapping();
    canStream = FALSE; // don't try again
    return FALSE;
  }

  if (!started) {
    PTRACE(6,"PVidInDev\tstart queuing all buffers, fd=" << videoFd);

    /* Queue all buffers */
    currentvideoBuffer = 0;

    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    for (unsigned int i=0; i<videoBufferCount; i++) {

       buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
       buf.memory = V4L2_MEMORY_MMAP;
       buf.index = i;

       if (::ioctl(videoFd, VIDIOC_QBUF, &buf) < 0) {
	  PTRACE(3,"PVidInDev\tVIDIOC_QBUF failed for buffer " << i << ": " << ::strerror(errno));
	  return FALSE;
       }
    }

    /* Start streaming */
    PTRACE(6,"PVidInDev\tstart streaming, fd=" << videoFd);
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (::ioctl(videoFd, VIDIOC_STREAMON, &type) < 0) {
       PTRACE(3,"PVidInDev\tSTREAMON failed : " << ::strerror(errno));
       return FALSE;
    }

    started = TRUE;

  }
  return TRUE;
}


BOOL PVideoInputDevice_V4L2::Stop()
{
  if (started) {
    PTRACE(6,"PVidInDev\tstop streaming, fd=" << videoFd);

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    started = FALSE;

    if (::ioctl(videoFd, VIDIOC_STREAMOFF, &type) < 0) {
      PTRACE(3,"PVidInDev\tSTREAMOFF failed : " << ::strerror(errno));
      return FALSE;
    }

    // no need to dequeue filled buffers, as this is handled by V4L2 at the next VIDIOC_STREAMON
  }

  return TRUE;
}


BOOL PVideoInputDevice_V4L2::IsCapturing()
{
  return started;
}

PStringList PVideoInputDevice_V4L2::GetInputDeviceNames()
{
  return GetNames().GetInputDeviceNames();  
}


BOOL PVideoInputDevice_V4L2::SetVideoFormat(VideoFormat newFormat)
{
  if (newFormat == Auto) {
    if (SetVideoFormat(PAL) ||
      SetVideoFormat(NTSC) ||
      SetVideoFormat(SECAM))
      return TRUE;
    else
      return FALSE;
  }

  if (!PVideoDevice::SetVideoFormat(newFormat)) {
    PTRACE(1,"PVideoDevice::SetVideoFormat failed for format " << newFormat);
    return FALSE;
  }

  struct {
#ifdef SOLARIS
    uint32_t code;
#else
    __u32 code;
#endif
    const char * name;
  } static const fmt[3] = { {V4L2_STD_PAL, "PAL"},
      {V4L2_STD_NTSC, "NTSC"},
      {V4L2_STD_SECAM, "SECAM"} };

  // set the video standard
  if (::ioctl(videoFd, VIDIOC_S_STD, &fmt[newFormat].code) < 0) {
    PTRACE(1,"VideoInputDevice\tS_STD failed : " << ::strerror(errno));
  }

  PTRACE(6,"PVidInDev\tset video format \"" << fmt[newFormat].name << "\", fd=" << videoFd);

  return TRUE;
}


int PVideoInputDevice_V4L2::GetNumChannels() 
{
  // if opened, return the capability value, else 1 as in videoio.cxx
  if (IsOpen ()) {

    struct v4l2_input videoEnumInput;
    videoEnumInput.index = 0;
    while (1) {
      if (::ioctl(videoFd, VIDIOC_ENUMINPUT, &videoEnumInput) < 0) {
        break;
      }
      else
        videoEnumInput.index++;
    }

    return videoEnumInput.index;
  }
  else
    return 1;
}


BOOL PVideoInputDevice_V4L2::SetChannel(int newChannel)
{
  if (!PVideoDevice::SetChannel(newChannel)) {
    PTRACE(1,"PVideoDevice::SetChannel failed for channel " << newChannel);
    return FALSE;
  }

  // set the channel
  if (::ioctl(videoFd, VIDIOC_S_INPUT, &channelNumber) < 0) {
    PTRACE(1,"VideoInputDevice\tS_INPUT failed : " << ::strerror(errno));    
    return FALSE;
  }

  PTRACE(6,"PVidInDev\tset channel " << newChannel << ", fd=" << videoFd);

  return TRUE;
}


BOOL PVideoInputDevice_V4L2::SetVideoChannelFormat (int newChannel, VideoFormat videoFormat) 
{
  if (!SetChannel(newChannel) ||
      !SetVideoFormat(videoFormat))
    return FALSE;

  return TRUE;
}


BOOL PVideoInputDevice_V4L2::SetColourFormat(const PString & newFormat)
{
  PINDEX colourFormatIndex = 0;
  while (newFormat != colourFormatTab[colourFormatIndex].colourFormat) {
    colourFormatIndex++;
    if (colourFormatIndex >= PARRAYSIZE(colourFormatTab))
      return FALSE;
  }

  if (!PVideoDevice::SetColourFormat(newFormat)) {
    PTRACE(3,"PVidInDev\tSetColourFormat failed for colour format " << newFormat);
    return FALSE;
  }

  BOOL resume = started;
  Stop();
  ClearMapping();

  struct v4l2_format videoFormat;
  memset(&videoFormat, 0, sizeof(struct v4l2_format));
  videoFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  // get the frame rate so we can preserve it throughout the S_FMT call
  struct v4l2_streamparm streamParm;
  unsigned int fi_n = 0, fi_d = 0;
  streamParm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (::ioctl(videoFd, VIDIOC_G_PARM, &streamParm) == 0 &&
        streamParm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) {
    fi_n = streamParm.parm.capture.timeperframe.numerator;
    fi_d = streamParm.parm.capture.timeperframe.denominator;
  } else {
    PTRACE(1,"PVidInDev\tG_PARM failed (preserving frame rate may not work) : " << ::strerror(errno));
  }

  // get the colour format
  if (::ioctl(videoFd, VIDIOC_G_FMT, &videoFormat) < 0) {
    PTRACE(1,"PVidInDev\tG_FMT failed : " << ::strerror(errno));
    return FALSE;
  }

  videoFormat.fmt.pix.pixelformat = colourFormatTab[colourFormatIndex].code;

  // set the colour format
  if (::ioctl(videoFd, VIDIOC_S_FMT, &videoFormat) < 0) {
    PTRACE(1,"PVidInDev\tS_FMT failed : " << ::strerror(errno));
    PTRACE(1,"\tused code of " << videoFormat.fmt.pix.pixelformat << " for palette: " << colourFormatTab[colourFormatIndex].colourFormat);
    return FALSE;
  }

  // get the colour format again to be careful about broken drivers
  if (::ioctl(videoFd, VIDIOC_G_FMT, &videoFormat) < 0) {
    PTRACE(1,"PVidInDev\tG_FMT failed : " << ::strerror(errno));
    return FALSE;
  }

  if (videoFormat.fmt.pix.pixelformat != colourFormatTab[colourFormatIndex].code) {
    PTRACE(3,"PVidInDev\tcolour format mismatch.");
    return FALSE;
  }

  // reset the frame rate because it may have been overridden by the call to S_FMT
  if (fi_n == 0 || fi_d == 0 || ::ioctl(videoFd, VIDIOC_S_PARM, &streamParm) < 0) {
    PTRACE(3,"PVidInDev\tunable to reset frame rate.");
  } else if (streamParm.parm.capture.timeperframe.numerator != fi_n ||
             streamParm.parm.capture.timeperframe.denominator  != fi_d) {
    PTRACE(3, "PVidInDev\tnew frame interval (" << streamParm.parm.capture.timeperframe.numerator
              << "/" << streamParm.parm.capture.timeperframe.denominator
              << ") differs from what was requested (" << fi_n << "/" << fi_d << ").");
  }

  frameBytes = videoFormat.fmt.pix.sizeimage;

  PTRACE(6,"PVidInDev\tset colour format \"" << newFormat << "\", fd=" << videoFd);

  if (resume)
    return Start();

  return TRUE;
}


BOOL PVideoInputDevice_V4L2::SetFrameRate(unsigned rate)
{
  if (!PVideoDevice::SetFrameRate(rate)) {
    PTRACE(3,"PVidInDev\tSetFrameRate failed for rate " << rate);
    return TRUE; // Ignore
  }

  if (canSetFrameRate) {
    videoStreamParm.parm.capture.timeperframe.numerator = 1;
    videoStreamParm.parm.capture.timeperframe.denominator = (rate ? rate : 1);

    // set the stream parameters
    if (::ioctl(videoFd, VIDIOC_S_PARM, &videoStreamParm) < 0)  {
      PTRACE(1,"PVidInDev\tS_PARM failed : "<< ::strerror(errno));
      return TRUE;
    }

    PTRACE(6,"PVidInDev\tset frame rate " << rate << "fps, fd=" << videoFd);
  }

  return TRUE;
}


BOOL PVideoInputDevice_V4L2::GetFrameSizeLimits(unsigned & minWidth,
                                                unsigned & minHeight,
                                                unsigned & maxWidth,
                                                unsigned & maxHeight) 
{
  minWidth=0;
  maxWidth=65535;
  minHeight=0;
  maxHeight=65535;

  // Before 2.6.19 there is no official way to enumerate frame sizes
  // in V4L2, but we can use VIDIOC_TRY_FMT to find the largest supported
  // size. This is roughly what the kernel V4L1 compatibility layer does.

  struct v4l2_format fmt;
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (::ioctl(videoFd, VIDIOC_G_FMT, &fmt) < 0) {
    return FALSE;
  }

  fmt.fmt.pix.width = fmt.fmt.pix.height = 10000;
  if (::ioctl(videoFd, VIDIOC_TRY_FMT, &fmt) < 0) {
    return FALSE;
  }
  maxWidth = fmt.fmt.pix.width;
  maxHeight = fmt.fmt.pix.height;
  return TRUE;
}


BOOL PVideoInputDevice_V4L2::SetFrameSize(unsigned width, unsigned height)
{
  if (!PVideoDevice::SetFrameSize(width, height)) {
    PTRACE(3,"PVidInDev\tSetFrameSize failed for size " << width << "x" << height);
    return FALSE;
  }

  BOOL resume = started;
  Stop();
  ClearMapping();

  if (!VerifyHardwareFrameSize(width, height)) {
    PTRACE(3,"PVidInDev\tVerifyHardwareFrameSize failed for size " << width << "x" << height);
    return FALSE;
  }

  PTRACE(6,"PVidInDev\tset frame size " << width << "x" << height << ", fd=" << videoFd);

  if (resume)
    return Start();

  return TRUE;
}


PINDEX PVideoInputDevice_V4L2::GetMaxFrameBytes()
{
  return GetMaxFrameBytesConverted(frameBytes);
}


BOOL PVideoInputDevice_V4L2::SetMapping()
{
  if (!canStream)
    return FALSE;

  struct v4l2_requestbuffers reqbuf;
  reqbuf.count = NUM_VIDBUF;
  reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  reqbuf.memory = V4L2_MEMORY_MMAP;

  if (::ioctl(videoFd, VIDIOC_REQBUFS, &reqbuf) < 0) {
    PTRACE(3,"PVidInDev\tREQBUFS failed : " << ::strerror(errno));
    return FALSE;
  }
  if (reqbuf.count < 1) {
    PTRACE(3,"PVidInDev\tNot enough video buffer available. (got " << reqbuf.count << ")");
    return FALSE;
  }
  if (reqbuf.count > NUM_VIDBUF) {
    PTRACE(3,"PVidInDev\tToo much video buffer allocated. (got " << reqbuf.count << ")");
    return FALSE;
  }

  struct v4l2_buffer buf;
  memset(&buf, 0, sizeof(buf));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  
  videoBufferCount = reqbuf.count;
  for (buf.index = 0; buf.index < videoBufferCount; buf.index++) {
    if (::ioctl(videoFd, VIDIOC_QUERYBUF, &buf) < 0) {
      PTRACE(3,"PVidInDev\tQUERYBUF failed : " << ::strerror(errno));
      return FALSE;
    }

    if ((videoBuffer[buf.index] = (BYTE *)::mmap(0, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, videoFd, buf.m.offset)) == MAP_FAILED) {
      PTRACE(3,"PVidInDev\tmmap failed : " << ::strerror(errno));
      return FALSE;
    }
  }

  isMapped = TRUE;

  PTRACE(7,"PVidInDev\tset mapping for " << videoBufferCount << " buffers, fd=" << videoFd);


  return TRUE;
}


void PVideoInputDevice_V4L2::ClearMapping()
{
  if (!canStream) // 'isMapped' wouldn't handle partial mappings
    return;

  struct v4l2_buffer buf;
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  for (buf.index = 0; ; buf.index++) {
    if (::ioctl(videoFd, VIDIOC_QUERYBUF, &buf) < 0)
      break;

#ifdef SOLARIS
    ::munmap((char*)videoBuffer[buf.index], buf.length);
#else
    ::munmap(videoBuffer[buf.index], buf.length);
#endif
  }

  isMapped = FALSE;

  PTRACE(7,"PVidInDev\tclear mapping, fd=" << videoFd);
}


BOOL PVideoInputDevice_V4L2::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{
  PTRACE(8,"PVidInDev\tGetFrameData()");

  m_pacing.Delay(1000/GetFrameRate());
  return GetFrameDataNoDelay(buffer, bytesReturned);
}


BOOL PVideoInputDevice_V4L2::GetFrameDataNoDelay(BYTE * buffer, PINDEX * bytesReturned)
{
  PTRACE(8,"PVidInDev\tGetFrameDataNoDelay()\tstarted:" << started << "  canSelect:" << canSelect);

  if (!started)
    return NormalReadProcess(buffer, bytesReturned);

  struct v4l2_buffer buf;
  memset(&buf, 0, sizeof(struct v4l2_buffer));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = currentvideoBuffer;

  if (::ioctl(videoFd, VIDIOC_DQBUF, &buf) < 0) {
    // strace resistance
    if (errno == EINTR) {
        if (::ioctl(videoFd, VIDIOC_DQBUF, &buf) < 0) {
          PTRACE(1,"PVidInDev\tDQBUF failed : " << ::strerror(errno));
          return FALSE;
        }
    }
  }

  currentvideoBuffer = (currentvideoBuffer+1) % NUM_VIDBUF;

  // If converting on the fly do it from frame store to output buffer,
  // otherwise do straight copy.
  if (converter != NULL)
    converter->Convert(videoBuffer[buf.index], buffer, buf.bytesused, bytesReturned);
  else {
    memcpy(buffer, videoBuffer[buf.index], buf.bytesused);
    if (bytesReturned != NULL)
      *bytesReturned = buf.bytesused;
  }

  PTRACE(8,"PVidInDev\tget frame data of " << buf.bytesused << "bytes, fd=" << videoFd);

  // requeue the buffer
  if (::ioctl(videoFd, VIDIOC_QBUF, &buf) < 0) {
    PTRACE(1,"PVidInDev\tQBUF failed : " << ::strerror(errno));
  }

  return TRUE;
}


// This video device does not support memory mapping - so use
// normal read process to extract a frame of video data.
BOOL PVideoInputDevice_V4L2::NormalReadProcess(BYTE * buffer, PINDEX * bytesReturned)
{ 
  if (!canRead)
    return FALSE;

  ssize_t bytesRead;

  do
    bytesRead = ::read(videoFd, buffer, frameBytes);
  while (bytesRead < 0 && errno == EINTR && IsOpen());

  if (bytesRead < 0) {
    
    PTRACE(1,"PVidInDev\tread failed (read = "<<bytesRead<< " expected " << frameBytes <<")");
    bytesRead = frameBytes;
  }

  if ((PINDEX)bytesRead != frameBytes) {
    PTRACE(1,"PVidInDev\tread returned fewer bytes than expected");
    // May result from a compressed format, otherwise indicates an error.
  }

  if (converter != NULL)
    return converter->ConvertInPlace(buffer, bytesReturned);

  if (bytesReturned != NULL)
    *bytesReturned = (PINDEX)bytesRead;

  return TRUE;
}

BOOL PVideoInputDevice_V4L2::VerifyHardwareFrameSize(unsigned width, unsigned height)
{
  struct v4l2_format videoFormat;
  videoFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  struct v4l2_streamparm streamParm;
  unsigned int fi_n = 0, fi_d = 0;
  streamParm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  // get the frame size
  if (::ioctl(videoFd, VIDIOC_G_FMT, &videoFormat) < 0) {
    PTRACE(1,"PVidInDev\tG_FMT failed : " << ::strerror(errno));
    return FALSE;
  }

  // get the frame rate so we can preserve it throughout the S_FMT call
  // Sidenote: V4L2 gives us the frame interval, i.e. 1/fps.
  if (::ioctl(videoFd, VIDIOC_G_PARM, &streamParm) == 0 &&
        streamParm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) {
    fi_n = streamParm.parm.capture.timeperframe.numerator;
    fi_d = streamParm.parm.capture.timeperframe.denominator;
  } else {
    PTRACE(1,"PVidInDev\tG_PARM failed (preserving frame rate may not work) : " << ::strerror(errno));
  }

  videoFormat.fmt.pix.width = width;
  videoFormat.fmt.pix.height = height;

  // set the frame size
  if (::ioctl(videoFd, VIDIOC_S_FMT, &videoFormat) < 0) {
    PTRACE(1,"PVidInDev\tS_FMT failed : " << ::strerror(errno));
    PTRACE(1,"\tused frame size of " << videoFormat.fmt.pix.width << "x" << videoFormat.fmt.pix.height);
    return FALSE;
  }

  // get the frame size again to be careful about broken drivers
  if (::ioctl(videoFd, VIDIOC_G_FMT, &videoFormat) < 0) {
    PTRACE(1,"PVidInDev\tG_FMT failed : " << ::strerror(errno));
    return FALSE;
  }

  if ((videoFormat.fmt.pix.width != width) || (videoFormat.fmt.pix.height != height)) {
    PTRACE(3,"PVidInDev\tframe size mismatch.");
    // allow the device to return actual frame size
    PVideoDevice::SetFrameSize(videoFormat.fmt.pix.width, videoFormat.fmt.pix.height);
    return FALSE;
  }

  // reset the frame rate because it may have been overridden by the call to S_FMT
  if (fi_n == 0 || fi_d == 0 || ::ioctl(videoFd, VIDIOC_S_PARM, &streamParm) < 0) {
    PTRACE(3,"PVidInDev\tunable to reset frame rate.");
  } else if (streamParm.parm.capture.timeperframe.numerator != fi_n ||
             streamParm.parm.capture.timeperframe.denominator  != fi_d) {
    PTRACE(3, "PVidInDev\tnew frame interval (" << streamParm.parm.capture.timeperframe.numerator
              << "/" << streamParm.parm.capture.timeperframe.denominator
              << ") differs from what was requested (" << fi_n << "/" << fi_d << ").");
  }

  frameBytes = videoFormat.fmt.pix.sizeimage;
  return TRUE;
}

/**
 * Query the current control setting
 * @param control is v4l2 control id (V4L2_CID_BRIGHTNESS, V4L2_CID_WHITENESS, ...)
 * @return  -1 control is unknown, or an error occured
 *         >=0 current value in a range [0-65535]
 */
int PVideoInputDevice_V4L2::GetControlCommon(unsigned int control, int *value) 
{
  if (!IsOpen())
    return -1;

  struct v4l2_queryctrl q;
  memset(&q, 0, sizeof(struct v4l2_queryctrl));
  q.id = control;
  if (::ioctl(videoFd, VIDIOC_QUERYCTRL, &q) < 0)
    return -1;

  struct v4l2_control c;
  memset(&c, 0, sizeof(struct v4l2_control));
  c.id = control;
  if (::ioctl(videoFd, VIDIOC_G_CTRL, &c) < 0)
    return -1;

  *value = ((c.value - q.minimum) * 65536) / ((q.maximum-q.minimum));
  return *value;
}

int PVideoInputDevice_V4L2::GetBrightness() 
{ 
  return GetControlCommon(V4L2_CID_BRIGHTNESS, &frameBrightness);
}

int PVideoInputDevice_V4L2::GetWhiteness() 
{
  return GetControlCommon(V4L2_CID_WHITENESS, &frameWhiteness);
}

int PVideoInputDevice_V4L2::GetColour() 
{ 
  return GetControlCommon(V4L2_CID_SATURATION, &frameColour);
}

int PVideoInputDevice_V4L2::GetContrast() 
{
  return GetControlCommon(V4L2_CID_CONTRAST, &frameContrast);
}

int PVideoInputDevice_V4L2::GetHue() 
{
  return GetControlCommon(V4L2_CID_HUE, &frameHue);
}

/**
 * Set a control to a new value
 *
 * @param control: V4L2_CID_BRIGHTNESS, V4L2_CID_WHITENESS, ...
 * @param newValue: 0-65535 Set this control to this range
 *                  -1 Set the default value
 * @return FALSE, if an error occur or the control is not supported
 */
BOOL PVideoInputDevice_V4L2::SetControlCommon(unsigned int control, int newValue)
{
#ifdef __sun
 PTRACE(1,"PVidInDev\t" << __FILE__ << ":" << __LINE__  << "() videoFd=" << videoFd);
#else
  PTRACE(1,"PVidInDev\t" << __FUNCTION__  << "() videoFd=" << videoFd);
#endif
  if (!IsOpen())
    return FALSE;

  struct v4l2_queryctrl q;
  memset(&q, 0, sizeof(struct v4l2_queryctrl));
  q.id = control;
  if (::ioctl(videoFd, VIDIOC_QUERYCTRL, &q) < 0)
    return FALSE;

  struct v4l2_control c;
  memset(&c, 0, sizeof(struct v4l2_control));
  c.id = control;
  if (newValue < 0)
    c.value = q.default_value;
  else
    c.value = q.minimum + ((q.maximum-q.minimum) * newValue)/65535;

  if (::ioctl(videoFd, VIDIOC_S_CTRL, &c) < 0)
    return FALSE;

  return TRUE;
}

BOOL PVideoInputDevice_V4L2::SetBrightness(unsigned newBrightness) 
{ 
  if (!SetControlCommon(V4L2_CID_BRIGHTNESS, newBrightness))
    return FALSE;
  frameBrightness = newBrightness;
  return TRUE;
}

BOOL PVideoInputDevice_V4L2::SetWhiteness(unsigned newWhiteness) 
{ 
  if (!SetControlCommon(V4L2_CID_WHITENESS, newWhiteness))
    return FALSE;

  frameWhiteness = newWhiteness;
  return TRUE;
}

BOOL PVideoInputDevice_V4L2::SetColour(unsigned newColour) 
{ 
  if (!SetControlCommon(V4L2_CID_SATURATION, newColour))
    return FALSE;
  frameColour = newColour;
  return TRUE;
}

BOOL PVideoInputDevice_V4L2::SetContrast(unsigned newContrast) 
{ 
  if (!SetControlCommon(V4L2_CID_CONTRAST, newContrast))
    return FALSE;
  frameContrast = newContrast;
  return TRUE;
}

BOOL PVideoInputDevice_V4L2::SetHue(unsigned newHue) 
{
  if (!SetControlCommon(V4L2_CID_HUE, newHue))
    return FALSE;
  frameHue=newHue;
  return TRUE;
}

BOOL PVideoInputDevice_V4L2::GetParameters (int *whiteness, int *brightness, int *colour, int *contrast, int *hue)
{
  if (!IsOpen())
    return FALSE;

  frameWhiteness = -1;
  frameBrightness = -1;
  frameColour = -1;
  frameContrast = -1;
  frameHue = -1;
  GetWhiteness();
  GetBrightness();
  GetColour();
  GetContrast();
  GetHue();

  *whiteness  = frameWhiteness;
  *brightness = frameBrightness;
  *colour     = frameColour;
  *contrast   = frameContrast;
  *hue        = frameHue;

  return TRUE;
}

BOOL PVideoInputDevice_V4L2::TestAllFormats()
{
  return TRUE;
}



// this is used to get more userfriendly names:

void
V4L2Names::Update()
{
  PTRACE(1,"Detecting V4L2 devices");
  PDirectory   procvideo2_4("/proc/video/dev");
  PDirectory   procvideo2_6("/sys/class/video4linux");
  PDirectory * procvideo;
  PString      entry;
  PStringList  devlist;
  PString      oldDevName;
  // Try and guess kernel version
  if (procvideo2_6.Exists()) {
    kernelVersion = K2_6;
    procvideo=&procvideo2_6;
  }
  else if (procvideo2_4.Exists()) {
    kernelVersion=K2_4;
    procvideo=&procvideo2_4;
  } 
  else {
    kernelVersion=KUNKNOWN;
    procvideo=0;
  }
  PWaitAndSignal m(mutex);
  inputDeviceNames.RemoveAll (); // flush the previous run
  if (procvideo) {
    PTRACE(2,"PV4L2Plugin\tdetected device metadata at "<<*procvideo);
    if ((kernelVersion==K2_6 && procvideo->Open(PFileInfo::SubDirectory) || 
        (procvideo->Open(PFileInfo::RegularFile)))) {
      do {
        entry = procvideo->GetEntryName();
        if ((entry.Left(5) == "video")) {
          PString thisDevice = "/dev/" + entry;
          int videoFd=::open((const char *)thisDevice, O_RDONLY | O_NONBLOCK);
          if ((videoFd > 0) || (errno == EBUSY)) {
            BOOL valid = FALSE;
            struct v4l2_capability videoCaps;
            memset(&videoCaps,0,sizeof(videoCaps));
            if ((errno == EBUSY) ||
                (::ioctl(videoFd, VIDIOC_QUERYCAP, &videoCaps) >= 0 &&
                (videoCaps.capabilities & V4L2_CAP_VIDEO_CAPTURE))) {
              PTRACE(1,"PV4L2Plugin\tdetected capture device " << videoCaps.card);
              valid = TRUE;
            }
            else {
              PTRACE(1,"PV4L2Plugin\t" << thisDevice << "is not deemed valid");
            }
            if (videoFd>0)
              ::close(videoFd);
            if(valid)
              inputDeviceNames += thisDevice;
          }
          else {
            PTRACE(1,"PV4L2Plugin\tcould not open " << thisDevice);
          }
        }
      } while (procvideo->Next());
    }
  }
  else {
    PTRACE(1,"Unable to detect v4l2 directory");
  }
  if (inputDeviceNames.GetSize() == 0) {
    POrdinalToString vid;
    ReadDeviceDirectory("/dev/", vid);

    for (PINDEX i = 0; i < vid.GetSize(); i++) {
      PINDEX cardnum = vid.GetKeyAt(i);
      int fd = ::open(vid[cardnum], O_RDONLY | O_NONBLOCK);
      if ((fd >= 0) || (errno == EBUSY)) {
        if (fd >= 0)
          ::close(fd);
        inputDeviceNames += vid[cardnum];
      }
    }
  }
  PopulateDictionary();
}

PString V4L2Names::BuildUserFriendly(PString devname)
{
  PString Result;

  int fd = ::open((const char *)devname, O_RDONLY);
  if(fd < 0) {
    return devname;
  }

  struct v4l2_capability videocap;
  memset(&videocap,0,sizeof(videocap));
  if (::ioctl(fd, VIDIOC_QUERYCAP, &videocap) < 0)  {
      ::close(fd);
      return devname;
    }
  
  ::close(fd);
  PString ufname((const char*)videocap.card);

  return ufname;
}

// End Of File ///////////////////////////////////////////////////////////////
