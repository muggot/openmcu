/*
 * thread.h
 *
 * Thread of execution control class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 * Copyright (c) 1999 ISDN Communications Ltd
 *
 */

 
#ifndef _PTHREAD

// CWP
#undef Yield

#ifndef __NUCLEUS_MNT__
#pragma interface
#endif

#include <setjmp.h>

class PProcess;
class PSemaphore;

class pwNUTask;
class TaskInfo;     // Nucleus stuff is included LATER.	

///////////////////////////////////////////////////////////////////////////////
// PThread

#include "../../thread.h"

public:
  int PXBlockOnIO(int handle,
                  int type,
                  const PTimeInterval & timeout);

private:
  pwNUTask * NucleusTask;
  TaskInfo * NucleusTaskInfo;
  long int n_suspendCount;

// So that it can get at its buffer in NucleusTask!
friend PString inet_ntoa(const char in[]);
// So that it can start up the Nucleus task
friend void InitializeCppApplication();
};

class pwNUTask : public Task
  {
private:
  PThread * AssociatedPThread;

  friend PThread * PThread::Current();

public:
static const UNSIGNED NUTimeSlicesPermitted;

  pwNUTask(UNSIGNED stacksize, OPTION priority, PThread * AssocPThread, const char *name)
    : Task((CHAR *)name,                      // As Given
            stacksize*3,              // As Given
            priority,                 // As Given
            NUTimeSlicesPermitted,    // Er... Pass!
            TRUE /* Preempt*/),       // Pre-emptable
      AssociatedPThread(AssocPThread)
    {
    }

  virtual void Entry();

// Our version of inet_ntoa needs to be thread-safe.  My solution is to have
// a buffer per task.
friend PString inet_ntoa(const char in[]);
friend PString inet_ntoa(struct in_addr addy);
private:
  char PROP_inet_ntoa_buffer[16];
  };


#endif
