/*
 * NucleusSemaphore.cxx
 *
 * pwlib's PSemaphore as implemented for Nucleus++
 *
 * Copyright (c) 1999 ISDN Communications Ltd
 *
 * Author: Chris Wayman Purvis
 *
 */

#include <ptlib.h>

#define new PNEW


PSemaphore::PSemaphore(unsigned initial, unsigned maximum)
  {
  pNucleusSemaphore = new FifoSemaphore("pwlib", maximum);
// Kludge to get round the fact that Nucleus semaphore can not be initialised
// with different initial and maximum values.  Note that to my knowledge we
// only use ones where maximum = 1 anyway!
  int i = initial;
  while (initial++ < maximum)
    {
    Wait();
    }
  }

PSemaphore::~PSemaphore()
  {
  delete pNucleusSemaphore;
  }

void PSemaphore::Wait()
  {
  pNucleusSemaphore->Obtain();
  }
  
BOOL PSemaphore::Wait(const PTimeInterval & timeout)
  {
// Convert timeout to a number of ticks!
  STATUS retval =  pNucleusSemaphore->Obtain( timeout.GetInterval()/
                                                        PTimer::Resolution());
  PAssert(retval != NU_INVALID_SEMAPHORE, "Wait on Invalid Semaphore");
  PAssert(retval != NU_INVALID_SUSPEND, "Wait on Semaphore From Non-Task");
  PAssert(retval != NU_UNAVAILABLE, "OS Problem");
  PAssert(retval != NU_SEMAPHORE_DELETED, "Semaphore Deleted");

  if (retval == NU_SUCCESS)
    return TRUE;
  else
    return FALSE;
  }

void PSemaphore::Signal()
  {
  pNucleusSemaphore->Release();
  }

BOOL PSemaphore::WillBlock() const
  {
  SemaphoreInfo * pNucleusSemaphoreInfo = 0;
  STATUS retval = pNucleusSemaphore->Information(&pNucleusSemaphoreInfo);
  PAssert(retval == NU_SUCCESS, "Failure of ATI Semaphore::Information()");
  return pNucleusSemaphoreInfo->semaphoreControlBlock.sm_tasks_waiting != 0;
  }

