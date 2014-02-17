/*
 * tlib.cxx
 *
 * Miscelaneous class implementation
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
 * $Log: tlib.cxx,v $
 * Revision 1.83  2007/07/09 00:13:27  csoutheren
 * Fix compile on MacOSX
 *
 * Revision 1.82  2007/07/06 02:12:14  csoutheren
 * Add extra memory leak debugging on Linux
 * Remove compile warnings
 *
 * Revision 1.81  2007/06/29 02:47:28  rjongbloed
 * Added PString::FindSpan() function (strspn equivalent) with slightly nicer semantics.
 *
 * Revision 1.80  2007/05/01 10:20:44  csoutheren
 * Applied 1703617 - Prevention of application deadlock caused by too many timers
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.79  2006/07/14 04:55:10  csoutheren
 * Applied 1520151 - Adds PID to tracefile + Rolling Date pattern
 * Thanks to Paul Nader
 *
 * Revision 1.78  2006/06/21 13:27:03  csoutheren
 * Fixed link problem with gcc 2.95.3
 *
 * Revision 1.77  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.76  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.75  2005/11/08 10:35:53  csoutheren
 * Added missing pragma
 *
 * Revision 1.74  2005/08/04 18:54:10  csoutheren
 * Applied fix for bug Bugs item #1244310
 * Fix problem wioth --disable-openh323 option
 * Thanks to Michael Manousos
 *
 * Revision 1.73  2005/02/22 03:25:43  dereksmithies
 * Add fix from huangzb@a-star.com.cn to correct a typo. Many thanks.
 *
 * Revision 1.72  2005/01/26 05:38:05  csoutheren
 * Added ability to remove config file support
 *
 * Revision 1.71  2005/01/04 08:09:43  csoutheren
 * Fixed Linux configure problems
 *
 * Revision 1.70  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.69  2003/09/18 23:52:08  dereksmithies
 * Fix checks on NULL pointers. Many thanks to Chris Rankin.
 *
 * Revision 1.68  2003/09/17 01:18:04  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.67  2003/01/24 10:21:06  robertj
 * Fixed issues in RTEMS support, thanks Vladimir Nesic
 *
 * Revision 1.66  2002/12/11 23:02:39  robertj
 * Added ability to set user identity temporarily and permanently.
 * Added ability to have username exclusively digits which corresponds to a uid.
 * Added get and set users group functions.
 *
 * Revision 1.65  2002/12/02 03:57:18  robertj
 * More RTEMS support patches, thank you Vladimir Nesic.
 *
 * Revision 1.64  2002/11/22 10:14:07  robertj
 * QNX port, thanks Xiaodan Tang
 *
 * Revision 1.63  2002/11/02 00:32:21  robertj
 * Further fixes to VxWorks (Tornado) port, thanks Andreas Sikkema.
 *
 * Revision 1.62  2002/10/17 13:44:27  robertj
 * Port to RTEMS, thanks Vladimir Nesic.
 *
 * Revision 1.61  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.60  2002/06/27 08:09:06  robertj
 * GNU GCC 3.1 compatibility under Solaris
 *
 * Revision 1.59  2001/11/25 23:30:31  robertj
 * Added PProcess::SetUserName() function to set euid
 *
 * Revision 1.58  2001/10/11 02:20:54  robertj
 * Added IRIX support (no audio/video), thanks Andre Schulze.
 *
 * Revision 1.57  2001/09/18 05:56:03  robertj
 * Fixed numerous problems with thread suspend/resume and signals handling.
 *
 * Revision 1.56  2001/08/11 15:38:43  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.55  2001/06/30 06:59:07  yurik
 * Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
 *
 * Revision 1.54  2001/03/29 03:24:31  robertj
 * Removed capture of SIGQUIT so can still dro a core on demand.
 *
 * Revision 1.53  2001/03/14 01:16:11  robertj
 * Fixed signals processing, now uses housekeeping thread to handle signals
 *   synchronously. This also fixes issues with stopping PServiceProcess.
 *
 * Revision 1.52  2001/03/07 07:31:25  yurik
 * refined BeOS constants
 *
 * Revision 1.51  2000/06/21 01:01:22  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.50  2000/04/09 18:19:23  rogerh
 * Add my changes for NetBSD support.
 *
 * Revision 1.49  2000/04/06 12:19:49  rogerh
 * Add Mac OS X support submitted by Kevin Packard
 *
 * Revision 1.48  2000/03/08 12:17:09  rogerh
 * Add OpenBSD support
 *
 * Revision 1.47  1999/09/03 02:26:25  robertj
 * Changes to aid in breaking I/O locks on thread termination. Still needs more work esp in BSD!
 *
 * Revision 1.46  1999/07/19 01:32:24  craigs
 * Changed signals used in pthreads code, is used by linux version.
 *
 * Revision 1.45  1999/07/11 13:42:13  craigs
 * pthreads support for Linux
 *
 * Revision 1.44  1999/06/28 09:28:02  robertj
 * Portability issues, especially n BeOS (thanks Yuri!)
 *
 * Revision 1.43  1999/05/13 04:44:18  robertj
 * Added SIGHUP and SIGWINCH handlers to increase and decrease the log levels.
 *
 * Revision 1.42  1999/03/02 05:41:59  robertj
 * More BeOS changes
 *
 * Revision 1.41  1999/02/26 04:10:39  robertj
 * More BeOS port changes
 *
 * Revision 1.40  1999/02/19 11:34:15  robertj
 * Added platform dependent function for "system configuration" directory.
 *
 * Revision 1.39  1999/02/06 05:49:44  robertj
 * BeOS port effort by Yuri Kiryanov <openh323@kiryanov.com>
 *
 * Revision 1.38  1999/01/11 12:10:39  robertj
 * Improved operating system version display.
 *
 * Revision 1.37  1999/01/08 01:31:01  robertj
 * Support for pthreads under FreeBSD
 *
 * Revision 1.36  1998/11/24 11:24:40  robertj
 * Added FreeBSD OSName
 *
 * Revision 1.35  1998/11/24 09:39:16  robertj
 * FreeBSD port.
 *
 * Revision 1.34  1998/10/31 14:14:21  robertj
 * Changed syncptack.h to syncthrd.h for more thread synchronisation objects.
 *
 * Revision 1.33  1998/10/19 00:29:57  robertj
 * Moved error stream to common.
 *
 * Revision 1.32  1998/09/24 04:12:22  robertj
 * Added open software license.
 *
 * Revision 1.31  1998/05/30 14:58:56  robertj
 * Fixed shutdown deadlock (and other failure modes) in cooperative threads.
 *
 * Revision 1.30  1998/04/17 15:13:08  craigs
 * Added lazy writes to Config cache
 *
 * Revision 1.29  1998/03/29 10:42:16  craigs
 * Changed for new initialisation scheme
 *
 * Revision 1.28  1998/03/26 05:01:12  robertj
 * Added PMutex and PSyncPoint classes.
 *
 * Revision 1.27  1998/01/04 08:09:23  craigs
 * Added support for PThreads through use of reentrant system calls
 *
 * Revision 1.26  1998/01/03 22:46:44  craigs
 * Added PThread support
 *
 * Revision 1.25  1997/05/10 08:04:15  craigs
 * Added new routines for access to PErrorStream
 *
 * Revision 1.24  1997/04/22 10:57:53  craigs
 * Removed DLL functions and added call the FreeStack
 *
 * Revision 1.23  1997/02/23 03:06:00  craigs
 * Changed for PProcess::Current reference
 *
 * Revision 1.22  1997/02/14 09:18:36  craigs
 * Changed for PProcess::Current being a reference rather that a ptr
 *
 * Revision 1.21  1996/12/30 03:21:46  robertj
 * Added timer to block on wait for child process.
 *
 * Revision 1.20  1996/12/29 13:25:02  robertj
 * Fixed GCC warnings.
 *
 * Revision 1.19  1996/11/16 11:11:46  craigs
 * Fixed problem with timeout on blocked IO channels
 *
 * Revision 1.18  1996/11/03 04:35:58  craigs
 * Added hack to avoid log timeouts, which shouldn't happen!
 *
 * Revision 1.17  1996/09/21 05:40:10  craigs
 * Changed signal hcnalding
 *
 * Revision 1.16  1996/09/03 11:55:19  craigs
 * Removed some potential problems with return values from system calls
 *
 * Revision 1.15  1996/06/29 01:43:11  craigs
 * Moved AllocateStack to switch.cxx to keep platform dependent routines in one place
 *
 * Revision 1.14  1996/06/10 12:46:53  craigs
 * Changed process.h include
 *
 * Revision 1.13  1996/05/25 06:06:33  craigs
 * Sun4 fixes and updated for gcc 2.7.2
 *
 * Revision 1.12  1996/05/09 10:55:59  craigs
 * More SunOS fixes
 *
 * Revision 1.11  1996/05/03 13:15:27  craigs
 * More Sun4 & Solaris fixes
 *
 * Revision 1.10  1996/05/03 13:11:35  craigs
 * More Sun4 fixes
 *
 * Revision 1.9  1996/05/02 12:11:54  craigs
 * Sun4 fixes
 *
 * Revision 1.8  1996/04/18 11:43:38  craigs
 * Changed GetHomeDir to use effective UID for uid, and changed to
 * look at passwd file info *before* $HOME variable
 *
 * Revision 1.7  1996/04/15 10:49:11  craigs
 * Last build prior to release of MibMaster v1.0
 *
 * Revision 1.6  1996/01/26 11:09:42  craigs
 * Added signal handlers
 *
 */

#define _OSUTIL_CXX

//#define SIGNALS_DEBUG

#pragma implementation "args.h"
#pragma implementation "pprocess.h"
#pragma implementation "thread.h"
#pragma implementation "semaphor.h"
#pragma implementation "mutex.h"
#pragma implementation "critsec.h"
#pragma implementation "psync.h"
#pragma implementation "syncpoint.h"
#pragma implementation "syncthrd.h"

#include "ptlib.h"
#include <ptlib/pprocess.h>

#ifdef P_VXWORKS
#include <sys/times.h>
#include <time.h>
#include <hostLib.h>
#include <remLib.h>
#include <taskLib.h>
#include <intLib.h>
#else
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>
#endif // P_VXWORKS
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#if defined(P_LINUX)
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/mman.h>
#endif

#if defined(P_LINUX) || defined(P_SUN4) || defined(P_SOLARIS) || defined(P_FREEBSD) || defined(P_OPENBSD) || defined(P_NETBSD) || defined(P_MACOSX) || defined(P_MACOS) || defined (P_AIX) || defined(__BEOS__) || defined(P_IRIX) || defined(P_QNX)
#include <sys/utsname.h>
#define  HAS_UNAME
#elif defined(P_RTEMS)
extern "C" {
#include <sys/utsname.h>
}
#define  HAS_UNAME
#endif

#include "uerror.h"

#if defined(P_HPUX9)
#define  SELECT(p1,p2,p3,p4,p5)    select(p1,(int *)(p2),(int *)(p3),(int *)(p4),p5)
#else
#define  SELECT(p1,p2,p3,p4,p5)    select(p1,p2,p3,p4,p5)
#endif

#if defined(P_SUN4)
extern "C" void bzero(void *, int);
extern "C" int select(int width,
      fd_set *readfds,
      fd_set *writefds,
      fd_set *exceptfds,
      struct timeval *timeout);
#endif

#ifdef __BEOS__
#include "OS.h"
#endif

#include "../common/pglobalstatic.cxx"


PString PProcess::GetOSClass()
{
#ifndef __BEOS__
  return PString("Unix");
#elif defined P_VXWORKS
  return PString("VxWorks");
#else
  return PString("Be Inc.");
#endif
}

PString PProcess::GetOSName()
{
#if defined(HAS_UNAME)
  struct utsname info;
  uname(&info);
#ifdef P_SOLARIS
  return PString(info.sysname) & info.release;
#else
  return PString(info.sysname);
#endif
#elif defined(P_VXWORKS)
  return PString::Empty();
#else
#warning No GetOSName specified
  return PString("Unknown");
#endif
}

PString PProcess::GetOSHardware()
{
#if defined(HAS_UNAME)
  struct utsname info;
  uname(&info);
  return PString(info.machine);
#elif defined(P_VXWORKS)
  return PString(sysModel());
#else
#warning No GetOSHardware specified
  return PString("unknown");
#endif
}

PString PProcess::GetOSVersion()
{
#if defined(HAS_UNAME)
  struct utsname info;
  uname(&info);
#ifdef P_SOLARIS
  return PString(info.version);
#else
  return PString(info.release);
#endif
#elif defined(P_VXWORKS)
  return PString(sysBspRev());
#else
#warning No GetOSVersion specified
  return PString("?.?");
#endif
}

PDirectory PProcess::GetOSConfigDir()
{
#ifdef P_VXWORKS
  return "./";
#else
  return "/etc";
#endif // P_VXWORKS
}

PDirectory PProcess::PXGetHomeDir ()

{
#ifdef P_VXWORKS
  return "./";
#else
  PString dest;
  char *ptr;
  struct passwd *pw = NULL;

#if defined(P_PTHREADS) && !defined(P_THREAD_SAFE_CLIB)
  struct passwd pwd;
  char buffer[1024];
#if defined (P_LINUX) || defined(P_AIX) || defined(P_IRIX) || (__GNUC__>=3 && defined(P_SOLARIS)) || defined(P_RTEMS)
  ::getpwuid_r(geteuid(), &pwd,
               buffer, 1024,
               &pw);
#else
  pw = ::getpwuid_r(geteuid(), &pwd, buffer, 1024);
#endif
#else
  pw = ::getpwuid(geteuid());
#endif

  if (pw != NULL && pw->pw_dir != NULL) 
    dest = pw->pw_dir;
  else if ((ptr = getenv ("HOME")) != NULL) 
    dest = ptr;
  else 
    dest = ".";

  if (dest.GetLength() > 0 && dest[dest.GetLength()-1] != '/')
    dest += "/";

  return dest;
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
// PProcess
//
// Return the effective user name of the process, eg "root" etc.

PString PProcess::GetUserName() const

{
#ifdef P_VXWORKS

  char pnamebuf[1024];
  int len = 1024;
  STATUS gethostresult;
  gethostresult =::gethostname(pnamebuf,len);  
  if (gethostresult == OK)
    return PString(pnamebuf,len);
  else
    return PString("VxWorks");

#else

#if defined(P_PTHREADS) && !defined(P_THREAD_SAFE_CLIB)
  struct passwd pwd;
  char buffer[1024];
  struct passwd * pw = NULL;
#if defined (P_LINUX) || defined (P_AIX) || defined(P_IRIX) || (__GNUC__>=3 && defined(P_SOLARIS)) || defined(P_RTEMS)
  ::getpwuid_r(geteuid(), &pwd, buffer, 1024, &pw);
#else
  pw = ::getpwuid_r(geteuid(), &pwd, buffer, 1024);
#endif
#else
  struct passwd * pw = ::getpwuid(geteuid());
#endif

  char * ptr;
  if (pw != NULL && pw->pw_name != NULL)
    return PString(pw->pw_name);
  else if ((ptr = getenv("USER")) != NULL)
    return PString(ptr);
  else
    return PString("user");
#endif // P_VXWORKS
}


BOOL PProcess::SetUserName(const PString & username, BOOL permanent)
{
#ifdef P_VXWORKS
  PAssertAlways("PProcess::SetUserName - not implemented for VxWorks");
  return FALSE;
#else
  if (username.IsEmpty())
    return seteuid(getuid()) != -1;

  int uid = -1;

  if (username[0] == '#') {
    PString s = username.Mid(1);
    if (s.FindSpan("1234567890") == P_MAX_INDEX)
      uid = s.AsInteger();
  }
  else {
#if defined(P_PTHREADS) && !defined(P_THREAD_SAFE_CLIB)
    struct passwd pwd;
    char buffer[1024];
    struct passwd * pw = NULL;
#if defined (P_LINUX) || defined (P_AIX) || defined(P_IRIX) || (__GNUC__>=3 && defined(P_SOLARIS)) || defined(P_RTEMS)
    ::getpwnam_r(username, &pwd, buffer, 1024, &pw);
#else
    pw = ::getpwnam_r(username, &pwd, buffer, 1024);
#endif
#else
    struct passwd * pw = ::getpwnam(username);
#endif

    if (pw != NULL && pw->pw_name != NULL)
      uid = pw->pw_uid;
    else {
      if (username.FindSpan("1234567890") == P_MAX_INDEX)
       uid = username.AsInteger();
    }
  }

  if (uid < 0)
    return FALSE;

  if (permanent)
    return setuid(uid) != -1;
    
  return seteuid(uid) != -1;
#endif // P_VXWORKS
}


///////////////////////////////////////////////////////////////////////////////
//
// PProcess
//
// Return the effective group name of the process, eg "wheel" etc.

PString PProcess::GetGroupName() const

{
#ifdef P_VXWORKS

  return PString("VxWorks");

#else

#if defined(P_PTHREADS) && !defined(P_THREAD_SAFE_CLIB)
  struct group grp;
  char buffer[1024];
  struct group * gr = NULL;
#if defined (P_LINUX) || defined (P_AIX) || defined(P_IRIX) || (__GNUC__>=3 && defined(P_SOLARIS)) || defined(P_RTEMS)
  ::getgrgid_r(getegid(), &grp, buffer, 1024, &gr);
#else
  gr = ::getgrgid_r(getegid(), &grp, buffer, 1024);
#endif
#else
  struct group * gr = ::getgrgid(getegid());
#endif

  char * ptr;
  if (gr != NULL && gr->gr_name != NULL)
    return PString(gr->gr_name);
  else if ((ptr = getenv("GROUP")) != NULL)
    return PString(ptr);
  else
    return PString("group");
#endif // P_VXWORKS
}


BOOL PProcess::SetGroupName(const PString & groupname, BOOL permanent)
{
#ifdef P_VXWORKS
  PAssertAlways("PProcess::SetGroupName - not implemented for VxWorks");
  return FALSE;
#else
  if (groupname.IsEmpty())
    return setegid(getgid()) != -1;

  int gid = -1;

  if (groupname[0] == '#') {
    PString s = groupname.Mid(1);
    if (s.FindSpan("1234567890") == P_MAX_INDEX)
      gid = s.AsInteger();
  }
  else {
#if defined(P_PTHREADS) && !defined(P_THREAD_SAFE_CLIB)
    struct group grp;
    char buffer[1024];
    struct group * gr = NULL;
#if defined (P_LINUX) || defined (P_AIX) || defined(P_IRIX) || (__GNUC__>=3 && defined(P_SOLARIS)) || defined(P_RTEMS)
    ::getgrnam_r(groupname, &grp, buffer, 1024, &gr);
#else
    gr = ::getgrnam_r(groupname, &grp, buffer, 1024);
#endif
#else
    struct group * gr = ::getgrnam(groupname);
#endif

    if (gr != NULL && gr->gr_name != NULL)
      gid = gr->gr_gid;
    else {
      if (groupname.FindSpan("1234567890") == P_MAX_INDEX)
       gid = groupname.AsInteger();
    }
  }

  if (gid < 0)
    return FALSE;

  if (permanent)
    return setgid(gid) != -1;
    
  return setegid(gid) != -1;
#endif // P_VXWORKS
}

extern PProcess * PProcessInstance;;

PString PX_GetThreadName(pthread_t id)
{
  if (PProcessInstance != NULL) {
    PWaitAndSignal m(PProcessInstance->threadMutex);
#   if defined (__FreeBSD__) && defined (P_64BIT) 
    PThread & thread = PProcessInstance->activeThreads[(unsigned long)id];
#   else
    PThread & thread = PProcessInstance->activeThreads[(unsigned)id];
#   endif
    return thread.GetThreadName();
  }
  return psprintf("%08x", id);
}

void PProcess::PXShowSystemWarning(PINDEX num)
{
  PXShowSystemWarning(num, "");
}

void PProcess::PXShowSystemWarning(PINDEX num, const PString & str)
{
  PProcess::Current()._PXShowSystemWarning(num, str);
}

void PProcess::_PXShowSystemWarning(PINDEX code, const PString & str)
{
  PError << "PWLib " << GetOSClass() << " error #" << code << '-' << str << endl;
}

void PXSignalHandler(int sig)
{
#ifdef SIGNALS_DEBUG
  fprintf(stderr,"\nSIGNAL<%u>\n",sig);
#endif

  PProcess & process = PProcess::Current();
  process.pxSignals |= 1 << sig;
  process.PXOnAsyncSignal(sig);
#if defined(P_MAC_MPTHREADS)
  process.SignalTimerChange();
#elif defined(P_PTHREADS)
  // Inform house keeping thread we have a signal to be processed
  process.SignalTimerChange();
#endif
  signal(sig, PXSignalHandler);
}

void PProcess::PXCheckSignals()
{
  if (pxSignals == 0)
    return;

#ifdef SIGNALS_DEBUG
  fprintf(stderr,"\nCHKSIG<%x>\n",pxSignals);
#endif

  for (int sig = 0; sig < 32; sig++) {
    int bit = 1 << sig;
    if ((pxSignals&bit) != 0) {
      pxSignals &= ~bit;
      PXOnSignal(sig);
    }
  }
}


void SetSignals(void (*handler)(int))
{
#ifdef SIGNALS_DEBUG
  fprintf(stderr,"\nSETSIG<%x>\n",(INT)handler);
#endif

  if (handler == NULL)
    handler = SIG_DFL;

#ifdef SIGHUP
  signal(SIGHUP, handler);
#endif
#ifdef SIGINT
  signal(SIGINT, handler);
#endif
#ifdef SIGUSR1
  signal(SIGUSR1, handler);
#endif
#ifdef SIGUSR2
  signal(SIGUSR2, handler);
#endif
#ifdef SIGPIPE
  signal(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGTERM
  signal(SIGTERM, handler);
#endif
#ifdef SIGWINCH
  signal(SIGWINCH, handler);
#endif
#ifdef SIGPROF
  signal(SIGPROF, handler);
#endif
}


void PProcess::PXOnAsyncSignal(int sig)
{
#ifdef SIGNALS_DEBUG
  fprintf(stderr,"\nASYNCSIG<%u>\n",sig);
#endif

  switch (sig) {
    case SIGINT:
    case SIGTERM:
    case SIGHUP:
      raise(SIGKILL);
      break;
    default:
      return;
  }
}

void PProcess::PXOnSignal(int sig)
{
#ifdef SIGNALS_DEBUG
  fprintf(stderr,"\nSYNCSIG<%u>\n",sig);
#endif
  if (sig == 28) {
#if PMEMORY_CHECK
    BOOL oldIgnore = PMemoryHeap::SetIgnoreAllocations(TRUE);
    static DWORD allocationIndex = 0;
#endif
    PStringStream strm;
    threadMutex.Wait();
    PINDEX i;
    strm << "===============\n";
    strm << activeThreads.GetSize() << " active threads\n";
    for (i = 0; i < activeThreads.GetSize(); ++i) {
      POrdinalKey key = activeThreads.GetKeyAt(i);
      PThread & thread = activeThreads[key];
      strm << "  " << thread << "\n";
    }
#if PMEMORY_CHECK
    strm << "---------------\n";
    PMemoryHeap::DumpObjectsSince(allocationIndex, strm);
    allocationIndex = PMemoryHeap::GetAllocationRequest();
#endif
    strm << "===============\n";
    threadMutex.Signal();
    fprintf(stderr, "%s", strm.GetPointer());
#if PMEMORY_CHECK
    PMemoryHeap::SetIgnoreAllocations(oldIgnore);
#endif
  }
}

void PProcess::CommonConstruct()
{
  // Setup signal handlers
  pxSignals = 0;

  SetSignals(&PXSignalHandler);

#if !defined(P_VXWORKS) && !defined(P_RTEMS)
  // initialise the timezone information
  tzset();
#endif

#ifdef P_CONFIG_FILE
  CreateConfigFilesDictionary();
#endif
}

void PProcess::CommonDestruct()
{
#ifdef P_CONFIG_FILE
  delete configFiles;
#endif
  configFiles = NULL;
  SetSignals(NULL);
}

// rtems fixes
#ifdef P_RTEMS

extern "C" {
#include <netinet/in.h>
#include <rtems/rtems_bsdnet.h>
  

int socketpair(int d, int type, int protocol, int sv[2])
{
    static int port_count = IPPORT_USERRESERVED;
    int s;
    int addrlen;
    struct sockaddr_in addr1, addr2;
    static int network_status = 1;
    

    if (network_status>0)
    {
        printf("\"Network\" initializing!\n");
        network_status = rtems_bsdnet_initialize_network();
  if (network_status == 0)
      printf("\"Network\" initialized!\n");
  else
  {
      printf("Error: %s\n", strerror(errno));
      return -1;
  }
    }

    /* prepare sv */
    sv[0]=sv[1]=-1;

    /* make socket */
    s = socket( d, type, protocol);
    if (s<0) 
        return -1;

    memset(&addr1, 0, sizeof addr1);
    addr1.sin_family = d;
    addr1.sin_port = htons(++port_count);
    addr1.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(s, (struct sockaddr *)&addr1, sizeof addr1) < 0) 
    {
  close(s);
        return -1;
    }
    if (listen(s, 2) < 0 ) 
    {
  close(s);
        return -1;
    }
    
    sv[0] = socket(d, type, protocol);
    if (sv[0] < 0) 
    {
  close(s);
        return -1;
    }
    
    memset(&addr2, 0, sizeof addr2);
    addr2.sin_family = d;
    addr2.sin_port = htons(++port_count);
    addr2.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(sv[0], (struct sockaddr *)&addr2, sizeof addr2) < 0)
    {
  close(s);
  close(sv[0]);
  sv[0]=-1;
        return -1;
    }
    if (connect(sv[0], (struct sockaddr *)&addr1, sizeof addr1) < 0)
    {
  close(s);
  close(sv[0]);
  sv[0]=-1;
        return -1;
    }
    
    sv[1] = accept(s, (struct sockaddr *)&addr2, &addrlen);
    if (sv[1] < 0)
    {
  close(s);
  close(sv[0]);
  sv[0]=-1;
        return -1;
    }

    close(s);
    return 0;
}

/*
 * Loopback interface
 */
extern int rtems_bsdnet_loopattach(rtems_bsdnet_ifconfig *);
static struct rtems_bsdnet_ifconfig loopback_config = {
    "lo0",                          /* name */
    rtems_bsdnet_loopattach,        /* attach function */

    NULL,                           /* link to next interface */

    "127.0.0.1",                    /* IP address */
    "255.0.0.0",                    /* IP net mask */
};

#include <bsp.h>
#warning Change lines below to match Your system settings

/*
 * Default network interface
 */
static struct rtems_bsdnet_ifconfig netdriver_config = {
    RTEMS_BSP_NETWORK_DRIVER_NAME,          /* name */
    RTEMS_BSP_NETWORK_DRIVER_ATTACH,        /* attach function */

    &loopback_config,                       /* link to next interface */

    "10.0.0.2",                             /* IP address */
    "255.255.255.0",                        /* IP net mask */

    NULL,                                   /* Driver supplies hardware address */
    0                                       /* Use default driver parameters */
};

/*
 * Network configuration
 */
struct rtems_bsdnet_config rtems_bsdnet_config = {
    &netdriver_config,

    NULL,                   /* no bootp function */

    1,                      /* Default network task priority */
    0,                      /* Default mbuf capacity */
    0,                      /* Default mbuf cluster capacity */

    "computer.name",        /* Host name */
    "domain.name",          /* Domain name */
    "10.0.0.1",             /* Gateway */
    "10.0.0.1",             /* Log host */
    {"10.0.0.1" },          /* Name server(s) */
    {"10.0.0.1" },          /* NTP server(s) */

};

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
#define CONFIGURE_TEST_NEEDS_TIMER_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK                         1000
#define CONFIGURE_TICKS_PER_TIMESLICE                             50

#define CONFIGURE_MAXIMUM_TASKS          rtems_resource_unlimited(50)
#define CONFIGURE_MAXIMUM_TIMERS         rtems_resource_unlimited(50)
#define CONFIGURE_MAXIMUM_SEMAPHORES     rtems_resource_unlimited(50)
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES rtems_resource_unlimited(50)
#define CONFIGURE_MAXIMUM_MUTEXES        rtems_resource_unlimited(50)

#define CONFIGURE_MAXIMUM_POSIX_THREADS                          500
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES                          500
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES              500
#define CONFIGURE_MAXIMUM_POSIX_KEYS                             500
#define CONFIGURE_MAXIMUM_POSIX_TIMERS                           500
#define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS                   500
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES                   500
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES                       500

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS                 500
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | RTEMS_TIMESLICE)

#ifdef DEBUG
#define STACK_CHECKER_ON
#endif

void* POSIX_Init(void*);
#define CONFIGURE_INIT
#include <confdefs.h>
}

#endif // P_RTEMS


//////////////////////////////////////////////////////////////////
//
//  Non-PTHREAD based routines
//

#if defined(P_MAC_MPTHREADS)
#include "tlibmpthrd.cxx"
#elif defined(P_PTHREADS)
#include "tlibthrd.cxx"
#elif defined(BE_THREADS)
#include "tlibbe.cxx"
#elif defined(VX_TASKS)
#include "tlibvx.cxx"
#endif
