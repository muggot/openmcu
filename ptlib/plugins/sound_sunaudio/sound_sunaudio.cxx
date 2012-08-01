/*
 * sound_sunaudio.cxx
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
 * Contributor(s): brian.lu@sun.com
 *
 * $Log: sound_sunaudio.cxx,v $
 * Revision 1.6  2007/06/26 10:57:06  dsandras
 * Applied patch from Elaine Xion <elaine xiong sun com> to fix
 * volume settings in the SunAudio plugin. Thanks !
 *
 * Revision 1.5  2006/10/12 18:20:03  dsandras
 * Fixed initialization of SUN Audio plugin thanks to Brian Lu <brian lu sun com>
 *
 * Revision 1.4  2006/09/17 16:01:38  dsandras
 * Fixed possible crash on hangup with "Media patch thread not terminated"
 * message thanks to Brian Lu <brian lu sun com>.
 *
 * Revision 1.3  2006/07/03 21:12:51  dsandras
 * Applied patch from Brian Lu <brian lu sun com> to add support for the AUDIODEV
 * environment variable. Many thanks!
 *
 * Revision 1.2  2006/05/29 18:40:28  dsandras
 * Applied patch from Brian Lu <brian lu sun com>. Thanks!
 *
 * Revision 1.1  2006/01/24 20:43:11  dsandras
 * Added Sunaudio support for OpenSolaris thanks to Brian Lu <brian lu sun com>.
 * Thanks a lot for this!
 *
 * The original codes are from ../../src/ptlib/unix/sunaudio.cxx
 */

#ifdef _GNUC_
#pragma implementation "sound.h"
#endif

#include <sys/types.h>
#include <stropts.h>
#include <sys/conf.h>
#include <sys/mixer.h>
#include <pthread.h>

#include <ptlib.h>

#include "sound_sunaudio.h"

PCREATE_SOUND_PLUGIN(SunAudio, PSoundChannelSunAudio);

#define AUDIO_DEVICE "/dev/audio"

///////////////////////////////////////////////////////////////////////////////

PSoundChannelSunAudio::PSoundChannelSunAudio()
{
  Construct();
}


PSoundChannelSunAudio::PSoundChannelSunAudio(const PString & device,
                             Directions dir,
                             unsigned numChannels,
                             unsigned sampleRate,
                             unsigned bitsPerSample)
{
  Construct();
  Open(device, dir, numChannels, sampleRate, bitsPerSample);
}


void PSoundChannelSunAudio::Construct()
{
   os_handle = -1;
}


PSoundChannelSunAudio::~PSoundChannelSunAudio()
{
  Close();
}

PStringArray PSoundChannelSunAudio::GetDeviceNames(Directions /*dir*/)
{
  PStringArray array;

  static char* audio_device_name=getenv("AUDIODEV");

  array[0] = audio_device_name? audio_device_name:AUDIO_DEVICE;
  return array;
}


PString PSoundChannelSunAudio::GetDefaultDevice(Directions /*dir*/)
{
  static char* audio_device_name=getenv("AUDIODEV");

  return audio_device_name? audio_device_name:AUDIO_DEVICE;
}


BOOL PSoundChannelSunAudio::Open(const PString & device,
                         Directions dir,
                         unsigned numChannels,
                         unsigned sampleRate,
                         unsigned bitsPerSample)
{
  audio_info_t audio_info;
  int err;

  Close();
  os_handle = -1;
  resampleRate = 0;

  if (!ConvertOSError(os_handle = ::open(device, (dir == Player ? O_WRONLY : O_RDONLY), 0 )))
    return FALSE;

  direction = dir;

  err = ::ioctl(os_handle,AUDIO_MIXER_MULTIPLE_OPEN);

  AUDIO_INITINFO(&audio_info);

  // save the default settings for resetting
  err = ::ioctl(os_handle, AUDIO_GETINFO, &audio_info);	
  if (err==EINVAL || err==EBUSY)
  {
    ::close(os_handle);
    os_handle = -1;
    return FALSE;
  }

  mDefaultPlayNumChannels = audio_info.play.channels; 
  mDefaultPlaySampleRate = audio_info.play.sample_rate; 
  mDefaultPlayBitsPerSample =  audio_info.play.precision; 

  mDefaultRecordNumChannels = audio_info.record.channels; 
  mDefaultRecordSampleRate = audio_info.record.sample_rate; 
  mDefaultRecordBitsPerSample =  audio_info.record.precision; 
  mDefaultRecordEncoding = audio_info.record.encoding;
  mDefaultRecordPort = audio_info.record.port;

  return SetFormat(numChannels, sampleRate, bitsPerSample);
}


BOOL PSoundChannelSunAudio::Close()
{
  if (os_handle < 0)
    return TRUE;
  return PChannel::Close();
}

BOOL PSoundChannelSunAudio::IsOpen() const 
{
  return os_handle >=0;
}


BOOL PSoundChannelSunAudio::SetFormat(unsigned numChannels,
                              unsigned sampleRate,
                              unsigned bitsPerSample){
  PAssert(numChannels >= 1 && numChannels <= 2, PInvalidParameter);
  PAssert(bitsPerSample == 8 || bitsPerSample == 16, PInvalidParameter);

  audio_info_t audio_info;
  int err;

  // Change only the values needed below
  AUDIO_INITINFO(&audio_info);	
  if (direction == Player){
    // sett parameters for playing sound
    mSampleRate = audio_info.play.sample_rate = sampleRate;	
    mNumChannels = audio_info.play.channels = numChannels;
    mBitsPerSample = audio_info.play.precision = bitsPerSample;
    audio_info.play.encoding = AUDIO_ENCODING_LINEAR; 

  } else {				
    // set parameters for recording sound
    audio_info.record.sample_rate = mSampleRate = sampleRate;	
    audio_info.record.channels = mNumChannels = numChannels;
    audio_info.record.precision = mBitsPerSample = bitsPerSample;
    audio_info.record.encoding = AUDIO_ENCODING_LINEAR;
  }

  // The actual setting of the parameters
  err=::ioctl(os_handle,AUDIO_SETINFO,&audio_info);	
  if (err==EINVAL || err==EBUSY)
    return FALSE;

  // Let's recheck the configuration...
  AUDIO_INITINFO(&audio_info);	
  err = ::ioctl(os_handle, AUDIO_GETINFO, &audio_info);	
  actualSampleRate =  (direction == Player) ? audio_info.play.sample_rate : audio_info.record.sample_rate;

  return TRUE;
}

unsigned PSoundChannelSunAudio::GetChannels() const  
{
   return mNumChannels;
}


unsigned PSoundChannelSunAudio::GetSampleRate() const  
{
   return actualSampleRate;
}

unsigned PSoundChannelSunAudio::GetSampleSize() const  
{
   return mBitsPerSample;
}

BOOL PSoundChannelSunAudio::SetBuffers(PINDEX size, PINDEX count)
{
  PAssert(size > 0 && count > 0 && count < 65536, PInvalidParameter);

  audio_info_t audio_info;
  int err;

  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  /* There is just one buffer for audio on solaris */
  AUDIO_INITINFO(&audio_info);

  if (direction == Player)
    audio_info.play.buffer_size = count*size;	
  else
    audio_info.record.buffer_size = count*size;	// Recorder

  // The actual setting of the parameters
  err = ::ioctl(os_handle,AUDIO_SETINFO,&audio_info);	
  if (err == EINVAL || err == EBUSY)
    return FALSE;

  return TRUE;
}


BOOL PSoundChannelSunAudio::GetBuffers(PINDEX & size, PINDEX & count)
{
  audio_info_t audio_info;
  int err;

  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  // There is just one buffer for sun audio on solaris
  // so COUNT is set to 1 and SIZE is set to the actually buffer size
  AUDIO_INITINFO(&audio_info);

  err = ::ioctl(os_handle,AUDIO_GETINFO,&audio_info);
  if (err == EINVAL || err == EBUSY)
    return FALSE;

  if (direction == Player)
    size = audio_info.play.buffer_size;
  else 
    size = audio_info.record.buffer_size;

  count = 1;

  return TRUE;
}


BOOL PSoundChannelSunAudio::Write(const void * buffer, PINDEX length)
{

  PINDEX total = 0;
  int ret = 0;

  if (os_handle < 0 )
    return SetErrorValues(NotOpen,EBADF);

  while (total < length) {
    PINDEX bytes = 0;

    while (!ConvertOSError(bytes = ::write(os_handle, (void *)(((unsigned char *)buffer) + total), length-total))) {
      if ((GetErrorCode() != Interrupted) || ( os_handle < 0)) {
        PTRACE(6, "SunAudio\tWirte failed");
        return FALSE;         
      }
      PTRACE(6, "SunAudio\tWrite interrupted");       
    }

    total += bytes;
    if (total != length)
      PTRACE(6, "SunAudio\tWrite completed short - " << total << " vs " << length << ". Write more data");
  }

  lastWriteCount = total;

  // Reset all the errors.
  return ConvertOSError(0, LastWriteError);
}

BOOL PSoundChannelSunAudio::PlaySound(const PSound & sound, BOOL wait)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  if (!Write((const BYTE *)sound, sound.GetSize()))
    return FALSE;

  if (wait)
    return WaitForPlayCompletion();

  return TRUE;
}


BOOL PSoundChannelSunAudio::PlayFile(const PFilePath & filename, BOOL wait)
{
  if (os_handle < 0 )
    return SetErrorValues(NotOpen,EBADF);

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


BOOL PSoundChannelSunAudio::HasPlayCompleted()
{
  int err;
  audio_info_t audio_info;

  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  AUDIO_INITINFO(&audio_info);		
  err = ::ioctl(os_handle, AUDIO_GETINFO, &audio_info);

  return err == 0 && audio_info.play.eof != 0;
}


BOOL PSoundChannelSunAudio::WaitForPlayCompletion()
{

  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  return ConvertOSError(::ioctl(os_handle, AUDIO_DRAIN, NULL));
}

BOOL PSoundChannelSunAudio::Read(void * buffer, PINDEX length)
{
 
  int ret;
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  PINDEX total = 0;
  while (total < length) {
    PINDEX bytes = 0;

    while (!ConvertOSError(bytes = ::read(os_handle, (void *)(((unsigned char *)buffer) + total), length-total))) {
      if ((GetErrorCode() != Interrupted) || (os_handle < 0)) {
        PTRACE(6, "SunAudio\tRead failed");
        return FALSE;
      }
      PTRACE(6, "SunAudio\tRead interrupted");
    } 

    total += bytes;
    if (total != length)
      PTRACE(6, "SunAudio\tRead completed short - " << total << " vs " << length << ". Reading more data");
  }

  lastReadCount = total;

  if (lastReadCount != length)
    PTRACE(6, "SunAudio\tRead completed short - " << lastReadCount << " vs " << length);
  else
    PTRACE(6, "SunAudio\tRead completed");
 
  return TRUE;
}


BOOL PSoundChannelSunAudio::RecordSound(PSound & sound)
{
   return FALSE;
}


BOOL PSoundChannelSunAudio::RecordFile(const PFilePath & filename)
{
   return FALSE;
}


BOOL PSoundChannelSunAudio::StartRecording()
{
  return TRUE;
}


BOOL PSoundChannelSunAudio::IsRecordBufferFull()
{
  int err;
  audio_info_t audio_info;

  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  AUDIO_INITINFO(&audio_info);		
  err = ::ioctl(os_handle, AUDIO_GETINFO, &audio_info);

  return err == 0 && audio_info.record.error != 0;
}


BOOL PSoundChannelSunAudio::AreAllRecordBuffersFull()
{
   /* There is a just one buffer */
   return IsRecordBufferFull();
}


BOOL PSoundChannelSunAudio::WaitForRecordBufferFull()
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  return PXSetIOBlock(PXReadBlock, readTimeout);
}


BOOL PSoundChannelSunAudio::WaitForAllRecordBuffersFull()
{
  return WaitForRecordBufferFull();
}


BOOL PSoundChannelSunAudio::Abort()
{
  audio_info_t audio_info;
  int err;

  if (os_handle < 0) 
  {
    PTRACE(1,"PSoundChannelSunAudio::Abort() os_handle is invalid");
    return FALSE;
  }

  AUDIO_INITINFO(&audio_info);


  audio_info.play.channels = mDefaultPlayNumChannels; 
  audio_info.play.sample_rate= mDefaultPlaySampleRate; 
  audio_info.play.precision = mDefaultPlayBitsPerSample; 

  audio_info.record.channels =  mDefaultRecordNumChannels;
  audio_info.record.sample_rate =  mDefaultRecordSampleRate; 
  audio_info.record.precision =  mDefaultRecordBitsPerSample; 
  audio_info.record.encoding =  mDefaultRecordEncoding; 
  audio_info.record.port =  mDefaultRecordPort; 

  err = ::ioctl(os_handle, AUDIO_SETINFO, &audio_info);	// Let's recheck the configuration...
  if (err==EINVAL || err==EBUSY)
    return FALSE;

  return TRUE;
}

BOOL PSoundChannelSunAudio::SetVolume(unsigned newVolume)
{
   audio_info_t audio_info;
   int err;

   /* Check if the new volume is valid or not */
   if ( newVolume < AUDIO_MIN_GAIN || newVolume > AUDIO_MAX_GAIN )
     return FALSE;

   newVolume = (newVolume * (AUDIO_MAX_GAIN - AUDIO_MIN_GAIN)) / 100;

   AUDIO_INITINFO(&audio_info);
   if ( direction == Player )
     audio_info.play.gain = newVolume;
   else 
     audio_info.record.gain = newVolume; 

   err=::ioctl(os_handle,AUDIO_SETINFO,&audio_info);	// The actual setting of the parameters
   if (err==EINVAL || err==EBUSY)
   {
     PTRACE(1,  "PSoundChannelSunAudio::SetVolume failed : " << ::strerror(errno)) ;
     return FALSE;
   }

   return TRUE;
}

BOOL  PSoundChannelSunAudio::GetVolume(unsigned & volume)
{
   audio_info_t audio_info;
   int err;

   AUDIO_INITINFO(&audio_info);

   err=::ioctl(os_handle,AUDIO_GETINFO,&audio_info);
   if (err==EINVAL || err==EBUSY)
   {
     PTRACE(1,  "PSoundChannelSunAudio::GetVolume failed : " << ::strerror(errno)) ;
     return FALSE;
   }

   volume =  ( direction == Player ) ?  audio_info.play.gain : audio_info.record.gain;

   volume = (volume * 100) / (AUDIO_MAX_GAIN - AUDIO_MIN_GAIN);

   return TRUE;
}

// End of file
