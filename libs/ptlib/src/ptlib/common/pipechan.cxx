/*
 * pipechan.cxx
 *
 * Sub-process communicating with pipe I/O channel class
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
 * Revision 1.8  2004/04/03 08:22:21  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.7  1998/11/30 04:59:04  robertj
 * New directory structure
 *
 * Revision 1.6  1998/10/30 10:42:32  robertj
 * Better function arrangement for multi platforming.
 *
 * Revision 1.5  1998/10/29 11:29:18  robertj
 * Added ability to set environment in sub-process.
 *
 * Revision 1.4  1998/10/26 09:11:06  robertj
 * Added ability to separate out stdout from stderr on pipe channels.
 *
 * Revision 1.3  1998/09/23 06:22:31  robertj
 * Added open source copyright license.
 *
 * Revision 1.2  1996/05/09 12:18:41  robertj
 * Fixed syntax error found by Mac platform.
 *
 * Revision 1.1  1996/04/14 02:54:14  robertj
 * Initial revision
 *
 */

#include <ptlib.h>
#include <ptlib/pipechan.h>

#include <ctype.h>


///////////////////////////////////////////////////////////////////////////////
// PPipeChannel

static BOOL SplitArgs(const PString & cmdline,
                      PString & progName,
                      PStringArray & arguments)
{
  PArgList list = cmdline;
  if (list.GetCount() == 0)
    return FALSE;

  progName = list[0];

  arguments.SetSize(list.GetCount()-1);
  for (PINDEX i = 1; i < list.GetCount(); i++)
    arguments[i-1] = list[i];

  return TRUE;
}


PPipeChannel::PPipeChannel(const PString & subProgram,
                           OpenMode mode,
                           BOOL searchPath,
                           BOOL stderrSeparate)
{
  PString progName;
  PStringArray arguments;
  if (SplitArgs(subProgram, progName, arguments))
    PlatformOpen(progName, arguments, mode, searchPath, stderrSeparate, NULL);
}


PPipeChannel::PPipeChannel(const PString & subProgram,
                           const PStringArray & arguments,
                           OpenMode mode,
                           BOOL searchPath,
                           BOOL stderrSeparate)
{
  PlatformOpen(subProgram, arguments, mode, searchPath, stderrSeparate, NULL);
}


PPipeChannel::PPipeChannel(const PString & subProgram,
                           const PStringToString & environment,
                           OpenMode mode,
                           BOOL searchPath,
                           BOOL stderrSeparate)
{
  PString progName;
  PStringArray arguments;
  if (SplitArgs(subProgram, progName, arguments))
    PlatformOpen(progName, arguments, mode, searchPath, stderrSeparate, &environment);
}


PPipeChannel::PPipeChannel(const PString & subProgram,
                           const PStringArray & arguments,
                           const PStringToString & environment,
                           OpenMode mode,
                           BOOL searchPath,
                           BOOL stderrSeparate)
{
  PlatformOpen(subProgram, arguments, mode, searchPath, stderrSeparate, &environment);
}


PObject::Comparison PPipeChannel::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PPipeChannel), PInvalidCast);
  return subProgName.Compare(((const PPipeChannel &)obj).subProgName);
}


PString PPipeChannel::GetName() const
{
  return subProgName;
}


BOOL PPipeChannel::Open(const PString & subProgram,
                        OpenMode mode,
                        BOOL searchPath,
                        BOOL stderrSeparate)
{
  PString progName;
  PStringArray arguments;
  if (!SplitArgs(subProgram, progName, arguments))
    return FALSE;
  return PlatformOpen(progName, arguments, mode, searchPath, stderrSeparate, NULL);
}


BOOL PPipeChannel::Open(const PString & subProgram,
                        const PStringArray & arguments,
                        OpenMode mode,
                        BOOL searchPath,
                        BOOL stderrSeparate)
{
  return PlatformOpen(subProgram, arguments, mode, searchPath, stderrSeparate, NULL);
}


BOOL PPipeChannel::Open(const PString & subProgram,
                        const PStringToString & environment,
                        OpenMode mode,
                        BOOL searchPath,
                        BOOL stderrSeparate)
{
  PString progName;
  PStringArray arguments;
  if (!SplitArgs(subProgram, progName, arguments))
    return FALSE;
  return PlatformOpen(progName, arguments, mode, searchPath, stderrSeparate, &environment);
}


BOOL PPipeChannel::Open(const PString & subProgram,
                        const PStringArray & arguments,
                        const PStringToString & environment,
                        OpenMode mode,
                        BOOL searchPath,
                        BOOL stderrSeparate)
{
  return PlatformOpen(subProgram, arguments, mode, searchPath, stderrSeparate, &environment);
}


// End Of File ///////////////////////////////////////////////////////////////
