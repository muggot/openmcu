/*
 * thread.h
 *
 * Executable thread encapsulation class (pre-emptive if OS allows).
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
 * $Log: thread.h,v $
 * Revision 1.54  2007/09/17 11:14:42  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.53  2007/09/17 05:30:43  rjongbloed
 * Added thread local storage in tracing to avoid a certain class of deadlocks.
 *
 * Revision 1.52  2007/07/08 23:54:53  rjongbloed
 * Fixed MSVC warnings
 *
 * Revision 1.51  2007/07/06 02:11:48  csoutheren
 * Add extra memory leak debugging on Linux
 * Remove compile warnings
 *
 * Revision 1.50  2007/06/25 20:17:35  csoutheren
 * Add ability to specify thread name
 *
 * Revision 1.49  2007/02/20 04:37:22  csoutheren
 * Fix for gcc
 *
 * Revision 1.48  2007/02/20 04:24:46  csoutheren
 * More rationalisation of PThread templates
 *
 * Revision 1.47  2007/02/19 04:38:15  csoutheren
 * Rationalised and documented thread creation templates
 *
 * Revision 1.46  2006/10/03 06:30:31  csoutheren
 * Fix error in PThreadObj templates
 *
 * Revision 1.45  2006/07/05 09:14:29  csoutheren
 * Fixed startup problems with using Resume in PThread descendants
 *
 * Revision 1.44  2006/07/05 03:59:19  csoutheren
 * Fix PThreadMain2Arg template
 *
 * Revision 1.43  2006/06/28 11:28:21  csoutheren
 * Patch 1456858 - increase default thread stack size
 * Thanks to drosario
 *
 * Revision 1.42  2006/05/25 23:28:27  csoutheren
 * Fixed compilation under gcc 4.0
 *
 * Revision 1.41  2006/05/23 22:11:27  csoutheren
 * Fixed compilation under gcc
 *
 * Revision 1.40  2006/05/23 00:58:05  csoutheren
 * Add templates for creating threads
 *
 * Revision 1.39  2006/01/29 22:35:46  csoutheren
 * Added fix for thread termination problems on SMP machines
 * Thanks to Derek Smithies
 *
 * Revision 1.38  2006/01/11 22:27:44  dereksmithies
 * Add extra comments describing the usage of Resume() in the constructor of a
 * class descended of PThread
 *
 * Revision 1.37  2005/11/30 12:47:38  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.36  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.35  2003/10/08 21:39:34  dereksmithies
 * Add a #define to cope with backward compatability issues for PThreadIdentifier
 * Thanks to Andrey S Pankov and Craig Southeren for their input.
 *
 * Revision 1.34  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.33  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.32  2002/10/04 04:33:27  robertj
 * Added functions for getting operating system thread identifier values.
 *
 * Revision 1.31  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.30  2002/06/27 06:44:28  robertj
 * Changed "" to PString::Empty() where assigning to PString.
 *
 * Revision 1.29  2002/04/24 01:49:22  robertj
 * Fixed error in PTRACE_BLOCk nesting level to now work when no tracing enabled.
 *
 * Revision 1.28  2002/04/24 01:09:56  robertj
 * Fixed problem with PTRACE_BLOCK indent level being correct across threads.
 *
 * Revision 1.27  2001/09/10 02:51:22  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.26  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.25  2000/11/28 12:55:36  robertj
 * Added static function to create a new thread class and automatically
 *   run a function on another class in the context of that thread.
 *
 * Revision 1.24  2000/10/20 05:31:09  robertj
 * Added function to change auto delete flag on a thread.
 *
 * Revision 1.23  2000/06/26 11:17:19  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.22  2000/02/29 12:26:14  robertj
 * Added named threads to tracing, thanks to Dave Harvey
 *
 * Revision 1.21  1999/06/06 05:07:17  robertj
 * Fixed documentation error.
 *
 * Revision 1.20  1999/03/09 02:59:51  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.19  1999/02/16 08:11:17  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.18  1998/11/20 03:18:33  robertj
 * Added thread WaitForTermination() function.
 *
 * Revision 1.17  1998/10/31 12:47:59  robertj
 * Removed ability to start threads immediately, race condition with vtable (Main() function).
 *
 * Revision 1.16  1998/09/23 06:21:41  robertj
 * Added open source copyright license.
 *
 * Revision 1.15  1996/03/02 03:15:51  robertj
 * Added automatic deletion of thread object instances on thread completion.
 *
 * Revision 1.14  1995/12/10 11:44:32  robertj
 * Fixed bug in non-platform threads and semaphore timeouts.
 *
 * Revision 1.13  1995/11/21 11:49:44  robertj
 * Added timeout on semaphore wait.
 *
 * Revision 1.12  1995/07/31 12:10:40  robertj
 * Added semaphore class.
 *
 * Revision 1.11  1995/06/17 11:13:35  robertj
 * Documentation update.
 *
 * Revision 1.10  1995/03/14 12:42:49  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.9  1995/01/16  09:42:13  robertj
 * Documentation.
 *
 * Revision 1.8  1994/09/25  10:45:22  robertj
 * Virtualised IsNoLongerBlocked for unix platform.
 *
 * Revision 1.6  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.5  1994/08/21  23:43:02  robertj
 * Added SuspendBlock state to cooperative multi-threading to fix logic fault.
 *
 * Revision 1.4  1994/08/04  12:32:22  robertj
 * Better name of thread block check function.
 *
 * Revision 1.3  1994/07/21  12:33:49  robertj
 * Moved cooperative threads to common.
 *
 * Revision 1.2  1994/07/02  03:03:49  robertj
 * Added restartable threads.
 *
 * Revision 1.1  1994/06/25  11:55:15  robertj
 * Initial revision
 *
 */

#ifndef _PTHREAD
#define _PTHREAD

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifdef Priority
#undef Priority
#endif

class PSemaphore;

#define PThreadIdentifer PThreadIdentifier

typedef P_THREADIDENTIFIER PThreadIdentifier;

///////////////////////////////////////////////////////////////////////////////
// PThread

/** This class defines a thread of execution in the system. A {\it thread} is
   an independent flow of processor instructions. This differs from a
   {\it process} which also embodies a program address space and resource
   allocation. So threads can share memory and resources as they run in the
   context of a given process. A process always contains at least one thread.
   This is reflected in this library by the #PProcess# class being
   descended from the PThread class.

   The implementation of a thread is platform dependent, but it is
   assumed that the platform has some support for native threads.
   Previous versions of PWLib has some support for co-operative
   threads, but this has been removed
 */
class PThread : public PObject
{
  PCLASSINFO(PThread, PObject);

  public:
  /**@name Construction */
  //@{
    /// Codes for thread priorities.
    enum Priority {
      /// Will only run if all other threads are blocked.
      LowestPriority,   

      /// Runs approximately half as often as normal.
      LowPriority,      

      /// Normal priority for a thread.
      NormalPriority,   

      /// Runs approximately twice as often as normal.
      HighPriority,     

      /// Is only thread that will run, unless blocked.
      HighestPriority,  

      NumPriorities
    };

    /// Codes for thread autodelete flag
    enum AutoDeleteFlag {
      /// Automatically delete thread object on termination.
      AutoDeleteThread,   

      /// Don't delete thread as it may not be on heap.
      NoAutoDeleteThread  
    };

    /** Create a new thread instance. Unless the #startSuspended#
       parameter is TRUE, the threads #Main()# function is called to
       execute the code for the thread.
       
       Note that the exact timing of the execution of code in threads can
       never be predicted. Thus you you can get a race condition on
       intialising a descendent class. To avoid this problem a thread is
       always started suspended. You must call the Resume() function after
       your descendent class construction is complete.

       If synchronisation is required between threads then the use of
       semaphores is essential.

       If the #deletion# is set to #AutoDeleteThread#
       then the PThread is assumed to be allocated with the new operator and
       may be freed using the delete operator as soon as the thread is
       terminated or executes to completion (usually the latter).

       The stack size argument retained only for source code compatibility for
       previous implementations. It is not used in the current code and
       may be removed in subsequent versions.
     */
    PThread(
      PINDEX ,                 ///< Not used - previously stack size
      AutoDeleteFlag deletion = AutoDeleteThread,
        ///< Automatically delete PThread instance on termination of thread.
      Priority priorityLevel = NormalPriority,  ///< Initial priority of thread.
      const PString & threadName = PString::Empty() ///< The name of the thread (for Debug/Trace)
    );

    /** Destroy the thread, this simply calls the #Terminate()# function
       with all its restrictions and penalties. See that function for more
       information.

       Note that the correct way for a thread to terminate is to return from
       the #Main()# function.
     */
    ~PThread();
  //@}

  /**@name Overrides from PObject */
  //@{
    /**Standard stream print function.
       The PObject class has a << operator defined that calls this function
       polymorphically.
      */
    void PrintOn(
      ostream & strm    ///< Stream to output text representation
    ) const;
  //@}

  /**@name Control functions */
  //@{
    /** Restart a terminated thread using the same stack priority etc that
       was current when the thread terminated.
       
       If the thread is still running then this function is ignored.
     */
    virtual void Restart();

    /** Terminate the thread. It is highly recommended that this is not used
       except in abnormal abort situations as not all clean up of resources
       allocated to the thread will be executed. This is especially true in
       C++ as the destructors of objects that are automatic variables are not
       called causing at the very least the possiblity of memory leaks.

       Note that the correct way for a thread to terminate is to return from
       the #Main()# function or self terminate by calling
       #Terminate()# within the context of the thread which can then
       assure that all resources are cleaned up.
     */
    virtual void Terminate();

    /** Determine if the thread has been terminated or ran to completion.

       @return
       TRUE if the thread has been terminated.
     */
    virtual BOOL IsTerminated() const;

    /** Block and wait for the thread to terminate.

       @return
       FALSE if the thread has not terminated and the timeout has expired.
     */
    void WaitForTermination() const;
    BOOL WaitForTermination(
      const PTimeInterval & maxWait  ///< Maximum time to wait for termination.
    ) const;

    /** Suspend or resume the thread.
    
       If #susp# is TRUE this increments an internal count of
       suspensions that must be matched by an equal number of calls to
       #Resume()# or #Suspend(FALSE)# before the
       thread actually executes again.

       If #susp# is FALSE then this decrements the internal count of
       suspensions. If the count is <= 0 then the thread will run. Note that
       the thread will not be suspended until an equal number of
       #Suspend(TRUE)# calls are made.
     */
    virtual void Suspend(
      BOOL susp = TRUE    ///< Flag to suspend or resume a thread.
    );

    /** Resume thread execution, this is identical to
       #Suspend(FALSE)#.

      The Resume() method may be called from within the constructor of a
      PThread descendant.  However, the Resume() should be in the
      constructor of the most descendant class. So, if you have a
      class B (which is descended of PThread), and a class C (which is
      descended of B), placing the call to Resume in the constructor of B is
      unwise.

      If you do place a call to Resume in the constructor, it
      should be at the end of the constructor, after all the other
      initialisation in the constructor.

      The reason the call to Resume() should be at the end of the
      construction process is simple - you want the thread to start
      when all the variables in the class have been correctly
      initialised.
     */
    virtual void Resume();

    /** Determine if the thread is currently suspended. This checks the
       suspension count and if greater than zero returns TRUE for a suspended
       thread.

       @return
       TRUE if thread is suspended.
     */
    virtual BOOL IsSuspended() const;

    /// Suspend the current thread for the specified amount of time.
    static void Sleep(
      const PTimeInterval & delay   ///< Time interval to sleep for.
    );

    /** Set the priority of the thread relative to other threads in the current
       process.
     */
    virtual void SetPriority(
      Priority priorityLevel    ///< New priority for thread.
    );

    /** Get the current priority of the thread in the current process.

       @return
       current thread priority.
     */
    virtual Priority GetPriority() const;

    /** Set the flag indicating thread object is to be automatically deleted
        when the thread ends.
     */
    virtual void SetAutoDelete(
      AutoDeleteFlag deletion = AutoDeleteThread  ///< New auto delete setting.
    );

    /** Reet the flag indicating thread object is to be automatically deleted
        when the thread ends.
     */
    void SetNoAutoDelete() { SetAutoDelete(NoAutoDeleteThread); }

    /** Get the name of the thread. Thread names are a optional debugging aid.

       @return
       current thread name.
     */
    virtual PString GetThreadName() const;

    /** Change the name of the thread. Thread names are a optional debugging aid.

       @return
       current thread name.
     */
    virtual void SetThreadName(
      const PString & name        ///< New name for the thread.
    );
  //@}

  /**@name Miscellaneous */
  //@{
    /** Get operating system specific thread identifier for this thread.
      * Note that the return value from these functions is only valid
      * if called by the owning thread. Calling this function for another
      * thread that may be terminating is a very bad idea.
      */
    virtual PThreadIdentifier GetThreadId() const;
    static PThreadIdentifier GetCurrentThreadId();

    /** User override function for the main execution routine of the thread. A
       descendent class must provide the code that will be executed in the
       thread within this function.
       
       Note that the correct way for a thread to terminate is to return from
       this function.
     */
    virtual void Main() = 0;

    /** Get the currently running thread object instance. It is possible, even
       likely, that the smae code may be executed in the context of differenct
       threads. Under some circumstances it may be necessary to know what the
       current codes thread is and this static function provides that
       information.

       @return
       pointer to current thread.
     */
    static PThread * Current();

    /** Yield to another thread without blocking.
        This duplicates the implicit thread yield that may occur on some
        I/O operations or system calls.

        This may not be implemented on all platforms.
     */
    static void Yield();

    /**Create a simple thread executing the specified notifier.
       This creates a simple PThread class that automatically executes the
       function defined by the PNotifier in the context of a new thread.
      */
    static PThread * Create(
      const PNotifier & notifier,     ///< Function to execute in thread.
      INT parameter = 0,              ///< Parameter value to pass to notifier.
      AutoDeleteFlag deletion = AutoDeleteThread,
        ///< Automatically delete PThread instance on termination of thread.
      Priority priorityLevel = NormalPriority,  ///< Initial priority of thread.
      const PString & threadName = PString::Empty(), ///< The name of the thread (for Debug/Trace)
      PINDEX stackSize = 65536         ///< Stack size on some platforms
    );
  //@}

  protected:
    void InitialiseProcessThread();
    /* Initialialise the primordial thread, the one in the PProcess. This is
       required due to the bootstrap logic of processes and threads.
     */

  private:
    PThread();
    // Create a new thread instance as part of a PProcess class.

    friend class PProcess;
    // So a PProcess can get at PThread() constructor but nothing else.

    PThread(const PThread &) { }
    // Empty constructor to prevent copying of thread instances.

    PThread & operator=(const PThread &) { return *this; }
    // Empty assignment operator to prevent copying of thread instances.

    BOOL autoDelete;
    // Automatically delete the thread on completion.

    // Give the thread a name for debugging purposes.
    PString threadName;

  private:
#if PTRACING
    PStringStream traceStream;
    unsigned      traceLevel;
    friend class PTrace;

    unsigned traceBlockIndentLevel;
    friend class PTrace::Block;
#endif


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/thread.h"
#else
#include "unix/ptlib/thread.h"
#endif
};

#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif

/** Define some templates to simplify the declaration
  * of simple PThread descendants with one or two paramaters 
  */

/*
   This class automates calling a global function with no arguments within it's own thread.
   It is used as follows:

   void GlobalFunction()
   {
   }

   ...
   PString arg;
   new PThreadMain(&GlobalFunction)
 */
class PThreadMain : public PThread
{
  PCLASSINFO(PThreadMain, PThread);
  public:
    typedef void (*FnType)(); 
    PThreadMain(FnType _fn, BOOL autoDelete = FALSE)
      : PThread(10000, autoDelete ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread), fn(_fn)
    { PThread::Resume(); }
    PThreadMain(const char * _file, int _line, FnType _fn, BOOL autoDelete = FALSE)
      : PThread(10000, autoDelete ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread,  NormalPriority,
        psprintf("%s:%08x-%s:%i", GetClass(), (void *)this, _file, _line)), fn(_fn)
    { PThread::Resume(); }
    virtual void Main()
    { (*fn)(); }
    FnType fn;
};

/*
   This template automates calling a global function with one argument within it's own thread.
   It is used as follows:

   void GlobalFunction(PString arg)
   {
   }

   ...
   PString arg;
   new PThread1Arg<PString>(arg, &GlobalFunction)
 */
template<typename Arg1Type>
class PThread1Arg : public PThread
{
  PCLASSINFO(PThread1Arg, PThread);
  public:
    typedef void (*FnType)(Arg1Type arg1); 
    PThread1Arg(Arg1Type _arg1, FnType _fn, BOOL autoDelete = FALSE)
      : PThread(10000, autoDelete ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread), fn(_fn),
        arg1(_arg1)
    { PThread::Resume(); }
    PThread1Arg(const char * _file, int _line, Arg1Type _arg1, FnType _fn, BOOL autoDelete = FALSE)
      : PThread(10000, autoDelete ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread,  NormalPriority,
        psprintf("%s:%08x-%s:%i", GetClass(), (void *)this, _file, _line)), fn(_fn),
        arg1(_arg1)
    { PThread::Resume(); }
    virtual void Main()
    { (*fn)(arg1); }
    FnType fn;
    Arg1Type arg1;
};


/*
   This template automates calling a global function with two arguments within it's own thread.
   It is used as follows:

   void GlobalFunction(PString arg1, int arg2)
   {
   }

   ...
   PString arg;
   new PThread2Arg<PString, int>(arg, &GlobalFunction)
 */
template<typename Arg1Type, typename Arg2Type>
class PThread2Arg : public PThread
{
  PCLASSINFO(PThread2Arg, PThread);
  public:
    typedef void (*FnType)(Arg1Type arg1, Arg2Type arg2); 
    PThread2Arg(Arg1Type _arg1, Arg2Type _arg2, FnType _fn, BOOL autoDelete = FALSE)
      : PThread(10000, autoDelete ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread), fn(_fn),
        arg1(_arg1), arg2(_arg2)
    { PThread::Resume(); }
    PThread2Arg(const char * _file, int _line, Arg1Type _arg1, Arg2Type _arg2, FnType _fn, BOOL autoDelete = FALSE)
      : PThread(10000, autoDelete ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread, NormalPriority,
        psprintf("%s:%08x-%s:%i", GetClass(), (void *)this, _file, _line)), fn(_fn),
        arg1(_arg1), arg2(_arg2)
    { PThread::Resume(); }
    virtual void Main()
    { (*fn)(arg1, arg2); }
    FnType fn;
    Arg1Type arg1;
    Arg2Type arg2;
};

/*
   This template automates calling a member function with no arguments within it's own thread.
   It is used as follows:

   class Example {
     public:
      void Function()
      {
      }
   };

   ...
   Example ex;
   new PThreadObj<Example>(ex, &Example::Function)
 */

template <typename ObjType>
class PThreadObj : public PThread
{
  public:
  PCLASSINFO(PThreadObj, PThread);
  public:
    typedef void (ObjType::*ObjTypeFn)(); 
    PThreadObj(ObjType & _obj, ObjTypeFn _fn, BOOL autoDelete = FALSE)
      : PThread(10000, autoDelete ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread),
        obj(_obj), fn(_fn)
    { PThread::Resume(); }
    PThreadObj(const char * _file, int _line, ObjType & _obj, ObjTypeFn _fn, BOOL autoDelete = FALSE)
      : PThread(10000, autoDelete ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread, NormalPriority,
        psprintf("%s:%08x-%s:%i", GetClass(), (void *)this, _file, _line)),
        obj(_obj), fn(_fn)
    { PThread::Resume(); }
    void Main()
    { (obj.*fn)(); }

  protected:
    ObjType & obj;
    ObjTypeFn fn;
};


/*
   This template automates calling a member function with one argument within it's own thread.
   It is used as follows:

   class Example {
     public:
      void Function(PString arg)
      {
      }
   };

   ...
   Example ex;
   PString str;
   new PThreadObj1Arg<Example>(ex, str, &Example::Function)
 */
template <class ObjType, typename Arg1Type>
class PThreadObj1Arg : public PThread
{
  PCLASSINFO(PThreadObj1Arg, PThread);
  public:
    typedef void (ObjType::*ObjTypeFn)(Arg1Type); 
    PThreadObj1Arg(ObjType & _obj, Arg1Type _arg1, ObjTypeFn _fn, BOOL autoDelete = FALSE)
      : PThread(10000, autoDelete ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread),
				obj(_obj), fn(_fn), arg1(_arg1)
    { PThread::Resume(); }
    PThreadObj1Arg(const char * _file, int _line, ObjType & _obj, Arg1Type _arg1, ObjTypeFn _fn, BOOL autoDelete = FALSE)
      : PThread(10000, autoDelete ? PThread::AutoDeleteThread : PThread::NoAutoDeleteThread, NormalPriority,
                                psprintf("%s:%08x-%s:%i", GetClass(), (void *)this, _file, _line)),
				obj(_obj), fn(_fn), arg1(_arg1)
    { PThread::Resume(); }
    void Main()
    { (obj.*fn)(arg1); }

  protected:
    ObjType & obj;
    ObjTypeFn fn;
    Arg1Type arg1;
};

#ifdef _MSC_VER
#pragma warning(default:4355)
#endif

#endif // _PTHREAD

// End Of File ///////////////////////////////////////////////////////////////
