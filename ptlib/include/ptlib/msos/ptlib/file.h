/*
 * file.h
 *
 * Disk file I/O channel class.
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
 * Revision 1.7  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.6  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.5  1998/11/30 02:55:10  robertj
 * New directory structure
 *
 * Revision 1.4  1998/09/24 03:30:02  robertj
 * Added open software license.
 *
 * Revision 1.3  1996/08/08 10:09:00  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.2  1995/06/04 12:46:49  robertj
 * Borland C++ compatibility.
 *
 * Revision 1.1  1994/06/25 12:13:01  robertj
 * Initial revision
 *
 * Revision 1.1  1994/04/12  08:31:05  robertj
 * Initial revision
 *
 */


#ifdef __BORLANDC__
#define _open ::open
#define _close ::close
#define _read ::read
#define _write ::write
#define _lseek ::lseek
#define _chsize ::chsize
#define _access ::access
#define _chmod ::chmod
#define _mkdir ::mkdir
#define _rmdir ::rmdir
#define _chdir ::chdir
#define _mktemp ::mktemp
#define _S_IWRITE S_IWRITE
#define _S_IREAD S_IREAD
#define _O_TEXT O_TEXT
#define _O_BINARY O_BINARY
#endif


///////////////////////////////////////////////////////////////////////////////
// PFile

  protected:
    virtual BOOL IsTextFile() const;
      // Return TRUE if text file translation is required

// End Of File ///////////////////////////////////////////////////////////////
