/*
 * tlibthrd.cxx
 *
 * Routines for pre-emptive threading system
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
 * $Log: tlibthrd.cxx,v $
 * Revision 1.175  2007/09/05 12:47:08  csoutheren
 * Fixes, fixes, fixes
 *
 * Revision 1.174  2007/09/05 12:40:11  csoutheren
 * Add generic test for mutex type
 *
 * Revision 1.173  2007/09/05 11:58:47  csoutheren
 * Fixed build on MacOSX
 *
 * Revision 1.170  2007/08/17 07:29:21  csoutheren
 * Fix build on MacOSX
 *
 * Revision 1.169  2007/08/17 07:05:13  csoutheren
 * Fix problem with false asserts based on mutex locking
 *
 * Revision 1.168  2007/08/17 05:29:19  csoutheren
 * Add field to Linux showing locking thread to assist in debugging
 *
 * Revision 1.167  2007/07/19 08:11:47  csoutheren
 * Fix problem with hex in log stream
 *
 * Revision 1.166  2007/07/06 02:12:14  csoutheren
 * Add extra memory leak debugging on Linux
 * Remove compile warnings
 *
 * Revision 1.165  2007/05/01 10:20:44  csoutheren
 * Applied 1703617 - Prevention of application deadlock caused by too many timers
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.164  2006/10/23 01:15:16  csoutheren
 * Revert to revision 1.153 to fix crash problem with SIP connections
 *
 * Revision 1.153  2006/06/25 21:46:38  dereksmithies
 * Thanks to Paul Nader for this fix which fixes thread cleanup
 * issues on SMP machines. Good find.
 *
 * Revision 1.152  2006/03/01 08:29:33  csoutheren
 * Applied patch #1439578 PTrace / PThread::PX_ThreadEnd deadlock fix
 * Thanks to Hannes Friederich
 *
 * Revision 1.151  2006/01/29 22:35:47  csoutheren
 * Added fix for thread termination problems on SMP machines
 * Thanks to Derek Smithies
 *
 * Revision 1.150  2005/12/05 22:35:24  csoutheren
 * Only assert in PTimedMutex destructor if _DEBUG is enabled
 *
 * Revision 1.149  2005/12/04 22:07:26  csoutheren
 * Fixed uninitialised variable
 *
 * Revision 1.148  2005/12/01 00:55:19  csoutheren
 * Removed chance of endless loop in PTimedMutex destructor
 *
 * Revision 1.147  2005/11/25 00:06:12  csoutheren
 * Applied patch #1364593 from Hannes Friederich
 * Also changed so PTimesMutex is no longer descended from PSemaphore on
 * non-Windows platforms
 *
 * Revision 1.146  2005/11/22 22:38:36  dsandras
 * Removed Assert that was causing problem if the mutex is locked when being
 * destroyed.
 *
 * Revision 1.145  2005/11/18 22:26:07  dsandras
 * Removed a few more CONST's to match with previous commit and fix permanent
 * deadlock.
 *
 * Revision 1.144  2005/11/14 22:41:53  csoutheren
 * Reverted Wait and Signal to non-const - there is no way we can guarantee that all
 * descendant classes everywhere will be changed over, so we have to keep the
 * original API
 *
 * Revision 1.143  2005/11/04 09:44:30  csoutheren
 * Applied patch #1324589
 * Removed race conditions in PSemaphore and thread handling
 * Thanks to Frederic Heem
 *
 * Revision 1.142  2005/11/04 06:56:10  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 * Revision 1.141  2005/07/22 04:19:18  csoutheren
 * Removed redundant check of thread ID introduced in last patch
 * Removed race condition in thread shutdown found by Derek Smithies
 *
 * Revision 1.140  2005/07/21 13:04:11  csoutheren
 * Removed race condition where activeThreads list does not contain
 * thread until some time after thread is started. Fixed by moving
 * list insertion to immediately after pthread_create, and using lock
 *
 * Revision 1.139  2005/07/21 00:09:08  csoutheren
 * Added workaround for braindead behaviour of pthread_kill
 * Thanks to "martin martin" <acevedoma@hotmail.com>
 *
 * Revision 1.138  2005/05/03 11:58:46  csoutheren
 * Fixed various problems reported by valgrind
 * Thanks to Paul Cadach
 *
 * Revision 1.137  2005/01/21 21:25:19  csoutheren
 * Removed incorrect return in PThread::WaitForTermination
 *
 * Revision 1.136  2005/01/16 23:00:37  csoutheren
 * Fixed problem when calling WaitForTermination from within the same thread
 *
 * Revision 1.135  2004/12/21 06:30:55  csoutheren
 * Added explicit stack size for pthreads to minimise VM usage, thanks to Diana Cionoiu
 *
 * Revision 1.134  2004/09/02 07:55:44  csoutheren
 * Added extra PXAbortBlock to WaitForTermination to assist in terminaing
 * threads under certain conditions
 *
 * Revision 1.133  2004/06/24 11:29:44  csoutheren
 * Changed to use pthread_mutex_timedlock for more efficient mutex wait operations
 * Thanks to Michal Zygmuntowicz
 *
 * Revision 1.132  2004/06/01 07:42:20  csoutheren
 * Restored memory allocation checking
 * Added configure flag to enable, thanks to Derek Smithies
 *
 * Revision 1.131  2004/05/21 00:49:16  csoutheren
 * Added PreShutdown to ~PProcess
 *
 * Revision 1.130  2004/04/27 04:36:47  rjongbloed
 * Fixed occassional crash on exit due to level 5 trace in PProcess
 *   destructor that needs an undestructed PProcess.
 * Added some more logging for unblocking threads.
 *
 * Revision 1.129  2004/04/12 03:35:27  csoutheren
 * Fixed problems with non-recursuve mutexes and critical sections on
 * older compilers and libc
 *
 * Revision 1.128  2004/04/12 00:58:45  csoutheren
 * Fixed PAtomicInteger on Linux, and modified PMutex to use it
 *
 * Revision 1.127  2004/04/11 07:58:08  csoutheren
 * Added configure.in check for recursive mutexes, and changed implementation
 * without recursive mutexes to use PCriticalSection or atomic word structs
 *
 * Revision 1.126  2004/03/24 02:37:04  csoutheren
 * Fixed problem with incorrect usage of sem_timedwait
 *
 * Revision 1.125  2004/03/23 04:56:23  csoutheren
 * Added patches to use XPG6 threading under Linux if available
 * Thanks to Matthew Hodgson
 *
 * Revision 1.124  2004/02/01 11:23:16  dsandras
 * Reverted previous Change and removed Yield call from Current (). Fix from Christian Meder <chris@onestepahead.de>. Thanks for your help, Christian!
 *
 * Revision 1.123  2004/01/31 13:49:18  dominance
 * Added 2.6 performance fix as proposed by Christian Meder <chris@onestepahead.de>.
 *
 * Revision 1.122  2003/09/17 09:02:15  csoutheren
 * Removed memory leak detection code
 *
 * Revision 1.121  2003/05/16 17:40:55  shawn
 * On Mac OS X, thread with the highest priority should use fixed priority
 * scheduling policy.  This avoids starvation caused by desktop activity.
 *
 * Revision 1.120  2003/05/02 00:58:40  dereks
 * Add test for linux at the end of PMutex::Signal. Thanks Robert!
 *
 * Revision 1.119  2003/05/02 00:39:11  dereks
 * Changes to make threading work on Redhat 9
 *
 * Revision 1.118  2003/04/24 12:03:13  rogerh
 * Calling pthread_mutex_unlock() on a mutex which is not locked can be
 * considered an error. NetBSD now enforce this error so we need to quickly
 * try locking the mutex before unlocking it in ~PThread and ~PSemaphore.
 *
 * Revision 1.117  2003/04/08 03:29:31  robertj
 * Fixed IsSuspeneded() so returns TRUE if thread not started yet, this makes
 *   it the same as the Win32 semantics.
 *
 * Revision 1.116  2003/03/10 15:37:00  rogerh
 * fix IsTerminated() function.
 *
 * Revision 1.115  2003/03/07 00:07:15  robertj
 * Fixed Mac OS X patch which broke every other platform.
 *
 * Revision 1.114  2003/03/06 08:58:48  rogerh
 * P_MACOSX now carries the OSRELEASE. Use this to enable better threads
 * support on Darwin 6.4. Submitted by Shawn.
 *
 * Revision 1.113  2003/02/20 23:32:00  robertj
 * More RTEMS support patches, thanks Sebastian Meyer.
 *
 * Revision 1.112  2003/01/24 10:21:06  robertj
 * Fixed issues in RTEMS support, thanks Vladimir Nesic
 *
 * Revision 1.111  2003/01/20 10:13:18  rogerh
 * NetBSD thread changes
 *
 * Revision 1.110  2003/01/20 10:05:46  rogerh
 * NetBSD thread changes
 *
 * Revision 1.109  2003/01/08 08:47:51  rogerh
 * Add new Sleep() function for GNU PTH threads.
 * Taken from NetBSD's package which uses PTH.
 * Note: I am not sure this works correctly.
 *
 * Revision 1.108  2003/01/06 18:49:15  rogerh
 * Back out pthead_kill to pthread_cancel change on NetBSD
 *
 * Revision 1.107  2002/12/11 05:39:26  robertj
 * Added logging for file handle changes.
 * Fixd bug where internal maxHandles not set when increased.
 *
 * Revision 1.106  2002/12/02 03:57:18  robertj
 * More RTEMS support patches, thank you Vladimir Nesic.
 *
 * Revision 1.105  2002/11/22 10:14:07  robertj
 * QNX port, thanks Xiaodan Tang
 *
 * Revision 1.104  2002/11/04 16:01:27  rogerh
 * Using pthread_cancel and not pthread_kill with SIGKILL to terminate a thread
 * On FreeBSD the thread does not have a handler for SIGKILL, it passes it up
 * to the main process which gets killed! Assume the other BSDs are the same.
 *
 * Revision 1.103  2002/10/24 00:40:56  robertj
 * Put back ability to terminate a thread from that threads context (removed
 *   in revision 1.101) but requires that destructor not do so.
 * Changed pipe close to allow for possible EINTR, and retry close.
 *
 * Revision 1.102  2002/10/24 00:25:13  robertj
 * Changed high load thread problem fix from the termination function to start
 *   function to finally, once and for all (I hope!) fix the race condition.
 *
 * Revision 1.101  2002/10/23 14:56:22  craigs
 * Fixed problem with pipe leak under some circumstances
 *
 * Revision 1.100  2002/10/23 04:29:32  robertj
 * Improved debugging for thread create/start/stop/destroy.
 * Fixed race condition bug if auto-delete thread starts and completes before
 *   pthread_create returns, PX_threadId is not set yet!
 *
 * Revision 1.99  2002/10/22 07:42:52  robertj
 * Added extra debugging for file handle and thread leak detection.
 *
 * Revision 1.98  2002/10/18 03:05:39  robertj
 * Fixed thread leak caused by fixing the thread crash a few revisions back,
 *   caused by strange pthreads behaviour, at least under Linux.
 *
 * Revision 1.97  2002/10/17 13:44:27  robertj
 * Port to RTEMS, thanks Vladimir Nesic.
 *
 * Revision 1.96  2002/10/17 12:57:24  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.95  2002/10/16 11:26:29  rogerh
 * Add missing include. Noticed by Matthias on the GnomeMeeting IRC
 *
 * Revision 1.94  2002/10/10 03:09:48  robertj
 * Fixed high load race condition when starting threads.
 *
 * Revision 1.93  2002/10/05 05:22:43  robertj
 * Fixed adding GetThreadId() function.
 *
 * Revision 1.92  2002/10/01 06:27:48  robertj
 * Added bullet proofing against possible EINTR error returns on all pthread
 *   functions when under heavy load. Linux really should NOT do this, but ...
 *
 * Revision 1.91  2002/09/04 03:14:18  robertj
 * Backed out changes submitted by Martin Froehlich as they do not appear to
 *   actually do anything other than add a sychronisation point. The variables
 *   the patches intended to protect were already protected.
 * Fixed bug where if a PMutex was signalled by a thread that did not have it
 *   locked, it would assert but continue to alter PMutex variables such that
 *   a deadlock or crash is likely.
 *
 * Revision 1.90  2002/08/29 01:50:40  robertj
 * Changed the pthread_create so does retries if get EINTR or EAGAIN errors
 *   which indicate a (possibly) temporary resource limit.
 * Enabled and adjusted tracing.
 *
 * Revision 1.89  2002/08/22 13:05:57  craigs
 * Fixed problems with mutex implementation thanks to Martin Froehlich
 *
 * Revision 1.88  2002/07/15 06:56:59  craigs
 * Fixed missing brace
 *
 * Revision 1.87  2002/07/15 06:39:23  craigs
 * Added function to allow raising of per-process file handle limit
 *
 * Revision 1.86  2002/06/27 06:38:58  robertj
 * Changes to remove memory leak display for things that aren't memory leaks.
 *
 * Revision 1.85  2002/06/27 02:04:01  robertj
 * Fixed NetBSD compatibility issue, thanks Motoyuki OHMORI.
 *
 * Revision 1.84  2002/06/04 00:25:31  robertj
 * Fixed incorrectly initialised trace indent, thanks Artis Kugevics
 *
 * Revision 1.83  2002/05/21 09:13:00  robertj
 * Fixed problem when using posix recursive mutexes, thanks Artis Kugevics
 *
 * Revision 1.82  2002/04/24 01:11:37  robertj
 * Fixed problem with PTRACE_BLOCK indent level being correct across threads.
 *
 * Revision 1.81  2002/04/16 10:57:26  rogerh
 * Change WaitForTermination() so it does not use 100% CPU.
 * Reported by Andrea <ghittino@tiscali.it>
 *
 * Revision 1.80  2002/01/23 04:26:36  craigs
 * Added copy constructors for PSemaphore, PMutex and PSyncPoint to allow
 * use of default copy constructors for objects containing instances of
 * these classes
 *
 * Revision 1.79  2002/01/10 06:36:58  robertj
 * Fixed possible resource leak under Solaris, thanks Joegen Baclor
 *
 * Revision 1.78  2001/12/17 11:06:46  robertj
 * Removed assert on NULL PThread::Current(), can occur if thread from other
 *   subsystem to pwlib
 *
 * Revision 1.77  2001/10/03 05:11:50  robertj
 * Fixed PSyncPoint wait with timeout when have pending signals.
 *
 * Revision 1.76  2001/09/27 23:50:03  craigs
 * Fixed typo in PSemaphone destructor
 *
 * Revision 1.75  2001/09/24 10:09:48  rogerh
 * Fix an uninitialised variable problem.
 *
 * Revision 1.74  2001/09/20 05:38:25  robertj
 * Changed PSyncPoint to use pthread cond so timed wait blocks properly.
 * Also prevented semaphore from being created if subclass does not use it.
 *
 * Revision 1.73  2001/09/19 17:37:47  craigs
 * Added support for nested mutexes under Linux
 *
 * Revision 1.72  2001/09/18 06:53:35  robertj
 * Made sure suspend can't exit early if get spurious signal
 *
 * Revision 1.71  2001/09/18 05:56:03  robertj
 * Fixed numerous problems with thread suspend/resume and signals handling.
 *
 * Revision 1.70  2001/09/10 03:03:02  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 * Changed threading so does not actually start thread until Resume(), makes
 *   the logic of start up much simpler and more portable.
 * Quite a bit of tidyin up of the pthreads code.
 *
 * Revision 1.69  2001/08/30 08:57:40  robertj
 * Changed calls to usleep to be PThread::Yield(), normalising single
 *   timeslice process swap out.
 *
 * Revision 1.68  2001/08/20 06:55:45  robertj
 * Fixed ability to have PMutex::Wait() with times less than one second.
 * Fixed small error in return value of block on I/O function, not critical.
 *
 * Revision 1.67  2001/08/07 02:50:03  craigs
 * Fixed potential race condition in IO blocking
 *
 * Revision 1.66  2001/07/09 13:23:37  rogerh
 * Fix a subtle bug in semaphore wait which showed up on FreeBSD
 *
 * Revision 1.65  2001/05/29 00:49:18  robertj
 * Added ability to put in a printf %x in thread name to get thread object
 *   address into user settable thread name.
 *
 * Revision 1.64  2001/05/23 00:18:55  robertj
 * Added support for real time threads, thanks Erland Lewin.
 *
 * Revision 1.63  2001/04/20 09:27:25  robertj
 * Fixed previous change for auto delete threads, must have thread ID zeroed.
 *
 * Revision 1.62  2001/04/20 09:09:05  craigs
 * Removed possible race condition whilst shutting down threads
 *
 * Revision 1.61  2001/03/20 06:44:25  robertj
 * Lots of changes to fix the problems with terminating threads that are I/O
 *   blocked, especially when doing orderly shutdown of service via SIGTERM.
 *
 * Revision 1.60  2001/03/14 01:16:11  robertj
 * Fixed signals processing, now uses housekeeping thread to handle signals
 *   synchronously. This also fixes issues with stopping PServiceProcess.
 *
 * Revision 1.59  2001/02/25 19:39:42  rogerh
 * Use a Semaphore on Mac OS X to support threads which are started as 'suspended'
 *
 * Revision 1.58  2001/02/24 14:49:22  rogerh
 * Add missing bracket
 *
 * Revision 1.57  2001/02/24 13:29:34  rogerh
 * Mac OS X change to avoid Assertion
 *
 * Revision 1.56  2001/02/24 13:24:24  rogerh
 * Add PThread support for Mac OS X and Darwin. There is one major issue. This
 * OS does not suport pthread_kill() and sigwait() so we cannot support the
 * Suspend() and Resume() functions to start and stop threads and we cannot
 * create new threads in 'suspended' mode.
 * Calling Suspend() raises an assertion. Calling Resume() does nothing.
 * Threads started in 'suspended' mode start immediatly.
 *
 * Revision 1.55  2001/02/21 22:48:42  robertj
 * Fixed incorrect test in PSemaphore::WillBlock() just added, thank Artis Kugevics.
 *
 * Revision 1.54  2001/02/20 00:21:14  robertj
 * Fixed major bug in PSemapahore::WillBlock(), thanks Tomas Heran.
 *
 * Revision 1.53  2000/12/21 12:36:32  craigs
 * Removed potential to stop threads twice
 *
 * Revision 1.52  2000/12/05 08:24:50  craigs
 * Fixed problem with EINTR causing havoc
 *
 * Revision 1.51  2000/11/16 11:06:38  rogerh
 * Add a better fix for the "user signal 2" aborts seen on FreeBSD 4.2 and above.
 * We need to sched_yeild() after the pthread_create() to make sure the new thread
 * actually has a chance to execute. The abort problem was caused when the
 * resume signal was issued before the thread was ready for it.
 *
 * Revision 1.50  2000/11/12 23:30:02  craigs
 * Added extra WaitForTermination to assist bug location
 *
 * Revision 1.49  2000/11/12 08:16:07  rogerh
 * This change and the previous change, make pthreads work on FreeBSD 4.2.
 * FreeBSD has improved its thread signal handling and now correctly generates a
 * SIGUSR2 signal on a thread (the Resume Signal).  However there was no handler
 * for this signal and applications would abort with "User signal 2".
 * So, a dummy sigResumeHandler has been added.
 *
 * Revision 1.48  2000/11/12 07:57:45  rogerh
 * *** empty log message ***
 *
 * Revision 1.47  2000/10/31 08:09:51  rogerh
 * Change return type of PX_GetThreadId() to save unnecessary typecasting
 *
 * Revision 1.46  2000/10/31 07:52:06  rogerh
 * Add type casts to allow the code to compile on FreeBSD 4.1.1
 *
 * Revision 1.45  2000/10/30 05:48:33  robertj
 * Added assert when get nested mutex.
 *
 * Revision 1.44  2000/10/24 03:32:40  robertj
 * Fixed problem where thread that uses PThread::Current() in dtor crashes.
 *
 * Revision 1.43  2000/10/20 06:11:48  robertj
 * Added function to change auto delete flag on a thread.
 *
 * Revision 1.42  2000/09/20 04:24:09  craigs
 * Added extra tracing, and removed segv on exit when using tracing
 *
 * Revision 1.41  2000/06/21 01:01:22  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.40  2000/04/13 07:21:10  rogerh
 * Fix typo in #defined
 *
 * Revision 1.39  2000/04/11 11:38:49  rogerh
 * More NetBSD Pthread changes
 *
 * Revision 1.38  2000/04/10 11:47:02  rogerh
 * Add initial NetBSD pthreads support
 *
 * Revision 1.37  2000/04/06 12:19:49  rogerh
 * Add Mac OS X support submitted by Kevin Packard
 *
 * Revision 1.36  2000/03/20 22:56:34  craigs
 * Fixed problems with race conditions caused by testing or changing
 * attributes on a terminated thread. Only occured on a fast machine!
 *
 * Revision 1.35  2000/03/17 03:45:40  craigs
 * Fixed problem with connect call hanging
 *
 * Revision 1.34  2000/03/08 12:17:09  rogerh
 * Add OpenBSD support
 *
 * Revision 1.33  2000/02/29 13:18:21  robertj
 * Added named threads to tracing, thanks to Dave Harvey
 *
 * Revision 1.32  2000/01/20 08:20:57  robertj
 * FreeBSD v3 compatibility changes, thanks Roger Hardiman & Motonori Shindo
 *
 * Revision 1.31  1999/11/18 14:02:57  craigs
 * Fixed problem with houskeeping thread termination
 *
 * Revision 1.30  1999/11/15 01:12:56  craigs
 * Fixed problem with PSemaphore::Wait consuming 100% CPU
 *
 * Revision 1.29  1999/10/30 13:44:11  craigs
 * Added correct method of aborting socket operations asynchronously
 *
 * Revision 1.28  1999/10/24 13:03:30  craigs
 * Changed to capture io break signal
 *
 * Revision 1.27  1999/09/23 06:52:16  robertj
 * Changed PSemaphore to use Posix semaphores.
 *
 * Revision 1.26  1999/09/03 02:26:25  robertj
 * Changes to aid in breaking I/O locks on thread termination. Still needs more work esp in BSD!
 *
 * Revision 1.25  1999/09/02 11:56:35  robertj
 * Fixed problem with destroying PMutex that is already locked.
 *
 * Revision 1.24  1999/08/24 13:40:56  craigs
 * Fixed problem with condwait destorys failing on linux
 *
 * Revision 1.23  1999/08/23 05:33:45  robertj
 * Made last threading changes Linux only.
 *
 * Revision 1.22  1999/08/23 05:14:13  robertj
 * Removed blocking of interrupt signals as does not work in Linux threads.
 *
 * Revision 1.21  1999/07/30 00:40:32  robertj
 * Fixed problem with signal variable in non-Linux platforms
 *
 * Revision 1.20  1999/07/19 01:32:24  craigs
 * Changed signals used in pthreads code, is used by linux version.
 *
 * Revision 1.19  1999/07/15 13:10:55  craigs
 * Fixed problem with EINTR in nontimed sempahore waits
 *
 * Revision 1.18  1999/07/15 13:05:33  robertj
 * Fixed problem with getting EINTR in semaphore wait, is normal, not error.
 *
 * Revision 1.17  1999/07/11 13:42:13  craigs
 * pthreads support for Linux
 *
 * Revision 1.16  1999/05/12 03:29:20  robertj
 * Fixed problem with semaphore free, done at wrong time.
 *
 * Revision 1.15  1999/04/29 08:41:26  robertj
 * Fixed problems with uninitialised mutexes in PProcess.
 *
 * Revision 1.14  1999/03/16 10:54:16  robertj
 * Added parameterless version of WaitForTermination.
 *
 * Revision 1.13  1999/03/16 10:30:37  robertj
 * Added missing PThread::WaitForTermination function.
 *
 * Revision 1.12  1999/01/12 12:09:51  robertj
 * Removed redundent member variable, was in common.
 * Fixed BSD threads compatibility.
 *
 * Revision 1.11  1999/01/11 12:05:56  robertj
 * Fixed some more race conditions in threads.
 *
 * Revision 1.10  1999/01/11 03:42:26  robertj
 * Fixed problem with destroying thread automatically.
 *
 * Revision 1.9  1999/01/09 03:37:28  robertj
 * Fixed problem with closing thread waiting on semaphore.
 * Improved efficiency of mutex to use pthread functions directly.
 *
 * Revision 1.8  1999/01/08 01:31:03  robertj
 * Support for pthreads under FreeBSD
 *
 * Revision 1.7  1998/12/15 12:41:07  robertj
 * Fixed signal handling so can now ^C a pthread version.
 *
 * Revision 1.6  1998/11/05 09:45:04  robertj
 * Removed StartImmediate option in thread construction.
 *
 * Revision 1.5  1998/09/24 04:12:25  robertj
 * Added open software license.
 *
 */

#include <ptlib/socket.h>
#include <sched.h>  // for sched_yield
#include <pthread.h>
#include <sys/resource.h>

#ifdef P_RTEMS
#define SUSPEND_SIG SIGALRM
#include <sched.h>
#else
#define SUSPEND_SIG SIGVTALRM
#endif

#ifdef P_MACOSX
#include <mach/mach.h>
#include <mach/thread_policy.h>
#include <sys/param.h>
#include <sys/sysctl.h>
// going to need the main thread for adjusting relative priority
static pthread_t baseThread;
#endif

#ifdef P_HAS_SEMAPHORES_XPG6
#include "semaphore.h"
#endif

int PX_NewHandle(const char *, int);

#define PPThreadKill(id, sig)  PProcess::Current().PThreadKill(id, sig)


#define PAssertPTHREAD(func, args) \
  { \
    unsigned threadOpRetry = 0; \
    while (PAssertThreadOp(func args, threadOpRetry, #func, __FILE__, __LINE__)); \
  }

static BOOL PAssertThreadOp(int retval,
                            unsigned & retry,
                            const char * funcname,
                            const char * file,
                            unsigned line)
{
  if (retval == 0) {
    PTRACE_IF(2, retry > 0, "PWLib\t" << funcname << " required " << retry << " retries!");
    return FALSE;
  }

  if (errno == EINTR || errno == EAGAIN) {
    if (++retry < 1000) {
#if defined(P_RTEMS)
      sched_yield();
#else
      usleep(10000); // Basically just swap out thread to try and clear blockage
#endif
      return TRUE;   // Return value to try again
    }
    // Give up and assert
  }

  PAssertFunc(file, line, NULL, psprintf("Function %s failed", funcname));
  return FALSE;
}


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


static pthread_mutex_t MutexInitialiser = PTHREAD_MUTEX_INITIALIZER;


#define new PNEW


void PHouseKeepingThread::Main()
{
  PProcess & process = PProcess::Current();

  while (!closing) {
    PTimeInterval delay = process.timers.Process();

    process.breakBlock.Wait(delay);

    process.PXCheckSignals();
  }
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

  breakBlock.Signal();
}


void PProcess::Construct()
{
#ifndef P_RTEMS
  // get the file descriptor limit
  struct rlimit rl;
  PAssertOS(getrlimit(RLIMIT_NOFILE, &rl) == 0);
  maxHandles = rl.rlim_cur;
  PTRACE(4, "PWLib\tMaximum per-process file handles is " << maxHandles);
#else
  maxHandles = 500; // arbitrary value
#endif

  // initialise the housekeeping thread
  housekeepingThread = NULL;

#ifdef P_MACOSX
    // records the main thread for priority adjusting
    baseThread = pthread_self();
#endif

  CommonConstruct();
}


BOOL PProcess::SetMaxHandles(int newMax)
{
#ifndef P_RTEMS
  // get the current process limit
  struct rlimit rl;
  PAssertOS(getrlimit(RLIMIT_NOFILE, &rl) == 0);

  // set the new current limit
  rl.rlim_cur = newMax;
  if (setrlimit(RLIMIT_NOFILE, &rl) == 0) {
    PAssertOS(getrlimit(RLIMIT_NOFILE, &rl) == 0);
    maxHandles = rl.rlim_cur;
    if (maxHandles == newMax) {
      PTRACE(2, "PWLib\tNew maximum per-process file handles set to " << maxHandles);
      return TRUE;
    }
  }
#endif // !P_RTEMS

  PTRACE(1, "PWLib\tCannot set per-process file handle limit to "
         << newMax << " (is " << maxHandles << ") - check permissions");
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

  PTRACE(5, "PWLib\tDestroyed process " << this);
}

BOOL PProcess::PThreadKill(pthread_t id, unsigned sig)
{
  PWaitAndSignal m(threadMutex);

# if defined (__FreeBSD__) && defined (P_64BIT) 
    if (!activeThreads.Contains((unsigned long)id)) return FALSE;
# else
    if (!activeThreads.Contains((unsigned)id)) return FALSE;
# endif

  return pthread_kill(id, sig) == 0;
}


//////////////////////////////////////////////////////////////////////////////

PThread::PThread()
{
  // see InitialiseProcessThread()
}


void PThread::InitialiseProcessThread()
{
  autoDelete          = FALSE;

  PX_origStackSize    = 0;
  PX_threadId         = pthread_self();
  PX_priority         = NormalPriority;
  PX_suspendCount     = 0;

#ifndef P_HAS_SEMAPHORES
  PX_waitingSemaphore = NULL;
  PX_WaitSemMutex = MutexInitialiser;
#endif

  PX_suspendMutex = MutexInitialiser;

#ifdef P_RTEMS
  PAssertOS(socketpair(AF_INET,SOCK_STREAM,0,unblockPipe) == 0);
#else
  PAssertOS(::pipe(unblockPipe) == 0);
#endif

  ((PProcess *)this)->activeThreads.DisallowDeleteObjects();
# if defined (__FreeBSD__) && defined (P_64BIT) 
  ((PProcess *)this)->activeThreads.SetAt((unsigned long)PX_threadId, this);
# else
  ((PProcess *)this)->activeThreads.SetAt((unsigned)PX_threadId, this);
# endif

  PX_firstTimeStart = FALSE;

  traceBlockIndentLevel = 0;
}


PThread::PThread(PINDEX stackSize,
                 AutoDeleteFlag deletion,
                 Priority priorityLevel,
                 const PString & name)
  : threadName(name)
{
  autoDelete = (deletion == AutoDeleteThread);

  PAssert(stackSize > 0, PInvalidParameter);
  PX_origStackSize = stackSize;
  PX_threadId = 0;
  PX_priority = priorityLevel;
  PX_suspendCount = 1;

#ifndef P_HAS_SEMAPHORES
  PX_waitingSemaphore = NULL;
  PX_WaitSemMutex = MutexInitialiser;
#endif

  PX_suspendMutex = MutexInitialiser;

#ifdef P_RTEMS
  PAssertOS(socketpair(AF_INET,SOCK_STREAM,0,unblockPipe) == 0);
#else
  PAssertOS(::pipe(unblockPipe) == 0);
#endif
  PX_NewHandle("Thread unblock pipe", PMAX(unblockPipe[0], unblockPipe[1]));

  // new thread is actually started the first time Resume() is called.
  PX_firstTimeStart = TRUE;

  traceBlockIndentLevel = 0;

  PTRACE(5, "PWLib\tCreated thread " << this << ' ' << threadName);
}


PThread::~PThread()
{
  if (PX_threadId != 0 && PX_threadId != pthread_self())
    Terminate();

  PAssertPTHREAD(::close, (unblockPipe[0]));
  PAssertPTHREAD(::close, (unblockPipe[1]));

#ifndef P_HAS_SEMAPHORES
  pthread_mutex_destroy(&PX_WaitSemMutex);
#endif

  // If the mutex was not locked, the unlock will fail */
  pthread_mutex_trylock(&PX_suspendMutex);
  pthread_mutex_unlock(&PX_suspendMutex);
  pthread_mutex_destroy(&PX_suspendMutex);

  if (this != &PProcess::Current())
    PTRACE(1, "PWLib\tDestroyed thread " << this << ' ' << threadName << "(id = " << ::hex << PX_threadId << ::dec << ")");
  else
    PProcessInstance = NULL;
}


void PThread::Restart()
{
  if (!IsTerminated())
    return;

  pthread_attr_t threadAttr;
  pthread_attr_init(&threadAttr);
  pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

#if defined(P_LINUX)

  // Set a decent (256K) stack size that won't eat all virtual memory
  pthread_attr_setstacksize(&threadAttr, 16*PTHREAD_STACK_MIN);

  /*
    Set realtime scheduling if our effective user id is root (only then is this
    allowed) AND our priority is Highest.
      As far as I can see, we could use either SCHED_FIFO or SCHED_RR here, it
    doesn't matter.
      I don't know if other UNIX OSs have SCHED_FIFO and SCHED_RR as well.

    WARNING: a misbehaving thread (one that never blocks) started with Highest
    priority can hang the entire machine. That is why root permission is 
    neccessary.
  */
  if ((geteuid() == 0) && (PX_priority == HighestPriority))
    PAssertPTHREAD(pthread_attr_setschedpolicy, (&threadAttr, SCHED_FIFO));
#elif defined(P_RTEMS)
  pthread_attr_setstacksize(&threadAttr, 2*PTHREAD_MINIMUM_STACK_SIZE);
  pthread_attr_setinheritsched(&threadAttr, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(&threadAttr, SCHED_OTHER);
  struct sched_param sched_param;
  sched_param.sched_priority = 125; /* set medium priority */
  pthread_attr_setschedparam(&threadAttr, &sched_param);
#endif

  PProcess & process = PProcess::Current();
  PINDEX newHighWaterMark = 0;
  static PINDEX highWaterMark = 0;

  // lock the thread list
  process.threadMutex.Wait();

  // create the thread
  PAssertPTHREAD(pthread_create, (&PX_threadId, &threadAttr, PX_ThreadStart, this));

  // put the thread into the thread list
# if defined (__FreeBSD__) && defined (P_64BIT) 
  process.activeThreads.SetAt((unsigned long)PX_threadId, this);
# else
  process.activeThreads.SetAt((unsigned)PX_threadId, this);
# endif
  if (process.activeThreads.GetSize() > highWaterMark)
    newHighWaterMark = highWaterMark = process.activeThreads.GetSize();

  // unlock the thread list
  process.threadMutex.Signal();

  PTRACE_IF(4, newHighWaterMark > 0, "PWLib\tThread high water mark set: " << newHighWaterMark);

#ifdef P_MACOSX
  if (PX_priority == HighestPriority) {
    PTRACE(1, "set thread to have the highest priority (MACOSX)");
    SetPriority(HighestPriority);
  }
#endif
}


void PX_SuspendSignalHandler(int)
{
  PThread * thread = PThread::Current();
  if (thread == NULL)
    return;

  BOOL notResumed = TRUE;
  while (notResumed) {
    BYTE ch;
    notResumed = ::read(thread->unblockPipe[0], &ch, 1) < 0 && errno == EINTR;
#if !( defined(P_NETBSD) && defined(P_NO_CANCEL) )
    pthread_testcancel();
#endif
  }
}


void PThread::Suspend(BOOL susp)
{
  PAssertPTHREAD(pthread_mutex_lock, (&PX_suspendMutex));

  // Check for start up condition, first time Resume() is called
  if (PX_firstTimeStart) {
    if (susp)
      PX_suspendCount++;
    else {
      if (PX_suspendCount > 0)
        PX_suspendCount--;
      if (PX_suspendCount == 0) {
        PX_firstTimeStart = FALSE;
        Restart();
      }
    }

    PAssertPTHREAD(pthread_mutex_unlock, (&PX_suspendMutex));
    return;
  }

#if defined(P_MACOSX) && (P_MACOSX <= 55)
  // Suspend - warn the user with an Assertion
  PAssertAlways("Cannot suspend threads on Mac OS X due to lack of pthread_kill()");
#else
  if (PPThreadKill(PX_threadId, 0)) {

    // if suspending, then see if already suspended
    if (susp) {
      PX_suspendCount++;
      if (PX_suspendCount == 1) {
        if (PX_threadId != pthread_self()) {
          signal(SUSPEND_SIG, PX_SuspendSignalHandler);
          PPThreadKill(PX_threadId, SUSPEND_SIG);
        }
        else {
          PAssertPTHREAD(pthread_mutex_unlock, (&PX_suspendMutex));
          PX_SuspendSignalHandler(SUSPEND_SIG);
          return;  // Mutex already unlocked
        }
      }
    }

    // if resuming, then see if to really resume
    else if (PX_suspendCount > 0) {
      PX_suspendCount--;
      if (PX_suspendCount == 0) 
        PXAbortBlock();
    }
  }

  PAssertPTHREAD(pthread_mutex_unlock, (&PX_suspendMutex));
#endif // P_MACOSX
}


void PThread::Resume()
{
  Suspend(FALSE);
}


BOOL PThread::IsSuspended() const
{
  if (PX_firstTimeStart)
    return TRUE;

  if (IsTerminated())
    return FALSE;

  PAssertPTHREAD(pthread_mutex_lock, ((pthread_mutex_t *)&PX_suspendMutex));
  BOOL suspended = PX_suspendCount != 0;
  PAssertPTHREAD(pthread_mutex_unlock, ((pthread_mutex_t *)&PX_suspendMutex));
  return suspended;
}


void PThread::SetAutoDelete(AutoDeleteFlag deletion)
{
  PAssert(deletion != AutoDeleteThread || this != &PProcess::Current(), PLogicError);
  autoDelete = deletion == AutoDeleteThread;
}

#ifdef P_MACOSX
// obtain thread priority of the main thread
static unsigned long
GetThreadBasePriority ()
{
    thread_basic_info_data_t threadInfo;
    policy_info_data_t       thePolicyInfo;
    unsigned int             count;

    if (baseThread == 0) {
      return 0;
    }

    // get basic info
    count = THREAD_BASIC_INFO_COUNT;
    thread_info (pthread_mach_thread_np (baseThread), THREAD_BASIC_INFO,
                 (integer_t*)&threadInfo, &count);

    switch (threadInfo.policy) {
    case POLICY_TIMESHARE:
      count = POLICY_TIMESHARE_INFO_COUNT;
      thread_info(pthread_mach_thread_np (baseThread),
                  THREAD_SCHED_TIMESHARE_INFO,
                  (integer_t*)&(thePolicyInfo.ts), &count);
      return thePolicyInfo.ts.base_priority;

    case POLICY_FIFO:
      count = POLICY_FIFO_INFO_COUNT;
      thread_info(pthread_mach_thread_np (baseThread),
                  THREAD_SCHED_FIFO_INFO,
                  (integer_t*)&(thePolicyInfo.fifo), &count);
      if (thePolicyInfo.fifo.depressed) 
        return thePolicyInfo.fifo.depress_priority;
      return thePolicyInfo.fifo.base_priority;

    case POLICY_RR:
      count = POLICY_RR_INFO_COUNT;
      thread_info(pthread_mach_thread_np (baseThread),
                  THREAD_SCHED_RR_INFO,
                  (integer_t*)&(thePolicyInfo.rr), &count);
      if (thePolicyInfo.rr.depressed) 
        return thePolicyInfo.rr.depress_priority;
      return thePolicyInfo.rr.base_priority;
    }

    return 0;
}
#endif

void PThread::SetPriority(Priority priorityLevel)
{
  PX_priority = priorityLevel;

#if defined(P_LINUX)
  if (IsTerminated())
    return;

  struct sched_param sched_param;
  
  if ((priorityLevel == HighestPriority) && (geteuid() == 0) ) {
    sched_param.sched_priority = sched_get_priority_min( SCHED_FIFO );
    
    PAssertPTHREAD(pthread_setschedparam, (PX_threadId, SCHED_FIFO, &sched_param));
  }
  else if (priorityLevel != HighestPriority) {
    /* priority 0 is the only permitted value for the SCHED_OTHER scheduler */ 
    sched_param.sched_priority = 0;
    
    PAssertPTHREAD(pthread_setschedparam, (PX_threadId, SCHED_OTHER, &sched_param));
  }
#endif

#if defined(P_MACOSX)
  if (IsTerminated())
    return;

  if (priorityLevel == HighestPriority) {
    /* get fixed priority */
    {
      int result;

      thread_extended_policy_data_t   theFixedPolicy;
      thread_precedence_policy_data_t thePrecedencePolicy;
      long                            relativePriority;

      theFixedPolicy.timeshare = false; // set to true for a non-fixed thread
      result = thread_policy_set (pthread_mach_thread_np(PX_threadId),
                                  THREAD_EXTENDED_POLICY,
                                  (thread_policy_t)&theFixedPolicy,
                                  THREAD_EXTENDED_POLICY_COUNT);
      if (result != KERN_SUCCESS) {
        PTRACE(1, "thread_policy - Couldn't set thread as fixed priority.");
      }

      // set priority

      // precedency policy's "importance" value is relative to
      // spawning thread's priority
      
      relativePriority = 62 - GetThreadBasePriority();
      PTRACE(1,  "relativePriority is " << relativePriority << " base priority is " << GetThreadBasePriority());
      
      thePrecedencePolicy.importance = relativePriority;
      result = thread_policy_set (pthread_mach_thread_np(PX_threadId),
                                  THREAD_PRECEDENCE_POLICY,
                                  (thread_policy_t)&thePrecedencePolicy, 
                                  THREAD_PRECEDENCE_POLICY_COUNT);
      if (result != KERN_SUCCESS) {
        PTRACE(1, "thread_policy - Couldn't set thread priority.");
      }
    }
  }
#endif
}


PThread::Priority PThread::GetPriority() const
{
#if defined(LINUX)
  int schedulingPolicy;
  struct sched_param schedParams;
  
  PAssertPTHREAD(pthread_getschedparam, (PX_threadId, &schedulingPolicy, &schedParams));
  
  switch( schedulingPolicy )
  {
    case SCHED_OTHER:
      break;
      
    case SCHED_FIFO:
    case SCHED_RR:
      return HighestPriority;
      
    default:
      /* Unknown scheduler. We don't know what priority this thread has. */
      PTRACE(1, "PWLib\tPThread::GetPriority: unknown scheduling policy #" << schedulingPolicy);
  }
#endif

  return NormalPriority; /* as good a guess as any */
}


#ifndef P_HAS_SEMAPHORES
void PThread::PXSetWaitingSemaphore(PSemaphore * sem)
{
  PAssertPTHREAD(pthread_mutex_lock, (&PX_WaitSemMutex));
  PX_waitingSemaphore = sem;
  PAssertPTHREAD(pthread_mutex_unlock, (&PX_WaitSemMutex));
}
#endif


#ifdef P_GNU_PTH
// GNU PTH threads version (used by NetBSD)
// Taken from NetBSD pkg patches
void PThread::Sleep(const PTimeInterval & timeout)
{
  PTime lastTime;
  PTime targetTime = PTime() + timeout;

  sched_yield();
  lastTime = PTime();

  while (lastTime < targetTime) {
    P_timeval tval = targetTime - lastTime;
    if (select(0, NULL, NULL, NULL, tval) < 0 && errno != EINTR)
      break;

    pthread_testcancel();

    lastTime = PTime();
  }
}

#else
// Normal Posix threads version
void PThread::Sleep(const PTimeInterval & timeout)
{
  PTime lastTime;
  PTime targetTime = lastTime + timeout;
  do {
    P_timeval tval = targetTime - lastTime;
    if (select(0, NULL, NULL, NULL, tval) < 0 && errno != EINTR)
      break;

#if !( defined(P_NETBSD) && defined(P_NO_CANCEL) )
    pthread_testcancel();
#endif

    lastTime = PTime();
  } while (lastTime < targetTime);
}
#endif

void PThread::Yield()
{
  sched_yield();
}


PThread * PThread::Current()
{
  PProcess & process = PProcess::Current();
  process.threadMutex.Wait();
# if defined (__FreeBSD__) && defined (P_64BIT) 
  PThread * thread = process.activeThreads.GetAt((unsigned long)pthread_self());
# else
  PThread * thread = process.activeThreads.GetAt((unsigned)pthread_self());
# endif
  process.threadMutex.Signal();
  return thread;
}


void PThread::Terminate()
{
  if (PX_origStackSize <= 0)
    return;

  // don't use PThread::Current, as the thread may already not be in the
  // active threads list
  if (PX_threadId == pthread_self()) {
    pthread_exit(0);
    return;
  }

  if (IsTerminated())
    return;

  PTRACE(2, "PWLib\tForcing termination of thread " << (void *)this);

  PXAbortBlock();
  WaitForTermination(20);

#if !defined(P_HAS_SEMAPHORES) && !defined(P_HAS_NAMED_SEMAPHORES)
  PAssertPTHREAD(pthread_mutex_lock, (&PX_WaitSemMutex));
  if (PX_waitingSemaphore != NULL) {
    PAssertPTHREAD(pthread_mutex_lock, (&PX_waitingSemaphore->mutex));
    PX_waitingSemaphore->queuedLocks--;
    PAssertPTHREAD(pthread_mutex_unlock, (&PX_waitingSemaphore->mutex));
    PX_waitingSemaphore = NULL;
  }
  PAssertPTHREAD(pthread_mutex_unlock, (&PX_WaitSemMutex));
#endif

#if ( defined(P_NETBSD) && defined(P_NO_CANCEL) )
  PPThreadKill(PX_threadId,SIGKILL);
#else
  if (PX_threadId) {
    pthread_cancel(PX_threadId);
  }
#endif
}


BOOL PThread::IsTerminated() const
{
  pthread_t id = PX_threadId;
  return (id == 0) || !PPThreadKill(id, 0);
}


void PThread::WaitForTermination() const
{
  if (this == Current()) {
    PTRACE(2, "WaitForTermination short circuited");
    return;
  }
  
  PXAbortBlock();   // this assist in clean shutdowns on some systems

  while (!IsTerminated()) {
    Sleep(10); // sleep for 10ms. This slows down the busy loop removing 100%
               // CPU usage and also yeilds so other threads can run.
  } 
}


BOOL PThread::WaitForTermination(const PTimeInterval & maxWait) const
{
  if (this == Current()) {
    PTRACE(2, "WaitForTermination(t) short circuited");
    return TRUE;
  }
  
  PTRACE(6, "PWLib\tWaitForTermination(" << maxWait << ')');

  PXAbortBlock();   // this assist in clean shutdowns on some systems
  PTimer timeout = maxWait;
  while (!IsTerminated()) {
    if (timeout == 0)
      return FALSE;
    Sleep(10); // sleep for 10ms. This slows down the busy loop removing 100%
               // CPU usage and also yeilds so other threads can run.
  }
  return TRUE;
}


void * PThread::PX_ThreadStart(void * arg)
{ 
  PThread * thread = (PThread *)arg;
  //don't need to detach the the thread, it was created in the PTHREAD_CREATE_DETACHED state
  // Added this to guarantee that the thread creation (PThread::Restart)
  // has completed before we start the thread. Then the PX_threadId has
  // been set.
  pthread_mutex_lock(&thread->PX_suspendMutex);
  thread->SetThreadName(thread->GetThreadName());
  pthread_mutex_unlock(&thread->PX_suspendMutex);

  // make sure the cleanup routine is called when the thread exits
  pthread_cleanup_push(&PThread::PX_ThreadEnd, arg);

  PTRACE(5, "PWLib\tStarted thread " << thread << ' ' << thread->threadName);

  // now call the the thread main routine
  thread->Main();

  // execute the cleanup routine
  pthread_cleanup_pop(1);

  return NULL;
}


void PThread::PX_ThreadEnd(void * arg)
{
  PProcess & process = PProcess::Current();
  process.threadMutex.Wait();

  PThread * thread = (PThread *)arg;
  pthread_t id = thread->GetThreadId();
  if (id == 0) {
    // Don't know why, but pthreads under Linux at least can call this function
    // multiple times! Probably a bug, but we have to allow for it.
    process.threadMutex.Signal();
    PTRACE(2, "PWLib\tAttempted to multiply end thread " << thread << " ThreadID=" << (void *)id);
    return;
  }  

 // remove this thread from the active thread list
# if defined (__FreeBSD__) && defined (P_64BIT) 
  process.activeThreads.SetAt((unsigned long)id, NULL);
# else
  process.activeThreads.SetAt((unsigned)id, NULL);
# endif

  // delete the thread if required, note this is done this way to avoid
  // a race condition, the thread ID cannot be zeroed before the if!
  PString threadName = thread->threadName;
  if (thread->autoDelete) {
    thread->PX_threadId = 0;  // Prevent terminating terminated thread
    process.threadMutex.Signal();
    PTRACE(5, "PWLib\tEnded thread " << thread << ' ' << threadName);

    /* It is now safe to delete this thread. Note that this thread
       is deleted after the process.threadMutex.Signal(), which means
       PWaitAndSignal(process.threadMutex) could not be used */
    delete thread;
  }
  else {
    thread->PX_threadId = 0;
    process.threadMutex.Signal();
    PTRACE(5, "PWLib\tEnded thread " << thread << ' ' << threadName);
  }
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
    PTRACE(6, "PWLib\tUnblocked I/O fd=" << unblockPipe[0]);
  }

  return retval;
}

void PThread::PXAbortBlock() const
{
  static BYTE ch = 0;
  ::write(unblockPipe[1], &ch, 1);
  PTRACE(6, "PWLib\tUnblocking I/O fd=" << unblockPipe[0] << " thread=" << GetThreadName());
}


///////////////////////////////////////////////////////////////////////////////

PSemaphore::PSemaphore(PXClass pxc)
{
  pxClass = pxc;

  // these should never be used, as this constructor is
  // only used for PMutex and PSyncPoint and they have their
  // own copy constructors
  
  initialVar = maxCountVar = 0;
  
  if(pxClass == PXSemaphore) {
#if defined(P_HAS_SEMAPHORES)
    /* call sem_init, otherwise sem_destroy fails*/
    PAssertPTHREAD(sem_init, (&semId, 0, 0));
#elif defined(P_HAS_NAMED_SEMAPHORES)
    semId = CreateSem(0);
#else
    currentCount = maximumCount = 0;
    queuedLocks = 0;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condVar, NULL);
#endif
  }
}


PSemaphore::PSemaphore(unsigned initial, unsigned maxCount)
{
  pxClass = PXSemaphore;

  initialVar  = initial;
  maxCountVar = maxCount;

#if defined(P_HAS_SEMAPHORES)
  PAssertPTHREAD(sem_init, (&semId, 0, initial));
#elif defined(P_HAS_NAMED_SEMAPHORES)
  semId = CreateSem(initialVar);
#else
  PAssertPTHREAD(pthread_mutex_init, (&mutex, NULL));
  PAssertPTHREAD(pthread_cond_init, (&condVar, NULL));
  
  PAssert(maxCount > 0, "Invalid semaphore maximum.");
  if (initial > maxCount)
    initial = maxCount;

  currentCount = initial;
  maximumCount = maxCount;
  queuedLocks  = 0;
#endif
}


PSemaphore::PSemaphore(const PSemaphore & sem) 
{
  pxClass = sem.GetSemClass();

  initialVar  = sem.GetInitial();
  maxCountVar = sem.GetMaxCount();

  if(pxClass == PXSemaphore) {
#if defined(P_HAS_SEMAPHORES)
    PAssertPTHREAD(sem_init, (&semId, 0, initialVar));
#elif defined(P_HAS_NAMED_SEMAPHORES)
    semId = CreateSem(initialVar);
#else
    PAssertPTHREAD(pthread_mutex_init, (&mutex, NULL));
    PAssertPTHREAD(pthread_cond_init, (&condVar, NULL));
  
    PAssert(maxCountVar > 0, "Invalid semaphore maximum.");
    if (initialVar > maxCountVar)
      initialVar = maxCountVar;

    currentCount = initialVar;
    maximumCount = maxCountVar;
    queuedLocks  = 0;
#endif
  }
}

PSemaphore::~PSemaphore()
{
  if(pxClass == PXSemaphore) {
#if defined(P_HAS_SEMAPHORES)
    PAssertPTHREAD(sem_destroy, (&semId));
#elif defined(P_HAS_NAMED_SEMAPHORES)
    PAssertPTHREAD(sem_close, (semId));
#else
    PAssert(queuedLocks == 0, "Semaphore destroyed with queued locks");
    PAssertPTHREAD(pthread_mutex_destroy, (&mutex));
    PAssertPTHREAD(pthread_cond_destroy, (&condVar));
#endif
  }
}

#if defined(P_HAS_NAMED_SEMAPHORES)
sem_t * PSemaphore::CreateSem(unsigned initialValue)
{
  sem_t *sem;

  // Since sem_open and sem_unlink are two operations, there is a small
  // window of opportunity that two simultaneous accesses may return
  // the same semaphore. Therefore, the static mutex is used to
  // prevent this, even if the chances are small
  static pthread_mutex_t semCreationMutex = PTHREAD_MUTEX_INITIALIZER;
  PAssertPTHREAD(pthread_mutex_lock, (&semCreationMutex));
  
  sem_unlink("/pwlib_sem");
  sem = sem_open("/pwlib_sem", (O_CREAT | O_EXCL), 700, initialValue);
  
  PAssertPTHREAD(pthread_mutex_unlock, (&semCreationMutex));
  
  PAssert(((int)sem != SEM_FAILED), "Couldn't create named semaphore");
  return sem;
}
#endif

void PSemaphore::Wait() 
{
#if defined(P_HAS_SEMAPHORES)
  PAssertPTHREAD(sem_wait, (&semId));
#elif defined(P_HAS_NAMED_SEMAPHORES)
  PAssertPTHREAD(sem_wait, (semId));
#else
  PAssertPTHREAD(pthread_mutex_lock, (&mutex));

  queuedLocks++;
  PThread::Current()->PXSetWaitingSemaphore(this);

  while (currentCount == 0) {
    int err = pthread_cond_wait(&condVar, &mutex);
    PAssert(err == 0 || err == EINTR, psprintf("wait error = %i", err));
  }

  PThread::Current()->PXSetWaitingSemaphore(NULL);
  queuedLocks--;

  currentCount--;

  PAssertPTHREAD(pthread_mutex_unlock, (&mutex));
#endif
}


BOOL PSemaphore::Wait(const PTimeInterval & waitTime) 
{
  if (waitTime == PMaxTimeInterval) {
    Wait();
    return TRUE;
  }

  // create absolute finish time 
  PTime finishTime;
  finishTime += waitTime;

#if defined(P_HAS_SEMAPHORES)
#ifdef P_HAS_SEMAPHORES_XPG6
  // use proper timed spinlocks if supported.
  // http://www.opengroup.org/onlinepubs/007904975/functions/sem_timedwait.html

  struct timespec absTime;
  absTime.tv_sec  = finishTime.GetTimeInSeconds();
  absTime.tv_nsec = finishTime.GetMicrosecond() * 1000;

  if (sem_timedwait(&semId, &absTime) == 0) {
    return TRUE;
  }
  else {
    return FALSE;
  }

#else
  // loop until timeout, or semaphore becomes available
  // don't use a PTimer, as this causes the housekeeping
  // thread to get very busy
  do {
    if (sem_trywait(&semId) == 0)
      return TRUE;

#if defined(P_LINUX)
  // sched_yield in a tight loop is bad karma
  // for the linux scheduler: http://www.ussg.iu.edu/hypermail/linux/kernel/0312.2/1127.html
    PThread::Current()->Sleep(10);
#else
    PThread::Yield();
#endif
  } while (PTime() < finishTime);

  return FALSE;

#endif
#elif defined(P_HAS_NAMED_SEMAPHORES)
  do {
    if(sem_trywait(semId) == 0)
      return TRUE;
    PThread::Current()->Sleep(10);
  } while (PTime() < finishTime);
  
  return FALSE;
#else

  struct timespec absTime;
  absTime.tv_sec  = finishTime.GetTimeInSeconds();
  absTime.tv_nsec = finishTime.GetMicrosecond() * 1000;

  PAssertPTHREAD(pthread_mutex_lock, (&mutex));

  PThread * thread = PThread::Current();
  thread->PXSetWaitingSemaphore(this);
  queuedLocks++;

  BOOL ok = TRUE;
  while (currentCount == 0) {
    int err = pthread_cond_timedwait(&condVar, &mutex, &absTime);
    if (err == ETIMEDOUT) {
      ok = FALSE;
      break;
    }
    else
      PAssert(err == 0 || err == EINTR, psprintf("timed wait error = %i", err));
  }

  thread->PXSetWaitingSemaphore(NULL);
  queuedLocks--;

  if (ok)
    currentCount--;

  PAssertPTHREAD(pthread_mutex_unlock, ((pthread_mutex_t *)&mutex));

  return ok;
#endif
}


void PSemaphore::Signal()
{
#if defined(P_HAS_SEMAPHORES)
  PAssertPTHREAD(sem_post, (&semId));
#elif defined(P_HAS_NAMED_SEMAPHORES)
  PAssertPTHREAD(sem_post, (semId));
#else
  PAssertPTHREAD(pthread_mutex_lock, (&mutex));

  if (currentCount < maximumCount)
    currentCount++;

  if (queuedLocks > 0) 
    PAssertPTHREAD(pthread_cond_signal, (&condVar));

  PAssertPTHREAD(pthread_mutex_unlock, (&mutex));
#endif
}


BOOL PSemaphore::WillBlock() const
{
#if defined(P_HAS_SEMAPHORES)
  if (sem_trywait((sem_t *)&semId) != 0) {
    PAssertOS(errno == EAGAIN || errno == EINTR);
    return TRUE;
  }
  PAssertPTHREAD(sem_post, ((sem_t *)&semId));
  return FALSE;
#elif defined(P_HAS_NAMED_SEMAPHORES)
  if (sem_trywait(semId) != 0) {
    PAssertOS(errno == EAGAIN || errno == EINTR);
    return TRUE;
  }
  PAssertPTHREAD(sem_post, (semId));
  return FALSE;
#else
  return currentCount == 0;
#endif
}

PTimedMutex::PTimedMutex()
//  : PSemaphore(PXMutex)
{
#if P_HAS_RECURSIVE_MUTEX
  pthread_mutexattr_t attr;
  PAssertPTHREAD(pthread_mutexattr_init, (&attr));

  PAssertPTHREAD(pthread_mutexattr_settype, (&attr,
#if P_HAS_RECURSIVE_MUTEX == 2
PTHREAD_MUTEX_RECURSIVE
#else
PTHREAD_MUTEX_RECURSIVE_NP
#endif
));

  PAssertPTHREAD(pthread_mutex_init, (&mutex, &attr));
  PAssertPTHREAD(pthread_mutexattr_destroy, (&attr));
#else
  PAssertPTHREAD(pthread_mutex_init, (&mutex, NULL));
#endif
}

PTimedMutex::PTimedMutex(const PTimedMutex & /*mut*/)
//  : PSemaphore(PXMutex)
{
#if P_HAS_RECURSIVE_MUTEX
  pthread_mutexattr_t attr;
  PAssertPTHREAD(pthread_mutexattr_init, (&attr));
  PAssertPTHREAD(pthread_mutexattr_settype, (&attr, 
#if P_HAS_RECURSIVE_MUTEX == 2
PTHREAD_MUTEX_RECURSIVE
#else
PTHREAD_MUTEX_RECURSIVE_NP
#endif
));

  PAssertPTHREAD(pthread_mutex_init, (&mutex, &attr));
  PAssertPTHREAD(pthread_mutexattr_destroy, (&attr));
#else
  pthread_mutex_init(&mutex, NULL);
#endif
}

PTimedMutex::~PTimedMutex()
{
  int result = pthread_mutex_destroy(&mutex);
  PINDEX i = 0;
  while ((result == EBUSY) && (i++ < 20)) {
    pthread_mutex_unlock(&mutex);
    result = pthread_mutex_destroy(&mutex);
  }
#ifdef _DEBUG
  PAssert((result == 0), "Error destroying mutex");
#endif
}

void PTimedMutex::Wait() 
{
  pthread_t currentThreadId = pthread_self();

#if P_HAS_RECURSIVE_MUTEX == 0

  // if the mutex is already acquired by this thread,
  // then just increment the lock count
  if (pthread_equal(lockerId, currentThreadId)) {
    // Note this does not need a lock as it can only be touched by the thread
    // which already has the mutex locked.
    ++lockCount;
    return;
  }
#endif

  // acquire the lock for real
  PAssertPTHREAD(pthread_mutex_lock, (&mutex));

#if P_HAS_RECURSIVE_MUTEX == 0
  PAssert((lockerId == (pthread_t)-1) && (lockCount.IsZero()),
          "PMutex acquired whilst locked by another thread");
  // Note this is protected by the mutex itself only the thread with
  // the lock can alter it.
#endif

  lockerId = currentThreadId;
}


BOOL PTimedMutex::Wait(const PTimeInterval & waitTime) 
{
  // get the current thread ID
  pthread_t currentThreadId = pthread_self();

  // if waiting indefinitely, then do so
  if (waitTime == PMaxTimeInterval) {
    Wait();
    lockerId = currentThreadId;
    return TRUE;
  }

#if P_HAS_RECURSIVE_MUTEX == 0
  // if we already have the mutex, return immediately
  if (pthread_equal(lockerId, currentThreadId)) {
    // Note this does not need a lock as it can only be touched by the thread
    // which already has the mutex locked.
    ++lockCount;
    return TRUE;
  }
#endif

  // create absolute finish time
  PTime finishTime;
  finishTime += waitTime;

#if P_PTHREADS_XPG6
  
  struct timespec absTime;
  absTime.tv_sec  = finishTime.GetTimeInSeconds();
  absTime.tv_nsec = finishTime.GetMicrosecond() * 1000;

  if (pthread_mutex_timedlock(&mutex, &absTime) != 0)
    return FALSE;

#if P_HAS_RECURSIVE_MUTEX == 0
  PAssert((lockerId == (pthread_t)-1) && (lockCount.IsZero()),
          "PMutex acquired whilst locked by another thread");
#endif

  // Note this is protected by the mutex itself only the thread with
  // the lock can alter it.
  lockerId = currentThreadId;
  return TRUE;

#else // P_PTHREADS_XPG6

  do {
    if (pthread_mutex_trylock(&mutex) == 0) {
#if P_HAS_RECURSIVE_MUTEX == 0
      PAssert((lockerId == (pthread_t)-1) && (lockCount.IsZero()),
              "PMutex acquired whilst locked by another thread");
#endif
      lockerId = currentThreadId;

      return TRUE;
    }

    PThread::Current()->Sleep(10); // sleep for 10ms
  } while (PTime() < finishTime);

  return FALSE;

#endif // P_PTHREADS_XPG6
}


void PTimedMutex::Signal()
{
#if P_HAS_RECURSIVE_MUTEX == 0
  if (!pthread_equal(lockerId, pthread_self())) {
    PAssertAlways("PMutex signal failed - no matching wait or signal by wrong thread");
    return;
  }

  // if lock was recursively acquired, then decrement the counter
  // Note this does not need a separate lock as it can only be touched by the thread
  // which already has the mutex locked.
  if (!lockCount.IsZero()) {
    --lockCount;
    return;
  }

  // otherwise mark mutex as available
  lockerId = (pthread_t)-1;

#endif

  PAssertPTHREAD(pthread_mutex_unlock, (&mutex));
}


BOOL PTimedMutex::WillBlock() const
{
#if P_HAS_RECURSIVE_MUTEX == 0
  pthread_t currentThreadId = pthread_self();
  if (currentThreadId == lockerId)
    return FALSE;
#endif

  pthread_mutex_t * mp = (pthread_mutex_t*)&mutex;
  if (pthread_mutex_trylock(mp) != 0)
    return TRUE;

  PAssertPTHREAD(pthread_mutex_unlock, (mp));
  return FALSE;
}


PSyncPoint::PSyncPoint()
  : PSemaphore(PXSyncPoint)
{
  PAssertPTHREAD(pthread_mutex_init, (&mutex, NULL));
  PAssertPTHREAD(pthread_cond_init, (&condVar, NULL));
  signalCount = 0;
}

PSyncPoint::PSyncPoint(const PSyncPoint &)
  : PSemaphore(PXSyncPoint)
{
  PAssertPTHREAD(pthread_mutex_init, (&mutex, NULL));
  PAssertPTHREAD(pthread_cond_init, (&condVar, NULL));
  signalCount = 0;
}

PSyncPoint::~PSyncPoint()
{
  PAssertPTHREAD(pthread_mutex_destroy, (&mutex));
  PAssertPTHREAD(pthread_cond_destroy, (&condVar));
}

void PSyncPoint::Wait()
{
  PAssertPTHREAD(pthread_mutex_lock, (&mutex));
  while (signalCount == 0)
    pthread_cond_wait(&condVar, &mutex);
  signalCount--;
  PAssertPTHREAD(pthread_mutex_unlock, (&mutex));
}


BOOL PSyncPoint::Wait(const PTimeInterval & waitTime)
{
  PAssertPTHREAD(pthread_mutex_lock, (&mutex));

  PTime finishTime;
  finishTime += waitTime;
  struct timespec absTime;
  absTime.tv_sec  = finishTime.GetTimeInSeconds();
  absTime.tv_nsec = finishTime.GetMicrosecond() * 1000;

  int err = 0;
  while (signalCount == 0) {
    err = pthread_cond_timedwait(&condVar, &mutex, &absTime);
    if (err == 0 || err == ETIMEDOUT)
      break;

    PAssertOS(err == EINTR && errno == EINTR);
  }

  if (err == 0)
    signalCount--;

  PAssertPTHREAD(pthread_mutex_unlock, (&mutex));

  return err == 0;
}


void PSyncPoint::Signal()
{
  PAssertPTHREAD(pthread_mutex_lock, (&mutex));
  signalCount++;
  PAssertPTHREAD(pthread_cond_signal, (&condVar));
  PAssertPTHREAD(pthread_mutex_unlock, (&mutex));
}


BOOL PSyncPoint::WillBlock() const
{
  return signalCount == 0;
}


