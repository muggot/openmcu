/*
 * doswin.cxx
 *
 * 16 bit implementation for MS-DOS and 16 bit Windows.
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
 * $Log: doswin.cxx,v $
 * Revision 1.10  1998/09/24 03:30:43  robertj
 * Added open software license.
 *
 * Revision 1.9  1996/01/02 12:55:15  robertj
 * Fixed copy of directories.
 *
 * Revision 1.8  1995/12/10 11:56:42  robertj
 * Moved error code for specific WIN32 and MS-DOS versions.
 *
 * Revision 1.7  1995/08/24 12:41:10  robertj
 * Changed PChannel so not a PContainer.
 *
 * Revision 1.6  1995/07/31 12:14:52  robertj
 * Added semaphore class.
 *
 * Revision 1.5  1995/06/17 00:59:18  robertj
 * Moved PPipeChannel::Execute from common dos/windows to individual files.
 *
 * Revision 1.4  1995/04/01 08:05:59  robertj
 * Fixed yield for straight DOS and QUICKWIN systems.
 *
 * Revision 1.3  1995/03/25 02:09:11  robertj
 * Added check for network login name.
 *
// Revision 1.2  1995/03/14  13:31:36  robertj
// Implemented DOS pipe channel.
//
// Revision 1.1  1995/03/14  12:45:16  robertj
// Initial revision
//
 */

#include "ptlib.h"

#include <fcntl.h>
#include <sys/stat.h>


///////////////////////////////////////////////////////////////////////////////
// Directories

void PDirectory::Construct()
{
  PString::operator=(CreateFullPath(*this, TRUE));
}


void PDirectory::CopyContents(const PDirectory & dir)
{
  scanMask = dir.scanMask;
  fileinfo = dir.fileinfo;
}


BOOL PDirectory::Open(int newScanMask)
{
  scanMask = newScanMask;

  if (_dos_findfirst(*this+"*.*", 0xff, &fileinfo) != 0)
    return FALSE;

  return Filtered() ? Next() : TRUE;
}


BOOL PDirectory::Next()
{
  do {
    if (_dos_findnext(&fileinfo) != 0)
      return FALSE;
  } while (Filtered());

  return TRUE;
}


PCaselessString PDirectory::GetEntryName() const
{
  return fileinfo.name;
}


BOOL PDirectory::IsSubDir() const
{
  return (fileinfo.attrib&_A_SUBDIR) != 0;
}


void PDirectory::Close()
{
  /* do nothing */
}


PCaselessString PDirectory::GetVolume() const
{
  struct find_t finf;
  if (_dos_findfirst(Left(3) + "*.*", _A_VOLID, &finf) != 0)
    return PCaselessString();
  return finf.name;
}


PString PDirectory::CreateFullPath(const PString & path, BOOL isDirectory)
{
  PString curdir;
  PAssert(getcwd(curdir.GetPointer(P_MAX_PATH),
                                   P_MAX_PATH) != NULL, POperatingSystemError);

  PString fullpath;

  PINDEX offset;
  if (path.GetLength() < 2 || path[1] != ':') {
    fullpath = curdir(0,1);
    offset = 0;
  }
  else {
    fullpath = path(0,1).ToUpper();
    offset = 2;
  }

  char slash = path[offset];
  if (slash != '\\' && slash != '/') {
    if (fullpath[0] == curdir[0])
      fullpath += curdir(2, P_MAX_INDEX);
    else if (_chdrive(fullpath[0]-'A'+1) == 0) {
      PString otherdir;
      PAssert(getcwd(otherdir.GetPointer(P_MAX_PATH),
                                   P_MAX_PATH) != NULL, POperatingSystemError);
      fullpath += otherdir(2, P_MAX_INDEX);
      _chdrive(curdir[0]-'A'+1);  // Put drive back
    }
    slash = fullpath[fullpath.GetLength()-1];
    if (slash != '\\' && slash != '/')
      fullpath += "\\";
  }

  fullpath += path(offset, P_MAX_INDEX);

  slash = fullpath[fullpath.GetLength()-1];
  if (isDirectory && slash != '\\' && slash != '/')
    fullpath += "\\";

  int pos;
  while ((pos = fullpath.Find('/')) != P_MAX_INDEX)
    fullpath[pos] = '\\';

  while ((pos = fullpath.Find("\\.\\")) != P_MAX_INDEX)
    fullpath = fullpath(0, pos) + fullpath(pos+3, P_MAX_INDEX);

  while ((pos = fullpath.Find("\\..\\")) != P_MAX_INDEX)
    fullpath = fullpath(0, fullpath.FindLast('\\', pos-1)) +
                                                  fullpath(pos+4, P_MAX_INDEX);

  return fullpath.ToUpper();
}


///////////////////////////////////////////////////////////////////////////////
// PChannel

PString PChannel::GetErrorText() const
{
  if (osError == 0)
    return PString();

  if (osError > 0 && osError < _sys_nerr && _sys_errlist[osError][0] != '\0')
    return _sys_errlist[osError];

  return psprintf("OS error %u", osError);
}


BOOL PChannel::ConvertOSError(int error)
{
  if (error >= 0) {
    lastError = NoError;
    osError = 0;
    return TRUE;
  }

  osError = errno;
  switch (osError) {
    case 0 :
      lastError = NoError;
      return TRUE;
    case ENOENT :
      lastError = NotFound;
      break;
    case EEXIST :
      lastError = FileExists;
      break;
    case EACCES :
      lastError = AccessDenied;
      break;
    case ENOMEM :
      lastError = NoMemory;
      break;
    case ENOSPC :
      lastError = DiskFull;
      break;
    case EINVAL :
      lastError = BadParameter;
      break;
    case EBADF :
      lastError = NotOpen;
      break;
    default :
      lastError = Miscellaneous;
  }

  return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// PPipeChannel

void PPipeChannel::Construct(const PString & subProgram,
                const char * const * arguments, OpenMode mode, BOOL searchPath)
{
  hasRun = FALSE;

  if (searchPath || subProgram.FindOneOf(":\\/") != P_MAX_INDEX)
    subProgName = subProgram;
  else
    subProgName = ".\\" + subProgram;
  if (arguments != NULL) {
    while (*arguments != NULL) {
      subProgName += " ";
      if (strchr(*arguments, ' ') == NULL)
        subProgName += *arguments;
      else {
        PString quote = '"';
        subProgName += quote + *arguments + quote;
      }
    }
  }
  
  if (mode != ReadOnly) {
    toChild = PFilePath("pw", NULL);
    os_handle = _open(toChild, _O_WRONLY|_O_CREAT|_O_BINARY,S_IREAD|S_IWRITE);
    if (!ConvertOSError(os_handle))
      return;
    subProgName += '<' + toChild;
  }

  if (mode != WriteOnly) {
    fromChild = PFilePath("pw", NULL);
    subProgName += '>' + fromChild;
  }

  if (mode == ReadOnly)
    Execute();
}


PPipeChannel::~PPipeChannel()
{
  Close();
}


BOOL PPipeChannel::Read(void * buffer, PINDEX amount)
{
  if (!hasRun)
    Execute();

  flush();
  lastReadCount = _read(GetHandle(), buffer, amount);
  return ConvertOSError(lastReadCount) && lastReadCount > 0;
}
      

BOOL PPipeChannel::Write(const void * buffer, PINDEX amount)
{
  if (hasRun) {
    osError = EBADF;
    lastError = NotOpen;
    return FALSE;
  }

  flush();
  lastWriteCount = _write(GetHandle(), buffer, amount);
  return ConvertOSError(lastWriteCount) && lastWriteCount >= amount;
}


BOOL PPipeChannel::Close()
{
  if (!hasRun)
    Execute();

  if (os_handle >= 0)
    _close(os_handle);

  PFile::Remove(toChild);
  PFile::Remove(fromChild);
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// PThread

PThread::~PThread()
{
  Terminate();
  _nfree(stackBase);   // Give stack back to the near heap
}


void PThread::Block(BlockFunction isBlockFun, PObject * obj)
{
  isBlocked = isBlockFun;
  blocker = obj;
  status = BlockedIO;
  Yield();
}


///////////////////////////////////////////////////////////////////////////////
// PProcess

void PProcess::OperatingSystemYield()
{
#ifdef P_QUICKWIN
  _wyield();
#endif
}


PString PProcess::GetUserName() const
{
  /* ----- Microsoft LAN Manager, Windows for Workgroups, IBM LAN Server ----- */
#pragma pack(1)
  static struct {
    char _far *computername;
    char _far *username;
    char _far *langroup;
    unsigned char ver_major;
    unsigned char ver_minor;
    char _far *logon_domain;
    char _far *oth_domains;
    char filler[32];
  } NEAR wksta;
#pragma pack()

  union REGS r;
  r.x.ax = 0x5F44;
  r.x.bx = 10;
  r.x.cx = sizeof(wksta);
  r.x.di = (WORD)&wksta;

  struct SREGS sregs;
  segread(&sregs);
  sregs.es = sregs.ds;
  int86x(0x21, &r, &r, &sregs);
  if (r.x.ax == 0 || r.x.ax == 0x5F44) {
    char name[32];
    strcpy(name, wksta.username);
    strlwr(name);
    return name;
  }


  /* ----- Novell NetWare ----- Get Connection Information E3(16) */

#pragma pack(1)
  static struct {
    unsigned short len;
    unsigned char func;
    unsigned char number;
  } NEAR gcireq;
  
  static struct {
    unsigned short len;
    unsigned long objectID;
    unsigned short objecttype;
    char objectname[48];
    unsigned char logintime[7];
    unsigned char reserved[39];
  } NEAR gcirep;
#pragma pack()

  /* Load Get Connection Number function code.   */
  r.x.ax = 0xDC00;
  int86x(0x21, &r, &r, &sregs);
  if (r.h.al > 0 && r.h.al <= 100) {
    /* If the connection number is in range 1-100,
     * invoke Get Connection Information to get the user name. */

    gcireq.len = sizeof(gcireq) - sizeof(gcireq.len);
    gcireq.func = 0x16;
    gcireq.number = r.h.al;
    gcirep.len = sizeof(gcirep) - sizeof(gcirep.len);

    r.h.ah = 0xE3;
    r.x.si = (unsigned short) &gcireq;
    r.x.di = (unsigned short) &gcirep;
    int86x(0x21, &r, &r, &sregs);
    if (r.h.al == 0) {
      strlwr(gcirep.objectname);
      return gcirep.objectname;
    }
  }


  /* Give up and use environment variables */
  const char * username = getenv("LOGNAME");
  if (username == NULL) {
    username = getenv("USER");
    if (username == NULL)
      username = "";
  }
  
  PAssert(*username != '\0', "Cannot determine user name, set LOGNAME.");
  return username;
}


// End Of File ///////////////////////////////////////////////////////////////
