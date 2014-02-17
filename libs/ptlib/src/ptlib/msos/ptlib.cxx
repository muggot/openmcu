/*
 * ptlib.cxx
 *
 * General implementation of classes for Microsoft operating systems.
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
 * $Log: ptlib.cxx,v $
 * Revision 1.85  2007/10/03 01:57:37  rjongbloed
 * Fixed DevStudio 2003 compatibility
 *
 * Revision 1.84  2007/10/03 01:18:47  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.83  2007/04/08 01:53:37  ykiryanov
 * Build to support ptlib dll creation
 *
 * Revision 1.82  2007/04/06 21:06:03  ykiryanov
 * Changed directory for wince to wm
 *
 * Revision 1.81  2006/06/21 04:20:07  csoutheren
 * Fixes for VS.net
 *
 * Revision 1.80  2006/04/09 11:03:59  csoutheren
 * Remove warnings on VS.net 2005
 *
 * Revision 1.79  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.78  2005/09/24 09:11:42  dominance
 * use unix-style slashes to not confuse mingw on win32
 *
 * Revision 1.77  2005/09/23 15:30:46  dominance
 * more progress to make mingw compile nicely. Thanks goes to Julien Puydt for pointing out to me how to do it properly. ;)
 *
 * Revision 1.76  2005/08/05 20:41:42  csoutheren
 * Added unix support for scattered read/write
 *
 * Revision 1.75  2005/08/05 19:42:09  csoutheren
 * Added support for scattered read/write
 *
 * Revision 1.74  2004/12/08 00:49:37  csoutheren
 * Fixed weird problem with not returning correct filetype when filename has multiple
 *  "." and slashes
 *
 * Revision 1.73  2004/10/23 10:51:40  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.72  2004/06/22 11:07:23  rjongbloed
 * Fixed incorrect test for error on _sopen return value, thanks Brian Coverstone
 *
 * Revision 1.71  2004/04/03 06:54:30  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.70  2003/09/26 13:46:18  rjongbloed
 * Fixed problem in Win32 NTFS security support, crashes if file has no security at all.
 *
 * Revision 1.69  2003/09/17 05:45:10  csoutheren
 * Removed recursive includes
 *
 * Revision 1.68  2003/06/29 01:40:50  ykiryanov
 * Added include <ptlib/wince/time.h> as in PPC2003 we have also another time.h now
 *
 * Revision 1.67  2003/06/23 16:31:03  ykiryanov
 * Excluded PFile::SetPermissions function from WinCE build
 *
 * Revision 1.66  2003/03/31 08:38:07  robertj
 * Added cygwin compatible usage of NT secureity access control lists in
 *   getting and setting "unix like" file permissions.
 *
 * Revision 1.65  2002/12/18 05:31:06  robertj
 * Moved PTimeInterval::GetInterval() to common code.
 *
 * Revision 1.64  2002/12/18 05:10:53  robertj
 * Fixed problem with returning DWORD time interval when PTimeInterval is
 *   out of range, especially when negative!
 *
 * Revision 1.63  2002/11/19 12:07:02  robertj
 * Added function to get root directory.
 *
 * Revision 1.62  2002/11/19 10:35:16  robertj
 * Added function to extract a path as an array of directories components.
 *
 * Revision 1.61  2002/11/08 06:02:53  robertj
 * Fixed problem wth getting file title if directory has a dot but the
 *   filename doesn't, thanks Peter 'Luna' Runestig
 *
 * Revision 1.60  2002/10/17 07:17:43  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.59  2002/07/25 08:42:33  robertj
 * Fixed conversion of string to 64 bit integer(s), thanks Jose Luis Urien
 *
 * Revision 1.58  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.57  2001/06/04 10:15:01  robertj
 * Fixed bug if tried to get file info on empty file path.
 *
 * Revision 1.56  2001/03/19 05:49:44  robertj
 * Redid int64 stream input to use library conversion function and
 *   set fail bit if no valid integer was found in the input stream.
 *
 * Revision 1.55  2001/03/15 23:49:42  robertj
 * Added missing operators for reading 64 bit integers from streams.
 *
 * Revision 1.54  2001/02/13 04:39:08  robertj
 * Fixed problem with operator= in container classes. Some containers will
 *   break unless the copy is virtual (eg PStringStream's buffer pointers) so
 *   needed to add a new AssignContents() function to all containers.
 *
 * Revision 1.53  2001/01/24 06:38:29  yurik
 * Windows CE port-related changes
 *
 * Revision 1.52  2000/07/09 14:05:46  robertj
 * Added file share options.
 *
 * Revision 1.51  2000/04/29 06:44:17  robertj
 * Added some stuff to make sure symbols included in library.
 *
 * Revision 1.50  2000/02/19 23:46:09  robertj
 * Fixed incorrect values for PFile::Access() function, thanks Stefan Ditscheid.
 *
 * Revision 1.49  1999/08/17 03:46:40  robertj
 * Fixed usage of inlines in optimised version.
 *
 * Revision 1.48  1999/06/14 07:59:38  robertj
 * Enhanced tracing again to add options to trace output (timestamps etc).
 *
 * Revision 1.47  1999/06/13 13:54:07  robertj
 * Added PConsoleChannel class for access to stdin/stdout/stderr.
 *
 * Revision 1.46  1999/06/09 02:05:20  robertj
 * Added ability to open file as standard input, output and error streams.
 *
 * Revision 1.45  1999/05/06 06:11:50  robertj
 * Fixed date to be forgiving of rubbish at end of date string.
 * Fixed PTime::GetHour() etc to not crash on time=-1.
 *
 * Revision 1.44  1998/11/30 07:30:31  robertj
 * Fixed problems with PFilePath parsing functions.
 *
 * Revision 1.43  1998/11/26 10:34:19  robertj
 * Fixed problem with PFileInfo::GetVolume() on UNC paths.
 *
 * Revision 1.42  1998/11/14 23:37:08  robertj
 * Fixed file path directory extraction, not able to return root directory
 *
 * Revision 1.41  1998/10/19 00:20:38  robertj
 * Moved error and trace stream functions to common code.
 *
 * Revision 1.40  1998/10/18 14:27:10  robertj
 * Improved tracing functions.
 *
 * Revision 1.39  1998/10/13 14:13:17  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.38  1998/09/24 03:30:53  robertj
 * Added open software license.
 *
 * Revision 1.37  1998/09/14 12:55:06  robertj
 * Changed memory debug to dump leaks not including static globals.
 *
 * Revision 1.36  1998/05/26 01:29:53  robertj
 * Removed assert as this Close() function is now called all the time for Unix reasons.
 *
 * Revision 1.35  1998/05/21 04:27:31  robertj
 * Compensated for MSC run time library bug.
 *
 * Revision 1.34  1998/04/01 01:54:45  robertj
 * Added memory leak checking to debug version.
 *
 * Revision 1.33  1998/03/29 06:16:51  robertj
 * Rearranged initialisation sequence so PProcess descendent constructors can do "things".
 *
 * Revision 1.32  1998/03/20 03:20:16  robertj
 * Added MSVC RT debug support.
 *
 * Revision 1.31  1998/01/26 00:54:15  robertj
 * 64 bit integer string conversions.
 *
 * Revision 1.30  1998/01/05 10:38:25  robertj
 * Unix pthreads compatibility, added threadsafe time functions.
 *
 * Revision 1.29  1997/12/11 10:40:29  robertj
 * Fixed bug in SetType() function of FilePath.
 *
 * Revision 1.28  1997/08/28 12:49:51  robertj
 * Fixed bug where could not change directory to UNC.
 *
 * Revision 1.27  1997/04/27 05:50:26  robertj
 * DLL support.
 *
 * Revision 1.26  1997/01/12 04:23:43  robertj
 * Fixed PDirectory::IsRoot() so works with UNC's
 *
 * Revision 1.25  1996/08/17 10:00:37  robertj
 * Changes for Windows DLL support.
 *
 * Revision 1.24  1996/08/08 10:09:23  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.23  1996/01/28 02:56:16  robertj
 * Fixed bug in PFilePath functions for if path ends in a directory separator.
 *
 * Revision 1.22  1996/01/23 13:23:51  robertj
 * Fixed bug in PFileInfo for if path ends in directory separator.
 *
 * Revision 1.21  1996/01/02 12:56:49  robertj
 * Fixed copy of directories.
 *
 * Revision 1.20  1995/12/10 11:59:33  robertj
 * Changes to main() startup mechanism to support Mac.
 * Fixed bug in time interfval constant variable initialisation. Not guarenteed to work.
 * Moved error code for specific WIN32 and MS-DOS versions.
 *
 * Revision 1.19  1995/10/14 15:12:29  robertj
 * Added function to get parent directory.
 *
 * Revision 1.18  1995/07/31 12:18:11  robertj
 * Removed PContainer from PChannel ancestor.
 *
 * Revision 1.17  1995/06/04 13:10:19  robertj
 * Fixed rename bug.
 *
 * Revision 1.16  1995/06/04 12:48:06  robertj
 * Changed unknown error string to hex.
 * Added missing GetInfo function for directory entries
 *
 * Revision 1.15  1995/04/25 11:33:35  robertj
 * Changes for DLL support.
 *
 * Revision 1.14  1995/04/22 00:53:49  robertj
 * Added Move() function to PFile.
 * Changed semantics of Rename() function in PFile.
 * Changed file path string to use PFilePath object.
 *
 * Revision 1.13  1995/03/12 05:00:08  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.12  1995/02/27  10:37:06  robertj
 * Added GetUserNmae().
 * Removed superfluous Read() and Write() for text files.
 *
 * Revision 1.11  1995/01/06  10:41:43  robertj
 * Moved identifiers into different scope.
 * Changed file size to 64 bit integer.
 *
 * Revision 1.10  1994/12/21  11:36:07  robertj
 * Fixed caseless string for file paths.
 *
 * Revision 1.9  1994/10/30  11:26:54  robertj
 * Fixed set current directory function.
 * Changed PFilePath to be case insignificant according to platform.
 *
 * Revision 1.8  1994/10/23  05:42:39  robertj
 * PipeChannel headers.
 * ConvertOSError function added.
 * Numerous implementation enhancements.
 *
 * Revision 1.7  1994/08/04  13:24:27  robertj
 * Added debug stream.
 *
 * Revision 1.6  1994/07/27  06:00:10  robertj
 * Backup
 *
 * Revision 1.5  1994/07/21  12:35:18  robertj
 * *** empty log message ***
 *
 * Revision 1.4  1994/07/17  11:01:04  robertj
 * Ehancements, implementation, bug fixes etc.
 *
 * Revision 1.3  1994/07/02  03:18:09  robertj
 * Multi-threading implementation.
 *
 * Revision 1.2  1994/06/25  12:13:01  robertj
 * Synchronisation.
 *
// Revision 1.1  1994/04/01  14:39:35  robertj
// Initial revision
//
 */

#include <ptlib.h>

#include <ptlib/pprocess.h>

#if !P_USE_INLINES
#include <ptlib/osutil.inl>
#include <ptlib/msos/ptlib/ptlib.inl>
#endif

#ifndef _WIN32_WCE
#include <share.h>
#endif


ostream & operator<<(ostream & s, PInt64 v)
{
  char buffer[25];

  if ((s.flags()&ios::hex) != 0)
    return s << _ui64toa(v, buffer, 16);

  if ((s.flags()&ios::oct) != 0)
    return s << _ui64toa(v, buffer, 8);

  if (v < 0) {
    s << '-';
    v = -v;
  }

  return s << _i64toa(v, buffer, 10);
}


ostream & operator<<(ostream & s, PUInt64 v)
{
  char buffer[25];
  return s << _ui64toa(v, buffer, (s.flags()&ios::oct) ? 8 : ((s.flags()&ios::hex) ? 16 : 10));
}


const int MaxDigits = (64+2)/3+1; // Maximum is 22 digit octal number, plus sign

static void GetDigits(BOOL sign, istream & s, char * buffer)
{
  PINDEX count = 0;

  while (isspace(s.peek()))
    s.get();

  if (s.peek() == '+')
    s.get(); // Skip leading '+'
  else if (sign && s.peek() == '-')
    s.get(buffer[count++]);

  if ((s.flags()&ios::oct) != 0) {
    while (isdigit(s.peek()) && s.peek() < '8' && count < MaxDigits)
      s.get(buffer[count++]);
  }
  else if ((s.flags()&ios::hex) != 0) {
    while (isxdigit(s.peek()) && count < MaxDigits)
      s.get(buffer[count++]);
  }
  else {
    while (isdigit(s.peek()) && count < MaxDigits)
      s.get(buffer[count++]);
  }

  buffer[count] = '\0';

  if (count > (buffer[0] == '-' ? 1 : 0))
    return;

  s.clear(ios::failbit);
}


istream & operator>>(istream & s, PInt64 & v)
{
  char b[MaxDigits+1];
  GetDigits(TRUE, s, b);
  v = _atoi64(b);
  return s;
}


istream & operator>>(istream & s, PUInt64 & v)
{
  char b[MaxDigits+1];
  GetDigits(FALSE, s, b);
  v = _atoi64(b);
  return s;
}


PInt64 PString::AsInt64(unsigned base) const
{
  if (base == 10)
    return _atoi64(theArray);

  PAssert(base >= 2 && base <= 36, PInvalidParameter);

  PInt64 total = 0;
  const char * ptr = theArray;

  while (isspace(*ptr))
    ptr++;

  BOOL negative = *ptr == '-';
  if (*ptr == '-' || *ptr == '+')
    ptr++;

  for (;;) {
    unsigned c = *ptr++;
    if (c < '0')
      break;

    if (c <= '9')
      c -= '0';
    else
      c = toupper(c) - 'A' + 10;

    if (c >= base)
      break;

    total = base * total + c;
  }

  if (negative)
    return -total;
  else
    return total;
}


PUInt64 PString::AsUnsigned64(unsigned base) const
{
  PAssert(base >= 2 && base <= 36, PInvalidParameter);

  PUInt64 total = 0;
  const char * ptr = theArray;

  while (isspace(*ptr))
    ptr++;

  for (;;) {
    unsigned c = *ptr++;
    if (c < '0')
      break;

    if (c <= '9')
      c -= '0';
    else
      c = toupper(c) - 'A' + 10;

    if (c >= base)
      break;

    total = base * total + c;
  }

  return total;
}


///////////////////////////////////////////////////////////////////////////////
// PTime

struct tm * PTime::os_localtime(const time_t * clock, struct tm * tb)
{
  struct tm * tp = ::localtime(clock);
  if (tp != NULL)
    return tp;

  memset(tb, 0, sizeof(*tb));
  return tb;
}


struct tm * PTime::os_gmtime(const time_t * clock, struct tm * tb)
{
  struct tm * tp = ::gmtime(clock);
  if (tp != NULL)
    return tp;

  memset(tb, 0, sizeof(*tb));
  return tb;
}


///////////////////////////////////////////////////////////////////////////////
// PChannel

void PChannel::Construct()
{
}


PString PChannel::GetName() const
{
  PAssertAlways(PUnimplementedFunction);
  return PString();
}


BOOL PChannel::Read(void *, PINDEX)
{
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
}


BOOL PChannel::Write(const void *, PINDEX)
{
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
}


BOOL PChannel::Close()
{
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Directories

PDirectory PDirectory::GetParent() const
{
  if (IsRoot())
    return *this;
  
  return *this + "..";
}


BOOL PDirectory::Change(const PString & p)
{
  PDirectory d = p;

  if (d[0] != '\\')
    if (_chdrive(toupper(d[0])-'A'+1) != 0)
      return FALSE;

  return _chdir(d + ".") == 0;
}


BOOL PDirectory::Filtered()
{
#if defined(_WIN32)
#ifdef _WIN32_WCE
  USES_CONVERSION;
  char * name = T2A(fileinfo.cFileName);
#else
  char * name = fileinfo.cFileName;
#endif // _WIN32_WCE
#else
  char * name = fileinfo.name;
#endif
  if (strcmp(name, ".") == 0)
    return TRUE;
  if (strcmp(name, "..") == 0)
    return TRUE;
  if (scanMask == PFileInfo::AllPermissions)
    return FALSE;

  PFileInfo inf;
  PAssert(PFile::GetInfo(*this+name, inf), POperatingSystemError);
  return (inf.type&scanMask) == 0;
}


BOOL PDirectory::IsRoot() const
{
  if ((*this)[1] == ':')
    return GetLength() == 3;

  PINDEX pos = FindOneOf("/\\", 2);
  return pos == GetLength();
}


PDirectory PDirectory::GetRoot() const
{
  if ((*this)[1] == ':')
    return Left(3);

  return Left(FindOneOf("/\\", 2));
}


PStringArray PDirectory::GetPath() const
{
  PStringArray path;

  if (IsEmpty())
    return path;

  if ((*this)[1] == ':')
    path = Tokenise("/\\", FALSE);
  else {
    path = Mid(2).Tokenise("/\\", FALSE);
    path[0].Splice("\\\\", 0);
  }

  PINDEX last = path.GetSize()-1;
  while (path[last].IsEmpty())
    path.SetSize(last--);

  return path;
}


BOOL PDirectory::GetInfo(PFileInfo & info) const
{
  return PFile::GetInfo(*this + GetEntryName(), info);
}


///////////////////////////////////////////////////////////////////////////////
// File Path

PFilePath::PFilePath(const PString & str)
  : PCaselessString(PDirectory::CreateFullPath(str, FALSE))
{
}


PFilePath::PFilePath(const char * cstr)
  : PCaselessString(PDirectory::CreateFullPath(cstr, FALSE))
{
}


PFilePath::PFilePath(const char * prefix, const char * dir)
{
  if (dir != NULL) {
    PDirectory tmpdir(dir);
    operator=(tmpdir);
  }
  else {
    PConfig cfg(PConfig::Environment);
    PString path = cfg.GetString("TMPDIR");
    if (path.IsEmpty()) {
      path = cfg.GetString("TMP");
      if (path.IsEmpty())
        path = cfg.GetString("TEMP");
    }
    if (path.IsEmpty() || path[path.GetLength()-1] != '\\')
      path += '\\';
    *this = path;
  }
  if (prefix != NULL)
    *this += prefix;
  else
    *this += "PW";
  *this += "XXXXXX";
  PAssert(_mktemp(GetPointer()) != NULL, "Could not make temporary file");
}


void PFilePath::AssignContents(const PContainer & cont)
{
  PCaselessString::AssignContents(cont);
  PCaselessString::AssignContents(PDirectory::CreateFullPath(*this, FALSE));
}


static PINDEX GetVolumeSubStringLength(const PString & path)
{
  if (path[1] == ':')
    return 2;

  if (path[0] == '\\' && path[1] == '\\') {
    PINDEX backslash = path.Find('\\', 2);
    if (backslash != P_MAX_INDEX) {
      backslash = path.Find('\\', backslash+1);
      if (backslash != P_MAX_INDEX)
        return backslash;
    }
  }

  PINDEX backslash = path.Find('\\');
  if (backslash != P_MAX_INDEX)
    return backslash;

  return 0;
}


PCaselessString PFilePath::GetVolume() const
{
  return Left(GetVolumeSubStringLength(*this));
}


PDirectory PFilePath::GetDirectory() const
{
  PINDEX backslash = FindLast('\\');
  if (backslash != P_MAX_INDEX)
    return Left(backslash+1);

  return PCaselessString();
}


PCaselessString PFilePath::GetPath() const
{
  return operator()(GetVolumeSubStringLength(*this), FindLast('\\', GetLength()-2));
}


PCaselessString PFilePath::GetFileName() const
{
  PINDEX backslash = FindLast('\\', GetLength()-2);
  if (backslash == P_MAX_INDEX)
    backslash = 0;
  else
    backslash++;

  return Mid(backslash);
}


PCaselessString PFilePath::GetTitle() const
{
  PINDEX backslash = FindLast('\\', GetLength()-2);
  if (backslash == P_MAX_INDEX)
    backslash = 0;
  else
    backslash++;

  PINDEX last_dot = FindLast('.');
  if (last_dot < backslash)
    last_dot = P_MAX_INDEX;

  return operator()(backslash, last_dot-1);
}


PCaselessString PFilePath::GetType() const
{
  PINDEX slash = FindLast('\\');
  if (slash == P_MAX_INDEX)
    slash = 0;
  PINDEX dot = FindLast('.');
  if (dot < slash)
    return PCaselessString();
  return operator()(dot, P_MAX_INDEX);
}


void PFilePath::SetType(const PCaselessString & type)
{
  PINDEX dot = Find('.', FindLast('\\'));
  if (dot != P_MAX_INDEX)
    Splice(type, dot, GetLength()-dot);
  else
    *this += type;
}


///////////////////////////////////////////////////////////////////////////////
// PFile

void PFile::SetFilePath(const PString & newName)
{
  if (!IsOpen())
    path = newName;
}


BOOL PFile::Access(const PFilePath & name, OpenMode mode)
{
  int accmode;

  switch (mode) {
    case ReadOnly :
#ifndef R_OK
#define R_OK 4
#endif
      accmode = R_OK;
      break;

    case WriteOnly :
#ifndef W_OK
#define W_OK 2
#endif
      accmode = W_OK;
      break;

    default :
      accmode = R_OK|W_OK;
  }

  return _access(name, accmode) == 0;
}


BOOL PFile::Remove(const PFilePath & name, BOOL force)
{
  if (remove(name) == 0)
    return TRUE;
  if (!force || errno != EACCES)
    return FALSE;
  if (_chmod(name, _S_IWRITE) != 0)
    return FALSE;
  return remove(name) == 0;
}


BOOL PFile::Rename(const PFilePath & oldname, const PString & newname, BOOL force)
{
  if (newname.FindOneOf(":\\/") != P_MAX_INDEX) {
#ifdef _WIN32_WCE
    set_errno(EINVAL);
#else
    errno = EINVAL;
#endif // _WIN32_WCE
    return FALSE;
  }
  PString fullname = oldname.GetDirectory() + newname;
  if (rename(oldname, fullname) == 0)
    return TRUE;
  if (!force || errno == ENOENT || !Exists(fullname))
    return FALSE;
  if (!Remove(fullname, TRUE))
    return FALSE;
  return rename(oldname, fullname) == 0;
}


BOOL PFile::Move(const PFilePath & oldname, const PFilePath & newname, BOOL force)
{
  if (rename(oldname, newname) == 0)
    return TRUE;
  if (errno == ENOENT)
    return FALSE;
  if (force && Exists(newname)) {
    if (!Remove(newname, TRUE))
      return FALSE;
    if (rename(oldname, newname) == 0)
      return TRUE;
  }
  return Copy(oldname, newname, force) && Remove(oldname);
}


#ifdef _WIN32_WCE

BOOL PFile::GetInfo(const PFilePath & name, PFileInfo & info)
{
  USES_CONVERSION;
  
  PString fn = name;
  PINDEX pos = fn.GetLength()-1;
  while (PDirectory::IsSeparator(fn[pos]))
    pos--;
  fn.Delete(pos+1, P_MAX_INDEX);
  
  HANDLE hFile = CreateFile(A2T((const char*)fn),0,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if (hFile==INVALID_HANDLE_VALUE) 
    return false;
  
  bool res=false;
  BY_HANDLE_FILE_INFORMATION FInfo;
  if (GetFileInformationByHandle(hFile,&FInfo))
  {
    info.created = FileTimeToTime(FInfo.ftCreationTime);
    info.modified = FileTimeToTime(FInfo.ftLastWriteTime);
    info.accessed = FileTimeToTime(FInfo.ftLastAccessTime);
    info.size = (__int64(FInfo.nFileSizeHigh)<<32)+__int64(FInfo.nFileSizeLow);
    
    info.permissions = PFileInfo::UserRead|PFileInfo::GroupRead|PFileInfo::WorldRead;
    
    if ((FInfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY)==0)
      info.permissions |= PFileInfo::UserWrite|PFileInfo::GroupWrite|PFileInfo::WorldWrite;
    
    if (FInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
    {
      info.type = PFileInfo::SubDirectory;
      info.permissions |= PFileInfo::UserExecute|PFileInfo::GroupExecute|PFileInfo::WorldExecute;
    }
    else
    {
      info.type = PFileInfo::RegularFile;
    }
    info.hidden = (FInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)!=0;
    res=true;
  }
  
  CloseHandle(hFile);
  return res;
}

#else // !_WIN32_WCE

#if defined(_WIN32)

static void TwiddleBits(int newPermissions,
                        int & oldPermissions,
                        int permission,
                        DWORD & mask,
                        DWORD access)
{
  if (newPermissions < 0) {
    if ((mask&access) == access)
      oldPermissions |= permission;
  }
  else {
    if ((newPermissions&permission) != 0)
      mask |= access;
    else
      mask &= ~access;
  }
}


static int FileSecurityPermissions(const PFilePath & filename, int newPermissions)
{
  // All of the following is to support cygwin style permissions

  PBYTEArray storage(sizeof(SECURITY_DESCRIPTOR));
  SECURITY_DESCRIPTOR * descriptor = (SECURITY_DESCRIPTOR *)storage.GetPointer();
  DWORD lengthNeeded = 0;

  if (!GetFileSecurity(filename,
                       DACL_SECURITY_INFORMATION,
                       descriptor,
                       storage.GetSize(),
                       &lengthNeeded)) {
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || lengthNeeded == 0)
      return -1;

    descriptor = (SECURITY_DESCRIPTOR *)storage.GetPointer(lengthNeeded);
    if (!GetFileSecurity(filename,
                         DACL_SECURITY_INFORMATION,
                         descriptor,
                         storage.GetSize(),
                         &lengthNeeded))
      return -1;
  }

  BOOL daclPresent, daclDefaulted;
  PACL dacl;
  if (!GetSecurityDescriptorDacl(descriptor, &daclPresent, &dacl, &daclDefaulted))
    return -1;

  if (!daclPresent || daclDefaulted || dacl == NULL)
    return -1;


  ACL_SIZE_INFORMATION aclSize;
  if (!GetAclInformation(dacl, &aclSize, sizeof(aclSize), AclSizeInformation))
    return -1;

  int oldPermissions = 0;
  int cygwinMask = 0;

  for (DWORD aceIndex = 0; aceIndex< aclSize.AceCount; aceIndex++) {
    LPVOID acePtr;
    GetAce(dacl, aceIndex, &acePtr);
    ACE_HEADER * aceHdr = (ACE_HEADER *)acePtr;
    if (aceHdr->AceType == ACCESS_ALLOWED_ACE_TYPE) {
      ACCESS_ALLOWED_ACE * ace = (ACCESS_ALLOWED_ACE *)acePtr;
      PString account, domain;
      DWORD accountLen = 1000;
      DWORD domainLen = 1000;
      SID_NAME_USE usage;
      if (LookupAccountSid(NULL, &ace->SidStart,
                           account.GetPointer(1000), &accountLen,
                           domain.GetPointer(1000), &domainLen,
                           &usage)) {
        if (account *= "None") {
          cygwinMask |= 2;
          TwiddleBits(newPermissions, oldPermissions, PFileInfo::WorldRead,
                      ace->Mask, FILE_READ_DATA|FILE_READ_ATTRIBUTES|FILE_READ_EA);
          TwiddleBits(newPermissions, oldPermissions, PFileInfo::WorldWrite,
                      ace->Mask, FILE_WRITE_DATA|FILE_APPEND_DATA|FILE_WRITE_ATTRIBUTES|FILE_WRITE_EA);
          TwiddleBits(newPermissions, oldPermissions, PFileInfo::WorldExecute,
                      ace->Mask, FILE_EXECUTE);
        }
        else if (account *= "EVERYONE") {
          cygwinMask |= 1;
          TwiddleBits(newPermissions, oldPermissions, PFileInfo::GroupRead,
                      ace->Mask, FILE_READ_DATA|FILE_READ_ATTRIBUTES|FILE_READ_EA);
          TwiddleBits(newPermissions, oldPermissions, PFileInfo::GroupWrite,
                      ace->Mask, FILE_WRITE_DATA|FILE_APPEND_DATA|FILE_WRITE_ATTRIBUTES|FILE_WRITE_EA);
          TwiddleBits(newPermissions, oldPermissions, PFileInfo::GroupExecute,
                      ace->Mask, FILE_EXECUTE);
        }
        else if (account == PProcess::Current().GetUserName()) {
          cygwinMask |= 4;
          TwiddleBits(newPermissions, oldPermissions, PFileInfo::UserRead,
                      ace->Mask, FILE_READ_DATA|FILE_READ_ATTRIBUTES|FILE_READ_EA);
          TwiddleBits(newPermissions, oldPermissions, PFileInfo::UserWrite,
                      ace->Mask, FILE_WRITE_DATA|FILE_APPEND_DATA|FILE_WRITE_ATTRIBUTES|FILE_WRITE_EA);
          TwiddleBits(newPermissions, oldPermissions, PFileInfo::UserExecute,
                      ace->Mask, FILE_EXECUTE);
        }
      }
    }
  }

  // Only do it if have the three ACE entries as per cygwin
  if (cygwinMask != 7)
    return -1;

  if (newPermissions != -1)
    SetFileSecurity(filename, DACL_SECURITY_INFORMATION, descriptor);

  return oldPermissions;
}

#endif


BOOL PFile::GetInfo(const PFilePath & name, PFileInfo & info)
{
  if (name.IsEmpty())
    return FALSE;

  PString fn = name;
  PINDEX pos = fn.GetLength()-1;
  while (PDirectory::IsSeparator(fn[pos]))
    pos--;
  fn.Delete(pos+1, P_MAX_INDEX);

  struct stat s;
  if (stat(fn, &s) != 0)
    return FALSE;

  info.created =  (s.st_ctime < 0) ? 0 : s.st_ctime;
  info.modified = (s.st_mtime < 0) ? 0 : s.st_mtime;
  info.accessed = (s.st_atime < 0) ? 0 : s.st_atime;
  info.size = s.st_size;

#if defined(_WIN32)
  info.permissions = FileSecurityPermissions(name, -1);
  if (info.permissions < 0)
#endif
  {
    info.permissions = 0;
    if ((s.st_mode&S_IREAD) != 0)
      info.permissions |= PFileInfo::UserRead|PFileInfo::GroupRead|PFileInfo::WorldRead;
    if ((s.st_mode&S_IWRITE) != 0)
      info.permissions |= PFileInfo::UserWrite|PFileInfo::GroupWrite|PFileInfo::WorldWrite;
    if ((s.st_mode&S_IEXEC) != 0)
      info.permissions |= PFileInfo::UserExecute|PFileInfo::GroupExecute|PFileInfo::WorldExecute;
  }

  switch (s.st_mode & S_IFMT) {
    case S_IFREG :
      info.type = PFileInfo::RegularFile;
      break;

    case S_IFDIR :
      info.type = PFileInfo::SubDirectory;
      break;

    default:
      info.type = PFileInfo::UnknownFileType;
      break;
  }

#if defined(_WIN32)
  info.hidden = (GetFileAttributes(name) & FILE_ATTRIBUTE_HIDDEN) != 0;
#else
  unsigned int attr;
  _dos_getfileattr(name, &attr);
  info.hidden = (attr & _A_HIDDEN) != 0;
#endif

  return TRUE;
}

#endif // _WIN32_WCE

BOOL PFile::SetPermissions(const PFilePath & name, int permissions)
{
#if defined(_WIN32) && !defined(_WIN32_WCE)
  FileSecurityPermissions(name, permissions);
#endif

  return _chmod(name, permissions&(_S_IWRITE|_S_IREAD)) == 0;
}


BOOL PFile::IsTextFile() const
{
  return FALSE;
}


BOOL PFile::Open(OpenMode mode, int opts)
{
  Close();
  clear();

  if (path.IsEmpty())
    path = PFilePath("PWL", NULL);

  int oflags = IsTextFile() ? _O_TEXT : _O_BINARY;
  switch (mode) {
    case ReadOnly :
      oflags |= O_RDONLY;
      if (opts == ModeDefault)
        opts = MustExist;
      break;

    case WriteOnly :
      oflags |= O_WRONLY;
      if (opts == ModeDefault)
        opts = Create|Truncate;
      break;

    case ReadWrite :
      oflags |= O_RDWR;
      if (opts == ModeDefault)
        opts = Create;
      break;

    default :
      PAssertAlways(PInvalidParameter);
  }

  if ((opts&Create) != 0)
    oflags |= O_CREAT;
  if ((opts&Exclusive) != 0)
    oflags |= O_EXCL;
  if ((opts&Truncate) != 0)
    oflags |= O_TRUNC;

  if ((opts&Temporary) != 0)
    removeOnClose = TRUE;

  int sflags = _SH_DENYNO;
  if ((opts&DenySharedRead) == DenySharedRead)
    sflags = _SH_DENYRD;
  else if ((opts&DenySharedWrite) == DenySharedWrite)
    sflags = _SH_DENYWR;
  else if ((opts&(DenySharedRead|DenySharedWrite)) != 0)
    sflags = _SH_DENYWR;

  os_handle = _sopen(path, oflags, sflags, S_IREAD|S_IWRITE);

  // As ConvertOSError tests for < 0 and some return values _sopen may be
  // negative, only pass -1 through.
  return ConvertOSError(os_handle == -1 ? -1 : 0);
}


BOOL PFile::SetLength(off_t len)
{
  return ConvertOSError(_chsize(GetHandle(), len));
}


///////////////////////////////////////////////////////////////////////////////
// PTextFile

BOOL PTextFile::IsTextFile() const
{
  return TRUE;
}


BOOL PTextFile::ReadLine(PString & str)
{
  char * ptr = str.GetPointer(100);
  PINDEX len = 0;
  int c;
  while ((c = ReadChar()) >= 0 && c != '\n') {
    *ptr++ = (char)c;
    if (++len >= str.GetSize())
      ptr = str.GetPointer(len + 100) + len;
  }
  *ptr = '\0';
  PAssert(str.MakeMinimumSize(), POutOfMemory);
  return c >= 0 || len > 0;
}


BOOL PTextFile::WriteLine(const PString & str)
{
  return WriteString(str) && WriteChar('\n');
}


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
  return "\\\\.\\Console";
}


BOOL PConsoleChannel::Read(void * buffer, PINDEX length)
{
  flush();
  lastReadCount = _read(os_handle, buffer, length);
  return ConvertOSError(lastReadCount, LastReadError) && lastReadCount > 0;
}


BOOL PConsoleChannel::Write(const void * buffer, PINDEX length)
{
  flush();
  lastWriteCount = _write(os_handle, buffer, length);
  return ConvertOSError(lastWriteCount, LastWriteError) && lastWriteCount >= length;
}


BOOL PConsoleChannel::Close()
{
  os_handle = -1;
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// PProcess

void PProcess::Construct()
{
  PSetErrorStream(&cerr);

#if !defined(_WIN32) && defined(_MSC_VER) && defined(_WINDOWS)
  _wsetscreenbuf(1, _WINBUFINF);
  _wsizeinfo ws;
  ws._version = _QWINVER;
  ws._type = _WINSIZEMAX;
  _wsetsize(1, &ws);
#endif

  houseKeeper = NULL;
}


BOOL PProcess::SetMaxHandles(int /*newLimit*/)
{
  // Not applicable
  return TRUE;
}


//////////////////////////////////////////////////////////////////////////////

#define INCLUDE_STUFF1(cls) \
  cls i##cls; \
  i##cls = i##cls

#define INCLUDE_STUFF2(cls) \
  INCLUDE_STUFF1(cls); \
  i##cls.GetPointer(); \
  i##cls.Attach(0, 0); \
  i##cls.SetAt(0, 0); \
  i##cls.GetAt(0); \
  i##cls[0]

void PDummyFunctionToMakeSureSymbolsAreInDEFFile()
{
  INCLUDE_STUFF2(PCharArray);
  INCLUDE_STUFF2(PShortArray);
  INCLUDE_STUFF2(PIntArray);
  INCLUDE_STUFF2(PLongArray);
  INCLUDE_STUFF2(PBYTEArray);
  INCLUDE_STUFF2(PWORDArray);
  INCLUDE_STUFF2(PUnsignedArray);
  INCLUDE_STUFF2(PDWORDArray);
  INCLUDE_STUFF1(PStringSet);
  INCLUDE_STUFF1(POrdinalToString);
  INCLUDE_STUFF1(PStringToOrdinal);
  INCLUDE_STUFF1(PStringToString);
}


// End Of File ///////////////////////////////////////////////////////////////
