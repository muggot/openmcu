/*
 * NucleusThread.cxx
 *
 * pwlib's PThread as implemented for Nucleus++
 *
 * Copyright (c) 1999 ISDN Communications Ltd
 *
 * Author: Chris Wayman Purvis
 *
 */

#include <ptlib.h>
#include <ptlib/sockets.h>
#include "net/inc/externs.h"
#include "plus/nucleus.h"
#include "net/inc/socketd.h"                 /* socket interface structures */
#include "net/target.h"
#include "net/inc/tcpdefs.h"
#include <sys/socket.h>

const UNSIGNED pwNUTask::NUTimeSlicesPermitted = 5;

// Map from pwlib priority level to Nucleus priority level
static int const priorities[] =
  {
  66,    // Lowest Priority
  64,    // Low Priority
  62,    // Normal Priority
  60,    // High Priority
  10     // Highest Priority
  };


#ifdef __NUCLEUS_MNT__
#define new PNEW
#endif

PThread::PThread(PINDEX stackSize, AutoDeleteFlag deletion,
                                    Priority PriorityLevel,
                                   const PString & ThreadName)
// Threads start in a singly-suspended state.  That's the way it works.
  : n_suspendCount(1)
  {
  PAssert(stackSize > 0, PInvalidParameter);

  autoDelete = (deletion == AutoDeleteThread);

  NucleusTask = new pwNUTask( stackSize,// Stack Size Requested
                              priorities[PriorityLevel],
                                    // Map from pwlib level to Nucleus level
                              this,
                              ThreadName);    // So it knows what to call back to!
  STATUS stat = NucleusTask->Information(&NucleusTaskInfo);
  PAssert(stat == NU_SUCCESS, "Failure to find TaskInfo block");
  }

PThread::~PThread()
  {
  if (!IsTerminated())
    {
    Terminate();
    }
  delete NucleusTask;
  }
  
void PThread::Restart()
  {
  PAssert(IsTerminated(), "Cannot restart running thread");
  
  NucleusTask->Reset();
  PAssertAlways("Reset, but not restarted...");
  }

void PThread::Terminate()
  {
  PAssert(!IsTerminated(),
                    "Cannot terminate a thread which is already terminated");
  STATUS stat = NucleusTask->Terminate();
  PAssert(stat == NU_SUCCESS, "Invalid Task Pointer on Termination");
  
  }

#if !P_USE_INLINES

// What a pfaff for something that's easy...
// Still, at last I've written a function I'm pretty confident will work!
BOOL PThread::IsTerminated() const
  {
#ifdef __NUCLEUS_MNT__
  cout << "q";
#else
  printf("q");
#endif
  STATUS stat = NucleusTaskInfo->Update();
  PAssert(stat == NU_SUCCESS, "Invalid Task Pointer on Termination Check");
  return ((NucleusTaskInfo->taskStatus == NU_TERMINATED) || (NucleusTaskInfo->taskStatus == NU_FINISHED));
  }
#endif

void PThread::WaitForTermination() const
  {
  while (!IsTerminated())
    {
    Current()->Sleep(10);
    }
  }

BOOL PThread::WaitForTermination(const PTimeInterval & maxWait) const
  {
  PTimer timeout = maxWait;
  while (!IsTerminated())
    {
    if (timeout == 0)
      {
      return FALSE;
      }
    Current()->Sleep(10);
    }
  return TRUE;
  }

void PThread::Suspend(BOOL susp)
  {
  STATUS stat;
  if (susp)
    {
    if (++n_suspendCount != 1)
      {
      stat = NucleusTask->Suspend();
      PAssert(stat == NU_SUCCESS, "Invalid Task Pointer on suspend");
      }
    }
  else
    {
    Resume();
    }
  }
  
#if !P_USE_INLINES
void PThread::Resume()
  {
  switch (--n_suspendCount)
    {
    case -1:
      PAssertAlways("Resuming thread that hasn't been suspended!");
      n_suspendCount = 0;
      break;
    case 0:
        {
        STATUS  stat = NucleusTask->Resume();
        PAssert(stat == NU_SUCCESS, "Invalid Task Pointer on resume");
        }
      break;
    default:  // Already handled in switch statement
      break; 
    }
  }

BOOL PThread::IsSuspended() const
  {
  return (n_suspendCount != 0);
  }

void PThread::SetPriority(Priority priorityLevel)
  {
  ((Task *)(NucleusTask))->ChangePriority(priorities[priorityLevel]);
  }

PThread::Priority PThread::GetPriority() const
  {
//  TaskInfo * NucleusTaskInfo = 0;
//  STATUS stat = NucleusTask->Information(&NucleusTaskInfo);
  NucleusTaskInfo->Update();
  PAssert(stat == NU_SUCCESS, "Invalid Task Pointer on GetPriority Request");

  BOOL FoundIt = FALSE;

  for ( int i = LowestPriority;
        i < NumPriorities;
        ++i)
    {
    if (NucleusTaskInfo->priority == priorities[i])
      {
      return (PThread::Priority)i;
      }
    }

  PAssertAlways ("PriorityLevel not valid for pwlib task!");
  return (PThread::Priority)NucleusTaskInfo->priority;
  }

PThread * PThread::Current()
  {
  return ((pwNUTask *)Task::Current())->AssociatedPThread;
  }
#endif

void PThread::Yield()
  {
  Task::Relinquish();
  }

void PThread::InitialiseProcessThread()
  {
//#pragma message("Anything to do in InitialiseProcessThread()?  Or all in PProcess constructor?")
// Certainly need to do some of the thready stuff that's normally done in the
// (non-trivial) PThread constructor.  Like starting the Nucleus thread!
  autoDelete          = FALSE;

  NucleusTask = new pwNUTask( (UNSIGNED)2048, // Stack Size Requested
                              (OPTION)priorities[HighPriority],
                                    // Map from pwlib level to Nucleus level
                              (PThread *)this,
                              "PWLIB");// So it knows what to call back to!
  NucleusTaskInfo = new TaskInfo();

  }

void PThread::Sleep(const PTimeInterval & time)
  {
    Task::Sleep(((time.GetInterval())/PTimer::Resolution()));
  }


#if 0
// Unnecessary as Nucleus has threads!
BOOL PThread::IsNoLongerBlocked()
  {
  STATUS stat = NucleusTaskInfo->Update();
  PAssert(stat == NU_SUCCESS, "Invalid Task Pointer on Blocking Check");

  PAssert(NucleusTaskInfo->taskStatus != NU_TERMINATED,
    "Operation on terminated thread");

// No longer blocked iff ready to go!
  return (NucleusTaskInfo->taskStatus) == NU_READY;
  }
#endif

PThread::PThread()
  : n_suspendCount(1)
  {
//Should all be done in IsNoLongerBlocked - but may even be unnecessary there!
  }

// Stolen out of tlib threads
int PThread::PXBlockOnIO(int handle, int type, const PTimeInterval & timeout)
{
  // make sure we flush the buffer before doing a write
  fd_set tmp_rfd, tmp_wfd, tmp_efd;
  fd_set * read_fds      = &tmp_rfd;
  fd_set * write_fds     = &tmp_wfd;
  fd_set * exception_fds = &tmp_efd;

  FD_ZERO(read_fds);
  FD_ZERO(write_fds);
  FD_ZERO(exception_fds);

  switch (type) {
    case PChannel::PXReadBlock:
    case PChannel::PXAcceptBlock:
      FD_SET(handle, read_fds);
      break;
    case PChannel::PXWriteBlock:
      FD_SET(handle, write_fds);
      break;
    case PChannel::PXConnectBlock:
      FD_SET(handle, write_fds);
      FD_SET(handle, exception_fds);
      break;
    default:
      PAssertAlways(PLogicError);
      return 0;
  }

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

  int retval = ::select(handle+1, read_fds, write_fds, exception_fds, tptr);

  PProcess::Current().PXCheckSignals();
  return retval;
}

void pwNUTask::Entry()
  {
  AssociatedPThread->Main();
  }
