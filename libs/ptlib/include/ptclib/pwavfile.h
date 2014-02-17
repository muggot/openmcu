/*
 * pwavfile.h
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
 * $Log: pwavfile.h,v $
 * Revision 1.31  2007/05/09 12:20:15  csoutheren
 * Applied 1712697 - WAVE form registration number for G.729A
 * Thanks to Borko Jandras
 *
 * Revision 1.30  2007/04/23 01:15:14  csoutheren
 * Removed warnings on Windows
 *
 * Revision 1.29  2007/04/20 07:59:29  csoutheren
 * Applied 1675658 - various pwavfile.[h|cxx] improvments
 * Thanks to Drazen Dimoti
 *
 * Revision 1.28  2007/04/19 04:33:53  csoutheren
 * Fixed problems with pre-compiled headers
 *
 * Revision 1.27  2006/08/08 08:14:59  rjongbloed
 * Fixed GCC warnings on packed structures that do not need packing.
 *
 * Revision 1.26  2006/06/21 05:19:38  csoutheren
 * Fixed build with latest PWLib
 *
 * Revision 1.25  2006/04/10 23:57:25  csoutheren
 * Checked in changes to remove some warnings with gcc effc++ flag
 *
 * Revision 1.24  2006/04/06 00:39:37  csoutheren
 * Ensure autoconvert format is preserved across file close
 *
 * Revision 1.23  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.22  2005/10/30 23:25:51  csoutheren
 * Fixed formatting
 * Removed throw() declarations (PWLib does not do exceptions)
 * Removed duplicate destructor declarations and definitions
 *
 * Revision 1.21  2005/10/30 19:41:53  dominance
 * fixed most of the warnings occuring during compilation
 *
 * Revision 1.20  2005/06/09 00:33:12  csoutheren
 * Fixed crash problem caused by recent leak fix
 * Removed bogus error when reading all of file contents in a single read
 *
 * Revision 1.19  2005/05/12 05:25:04  csoutheren
 * PWAVFile format abstract factory must use a PCaseless as a key
 *
 * Revision 1.18  2005/03/19 02:52:53  csoutheren
 * Fix warnings from gcc 4.1-20050313 shapshot
 *
 * Revision 1.17  2005/01/04 07:44:02  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.16  2004/11/11 07:34:50  csoutheren
 * Added #include <ptlib.h>
 *
 * Revision 1.15  2004/11/08 04:07:40  csoutheren
 * Fixed crash opportunity under some conditions
 * Fixed incorrect WAV file type display
 *
 * Revision 1.14  2004/07/15 03:12:41  csoutheren
 * Migrated changes from crs_vxnml_devel branch into main trunk
 *
 * Revision 1.13.4.4  2004/07/13 08:13:04  csoutheren
 * Lots of implementation of factory-based PWAVFile
 *
 * Revision 1.13.4.3  2004/07/12 09:17:19  csoutheren
 * Fixed warnings and errors under Linux
 *
 * Revision 1.13.4.2  2004/07/12 08:30:16  csoutheren
 * More fixes for abstract factory implementation of PWAVFile
 *
 * Revision 1.13.4.1  2004/07/07 07:07:41  csoutheren
 * Changed PWAVFile to use abstract factories (extensively)
 * Removed redundant blocking/unblocking when using G.723.1
 * More support for call transfer
 *
 * Revision 1.13  2003/03/07 06:12:05  robertj
 * Added more WAV file "magic numbers".
 *
 * Revision 1.12  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.11  2002/06/20 00:51:38  craigs
 * Added virtuals to allow overriding
 *
 * Revision 1.10  2002/05/21 01:56:53  robertj
 * Removed the enum which made yet another set of magic numbers for audio
 *   formats, now uses the WAV file format numbers.
 * Fixed failure to write header when destroying object without and explicit
 *   call to Close().
 * Fixed missing Open() function which does not have file name parameter.
 * Added ability to set the audio format after construction.
 *
 * Revision 1.9  2002/01/22 03:55:07  craigs
 * Added #define guards when file moved to PTCLib
 *
 * Revision 1.8  2002/01/13 21:00:41  rogerh
 * The type of new .WAV files must now be specified in the class constructor.
 * Take out Open() function from the last commit and create a new Open()
 * function which replaces the one in the PFile base class.
 *
 * Revision 1.7  2002/01/11 16:33:46  rogerh
 * Create a PWAVFile Open() function, which processes the WAV header
 *
 * Revision 1.6  2001/10/16 13:27:37  rogerh
 * Add support for writing G.723.1 WAV files.
 * MS Windows can play G.723.1 WAV Files in Media Player and Sound Recorder.
 * Sound Recorder can also convert them to normal PCM format WAV files.
 * Thanks go to M.Stoychev <M.Stoychev@cnsys.bg> for sample WAV files.
 *
 * Revision 1.5  2001/10/15 11:48:15  rogerh
 * Add GetFormat to return the format of a WAV file
 *
 * Revision 1.4  2001/07/23 01:20:20  rogerh
 * Add updates from Shawn - ensure isvalidWAV is false for zero length files.
 * GetDataLength uses actual file size to support file updates as well as appends.
 * Add updates from Roger - Update Header() just writes to specific fields which
 * preserves any 'extra' data in an existing header between FORMAT and DATA chunks.
 *
 * Revision 1.3  2001/07/20 07:06:27  rogerh
 * Fix a typo
 *
 * Revision 1.2  2001/07/20 03:30:59  robertj
 * Minor cosmetic changes to new PWAVFile class.
 *
 * Revision 1.1  2001/07/19 09:55:48  rogerh
 * Add PWAVFile, a class to read and write .wav files, written by
 * Roger Hardiman and <roger@freebsd.org> and
 * Shawn Pai-Hsiang Hsiao <shawn@eecs.harvard.edu>
 *
 *
 */

#ifndef _PWAVFILE
#define _PWAVFILE

//#ifdef P_USE_PRAGMA
//#pragma interface
//#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#include <ptlib/pfactory.h>

class PWAVFile;

namespace PWAV {

#ifdef __GNUC__
#define P_PACKED    __attribute__ ((packed));
#else
#define P_PACKED
#pragma pack(1)
#endif

struct ChunkHeader
{
  char    tag[4];
  PInt32l len    P_PACKED;
};

struct RIFFChunkHeader 
{
  ChunkHeader hdr;
  char        tag[4];
};

struct FMTChunk
{
  ChunkHeader hdr;                    ///< chunk header (already packed)
  PUInt16l format          P_PACKED;  ///< Format 
  PUInt16l numChannels     P_PACKED;  ///< Channels 0x01 = mono, 0x02 = stereo
  PUInt32l sampleRate      P_PACKED;  ///< Sample Rate in Hz
  PUInt32l bytesPerSec     P_PACKED;  ///< Average bytes Per Second
  PUInt16l bytesPerSample  P_PACKED;  ///< Bytes Per Sample, eg 2
  PUInt16l bitsPerSample   P_PACKED;  ///< Bits Per Sample, eg 16
};

}; // namespace PWAV

#ifdef __GNUC__
#undef P_PACKED
#else
#pragma pack()
#endif

/**
  * abstract factory class for handling WAV files formats
  */
class PWAVFileFormat
{
  public:
    virtual ~PWAVFileFormat() { }

    /**
      * return a PWAVFile format code
      */
    virtual unsigned GetFormat() const = 0;

    /**
      * return a string that can be used as a media format
      */
    virtual PString GetFormatString() const = 0;

    /**
     *  return a string that can be used as a text description
     */
    virtual PString GetDescription() const = 0;

    /**
     *  populate the header with the correct values
     */
    virtual void CreateHeader(PWAV::FMTChunk & header, PBYTEArray & extendedHeader) = 0;

    /**
     *  populate the header with the correct values after initial parameters are set
     */
    virtual void UpdateHeader(PWAV::FMTChunk & /*header*/, PBYTEArray & /*extendedHeader*/)
    { }

    /**
      * write any extra headers after the FORMAT chunk
      */
    virtual BOOL WriteExtraChunks(PWAVFile & /*file*/)
    { return TRUE; }

    /**
      * read any extra headers after the FORMAT chunk
      */
    virtual BOOL ReadExtraChunks(PWAVFile & /*file*/)
    { return TRUE; }

     /**
      * called before the reading/writing starts
      */
    virtual void OnStart()
    { }

     /**
      * called after the reading/writing stops
      */
    virtual void OnStop()
    { }

    /**
      * write data to the file
      */
    virtual BOOL Read(PWAVFile & file, void * buf, PINDEX & len);

    /**
      * read data from the file
      */
    virtual BOOL Write(PWAVFile & file, const void * buf, PINDEX & len);
};

typedef PFactory<PWAVFileFormat, PCaselessString> PWAVFileFormatByFormatFactory;
typedef PFactory<PWAVFileFormat, unsigned> PWAVFileFormatByIDFactory;

/**
  * abstract factory class for autoconversion of WAV files to/from PCM-16
  */
class PWAVFileConverter 
{
  public:
    virtual ~PWAVFileConverter() { }
    virtual unsigned GetFormat    (const PWAVFile & file) const = 0;
    virtual off_t GetPosition     (const PWAVFile & file) const = 0;
    virtual BOOL SetPosition      (PWAVFile & file, off_t pos, PFile::FilePositionOrigin origin) = 0;
    virtual unsigned GetSampleSize(const PWAVFile & file) const = 0;
    virtual off_t GetDataLength   (PWAVFile & file) = 0;
    virtual BOOL Read             (PWAVFile & file, void * buf, PINDEX len)  = 0;
    virtual BOOL Write            (PWAVFile & file, const void * buf, PINDEX len) = 0;
};

typedef PFactory<PWAVFileConverter, unsigned> PWAVFileConverterFactory;

/**A class representing a WAV audio file.
  */
class PWAVFile : public PFile
{
  PCLASSINFO(PWAVFile, PFile);

  public:
  /**@name Construction */
  //@{
    /**When a file is opened for writing, we can specify if this is a PCM
       wav file or a G.723.1 wav file.
     */
    enum {
      fmt_PCM         = 1,      ///< PCM, 8kHz, 16 bit, mono
      fmt_MSADPCM     = 2,      ///< MS-ADPCM, 8kHz, mono
      fmt_ALaw        = 6,      ///< A-Law 8kHz
      fmt_uLaw        = 7,      ///< u-Law 8kHz
      fmt_VOXADPCM    = 0x10,   ///< OKI ADPCM
      fmt_IMAADPCM    = 0x11,   ///< IMA-ADPCM, 8kHz mono
      fmt_GSM         = 0x31,   ///< GSM
      fmt_G728        = 0x41,   ///< RFC2361
      fmt_G723        = 0x42,   ///< RFC2361
      fmt_MSG7231     = 0x42,   ///< Microsoft G.723.1
      fmt_G726        = 0x64,   ///< RFC2361
      fmt_G722        = 0x65,   ///< RFC2361
      fmt_G729        = 0x83,   ///< RFC2361
      fmt_VivoG7231   = 0x111,  ///< VivoActive G.723.1

      // For backward compatibility
      PCM_WavFile     = fmt_PCM,
      G7231_WavFile   = fmt_VivoG7231,

      // allow opening files without knowing the format
      fmt_NotKnown    = 0x10000
    };

    /**Create a WAV file object but do not open it. It does not
       initially have a valid file name. However, an attempt to open the file
       using the #PFile::Open()# function will generate a unique
       temporary file.

       If a WAV file is being created, the type parameter can be used
       to create a PCM Wave file or a G.723.1 Wave file by using
       #WaveType enum#
     */
    PWAVFile(
      unsigned format = fmt_PCM ///< Type of WAV File to create
    );
    static PWAVFile * format(
      const PString & format    ///< Type of WAV File to create
    );

    /**Create a unique temporary file name, and open the file in the specified
       mode and using the specified options. Note that opening a new, unique,
       temporary file name in ReadOnly mode will always fail. This would only
       be usefull in a mode and options that will create the file.

       If a WAV file is being created, the type parameter can be used
       to create a PCM Wave file or a G.723.1 Wave file by using
       #WaveType enum#

       The #PChannel::IsOpen()# function may be used after object
       construction to determine if the file was successfully opened.
     */
    PWAVFile(
      OpenMode mode,          ///< Mode in which to open the file.
      int opts = ModeDefault, ///< #OpenOptions enum# for open operation.
      unsigned format = fmt_PCM ///< Type of WAV File to create
    );
    static PWAVFile * format(
      const PString & format,  ///< Type of WAV File to create
      PFile::OpenMode mode,          ///< Mode in which to open the file.
      int opts = PFile::ModeDefault ///< #OpenOptions enum# for open operation.
    );

    /**Create a WAV file object with the specified name and open it in
       the specified mode and with the specified options.
       If a WAV file is being created, the type parameter can be used
       to create a PCM Wave file or a G.723.1 Wave file by using
       #WaveType enum#

       The #PChannel::IsOpen()# function may be used after object
       construction to determine if the file was successfully opened.
     */
    PWAVFile(
      const PFilePath & name,     ///< Name of file to open.
      OpenMode mode = ReadWrite,  ///< Mode in which to open the file.
      int opts = ModeDefault,     ///< #OpenOptions enum# for open operation.
      unsigned format = fmt_PCM ///< Type of WAV File to create
    );
    PWAVFile(
      const PString & format,  ///< Type of WAV File to create
      const PFilePath & name,     ///< Name of file to open.
      OpenMode mode = PFile::ReadWrite,  ///< Mode in which to open the file.
      int opts = PFile::ModeDefault     ///< #OpenOptions enum# for open operation.
    );

    /**Close the file before destruction.
      */
    ~PWAVFile();
  //@}

  /**@name Overrides from class PFile */
  //@{
    /**Call PFile::Read() to read in audio data and perform necessary
       processing such as byte-order swaping.

       @return
       TRUE indicates that at least one character was read from the channel.
       FALSE means no bytes were read due to timeout or some other I/O error.
    */
    virtual BOOL Read(
      void * buf,   ///< Pointer to a block of memory to receive the read bytes.
      PINDEX len    ///< Maximum number of bytes to read into the buffer.
    );

    /**Call PFile::Write() to write out audio data and perform necessary
       processing such as byte-order swaping.

       @return
       TRUE indicates that at least one character was written to the channel.
       FALSE means no bytes were written due to timeout or some other I/O error.
    */
    virtual BOOL Write(
      const void * buf,   ///< Pointer to a block of memory to receive the write bytes.
      PINDEX len    ///< Maximum number of bytes to write to the channel.
    );

    /**Open the current file in the specified mode and with
       the specified options. If the file object already has an open file then
       it is closed.
       
       If there has not been a filename attached to the file object (via
       #SetFilePath()#, the #name# parameter or a previous
       open) then a new unique temporary filename is generated.

       @return
       TRUE if the file was successfully opened.
     */
    virtual BOOL Open(
      OpenMode mode = ReadWrite,  ///< Mode in which to open the file.
      int opts = ModeDefault      ///< Options for open operation.
    );

    /**Open the specified WAV file name in the specified mode and with
       the specified options. If the file object already has an open file then
       it is closed.
       This reads (and validates) the header for existing files.
       For new files, it creates a new file (and header) using the type of
       WAV file specified in the class constructor.
       
       Note: if #mode# is StandardInput, StandardOutput or StandardError,   
       then the #name# parameter is ignored.

       @return
       TRUE if the file was successfully opened.
     */
    virtual BOOL Open(
      const PFilePath & name,    ///< Name of file to open.
      OpenMode mode = ReadWrite, ///< Mode in which to open the file.
      int opts = ModeDefault     ///< #OpenOptions enum# for open operation.
    );

    /** Close the file channel.
        If a WAV file has been written to, this will update the header
        to contain the correct size information.
        @return TRUE if close was OK.
      */
    virtual BOOL Close();

    /**Set the current active position in the file for the next read or write
       operation. The #pos# variable is a signed number which is
       added to the specified origin. For #origin == PFile::Start#
       only positive values for #pos# are meaningful. For
       #origin == PFile::End# only negative values for
       #pos# are meaningful.

       Note that for a WAV file, the origin of the file is right after
       the header. That is, the WAV header is not included when
       perform SetPosition().

       @return
       TRUE if the new file position was set.
     */
    virtual BOOL SetPosition(
      off_t pos,                         ///< New position to set.
      FilePositionOrigin origin = Start  ///< Origin for position change.
    );

    /**Get the current active position in the file for the next read
       or write operation. The WAV header is excluded from calculation
       the position.

       @return
       current file position relative to the end of the WAV header.
     */
    virtual off_t GetPosition() const;
  //@}

  /**@name Member variable access */
  //@{
    /**Find out the format of the WAV file. Eg 0x01 for PCM, 0x42 or 0x111 for G.723.1.
    */
    virtual BOOL SetFormat(unsigned fmt);
    virtual BOOL SetFormat(const PString & format);

    /**Find out the format of the WAV file. Eg 0x01 for PCM, 0x42 or 0x111 for G.723.1.
    */
    virtual unsigned GetFormat() const;
    virtual PString GetFormatAsString() const;

    /**Find out the number of channels the WAV file has. Typically this is 1 for
       mono and 2 for stereo.
    */
    virtual unsigned GetChannels() const;
    virtual void SetChannels(unsigned v);

    /**Find out the sample rate of the WAV file in Hz.
    */
    virtual unsigned GetSampleRate() const;
    virtual void SetSampleRate(unsigned v);

    /**Find out how may bits there are per sample, eg 8 or 16.
    */
    virtual unsigned GetSampleSize() const;
    virtual void SetSampleSize(unsigned v);

    /**Find out how may bytes there are per second
    */
    virtual unsigned GetBytesPerSecond() const;
    virtual void SetBytesPerSecond(unsigned v);

    /**Find out the size of WAV header presented in the file.
    */
    off_t GetHeaderLength() const;

    /**Find out how many bytes of audio data there are.
    */
    virtual off_t GetDataLength();

    /**Determine if the WAV file is a valid wave file.

      @return
      TRUE indicates that the WAV file is valid
      FALSE indicates that the WAV file is invalid
    */
    BOOL IsValid() const { return isValidWAV; }

    /**
      *Return a string that describes the WAV format
      */
    PString GetFormatString() const
    { if (formatHandler == NULL) return PString("N/A"); else return formatHandler->GetFormatString(); }

    /**
      * enable autoconversion between PCM-16 and the native format
      */
    void SetAutoconvert();

  //@}
 
    friend class PWAVFileConverter;

    BOOL RawRead(void * buf, PINDEX len);
    BOOL RawWrite(const void * buf, PINDEX len);

    BOOL FileRead(void * buf, PINDEX len);
    BOOL FileWrite(const void * buf, PINDEX len);

    off_t RawGetPosition() const;
    BOOL RawSetPosition(off_t pos, FilePositionOrigin origin);
    off_t RawGetDataLength();

    void SetLastReadCount(PINDEX v) { lastReadCount = v; } 

    PWAV::FMTChunk wavFmtChunk;
    PBYTEArray extendedHeader;

  protected:
    void Construct();
    void SelectFormat(unsigned fmt);
    void SelectFormat(const PString & format);

    PBYTEArray wavHeaderData;

    BOOL ProcessHeader();
    BOOL GenerateHeader();
    BOOL UpdateHeader();

    BOOL     isValidWAV;

    unsigned int origFmt;
    PWAVFileFormat * formatHandler;

    BOOL     autoConvert;
    PWAVFileConverter * autoConverter;

    off_t lenHeader;
    off_t lenData;

    BOOL     header_needs_updating;
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
