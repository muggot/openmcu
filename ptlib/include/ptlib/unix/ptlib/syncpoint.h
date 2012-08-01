/*
 * syncpoint.h
 *
 * Thread synchronisation point (event) class.
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
 * basis, WITHOUT WARRANTY OF ANY KIND, eiTY OF ANY KIND, either express or implied. See
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
 * $Log: syncpoint.h,v $
 * Revision 1.8  2005/11/25 00:06:12  csoutheren
 * Applied patch #1364593 from Hannes Friederich
 * Also changed so PTimesMutex is no longer descended from PSemaphore on
 * non-Windows platforms
 *
 * Revision 1.7  2004/04/30 16:18:41  ykiryanov
 * BeOS modifications derived from BLocker use
 *
 * Revision 1.6  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.5  2001/09/20 05:38:25  robertj
 * Changed PSyncPoint to use pthread cond so timed wait blocks properly.
 * Also prevented semaphore from being created if subclass does not use it.
 *
 * Revision 1.4  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.3  1998/11/30 22:07:18  robertj
 * New directory structure.
 *
 * Revision 1.2  1998/09/24 04:11:59  robertj
 * Added open software license.
 *
 * Revision 1.1  1998/03/24 07:31:04  robertj
 * Initial revision
 *
 * Revision 1.1  1998/03/23 02:42:03  robertj
 * Initial revision
 *
 */

///////////////////////////////////////////////////////////////////////////////
// PSyncPoint

#if defined(P_PTHREADS)
  public:
    virtual ~PSyncPoint();
#endif

#if defined(P_PTHREADS) || defined(__BEOS__) || defined(P_MAC_MPTHREADS)
  public:
    virtual void Wait();
    virtual BOOL Wait(const PTimeInterval & timeout);
    virtual void Signal();
    virtual BOOL WillBlock() const;
  private:
    unsigned signalCount;
#endif

// End Of File ////////////////////////////////////////////////////////////////
