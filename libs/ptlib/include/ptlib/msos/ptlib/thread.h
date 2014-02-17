/*
 * thread.h
 *
 * Thread of execution control class.
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
 * $Log: thread.h,v $
 * Revision 1.24  2005/11/30 12:47:38  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.23  2005/11/09 09:11:39  csoutheren
 * Moved Windows-specific AttachThreadInput callsto seperate member function
 * on PThread. This removes a linearly increasing delay in creating new threads
 *
 * Revision 1.22  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.21  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.20  2002/10/04 04:34:02  robertj
 * Added functions for getting operating system thread identifier values.
 *
 * Revision 1.19  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.18  1999/08/25 02:41:16  robertj
 * Fixed problem with creating windows in background threads, not happening until have a message sent.
 *
 * Revision 1.17  1998/11/30 02:55:41  robertj
 * New directory structure
 *
 * Revision 1.16  1998/09/24 03:30:34  robertj
 * Added open software license.
 *
 * Revision 1.15  1998/04/01 01:53:14  robertj
 * Fixed problem with NoAutoDelete threads.
 *
 * Revision 1.14  1996/08/17 10:00:36  robertj
 * Changes for Windows DLL support.
 *
 * Revision 1.13  1996/08/08 10:09:19  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.12  1996/07/27 04:08:34  robertj
 * Changed thread creation to use C library function instead of direct WIN32.
 *
 * Revision 1.11  1996/06/13 13:32:12  robertj
 * Rewrite of auto-delete threads, fixes Windows95 total crash.
 *
 * Revision 1.10  1996/03/31 09:08:42  robertj
 * Added mutex to thread dictionary access.
 *
 * Revision 1.9  1995/12/10 11:48:54  robertj
 * Fixed bug in application shutdown of child threads.
 *
 * Revision 1.8  1995/08/24 12:38:36  robertj
 * Added extra conditional compile for WIN32 code.
 *
 * Revision 1.7  1995/07/02 01:23:51  robertj
 * Allowed access to thread info to descendents.
 *
 * Revision 1.6  1995/04/25 11:19:53  robertj
 * Fixes for DLL use in WIN32.
 *
 * Revision 1.5  1995/03/12 05:00:02  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.4  1994/07/27  06:00:10  robertj
 * Backup
 *
 * Revision 1.3  1994/07/21  12:35:18  robertj
 * *** empty log message ***
 *
 * Revision 1.2  1994/07/02  03:18:09  robertj
 * Multi-threading implementation.
 *
 * Revision 1.1  1994/06/25  12:13:01  robertj
 * Initial revision
 *
 */


///////////////////////////////////////////////////////////////////////////////
// PThread

  public:
    HANDLE GetHandle() const { return threadHandle; }
    void Win32AttachThreadInput();

  protected:
    HANDLE threadHandle;
    UINT   threadId;

  private:
    PINDEX originalStackSize;

    static UINT __stdcall MainFunction(void * thread);

// End Of File ///////////////////////////////////////////////////////////////
