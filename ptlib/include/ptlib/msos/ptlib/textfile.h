/*
 * textfile.h
 *
 * Text file I/O channel class.
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
 * Revision 1.7  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.6  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.5  1998/11/30 02:55:39  robertj
 * New directory structure
 *
 * Revision 1.4  1998/09/24 03:30:32  robertj
 * Added open software license.
 *
 * Revision 1.3  1996/08/08 10:09:18  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.2  1995/03/12 05:00:01  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.1  1994/06/25  12:13:01  robertj
 * Initial revision
 *
 * Revision 1.1  1994/04/12  08:31:05  robertj
 * Initial revision
 *
 */

///////////////////////////////////////////////////////////////////////////////
// PTextFile

  protected:
    virtual BOOL IsTextFile() const;
      // Return TRUE if text file translation is required

  private:
    char characterAfterCarriageReturn;

// End Of File ///////////////////////////////////////////////////////////////
