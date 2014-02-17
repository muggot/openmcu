/*
 * pipechan.h
 *
 * Sub-process communicating with a pipe I/O channel class.
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
 * $Log: pipechan.h,v $
 * Revision 1.10  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.9  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.8  1998/11/30 02:55:22  robertj
 * New directory structure
 *
 * Revision 1.7  1998/10/26 09:11:30  robertj
 * Added ability to separate out stdout from stderr on pipe channels.
 *
 * Revision 1.6  1998/09/24 03:30:15  robertj
 * Added open software license.
 *
 * Revision 1.5  1996/11/16 10:53:30  robertj
 * Fixed bug in PPipeChannel test for open channel, win95 support.
 *
 * Revision 1.4  1996/08/08 10:09:07  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.3  1995/03/14 13:31:35  robertj
 * Implemented DOS pipe channel.
 *
 * Revision 1.2  1995/03/12  04:59:56  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.1  1994/10/23  05:35:36  robertj
 * Initial revision
 *
 */


///////////////////////////////////////////////////////////////////////////////
// PPipeChannel

  public:
    virtual BOOL IsOpen() const;
  protected:
    PROCESS_INFORMATION info;
    HANDLE hToChild, hFromChild, hStandardError;

// End Of File ///////////////////////////////////////////////////////////////
