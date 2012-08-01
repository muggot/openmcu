/*
 * memfile.h
 *
 * WAV file I/O channel class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2002 Equivalence Pty. Ltd.
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
 * Equivalence Pty Ltd
 *
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: memfile.h,v $
 * Revision 1.7  2007/04/18 23:49:50  csoutheren
 * Add usage of precompiled headers
 *
 * Revision 1.6  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.5  2004/11/11 07:34:50  csoutheren
 * Added #include <ptlib.h>
 *
 * Revision 1.4  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.3  2002/08/05 05:40:45  robertj
 * Fixed missing pragma interface/implementation
 *
 * Revision 1.2  2002/06/27 03:53:35  robertj
 * Cleaned up documentation and added Compare() function.
 *
 * Revision 1.1  2002/06/26 09:01:19  craigs
 * Initial version
 *
 */

#ifndef _PMEMFILE
#define _PMEMFILE

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

/**This class is used to allow a block of memory to substitute for a disk file.
 */
class PMemoryFile : public PFile
{
  PCLASSINFO(PMemoryFile, PFile);
  public:
  /**@name Construction */
  //@{
    /**Create a new, empty, memory file.
      */
    PMemoryFile();

    /**Create a new memory file initialising to the specified content.
      */
    PMemoryFile(
      const PBYTEArray & data  ///< New content filr memory file.
    );
  //@}


  /**@name Overrides from class PObject */
  //@{
    /**Determine the relative rank of the two objects. This is essentially the
       string comparison of the #PFilePath# names of the files.

       @return
       relative rank of the file paths.
     */
    Comparison Compare(
      const PObject & obj   ///< Other file to compare against.
    ) const;
  //@}


  /**@name Overrides from class PChannel */
  //@{
    /**Low level read from the memory file channel. The read timeout is
       ignored.  The GetLastReadCount() function returns the actual number
       of bytes read.

       The GetErrorCode() function should be consulted after Read() returns
       FALSE to determine what caused the failure.

       @return
       TRUE indicates that at least one character was read from the channel.
       FALSE means no bytes were read due to timeout or some other I/O error.
     */
    virtual BOOL Read(
      void * buf,   ///< Pointer to a block of memory to receive the read bytes.
      PINDEX len    ///< Maximum number of bytes to read into the buffer.
    );

    /**Low level write to the memory file channel. The write timeout is
       ignored. The GetLastWriteCount() function returns the actual number
       of bytes written.

       The GetErrorCode() function should be consulted after Write() returns
       FALSE to determine what caused the failure.

       @return TRUE if at least len bytes were written to the channel.
     */
    virtual BOOL Write(
      const void * buf, ///< Pointer to a block of memory to write.
      PINDEX len        ///< Number of bytes to write.
    );
  //@}


  /**@name Overrides from class PFile */
  //@{
    /**Get the current size of the file.
       The size of the file corresponds to the size of the data array.

       @return
       length of file in bytes.
     */
    off_t GetLength() const;
      
    /**Set the size of the file, padding with 0 bytes if it would require
       expanding the file, or truncating it if being made shorter.

       @return
       TRUE if the file size was changed to the length specified.
     */
    BOOL SetLength(
      off_t len   ///< New length of file.
    );

    /**Set the current active position in the file for the next read or write
       operation. The #pos# variable is a signed number which is
       added to the specified origin. For #origin == PFile::Start#
       only positive values for #pos# are meaningful. For
       #origin == PFile::End# only negative values for
       #pos# are meaningful.

       @return
       TRUE if the new file position was set.
     */
    BOOL SetPosition(
      off_t pos,                         ///< New position to set.
      FilePositionOrigin origin = Start  ///< Origin for position change.
    );

    /**Get the current active position in the file for the next read or write
       operation.

       @return
       current file position relative to start of file.
     */
    off_t GetPosition() const;
  //@}


  /**@name Overrides from class PFile */
  //@{
    /**Get the memory data the file has operated with.
      */
    const PBYTEArray & GetData() const { return data; }
  //@}


  protected:
    PBYTEArray data;
    off_t position;
};


#endif // _PMEMFILE


// End of File ///////////////////////////////////////////////////////////////

