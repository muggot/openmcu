/*
 * textfile.h
 *
 * A text file I/O channel class.
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
 * $Log: textfile.h,v $
 * Revision 1.20  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.19  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.18  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.17  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.16  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.15  1999/03/09 02:59:51  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.14  1999/02/16 08:11:17  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.13  1998/09/23 06:21:39  robertj
 * Added open source copyright license.
 *
 * Revision 1.12  1995/07/31 12:15:49  robertj
 * Removed PContainer from PChannel ancestor.
 *
 * Revision 1.11  1995/06/17 11:13:34  robertj
 * Documentation update.
 *
 * Revision 1.10  1995/03/14 12:42:48  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.9  1995/01/14  06:19:42  robertj
 * Documentation
 *
 * Revision 1.8  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.7  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.6  1994/04/20  12:17:44  robertj
 * PFilePath addition
 *
 * Revision 1.5  1994/04/01  14:17:26  robertj
 * Fixed container for text file.
 *
 * Revision 1.4  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.3  1993/08/21  01:50:33  robertj
 * Made Clone() function optional, default will assert if called.
 *
 * Revision 1.2  1993/07/14  12:49:16  robertj
 * Fixed RCS keywords.
 *
 */

#ifndef _PTEXTFILE
#define _PTEXTFILE

#ifdef P_USE_PRAGMA
#pragma interface
#endif


///////////////////////////////////////////////////////////////////////////////
// Text Files

/** A class representing a a structured file that is portable accross CPU
   architectures. Essentially this will normalise the end of line character
   which differs fromplatform to platform.
 */
class PTextFile : public PFile
{
  PCLASSINFO(PTextFile, PFile);

  public:
  /**@name Construction */
  //@{
    /** Create a text file object but do not open it. It does not initially
       have a valid file name. However, an attempt to open the file using the
       #PFile::Open()# function will generate a unique temporary file.
     */
    PTextFile();

    /** Create a unique temporary file name, and open the file in the specified
       mode and using the specified options. Note that opening a new, unique,
       temporary file name in ReadOnly mode will always fail. This would only
       be usefull in a mode and options that will create the file.

       The #PChannel::IsOpen()# function may be used after object
       construction to determine if the file was successfully opened.
     */
    PTextFile(
      OpenMode mode,          ///< Mode in which to open the file.
      int opts = ModeDefault  ///< #OpenOptions enum# for open operation.
    );
      
    /** Create a text file object with the specified name and open it in the
       specified mode and with the specified options.

       The #PChannel::IsOpen()# function may be used after object
       construction to determine if the file was successfully opened.
     */
    PTextFile(
      const PFilePath & name,    ///< Name of file to open.
      OpenMode mode = ReadWrite, ///< Mode in which to open the file.
      int opts = ModeDefault     ///< #OpenOptions enum# for open operation.
    );
  //@}

  /**@name Line I/O functions */
  //@{
    /** Read a line from the text file. What constitutes an end of line in the
       file is platform dependent.
       
       Use the #PChannel::GetLastError()# function to determine if there
       was some error other than end of file.
       
       @return
       TRUE if successful, FALSE if at end of file or a read error.
     */
    BOOL ReadLine(
      PString & str  ///< String into which line of text is read.
    );

    /** Read a line from the text file. What constitutes an end of line in the
       file is platform dependent.
       
       Use the #PChannel::GetLastError()# function to determine the
       failure mode.

       @return
       TRUE if successful, FALSE if an error occurred.
     */
    BOOL WriteLine(
      const PString & str  ///< String to write with end of line terminator.
    );
  //@}


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/textfile.h"
#else
#include "unix/ptlib/textfile.h"
#endif
};

#endif


// End Of File ///////////////////////////////////////////////////////////////
