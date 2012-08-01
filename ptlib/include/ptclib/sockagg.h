/*
 * sockagg.h
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
 * $Log: sockagg.h,v $
 * Revision 1.9  2006/03/09 05:32:59  csoutheren
 * Reverted to conservative locking strategy, with OnClose
 *
 * Revision 1.8  2006/03/07 07:38:02  csoutheren
 * Refine timing windows on socket handling and cleanup unused code
 *
 * Revision 1.7  2006/03/06 02:37:25  csoutheren
 * Change handle locking to help prevent aggregation threads from hogging list
 *  access
 *
 * Revision 1.6  2006/03/02 07:50:37  csoutheren
 * Cleanup unused code
 * Add OnClose function
 *
 * Revision 1.5  2006/01/18 07:16:56  csoutheren
 * Latest version of socket aggregation code
 *
 * Revision 1.4  2006/01/03 04:23:32  csoutheren
 * Fixed Unix implementation
 *
 * Revision 1.3  2005/12/23 06:44:30  csoutheren
 * Working implementation
 *
 * Revision 1.2  2005/12/22 07:27:36  csoutheren
 * More implementation
 *
 * Revision 1.1  2005/12/22 03:55:52  csoutheren
 * Added initial version of socket aggregation classes
 *
 *
 */


#ifndef _SOCKAGG_H
#define _SOCKAGG_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>
#include <ptlib/sockets.h>

/*

These classes implements a generalised method for aggregating sockets so that they can be handled by a single thread. It is
intended to provide a backwards-compatible mechanism to supplant the "one socket - one thread" model used by OpenH323
and OPAL with a model that provides a better thread to call ratio. A more complete explanation of this model can be
found in the white paper "Increasing the Maximum Call Density of OpenH323 and OPAL" which can be at:

         http://www.voxgratia.org/docs/call%20thread%20handling%20model%201.0.pdf

There are two assumptions made by this code:

  1) The most efficient way to handle I/O is for a thread to be blocked on I/O. Any sort of timer or other
     polling mechanism is less efficient

  2) The time taken to handle a received PDU is relatively small, and will not interfere with the handling of
     other calls that are handled in the same thread

UDP and TCP sockets are aggregated in different ways. UDP sockets are aggregated on the basis of a simple loop that looks
for received datagrams and then processes them. TCP sockets are more complex because there is no intrinsic record-marking 
protocol, so it is difficult to know when a complete PDU has been received. This problem is solved by having the loop collect
received data into a buffer until the handling routine decides that a full PDU has been received.

At the heart of each socket aggregator is a select statement that contains all of the file descriptors that are managed
by the thread. One extra handle for a pipe (or on Windows, a local socket) is added to each handle list so that the thread can
be woken up in order to allow the addition or removal of sockets to the list

*/

#include <ptlib.h>
#include <functional>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////
//
// this class encapsulates the system specific handle needed to specifiy a socket.
// On Unix systems, this is a simple file handle. This file handle is used to uniquely
// identify the socket and used in the "select" system call
// On Windows systems the SOCKET handle is used to identify the socket, but a seperate WSAEVENT
// handle is needed for the WSWaitForMultpleEvents call.
// This is further complicated by the fact that we need to treat some pairs of sockets as a single
// entity (i.e. RTP sockets) to avoid rewriting the RTP handler code.
//

class PAggregatedHandle;

class PAggregatorFD 
{
  public:
#ifdef _WIN32
    typedef WSAEVENT FD;
    typedef SOCKET FDType;
    SOCKET socket;
#else
    typedef int FD;
    typedef int FDType;
#endif

    PAggregatorFD(FDType fd);

    FD fd;

    ~PAggregatorFD();
    bool IsValid();
};

typedef std::vector<PAggregatorFD *> PAggregatorFDList_t;

/////////////////////////////////////////////////////////////////////////////////////
//
// This class defines an abstract class used to define a handle that can be aggregated
//
// Normally this will correspond directly to a socket, but for RTP this actually corresponds to two sockets
// which greatly complicates things
//

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)
#endif

class PAggregatedHandle : public PObject
{
  PCLASSINFO(PAggregatedHandle, PObject);
  public:
    PAggregatedHandle(BOOL _autoDelete = FALSE)
      : autoDelete(_autoDelete), closed(FALSE), beingProcessed(FALSE), preReadDone(FALSE)
    { }

    virtual PAggregatorFDList_t GetFDs() = 0;

    virtual PTimeInterval GetTimeout()
    { return PMaxTimeInterval; }

    virtual BOOL Init()      { return TRUE; }
    virtual BOOL PreRead()   { return TRUE; }
    virtual BOOL OnRead() = 0;
    virtual void DeInit()    { }
    virtual void OnClose()   { }

    virtual BOOL IsPreReadDone() const
    { return preReadDone; }

    virtual void SetPreReadDone(BOOL v = TRUE)
    { preReadDone = v; }

    BOOL autoDelete;
    BOOL closed;
    BOOL beingProcessed;

  protected:
    BOOL preReadDone;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif


/////////////////////////////////////////////////////////////////////////////////////
//
// This class is the actual socket aggregator
//

class PHandleAggregator : public PObject
{
  PCLASSINFO(PHandleAggregator, PObject)
  public:
    class EventBase
    {
      public:
        virtual PAggregatorFD::FD GetHandle() = 0;
        virtual void Set() = 0;
        virtual void Reset() = 0;
    };

    typedef std::vector<PAggregatedHandle *> PAggregatedHandleList_t;

    class WorkerThreadBase : public PThread
    {
      public:
        WorkerThreadBase(EventBase & _event);

        virtual void Trigger() = 0;
        void Main();

        PMutex workerMutex;

        EventBase & event;
        PAggregatedHandleList_t handleList;
        BOOL listChanged;
        BOOL shutdown;
    };

    typedef std::vector<WorkerThreadBase *> WorkerList_t;

    PHandleAggregator(unsigned _max = 10);

    BOOL AddHandle(PAggregatedHandle * handle);

    BOOL RemoveHandle(PAggregatedHandle * handle);

    PMutex listMutex;
    WorkerList_t workers;
    unsigned maxWorkerSize;
};


/////////////////////////////////////////////////////////////////////////////////////
//
// This template class allows the creation of aggregators for sockets that are
// descendants of PIPSocket
//

template <class PSocketType>
class PSocketAggregator : public PHandleAggregator
{
  PCLASSINFO(PSocketAggregator, PHandleAggregator)
  public:
    class AggregatedPSocket : public PAggregatedHandle
    {
      public:
        AggregatedPSocket(PSocketType * _s)
          : psocket(_s), fd(_s->GetHandle()) { }

        BOOL OnRead()
        { return psocket->OnRead(); }

        PAggregatorFDList_t GetFDs()
        { PAggregatorFDList_t list; list.push_back(&fd); return list; }

      protected:
        PSocketType * psocket;
        PAggregatorFD fd;
    };

    typedef std::map<PSocketType *, AggregatedPSocket *> SocketList_t;
    SocketList_t socketList;

    BOOL AddSocket(PSocketType * sock)
    { 
      PWaitAndSignal m(listMutex);

      AggregatedPSocket * handle = new AggregatedPSocket(sock);
      if (AddHandle(handle)) {
        socketList.insert(SocketList_t::value_type(sock, handle));
        return true;
      }

      delete handle;
      return false;
    }

    BOOL RemoveSocket(PSocketType * sock)
    { 
      PWaitAndSignal m(listMutex);

      typename SocketList_t::iterator r = socketList.find(sock);
      if (r == socketList.end()) 
        return FALSE;

      AggregatedPSocket * handle = r->second;
      RemoveHandle(handle);
      delete handle;
      socketList.erase(r);
      return TRUE;
    }
};

#endif
