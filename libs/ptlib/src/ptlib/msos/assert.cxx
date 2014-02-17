/*
 * assert.cxx
 *
 * Function to implement assert clauses.
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
 * $Log: assert.cxx,v $
 * Revision 1.44  2007/10/03 01:18:47  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.43  2007/04/08 01:53:37  ykiryanov
 * Build to support ptlib dll creation
 *
 * Revision 1.42  2007/04/06 21:05:10  ykiryanov
 * added win ce define
 *
 * Revision 1.41  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.40  2005/09/29 15:58:20  dominance
 * one more step towards having mingw build pwlib cleanly
 *
 * Revision 1.39  2004/07/06 10:12:55  csoutheren
 * Added static integer o factory template to assist in ensuring factories are instantiated
 *
 * Revision 1.38  2004/04/03 06:54:30  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.37  2002/09/25 00:54:50  robertj
 * Fixed memory leak on assertion.
 *
 * Revision 1.36  2002/09/23 07:17:24  robertj
 * Changes to allow winsock2 to be included.
 *
 * Revision 1.35  2002/06/25 02:25:29  robertj
 * Improved assertion system to allow C++ class name to be displayed if
 *   desired, especially relevant to container classes.
 *
 * Revision 1.34  2001/08/17 19:18:15  yurik
 * Fixed compile error in release mode
 *
 * Revision 1.33  2001/08/16 18:38:05  yurik
 * Fixed assert function
 *
 * Revision 1.32  2001/04/26 06:07:34  yurik
 * UI improvements
 *
 * Revision 1.31  2001/03/29 23:33:00  robertj
 * Added missing structure initialisation, thanks Victor H.
 *
 * Revision 1.30  2001/03/02 06:54:04  yurik
 * Rephrased pragma message
 *
 * Revision 1.29  2001/01/24 06:56:03  yurik
 * Correcting a typo in WinCE related code
 *
 * Revision 1.28  2001/01/24 06:34:44  yurik
 * Windows CE port-related changes
 *
 * Revision 1.27  2000/05/23 05:50:43  robertj
 * Attempted to fix stack dump, still refuses to work even though used to work perfectly.
 *
 * Revision 1.26  2000/03/04 08:07:07  robertj
 * Fixed problem with window not appearing when assert on GUI based win32 apps.
 *
 * Revision 1.25  1999/02/16 08:08:06  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.24  1999/02/12 01:01:57  craigs
 * Fixed problem with linking static versions of libraries
 *
 * Revision 1.23  1998/12/04 10:10:45  robertj
 * Added virtual for determining if process is a service. Fixes linkage problem.
 *
 * Revision 1.22  1998/11/30 05:33:08  robertj
 * Fixed duplicate debug stream class, ther can be only one.
 *
 * Revision 1.21  1998/11/30 04:48:38  robertj
 * New directory structure
 *
 * Revision 1.20  1998/09/24 03:30:39  robertj
 * Added open software license.
 *
 * Revision 1.19  1997/03/18 21:22:31  robertj
 * Display error message if assert stack dump fails
 *
 * Revision 1.18  1997/02/09 01:27:18  robertj
 * Added stack dump under NT.
 *
 * Revision 1.17  1997/02/05 11:49:40  robertj
 * Changed current process function to return reference and validate objects descendancy.
 *
 * Revision 1.16  1997/01/04 06:52:04  robertj
 * Removed the press a key to continue under win  '95.
 *
 * Revision 1.15  1996/11/18 11:30:00  robertj
 * Removed int 3 on non-debug versions.
 *
 * Revision 1.14  1996/11/16 10:51:51  robertj
 * Changed assert to display message and break if in debug mode service.
 *
 * Revision 1.13  1996/11/10 21:02:08  robertj
 * Fixed bug in assertion when as a service, string buffer not big enough.
 *
 * Revision 1.12  1996/10/08 13:00:46  robertj
 * Changed default for assert to be ignore, not abort.
 *
 * Revision 1.11  1996/07/27 04:08:13  robertj
 * Changed SystemLog to be stream based rather than printf based.
 *
 * Revision 1.10  1996/05/30 11:48:28  robertj
 * Fixed press a key to continue to only require one key.
 *
 * Revision 1.9  1996/05/23 10:03:20  robertj
 * Windows 95 support.
 *
 * Revision 1.8  1996/03/04 12:39:35  robertj
 * Fixed Win95 support for console tasks.
 *
 * Revision 1.7  1996/01/28 14:13:04  robertj
 * Made PServiceProcess special case global not just WIN32.
 *
 * Revision 1.6  1995/12/10 11:55:09  robertj
 * Numerous fixes for WIN32 service processes.
 *
 * Revision 1.5  1995/04/25 11:32:34  robertj
 * Fixed Borland compiler warnings.
 *
 * Revision 1.4  1995/03/12 05:00:04  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.3  1994/10/30  11:25:09  robertj
 * Added error number to assert.
 *
 * Revision 1.2  1994/06/25  12:13:01  robertj
 * Synchronisation.
 *
 * Revision 1.1  1994/04/01  14:39:35  robertj
 * Initial revision
 */

#define P_DISABLE_FACTORY_INSTANCES

#include <ptlib.h>
#include <ptlib/svcproc.h>


///////////////////////////////////////////////////////////////////////////////
// PProcess

#if defined(_WIN32)
#ifndef _WIN32_WCE
#include <imagehlp.h>

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM thisProcess)
{
  char wndClassName[100];
  GetClassName(hWnd, wndClassName, sizeof(wndClassName));
  if (strcmp(wndClassName, "ConsoleWindowClass") != 0)
    return TRUE;

  DWORD wndProcess;
  GetWindowThreadProcessId(hWnd, &wndProcess);
  if (wndProcess != (DWORD)thisProcess)
    return TRUE;

  cerr << "\nPress a key to continue . . .";
  cerr.flush();

  HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
  SetConsoleMode(in, ENABLE_PROCESSED_INPUT);
  FlushConsoleInputBuffer(in);
  char dummy;
  DWORD readBytes;
  ReadConsole(in, &dummy, 1, &readBytes, NULL);
  return FALSE;
}
#endif // _WIN32_WCE

void PWaitOnExitConsoleWindow()
{
#ifndef _WIN32_WCE
  EnumWindows(EnumWindowsProc, GetCurrentProcessId());
#else
#endif // _WIN32_WCE
}

#ifndef _WIN32_WCE
class PImageDLL : public PDynaLink
{
  PCLASSINFO(PImageDLL, PDynaLink)
  public:
    PImageDLL();

  BOOL (__stdcall *SymInitialize)(
    IN HANDLE   hProcess,
    IN LPSTR    UserSearchPath,
    IN BOOL     fInvadeProcess
    );
  BOOL (__stdcall *SymCleanup)(
    IN HANDLE hProcess
    );
  DWORD (__stdcall *SymGetOptions)();
  DWORD (__stdcall *SymSetOptions)(
    DWORD options
    );
  DWORD (__stdcall *SymLoadModule)(
    HANDLE hProcess,
    HANDLE hFile,     
    PSTR   ImageName,  
    PSTR   ModuleName, 
    DWORD  BaseOfDll,  
    DWORD  SizeOfDll   
    );
  BOOL (__stdcall *StackWalk)(
    DWORD                             MachineType,
    HANDLE                            hProcess,
    HANDLE                            hThread,
    LPSTACKFRAME                      StackFrame,
    LPVOID                            ContextRecord,
    PREAD_PROCESS_MEMORY_ROUTINE      ReadMemoryRoutine,
    PFUNCTION_TABLE_ACCESS_ROUTINE    FunctionTableAccessRoutine,
    PGET_MODULE_BASE_ROUTINE          GetModuleBaseRoutine,
    PTRANSLATE_ADDRESS_ROUTINE        TranslateAddress
    );
  BOOL (__stdcall *SymGetSymFromAddr)(
    IN  HANDLE              hProcess,
    IN  DWORD               dwAddr,
    OUT PDWORD              pdwDisplacement,
    OUT PIMAGEHLP_SYMBOL    Symbol
    );

  PFUNCTION_TABLE_ACCESS_ROUTINE SymFunctionTableAccess;
  PGET_MODULE_BASE_ROUTINE       SymGetModuleBase;

  BOOL (__stdcall *SymGetModuleInfo)(
    IN  HANDLE              hProcess,
    IN  DWORD               dwAddr,
    OUT PIMAGEHLP_MODULE    ModuleInfo
    );
};


PImageDLL::PImageDLL()
  : PDynaLink("IMAGEHLP.DLL")
{
  if (!GetFunction("SymInitialize", (Function &)SymInitialize) ||
      !GetFunction("SymCleanup", (Function &)SymCleanup) ||
      !GetFunction("SymGetOptions", (Function &)SymGetOptions) ||
      !GetFunction("SymSetOptions", (Function &)SymSetOptions) ||
      !GetFunction("SymLoadModule", (Function &)SymLoadModule) ||
      !GetFunction("StackWalk", (Function &)StackWalk) ||
      !GetFunction("SymGetSymFromAddr", (Function &)SymGetSymFromAddr) ||
      !GetFunction("SymFunctionTableAccess", (Function &)SymFunctionTableAccess) ||
      !GetFunction("SymGetModuleBase", (Function &)SymGetModuleBase) ||
      !GetFunction("SymGetModuleInfo", (Function &)SymGetModuleInfo))
    Close();
}


#endif
#endif

void PAssertFunc(const char * msg)
{
  ostrstream str;
  str << msg;

#if defined(_WIN32) && defined(_M_IX86)
  PImageDLL imagehlp;
  if (imagehlp.IsLoaded()) {
    // Turn on load lines.
    imagehlp.SymSetOptions(imagehlp.SymGetOptions()|SYMOPT_LOAD_LINES);
    HANDLE hProcess;
    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof(ver);
    ::GetVersionEx(&ver);
    if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
      hProcess = GetCurrentProcess();
    else
      hProcess = (HANDLE)GetCurrentProcessId();
    if (imagehlp.SymInitialize(hProcess, NULL, TRUE)) {
      HANDLE hThread = GetCurrentThread();
      // The thread information.
      CONTEXT threadContext;
      threadContext.ContextFlags = CONTEXT_FULL ;
      if (GetThreadContext(hThread, &threadContext)) {
        STACKFRAME frame;
        memset(&frame, 0, sizeof(frame));

#if defined (_M_IX86)
#define IMAGE_FILE_MACHINE IMAGE_FILE_MACHINE_I386
        frame.AddrPC.Offset    = threadContext.Eip;
        frame.AddrPC.Mode      = AddrModeFlat;
        frame.AddrStack.Offset = threadContext.Esp;
        frame.AddrStack.Mode   = AddrModeFlat;
        frame.AddrFrame.Offset = threadContext.Ebp;
        frame.AddrFrame.Mode   = AddrModeFlat;

#elif defined (_M_ALPHA)
#define IMAGE_FILE_MACHINE IMAGE_FILE_MACHINE_ALPHA
        frame.AddrPC.Offset = (unsigned long)threadContext.Fir;
        frame.AddrPC.Mode   = AddrModeFlat;
#else
#error ( "Unknown machine!" )
#endif

        int frameCount = 0;
        while (frameCount++ < 16 &&
               imagehlp.StackWalk(IMAGE_FILE_MACHINE,
                                  hProcess,
                                  hThread,
                                  &frame,
                                  &threadContext,
                                  NULL, // ReadMemoryRoutine
                                  imagehlp.SymFunctionTableAccess,
                                  imagehlp.SymGetModuleBase,
                                  NULL)) {
          if (frameCount > 1 && frame.AddrPC.Offset != 0) {
            char buffer[sizeof(IMAGEHLP_SYMBOL)+100];
            PIMAGEHLP_SYMBOL symbol = (PIMAGEHLP_SYMBOL)buffer;
            symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
            symbol->MaxNameLength = sizeof(buffer)-sizeof(IMAGEHLP_SYMBOL);
            DWORD displacement = 0;
            if (imagehlp.SymGetSymFromAddr(hProcess,
                                           frame.AddrPC.Offset,
                                           &displacement,
                                           symbol)) {
              str << "\n    " << symbol->Name;
            }
            else {
              str << "\n    0x"
                  << hex << setfill('0')
                  << setw(8) << frame.AddrPC.Offset
                  << dec << setfill(' ');
            }
            str << '(' << hex << setfill('0');
            for (PINDEX i = 0; i < PARRAYSIZE(frame.Params); i++) {
              if (i > 0)
                str << ", ";
              if (frame.Params[i] != 0)
                str << "0x";
              str << frame.Params[i];
            }
            str << setfill(' ') << ')';
            if (displacement != 0)
              str << " + 0x" << displacement;
          }
        }

        if (frameCount <= 2) {
          DWORD e = ::GetLastError();
          str << "\n    No stack dump: IMAGEHLP.DLL StackWalk failed: error=" << e;
        }
      }
      else {
        DWORD e = ::GetLastError();
        str << "\n    No stack dump: IMAGEHLP.DLL GetThreadContext failed: error=" << e;
      }

      imagehlp.SymCleanup(hProcess);
    }
    else {
      DWORD e = ::GetLastError();
      str << "\n    No stack dump: IMAGEHLP.DLL SymInitialise failed: error=" << e;
    }
  }
  else {
    DWORD e = ::GetLastError();
    str << "\n    No stack dump: IMAGEHLP.DLL could not be loaded: error=" << e;
  }
#endif

  str << ends;
#ifdef _MSC_VER
  const char * pstr = str.str();
  // Unfreeze str so frees memory
  str.rdbuf()->freeze(0);
#else
  // Copy to local variable so char ptr does not become invalidated
  std::string sstr = str.str();
  const char * pstr = sstr.c_str ();
#endif
  // Must do nothing to str after this or it invalidates pstr

  if (PProcess::Current().IsServiceProcess()) {
#ifndef _WIN32_WCE
	PSystemLog::Output(PSystemLog::Fatal, pstr);
#if defined(_MSC_VER) && defined(_DEBUG)
    if (PServiceProcess::Current().debugMode)
      __asm int 3;
#endif
#endif // !_WIN32_WCE
    return;
  }

#if defined(_WIN32)
  static HANDLE mutex = CreateSemaphore(NULL, 1, 1, NULL);
  WaitForSingleObject(mutex, INFINITE);
#endif

  if (PProcess::Current().IsGUIProcess()) {
    switch (MessageBox(NULL, 
#if !defined(_WIN32_WCE)
		pstr, 
		"Portable Windows Library",
        MB_ABORTRETRYIGNORE|MB_ICONHAND|MB_TASKMODAL)) {
#else
		PString(pstr).AsUCS2(), 
		L"Portable Windows Library",
        MB_ABORTRETRYIGNORE|MB_ICONHAND)) {
#endif // !_WIN32_WCE
      case IDABORT :
#if !defined(_WIN32_WCE)
		  FatalExit(1);  // Never returns
#else
		  ExitProcess(1);
#endif // !_WIN32_WCE
      case IDRETRY :
        DebugBreak();
    }
#if defined(_WIN32)
    ReleaseSemaphore(mutex, 1, NULL);
#endif
    return;
  }

  for (;;) {
    cerr << pstr << "\n<A>bort, <B>reak, <I>gnore? ";
    cerr.flush();
    switch (cin.get()) {
      case 'A' :
      case 'a' :
        cerr << "Aborted" << endl;
        _exit(100);
        
      case 'B' :
      case 'b' :
        cerr << "Break" << endl;
#if defined(_WIN32)
        ReleaseSemaphore(mutex, 1, NULL);
#endif
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
        __asm int 3;
#endif

      case 'I' :
      case 'i' :
      case EOF :
        cerr << "Ignored" << endl;
#if defined(_WIN32)
        ReleaseSemaphore(mutex, 1, NULL);
#endif
        return;
    }
  }
}


// End Of File ///////////////////////////////////////////////////////////////
