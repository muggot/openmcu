/*
 * pwavfile.cxx
 *
 * WAV file I/O channel class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2001 Equivalence Pty. Ltd.
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
 * Roger Hardiman <roger@freebsd.org>
 * and Shawn Pai-Hsiang Hsiao <shawn@eecs.harvard.edu>
 *
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: pwavfile.cxx,v $
 * Revision 1.53  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.52  2007/05/09 12:04:23  csoutheren
 * Applied 1705775 - PWAVFile::UpdateHeader() redundancy
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.51  2007/04/20 07:59:29  csoutheren
 * Applied 1675658 - various pwavfile.[h|cxx] improvments
 * Thanks to Drazen Dimoti
 *
 * Revision 1.50  2007/04/20 07:26:51  csoutheren
 * Applied 1703655 - PWAVFile fixes to stereo recording
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.49  2006/07/05 04:00:25  csoutheren
 * Ensure Read and Write fail gracefully when file not open
 *
 * Revision 1.48  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.47  2006/04/10 23:57:27  csoutheren
 * Checked in changes to remove some warnings with gcc effc++ flag
 *
 * Revision 1.46  2006/04/09 22:22:35  dereksmithies
 * Fix a warning about comparison of signed and unsigned numbers.
 *
 * Revision 1.45  2006/04/06 00:39:37  csoutheren
 * Ensure autoconvert format is preserved across file close
 *
 * Revision 1.44  2006/01/16 07:31:57  csoutheren
 * Removed deletion of PWAVFIle format converters.
 *  These look like memory leaks, but are not - the converters are static objects that
 *  cannot be deleted
 *
 * Revision 1.43  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.42  2005/11/25 01:01:15  csoutheren
 * Applied patch #1351168
 * PWlib various fixes
 *
 * Revision 1.41  2005/10/30 23:25:52  csoutheren
 * Fixed formatting
 * Removed throw() declarations (PWLib does not do exceptions)
 * Removed duplicate destructor declarations and definitions
 *
 * Revision 1.40  2005/10/30 19:41:53  dominance
 * fixed most of the warnings occuring during compilation
 *
 * Revision 1.39  2005/06/09 00:33:20  csoutheren
 * Fixed crash problem caused by recent leak fix
 * Removed bogus error when reading all of file contents in a single read
 *
 * Revision 1.38  2005/06/07 09:28:46  csoutheren
 * Fixed bug #1204964 - ensure full cleanup when WAV file is closed
 * Thanks to Zdenek Broz
 *
 * Revision 1.37  2005/03/22 07:32:55  csoutheren
 * Fixed problem with incorrect message being displayed when reading past end of file
 *
 * Revision 1.36  2005/01/04 08:09:42  csoutheren
 * Fixed Linux configure problems
 *
 * Revision 1.35  2004/11/08 04:07:40  csoutheren
 * Fixed crash opportunity under some conditions
 * Fixed incorrect WAV file type display
 *
 * Revision 1.34  2004/07/19 12:32:25  csoutheren
 * Removed vestigial debug comment
 *
 * Revision 1.33  2004/07/19 12:23:38  csoutheren
 * Removed compiler crash under gcc 3.4.0
 *
 * Revision 1.32  2004/07/15 03:12:42  csoutheren
 * Migrated changes from crs_vxnml_devel branch into main trunk
 *
 * Revision 1.31.4.4  2004/07/13 08:13:05  csoutheren
 * Lots of implementation of factory-based PWAVFile
 *
 * Revision 1.31.4.3  2004/07/12 09:17:20  csoutheren
 * Fixed warnings and errors under Linux
 *
 * Revision 1.31.4.2  2004/07/12 08:30:16  csoutheren
 * More fixes for abstract factory implementation of PWAVFile
 *
 * Revision 1.31.4.1  2004/07/07 07:07:42  csoutheren
 * Changed PWAVFile to use abstract factories (extensively)
 * Removed redundant blocking/unblocking when using G.723.1
 * More support for call transfer
 *
 * Revision 1.31  2003/07/29 11:27:16  csoutheren
 * Changed to use autoconf detected swab function
 *
 * Revision 1.30  2003/07/28 18:39:09  dsandras
 * Linux has a swab function. Patch from Alexander Larsson <alexl@redhat.com>.
 *
 * Revision 1.29  2003/02/20 23:32:00  robertj
 * More RTEMS support patches, thanks Sebastian Meyer.
 *
 * Revision 1.28  2002/12/20 08:43:42  robertj
 * Fixed incorrect header length for MS-GSM, thanks Martijn Roest & Kanchana
 *
 * Revision 1.27  2002/07/12 01:25:25  craigs
 * Repaired reintroduced problem with SID frames in WAV files
 *
 * Revision 1.26  2002/07/02 06:25:25  craigs
 * Added ability to create files in MS G.723.1 format
 *
 * Revision 1.25  2002/06/20 00:54:41  craigs
 * Added explicit class names to some functions to alloew overriding
 *
 * Revision 1.24  2002/06/12 07:28:16  craigs
 * Fixed problem with opening WAV files in read mode
 *
 * Revision 1.23  2002/05/23 05:04:11  robertj
 * Set error code if get invalid sized write for G.723.1 wav file.
 *
 * Revision 1.22  2002/05/23 03:59:55  robertj
 * Changed G.723.1 WAV file so every frame is 24 bytes long.
 *
 * Revision 1.21  2002/05/21 01:59:54  robertj
 * Removed the enum which made yet another set of magic numbers for audio
 *   formats, now uses the WAV file format numbers.
 * Fixed missing Open() function which does not have file name parameter.
 * Added ability to set the audio format after construction.
 * Added automatic expansion of G.723.1 SID frames into 24 zero bytes as
 *   those formats do not currently support 4 byte frames.
 * Fixed trace output to include "module" section.
 *
 * Revision 1.20  2002/02/06 00:52:23  robertj
 * Fixed GNU warning.
 *
 * Revision 1.19  2002/01/31 15:29:26  rogerh
 * Fix a problem with .wav files recorded in GoldWave.  The GoldWave copyright
 * string (embedded at the end of the wav file) was returned as audio data and
 * heared as noise. Javi <fjmchm@hotmail.com> reported the problem.
 *
 * Revision 1.18  2002/01/22 03:55:59  craigs
 * Added include of ptclib/pwavfile.cxx as this is now in PTCLib
 *
 * Revision 1.17  2002/01/13 21:01:55  rogerh
 * The class contructor is now used to specify the type of new WAV files
 * (eg PCM or G7231)
 *
 * Revision 1.16  2002/01/11 16:33:46  rogerh
 * Create a PWAVFile Open() function, which processes the WAV header
 *
 * Revision 1.15  2001/10/16 13:27:37  rogerh
 * Add support for writing G.723.1 WAV files.
 * MS Windows can play G.723.1 WAV Files in Media Player and Sound Recorder.
 * Sound Recorder can also convert them to normal PCM format WAV files.
 * Thanks go to M.Stoychev <M.Stoychev@cnsys.bg> for sample WAV files.
 *
 * Revision 1.14  2001/10/15 11:48:15  rogerh
 * Add GetFormat to return the format of a WAV file
 *
 * Revision 1.13  2001/10/15 07:27:38  rogerh
 * Add support for reading WAV fils containing G.723.1 audio data.
 *
 * Revision 1.12  2001/09/29 07:41:42  rogerh
 * Add fix from Patrick Koorevaar <pkoorevaar@hotmail.com>
 *
 * Revision 1.11  2001/08/15 12:52:20  rogerh
 * Fix typo
 *
 * Revision 1.10  2001/08/15 12:21:45  rogerh
 * Make Solaris use our swab() function instead of the C library version.
 * Submitted by Andre Schulze <as8@rncmm2.urz.tu-dresden.de>
 *
 * Revision 1.9  2001/07/23 02:57:42  robertj
 * Fixed swab definition for Linux alpha.
 *
 * Revision 1.8  2001/07/23 01:20:20  rogerh
 * Add updates from Shawn - ensure isvalidWAV is false for zero length files.
 * GetDataLength uses actual file size to support file updates as well as appends.
 * Add updates from Roger - Update Header() just writes to specific fields which
 * preserves any 'extra' data in an existing header between FORMAT and DATA chunks.
 *
 * Revision 1.7  2001/07/20 07:32:36  rogerh
 * Back out previous change. BSD systems already have swab in the C library.
 * Also use swab in Write()
 *
 * Revision 1.6  2001/07/20 07:09:12  rogerh
 * We need to byte swap on more then just Linux and BeOS.
 *
 * Revision 1.5  2001/07/20 04:14:47  robertj
 * Fixed swab implementation on Linux alpha
 *
 * Revision 1.4  2001/07/20 03:30:59  robertj
 * Minor cosmetic changes to new PWAVFile class.
 *
 * Revision 1.3  2001/07/19 09:57:24  rogerh
 * Use correct filename
 *
 * Revision 1.2  2001/07/19 09:53:29  rogerh
 * Add the PWAVFile class to read and write .wav files
 * The PWAVFile class was written by Roger Hardiman <roger@freebsd.org>
 * and Shawn Pai-Hsiang Hsiao <shawn@eecs.harvard.edu>
 *
 */

#ifdef __GNUC__
#pragma implementation "pwavfile.h"
#endif

#include <ptlib.h>
#include <ptlib/pfactory.h>
#include <ptclib/pwavfile.h>

#define new PNEW


const char WAVLabelRIFF[4] = { 'R', 'I', 'F', 'F' };
const char WAVLabelWAVE[4] = { 'W', 'A', 'V', 'E' };
const char WAVLabelFMT_[4] = { 'f', 'm', 't', ' ' };
const char WAVLabelFACT[4] = { 'F', 'A', 'C', 'T' };
const char WAVLabelDATA[4] = { 'd', 'a', 't', 'a' };

PINSTANTIATE_FACTORY(PWAVFileFormat, unsigned)
PINSTANTIATE_FACTORY(PWAVFileConverter, unsigned)

inline BOOL ReadAndCheck(PWAVFile & file, void * buf, PINDEX len)
{
  return file.FileRead(buf, len) && (file.PFile::GetLastReadCount() == len);
}

inline BOOL WriteAndCheck(PWAVFile & file, void * buf, PINDEX len)
{
  return file.FileWrite(buf, len) && (file.GetLastWriteCount() == len);
}

#if PBYTE_ORDER==PBIG_ENDIAN
#  if defined(USE_SYSTEM_SWAB)
#    define SWAB(a,b,c) ::swab(a,b,c)
#  else
static void SWAB(const void * void_from, void * void_to, register size_t len)
{
  register const char * from = (const char *)void_from;
  register char * to = (char *)void_to;

  while (len > 1) {
    char b = *from++;
    *to++ = *from++;
    *to++ = b;
    len -= 2;
  }
}
#  endif
#else
#  define SWAB(a,b,c) {}
#endif

///////////////////////////////////////////////////////////////////////////////
// PWAVFile

PWAVFile::PWAVFile(unsigned fmt)
  : PFile(), origFmt(fmt)
{
  Construct();
  SelectFormat(fmt);
}

PWAVFile * PWAVFile::format(const PString & format)
{
  PWAVFile * file = new PWAVFile;
  file->origFmt = 0xffffffff;
  file->Construct();
  file->SelectFormat(format);
  return file;
}

PWAVFile::PWAVFile(OpenMode mode, int opts, unsigned fmt)
  : PFile(mode, opts), origFmt(fmt)
{
  Construct();
  SelectFormat(fmt);
}

PWAVFile * PWAVFile::format(
  const PString & format,
  PFile::OpenMode mode,
  int opts
)
{
  PWAVFile * file = new PWAVFile(mode, opts);
  file->origFmt = 0xffffffff;
  file->Construct();
  file->SelectFormat(format);
  return file;
}

PWAVFile::PWAVFile(const PFilePath & name, OpenMode mode, int opts, unsigned fmt)
  : origFmt(fmt)
{
  Construct();
  SelectFormat(fmt);
  Open(name, mode, opts);
}

PWAVFile::PWAVFile(
  const PString & format,  
  const PFilePath & name,  
  OpenMode mode,
  int opts 
)
{
  origFmt = 0xffffffff;
  Construct();
  SelectFormat(format);
  Open(name, mode, opts);
}

PWAVFile::~PWAVFile()
{ 
  Close(); 
  if (formatHandler != NULL)
    delete formatHandler;
}


void PWAVFile::Construct()
{
  lenData                 = 0;
  lenHeader               = 0;
  isValidWAV              = FALSE;
  header_needs_updating   = FALSE;
  autoConvert             = FALSE;
  autoConverter           = NULL;

  formatHandler           = NULL;
  wavFmtChunk.hdr.len     = sizeof(wavFmtChunk) - sizeof(wavFmtChunk.hdr);
}

void PWAVFile::SelectFormat(unsigned fmt)
{
  if (formatHandler != NULL) {
    delete formatHandler;
    formatHandler = NULL;
  }
  if (fmt != fmt_NotKnown) {
    formatHandler       = PWAVFileFormatByIDFactory::CreateInstance(fmt);
    wavFmtChunk.format  = (WORD)fmt;
  }
}

void PWAVFile::SelectFormat(const PString & format)
{
  if (formatHandler != NULL) {
    delete formatHandler;
    formatHandler = NULL;
  }
  if (!format.IsEmpty())
    formatHandler = PWAVFileFormatByFormatFactory::CreateInstance(format);
  if (formatHandler != NULL) {
    wavFmtChunk.format = (WORD)formatHandler->GetFormat();
    if (origFmt == 0xffffffff)
      origFmt = wavFmtChunk.format;
  }
}

BOOL PWAVFile::Open(OpenMode mode, int opts)
{
  if (!(PFile::Open(mode, opts)))
    return FALSE;

  isValidWAV = FALSE;

  // Try and process the WAV file header information.
  // Either ProcessHeader() or GenerateHeader() must be called.

  if (PFile::GetLength() > 0) {

    // try and process the WAV file header information
    if (mode == ReadOnly || mode == ReadWrite) {
      isValidWAV = ProcessHeader();
    }
    if (mode == WriteOnly) {
      lenData = -1;
      GenerateHeader();
    }
  }
  else {

    // generate header
    if (mode == ReadWrite || mode == WriteOnly) {
      lenData = -1;
      GenerateHeader();
    }
    if (mode == ReadOnly) {
      isValidWAV = FALSE; // ReadOnly on a zero length file
    }
  }

  // if we did not know the format when we opened, then we had better know it now
  if (formatHandler == NULL) {
    Close();
    return FALSE;
  }

  return TRUE;
}


BOOL PWAVFile::Open(const PFilePath & name, OpenMode mode, int opts)
{
  if (IsOpen())
    Close();
  SetFilePath(name);
  return Open(mode, opts);
}


BOOL PWAVFile::Close()
{
  if (autoConverter != NULL) {
    autoConverter = NULL;
  }

  if (!PFile::IsOpen())
    return TRUE;

  if (header_needs_updating)
    UpdateHeader();

  if (formatHandler != NULL) 
    formatHandler->OnStop();

  delete formatHandler;
  formatHandler = NULL;
  if (origFmt != 0xffffffff)
    SelectFormat(origFmt);

  return PFile::Close();
}

void PWAVFile::SetAutoconvert()
{ 
  autoConvert = TRUE; 
}


// Performs necessary byte-order swapping on for big-endian platforms.
BOOL PWAVFile::Read(void * buf, PINDEX len)
{
	if (!IsOpen())
		return FALSE;

  if (autoConverter != NULL)
    return autoConverter->Read(*this, buf, len);

  return RawRead(buf, len);
}

BOOL PWAVFile::RawRead(void * buf, PINDEX len)
{
  // Some wav files have extra data after the sound samples in a LIST chunk.
  // e.g. WAV files made in GoldWave have a copyright and a URL in this chunk.
  // We do not want to return this data by mistake.
  PINDEX readlen = len;
  off_t pos = PFile::GetPosition();
  if (pos >= (lenHeader+lenData))
    return FALSE;
  
  if ((pos + len) > (lenHeader+lenData))
    readlen = (lenHeader+lenData) - pos;

  if (formatHandler != NULL)
    return formatHandler->Read(*this, buf, readlen);

  return FileRead(buf, readlen);
}

BOOL PWAVFile::FileRead(void * buf, PINDEX len)
{
  return PFile::Read(buf, len);
}

// Performs necessary byte-order swapping on for big-endian platforms.
BOOL PWAVFile::Write(const void * buf, PINDEX len)
{
	if (!IsOpen())
		return FALSE;

  // Needs to update header on close.
  header_needs_updating = TRUE;

  if (autoConverter != NULL)
    return autoConverter->Write(*this, buf, len);

  return RawWrite(buf, len);
}

BOOL PWAVFile::RawWrite(const void * buf, PINDEX len)
{
  // Needs to update header on close.
  header_needs_updating = TRUE;

  if (formatHandler != NULL)
    return formatHandler->Write(*this, buf, len);

  return FileWrite(buf, len);
}

BOOL PWAVFile::FileWrite(const void * buf, PINDEX len)
{
  return PFile::Write(buf, len);
}

// Both SetPosition() and GetPosition() are offset by lenHeader.
BOOL PWAVFile::SetPosition(off_t pos, FilePositionOrigin origin)
{
  if (autoConverter != NULL)
    return autoConverter->SetPosition(*this, pos, origin);

  return RawSetPosition(pos, origin);
}

BOOL PWAVFile::RawSetPosition(off_t pos, FilePositionOrigin origin)
{
  if (isValidWAV) {
    pos += lenHeader;
  }

  return PFile::SetPosition(pos, origin);
}


off_t PWAVFile::GetPosition() const
{
  if (autoConverter != NULL)
    return autoConverter->GetPosition(*this);

  return RawGetPosition();
}

off_t PWAVFile::RawGetPosition() const
{
  off_t pos = PFile::GetPosition();

  if (isValidWAV) {
    if (pos >= lenHeader) {
      pos -= lenHeader;
    }
    else {
      pos = 0;
    }
  }

  return (pos);
}


unsigned PWAVFile::GetFormat() const
{
  if (isValidWAV)
    return wavFmtChunk.format;
  else
    return 0;
}

PString PWAVFile::GetFormatAsString() const
{
  if (isValidWAV && formatHandler != NULL)
    return formatHandler->GetFormat();
  else
    return PString::Empty();
}

unsigned PWAVFile::GetChannels() const
{
  if (isValidWAV)
    return wavFmtChunk.numChannels;
  else
    return 0;
}

void PWAVFile::SetChannels(unsigned v) 
{
  wavFmtChunk.numChannels = (WORD)v;
  if (wavFmtChunk.numChannels == 1 || wavFmtChunk.numChannels == 2)
  {
      wavFmtChunk.bytesPerSample = (wavFmtChunk.bitsPerSample/8) * wavFmtChunk.numChannels;
      wavFmtChunk.bytesPerSec = wavFmtChunk.sampleRate * wavFmtChunk.bytesPerSample;
  }
  header_needs_updating = TRUE;
}

unsigned PWAVFile::GetSampleRate() const
{
  if (isValidWAV)
    return wavFmtChunk.sampleRate;
  else
    return 0;
}

void PWAVFile::SetSampleRate(unsigned v) 
{
  wavFmtChunk.sampleRate = (WORD)v;
  header_needs_updating = TRUE;
}

unsigned PWAVFile::GetSampleSize() const
{
  if (isValidWAV)
    return wavFmtChunk.bitsPerSample;
  else
    return 0;
}

void PWAVFile::SetSampleSize(unsigned v) 
{
  wavFmtChunk.bitsPerSample = (WORD)v;
  header_needs_updating = TRUE;
}

unsigned PWAVFile::GetBytesPerSecond() const
{
  if (isValidWAV)
    return wavFmtChunk.bytesPerSec;
  else
    return 0;
}

void PWAVFile::SetBytesPerSecond(unsigned v)
{
  wavFmtChunk.bytesPerSec = (WORD)v;
  header_needs_updating = TRUE;
}

off_t PWAVFile::GetHeaderLength() const
{
  if (isValidWAV)
    return lenHeader;
  else
    return 0;
}


off_t PWAVFile::GetDataLength()
{
  if (autoConverter != NULL)
    return autoConverter->GetDataLength(*this);

  return RawGetDataLength();
}

off_t PWAVFile::RawGetDataLength()
{
  if (isValidWAV) {
    // Updates data length before returns.
    lenData = PFile::GetLength() - lenHeader;
    return lenData;
  }
  else
    return 0;
}


BOOL PWAVFile::SetFormat(unsigned fmt)
{
  if (IsOpen() || isValidWAV)
    return FALSE;

  SelectFormat(fmt);

  return TRUE;
}

BOOL PWAVFile::SetFormat(const PString & format)
{
  if (IsOpen() || isValidWAV)
    return FALSE;

  SelectFormat(format);

  return TRUE;
}

static inline BOOL NeedsConverter(const PWAV::FMTChunk & fmtChunk)
{
  return (fmtChunk.format != PWAVFile::fmt_PCM) || (fmtChunk.bitsPerSample != 16);
}

BOOL PWAVFile::ProcessHeader() 
{
  if (autoConverter != NULL) {
    delete autoConverter;
    autoConverter = NULL;
  }

  // Process the header information
  // This comes in 3 or 4 chunks, either RIFF, FORMAT and DATA
  // or RIFF, FORMAT, FACT and DATA.

  if (!IsOpen()) {
    PTRACE(1,"WAV\tProcessHeader: Not Open");
    return (FALSE);
  }

  // go to the beginning of the file
  if (!PFile::SetPosition(0)) {
    PTRACE(1,"WAV\tProcessHeader: Cannot Set Pos");
    return (FALSE);
  }

  // Read the RIFF chunk.
  struct PWAV::RIFFChunkHeader riffChunk;
  if (!ReadAndCheck(*this, &riffChunk, sizeof(riffChunk)))
    return FALSE;

  // check if tags are correct
  if (strncmp(riffChunk.hdr.tag, WAVLabelRIFF, sizeof(WAVLabelRIFF)) != 0) {
    PTRACE(1,"WAV\tProcessHeader: Not RIFF");
    return (FALSE);
  }

  if (strncmp(riffChunk.tag, WAVLabelWAVE, sizeof(WAVLabelWAVE)) != 0) {
    PTRACE(1,"WAV\tProcessHeader: Not WAVE");
    return (FALSE);
  }

  // Read the known part of the FORMAT chunk.
  if (!ReadAndCheck(*this, &wavFmtChunk, sizeof(wavFmtChunk)))
    return FALSE;

  // check if labels are correct
  if (strncmp(wavFmtChunk.hdr.tag, WAVLabelFMT_, sizeof(WAVLabelFMT_)) != 0) {
    PTRACE(1,"WAV\tProcessHeader: Not FMT");
    return (FALSE);
  }

  // if we opened the file without knowing the format, then try and set the format now
  if (formatHandler == NULL) {
    SelectFormat(wavFmtChunk.format);
    if (formatHandler == NULL) {
      Close();
      return FALSE;
    }
  }

  // read the extended format chunk (if any)
  extendedHeader.SetSize(0);
  if ((unsigned)wavFmtChunk.hdr.len > (sizeof(wavFmtChunk) - sizeof(wavFmtChunk.hdr))) {
    extendedHeader.SetSize(wavFmtChunk.hdr.len - (sizeof(wavFmtChunk) - sizeof(wavFmtChunk.hdr)));
    if (!ReadAndCheck(*this, extendedHeader.GetPointer(), extendedHeader.GetSize()))
      return FALSE;
  }

  // give format handler a chance to read extra chunks
  if (!formatHandler->ReadExtraChunks(*this))
    return FALSE;

  PWAV::ChunkHeader chunkHeader;

  // ignore chunks until we see a DATA chunk
  for (;;) {
    if (!ReadAndCheck(*this, &chunkHeader, sizeof(chunkHeader)))
      return FALSE;
    if (strncmp(chunkHeader.tag, WAVLabelDATA, sizeof(WAVLabelDATA)) == 0) 
      break;
    if (!PFile::SetPosition(PFile::GetPosition() + + chunkHeader.len)) {
      PTRACE(1,"WAV\tProcessHeader: Cannot set new position");
      return FALSE;
    }
  }

  // calculate the size of header and data for accessing the WAV data.
  lenHeader  = PFile::GetPosition(); 
  lenData    = chunkHeader.len;

  // get ptr to data handler if in autoconvert mode
  if (autoConvert && NeedsConverter(wavFmtChunk)) {
    autoConverter = PWAVFileConverterFactory::CreateInstance(wavFmtChunk.format);
    if (autoConverter == NULL) {
      PTRACE(1, "PWAVFile\tNo format converter for type " << (int)wavFmtChunk.format);
    }
  }

  formatHandler->OnStart();

  return TRUE;
}


// Generates the wave file header.
// Two types of header are supported.
// a) PCM data, set to 8000Hz, mono, 16-bit samples
// b) G.723.1 data
// When this function is called with lenData < 0, it will write the header
// as if the lenData is LONG_MAX minus header length.
// Note: If it returns FALSE, the file may be left in inconsistent state.

BOOL PWAVFile::GenerateHeader()
{
  if (autoConverter != NULL) {
    autoConverter = NULL;
  }

  if (!IsOpen()) {
    PTRACE(1, "WAV\tGenerateHeader: Not Open");
    return (FALSE);
  }

  // length of audio data is set to a large value if lenData does not
  // contain a valid (non negative) number. We must then write out real values
  // when we close the wav file.
  int audioDataLen;
  if (lenData < 0) {
    audioDataLen = LONG_MAX - wavFmtChunk.hdr.len;
    header_needs_updating = TRUE;
  } else {
    audioDataLen = lenData;
  }

  // go to the beginning of the file
  if (!PFile::SetPosition(0)) {
    PTRACE(1,"WAV\tGenerateHeader: Cannot Set Pos");
    return (FALSE);
  }

  // write the WAV file header
  PWAV::RIFFChunkHeader riffChunk;
  memcpy(riffChunk.hdr.tag, WAVLabelRIFF, sizeof(WAVLabelRIFF));
  memcpy(riffChunk.tag,     WAVLabelWAVE, sizeof(WAVLabelWAVE));
  riffChunk.hdr.len = lenHeader + audioDataLen - sizeof(riffChunk.hdr);

  if (!WriteAndCheck(*this, &riffChunk, sizeof(riffChunk)))
    return FALSE;

  // populate and write the WAV header with the default data
  memcpy(wavFmtChunk.hdr.tag,  WAVLabelFMT_, sizeof(WAVLabelFMT_));
  wavFmtChunk.hdr.len = sizeof(wavFmtChunk) - sizeof(wavFmtChunk.hdr);  // set default length assuming no extra bytes

  // allow the format handler to modify the header and extra bytes
  if(formatHandler == NULL){
    PTRACE(1,"WAV\tGenerateHeader: format handler is null!");
    return FALSE;
  }
  formatHandler->CreateHeader(wavFmtChunk, extendedHeader);

  // write the basic WAV header
  if (
      !WriteAndCheck(*this, &wavFmtChunk, sizeof(wavFmtChunk)) ||
      ((extendedHeader.GetSize() > 0) && !WriteAndCheck(*this, extendedHeader.GetPointer(), extendedHeader.GetSize()))
     )
    return FALSE;

  // allow the format handler to write additional chunks
  if (!formatHandler->WriteExtraChunks(*this))
    return FALSE;

  // Write the DATA chunk.
  PWAV::ChunkHeader dataChunk;
  memcpy(dataChunk.tag, WAVLabelDATA, sizeof(WAVLabelDATA));
  dataChunk.len = audioDataLen;
  if (!WriteAndCheck(*this, &dataChunk, sizeof(dataChunk)))
    return FALSE;

  isValidWAV = TRUE;

  // get the length of the header
  lenHeader = PFile::GetPosition();

  // get pointer to auto converter 
  if (autoConvert && NeedsConverter(wavFmtChunk)) {
    autoConverter = PWAVFileConverterFactory::CreateInstance(wavFmtChunk.format);
    if (autoConverter == NULL) {
      PTRACE(1, "PWAVFile\tNo format converter for type " << (int)wavFmtChunk.format);
      return FALSE;
    }
  }

  return (TRUE);
}

// Update the WAV header according to the file length
BOOL PWAVFile::UpdateHeader()
{
  // Check file is still open
  if (!IsOpen()) {
    PTRACE(1,"WAV\tUpdateHeader: Not Open");
    return (FALSE);
  }

  // Check there is already a valid header
  if (!isValidWAV) {
    PTRACE(1,"WAV\tUpdateHeader: File not valid");
    return (FALSE);
  }

  // Find out the length of the audio data
  lenData = PFile::GetLength() - lenHeader;

  // rewrite the length in the RIFF chunk
  PInt32l riffChunkLen = (lenHeader - 8) + lenData; // size does not include first 8 bytes
  PFile::SetPosition(4);
  if (!WriteAndCheck(*this, &riffChunkLen, sizeof(riffChunkLen)))
    return FALSE;

  // rewrite the data length field in the data chunk
  PInt32l dataChunkLen = lenData;
  PFile::SetPosition(lenHeader - 4);
  if (!WriteAndCheck(*this, &dataChunkLen, sizeof(dataChunkLen)))
    return FALSE;

  if(formatHandler == NULL){
    PTRACE(1,"WAV\tGenerateHeader: format handler is null!");
    return FALSE;
  }
  formatHandler->UpdateHeader(wavFmtChunk, extendedHeader);

  PFile::SetPosition(12);
  if (!WriteAndCheck(*this, &wavFmtChunk, sizeof(wavFmtChunk)))
    return FALSE;

  if (!WriteAndCheck(*this, extendedHeader.GetPointer(), extendedHeader.GetSize()))
    return FALSE;

  header_needs_updating = FALSE;

  return TRUE;
}


//////////////////////////////////////////////////////////////////

BOOL PWAVFileFormat::Read(PWAVFile & file, void * buf, PINDEX & len)
{ 
  if (!file.FileRead(buf, len))
    return FALSE;

  len = file.GetLastReadCount();
  return TRUE;
}

BOOL PWAVFileFormat::Write(PWAVFile & file, const void * buf, PINDEX & len)
{ 
  if (!file.FileWrite(buf, len))
    return FALSE;

  len = file.GetLastWriteCount();
  return TRUE;
}

//////////////////////////////////////////////////////////////////

class PWAVFileFormatPCM : public PWAVFileFormat
{
  public:
    void CreateHeader(PWAV::FMTChunk & wavFmtChunk, PBYTEArray & extendedHeader);
    void UpdateHeader(PWAV::FMTChunk & wavFmtChunk, PBYTEArray & extendedHeader);

    unsigned GetFormat() const
    { return PWAVFile::fmt_PCM; }

    PString GetDescription() const
    { return "PCM"; }

    PString GetFormatString() const
    { return "PCM-16"; }

    BOOL Read(PWAVFile & file, void * buf, PINDEX & len);
    BOOL Write(PWAVFile & file, const void * buf, PINDEX & len);
};

PWAVFileFormatByIDFactory::Worker<PWAVFileFormatPCM> pcmIDWAVFormat(PWAVFile::fmt_PCM);
PWAVFileFormatByFormatFactory::Worker<PWAVFileFormatPCM> pcmFormatWAVFormat("PCM-16");

void PWAVFileFormatPCM::CreateHeader(PWAV::FMTChunk & wavFmtChunk, 
                                     PBYTEArray & /*extendedHeader*/)
{
  wavFmtChunk.hdr.len         = sizeof(wavFmtChunk) - sizeof(wavFmtChunk.hdr);  // no extended information
  wavFmtChunk.format          = PWAVFile::fmt_PCM;
  wavFmtChunk.numChannels     = 1;
  wavFmtChunk.sampleRate      = 8000;
  wavFmtChunk.bytesPerSample  = 2;
  wavFmtChunk.bitsPerSample   = 16;
  wavFmtChunk.bytesPerSec     = wavFmtChunk.sampleRate * wavFmtChunk.bytesPerSample;
}

void PWAVFileFormatPCM::UpdateHeader(PWAV::FMTChunk & wavFmtChunk, 
                                     PBYTEArray & /*extendedHeader*/)
{
  wavFmtChunk.bytesPerSample  = 2 * wavFmtChunk.numChannels;
  wavFmtChunk.bytesPerSec     = wavFmtChunk.sampleRate * 2 * wavFmtChunk.numChannels;
}

BOOL PWAVFileFormatPCM::Read(PWAVFile & file, void * buf, PINDEX & len)
{
  if (!file.FileRead(buf, len))
    return FALSE;

  len = file.GetLastReadCount();

  // WAV files are little-endian. So swap the bytes if this is
  // a big endian machine and we have 16 bit samples
  // Note: swab only works on even length buffers.
  if (file.wavFmtChunk.bitsPerSample == 16) {
    SWAB(buf, buf, len);
  }

  return TRUE;
}

BOOL PWAVFileFormatPCM::Write(PWAVFile & file, const void * buf, PINDEX & len)
{
  // WAV files are little-endian. So swap the bytes if this is
  // a big endian machine and we have 16 bit samples
  // Note: swab only works on even length buffers.
  if (file.wavFmtChunk.bitsPerSample == 16) {
    SWAB(buf, (void *)buf, len);
  }

  if (!file.FileWrite(buf, len))
    return FALSE;

  len = file.GetLastWriteCount();
  return TRUE;
}

//////////////////////////////////////////////////////////////////

#ifdef __GNUC__
#define P_PACKED    __attribute__ ((packed));
#else
#define P_PACKED
#pragma pack(1)
#endif

struct G7231ExtendedInfo {
  PInt16l data1      P_PACKED;      // 1
  PInt16l data2      P_PACKED;      // 480
};

struct G7231FACTChunk {
  PWAV::ChunkHeader hdr;
  PInt32l data1      P_PACKED;      // 0   Should be number of samples.
};

#ifdef __GNUC__
#undef P_PACKED
#else
#pragma pack()
#endif


class PWAVFileFormatG7231 : public PWAVFileFormat
{
  public:
    PWAVFileFormatG7231(unsigned short _g7231)
      : g7231(_g7231) { }

    virtual ~PWAVFileFormatG7231() {}

    void CreateHeader(PWAV::FMTChunk & wavFmtChunk, PBYTEArray & extendedHeader);
    BOOL WriteExtraChunks(PWAVFile & file);

    PString GetFormatString() const
    { return "G.723.1"; }   // must match string in mediafmt.h

    void OnStart();
    BOOL Read(PWAVFile & file, void * buf, PINDEX & len);
    BOOL Write(PWAVFile & file, const void * buf, PINDEX & len);

  protected:
    unsigned short g7231;
    BYTE cacheBuffer[24];
    PINDEX cacheLen;
    PINDEX cachePos;
};

void PWAVFileFormatG7231::CreateHeader(PWAV::FMTChunk & wavFmtChunk, PBYTEArray & extendedHeader)
{
  wavFmtChunk.hdr.len         = sizeof(wavFmtChunk) - sizeof(wavFmtChunk.hdr) + sizeof(sizeof(G7231ExtendedInfo));
  wavFmtChunk.format          = g7231;
  wavFmtChunk.numChannels     = 1;
  wavFmtChunk.sampleRate      = 8000;
  wavFmtChunk.bytesPerSample  = 24;
  wavFmtChunk.bitsPerSample   = 0;
  wavFmtChunk.bytesPerSec     = 800;

  extendedHeader.SetSize(sizeof(G7231ExtendedInfo));
  G7231ExtendedInfo * g7231Info = (G7231ExtendedInfo *)extendedHeader.GetPointer(sizeof(G7231ExtendedInfo));

  g7231Info->data1 = 1;
  g7231Info->data2 = 480;
}

BOOL PWAVFileFormatG7231::WriteExtraChunks(PWAVFile & file)
{
  // write the fact chunk
  G7231FACTChunk factChunk;
  memcpy(factChunk.hdr.tag, "FACT", 4);
  factChunk.hdr.len = sizeof(factChunk) - sizeof(factChunk.hdr);
  factChunk.data1 = 0;
  return file.FileWrite(&factChunk, sizeof(factChunk));
}

static PINDEX G7231FrameSizes[4] = { 24, 20, 4, 1 };

void PWAVFileFormatG7231::OnStart()
{
  cacheLen = cachePos = 0;
}

BOOL PWAVFileFormatG7231::Read(PWAVFile & file, void * origData, PINDEX & origLen)
{
  // Note that Microsoft && VivoActive G.2723.1 codec cannot do SID frames, so
  // we must parse the data and remove SID frames
  // also note that frames are always written as 24 byte frames, so each frame must be unpadded

  PINDEX bytesRead = 0;
  while (bytesRead < origLen) {

    // keep reading until we find a 20 or 24 byte frame
    while (cachePos == cacheLen) {
      if (!file.FileRead(cacheBuffer, 24))
        return FALSE;

      // calculate actual length of frame
      PINDEX frameLen = G7231FrameSizes[cacheBuffer[0] & 3];
      if (frameLen == 20 || frameLen == 24) {
        cacheLen = frameLen;
        cachePos = 0;
      }
    }

    // copy data to requested buffer
    PINDEX copyLen = PMIN(origLen-bytesRead, cacheLen-cachePos);
    memcpy(origData, cacheBuffer+cachePos, copyLen);
    origData = copyLen + (char *)origData;
    cachePos += copyLen;
    bytesRead += copyLen;
  }

  origLen = bytesRead;

  return TRUE;
}

BOOL PWAVFileFormatG7231::Write(PWAVFile & file, const void * origData, PINDEX & len)
{
  // Note that Microsoft && VivoActive G.2723.1 codec cannot do SID frames, so
  // we must parse the data and remove SID frames
  // also note that frames are always written as 24 byte frames, so each frame must be padded
  PINDEX written = 0;

  BYTE frameBuffer[24];
  while (len > 0) {

    // calculate actual length of frame
    PINDEX frameLen = G7231FrameSizes[(*(char *)origData) & 3];
    if (len < frameLen)
      return FALSE;

    // we can write 24 byte frame straight out, 
    // 20 byte frames need to be reblocked
    // we ignore any other frames

    const void * buf = NULL;
    switch (frameLen) {
      case 24:
        buf = origData;
        break;
      case 20:
        memcpy(frameBuffer, origData, 20);
        buf = frameBuffer;
        break;
      default:
        break;
    }

    if (buf != NULL && !file.FileWrite(buf, 24))
      return FALSE;
    else
      written += 24;

    origData = (char *)origData + frameLen;
    len -= frameLen;
  }

  len = written;

  return TRUE;
}

class PWAVFileFormatG7231_vivo : public PWAVFileFormatG7231
{
  public:
    PWAVFileFormatG7231_vivo()
      : PWAVFileFormatG7231(PWAVFile::fmt_VivoG7231) { }
    virtual ~PWAVFileFormatG7231_vivo() {}
    unsigned GetFormat() const
    { return PWAVFile::fmt_VivoG7231; }
    PString GetDescription() const
    { return GetFormatString() & "Vivo"; }
};

PWAVFileFormatByIDFactory::Worker<PWAVFileFormatG7231_vivo> g7231VivoWAVFormat(PWAVFile::fmt_VivoG7231);
PWAVFileFormatByFormatFactory::Worker<PWAVFileFormatG7231_vivo> g7231FormatWAVFormat("G.723.1");

class PWAVFileFormatG7231_ms : public PWAVFileFormatG7231
{
  public:
    PWAVFileFormatG7231_ms()
      : PWAVFileFormatG7231(PWAVFile::fmt_MSG7231) { }
    virtual ~PWAVFileFormatG7231_ms() {}
    unsigned GetFormat() const
    { return PWAVFile::fmt_MSG7231; }
    PString GetDescription() const
    { return GetFormatString() & "MS"; }
};

PWAVFileFormatByIDFactory::Worker<PWAVFileFormatG7231_ms> g7231MSWAVFormat(PWAVFile::fmt_MSG7231);

//////////////////////////////////////////////////////////////////

class PWAVFileConverterPCM : public PWAVFileConverter
{
  public:
    virtual ~PWAVFileConverterPCM() {}
    unsigned GetFormat    (const PWAVFile & file) const;
    off_t GetPosition     (const PWAVFile & file) const;
    BOOL SetPosition      (PWAVFile & file, off_t pos, PFile::FilePositionOrigin origin);
    unsigned GetSampleSize(const PWAVFile & file) const;
    off_t GetDataLength   (PWAVFile & file);
    BOOL Read             (PWAVFile & file, void * buf, PINDEX len);
    BOOL Write            (PWAVFile & file, const void * buf, PINDEX len);
};

unsigned PWAVFileConverterPCM::GetFormat(const PWAVFile &) const
{
  return PWAVFile::fmt_PCM;
}

off_t PWAVFileConverterPCM::GetPosition(const PWAVFile & file) const
{
  off_t pos = file.RawGetPosition();
  return pos * 2;
}

BOOL PWAVFileConverterPCM::SetPosition(PWAVFile & file, off_t pos, PFile::FilePositionOrigin origin)
{
  pos /= 2;
  return file.SetPosition(pos, origin);
}

unsigned PWAVFileConverterPCM::GetSampleSize(const PWAVFile &) const
{
  return 16;
}

off_t PWAVFileConverterPCM::GetDataLength(PWAVFile & file)
{
  return file.RawGetDataLength() * 2;
}

BOOL PWAVFileConverterPCM::Read(PWAVFile & file, void * buf, PINDEX len)
{
  if (file.wavFmtChunk.bitsPerSample == 16)
    return file.PWAVFile::RawRead(buf, len);

  if (file.wavFmtChunk.bitsPerSample != 8) {
    PTRACE(1, "PWAVFile\tAttempt to read autoconvert PCM data with unsupported number of bits per sample " << (int)file.wavFmtChunk.bitsPerSample);
    return FALSE;
  }

  // read the PCM data with 8 bits per sample
  PINDEX samples = (len / 2);
  PBYTEArray pcm8;
  if (!file.PWAVFile::RawRead(pcm8.GetPointer(samples), samples))
    return FALSE;

  // convert to PCM-16
  PINDEX i;
  short * pcmPtr = (short *)buf;
  for (i = 0; i < samples; i++)
    *pcmPtr++ = (unsigned short)((pcm8[i] << 8) - 0x8000);

  // fake the lastReadCount
  file.SetLastReadCount(len);

  return TRUE;
}


BOOL PWAVFileConverterPCM::Write(PWAVFile & file, const void * buf, PINDEX len)
{
  if (file.wavFmtChunk.bitsPerSample == 16)
    return file.PWAVFile::RawWrite(buf, len);

  PTRACE(1, "PWAVFile\tAttempt to write autoconvert PCM data with unsupported number of bits per sample " << (int)file.wavFmtChunk.bitsPerSample);
  return FALSE;
}

PWAVFileConverterFactory::Worker<PWAVFileConverterPCM> pcmConverter(PWAVFile::fmt_PCM, true);

//////////////////////////////////////////////////////////////////
