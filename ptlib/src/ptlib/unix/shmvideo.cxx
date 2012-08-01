/*
 * shmvideo.cxx
 *
 * This file contains the class hierarchy for both shared memory video
 * input and output devices.
 *
 * Copyright (c) 2003 Pai-Hsiang Hsiao
 * Copyright (c) 1998-2003 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * $Log: shmvideo.cxx,v $
 * Revision 1.4  2007/05/16 07:54:21  csoutheren
 * Fix problems created by gcc 4.2.0
 *
 * Revision 1.3  2007/04/20 06:34:48  csoutheren
 * Fix compilation on MacOSX
 *
 * Revision 1.2  2007/04/14 07:08:55  rjongbloed
 * Major update of video subsystem:
 *   Abstracted video frame info (width, height etc) into separate class.
 *   Changed devices, converter and video file to use above.
 *   Enhanced video file hint detection for frame rate and more
 *     flexible formats.
 *   Fixed issue if need to convert both colour format and size, had to do
 *     colour format first or it didn't convert size.
 *   Win32 video output device can be selected by "MSWIN" alone.
 *
 * Revision 1.1  2006/07/18 05:17:24  csoutheren
 * Added shared memory video devices
 * Thanks to Hannes Friederich
 *
 * Revision 1.2  2003/06/12 21:34:13  shawn
 * makes code consistent with documentation
 *
 * Revision 1.1  2003/06/12 19:39:11  shawn
 * Added shared memory video input/output devices.  Video frames of these two
 * devices are stored in a named shared memory region and can be accessed by
 * other applications.
 *
 *
 */

#define P_FORCE_STATIC_PLUGIN

#include <ptlib.h>
#include <ptlib/videoio.h>
#include <ptlib/vconvert.h>
#include <ptlib/unix/ptlib/shmvideo.h>

#ifdef __MACOSX__
namespace PWLibStupidOSXHacks {
	int loadShmVideoStuff;
};

#endif

class PColourConverter;

static const char *
ShmKeyFileName()
{
  return "/dev/null";
}

BOOL
PVideoOutputDevice_Shm::shmInit()
{
  semLock = sem_open(SEM_NAME_OF_OUTPUT_DEVICE,
		     O_RDWR, S_IRUSR|S_IWUSR, 0);

  if (semLock != (sem_t *)SEM_FAILED) {
    shmKey = ftok(ShmKeyFileName(), 0);
    if (shmKey >= 0) {
      shmId = shmget(shmKey, SHMVIDEO_BUFSIZE, 0666);
      if (shmId >= 0) {
        shmPtr = shmat(shmId, NULL, 0);
        if (shmPtr) {
          return TRUE;
        }
        else {
          PTRACE(1, "SHMV\t shmInit can not attach shared memory" << endl);
          shmctl(shmId, IPC_RMID, NULL);
          sem_close(semLock);
        }
      }
      else {
        PTRACE(1, "SHMV\t shmInit can not find the shared memory" << endl);
        sem_close(semLock);
      }
    }
    else {
      PTRACE(1, "SHMV\t shmInit can not create key for shared memory" << endl);
      sem_close(semLock);
    }
  }
  else {
    PTRACE(1, "SHMV\t shmInit can not create semaphore" << endl);
  }

  semLock = (sem_t *)SEM_FAILED;
  shmKey = -1;
  shmId = -1;
  shmPtr = NULL;

  return FALSE;
}

PVideoOutputDevice_Shm::PVideoOutputDevice_Shm()
{
	colourFormat = "RGB24";
	bytesPerPixel = 3;
	frameStore.SetSize(frameWidth * frameHeight * bytesPerPixel);
	
  semLock = (sem_t *)SEM_FAILED;
  shmKey = -1;
  shmId = -1;
  shmPtr = NULL;

  PTRACE(6, "SHMV\t Constructor of PVideoOutputDevice_Shm");
}

BOOL PVideoOutputDevice_Shm::SetColourFormat(const PString & colourFormat)
{
	if( colourFormat == "RGB32")
		bytesPerPixel = 4;
	else if(colourFormat == "RGB24")
		bytesPerPixel = 3;
	else
		return false;
	
	return PVideoOutputDevice::SetColourFormat(colourFormat) && SetFrameSize(frameWidth, frameHeight);
}

BOOL PVideoOutputDevice_Shm::SetFrameSize(unsigned width, unsigned height)
{
	if (!PVideoOutputDevice::SetFrameSize(width, height))
		return FALSE;
	
	return frameStore.SetSize(frameWidth*frameHeight*bytesPerPixel);
}

PINDEX PVideoOutputDevice_Shm::GetMaxFrameBytes()
{
	return frameStore.GetSize();
}

BOOL PVideoOutputDevice_Shm::SetFrameData(unsigned x, unsigned y,
                                         unsigned width, unsigned height,
                                         const BYTE * data,
                                         BOOL endFrame)
{
	if (x+width > frameWidth || y+height > frameHeight)
		return FALSE;
	
	if (x == 0 && width == frameWidth && y == 0 && height == frameHeight) {
		if (converter != NULL)
			converter->Convert(data, frameStore.GetPointer());
		else
			memcpy(frameStore.GetPointer(), data, height*width*bytesPerPixel);
	}
	else {
		if (converter != NULL) {
			PAssertAlways("Converted output of partial RGB frame not supported");
			return FALSE;
		}
		
		if (x == 0 && width == frameWidth)
			memcpy(frameStore.GetPointer() + y*width*bytesPerPixel, data, height*width*bytesPerPixel);
		else {
			for (unsigned dy = 0; dy < height; dy++)
				memcpy(frameStore.GetPointer() + ((y+dy)*width + x)*bytesPerPixel,
					   data + dy*width*bytesPerPixel, width*bytesPerPixel);
		}
	}
	
	if (endFrame)
		return EndFrame();
	
	return TRUE;
}

BOOL
PVideoOutputDevice_Shm::Open(const PString & name,
			   BOOL /*startImmediate*/)
{
  PTRACE(1, "SHMV\t Open of PVideoOutputDevice_Shm");

  Close();

  if (shmInit() == TRUE) {
    deviceName = name;
    return TRUE;
  }
  else {
    return FALSE;
  }
}

BOOL
PVideoOutputDevice_Shm::IsOpen()
{
  if (semLock != (sem_t *)SEM_FAILED) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

BOOL
PVideoOutputDevice_Shm::Close()
{
  if (semLock != (sem_t *)SEM_FAILED) {
    shmdt(shmPtr);
    sem_close(semLock);
    shmPtr = NULL;
  }
  return TRUE;
}

PStringList
PVideoOutputDevice_Shm::GetDeviceNames() const
{
  PStringList list;
  list += "shm";
  return list;
}

BOOL
PVideoOutputDevice_Shm::EndFrame()
{
  long *ptr = (long *)shmPtr;

  if (semLock == (sem_t *)SEM_FAILED) {
    return FALSE;
  }

  if (bytesPerPixel != 3 && bytesPerPixel != 4) {
    PTRACE(1, "SHMV\t EndFrame() does not handle bytesPerPixel!={3,4}"<<endl);
    return FALSE;
  }

  if (frameWidth*frameHeight*bytesPerPixel > SHMVIDEO_FRAMESIZE) {
    return FALSE;
  }

  // write header info so the consumer knows what to expect
  ptr[0] = frameWidth;
  ptr[1] = frameHeight;
  ptr[2] = bytesPerPixel;

  PTRACE(1, "writing " << frameStore.GetSize() << " bytes" << endl);
  if (memcpy((char *)shmPtr+sizeof(long)*3,
             frameStore, frameStore.GetSize()) == NULL) {
    return FALSE;
  }

  sem_post(semLock);

  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

PCREATE_VIDINPUT_PLUGIN(Shm);

BOOL
PVideoInputDevice_Shm::shmInit()
{
  semLock = sem_open(SEM_NAME_OF_INPUT_DEVICE,
		     O_RDWR, S_IRUSR|S_IWUSR, 0);

  if (semLock != (sem_t *)SEM_FAILED) {
    shmKey = ftok(ShmKeyFileName(), 100);
    if (shmKey >= 0) {
      shmId = shmget(shmKey, SHMVIDEO_BUFSIZE, 0666);
      if (shmId >= 0) {
        shmPtr = shmat(shmId, NULL, 0);
        if (shmPtr) {
          return TRUE;
        }
        else {
          PTRACE(1, "SHMV\t shmInit can not attach shared memory" << endl);
          shmctl(shmId, IPC_RMID, NULL);
          sem_close(semLock);
        }
      }
      else {
        PTRACE(1, "SHMV\t shmInit can not find the shared memory" << endl);
        sem_close(semLock);
      }
    }
    else {
      PTRACE(1, "SHMV\t shmInit can not create key for shared memory" << endl);
      sem_close(semLock);
    }
  }
  else {
    PTRACE(1, "SHMV\t shmInit can not create semaphore" << endl);
  }

  semLock = (sem_t *)SEM_FAILED;
  shmKey = -1;
  shmId = -1;
  shmPtr = NULL;

  return FALSE;
}

PVideoInputDevice_Shm::PVideoInputDevice_Shm()
{
  semLock = (sem_t *)SEM_FAILED;
  shmKey = -1;
  shmId = -1;
  shmPtr = NULL;

  PTRACE(4, "SHMV\t Constructor of PVideoInputDevice_Shm");
}

BOOL
PVideoInputDevice_Shm::Open(const PString & name,
			  BOOL /*startImmediate*/)
{
  PTRACE(1, "SHMV\t Open of PVideoInputDevice_Shm");

  Close();

  if (shmInit() == TRUE) {
    deviceName = name;
    return TRUE;
  }
  else {
    return FALSE;
  }
}

BOOL
PVideoInputDevice_Shm::IsOpen()
{
  if (semLock != (sem_t *)SEM_FAILED) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

BOOL
PVideoInputDevice_Shm::Close()
{
  if (semLock != (sem_t *)SEM_FAILED) {
    shmdt(shmPtr);
    sem_close(semLock);
    shmPtr = NULL;
  }
  return TRUE;
}

BOOL PVideoInputDevice_Shm::IsCapturing()
{
	return TRUE;
}

PINDEX PVideoInputDevice_Shm::GetMaxFrameBytes()
{
	return videoFrameSize;
}

PStringList
PVideoInputDevice_Shm::GetInputDeviceNames()
{
  PStringList list;
  list += "shm";
  return list;
}

BOOL
PVideoInputDevice_Shm::GetFrameSizeLimits(unsigned & minWidth,
					unsigned & minHeight,
					unsigned & maxWidth,
					unsigned & maxHeight) 
{
  minWidth  = 176;
  minHeight = 144;
  maxWidth  = 352;
  maxHeight =  288;

  return TRUE;
}

static void RGBtoYUV420PSameSize (const BYTE *, BYTE *, unsigned, BOOL, 
                                  int, int);


#define rgbtoyuv(r, g, b, y, u, v) \
  y=(BYTE)(((int)30*r  +(int)59*g +(int)11*b)/100); \
  u=(BYTE)(((int)-17*r  -(int)33*g +(int)50*b+12800)/100); \
  v=(BYTE)(((int)50*r  -(int)42*g -(int)8*b+12800)/100); \



static void RGBtoYUV420PSameSize (const BYTE * rgb,
                                  BYTE * yuv,
                                  unsigned rgbIncrement,
                                  BOOL flip, 
                                  int srcFrameWidth, int srcFrameHeight) 
{
  const unsigned planeSize = srcFrameWidth*srcFrameHeight;
  const unsigned halfWidth = srcFrameWidth >> 1;
  
  // get pointers to the data
  BYTE * yplane  = yuv;
  BYTE * uplane  = yuv + planeSize;
  BYTE * vplane  = yuv + planeSize + (planeSize >> 2);
  const BYTE * rgbIndex = rgb;

  for (int y = 0; y < (int) srcFrameHeight; y++) {
    BYTE * yline  = yplane + (y * srcFrameWidth);
    BYTE * uline  = uplane + ((y >> 1) * halfWidth);
    BYTE * vline  = vplane + ((y >> 1) * halfWidth);

    if (flip)
      rgbIndex = rgb + (srcFrameWidth*(srcFrameHeight-1-y)*rgbIncrement);

    for (int x = 0; x < (int) srcFrameWidth; x+=2) {
      rgbtoyuv(rgbIndex[0], rgbIndex[1], rgbIndex[2],*yline, *uline, *vline);
      rgbIndex += rgbIncrement;
      yline++;
      rgbtoyuv(rgbIndex[0], rgbIndex[1], rgbIndex[2],*yline, *uline, *vline);
      rgbIndex += rgbIncrement;
      yline++;
      uline++;
      vline++;
    }
  }
}


BOOL PVideoInputDevice_Shm::GetFrame(PBYTEArray & frame)
{
	PINDEX returned;
	if (!GetFrameData(frame.GetPointer(GetMaxFrameBytes()), &returned))
		return FALSE;
	
	frame.SetSize(returned);
	return TRUE;
}

BOOL
PVideoInputDevice_Shm::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{    
  m_pacing.Delay(1000/GetFrameRate());

  return GetFrameDataNoDelay(buffer, bytesReturned);
}

BOOL
PVideoInputDevice_Shm::GetFrameDataNoDelay (BYTE *buffer, PINDEX *bytesReturned)
{
  long *bufPtr = (long *)shmPtr;

  unsigned width = 0;
  unsigned height = 0;
  unsigned rgbIncrement = 4;

  GetFrameSize (width, height);

  bufPtr[0] = width;
  bufPtr[1] = height;

  if (semLock != (sem_t *)SEM_FAILED && sem_trywait(semLock) == 0) {
    if (bufPtr[0] == (long)width && bufPtr[1] == (long)height) {
      rgbIncrement = bufPtr[2];
      RGBtoYUV420PSameSize ((BYTE *)(bufPtr+3), buffer, rgbIncrement, FALSE, 
			    width, height);
	  
	  *bytesReturned = videoFrameSize;
      return TRUE;
    }
  }

  return FALSE;
}

BOOL PVideoInputDevice_Shm::TestAllFormats()
{
	return TRUE;
}

BOOL PVideoInputDevice_Shm::Start()
{
	return TRUE;
}

BOOL PVideoInputDevice_Shm::Stop()
{
	return TRUE;
}
