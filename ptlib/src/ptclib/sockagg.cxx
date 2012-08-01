/*
 * sockagg.cxx
 *
 * Generalised Socket Aggregation functions
 *
 * Portable Windows Library
 *
 * Copyright (C) 2005 Post Increment
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
 * The Initial Developer of the Original Code is Post Increment
 *
 * Portions of this code were written with the financial assistance of 
 * Metreos Corporation (http://www.metros.com).
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: sockagg.cxx,v $
 * Revision 1.22  2007/10/03 01:18:46  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.21  2007/09/17 11:14:46  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.20  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.19  2006/11/20 03:17:24  csoutheren
 * Fixed incorrect check time on trace message
 *
 * Revision 1.18  2006/07/22 06:27:58  rjongbloed
 * Added auo-load of Winsock v2 library required by agreggated sockets.
 *
 * Revision 1.17  2006/03/13 23:34:21  csoutheren
 * Added log message when handle creates aggregator
 *
 * Revision 1.16  2006/03/09 05:32:59  csoutheren
 * Reverted to conservative locking strategy, with OnClose
 *
 * Revision 1.15  2006/03/07 11:04:56  csoutheren
 * Ensure socket aggregation not used on Linux
 *
 * Revision 1.14  2006/03/07 07:38:02  csoutheren
 * Refine timing windows on socket handling and cleanup unused code
 *
 * Revision 1.13  2006/03/06 02:37:26  csoutheren
 * Change handle locking to help prevent aggregation threads from hogging list
 *  access
 *
 * Revision 1.12  2006/03/02 07:50:38  csoutheren
 * Cleanup unused code
 * Add OnClose function
 *
 * Revision 1.11  2006/02/28 02:26:00  csoutheren
 * Renamed variable to be not same as member
 *
 * Revision 1.10  2006/02/28 02:08:02  csoutheren
 * Modified aggregation to load balance better
 *
 * Revision 1.9  2006/02/08 04:02:25  csoutheren
 * Added ability to enable and disable socket aggregation
 *
 * Revision 1.8  2006/01/23 05:57:39  csoutheren
 * More aggegator implementation
 *
 * Revision 1.7  2006/01/18 07:16:56  csoutheren
 * Latest version of socket aggregation code
 *
 * Revision 1.6  2006/01/05 11:39:32  rjongbloed
 * Fixed DevStudio warning
 *
 * Revision 1.5  2006/01/03 04:23:32  csoutheren
 * Fixed Unix implementation
 *
 * Revision 1.4  2005/12/23 07:49:27  csoutheren
 * Fixed Unix implementation
 *
 * Revision 1.3  2005/12/23 06:44:31  csoutheren
 * Working implementation
 *
 * Revision 1.2  2005/12/22 07:27:36  csoutheren
 * More implementation
 *
 * Revision 1.1  2005/12/22 03:55:52  csoutheren
 * Added initial version of socket aggregation classes
 *
 */

#ifdef __GNUC__
#pragma implementation "sockagg.h"
#endif


#include <ptlib.h>
#include <ptclib/sockagg.h>

#define new PNEW


////////////////////////////////////////////////////////////////

#if _WIN32

#pragma comment(lib, "Ws2_32.lib")

class LocalEvent : public PHandleAggregator::EventBase
{
  public:
    LocalEvent()
    { 
      event = CreateEvent(NULL, TRUE, FALSE,NULL); 
      PAssert(event != NULL, "CreateEvent failed");
    }

    ~LocalEvent()
    { CloseHandle(event); }

    PAggregatorFD::FD GetHandle()
    { return event; }

    void Set()
    { SetEvent(event);  }

    void Reset()
    { ResetEvent(event); }

  protected:
    HANDLE event;
};

PAggregatorFD::PAggregatorFD(SOCKET v)
  : socket(v) 
{ 
  fd = WSACreateEvent(); 
  PAssert(WSAEventSelect(socket, fd, FD_READ | FD_CLOSE) == 0, "WSAEventSelect failed"); 
}

PAggregatorFD::~PAggregatorFD()
{ 
  WSACloseEvent(fd); 
}

bool PAggregatorFD::IsValid()
{ 
  return socket != INVALID_SOCKET; 
}

#else // #if _WIN32

#include <fcntl.h>

class LocalEvent : public PHandleAggregator::EventBase
{
  public:
    LocalEvent()
    { ::pipe(fds); }

    virtual ~LocalEvent()
    {
      close(fds[0]);
      close(fds[1]);
    }

    PAggregatorFD::FD GetHandle()
    { return fds[0]; }

    void Set()
    { char ch; write(fds[1], &ch, 1); }

    void Reset()
    { char ch; read(fds[0], &ch, 1); }

  protected:
    int fds[2];
};

PAggregatorFD::PAggregatorFD(int v)
  : fd(v) 
{ 
}

PAggregatorFD::~PAggregatorFD()
{
}

bool PAggregatorFD::IsValid()
{ 
  return fd >= 0; 
}

#endif // #endif _WIN32
  

////////////////////////////////////////////////////////////////

PHandleAggregator::WorkerThreadBase::WorkerThreadBase(EventBase & _event)
  : PThread(100, NoAutoDeleteThread, NormalPriority, "Aggregator:%0x"), event(_event), listChanged(TRUE), shutdown(FALSE)
{ 
}

class WorkerThread : public PHandleAggregator::WorkerThreadBase
{
  public:
    WorkerThread()
      : WorkerThreadBase(localEvent)
    { }

    ~WorkerThread()
    {
    }

    void Trigger()
    { localEvent.Set(); }

    LocalEvent localEvent;
};


////////////////////////////////////////////////////////////////

PHandleAggregator::PHandleAggregator(unsigned _max)
  : maxWorkerSize(_max)
{ 
}

BOOL PHandleAggregator::AddHandle(PAggregatedHandle * handle)
{
  // perform the handle init function
  if (!handle->Init())
    return FALSE;

  PWaitAndSignal m(listMutex);

  // if the maximum number of worker threads has been reached, then
  // use the worker thread with the minimum number of handles
  if (workers.size() >= maxWorkerSize) {
    WorkerList_t::iterator minWorker = workers.end();
    size_t minSizeFound = 0x7ffff;
    WorkerList_t::iterator r;
    for (r = workers.begin(); r != workers.end(); ++r) {
      WorkerThreadBase & worker = **r;
      PWaitAndSignal m2(worker.workerMutex);
      if (!worker.shutdown && (worker.handleList.size() <= minSizeFound)) {
        minSizeFound = worker.handleList.size();
        minWorker     = r;
      }
    }

    // add the worker to the thread
    PAssert(minWorker != workers.end(), "could not find minimum worker");

    WorkerThreadBase & worker = **minWorker;
    PWaitAndSignal m2(worker.workerMutex);
    worker.handleList.push_back(handle);
    PTRACE(4, "SockAgg\tAdding handle " << (void *)handle << " to aggregator - " << worker.handleList.size() << " handles");
    worker.listChanged = TRUE;
    worker.Trigger();
    return TRUE;
  }

  PTRACE(4, "SockAgg\tCreating new aggregator for " << (void *)handle);

  // no worker threads usable, create a new one
  WorkerThread * worker = new WorkerThread;
  worker->handleList.push_back(handle);
  worker->Resume();
  workers.push_back(worker);

  PTRACE(4, "SockAgg\tAdding handle " << (void *)handle << " to new aggregator");

  return TRUE;
}

BOOL PHandleAggregator::RemoveHandle(PAggregatedHandle * handle)
{
  listMutex.Wait();

  // look for the thread containing the handle we need to delete
  WorkerList_t::iterator r;
  for (r = workers.begin(); r != workers.end(); ++r) {
    WorkerThreadBase * worker = *r;

    // lock the worker
    worker->workerMutex.Wait();

    PAggregatedHandleList_t & hList = worker->handleList;

    // if handle is not in this thread, then continue searching
    PAggregatedHandleList_t::iterator s = find(hList.begin(), hList.end(), handle);
    if (s == hList.end()) {
      worker->workerMutex.Signal();
      continue;
    }

    PAssert(*s == handle, "Found handle is not correct!");

    PAssert(!handle->beingProcessed, "trying to close handle that is in use");

    // remove the handle from the worker's list of handles
    worker->handleList.erase(s);

    // do the de-init action
    handle->DeInit();

    // delete the handle if autodelete enabled
    if (handle->autoDelete)
      delete handle;

    // if the worker thread has enough handles to keep running, trigger it to update
    if (worker->handleList.size() > 0) {
      PTRACE(4, "SockAgg\tRemoved handle " << (void *)handle << " from aggregator - " << worker->handleList.size() << " handles remaining");
      worker->listChanged = TRUE;
      worker->Trigger();
      worker->workerMutex.Signal();

      listMutex.Signal();

      return TRUE;
    }

    PTRACE(4, "SockAgg\tworker thread empty - closing down");

    // remove the worker thread from the list of workers
    workers.erase(r);

    // shutdown the thread
    worker->shutdown = TRUE;
    worker->Trigger();
    worker->workerMutex.Signal();

    // unlock the list
    listMutex.Signal();

    // the worker is now finished
    if (!worker->WaitForTermination(10000)) {
      PTRACE(4, "SockAgg\tWorker did not terminate promptly");
    }

    delete worker;

    return TRUE;
  }

  listMutex.Signal();

  PAssertAlways("Cannot find aggregator handle");

  return FALSE;
}

////////////////////////////////////////////////////////////////

typedef std::vector<PAggregatorFD::FD> fdList_t;

#ifdef _WIN32
#define	FDLIST_SIZE	WSA_MAXIMUM_WAIT_EVENTS
#else
#define	FDLIST_SIZE	64
#endif

void PHandleAggregator::WorkerThreadBase::Main()
{
  PTRACE(4, "SockAgg\taggregator started");

  fdList_t                  fdList;
  PAggregatorFDList_t       aggregatorFdList;

  typedef std::map<PAggregatorFD::FD, PAggregatedHandle *> PAggregatorFdToHandleMap_t;
  PAggregatorFdToHandleMap_t aggregatorFdToHandleMap;

  for (;;) {

    // create the list of fds to wait on and find minimum timeout
    PTimeInterval timeout(PMaxTimeInterval);
    PAggregatedHandle * timeoutHandle = NULL;

#ifndef _WIN32
    fd_set rfds;
    FD_ZERO(&rfds);
    int maxFd = 0;
#endif

    {
      PWaitAndSignal m(workerMutex);

      // check for shutdown
      if (shutdown)
        break;

      // if the list of handles has changed, clear the list of handles
      if (listChanged) {
        aggregatorFdList.erase       (aggregatorFdList.begin(),      aggregatorFdList.end());
        aggregatorFdList.reserve     (FDLIST_SIZE);
        fdList.erase                 (fdList.begin(),                fdList.end());
        fdList.reserve               (FDLIST_SIZE);
        aggregatorFdToHandleMap.erase(aggregatorFdToHandleMap.begin(),         aggregatorFdToHandleMap.end());
      }

      PAggregatedHandleList_t::iterator r;
      for (r = handleList.begin(); r != handleList.end(); ++r) {
        PAggregatedHandle * handle = *r;

        if (handle->closed)
          continue;

        if (listChanged) {
          PAggregatorFDList_t fds = handle->GetFDs();
          PAggregatorFDList_t::iterator s;
          for (s = fds.begin(); s != fds.end(); ++s) {
            fdList.push_back          ((*s)->fd);
            aggregatorFdList.push_back((*s));
            aggregatorFdToHandleMap.insert(PAggregatorFdToHandleMap_t::value_type((*s)->fd, handle));
          }
        }

        if (!handle->IsPreReadDone()) {
          handle->PreRead();
          handle->SetPreReadDone();
        }

        PTimeInterval t = handle->GetTimeout();
        if (t < timeout) {
          timeout = t;
          timeoutHandle = handle;
        }
      }

      // add in the event fd
      if (listChanged) {
        fdList.push_back(event.GetHandle());
        listChanged = FALSE;
      }

#ifndef _WIN32
      // create the list of FDs
      fdList_t::iterator s;
      for (s = fdList.begin(); s != fdList.end(); ++s) {
        FD_SET(*s, &rfds);
        maxFd = PMAX(maxFd, *s);
      }
#endif
    } // workerMutex goes out of scope

#ifdef _WIN32
    DWORD nCount = fdList.size();
    DWORD ret = WSAWaitForMultipleEvents(nCount, 
                                         &fdList[0], 
                                         false, 
                                         (timeout == PMaxTimeInterval) ? WSA_INFINITE : (DWORD)timeout.GetMilliSeconds(), 
                                         FALSE);

    if (ret == WAIT_FAILED) {
      PTRACE(1, "SockAgg\tWSAWaitForMultipleEvents error " << WSAGetLastError());
    }

    {
      PWaitAndSignal m(workerMutex);

      // check for shutdown
      if (shutdown)
        break;

      if (ret == WAIT_TIMEOUT) {
        // make sure the handle did not disappear while we were waiting
        PAggregatedHandleList_t::iterator s = find(handleList.begin(), handleList.end(), timeoutHandle);
        if (s == handleList.end()) {
          PTRACE(4, "SockAgg\tHandle was removed while waiting");
        } 
        else {
          PTime start;

          timeoutHandle->beingProcessed = TRUE;
          timeoutHandle->closed = !timeoutHandle->OnRead();
          timeoutHandle->beingProcessed = FALSE;
  
          unsigned duration = (unsigned)(PTime() - start).GetMilliSeconds();
          if (duration > 50) {
            PTRACE(4, "SockAgg\tWarning - aggregator read routine was of extended duration = " << duration << " msecs");
          }
          if (!timeoutHandle->closed)
            timeoutHandle->SetPreReadDone(FALSE);
        }
      }

      else if (WAIT_OBJECT_0 <= ret && ret <= (WAIT_OBJECT_0 + nCount - 1)) {
        DWORD index = ret - WAIT_OBJECT_0;

        // if the event was triggered, redo the select
        if (index == nCount-1) {
          event.Reset();
          continue;
        }

        PAggregatorFD * fd = aggregatorFdList[index];
        PAssert(fdList[index] == fd->fd, "Mismatch in fd lists");

        PAggregatorFdToHandleMap_t::iterator r = aggregatorFdToHandleMap.find(fd->fd);
        if (r != aggregatorFdToHandleMap.end()) {
          PAggregatedHandle * handle = r->second;
          PAggregatedHandleList_t::iterator s = find(handleList.begin(), handleList.end(), handle);
          if (s == handleList.end()) {
            PTRACE(4, "SockAgg\tHandle was removed while waiting");
          }
          else {
            WSANETWORKEVENTS events;
            WSAEnumNetworkEvents(fd->socket, fd->fd, &events);
            if (events.lNetworkEvents != 0) {

              // check for read events first so we process any data that arrives before closing
              if ((events.lNetworkEvents & FD_READ) != 0) {

                PTime start;

                handle->beingProcessed = TRUE;
                handle->closed = !handle->OnRead();
                handle->beingProcessed = FALSE;
    
#if PTRACING
                if (PTrace::CanTrace(4)) {
                  unsigned duration = (unsigned)(PTime() - start).GetMilliSeconds();
                  if (duration > 50)
                    PTRACE(4, "SockAgg\tWarning - aggregator read routine was of extended duration = " << duration << " msecs");
                }
#endif
              }

              // check for socket close
              if ((events.lNetworkEvents & FD_CLOSE) != 0)
                handle->closed = TRUE;

              if (!handle->closed) {
                // prepare for next read
                handle->SetPreReadDone(FALSE);
              } else {
                handle->beingProcessed = TRUE;
                handle->OnClose();
                handle->beingProcessed = FALSE;

                // make sure the list is refreshed without the closed socket
                listChanged = TRUE;
              }
            }
          }
        }
      }
    } // workerMutex goes out of scope

#else

#error "aggregation not yet implemented on Unix"

#if 0

    P_timeval pv = timeout;
    int ret = ::select(maxFd+1, &rfds, NULL, NULL, pv);

    if (ret < 0) {
      PTRACE(1, "SockAgg\tSelect failed with error " << errno);
    }

    // loop again if nothing was ready
    if (ret <= 0)
      continue;

    {
      PWaitAndSignal m(workerMutex);

      // check for shutdown
      if (shutdown)
        break;

      if (ret == 0) {
        PTime start;
        BOOL closed = !timeoutHandle->OnRead();
        unsigned duration = (unsigned)(PTime() - start).GetMilliSeconds();
        if (duration > 50) {
          PTRACE(4, "SockAgg\tWarning - aggregator read routine was of extended duration = " << duration << " msecs");
        }
        if (!closed)
          timeoutHandle->SetPreReadDone(FALSE);
      }

      // check the event first
      else if (FD_ISSET(event.GetHandle(), &rfds)) {
        event.Reset();
        continue;
      }

      else {
        PAggregatorFD * fd = aggregatorFdList[ret];
        PAssert(fdList[ret] == fd->fd, "Mismatch in fd lists");
        PAggregatorFdToHandleMap_t::iterator r = aggregatorFdToHandleMap.find(fd->fd);
        if (r != aggregatorFdToHandleMap.end()) {
          PAggregatedHandle * handle = r->second;
          PTime start;
          BOOL closed = !handle->OnRead();
          unsigned duration = (unsigned)(PTime() - start).GetMilliSeconds();
          if (duration > 50) {
            PTRACE(4, "SockAgg\tWarning - aggregator read routine was of extended duration = " << duration << " msecs");
          }
          if (!closed)
            handle->SetPreReadDone(FALSE);
        }
      }
    } // workerMutex goes out of scope
#endif // #if 0
#endif
  }

  PTRACE(4, "SockAgg\taggregator finished");
}


