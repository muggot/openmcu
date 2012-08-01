/*
 * pipe.cxx
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
 * $Log: pipe.cxx,v $
 * Revision 1.6  2007/10/03 01:18:47  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.5  2004/10/23 10:51:59  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.4  2004/04/03 06:54:30  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.3  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.2  1998/11/30 07:37:56  robertj
 * Fixed (previous bug) of incorrect handle values.
 *
 * Revision 1.1  1998/11/30 04:57:42  robertj
 * Initial revision
 *
 */

#include <ptlib.h>
#include <ptlib/pipechan.h>

#include <ctype.h>

///////////////////////////////////////////////////////////////////////////////
// PPipeChannel

PPipeChannel::PPipeChannel()
{
  hToChild = hFromChild = hStandardError = INVALID_HANDLE_VALUE;
}


#ifdef _WIN32_WCE
BOOL PPipeChannel::PlatformOpen(const PString &, const PStringArray &, OpenMode, BOOL, BOOL, const PStringToString *)
{
  return FALSE;
}
#else
BOOL PPipeChannel::PlatformOpen(const PString & subProgram,
                                const PStringArray & argumentList,
                                OpenMode mode,
                                BOOL searchPath,
                                BOOL stderrSeparate,
                                const PStringToString * environment)
{
  subProgName = subProgram;

  const char * prog = NULL;
  PStringStream cmdLine;
  if (searchPath)
    cmdLine << subProgram;
  else
    prog = subProgram;

  for (PINDEX i = 0; i < argumentList.GetSize(); i++) {
    cmdLine << ' ';
    if (argumentList[i].Find(' ') == P_MAX_INDEX)
      cmdLine << argumentList[i];
    else if (argumentList[i].Find('"') == P_MAX_INDEX)
      cmdLine << '"' << argumentList[i] << '"';
    else
      cmdLine << '\'' << argumentList[i] << '\'';
  }

  PCharArray envBuf;
  char * envStr = NULL;
  if (environment != NULL) {
    PINDEX size = 0;
    for (PINDEX e = 0; e < environment->GetSize(); e++) {
      PString str = environment->GetKeyAt(e) + '=' + environment->GetDataAt(e);
      PINDEX len = str.GetLength() + 1;
      envBuf.SetSize(size + len);
      memcpy(envBuf.GetPointer()+size, (const char *)str, len);
      size += len;
    }
    envStr = envBuf.GetPointer();
  }

  STARTUPINFO startup;
  memset(&startup, 0, sizeof(startup));
  startup.cb = sizeof(startup);
  startup.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
  startup.wShowWindow = SW_HIDE;
  startup.hStdInput = INVALID_HANDLE_VALUE;
  startup.hStdOutput = INVALID_HANDLE_VALUE;
  startup.hStdError = INVALID_HANDLE_VALUE;

  SECURITY_ATTRIBUTES security;
  security.nLength = sizeof(security);
  security.lpSecurityDescriptor = NULL;
  security.bInheritHandle = TRUE;

  if (mode == ReadOnly)
    hToChild = INVALID_HANDLE_VALUE;
  else {
    HANDLE writeEnd;
    PAssertOS(CreatePipe(&startup.hStdInput, &writeEnd, &security, 0));
    PAssertOS(DuplicateHandle(GetCurrentProcess(), writeEnd,
                              GetCurrentProcess(), &hToChild, 0, FALSE,
                              DUPLICATE_CLOSE_SOURCE|DUPLICATE_SAME_ACCESS));
  }

  if (mode == WriteOnly)
    hFromChild = INVALID_HANDLE_VALUE;
  else if (mode == ReadWriteStd) {
    hFromChild = INVALID_HANDLE_VALUE;
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  }
  else {
    PAssertOS(CreatePipe(&hFromChild, &startup.hStdOutput, &security, 0));
    if (stderrSeparate)
      PAssertOS(CreatePipe(&hStandardError, &startup.hStdError, &security, 0));
    else {
      startup.hStdError = startup.hStdOutput;
      hStandardError = INVALID_HANDLE_VALUE;
    }
  }

  if (ConvertOSError(CreateProcess(prog, cmdLine.GetPointer(),
                                   NULL, NULL, TRUE, 0, envStr,
                                   NULL, &startup, &info) ? 0 : -2))
    os_handle = info.dwProcessId;
  else {
    if (hToChild != INVALID_HANDLE_VALUE)
      CloseHandle(hToChild);
    if (hFromChild != INVALID_HANDLE_VALUE)
      CloseHandle(hFromChild);
    if (hStandardError != INVALID_HANDLE_VALUE)
      CloseHandle(hStandardError);
  }

  if (startup.hStdInput != INVALID_HANDLE_VALUE)
    CloseHandle(startup.hStdInput);

  if (mode != ReadWriteStd) {
    if (startup.hStdOutput != INVALID_HANDLE_VALUE)
      CloseHandle(startup.hStdOutput);
    if (startup.hStdOutput != startup.hStdError)
      CloseHandle(startup.hStdError);
  }

  return IsOpen();
}
#endif // !_WIN32_WCE



PPipeChannel::~PPipeChannel()
{
  Close();
}


BOOL PPipeChannel::IsOpen() const
{
  return os_handle != -1;
}


int PPipeChannel::GetReturnCode() const
{
  DWORD code;
  if (GetExitCodeProcess(info.hProcess, &code) && (code != STILL_ACTIVE))
    return code;

  ((PPipeChannel*)this)->ConvertOSError(-2);
  return -1;
}


BOOL PPipeChannel::CanReadAndWrite()
{
  return TRUE;
}

BOOL PPipeChannel::IsRunning() const
{
  DWORD code;
  return GetExitCodeProcess(info.hProcess, &code) && (code == STILL_ACTIVE);
}


int PPipeChannel::WaitForTermination()
{
  if (WaitForSingleObject(info.hProcess, INFINITE) == WAIT_OBJECT_0)
    return GetReturnCode();

  ConvertOSError(-2);
  return -1;
}


int PPipeChannel::WaitForTermination(const PTimeInterval & timeout)
{
  if (WaitForSingleObject(info.hProcess, timeout.GetInterval()) == WAIT_OBJECT_0)
    return GetReturnCode();

  ConvertOSError(-2);
  return -1;
}


BOOL PPipeChannel::Kill(int signal)
{
  return ConvertOSError(TerminateProcess(info.hProcess, signal) ? 0 : -2);
}


BOOL PPipeChannel::Read(void * buffer, PINDEX len)
{
  lastReadCount = 0;
  DWORD count;
  if (!ConvertOSError(ReadFile(hFromChild, buffer, len, &count, NULL) ? 0 :-2, LastReadError))
    return FALSE;
  lastReadCount = count;
  return lastReadCount > 0;
}
      

BOOL PPipeChannel::Write(const void * buffer, PINDEX len)
{
  lastWriteCount = 0;
  DWORD count;
  if (!ConvertOSError(WriteFile(hToChild, buffer, len, &count, NULL) ? 0 : -2, LastWriteError))
    return FALSE;
  lastWriteCount = count;
  return lastWriteCount >= len;
}


BOOL PPipeChannel::Close()
{
  if (IsOpen()) {
    os_handle = -1;
    if (hToChild != INVALID_HANDLE_VALUE)
      CloseHandle(hToChild);
    if (hFromChild != INVALID_HANDLE_VALUE)
      CloseHandle(hFromChild);
    if (hStandardError != INVALID_HANDLE_VALUE)
      CloseHandle(hStandardError);
    if (!TerminateProcess(info.hProcess, 1))
      return FALSE;
  }
  return TRUE;
}


BOOL PPipeChannel::Execute()
{
  flush();
  clear();
  if (hToChild != INVALID_HANDLE_VALUE)
    CloseHandle(hToChild);
  hToChild = INVALID_HANDLE_VALUE;
  return TRUE;
}


#ifdef _WIN32_WCE
BOOL PPipeChannel::ReadStandardError(PString &, BOOL)
{
  return FALSE;
}
#else
BOOL PPipeChannel::ReadStandardError(PString & errors, BOOL wait)
{
  DWORD available, bytesRead;
  if (!PeekNamedPipe(hStandardError, NULL, 0, NULL, &available, NULL))
    return ConvertOSError(-2, LastReadError);

  if (available != 0)
    return ConvertOSError(ReadFile(hStandardError,
                          errors.GetPointer(available+1), available,
                          &bytesRead, NULL) ? 0 : -2, LastReadError);

  if (wait)
    return FALSE;

  char firstByte;
  if (!ReadFile(hStandardError, &firstByte, 1, &bytesRead, NULL))
    return ConvertOSError(-2, LastReadError);

  errors = firstByte;

  if (!PeekNamedPipe(hStandardError, NULL, 0, NULL, &available, NULL))
    return ConvertOSError(-2, LastReadError);

  if (available == 0)
    return TRUE;

  return ConvertOSError(ReadFile(hStandardError,
                        errors.GetPointer(available+2)+1, available,
                        &bytesRead, NULL) ? 0 : -2, LastReadError);
}
#endif // !_WIN32_WCE


// End Of File ///////////////////////////////////////////////////////////////
