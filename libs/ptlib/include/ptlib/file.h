/*
 * file.h
 *
 * Operating System file I/O channel class.
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
 * $Log: file.h,v $
 * Revision 1.43  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.42  2003/09/26 09:58:50  rogerhardiman
 * Move #include <sys/stat.h> from the unix file.h to the main file.h
 * FreeBSD's sys/stat.h includes extern "C" for some prototypes and you
 * cannot have an extern "C" in the middle of a C++ class
 *
 * Revision 1.41  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.40  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.39  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.38  2002/07/02 08:00:55  craigs
 * Also made GetPosition, SetPosition and SetLength virtual as well
 *
 * Revision 1.37  2002/07/02 07:59:42  craigs
 * Added virtual to GetLength call
 *
 * Revision 1.36  2002/01/13 20:54:55  rogerh
 * Make the other Open() virtual so it can be overridden (eg in PWAVFile)
 *
 * Revision 1.35  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.34  2000/09/27 05:58:07  craigs
 * Added virtual to PFile::Open to allow overriding in descandant classes
 *
 * Revision 1.33  2000/07/09 14:05:46  robertj
 * Added file share options.
 *
 * Revision 1.32  1999/06/13 13:54:07  robertj
 * Added PConsoleChannel class for access to stdin/stdout/stderr.
 *
 * Revision 1.31  1999/06/09 02:05:20  robertj
 * Added ability to open file as standard input, output and error streams.
 *
 * Revision 1.30  1999/03/09 02:59:49  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.29  1999/02/16 08:07:11  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.28  1998/09/23 06:20:35  robertj
 * Added open source copyright license.
 *
 * Revision 1.27  1995/07/31 12:15:43  robertj
 * Removed PContainer from PChannel ancestor.
 *
 * Revision 1.26  1995/06/17 11:12:33  robertj
 * Documentation update.
 *
 * Revision 1.25  1995/04/22 00:43:57  robertj
 * Added Move() function and changed semantics of Rename().
 * Changed all file name strings to PFilePath objects.
 *
 * Revision 1.24  1995/03/14 12:41:23  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.23  1995/03/12  04:37:13  robertj
 * Moved GetHandle() function from PFile to PChannel.
 *
 * Revision 1.22  1995/01/14  06:22:11  robertj
 * Documentation
 *
 * Revision 1.21  1994/12/21  11:52:54  robertj
 * Documentation and variable normalisation.
 *
 * Revision 1.20  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.19  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.18  1994/08/21  23:43:02  robertj
 * Added "remove on close" feature for temporary files.
 * Added "force" option to Remove/Rename etc to override write protection.
 * Added function to set file permissions.
 *
 * Revision 1.17  1994/07/17  10:46:06  robertj
 * Moved data to platform dependent files.
 *
 * Revision 1.16  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.15  1994/04/20  12:17:44  robertj
 * Split name into PFilePath
 *
 * Revision 1.14  1994/04/01  14:11:03  robertj
 * Added const to functions.
 * Added SetName function.
 *
 * Revision 1.13  1994/03/07  07:38:19  robertj
 * Major enhancementsacross the board.
 *
 * Revision 1.12  1994/01/13  03:40:22  robertj
 * Added hidden flag to file info.
 *
 * Revision 1.12  1994/01/13  03:36:48  robertj
 * Created intermediate class PInteractorLayout for dialog-ish windows.
 *
 * Revision 1.11  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.10  1993/12/31  06:45:38  robertj
 * Made inlines optional for debugging purposes.
 *
 * Revision 1.9  1993/09/27  16:35:25  robertj
 * Changed GetName() to GetTitle(), better naming convention.
 * Moved internal functions to private section.
 *
 * Revision 1.8  1993/08/31  03:38:02  robertj
 * Changed PFile::Status to PFile::Info due to X-Windows compatibility.
 *
 * Revision 1.7  1993/08/27  18:17:47  robertj
 * Moved code from MS-DOS platform to common files.
 *
 * Revision 1.6  1993/08/21  04:40:19  robertj
 * Added Copy() function.
 *
 * Revision 1.5  1993/08/21  01:50:33  robertj
 * Made Clone() function optional, default will assert if called.
 *
 * Revision 1.4  1993/08/01  14:05:27  robertj
 * Added GetFileName() function required for proper portability.
 * Improved some comments.
 *
 * Revision 1.3  1993/07/14  12:49:16  robertj
 * Fixed RCS keywords.
 *
 */


#ifndef _PFILE
#define _PFILE

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _WIN32
#include <sys/stat.h>
#endif



///////////////////////////////////////////////////////////////////////////////
// Binary Files

/**This class represents a disk file. This is a particular type of I/O channel
   that has certain attributes. All platforms have a disk file, though exact
   details of naming convertions etc may be different.

   The basic model for files is that they are a named sequence of bytes that
   persists within a directory structure. The transfer of data to and from
   the file is made at a current position in the file. This may be set to
   random locations within the file.
 */
class PFile : public PChannel
{
  PCLASSINFO(PFile, PChannel);

  public:
  /**@name Construction */
  //@{
    /**Create a file object but do not open it. It does not initially have a
       valid file name. However, an attempt to open the file using the
       #Open()# function will generate a unique temporary file.
     */
    PFile();

    /**When a file is opened, it may restrict the access available to
       operations on the object instance. A value from this enum is passed to
       the #Open()# function to set the mode.
     */
    enum OpenMode {
      /// File can be read but not written.
      ReadOnly,
      /// File can be written but not read.
      WriteOnly,
      /// File can be both read and written.
      ReadWrite
    };

    /**When a file is opened, a number of options may be associated with the
       open file. These describe what action to take on opening the file and
       what to do on closure. A value from this enum is passed to the
       #Open()# function to set the options.

       The #ModeDefault# option will use the following values:
\begin{tabular}{rr}
          Mode  & Options   \\
\hline
          #ReadOnly#  &    #MustExist# \\
          #WriteOnly# &    #Create | Truncate# \\
          #ReadWrite# &    #Create# \\
\hline
\end{tabular}
     */
    enum OpenOptions {
      /// File options depend on the OpenMode parameter.
      ModeDefault = -1, 
      /// File open fails if file does not exist.
      MustExist = 0,    
      /// File is created if it does not exist.
      Create = 1,       
      /// File is set to zero length if it already exists.
      Truncate = 2,     
      /// File open fails if file already exists.
      Exclusive = 4,    
      /// File is temporary and is to be deleted when closed.
      Temporary = 8,
      /// File may not be read by another process.
      DenySharedRead = 16,
      /// File may not be written by another process.
      DenySharedWrite = 32
    };

    /**Create a unique temporary file name, and open the file in the specified
       mode and using the specified options. Note that opening a new, unique,
       temporary file name in ReadOnly mode will always fail. This would only
       be usefull in a mode and options that will create the file.

       The #PChannel::IsOpen()# function may be used after object
       construction to determine if the file was successfully opened.
     */
    PFile(
      OpenMode mode,          ///< Mode in which to open the file.
      int opts = ModeDefault  ///< #OpenOptions enum# for open operation.
    );

    /**Create a file object with the specified name and open it in the
       specified mode and with the specified options.

       The #PChannel::IsOpen()# function may be used after object
       construction to determine if the file was successfully opened.
     */
    PFile(
      const PFilePath & name,    ///< Name of file to open.
      OpenMode mode = ReadWrite, ///< Mode in which to open the file.
      int opts = ModeDefault     ///< #OpenOptions enum# for open operation.
    );

    /// Close the file on destruction.
    ~PFile();
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
    /**Get the platform and I/O channel type name of the channel. For example,
       it would return the filename in #PFile# type channels.

       @return
       the name of the channel.
     */
    virtual PString GetName() const;

    /**Low level read from the file channel. The read timeout is ignored for
       file I/O. The GetLastReadCount() function returns the actual number
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

    /**Low level write to the file channel. The write timeout is ignored for
       file I/O. The GetLastWriteCount() function returns the actual number
       of bytes written.

       The GetErrorCode() function should be consulted after Write() returns
       FALSE to determine what caused the failure.

       @return TRUE if at least len bytes were written to the channel.
     */
    virtual BOOL Write(
      const void * buf, ///< Pointer to a block of memory to write.
      PINDEX len        ///< Number of bytes to write.
    );

    /** Close the file channel.
        @return TRUE if close was OK.
      */
    virtual BOOL Close();
  //@}


  /**@name File manipulation functions */
  //@{
    /**Check for file existance. 
       Determine if the file specified actually exists within the platforms
       file system.

       @return
       TRUE if the file exists.
     */
    static BOOL Exists(
      const PFilePath & name  ///< Name of file to see if exists.
    );

    /**Check for file existance.
       Determine if the file path specification associated with the instance
       of the object actually exists within the platforms file system.

       @return
       TRUE if the file exists.
     */
    BOOL Exists() const;

    /**Check for file access modes.
       Determine if the file specified may be opened in the specified mode. This would
       check the current access rights to the file for the mode. For example,
       for a file that is read only, using mode == ReadWrite would return
       FALSE but mode == ReadOnly would return TRUE.

       @return
       TRUE if a file open would succeed.
     */
    static BOOL Access(
      const PFilePath & name, ///< Name of file to have its access checked.
      OpenMode mode         ///< Mode in which the file open would be done.
    );

    /**Check for file access modes.
       Determine if the file path specification associated with the
       instance of the object may be opened in the specified mode. This would
       check the current access rights to the file for the mode. For example,
       for a file that is read only, using mode == ReadWrite would return
       FALSE but mode == ReadOnly would return TRUE.

       @return
       TRUE if a file open would succeed.
     */
    BOOL Access(
      OpenMode mode         ///< Mode in which the file open would be done.
    );

    /**Delete the specified file. If #force# is FALSE and the file
       is protected against being deleted then the function fails. If
       #force# is TRUE then the protection is ignored. What
       constitutes file deletion protection is platform dependent, eg on DOS
       is the Read Only attribute and on a Novell network it is a Delete
       trustee right. Some protection may not be able to overridden with the
       #force# parameter at all, eg on a Unix system and you are
       not the owner of the file.

       @return
       TRUE if the file was deleted.
     */
    static BOOL Remove(
      const PFilePath & name,   // Name of file to delete.
      BOOL force = FALSE      // Force deletion even if file is protected.
    );

    /**Delete the current file. If #force# is FALSE and the file
       is protected against being deleted then the function fails. If
       #force# is TRUE then the protection is ignored. What
       constitutes file deletion protection is platform dependent, eg on DOS
       is the Read Only attribute and on a Novell network it is a Delete
       trustee right. Some protection may not be able to overridden with the
       #force# parameter at all, eg on a Unix system and you are
       not the owner of the file.

       @return
       TRUE if the file was deleted.
     */
    BOOL Remove(
      BOOL force = FALSE      // Force deletion even if file is protected.
    );

    /**Change the specified files name. This does not move the file in the
       directory hierarchy, it only changes the name of the directory entry.

       The #newname# parameter must consist only of the file name
       part, as returned by the #PFilePath::GetFileName()# function. Any
       other file path parts will cause an error.

       The first form uses the file path specification associated with the
       instance of the object. The name within the instance is changed to the
       new name if the function succeeds. The second static function uses an
       arbitrary file specified by name.

       @return
       TRUE if the file was renamed.
     */
    static BOOL Rename(
      const PFilePath & oldname,  ///< Old name of the file.
      const PString & newname,    ///< New name for the file.
      BOOL force = FALSE
        ///< Delete file if a destination exists with the same name.
    );

    /**Change the current files name.
       This does not move the file in the
       directory hierarchy, it only changes the name of the directory entry.

       The #newname# parameter must consist only of the file name
       part, as returned by the #PFilePath::GetFileName()# function. Any
       other file path parts will cause an error.

       The first form uses the file path specification associated with the
       instance of the object. The name within the instance is changed to the
       new name if the function succeeds. The second static function uses an
       arbitrary file specified by name.

       @return
       TRUE if the file was renamed.
     */
    BOOL Rename(
      const PString & newname,  ///< New name for the file.
      BOOL force = FALSE
        ///< Delete file if a destination exists with the same name.
    );

    /**Make a copy of the specified file.

       @return
       TRUE if the file was renamed.
     */
    static BOOL Copy(
      const PFilePath & oldname,  ///< Old name of the file.
      const PFilePath & newname,  ///< New name for the file.
      BOOL force = FALSE
        ///< Delete file if a destination exists with the same name.
    );

    /**Make a copy of the current file.

       @return
       TRUE if the file was renamed.
     */
    BOOL Copy(
      const PFilePath & newname,  ///< New name for the file.
      BOOL force = FALSE
        ///< Delete file if a destination exists with the same name.
    );

    /**Move the specified file. This will move the file from one position in
       the directory hierarchy to another position. The actual operation is
       platform dependent but  the reslt is the same. For instance, for Unix,
       if the move is within a file system then a simple rename is done, if
       it is across file systems then a copy and a delete is performed.

       @return
       TRUE if the file was moved.
     */
    static BOOL Move(
      const PFilePath & oldname,  ///< Old path and name of the file.
      const PFilePath & newname,  ///< New path and name for the file.
      BOOL force = FALSE
        ///< Delete file if a destination exists with the same name.
    );

    /**Move the current file. This will move the file from one position in
       the directory hierarchy to another position. The actual operation is
       platform dependent but  the reslt is the same. For instance, for Unix,
       if the move is within a file system then a simple rename is done, if
       it is across file systems then a copy and a delete is performed.

       @return
       TRUE if the file was moved.
     */
    BOOL Move(
      const PFilePath & newname,  ///< New path and name for the file.
      BOOL force = FALSE
        ///< Delete file if a destination exists with the same name.
    );
  //@}

  /**@name File channel functions */
  //@{
    /**Get the full path name of the file. The #PFilePath# object
       describes the full file name specification for the particular platform.

       @return
       the name of the file.
     */
    const PFilePath & GetFilePath() const;

    /**Set the full path name of the file. The #PFilePath# object
       describes the full file name specification for the particular platform.
     */
    void SetFilePath(
      const PString & path    ///< New file path.
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
      OpenMode mode = ReadWrite,  // Mode in which to open the file.
      int opts = ModeDefault      // Options for open operation.
    );

    /**Open the specified file name in the specified mode and with
       the specified options. If the file object already has an open file then
       it is closed.
       
       Note: if #mode# is StandardInput, StandardOutput or StandardError,
       then the #name# parameter is ignored.

       @return
       TRUE if the file was successfully opened.
     */
    virtual BOOL Open(
      const PFilePath & name,    // Name of file to open.
      OpenMode mode = ReadWrite, // Mode in which to open the file.
      int opts = ModeDefault     // #OpenOptions enum# for open operation.
    );
      
    /**Get the current size of the file.

       @return
       length of file in bytes.
     */
    virtual off_t GetLength() const;
      
    /**Set the size of the file, padding with 0 bytes if it would require
       expanding the file, or truncating it if being made shorter.

       @return
       TRUE if the file size was changed to the length specified.
     */
    virtual BOOL SetLength(
      off_t len   // New length of file.
    );

    /// Options for the origin in setting the file position.
    enum FilePositionOrigin {
      /// Set position relative to start of file.
      Start = SEEK_SET,   
      /// Set position relative to current file position.
      Current = SEEK_CUR, 
      /// Set position relative to end of file.
      End = SEEK_END      
    };

    /**Set the current active position in the file for the next read or write
       operation. The #pos# variable is a signed number which is
       added to the specified origin. For #origin == PFile::Start#
       only positive values for #pos# are meaningful. For
       #origin == PFile::End# only negative values for
       #pos# are meaningful.

       @return
       TRUE if the new file position was set.
     */
    virtual BOOL SetPosition(
      off_t pos,                         ///< New position to set.
      FilePositionOrigin origin = Start  ///< Origin for position change.
    );

    /**Get the current active position in the file for the next read or write
       operation.

       @return
       current file position relative to start of file.
     */
    virtual off_t GetPosition() const;

    /**Determine if the current file position is at the end of the file. If
       this is TRUE then any read operation will fail.

       @return
       TRUE if at end of file.
     */
    BOOL IsEndOfFile() const;
      
    /**Get information (eg protection, timestamps) on the specified file.

       @return
       TRUE if the file info was retrieved.
     */
    static BOOL GetInfo(
      const PFilePath & name,  // Name of file to get the information on.
      PFileInfo & info
      // #PFileInfo# structure to receive the information.
    );

    /**Get information (eg protection, timestamps) on the current file.

       @return
       TRUE if the file info was retrieved.
     */
    BOOL GetInfo(
      PFileInfo & info
      // #PFileInfo# structure to receive the information.
    );

    /**Set permissions on the specified file.

       @return
       TRUE if the file was renamed.
     */
    static BOOL SetPermissions(
      const PFilePath & name,   // Name of file to change the permission of.
      int permissions           // New permissions mask for the file.
    );
    /**Set permissions on the current file.

       @return
       TRUE if the file was renamed.
     */
    BOOL SetPermissions(
      int permissions           // New permissions mask for the file.
    );
  //@}

  protected:
    // Member variables
    /// The fully qualified path name for the file.
    PFilePath path;

    /// File is to be removed when closed.
    BOOL removeOnClose;


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/file.h"
#else
#include "unix/ptlib/file.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
