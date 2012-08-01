/*
 * semaphor.h
 *
 * Thread synchonisation semaphore class.
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
 * $Log: semaphor.h,v $
 * Revision 1.9  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.8  2002/01/23 04:46:11  craigs
 * Added copy Constructors for PSemaphore, PMutex and PSyncPoint
 *
 * Revision 1.7  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.6  1998/11/30 02:55:27  robertj
 * New directory structure
 *
 * Revision 1.5  1998/09/24 03:30:21  robertj
 * Added open software license.
 *
 * Revision 1.4  1998/03/20 03:19:49  robertj
 * Added special classes for specific sepahores, PMutex and PSyncPoint.
 *
 * Revision 1.3  1996/08/08 10:09:11  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.2  1996/06/13 13:32:10  robertj
 * Rewrite of auto-delete threads, fixes Windows95 total crash.
 *
 * Revision 1.1  1995/08/01 21:42:22  robertj
 * Initial revision
 *
 */

///////////////////////////////////////////////////////////////////////////////
// PSemaphore

  public:
    HANDLE GetHandle() const { return handle; }
    unsigned GetInitialVal() const  { return initialVal; }
    unsigned GetMaxCountVal() const { return maxCountVal; }

  protected:
    PSemaphore(HANDLE h);
    HANDLE handle;
    unsigned initialVal;
    unsigned maxCountVal;

// End Of File ///////////////////////////////////////////////////////////////
