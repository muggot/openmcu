/*
 * psync.h
 *
 * Abstract synchronisation semaphore class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 * Copyright (c) 2005 Post Increment
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
 * $Log: psync.h,v $
 * Revision 1.7  2007/08/17 07:29:21  csoutheren
 * Fix build on MacOSX
 *
 * Revision 1.6  2007/08/17 05:29:19  csoutheren
 * Add field to Linux showing locking thread to assist in debugging
 *
 * Revision 1.5  2007/05/07 14:05:09  csoutheren
 * Add PSyncNULL
 *
 * Revision 1.4  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.3  2005/11/14 22:29:13  csoutheren
 * Reverted Wait and Signal to non-const - there is no way we can guarantee that all
 * descendant classes everywhere will be changed over, so we have to keep the
 * original  API
 *
 * Revision 1.2  2005/11/04 06:56:10  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 * Revision 1.1  2005/11/04 06:34:20  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 */

#ifndef _PSYNC
#define _PSYNC

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/contain.h>

class PSync : public PObject
{
  public:
  /**@name Operations */
  //@{
    /**Block until the synchronisation object is available
     */
    virtual void Wait() = 0;

    /**Signal that the synchronisation object is available
     */
    virtual void Signal() = 0;
  //@}

#ifdef P_PTHREADS
    PSync()
      : lockerId(pthread_t(-1)) { }
  protected:
    pthread_t lockerId;
#endif
};

class PSyncNULL : public PSync
{
  public:
    virtual void Wait() { }
    virtual void Signal() { }
};

/**This class waits for the semaphore on construction and automatically
   signals the semaphore on destruction. Any descendent of PSemaphore
   may be used.

  This is very usefull for constructs such as:
\begin{verbatim}
    void func()
    {
      PWaitAndSignal mutexWait(myMutex);
      if (condition)
        return;
      do_something();
      if (other_condition)
        return;
      do_something_else();
    }
\end{verbatim}
 */

class PWaitAndSignal {
  public:
    /**Create the semaphore wait instance.
       This will wait on the specified semaphore using the #Wait()# function
       before returning.
      */
    inline PWaitAndSignal(
      const PSync & sem,   ///< Semaphore descendent to wait/signal.
      BOOL wait = TRUE    ///< Wait for semaphore before returning.
    ) : sync((PSync &)sem)
    { if (wait) sync.Wait(); }

    /** Signal the semaphore.
        This will execute the Signal() function on the semaphore that was used
        in the construction of this instance.
     */
    ~PWaitAndSignal()
    { sync.Signal(); }

  protected:
    PSync & sync;
};

#endif // PSYNC

