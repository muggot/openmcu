/*
 * sound.cxx
 *
 * Sound driver implementation.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): Loopback feature: Philip Edelbrock <phil@netroedge.com>.
 *
 * $Log: ossaix.cxx,v $
 * Revision 1.3  2002/02/09 00:52:01  robertj
 * Slight adjustment to API and documentation for volume functions.
 *
 * Revision 1.2  2002/02/07 20:57:21  dereks
 * add SetVolume and GetVolume methods to PSoundChannel
 *
 * Revision 1.1  2000/06/21 01:01:22  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.17  2000/05/11 02:05:54  craigs
 * Fixed problem with PLayFile not recognizing wait flag
 *
 * Revision 1.16  2000/05/10 02:10:44  craigs
 * Added implementation for PlayFile command
 *
 * Revision 1.15  2000/05/02 08:30:26  craigs
 * Removed "memory leaks" caused by brain-dead GNU linker
 *
 * Revision 1.14  2000/04/09 18:19:23  rogerh
 * Add my changes for NetBSD support.
 *
 * Revision 1.13  2000/03/08 12:17:09  rogerh
 * Add OpenBSD support
 *
 * Revision 1.12  2000/03/04 13:02:28  robertj
 * Added simple play functions for sound files.
 *
 * Revision 1.11  2000/02/15 23:11:34  robertj
 * Audio support for FreeBSD, thanks Roger Hardiman.
 *
 * Revision 1.10  2000/01/08 06:41:08  craigs
 * Fixed problem whereby failure to open sound device returns TRUE
 *
 * Revision 1.9  1999/08/24 13:40:26  craigs
 * Fixed problem with EINTR causing sound channel reads and write to fail
 * Thanks to phil@netroedge.com!
 *
 * Revision 1.8  1999/08/17 09:42:22  robertj
 * Fixed close of sound channel in loopback mode closing stdin!
 *
 * Revision 1.7  1999/08/17 09:28:47  robertj
 * Added audio loopback psuedo-device (thanks Philip Edelbrock)
 *
 * Revision 1.6  1999/07/19 01:31:49  craigs
 * Major rewrite to assure ioctls are all done in the correct order as OSS seems
 *    to be incredibly sensitive to this.
 *
 * Revision 1.5  1999/07/11 13:42:13  craigs
 * pthreads support for Linux
 *
 * Revision 1.4  1999/06/30 13:49:26  craigs
 * Added code to allow full duplex audio
 *
 * Revision 1.3  1999/05/28 14:14:29  robertj
 * Added function to get default audio device.
 *
 * Revision 1.2  1999/05/22 12:49:05  craigs
 * Finished implementation for Linux OSS interface
 *
 * Revision 1.1  1999/02/25 03:45:00  robertj
 * Sound driver implementation changes for various unix platforms.
 *
 * Revision 1.1  1999/02/22 13:24:47  robertj
 * Added first cut sound implmentation.
 *
 */

#pragma implementation "sound.h"

#include <ptlib.h>

#ifdef P_LINUX
#include <sys/soundcard.h>
#include <sys/time.h>
#endif

#ifdef P_FREEBSD
#include <machine/soundcard.h>
#endif

#if defined(P_OPENBSD) || defined(P_NETBSD)
#include <soundcard.h>
#endif


///////////////////////////////////////////////////////////////////////////////
// declare type for sound handle dictionary

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

PDICTIONARY(SoundHandleDict, PString, SoundHandleEntry);

#define LOOPBACK_BUFFER_SIZE 5000
#define BYTESINBUF ((startptr<endptr)?(endptr-startptr):(LOOPBACK_BUFFER_SIZE+endptr-startptr))

static char buffer[LOOPBACK_BUFFER_SIZE];
static int  startptr, endptr;

PMutex PSoundChannel::dictMutex;

static SoundHandleDict & handleDict()
{
  static SoundHandleDict dict;
  return dict;
}

PSound::PSound(unsigned channels,
               unsigned samplesPerSecond,
               unsigned bitsPerSample,
               PINDEX   bufferSize,
               const BYTE * buffer)
{
  encoding = 0;
  numChannels = channels;
  sampleRate = samplesPerSecond;
  sampleSize = bitsPerSample;
  SetSize(bufferSize);
  if (buffer != NULL)
    memcpy(GetPointer(), buffer, bufferSize);
}


PSound::PSound(const PFilePath & filename)
{
  encoding = 0;
  numChannels = 1;
  sampleRate = 8000;
  sampleSize = 16;
  Load(filename);
}


PSound & PSound::operator=(const PBYTEArray & data)
{
  PBYTEArray::operator=(data);
  return *this;
}


void PSound::SetFormat(unsigned channels,
                       unsigned samplesPerSecond,
                       unsigned bitsPerSample)
{
  encoding = 0;
  numChannels = channels;
  sampleRate = samplesPerSecond;
  sampleSize = bitsPerSample;
  formatInfo.SetSize(0);
}


BOOL PSound::Load(const PFilePath & /*filename*/)
{
  return FALSE;
}


BOOL PSound::Save(const PFilePath & /*filename*/)
{
  return FALSE;
}


BOOL PSound::Play()
{
  PSoundChannel channel(PSoundChannel::GetDefaultDevice(PSoundChannel::Player),
                        PSoundChannel::Player);
  if (!channel.IsOpen())
    return FALSE;

  return channel.PlaySound(*this, TRUE);
}


BOOL PSound::PlayFile(const PFilePath & file, BOOL wait)
{
  PSoundChannel channel(PSoundChannel::GetDefaultDevice(PSoundChannel::Player),
                        PSoundChannel::Player);
  if (!channel.IsOpen())
    return FALSE;

  return channel.PlayFile(file, wait);
}


///////////////////////////////////////////////////////////////////////////////

SoundHandleEntry::SoundHandleEntry()
{
  handle    = -1;
  direction = 0;
}

///////////////////////////////////////////////////////////////////////////////

PSoundChannel::PSoundChannel()
{
  Construct();
}


PSoundChannel::PSoundChannel(const PString & device,
                             Directions dir,
                             unsigned numChannels,
                             unsigned sampleRate,
                             unsigned bitsPerSample)
{
  Construct();
  Open(device, dir, numChannels, sampleRate, bitsPerSample);
}


void PSoundChannel::Construct()
{
  os_handle = -1;
}


PSoundChannel::~PSoundChannel()
{
  Close();
}


PStringArray PSoundChannel::GetDeviceNames(Directions /*dir*/)
{
  static const char * const devices[] = {
    "/dev/audio",
    "/dev/dsp",
    "/dev/dspW",
    "loopback"
  };

  return PStringArray(PARRAYSIZE(devices), devices);
}


PString PSoundChannel::GetDefaultDevice(Directions /*dir*/)
{
  return "/dev/dsp";
}


BOOL PSoundChannel::Open(const PString & _device,
                              Directions _dir,
                                unsigned _numChannels,
                                unsigned _sampleRate,
                                unsigned _bitsPerSample)
{
  Close();

  // lock the dictionary
  dictMutex.Wait();

  // make the direction value 1 or 2
  int dir = _dir + 1;

  // if this device in in the dictionary
  if (handleDict().Contains(_device)) {

    SoundHandleEntry & entry = handleDict()[_device];

    // see if the sound channel is already open in this direction
    if ((entry.direction & dir) != 0) {
      dictMutex.Signal();
      return FALSE;
    }

    // flag this entry as open in this direction
    entry.direction |= dir;
    os_handle = entry.handle;

  } else {

    // this is the first time this device has been used
    // open the device in read/write mode always
    if (_device == "loopback") {
      startptr = endptr = 0;
      os_handle = 0; // Use os_handle value 0 to indicate loopback, cannot ever be stdin!
    }
    else if (!ConvertOSError(os_handle = ::open((const char *)_device, O_RDWR))) {
      dictMutex.Signal();
      return FALSE;
    }

    // add the device to the dictionary
    SoundHandleEntry * entry = PNEW SoundHandleEntry;
    handleDict().SetAt(_device, entry); 

    // save the information into the dictionary entry
    entry->handle        = os_handle;
    entry->direction     = dir;
    entry->numChannels   = _numChannels;
    entry->sampleRate    = _sampleRate;
    entry->bitsPerSample = _bitsPerSample;
    entry->isInitialised = FALSE;
    entry->fragmentValue = 0x7fff0008;
  }
   
   
  // unlock the dictionary
  dictMutex.Signal();

  // save the direction and device
  direction     = _dir;
  device        = _device;
  isInitialised = FALSE;

  return TRUE;
}

BOOL PSoundChannel::Setup()
{
  if (os_handle < 0)
    return FALSE;

  if (isInitialised)
    return TRUE;

  // lock the dictionary
  dictMutex.Wait();

  // the device must always be in the dictionary
  PAssertOS(handleDict().Contains(device));

  // get record for the device
  SoundHandleEntry & entry = handleDict()[device];

  BOOL stat = FALSE;
  if (entry.isInitialised)  {
    isInitialised = TRUE;
    stat          = TRUE;
  } else if (device == "loopback")
    stat = TRUE;
  else {

  // must always set paramaters in the following order:
  //   buffer paramaters
  //   sample format (number of bits)
  //   number of channels (mon/stereo)
  //   speed (sampling rate)

    int arg, val;

    // reset the device first so it will accept the new parms

#ifndef P_AIX
    if (ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_RESET, &arg))) {

      arg = val = entry.fragmentValue;
      //if (ConvertOSError(ioctl(os_handle, SNDCTL_DSP_SETFRAGMENT, &arg)) || (arg != val)) {
      ::ioctl(os_handle, SNDCTL_DSP_SETFRAGMENT, &arg); {

        arg = val = (entry.bitsPerSample == 16) ? AFMT_S16_LE : AFMT_S8;
        if (ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_SETFMT, &arg)) || (arg != val)) {

          arg = val = (entry.numChannels == 2) ? 1 : 0;
          if (ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_STEREO, &arg)) || (arg != val)) {

            arg = val = entry.sampleRate;
            if (ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_SPEED, &arg)) || (arg != val)) 
              stat = TRUE;
          }
        }
      }
    }
#endif

  }

  entry.isInitialised = TRUE;
  isInitialised       = TRUE;

  dictMutex.Signal();

  return stat;
}

BOOL PSoundChannel::Close()
{
  // if the channel isn't open, do nothing
  if (os_handle < 0)
    return TRUE;

  if (os_handle == 0) {
    os_handle = -1;
    return TRUE;
  }

  // the device must be in the dictionary
  dictMutex.Wait();
  SoundHandleEntry * entry;
  PAssert((entry = handleDict().GetAt(device)) != NULL, "Unknown sound device \"" + device + "\" found");

  // modify the directions bit mask in the dictionary
  entry->direction ^= (direction+1);

  // if this is the last usage of this entry, then remove it
  if (entry->direction == 0) {
    handleDict().RemoveAt(device);
    dictMutex.Signal();
    return PChannel::Close();
  }

  // flag this channel as closed
  dictMutex.Signal();
  os_handle = -1;
  return TRUE;
}

BOOL PSoundChannel::Write(const void * buf, PINDEX len)
{
  if (!Setup())
    return FALSE;

  if (os_handle > 0) {
    while (!ConvertOSError(::write(os_handle, (void *)buf, len)))
      if (GetErrorCode() != Interrupted)
        return FALSE;
    return TRUE;
  }

  int index = 0;

  while (len > 0) {
    len--;
    buffer[endptr++] = ((char *)buf)[index++];
    if (endptr == LOOPBACK_BUFFER_SIZE)
      endptr = 0;
    while (((startptr - 1) == endptr) || ((endptr==LOOPBACK_BUFFER_SIZE - 1) && (startptr==0))) {
      usleep(5000);
    }
  }
  return TRUE;
}

BOOL PSoundChannel::Read(void * buf, PINDEX len)
{
  if (!Setup())
    return FALSE;

  if (os_handle > 0) {
    while (!ConvertOSError(::read(os_handle, (void *)buf, len)))
      if (GetErrorCode() != Interrupted)
        return FALSE;
    return TRUE;
  }

  int index = 0;

  while (len > 0) {
    while (startptr == endptr)
      usleep(5000);
    len--;
    ((char *)buf)[index++]=buffer[startptr++];
    if (startptr == LOOPBACK_BUFFER_SIZE)
      startptr = 0;
  } 
  return TRUE;
}


BOOL PSoundChannel::SetFormat(unsigned numChannels,
                              unsigned sampleRate,
                              unsigned bitsPerSample)
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  // check parameters
  PAssert((bitsPerSample == 8) || (bitsPerSample == 16), PInvalidParameter);
  PAssert(numChannels >= 1 && numChannels <= 2, PInvalidParameter);

  Abort();

  // lock the dictionary
  dictMutex.Wait();

  // the device must always be in the dictionary
  PAssertOS(handleDict().Contains(device));

  // get record for the device
  SoundHandleEntry & entry = handleDict()[device];

  entry.numChannels   = numChannels;
  entry.sampleRate    = sampleRate;
  entry.bitsPerSample = bitsPerSample;
  entry.isInitialised  = FALSE;

  // unlock dictionary
  dictMutex.Signal();

  // mark this channel as uninitialised
  isInitialised = FALSE;

  return TRUE;
}


BOOL PSoundChannel::SetBuffers(PINDEX size, PINDEX count)
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  Abort();

  PAssert(size > 0 && count > 0 && count < 65536, PInvalidParameter);
  int arg = 1;
  while (size > (PINDEX)(1 << arg))
    arg++;

  arg |= count << 16;

  // lock the dictionary
  dictMutex.Wait();

  // the device must always be in the dictionary
  PAssertOS(handleDict().Contains(device));

  // get record for the device
  SoundHandleEntry & entry = handleDict()[device];

  // set information in the common record
  entry.fragmentValue = arg;
  entry.isInitialised = FALSE;

  // flag this channel as not initialised
  isInitialised       = FALSE;

  dictMutex.Signal();

  return TRUE;
}


BOOL PSoundChannel::GetBuffers(PINDEX & size, PINDEX & count)
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  // lock the dictionary
  dictMutex.Wait();

  // the device must always be in the dictionary
  PAssertOS(handleDict().Contains(device));

  SoundHandleEntry & entry = handleDict()[device];

  int arg = entry.fragmentValue;

  dictMutex.Signal();

  count = arg >> 16;
  size = 1 << (arg&0xffff);
  return TRUE;
}


BOOL PSoundChannel::PlaySound(const PSound & sound, BOOL wait)
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  Abort();

  if (!Write((const BYTE *)sound, sound.GetSize()))
    return FALSE;

  if (wait)
    return WaitForPlayCompletion();

  return TRUE;
}


BOOL PSoundChannel::PlayFile(const PFilePath & filename, BOOL wait)
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  PFile file(filename, PFile::ReadOnly);
  if (!file.IsOpen())
    return FALSE;

  for (;;) {
    BYTE buffer[256];
    if (!file.Read(buffer, 256))
      break;
    PINDEX len = file.GetLastReadCount();
    if (len == 0)
      break;
    if (!Write(buffer, len))
      break;
  }

  file.Close();

  if (wait)
    return WaitForPlayCompletion();

  return TRUE;
}


BOOL PSoundChannel::HasPlayCompleted()
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  if (os_handle == 0)
    return BYTESINBUF <= 0;

#ifndef P_AIX
  audio_buf_info info;
  if (!ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_GETOSPACE, &info)))
    return FALSE;

  return info.fragments == info.fragstotal;
#else
  return 0;
#endif

}


BOOL PSoundChannel::WaitForPlayCompletion()
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  if (os_handle == 0) {
    while (BYTESINBUF > 0)
      usleep(1000);
    return TRUE;
  }
#ifndef P_AIX
  return ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_SYNC, NULL));
#else
  return 0;
#endif
}


BOOL PSoundChannel::RecordSound(PSound & sound)
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  return FALSE;
}


BOOL PSoundChannel::RecordFile(const PFilePath & filename)
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  return FALSE;
}


BOOL PSoundChannel::StartRecording()
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  if (os_handle == 0)
    return TRUE;

  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(os_handle, &fds);

  struct timeval timeout;
  memset(&timeout, 0, sizeof(timeout));

  return ConvertOSError(::select(1, &fds, NULL, NULL, &timeout));
}


BOOL PSoundChannel::IsRecordBufferFull()
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  if (os_handle == 0)
    return (BYTESINBUF > 0);

#ifndef P_AIX
  audio_buf_info info;
  if (!ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_GETISPACE, &info)))
    return FALSE;

  return info.fragments > 0;
#else
  return 0;
#endif
}


BOOL PSoundChannel::AreAllRecordBuffersFull()
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  if (os_handle == 0)
    return (BYTESINBUF == LOOPBACK_BUFFER_SIZE);

#ifndef P_AIX
  audio_buf_info info;
  if (!ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_GETISPACE, &info)))
    return FALSE;

  return info.fragments == info.fragstotal;
#else
  return 0;
#endif
}


BOOL PSoundChannel::WaitForRecordBufferFull()
{
  if (os_handle < 0) {
    lastError = NotOpen;
    return FALSE;
  }

  return PXSetIOBlock(PXReadBlock, readTimeout);
}


BOOL PSoundChannel::WaitForAllRecordBuffersFull()
{
  return FALSE;
}


BOOL PSoundChannel::Abort()
{
  if (os_handle == 0) {
    startptr = endptr = 0;
    return TRUE;
  }

#ifndef P_AIX
  return ConvertOSError(ioctl(os_handle, SNDCTL_DSP_RESET, NULL));
#else
  return 0;
#endif
}

BOOL PSoundChannel::SetVolume(unsigned newVolume)
{
  cerr << __FILE__ << "PSoundChannel :: SetVolume called in error. Please fix"<<endl;
  return FALSE;
}

BOOL  PSoundChannel::GetVolume(unsigned & volume)
{
 cerr << __FILE__ << "PSoundChannel :: GetVolume called in error. Please fix"<<endl;
  return FALSE;
}



// End of file
