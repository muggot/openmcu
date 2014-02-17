/*
 * ptlib.inl
 *
 * Non-GUI classes inline function implementation.
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
 * Revision 1.26  2007/10/03 01:18:44  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.25  2006/01/18 07:18:00  csoutheren
 * Added explicit copy constructor for PCriticalSection on Windows
 *
 * Revision 1.24  2005/11/14 22:29:13  csoutheren
 * Reverted Wait and Signal to non-const - there is no way we can guarantee that all
 * descendant classes everywhere will be changed over, so we have to keep the
 * original  API
 *
 * Revision 1.23  2005/11/04 06:34:20  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 * Revision 1.22  2004/04/11 02:55:18  csoutheren
 * Added PCriticalSection for Windows
 * Added compile time option for PContainer to use critical sections to provide thread safety under some circumstances
 *
 * Revision 1.21  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.20  1999/07/06 04:46:00  robertj
 * Fixed being able to case an unsigned to a PTimeInterval.
 * Improved resolution of PTimer::Tick() to be millisecond accurate.
 *
 * Revision 1.19  1998/11/30 03:02:17  robertj
 * Moved PPipeChannel code to .cxx file to avoid linking unused code.
 *
 * Revision 1.18  1998/11/14 23:37:06  robertj
 * Fixed file path directory extraction, not able to return root directory
 *
 * Revision 1.17  1998/09/24 03:30:19  robertj
 * Added open software license.
 *
 * Revision 1.16  1996/08/20 12:10:36  robertj
 * Fixed bug in timers wrapping unexpectedly and producing fast timeout.
 *
 * Revision 1.15  1996/07/20 05:32:26  robertj
 * MSVC 4.1 compatibility.
 *
 * Revision 1.14  1996/05/15 10:23:25  robertj
 * Changed millisecond access functions to get 64 bit integer.
 *
 * Revision 1.13  1996/03/31 09:08:23  robertj
 * Added mutex to thread dictionary access.
 *
 * Revision 1.12  1996/03/04 12:38:56  robertj
 * Moved calculation of stackTop to platform dependent code.
 *
 * Revision 1.11  1995/12/10 11:48:27  robertj
 * Fixed bug in application shutdown of child threads.
 *
 * Revision 1.10  1995/04/22 00:52:55  robertj
 * Added GetDirectory() function to PFilePath.
 *
 * Revision 1.9  1995/03/12 04:59:58  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.8  1994/12/21  11:55:09  robertj
 * Fixed file paths returning correct string type.
 *
 * Revision 1.7  1994/10/23  05:38:57  robertj
 * PipeChannel implementation.
 * Added directory exists function.
 *
 * Revision 1.6  1994/08/22  00:18:02  robertj
 * Renamed CheckBlock() to IsNoLongerBlocked()
 *
 * Revision 1.5  1994/07/27  06:00:10  robertj
 * Backup
 *
 * Revision 1.4  1994/07/21  12:35:18  robertj
 * *** empty log message ***
 *
 * Revision 1.3  1994/07/02  03:18:09  robertj
 * Multi-threading support.
 * Fixed bug in time intervals being signed.
 *
 * Revision 1.2  1994/06/25  12:13:01  robertj
 * Synchronisation.
 *
 * Revision 1.1  1994/04/01  14:38:42  robertj
 * Initial revision
 */


///////////////////////////////////////////////////////////////////////////////
// PTimer

#if !defined(_WIN32)

#if CLOCKS_PER_SEC==1000

PINLINE PTimeInterval PTimer::Tick()
  { return clock(); }

PINLINE unsigned PTimer::Resolution()
  { return 1; }

#else

PINLINE PTimeInterval PTimer::Tick()
  { return (PInt64)clock()*CLOCKS_PER_SEC/1000; }

PINLINE unsigned PTimer::Resolution()
  { return 1000/CLOCKS_PER_SEC; }

#endif

#endif


///////////////////////////////////////////////////////////////////////////////
// PDirectory

PINLINE BOOL PDirectory::IsSeparator(char c)
  { return c == ':' || c == '/' || c == '\\'; }

PINLINE BOOL PDirectory::Exists(const PString & p)
  { return _access(p+".", 0) == 0; }

PINLINE BOOL PDirectory::Create(const PString & p, int)
  { return _mkdir(p) == 0; }

PINLINE BOOL PDirectory::Remove(const PString & p)
  { return _rmdir(p) == 0; }


PINLINE BOOL PDirectory::Restart(int scanMask)
  { return Open(scanMask); }



///////////////////////////////////////////////////////////////////////////////
// PFile

PINLINE BOOL PFile::Exists(const PFilePath & name)
  { return _access(name, 0) == 0; }


///////////////////////////////////////////////////////////////////////////////
// PThread

PINLINE void PThread::Sleep(const PTimeInterval & delay)
  { ::Sleep(delay.GetInterval()); }

PINLINE PThread::PThread()
  { }

PINLINE PThreadIdentifier PThread::GetThreadId() const
  { return threadId; }

PINLINE PThreadIdentifier PThread::GetCurrentThreadId()
  { return ::GetCurrentThreadId(); }

///////////////////////////////////////////////////////////////////////////////
// PCriticalSection

PINLINE PCriticalSection::PCriticalSection()
{
  ::InitializeCriticalSection(&criticalSection);
}

PINLINE PCriticalSection::PCriticalSection(const PCriticalSection &)
{
  ::InitializeCriticalSection(&criticalSection);
}

PINLINE PCriticalSection::~PCriticalSection()
{
  ::DeleteCriticalSection(&criticalSection);
}

PINLINE void PCriticalSection::Wait()
{
  ::EnterCriticalSection(&criticalSection);
}

PINLINE void PCriticalSection::Signal()
{
  ::LeaveCriticalSection(&criticalSection);
}

// End Of File ///////////////////////////////////////////////////////////////
