/*
 * video4bsd.cxx
 *
 * Classes to support streaming video input (grabbing) and output.
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
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): Roger Hardiman <roger@freebsd.org>
 *
 * $Log: vidinput_bsd.cxx,v $
 * Revision 1.4  2007/04/27 17:34:44  dsandras
 * Applied patch from Luc Saillard to fix things after the latest change
 * which broke all drivers. Thanks Luc <luc saillard org>.
 *
 * Revision 1.3  2006/06/20 06:08:06  csoutheren
 * Applied patch 1471691
 * Fix vidinput_bsd plugin
 * Thanks to Joerg Pulz
 *
 * Revision 1.2  2005/08/09 09:08:09  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.1.8.2  2005/07/24 09:01:47  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.1.8.1  2005/07/17 09:27:04  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.1  2003/12/12 04:38:17  rogerhardiman
 * Add plugin for the BSD Video Capture API (also called the meteor API)
 * for FreeBSD, NetBSD and OpenBSD
 *
 * Revision 1.20  2002/10/28 19:12:45  rogerh
 * Add svideo input support for Lars Eggert <larse@isi.edu>
 *
 * Revision 1.19  2002/04/10 08:40:36  rogerh
 * Simplify the SetVideoChannelFormat() code. Use the implementation in the
 * ancestor class.
 *
 * Revision 1.18  2002/04/05 06:41:54  rogerh
 * Apply video changes from Damien Sandras <dsandras@seconix.com>.
 * The Video Channel and Format are no longer set in Open(). Instead
 * call the new SetVideoChannelFormat() method. This makes video capture
 * and GnomeMeeting more stable with certain Linux video capture devices.
 *
 * Revision 1.17  2002/01/08 17:16:13  rogerh
 * Add code to grab Even fields (instead of interlaced frames) whenever
 * possible. This improves the image quality.
 * Add TestAllFormats
 *
 * Revision 1.16  2001/12/05 14:45:20  rogerh
 * Implement GetFrameData and GetFrameDataNoDelay
 *
 * Revision 1.15  2001/12/05 14:32:48  rogerh
 * Add GetParemters function
 *
 * Revision 1.14  2001/08/06 06:56:16  rogerh
 * Add scaling for new methods to match BSD's Meteor API
 *
 * Revision 1.13  2001/08/06 06:19:33  rogerh
 * Implement Brightness, Contract and Hue methods.
 *
 * Revision 1.12  2001/03/26 16:02:01  rogerh
 * Add dummy function for VerifyHardwareFrameSize
 *
 * Revision 1.11  2001/03/08 03:59:13  robertj
 * Fixed previous change, needed to allow for -1 as chammelNumber in Open().
 *
 * Revision 1.10  2001/03/08 02:23:17  robertj
 * Added improved defaulting of video formats so Open() does not fail.
 *
 * Revision 1.9  2001/03/07 00:10:05  robertj
 * Improved the device list, uses /proc, thanks Thorsten Westheider.
 *
 * Revision 1.8  2001/03/03 23:29:00  robertj
 * Oops, fixed BSD version of video.
 *
 * Revision 1.7  2001/03/03 23:25:07  robertj
 * Fixed use of video conversion function, returning bytes in destination frame.
 *
 * Revision 1.6  2001/03/03 06:13:01  robertj
 * Major upgrade of video conversion and grabbing classes.
 *
 * Revision 1.5  2001/01/11 13:26:39  rogerh
 * Add me in the Contributors section
 *
 * Revision 1.4  2001/01/05 18:12:30  rogerh
 * First fully working version of video4bsd.
 * Note that Start() and Stop() are not called, hence the first time hacks
 * in GetFrameData(). Also video is always grabbed in interlaced mode
 * so it does not look as good as it could.
 *
 * Revision 1.3  2001/01/05 14:52:36  rogerh
 * More work on the FreeBSD video capture code
 *
 * Revision 1.2  2001/01/04 18:02:16  rogerh
 * remove some old parts refering to linux
 *
 * Revision 1.1  2001/01/04 18:00:43  rogerh
 * Start to add support for video capture using on FreeBSD/NetBSD and OpenBSD
 * using the Meteor API (used by the Matrox Meteor and the bktr driver for
 * Bt848/Bt878 TV Tuner Cards). This is incomplete but it does compile.
 */

#pragma implementation "vidinput_bsd.h"

#include "vidinput_bsd.h"

PCREATE_VIDINPUT_PLUGIN(BSDCAPTURE);

///////////////////////////////////////////////////////////////////////////////
// PVideoInputBSDCAPTURE

PVideoInputDevice_BSDCAPTURE::PVideoInputDevice_BSDCAPTURE()
{
  videoFd     = -1;
  canMap      = -1;
}

PVideoInputDevice_BSDCAPTURE::~PVideoInputDevice_BSDCAPTURE()
{
  Close();
}

BOOL PVideoInputDevice_BSDCAPTURE::Open(const PString & devName, BOOL startImmediate)
{
  if (IsOpen())
  Close();

  deviceName = devName;
  videoFd = ::open((const char *)devName, O_RDONLY);
  if (videoFd < 0) {
    videoFd = -1;
    return FALSE;
  }
 
  // fill in a device capabilities structure
  videoCapability.minheight = 32;
  videoCapability.minwidth  = 32;
  videoCapability.maxheight = 768;
  videoCapability.maxwidth  = 576;
  videoCapability.channels  = 5;

  // set height and width
  frameHeight = videoCapability.maxheight;
  frameWidth  = videoCapability.maxwidth;
  
  // select the specified input
  if (!SetChannel(channelNumber)) {
    ::close (videoFd);
    videoFd = -1;
    return FALSE;
  } 
  
  // select the video format (eg PAL, NTSC)
  if (!SetVideoFormat(videoFormat)) {
    ::close (videoFd);
    videoFd = -1;
    return FALSE;
  }
 
  // select the colpur format (eg YUV420, or RGB)
  if (!SetColourFormat(colourFormat)) {
    ::close (videoFd);
    videoFd = -1;
    return FALSE;
  }

  // select the image size
  if (!SetFrameSize(frameWidth, frameHeight)) {
    ::close (videoFd);
    videoFd = -1;
    return FALSE;
  }

  return TRUE;    
}


BOOL PVideoInputDevice_BSDCAPTURE::IsOpen() 
{
    return videoFd >= 0;
}


BOOL PVideoInputDevice_BSDCAPTURE::Close()
{
  if (!IsOpen())
    return FALSE;

  ClearMapping();
  ::close(videoFd);
  videoFd = -1;
  canMap  = -1;
  
  return TRUE;
}

BOOL PVideoInputDevice_BSDCAPTURE::Start()
{
  return TRUE;
}


BOOL PVideoInputDevice_BSDCAPTURE::Stop()
{
  return TRUE;
}


BOOL PVideoInputDevice_BSDCAPTURE::IsCapturing()
{
  return IsOpen();
}


PStringList PVideoInputDevice_BSDCAPTURE::GetInputDeviceNames()
{
  PStringList list;

  if (PFile::Exists("/dev/bktr0"))
    list.AppendString("/dev/bktr0");
  if (PFile::Exists("/dev/bktr1"))
    list.AppendString("/dev/bktr1");
  if (PFile::Exists("/dev/meteor0"))
    list.AppendString("/dev/meteor0");
  if (PFile::Exists("/dev/meteor1"))
    list.AppendString("/dev/meteor1");

  return list;
}


BOOL PVideoInputDevice_BSDCAPTURE::SetVideoFormat(VideoFormat newFormat)
{
  if (!PVideoDevice::SetVideoFormat(newFormat))
    return FALSE;

  // set channel information
  static int fmt[4] = { METEOR_FMT_PAL, METEOR_FMT_NTSC,
                        METEOR_FMT_SECAM, METEOR_FMT_AUTOMODE };
  int format = fmt[newFormat];

  // set the information
  if (::ioctl(videoFd, METEORSFMT, &format) >= 0)
    return TRUE;

  // setting the format failed. Fall back trying other standard formats

  if (newFormat != Auto)
    return FALSE;

  if (SetVideoFormat(PAL))
    return TRUE;
  if (SetVideoFormat(NTSC))
    return TRUE;
  if (SetVideoFormat(SECAM))
    return TRUE;

  return FALSE;  
}


int PVideoInputDevice_BSDCAPTURE::GetNumChannels() 
{
  return videoCapability.channels;
}


BOOL PVideoInputDevice_BSDCAPTURE::SetChannel(int newChannel)
{
  if (!PVideoDevice::SetChannel(newChannel))
    return FALSE;

  // set channel information
  static int chnl[5] = { METEOR_INPUT_DEV0, METEOR_INPUT_DEV1,
                         METEOR_INPUT_DEV2, METEOR_INPUT_DEV3,
                         METEOR_INPUT_DEV_SVIDEO };
  int channel = chnl[newChannel];

  // set the information
  if (::ioctl(videoFd, METEORSINPUT, &channel) < 0)
    return FALSE;

  return TRUE;
}


BOOL PVideoInputDevice_BSDCAPTURE::SetColourFormat(const PString & newFormat)
{
  if (!PVideoDevice::SetColourFormat(newFormat))
    return FALSE;

  ClearMapping();

  frameBytes = CalculateFrameBytes(frameWidth, frameHeight, colourFormat);

  return TRUE;

}


BOOL PVideoInputDevice_BSDCAPTURE::SetFrameRate(unsigned rate)
{
  if (!PVideoDevice::SetFrameRate(rate))
    return FALSE;

  return TRUE;
}


BOOL PVideoInputDevice_BSDCAPTURE::GetFrameSizeLimits(unsigned & minWidth,
                                           unsigned & minHeight,
                                           unsigned & maxWidth,
                                           unsigned & maxHeight) 
{
  if (!IsOpen())
    return FALSE;

  minWidth  = videoCapability.minwidth;
  minHeight = videoCapability.minheight;
  maxWidth  = videoCapability.maxwidth;
  maxHeight = videoCapability.maxheight;
  return TRUE;

}


BOOL PVideoInputDevice_BSDCAPTURE::SetFrameSize(unsigned width, unsigned height)
{
  if (!PVideoDevice::SetFrameSize(width, height))
    return FALSE;
  
  ClearMapping();

  frameBytes = CalculateFrameBytes(frameWidth, frameHeight, colourFormat);
  
  return TRUE;
}


PINDEX PVideoInputDevice_BSDCAPTURE::GetMaxFrameBytes()
{
  return GetMaxFrameBytesConverted(frameBytes);
}


BOOL PVideoInputDevice_BSDCAPTURE::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{
  m_pacing.Delay(1000/GetFrameRate());
  return GetFrameDataNoDelay(buffer,bytesReturned);
}


BOOL PVideoInputDevice_BSDCAPTURE::GetFrameDataNoDelay(BYTE * buffer, PINDEX * bytesReturned)
{

  // Hack time. It seems that the Start() and Stop() functions are not
  // actually called, so we will have to initialise the frame grabber
  // here on the first pass through this GetFrameData() function

  if (canMap < 0) {

    struct meteor_geomet geo;
    geo.rows = frameHeight;
    geo.columns = frameWidth;
    geo.frames = 1;
    geo.oformat = METEOR_GEO_YUV_422 | METEOR_GEO_YUV_12;

    // Grab even field (instead of interlaced frames) where possible to stop
    // jagged interlacing artifacts. NTSC is 640x480, PAL/SECAM is 768x576.
    if (  ((PVideoDevice::GetVideoFormat() == PAL) && (frameHeight <= 288))
       || ((PVideoDevice::GetVideoFormat() == SECAM) && (frameHeight <= 288))
       || ((PVideoDevice::GetVideoFormat() == NTSC) && (frameHeight <= 240)) ){
        geo.oformat |=  METEOR_GEO_EVEN_ONLY;
    }

    // set the new geometry
    if (ioctl(videoFd, METEORSETGEO, &geo) < 0) {
      return FALSE;
    }

    mmap_size = frameBytes;
    videoBuffer = (BYTE *)::mmap(0, mmap_size, PROT_READ, 0, videoFd, 0);
    if (videoBuffer < 0) {
      return FALSE;
    } else {
      canMap = 1;
    }
 
    // put the grabber into continuous capture mode
    int mode =  METEOR_CAP_CONTINOUS;
    if (ioctl(videoFd, METEORCAPTUR, &mode) < 0 ) {
      return FALSE;
    }
  }


  // Copy a snapshot of the image from the mmap buffer
  // Really there should be some synchronisation here to avoid tearing
  // in the image, but we will worry about that later

  if (converter != NULL)
    return converter->Convert(videoBuffer, buffer, bytesReturned);

  memcpy(buffer, videoBuffer, frameBytes);

  if (bytesReturned != NULL)
    *bytesReturned = frameBytes;

  
  return TRUE;
}


void PVideoInputDevice_BSDCAPTURE::ClearMapping()
{
  if (canMap == 1) {

    // better stop grabbing first
    // Really this should be in the Stop() function, but that is
    // not actually called anywhere.

    int mode =  METEOR_CAP_STOP_CONT;
    ioctl(videoFd, METEORCAPTUR, &mode);

    if (videoBuffer != NULL)
      ::munmap(videoBuffer, mmap_size);

    canMap = -1;
    videoBuffer = NULL;
  }
}

BOOL PVideoInputDevice_BSDCAPTURE::VerifyHardwareFrameSize(unsigned width,
                                                unsigned height)
{
	// Assume the size is valid
	return TRUE;
}


int PVideoInputDevice_BSDCAPTURE::GetBrightness()
{
  if (!IsOpen())
    return -1;

  unsigned char data;
  if (::ioctl(videoFd, METEORGBRIG, &data) < 0)
    return -1;
  frameBrightness = (data << 8);

  return frameBrightness;
}

int PVideoInputDevice_BSDCAPTURE::GetContrast()
{
  if (!IsOpen())
    return -1;

  unsigned char data;
  if (::ioctl(videoFd, METEORGCONT, &data) < 0)
    return -1;
  frameContrast = (data << 8);

 return frameContrast;
}

int PVideoInputDevice_BSDCAPTURE::GetHue()
{
  if (!IsOpen())
    return -1;

  char data;
  if (::ioctl(videoFd, METEORGHUE, &data) < 0)
    return -1;
  frameHue = ((data + 128) << 8);

  return frameHue;
}

BOOL PVideoInputDevice_BSDCAPTURE::SetBrightness(unsigned newBrightness)
{
  if (!IsOpen())
    return FALSE;

  unsigned char data = (newBrightness >> 8); // rescale for the ioctl
  if (::ioctl(videoFd, METEORSBRIG, &data) < 0)
    return FALSE;

  frameBrightness=newBrightness;
  return TRUE;
}

BOOL PVideoInputDevice_BSDCAPTURE::SetContrast(unsigned newContrast)
{
  if (!IsOpen())
    return FALSE;

  unsigned char data = (newContrast >> 8); // rescale for the ioctl
  if (::ioctl(videoFd, METEORSCONT, &data) < 0)
    return FALSE;

  frameContrast = newContrast;
  return TRUE;
}

BOOL PVideoInputDevice_BSDCAPTURE::SetHue(unsigned newHue)
{
  if (!IsOpen())
    return FALSE;

  char data = (newHue >> 8) - 128; // ioctl takes a signed char
  if (::ioctl(videoFd, METEORSHUE, &data) < 0)
    return FALSE;

  frameHue=newHue;
  return TRUE;
}

BOOL PVideoInputDevice_BSDCAPTURE::GetParameters (int *whiteness, int *brightness,
                                      int *colour, int *contrast, int *hue)
{
  if (!IsOpen())
    return FALSE;

  unsigned char data;
  char signed_data;

  if (::ioctl(videoFd, METEORGBRIG, &data) < 0)
    return -1;
  *brightness = (data << 8);

  if (::ioctl(videoFd, METEORGCONT, &data) < 0)
    return -1;
  *contrast = (data << 8);

  if (::ioctl(videoFd, METEORGHUE, &signed_data) < 0)
    return -1;
  *hue = ((data + 128) << 8);

  // The bktr driver does not have colour or whiteness ioctls
  // so set them to the current global values
  *colour     = frameColour;
  *whiteness  = frameWhiteness;

  // update the global settings
  frameBrightness = *brightness;
  frameContrast   = *contrast;
  frameHue        = *hue;

  return TRUE;
}

BOOL PVideoInputDevice_BSDCAPTURE::TestAllFormats()
{
  return TRUE;
}
// End Of File ///////////////////////////////////////////////////////////////
