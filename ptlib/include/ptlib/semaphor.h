/*
 * semaphor.h
 *
 * Thread synchronisation semaphore class.
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
 * Revision 1.22  2007/09/05 11:58:47  csoutheren
 * Fixed build on MacOSX
 *
 * Revision 1.21  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.20  2005/11/14 22:29:13  csoutheren
 * Reverted Wait and Signal to non-const - there is no way we can guarantee that all
 * descendant classes everywhere will be changed over, so we have to keep the
 * original  API
 *
 * Revision 1.19  2005/11/04 06:34:20  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 * Revision 1.18  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.17  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.16  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.15  2002/01/23 04:26:36  craigs
 * Added copy constructors for PSemaphore, PMutex and PSyncPoint to allow
 * use of default copy constructors for objects containing instances of
 * these classes
 *
 * Revision 1.14  2001/11/23 00:55:18  robertj
 * Changed PWaitAndSignal so works inside const functions.
 *
 * Revision 1.13  2001/06/01 04:00:21  yurik
 * Removed dependency on obsolete function
 *
 * Revision 1.12  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.11  2001/04/23 00:34:29  robertj
 * Added ability for PWaitAndSignal to not wait on semaphore.
 *
 * Revision 1.10  2001/01/27 23:40:09  yurik
 * WinCE port-related - CreateEvent used instead of CreateSemaphore
 *
 * Revision 1.9  2000/12/19 22:20:26  dereks
 * Add video channel classes to connect to the PwLib PVideoInputDevice class.
 * Add PFakeVideoInput class to generate test images for video.
 *
 * Revision 1.8  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.7  1999/02/16 08:11:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.6  1998/11/19 05:17:37  robertj
 * Added PWaitAndSignal class for easier mutexing.
 *
 * Revision 1.5  1998/09/23 06:21:19  robertj
 * Added open source copyright license.
 *
 * Revision 1.4  1998/03/20 03:16:11  robertj
 * Added special classes for specific sepahores, PMutex and PSyncPoint.
 *
 * Revision 1.3  1995/12/10 11:34:50  robertj
 * Fixed incorrect order of parameters in semaphore constructor.
 *
 * Revision 1.2  1995/11/21 11:49:42  robertj
 * Added timeout on semaphore wait.
 *
 * Revision 1.1  1995/08/01 21:41:24  robertj
 * Initial revision
 *
 */

#ifndef _PSEMAPHORE
#define _PSEMAPHORE

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/psync.h>
#include <limits.h>
#include <ptlib/critsec.h>

/**This class defines a thread synchonisation object. This is in the form of a
   integer semaphore. The semaphore has a count and a maximum value. The
   various combinations of count and usage of the #Wait()# and
   #Signal()# functions determine the type of synchronisation mechanism
   to be employed.

   The #Wait()# operation is that if the semaphore count is > 0,
   decrement the semaphore and return. If it is = 0 then wait (block).

   The #Signal()# operation is that if there are waiting threads then
   unblock the first one that was blocked. If no waiting threads and the count
   is less than the maximum then increment the semaphore.

   The most common is to create a mutual exclusion zone. A mutex is where a
   piece of code or data cannot be accessed by more than one thread at a time.
   To prevent this the PSemaphore is used in the following manner:
\begin{verbatim}
      PSemaphore mutex(1, 1);  // Maximum value of 1 and initial value of 1.

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
class PSemaphore : public PSync
{
  PCLASSINFO(PSemaphore, PSync);

  public:
  /**@name Construction */
  //@{
    /**Create a new semaphore with maximum count and initial value specified.
       If the initial value is larger than the maximum value then is is set to
       the maximum value.
     */
    PSemaphore(
      unsigned initial, ///< Initial value for semaphore count.
      unsigned maximum  ///< Maximum value for semaphore count.
    );

    /** Create a new Semaphore with the same initial and maximum values as the original
     */
    PSemaphore(const PSemaphore &);

    /**Destroy the semaphore. This will assert if there are still waiting
       threads on the semaphore.
     */
    ~PSemaphore();
  //@}

  /**@name Operations */
  //@{
    /**If the semaphore count is > 0, decrement the semaphore and return. If
       if is = 0 then wait (block).
     */
    virtual void Wait();

    /**If the semaphore count is > 0, decrement the semaphore and return. If
       if is = 0 then wait (block) for the specified amount of time.

       @return
       TRUE if semaphore was signalled, FALSE if timed out.
     */
    virtual BOOL Wait(
      const PTimeInterval & timeout // Amount of time to wait for semaphore.
    );

    /**If there are waiting (blocked) threads then unblock the first one that
       was blocked. If no waiting threads and the count is less than the
       maximum then increment the semaphore.
     */
    virtual void Signal();

    /**Determine if the semaphore would block if the #Wait()# function
       were called.

       @return
       TRUE if semaphore will block when Wait() is called.
     */
    virtual BOOL WillBlock() const;
  //@}

  private:
    PSemaphore & operator=(const PSemaphore &) { return *this; }


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/semaphor.h"
#else
#include "unix/ptlib/semaphor.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
