/*
 * tlibvx.cxx
 *
 * Thread library implementation for VxWorks
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
 * Contributor(s):  ______________________________________.
 *
 * $Log: tlibvx.cxx,v $
 * Revision 1.4  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * 
 * Revision 1.3  2003/05/21 00:49:16  csoutheren
 *  Added PreShutdown to ~PProcess
 * 
 * Revision 1.2  2003/02/26 01:14:27  robertj
 *  Fixed race condition where thread can terminatebefore an IsSuspeded() call
 *  occurs and cause an assert, thanks Sebastian Meyer
 * 
 * Revision 1.1  2002/11/05 01:43:39  robertj
 *  Added missing VxWorks files. Thanks Andreas Sikkema
 *
 * Revision 1.0 ?????????????
*/


class PProcess;
class PSemaphore;

#include <ptlib.h>
#include <ptlib/socket.h>
#include <trclib.h>
#include <usrlib.h>
// Forward to undocumented system call
extern "C" void dbgPrintCall(INSTR * callAdrs, int funcAdrs, int nargs,
                             UINT32 * pArgs);

#define VX_LOWEST_PRIORITY          250
#define VX_LOW_PRIORITY             200
#define VX_NORMAL_PRIORITY          150
#define VX_DISPLAY_PRIORITY         100
#define VX_URGENT_DISPLAY_PRIORITY  50  

///////////////////////////////////////////////////////////////////////////////
// Critical Section Implementation
// -------------------------------
class CCriticalSection {
public:
  CCriticalSection();
  ~CCriticalSection();

  void Lock();
  void Unlock();

private:
  int    intLevel;
  STATUS taskLocked;
  bool   locked;
};

CCriticalSection::CCriticalSection()
{
  intLevel   = 0;
  taskLocked = ERROR;
  locked     = false;
}

CCriticalSection::~CCriticalSection()
{
  // Unlock anyway when someone forgot to call Unlock after Lock
  if (locked == true)
    Unlock();
}

void CCriticalSection::Lock()
{
  if (locked == false) {
    if (::intContext() == FALSE)
      taskLocked = ::taskLock();
    else
      taskLocked = ERROR;
    intLevel = ::intLock();
    locked = true;
  }
}

void CCriticalSection::Unlock()
{
  if (locked == true) {
    ::intUnlock(intLevel);
    if (taskLocked == OK) 
      ::taskUnlock();
    locked = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Threads
static int const priorities[] = {
  VX_LOWEST_PRIORITY,
  VX_LOW_PRIORITY,
  VX_NORMAL_PRIORITY,
  VX_DISPLAY_PRIORITY,
  VX_URGENT_DISPLAY_PRIORITY
};

int PThread::ThreadFunction(void *threadPtr)
{
	PAssertNULL(threadPtr);
  PThread * thread = (PThread *)threadPtr;
  PProcess & process = PProcess::Current();
  process.activeThreadMutex.Wait();
  process.activeThreads.SetAt(thread->PX_threadId, thread);
  process.activeThreadMutex.Signal();
  process.SignalTimerChange();

  if (::semTake(thread->syncPoint, WAIT_FOREVER) == OK) {
    if (::semDelete(thread->syncPoint) == OK)
      thread->syncPoint = NULL;
  thread->Main();
  }
  else
    printf("::ThreadFunction> ::semTake failed, errno=0x%X\n",errno);
  
  // And delete from administration
  process.activeThreadMutex.Wait();
  process.activeThreads.SetAt(thread->PX_threadId, NULL);
  process.activeThreadMutex.Signal();
  thread->PX_threadId = 0;

  return 0;
}

void PThread::Trace(PThreadIdentifer threadId)
{
  if (threadId == 0)
    threadId = PThread::GetCurrentThreadId();

  printf("Task name=%s\n", ::taskName(threadId));

  ::taskRegsShow(threadId); 
  REG_SET regSet;       
  if (::taskRegsGet(threadId, &regSet) != ERROR)
    ::trcStack(&regSet, (FUNCPTR)dbgPrintCall, threadId);
  else
    printf("::Terminate> ::taskRegsGet failed, errno=0x%X\n", errno);
  ::taskShow(0, 2);
  printf("\n");
  ::checkStack(0);
}

PThread::PThread()
 : PX_threadId(ERROR),
   priority(VX_NORMAL_PRIORITY),
   originalStackSize(0)
{
}

PThread::PThread(PINDEX stackSize,
                 AutoDeleteFlag deletion,
                 Priority priorityLevel,
     		   const PString & name
)
{
  PAssert(stackSize > 0, PInvalidParameter);
  autoDelete = (deletion == AutoDeleteThread);
  originalStackSize = stackSize;

  priority = priorities[priorityLevel];

  syncPoint = ::semMCreate(SEM_Q_FIFO);
  if (syncPoint != NULL) {
    if (::semTake(syncPoint, NO_WAIT) == OK) {
      STATUS taskLocked;
      taskLocked = ::taskLock();
      PX_threadId = ::taskSpawn(name,                         // Name
					priority,                     // Priority 
					0,                            // options	
					stackSize,                    // stacksize
					(FUNCPTR)ThreadFunction,      // entrypoint
					(int)this,0,0,0,0,0,0,0,0,0); // arg 1 --- arg 10

      if (PX_threadId != ERROR) {
        // threads are created suspended
        Suspend();
        ::semGive(syncPoint);

        if (taskLocked == OK) 
          ::taskUnlock();

        if (autoDelete) {
          PProcess & process = PProcess::Current();
          process.deleteThreadMutex.Wait();
          process.autoDeleteThreads.Append(this);
          process.deleteThreadMutex.Signal();
        }
      }
      else {
        if (taskLocked == OK) 
          ::taskUnlock();
        printf("::PThread> ::taskSpawn failed, errno=0x%X\n", errno);
        PX_threadId = 0;
        ::semDelete(syncPoint);
        syncPoint = NULL;
      }                 
    }
    else {
      printf("::PThread> ::semTake failed, errno=0x%X\n", errno);
      ::semDelete(syncPoint);
      syncPoint = NULL;
    }
  }
}


PThread::~PThread()
{
  if (originalStackSize <= 0)
    return;

  if (!IsTerminated())
    Terminate();
}


void PThread::Restart()
{
  if (IsTerminated()) {
    PX_threadId = ::taskSpawn(NULL,                         // Auto name tn
					priority, 										// Priority 
					0,														// options	
					originalStackSize,            // stacksize
					(FUNCPTR)ThreadFunction,			// entrypoint
					(int)this,0,0,0,0,0,0,0,0,0);	// arg 1 --- arg 10
         
    if (PX_threadId == ERROR) {
      printf("::Restart> ::taskSpawn failed, errno=0x%X\n", errno);
      PX_threadId = 0;
    }                  
  }
  else
    printf("::Restart> Cannot restart running thread\n");
}

void PThread::Terminate()
{
  if (originalStackSize <= 0)
    return;
	
  if (!IsTerminated()) {
    if (::taskDelete(PX_threadId) == ERROR)
      printf("::Terminate> ::taskDelete failed, errno=0x%X\n", errno);
    
    // And delete from administration
    PProcess & process = PProcess::Current();
    process.activeThreadMutex.Wait();
    process.activeThreads.SetAt(PX_threadId, NULL);
    process.activeThreadMutex.Signal();
    PX_threadId = 0;
  }
}


BOOL PThread::IsTerminated() const
{
  STATUS stat = ERROR;
  if (PX_threadId != 0)
    stat = ::taskIdVerify(PX_threadId);

		return stat == ERROR;
}


void PThread::WaitForTermination() const
{
  while (!IsTerminated()) {
    Current()->Sleep(100);
	}
}


BOOL PThread::WaitForTermination(const PTimeInterval & maxWait) const
{
  if (PX_threadId == 0)
    return TRUE;

  PTimer timeout = maxWait;
  while (!IsTerminated()) {
    if (timeout == 0)
      return FALSE;
    Current()->Sleep(100);
  }
 return TRUE;
}


void PThread::Suspend(BOOL susp)
{
  if (!IsTerminated()) {
    if (susp) {
      if (::taskSuspend(PX_threadId) == ERROR)
        printf("::Suspend> Thread doesn't want to suspend, errno=0x%X\n", errno);
  }
  else {
      if (::taskResume(PX_threadId) == ERROR)
        printf("::Suspend> Thread doesn't want to resume, errno=0x%X\n", errno);
    }
	}
  else
    printf("::Suspend> Operation on terminated thread\n");
}


void PThread::Resume()
{
	if (!IsTerminated()) {
    if (::taskResume(PX_threadId) == ERROR)
      printf("::Resume> Thread doesn't want to resume, errno=0x%X\n", errno);
	}
  else
    printf("::Resume> Operation on terminated thread\n");
}


BOOL PThread::IsSuspended() const
{
  BOOL isSuspended = FALSE;
  if (!IsTerminated())
    isSuspended = ::taskIsSuspended(PX_threadId);
  else
    printf("::IsSuspended> Operation on terminated thread\n");

  return isSuspended;
}

void PThread::SetAutoDelete(AutoDeleteFlag deletion)
{
  PAssert(deletion != AutoDeleteThread || this != &PProcess::Current(), PLogicError);
  autoDelete = deletion == AutoDeleteThread;
}

void PThread::SetPriority(Priority priorityLevel)
{
  if (!IsTerminated()) {
  priority = priorities[priorityLevel];
      if (::taskPrioritySet(PX_threadId, priority ) == ERROR)
        printf("::SetPriority> ::taskPrioritySet failed, errno: 0x%X\n", errno);
  }
  else
    printf("::SetPriority> Operation on terminated thread\n");
}


PThread::Priority PThread::GetPriority() const
{
  if (!IsTerminated()) {
	int prio;
    if (::taskPriorityGet(PX_threadId, &prio) == OK) {
  switch (prio) {
    case VX_LOWEST_PRIORITY :
      return LowestPriority;
    case VX_LOW_PRIORITY :
      return LowPriority;
    case VX_NORMAL_PRIORITY :
      return NormalPriority;
    case VX_DISPLAY_PRIORITY :
      return HighPriority;
    case VX_URGENT_DISPLAY_PRIORITY :
      return HighestPriority;
        default:
          printf("::GetPriority> Priority %d not mapped\n", prio);
          break;
  }
    }                                               
    else
      printf("::GetPriority> ::taskPriorityGet failed, errno=0x%X\n", errno);
  }
  else
    printf("::GetPriority> Operation on terminated thread\n");

  return LowestPriority;
}

void PThread::Yield()
{
  ::taskDelay(NO_WAIT);
}

void PThread::Sleep( const PTimeInterval & delay ) // Time interval to sleep for in microsec.
{
  ::taskDelay(delay.GetInterval()*sysClkRateGet()/1000);
}

void PThread::InitialiseProcessThread()
{
  originalStackSize = 0;
  autoDelete = FALSE;

  PX_threadId = ::taskIdSelf();
  PAssertOS((PX_threadId != ERROR) && (PX_threadId != 0));

  ((PProcess *)this)->activeThreads.DisallowDeleteObjects();
  ((PProcess *)this)->activeThreads.SetAt(PX_threadId, this);
}


PThread * PThread::Current()
{
  PProcess & process = PProcess::Current();
  process.activeThreadMutex.Wait();
  
  PThread * thread = process.activeThreads.GetAt(::taskIdSelf());

  process.activeThreadMutex.Signal();
  return thread;
}

int PThread::PXBlockOnChildTerminate(int pid, const PTimeInterval & /*timeout*/) // Fix timeout
{
  while (!IsTerminated()) {
    Current()->Sleep(100);
  }
  return TRUE;
}

int PThread::PXBlockOnIO(int handle, int type, const PTimeInterval & timeout)
{
  // make sure we flush the buffer before doing a write
  fd_set tmp_rfd, tmp_wfd, tmp_efd;
  fd_set * read_fds      = &tmp_rfd;
  fd_set * write_fds     = &tmp_wfd;
  fd_set * exception_fds = &tmp_efd;

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

  P_timeval tval = timeout;
  return ::select(handle+1, read_fds, write_fds, exception_fds, tval);
}

void PThread::PXAbortBlock() const
{
}

///////////////////////////////////////////////////////////////////////////////
// PProcess::HouseKeepingThread

void PProcess::Construct()
{
  // hard coded value, change this to handle more sockets at once with the select call
  maxHandles = 1024; 
  houseKeeper=NULL;
  CommonConstruct();
}

PProcess::HouseKeepingThread::HouseKeepingThread()
  : PThread(10000, NoAutoDeleteThread, HighPriority, PString("HKeeping"))
{
  Resume();
}

void PProcess::HouseKeepingThread::Main()
{
	PProcess & process = PProcess::Current();

	while(1) {
		process.deleteThreadMutex.Wait();
    for (PINDEX i = 0; i < process.autoDeleteThreads.GetSize(); i++) {
			PThread * pThread = (PThread *) process.autoDeleteThreads.GetAt(i);
			if( pThread->IsTerminated() )
				process.autoDeleteThreads.RemoveAt(i--);
			}
		process.deleteThreadMutex.Signal();
		PTimeInterval nextTimer = process.timers.Process();
    if (nextTimer != PMaxTimeInterval) {
			if ( nextTimer.GetInterval() > 10000 )
				nextTimer = 10000;
			}
		breakBlock.Wait( nextTimer );
	}
}

void PProcess::SignalTimerChange()
{
  if (houseKeeper == NULL) {
    // Prevent reentrance before the following assignment is done
    // Placed after above if-statement due to efficiency, and so 
    // requires an another NULL-test.
    CCriticalSection section;
    section.Lock();
  if (houseKeeper == NULL)
     houseKeeper = new HouseKeepingThread;  
    section.Unlock();
  }
  else
    houseKeeper->breakBlock.Signal();
}

///////////////////////////////////////////////////////////////////////////////
// PProcess

PProcess::~PProcess()
{
  PreShutdown();

  Sleep(100);  // Give threads time to die a natural death

  delete houseKeeper;

  // OK, if there are any left we get really insistent...
  activeThreadMutex.Wait();
  for (PINDEX i = 0; i < activeThreads.GetSize(); i++) {
    PThread & thread = activeThreads.GetDataAt(i);
    if (this != &thread && !thread.IsTerminated())
      thread.Terminate();  // With extreme prejudice
  }
  activeThreadMutex.Signal();

  deleteThreadMutex.Wait();
  autoDeleteThreads.RemoveAll();
  deleteThreadMutex.Signal();

  delete configFiles;
}

///////////////////////////////////////////////////////////////////////////////
// PSemaphore

PSemaphore::PSemaphore(SEM_ID anId)
{
  initialVar = 1;
  maxCountVar = UINT_MAX;
  semId = anId;
  PAssertOS(semId != NULL);
}

PSemaphore::PSemaphore(unsigned initial, unsigned maxCount)
{
  if (initial > maxCount)
    initial = maxCount;
  initialVar = initial;
  maxCountVar = maxCount;
  semId = ::semCCreate(SEM_Q_FIFO, initial);
  PAssertOS(semId != NULL);
}

PSemaphore::PSemaphore(const PSemaphore & sem)
{
  initialVar = sem.GetInitial();
  maxCountVar = sem.GetMaxCount();

  if (initialVar > maxCountVar)
    initialVar = maxCountVar;
  semId = ::semCCreate(SEM_Q_FIFO, initialVar);
  PAssertOS(semId != NULL);
}

PSemaphore::~PSemaphore()
	{
  if (semId != NULL) {
    if (::semDelete(semId) == OK)
      semId = NULL;
    else
      printf("~PSemaphore> Error delete with ID=0x%X with errno: 0x%X\n", 
             (unsigned int)semId, errno);
	}
}

void PSemaphore::Wait()
{
  STATUS result = ::semTake(semId, WAIT_FOREVER);
  if (result == OK) {
    CCriticalSection section;
    section.Lock();
    initialVar--;
    section.Unlock();
  }
	PAssertOS(result != ERROR);
}


BOOL PSemaphore::Wait(const PTimeInterval & timeout)
{
  long wait;
	if (timeout == PMaxTimeInterval) {
		wait = WAIT_FOREVER;
	}
	else {
	 int ticks = sysClkRateGet();
	 wait = (timeout.GetInterval() * ticks);
 	 wait = wait / 1000;
    wait++; // wait at least one tick
	}
  STATUS result = ::semTake(semId, wait);
  if (result == OK) {
    CCriticalSection section;
    section.Lock();
    initialVar--;
    section.Unlock();
  }
  PAssertOS((result != ERROR) || (errno == S_objLib_OBJ_TIMEOUT));
  return result != ERROR;
}

void PSemaphore::Signal()
{
  CCriticalSection section;
  section.Lock();
  if (initialVar < maxCountVar) {
    section.Unlock();
    STATUS result = ::semGive(semId);
    section.Lock();
    if (result == OK)
      initialVar++;
    PAssertOS(result != ERROR);
  }
  section.Unlock();
}

BOOL PSemaphore::WillBlock() const
{
  return initialVar == 0;
}

///////////////////////////////////////////////////////////////////////////////
// PMutex  
PMutex::PMutex() 
: PSemaphore( ::semMCreate(SEM_Q_FIFO) )
{
}

PMutex::PMutex(const PMutex & mutex)
: PSemaphore(::semMCreate(SEM_Q_FIFO))
{
}

PMutex::~PMutex()
{
  if (semId != NULL) {
    if (::semDelete(semId) == OK)
      semId = NULL;
    else
      printf("~PMutex> Error delete with ID=0x%X with errno: 0x%X\n", 
             (unsigned int)semId, errno);
  }
}

void PMutex::Wait()
{
  STATUS result = ::semTake(semId, WAIT_FOREVER);
	PAssertOS(result == OK);
}

BOOL PMutex::Wait(const PTimeInterval & timeout)
{
  long wait;
	if (timeout == PMaxTimeInterval) {
		wait = WAIT_FOREVER;
	}
	else {
	 int ticks = sysClkRateGet();
	 wait = (timeout.GetMilliSeconds() * ticks);
	 wait = wait / 1000;
    wait++; // wait at least one tick
	}
  STATUS result = ::semTake(semId, wait);
  PAssertOS((result != ERROR) || (errno == S_objLib_OBJ_TIMEOUT));
  return result != ERROR;
}

void PMutex::Signal()
{
	::semGive(semId);
}

BOOL PMutex::WillBlock() const 
{
  STATUS result = ::semTake(semId, NO_WAIT);
  PAssertOS((result != ERROR) || (errno == S_objLib_OBJ_UNAVAILABLE));
  if (result != ERROR)
    ::semGive(semId);
		return result == ERROR;
}

///////////////////////////////////////////////////////////////////////////////
// PSyncPoint

PSyncPoint::PSyncPoint()
  : PSemaphore(0, 1)
{
}

PSyncPoint::PSyncPoint(const PSyncPoint & syncPoint)
  : PSemaphore(syncPoint)
{
}

// End Of File ///////////////////////////////////////////////////////////////
