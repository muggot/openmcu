/*
 * sunaudio.cxx
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
 * Contributor(s): ______________________________________.
 *
 * $Log: sunaudio.cxx,v $
 * Revision 1.8  2002/03/27 06:53:47  robertj
 * iSome more implementation, thanks Raimo Ruokonen
 *
 * Revision 1.7  2002/02/09 00:52:01  robertj
 * Slight adjustment to API and documentation for volume functions.
 *
 * Revision 1.6  2002/02/07 20:57:21  dereks
 * add SetVolume and GetVolume methods to PSoundChannel
 *
 * Revision 1.5  2001/10/18 01:15:18  robertj
 * Fixed compile error.
 *
 * Revision 1.4  1999/09/03 02:01:34  robertj
 * Added missing functions so will at least link
 *
 * Revision 1.3  1999/06/28 09:28:02  robertj
 * Portability issues, especially n BeOS (thanks Yuri!)
 *
 * Revision 1.2  1999/03/02 04:34:10  robertj
 * Fixed header comments
 *
 */

#pragma implementation "sound.h"

#include <ptlib.h>

#include <sys/audioio.h>


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
}


PSoundChannel::~PSoundChannel()
{
  Close();
}


PStringArray PSoundChannel::GetDeviceNames(Directions /*dir*/)
{
  PStringArray array;

  array[0] = "/dev/audio";
  array[1] = "/dev/dsp";

  return array;
}


PString PSoundChannel::GetDefaultDevice(Directions /*dir*/)
{
  return "/dev/audio";
}


BOOL PSoundChannel::Open(const PString & device,
                         Directions dir,
                         unsigned numChannels,
                         unsigned sampleRate,
                         unsigned bitsPerSample)
{
  Close();
  if (!ConvertOSError(os_handle = ::open(device, (dir == Player ? O_WRONLY : O_RDONLY) ,0)))
     return FALSE;

  direction = dir;
  if (dir == Player) {
    int	flag = fcntl(os_handle, F_GETFL, 0)| O_NONBLOCK| O_NDELAY;

    if (fcntl(os_handle, F_SETFL, flag) < 0) {
      PTRACE(1,"F_SETFL fcntl ERROR");
      return FALSE;
    }
  }

  return SetFormat(numChannels, sampleRate, bitsPerSample);
}


BOOL PSoundChannel::Close()
{
  return PChannel::Close();
}


BOOL PSoundChannel::SetFormat(unsigned numChannels,
                              unsigned sampleRate,
                              unsigned bitsPerSample){
  PAssert(numChannels >= 1 && numChannels <= 2, PInvalidParameter);
  PAssert(bitsPerSample == 8 || bitsPerSample == 16, PInvalidParameter);

  audio_info_t audio_info;
  int err;

  AUDIO_INITINFO(&audio_info);		// Change only the values needed below
  if (direction == Player){
    mSampleRate = audio_info.play.sample_rate = sampleRate;	// Output settings
    mNumChannels = audio_info.play.channels = numChannels;
    mBitsPerSample = audio_info.play.precision = bitsPerSample;
    audio_info.play.encoding = AUDIO_ENCODING_LINEAR; 
    audio_info.play.port &= AUDIO_HEADPHONE;
    audio_info.play.port &= (~AUDIO_SPEAKER);	// No speaker output
  } else {				//  Recorder
    mSampleRate = audio_info.record.sample_rate = sampleRate;	// Input settings
    mNumChannels = audio_info.record.channels = numChannels;
    mBitsPerSample = audio_info.record.precision = bitsPerSample;
    audio_info.record.encoding = AUDIO_ENCODING_LINEAR;
    audio_info.record.port &= AUDIO_MICROPHONE;
    audio_info.record.port &= (~AUDIO_LINE_IN);
  }
  err=::ioctl(os_handle,AUDIO_SETINFO,&audio_info);	// The actual setting of the parameters
  if(err==EINVAL || err==EBUSY)
     return FALSE;

  err = ::ioctl(os_handle, AUDIO_GETINFO, &audio_info);	// Let's recheck the configuration...
  if (direction == Player){
    actualSampleRate = audio_info.play.sample_rate;
//    PAssert(actualSampleRate==sampleRate && audio_info.play.precision==bitsPerSample && audio_info.play.encoding==AUDIO_ENCODING_LINEAR, PInvalidParameter);
  }else{
    actualSampleRate = audio_info.record.sample_rate;
//    PAssert(actualSampleRate==sampleRate && audio_info.record.precision==bitsPerSample && audio_info.record.encoding==AUDIO_ENCODING_LINEAR, PInvalidParameter);
  }
  return TRUE;
}


BOOL PSoundChannel::SetBuffers(PINDEX size, PINDEX count)
{
  PAssert(size > 0 && count > 0 && count < 65536, PInvalidParameter);

  audio_info_t audio_info;
  int err;

  AUDIO_INITINFO(&audio_info);		// Change only the values needed below
  if (direction == Player)
    audio_info.play.buffer_size = count*size;	// man audio says there is no way to set the buffer count... This doesn't affect the actual buffer size on Solaris ?
  else
    audio_info.record.buffer_size = count*size;		// Recorder

  err = ioctl(os_handle,AUDIO_SETINFO,&audio_info);	// The actual setting of the parameters
  if (err == EINVAL || err == EBUSY)
    return FALSE;

  return TRUE;
}


BOOL PSoundChannel::GetBuffers(PINDEX & size, PINDEX & count)
{
  return TRUE;
}


BOOL PSoundChannel::Write(const void * buf, PINDEX len)
{
  return PChannel::Write(buf, len);

/* Implementation based on OSS PSoundChannel::Write. This works, but no difference on sound when compared to the basic implementation...
    while (!ConvertOSError(err=::write(os_handle, (void *)buf, len)))
      if (GetErrorCode() != Interrupted){
	        return FALSE;
	}
    return TRUE;
*/
}


BOOL PSoundChannel::PlaySound(const PSound & sound, BOOL wait)
{
  Abort();

  if (!Write((const BYTE *)sound, sound.GetSize()))
    return FALSE;

  if (wait)
    return WaitForPlayCompletion();

  return TRUE;
}


BOOL PSoundChannel::PlayFile(const PFilePath & filename, BOOL wait)
{
}


BOOL PSoundChannel::HasPlayCompleted()
{
}


BOOL PSoundChannel::WaitForPlayCompletion()
{
}


BOOL PSoundChannel::Read(void * buffer, PINDEX length)
{
  return PChannel::Read(buffer, length);
}


BOOL PSoundChannel::RecordSound(PSound & sound)
{
}


BOOL PSoundChannel::RecordFile(const PFilePath & filename)
{
}


BOOL PSoundChannel::StartRecording()
{
}


BOOL PSoundChannel::IsRecordBufferFull()
{
}


BOOL PSoundChannel::AreAllRecordBuffersFull()
{
}


BOOL PSoundChannel::WaitForRecordBufferFull()
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  return PXSetIOBlock(PXReadBlock, readTimeout);
}


BOOL PSoundChannel::WaitForAllRecordBuffersFull()
{
  return FALSE;
}


BOOL PSoundChannel::Abort()
{
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

