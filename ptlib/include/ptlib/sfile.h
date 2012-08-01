/*
 * sfile.h
 *
 * Structured file I/O channel class.
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
 * $Log: sfile.h,v $
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
 * Revision 1.14  1999/02/16 08:11:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.13  1998/09/24 07:24:01  robertj
 * Moved structured fiel into separate module so don't need silly implementation file for GNU C.
 *
 * Revision 1.12  1998/09/23 06:21:23  robertj
 * Added open source copyright license.
 *
 * Revision 1.11  1996/01/23 13:15:38  robertj
 * Mac Metrowerks compiler support.
 *
 * Revision 1.10  1995/06/17 11:13:19  robertj
 * Documentation update.
 *
 * Revision 1.9  1995/03/14 12:42:34  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.8  1995/01/14  06:19:39  robertj
 * Documentation
 *
 * Revision 1.7  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.6  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.5  1994/04/20  12:17:44  robertj
 * PFilePath split
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

#ifndef _PSTRUCTUREDFILE
#define _PSTRUCTUREDFILE

#ifdef P_USE_PRAGMA
#pragma interface
#endif


/**A class representing a a structured file that is portable accross CPU
   architectures (as in the XDR protocol).
   
   This differs from object serialisation in that the access is always to a
   disk file and is random access. It would primarily be used for database
   type applications.
 */
class PStructuredFile : public PFile
{
  PCLASSINFO(PStructuredFile, PFile);

  private:
    BOOL Read(void * buf, PINDEX len) { return PFile::Read(buf, len); }
    BOOL Write(const void * buf, PINDEX len) { return PFile::Write(buf, len); }

  public:
  /**@name Construction */
  //@{
    /**Create a structured file object but do not open it. It does not
       initially have a valid file name. However, an attempt to open the file
       using the #PFile::Open()# function will generate a unique
       temporary file.
       
       The initial structure size is one byte.
     */
    PStructuredFile();

    /**Create a unique temporary file name, and open the file in the specified
       mode and using the specified options. Note that opening a new, unique,
       temporary file name in ReadOnly mode will always fail. This would only
       be usefull in a mode and options that will create the file.

       The #PChannel::IsOpen()# function may be used after object
       construction to determine if the file was successfully opened.
     */
    PStructuredFile(
      OpenMode mode,          ///< Mode in which to open the file.
      int opts = ModeDefault  ///< #OpenOptions enum# for open operation.
    );
      
    /**Create a structured file object with the specified name and open it in
       the specified mode and with the specified options.

       The #PChannel::IsOpen()# function may be used after object
       construction to determine if the file was successfully opened.
     */
    PStructuredFile(
      const PFilePath & name,    ///< Name of file to open.
      OpenMode mode = ReadWrite, ///< Mode in which to open the file.
      int opts = ModeDefault     ///< #OpenOptions enum# for open operation.
    );
  //@}

  /**@name Structured I/O functions */
  //@{
    /**Read a sequence of bytes into the specified buffer, translating the
       structure according to the specification made in the
       #SetStructure()# function.

       @return
       TRUE if the structure was successfully read.
     */
    BOOL Read(
      void * buffer   ///< Pointer to structure to receive data.
    );
      
    /**Write a sequence of bytes into the specified buffer, translating the
       structure according to the specification made in the
       #SetStructure()# function.

       @return
       TRUE if the structure was successfully written.
     */
    BOOL Write(
      const void * buffer   ///< Pointer to structure to write data from.
    );
  //@}

  /**@name Structure definition functions */
  //@{
    /**Get the size of each structure in the file.

       @return
       number of bytes in a structure.
     */
    PINDEX GetStructureSize() { return structureSize; }

    /// All element types in a structure
    enum ElementType {
      /// Element is a single character.
      Character,    
      /// Element is a 16 bit integer.
      Integer16,    
      /// Element is a 32 bit integer.
      Integer32,    
      /// Element is a 64 bit integer.
      Integer64,    
      /// Element is a 32 bit IEE floating point number.
      Float32,      
      /// Element is a 64 bit IEE floating point number.
      Float64,      
      /// Element is a 80 bit IEE floating point number.
      Float80,      
      NumElementTypes
    };

    /// Elements in the structure definition.
    struct Element {
      /// Type of element in structure.
      ElementType type;   
      /// Count of elements of this type.
      PINDEX      count;  
    };

    /** Set the structure of each record in the file. */
    void SetStructure(
      Element * structure,  ///< Array of structure elements
      PINDEX numElements    ///< Number of structure elements in structure.
    );
  //@}

  protected:
  // Member variables
    /// Number of bytes in structure.
    PINDEX structureSize;

    /// Array of elements in the structure.
    Element * structure;

    /// Number of elements in the array.
    PINDEX numElements;


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/sfile.h"
#else
#include "unix/ptlib/sfile.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
