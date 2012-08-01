/*
 * macosxaudio.cxx
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
 * Contributor(s): 
 *
 * $Log: macosaudio.cxx,v $
 * Revision 1.3  2002/02/09 00:52:01  robertj
 * Slight adjustment to API and documentation for volume functions.
 *
 * Revision 1.2  2002/02/07 20:57:21  dereks
 * add SetVolume and GetVolume methods to PSoundChannel
 *
 * Revision 1.1  2001/08/11 15:38:43  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 *
 */

#pragma implementation "sound.h"

#include <ptlib.h>

// goddamn apple headers
#undef TCP_NODELAY
#undef TCP_MAXSEG

//#include "macosaudio/SoundInput.h"
//#include "macosaudio/SoundPlayer.h"
#include "macosaudio/MacMain.h" // interface to the Main Thread for Carbon
#include "macosaudio/ringbuffer.h"

#define DEFAULTSAMPLESIZE 16

// PSound
PSound::PSound(unsigned channels,
               unsigned samplesPerSecond,
               unsigned bitsPerSample,
               PINDEX   bufferSize,
               const BYTE * buffer)
{
	// Copy format
	SetFormat(channels, samplesPerSecond, DEFAULTSAMPLESIZE);
    
    // Copy data passed
    SetSize(bufferSize); 
    if( buffer )
    	memcpy(GetPointer(), buffer, bufferSize);
}


PSound::PSound(const PFilePath & filename)
{
	// Copy format
	SetFormat(1, 8000, DEFAULTSAMPLESIZE);
	
	// Load the sound
    SetSize(0);
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
	formatInfo.SetSize( 0 ); // media format inside

	// Build our format definition
	sampleSize = bitsPerSample;
	sampleRate = samplesPerSecond;
	numChannels = channels;
}

BOOL PSound::Load(const PFilePath & filename)
{
    return FALSE;
}


BOOL PSound::Save(const PFilePath & filename)
{
    return FALSE;
}


PSoundChannel::PSoundChannel() :
	mpInput(NULL),
	mNumChannels(1), mSampleRate(8000), mBitsPerSample(16)
{
  Construct();
}


PSoundChannel::PSoundChannel(const PString & device,
                             Directions dir,
                             unsigned numChannels,
                             unsigned sampleRate,
                             unsigned bitsPerSample) :
	mpInput(NULL),
	mNumChannels(numChannels), mSampleRate(sampleRate), mBitsPerSample(bitsPerSample)
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
  
	if( direction == Recorder && mpInput )
		delete mpInput;
}


PStringArray PSoundChannel::GetDeviceNames(Directions /*dir*/)
{
  PStringArray array;

  array[0]= "built-in audio";
  return array;
}


PString PSoundChannel::GetDefaultDevice(Directions /*dir*/)
{
  return "built-in audio";
}


BOOL PSoundChannel::Open(const PString & dev,
                         Directions dir,
                         unsigned numChannels,
                         unsigned sampleRate,
                         unsigned bitsPerSample)
{
    Close();
    PTRACE(1, "PSoundChannel::Open: " <<  dev << "," << (int)dir);
	
	device = dev;
	direction = dir;
	
  	SetFormat(numChannels, sampleRate, bitsPerSample);

    soundParams sp(numChannels, bitsPerSample, sampleRate);

    if( direction == Player )
	{
        int err;

        // XXX PString only has a const unsigned char * operator
        // XXX if HAS_UNICODE is not defined -- yet they don't provide
        // XXX any decent operators for portably extracting into a
        // XXX char buffer.
        commandRequest r(kOpenPlayer,
                         (const unsigned char *)device, &sp);
        err = r.CarbonQueue();
        if (err == 0 && r.m_status == 0) {
            os_handle = r.m_result;
        }
        else {
            fprintf(stderr,"OpenPlayer failed! %d\n",err);
            goto bail;
        }

        r = commandRequest(kSetFormatPlayer, os_handle, &sp);
        err = r.CarbonQueue();
        if (err != 0 || r.m_status)
            goto bail; // destructor will close channel

        PTRACE(1, "PSoundChannel::Open(p): returning TRUE");
  		return TRUE;
	}
	else
 	if( direction == Recorder )
	{
        mpInput = new JRingBuffer(4096);
        if (!mpInput) goto bail;

        int err;
        commandRequest r(kOpenRecorder, (const unsigned char *)dev, &sp);
        err = r.CarbonQueue();
        if (err == 0 && r.m_status == 0) {
            os_handle = r.m_result;
        }
        else goto bail;
        r = commandRequest(kSetFormatRecorder, os_handle, &sp);
        err = r.CarbonQueue();
        if (err || r.m_status) goto bail;

        r = commandRequest(kStartRecorder,
                           os_handle, (void *)mpInput);
        err = r.CarbonQueue();
        if (err || r.m_status) goto bail;

        PTRACE(1, "PSoundChannel::Open(r): returning TRUE");
        return TRUE;
 	} else assert(0); // bad direction type

  bail: // badness
   	return FALSE;
}


BOOL PSoundChannel::SetFormat(unsigned numChannels,
                              unsigned sampleRate,
                              unsigned bitsPerSample)
{
    PTRACE(1, "PSoundChannel::SetFormat: " <<  numChannels << "," << sampleRate << "," << bitsPerSample);
	
	PAssert(numChannels >= 1 && numChannels <= 2, PInvalidParameter);
	PAssert(	bitsPerSample == 0 ||
		bitsPerSample == 8 || 
		bitsPerSample == 16 ||
		bitsPerSample == 32, PInvalidParameter);

	if( !bitsPerSample )
		PError << "\tWarning: sample bits parameter is zero. Float?" << endl;
	
	mNumChannels = numChannels;
	mSampleRate = sampleRate;
	mBitsPerSample = bitsPerSample;
	
    soundParams sp(numChannels, bitsPerSample, sampleRate);

  	if( direction == Player )
	{
		if( os_handle < 0 )
			return FALSE;		

        commandRequest r(kSetFormatPlayer, os_handle, &sp);
        int err = r.CarbonQueue();
        if (err || r.m_status)
            return FALSE;
        return TRUE;
	}
	else
	if( direction == Recorder )
	{
		if( os_handle < 0 )
			return FALSE;		

        commandRequest r(kSetFormatRecorder, os_handle, &sp);
        int err = r.CarbonQueue();
        if (err || r.m_status)
            return FALSE;
        return TRUE;
    }
    
	return FALSE;
}

BOOL PSoundChannel::Read( void * buf, PINDEX len)
{
  	if( direction == Player )
	{
		return FALSE;
	}
	else
	if( direction == Recorder )
	{
        static int total_read = 0;
        static int message_level = 0;
        
		if( !mpInput )
			return FALSE;
		
		int rlen = mpInput->Read(buf, len);
        PAssert(rlen == -1 || rlen == len, "huh?");
        if (rlen >= 0) {
            static Nanoseconds gStartTime, gMostRecentTime;
            if (total_read == 0) gStartTime = AbsoluteToNanoseconds(UpTime());
            else gMostRecentTime = AbsoluteToNanoseconds(UpTime());
            total_read += rlen;
            if (total_read >= 80000 && message_level == 0) {
                fprintf(stderr,"80000 bytes read\n");
                message_level = 1;
            }
            else if (total_read >= 240000 && message_level == 1) {
                fprintf(stderr,"240000 bytes read at %d samples per sec\n",
                        (total_read*10) / 
                        (long)( (*(long long *)&gMostRecentTime -
                                 *(long long *)&gStartTime ) / 100000000LL));
                message_level = 2;
            }
        }
        
		return rlen == len? TRUE : FALSE;
	}

	return FALSE;
}

BOOL PSoundChannel::Write( const void * buf, PINDEX len)
{
  	if( direction == Player )
	{
        commandRequest r(kPlaySample, (unsigned long)os_handle,
                         (const unsigned char *)buf,
                         (unsigned long)len);
        int err = r.CarbonQueue();
        if (err || r.m_status) {
            fprintf(stderr,"Write failed: err %d status %d\n", err, r.m_status);
            return FALSE;
        }
        
        return TRUE;
	}
	else
	if( direction == Recorder )
	{
		return FALSE;
	}

	return FALSE;
}

BOOL PSoundChannel::Close()
{
  	if( direction == Player )
	{
        if (os_handle >= 0) {
            commandRequest r(kClosePlayer, os_handle);
            (void) r.CarbonQueue();
        }
        isInitialised = false;
        os_handle = -1;
		return TRUE;
	}
	else
	if( direction == Recorder )
	{
        if (os_handle > 0) {
            commandRequest r(kCloseRecorder, os_handle);
            (void) r.CarbonQueue();
        }
        
	 	isInitialised = false;
        os_handle = -1;
		return TRUE;
	}

	return FALSE;
}

BOOL PSoundChannel::SetBuffers(PINDEX size, PINDEX count)
{
  PAssert(size > 0 && count > 0 && count < 65536, PInvalidParameter);

  return TRUE;
}


BOOL PSoundChannel::GetBuffers(PINDEX & size, PINDEX & count)
{
  return TRUE;
}


BOOL PSoundChannel::PlaySound(const PSound & sound, BOOL wait)
{
  if (!Write((const BYTE *)sound, sound.GetSize()))
    return FALSE;

  if (wait)
    return WaitForPlayCompletion();
  return TRUE;
}


BOOL PSoundChannel::PlayFile(const PFilePath & filename, BOOL wait)
{
  return FALSE;
}


BOOL PSoundChannel::HasPlayCompleted()
{
    commandRequest r(kIsPlaying, os_handle);
    (void)r.CarbonQueue();
	return r.m_result ? FALSE : TRUE;
}


BOOL PSoundChannel::WaitForPlayCompletion()
{
    commandRequest r(kWaitForPlayCompletion, os_handle);
    int err = r.CarbonQueue();
    if (err == 0) err = r.m_status;
    return err == 0;
}


BOOL PSoundChannel::RecordSound(PSound & sound)
{
  return FALSE;
}


BOOL PSoundChannel::RecordFile(const PFilePath & filename)
{
  return FALSE;
}


BOOL PSoundChannel::StartRecording()
{
  	if( direction == Player )
	{
		return FALSE;
	}
	else
	if( direction == Recorder )
	{
		PAssertNULL(mpInput);

        commandRequest r(kStartRecorder, os_handle, mpInput);
        int err = r.CarbonQueue();
        if (err == 0) err = r.m_status;
		
		if( err == 0 )
			PError << "Recording started" << endl;
		return err == 0 ? TRUE : FALSE;
	}

	return FALSE;
}


// Is there any record data to read?
BOOL PSoundChannel::IsRecordBufferFull()
{
	if( direction == Recorder )
	{
		PAssertNULL(mpInput);
        return !mpInput->IsEmpty();
    }
    return FALSE;
}


BOOL PSoundChannel::AreAllRecordBuffersFull()
{
	if( direction == Recorder )
	{
		PAssertNULL(mpInput);
        return mpInput->IsFull();
    }
    return FALSE;
}


BOOL PSoundChannel::WaitForRecordBufferFull()
{
	if( direction == Recorder )
	{
		PAssertNULL(mpInput);
        return mpInput->WaitForData();
    }
    return FALSE;
}


BOOL PSoundChannel::WaitForAllRecordBuffersFull()
{
  return FALSE;
}


BOOL PSoundChannel::Abort()
{
  	if( direction == Player )
	{
        commandRequest r(kStopPlayer, os_handle);
        int err = r.CarbonQueue();
        if (err == 0) err = r.m_status; 
       return err == 0;
	}
  	else
  	if( direction == Recorder )
	{
		if(!mpInput)
			return FALSE;
			
        commandRequest r(kStopRecorder, os_handle);
        int err = r.CarbonQueue();
        if (err == 0) err = r.m_status;
        return TRUE;
	}
	
	return FALSE;
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

