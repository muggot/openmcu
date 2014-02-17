/*
 * thread.h
 *
 * Thread of execution control class.
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
 * Revision 1.39  2006/10/06 04:49:00  csoutheren
 * Fixed problem using PThread::Current in the destructor of autodelete threads
 *
 * Revision 1.38  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.37  2004/05/02 18:00:02  ykiryanov
 * Made unblock pipe public for BeOS
 *
 * Revision 1.36  2004/05/02 17:49:15  ykiryanov
 * Changed name of unblock pipe for BeOS to make compatible with socket code
 *
 * Revision 1.35  2004/04/18 00:27:32  ykiryanov
 * Added variable to count thread suspends. Removed thread name
 *
 * Revision 1.34  2004/04/03 15:50:30  ykiryanov
 * Changed names to easily distinguish class members. Added PThread name
 *
 * Revision 1.33  2004/04/02 03:42:07  ykiryanov
 * Added unblockPipe to improve code
 *
 * Revision 1.32  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.31  2002/11/02 00:32:21  robertj
 * Further fixes to VxWorks (Tornado) port, thanks Andreas Sikkema.
 *
 * Revision 1.30  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.29  2002/10/05 05:22:43  robertj
 * Fixed adding GetThreadId() function.
 *
 * Revision 1.28  2002/10/04 04:33:44  robertj
 * Added functions for getting operating system thread identifer values.
 *
 * Revision 1.27  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.26  2001/09/18 05:56:03  robertj
 * Fixed numerous problems with thread suspend/resume and signals handling.
 *
 * Revision 1.25  2001/09/10 03:02:41  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 * Changed threading so does not actually start thread until Resume(), makes
 *   the logic of start up much simpler and more portable.
 * Quite a bit of tidyin up of the pthreads code.
 *
 * Revision 1.24  2001/08/11 07:57:30  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.23  2001/07/09 04:26:08  yurik
 * Fixed lack of pthread_self function on BeOS
 *
 * Revision 1.22  2001/05/23 00:18:55  robertj
 * Added support for real time threads, thanks Erland Lewin.
 *
 * Revision 1.21  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.20  2001/03/20 06:44:25  robertj
 * Lots of changes to fix the problems with terminating threads that are I/O
 *   blocked, especially when doing orderly shutdown of service via SIGTERM.
 *
 * Revision 1.19  2001/02/25 19:40:35  rogerh
 * Add a suspend Semaphore for MAC OS threads started as 'suspended'
 *
 * Revision 1.18  2000/10/31 08:07:28  rogerh
 * Use proper return type for PX_GetThreadID
 *
 * Revision 1.17  2000/03/17 03:46:32  craigs
 * Removed spurious stuff for PThread implementation
 *
 * Revision 1.16  1999/10/30 13:45:02  craigs
 * Added pipe to thread to allow asynchronous abort of socket operations
 *
 * Revision 1.15  1999/09/03 02:26:25  robertj
 * Changes to aid in breaking I/O locks on thread termination. Still needs more work esp in BSD!
 *
 * Revision 1.14  1999/03/02 05:41:58  robertj
 * More BeOS changes
 *
 * Revision 1.13  1999/01/12 11:22:19  robertj
 * Removed redundent variable, is in common.
 *
 * Revision 1.12  1999/01/09 03:35:52  robertj
 * Fixed problem with closing thread waiting on semaphore.
 *
 * Revision 1.11  1998/11/30 22:07:23  robertj
 * New directory structure.
 *
 * Revision 1.10  1998/09/24 04:12:03  robertj
 * Added open software license.
 *
 * Revision 1.9  1998/01/03 23:06:32  craigs
 * Added PThread support
 *
 * Revision 1.8  1997/04/22 11:00:44  craigs
 * Added FreeStack function
 *
 * Revision 1.7  1996/12/30 03:23:52  robertj
 * Added timeout to block on child process function.
 *
 * Revision 1.6  1996/08/03 12:09:51  craigs
 * Changed for new common directories
 *
 * Revision 1.5  1996/04/15 10:50:48  craigs
 * Last revision prior to release of MibMaster
 *
 * Revision 1.4  1996/01/26 11:08:45  craigs
 * Fixed problem with blocking Accept calls
 *
 * Revision 1.3  1995/12/08 13:16:38  craigs
 * Added semaphore include and friend class
 *
 * Revision 1.2  1995/07/09 00:35:00  craigs
 * Latest and greatest omnibus change
 *
 * Revision 1.1  1995/01/23 18:43:27  craigs
 * Initial revision
 *
 * Revision 1.1  1994/04/12  08:31:05  robertj
 * Initial revision
 *
 */

///////////////////////////////////////////////////////////////////////////////
// PThread

  public:
    int PXBlockOnChildTerminate(int pid, const PTimeInterval & timeout);

    int PXBlockOnIO(int handle,
                    int type,
                   const PTimeInterval & timeout);

    void PXAbortBlock() const;

#ifdef P_PTHREADS

  public:
#ifndef P_HAS_SEMAPHORES
    void PXSetWaitingSemaphore(PSemaphore * sem);
#endif

  protected:
    static void * PX_ThreadStart(void *);
    static void PX_ThreadEnd(void *);

    PINDEX          PX_origStackSize;
    Priority        PX_priority;
    pthread_t       PX_threadId;
    pthread_mutex_t PX_suspendMutex;
    int             PX_suspendCount;
    BOOL            PX_firstTimeStart;
    BOOL            ending;

#ifndef P_HAS_SEMAPHORES
    PSemaphore    * PX_waitingSemaphore;
    pthread_mutex_t PX_WaitSemMutex;
#endif

    int unblockPipe[2];
    friend class PSocket;
    friend void PX_SuspendSignalHandler(int);

#elif defined(__BEOS__)

  protected:
    static int32 ThreadFunction(void * threadPtr);
    thread_id mId;
    int32 mPriority;
    PINDEX mStackSize;
    int32 mSuspendCount;
  public:
    int unblockPipe[2];

#elif defined(P_MAC_MPTHREADS)
  public:
    void PXSetWaitingSemaphore(PSemaphore * sem);
    //void InitialiseProcessThread();
    static long PX_ThreadStart(void *);
    static void PX_ThreadEnd(void *);
    MPTaskID    PX_GetThreadId() const;

  protected:
    void PX_NewThread(BOOL startSuspended);

    PINDEX     PX_origStackSize;
    int        PX_suspendCount;
    PSemaphore *suspend_semaphore;
    long       PX_signature;
    enum { kMPThreadSig = 'THRD', kMPDeadSig = 'DEAD'};

    MPTaskID   PX_threadId;
    MPSemaphoreID PX_suspendMutex;

    int unblockPipe[2];
    friend class PSocket;

#elif defined(VX_TASKS)
  public:
    SEM_ID syncPoint;
    static void Trace(PThreadIdentifer threadId = 0);

  private:
    static int ThreadFunction(void * threadPtr);
    long PX_threadId;
    int priority;
    PINDEX originalStackSize;

#endif


// End Of File ////////////////////////////////////////////////////////////////
