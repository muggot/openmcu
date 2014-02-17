/*
 * sound_directsound.h
 *
 * DirectX Sound driver implementation.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2006-2007 Novacom, a division of IT-Optics
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
 * The Initial Developer of the Original DirectSound Code is 
 * Vincent Luba <vincent.luba@novacom.be>
 *
 * Contributor(s): /
 *
 * $Log: sound_directsound.h,v $
 * Revision 1.2  2007/05/19 08:54:42  rjongbloed
 * Further integration of DirectSound plugin thanks to Vincent Luba.and NOVACOM (http://www.novacom.be).
 *
 * Revision 1.1  2007/05/15 21:39:26  dsandras
 * Added initial code for a DirectSound plugin thanks to Vincent Luba.
 * Code contributed by NOVACOM (http://www.novacom.be).
 *
 * Revision 1.1  2006/09/15 10:04:05  luba vincent
 * First implementation of DirectSound driver
 */

#ifndef __DIRECTSOUND_H__
#define __DIRECTSOUND_H__


#include <ptlib.h>
#include <ptbuildopts.h>

#ifdef P_DIRECTSOUND

#include <ptlib/sound.h>

#include <dsound.h>


typedef struct 
{
  GUID guids [20];
  PStringArray names;

} DirectSoundDevices;


class PSoundChannelDirectSound: public PSoundChannel
{
public:
    PSoundChannelDirectSound();
    void Construct();
    PSoundChannelDirectSound(const PString &device,
			     PSoundChannel::Directions dir,
			     unsigned numChannels,
			     unsigned sampleRate,
			     unsigned bitsPerSample);
    ~PSoundChannelDirectSound();
    static PStringArray GetDeviceNames(PSoundChannel::Directions);
    static DirectSoundDevices DevicesEnumerators (PSoundChannel::Directions);
    static PString GetDefaultDevice(PSoundChannel::Directions);
    BOOL Open(const PString & _device,
              Directions _dir,
              unsigned _numChannels,
              unsigned _sampleRate,
              unsigned _bitsPerSample);
    BOOL Setup();
    BOOL Close();
    BOOL Write(const void * buf, PINDEX len);
    BOOL Read(void * buf, PINDEX len);
    BOOL SetFormat(unsigned numChannels,
                   unsigned sampleRate,
                   unsigned bitsPerSample);
    unsigned GetChannels() const;
    unsigned GetSampleRate() const;
    unsigned GetSampleSize() const;
    BOOL SetBuffers(PINDEX size, PINDEX count);
    BOOL GetBuffers(PINDEX & size, PINDEX & count);
    BOOL PlaySound(const PSound & sound, BOOL wait);
    BOOL PlayFile(const PFilePath & filename, BOOL wait);
    BOOL HasPlayCompleted();
    BOOL WaitForPlayCompletion();
    BOOL RecordSound(PSound & sound);
    BOOL RecordFile(const PFilePath & filename);
    BOOL StartRecording();
    BOOL IsRecordBufferFull();
    BOOL AreAllRecordBuffersFull();
    BOOL WaitForRecordBufferFull();
    BOOL WaitForAllRecordBuffersFull();
    BOOL Abort();
    BOOL SetVolume (unsigned);
    BOOL GetVolume (unsigned &);
    BOOL IsOpen() const;

private:

    unsigned mNumChannels;
    unsigned mSampleRate;
    unsigned mBitsPerSample;
    
    BOOL isInitialised;
    BOOL isOpen;

    Directions mDirection;

    LPDIRECTSOUNDCAPTURE8 sAudioCaptureDevice;
    LPDIRECTSOUNDCAPTUREBUFFER8 mAudioCaptureBuffer;

    LPDIRECTSOUND8 sAudioPlaybackDevice;
    LPDIRECTSOUNDBUFFER8 mAudioPlaybackBuffer;
    LPDIRECTSOUNDBUFFER mAudioPrimaryPlaybackBuffer;
    
    BOOL InitPlaybackBuffer();
    BOOL InitPlaybackDevice(GUID *pGUID);
    
    BOOL InitCaptureBuffer();
    BOOL InitCaptureDevice(GUID *pGUID);
    
    BOOL GetDeviceID (PString deviceName, GUID *pGUID);

    PINDEX WriteToDXBuffer(const void * buf, PINDEX len);
    PINDEX ReadFromDXBuffer(const void * buf, PINDEX len);
    DWORD GetDXBufferFreeSpace ();
    void FlushBuffer ();
    
    BOOL SetFormat ();

    PINDEX mOutburst;
    BOOL mStreaming;
    PINDEX mBufferSize;
    PINDEX mDXBufferSize;
    PINDEX mBufferCount;

    PINDEX mDXOffset;
    PINDEX mVolume;

    WAVEFORMATEX mWFX;
    
    PMutex           bufferMutex;

    //static PDirectSoundDevices mDevices;
};

#endif // P_DIRECTSOUND

#endif  /* __DIRECTSOUND_H__ */
