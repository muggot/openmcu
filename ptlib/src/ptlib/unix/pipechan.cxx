/*
 * pipechan.cxx
 *
 * Sub-process commuicating with pip I/O channel implementation
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
 * $Log: pipechan.cxx,v $
 * Revision 1.45  2006/07/14 05:46:14  csoutheren
 * Applied 1470324 - Use _exit instead of exit in PPipeChannel
 *
 * Revision 1.44  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.43  2004/12/15 00:39:01  csoutheren
 * Fixed problem with passing arguments to PPipeChannel, thanks to Derek Smithies
 *
 * Revision 1.42  2003/04/22 23:43:51  craigs
 * MacOSX changes as per Hugo Santos
 *
 * Revision 1.41  2003/01/09 08:21:47  robertj
 * Fixed possibly handle leak if fork() fails.
 * Also added belt and braces checks for making sure no handle can leak in
 *   other unknown logic. Plus do handle high water mark logging.
 *
 * Revision 1.40  2003/01/08 01:33:52  craigs
 * Fixed problem with not checking errno on return from waitpid
 *
 * Revision 1.39  2003/01/08 01:29:22  craigs
 * More changes for return code to waitpid
 *
 * Revision 1.38  2002/12/18 01:12:09  craigs
 * Remove erroneous WUNTRACED and added support for EINTR from waitpid
 *
 * Revision 1.37  2002/12/05 05:11:16  craigs
 * Fixed IsRunning and WaitForTermination to provide the correct return
 * codes from subprograms
 *
 * Revision 1.36  2002/12/02 03:57:18  robertj
 * More RTEMS support patches, thank you Vladimir Nesic.
 *
 * Revision 1.35  2002/11/22 10:14:07  robertj
 * QNX port, thanks Xiaodan Tang
 *
 * Revision 1.34  2002/10/17 13:44:27  robertj
 * Port to RTEMS, thanks Vladimir Nesic.
 *
 * Revision 1.33  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.32  2002/07/31 07:30:11  craigs
 * WaitForTermination now returns exit code of program, as required
 *
 * Revision 1.31  2001/10/11 02:20:54  robertj
 * Added IRIX support (no audio/video), thanks Andre Schulze.
 *
 * Revision 1.30  2001/08/16 11:58:22  rogerh
 * Add more Mac OS X changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.29  2001/08/12 06:32:04  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.28  2001/06/30 06:59:07  yurik
 * Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
 *
 * Revision 1.27  2000/06/21 01:01:22  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.26  2000/04/09 18:19:23  rogerh
 * Add my changes for NetBSD support.
 *
 * Revision 1.25  2000/04/06 12:11:32  rogerh
 * MacOS X support submitted by Kevin Packard
 *
 * Revision 1.24  2000/03/08 12:17:09  rogerh
 * Add OpenBSD support
 *
 * Revision 1.23  1999/06/28 09:28:02  robertj
 * Portability issues, especially n BeOS (thanks Yuri!)
 *
 * Revision 1.22  1999/02/22 13:26:54  robertj
 * BeOS port changes.
 *
 * Revision 1.21  1998/11/30 21:51:46  robertj
 * New directory structure.
 *
 * Revision 1.20  1998/11/24 10:25:19  robertj
 * Fixed environment variable on FreeBSD
 *
 * Revision 1.19  1998/11/24 09:39:11  robertj
 * FreeBSD port.
 *
 * Revision 1.18  1998/11/06 01:06:05  robertj
 * Solaris environment variable name.
 *
 * Revision 1.17  1998/11/05 09:42:01  robertj
 * Fixed bug in direct stdout mode opening redirected stdout.
 * Solaris support, missing environ declaration.
 * Added assert for unsupported timeout in WaitForTermination() under solaris.
 *
 * Revision 1.16  1998/11/02 11:11:19  robertj
 * Added pipe output to stdout/stderr.
 *
 * Revision 1.15  1998/11/02 10:30:40  robertj
 * GNU v6 compatibility.
 *
 * Revision 1.14  1998/11/02 10:07:34  robertj
 * Added ReadStandardError implementation
 *
 * Revision 1.13  1998/10/30 13:02:50  robertj
 * New pipe channel enhancements.
 *
 * Revision 1.12  1998/10/26 11:09:56  robertj
 * added separation of stdout and stderr.
 *
 * Revision 1.11  1998/09/24 04:12:14  robertj
 * Added open software license.
 *
 */

#pragma implementation "pipechan.h"

#include <ptlib.h>
#include <ptlib/pipechan.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#if defined(P_LINUX) || defined(P_SOLARIS)
#include <termio.h>
#endif

#if defined(P_MACOSX)
#include <crt_externs.h>
#endif

#include "../common/pipechan.cxx"


int PX_NewHandle(const char *, int);


////////////////////////////////////////////////////////////////
//
//  PPipeChannel
//

PPipeChannel::PPipeChannel()
{
  toChildPipe[0] = toChildPipe[1] = -1;
  fromChildPipe[0] = fromChildPipe[1] = -1;
  stderrChildPipe[0] = stderrChildPipe[1] = -1;
}


BOOL PPipeChannel::PlatformOpen(const PString & subProgram,
                                const PStringArray & argumentList,
                                OpenMode mode,
                                BOOL searchPath,
                                BOOL stderrSeparate,
                                const PStringToString * environment)
{
#if defined(P_VXWORKS) || defined(P_RTEMS)
  PAssertAlways("PPipeChannel::PlatformOpen");
  return FALSE;
#else
  subProgName = subProgram;

  // setup the pipe to the child
  if (mode == ReadOnly)
    toChildPipe[0] = toChildPipe[1] = -1;
  else {
    PAssert(pipe(toChildPipe) == 0, POperatingSystemError);
    PX_NewHandle("PPipeChannel toChildPipe", PMAX(toChildPipe[0], toChildPipe[1]));
  }
 
  // setup the pipe from the child
  if (mode == WriteOnly || mode == ReadWriteStd)
    fromChildPipe[0] = fromChildPipe[1] = -1;
  else {
    PAssert(pipe(fromChildPipe) == 0, POperatingSystemError);
    PX_NewHandle("PPipeChannel fromChildPipe", PMAX(fromChildPipe[0], fromChildPipe[1]));
  }

  if (stderrSeparate)
    PAssert(pipe(stderrChildPipe) == 0, POperatingSystemError);
  else {
    stderrChildPipe[0] = stderrChildPipe[1] = -1;
    PX_NewHandle("PPipeChannel stderrChildPipe", PMAX(stderrChildPipe[0], stderrChildPipe[1]));
  }

  // fork to allow us to execute the child
#if defined(__BEOS__) || defined(P_IRIX)
  childPid = fork();
#else
  childPid = vfork();
#endif
  if (childPid < 0)
    return FALSE;

  if (childPid > 0) {
    // setup the pipe to the child
    if (toChildPipe[0] != -1) {
      ::close(toChildPipe[0]);
      toChildPipe[0] = -1;
    }

    if (fromChildPipe[1] != -1) {
      ::close(fromChildPipe[1]);
      fromChildPipe[1] = -1;
    }
 
    if (stderrChildPipe[1] != -1) {
      ::close(stderrChildPipe[1]);
      stderrChildPipe[1] = -1;
    }
 
    os_handle = 0;
    return TRUE;
  }

  // the following code is in the child process

  // if we need to write to the child, make sure the child's stdin
  // is redirected
  if (toChildPipe[0] != -1) {
    ::close(STDIN_FILENO);
    ::dup(toChildPipe[0]);
    ::close(toChildPipe[0]);
    ::close(toChildPipe[1]);  
  } else {
    int fd = open("/dev/null", O_RDONLY);
    PAssertOS(fd >= 0);
    ::close(STDIN_FILENO);
    ::dup(fd);
    ::close(fd);
  }

  // if we need to read from the child, make sure the child's stdout
  // and stderr is redirected
  if (fromChildPipe[1] != -1) {
    ::close(STDOUT_FILENO);
    ::dup(fromChildPipe[1]);
    ::close(STDERR_FILENO);
    if (!stderrSeparate)
      ::dup(fromChildPipe[1]);
    ::close(fromChildPipe[1]);
    ::close(fromChildPipe[0]); 
  } else if (mode != ReadWriteStd) {
    int fd = ::open("/dev/null", O_WRONLY);
    PAssertOS(fd >= 0);
    ::close(STDOUT_FILENO);
    ::dup(fd);
    ::close(STDERR_FILENO);
    if (!stderrSeparate)
      ::dup(fd);
    ::close(fd);
  }

  if (stderrSeparate) {
    ::dup(stderrChildPipe[1]);
    ::close(stderrChildPipe[1]);
    ::close(stderrChildPipe[0]); 
  }

  // set the SIGINT and SIGQUIT to ignore so the child process doesn't
  // inherit them from the parent
  signal(SIGINT,  SIG_IGN);
  signal(SIGQUIT, SIG_IGN);

  // and set ourselves as out own process group so we don't get signals
  // from our parent's terminal (hopefully!)
  PSETPGRP();

  // setup the arguments, not as we are about to execl or exit, we don't
  // care about memory leaks, they are not real!
  char ** args = (char **)calloc(argumentList.GetSize()+2, sizeof(char *));
  args[0] = strdup(subProgName.GetTitle());
  PINDEX i;
  for (i = 0; i < argumentList.GetSize(); i++) 
    args[i+1] = strdup(argumentList[i].GetPointer());

  // Set up new environment if one specified.
  if (environment != NULL) {
#if defined(P_SOLARIS) || defined(P_FREEBSD) || defined(P_OPENBSD) || defined (P_NETBSD) || defined(__BEOS__) || defined(P_MACOSX) || defined(P_MACOS) || defined (P_AIX) || defined(P_IRIX) || defined(P_QNX)
    extern char ** environ;
#  if defined(P_MACOSX)
#    define environ (*_NSGetEnviron())
#  endif
#  define __environ environ
#endif
    __environ = (char **)calloc(environment->GetSize()+1, sizeof(char*));
    for (i = 0; i < environment->GetSize(); i++) {
      PString str = environment->GetKeyAt(i) + '=' + environment->GetDataAt(i);
      __environ[i] = strdup(str);
    }
  }

  // execute the child as required
  if (searchPath)
    execvp(subProgram, args);
  else
    execv(subProgram, args);

  _exit(2);
  return FALSE;
#endif // P_VXWORKS || P_RTEMS
}


BOOL PPipeChannel::Close()
{
  // close pipe from child
  if (fromChildPipe[0] != -1) {
    ::close(fromChildPipe[0]);
    fromChildPipe[0] = -1;
  }

  if (fromChildPipe[1] != -1) {
    ::close(fromChildPipe[1]);
    fromChildPipe[1] = -1;
  }

  // close pipe to child
  if (toChildPipe[0] != -1) {
    ::close(toChildPipe[0]);
    toChildPipe[0] = -1;
  }

  if (toChildPipe[1] != -1) {
    ::close(toChildPipe[1]);
    toChildPipe[1] = -1;
  }

  // close pipe to child
  if (stderrChildPipe[0] != -1) {
    ::close(stderrChildPipe[0]);
    stderrChildPipe[0] = -1;
  }

  if (stderrChildPipe[1] != -1) {
    ::close(stderrChildPipe[1]);
    stderrChildPipe[1] = -1;
  }

  // kill the child process
  if (IsRunning()) {
    kill (childPid, SIGKILL);
    WaitForTermination();
  }

  // ensure this channel looks like it is closed
  os_handle = -1;
  childPid  = 0;

  return TRUE;
}

BOOL PPipeChannel::Read(void * buffer, PINDEX len)
{
  PAssert(IsOpen(), "Attempt to read from closed pipe");
  PAssert(fromChildPipe[0] != -1, "Attempt to read from write-only pipe");

  os_handle = fromChildPipe[0];
  BOOL status = PChannel::Read(buffer, len);
  os_handle = 0;
  return status;
}

BOOL PPipeChannel::Write(const void * buffer, PINDEX len)
{
  PAssert(IsOpen(), "Attempt to write to closed pipe");
  PAssert(toChildPipe[1] != -1, "Attempt to write to read-only pipe");

  os_handle = toChildPipe[1];
  BOOL status = PChannel::Write(buffer, len);
  os_handle = 0;
  return status;
}

BOOL PPipeChannel::Execute()
{
  flush();
  clear();
  if (toChildPipe[1] != -1) {
    ::close(toChildPipe[1]);
    toChildPipe[1] = -1;
  }
  return TRUE;
}


PPipeChannel::~PPipeChannel()
{
  Close();
}

int PPipeChannel::GetReturnCode() const
{
  return retVal;
}

BOOL PPipeChannel::IsRunning() const
{
  if (childPid == 0)
    return FALSE;

#if defined(P_PTHREADS) || defined(P_MAC_MPTHREADS)

  int err;
  int status;
  if ((err = waitpid(childPid, &status, WNOHANG)) == 0)
    return TRUE;

  if (err != childPid)
    return FALSE;

  PPipeChannel * thisW = (PPipeChannel *)this;
  thisW->childPid = 0;

  if (WIFEXITED(status)) {
    thisW->retVal = WEXITSTATUS(status);
    PTRACE(2, "PipeChannel\tChild exited with code " << retVal);
  } else if (WIFSIGNALED(status)) {
    PTRACE(2, "PipeChannel\tChild was signalled with " << WTERMSIG(status));
    thisW->retVal = -1;
  } else if (WIFSTOPPED(status)) {
    PTRACE(2, "PipeChannel\tChild was stopped with " << WSTOPSIG(status));
    thisW->retVal = -1;
  } else {
    PTRACE(2, "PipeChannel\tChild was stopped with unknown status" << status);
    thisW->retVal = -1;
  }

  return FALSE;

#else
  return kill(childPid, 0) == 0;
#endif
}

int PPipeChannel::WaitForTermination()
{
  if (childPid == 0)
    return retVal;

  int err;

#if defined(P_PTHREADS) || defined(P_MAC_MPTHREADS)
  int status;
  do {
    err = waitpid(childPid, &status, 0);
    if (err == childPid) {
      childPid = 0;
      if (WIFEXITED(status)) {
        retVal = WEXITSTATUS(status);
        PTRACE(2, "PipeChannel\tChild exited with code " << retVal);
      } else if (WIFSIGNALED(status)) {
        PTRACE(2, "PipeChannel\tChild was signalled with " << WTERMSIG(status));
        retVal = -1;
      } else if (WIFSTOPPED(status)) {
        PTRACE(2, "PipeChannel\tChild was stopped with " << WSTOPSIG(status));
        retVal = -1;
      } else {
        PTRACE(2, "PipeChannel\tChild was stopped with unknown status" << status);
        retVal = -1;
      }
      return retVal;
    }
  } while (errno == EINTR);
#else
  if ((err = kill (childPid, 0)) == 0)
    return retVal = PThread::Current()->PXBlockOnChildTerminate(childPid, PMaxTimeInterval);
#endif

  ConvertOSError(err);
  return -1;
}

int PPipeChannel::WaitForTermination(const PTimeInterval & timeout)
{
  if (childPid == 0)
    return retVal;

  int err;

#if defined(P_PTHREADS) || defined(P_MAC_MPTHREADS)
  PAssert(timeout == PMaxTimeInterval, PUnimplementedFunction);
  int status;
  do {
    err = waitpid(childPid, &status, 0);
    if (err == childPid) {
      childPid = 0;
      if (WIFEXITED(status)) {
        retVal = WEXITSTATUS(status);
        PTRACE(2, "PipeChannel\tChild exited with code " << retVal);
      } else if (WIFSIGNALED(status)) {
        PTRACE(2, "PipeChannel\tChild was signalled with " << WTERMSIG(status));
        retVal = -1;
      } else if (WIFSTOPPED(status)) {
        PTRACE(2, "PipeChannel\tChild was stopped with " << WSTOPSIG(status));
        retVal = -1;
      } else {
        PTRACE(2, "PipeChannel\tChild was stopped with unknown status" << status);
        retVal = -1;
      }
      return retVal;
    }
  } while (errno == EINTR);
#else
  if ((err = kill (childPid, 0)) == 0)
    return retVal = PThread::Current()->PXBlockOnChildTerminate(childPid, timeout);
#endif

  ConvertOSError(err);
  return -1;
}

BOOL PPipeChannel::Kill(int killType)
{
  return ConvertOSError(kill (childPid, killType));
}

BOOL PPipeChannel::CanReadAndWrite()
{
  return TRUE;
}


BOOL PPipeChannel::ReadStandardError(PString & errors, BOOL wait)
{
  PAssert(IsOpen(), "Attempt to read from closed pipe");
  PAssert(stderrChildPipe[0] != -1, "Attempt to read from write-only pipe");

  os_handle = stderrChildPipe[0];
  
  BOOL status = FALSE;
#ifndef BE_BONELESS
  int available;
  if (ConvertOSError(ioctl(stderrChildPipe[0], FIONREAD, &available))) {
    if (available != 0)
      status = PChannel::Read(errors.GetPointer(available+1), available);
    else if (wait) {
      char firstByte;
      status = PChannel::Read(&firstByte, 1);
      if (status) {
        errors = firstByte;
        if (ConvertOSError(ioctl(stderrChildPipe[0], FIONREAD, &available))) {
          if (available != 0)
            status = PChannel::Read(errors.GetPointer(available+2)+1, available);
        }
      }
    }
  }
#endif

  os_handle = 0;
  return status;
}


