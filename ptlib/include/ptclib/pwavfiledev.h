/*
 * pwavfiledev.cxx
 *
 * Sound file device declaration
 *
 * Portable Windows Library
 *
 * Copyright (C) 2007 Post Increment
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
 * Robert Jongbloed <robertj@postincrement.com>
 *
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: pwavfiledev.h,v $
 * Revision 1.2  2007/10/09 02:35:39  rjongbloed
 * Added ability to make WAV file audio record device auto repeat the WAV file
 *   when reach end of file, signaled by appending an '*' to end of filename.
 *
 * Revision 1.1  2007/04/13 07:03:20  rjongbloed
 * Added WAV file audio device "plug in".
 *
 */

#ifndef _PWAVFILEDEV
#define _PWAVFILEDEV

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>

#include <ptlib/sound.h>
#include <ptclib/pwavfile.h>
#include <ptclib/delaychan.h>

#if defined(P_WAVFILE)


///////////////////////////////////////////////////////////////////////////////////////////
//
// This class defines a sound channel device that reads audio from a raw WAV file
//

class PSoundChannel_WAVFile : public PSoundChannel
{
 PCLASSINFO(PSoundChannel_WAVFile, PSoundChannel);
 public:
    PSoundChannel_WAVFile();
    PSoundChannel_WAVFile(const PString &device,
                     PSoundChannel::Directions dir,
                     unsigned numChannels,
                     unsigned sampleRate,
                     unsigned bitsPerSample);
    ~PSoundChannel_WAVFile();
    static PStringArray GetDeviceNames(PSoundChannel::Directions = Player);
    BOOL Open(const PString & _device,
              Directions _dir,
              unsigned _numChannels,
              unsigned _sampleRate,
              unsigned _bitsPerSample);
    virtual PString GetName() const;
    BOOL Close();
    BOOL IsOpen() const;
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
    BOOL HasPlayCompleted();
    BOOL WaitForPlayCompletion();
    BOOL StartRecording();
    BOOL IsRecordBufferFull();
    BOOL AreAllRecordBuffersFull();
    BOOL WaitForRecordBufferFull();
    BOOL WaitForAllRecordBuffersFull();

protected:
    PWAVFile       m_WAVFile;
    PAdaptiveDelay m_Pacing;
    bool           m_autoRepeat;
};


#endif // defined(P_WAVFILE)

#endif // _PWAVFILEDEV

