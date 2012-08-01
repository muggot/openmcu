/*
 * filepath.h
 *
 * File system path string abstraction class.
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
 * $Log: filepath.h,v $
 * Revision 1.22  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.21  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.20  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.19  2002/11/19 10:32:26  robertj
 * Changed PFilePath so can be empty string, indicating illegal path.
 *
 * Revision 1.18  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.17  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.16  2001/02/13 04:39:08  robertj
 * Fixed problem with operator= in container classes. Some containers will
 *   break unless the copy is virtual (eg PStringStream's buffer pointers) so
 *   needed to add a new AssignContents() function to all containers.
 *
 * Revision 1.15  1999/03/09 02:59:49  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.14  1999/02/16 08:07:11  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.13  1998/11/30 08:57:16  robertj
 * Fixed problem where if += is used on PFilePath, it no longer may be normalised.
 *
 * Revision 1.12  1998/09/23 06:20:37  robertj
 * Added open source copyright license.
 *
 * Revision 1.11  1998/02/16 00:14:57  robertj
 * Added functions to validate characters in a filename.
 *
 * Revision 1.10  1995/07/31 12:03:37  robertj
 * Added copy constructor and assignment operator for right types.
 *
 * Revision 1.9  1995/04/22 00:43:43  robertj
 * Added Move() function and changed semantics of Rename().
 * Changed all file name strings to PFilePath objects.
 *
 * Revision 1.8  1995/03/14 12:41:25  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.7  1994/12/21  11:52:57  robertj
 * Documentation and variable normalisation.
 *
 * Revision 1.6  1994/10/24  00:06:58  robertj
 * Changed PFilePath and PDirectory so descends from either PString or
 *     PCaselessString depending on the platform.
 *
 * Revision 1.5  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.4  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.3  1994/08/21  23:43:02  robertj
 * Changed parameter before variable argument list to NOT be a reference.
 *
 * Revision 1.2  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.1  1994/04/20  12:17:44  robertj
 * Initial revision
 *
 */

#ifndef _PFILEPATH
#define _PFILEPATH

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#ifdef DOC_PLUS_PLUS
/** Base string type for a file path.
    For platforms where filenames are case significant (eg Unix) this class
    is a synonym for #PString#. If it is for a platform where case is not
    significant (eg Win32, Mac) then this is a synonym for #PCaselessString#.
 */
class PFilePathString : public PString { };
#endif


///////////////////////////////////////////////////////////////////////////////
// File Specification

/**This class describes a full description for a file on the particular
   platform. This will always uniquely identify the file on currently mounted
   volumes.

   An empty string for a PFilePath indicates an illegal path.

   The ancestor class is dependent on the platform. For file systems that are
   case sensitive, eg Unix, the ancestor is #PString#. For other
   platforms, the ancestor class is #PCaselessString#.
 */
class PFilePath : public PFilePathString
{
  PCLASSINFO(PFilePath, PFilePathString);

  public:
  /**@name Construction */
  //@{
    /**Create a file specification object.
     */
    PFilePath();

    /**Create a file specification object with the specified file name.
    
       The string passed in may be a full or partial specification for a file
       as determined by the platform. It is unusual for this to be a literal
       string, unless only the file title is specified, as that would be
       platform specific.

       The partial file specification is translated into a canonical form
       which always absolutely references the file.
     */
    PFilePath(
      const char * cstr   ///< Partial C string for file name.
    );

    /**Create a file specification object with the specified file name.
    
       The string passed in may be a full or partial specification for a file
       as determined by the platform. It is unusual for this to be a literal
       string, unless only the file title is specified, as that would be
       platform specific.

       The partial file specification is translated into a canonical form
       which always absolutely references the file.
     */
    PFilePath(
      const PString & str ///< Partial PString for file name.
    );

    /**Create a file specification object with the specified file name.
     */
    PFilePath(
      const PFilePath & path ///< Previous path for file name.
    );

    /**Create a file spec object with a generated temporary name. The first
       parameter is a prefix for the filename to which a unique number is
       appended. The second parameter is the directory in which the file is to
       be placed. If this is NULL a system standard directory is used.
     */
    PFilePath(
      const char * prefix,  ///< Prefix string for file title.
      const char * dir      ///< Directory in which to place the file.
    );

    /**Change the file specification object to the specified file name.
     */
    PFilePath & operator=(
      const PFilePath & path ///< Previous path for file name.
    );
    /**Change the file specification object to the specified file name.

       The string passed in may be a full or partial specifiaction for a file
       as determined by the platform. It is unusual for this to be a literal
       string, unless only the file title is specified, as that would be
       platform specific.

       The partial file specification is translated into a canonical form
       which always absolutely references the file.
     */
    PFilePath & operator=(
      const PString & str ///< Partial PString for file name.
    );
    /**Change the file specification object to the specified file name.

       The string passed in may be a full or partial specifiaction for a file
       as determined by the platform. It is unusual for this to be a literal
       string, unless only the file title is specified, as that would be
       platform specific.

       The partial file specification is translated into a canonical form
       which always absolutely references the file.
     */
    PFilePath & operator=(
      const char * cstr ///< Partial "C" string for file name.
    );
  //@}

  /**@name Path addition functions */
  //@{
    /**Concatenate a string to the file path, modifiying that path.

       @return
       reference to string that was concatenated to.
     */
    PFilePath & operator+=(
      const PString & str   ///< String to concatenate.
    );

    /**Concatenate a C string to a path, modifiying that path. The
       #cstr# parameter is typically a literal string, eg:
\begin{verbatim}
        myStr += "fred";
\end{verbatim}

       @return
       reference to string that was concatenated to.
     */
    PFilePath & operator+=(
      const char * cstr  ///< C string to concatenate.
    );

    /**Concatenate a single character to a path. The #ch#
       parameter is typically a literal, eg:
\begin{verbatim}
        myStr += '!';
\end{verbatim}

       @return
       new string with concatenation of the object and parameter.
     */
    PFilePath & operator+=(
      char ch   // Character to concatenate.
    );
  //@}

  /**@name Path decoding access functions */
  //@{
    /**Get the drive/volume name component of the full file specification. This
       is very platform specific. For example in DOS & NT it is the drive
       letter followed by a colon ("C:"), for Macintosh it is the volume name
       ("Untitled") and for Unix it is empty ("").
       
       @return
       string for the volume name part of the file specification..
     */
    PFilePathString GetVolume() const;
      
    /**Get the directory path component of the full file specification. This
       will include leading and trailing directory separators. For example
       on DOS this could be "\SRC\PWLIB\", for Macintosh ":Source:PwLib:" and
       for Unix "/users/equivalence/src/pwlib/".

       @return
       string for the path part of the file specification.
     */
    PFilePathString GetPath() const;

    /**Get the title component of the full file specification, eg for the DOS
       file "C:\SRC\PWLIB\FRED.DAT" this would be "FRED".

       @return
       string for the title part of the file specification.
     */
    PFilePathString GetTitle() const;

    /**Get the file type of the file. Note that on some platforms this may
       actually be part of the full name string. eg for DOS file
       "C:\SRC\PWLIB\FRED.TXT" this would be ".TXT" but on the Macintosh this
       might be "TEXT".

       Note there are standard translations from file extensions, eg ".TXT"
       and some Macintosh file types, eg "TEXT".

       @return
       string for the type part of the file specification.
     */
    PFilePathString GetType() const;

    /**Get the actual directory entry name component of the full file
       specification. This may be identical to
       #GetTitle() + GetType()# or simply #GetTitle()#
       depending on the platform. eg for DOS file "C:\SRC\PWLIB\FRED.TXT" this
       would be "FRED.TXT".

       @return
       string for the file name part of the file specification.
     */
    PFilePathString GetFileName() const;

    /**Get the the directory that the file is contained in.  This may be 
       identical to #GetVolume() + GetPath()# depending on the 
       platform. eg for DOS file "C:\SRC\PWLIB\FRED.TXT" this would be 
       "C:\SRC\PWLIB\".

       Note that for Unix platforms, this returns the {\bf physical} path
       of the directory. That is all symlinks are resolved. Thus the directory
       returned may not be the same as the value of #GetPath()#.

       @return
       Directory that the file is contained in.
     */
    PDirectory GetDirectory() const;

    /**Set the type component of the full file specification, eg for the DOS
       file "C:\SRC\PWLIB\FRED.DAT" would become "C:\SRC\PWLIB\FRED.TXT".
     */
    void SetType(
      const PFilePathString & type  ///< New type of the file.
    );
  //@}

  /**@name Miscellaneous functions */
  //@{
    /**Test if the character is valid in a filename.

       @return
       TRUE if the character is valid for a filename.
     */
    static BOOL IsValid(
      char c    ///< Character to test for validity.
    );

    /**Test if all the characters are valid in a filename.

       @return
       TRUE if the character is valid for a filename.
     */
    static BOOL IsValid(
      const PString & str   ///< String to test for validity.
    );
  //@}


  protected:
    virtual void AssignContents(const PContainer & cont);


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/filepath.h"
#else
#include "unix/ptlib/filepath.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
