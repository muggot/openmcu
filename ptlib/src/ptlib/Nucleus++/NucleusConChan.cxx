/*
 * osutil.cxx
 *
 * Operating System classes implementation
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
 * $Log: NucleusConChan.cxx,v $
 * Revision 1.1  2000/06/26 11:17:20  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.48  1999/08/17 07:37:36  robertj
 * Fixed inlines so are inlined for optimised version
 *
 * Revision 1.47  1999/06/28 09:28:02  robertj
 * Portability issues, especially n BeOS (thanks Yuri!)
 *
 * Revision 1.46  1999/06/26 08:21:12  robertj
 * Fixed bug in PFilePath::SetType finding dots outside of file name in path.
 *
 * Revision 1.45  1999/06/14 08:39:57  robertj
 * Added PConsoleChannel class for access to stdin/stdout/stderr
 *
 * Revision 1.44  1999/06/09 04:08:46  robertj
 * Added support for opening stdin/stdout/stderr as PFile objects.
 *
 * Revision 1.43  1999/02/22 13:26:53  robertj
 * BeOS port changes.
 *
 * Revision 1.42  1998/12/12 01:06:24  robertj
 * Fixed off by one error in month on FreeBSD platform
 *
 * Revision 1.41  1998/11/30 21:51:43  robertj
 * New directory structure.
 *
 * Revision 1.40  1998/11/26 11:54:16  robertj
 * Fixed error return on PFile::GetInfo
 *
 * Revision 1.39  1998/11/24 09:39:09  robertj
 * FreeBSD port.
 *
 * Revision 1.38  1998/11/10 13:00:52  robertj
 * Fixed strange problems with readdir_r usage.
 *
 * Revision 1.37  1998/11/06 04:44:46  robertj
 * Solaris compatibility
 *
 * Revision 1.36  1998/11/05 12:03:13  robertj
 * Fixed solaris compatibility and Linux warning on readdir_r function.
 *
 * Revision 1.35  1998/11/05 09:05:55  craigs
 * Changed directory routines to use reenttrant functions, and added PDirectory::GetParent
 *
 * Revision 1.34  1998/09/24 07:39:49  robertj
 * Removed file that only had #pragma implmentation for PTextFile and nothing else.
 *
 * Revision 1.33  1998/09/24 04:12:12  robertj
 * Added open software license.
 *
 */

#include <ptlib.h>


#ifdef P_USE_LANGINFO
#pragma message ("H")
#include <langinfo.h>
#endif

#define	LINE_SIZE_STEP	100

#define	DEFAULT_FILE_MODE	(S_IRUSR|S_IWUSR|S_IROTH|S_IRGRP)

#define new PNEW

///////////////////////////////////////////////////////////////////////////////
// PConsoleChannel

PConsoleChannel::PConsoleChannel()
{
}


PConsoleChannel::PConsoleChannel(ConsoleType type)
{
  Open(type);
}


BOOL PConsoleChannel::Open(ConsoleType type)
{
  switch (type) {
    case StandardInput :
      os_handle = 0;
      return TRUE;

    case StandardOutput :
      os_handle = 1;
      return TRUE;

    case StandardError :
      os_handle = 2;
      return TRUE;
  }

  return FALSE;
}


PString PConsoleChannel::GetName() const
{
  return "Console";
}

#ifdef __NUCLEUS_MNT__
BOOL PConsoleChannel::Read(void * buffer, PINDEX length)
  {
  flush();
  cin >> (char *)buffer;
  PINDEX buflen =lastReadCount = strlen((char *)buffer);

  return buflen < length ? buflen : length;
  }


BOOL PConsoleChannel::Write(const void * buffer, PINDEX length)
  {
  flush();
  cout << PString((const char *)buffer, length) << "\n";
  return TRUE;
  }
#endif

BOOL PConsoleChannel::Close()
  {
  os_handle = -1;
  return TRUE;
  }

