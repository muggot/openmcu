/*
 * contain.h
 *
 * General container classes.
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
 * $Log: contain.h,v $
 * Revision 1.66  2007/10/03 20:52:27  dsandras
 * Applied patch from Matthias Schneider for mingw compilation. Thanks a lot!
 *
 * Revision 1.65  2007/10/03 01:18:44  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.64  2007/09/30 17:34:40  dsandras
 * Killed GCC 4.2 warnings.
 *
 * Revision 1.63  2007/09/27 03:28:47  rjongbloed
 * Remove warnings over using posix functions.
 *
 * Revision 1.62  2007/06/26 03:08:29  rjongbloed
 * Assure some MSVC system symbols are not redefined if already defined.
 *
 * Revision 1.61  2007/04/08 01:53:16  ykiryanov
 * Build to support ptlib dll creation
 *
 * Revision 1.60  2007/04/04 06:04:33  ykiryanov
 * This is a first cut of Windows Mobile 5.0 PocketPC SDK ARM4I port
 *
 * Revision 1.59  2007/02/10 07:25:18  csoutheren
 * Set WIN32 to a value to provide compatibility with headers that use this define in expressions
 *
 * Revision 1.58  2006/07/27 10:30:49  rjongbloed
 * Changed _CRT_SECURE_NO_DEPRECATE definition to be compatible with wxWidgets, avoiding warning
 *
 * Revision 1.57  2006/04/09 11:44:21  csoutheren
 * And then refix VS.net 2003
 *
 * Revision 1.56  2006/04/09 11:03:59  csoutheren
 * Remove warnings on VS.net 2005
 *
 * Revision 1.55  2005/09/25 10:51:23  dominance
 * almost complete the mingw support. We'll be there soon. ;)
 *
 * Revision 1.54  2005/09/24 09:11:42  dominance
 * use unix-style slashes to not confuse mingw on win32
 *
 * Revision 1.53  2005/09/23 15:30:46  dominance
 * more progress to make mingw compile nicely. Thanks goes to Julien Puydt for pointing out to me how to do it properly. ;)
 *
 * Revision 1.52  2005/09/18 13:01:43  dominance
 * fixed pragma warnings when building with gcc.
 *
 * Revision 1.51  2005/03/10 06:37:19  csoutheren
 * Removed use of typeid on WIndows to get class name because it is not threadsafe
 * In fact, lets just use #classname everywhere because that will always work
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.50  2004/10/30 19:23:45  ykiryanov
 * Ifdefd strcasecmp for WinCE port
 *
 * Revision 1.49  2004/10/23 11:34:14  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.48  2004/08/15 06:45:48  rjongbloed
 * Disabled warning about conversion from int to bool type.
 *
 * Revision 1.47  2004/06/30 12:17:04  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.46  2004/06/19 09:10:24  csoutheren
 * Removed MSVC warnings for <queue>
 *
 * Revision 1.45  2004/06/01 23:25:20  csoutheren
 * Disabled warnings under MSVC
 *
 * Revision 1.44  2004/06/01 05:22:43  csoutheren
 * Restored memory check functionality
 *
 * Revision 1.43  2004/05/19 23:34:43  csoutheren
 * Added <algorithm>
 *
 * Revision 1.42  2004/05/13 14:53:35  csoutheren
 * Add "abstract factory" template classes
 *
 * Revision 1.41  2004/04/15 03:58:40  csoutheren
 * Removed PCONTAINER_USES_CRITSEC
 *
 * Revision 1.40  2004/04/13 10:12:21  csoutheren
 * Fix for MSVC (grrr)
 *
 * Revision 1.39  2004/04/13 03:13:29  csoutheren
 * VS.net won't compile without "using namespace std"
 *
 * Revision 1.38  2004/04/12 03:04:10  csoutheren
 * Removed <vector> warnings under Windows
 *
 * Revision 1.37  2004/04/11 22:47:03  csoutheren
 * Remove unused parameter warnings for Windows <string>
 *
 * Revision 1.36  2004/04/11 13:26:25  csoutheren
 * Removed namespace problems and removed warnings for Windows <string>
 *
 * Revision 1.35  2004/04/11 02:55:18  csoutheren
 * Added PCriticalSection for Windows
 * Added compile time option for PContainer to use critical sections to provide thread safety under some circumstances
 *
 * Revision 1.34  2004/04/09 06:40:48  rjongbloed
 * Removed warning in VC++2003 version about wspapi.h
 *
 * Revision 1.33  2004/04/09 00:42:59  csoutheren
 * Changed Unix build to use slightly different method for
 * keep class names, as GCC does not use actual class names for typeinfo
 *
 * Revision 1.32  2004/04/03 08:22:20  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.31  2004/04/03 06:54:23  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.30  2004/01/06 21:17:38  csoutheren
 * Fixed formatting of warning message regarding old include system
 * Thanks to Louis R. Marascio
 *
 * Revision 1.29  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.28  2002/09/23 07:17:23  robertj
 * Changes to allow winsock2 to be included.
 *
 * Revision 1.27  2002/03/15 05:58:52  robertj
 * Added strncasecmp macro for unix compatibility
 *
 * Revision 1.26  2000/05/02 02:58:09  robertj
 * Added strcasecmp macro, BSDish version of stricmp
 *
 * Revision 1.25  2000/02/28 11:39:52  robertj
 * Removed warning for if STRICT (for windows.h) already defined.
 *
 * Revision 1.24  1999/03/09 10:30:17  robertj
 * Fixed ability to have PMEMORY_CHECK on/off on both debug/release versions.
 *
 * Revision 1.23  1998/11/30 02:55:06  robertj
 * New directory structure
 *
 * Revision 1.22  1998/10/13 14:13:16  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.21  1998/09/24 03:29:57  robertj
 * Added open software license.
 *
 * Revision 1.20  1998/03/09 07:15:15  robertj
 * Added support for MemCheck-32 program.
 *
 * Revision 1.19  1998/01/26 00:52:13  robertj
 * Fixed istream << operator for PInt64, should be reference.
 *
 * Revision 1.18  1997/07/08 13:01:30  robertj
 * DLL support.
 *
 * Revision 1.17  1997/01/12 04:13:07  robertj
 * Changed library to support NT 4.0 API changes.
 *
 * Revision 1.16  1996/09/14 12:38:57  robertj
 * Moved template define from command line to code.
 * Fixed correct application of windows defines.
 *
 * Revision 1.15  1996/08/17 10:00:33  robertj
 * Changes for Windows DLL support.
 *
 * Revision 1.14  1996/08/08 10:08:58  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.13  1996/07/15 10:26:55  robertj
 * MSVC 4.1 Support
 *
 * Revision 1.12  1996/03/31 09:07:29  robertj
 * Removed bad define in NT headers.
 *
 * Revision 1.11  1996/01/28 02:54:27  robertj
 * Removal of MemoryPointer classes as usage didn't work for GNU.
 *
 * Revision 1.10  1996/01/23 13:23:15  robertj
 * Added const version of PMemoryPointer
 *
 * Revision 1.9  1995/11/09 12:23:46  robertj
 * Added 64 bit integer support.
 * Added platform independent base type access classes.
 *
 * Revision 1.8  1995/04/25 11:31:18  robertj
 * Changes for DLL support.
 *
 * Revision 1.7  1995/03/12 04:59:54  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.6  1995/01/09  12:28:45  robertj
 * Moved EXPORTED definition from applicat.h
 *
 * Revision 1.5  1995/01/06  10:47:08  robertj
 * Added 64 bit integer.
 *
 * Revision 1.4  1994/11/19  00:18:26  robertj
 * Changed PInteger to be INT, ie standard type like BOOL/WORD etc.
 *
 * Revision 1.3  1994/07/02  03:18:09  robertj
 * Support for 16 bit systems.
 *
 * Revision 1.2  1994/06/25  12:13:01  robertj
 * Synchronisation.
 *
 * Revision 1.1  1994/04/01  14:38:42  robertj
 * Initial revision
 *
 */

#ifndef _CONTAIN_H
#ifndef _WIN32_WCE
#error "Please remove pwlib\include\ptlib\msos from the tool include path" \
"and from the pre-processor options for this project"
#endif // !_WIN32_WCE
#endif

#ifndef _OBJECT_H
#define _OBJECT_H


#ifdef _MSC_VER

  #pragma warning(disable:4201)  // nonstandard extension: nameless struct/union
  #pragma warning(disable:4251)  // disable warning exported structs
  #pragma warning(disable:4511)  // default copy ctor not generated warning
  #pragma warning(disable:4512)  // default assignment op not generated warning
  #pragma warning(disable:4514)  // unreferenced inline removed
  #pragma warning(disable:4699)  // precompiled headers
  #pragma warning(disable:4702)  // disable warning about unreachable code
  #pragma warning(disable:4705)  // disable warning about statement has no effect
  #pragma warning(disable:4710)  // inline not expanded warning
  #pragma warning(disable:4711)  // auto inlining warning
  #pragma warning(disable:4786)  // identifier was truncated to '255' characters in the debug information
  #pragma warning(disable:4097)  // typedef synonym for class
  #pragma warning(disable:4800)  // forcing value to bool 'true' or 'false' (performance warning)

  #if _MSC_VER>=800
    #define PHAS_TEMPLATES
  #endif

  #if !defined(__USE_STL__) && (_MSC_VER>=1300)
    #define __USE_STL__ 1
  #endif

  #if !defined(_CRT_SECURE_NO_DEPRECATE) && (_MSC_VER>=1400)
    #define _CRT_SECURE_NO_DEPRECATE 1
  #endif

  #if !defined(_CRT_NONSTDC_NO_WARNINGS) && (_MSC_VER>=1400)
    #define _CRT_NONSTDC_NO_WARNINGS 1
  #endif

#endif // _MSC_VER


///////////////////////////////////////////////////////////////////////////////
// Machine & Compiler dependent declarations

#if defined(_WIN32) && !defined(WIN32)
  #define WIN32  1
#endif

#if defined(_WINDOWS) || defined(_WIN32)

  #ifndef WINVER
  #define WINVER 0x401
  #endif

  #ifndef STRICT
  #define STRICT
  #endif

  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #endif

  #include <windows.h>

  #undef DELETE   // Remove define from NT headers.


#else

  typedef unsigned char  BYTE;  //  8 bit unsigned integer quantity
  typedef unsigned short WORD;  // 16 bit unsigned integer quantity
  typedef unsigned long  DWORD; // 32 bit unsigned integer quantity
  typedef int            BOOL;  // type returned by expresion (i != j)

  #define TRUE 1
  #define FALSE 0

  #define NEAR __near

#endif


// Declaration for exported callback functions to OS
#if defined(_WIN32)
  #define PEXPORTED __stdcall
#elif defined(_WINDOWS)
  #define PEXPORTED WINAPI __export
#else
  #define PEXPORTED __far __pascal
#endif


// Declaration for static global variables (WIN16 compatibility)
#if defined(_WIN32)
  #define PSTATIC
#else
  #define PSTATIC __near
#endif


// Declaration for platform independent architectures
#define PCHAR8 PANSI_CHAR
#define PBYTE_ORDER PLITTLE_ENDIAN


// Declaration for integer that is the same size as a void *
#if defined(_WIN32)
  typedef int INT;
#else
  typedef long INT;   
#endif


// Declaration for signed integer that is 16 bits
typedef short PInt16;

// Declaration for signed integer that is 32 bits
typedef long PInt32;

#ifdef __MINGW32__
  #define __USE_STL__
  using namespace std;
  #define P_HAS_INT64
  typedef signed __int64 PInt64;
  typedef unsigned __int64 PUInt64;
#endif

// Declaration for 64 bit unsigned integer quantity
#if defined(_MSC_VER)

  #define P_HAS_INT64

  typedef signed __int64 PInt64;
  typedef unsigned __int64 PUInt64;

  #if _MSC_VER<1300

    class ostream;
    class istream;

    ostream & operator<<(ostream & s, PInt64 v);
    ostream & operator<<(ostream & s, PUInt64 v);

    istream & operator>>(istream & s, PInt64 & v);
    istream & operator>>(istream & s, PUInt64 & v);

  #endif

#endif


// Standard array index type (depends on compiler)
// Type used in array indexes especially that required by operator[] functions.
#if defined(_MSC_VER) || defined(__MINGW32__)

  #define PINDEX int
  #if defined(_WIN32)
    const PINDEX P_MAX_INDEX = 0x7fffffff;
  #else
    const PINDEX P_MAX_INDEX = 0x7fff;
  #endif
    inline PINDEX PABSINDEX(PINDEX idx) { return (idx < 0 ? -idx : idx)&P_MAX_INDEX; }
  #define PASSERTINDEX(idx) PAssert((idx) >= 0, PInvalidArrayIndex)

#else

  #define PINDEX unsigned
  #ifndef SIZEOF_INT
  # define SIZEOF_INT sizeof(int)
  #endif
  #if SIZEOF_INT == 4
     const PINDEX P_MAX_INDEX = 0xffffffff;
  #else
     const PINDEX P_MAX_INDEX = 0xffff;
  #endif
  #define PABSINDEX(idx) (idx)
  #define PASSERTINDEX(idx)

#endif

#ifndef _WIN32_WCE 

  #if _MSC_VER>=1400
    #define strcasecmp(s1,s2) _stricmp(s1,s2)
    #define strncasecmp(s1,s2,n) _strnicmp(s1,s2,n)
  #else
    #define strcasecmp(s1,s2) stricmp(s1,s2)
    #define strncasecmp(s1,s2,n) strnicmp(s1,s2,n)
    //#define _putenv ::putenv
    //#define _close ::close
    //#define _access ::access
  #endif

#endif // !_WIN32_WCE 


class PWin32Overlapped : public OVERLAPPED
{
  // Support class for overlapped I/O in Win32.
  public:
    PWin32Overlapped();
    ~PWin32Overlapped();
    void Reset();
};


enum { PWIN32ErrorFlag = 0x40000000 };

class PString;

class RegistryKey
{
  public:
    enum OpenMode {
      ReadOnly,
      ReadWrite,
      Create
    };
    RegistryKey(const PString & subkey, OpenMode mode);
    ~RegistryKey();

    BOOL EnumKey(PINDEX idx, PString & str);
    BOOL EnumValue(PINDEX idx, PString & str);
    BOOL DeleteKey(const PString & subkey);
    BOOL DeleteValue(const PString & value);
    BOOL QueryValue(const PString & value, PString & str);
    BOOL QueryValue(const PString & value, DWORD & num, BOOL boolean);
    BOOL SetValue(const PString & value, const PString & str);
    BOOL SetValue(const PString & value, DWORD num);
  private:
    HKEY key;
};

#ifndef _WIN32_WCE
  extern "C" int PASCAL WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
#else
  extern "C" int PASCAL WinMain(HINSTANCE, HINSTANCE, LPTSTR, int);
#endif

// used by various modules to disable the winsock2 include to avoid header file problems
#ifndef P_KNOCKOUT_WINSOCK2

  #if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4127 4706)
  #endif

  #if defined(P_WINSOCKv1)

    #include <winsock.h>

  #else // P_WINSOCKv1

    ///IPv6 support
    // Needed for for IPv6 socket API. Must be included before "windows.h"
    #include <winsock2.h> // Version 2 of windows socket
    #include <ws2tcpip.h> // winsock2 is not complete, ws2tcpip add some defines such as IP_TOS

    #if P_HAS_IPV6 && !defined IPPROTO_IPV6
      #include "tpipv6.h"  // For IPv6 Tech Preview.
    #endif

  #endif // P_WINSOCKv1

  #if defined(_MSC_VER)
    #pragma warning(pop)
  #endif

  #define PIPX

  typedef int socklen_t;

#endif  // P_KNOCKOUT_WINSOCK2

#if defined(_MSC_VER) && !defined(_WIN32)
  extern "C" int __argc;
  extern "C" char ** __argv;
#endif

#ifdef __BORLANDC__
  #define __argc _argc
  #define __argv _argv
#endif

#undef Yield

#define P_THREADIDENTIFIER DWORD

#if defined(_MSC_VER)
  #pragma warning(disable:4201)
#endif

#include <malloc.h>
#include <mmsystem.h>


#ifndef _WIN32_WCE
#ifdef _MSC_VER
  #include <crtdbg.h>
#endif
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <errno.h>
  #include <io.h>
  #include <fcntl.h>
  #include <direct.h>
  #include <vfw.h>
#else
  #include <ptlib/wm/stdlibx.h>
  #include <ptlib/wm/errno.h>
  #include <ptlib/wm/sys/types.h>
  #include <ptlib/wm/time.h>
  #include <ptlib/wm/cevfw.h>
#endif

#define   P_HAS_TYPEINFO  1

//#define   PCONTAINER_USES_CRITSEC   1

// preload <string> and kill warnings
#if defined(_MSC_VER)
  #pragma warning(push)
  #include <yvals.h>    
  #pragma warning(disable:4100)
  #pragma warning(disable:4018)
  #pragma warning(disable:4663)
  #pragma warning(disable:4146)
  #pragma warning(disable:4244)
  #pragma warning(disable:4786)
#endif
#include <string>
#if defined(_MSC_VER)
  #pragma warning(pop)
#endif

// preload <vector> and kill warnings
#if defined(_MSC_VER)
  #pragma warning(push)
  #include <yvals.h>    
  #pragma warning(disable:4018)
  #pragma warning(disable:4663)
  #pragma warning(disable:4786)
#endif
#include <vector>
#if defined(_MSC_VER)
  #pragma warning(pop)
#endif

// preload <map> and kill warnings
#if defined(_MSC_VER)
  #pragma warning(push)
  #include <yvals.h>    
  #pragma warning(disable:4018)
  #pragma warning(disable:4663)
  #pragma warning(disable:4786)
#endif
#include <map>
#if defined(_MSC_VER)
  #pragma warning(pop)
#endif

// preload <utility> and kill warnings
#if defined(_MSC_VER)
  #pragma warning(push)
  #include <yvals.h>    
  #pragma warning(disable:4786)
#endif
#include <utility>
#if defined(_MSC_VER)
  #pragma warning(pop)
#endif

// preload <iterator> and kill warnings
#if defined(_MSC_VER)
  #pragma warning(push)
  #include <yvals.h>    
  #pragma warning(disable:4786)
#endif
#include <iterator>
#if defined(_MSC_VER)
  #pragma warning(pop)
#endif

// preload <algorithm> and kill warnings
#include <algorithm>

// preload <queue> and kill warnings
#if defined(_MSC_VER)
  #pragma warning(push)
  #include <yvals.h>    
  #pragma warning(disable:4284)
#endif
#include <queue>
#if defined(_MSC_VER)
  #pragma warning(pop)
#endif

// VS.net won't work without this :(
#if _MSC_VER>=1300
  using namespace std;
#endif

#if defined(_MSC_VER)
  #pragma warning(disable:4786)
#endif


#endif // _OBJECT_H


// End Of File ///////////////////////////////////////////////////////////////
