/*
 * mutex.h
 *
 * Mutual exclusion thread synchonisation class.
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
 * $Log: mutex.h,v $
 * Revision 1.16  2007/09/05 11:58:47  csoutheren
 * Fixed build on MacOSX
 *
 * Revision 1.15  2007/09/05 11:09:09  csoutheren
 * Removed misleading and incorrect code from Linux implementation of
 * PCriticalSection. Apologies to Hannes Friederich :(
 *
 * Revision 1.14  2005/11/25 00:06:12  csoutheren
 * Applied patch #1364593 from Hannes Friederich
 * Also changed so PTimesMutex is no longer descended from PSemaphore on
 * non-Windows platforms
 *
 * Revision 1.13  2005/11/08 22:31:00  csoutheren
 * Moved declaration of PMutex
 *
 * Revision 1.12  2005/11/08 22:18:31  csoutheren
 * Changed PMutex to use PTimedMutex on non-Windows platforms because
 * sem_wait is not recursive. Very sad.
 * Thanks to Frederic Heem for finding this problem
 *
 * Revision 1.11  2005/11/04 06:34:20  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 * Revision 1.10  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.9  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.8  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.7  2002/01/23 04:26:36  craigs
 * Added copy constructors for PSemaphore, PMutex and PSyncPoint to allow
 * use of default copy constructors for objects containing instances of
 * these classes
 *
 * Revision 1.6  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.5  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.4  1999/02/16 08:12:22  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.3  1998/11/30 02:50:59  robertj
 * New directory structure
 *
 * Revision 1.2  1998/09/23 06:20:55  robertj
 * Added open source copyright license.
 *
 * Revision 1.1  1998/03/23 02:41:31  robertj
 * Initial revision
 *
 */

#ifndef _PMUTEX
#define _PMUTEX

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/critsec.h>
#include <ptlib/semaphor.h>

/**This class defines a thread mutual exclusion object. A mutex is where a
   piece of code or data cannot be accessed by more than one thread at a time.
   To prevent this the PMutex is used in the following manner:
\begin{verbatim}
      PMutex mutex;

      ...

      mutex.Wait();

      ... critical section - only one thread at a time here.

      mutex.Signal();

      ...
\end{verbatim}
    The first thread will pass through the #Wait()# function, a second
    thread will block on that function until the first calls the
    #Signal()# function, releasing the second thread.
 */

/*
 * On Windows, It is convenient for PTimedMutex to be an ancestor of PSemaphore
 * But that is the only platform where it is - every other platform (i.e. Unix)
 * uses different constructs for these objects, so there is no need for a PTimedMute
 * to carry around all of the PSemaphore members
 */

#ifdef _WIN32
class PTimedMutex : public PSemaphore
{
  PCLASSINFO(PTimedMutex, PSemaphore);
#else
class PTimedMutex : public PSync
{
  PCLASSINFO(PTimedMutex, PSync)
#endif

  public:
    /* Create a new mutex.
       Initially the mutex will not be "set", so the first call to Wait() will
       never wait.
     */
    PTimedMutex();
    PTimedMutex(const PTimedMutex & mutex);

// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/mutex.h"
#else
#include "unix/ptlib/mutex.h"
#endif
};

// On Windows, critical sections are recursive and so we can use them for mutexes
// The only Posix mutex that is recursive is pthread_mutex, so we have to use that
#ifdef _WIN32
typedef PCriticalSection PMutex;
#else
typedef PTimedMutex PMutex;
#define	PCriticalSection PTimedMutex
#endif

#endif

// End Of File ///////////////////////////////////////////////////////////////
