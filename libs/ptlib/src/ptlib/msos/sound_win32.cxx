/*
 * sound.cxx
 *
 * Implementation of sound classes for Win32
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
 * $Log: sound_win32.cxx,v $
 *
 *
 * Revision 1.22  2013/01/25 zaozerskiy (zaozerskiy@mail.ru)
 * Добавлено управление микрофоном для ОС Windows SetVolume и GetVolume
 *
 * Revision 1.21  2007/10/03 01:18:47  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.20  2007/04/08 01:53:37  ykiryanov
 * Build to support ptlib dll creation
 *
 * Revision 1.19  2007/04/06 21:06:57  ykiryanov
 * changed directory name  for wince to wm
 *
 * Revision 1.18  2006/07/31 12:08:28  rjongbloed
 * Fixed problem with WAV file asynchronous play back
 *
 * Revision 1.17  2006/06/21 04:20:07  csoutheren
 * Fixes for VS.net
 *
 * Revision 1.16  2006/04/09 11:03:59  csoutheren
 * Remove warnings on VS.net 2005
 *
 * Revision 1.15  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.14  2005/10/06 08:14:55  csoutheren
 * Fixed race condition in sound driver when shutting down with driver that is not open
 *
 * Revision 1.13  2005/09/18 13:01:43  dominance
 * fixed pragma warnings when building with gcc.
 *
 * Revision 1.12  2005/07/03 13:48:58  shorne
 * Add the ability to play sound to specified device.
 *
 * Revision 1.11  2005/04/21 05:27:04  csoutheren
 * Prevent weird deadlocks when using record-only or play-only sound channels
 *
 * Revision 1.10  2005/01/04 07:44:04  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.9  2004/10/23 11:16:17  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.8  2004/08/16 06:41:00  csoutheren
 * Added adapters template to make device plugins available via the abstract factory interface
 *
 * Revision 1.7  2004/04/09 06:52:18  rjongbloed
 * Removed #pargma linker command for /delayload of DLL as documentations sais that
 *   you cannot do this.
 *
 * Revision 1.6  2004/02/23 23:52:20  csoutheren
 * Added pragmas to avoid every Windows application needing to include libs explicitly
 *
 * Revision 1.5  2004/02/15 03:59:20  rjongbloed
 * Fixed the default number of buffer to be the value determined emprirically in
 *    OpenH323, thanks Ted Szoczei
 *
 * Revision 1.4  2003/12/29 03:29:26  csoutheren
 * Allowed access to Windows sound channel declaration, just in case it is required
 *
 * Revision 1.3  2003/12/29 02:00:40  csoutheren
 * Moved some declarations to sound_win32.h to allow access
 *
 * Revision 1.2  2003/11/18 10:50:44  csoutheren
 * Changed name of Windows sound device
 *
 * Revision 1.1  2003/11/12 04:39:56  csoutheren
 * Changed to work with new plugin system
 *
 * Revision 1.37  2003/11/05 05:57:58  csoutheren
 * Added #pragma to include required libs
 *
 * Revision 1.36  2003/09/17 05:45:10  csoutheren
 * Removed recursive includes
 *
 * Revision 1.35  2003/06/05 05:20:35  rjongbloed
 * Fixed WinCE compatibility, thanks Yuri Kiryanov
 *
 * Revision 1.34  2003/05/29 08:57:38  rjongbloed
 * Futher changes to not alter balance when changing volume setting, also fixed
 *   correct return of volume level if balance not centred, thanks Diego Tбrtara
 *
 * Revision 1.33  2003/05/01 00:17:40  robertj
 * Fixed setting of stereo volume levels, thanks Diego Tбrtara
 *
 * Revision 1.32  2002/08/05 01:22:59  robertj
 * Fixed possible range error on SetVolume(), thanks Sonya Cooper-Hull
 *
 * Revision 1.31  2002/02/08 09:59:45  robertj
 * Slight adjustment to API and documentation for volume functions.
 * Added implementation for volume function on play, still needs recording.
 *
 * Revision 1.30  2002/02/07 20:57:21  dereks
 * add SetVolume and GetVolume methods to PSoundChannel
 *
 * Revision 1.29  2001/10/23 02:49:48  robertj
 * Fixed problem with Abort() not always breaking I/O blocked threads.
 *
 * Revision 1.28  2001/10/12 03:50:27  robertj
 * Fixed race condition on using Abort() which Reading from another thread.
 * Fixed failure to start recording if called WaitForXXXFull() functions.
 *
 * Revision 1.27  2001/10/10 03:29:34  yurik
 * Added open with format other than PCM
 *
 * Revision 1.26  2001/09/22 03:36:56  yurik
 * Put code to prevent audio channel disconnection
 *
 * Revision 1.25  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.24  2001/09/10 02:48:51  robertj
 * Removed previous change as breaks semantics of Read() function, moved test
 *   for zero buffer length to part that waits for buffer to be full.
 *
 * Revision 1.23  2001/09/09 17:37:49  yurik
 * dwBytesRecorded in WAVEHDR could return 0. We should not close the channel in this case
 *
 * Revision 1.22  2001/09/09 02:17:11  yurik
 * Returned to 1.20
 *
 * Revision 1.20  2001/07/01 02:45:01  yurik
 * WinCE compiler wants implicit cast to format
 *
 * Revision 1.19  2001/05/04 09:38:07  robertj
 * Fixed problem with some WAV files having small WAVEFORMATEX chunk.
 *
 * Revision 1.18  2001/04/10 00:51:11  robertj
 * Fixed bug in using incorrect function to delete event handle, thanks Victor H.
 *
 * Revision 1.17  2001/03/15 23:39:29  robertj
 * Fixed bug with trying to write block larger than one buffer, thanks Norbert Oertel
 *
 * Revision 1.16  2001/02/07 04:45:54  robertj
 * Added functions to get current sound channel format parameters.
 *
 * Revision 1.15  2000/07/04 04:30:47  robertj
 * Fixed shutdown issues with buffers in use, again.
 *
 * Revision 1.14  2000/07/01 09:39:31  robertj
 * Fixed shutdown issues with buffers in use.
 *
 * Revision 1.13  2000/06/29 00:39:29  robertj
 * Fixed bug when PWaveFormat is assigned to itself.
 *
 * Revision 1.12  2000/05/22 07:17:50  robertj
 * Fixed missing initialisation of format data block in Win32 PSound::Load().
 *
 * Revision 1.11  2000/05/01 05:59:11  robertj
 * Added mutex to PSoundChannel buffer structure.
 *
 * Revision 1.10  2000/03/04 10:15:32  robertj
 * Added simple play functions for sound files.
 *
 * Revision 1.9  2000/02/17 11:33:33  robertj
 * Changed PSoundChannel::Write so blocks instead of error if no buffers available.
 *
 * Revision 1.8  1999/10/09 01:22:07  robertj
 * Fixed error display for sound channels.
 *
 * Revision 1.7  1999/09/23 04:28:44  robertj
 * Allowed some Win32 only access to wave format in sound channel
 *
 * Revision 1.6  1999/07/08 08:39:53  robertj
 * Fixed bug when breaking block by closing the PSoundChannel in other thread.
 *
 * Revision 1.5  1999/06/24 14:01:25  robertj
 * Fixed bug in not returning correct default recorder (waveIn) device.
 *
 * Revision 1.4  1999/06/07 01:36:28  robertj
 * Fixed incorrect;ly set block alignment in sound structure.
 *
 * Revision 1.3  1999/05/28 14:04:51  robertj
 * Added function to get default audio device.
 *
 * Revision 1.2  1999/02/22 10:15:15  robertj
 * Sound driver interface implementation to Linux OSS specification.
 *
 * Revision 1.1  1999/02/16 06:02:07  robertj
 * Major implementation to Linux OSS model
 */

#define P_FORCE_STATIC_PLUGIN

#include <ptlib.h>
#include <ptlib/sound.h>

#if defined(_WIN32) && !defined(P_FORCE_STATIC_PLUGIN)
#error "sound_win32.cxx must be compiled without precompiled headers"
#endif

#include <ptlib/plugin.h>
#include <ptlib/msos/ptlib/sound_win32.h>

#ifndef _WIN32_WCE
#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif
#else
#include <ptlib/wm/mmsystemx.h>
#endif

class PSound;

PCREATE_SOUND_PLUGIN(WindowsMultimedia, PSoundChannelWin32);

class PMultiMediaFile
{
  public:
    PMultiMediaFile();
    ~PMultiMediaFile();

    BOOL CreateWaveFile(const PFilePath & filename,
                        const PWaveFormat & waveFormat,
                        DWORD dataSize);
    BOOL OpenWaveFile(const PFilePath & filename,
                      PWaveFormat & waveFormat,
                      DWORD & dataSize);

    BOOL Open(const PFilePath & filename, DWORD dwOpenFlags, LPMMIOINFO lpmmioinfo = NULL);
    BOOL Close(UINT wFlags = 0);
    BOOL Ascend(MMCKINFO & ckinfo, UINT wFlags = 0);
    BOOL Descend(UINT wFlags, MMCKINFO & ckinfo, LPMMCKINFO lpckParent = NULL);
    BOOL Read(void * data, PINDEX len);
    BOOL CreateChunk(MMCKINFO & ckinfo, UINT wFlags = 0);
    BOOL Write(const void * data, PINDEX len);

    DWORD GetLastError() const { return dwLastError; }

  protected:
    HMMIO hmmio;
    DWORD dwLastError;
};


#define new PNEW


///////////////////////////////////////////////////////////////////////////////

PMultiMediaFile::PMultiMediaFile()
{
  hmmio = NULL;
}


PMultiMediaFile::~PMultiMediaFile()
{
  Close();
}


BOOL PMultiMediaFile::CreateWaveFile(const PFilePath & filename,
                                     const PWaveFormat & waveFormat,
                                     DWORD dataSize)
{
  if (!Open(filename, MMIO_CREATE|MMIO_WRITE))
    return FALSE;

  MMCKINFO mmChunk;
  mmChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
  mmChunk.cksize = 4 + // Form type
                   4 + sizeof(DWORD) + waveFormat.GetSize() + // fmt chunk
                   4 + sizeof(DWORD) + dataSize;              // data chunk

  // Create a RIFF chunk
  if (!CreateChunk(mmChunk, MMIO_CREATERIFF))
    return FALSE;

  // Save the format sub-chunk
  mmChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
  mmChunk.cksize = waveFormat.GetSize();
  if (!CreateChunk(mmChunk))
    return FALSE;

  if (!Write(waveFormat, waveFormat.GetSize()))
    return FALSE;

  // Save the data sub-chunk
  mmChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
  mmChunk.cksize = dataSize;
  return CreateChunk(mmChunk);
}


BOOL PMultiMediaFile::OpenWaveFile(const PFilePath & filename,
                                   PWaveFormat  & waveFormat,
                                   DWORD & dataSize)
{
  // Open wave file
  if (!Open(filename, MMIO_READ | MMIO_ALLOCBUF))
    return FALSE;

  MMCKINFO mmParentChunk, mmSubChunk;
  dwLastError = MMSYSERR_NOERROR;

  // Locate a 'RIFF' chunk with a 'WAVE' form type
  mmParentChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
  if (!Descend(MMIO_FINDRIFF, mmParentChunk))
    return FALSE;

  // Find the format chunk
  mmSubChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
  if (!Descend(MMIO_FINDCHUNK, mmSubChunk, &mmParentChunk))
    return FALSE;

  // Get the size of the format chunk, allocate memory for it
  if (!waveFormat.SetSize(mmSubChunk.cksize))
    return FALSE;

  // Read the format chunk
  if (!Read(waveFormat.GetPointer(), waveFormat.GetSize()))
    return FALSE;

  // Ascend out of the format subchunk
  Ascend(mmSubChunk);

  // Find the data subchunk
  mmSubChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
  if (!Descend(MMIO_FINDCHUNK, mmSubChunk, &mmParentChunk))
    return FALSE;

  // Get the size of the data subchunk
  if (mmSubChunk.cksize == 0) {
    dwLastError = MMSYSERR_INVALPARAM;
    return FALSE;
  }

  dataSize = mmSubChunk.cksize;
  return TRUE;
}


BOOL PMultiMediaFile::Open(const PFilePath & filename,
                          DWORD dwOpenFlags,
                          LPMMIOINFO lpmmioinfo)
{
  MMIOINFO local_mmioinfo;
  if (lpmmioinfo == NULL) {
    lpmmioinfo = &local_mmioinfo;
    memset(lpmmioinfo, 0, sizeof(local_mmioinfo));
  }

  hmmio = mmioOpen((char *)(const char *)filename, lpmmioinfo, dwOpenFlags);

  dwLastError = lpmmioinfo->wErrorRet;

  return hmmio != NULL;
}


BOOL PMultiMediaFile::Close(UINT wFlags)
{
  if (hmmio == NULL)
    return FALSE;

  mmioClose(hmmio, wFlags);
  hmmio = NULL;
  return TRUE;
}


BOOL PMultiMediaFile::Ascend(MMCKINFO & ckinfo, UINT wFlags)
{
  dwLastError = mmioAscend(hmmio, &ckinfo, wFlags);
  return dwLastError == MMSYSERR_NOERROR;
}


BOOL PMultiMediaFile::Descend(UINT wFlags, MMCKINFO & ckinfo, LPMMCKINFO lpckParent)
{
  dwLastError = mmioDescend(hmmio, &ckinfo, lpckParent, wFlags);
  return dwLastError == MMSYSERR_NOERROR;
}


BOOL PMultiMediaFile::Read(void * data, PINDEX len)
{
  return mmioRead(hmmio, (char *)data, len) == len;
}


BOOL PMultiMediaFile::CreateChunk(MMCKINFO & ckinfo, UINT wFlags)
{
  dwLastError = mmioCreateChunk(hmmio, &ckinfo, wFlags);
  return dwLastError == MMSYSERR_NOERROR;
}


BOOL PMultiMediaFile::Write(const void * data, PINDEX len)
{
  return mmioWrite(hmmio, (char *)data, len) == len;
}


///////////////////////////////////////////////////////////////////////////////

PWaveFormat::PWaveFormat()
{
  size = 0;
  waveFormat = NULL;
}


PWaveFormat::~PWaveFormat()
{
  if (waveFormat != NULL)
    free(waveFormat);
}


PWaveFormat::PWaveFormat(const PWaveFormat & fmt)
{
  size = fmt.size;
  waveFormat = (WAVEFORMATEX *)malloc(size);
  PAssert(waveFormat != NULL, POutOfMemory);

  memcpy(waveFormat, fmt.waveFormat, size);
}


PWaveFormat & PWaveFormat::operator=(const PWaveFormat & fmt)
{
  if (this == &fmt)
    return *this;

  if (waveFormat != NULL)
    free(waveFormat);

  size = fmt.size;
  waveFormat = (WAVEFORMATEX *)malloc(size);
  PAssert(waveFormat != NULL, POutOfMemory);

  memcpy(waveFormat, fmt.waveFormat, size);
  return *this;
}


void PWaveFormat::PrintOn(ostream & out) const
{
  if (waveFormat == NULL)
    out << "<null>";
  else {
    out << waveFormat->wFormatTag << ','
        << waveFormat->nChannels << ','
        << waveFormat->nSamplesPerSec << ','
        << waveFormat->nAvgBytesPerSec << ','
        << waveFormat->nBlockAlign << ','
        << waveFormat->wBitsPerSample;
    if (waveFormat->cbSize > 0) {
      out << hex << setfill('0');
      const BYTE * ptr = (const BYTE *)&waveFormat[1];
      for (PINDEX i = 0; i < waveFormat->cbSize; i++)
        out << ',' << setw(2) << (unsigned)*ptr++;
      out << dec << setfill(' ');
    }
  }
}


void PWaveFormat::ReadFrom(istream &)
{
}


void PWaveFormat::SetFormat(unsigned numChannels,
                            unsigned sampleRate,
                            unsigned bitsPerSample)
{
  PAssert(numChannels == 1 || numChannels == 2, PInvalidParameter);
  PAssert(bitsPerSample == 8 || bitsPerSample == 16, PInvalidParameter);

  if (waveFormat != NULL)
    free(waveFormat);

  size = sizeof(WAVEFORMATEX);
  waveFormat = (WAVEFORMATEX *)malloc(sizeof(WAVEFORMATEX));
  PAssert(waveFormat != NULL, POutOfMemory);

  waveFormat->wFormatTag = WAVE_FORMAT_PCM;
  waveFormat->nChannels = (WORD)numChannels;
  waveFormat->nSamplesPerSec = sampleRate;
  waveFormat->wBitsPerSample = (WORD)bitsPerSample;
  waveFormat->nBlockAlign = (WORD)(numChannels*(bitsPerSample+7)/8);
  waveFormat->nAvgBytesPerSec = waveFormat->nSamplesPerSec*waveFormat->nBlockAlign;
  waveFormat->cbSize = 0;
}


void PWaveFormat::SetFormat(const void * data, PINDEX size)
{
  SetSize(size);
  memcpy(waveFormat, data, size);
}


BOOL PWaveFormat::SetSize(PINDEX sz)
{
  if (waveFormat != NULL)
    free(waveFormat);

  size = sz;
  if (sz == 0)
    waveFormat = NULL;
  else {
    if (sz < sizeof(WAVEFORMATEX))
      sz = sizeof(WAVEFORMATEX);
    waveFormat = (WAVEFORMATEX *)calloc(sz, 1);
    waveFormat->cbSize = (WORD)(sz - sizeof(WAVEFORMATEX));
  }

  return waveFormat != NULL;
}


///////////////////////////////////////////////////////////////////////////////

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


BOOL PSound::Load(const PFilePath & filename)
{
  // Open wave file
  PMultiMediaFile mmio;
  PWaveFormat waveFormat;
  DWORD dataSize;
  if (!mmio.OpenWaveFile(filename, waveFormat, dataSize)) {
    dwLastError = mmio.GetLastError();
    return FALSE;
  }

  encoding = waveFormat->wFormatTag;
  numChannels = waveFormat->nChannels;
  sampleRate = waveFormat->nSamplesPerSec;
  sampleSize = waveFormat->wBitsPerSample;

  if (encoding != 0) {
    PINDEX formatSize = waveFormat->cbSize + sizeof(WAVEFORMATEX);
    memcpy(formatInfo.GetPointer(formatSize), waveFormat, formatSize);
  }

  // Allocate and lock memory for the waveform data.
  if (!SetSize(dataSize)) {
    dwLastError = MMSYSERR_NOMEM;
    return FALSE;
  }

  // Read the waveform data subchunk
  if (!mmio.Read(GetPointer(), GetSize())) {
    dwLastError = mmio.GetLastError();
    return FALSE;
  }

  return TRUE;
}


BOOL PSound::Save(const PFilePath & filename)
{
  PWaveFormat waveFormat;
  if (encoding == 0)
    waveFormat.SetFormat(numChannels, sampleRate, sampleSize);
  else {
    waveFormat.SetSize(GetFormatInfoSize());
    memcpy(waveFormat.GetPointer(), GetFormatInfoData(), GetFormatInfoSize());
  }

  // Open wave file
  PMultiMediaFile mmio;
  if (!mmio.CreateWaveFile(filename, waveFormat, GetSize())) {
    dwLastError = mmio.GetLastError();
    return FALSE;
  }

  if (!mmio.Write(GetPointer(), GetSize())) {
    dwLastError = mmio.GetLastError();
    return FALSE;
  }

  return TRUE;
}


BOOL PSound::Play()
{
  PSoundChannel channel(PSoundChannel::GetDefaultDevice(PSoundChannel::Player),
                        PSoundChannel::Player);
  if (!channel.IsOpen())
    return FALSE;

  return channel.PlaySound(*this, TRUE);
}

BOOL PSound::Play(const PString & device)
{

  PSoundChannel channel(device,
                       PSoundChannel::Player);
  if (!channel.IsOpen())
    return FALSE;

  return channel.PlaySound(*this, TRUE);
}

BOOL PSound::PlayFile(const PFilePath & file, BOOL wait)
{
#ifndef _WIN32_WCE
  return ::PlaySound(file, NULL, SND_FILENAME|(wait ? SND_SYNC : SND_ASYNC));
#else
  return ::PlaySound((LPCWSTR) file.AsUCS2(), NULL, SND_FILENAME|(wait ? SND_SYNC : SND_ASYNC));
#endif
}


///////////////////////////////////////////////////////////////////////////////

PWaveBuffer::PWaveBuffer(PINDEX sz)
 : PBYTEArray(sz)
{
  hWaveOut = NULL;
  hWaveIn = NULL;
  header.dwFlags = WHDR_DONE;
}


PWaveBuffer::~PWaveBuffer()
{
  Release();
}


PWaveBuffer & PWaveBuffer::operator=(const PSound & sound)
{
  PBYTEArray::operator=(sound);
  return *this;
}


void PWaveBuffer::PrepareCommon(PINDEX count)
{
  Release();

  memset(&header, 0, sizeof(header));
  header.lpData = (char *)GetPointer();
  header.dwBufferLength = count;
  header.dwUser = (DWORD)this;
}


DWORD PWaveBuffer::Prepare(HWAVEOUT hOut, PINDEX & count)
{
  // Set up WAVEHDR structure and prepare it to be written to wave device
  if (count > GetSize())
    count = GetSize();

  PrepareCommon(count);
  hWaveOut = hOut;
  return waveOutPrepareHeader(hWaveOut, &header, sizeof(header));
}


DWORD PWaveBuffer::Prepare(HWAVEIN hIn)
{
  // Set up WAVEHDR structure and prepare it to be read from wave device
  PrepareCommon(GetSize());
  hWaveIn = hIn;
  return waveInPrepareHeader(hWaveIn, &header, sizeof(header));
}


DWORD PWaveBuffer::Release()
{
  DWORD err = MMSYSERR_NOERROR;

  // There seems to be some pathalogical cases where on an Abort() call the buffers
  // still are "in use", even though waveOutReset() was called. So wait until the
  // sound driver has finished with the buffer before releasing it.

  if (hWaveOut != NULL) {
    if ((err = waveOutUnprepareHeader(hWaveOut, &header, sizeof(header))) == WAVERR_STILLPLAYING)
      return err;
    hWaveOut = NULL;
  }

  if (hWaveIn != NULL) {
    if ((err = waveInUnprepareHeader(hWaveIn, &header, sizeof(header))) == WAVERR_STILLPLAYING)
      return err;
    hWaveIn = NULL;
  }

  header.dwFlags |= WHDR_DONE;
  return err;
}


///////////////////////////////////////////////////////////////////////////////





PSoundChannelWin32::PSoundChannelWin32()
{
  Construct();
}


PSoundChannelWin32::PSoundChannelWin32(const PString & device,
                             Directions dir,
                             unsigned numChannels,
                             unsigned sampleRate,
                             unsigned bitsPerSample)
{
  Construct();
  Open(device, dir, numChannels, sampleRate, bitsPerSample);
}


void PSoundChannelWin32::Construct()
{
  direction = Player;
  hWaveOut = NULL;
  hWaveIn = NULL;
  hEventDone = CreateEvent(NULL, FALSE, FALSE, NULL);

  waveFormat.SetFormat(1, 8000, 16);

  bufferByteOffset = P_MAX_INDEX;

  SetBuffers(32768, 3);
}


PSoundChannelWin32::~PSoundChannelWin32()
{
  Close();

  if (hEventDone != NULL)
    CloseHandle(hEventDone);
}


PString PSoundChannelWin32::GetName() const
{
  return deviceName;
}


PStringArray PSoundChannelWin32::GetDeviceNames(Directions dir)
{
  PStringArray array;

  unsigned numDevs, id;

  switch (dir) {
    case Player :
      numDevs = waveOutGetNumDevs();
      for (id = 0; id < numDevs; id++) {
        WAVEOUTCAPS caps;
        if (waveOutGetDevCaps(id, &caps, sizeof(caps)) == 0)
          array[array.GetSize()] = caps.szPname;
      }
      break;

    case Recorder :
      numDevs = waveInGetNumDevs();
      for (id = 0; id < numDevs; id++) {
        WAVEINCAPS caps;
        if (waveInGetDevCaps(id, &caps, sizeof(caps)) == 0)
          array[array.GetSize()] = caps.szPname;
      }
      break;
  }

  return array;
}


PString PSoundChannelWin32::GetDefaultDevice(Directions dir)
{
  RegistryKey registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Multimedia\\Sound Mapper",
                       RegistryKey::ReadOnly);

  PString str;

  if (dir == Player) {
    if (!registry.QueryValue("Playback", str)) {
      WAVEOUTCAPS caps;
      if (waveOutGetDevCaps(0, &caps, sizeof(caps)) == 0)
        str = caps.szPname;
    }
  }
  else {
    if (!registry.QueryValue("Record", str)) {
      WAVEINCAPS caps;
      if (waveInGetDevCaps(0, &caps, sizeof(caps)) == 0)
        str = caps.szPname;
    }
  }

  return str;
}

BOOL PSoundChannelWin32::GetDeviceID(const PString & device, Directions dir, unsigned& id)
{
  BOOL bad = TRUE;

  if (device[0] == '#') {
    id = device.Mid(1).AsUnsigned();
    switch (dir) {
      case Player :
        if (id < waveOutGetNumDevs()) {
          WAVEOUTCAPS caps;
          if (waveOutGetDevCaps(id, &caps, sizeof(caps)) == 0) {
            deviceName = caps.szPname;
            bad = FALSE;
          }
        }
        break;

      case Recorder :
        if (id < waveInGetNumDevs()) {
          WAVEINCAPS caps;
          if (waveInGetDevCaps(id, &caps, sizeof(caps)) == 0) {
            deviceName = caps.szPname;
            bad = FALSE;
          }
        }
        break;
    }
  }
  else {
    switch (dir) {
      case Player :
        for (id = 0; id < waveOutGetNumDevs(); id++) {
          WAVEOUTCAPS caps;
          if (waveOutGetDevCaps(id, &caps, sizeof(caps)) == 0 &&
              strcasecmp(caps.szPname, device) == 0) {
            deviceName = caps.szPname;
            bad = FALSE;
            break;
          }
        }
        break;

      case Recorder :
        for (id = 0; id < waveInGetNumDevs(); id++) {
          WAVEINCAPS caps;
          if (waveInGetDevCaps(id, &caps, sizeof(caps)) == 0 &&
              strcasecmp(caps.szPname, device) == 0) {
            deviceName = caps.szPname;
            bad = FALSE;
            break;
          }
        }
        break;
    }
  }

  if (bad)
    return SetErrorValues(NotFound, MMSYSERR_BADDEVICEID|PWIN32ErrorFlag);

  return TRUE;
}

BOOL PSoundChannelWin32::Open(const PString & device,
                         Directions dir,
                         unsigned numChannels,
                         unsigned sampleRate,
                         unsigned bitsPerSample)
{
  Close();
  unsigned id = 0;

  if( !GetDeviceID(device, dir, id) )
    return FALSE;

  waveFormat.SetFormat(numChannels, sampleRate, bitsPerSample);

  direction = dir;
  return OpenDevice(id);
}

BOOL PSoundChannelWin32::Open(const PString & device,
                         Directions dir,
             const PWaveFormat& format)
{
  Close();
  unsigned id = 0;

  if( !GetDeviceID(device, dir, id) )
    return FALSE;

  waveFormat = format;

  direction = dir;
  return OpenDevice(id);
}

BOOL PSoundChannelWin32::OpenDevice(unsigned id)
{
  Close();

  PWaitAndSignal mutex(bufferMutex);

  bufferByteOffset = P_MAX_INDEX;
  bufferIndex = 0;

  WAVEFORMATEX* format = (WAVEFORMATEX*) waveFormat;

  DWORD osError = MMSYSERR_BADDEVICEID;
  switch (direction) {
    case Player :
      osError = waveOutOpen(&hWaveOut, id, format,
                            (DWORD)hEventDone, 0, CALLBACK_EVENT);
      break;

    case Recorder :
      osError = waveInOpen(&hWaveIn, id, format,
                           (DWORD)hEventDone, 0, CALLBACK_EVENT);
      break;
  }

  if (osError != MMSYSERR_NOERROR)
    return SetErrorValues(NotFound, osError|PWIN32ErrorFlag);

  os_handle = id;
  return TRUE;
}

BOOL PSoundChannelWin32::IsOpen() const
{ 
  return os_handle >= 0;
}

BOOL PSoundChannelWin32::SetFormat(unsigned numChannels,
                              unsigned sampleRate,
                              unsigned bitsPerSample)
{
  Abort();

  waveFormat.SetFormat(numChannels, sampleRate, bitsPerSample);

  return OpenDevice(os_handle);
}


BOOL PSoundChannelWin32::SetFormat(const PWaveFormat & format)
{
  Abort();

  waveFormat = format;

  return OpenDevice(os_handle);
}


unsigned PSoundChannelWin32::GetChannels() const
{
  return waveFormat->nChannels;
}


unsigned PSoundChannelWin32::GetSampleRate() const
{
  return waveFormat->nSamplesPerSec;
}


unsigned PSoundChannelWin32::GetSampleSize() const
{
  return waveFormat->wBitsPerSample;
}


BOOL PSoundChannelWin32::Close()
{
  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF);

  Abort();

  if (hWaveOut != NULL) {
    while (waveOutClose(hWaveOut) == WAVERR_STILLPLAYING)
      waveOutReset(hWaveOut);
    hWaveOut = NULL;
  }

  if (hWaveIn != NULL) {
    while (waveInClose(hWaveIn) == WAVERR_STILLPLAYING)
      waveInReset(hWaveIn);
    hWaveIn = NULL;
  }

  Abort();

  os_handle = -1;
  return TRUE;
}


BOOL PSoundChannelWin32::SetBuffers(PINDEX size, PINDEX count)
{
  Abort();

  PAssert(size > 0 && count > 0, PInvalidParameter);

  BOOL ok = TRUE;

  PWaitAndSignal mutex(bufferMutex);

  if (!buffers.SetSize(count))
    ok = FALSE;
  else {
    for (PINDEX i = 0; i < count; i++) {
      if (buffers.GetAt(i) == NULL)
        buffers.SetAt(i, new PWaveBuffer(size));
      if (!buffers[i].SetSize(size))
        ok = FALSE;
    }
  }

  bufferByteOffset = P_MAX_INDEX;
  bufferIndex = 0;

  return ok;
}


BOOL PSoundChannelWin32::GetBuffers(PINDEX & size, PINDEX & count)
{
  PWaitAndSignal mutex(bufferMutex);

  count = buffers.GetSize();

  if (count == 0)
    size = 0;
  else
    size = buffers[0].GetSize();

  return TRUE;
}


BOOL PSoundChannelWin32::Write(const void * data, PINDEX size)
{
  lastWriteCount = 0;

  if (hWaveOut == NULL)
    return SetErrorValues(NotOpen, EBADF, LastWriteError);

  const BYTE * ptr = (const BYTE *)data;

  bufferMutex.Wait();

  DWORD osError = MMSYSERR_NOERROR;
  while (size > 0) {
    PWaveBuffer & buffer = buffers[bufferIndex];
    while ((buffer.header.dwFlags&WHDR_DONE) == 0) {
      bufferMutex.Signal();
      // No free buffers, so wait for one
      if (WaitForSingleObject(hEventDone, INFINITE) != WAIT_OBJECT_0)
        return SetErrorValues(Miscellaneous, ::GetLastError()|PWIN32ErrorFlag, LastWriteError);
      bufferMutex.Wait();
    }

    // Can't write more than a buffer full
    PINDEX count = size;
    if ((osError = buffer.Prepare(hWaveOut, count)) != MMSYSERR_NOERROR)
      break;

    memcpy(buffer.GetPointer(), ptr, count);

    if ((osError = waveOutWrite(hWaveOut, &buffer.header, sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
      break;

    bufferIndex = (bufferIndex+1)%buffers.GetSize();
    lastWriteCount += count;
    size -= count;
    ptr += count;
  }

  bufferMutex.Signal();

  if (size != 0)
    return SetErrorValues(Miscellaneous, osError|PWIN32ErrorFlag, LastWriteError);

  return TRUE;
}


BOOL PSoundChannelWin32::PlaySound(const PSound & sound, BOOL wait)
{
  Abort();

  BOOL ok = FALSE;

  PINDEX bufferSize;
  PINDEX bufferCount;
  GetBuffers(bufferSize, bufferCount);

  unsigned numChannels = waveFormat->nChannels;
  unsigned sampleRate = waveFormat->nSamplesPerSec;
  unsigned bitsPerSample = waveFormat->wBitsPerSample;
  if (sound.GetEncoding() == 0)
    ok = SetFormat(sound.GetChannels(), sound.GetSampleRate(), sound.GetSampleSize());
  else {
    waveFormat.SetFormat(sound.GetFormatInfoData(), sound.GetFormatInfoSize());
    ok = OpenDevice(os_handle);
  }

  if (ok) {
    bufferMutex.Wait();

    // To avoid lots of copying of sound data, we fake the PSound buffer into
    // the internal buffers and play directly from the PSound object.
    buffers.SetSize(1);
    PWaveBuffer & buffer = buffers[0];
    buffer = sound;

    DWORD osError;
    PINDEX count = sound.GetSize();
    if ((osError = buffer.Prepare(hWaveOut, count)) == MMSYSERR_NOERROR &&
        (osError = waveOutWrite(hWaveOut, &buffer.header, sizeof(WAVEHDR))) == MMSYSERR_NOERROR) {
      if (wait)
        ok = WaitForPlayCompletion();
    }
    else {
      SetErrorValues(Miscellaneous, osError|PWIN32ErrorFlag, LastWriteError);
      ok = FALSE;
    }

    bufferMutex.Signal();
  }

  SetFormat(numChannels, sampleRate, bitsPerSample);
  SetBuffers(bufferSize, bufferCount);
  return ok;
}


BOOL PSoundChannelWin32::PlayFile(const PFilePath & filename, BOOL wait)
{
  Abort();

  PMultiMediaFile mmio;
  PWaveFormat fileFormat;
  DWORD dataSize;
  if (!mmio.OpenWaveFile(filename, fileFormat, dataSize))
    return SetErrorValues(NotOpen, mmio.GetLastError()|PWIN32ErrorFlag, LastWriteError);

  // Save old format and set to one loaded from file.
  unsigned numChannels = waveFormat->nChannels;
  unsigned sampleRate = waveFormat->nSamplesPerSec;
  unsigned bitsPerSample = waveFormat->wBitsPerSample;
  waveFormat = fileFormat;
  if (!OpenDevice(os_handle)) {
    SetFormat(numChannels, sampleRate, bitsPerSample);
    return FALSE;
  }

  bufferMutex.Wait();

  DWORD osError = MMSYSERR_NOERROR;
  while (dataSize > 0) {
    PWaveBuffer & buffer = buffers[bufferIndex];
    while ((buffer.header.dwFlags&WHDR_DONE) == 0) {
      bufferMutex.Signal();
      // No free buffers, so wait for one
      if (WaitForSingleObject(hEventDone, INFINITE) != WAIT_OBJECT_0) {
        osError = ::GetLastError();
        SetFormat(numChannels, sampleRate, bitsPerSample);
        return SetErrorValues(Miscellaneous, osError|PWIN32ErrorFlag, LastWriteError);
      }
      bufferMutex.Wait();
    }

    // Can't write more than a buffer full
    PINDEX count = dataSize;
    if ((osError = buffer.Prepare(hWaveOut, count)) != MMSYSERR_NOERROR)
      break;

    // Read the waveform data subchunk
    if (!mmio.Read(buffer.GetPointer(), count)) {
      osError = mmio.GetLastError();
      break;
    }

    if ((osError = waveOutWrite(hWaveOut, &buffer.header, sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
      break;

    bufferIndex = (bufferIndex+1)%buffers.GetSize();
    dataSize -= count;
  }

  bufferMutex.Signal();

  if (osError != MMSYSERR_NOERROR) {
    SetFormat(numChannels, sampleRate, bitsPerSample);
    return SetErrorValues(Miscellaneous, osError|PWIN32ErrorFlag, LastWriteError);
  }

  if (dataSize == 0 && wait) {
    WaitForPlayCompletion();
    SetFormat(numChannels, sampleRate, bitsPerSample);
  }

  return TRUE;
}


BOOL PSoundChannelWin32::HasPlayCompleted()
{
  PWaitAndSignal mutex(bufferMutex);

  for (PINDEX i = 0; i < buffers.GetSize(); i++) {
    if ((buffers[i].header.dwFlags&WHDR_DONE) == 0)
      return FALSE;
  }

  return TRUE;
}


BOOL PSoundChannelWin32::WaitForPlayCompletion()
{
  while (!HasPlayCompleted()) {
    if (WaitForSingleObject(hEventDone, INFINITE) != WAIT_OBJECT_0)
      return FALSE;
  }

  return TRUE;
}


BOOL PSoundChannelWin32::StartRecording()
{
  PWaitAndSignal mutex(bufferMutex);

  // See if has started already.
  if (bufferByteOffset != P_MAX_INDEX)
    return TRUE;

  DWORD osError;

  // Start the first read, queue all the buffers
  for (PINDEX i = 0; i < buffers.GetSize(); i++) {
    PWaveBuffer & buffer = buffers[i];
    if ((osError = buffer.Prepare(hWaveIn)) != MMSYSERR_NOERROR)
      return FALSE;
    if ((osError = waveInAddBuffer(hWaveIn, &buffer.header, sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
      return FALSE;
  }

  bufferByteOffset = 0;

  if ((osError = waveInStart(hWaveIn)) == MMSYSERR_NOERROR) // start recording
    return TRUE;

  bufferByteOffset = P_MAX_INDEX;
  return SetErrorValues(Miscellaneous, osError|PWIN32ErrorFlag, LastReadError);
}


BOOL PSoundChannelWin32::Read(void * data, PINDEX size)
{
  lastReadCount = 0;

  if (hWaveIn == NULL)
    return SetErrorValues(NotOpen, EBADF, LastReadError);

  if (!WaitForRecordBufferFull())
    return FALSE;

  PWaitAndSignal mutex(bufferMutex);

  // Check to see if Abort() was called in another thread
  if (bufferByteOffset == P_MAX_INDEX)
    return FALSE;

  PWaveBuffer & buffer = buffers[bufferIndex];

  lastReadCount = buffer.header.dwBytesRecorded - bufferByteOffset;
  if (lastReadCount > size)
    lastReadCount = size;

  memcpy(data, &buffer[bufferByteOffset], lastReadCount);

  bufferByteOffset += lastReadCount;
  if (bufferByteOffset >= (PINDEX)buffer.header.dwBytesRecorded) {
    DWORD osError;
    if ((osError = buffer.Prepare(hWaveIn)) != MMSYSERR_NOERROR)
      return SetErrorValues(Miscellaneous, osError|PWIN32ErrorFlag, LastReadError);
    if ((osError = waveInAddBuffer(hWaveIn, &buffer.header, sizeof(WAVEHDR))) != MMSYSERR_NOERROR)
      return SetErrorValues(Miscellaneous, osError|PWIN32ErrorFlag, LastReadError);

    bufferIndex = (bufferIndex+1)%buffers.GetSize();
    bufferByteOffset = 0;
  }

  return TRUE;
}


BOOL PSoundChannelWin32::RecordSound(PSound & sound)
{
  if (!WaitForAllRecordBuffersFull())
    return FALSE;

  sound.SetFormat(waveFormat->nChannels,
                  waveFormat->nSamplesPerSec,
                  waveFormat->wBitsPerSample);

  PWaitAndSignal mutex(bufferMutex);

  if (buffers.GetSize() == 1 &&
          (PINDEX)buffers[0].header.dwBytesRecorded == buffers[0].GetSize())
    sound = buffers[0];
  else {
    PINDEX totalSize = 0;
    PINDEX i;
    for (i = 0; i < buffers.GetSize(); i++)
      totalSize += buffers[i].header.dwBytesRecorded;

    if (!sound.SetSize(totalSize))
      return SetErrorValues(NoMemory, ENOMEM, LastReadError);

    BYTE * ptr = sound.GetPointer();
    for (i = 0; i < buffers.GetSize(); i++) {
      PINDEX sz = buffers[i].header.dwBytesRecorded;
      memcpy(ptr, buffers[i], sz);
      ptr += sz;
    }
  }

  return TRUE;
}


BOOL PSoundChannelWin32::RecordFile(const PFilePath & filename)
{
  if (!WaitForAllRecordBuffersFull())
    return FALSE;

  PWaitAndSignal mutex(bufferMutex);

  PINDEX dataSize = 0;
  PINDEX i;
  for (i = 0; i < buffers.GetSize(); i++)
    dataSize += buffers[i].header.dwBytesRecorded;

  PMultiMediaFile mmio;
  if (!mmio.CreateWaveFile(filename, waveFormat, dataSize))
    return SetErrorValues(Miscellaneous, mmio.GetLastError()|PWIN32ErrorFlag, LastReadError);

  for (i = 0; i < buffers.GetSize(); i++) {
    if (!mmio.Write(buffers[i], buffers[i].header.dwBytesRecorded))
      return SetErrorValues(Miscellaneous, mmio.GetLastError()|PWIN32ErrorFlag, LastReadError);
  }

  return TRUE;
}


BOOL PSoundChannelWin32::IsRecordBufferFull()
{
  PWaitAndSignal mutex(bufferMutex);

  return (buffers[bufferIndex].header.dwFlags&WHDR_DONE) != 0 &&
          buffers[bufferIndex].header.dwBytesRecorded > 0;
}


BOOL PSoundChannelWin32::AreAllRecordBuffersFull()
{
  PWaitAndSignal mutex(bufferMutex);

  for (PINDEX i = 0; i < buffers.GetSize(); i++) {
    if ((buffers[i].header.dwFlags&WHDR_DONE) == 0 ||
         buffers[i].header.dwBytesRecorded    == 0)
      return FALSE;
  }

  return TRUE;
}


BOOL PSoundChannelWin32::WaitForRecordBufferFull()
{
  if (!StartRecording())  // Start the first read, queue all the buffers
    return FALSE;

  while (!IsRecordBufferFull()) {
    if (WaitForSingleObject(hEventDone, INFINITE) != WAIT_OBJECT_0)
      return FALSE;

    PWaitAndSignal mutex(bufferMutex);
    if (bufferByteOffset == P_MAX_INDEX)
      return FALSE;
  }

  return TRUE;
}


BOOL PSoundChannelWin32::WaitForAllRecordBuffersFull()
{
  if (!StartRecording())  // Start the first read, queue all the buffers
    return FALSE;

  while (!AreAllRecordBuffersFull()) {
    if (WaitForSingleObject(hEventDone, INFINITE) != WAIT_OBJECT_0)
      return FALSE;

    PWaitAndSignal mutex(bufferMutex);
    if (bufferByteOffset == P_MAX_INDEX)
      return FALSE;
  }

  return TRUE;
}


BOOL PSoundChannelWin32::Abort()
{
  DWORD osError = MMSYSERR_NOERROR;

  if (hWaveOut != NULL)
    osError = waveOutReset(hWaveOut);

  if (hWaveIn != NULL)
    osError = waveInReset(hWaveIn);

  {
    PWaitAndSignal mutex(bufferMutex);

    if (hWaveOut != NULL || hWaveIn != NULL) {
      for (PINDEX i = 0; i < buffers.GetSize(); i++) {
        while (buffers[i].Release() == WAVERR_STILLPLAYING) {
          if (hWaveOut != NULL)
            waveOutReset(hWaveOut);
          if (hWaveIn != NULL)
            waveInReset(hWaveIn);
        }
      }
    }

    bufferByteOffset = P_MAX_INDEX;
    bufferIndex = 0;

    // Signal any threads waiting on this event, they should then check
    // the bufferByteOffset variable for an abort.
    SetEvent(hEventDone);
  }

  if (osError != MMSYSERR_NOERROR)
    return SetErrorValues(Miscellaneous, osError|PWIN32ErrorFlag);

  return TRUE;
}


PString PSoundChannelWin32::GetErrorText(ErrorGroup group) const
{
  PString str;

  if ((lastErrorNumber[group]&PWIN32ErrorFlag) == 0)
    return PChannel::GetErrorText(group);

  DWORD osError = lastErrorNumber[group]&~PWIN32ErrorFlag;
  if (direction == Recorder) {
    if (waveInGetErrorText(osError, str.GetPointer(256), 256) != MMSYSERR_NOERROR)
      return PChannel::GetErrorText(group);
  }
  else {
    if (waveOutGetErrorText(osError, str.GetPointer(256), 256) != MMSYSERR_NOERROR)
      return PChannel::GetErrorText(group);
  }

  return str;
}


BOOL PSoundChannelWin32::SetVolume(unsigned newVolume)
{
	if (!IsOpen())
		return SetErrorValues(NotOpen, EBADF);

	DWORD rawVolume = newVolume*65536/100;
	if (rawVolume > 65535)
		rawVolume = 65535;

	UINT tId;
	if (direction == Recorder) 
	{
		tId = (UINT)hWaveIn;


		HMIXER hMixer;
		HRESULT hr;	
		hr = mixerOpen(&hMixer, tId, 0, 0, MIXER_OBJECTF_HWAVEIN);
		if ( FAILED( hr ) ) 
		{
			printf("error - mixerOpen\n");
			return SetErrorValues(NotFound, hr|PWIN32ErrorFlag);
		}

		MIXERLINE mxl;
		memset(&mxl, 0, sizeof(MIXERLINE));
		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;//MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		hr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);

		if (FAILED(hr) || mxl.cControls==0)
		{
			///////////////////// - хз винда тупит!!! ///////////////////////////////
			mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
			hr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
			if (FAILED(hr) || mxl.cControls==0)
			{
				printf("error - mixerGetLineInfo (%s) mxl.cControls=%d\n", mxl.szName, mxl.cControls);
				mixerClose(hMixer);
				return SetErrorValues(NotFound, hr|PWIN32ErrorFlag);
			}
		}

		MIXERCONTROL mc;
		MIXERLINECONTROLS mxlc;

		memset(&mxlc, 0, sizeof(MIXERLINECONTROLS));
		memset(&mc, 0, sizeof(MIXERCONTROL));

		mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		mxlc.cControls = 1;
		mxlc.pamxctrl = &mc;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);

		hr = mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if(FAILED(hr))
		{
			printf("error - mixerGetLineControls (%s)\n", mc.szName);
			mixerClose(hMixer);
			return SetErrorValues(NotFound, hr|PWIN32ErrorFlag);
		}

		if(mxl.cChannels==0)
		{
			printf("error - mxl.cChannels==0\n");
			mixerClose(hMixer);
			return SetErrorValues(NotFound, hr|PWIN32ErrorFlag);
		}

		// getting value
		MIXERCONTROLDETAILS mxcd;
		memset(&mxcd, 0, sizeof(MIXERCONTROLDETAILS));
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.cChannels = mxl.cChannels;
		mxcd.dwControlID = mc.dwControlID;

		MIXERCONTROLDETAILS_UNSIGNED mxdu[2];
		mxdu[0].dwValue = (DWORD)(newVolume*65535.0/100.0);//(mc.Bounds.dwMaximum - mc.Bounds.dwMinimum));
		mxdu[1].dwValue = mxdu[0].dwValue;
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED) * mxcd.cChannels;
		mxcd.paDetails = mxdu;

		if( (hr = mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE)) != MMSYSERR_NOERROR)
		{
			printf("error - mixerSetControlDetails\n");
			mixerClose(hMixer);
			return SetErrorValues(NotFound, hr|PWIN32ErrorFlag);
		}

		printf("newVolume = %d\n", newVolume);
		mixerClose(hMixer);
		return TRUE;
	}
	else
	{
		tId = (UINT) hWaveOut;
	}

	WAVEOUTCAPS caps;
	if (waveOutGetDevCaps(tId, &caps, sizeof(caps)) == MMSYSERR_NOERROR) 
	{
		// If the device does not support L/R volume only the low word matters
		if ((caps.dwSupport & WAVECAPS_LRVOLUME) != 0) 
		{
			// Mantain balance
			DWORD oldVolume = 0;
			if (waveOutGetVolume(hWaveOut, &oldVolume) == MMSYSERR_NOERROR)
			{
				// GetVolume() is supposed to return the value we intended to set.
				// So do the proper calculations
				// 1. (L + R) / 2 = rawVolume      -> GetVolume() formula
				// 2. L / R       = oldL / oldR    -> Unmodified balance
				// Being:
				// oldL = LOWORD(oldVolume)
				// oldR = HIWORD(oldVolume)

				DWORD rVol, lVol;
				DWORD oldL, oldR;

				// Old volume values
				oldL = LOWORD(oldVolume);
				oldR = HIWORD(oldVolume);

				lVol = rVol = 0;

				// First sort out extreme cases
				if ( oldL == oldR )
					rVol = lVol = rawVolume;
				else if ( oldL == 0 )
					rVol = rawVolume;
				else if ( oldR == 0 )
					lVol = rawVolume;
				else {
#ifndef _WIN32_WCE
					rVol = ::MulDiv( 2 * rawVolume, oldR, oldL + oldR );
					lVol = ::MulDiv( rVol, oldL, oldR );
#else
					rVol = 2 * rawVolume * oldR / ( oldL + oldR );
					lVol = rVol * oldL / oldR;
#endif
				}

				rawVolume = MAKELPARAM(lVol, rVol);
			}
			else 
			{
				// Couldn't get current volume. Assume centered balance
				rawVolume = MAKELPARAM(rawVolume, rawVolume);
			}
		}
	}
	else
	{
		// Couldn't get device caps. Assume centered balance
		// If the device does not support independant L/R volume
		// the high-order word (R) is ignored
		rawVolume = MAKELPARAM(rawVolume, rawVolume);
	}

	if (direction == Recorder)
	{
		// Does not appear to be an input volume!!
	}
	else 
	{
		DWORD osError = waveOutSetVolume(hWaveOut, rawVolume);
		if (osError != MMSYSERR_NOERROR)
			return SetErrorValues(Miscellaneous, osError|PWIN32ErrorFlag);
	}

	return TRUE;
}



BOOL PSoundChannelWin32::GetVolume(unsigned & oldVolume)
{
	if (!IsOpen())
		return SetErrorValues(NotOpen, EBADF);

	DWORD rawVolume = 0;
	UINT tId;

	if (direction == Recorder) 
	{
		tId = (UINT)hWaveIn;

		HMIXER hMixer;
		HRESULT hr;	
		hr = mixerOpen(&hMixer, tId, 0, 0, MIXER_OBJECTF_HWAVEIN);
		if ( FAILED( hr ) ) 
		{
			printf("error - mixerOpen\n");
			return SetErrorValues(NotFound, hr|PWIN32ErrorFlag);
		}

		MIXERLINE mxl;
		memset(&mxl, 0, sizeof(MIXERLINE));
		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;//MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		hr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);

		if (FAILED(hr) || mxl.cControls==0)
		{
			///////////////////// - хз винда тупит!!! ///////////////////////////////
			mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
			hr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
			if (FAILED(hr) || mxl.cControls==0)
			{
				printf("error - mixerGetLineInfo (%s) mxl.cControls=%d\n", mxl.szName, mxl.cControls);
				mixerClose(hMixer);
				return SetErrorValues(NotFound, hr|PWIN32ErrorFlag);
			}
		}

		MIXERCONTROL mc;
		MIXERLINECONTROLS mxlc;

		memset(&mxlc, 0, sizeof(MIXERLINECONTROLS));
		memset(&mc, 0, sizeof(MIXERCONTROL));

		mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		mxlc.cControls = 1;
		mxlc.pamxctrl = &mc;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);

		hr = mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if(FAILED(hr))
		{
			printf("error - mixerGetLineControls (%s)\n", mc.szName);
			mixerClose(hMixer);
			return SetErrorValues(NotFound, hr|PWIN32ErrorFlag);
		}

		if(mxl.cChannels==0)
		{
			printf("error - mxl.cChannels==0\n");
			mixerClose(hMixer);
			return SetErrorValues(NotFound, hr|PWIN32ErrorFlag);
		}

		// getting value
		MIXERCONTROLDETAILS mxcd;
		memset(&mxcd, 0, sizeof(MIXERCONTROLDETAILS));
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.cChannels = mxl.cChannels;
		mxcd.dwControlID = mc.dwControlID;

		MIXERCONTROLDETAILS_UNSIGNED mxdu[2];
		memset(mxdu, 0, sizeof(MIXERCONTROLDETAILS_UNSIGNED)*2);
		mxdu[0].dwValue = (DWORD)-1; 
		mxdu[1].dwValue = (DWORD)-1; 
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED)*mxcd.cChannels;
		mxcd.paDetails = mxdu;

		if( (hr = mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE)) != MMSYSERR_NOERROR)
		{
			printf("error - mixerSetControlDetails\n");
			mixerClose(hMixer);
			return SetErrorValues(NotFound, hr|PWIN32ErrorFlag);
		}

		mixerClose(hMixer);

		if(mxcd.cChannels==1)
			oldVolume = (unsigned)((double)mxdu[0].dwValue/65535.0*100.0);
		if(mxcd.cChannels==2)
			oldVolume = (unsigned)((double)(mxdu[0].dwValue+mxdu[1].dwValue)/2.0/65535.0*100.0);
		printf("oldVolume = %d\n", oldVolume);
		return TRUE;
	}
	else
	{
		tId = (UINT)hWaveOut;
		DWORD osError = waveOutGetVolume(hWaveOut, &rawVolume);
		if (osError != MMSYSERR_NOERROR)
			return SetErrorValues(Miscellaneous, osError|PWIN32ErrorFlag);
	}

	WAVEOUTCAPS caps;
	if (waveOutGetDevCaps(tId, &caps, sizeof(caps)) == MMSYSERR_NOERROR &&
		(caps.dwSupport & WAVECAPS_LRVOLUME) != 0)
		rawVolume = (HIWORD(rawVolume) + LOWORD(rawVolume)) / 2;

	oldVolume = rawVolume*100/65536;
	return TRUE;
}

// End of File ///////////////////////////////////////////////////////////////

