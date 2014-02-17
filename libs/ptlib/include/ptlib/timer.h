/*
 * timer.h
 *
 * Real time down counting time interval class.
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
 * $Log: timer.h,v $
 * Revision 1.29  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.28  2005/06/02 19:25:18  dsandras
 * Applied patch from Miguel Rodríguez Pérez <miguelrp  @  gmail.com> (migras) to fix compilation with gcc 4.0.1.
 *
 * Revision 1.27  2003/09/17 09:01:00  csoutheren
 * Moved PSmartPointer and PNotifier into seperate files
 * Added detection for system regex libraries on all platforms
 *
 * Revision 1.26  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.25  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.24  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.23  2002/05/28 13:05:26  robertj
 * Fixed PTimer::SetInterval so it restarts timer as per operator=()
 *
 * Revision 1.22  2002/04/09 00:09:10  robertj
 * Improved documentation on PTimer usage.
 *
 * Revision 1.21  2001/11/14 06:06:26  robertj
 * Added functions on PTimer to get reset value and restart timer to it.
 *
 * Revision 1.20  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.19  2000/08/30 03:16:59  robertj
 * Improved multithreaded reliability of the timers under stress.
 *
 * Revision 1.18  2000/01/06 14:09:42  robertj
 * Fixed problems with starting up timers,losing up to 10 seconds
 *
 * Revision 1.17  1999/03/09 02:59:51  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.16  1999/02/16 08:11:17  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.15  1998/09/23 06:21:45  robertj
 * Added open source copyright license.
 *
 * Revision 1.14  1996/12/21 07:57:22  robertj
 * Fixed possible deadlock in timers.
 *
 * Revision 1.13  1996/05/18 09:18:37  robertj
 * Added mutex to timer list.
 *
 * Revision 1.12  1995/06/17 11:13:36  robertj
 * Documentation update.
 *
 * Revision 1.11  1995/04/02 09:27:34  robertj
 * Added "balloon" help.
 *
 * Revision 1.10  1995/03/14 12:42:51  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.9  1995/01/18  09:01:06  robertj
 * Added notifiers to timers.
 * Documentation.
 *
 * Revision 1.8  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.7  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.6  1994/07/02  03:03:49  robertj
 * Redesign of timers.
 *
 * Revision 1.5  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.4  1994/03/07  07:38:19  robertj
 * Major enhancementsacross the board.
 *
 * Revision 1.3  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.2  1993/08/31  03:38:02  robertj
 * Added missing virtual on destructor.
 *
 * Revision 1.1  1993/08/27  18:17:47  robertj
 * Initial revision
 *
 */

#ifndef _PTIMER
#define _PTIMER

#ifdef P_USE_PRAGMA
#pragma interface
#endif

class PThread;

#include <ptlib/notifier.h>

/**
   A class representing a system timer. The time interval ancestor value is
   the amount of time left in the timer.

   A timer on completion calls the virtual function #OnTimeout()#. This
   will in turn call the callback function provided by the instance. The user
   may either override the virtual function or set a callback as desired.
   
   A list of active timers is maintained by the applications #PProcess# 
   instance and the timeout functions are executed in the context of a single
   thread of execution. There are many consequences of this: only one timeout
   function can be executed at a time and thus a user should not execute a
   lot of code in the timeout call-back functions or it will dealy the timely
   execution of other timers call-back functions.

   Also timers are not very accurate in sub-second delays, even though you can
   set the timer in milliseconds, its accuracy is only to -0/+250 ms. Even
   more (potentially MUCH more) if there are delays in the user call-back
   functions.

   Another trap is you cannot destroy a timer in its own call-back. There is
   code to cause an assert if you try but it is very easy to accidentally do
   this when you delete an object that contains an onject that contains the
   timer!

   Finally static timers cause race conditions on start up and termination and
   should be avoided.
 */
class PTimer : public PTimeInterval
{
  PCLASSINFO(PTimer, PTimeInterval);

  public:
  /**@name Construction */
  //@{
    /** Create a new timer object and start it in one shot mode for the
       specified amount of time. If the time was zero milliseconds then the
       timer is {\bf not} started, ie the callback function is not executed
       immediately.
      */
    PTimer(
      long milliseconds = 0,  ///< Number of milliseconds for timer.
      int seconds = 0,        ///< Number of seconds for timer.
      int minutes = 0,        ///< Number of minutes for timer.
      int hours = 0,          ///< Number of hours for timer.
      int days = 0            ///< Number of days for timer.
    );
    PTimer(
      const PTimeInterval & time    ///< New time interval for timer.
    );

    /** Restart the timer in one shot mode using the specified time value. If
       the timer was already running, the "time left" is simply reset.

       @return
       reference to the timer.
     */
    PTimer & operator=(
      DWORD milliseconds            ///< New time interval for timer.
    );
    PTimer & operator=(
      const PTimeInterval & time    ///< New time interval for timer.
    );

    /** Destroy the timer object, removing it from the applications timer list
       if it was running.
     */
    virtual ~PTimer();
  //@}

  /**@name Control functions */
  //@{
    /** Set the value of the time interval. The time interval, in milliseconds,
       is the sum of all of the parameters. For example all of the following
       are equivalent:
\begin{verbatim}
              SetInterval(120000)
              SetInterval(60000, 60)
              SetInterval(60000, 0, 1)
              SetInterval(0, 60, 1)
              SetInterval(0, 0, 2)
\end{verbatim}
     */
    virtual void SetInterval(
      PInt64 milliseconds = 0,  ///< Number of milliseconds for interval.
      long seconds = 0,         ///< Number of seconds for interval.
      long minutes = 0,         ///< Number of minutes for interval.
      long hours = 0,           ///< Number of hours for interval.
      int days = 0              ///< Number of days for interval.
    );

    /** Start a timer in continous cycle mode. Whenever the timer runs out it
       is automatically reset to the time specified. Thus, it calls the
       notification function every time interval.
     */
    void RunContinuous(
      const PTimeInterval & time    // New time interval for timer.
    );

    /** Stop a running timer. The imer will not call the notification function
       and is reset back to the original timer value. Thus when the timer
       is restarted it begins again from the beginning.
     */
    void Stop();

    /** Determine if the timer is currently running. This really is only useful
       for one shot timers as repeating timers are always running.
       
       @return
       TRUE if timer is still counting.
     */
    BOOL IsRunning() const;

    /** Pause a running timer. This differs from the #Stop()# function in
       that the timer may be resumed at the point that it left off. That is
       time is "frozen" while the timer is paused.
     */
    void Pause();

    /** Restart a paused timer continuing at the time it was paused. The time
       left at the moment the timer was paused is the time until the next
       call to the notification function.
     */
    void Resume();

    /** Determine if the timer is currently paused.

       @return
       TRUE if timer paused.
     */
    BOOL IsPaused() const;

    /** Restart a timer continuing from the time it was initially.
     */
    void Reset();

    /** Get the time this timer was set to initially.
     */
    const PTimeInterval & GetResetTime() const;
  //@}

  /**@name Notification functions */
  //@{
    /**This function is called on time out. That is when the system timer
       processing decrements the timer from a positive value to less than or
       equal to zero. The interval is then reset to zero and the function
       called.

       Please note that the application should not execute large amounts of
       code in this call back or the accuracy of ALL timers can be severely
       impacted.

       The default behaviour of this function is to call the #PNotifier# 
       callback function.
     */
    virtual void OnTimeout();

    /** Get the current call back function that is called whenever the timer
       expires. This is called by the #OnTimeout()# function.

       @return
       current notifier for the timer.
     */
    const PNotifier & GetNotifier() const;

    /** Set the call back function that is called whenever the timer expires.
       This is called by the #OnTimeout()# function.
     */
    void SetNotifier(
      const PNotifier & func  // New notifier function for the timer.
    );
  //@}

  /**@name Global real time functions */
  //@{
    /** Get the number of milliseconds since some arbtrary point in time. This
       is a platform dependent function that yields a real time counter.
       
       Note that even though this function returns milliseconds, the value may
       jump in minimum quanta according the platforms timer system, eg under
       MS-DOS and MS-Windows the values jump by 55 every 55 milliseconds. The
       #Resolution()# function may be used to determine what the minimum
       time interval is.
    
       @return
       millisecond counter.
     */
    static PTimeInterval Tick();

    /** Get the smallest number of milliseconds that the timer can be set to.
       All actual timing events will be rounded up to the next value. This is
       typically the platforms internal timing units used in the #Tick()#
       function.
       
       @return
       minimum number of milliseconds per timer "tick".
     */
    static unsigned Resolution();
  //@}

  private:
    void Construct();

    /* Start or restart the timer from the #resetTime# variable.
       This is an internal function.
     */
    void StartRunning(
      BOOL once   // Flag for one shot or continuous.
    );

    /* Process the timer decrementing it by the delta amount and calling the
       #OnTimeout()# when zero. This is used internally by the
       #PTimerList::Process()# function.
     */
    void Process(
      const PTimeInterval & delta,    // Time interval since last call.
      PTimeInterval & minTimeLeft     // Minimum time left till next timeout.
    );

  // Member variables
    PNotifier callback;
    // Callback function for expired timers.

    PTimeInterval resetTime;
    // The time to reset a timer to when RunContinuous() is called.

    BOOL oneshot;
    // Timer operates once then stops.

    enum { Stopped, Starting, Running, Paused } state;
    // Timer state.


  friend class PTimerList;
    class PTimerList * timerList;


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/timer.h"
#else
#include "unix/ptlib/timer.h"
#endif
};

#endif


// End Of File ///////////////////////////////////////////////////////////////
