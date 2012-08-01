/*
 * pvfiledev.cxx
 *
 * Video file declaration
 *
 * Portable Windows Library
 *
 * Copyright (C) 2004 Post Increment
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
 * The Initial Developer of the Original Code is
 * Craig Southeren <craigs@postincrement.com>
 *
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: pvfiledev.cxx,v $
 * Revision 1.17  2007/09/20 10:01:32  shorne
 * Fixed bug with repeating YUVfile
 *
 * Revision 1.16  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.15  2007/04/20 06:47:14  csoutheren
 * Really disable video code when video is turned off
 *
 * Revision 1.14  2007/04/13 07:13:14  rjongbloed
 * Major update of video subsystem:
 *   Abstracted video frame info (width, height etc) into separate class.
 *   Changed devices, converter and video file to use above.
 *   Enhanced video file hint detection for frame rate and more
 *     flexible formats.
 *   Fixed issue if need to convert both colour format and size, had to do
 *     colour format first or it didn't convert size.
 *   Win32 video output device can be selected by "MSWIN" alone.
 *
 * Revision 1.13  2007/04/08 06:21:06  rjongbloed
 * Changed YUVFile video driver so if default device name (*.yuv) is used then will
 *   a) read from the first yuv file in the current directoy
 *   b) write to the file ./videoXXXX.yuv where XXX is uiique.
 *
 * Revision 1.12  2007/04/03 12:09:38  rjongbloed
 * Fixed various "file video device" issues:
 *   Remove filename from PVideoDevice::OpenArgs (use deviceName)
 *   Added driverName to PVideoDevice::OpenArgs (so can select YUVFile)
 *   Added new statics to create correct video input/output device object
 *     given a PVideoDevice::OpenArgs structure.
 *   Fixed begin able to write to YUVFile when YUV420P colour format
 *     is not actually selected.
 *   Fixed truncating output video file if overwriting.
 *
 * Revision 1.11  2007/04/02 05:29:54  rjongbloed
 * Tidied some trace logs to assure all have a category (bit before a tab character) set.
 *
 * Revision 1.10  2006/11/01 00:46:01  csoutheren
 * Fix problem in video output file device
 *
 * Revision 1.9  2006/10/31 04:10:40  csoutheren
 * Make sure PVidFileDev class is loaded, and make it work with OPAL
 *
 * Revision 1.8  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.7  2006/04/19 04:09:04  csoutheren
 * Allow frame size conversions
 *
 * Revision 1.6  2006/03/17 06:55:33  csoutheren
 * Removed unused member variable
 *
 * Revision 1.5  2006/03/06 06:04:13  csoutheren
 * Added YUVFile video output device
 *
 * Revision 1.4  2006/02/24 04:51:26  csoutheren
 * Fixed problem with using CIF from video files
 * Added support for video files in y4m format
 *
 * Revision 1.3  2006/02/20 09:31:58  csoutheren
 * Fixed link problem on Linux
 *
 * Revision 1.2  2006/02/20 06:49:45  csoutheren
 * Added video file and video file input device code
 *
 * Revision 1.1  2006/02/20 06:17:28  csoutheren
 * Added ability to read video from a file
 *
 */

#ifdef __GNUC__
#pragma implementation "pvfiledev.h"
#endif

#define P_FORCE_STATIC_PLUGIN

#include <ptlib.h>

#if P_VIDEO
#if P_VIDFILE

#include <ptlib/vconvert.h>
#include <ptclib/pvfiledev.h>
#include <ptlib/pfactory.h>
#include <ptlib/pluginmgr.h>
#include <ptlib/videoio.h>


static const char DefaultYUVFileName[] = "*.yuv";


#define new PNEW


///////////////////////////////////////////////////////////////////////////////
// PVideoInputDevice_YUVFile

class PVideoInputDevice_YUVFile_PluginServiceDescriptor : public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject * CreateInstance(int /*userData*/) const
    {
        return new PVideoInputDevice_YUVFile;
    }
    virtual PStringList GetDeviceNames(int /*userData*/) const
    {
        return PVideoInputDevice_YUVFile::GetInputDeviceNames();
    }
    virtual bool ValidateDeviceName(const PString & deviceName, int /*userData*/) const
    {
        return (deviceName.Right(4) *= ".yuv") && PFile::Access(deviceName, PFile::ReadOnly);
    }
} PVideoInputDevice_YUVFile_descriptor;

PCREATE_PLUGIN(YUVFile, PVideoInputDevice, &PVideoInputDevice_YUVFile_descriptor);



PVideoInputDevice_YUVFile::PVideoInputDevice_YUVFile()
{
  file = NULL;

  SetColourFormat("YUV420P");
  channelNumber = 0; 
  grabCount = 0;
  SetFrameRate(10);
}


PVideoInputDevice_YUVFile::~PVideoInputDevice_YUVFile()
{
  Close();
}


BOOL PVideoInputDevice_YUVFile::Open(const PString & _deviceName, BOOL /*startImmediate*/)
{
  Close();

  PFilePath fileName;
  if (_deviceName != DefaultYUVFileName)
    fileName = _deviceName;
  else {
    PDirectory dir;
    if (dir.Open(PFileInfo::RegularFile|PFileInfo::SymbolicLink)) {
      do {
        if (dir.GetEntryName().Right(4) == (DefaultYUVFileName+1)) {
          fileName = dir.GetEntryName();
          break;
        }
      } while (dir.Next());
    }
    if (fileName.IsEmpty()) {
      PTRACE(1, "YUVFile\tCannot find any file using " << dir << DefaultYUVFileName << " as video input device");
      return FALSE;
    }
  }

  file = PFactory<PVideoFile>::CreateInstance("yuv");
  if (file == NULL || !file->Open(fileName, PFile::ReadOnly, PFile::MustExist)) {
    PTRACE(1, "YUVFile\tCannot open file " << fileName << " as video input device");
    return FALSE;
  }

  if (!file->IsUnknownFrameSize()) {
    unsigned width, height;
    file->GetFrameSize(width, height);
    SetFrameSize(width, height);
  }

  deviceName = file->GetFilePath();
  return TRUE;    
}


BOOL PVideoInputDevice_YUVFile::IsOpen() 
{
  return file != NULL && file->IsOpen();
}


BOOL PVideoInputDevice_YUVFile::Close()
{
  BOOL ok = file != NULL && file->Close();

  delete file;
  file = NULL;

  return ok;
}


BOOL PVideoInputDevice_YUVFile::Start()
{
  return TRUE;
}


BOOL PVideoInputDevice_YUVFile::Stop()
{
  return TRUE;
}


BOOL PVideoInputDevice_YUVFile::IsCapturing()
{
  return IsOpen();
}


PStringList PVideoInputDevice_YUVFile::GetInputDeviceNames()
{
  PStringList list;
  list.AppendString(DefaultYUVFileName);
  return list;
}


BOOL PVideoInputDevice_YUVFile::SetVideoFormat(VideoFormat newFormat)
{
  return PVideoDevice::SetVideoFormat(newFormat);
}


int PVideoInputDevice_YUVFile::GetNumChannels() 
{
  return ChannelCount;  
}


BOOL PVideoInputDevice_YUVFile::SetChannel(int newChannel)
{
  return PVideoDevice::SetChannel(newChannel);
}

BOOL PVideoInputDevice_YUVFile::SetColourFormat(const PString & newFormat)
{
  if (!(newFormat *= "YUV420P"))
    return FALSE;

  return PVideoDevice::SetColourFormat(newFormat);
}


BOOL PVideoInputDevice_YUVFile::SetFrameRate(unsigned rate)
{
  // if the file does not know what frame rate it is, then set it
  if (file == NULL || (file->IsUnknownFrameSize() && !file->SetFrameRate(rate)))
    return FALSE;

  return PVideoDevice::SetFrameRate(file->GetFrameRate());
}


BOOL PVideoInputDevice_YUVFile::GetFrameSizeLimits(unsigned & minWidth,
                                           unsigned & minHeight,
                                           unsigned & maxWidth,
                                           unsigned & maxHeight) 
{
  unsigned width, height;
  if (file == NULL || !file->GetFrameSize(width, height))
    return FALSE;
  minWidth  = maxWidth  = width;
  minHeight = maxHeight = height;
  return TRUE;
}

BOOL PVideoInputDevice_YUVFile::SetFrameSize(unsigned width, unsigned height)
{
  // if the file does not know what size it is, then set it
  if (file == NULL || (file->IsUnknownFrameSize() && !file->SetFrameSize(width, height)))
    return FALSE;

  file->GetFrameSize(frameWidth, frameHeight);

  videoFrameSize = CalculateFrameBytes(frameWidth, frameHeight, colourFormat);
  return videoFrameSize > 0 && width == frameWidth && height == frameHeight;
}


PINDEX PVideoInputDevice_YUVFile::GetMaxFrameBytes()
{
  return GetMaxFrameBytesConverted(videoFrameSize);
}


BOOL PVideoInputDevice_YUVFile::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{    
  pacing.Delay(1000/GetFrameRate());
  return GetFrameDataNoDelay(buffer, bytesReturned);
}

 
BOOL PVideoInputDevice_YUVFile::GetFrameDataNoDelay(BYTE *destFrame, PINDEX * bytesReturned)
{
  if (file == NULL)
    return FALSE;

  grabCount++;

  BYTE * readBuffer = destFrame;

  if (converter != NULL)
    readBuffer = frameStore.GetPointer(videoFrameSize);

  if (file->IsOpen()) {
    if (!file->ReadFrame(readBuffer))
      file->Close();
  }

  if (!file->IsOpen()) {
    switch (channelNumber) {
      case Channel_PlayAndClose:
      default:
        return FALSE;

      case Channel_PlayAndRepeat:
	    file->Open(deviceName, PFile::ReadOnly, PFile::MustExist);
        if (!file->SetPosition(0) || !file->ReadFrame(readBuffer))
          return FALSE;
        break;

      case Channel_PlayAndKeepLast:
        break;

      case Channel_PlayAndShowBlack:
        FillRect(readBuffer, 0, 0, frameWidth, frameHeight, 0, 0, 0);
        break;
    }
  }

  if (converter == NULL) {
    if (bytesReturned != NULL)
      *bytesReturned = videoFrameSize;
  } else {
    converter->SetSrcFrameSize(frameWidth, frameHeight);
    if (!converter->Convert(readBuffer, destFrame, bytesReturned))
      return FALSE;
    if (bytesReturned != NULL)
      *bytesReturned = converter->GetMaxDstFrameBytes();
  }

  return TRUE;
}


void PVideoInputDevice_YUVFile::GrabBlankImage(BYTE *resFrame)
{
  // Change colour every second, cycle is:
  // black, red, green, yellow, blue, magenta, cyan, white
  int mask = grabCount/frameRate;
  FillRect(resFrame,
           0, 0, frameWidth, frameHeight, //Fill the whole frame with the colour.
           (mask&1) ? 255 : 0, // red
           (mask&2) ? 255 : 0, // green
           (mask&4) ? 255 : 0);//blue
}

void PVideoInputDevice_YUVFile::FillRect(BYTE * frame,
                   int xPos, int initialYPos,
                   int rectWidth, int rectHeight,
                   int r, int g,  int b)
{
// PTRACE(0,"x,y is"<<xPos<<" "<<yPos<<" and size is "<<rectWidth<<" "<<rectHeight);

  //This routine fills a region of the video image with data. It is used as the central
  //point because one only has to add other image formats here.

  int yPos = initialYPos;

  int offset       = ( yPos * frameWidth ) + xPos;
  int colourOffset = ( (yPos * frameWidth) >> 2) + (xPos >> 1);

  int Y  =  ( 257 * r + 504 * g +  98 * b)/1000 + 16;
  int Cb =  (-148 * r - 291 * g + 439 * b)/1000 + 128;
  int Cr =  ( 439 * r - 368 * g -  71 * b)/1000 + 128;

  unsigned char * Yptr  = frame + offset;
  unsigned char * CbPtr = frame + (frameWidth * frameHeight) + colourOffset;
  unsigned char * CrPtr = frame + (frameWidth * frameHeight) + (frameWidth * frameHeight/4)  + colourOffset;

  int rr ;
  int halfRectWidth = rectWidth >> 1;
  int halfWidth     = frameWidth >> 1;
  
  for (rr = 0; rr < rectHeight;rr+=2) {
    memset(Yptr, Y, rectWidth);
    Yptr += frameWidth;
    memset(Yptr, Y, rectWidth);
    Yptr += frameWidth;

    memset(CbPtr, Cb, halfRectWidth);
    memset(CrPtr, Cr, halfRectWidth);

    CbPtr += halfWidth;
    CrPtr += halfWidth;
  }
}

///////////////////////////////////////////////////////////////////////////////
// PVideoOutputDevice_YUVFile

class PVideoOutputDevice_YUVFile_PluginServiceDescriptor : public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject * CreateInstance(int /*userData*/) const
    {
        return new PVideoOutputDevice_YUVFile;
    }
    virtual PStringList GetDeviceNames(int /*userData*/) const
    {
        return PVideoOutputDevice_YUVFile::GetOutputDeviceNames();
    }
    virtual bool ValidateDeviceName(const PString & deviceName, int /*userData*/) const
    {
        return (deviceName.Right(4) *= ".yuv") && PFile::Access(deviceName, PFile::WriteOnly);
    }
} PVideoOutputDevice_YUVFile_descriptor;

PCREATE_PLUGIN(YUVFile, PVideoOutputDevice, &PVideoOutputDevice_YUVFile_descriptor);


PVideoOutputDevice_YUVFile::PVideoOutputDevice_YUVFile()
{
  file = NULL;
}


PVideoOutputDevice_YUVFile::~PVideoOutputDevice_YUVFile()
{
  Close();
}


BOOL PVideoOutputDevice_YUVFile::Open(const PString & _deviceName, BOOL /*startImmediate*/)
{
  PFilePath fileName;
  if (_deviceName != DefaultYUVFileName)
    fileName = _deviceName;
  else {
    unsigned unique = 0;
    do {
      fileName.Empty();
      fileName.sprintf("video%03u.yuv", ++unique);
    } while (PFile::Exists(fileName));
  }

  file = PFactory<PVideoFile>::CreateInstance("yuv");
  if (file == NULL || !file->Open(fileName, PFile::WriteOnly, PFile::Create|PFile::Truncate)) {
    PTRACE(1, "YUVFile\tCannot create file " << fileName << " as video output device");
    return FALSE;
  }

  deviceName = file->GetFilePath();
  return TRUE;
}

BOOL PVideoOutputDevice_YUVFile::Close()
{
  BOOL ok = file == NULL || file->Close();

  delete file;
  file = NULL;

  return ok;
}

BOOL PVideoOutputDevice_YUVFile::Start()
{
  return file != NULL && file->SetFrameSize(frameHeight, frameWidth);
}

BOOL PVideoOutputDevice_YUVFile::Stop()
{
  return TRUE;
}

BOOL PVideoOutputDevice_YUVFile::IsOpen()
{
  return file != NULL && file->IsOpen();
}


PStringList PVideoOutputDevice_YUVFile::GetOutputDeviceNames()
{
  PStringList list;
  list.AppendString(DefaultYUVFileName);
  return list;
}


BOOL PVideoOutputDevice_YUVFile::SetColourFormat(const PString & newFormat)
{
  if (!(newFormat *= "YUV420P"))
    return FALSE;

  return PVideoDevice::SetColourFormat(newFormat);
}


PINDEX PVideoOutputDevice_YUVFile::GetMaxFrameBytes()
{
  return GetMaxFrameBytesConverted(CalculateFrameBytes(frameWidth, frameHeight, colourFormat));
}


BOOL PVideoOutputDevice_YUVFile::SetFrameData(unsigned x, unsigned y,
                                              unsigned width, unsigned height,
                                              const BYTE * data,
                                              BOOL /*endFrame*/)
{
  if (x != 0 || y != 0 || width != frameWidth || height != frameHeight) {
    PTRACE(1, "YUVFile\tOutput device only supports full frame writes");
    return FALSE;
  }

  if (file == NULL || (file->IsUnknownFrameSize() && !file->SetFrameSize(width, height)))
    return FALSE;

  if (converter == NULL)
    return file->WriteFrame(data);

  converter->Convert(data, frameStore.GetPointer(GetMaxFrameBytes()));
  return file->WriteFrame(frameStore);
}


#endif // P_VIDFILE
#endif

