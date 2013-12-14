/*
 * svcproc.cxx
 *
 * Service process implementation for Win95 and WinNT
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
 * $Log: svcproc.cxx,v $
 * Revision 1.90  2007/09/30 17:34:40  dsandras
 * Killed GCC 4.2 warnings.
 *
 * Revision 1.89  2007/09/20 05:43:25  rjongbloed
 * Added ability to log output to debugger.
 *
 * Revision 1.88  2007/09/20 05:31:53  rjongbloed
 * Added ability to log output to debugger.
 *
 * Revision 1.87  2007/09/17 11:14:47  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.86  2007/09/08 11:34:29  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.85  2007/08/15 17:45:37  shorne
 * Fix so PServiceProcess can run other than in Debug Mode
 *
 * Revision 1.84  2006/10/11 01:04:59  csoutheren
 * Applied path 1549187 - Fixed problem with PServiceProcess under Windows
 * Thanks to Borko Jandras
 *
 * Revision 1.83  2006/06/25 11:22:57  csoutheren
 * Add pragmas to automate linking with VS 2005
 *
 * Revision 1.82  2006/06/21 04:20:07  csoutheren
 * Fixes for VS.net
 *
 * Revision 1.81  2006/04/09 11:03:59  csoutheren
 * Remove warnings on VS.net 2005
 *
 * Revision 1.80  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.79  2005/10/21 06:03:51  csoutheren
 * Fixed warning on VS.NET 2005
 *
 * Revision 1.78  2004/06/01 05:22:44  csoutheren
 * Restored memory check functionality
 *
 * Revision 1.77  2004/04/03 08:22:22  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.76  2004/04/03 06:54:30  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.75  2004/03/20 09:08:15  rjongbloed
 * Changed interaction between PTrace and PSystemLog so that the tracing code does
 *   not need to know about the system log, thus reducing the code footprint for most apps.
 *
 * Revision 1.74  2003/09/17 09:02:14  csoutheren
 * Removed memory leak detection code
 *
 * Revision 1.73  2003/09/17 05:45:10  csoutheren
 * Removed recursive includes
 *
 * Revision 1.72  2003/02/11 07:58:40  robertj
 * Added ignore allocations so don't get meaningless memory leak at end of run
 *   cause by system log stream not being deleted, thanks Sebastian Meyer
 *
 * Revision 1.71  2002/11/01 05:16:41  robertj
 * Fixed additional debug levels in PSystemLog to text file.
 *
 * Revision 1.70  2002/09/23 07:17:24  robertj
 * Changes to allow winsock2 to be included.
 *
 * Revision 1.69  2002/03/10 15:37:35  robertj
 * Added milliseconds to timestamp output
 *
 * Revision 1.68  2001/12/13 09:20:57  robertj
 * Fixed system log so does not crash if current thread not created by PWLib.
 *
 * Revision 1.67  2001/09/13 00:23:33  robertj
 * Fixed problem where system log output can occur with no current thread.
 *
 * Revision 1.66  2001/06/02 01:33:29  robertj
 * Thread name is always presend now so always use it in system log
 *    output  and make it wider in text output and tab area in window.
 *
 * Revision 1.65  2001/04/18 00:19:07  robertj
 * Removed newline from error code string in NT event.
 *
 * Revision 1.64  2001/04/16 23:04:25  craigs
 * Fixed problem with unknown command causing page fault due to missing comma in command list
 *
 * Revision 1.63  2001/04/12 01:34:02  robertj
 * Added threads to NT event log.
 *
 * Revision 1.62  2001/03/24 05:53:12  robertj
 * Added Windows 98 and ME to GetOSName()
 *
 * Revision 1.61  2001/03/24 05:37:01  robertj
 * Changed default directory of log file to same as executable.
 * Change tray icon to wait for service stop before displaying message.
 * Changed tray icon message display to not have date and thread.
 *
 * Revision 1.60  2001/03/23 05:35:34  robertj
 * Added ability for a service to output trace/system log to file while in debug mode.
 * Added saving of debug window position.
 *
 * Revision 1.59  2001/02/15 01:12:15  robertj
 * Moved some often repeated HTTP service code into PHTTPServiceProcess.
 *
 * Revision 1.58  2001/02/13 03:30:22  robertj
 * Added function to do heap validation.
 *
 * Revision 1.57  2000/05/02 03:16:46  robertj
 * Added display of thread name in SystemLog, thanks Ashley Unitt.
 *
 * Revision 1.56  1999/09/13 13:15:08  robertj
 * Changed PTRACE so will output to system log in PServiceProcess applications.
 *
 * Revision 1.55  1999/08/07 01:43:41  robertj
 * Added "NoWin" option to prevent display of window in command line commands.
 *
 * Revision 1.54  1999/07/16 03:22:16  robertj
 * Fixed tray icon version command so does not ask question.
 *
 * Revision 1.53  1999/06/14 07:59:39  robertj
 * Enhanced tracing again to add options to trace output (timestamps etc).
 *
 * Revision 1.52  1999/04/21 01:57:09  robertj
 * Added confirmation dialog to menu commands.
 *
 * Revision 1.51  1999/03/09 10:30:19  robertj
 * Fixed ability to have PMEMORY_CHECK on/off on both debug/release versions.
 *
 * Revision 1.50  1999/02/16 08:08:07  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.49  1999/01/29 12:20:19  robertj
 * Changed service process to output trace info to the Win32 debug output.
 *
 * Revision 1.48  1998/12/04 10:10:45  robertj
 * Added virtual for determining if process is a service. Fixes linkage problem.
 *
 * Revision 1.47  1998/11/30 04:50:17  robertj
 * New directory structure
 *
 * Revision 1.46  1998/10/18 14:28:34  robertj
 * Renamed argv/argc to eliminate accidental usage.
 * Fixed strange problem withs etting debug window tabstops in optimised version.
 *
 * Revision 1.45  1998/10/13 14:14:09  robertj
 * Added thread ID to log.
 * Added heap debug display to service menus.
 *
 * Revision 1.44  1998/09/24 03:30:57  robertj
 * Added open software license.
 *
 * Revision 1.43  1998/08/20 06:06:03  robertj
 * Fixed bug where web page can be asked for when service not running.
 *
 * Revision 1.42  1998/05/21 04:29:44  robertj
 * Fixed "Proxies stopped" dialog appearing when shutting down windows.
 *
 * Revision 1.41  1998/05/07 05:21:38  robertj
 * Improved formatting of debug window, adding tabs and tab stops.
 *
 * Revision 1.40  1998/04/07 13:32:14  robertj
 * Changed startup code to support PApplication class.
 *
 * Revision 1.39  1998/04/01 01:52:53  robertj
 * Fixed problem with NoAutoDelete threads.
 *
 * Revision 1.38  1998/03/29 06:16:53  robertj
 * Rearranged initialisation sequence so PProcess descendent constructors can do "things".
 *
 * Revision 1.37  1998/03/20 03:20:45  robertj
 * Lined up debug output.
 *
 * Revision 1.36  1998/03/05 12:49:55  robertj
 * MemCheck fixes.
 *
 * Revision 1.35  1998/02/20 23:01:10  robertj
 * Fixed bug where application exits on log out in win95.
 *
 * Revision 1.34  1998/02/16 01:43:57  robertj
 * Really fixed spurious error display on install/start/stop etc
 *
 * Revision 1.33  1998/02/16 00:12:22  robertj
 * Added tray icon support.
 * Fixed problem with services and directory paths with spaces in them.
 *
 * Revision 1.32  1998/02/03 06:16:31  robertj
 * Added extra log levels.
 * Fixed bug where window disappears after debug service termination.
 *
 * Revision 1.31  1998/01/26 00:56:11  robertj
 * Changed ServiceProcess to exclusively use named event to detect running process.
 *
 * Revision 1.30  1997/12/18 05:05:45  robertj
 * Added Edit menu.
 *
 * Revision 1.29  1997/11/04 06:01:45  robertj
 * Fix of "service hung at startup" message for NT service.
 *
 * Revision 1.28  1997/10/30 10:17:10  robertj
 * Fixed bug in detection of running service.
 *
 * Revision 1.27  1997/10/03 15:14:17  robertj
 * Fixed crash on exit.
 *
 * Revision 1.26  1997/08/28 12:50:32  robertj
 * Fixed race condition in cleaning up threads on application termination.
 *
 * Revision 1.25  1997/07/17 12:43:29  robertj
 * Fixed bug for auto-start of service under '95.
 *
 * Revision 1.24  1997/07/14 11:47:20  robertj
 * Added "const" to numerous variables.
 *
 * Revision 1.23  1997/07/08 13:00:30  robertj
 * DLL support.
 * Fixed '95 support so service runs without logging in.
 *
 * Revision 1.22  1997/04/27 05:50:27  robertj
 * DLL support.
 *
 * Revision 1.21  1997/03/18 21:23:27  robertj
 * Fix service manager falsely accusing app of crashing if OnStart() is slow.
 *
 * Revision 1.20  1997/02/05 11:50:40  robertj
 * Changed current process function to return reference and validate objects descendancy.
 * Changed log file name calculation to occur only once.
 * Added some MSVC memory debugging functions.
 *
 * Revision 1.19  1996/12/05 11:53:49  craigs
 * Fixed failure to output PError to debug window if CRLF pairs used
 *
 * Revision 1.18  1996/11/30 12:07:19  robertj
 * Changed service creation for NT so is auto-start,
 *
 * Revision 1.17  1996/11/18 11:32:04  robertj
 * Fixed bug in doing a "stop" command closing ALL instances of service.
 *
 * Revision 1.16  1996/11/12 10:15:16  robertj
 * Fixed bug in NT 3.51 locking up when needs to output to window.
 *
 * Revision 1.15  1996/11/10 21:04:32  robertj
 * Added category names to event log.
 * Fixed menu enables for debug and command modes.
 *
 * Revision 1.14  1996/11/04 03:39:13  robertj
 * Improved detection of running service so debug mode cannot run.
 *
 * Revision 1.13  1996/10/31 12:54:01  robertj
 * Fixed bug in window not being displayed when command line used.
 *
 * Revision 1.12  1996/10/18 11:22:14  robertj
 * Fixed problems with window not being shown under NT.
 *
 * Revision 1.11  1996/10/14 03:09:58  robertj
 * Fixed major bug in debug outpuit locking up (infinite loop)
 * Changed menus so cannot start service if in debug mode
 *
 * Revision 1.10  1996/10/08 13:04:43  robertj
 * Rewrite to use standard window isntead of console window.
 *
 * Revision 1.9  1996/09/16 12:56:27  robertj
 * DLL support
 *
 * Revision 1.8  1996/09/14 12:34:23  robertj
 * Fixed problem with spontaneous exit from app under Win95.
 *
 * Revision 1.7  1996/08/19 13:36:03  robertj
 * Added "Debug" level to system log.
 *
 * Revision 1.6  1996/07/30 12:23:32  robertj
 * Added better service running test.
 * Changed SIGINTR handler to just set termination event.
 *
 * Revision 1.5  1996/07/27 04:07:57  robertj
 * Changed thread creation to use C library function instead of direct WIN32.
 * Changed SystemLog to be stream based rather than printf based.
 * Fixed Win95 support for service start/stop and prevent multiple starts.
 *
 * Revision 1.4  1996/06/10 09:54:08  robertj
 * Fixed Win95 service install bug (typo!)
 *
 * Revision 1.3  1996/05/30 11:49:10  robertj
 * Fixed crash on exit bug.
 *
 * Revision 1.2  1996/05/23 10:03:21  robertj
 * Windows 95 support.
 *
 * Revision 1.1  1996/05/15 21:11:51  robertj
 * Initial revision
 *
 */

#include <ptlib.h>

#include <winuser.h>
#include <winnls.h>
#include <shellapi.h>
#include <commdlg.h>

#include <process.h>
#include <signal.h>
#include <fcntl.h>
#include <io.h>

#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"comdlg32.lib")


#ifdef __USE_STL__
#include <fstream>
#else
#include <fstream.h>
#endif

#include <ptlib/svcproc.h>


#define new PNEW


#define UWM_SYSTRAY (WM_USER + 1)
#define ICON_RESID 1
#define SYSTRAY_ICON_ID 1

static HINSTANCE hInstance;

#define DATE_WIDTH    72
#define THREAD_WIDTH  80
#define LEVEL_WIDTH   32
#define PROTO_WIDTH   40
#define ACTION_WIDTH  48

enum {
  SvcCmdTray,
  SvcCmdNoTray,
  SvcCmdVersion,
  SvcCmdInstall,
  SvcCmdRemove,
  SvcCmdStart,
  SvcCmdStop,
  SvcCmdPause,
  SvcCmdResume,
  SvcCmdDeinstall,
  SvcCmdNoWindow,
  NumSvcCmds
};

static const char * const ServiceCommandNames[NumSvcCmds] = {
  "Tray",
  "NoTray",
  "Version",
  "Install",
  "Remove",
  "Start",
  "Stop",
  "Pause",
  "Resume",
  "Deinstall",
  "NoWin"
};


static const char WindowLogOutput[] = "Window Log Output";
static const char DebuggerLogOutput[] = "Debugger Log Output";


class PNotifyIconData : public NOTIFYICONDATA {
  public:
    PNotifyIconData(HWND hWnd, UINT flags, const char * tip = NULL);
    void Add()    { Shell_NotifyIcon(NIM_ADD,    this); }
    void Delete() { Shell_NotifyIcon(NIM_DELETE, this); }
    void Modify() { Shell_NotifyIcon(NIM_MODIFY, this); }
};


PNotifyIconData::PNotifyIconData(HWND window, UINT flags, const char * tip)
{
  cbSize = sizeof(NOTIFYICONDATA);
  hWnd   = window;
  uID    = SYSTRAY_ICON_ID;
  uFlags = flags;
  if (tip != NULL) {
    strncpy(szTip, tip, sizeof(szTip)-1);
    szTip[sizeof(szTip)-1] = '\0';
    uFlags |= NIF_TIP;
  }
}


enum TrayIconRegistryCommand {
  AddTrayIcon,
  DelTrayIcon,
  CheckTrayIcon
};

static BOOL TrayIconRegistry(PServiceProcess * svc, TrayIconRegistryCommand cmd)
{
  HKEY key;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                   "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                   0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS)
    return FALSE;

  DWORD err = 1;
  DWORD type;
  DWORD len;
  PString str;
  switch (cmd) {
    case CheckTrayIcon :
      err = RegQueryValueEx(key, svc->GetName(), 0, &type, NULL, &len);
      break;

    case AddTrayIcon :
      str = "\"" + svc->GetFile() + "\" Tray";
      err = RegSetValueEx(key, svc->GetName(), 0, REG_SZ,
                         (LPBYTE)(const char *)str, str.GetLength() + 1);
      break;

    case DelTrayIcon :
      err = RegDeleteValue(key, (char *)(const char *)svc->GetName());
  }

  RegCloseKey(key);
  return err == ERROR_SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////
// PSystemLog

void PSystemLog::Output(Level level, const char * msg)
{
  PServiceProcess & process = PServiceProcess::Current();
  if (level > process.GetLogLevel())
    return;

  DWORD err = ::GetLastError();

  if (process.isWin95 || process.controlWindow != NULL) {
    static HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
    WaitForSingleObject(mutex, INFINITE);

    ostream * out;
    if (process.systemLogFileName == WindowLogOutput || process.systemLogFileName == DebuggerLogOutput)
      out = new PStringStream;
    else
      out = new ofstream((const char *)process.systemLogFileName, ios::app);

    PTime now;
    *out << now.AsString("yyyy/MM/dd hh:mm:ss.uuu\t");
    PThread * thread = PThread::Current();
    if (thread == NULL)
      *out << "ThreadID=0x"
           << setfill('0') << hex
           << setw(8) << GetCurrentThreadId()
           << setfill(' ') << dec;
    else {
      PString threadName = thread->GetThreadName();
      if (threadName.GetLength() <= 23)
        *out << setw(23) << threadName;
      else
        *out << threadName.Left(10) << "..." << threadName.Right(10);
    }

    *out << '\t';
    if (level < 0)
      *out << "Message";
    else {
      static const char * const levelName[4] = {
        "Fatal error",
        "Error",
        "Warning",
        "Info"
      };
      if (level < PARRAYSIZE(levelName))
        *out << levelName[level];
      else
        *out << "Debug" << (level-Info);
    }

    *out << '\t' << msg;
    if (level < Info && err != 0)
      *out << " - error = " << err << endl;
    else if (msg[0] == '\0' || msg[strlen(msg)-1] != '\n')
      *out << endl;

    if (process.systemLogFileName == WindowLogOutput)
      process.DebugOutput(*(PStringStream*)out);
    else if (process.systemLogFileName == DebuggerLogOutput)
      OutputDebugStringA(*(PStringStream*)out);

    delete out;
    ReleaseMutex(mutex);
    SetLastError(0);
  }
  else {
    // Use event logging to log the error.
    HANDLE hEventSource = RegisterEventSource(NULL, process.GetName());
    if (hEventSource == NULL)
      return;

    PString threadName;
    PThread * thread = PThread::Current();
    if (thread != NULL)
      threadName = thread->GetThreadName();
    else
      threadName.sprintf("%u", GetCurrentThreadId());

    char thrdbuf[16];
    if (threadName.IsEmpty())
      sprintf(thrdbuf, "0x%08X", thread);
    else {
      strncpy(thrdbuf, threadName, sizeof(thrdbuf)-1);
      thrdbuf[sizeof(thrdbuf)-1] = '\0';
    }

    char errbuf[25];
    if (level > StdError && level < Info && err != 0)
      ::sprintf(errbuf, "Error code = %d", err);
    else
      errbuf[0] = '\0';

    LPCTSTR strings[4];
    strings[0] = thrdbuf;
    strings[1] = msg;
    strings[2] = errbuf;
    strings[3] = level != Fatal ? "" : " Program aborted.";

    static const WORD levelType[Info+1] = {
      EVENTLOG_INFORMATION_TYPE,
      EVENTLOG_ERROR_TYPE,
      EVENTLOG_ERROR_TYPE,
      EVENTLOG_WARNING_TYPE
    };
    ReportEvent(hEventSource, // handle of event source
                (WORD)(level < Info ? levelType[level+1]
                                    : EVENTLOG_INFORMATION_TYPE), // event type
                (WORD)(level+1),      // event category
                0x1000,               // event ID
                NULL,                 // current user's SID
                PARRAYSIZE(strings),  // number of strings
                0,                    // no bytes of raw data
                strings,              // array of error strings
                NULL);                // no raw data
    DeregisterEventSource(hEventSource);
  }
}


int PSystemLog::Buffer::overflow(int c)
{
  if (pptr() >= epptr()) {
    PMEMORY_IGNORE_ALLOCATIONS_FOR_SCOPE;

    int ppos = pptr() - pbase();
    char * newptr = string.GetPointer(string.GetSize() + 10);
    setp(newptr, newptr + string.GetSize() - 1);
    pbump(ppos);
  }
  if (c != EOF) {
    *pptr() = (char)c;
    pbump(1);
  }

  return 0;
}


int PSystemLog::Buffer::underflow()
{
  return EOF;
}


int PSystemLog::Buffer::sync()
{
  Level logLevel = log->logLevel;

#if PTRACING
  if (log->width() != 0 &&(PTrace::GetOptions()&PTrace::SystemLogStream) != 0) {
    // Trace system sets the ios stream width as the last thing it does before
    // doing a flush, which gets us here. SO now we can get a PTRACE looking
    // exactly like a PSYSTEMLOG of appropriate level.
    unsigned traceLevel = (int)log->width() -1 + PSystemLog::Warning;
    log->width(0);
    if (traceLevel >= PSystemLog::NumLogLevels)
      traceLevel = PSystemLog::NumLogLevels-1;
    logLevel = (Level)traceLevel;
  }
#endif

  PSystemLog::Output(logLevel, string);

  PMEMORY_IGNORE_ALLOCATIONS_FOR_SCOPE;

  string.SetSize(10);
  char * base = string.GetPointer();
  *base = '\0';
  setp(base, base + string.GetSize() - 1);
 
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// PServiceProcess

PServiceProcess::PServiceProcess(const char * manuf, const char * name,
                           WORD major, WORD minor, CodeStatus stat, WORD build)
  : PProcess(manuf, name, major, minor, stat, build),
    systemLogFileName(GetFile().GetDirectory() + GetName() + " Log.TXT")
{
  controlWindow = debugWindow = NULL;
  currentLogLevel = PSystemLog::Warning;
}


PServiceProcess & PServiceProcess::Current()
{
  PServiceProcess & process = (PServiceProcess &)PProcess::Current();
  PAssert(PIsDescendant(&process, PServiceProcess), "Not a service!");
  return process;
}


const char * PServiceProcess::GetServiceDependencies() const
{
  return "EventLog\0";
}


BOOL PServiceProcess::IsServiceProcess() const
{
  return TRUE;
}


static BOOL IsServiceRunning(PServiceProcess * svc)
{
  HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, svc->GetName());
  if (hEvent == NULL)
    return ::GetLastError() == ERROR_ACCESS_DENIED;

  CloseHandle(hEvent);
  return TRUE;
}


int PServiceProcess::_main(void * arg)
{
  {
    PMEMORY_IGNORE_ALLOCATIONS_FOR_SCOPE;

    PSetErrorStream(new PSystemLog(PSystemLog::StdError));
#if PTRACING
    PTrace::SetStream(new PSystemLog(PSystemLog::Debug3));
    PTrace::ClearOptions(PTrace::FileAndLine);
    PTrace::SetOptions(PTrace::SystemLogStream);
    PTrace::SetLevel(4);
#endif
  }

  hInstance = (HINSTANCE)arg;

  OSVERSIONINFO verinfo;
  verinfo.dwOSVersionInfoSize = sizeof(verinfo);
  GetVersionEx(&verinfo);
  switch (verinfo.dwPlatformId) {
    case VER_PLATFORM_WIN32_NT :
      isWin95 = FALSE;
      break;
    case VER_PLATFORM_WIN32_WINDOWS :
      isWin95 = TRUE;
      break;
    default :
      PError << "Unsupported Win32 platform type!" << endl;
      return 1;
  }

  debugMode = arguments.GetCount() > 0 && 
	              (strcasecmp(arguments[0], "Debug") == 0 || strcasecmp(arguments[0], "foreground") == 0);
  currentLogLevel = debugMode ? PSystemLog::Info : PSystemLog::Warning;

  if (!debugMode && arguments.GetCount() > 0) {
    for (PINDEX a = 0; a < arguments.GetCount(); a++)
      ProcessCommand(arguments[a]);

    if (controlWindow == NULL || controlWindow == (HWND)-1)
      return GetTerminationValue();

    if (debugWindow != NULL && debugWindow != (HWND)-1) {
      ::SetLastError(0);
      PError << "Close window or select another command from the Control menu.\n" << endl;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return GetTerminationValue();
  }

  if (!debugMode && !isWin95) {
    static SERVICE_TABLE_ENTRY dispatchTable[] = {
      { (char*) "", PServiceProcess::StaticMainEntry },
      { NULL, NULL }
    };
    dispatchTable[0].lpServiceName = (char *)(const char *)GetName();

    if (StartServiceCtrlDispatcher(dispatchTable))
      return GetTerminationValue();

    PSystemLog::Output(PSystemLog::Fatal, "StartServiceCtrlDispatcher failed.");
    MessageBox(NULL, "Not run as a service!", GetName(), MB_TASKMODAL);
    return 1;
  }

  if (!CreateControlWindow(debugMode))
    return 1;

  if (IsServiceRunning(this)) {
    MessageBox(NULL, "Service already running", GetName(), MB_TASKMODAL);
    return 3;
  }

  if (debugMode) {
    ::SetLastError(0);
    PError << "Service simulation started for \"" << GetName() << "\" version " << GetVersion(TRUE) << "\n"
              "Close window to terminate.\n" << endl;
  }

  terminationEvent = CreateEvent(NULL, TRUE, FALSE, GetName());
  PAssertOS(terminationEvent != NULL);

  threadHandle = (HANDLE)_beginthread(StaticThreadEntry, 0, this);
  PAssertOS(threadHandle != (HANDLE)-1);

  SetTerminationValue(0);

  MSG msg;
  msg.message = WM_QUIT+1; //Want somethingthat is not WM_QUIT
  do {
    switch (MsgWaitForMultipleObjects(1, &terminationEvent,
                                      FALSE, INFINITE, QS_ALLINPUT)) {
      case WAIT_OBJECT_0+1 :
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
          if (msg.message != WM_QUIT) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
          }
        }
        break;

      default :
        // This is a work around for '95 coming up with an erroneous error
        if (::GetLastError() == ERROR_INVALID_HANDLE &&
                          WaitForSingleObject(terminationEvent, 0) == WAIT_TIMEOUT)
          break;
        // Else fall into next case

      case WAIT_OBJECT_0 :
        if (!debugMode || controlWindow == NULL)
          msg.message = WM_QUIT;
        else {
          PError << "nService simulation stopped for \"" << GetName() << "\".\n\n"
                    "Close window to terminate.\n" << endl;
          ResetEvent(terminationEvent);
        }
    }
  } while (msg.message != WM_QUIT);

  if (controlWindow != NULL)
    DestroyWindow(controlWindow);

  // Set thread ID for process to this thread
  activeThreadMutex.Wait();
  activeThreads.SetAt(threadId, NULL);
  threadId = GetCurrentThreadId();
  threadHandle = GetCurrentThread();
  activeThreads.SetAt(threadId, this);
  activeThreadMutex.Signal();
  OnStop();

  return GetTerminationValue();
}


enum {
  ExitMenuID = 100,
  HideMenuID,
  ControlMenuID,
  CopyMenuID,
  CutMenuID,
  DeleteMenuID,
  SelectAllMenuID,
#if PMEMORY_HEAP
  MarkMenuID,
  DumpMenuID,
  StatsMenuID,
  ValidateMenuID,
#endif
  OutputToMenuID,
  WindowOutputMenuID,
  DebuggerOutputMenuID,
  SvcCmdBaseMenuID = 1000,
  LogLevelBaseMenuID = 2000
};

static const char ServiceSimulationSectionName[] = "Service Simulation Parameters";
static const char WindowLeftKey[] = "Window Left";
static const char WindowTopKey[] = "Window Top";
static const char WindowRightKey[] = "Window Right";
static const char WindowBottomKey[] = "Window Bottom";
static const char SystemLogFileNameKey[] = "System Log File Name";


BOOL PServiceProcess::CreateControlWindow(BOOL createDebugWindow)
{
  if (controlWindow != NULL)
    return TRUE;

  WNDCLASS wclass;
  wclass.style = CS_HREDRAW|CS_VREDRAW;
  wclass.lpfnWndProc = (WNDPROC)StaticWndProc;
  wclass.cbClsExtra = 0;
  wclass.cbWndExtra = 0;
  wclass.hInstance = hInstance;
  wclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICON_RESID));
  wclass.hCursor = NULL;
  wclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wclass.lpszMenuName = NULL;
  wclass.lpszClassName = GetName();
  if (RegisterClass(&wclass) == 0)
    return FALSE;

  HMENU menubar = CreateMenu();
  HMENU menu = CreatePopupMenu();
  AppendMenu(menu, MF_STRING, OutputToMenuID, "&Output To...");
  AppendMenu(menu, MF_STRING, WindowOutputMenuID, "Output To &Window");
  AppendMenu(menu, MF_STRING, DebuggerOutputMenuID, "Output To &Debugger");
  AppendMenu(menu, MF_SEPARATOR, 0, NULL);
  AppendMenu(menu, MF_STRING, ControlMenuID, "&Control");
  AppendMenu(menu, MF_STRING, HideMenuID, "&Hide");
  AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdVersion, "&Version");
  AppendMenu(menu, MF_SEPARATOR, 0, NULL);
#if PMEMORY_HEAP
  AppendMenu(menu, MF_STRING, MarkMenuID, "&Mark Memory");
  AppendMenu(menu, MF_STRING, DumpMenuID, "&Dump Memory");
  AppendMenu(menu, MF_STRING, StatsMenuID, "&Statistics");
  AppendMenu(menu, MF_STRING, ValidateMenuID, "&Validate Heap");
  AppendMenu(menu, MF_SEPARATOR, 0, NULL);
#endif
  AppendMenu(menu, MF_STRING, ExitMenuID, "E&xit");
  AppendMenu(menubar, MF_POPUP, (UINT)menu, "&File");

  menu = CreatePopupMenu();
  AppendMenu(menu, MF_STRING, CopyMenuID, "&Copy");
  AppendMenu(menu, MF_STRING, CutMenuID, "C&ut");
  AppendMenu(menu, MF_STRING, DeleteMenuID, "&Delete");
  AppendMenu(menu, MF_SEPARATOR, 0, NULL);
  AppendMenu(menu, MF_STRING, SelectAllMenuID, "&Select All");
  AppendMenu(menubar, MF_POPUP, (UINT)menu, "&Edit");

  menu = CreatePopupMenu();
  AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdInstall, "&Install");
  AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdRemove, "&Remove");
  AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdDeinstall, "&Deinstall");
  AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdStart, "&Start");
  AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdStop, "S&top");
  AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdPause, "&Pause");
  AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdResume, "R&esume");
  AppendMenu(menubar, MF_POPUP, (UINT)menu, "&Control");

  menu = CreatePopupMenu();
  AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+PSystemLog::Fatal,   "&Fatal Error");
  AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+PSystemLog::Error,   "&Error");
  AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+PSystemLog::Warning, "&Warning");
  AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+PSystemLog::Info,    "&Information");
  AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+PSystemLog::Debug,   "&Debug");
  AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+PSystemLog::Debug2,  "Debug &2");
  AppendMenu(menu, MF_STRING, LogLevelBaseMenuID+PSystemLog::Debug3,  "Debug &3");
  AppendMenu(menubar, MF_POPUP, (UINT)menu, "&Log Level");

  if (CreateWindow(GetName(),
                   GetName(),
                   WS_OVERLAPPEDWINDOW,
                   CW_USEDEFAULT, CW_USEDEFAULT,
                   CW_USEDEFAULT, CW_USEDEFAULT, 
                   NULL,
                   menubar,
                   hInstance,
                   NULL) == NULL)
    return FALSE;

  if (createDebugWindow && debugWindow == NULL) {
    PConfig cfg(ServiceSimulationSectionName);
    int l = cfg.GetInteger(WindowLeftKey, -1);
    int t = cfg.GetInteger(WindowTopKey, -1);
    int r = cfg.GetInteger(WindowRightKey, -1);
    int b = cfg.GetInteger(WindowBottomKey, -1);
    if (l > 0 && t > 0 && r > 0 && b > 0)
      SetWindowPos(controlWindow, NULL, l, t, r-l, b-t, 0);

    debugWindow = CreateWindow("edit",
                               "",
                               WS_CHILD|WS_HSCROLL|WS_VSCROLL|WS_VISIBLE|WS_BORDER|
                                      ES_MULTILINE|ES_READONLY,
                               0, 0, 0, 0,
                               controlWindow,
                               (HMENU)10,
                               hInstance,
                               NULL);
    SendMessage(debugWindow, EM_SETLIMITTEXT, isWin95 ? 32000 : 128000, 0);
    DWORD TabStops[] = {
      DATE_WIDTH,
      DATE_WIDTH+THREAD_WIDTH,
      DATE_WIDTH+THREAD_WIDTH+LEVEL_WIDTH,
      DATE_WIDTH+THREAD_WIDTH+LEVEL_WIDTH+PROTO_WIDTH,
      DATE_WIDTH+THREAD_WIDTH+LEVEL_WIDTH+PROTO_WIDTH+ACTION_WIDTH,
      DATE_WIDTH+THREAD_WIDTH+LEVEL_WIDTH+PROTO_WIDTH+ACTION_WIDTH+32  // Standard tab width
    };
    SendMessage(debugWindow, EM_SETTABSTOPS, PARRAYSIZE(TabStops), (LPARAM)(LPDWORD)TabStops);

    systemLogFileName = cfg.GetString(SystemLogFileNameKey);
    if (systemLogFileName.IsEmpty())
      systemLogFileName = WindowLogOutput;
    if (systemLogFileName != WindowLogOutput) {
      if (systemLogFileName != DebuggerLogOutput)
        PFile::Remove(systemLogFileName);
      DebugOutput("Sending all system log output to \"" + systemLogFileName + "\".\n");
    }
  }

  return TRUE;
}


LPARAM WINAPI PServiceProcess::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  return Current().WndProc(hWnd, msg, wParam, lParam);
}


static void SaveWindowPosition(HWND hWnd)
{
  RECT r;
  GetWindowRect(hWnd, &r);
  PConfig cfg(ServiceSimulationSectionName);
  cfg.SetInteger(WindowLeftKey, r.left);
  cfg.SetInteger(WindowTopKey, r.top);
  cfg.SetInteger(WindowRightKey, r.right);
  cfg.SetInteger(WindowBottomKey, r.bottom);
}


LPARAM PServiceProcess::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#if PMEMORY_HEAP
  static PMemoryHeap::State memoryState;
#endif

  switch (msg) {
    case WM_CREATE :
      controlWindow = hWnd;
      break;

    case WM_DESTROY :
      if (debugWindow == (HWND)-1) {
        PNotifyIconData nid(hWnd, NIF_TIP);
        nid.Delete(); // This removes the systray icon
      }

      controlWindow = debugWindow = NULL;

      PostQuitMessage(0);
      break;

    case WM_ENDSESSION :
      if (wParam && (debugMode || lParam != ENDSESSION_LOGOFF) && debugWindow != (HWND)-1)
        OnStop();
      return 0;

    case WM_MOVE :
      if (debugWindow != NULL)
        SaveWindowPosition(hWnd);
      break;

    case WM_SIZE :
      if (debugWindow != NULL && debugWindow != (HWND)-1) {
        SaveWindowPosition(hWnd);
        MoveWindow(debugWindow, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
      }
      break;

    case WM_INITMENUPOPUP :
    {
      int enableItems = MF_BYCOMMAND|(debugMode ? MF_ENABLED : MF_GRAYED);
      for (int i = PSystemLog::Fatal; i < PSystemLog::NumLogLevels; i++) {
        CheckMenuItem((HMENU)wParam, LogLevelBaseMenuID+i, MF_BYCOMMAND|MF_UNCHECKED);
        EnableMenuItem((HMENU)wParam, LogLevelBaseMenuID+i, enableItems);
      }
      CheckMenuItem((HMENU)wParam, LogLevelBaseMenuID+GetLogLevel(), MF_BYCOMMAND|MF_CHECKED);

      enableItems = MF_BYCOMMAND|(debugMode ? MF_GRAYED : MF_ENABLED);
      EnableMenuItem((HMENU)wParam, SvcCmdBaseMenuID+SvcCmdStart, enableItems);
      EnableMenuItem((HMENU)wParam, SvcCmdBaseMenuID+SvcCmdStop, enableItems);
      EnableMenuItem((HMENU)wParam, SvcCmdBaseMenuID+SvcCmdPause, enableItems);
      EnableMenuItem((HMENU)wParam, SvcCmdBaseMenuID+SvcCmdResume, enableItems);

      DWORD start, finish;
      if (debugWindow != NULL && debugWindow != (HWND)-1)
        SendMessage(debugWindow, EM_GETSEL, (WPARAM)&start, (LPARAM)&finish);
      else
        start = finish = 0;
      enableItems = MF_BYCOMMAND|(start == finish ? MF_GRAYED : MF_ENABLED);
      EnableMenuItem((HMENU)wParam, CopyMenuID, enableItems);
      EnableMenuItem((HMENU)wParam, CutMenuID, enableItems);
      EnableMenuItem((HMENU)wParam, DeleteMenuID, enableItems);

      enableItems = MF_BYCOMMAND|(IsServiceRunning(this) ? MF_ENABLED : MF_GRAYED);
      EnableMenuItem((HMENU)wParam, ControlMenuID, enableItems);
      break;
    }

    case WM_COMMAND :
      switch (wParam) {
        case ExitMenuID :
          DestroyWindow(hWnd);
          break;

        case ControlMenuID :
          if (IsServiceRunning(this))
            OnControl();
          break;

        case HideMenuID :
          ShowWindow(hWnd, SW_HIDE);
          break;

#if PMEMORY_HEAP
        case MarkMenuID :
          PMemoryHeap::GetState(memoryState);
          break;

        case DumpMenuID :
          PMemoryHeap::DumpObjectsSince(memoryState);
          break;

        case StatsMenuID :
          PMemoryHeap::DumpStatistics();
          break;
        case ValidateMenuID :
          PMemoryHeap::ValidateHeap();
          break;
#endif

        case CopyMenuID :
          if (debugWindow != NULL && debugWindow != (HWND)-1)
            SendMessage(debugWindow, WM_COPY, 0, 0);
          break;

        case CutMenuID :
          if (debugWindow != NULL && debugWindow != (HWND)-1)
            SendMessage(debugWindow, WM_CUT, 0, 0);
          break;

        case DeleteMenuID :
          if (debugWindow != NULL && debugWindow != (HWND)-1)
            SendMessage(debugWindow, WM_CLEAR, 0, 0);
          break;

        case SelectAllMenuID :
          if (debugWindow != NULL && debugWindow != (HWND)-1)
            SendMessage(debugWindow, EM_SETSEL, 0, -1);
          break;

        case OutputToMenuID :
          if (debugWindow != NULL && debugWindow != (HWND)-1) {
            char fileBuffer[_MAX_PATH];
            OPENFILENAME fileDlgInfo;
            memset(&fileDlgInfo, 0, sizeof(fileDlgInfo));
            fileDlgInfo.lStructSize = sizeof(fileDlgInfo);
            fileDlgInfo.hwndOwner = hWnd;
            fileDlgInfo.hInstance = hInstance;
            fileBuffer[0] = '\0';
            fileDlgInfo.lpstrFile = fileBuffer;
            char customFilter[100];
            strcpy(customFilter, "All Files");
            memcpy(&customFilter[strlen(customFilter)+1], "*.*\0", 5);
            fileDlgInfo.lpstrCustomFilter = customFilter;
            fileDlgInfo.nMaxCustFilter = sizeof(customFilter);
            fileDlgInfo.nMaxFile = sizeof(fileBuffer);
            fileDlgInfo.Flags = OFN_ENABLEHOOK|OFN_HIDEREADONLY|OFN_NOVALIDATE|OFN_EXPLORER|OFN_CREATEPROMPT;
            fileDlgInfo.lCustData = (DWORD)this;
            if (GetSaveFileName(&fileDlgInfo)) {
              if (systemLogFileName != fileBuffer) {
                systemLogFileName = fileBuffer;
                PFile::Remove(systemLogFileName);
                PConfig cfg(ServiceSimulationSectionName);
                cfg.SetString(SystemLogFileNameKey, systemLogFileName);
                DebugOutput("Sending all system log output to \"" + systemLogFileName + "\".\n");
                PError << "Logging started for \"" << GetName() << "\" version " << GetVersion(TRUE) << endl;
              }
            }
          }
          break;

        case WindowOutputMenuID :
          if (systemLogFileName != WindowLogOutput) {
            PError << "Logging stopped." << endl;
            DebugOutput("System log output to \"" + systemLogFileName + "\" stopped.\n");
            systemLogFileName = WindowLogOutput;
            PConfig cfg(ServiceSimulationSectionName);
            cfg.SetString(SystemLogFileNameKey, systemLogFileName);
          }
          break;

        case DebuggerOutputMenuID :
          if (systemLogFileName != DebuggerLogOutput) {
            PError << "Logging stopped." << endl;
            DebugOutput("System log output to \"" + systemLogFileName + "\" stopped.\n");
            systemLogFileName = DebuggerLogOutput;
            PConfig cfg(ServiceSimulationSectionName);
            cfg.SetString(SystemLogFileNameKey, systemLogFileName);
          }
          break;

        default :
          if (wParam >= LogLevelBaseMenuID+PSystemLog::Fatal && wParam < LogLevelBaseMenuID+PSystemLog::NumLogLevels) {
            SetLogLevel((PSystemLog::Level)(wParam-LogLevelBaseMenuID));
#if PTRACING
            PTrace::SetLevel(wParam-LogLevelBaseMenuID-PSystemLog::Warning);
#endif
          }
          else if (wParam >= SvcCmdBaseMenuID && wParam < SvcCmdBaseMenuID+NumSvcCmds) {
            const char * cmdname = ServiceCommandNames[wParam-SvcCmdBaseMenuID];
            if (wParam == SvcCmdBaseMenuID+SvcCmdVersion ||
                MessageBox(hWnd, cmdname & GetName() & "?", GetName(),
                           MB_ICONQUESTION|MB_YESNO) == IDYES)
              ProcessCommand(cmdname);
          }
      }
      break;

    // Notification of event over sysTray icon
    case UWM_SYSTRAY :
      switch (lParam) {
        case WM_MOUSEMOVE :
          // update status of process for tool tips if no buttons down
          if (wParam == SYSTRAY_ICON_ID) {
            PNotifyIconData nid(hWnd, NIF_TIP,
                          GetName() & (IsServiceRunning(this) ? "is" : "not") & "running.");
            nid.Modify(); // Modify tooltip
          }
          break;

        // Click on icon - display message
        case WM_LBUTTONDBLCLK :
          if (IsServiceRunning(this))
            OnControl();
          else {
            SetForegroundWindow(hWnd); // Our MessageBox pops up in front
            MessageBox(hWnd, "Service is not running!", GetName(), MB_TASKMODAL);
          }
          break;

        // Popup menu
        case WM_RBUTTONUP :
          POINT pt;
          GetCursorPos(&pt);

          HMENU menu = CreatePopupMenu();
          AppendMenu(menu, MF_STRING, ControlMenuID, "&Open Properties");
          AppendMenu(menu, MF_SEPARATOR, 0, NULL);
          AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdVersion, "&Version");
          if (IsServiceRunning(this)) {
            MENUITEMINFO inf;
            inf.cbSize = sizeof(inf);
            inf.fMask = MIIM_STATE;
            inf.fState = MFS_DEFAULT;
            SetMenuItemInfo(menu, ControlMenuID, FALSE, &inf);
            AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdStop, "&Stop Service");
          }
          else {
            EnableMenuItem(menu, ControlMenuID, MF_GRAYED);
            AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdStart, "&Start Service");
          }
          AppendMenu(menu, MF_STRING, SvcCmdBaseMenuID+SvcCmdNoTray, "&Tray Icon");
          CheckMenuItem(menu, SvcCmdBaseMenuID+SvcCmdNoTray,
                        TrayIconRegistry(this, CheckTrayIcon) ? MF_CHECKED : MF_UNCHECKED);
          AppendMenu(menu, MF_SEPARATOR, 0, NULL);
          AppendMenu(menu, MF_STRING, ExitMenuID, "&Close");

          /* SetForegroundWindow and the ensuing null PostMessage is a
             workaround for a Windows 95 bug (see MSKB article Q135788,
             http://www.microsoft.com/kb/articles/q135/7/88.htm, I think).
             In typical Microsoft style this bug is listed as "by design".
             SetForegroundWindow also causes our MessageBox to pop up in front
             of any other application's windows. */
          SetForegroundWindow(hWnd);

          /* We specifiy TPM_RETURNCMD, so TrackPopupMenu returns the menu
             selection instead of returning immediately and our getting a
             WM_COMMAND with the selection. You don't have to do it this way.
          */
          WndProc(hWnd, WM_COMMAND, TrackPopupMenu(menu,            // Popup menu to track
                                                   TPM_RETURNCMD |  // Return menu code
                                                   TPM_RIGHTBUTTON, // Track right mouse button?
                                                   pt.x, pt.y,      // screen coordinates
                                                   0,               // reserved
                                                   hWnd,            // owner
                                                   NULL),           // LPRECT user can click in without dismissing menu
                                                   0);
          PostMessage(hWnd, 0, 0, 0); // see above
          DestroyMenu(menu); // Delete loaded menu and reclaim its resources
          break;
      }
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}


void PServiceProcess::DebugOutput(const char * out)
{
  if (controlWindow == NULL)
    return;

  if (debugWindow == NULL || debugWindow == (HWND)-1) {
    for (PINDEX i = 0; i < 3; i++) {
      const char * tab = strchr(out, '\t');
      if (tab == NULL)
        break;
      out = tab+1;
    }
    MessageBox(controlWindow, out, GetName(), MB_TASKMODAL);
    return;
  }


  if (!IsWindowVisible(controlWindow))
    ShowWindow(controlWindow, SW_SHOWDEFAULT);

  int len = strlen(out);
  int max = isWin95 ? 32000 : 128000;
  while (GetWindowTextLength(debugWindow)+len >= max) {
    SendMessage(debugWindow, WM_SETREDRAW, FALSE, 0);
    DWORD start, finish;
    SendMessage(debugWindow, EM_GETSEL, (WPARAM)&start, (LPARAM)&finish);
    SendMessage(debugWindow, EM_SETSEL, 0,
                SendMessage(debugWindow, EM_LINEINDEX, 1, 0));
    SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)"");
    SendMessage(debugWindow, EM_SETSEL, start, finish);
    SendMessage(debugWindow, WM_SETREDRAW, TRUE, 0);
  }

  SendMessage(debugWindow, EM_SETSEL, max, max);
  char * lf;
  char * prev = (char *)out;
  while ((lf = strchr(prev, '\n')) != NULL) {
    if (*(lf-1) == '\r')
      prev = lf+1;
    else {
      *lf++ = '\0';
      SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)out);
      SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)"\r\n");
      out = (const char *)lf;
      prev = lf;
    }
  }
  if (*out != '\0')
    SendMessage(debugWindow, EM_REPLACESEL, FALSE, (DWORD)out);
}


void PServiceProcess::StaticMainEntry(DWORD argc, LPTSTR * argv)
{
  Current().MainEntry(argc, argv);
}


void PServiceProcess::MainEntry(DWORD argc, LPTSTR * argv)
{
  // SERVICE_STATUS members that don't change
  status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  status.dwServiceSpecificExitCode = 0;

  // register our service control handler:
  statusHandle = RegisterServiceCtrlHandler(GetName(), StaticControlEntry);
  if (statusHandle == FALSE)
    return;

  // report the status to Service Control Manager.
  if (!ReportStatus(SERVICE_START_PENDING, NO_ERROR, 1, 20000))
    return;

  // create the stop event object. The control handler function signals
  // this event when it receives the "stop" control code.
  terminationEvent = CreateEvent(NULL, TRUE, FALSE, (const char *)GetName());
  if (terminationEvent == NULL)
    return;

  startedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (startedEvent == NULL)
    return;

  GetArguments().SetArgs(argc, argv);

  // start the thread that performs the work of the service.
  threadHandle = (HANDLE)_beginthread(StaticThreadEntry, 0, this);
  if (threadHandle != (HANDLE)-1) {
    while (WaitForSingleObject(startedEvent, 10000) == WAIT_TIMEOUT) {
      if (!ReportStatus(SERVICE_START_PENDING, NO_ERROR, 1, 20000))
        return;
    }
    // Wait here for the end
    WaitForSingleObject(terminationEvent, INFINITE);
  }

  CloseHandle(startedEvent);
  CloseHandle(terminationEvent);
  ReportStatus(SERVICE_STOPPED, terminationValue);
}


void PServiceProcess::StaticThreadEntry(void * arg)
{
  ((PServiceProcess *)arg)->ThreadEntry();
}


void PServiceProcess::ThreadEntry()
{
  activeThreadMutex.Wait();
  threadId = GetCurrentThreadId();
  threadHandle = GetCurrentThread();
  activeThreads.SetAt(threadId, this);
  activeThreadMutex.Signal();

  SetTerminationValue(1);
  BOOL ok = OnStart();

  // signal the above function to stop reporting the "start pending" status
  // and start waiting for the termination event
  if (!debugMode)
    SetEvent(startedEvent);

  // if the OnStart handler reported success, enter the main loop
  if (ok) {
    ReportStatus(SERVICE_RUNNING);
    SetTerminationValue(0);
    Main();
  }

  ReportStatus(SERVICE_STOP_PENDING, terminationValue, 1, 30000);
  SetEvent(terminationEvent);
}


void PServiceProcess::StaticControlEntry(DWORD code)
{
  Current().ControlEntry(code);
}


void PServiceProcess::ControlEntry(DWORD code)
{
  switch (code) {
    case SERVICE_CONTROL_PAUSE : // Pause the service if it is running.
      if (status.dwCurrentState != SERVICE_RUNNING)
        ReportStatus(status.dwCurrentState);
      else {
        if (OnPause())
          ReportStatus(SERVICE_PAUSED);
      }
      break;

    case SERVICE_CONTROL_CONTINUE : // Resume the paused service.
      if (status.dwCurrentState == SERVICE_PAUSED)
        OnContinue();
      ReportStatus(status.dwCurrentState);
      break;

    case SERVICE_CONTROL_STOP : // Stop the service.
      // Report the status, specifying the checkpoint and waithint, before
      // setting the termination event.
      ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 1, 30000);
      OnStop();
      SetEvent(terminationEvent);
      break;

    case SERVICE_CONTROL_INTERROGATE : // Update the service status.
    default :
      ReportStatus(status.dwCurrentState);
  }
}


BOOL PServiceProcess::ReportStatus(DWORD dwCurrentState,
                                   DWORD dwWin32ExitCode,
                                   DWORD dwCheckPoint,
                                   DWORD dwWaitHint)
{
  // Disable control requests until the service is started.
  if (dwCurrentState == SERVICE_START_PENDING)
    status.dwControlsAccepted = 0;
  else
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

  // These SERVICE_STATUS members are set from parameters.
  status.dwCurrentState = dwCurrentState;
  status.dwWin32ExitCode = dwWin32ExitCode;
  status.dwCheckPoint = dwCheckPoint;
  status.dwWaitHint = dwWaitHint;

  if (debugMode || isWin95)
    return TRUE;

  // Report the status of the service to the service control manager.
  if (SetServiceStatus(statusHandle, &status))
    return TRUE;

  // If an error occurs, stop the service.
  PSystemLog::Output(PSystemLog::Error, "SetServiceStatus failed");
  return FALSE;
}


void PServiceProcess::OnStop()
{
}


BOOL PServiceProcess::OnPause()
{
  SuspendThread(threadHandle);
  return TRUE;
}


void PServiceProcess::OnContinue()
{
  ResumeThread(threadHandle);
}


void PServiceProcess::OnControl()
{
}



class ServiceManager
{
  public:
    ServiceManager()  { error = 0; }

    virtual BOOL Create(PServiceProcess * svc) = 0;
    virtual BOOL Delete(PServiceProcess * svc) = 0;
    virtual BOOL Start(PServiceProcess * svc) = 0;
    virtual BOOL Stop(PServiceProcess * svc) = 0;
    virtual BOOL Pause(PServiceProcess * svc) = 0;
    virtual BOOL Resume(PServiceProcess * svc) = 0;

    DWORD GetError() const { return error; }

  protected:
    DWORD error;
};


class Win95_ServiceManager : public ServiceManager
{
  public:
    virtual BOOL Create(PServiceProcess * svc);
    virtual BOOL Delete(PServiceProcess * svc);
    virtual BOOL Start(PServiceProcess * svc);
    virtual BOOL Stop(PServiceProcess * svc);
    virtual BOOL Pause(PServiceProcess * svc);
    virtual BOOL Resume(PServiceProcess * svc);
};


BOOL Win95_ServiceManager::Create(PServiceProcess * svc)
{
  HKEY key;
  if (RegCreateKey(HKEY_LOCAL_MACHINE,
                   "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                   &key) == ERROR_SUCCESS) {
    RegDeleteValue(key, (char *)(const char *)svc->GetName());
    RegCloseKey(key);
  }

  if ((error = RegCreateKey(HKEY_LOCAL_MACHINE,
                            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices",
                            &key)) != ERROR_SUCCESS)
    return FALSE;

  PString cmd = "\"" + svc->GetFile() + "\"";
  error = RegSetValueEx(key, svc->GetName(), 0, REG_SZ,
                        (LPBYTE)(const char *)cmd, cmd.GetLength() + 1);

  RegCloseKey(key);

  return error == ERROR_SUCCESS;
}


BOOL Win95_ServiceManager::Delete(PServiceProcess * svc)
{
  HKEY key;
  if (RegCreateKey(HKEY_LOCAL_MACHINE,
                   "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                   &key) == ERROR_SUCCESS) {
    RegDeleteValue(key, (char *)(const char *)svc->GetName());
    RegCloseKey(key);
  }

  if ((error = RegCreateKey(HKEY_LOCAL_MACHINE,
                            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices",
                            &key)) != ERROR_SUCCESS)
    return FALSE;

  error = RegDeleteValue(key, (char *)(const char *)svc->GetName());

  RegCloseKey(key);

  return error == ERROR_SUCCESS;
}


BOOL Win95_ServiceManager::Start(PServiceProcess * service)
{
  if (IsServiceRunning(service)) {
    PError << "Service already running" << endl;
    error = 1;
    return FALSE;
  }

  BOOL ok = _spawnl(_P_DETACH, service->GetFile(), service->GetFile(), NULL) >= 0;
  error = errno;
  return ok;
}


BOOL Win95_ServiceManager::Stop(PServiceProcess * service)
{
  HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, service->GetName());
  if (hEvent == NULL) {
    error = ::GetLastError();
    PError << "Service is not running" << endl;
    return FALSE;
  }

  SetEvent(hEvent);
  CloseHandle(hEvent);

  // Wait for process to go away.
  for (PINDEX i = 0; i < 20; i++) {
    hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, service->GetName());
    if (hEvent == NULL)
      return TRUE;
    CloseHandle(hEvent);
    ::Sleep(500);
  }

  error = 0x10000000;
  return FALSE;
}


BOOL Win95_ServiceManager::Pause(PServiceProcess *)
{
  PError << "Cannot pause service under Windows 95" << endl;
  error = 1;
  return FALSE;
}


BOOL Win95_ServiceManager::Resume(PServiceProcess *)
{
  PError << "Cannot resume service under Windows 95" << endl;
  error = 1;
  return FALSE;
}



class NT_ServiceManager : public ServiceManager
{
  public:
    NT_ServiceManager()  { schSCManager = schService = NULL; }
    ~NT_ServiceManager();

    BOOL Create(PServiceProcess * svc);
    BOOL Delete(PServiceProcess * svc);
    BOOL Start(PServiceProcess * svc);
    BOOL Stop(PServiceProcess * svc)
      { return Control(svc, SERVICE_CONTROL_STOP); }
    BOOL Pause(PServiceProcess * svc)
      { return Control(svc, SERVICE_CONTROL_PAUSE); }
    BOOL Resume(PServiceProcess * svc)
      { return Control(svc, SERVICE_CONTROL_CONTINUE); }

    DWORD GetError() const { return error; }

  private:
    BOOL OpenManager();
    BOOL Open(PServiceProcess * svc);
    BOOL Control(PServiceProcess * svc, DWORD command);

    SC_HANDLE schSCManager, schService;
};


NT_ServiceManager::~NT_ServiceManager()
{
  if (schService != NULL)
    CloseServiceHandle(schService);
  if (schSCManager != NULL)
    CloseServiceHandle(schSCManager);
}


BOOL NT_ServiceManager::OpenManager()
{
  schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (schSCManager != NULL)
    return TRUE;

  error = ::GetLastError();
  PError << "Could not open Service Manager." << endl;
  return FALSE;
}


BOOL NT_ServiceManager::Open(PServiceProcess * svc)
{
  if (!OpenManager())
    return FALSE;

  schService = OpenService(schSCManager, svc->GetName(), SERVICE_ALL_ACCESS);
  if (schService != NULL)
    return TRUE;

  error = ::GetLastError();
  PError << "Service is not installed." << endl;
  return FALSE;
}


BOOL NT_ServiceManager::Create(PServiceProcess * svc)
{
  if (!OpenManager())
    return FALSE;

  schService = OpenService(schSCManager, svc->GetName(), SERVICE_ALL_ACCESS);
  if (schService != NULL) {
    PError << "Service is already installed." << endl;
    return FALSE;
  }

  PString binaryFilename;
  GetShortPathName(svc->GetFile(), binaryFilename.GetPointer(_MAX_PATH), _MAX_PATH);
  schService = CreateService(
                    schSCManager,                   // SCManager database
                    svc->GetName(),                 // name of service
                    svc->GetName(),                 // name to display
                    SERVICE_ALL_ACCESS,             // desired access
                    SERVICE_WIN32_OWN_PROCESS,      // service type
                    SERVICE_AUTO_START,             // start type
                    SERVICE_ERROR_NORMAL,           // error control type
                    binaryFilename,                 // service's binary
                    NULL,                           // no load ordering group
                    NULL,                           // no tag identifier
                    svc->GetServiceDependencies(),  // no dependencies
                    NULL,                           // LocalSystem account
                    NULL);                          // no password
  if (schService == NULL) {
    error = ::GetLastError();
    return FALSE;
  }

  HKEY key;
  if ((error = RegCreateKey(HKEY_LOCAL_MACHINE,
             "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" +
                                       svc->GetName(), &key)) != ERROR_SUCCESS)
    return FALSE;

  LPBYTE fn = (LPBYTE)(const char *)binaryFilename;
  PINDEX fnlen = binaryFilename.GetLength()+1;
  if ((error = RegSetValueEx(key, "EventMessageFile",
                             0, REG_EXPAND_SZ, fn, fnlen)) == ERROR_SUCCESS &&
      (error = RegSetValueEx(key, "CategoryMessageFile",
                             0, REG_EXPAND_SZ, fn, fnlen)) == ERROR_SUCCESS) {
    DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
    if ((error = RegSetValueEx(key, "TypesSupported",
                               0, REG_DWORD, (LPBYTE)&dwData, sizeof(DWORD))) == ERROR_SUCCESS) {
      dwData = PSystemLog::NumLogLevels;
      error = RegSetValueEx(key, "CategoryCount", 0, REG_DWORD, (LPBYTE)&dwData, sizeof(DWORD));
    }
  }

  RegCloseKey(key);

  return error == ERROR_SUCCESS;
}


BOOL NT_ServiceManager::Delete(PServiceProcess * svc)
{
  if (!Open(svc))
    return FALSE;

  PString name = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" + svc->GetName();
  error = ::RegDeleteKey(HKEY_LOCAL_MACHINE, (char *)(const char *)name);

  if (!::DeleteService(schService))
    error = ::GetLastError();

  return error == ERROR_SUCCESS;
}


BOOL NT_ServiceManager::Start(PServiceProcess * svc)
{
  if (!Open(svc))
    return FALSE;

  BOOL ok = ::StartService(schService, 0, NULL);
  error = ::GetLastError();

  if (!ok)
    return FALSE;

  SERVICE_STATUS serviceStatus;

  // query the service status
  if (!QueryServiceStatus(schService, &serviceStatus))
    return FALSE;

  // if pending periodicaly re-query the status
  while (serviceStatus.dwCurrentState == SERVICE_START_PENDING) {
    DWORD waitTime = PMIN(serviceStatus.dwWaitHint / 10, 10000);
	Sleep(waitTime);

	if (! QueryServiceStatus(schService, &serviceStatus)) break;
  }

  if (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
    return TRUE;
  } else {
    error = serviceStatus.dwWin32ExitCode;
    return FALSE;
  }
}


BOOL NT_ServiceManager::Control(PServiceProcess * svc, DWORD command)
{
  if (!Open(svc))
    return FALSE;

  SERVICE_STATUS status;
  BOOL ok = ::ControlService(schService, command, &status);
  error = ::GetLastError();
  return ok;
}


BOOL PServiceProcess::ProcessCommand(const char * cmd)
{
  PINDEX cmdNum = 0;
  while (strcasecmp(cmd, ServiceCommandNames[cmdNum]) != 0) {
    if (++cmdNum >= NumSvcCmds) {
      if (!CreateControlWindow(TRUE))
        return FALSE;
      if (*cmd != '\0')
        PError << "Unknown command \"" << cmd << "\".\n";
      else
        PError << "Could not start service.\n";
      PError << "usage: " << GetName() << " [ ";
      for (cmdNum = 0; cmdNum < NumSvcCmds-1; cmdNum++)
        PError << ServiceCommandNames[cmdNum] << " | ";
      PError << ServiceCommandNames[cmdNum] << " ]" << endl;
      return FALSE;
    }
  }

  NT_ServiceManager nt;
  Win95_ServiceManager win95;

  ServiceManager * svcManager;
  if (isWin95)
    svcManager = &win95;
  else
    svcManager = &nt;

  BOOL good = FALSE;
  switch (cmdNum) {
    case SvcCmdNoWindow :
      if (controlWindow == NULL)
        controlWindow = (HWND)-1;
      break;

    case SvcCmdTray :
      if (CreateControlWindow(FALSE)) {
        PNotifyIconData nid(controlWindow, NIF_MESSAGE|NIF_ICON, GetName());
        nid.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(ICON_RESID), IMAGE_ICON, // 16x16 icon
                               GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
        nid.uCallbackMessage = UWM_SYSTRAY; // message sent to nid.hWnd
        nid.Add();    // This adds the icon
        debugWindow = (HWND)-1;
        systemLogFileName = DebuggerLogOutput;
        return TRUE;
      }
      return FALSE;

    case SvcCmdNoTray :
      if (TrayIconRegistry(this, CheckTrayIcon)) {
        TrayIconRegistry(this, DelTrayIcon);
        PError << "Tray icon removed.";
      }
      else {
        TrayIconRegistry(this, AddTrayIcon);
        PError << "Tray icon installed.";
      }
      return TRUE;

    case SvcCmdVersion : // Version command
      ::SetLastError(0);
      PError << GetName() << " Version " << GetVersion(TRUE)
             << " by " << GetManufacturer()
             << " on " << GetOSClass()   << ' ' << GetOSName()
             << " ("   << GetOSVersion() << '-' << GetOSHardware() << ')' << endl;
      return TRUE;

    case SvcCmdInstall : // install
      good = svcManager->Create(this);
      TrayIconRegistry(this, AddTrayIcon);
      break;

    case SvcCmdRemove : // remove
      good = svcManager->Delete(this);
      TrayIconRegistry(this, DelTrayIcon);
      break;

    case SvcCmdStart : // start
      good = svcManager->Start(this);
      break;

    case SvcCmdStop : // stop
      good = svcManager->Stop(this);
      break;

    case SvcCmdPause : // pause
      good = svcManager->Pause(this);
      break;

    case SvcCmdResume : // resume
      good = svcManager->Resume(this);
      break;

    case SvcCmdDeinstall : // deinstall
      svcManager->Delete(this);
      TrayIconRegistry(this, DelTrayIcon);
      PConfig cfg;
      PStringList sections = cfg.GetSections();
      PINDEX i;
      for (i = 0; i < sections.GetSize(); i++)
        cfg.DeleteSection(sections[i]);
      good = TRUE;
      break;
  }

  SetLastError(0);

  PError << "Service command \"" << ServiceCommandNames[cmdNum] << "\" ";
  if (good)
    PError << "successful.";
  else {
    PError << "failed - ";
    switch (svcManager->GetError()) {
      case ERROR_ACCESS_DENIED :
        PError << "Access denied";
        break;
      case 0x10000000 :
        PError << "process still running.";
        break;
      default :
        PError << "error code = " << svcManager->GetError();
    }
  }
  PError << endl;

  return TRUE;
}


// End Of File ///////////////////////////////////////////////////////////////
