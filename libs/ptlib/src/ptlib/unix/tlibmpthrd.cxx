/*
 * tlibmpthrd.cxx
 *
 * Routines for Macintosh pre-emptive threading system
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
 * $Log: tlibmpthrd.cxx,v $
 * Revision 1.4  2002/06/27 06:38:58  robertj
 * Changes to remove memory leak display for things that aren't memory leaks.
 *
 * Revision 1.3  2002/02/19 07:40:59  rogerh
 * Remove PMutex destructor for Carbon.
 *
 * Revision 1.2  2002/02/19 07:28:02  rogerh
 * PXAbortIO -> PXAbortBlock. Submitted by Peter Johnson <paj@mac.com>
 *
 * Revision 1.1  2001/08/11 15:38:43  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 */

#include <sys/resource.h>
#include <new> // just because I want to throw std::bad_alloc...

#ifndef NDEBUG
#define DEBUG_THREADS
extern int debug_mpthreads;
#endif

PDECLARE_CLASS(PHouseKeepingThread, PThread)
  public:
    PHouseKeepingThread()
      : PThread(1000, NoAutoDeleteThread, NormalPriority, "Housekeeper")
      { closing = FALSE; Resume(); }

    void Main();
    void SetClosing() { closing = TRUE; }

  protected:
    BOOL closing;
};


#define new PNEW


int PThread::PXBlockOnIO(int handle, int type, const PTimeInterval & timeout)
{
  //PTRACE(1,"PThread::PXBlockOnIO(" << handle << ',' << type << ')');

  // make sure we flush the buffer before doing a write
  fd_set tmp_rfd, tmp_wfd, tmp_efd;
  fd_set * read_fds      = &tmp_rfd;
  fd_set * write_fds     = &tmp_wfd;
  fd_set * exception_fds = &tmp_efd;

  struct timeval * tptr = NULL;
  struct timeval   timeout_val;
  if (timeout != PMaxTimeInterval) {
    static const PTimeInterval oneDay(0, 0, 0, 0, 1);
    if (timeout < oneDay) {
      timeout_val.tv_usec = (timeout.GetMilliSeconds() % 1000) * 1000;
      timeout_val.tv_sec  = timeout.GetSeconds();
      tptr                = &timeout_val;
    }
  }

  int retval;

  for (;;) {

    FD_ZERO (read_fds);
    FD_ZERO (write_fds);
    FD_ZERO (exception_fds);

    switch (type) {
      case PChannel::PXReadBlock:
      case PChannel::PXAcceptBlock:
        FD_SET (handle, read_fds);
        break;
      case PChannel::PXWriteBlock:
        FD_SET (handle, write_fds);
        break;
      case PChannel::PXConnectBlock:
        FD_SET (handle, write_fds);
        FD_SET (handle, exception_fds);
        break;
      default:
        PAssertAlways(PLogicError);
        return 0;
    }

    // include the termination pipe into all blocking I/O functions
    int width = handle+1;
    FD_SET(unblockPipe[0], read_fds);
    width = PMAX(width, unblockPipe[0]+1);
  
    retval = ::select(width, read_fds, write_fds, exception_fds, tptr);

    if ((retval >= 0) || (errno != EINTR))
      break;
  }

  if ((retval == 1) && FD_ISSET(unblockPipe[0], read_fds)) {
    BYTE ch;
    ::read(unblockPipe[0], &ch, 1);
    errno = EINTR;
    retval =  -1;
    //PTRACE(1,"Unblocked I/O");
  }

  return retval;
}

void PThread::PXAbortBlock() const
{
  BYTE ch;
  ::write(unblockPipe[1], &ch, 1);
}


// For Mac OS, the housekeeping thread has two jobs:
// First, poll for synchronous signals (as is traditional), and
// second, to poll the MPThread termination notification queue and clean up
// deceased PThreads.  
// There is an ickiness here which depends on a current restriction of 
// Mac OS X:  synchronous signals (i.e. not signals resulting from
// exceptions) are only delivered to the main thread.  I assume that
// it is therefore safe for the main thread to call MPNotifyQueue from
// a signal handler if and only if the main thread never calls MPNotifyQueue
// on the termination notification queue from its main code.  This ought to
// be acceptable if notifying a queue is single-threaded per queue; if
// MPNotifyQueue has a global critical section, this will work very badly.

static MPQueueID terminationNotificationQueue = 0;
// This bites.  Threads don't know what process they come from (even though
// there can be only one), yet when we're winding down the process thread
// kills the housekeeper before it can clean up all the other threads.
// So the process thread has to poll the termination queue, but it does so
// from PThread context, so it can't know there's no housekeeper.  yuck.
static BOOL noHousekeeper = 0;

static void SetUpTermQueue() 
{
    OSStatus err;
    // Let us PLEASE try not to get into any "create/delete" loops here.
    // Create it and be DONE with it.
    while (!terminationNotificationQueue) {
        MPQueueID tempQueue;
        err = MPCreateQueue(&tempQueue);
        PAssert(err == noErr, "MPCreateQueue failed");
        // When Motorola finally finishes the 620, there's a lot of Mac code
        // gonna need to be rewritten.
        // HAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHA!!!
        // "finishes the 620".  hee hee hee!
        if (!OTCompareAndSwap32(0, (UInt32)tempQueue,
                                (UInt32*)&terminationNotificationQueue)) {
            // then someone else snuck in and initialized it.
            MPDeleteQueue(tempQueue);
        } else {
#ifdef DEBUG_THREADS
            if (debug_mpthreads)
                fprintf(stderr,"set up notification queue\n");
#endif
            // XXX MPNotifyQueue is perfectly willing to allocate memory
            // XXX for the items dropped in the queue.  However, if it can't,
            // XXX then life just goes on -- and we miss a thread exit message.
            // XXX If we reserve queue space, however, then we guarantee two
            // XXX things:  1, we absolutely will be able to receive N
            // XXX notifications, and 2, we absolutely will drop the N+1st
            // XXX on the floor, spare memory or no.  The target applications
            // XXX for this library do not appear (currently) to generate
            // XXX absurd numbers of threads, so I'll reserve an absurd number
            // XXX of messages, and pretend that nothing can go wrong.
            // XXX n go wrong.n go wrong.n go wrong.n go wrong.n go wrong.
            // XXX If the following fails, it's probably for lack of memory,
            // XXX in which case the queue will just try dynamic allocation.
            (void)MPSetQueueReserve(terminationNotificationQueue, 128);
        }
    }
}

static BOOL PollNotificationQueue(Duration timeout)
{
    OSStatus err = noErr;
    void *parm1, *parm2, *parm3;
    
    err = MPWaitOnQueue(terminationNotificationQueue,
                        &parm1, &parm2, &parm3,
                        timeout);
    if (err == noErr) {
        // then we got a notification
        if ((int)parm1 == 1) {
            // then it was a thread death notification, parm2 is
            // the PThread pointer
#ifdef DEBUG_THREADS
            if (debug_mpthreads)
                fprintf(stderr,"notified of %p death\n", parm2);
#endif
            PThread::PX_ThreadEnd(parm2);
        } // else parm1 == 0 and it's just a wakeup notice
    }
    return err == noErr;
}

void PHouseKeepingThread::Main()
{
    PProcess & process = PProcess::Current();

    SetUpTermQueue();

    while (!closing) {
        PTimeInterval waitTime = process.timers.Process();

        Duration timeout;
        if (waitTime == PMaxTimeInterval)
            timeout = kDurationForever;
        else {
            // "Values of type Duration are 32 bits long.  They are intepreted
            //  in a manner consistend with the Time Manager -- positive values
            //  are in units of milliseconds, negative values are in units of
            //  microseconds."  
            // Fortunately, PMaxTimeInterval is limited to a positive 32-bit
            // number of milliseconds.
            timeout = (long)waitTime.GetMilliSeconds();
        }

        // Block on the notification queue

        (void)PollNotificationQueue(timeout);
        
        // whether we timed out or got notified, check the signals.
        process.PXCheckSignals();
    }
    noHousekeeper = 1;
#ifdef DEBUG_THREADS
    if (debug_mpthreads)
        fprintf(stderr,"housekeeper exiting\n");
#endif
}


void PProcess::Construct()
{
  // set the file descriptor limit to something sensible
  struct rlimit rl;
  PAssertOS(getrlimit(RLIMIT_NOFILE, &rl) == 0);
  rl.rlim_cur = rl.rlim_max;
  PAssertOS(setrlimit(RLIMIT_NOFILE, &rl) == 0);

  SetUpTermQueue();

  // initialise the housekeeping thread
  housekeepingThread = NULL;

  CommonConstruct();
}


PProcess::~PProcess()
{
  // Don't wait for housekeeper to stop if Terminate() is called from it.
  if (housekeepingThread != NULL && PThread::Current() != housekeepingThread) {
    housekeepingThread->SetClosing();
    SignalTimerChange();
    housekeepingThread->WaitForTermination();
    delete housekeepingThread;
    housekeepingThread = 0;
  }
  // XXX try to gracefully handle shutdown transient where the housekeeping
  // XXX thread hasn't managed to clean up all the threads
  while (PollNotificationQueue(kDurationImmediate)) ;
  
  CommonDestruct();
}


PThread::PThread()
{
  // see InitialiseProcessThread()
}


void PThread::InitialiseProcessThread()
{
  OSStatus err        = 0;
  PX_origStackSize    = 0;
  autoDelete          = FALSE;
  PX_threadId         = MPCurrentTaskID();
  PX_suspendCount     = 0;

  ::pipe(unblockPipe);

  // Sadly, Mac OS MPThreads can't just initialize a block of memory into
  // an MPSemaphore (XXX ought to be a CriticalRegion, but they're broken
  // in Mac OS X 10.0.x!)
  PX_suspendMutex = 0;
  if ((err = MPCreateSemaphore(1,1,&PX_suspendMutex))
      != 0) {
      PAssertOS(err == 0);
      throw std::bad_alloc();
  }

  ((PProcess *)this)->activeThreads.DisallowDeleteObjects();
  ((PProcess *)this)->activeThreads.SetAt((unsigned)PX_threadId, this);
}


PThread::PThread(PINDEX stackSize,
                 AutoDeleteFlag deletion,
                 Priority /*priorityLevel*/,
                 const PString & name)
        : threadName(name), PX_signature(kMPThreadSig)
{
  OSStatus err = 0;

  PAssert(stackSize > 0, PInvalidParameter);

  PX_origStackSize = stackSize;
  autoDelete       = (deletion == AutoDeleteThread);

  // Sadly, Mac OS MPThreads can't just initialize a block of memory into
  // an MPSemaphore (XXX ought to be a CriticalRegion, but they're broken
  // in Mac OS X 10.0.x!)
  PX_suspendMutex = 0;
  if ((err = MPCreateSemaphore(1,1,&PX_suspendMutex)) != 0) {
      PAssert(err == 0, "MPCreateSemaphore failed");
      throw std::bad_alloc();
  }

  ::pipe(unblockPipe);

  // throw the new thread
  PX_NewThread(TRUE);
}


PThread::~PThread()
{
  if (!IsTerminated()) 
    Terminate();

  ::close(unblockPipe[0]);
  ::close(unblockPipe[1]);

  if (PX_suspendMutex)
      MPDeleteSemaphore(PX_suspendMutex);
#ifdef DEBUG_THREADS
  if (debug_mpthreads)
      fprintf(stderr,"thread %p destructing\n", this);
#endif
  PX_signature = kMPDeadSig;
}

void PThread::PX_NewThread(BOOL startSuspended)
{
  OSErr err;
  // initialise suspend counter and create mutex
  PX_suspendCount = startSuspended ? 1 : 0;

  // initialise Suspend/Resume semaphore (for Mac OS X)
  // XXX The MPThread manager allows for starting tasks "suspended", but I
  // XXX suspect that only works if you have a debugger registered.
  suspend_semaphore = new PSemaphore(0,1);

  // throw the thread

  SetUpTermQueue();
  
  // create the task.
#ifdef DEBUG_THREADS
  if (debug_mpthreads)
      fprintf(stderr,"thread %p being started\n", (void *)this);
#endif
  err = MPCreateTask( (TaskProc)PX_ThreadStart, (void*)this,
                      65536, // stacksize
                      terminationNotificationQueue,
                      (void *)1,    // param 1 == "death"
                      (void *)this, // param 2 == "PThread to clean up"
                      0, // no options
                      &PX_threadId);
  PAssert(err == 0, "MPCreateTask failed");
  if (err) throw std::bad_alloc();

}

long PThread::PX_ThreadStart(void * arg)
{ 
  MPTaskID threadId = MPCurrentTaskID();

  // self-detach (no need)

  PThread * thread = (PThread *)arg;
  thread->SetThreadName(thread->GetThreadName());

  PProcess & process = PProcess::Current();

  // add thread to thread list
  process.threadMutex.Wait();
  process.activeThreads.SetAt((unsigned)threadId, thread);
  process.threadMutex.Signal();

  // if we are not supposed to start suspended, then don't wait
  // if we are supposed to start suspended, then wait for a resume

  if (thread->PX_suspendCount != 0) {
    thread->suspend_semaphore->Wait();	// Wait for the Resume
  }

  // now call the the thread main routine
  //PTRACE(1, "tlibthrd\tAbout to call Main");
  thread->Main();

#ifdef DEBUG_THREADS
  if (debug_mpthreads)
      fprintf(stderr,"thread %p returning\n", thread);
#endif
  return 0;
}


void PProcess::SignalTimerChange()
{
  if (housekeepingThread == NULL) {
#if PMEMORY_CHECK
    BOOL oldIgnoreAllocations = PMemoryHeap::SetIgnoreAllocations(TRUE);
#endif
    housekeepingThread = new PHouseKeepingThread;
#if PMEMORY_CHECK
    PMemoryHeap::SetIgnoreAllocations(oldIgnoreAllocations);
#endif
  }

  SetUpTermQueue();
  MPNotifyQueue(terminationNotificationQueue, 0, 0, 0);
}


void PThread::PX_ThreadEnd(void * arg)
{
  PThread * thread = (PThread *)arg;
  PProcess & process = PProcess::Current();
  
  MPTaskID id = thread->PX_GetThreadId();
  if (id != 0) {

    // remove this thread from the active thread list
    process.threadMutex.Wait();
    process.activeThreads.SetAt((unsigned)id, NULL);
    process.threadMutex.Signal();
  }

  // delete the thread if required, note this is done this way to avoid
  // a race condition, the thread ID cannot be zeroed before the if!
  if (thread->autoDelete) {
    thread->PX_threadId = 0;  // Prevent terminating terminated thread
    delete thread;
  }
  else
    thread->PX_threadId = 0;
}


MPTaskID PThread::PX_GetThreadId() const
{
  return PX_threadId;
}


void PThread::Restart()
{
  if (IsTerminated())
    return;

  PX_NewThread(FALSE);
}


void PThread::Terminate()
{
  if (PX_origStackSize <= 0)
    return;

  if (IsTerminated())
    return;

  PTRACE(1, "tlibthrd\tForcing termination of thread " << (void *)this);

  if (Current() == this)
      MPExit(0);
  else {
      MPTaskID taskId = PX_threadId;
      WaitForTermination();
      // XXX Dire Consequences[TM] are warned of when one uses MPTerminateTask.
      // XXX However, the same Dire Consequences are predicted (I think) for
      // XXX pthread_kill which the PWLIB code already uses.
      // XXX However, the only thing the cleanup function does is removes the
      // XXX thread from the thread table, which is already performed by the
      // XXX housekeeping thread; PWLIB doesn't try to salvage locks or
      // XXX anything clever like that.
      // XXX I just hope taskIds aren't quickly reused.
      if (taskId != 0)
          (void)MPTerminateTask(taskId, kMPTaskAbortedErr);
  }
}


void PThread::PXSetWaitingSemaphore(PSemaphore * sem)
{
    // not needed
}


BOOL PThread::IsTerminated() const
{
  if (PX_threadId == 0) {
    //PTRACE(1, "tlibthrd\tIsTerminated(" << (void *)this << ") = 0");
    return TRUE;
  }

#ifdef _not_def_ // Sigh.  no MPGetNextTaskID on MOSX
  // This seems like a silly way to do this, but I think it might work.
  // The end condition for MPGetNextTaskID isn't documented, so I try both
  // logical possibilities.
  MPTaskID sometask = 0;
  MPProcessID myproc = 0;
  while (MPGetNextTaskID(myproc, &sometask) == noErr) {
      if (sometask == 0) break;
      if (sometask == PX_threadId) {
          //PTRACE(1, "tlibthrd\tIsTerminated(" << (void *)this << ") not dead yet");
          return FALSE;
      }
  }
  // didn't find it, it's dead
  //PTRACE(1, "tlibthrd\tIsTerminated(" << (void *)this << ") = 0");
  return TRUE;
#else
  return FALSE; // ENOCLUE
#endif
}

// Mac OS X and Darwin 1.2 does not support pthread_kill() or sigwait()
// so we cannot implement suspend and resume using signals. Instead we have a
// partial implementation using a Semaphore.
// As a result, we can create a thread in a suspended state and then 'resume'
// it, but once it is going, we can no longer suspend it.
// So, for Mac OS X, we will accept Resume() calls (or Suspend(FALSE))
// but reject Suspend(TRUE) calls with an Assertion. This will indicate
// to a user that we cannot Suspend threads on Mac OS X

void PThread::Suspend(BOOL susp)
{
  OSStatus err;
  err = MPWaitOnSemaphore(PX_suspendMutex,kDurationForever);
  PAssert(err == 0, "MPWaitOnSemaphore failed");

  if (susp) {
    // Suspend - warn the user with an Assertion
    PAssertAlways("Cannot suspend threads on Mac OS X due to lack of pthread_kill()");
  }

  // if resuming, then see if to really resume
  else if (PX_suspendCount > 0) {
    PX_suspendCount--;
    if (PX_suspendCount == 0)  {
      suspend_semaphore->Signal();
    }
  }

  err = MPSignalSemaphore(PX_suspendMutex);
  PAssert( err == 0, "MPSignalSemaphore failed");
}

void PThread::Resume()
{
  Suspend(FALSE);
}


BOOL PThread::IsSuspended() const
{
  OSStatus err;

  if (IsTerminated())
    return FALSE;

  err = MPWaitOnSemaphore(PX_suspendMutex, kDurationForever);
  PAssert(err == 0, "MPWaitOnSemaphore failed");
  BOOL suspended = PX_suspendCount > 0;
  err = MPSignalSemaphore(PX_suspendMutex);
  PAssert(err == 0, "MPSignalSemaphore failed");
  return suspended;
}


void PThread::SetAutoDelete(AutoDeleteFlag deletion)
{
  PAssert(deletion != AutoDeleteThread || this != &PProcess::Current(), PLogicError);
  autoDelete = deletion == AutoDeleteThread;
}


void PThread::SetPriority(Priority /*priorityLevel*/)
{
}


PThread::Priority PThread::GetPriority() const
{
  return LowestPriority;
}


void PThread::Yield()
{
  ::sleep(0);
}


PThread * PThread::Current()
{
  PProcess & process = PProcess::Current();
  process.threadMutex.Wait();
  PThread * thread = process.activeThreads.GetAt((unsigned)MPCurrentTaskID());
  process.threadMutex.Signal();
  return PAssertNULL(thread);
}


void PThread::Sleep(const PTimeInterval & timeout)
{
    AbsoluteTime expiry;
    Duration delta = kDurationForever;
    
    if (timeout != PMaxTimeInterval) {
        delta = timeout.GetMilliSeconds();
    }
    expiry = AddDurationToAbsolute(delta, UpTime());
    
    (void)MPDelayUntil(&expiry);
}


void PThread::WaitForTermination() const
{
  PAssert(Current() != this, "Waiting for self termination!");
  
  PXAbortBlock();

  while (!IsTerminated()) {
    PAssert(PX_signature == kMPThreadSig, "bad signature in living thread");
    Current()->Sleep(10);
#ifdef DEBUG_THREADS
    if (debug_mpthreads)
        fprintf(stderr,"spinning for termination of thread %p\n", (void *)this);
#endif  
    if (noHousekeeper) PollNotificationQueue(kDurationImmediate);
  }
}


BOOL PThread::WaitForTermination(const PTimeInterval & maxWait) const
{
  PAssert(Current() != this, "Waiting for self termination!");
  
  //PTRACE(1, "tlibthrd\tWaitForTermination(delay)");
  PXAbortBlock();

  PTimer timeout = maxWait;
  while (!IsTerminated()) {
    if (timeout == 0)
      return FALSE;
    Current()->Sleep(10);
  }
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////

PSemaphore::PSemaphore(unsigned initial, unsigned maxCount)
{
    OSStatus err = MPCreateSemaphore(maxCount, initial, &semId);
    PAssert(err == 0, "MPCreateSemaphore failed");
    PAssert((long)semId != 0 && (long)semId != -1, "stupid semId");
}


PSemaphore::~PSemaphore()
{
    OSStatus err = MPDeleteSemaphore(semId);
    PAssert(err == 0, "MPDeleteSemaphore failed");
    *(long *)&semId = -1;
}


void PSemaphore::Wait()
{
    assert((long)semId != 0);
    assert((long)semId != -1);
    
    PAssert((long)semId != -1, "wait on destructed PSemaphore");
    PAssert((long)semId != 0, "semId stomped");
    OSStatus err = MPWaitOnSemaphore(semId, kDurationForever);
    PAssert(err == 0, "MPWaitOnSemaphore failed");
}


BOOL PSemaphore::Wait(const PTimeInterval & waitTime)
{
  OSErr err = 0;
    
  if (waitTime == PMaxTimeInterval) {
    Wait();
    return TRUE;
  }

  Duration timeout = waitTime.GetMilliSeconds();
  if ((err = MPWaitOnSemaphore(semId, timeout)) == noErr)
      return TRUE;
  if (err == kMPTimeoutErr)
      return FALSE;
  PAssert(err == 0, psprintf("timed wait error = %i", err));
  return FALSE;
}

void PSemaphore::Signal()
{
    OSStatus err = MPSignalSemaphore(semId);
    // was it already signalled?
    if (err == kMPInsufficientResourcesErr) err = 0;
    PAssert(err == 0, "MPSignalSemaphore failed");
}


BOOL PSemaphore::WillBlock() const
{
    OSStatus err = MPWaitOnSemaphore(semId, kDurationImmediate);
    if (err == kMPTimeoutErr)
        return TRUE;
    PAssert(err == 0, psprintf("timed wait error = %i", err));
    (void)MPSignalSemaphore(semId);
    return FALSE;
}

// Ideally, a PMutex would contain an MPCriticalSection instead of a
// semaphore, but the class derivation is outside the machine-specific
// code, and I'm unwilling to do something gross like implement a bogus
// constructor for PSemaphore which doesn't allocate a semaphore.

PMutex::PMutex()
  : PSemaphore(1, 1)
{
}

void PMutex::Wait()
{
	PSemaphore::Wait();
}

BOOL PMutex::Wait(const PTimeInterval & timeout)
{
	return PSemaphore::Wait(timeout);
}

void PMutex::Signal()
{
	PSemaphore::Signal();
}

BOOL PMutex::WillBlock() const 
{
	return PSemaphore::WillBlock();
}

PSyncPoint::PSyncPoint()
  : PSemaphore(0, 1)
{
}
