/*
 * main.h
 *
 * PWLib application header file for ptimer
 *
 * Copyright (c) 2006 Indranet Technologies Ltd.
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
 * The Initial Developer of the Original Code is Indranet Technologies Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: main.h,v $
 * Revision 1.4  2007/05/01 03:15:26  dereksmithies
 * Add a second test, to test the repeated initialisation of some PTimer instances.
 *
 * Revision 1.3  2006/06/21 03:28:42  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.2  2006/05/24 02:28:18  dereksmithies
 * add separate thread to get the timer to start.
 * Add option to check if the timer has started.
 * Fix use of the parameters.
 *
 * Revision 1.1  2006/05/23 04:36:49  dereksmithies
 * Initial release of a test program to examine the operation of PTimer.
 *
 *
 *
 */

#ifndef _PTimer_MAIN_H
#define _PTimer_MAIN_H

#include <ptlib/pprocess.h>

/**A class that does a PTimer functionality. This class runs once. It
   is started, and on completion of the delay it toggles a flag. At
   that point, this timer has finished. */
class MyTimer : public PTimer
{
  PCLASSINFO(MyTimer, PTimer);
 public:
  /**constructor */
  MyTimer();

  /**method used to start everything */
  void StartRunning(PSyncPoint * _exitFlag, PINDEX delayMs);

  /**Method called when this timer finishes */
  virtual void OnTimeout();

 protected:
  /**The flag to mark the end of this timer */
  PSyncPoint *exitFlag;
  
  /**The duration we delay for */
  PTimeInterval delayPeriod;
  
  /**The time at which we started */
  PTime startTime;


};

/////////////////////////////////////////////////////////////////////////////
  
/**This class is a simple simple thread that just creates, waits a
   period of time, and exits.It is designed to test the PwLib methods
   for reporting the status of a thread. This class will be created
   over and over- millions of times is possible if left long
   enough. If the pwlib thread status functions are broken, a segfault
   will result. Past enxperience has found a fault in pwlib with the
   BusyWait option on, with SMP machines and a delay period of 20ms */
class DelayThread : public PThread
{
  PCLASSINFO(DelayThread, PThread);
  
public:
  DelayThread(PINDEX _delay, BOOL _checkTimer);
    
  ~DelayThread();

  void Main();

 protected:
  PINDEX delay;

  BOOL checkTimer;

  MyTimer localPTimer;

  PSyncPoint endMe;
};

/////////////////////////////////////////////////////////////////////////////
/**This class turns the ptimer on from a separate thread to the delay thread.
   Then, DelayThread checks this ptimer, and waits for it to finish.
*/
class TimerOnThread : public PThread
{
  PCLASSINFO(TimerOnThread, PThread);
  
public:
  TimerOnThread(PTimer & _timer);
    
  void Main();

 protected:

  PTimer timer;
};

////////////////////////////////////////////////////////////////////////////////
/**This thread handles the Users console requests to query the status of 
   the launcher thread. It provides a means for the user to close down this
   program - without having to use Ctrl-C*/
class UserInterfaceThread : public PThread
{
  PCLASSINFO(UserInterfaceThread, PThread);
  
public:
  UserInterfaceThread()
    : PThread(10000, NoAutoDeleteThread)
    { }

  void Main();
    
 protected:
};

///////////////////////////////////////////////////////////////////////////////
/**This thread launches multiple instances of the BusyWaitThread. Each
   thread launched is busy monitored for termination. When the thread
   terminates, the thread is deleted, and a new one is created. This
   process repeats until segfault or termination by the user */
class LauncherThread : public PThread
{
  PCLASSINFO(LauncherThread, PThread);
  
public:
  LauncherThread()
    : PThread(10000, NoAutoDeleteThread)
    { iteration = 0; keepGoing = TRUE; }
  
  void Main();
    
  PINDEX GetIteration() { return iteration; }

  virtual void Terminate() { keepGoing = FALSE; }

  PTimeInterval GetElapsedTime() { return PTime() - startTime; }

 protected:
  PINDEX iteration;
  PTime startTime;
  BOOL  keepGoing;
};

////////////////////////////////////////////////////////////////////////////////


class PTimerTest : public PProcess
{
  PCLASSINFO(PTimerTest, PProcess)

  public:
    PTimerTest();
    virtual void Main();

    PINDEX Delay()      { return delay; }

    PINDEX Interval()   { return interval; }

    BOOL   CheckTimer() { return checkTimer; }

    static PTimerTest & Current()
      { return (PTimerTest &)PProcess::Current(); }
    
    void TooSoon(PTimeInterval & elapsed);

 protected:

    PINDEX delay;

    PINDEX interval;

    BOOL   checkTimer;


    /**Code to run the second test supported by this application. */
    void RunSecondTest();

  /**First internal timer that we manage */
  PTimer firstTimer;

  /**Second internal timer that we manage */
  PTimer secondTimer;

#ifdef DOC_PLUS_PLUS
  /**A pwlib callback function which is activated when the first timer
   * fires */
    void OnFirstTimerExpired(PTimer &, INT);

  /**A pwlib callback function which is activated when the second timer 
     fires.. */
    void OnSecondTimerExpired(PTimer &, INT);
#else
    PDECLARE_NOTIFIER(PTimer, PTimerTest, OnFirstTimerExpired);

    PDECLARE_NOTIFIER(PTimer, PTimerTest, OnSecondTimerExpired);
#endif

#ifdef DOC_PLUS_PLUS
    /**This Thread will continually restart the first timer. If
       there is a bug in pwlib, it will eventually lock up and do no more. At
       which point, the monitor thread will fire, and say, nothing is
       happening. This thread sets the value of an atomic integer every time
       it runs, to indicate activity.*/
    virtual void RestartFirstTimerMain(PThread &, INT);
#else
    PDECLARE_NOTIFIER(PThread, PTimerTest, RestartFirstTimerMain);
#endif

#ifdef DOC_PLUS_PLUS
    /**This Thread will continually restart the second timer. If
       there is a bug in pwlib, it will eventually lock up and do no more. At
       which point, the monitor thread will fire, and say, nothing is
       happening. This thread sets the value of an atomic integer every time
       it runs, to indicate activity.*/
    virtual void RestartSecondTimerMain(PThread &, INT);
#else
    PDECLARE_NOTIFIER(PThread, PTimerTest, RestartSecondTimerMain);
#endif

/**The integer that is set, to indicate activity of the RestartTimer thread */
    PAtomicInteger restartActivity;

    

};



#endif  // _PTimer_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
