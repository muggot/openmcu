/*
 * mutex.h
 *
 * Mutual exclusion thread synchronisation class.
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
 * basis, WITHOUT WARRANTY OF ANY KIND, eitF ANY KIND, either express or implied. See
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
 * $Log: mutex.h,v $
 * Revision 1.28  2007/08/17 05:29:19  csoutheren
 * Add field to Linux showing locking thread to assist in debugging
 *
 * Revision 1.27  2005/11/25 00:06:12  csoutheren
 * Applied patch #1364593 from Hannes Friederich
 * Also changed so PTimesMutex is no longer descended from PSemaphore on
 * non-Windows platforms
 *
 * Revision 1.26  2005/11/18 22:26:07  dsandras
 * Removed a few more CONST's to match with previous commit and fix permanent
 * deadlock.
 *
 * Revision 1.25  2005/11/04 06:56:10  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 * Revision 1.24  2004/04/30 16:15:13  ykiryanov
 * BeOS modifications derived from BLocker use
 *
 * Revision 1.23  2004/04/12 03:35:27  csoutheren
 * Fixed problems with non-recursuve mutexes and critical sections on
 * older compilers and libc
 *
 * Revision 1.22  2004/04/12 00:58:45  csoutheren
 * Fixed PAtomicInteger on Linux, and modified PMutex to use it
 *
 * Revision 1.21  2004/04/11 07:58:08  csoutheren
 * Added configure.in check for recursive mutexes, and changed implementation
 * without recursive mutexes to use PCriticalSection or atomic word structs
 *
 * Revision 1.20  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.19  2002/10/10 05:39:51  robertj
 * Fixed VxWorks port breaking other platforms.
 *
 * Revision 1.18  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.17  2002/09/04 03:14:18  robertj
 * Backed out changes submitted by Martin Froehlich as they do not appear to
 *   actually do anything other than add a sychronisation point. The variables
 *   the patches intended to protect were already protected.
 * Fixed bug where if a PMutex was signalled by a thread that did not have it
 *   locked, it would assert but continue to alter PMutex variables such that
 *   a deadlock or crash is likely.
 *
 * Revision 1.16  2002/08/22 13:05:57  craigs
 * Fixed problems with mutex implementation thanks to Martin Froehlich
 *
 * Revision 1.15  2001/09/20 05:38:25  robertj
 * Changed PSyncPoint to use pthread cond so timed wait blocks properly.
 * Also prevented semaphore from being created if subclass does not use it.
 *
 * Revision 1.14  2001/09/19 17:37:47  craigs
 * Added support for nested mutexes under Linux
 *
 * Revision 1.13  2001/08/11 07:57:30  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.12  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.11  2000/12/16 13:11:09  rogerh
 * Remote the 'public:' line. It is redundant as 'public:' is already
 * specified in the ../../mutex.h header file. Problem spotted following a
 * BeOS patch from Yuri Kiryanov <openh323@kiryanov.com>
 *
 * Revision 1.10  2000/12/15 13:20:17  rogerh
 * Fix typo
 *
 * Revision 1.9  2000/12/15 12:50:14  rogerh
 * Fix some BeOS problems with #if defines.
 * Requested by Yuri Kiryanov <openh323@kiryanov.com>
 *
 * Revision 1.8  2000/10/30 05:48:33  robertj
 * Added assert when get nested mutex.
 *
 * Revision 1.7  1999/09/23 06:52:16  robertj
 * Changed PSemaphore to use Posix semaphores.
 *
 * Revision 1.6  1999/09/02 11:56:35  robertj
 * Fixed problem with destroying PMutex that is already locked.
 *
 * Revision 1.5  1999/03/05 07:03:27  robertj
 * Some more BeOS port changes.
 *
 * Revision 1.4  1999/01/09 03:35:09  robertj
 * Improved efficiency of mutex to use pthread functions directly.
 *
 * Revision 1.3  1998/11/30 22:06:51  robertj
 * New directory structure.
 *
 * Revision 1.2  1998/09/24 04:11:41  robertj
 * Added open software license.
 *
 * Revision 1.1  1998/03/24 07:31:04  robertj
 * Initial revision
 *
 */

///////////////////////////////////////////////////////////////////////////////
// PMutex

#if defined(P_PTHREADS) || defined(VX_TASKS)
    virtual ~PTimedMutex();
    mutable pthread_mutex_t mutex;
#endif

#if defined(P_PTHREADS) || defined(__BEOS__) || defined(P_MAC_MPTHREADS) || defined(VX_TASKS)
    virtual void Wait();
    virtual BOOL Wait(const PTimeInterval & timeout);
    virtual void Signal();
    virtual BOOL WillBlock() const;

  protected:

#  if defined(P_PTHREADS) && !defined(VX_TASKS)
#    if P_HAS_RECURSIVE_MUTEX == 0
       mutable PAtomicInteger lockCount;
#    endif
#  endif

#endif


// End Of File ////////////////////////////////////////////////////////////////
