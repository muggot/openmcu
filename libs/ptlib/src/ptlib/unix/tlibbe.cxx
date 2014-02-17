/* 
 * tlibbe.cxx
 *
 * Thread library implementation for BeOS
 *
 * Portable Windows Library
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
 * Portions are Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): Yuri Kiryanov, ykiryanov at users.sourceforge.net
 *
 * $Log: tlibbe.cxx,v $
 * Revision 1.34  2007/05/01 10:20:44  csoutheren
 * Applied 1703617 - Prevention of application deadlock caused by too many timers
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.33  2004/10/26 18:29:41  ykiryanov
 * Added ostream::write and istream::read with 2nd param as streamsize to please
 * New toolchain
 *
 * Revision 1.32  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.31  2004/05/30 04:48:15  ykiryanov
 * Sync point is better when semaphore based
 *
 * Revision 1.30  2004/05/24 04:17:16  ykiryanov
 * Made PSyncPoint::Wait to return FALSE if called with timeout 0
 *
 * Revision 1.29  2004/05/23 22:20:37  ykiryanov
 * Got rid of 2 housekeeper thread problem
 *
 * Revision 1.28  2004/05/21 00:49:16  csoutheren
 * Added PreShutdown to ~PProcess
 *
 * Revision 1.27  2004/05/02 18:00:54  ykiryanov
 * Renamed unblock pipe to make code compatible with socket code
 *
 * Revision 1.26  2004/05/02 16:59:09  ykiryanov
 * Fixed assert in setting priority to threads
 *
 * Revision 1.25  2004/04/30 16:10:20  ykiryanov
 * Added PMutex code based on BLocker to support recursive locks
 *
 * Revision 1.24  2004/04/25 21:51:37  ykiryanov
 * Cleaned up thread termination act. Very cool
 *
 * Revision 1.23  2004/04/25 04:32:37  ykiryanov
 * Fixed very old bug - no get thread id code in InitialiseProcessThread
 *
 * Revision 1.22  2004/04/18 00:23:40  ykiryanov
 * Rearranged code to be more reliable. We nearly there
 *
 * Revision 1.21  2004/04/02 03:17:19  ykiryanov
 * New version, improved
 *
 * Revision 1.20  2004/02/23 23:40:42  ykiryanov
 * Added missing constructor for PMutex
 *
 * Revision 1.19  2004/02/23 21:23:09  ykiryanov
 * Removed assert line to enable semaphore constructor
 *
 * Revision 1.18  2004/02/23 20:37:17  ykiryanov
 * Changed function definition PXBlockIO to prototype one
 *
 * Revision 1.17  2004/02/23 18:10:39  ykiryanov
 * Added a parameter to semaphore constructor to avoid ambiguity
 *
 * Revision 1.16  2004/02/23 00:02:20  ykiryanov
 * Changed my e-mail to ykiryanov at users.sourceforge.net. Just in case someone wants to collaborate
 *
 * Revision 1.15  2004/02/22 23:59:28  ykiryanov
 * Added missing functions: PProcess::SetMaxHandles(), PThread::GetCurrentThreadId(), 
 * PThread::PXAbortBlock(), PSyncPoint::Signal(), ::Wait(), ::Wait(timeout), ::WillBlock()
 *
 * Revision 1.14  2004/02/22 04:35:04  ykiryanov
 * Removed PMutex desctructor
 *
 * Revision 1.13  2003/02/26 01:13:18  robertj
 * Fixed race condition where thread can terminatebefore an IsSuspeded() call
 *   occurs and cause an assert, thanks Sebastian Meyer
 *
 * Revision 1.12  2001/06/30 06:59:07  yurik
 * Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
 *
 * Revision 1.11  2001/03/07 06:57:32  yurik
 * Changed email to current one
 *
 * Revision 1.10  2001/01/16 12:32:06  rogerh
 * Remove duplicate SetAutoDelete() function. Submitted by
 * Jac Goudsmit <jac_goudsmit@yahoo.com>
 *
 *
 */

class PThread;
class PProcess;
class PSemaphore;
class PSyncPoint;

class PMutex; 

#include <ptlib.h>
#include <ptlib/socket.h>

#ifdef B_ZETA_VERSION 
#include <posix/rlimit.h>
#endif // Zeta

// For class BLocker
#include <be/support/Locker.h>

int PX_NewHandle(const char *, int);

#define DEBUG_SEMAPHORES1 1

//////////////////////////////////////////////////////////////////////////////
// Threads

static int const priorities[] = {
  1, // Lowest priority is 1. 0 is not
  B_LOW_PRIORITY,
  B_NORMAL_PRIORITY,
  B_DISPLAY_PRIORITY,
  B_URGENT_DISPLAY_PRIORITY,
};

int32 PThread::ThreadFunction(void * threadPtr)
{
  PThread * thread = (PThread *)PAssertNULL(threadPtr);

  PProcess & process = PProcess::Current();

  process.threadMutex.Wait();
  process.activeThreads.SetAt((unsigned) thread->mId, thread);
  process.threadMutex.Signal();

  thread->Main();

  return 0;
}

PThread::PThread()
 : autoDelete(TRUE),
   mId(B_BAD_THREAD_ID),
   mPriority(B_NORMAL_PRIORITY),
   mStackSize(0),
   mSuspendCount(0)
{
}

void PThread::InitialiseProcessThread()
{
  autoDelete = FALSE;

  mId = find_thread(NULL);
  mPriority = B_NORMAL_PRIORITY;
  mStackSize = 0;
  mSuspendCount = 1;
  
  PAssert(::pipe(unblockPipe) == 0, "Pipe creation failed in InitialiseProcessThread!");
  PAssertOS(unblockPipe[0]);
  PAssertOS(unblockPipe[1]);
  
  ((PProcess *)this)->activeThreads.DisallowDeleteObjects();
  ((PProcess *)this)->activeThreads.SetAt(mId, this);
}

PThread::PThread(PINDEX stackSize,
                 AutoDeleteFlag deletion,
                 Priority priorityLevel,
                 const PString & name)
 : mId(B_BAD_THREAD_ID),
   mPriority(B_NORMAL_PRIORITY),
   mStackSize(0),
   mSuspendCount(0)
{
  PAssert(stackSize > 0, PInvalidParameter);
  autoDelete = deletion == AutoDeleteThread;
 
  mId =  ::spawn_thread(ThreadFunction, // Function 
         (const char*) name, // Name
         priorities[priorityLevel], // Priority 
         (void *) this); // Pass this as cookie

  PAssertOS(mId >= B_NO_ERROR);
    
  mSuspendCount = 1;
  mStackSize = stackSize;
  mPriority = priorities[priorityLevel];

  threadName.sprintf(name, mId);
  ::rename_thread(mId, (const char*) threadName); // real, unique name - with id

  PAssert(::pipe(unblockPipe) == 0, "Pipe creation failed in PThread constructor");
  PX_NewHandle("Thread unblock pipe", PMAX(unblockPipe[0], unblockPipe[1]));
}

PThread * PThread::Current()
{
  PProcess & process = PProcess::Current();
  process.threadMutex.Wait();
  PThread * thread = process.activeThreads.GetAt((unsigned)find_thread(NULL));
  process.threadMutex.Signal();
  return thread;    
}

PThread::~PThread()
{
  // if we are not process, remove this thread from the active thread list
  PProcess & process = PProcess::Current();
  if(process.GetThreadId() != GetThreadId())
  {
    process.threadMutex.Wait();
    process.activeThreads.RemoveAt((unsigned) mId);
    process.threadMutex.Signal();
  }

  if (!IsTerminated())
    Terminate();

  ::close(unblockPipe[0]);
  ::close(unblockPipe[1]);
}


void PThread::Restart()
{
  if(!IsTerminated())
    return;

  mId =  ::spawn_thread(ThreadFunction, // Function 
         "PWLT", // Name
          mPriority, 
          (void *) this); // Pass this as cookie

  PAssertOS(mId >= B_NO_ERROR);

  threadName.sprintf("PWLib Thread %d", mId);
  ::rename_thread(mId, (const char*) threadName); // real, unique name - with id
}

void PThread::Terminate()
{
  if(mStackSize <=0)
    return;

  if(mId == find_thread(NULL))
  {
    ::exit_thread(0);
    return;
  }

  if(IsTerminated())
    return;

  PXAbortBlock();
  WaitForTermination(20);

 if(mId > B_BAD_THREAD_ID)
   ::kill_thread(0);
}

BOOL PThread::IsTerminated() const
{
  return mId == B_BAD_THREAD_ID;
}


void PThread::WaitForTermination() const
{
  WaitForTermination(PMaxTimeInterval);
}


BOOL PThread::WaitForTermination(const PTimeInterval & /*maxWait*/) const // Fix timeout
{
  status_t result = B_NO_ERROR;
  status_t exit_value = B_NO_ERROR;

  result = ::wait_for_thread(mId, &exit_value);
  if ( result == B_INTERRUPTED ) { // thread was killed.
    return TRUE;
  }

  if ( result == B_OK ) { // thread is dead
    #ifdef DEBUG_THREADS
    PError << "B_OK" << endl;
    #endif
    return TRUE;
  }

  if ( result == B_BAD_THREAD_ID ) { // thread has invalid id
    return TRUE;
  }

  return FALSE;
}


void PThread::Suspend(BOOL susp)
{

  PAssert(!IsTerminated(), "Operation on terminated thread");
  if (susp)
  {
    status_t result = ::suspend_thread(mId);
    if(B_OK == result)
	::atomic_add(&mSuspendCount, 1);

    PAssert(result == B_OK, "Thread don't want to be suspended");
  }
  else
    Resume();
}


void PThread::Resume()
{
  PAssert(!IsTerminated(), "Operation on terminated thread");
  status_t result = ::resume_thread(mId);
  if(B_OK == result)
    ::atomic_add(&mSuspendCount, -1);

  PAssert(result == B_NO_ERROR, "Thread doesn't want to resume");
}


BOOL PThread::IsSuspended() const
{
  return (mSuspendCount > 0);
}

void PThread::SetAutoDelete(AutoDeleteFlag deletion)
{
  PAssert(deletion != AutoDeleteThread || this != &PProcess::Current(), PLogicError);
  autoDelete = deletion == AutoDeleteThread;
}

void PThread::SetPriority(Priority priorityLevel)
{
  PAssert(!IsTerminated(), "Operation on terminated thread");

  mPriority = priorities[priorityLevel];
  status_t result = ::set_thread_priority(mId, mPriority );
  if(result != B_OK)
    PTRACE(0, "Changing thread priority failed, error " << strerror(result) << endl);

}


PThread::Priority PThread::GetPriority() const
{
  if(!IsTerminated())
  {

  switch (mPriority) {
    case 0 :
      return LowestPriority;
    case B_LOW_PRIORITY :
      return LowPriority;
    case B_NORMAL_PRIORITY :
      return NormalPriority;
    case B_DISPLAY_PRIORITY :
      return HighPriority;
    case B_URGENT_DISPLAY_PRIORITY :
      return HighestPriority;
  }
  PAssertAlways(POperatingSystemError);
  
  }
  return LowestPriority;
}

void PThread::Yield()
{
  // we just sleep for long enough to cause a reschedule (100 microsec)
  ::snooze(100);
}

void PThread::Sleep( const PTimeInterval & delay ) // Time interval to sleep for.
{
  bigtime_t microseconds = 
		delay == PMaxTimeInterval ? B_INFINITE_TIMEOUT : (delay.GetMilliSeconds() * 1000 );
 
  status_t result = ::snooze( microseconds ) ; // delay in ms, snooze in microsec
  PAssert(result == B_OK, "Thread has insomnia");
}

int PThread::PXBlockOnChildTerminate(int pid, const PTimeInterval & /*timeout*/) // Fix timeout
{
  status_t result = B_NO_ERROR;
  status_t exit_value = B_NO_ERROR;

  result = ::wait_for_thread(pid, &exit_value);
  if ( result == B_INTERRUPTED ) 
  { 
    // thread was killed.
    #ifdef DEBUG_THREADS
    PError << "B_INTERRUPTED" << endl;
    #endif
    return 1;
  }

  if ( result == B_OK ) 
  { 
    // thread is dead
     return 1;
  }

  if ( result == B_BAD_THREAD_ID ) 
  { 
    // thread has invalid id
    return 1;
  }

  return 0; // ???
}

PThreadIdentifier PThread::GetCurrentThreadId(void)
{
  return ::find_thread(NULL);
}

int PThread::PXBlockOnIO(int handle, int type, const PTimeInterval & timeout)
{
  PTRACE(7, "PWLib\tPThread::PXBlockOnIO(" << handle << ',' << type << ')');

  if ((handle < 0) || (handle >= PProcess::Current().GetMaxHandles())) {
    PTRACE(2, "PWLib\tAttempt to use illegal handle in PThread::PXBlockOnIO, handle=" << handle);
    errno = EBADF;
    return -1;
  }

  // make sure we flush the buffer before doing a write
  P_fd_set read_fds;
  P_fd_set write_fds;
  P_fd_set exception_fds;

  int retval;
  do {
    switch (type) {
      case PChannel::PXReadBlock:
      case PChannel::PXAcceptBlock:
        read_fds = handle;
        write_fds.Zero();
        exception_fds.Zero();
        break;
      case PChannel::PXWriteBlock:
        read_fds.Zero();
        write_fds = handle;
        exception_fds.Zero();
        break;
      case PChannel::PXConnectBlock:
        read_fds.Zero();
        write_fds = handle;
        exception_fds = handle;
        break;
      default:
        PAssertAlways(PLogicError);
        return 0;
    }

    // include the termination pipe into all blocking I/O functions
    read_fds += unblockPipe[0];

    P_timeval tval = timeout;
    retval = ::select(PMAX(handle, unblockPipe[0])+1,
                      read_fds, write_fds, exception_fds, tval);
  } while (retval < 0 && errno == EINTR);

  if ((retval == 1) && read_fds.IsPresent(unblockPipe[0])) {
    BYTE ch;
    ::read(unblockPipe[0], &ch, 1);
    errno = EINTR;
    retval =  -1;
    PTRACE(6, "PWLib\tUnblocked I/O");
  }

  return retval;
}

void PThread::PXAbortBlock(void) const
{
  BYTE ch;
  ::write(unblockPipe[1], &ch, 1);
}

///////////////////////////////////////////////////////////////////////////////
// PProcess
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

void PProcess::Construct()
{
  maxHandles = FOPEN_MAX;
  PTRACE(4, "PWLib\tMaximum per-process file handles is " << maxHandles);

  // initialise the housekeeping thread
  housekeepingThread = NULL;

  CommonConstruct();
}

void PHouseKeepingThread::Main()
{
  PProcess & process = PProcess::Current();

  while (!closing) {
    PTimeInterval delay = process.timers.Process();

    globalBreakBlock.Wait(delay);

    process.PXCheckSignals();
  }    
}

void PProcess::SignalTimerChange()
{
  if (housekeepingThread == NULL)
  {  
    housekeepingThread = new PHouseKeepingThread;
  }

  globalBreakBlock.Signal();
}

BOOL PProcess::SetMaxHandles(int newMax)
{
  return FALSE;
}

PProcess::~PProcess()
{
  PreShutdown();

  // Don't wait for housekeeper to stop if Terminate() is called from it.
  if (housekeepingThread != NULL && PThread::Current() != housekeepingThread) {
    housekeepingThread->SetClosing();
    SignalTimerChange();
    housekeepingThread->WaitForTermination();
    delete housekeepingThread;
  }

  CommonDestruct();
}

///////////////////////////////////////////////////////////////////////////////
// PSemaphore
PSemaphore::PSemaphore(BOOL fNested) : mfNested(fNested)
{
}

PSemaphore::PSemaphore(unsigned initial, unsigned)
{
  Create(initial);
}
 
void PSemaphore::Create(unsigned initial)
{
  mOwner = ::find_thread(NULL);
  PAssertOS(mOwner != B_BAD_THREAD_ID);
  if(!mfNested)
  {
    mCount = initial;
    semId = ::create_sem(initial, "PWLS"); 

    PAssertOS(semId >= B_NO_ERROR);

    #ifdef DEBUG_SEMAPHORES
    sem_info info;
    get_sem_info(semId, &info);
    PError << "::create_sem (PSemaphore()), id: " << semId << ", this: " << this << ", count:" << info.count << endl;
    #endif
  }
  else // Use BLocker
  {
    semId = (sem_id) new BLocker("PWLN", true); // PWLib use recursive locks. true for benaphore style, false for not
  }
}

PSemaphore::~PSemaphore()
{
  if(!mfNested)
  {
    status_t result = B_NO_ERROR;
    PAssertOS(semId >= B_NO_ERROR);
  
    // Transmit ownership of the semaphore to our thread
    thread_id curThread = ::find_thread(NULL);
    if(mOwner != curThread)
    {
     thread_info tinfo;
     ::get_thread_info(curThread, &tinfo);
     ::set_sem_owner(semId, tinfo.team);
      mOwner = curThread; 
    } 
 
    #ifdef DEBUG_SEMAPHORES
    sem_info info;
    get_sem_info(semId, &info);
    PError << "::delete_sem, id: " << semId << ", this: " << this << ", name: " << info.name << ", count:" << info.count;
    #endif 

    // Deleting the semaphore id
    result = ::delete_sem(semId);

    #ifdef DEBUG_SEMAPHORES
    if( result != B_NO_ERROR )
      PError << "...delete_sem failed, error: " << strerror(result) << endl;
    #endif
  }
  else // Use BLocker
  {
    delete (BLocker*) semId; // Thanks!
  }
}

void PSemaphore::Wait()
{
  if(!mfNested)
  {
    PAssertOS(semId >= B_NO_ERROR);
 
    status_t result = B_NO_ERROR;

    #ifdef DEBUG_SEMAPHORES
    sem_info info;
    get_sem_info(semId, &info);
    PError << "::acquire_sem, id: " << semId << ", name: " << info.name << ", count:" << info.count << endl;
    #endif 

    while((B_BAD_THREAD_ID != mOwner) 
      && ((result = ::acquire_sem(semId)) == B_INTERRUPTED))
    {
    }
  }
  else
  {
    ((BLocker*)semId)->Lock(); // Using class to support recursive locks 
  }
}

BOOL PSemaphore::Wait(const PTimeInterval & timeout)
{
  PInt64 ms = timeout.GetMilliSeconds();
  bigtime_t microseconds = ms * 1000;

  status_t result = B_NO_ERROR;
   
  if(!mfNested)
  {
    PAssertOS(semId >= B_NO_ERROR);
    PAssertOS(timeout < PMaxTimeInterval);

    #ifdef DEBUG_SEMAPHORES
    sem_info info;
    get_sem_info(semId, &info);
    PError << "::acquire_sem_etc " << semId << ",this: " << this << ", name: " << info.name << ", count:" << info.count 
      << ", ms: " << microseconds << endl;
    #endif
 
    while((B_BAD_THREAD_ID != mOwner) 
      && ((result = ::acquire_sem_etc(semId, 1, 
        B_RELATIVE_TIMEOUT, microseconds)) == B_INTERRUPTED))
    {
    }
  }
  else
  {
    result = ((BLocker*)semId)->LockWithTimeout(microseconds); // Using BLocker class to support recursive locks 
  }

  return ms == 0 ? FALSE : result == B_OK;
}

void PSemaphore::Signal()
{
  if(!mfNested)
  {
    PAssertOS(semId >= B_NO_ERROR);
 
    #ifdef DEBUG_SEMAPHORES
    sem_info info;
    get_sem_info(semId, &info);
    PError << "::release_sem " << semId << ", this: " << this << ", name: " << info.name << ", count:" << info.count << endl;
    #endif 
      ::release_sem(semId);
   }
   else
   {
     ((BLocker*)semId)->Unlock(); // Using BLocker class to support recursive locks 
   }		
}

BOOL PSemaphore::WillBlock() const
{
  if(!mfNested)
  {
    PAssertOS(semId >= B_NO_ERROR);

    #ifdef DEBUG_SEMAPHORES
    sem_info info;
    get_sem_info(semId, &info);
    PError << "::acquire_sem_etc (WillBlock) " << semId << ", this: " << this << ", name: " << info.name << ", count:" << info.count << endl;
    #endif
	
    status_t result = ::acquire_sem_etc(semId, 0, B_RELATIVE_TIMEOUT, 0);
    return result == B_WOULD_BLOCK;
  }
  else
  {
    return mOwner == find_thread(NULL); // If we are in our own thread, we won't lock
  }
}

///////////////////////////////////////////////////////////////////////////////
// PSyncPoint

PSyncPoint::PSyncPoint()
 : PSemaphore(FALSE) // FALSE is semaphore based, TRUE means implemented through BLocker
{
   PSemaphore::Create(0);
}

void PSyncPoint::Signal()
{
  PSemaphore::Signal();
}
                                                                                                      
void PSyncPoint::Wait()
{
  PSemaphore::Wait();
}
                                                                                                      
BOOL PSyncPoint::Wait(const PTimeInterval & timeout)
{
  return PSemaphore::Wait(timeout);
}
                                                                                                      
BOOL PSyncPoint::WillBlock() const
{
  return PSemaphore::WillBlock();
}

//////////////////////////////////////////////////////////////////////////////
// PMutex, derived from BLightNestedLocker  

PMutex::PMutex() 
  : PSemaphore(TRUE) // TRUE means implemented through BLocker
{
  PSemaphore::Create(0);
}

PMutex::PMutex(const PMutex&) 
 : PSemaphore(TRUE)
{
  PAssertAlways("PMutex copy constructor not supported");
} 

void PMutex::Signal()
{
  PSemaphore::Signal();
}
                                                                                                      
void PMutex::Wait()
{
  PSemaphore::Wait();
}
                                                                                                      
BOOL PMutex::Wait(const PTimeInterval & timeout)
{
  return PSemaphore::Wait(timeout);
}
                                                                                                      
BOOL PMutex::WillBlock() const
{
  return PSemaphore::WillBlock();
}

//////////////////////////////////////////////////////////////////////////////
// Extra functionality not found in BeOS

int seteuid(uid_t uid) { return 0; }
int setegid(gid_t gid) { return 0; }

///////////////////////////////////////////////////////////////////////////////
// Toolchain dependent stuff
#if (__GNUC_MINOR__  > 9)
#warning "Using gcc 2.95.x"
    ostream& ostream::write(const char *s, streamsize n) { return write(s, (long) n); };
    istream& istream::read(char *s, streamsize n) { return read(s, (long) n); };
#endif // gcc minor  > 9  

// End Of File ///////////////////////////////////////////////////////////////
