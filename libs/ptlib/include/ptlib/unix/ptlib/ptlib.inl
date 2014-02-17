/*
 * ptlib.inl
 *
 * Operating System classes inline function implementation
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
 * $Log: ptlib.inl,v $
 * Revision 1.40  2007/09/05 11:09:10  csoutheren
 * Removed misleading and incorrect code from Linux implementation of
 * PCriticalSection. Apologies to Hannes Friederich :(
 *
 * Revision 1.39  2007/09/05 08:03:25  hfriederich
 * Implement PCriticalSection with named semaphores
 *
 * Revision 1.38  2007/08/17 05:29:19  csoutheren
 * Add field to Linux showing locking thread to assist in debugging
 *
 * Revision 1.37  2006/06/21 13:27:03  csoutheren
 * Fixed link problem with gcc 2.95.3
 *
 * Revision 1.36  2005/11/14 22:41:53  csoutheren
 * Reverted Wait and Signal to non-const - there is no way we can guarantee that all
 * descendant classes everywhere will be changed over, so we have to keep the
 * original API
 *
 * Revision 1.35  2005/11/04 06:56:10  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 * Revision 1.34  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.33  2004/04/18 12:37:40  csoutheren
 * Modified to detect sem_wait etc on Linux systems
 *
 * Revision 1.32  2004/04/11 03:20:42  csoutheren
 * Added Unix implementation of PCriticalSection
 *
 * Revision 1.31  2004/04/03 15:47:58  ykiryanov
 * Changed thread id member name for BeOS
 *
 * Revision 1.30  2004/02/21 21:59:10  ykiryanov
 * Fixed more Be thread related functions
 *
 * Revision 1.29  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.28  2003/02/26 02:01:10  robertj
 * Fixed compatibility with a MIPS compiler, Thanks Eize Slange
 *
 * Revision 1.27  2002/11/19 12:14:44  robertj
 * Added function to get root directory.
 *
 * Revision 1.26  2002/11/02 00:32:21  robertj
 * Further fixes to VxWorks (Tornado) port, thanks Andreas Sikkema.
 *
 * Revision 1.25  2002/10/17 13:44:27  robertj
 * Port to RTEMS, thanks Vladimir Nesic.
 *
 * Revision 1.24  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.23  2002/06/27 06:45:00  robertj
 * Changed "" to PString::Empty() where assigning to PString.
 *
 * Revision 1.22  2002/01/26 23:56:43  craigs
 * Changed for GCC 3.0 compatibility, thanks to manty@manty.net
 *
 * Revision 1.21  2001/03/20 06:44:25  robertj
 * Lots of changes to fix the problems with terminating threads that are I/O
 *   blocked, especially when doing orderly shutdown of service via SIGTERM.
 *
 * Revision 1.20  2001/03/12 02:35:20  robertj
 * Fixed PDirectory::Exists so only returns TRUE if a directory and not file.
 *
 * Revision 1.19  2000/04/05 02:55:11  robertj
 * Added microseconds to PTime class.
 *
 * Revision 1.18  1998/11/24 09:38:22  robertj
 * FreeBSD port.
 *
 * Revision 1.17  1998/11/10 12:59:18  robertj
 * Fixed strange problems with readdir_r usage.
 *
 * Revision 1.16  1998/10/18 10:02:47  robertj
 * Fixed program argument access functions.
 *
 * Revision 1.15  1998/09/24 04:11:49  robertj
 * Added open software license.
 *
 */

#if defined(P_LINUX)
#if (__GNUC_MINOR__ < 7 && __GNUC__ <= 2)
#include <localeinfo.h>
#else
#define P_USE_LANGINFO
#endif
#elif defined(P_HPUX9)
#define P_USE_LANGINFO
#elif defined(P_SUN4)
#endif

#ifdef P_USE_LANGINFO
#include <langinfo.h>
#endif

PINLINE DWORD PProcess::GetProcessID() const
{
#ifdef P_VXWORKS
  return PX_threadId;
#else
  return (DWORD)getpid();
#endif // P_VXWORKS
}

///////////////////////////////////////////////////////////////////////////////

PINLINE unsigned PTimer::Resolution()
{
#if defined(P_SUN4)
  return 1000;
#elif defined(P_RTEMS)
  rtems_interval ticks_per_sec; 
  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_sec); 
  return (unsigned)(1000/ticks_per_sec);
#else
  return (unsigned)(1000/CLOCKS_PER_SEC);
#endif
}

///////////////////////////////////////////////////////////////////////////////

PINLINE BOOL PDirectory::IsRoot() const
  { return IsSeparator((*this)[0]) && ((*this)[1] == '\0'); }

PINLINE PDirectory PDirectory::GetRoot() const
  { return PString(PDIR_SEPARATOR); }

PINLINE BOOL PDirectory::IsSeparator(char ch)
  { return ch == PDIR_SEPARATOR; }

PINLINE BOOL PDirectory::Change(const PString & p)
  { return chdir((char *)(const char *)p) == 0; }

///////////////////////////////////////////////////////////////////////////////

PINLINE PString PFilePath::GetVolume() const
  { return PString::Empty(); }

///////////////////////////////////////////////////////////////////////////////

PINLINE BOOL PFile::Remove(const PFilePath & name, BOOL)
  { return unlink((char *)(const char *)name) == 0; }

///////////////////////////////////////////////////////////////////////////////

PINLINE PString PChannel::GetName() const
  { return channelName; }

#ifdef BE_THREADS

PINLINE PThreadIdentifier PThread::GetThreadId() const
  { return mId; }

#else // !BE_THREADS

PINLINE PThreadIdentifier PThread::GetThreadId() const
  { return PX_threadId; }

#ifndef VX_TASKS
PINLINE PThreadIdentifier PThread::GetCurrentThreadId()
  { return ::pthread_self(); }
#else
PINLINE PThreadIdentifier PThread::GetCurrentThreadId()
  { return ::taskIdSelf(); }
#endif // !VX_TASKS

#endif // BE_THREADS

// End Of File ///////////////////////////////////////////////////////////////
