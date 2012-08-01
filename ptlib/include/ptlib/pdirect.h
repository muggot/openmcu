/*
 * pdirect.h
 *
 * File system directory class.
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
 * $Log: pdirect.h,v $
 * Revision 1.43  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.42  2004/09/17 03:51:15  csoutheren
 * More fixes for PDirectory problem
 *
 * Revision 1.42  2004/09/15 05:11:39  csoutheren
 * Fixed problem with PDirectory destructor not calling Close()
 * Thanks to Paul Long
 *
 * Revision 1.41  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.40  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.39  2003/06/06 15:04:17  dsandras
 *
 * Fixed compilation warning with gcc 3.3 by removing the PINLINE
 *
 * Revision 1.38  2002/11/20 00:13:43  robertj
 * Fixed some documentation
 *
 * Revision 1.37  2002/11/19 12:07:02  robertj
 * Added function to get root directory.
 *
 * Revision 1.36  2002/11/19 10:34:59  robertj
 * Added function to extract a path as an array of directories components.
 *
 * Revision 1.35  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.34  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.33  2001/02/13 06:55:21  robertj
 * Fixed problem with operator= in PDirectory class, part of larger change previously made.
 *
 * Revision 1.32  2000/06/26 11:17:19  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.31  2000/04/03 18:41:27  robertj
 * Fixed BeOS compatibility problem with openlog() function.
 *
 * Revision 1.30  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.29  1999/02/16 08:11:09  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.28  1998/09/23 06:21:06  robertj
 * Added open source copyright license.
 *
 * Revision 1.27  1998/03/05 12:44:34  robertj
 * Added cluster size.
 *
 * Revision 1.26  1997/03/31 11:34:00  robertj
 * Fixed default permissions for directories , different from that for files.
 *
 * Revision 1.25  1997/01/12 04:22:21  robertj
 * Added function to get disk size and free space.
 *
 * Revision 1.24  1995/12/23 03:45:31  robertj
 * Added constructor for C string literals.
 *
 * Revision 1.23  1995/11/09 12:17:23  robertj
 * Added platform independent base type access classes.
 *
 * Revision 1.22  1995/10/14 15:02:22  robertj
 * Added function to get parent directory.
 *
 * Revision 1.21  1995/06/17 11:12:52  robertj
 * Documentation update.
 *
 * Revision 1.20  1995/03/14 12:42:00  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.19  1995/03/12  04:42:48  robertj
 * Updated documentation.
 * Changed return type of functions to the correct case string.
 *
 * Revision 1.18  1995/02/22  10:50:33  robertj
 * Changes required for compiling release (optimised) version.
 *
 * Revision 1.17  1995/01/06  10:42:25  robertj
 * Moved identifiers into different scope.
 * Changed file size to 64 bit integer.
 * Documentation
 *
 * Revision 1.16  1994/10/24  00:07:03  robertj
 * Changed PFilePath and PDirectory so descends from either PString or
 *     PCaselessString depending on the platform.
 *
 * Revision 1.15  1994/10/23  04:49:25  robertj
 * Chnaged PDirectory to descend of PString.
 * Added PDirectory Exists() function.
 *
 * Revision 1.14  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.13  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.12  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.11  1994/04/20  12:17:44  robertj
 * Split name into PFilePath
 *
 * Revision 1.10  1994/04/11  14:16:27  robertj
 * Added function for determining if character is a valid directory separator.
 *
 * Revision 1.9  1994/04/01  14:14:57  robertj
 * Put platform independent file permissions and type codes back.
 *
 * Revision 1.7  1994/01/13  03:17:55  robertj
 * Added functions to get the name of the volume the directory is contained in
 *    and a function to determine if the directory is a root directory of the
 *    volume (ie is does not have a parent directory).
 *
 * Revision 1.6  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.5  1993/12/31  06:45:38  robertj
 * Made inlines optional for debugging purposes.
 *
 * Revision 1.4  1993/08/21  01:50:33  robertj
 * Made Clone() function optional, default will assert if called.
 *
 * Revision 1.3  1993/07/14  12:49:16  robertj
 * Fixed RCS keywords.
 *
 */


#ifndef _PDIRECTORY
#define _PDIRECTORY

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifdef Fifo
#undef Fifo
#endif

#ifdef _WIN32
#define PDIR_SEPARATOR '\\'
const PINDEX P_MAX_PATH = _MAX_PATH;
typedef PCaselessString PFilePathString;
#else
#define PDIR_SEPARATOR '/'
#define P_MAX_PATH    (_POSIX_PATH_MAX)
typedef PString PFilePathString;
#endif

///////////////////////////////////////////////////////////////////////////////
// File System

/**Class containing the system information on a file path. Information can be
   obtained on any directory entry event if it is not a "file" in the strictest
   sense. Sub-directories, devices etc may also have information retrieved.
 */
class PFileInfo : public PObject
{
  PCLASSINFO(PFileInfo, PObject);

  public:
    /**All types that a particular file path may be. Not all platforms support
       all of the file types. For example under DOS no file may be of the
       type #SymbolicLink#.
     */
    enum FileTypes {
      /// Ordinary disk file.
      RegularFile = 1,        
      /// File path is a symbolic link.
      SymbolicLink = 2,       
      /// File path is a sub-directory
      SubDirectory = 4,       
      /// File path is a character device name.
      CharDevice = 8,         
      /// File path is a block device name.
      BlockDevice = 16,       
      /// File path is a fifo (pipe) device.
      Fifo = 32,              
      /// File path is a socket device.
      SocketDevice = 64,      
      /// File path is of an unknown type.
      UnknownFileType = 256,  
      /// Mask for all file types.
      AllFiles = 0x1ff        
    };

    /// File type for this file. Only one bit is set at a time here.
    FileTypes type;

    /**Time of file creation of the file. Not all platforms support a separate
       creation time in which case the last modified time is returned.
     */
    PTime created;

    /// Time of last modifiaction of the file.
    PTime modified;

    /**Time of last access to the file. Not all platforms support a separate
       access time in which case the last modified time is returned.
     */
    PTime accessed;

    /**Size of the file in bytes. This is a quadword or 8 byte value to allow
       for files greater than 4 gigabytes.
     */
    PUInt64 size;

    /// File access permissions for the file.
    enum Permissions {
      /// File has world execute permission
      WorldExecute = 1,   
      /// File has world write permission
      WorldWrite = 2,     
      /// File has world read permission
      WorldRead = 4,      
      /// File has group execute permission
      GroupExecute = 8,   
      /// File has group write permission
      GroupWrite = 16,    
      /// File has group read permission
      GroupRead = 32,     
      /// File has owner execute permission
      UserExecute = 64,   
      /// File has owner write permission
      UserWrite = 128,    
      /// File has owner read permission
      UserRead = 256,     
      /// All possible permissions.
      AllPermissions = 0x1ff,   
      /// Owner read & write plus group and world read permissions.
      DefaultPerms = UserRead|UserWrite|GroupRead|WorldRead,
      /// Owner read & write & execute plus group and world read & exectute permissions.
      DefaultDirPerms = DefaultPerms|UserExecute|GroupExecute|WorldExecute
      
    };

    /**A bit mask of all the file acces permissions. See the
       #Permissions enum# for the possible bit values.
       
       Not all platforms support all permissions.
     */
    int permissions;

    /**File is a hidden file. What constitutes a hidden file is platform
       dependent, for example under unix it is a file beginning with a '.'
       character while under MS-DOS there is a file system attribute for it.
     */
    BOOL hidden;
};


/**Class to represent a directory in the operating system file system. A
   directory is a special file that contains a list of file paths.
   
   The directory paths are highly platform dependent and a minimum number of
   assumptions should be made.
   
   The PDirectory object is a string consisting of a possible volume name, and
   a series directory names in the path from the volumes root to the directory
   that the object represents. Each directory is separated by the platform
   dependent separator character which is defined by the PDIR_SEPARATOR macro.
   The path always has a trailing separator.

   Some platforms allow more than one character to act as a directory separator
   so when doing any processing the #IsSeparator()# function should be
   used to determine if a character is a possible separator.

   The directory may be opened to gain access to the list of files that it
   contains. Note that the directory does {\bf not} contain the "." and ".."
   entries that some platforms support.

   The ancestor class is dependent on the platform. For file systems that are
   case sensitive, eg Unix, the ancestor is #PString#. For other
   platforms, the ancestor class is #PCaselessString#.
 */
class PDirectory : public PFilePathString
{
  PCONTAINERINFO(PDirectory, PFilePathString);

  public:
  /**@name Construction */
  //@{
    /// Create a directory object of the current working directory
    PDirectory();
      
    /**Create a directory object of the specified directory. The
       #pathname# parameter may be a relative directory which is
       made absolute by the creation of the #PDirectory# object.
     */
    PDirectory(
      const char * cpathname      ///< Directory path name for new object.
    );

    /**Create a directory object of the specified directory. The
       #pathname# parameter may be a relative directory which is
       made absolute by the creation of the #PDirectory# object.
     */
    PDirectory(
      const PString & pathname    ///< Directory path name for new object.
    );

    /**Set the directory to the specified path.
     */
    PDirectory & operator=(
      const PString & pathname    ///< Directory path name for new object.
    );

    /**Set the directory to the specified path.
     */
    PDirectory & operator=(
      const char * cpathname      ///< Directory path name for new object.
    );
  //@}

  /**@name Access functions */
  //@{
    /**Get the directory for the parent to the current directory. If the
       directory is already the root directory it returns the root directory
       again.

       @return
       parent directory.
     */
    PDirectory GetParent() const;

    /**Get the volume name that the directory is in.
    
       This is platform dependent, for example for MS-DOS it is the 11
       character volume name for the drive, eg "DOS_DISK", and for Macintosh it
       is the disks volume name eg "Untitled". For a unix platform it is the
       device name for the file system eg "/dev/sda1".

       @return
       string for the directory volume.
     */
    PFilePathString GetVolume() const;

    /**Determine if the directory is the root directory of a volume.
    
       @return
       TRUE if the object is a root directory.
     */
    BOOL IsRoot() const;

    /**Get the root directory of a volume.
    
       @return
       root directory.
     */
    PDirectory GetRoot() const;

    /**Get the directory path as an array of strings.
       The first element in the array is the volume string, eg under Win32 it
       is "c:" or "\\machine", while under unix it is an empty string.
      */
    PStringArray GetPath() const;

    /**Determine if the character #ch# is a directory path
       separator.

       @return
       TRUE if may be used to separate directories in a path.
     */
    PINLINE static BOOL IsSeparator(
      char ch    ///< Character to check as being a separator.
    );

    /**Determine the total number of bytes and number of bytes free on the
       volume that this directory is contained on.

       Note that the free space will be the physical limit and if user quotas
       are in force by the operating system, the use may not actually be able
       to use all of these bytes.

       @return
       TRUE if the information could be determined.
     */
    BOOL GetVolumeSpace(
      PInt64 & total,     ///< Total number of bytes available on volume
      PInt64 & free,      ///< Number of bytes unused on the volume
      DWORD & clusterSize ///< "Quantisation factor" in bytes for files on volume
    ) const;
  //@}

  /**@name File system functions */
  //@{
    /**Test for if the directory exists.

       @return
       TRUE if directory exists.
     */
    BOOL Exists() const;

    /**Test for if the specified directory exists.

       @return
       TRUE if directory exists.
     */
    static BOOL Exists(
      const PString & path   ///< Directory file path.
    );
      
    /**Change the current working directory to the objects location.

       @return
       TRUE if current working directory was changed.
     */
    BOOL Change() const;

    /**Change the current working directory to that specified..

       @return
       TRUE if current working directory was changed.
     */
    static BOOL Change(
      const PString & path   ///< Directory file path.
    );
      
    /**Create a new directory with the specified permissions.

       @return
       TRUE if directory created.
     */
    BOOL Create(
      int perm = PFileInfo::DefaultDirPerms    // Permission on new directory.
    ) const;
    /**Create a new directory as specified with the specified permissions.

       @return
       TRUE if directory created.
     */
    static BOOL Create(
      const PString & p,   ///< Directory file path.
      int perm = PFileInfo::DefaultDirPerms    ///< Permission on new directory.
    );

    /**Delete the directory.

       @return
       TRUE if directory was deleted.
     */
    BOOL Remove();

    /**Delete the specified directory.

       @return
       TRUE if directory was deleted.
     */
    static BOOL Remove(
      const PString & path   ///< Directory file path.
    );
  //@}

  /**@name Directory listing functions */
  //@{
    /**Open the directory for scanning its list of files. Once opened the
       #GetEntryName()# function may be used to get the current directory
       entry and the #Next()# function used to move to the next directory
       entry.
       
       Only files that are of a type that is specified in the mask will be
       returned.
       
       Note that the directory scan will {\bf not} return the "." and ".."
       entries that some platforms support.

       @return
       TRUE if directory was successfully opened, and there was at least one
       file in it of the specified types.
     */
    virtual BOOL Open(
      int scanMask = PFileInfo::AllFiles    ///< Mask of files to provide.
    );
      
    /**Restart file list scan from the beginning of directory. This is similar
       to the #Open()# command but does not require that the directory be
       closed (using #Close()#) first.

       Only files that are of a type that is specified in the mask will be
       returned.

       Note that the directory scan will {\bf not} return the "." and ".."
       entries that some platforms support.

       @return
       TRUE if directory was successfully opened, and there was at least one
       file in it of the specified types.
     */
    virtual BOOL Restart(
      int scanMask = PFileInfo::AllFiles    ///< Mask of files to provide.
    );
      
    /**Move to the next file in the directory scan.
    
       Only files that are of a type that is specified in the mask passed to
       the #Open()# or #Restart()# functions will be returned.

       Note that the directory scan will {\bf not} return the "." and ".."
       entries that some platforms support.

       @return
       TRUE if there is another valid file in the directory.
     */
    BOOL Next();
      
    /// Close the directory during or after a file list scan.
    virtual void Close();

    /**Get the name (without the volume or directory path) of the current
       entry in the directory scan. This may be the name of a file or a
       subdirectory or even a link or device for operating systems that support
       them.
       
       To get a full path name concatenate the PDirectory object itself with
       the entry name.
       
       Note that the directory scan will {\bf not} return the "." and ".."
       entries that some platforms support.

       @return
       string for directory entry.
     */
    virtual PFilePathString GetEntryName() const;

    /**Determine if the directory entry currently being scanned is itself
       another directory entry.
       
       Note that the directory scan will {\bf not} return the "." and ".."
       entries that some platforms support.

       @return
       TRUE if entry is a subdirectory.
     */
    virtual BOOL IsSubDir() const;

    /**Get file information on the current directory entry.
    
       @return
       TRUE if file information was successfully retrieved.
     */
    virtual BOOL GetInfo(
      PFileInfo & info    ///< Object to receive the file information.
    ) const;
  //@}


  protected:
    // New functions for class
    void Construct();
    void Destruct()
    { Close(); PFilePathString::Destruct(); }

    // Member variables
    /// Mask of file types that the directory scan will return.
    int scanMask;

// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/pdirect.h"
#else
#include "unix/ptlib/pdirect.h"
#endif

};

#endif

// End Of File ///////////////////////////////////////////////////////////////
