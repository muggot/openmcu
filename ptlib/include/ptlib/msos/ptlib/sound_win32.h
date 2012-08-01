/*
 * sound.h
 *
 * Sound class.
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
 * $Log: sound_win32.h,v $
 * Revision 1.5  2005/11/30 12:47:38  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.4  2005/09/18 13:01:43  dominance
 * fixed pragma warnings when building with gcc.
 *
 * Revision 1.3  2004/10/23 11:32:27  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.2  2003/12/29 03:28:56  csoutheren
 * Allowed access to Windows sound channel declaration, just in case it is required
 *
 * Revision 1.1  2003/12/29 01:59:48  csoutheren
 * Initial version
 *
 */


///////////////////////////////////////////////////////////////////////////////
// PSound

#ifndef _PSOUND_WIN32
#define _PSOUND_WIN32

#include <mmsystem.h>
#ifndef _WIN32_WCE
#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif
#endif

class PWaveFormat : public PObject
{
  PCLASSINFO(PWaveFormat, PObject)
  public:
    PWaveFormat();
    ~PWaveFormat();
    PWaveFormat(const PWaveFormat & fmt);
    PWaveFormat & operator=(const PWaveFormat & fmt);

    void PrintOn(ostream &) const;
    void ReadFrom(istream &);

    void SetFormat(unsigned numChannels, unsigned sampleRate, unsigned bitsPerSample);
    void SetFormat(const void * data, PINDEX size);

    BOOL           SetSize   (PINDEX sz);
    PINDEX         GetSize   () const { return  size;       }
    void         * GetPointer() const { return  waveFormat; }
    WAVEFORMATEX * operator->() const { return  waveFormat; }
    WAVEFORMATEX & operator *() const { return *waveFormat; }
    operator   WAVEFORMATEX *() const { return  waveFormat; }

  protected:
    PINDEX         size;
    WAVEFORMATEX * waveFormat;
};

class PWaveBuffer : public PBYTEArray
{
  PCLASSINFO(PWaveBuffer, PBYTEArray);
  private:
    PWaveBuffer(PINDEX sz = 0);
    ~PWaveBuffer();

    PWaveBuffer & operator=(const PSound & sound);

    DWORD Prepare(HWAVEOUT hWaveOut, PINDEX & count);
    DWORD Prepare(HWAVEIN hWaveIn);
    DWORD Release();

    void PrepareCommon(PINDEX count);

    HWAVEOUT hWaveOut;
    HWAVEIN  hWaveIn;
    WAVEHDR  header;

  friend class PSoundChannelWin32;
};

PARRAY(PWaveBufferArray, PWaveBuffer);

class PSoundChannelWin32: public PSoundChannel
{
 public:
    PSoundChannelWin32();
    void Construct();
    PSoundChannelWin32(const PString &device,
                     PSoundChannel::Directions dir,
                     unsigned numChannels,
                     unsigned sampleRate,
                     unsigned bitsPerSample);
    ~PSoundChannelWin32();
    static PStringArray GetDeviceNames(PSoundChannel::Directions = Player);
    static PString GetDefaultDevice(PSoundChannel::Directions);
    BOOL Open(const PString & _device,
              Directions _dir,
              unsigned _numChannels,
              unsigned _sampleRate,
              unsigned _bitsPerSample);
    BOOL Setup();
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
    BOOL SetVolume(unsigned newVal);
    BOOL GetVolume(unsigned &devVol);

  public:
    // Overrides from class PChannel
    virtual PString GetName() const;
      // Return the name of the channel.

      
    PString GetErrorText(ErrorGroup group = NumErrorGroups) const;
    // Get a text form of the last error encountered.

    BOOL SetFormat(const PWaveFormat & format);

    BOOL Open(const PString & device, Directions dir,const PWaveFormat & format);
    // Open with format other than PCM

  protected:
    PString     deviceName;
    Directions  direction;
    HWAVEIN     hWaveIn;
    HWAVEOUT    hWaveOut;
    HANDLE      hEventDone;
    PWaveFormat waveFormat;

    PWaveBufferArray buffers;
    PINDEX           bufferIndex;
    PINDEX           bufferByteOffset;
    PMutex           bufferMutex;

  private:
    BOOL OpenDevice(unsigned id);
    BOOL GetDeviceID(const PString & device, Directions dir, unsigned& id);
};


#endif

// End Of File ///////////////////////////////////////////////////////////////
