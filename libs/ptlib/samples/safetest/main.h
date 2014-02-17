/*
 * main.h
 *
 * PWLib application header file for safetest
 *
 * Copyright (c) 2003 Equivalence Pty. Ltd.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: main.h,v $
 * Revision 1.11  2006/10/14 22:55:50  dereksmithies
 * Remove garbage thread creation/deletion. use pwlib method instead.
 * Add a poor mechanism to wait for all threads to end.
 *
 * Revision 1.10  2006/06/21 03:28:43  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.9  2006/03/25 09:01:44  dereksmithies
 * Add reporting options, and different methods for spawning threads. All stable and reliable.
 *
 * Revision 1.8  2006/03/23 05:07:28  dereksmithies
 * Fix threading issues - I think.
 *
 * Revision 1.7  2006/03/22 04:24:51  dereksmithies
 * Tidyups. Add Pragmas. make it slightly more friendly for 1 cpu boxes.
 *
 * Revision 1.6  2006/02/13 04:17:23  dereksmithies
 * Formatting fixes.
 *
 * Revision 1.5  2006/02/12 21:42:07  dereksmithies
 * Add lots of doxygen style comments, and an introductory page.
 *
 * Revision 1.4  2006/02/09 21:43:15  dereksmithies
 * Remove the notion of CleanerThread. This just confuses things.
 *
 * Revision 1.3  2006/02/09 21:07:23  dereksmithies
 * Add new (and temporary) thread to close down each DelayThread instance.
 * Now, it is less cpu intensive. No need for garbage thread to run.
 *
 * Revision 1.2  2006/02/07 02:02:00  dereksmithies
 * use a more sane method to keep track of the number of running DelayThread instances.
 *
 * Revision 1.1  2006/02/07 01:02:56  dereksmithies
 * Initial release of code to test the PSafeDictionary structure in pwlib.
 * Thanks to Indranet Technologies for supporting this work.
 *
 *
 *
 */

#ifndef _SafeTest_MAIN_H
#define _SafeTest_MAIN_H


#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/pprocess.h>
#include <ptclib/random.h>

#include <ptlib/safecoll.h>

class SafeTest;
class DelayThread;
class LauncherThread;

/////////////////////////////////////////////////////////////////////////////
/**This class writes regular reports to the console on progress */
class ReporterThread : public PThread
{
  PCLASSINFO(ReporterThread, PThread);

 public:
  /**Constructor, to link back to the LaucherThread*/
  ReporterThread(LauncherThread & _launcher);

  /**Shut down this thread */
  void Terminate();
  
  /**Do the work of reporting */
  void Main();

 protected:
  /**Link to the LauncherThread, which we use for reporting with */
  LauncherThread & launcher;

  /**Flag used for timing the 1 minute between reports, and wake up on
     termination */
  PSyncPoint exitFlag;
  
  /**Flag to indicate end this thread */
  BOOL terminateNow;
};

/////////////////////////////////////////////////////////////////////////////

/**This class is written to avoid the usage of the PThread::Create mechanism. 
   It is used in terminating a DelayThread class. */
class DelayThreadTermination : public PThread
{
  PCLASSINFO(DelayThreadTermination, PThread);
 public:
  DelayThreadTermination(DelayThread &_delayThread);

  /**Do the work of terminating the DelayThread instance */
  void Main();

 protected:
  
  /**Master reference to the thread we do delay for */
  DelayThread & delayThread;

  /**The name we assign to this thread */
  PStringStream thisThreadName;
};


/////////////////////////////////////////////////////////////////////////////

/**This class is written to avoid the usage of the PThread::Create
   mechanism. It calls the delay method in the DelayThread instance,
   and exits.
*/
class DelayWorkerThread : public PThread
{
  PCLASSINFO(DelayWorkerThread, PThread);
 public:
  DelayWorkerThread(DelayThread &_delayThread, PInt64 _iteration);

  /**Do the work of advising the SafeTest about this */
  void Main();

 protected:
  
  /**Master reference to the thread we do delay for */
  DelayThread & delayThread;

  /** Iteration we are representing */
  PInt64 iteration;

  /**The name we make up for this DelayWorkerThread instance */
  PStringStream thisThreadName;
};



/**This class has the job of closing a DelayThread instance. It
   advises the master SafeTest of the end of the DelayThread, and then
   exits.  This class is instantiated by the DelayThread class, waits
   for 1 second, and then kills the DelayThread instance */
class OnDelayThreadEnd : public PThread
{
  PCLASSINFO(OnDelayThreadEnd, PThread);
 public:
  OnDelayThreadEnd(SafeTest &_safeTest, const PString & _delayThreadId);

  /**Do the work of advising the SafeTest about this */
  void Main();

 protected:
  
  /**Master reference to the master id */
  SafeTest &safeTest;

  /**Id of the DelayThread instance we end */
  PString delayThreadId;
};
 
  
/**This class is a simple simple thread that just creates, waits a
   period of time, and exits.It is designed to test the PwLib methods
   for reporting the status of a thread. This class will be created
   over and over- millions of times is possible if left long
   enough. */
class DelayThread : public PSafeObject
{
  PCLASSINFO(DelayThread, PSafeObject);
  
public:
  /**Create this class, so it runs for the specified delay period, The
     class is assigned a unique ID tag, based on the parameter
     idNumber */
  DelayThread(SafeTest & _safeTest, PINDEX _delay, PInt64 idNumber);
    
  /**Destroy this class. Includes a check to see if this class is
     still running*/
  ~DelayThread();

  /**Last thing the thread which runs in this class does, and
     initiates the close down */
  void Release();

  /**Report the id used by this class */
  PString GetId() { return id; }

  /**Pretty print the id of this class */
  virtual void PrintOn(ostream & strm) const;

#ifdef DOC_PLUS_PLUS
  /**This method is where the delay is done */
    virtual void DelayThreadMain(PThread &, INT);
#else
    PDECLARE_NOTIFIER(PThread, DelayThread, DelayThreadMain);
#endif

#ifdef DOC_PLUS_PLUS
  /**This contains the 1 notifier that is used when closing down this
     instance of DelayThread class. It is called by a custom thread,
     which initiates the deletion of this Delaythread instance */
    virtual void OnReleaseThreadMain(PThread &, INT);
#else
    PDECLARE_NOTIFIER(PThread, DelayThread, OnReleaseThreadMain);
#endif

 protected:

    /**Reference back to the class that knows everything, and holds the list
       of instances of this DelayThread class */
    SafeTest & safeTest;

    /**The time period (ms) we have to wait for in this thread */
    PINDEX delay;

    /**the label assigned to this instance of this DelayThread class */
    PString id;

    /**Flag to indicate we are still going */
    BOOL threadRunning;

    /**The name of this thread */
    PStringStream name;
};

////////////////////////////////////////////////////////////////////////////////
/**This thread handles the Users console requests to query the status of 
   the launcher thread. It provides a means for the user to close down this
   program - without having to use Ctrl-C*/
class UserInterfaceThread : public PThread
{
  PCLASSINFO(UserInterfaceThread, PThread);
  
public:
  /**Constructor */
  UserInterfaceThread(SafeTest &_safeTest)
    : PThread(10000, NoAutoDeleteThread), safeTest(_safeTest)
    { }

  /**Do the work of listening for user commands from the command line */
  void Main();
    
 protected:
  /**Reference to the class that holds the key data on everything */
  SafeTest & safeTest;
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
  /**Create this thread. Note that the program only contains one
     instance of this thread */
  LauncherThread(SafeTest &_safe_test);
  
  /**Destructor. The only done is to remove the ReporterThread */
  ~LauncherThread();

  /**Where all the work is done */
  void Main();
    
  /**Access function, which is callled by the UserInterfaceThread.  It
   reports the number of DelayThread instances that have been
   created.*/
  PInt64 GetIteration() { return iteration; }

  /**Cause this thread to stop work and end */
  virtual void Terminate() { keepGoing = FALSE; }

  /**Access function, which is callled by the UserInterfaceThread.
   It reports the time since this program stared.*/
  PTimeInterval GetElapsedTime() { return PTime() - startTime; }

  /**Report the time to launch each DelayThread instance */
  void ReportAverageTime();

  /**Report the number of DelayThread instances we have launched */
  void ReportIterations();

  /**Report the time elapsed since this began launching DelayThread
     instances */
  void ReportElapsedTime();

 protected:
  /**Reference back to the master class */
  SafeTest & safeTest;

  /**Pointer to the reporting thread that may be running */
  PThread *reporter;

  /**Count on the number of DelayThread instances that have been
     created */
  PInt64          iteration;
  
  /**The time at which this program was started */
  PTime           startTime;

  /**A flag to indicate that we have to keep on doing our job, of
     launching DelayThread instances */
  BOOL            keepGoing;
};

////////////////////////////////////////////////////////////////////////////////

/**
   The core class that a)processes command line and b)launches
   relevant classes. This class contains a wrapper for the 
   \code int main(int argc, char **argv); \endcode function */
class SafeTest : public PProcess
{
  PCLASSINFO(SafeTest, PProcess)

  public:
  /**Constructor */
    SafeTest();

  /**Where execution first starts in this program, and it does all the
     command line processing */
    virtual void Main();

    /**Report the user specified delay, which is used in DelayThread
       instances. Units are in milliseconds */
    PINDEX Delay()    { return delay; }

    /**Report the user specified number of DelayThreads that can be in
       action. */
    PINDEX ActiveCount() { return activeCount; }

    /**Callback for removing a DelayThread from the list of active
       delaythreads */
    void OnReleased(DelayThread & delayThread);

    /**Callback for removing a DelayThread from the list of active
       delaythreads */
    void OnReleased(const PString & delayThreadId);

    /**Append this DelayThread to delayThreadsActive, cause it is a valid
       and running DelayThread */
    void AppendRunning(PSafePtr<DelayThread> delayThread, PString id);

    /**Number of active DelayThread s*/
    PINDEX CurrentSize() { return currentSize; }

    /**Find a DelayThread instance witth the specified token.
 
       Note the caller of this function MUST call the DelayThread::Unlock()
       function if this function returns a non-NULL pointer. If it does not
       then a deadlock can occur.
      */
    PSafePtr<DelayThread> FindDelayThreadWithLock(
      const PString & token,  ///<  Token to identify connection
      PSafetyMode mode = PSafeReadWrite
    ) { return delayThreadsActive.FindWithLock(token, mode); }


    /**Return a random number, of size 0 .. (delay/4), for use in
       making the delay threads random in duration. */
    PINDEX GetRandom() { return random.Generate() % (delay >> 2); }

    /**Report the status of the useOnThreadEnd flag */
    BOOL UseOnThreadEnd();

    /**Return TRUE or FALSE, to decide if we use PThread::Create */
    BOOL AvoidPThreadCreate() { return avoidPThreadCreate; }


    /**Return TRUE or FALSE to determine if a thread should be
       launched to regularly report on status */
    BOOL RegularReporting() { return regularReporting; }
 protected:

    /**The thread safe list of DelayThread s that we manage */
    class DelayThreadsDict : public PSafeDictionary<PString, DelayThread>
    {
      /**One function that we have to define for this usage, and that
         is the deletion of existing methods. PWLib will have removed
         the object to be deleted from the list. We simply need to
         delete it. */
        virtual void DeleteObject(PObject * object) const;
    } delayThreadsActive;

    /**The flag to say when we exit */
    BOOL exitNow;
     
    /**The delay each thread has to wait for */
    PINDEX delay;

    /**The number of threads that can be active */
    PINDEX activeCount;

    /**The number of entries in the dictionary */
    PAtomicInteger currentSize;

    /**Random number generator, which is used to keep track of the
       variability in the delay period */
    PRandom random;

    /**Flag to indicate that we use the OnDelayThreadEnd mechanism for
       signifing the end of a DelayThread */
    BOOL useOnThreadEnd;

    /**Flag to indicate if we can use PThread::Create to generate
       temporary thread, or if we use the supplied thread class */
    BOOL avoidPThreadCreate;
    

    /**Flag to determine if a thread is used to regularly report on status */
    BOOL regularReporting;
};



#endif  // _SafeTest_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
