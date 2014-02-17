/*
 * object.h
 *
 * Mother of all ancestor classes.
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
 * $Log: object.h,v $
 * Revision 1.131  2007/10/03 01:18:44  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.130  2007/09/17 11:14:42  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.129  2007/09/15 21:46:50  dsandras
 * Applied patch from Matthias Schneider to fix WIN32 build. Many thanks!
 *
 * Revision 1.128  2007/09/12 18:43:43  ykiryanov
 * Added !defined(_WIN32_WCE)  to new memory leakage dectection code. Memory organized other way on WCE based devices and crtdbg.h API  is not present
 *
 * Revision 1.127  2007/09/12 00:55:41  rjongbloed
 * Improved memory leak detection, reduce false positives.
 *
 * Revision 1.126  2007/09/09 10:29:49  rjongbloed
 * Fixed DevStudio 2003 build with memory check code.
 *
 * Revision 1.125  2007/09/09 09:42:48  rjongbloed
 * Fixed DevStudio 2003 build with memory check code.
 *
 * Revision 1.124  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.123  2007/08/17 08:46:01  csoutheren
 * Remove unnecessary inclusions of <iostream>
 *
 * Revision 1.122  2007/07/16 05:38:20  csoutheren
 * Fix compile problem when enabling memory checking on Windows
 *
 * Revision 1.121  2007/07/06 02:11:48  csoutheren
 * Add extra memory leak debugging on Linux
 * Remove compile warnings
 *
 * Revision 1.120  2007/05/16 07:54:21  csoutheren
 * Fix problems created by gcc 4.2.0
 *
 * Revision 1.119  2006/07/14 04:55:09  csoutheren
 * Applied 1520151 - Adds PID to tracefile + Rolling Date pattern
 * Thanks to Paul Nader
 *
 * Revision 1.118  2006/06/20 09:49:16  csoutheren
 * Applied 1489468
 * Fix stlport on old gcc/non-gcc compilers
 * Thanks to Adam Butcher
 *
 * Revision 1.117  2006/04/10 23:57:27  csoutheren
 * Checked in changes to remove some warnings with gcc effc++ flag
 *
 * Revision 1.116  2006/03/20 00:24:56  csoutheren
 * Applied patch #1446482
 * Thanks to Adam Butcher
 *
 * Revision 1.115  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.114  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.113  2005/09/18 11:05:36  dominance
 * include/ptlib/channel.h, include/ptlib/pstring.h, src/ptlib/common/contain.cxx,
 * src/ptlib/common/pchannel.cxx:
 *   correct the STL defined checking to use proper syntax.
 *
 * include/ptlib/object.h:
 *   re-add typedef to compile on mingw
 *
 * make/ptlib-config.in:
 *   import a long-standing fix from the Debian packs which allows usage of
 *   ptlib-config without manually adding -lpt for each of the subsequent
 *   projects
 *
 * Revision 1.112  2005/08/30 06:36:39  csoutheren
 * Added ability to rotate output logs on a daily basis
 *
 * Revision 1.111  2005/03/10 06:37:20  csoutheren
 * Removed use of typeid on WIndows to get class name because it is not threadsafe
 * In fact, lets just use #classname everywhere because that will always work
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.110  2004/08/14 14:17:29  csoutheren
 * Fixed problem with PAssert and associated functions caused by using expressions
 * as statements. inline functions are your friend :)
 *
 * Revision 1.109  2004/08/05 12:09:35  rjongbloed
 * Added macros for "remove const" and "down cast" funcions with and without RTTI.
 * Added ability to disable Asserts.
 * Change PAssert macros so pass through the boolean result so that they can be used
 *   in if statements, allowing a chance to continue if ignore assert.
 *
 * Revision 1.108  2004/07/11 07:56:35  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.107  2004/07/03 06:49:49  rjongbloed
 * Added PTRACE_PARAM() macro to fix warnings on parameters used in PTRACE
 *  macros only.
 *
 * Revision 1.106  2004/06/01 07:42:19  csoutheren
 * Restored memory allocation checking
 * Added configure flag to enable, thanks to Derek Smithies
 *
 * Revision 1.105  2004/06/01 05:22:43  csoutheren
 * Restored memory check functionality
 *
 * Revision 1.104  2004/05/12 04:36:17  csoutheren
 * Fixed problems with using sem_wait and friends on systems that do not
 * support atomic integers
 *
 * Revision 1.103  2004/04/18 04:33:36  rjongbloed
 * Changed all operators that return BOOL to return standard type bool. This is primarily
 *   for improved compatibility with std STL usage removing many warnings.
 *
 * Revision 1.102  2004/04/11 13:26:25  csoutheren
 * Removed namespace problems and removed warnings for Windows <string>
 *
 * Revision 1.101  2004/04/11 03:20:41  csoutheren
 * Added Unix implementation of PCriticalSection
 *
 * Revision 1.100  2004/04/11 02:55:17  csoutheren
 * Added PCriticalSection for Windows
 * Added compile time option for PContainer to use critical sections to provide thread safety under some circumstances
 *
 * Revision 1.99  2004/04/09 11:54:46  csoutheren
 * Added configure.in check for STL streams, and tested with gcc 2.95.3,
 * gcc 3.3.1, and gcc 3.3.3
 *
 * Revision 1.98  2004/04/09 07:53:51  rjongbloed
 * Fixed backward compatibility after STL streams change
 *
 * Revision 1.97  2004/04/09 00:56:35  csoutheren
 * Fixed problem with new class name code
 *
 * Revision 1.96  2004/04/09 00:42:58  csoutheren
 * Changed Unix build to use slightly different method for
 * keep class names, as GCC does not use actual class names for typeinfo
 *
 * Revision 1.95  2004/04/04 13:24:18  rjongbloed
 * Changes to support native C++ Run Time Type Information
 *
 * Revision 1.94  2004/04/03 08:57:31  csoutheren
 * Replaced pseudo-RTTI with real RTTI
 *
 * Revision 1.93  2004/04/03 08:22:20  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.92  2004/04/03 07:41:00  csoutheren
 * Fixed compile problem with ostringstream/ostrstream
 *
 * Revision 1.91  2004/04/03 07:16:05  rjongbloed
 * Fixed backward compatibility with MSVC 6
 *
 * Revision 1.90  2004/04/03 06:54:22  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.89  2003/09/17 09:00:59  csoutheren
 * Moved PSmartPointer and PNotifier into seperate files
 * Added detection for system regex libraries on all platforms
 *
 * Revision 1.88  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.87  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.86  2002/10/14 21:42:37  rogerh
 * Only use malloc.h on Windows
 *
 * Revision 1.85  2002/10/10 04:43:43  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.84  2002/10/08 12:41:51  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.83  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.82  2002/08/06 02:27:58  robertj
 * GNU C++ v3 compatibility.
 *
 * Revision 1.81  2002/06/25 02:22:47  robertj
 * Improved assertion system to allow C++ class name to be displayed if
 *   desired, especially relevant to container classes.
 *
 * Revision 1.80  2002/06/14 10:29:43  rogerh
 * STL + gcc 3.1 compile fix. Submitted by Klaus Kaempf <kkaempf@suse.de>
 *
 * Revision 1.79  2002/06/13 08:34:05  rogerh
 * gcc 3.1 needs iostream instead of iostream.h
 *
 * Revision 1.78  2002/05/22 00:23:31  craigs
 * Added GMTTime flag to tracing options
 *
 * Revision 1.77  2002/04/19 00:20:51  craigs
 * Added option to append to log file rather than create anew each time
 *
 * Revision 1.76  2002/01/26 23:55:55  craigs
 * Changed for GCC 3.0 compatibility, thanks to manty@manty.net
 *
 * Revision 1.75  2001/10/18 19:56:26  yurik
 * Fixed WinCE x86 compilation problems with memory check off
 *
 * Revision 1.74  2001/08/12 11:26:07  robertj
 * Put back PMEMORY_CHECK taken out by the Carbon port.
 *
 * Revision 1.73  2001/08/11 07:57:30  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.72  2001/05/03 06:27:29  robertj
 * Added return value to PMemoryCheck::SetIgnoreAllocations() so get previous state.
 *
 * Revision 1.71  2001/03/24 01:11:10  robertj
 * Added missing PTRACE_IF define in non PTRACING mode.
 *
 * Revision 1.70  2001/03/23 05:34:09  robertj
 * Added PTRACE_IF to output trace if a conditional is TRUE.
 *
 * Revision 1.69  2001/03/01 02:15:16  robertj
 * Fixed PTRACE_LINE() so drops filename and line which may not be in trace otherwise.
 *
 * Revision 1.68  2001/02/22 08:16:41  robertj
 * Added standard trace file setup subroutine.
 *
 * Revision 1.67  2001/02/13 03:27:24  robertj
 * Added function to do heap validation.
 *
 * Revision 1.66  2001/02/09 04:41:27  robertj
 * Removed added non memrycheck implementations of new/delete when using GNU C++.
 *
 * Revision 1.65  2001/02/07 04:47:49  robertj
 * Added changes for possible random crashes in multi DLL environment
 *   due to memory allocation wierdness, thanks Milan Dimitrijevic.
 *
 * Revision 1.64  2001/01/24 06:15:44  yurik
 * Windows CE port-related declarations
 *
 * Revision 1.63  2000/07/28 05:13:47  robertj
 * Fixed silly mistake in runtime_malloc() function, should return a pointer!
 *
 * Revision 1.62  2000/07/20 05:46:34  robertj
 * Added runtime_malloc() function for cases where memory check code must be bypassed.
 *
 * Revision 1.61  2000/07/13 15:45:35  robertj
 * Removed #define std that causes everyone so much grief!
 *
 * Revision 1.60  2000/06/26 11:17:19  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.59  2000/02/29 12:26:14  robertj
 * Added named threads to tracing, thanks to Dave Harvey
 *
 * Revision 1.58  2000/01/07 12:31:12  robertj
 * Fixed 8 byte alignment on memory heap checking.
 *
 * Revision 1.57  2000/01/05 00:29:12  robertj
 * Fixed alignment problems in memory checking debug functions.
 *
 * Revision 1.56  1999/11/30 00:22:54  robertj
 * Updated documentation for doc++
 *
 * Revision 1.55  1999/11/01 00:10:27  robertj
 * Added override of new functions for MSVC memory check code.
 *
 * Revision 1.54  1999/10/19 09:21:30  robertj
 * Added functions to get current trace options and level.
 *
 * Revision 1.53  1999/09/13 13:15:06  robertj
 * Changed PTRACE so will output to system log in PServiceProcess applications.
 *
 * Revision 1.52  1999/08/24 08:15:23  robertj
 * Added missing operator on smart pointer to return the pointer!
 *
 * Revision 1.51  1999/08/24 06:54:36  robertj
 * Cleaned up the smart pointer code (macros).
 *
 * Revision 1.50  1999/08/22 13:38:39  robertj
 * Fixed termination hang up problem with memory check code under unix pthreads.
 *
 * Revision 1.49  1999/08/17 03:46:40  robertj
 * Fixed usage of inlines in optimised version.
 *
 * Revision 1.48  1999/08/10 10:45:09  robertj
 * Added mutex in memory check detection code.
 *
 * Revision 1.47  1999/07/18 15:08:24  robertj
 * Fixed 64 bit compatibility
 *
 * Revision 1.46  1999/06/14 07:59:37  robertj
 * Enhanced tracing again to add options to trace output (timestamps etc).
 *
 * Revision 1.45  1999/05/01 11:29:19  robertj
 * Alpha linux port changes.
 *
 * Revision 1.44  1999/04/18 12:58:39  robertj
 * MSVC 5 backward compatibility
 *
 * Revision 1.43  1999/03/09 10:30:17  robertj
 * Fixed ability to have PMEMORY_CHECK on/off on both debug/release versions.
 *
 * Revision 1.42  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.41  1999/02/23 07:11:26  robertj
 * Improved trace facility adding trace levels and #define to remove all trace code.
 *
 * Revision 1.40  1999/02/22 10:48:14  robertj
 * Fixed delete operator prototypes for MSVC6 and GNU compatibility.
 *
 * Revision 1.39  1999/02/19 11:33:02  robertj
 * Fixed compatibility problems with GNU/MSVC6
 *
 * Revision 1.38  1999/02/16 08:12:22  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.37  1999/01/07 03:35:35  robertj
 * Added default for PCHAR8 to ANSI, removes need for compiler option.
 *
 * Revision 1.36  1998/12/15 09:00:29  robertj
 * Fixed 8 byte alignment problem in memory leak check code for sparc.
 *
 * Revision 1.35  1998/11/03 00:57:19  robertj
 * Added allocation breakpoint variable.
 *
 * Revision 1.34  1998/10/26 11:05:26  robertj
 * Added raw free for things allocated within the runtime library.
 *
 * Revision 1.33  1998/10/18 14:26:55  robertj
 * Improved tracing functions.
 *
 * Revision 1.32  1998/10/15 07:47:21  robertj
 * Added ability to ignore G++lib memory leaks.
 *
 * Revision 1.31  1998/10/15 01:53:58  robertj
 * GNU compatibility.
 *
 * Revision 1.30  1998/10/13 14:23:29  robertj
 * Complete rewrite of memory leak detection.
 *
 * Revision 1.29  1998/09/23 06:20:57  robertj
 * Added open source copyright license.
 *
 * Revision 1.28  1998/09/14 12:29:11  robertj
 * Fixed memory leak dump under windows to not include static globals.
 * Fixed problem with notifier declaration not allowing implementation inline after macro.
 *
 * Revision 1.27  1997/07/08 13:13:45  robertj
 * DLL support.
 *
 * Revision 1.26  1997/04/27 05:50:11  robertj
 * DLL support.
 *
 * Revision 1.25  1997/02/05 11:54:10  robertj
 * Fixed problems with memory check and leak detection.
 *
 * Revision 1.24  1996/09/16 12:57:23  robertj
 * DLL support
 *
 * Revision 1.23  1996/08/17 10:00:23  robertj
 * Changes for Windows DLL support.
 *
 * Revision 1.22  1996/07/15 10:27:51  robertj
 * Changed endian classes to be memory mapped.
 *
 * Revision 1.21  1996/05/09 12:14:48  robertj
 * Fixed up 64 bit integer class for Mac platform.
 *
 * Revision 1.20  1996/02/24 14:19:29  robertj
 * Fixed bug in endian independent integer code for memory transfers.
 *
 * Revision 1.19  1996/01/28 02:46:43  robertj
 * Removal of MemoryPointer classes as usage didn't work for GNU.
 * Added missing bit shift operators to 64 bit integer class.
 *
 * Revision 1.18  1996/01/23 13:14:32  robertj
 * Added const version of PMemoryPointer.
 * Added constructor to endian classes for the base type.
 *
 * Revision 1.17  1996/01/02 11:54:11  robertj
 * Mac OS compatibility changes.
 *
 * Revision 1.16  1995/11/09 12:17:10  robertj
 * Added platform independent base type access classes.
 *
 * Revision 1.15  1995/06/17 11:12:47  robertj
 * Documentation update.
 *
 * Revision 1.14  1995/06/04 12:34:19  robertj
 * Added trace functions.
 *
 * Revision 1.13  1995/04/25 12:04:35  robertj
 * Fixed borland compatibility.
 * Fixed function hiding ancestor virtuals.
 *
 * Revision 1.12  1995/03/14 12:41:54  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.11  1995/03/12  04:40:55  robertj
 * Changed standard error code for not open from file to channel.
 *
 * Revision 1.10  1995/02/19  04:19:14  robertj
 * Added dynamically linked command processing.
 *
 * Revision 1.9  1995/02/05  00:48:07  robertj
 * Fixed template version.
 *
 * Revision 1.8  1995/01/15  04:51:31  robertj
 * Mac compatibility.
 * Added levels of memory checking.
 *
 * Revision 1.7  1995/01/09  12:38:31  robertj
 * Changed variable names around during documentation run.
 * Fixed smart pointer comparison.
 * Fixed serialisation stuff.
 * Documentation.
 *
 * Revision 1.6  1995/01/03  09:39:06  robertj
 * Put standard malloc style memory allocation etc into memory check system.
 *
 * Revision 1.5  1994/12/12  10:08:30  robertj
 * Renamed PWrapper to PSmartPointer..
 *
 * Revision 1.4  1994/12/05  11:23:28  robertj
 * Fixed PWrapper macros.
 *
 * Revision 1.3  1994/11/19  00:22:55  robertj
 * Changed PInteger to be INT, ie standard type like BOOL/WORD etc.
 * Moved null object check in notifier to construction rather than use.
 * Added virtual to the callback function in notifier destination class.
 *
 * Revision 1.2  1994/11/03  09:25:30  robertj
 * Made notifier destination object not to be descendent of PObject.
 *
 * Revision 1.1  1994/10/30  12:01:37  robertj
 * Initial revision
 *
 */

#ifndef _POBJECT_H
#define _POBJECT_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifdef _WIN32
#include "msos/ptlib/contain.h"
#else
#include "unix/ptlib/contain.h"
#endif

#if defined(P_VXWORKS)
#include <private/stdiop.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <string.h>

#ifdef __USE_STL__
  #include <string>
  #include <iomanip>
  #include <iostream>
  #if (__GNUC__ >= 3)
    #include <sstream>
    typedef std::ostringstream ostrstream;
  #else
    #include <strstream>
  #endif
  //using namespace std;
  #ifdef _STLP_IOS_BASE_H
  typedef std::ios_base::fmtflags _Ios_Fmtflags;
  #endif
#else
  #if (__GNUC__ >= 3)
    #include <iosfwd>
    #ifndef __MWERKS__
      #include <iomanip>
    #endif
  #else
    #include <iosfwd>
    #ifdef __GNUC__
      #include <strstream.h>
    #else
      #include <strstrea.h>
    #endif
    #ifndef __MWERKS__
      #include <iomanip.h>
    #endif
  #endif
#endif

#if (__GNUC__ < 3) && !defined(_STLP_IOS_BASE_H)
typedef long _Ios_Fmtflags;
#endif

#if _MSC_VER<1300
#define _BADOFF -1
#endif

///////////////////////////////////////////////////////////////////////////////
// Disable inlines when debugging for faster compiles (the compiler doesn't
// actually inline the function with debug on any way).

#ifndef P_USE_INLINES
#ifdef _DEBUG
#define P_USE_INLINES 0
#else
#define P_USE_INLINES 0
#endif
#endif

#if P_USE_INLINES
#define PINLINE inline
#else
#define PINLINE
#endif


///////////////////////////////////////////////////////////////////////////////
// Declare the debugging support

#ifndef P_USE_ASSERTS
#define P_USE_ASSERTS 1
#endif

#if !P_USE_ASSERTS

#define PAssert(b, m) (b)
#define PAssert2(b, c, m) (b)
#define PAssertOS(b) (b)
#define PAssertNULL(p) (p)
#define PAssertAlways(m)
#define PAssertAlways2(c, m)

#else // P_USE_ASSERTS

/// Standard assert messages for the PAssert macro.
enum PStandardAssertMessage {
  PLogicError,              // A logic error occurred.
  POutOfMemory,             // A new or malloc failed.
  PNullPointerReference,    // A reference was made through a NULL pointer.
  PInvalidCast,             // An invalid cast to descendant is required.
  PInvalidArrayIndex,       // An index into an array was negative.
  PInvalidArrayElement,     // A NULL array element object was accessed.
  PStackEmpty,              // A Pop() was made of a stack with no elements.
  PUnimplementedFunction,   // Funtion is not implemented.
  PInvalidParameter,        // Invalid parameter was passed to a function.
  POperatingSystemError,    // Error was returned by Operating System.
  PChannelNotOpen,          // Operation attempted when channel not open.
  PUnsupportedFeature,      // Feature is not supported.
  PInvalidWindow,           // Access through invalid window.
  PMaxStandardAssertMessage
};

#define __CLASS__ NULL

void PAssertFunc(const char * file, int line, const char * className, PStandardAssertMessage msg);
void PAssertFunc(const char * file, int line, const char * className, const char * msg);
void PAssertFunc(const char * full_msg);

inline bool PAssertFuncInline(bool b, const char * file, int line, const char * className, PStandardAssertMessage msg)
{
  if (!b) 
    PAssertFunc(file, line, className, msg);
  return b;
}
inline bool PAssertFuncInline(bool b, const char * file, int line, const char * className, const char * msg)
{
  if (!b) 
    PAssertFunc(file, line, className, msg);
  return b;
}

/** This macro is used to assert that a condition must be TRUE.
If the condition is FALSE then an assert function is called with the source
file and line number the macro was instantiated on, plus the message described
by the #msg# parameter. This parameter may be either a standard value
from the #PStandardAssertMessage# enum or a literal string.
*/
#define PAssert(b, m) PAssertFuncInline((b), __FILE__,__LINE__,__CLASS__,(m))

/** This macro is used to assert that a condition must be TRUE.
If the condition is FALSE then an assert function is called with the source
file and line number the macro was instantiated on, plus the message described
by the #msg# parameter. This parameter may be either a standard value
from the #PStandardAssertMessage# enum or a literal string.
The #c# parameter specifies the class name that the error occurred in
*/
#define PAssert2(b, c, m) PAssertFuncInline((b), __FILE__,__LINE__,(c),(m))

/** This macro is used to assert that an operating system call succeeds.
If the condition is FALSE then an assert function is called with the source
file and line number the macro was instantiated on, plus the message
described by the #POperatingSystemError# value in the #PStandardAssertMessage#
enum.
 */
#define PAssertOS(b) PAssertFuncInline((b), __FILE__,__LINE__,__CLASS__,POperatingSystemError)

/** This macro is used to assert that a pointer must be non-null.
If the pointer is NULL then an assert function is called with the source file
and line number the macro was instantiated on, plus the message described by
the PNullPointerReference value in the #PStandardAssertMessage# enum.

Note that this evaluates the expression defined by #ptr# twice. To
prevent incorrect behaviour with this, the macro will assume that the
#ptr# parameter is an L-Value.
 */
#define PAssertNULL(p) (((p)!=NULL)?(p): \
                     (PAssertFunc(__FILE__,__LINE__, __CLASS__, PNullPointerReference),(p)))

/** This macro is used to assert immediately.
The assert function is called with the source file and line number the macro
was instantiated on, plus the message described by the #msg# parameter. This
parameter may be either a standard value from the #PStandardAssertMessage#
enum or a literal string.
*/
#define PAssertAlways(m) PAssertFunc(__FILE__,__LINE__,__CLASS__,(m))

/** This macro is used to assert immediately.
The assert function is called with the source file and line number the macro
was instantiated on, plus the message described by the #msg# parameter. This
parameter may be either a standard value from the #PStandardAssertMessage#
enum or a literal string.
*/
#define PAssertAlways2(c, m) PAssertFunc(__FILE__,__LINE__,(c),(m))

#endif // P_USE_ASSERTS


/** Get the stream being used for error output.
This stream is used for all trace output using the various trace functions
and macros.
*/
ostream & PGetErrorStream();

/** Set the stream to be used for error output.
This stream is used for all error output using the #PError# macro.
*/
void PSetErrorStream(ostream * strm /** New stream for error output */ );

/** This macro is used to access the platform specific error output stream.
This is to be used in preference to assuming #cerr# is always available. On
Unix platforms this {\bfis} #cerr# but for MS-Windows this is another stream
that uses the OutputDebugString() Windows API function. Note that a MS-DOS or
Windows NT console application would still use #cerr#.

The #PError# stream would normally only be used for debugging information as
a suitable display is not always available in windowed environments.
   
The macro is a wrapper for a global variable #PErrorStream# which is a pointer
to an #ostream#. The variable is initialised to #cerr# for all but MS-Windows
and NT GUI applications. An application could change this pointer to a
#ofstream# variable of #PError# output is wished to be redirected to a file.
*/
#define PError (PGetErrorStream())



///////////////////////////////////////////////////////////////////////////////
// Debug and tracing

#ifndef PTRACING
#define PTRACING 1
#endif

#if PTRACING

/**Class to encapsulate tracing functions.
   This class does not require any instances and is only being used as a
   method of grouping functions together in a name space.
  */
class PTrace
{
public:
  /// Options for trace output.
  enum Options {
    /**Include PTrace::Block constructs in output
       If this is bit is clear, all PTrace::Block output is inhibited
       regardless of the trace level. If set, the PTrace::Block may occur
       provided the trace level is greater than zero.
    */
    Blocks = 1,
    /// Include date and time in all output
    DateAndTime = 2,
    /// Include (millisecond) timestamp in all output
    Timestamp = 4,
    /// Include identifier for thread trace is made from in all output
    Thread = 8,
    /// Include trace level in all output
    TraceLevel = 16,
    /// Include the file and line for the trace call in all output
    FileAndLine = 32,
    /// Include thread object pointer address in all trace output
    ThreadAddress = 64,
    /// Append to log file rather than resetting every time
    AppendToFile = 128,
    /** Output timestamps in GMT time rather than local time
      */
    GMTTime = 256,
    /** If set, log file will be rotated daily
      */
    RotateDaily = 512,
    /** SystemLog flag for tracing within a PServiceProcess application. Must
        be set in conjection with SetStream(new PSystemLog).
      */
    SystemLogStream = 32768
  };

  /**Set the most common trace options.
     If filename is not NULL then a PTextFile is created and attached the
     trace output stream. This object is never closed or deleted until the
     termination of the program.

     A trace output of the program name version and OS is written as well.
    */
  static void Initialise(
    unsigned level,
    const char * filename = NULL,
    unsigned options = Timestamp | Thread | Blocks
  );

  /**Set the most common trace options.
     If filename is not NULL then a PTextFile is created and attached the
     trace output stream. This object is never closed or deleted until the
     termination of the program.

     If rolloverPatterm is not NULL it is used as the time format patterm
     appended to filename if the RotateDaily is set. Default: yyyy_MM_dd

     A trace output of the program name version and OS is written as well.
    */
  static void Initialise(
    unsigned level,
    const char * filename,
    const char * rolloverPattern,
    unsigned options = Timestamp | Thread | Blocks
  );

  /** Set the trace options.
  The PTRACE(), PTRACE_BLOCK() and PTRACE_LINE() macros output trace text that
  may contain assorted values. These are defined by the Options enum.

  Note this function OR's the bits included in the options parameter.
  */
  static void SetOptions(unsigned options /** New level for trace */ );

  /** Clear the trace options.
  The PTRACE(), PTRACE_BLOCK() and PTRACE_LINE() macros output trace text that
  may contain assorted values. These are defined by the Options enum.

  Note this function AND's the complement of the bits included in the options
  parameter.
  */
  static void ClearOptions(unsigned options /** New level for trace */ );

  /** Get the current trace options.
  The PTRACE(), PTRACE_BLOCK() and PTRACE_LINE() macros output trace text that
  may contain assorted values. These are defined by the Options enum.
  */
  static unsigned GetOptions();

  /** Set the trace level.
  The PTRACE() macro checks to see if its level is equal to or lower then the
  level set by this function. If so then the trace text is output to the trace
  stream.
  */
  static void SetLevel(unsigned level /** New level for trace */ );

  /** Get the trace level.
  The PTRACE() macro checks to see if its level is equal to or lower then the
  level set by this function. If so then the trace text is output to the trace
  stream.
  */
  static unsigned GetLevel();

  /** Determine if the level may cause trace output.
  This checks against the current global trace level set by #PSetTraceLevel#
  for if the trace output may be emitted. This is used by the PTRACE macro.
  */
  static BOOL CanTrace(unsigned level /** Trace level to check */);

  /** Set the stream to be used for trace output.
  This stream is used for all trace output using the various trace functions
  and macros.
  */
  static void SetStream(ostream * out /** New output stream from trace. */ );

  /** Begin a trace output.
  If the trace stream output is used outside of the provided macros, it
  should be noted that a mutex is obtained on the call to #PBeginTrace# which
  will prevent any other threads from using the trace stream until the
  #PEndTrace# function is called.

  So a typical usage would be:
  \begin{verbatim}
    ostream & s = PTrace::Begin(3, __FILE__, __LINE__);
    s << "hello";
    if (want_there)
      s << " there";
    s << '!' << PTrace::End();
  \end{verbatim}
  */
  static ostream & Begin(
    unsigned level,         ///< Log level for output
    const char * fileName,  ///< Filename of source file being traced
    int lineNum             ///< Line number of source file being traced.
  );

  /** End a trace output.
  If the trace stream output is used outside of the provided macros, the
  #PEndTrace# function must be used at the end of the section of trace
  output. A mutex is obtained on the call to #PBeginTrace# which will prevent
  any other threads from using the trace stream until the PEndTrace. The
  #PEndTrace# is used in a similar manner to #::endl# or #::flush#.

  So a typical usage would be:
  \begin{verbatim}
    ostream & s = PTrace::Begin();
    s << "hello";
    if (want_there)
      s << " there";
    s << '!' << PTrace::End();
  \end{verbatim}
  */
  static ostream & End(ostream & strm /** Trace output stream being completed */);


  /** Class to trace Execution blocks.
  This class is used for tracing the entry and exit of program blocks. Upon
  construction it outputs an entry trace message and on destruction outputs an
  exit trace message. This is normally only used from in the PTRACE_BLOCK macro.
  */
  class Block {
    public:
      /** Output entry trace message. */
      Block(
        const char * fileName, ///< Filename of source file being traced
        int lineNum,           ///< Line number of source file being traced.
        const char * traceName
          ///< String to be output with trace, typically it is the function name.
       );
      Block(const Block & obj)
        : file(obj.file), line(obj.line), name(obj.name) { }
      /// Output exit trace message.
      ~Block();
    private:
      Block & operator=(const Block &)
      { return *this; }
      const char * file;
      int          line;
      const char * name;
  };
};

/* Macro to conditionally declare a parameter to a function to avoid compiler
   warning due that parameter only being used in a PTRACE */
#define PTRACE_PARAM(param) param

/** Trace an execution block.
This macro creates a trace variable for tracking the entry and exit of program
blocks. It creates an instance of the PTraceBlock class that will output a
trace message at the line PTRACE_BLOCK is called and then on exit from the
scope it is defined in.
*/
#define PTRACE_BLOCK(name) PTrace::Block __trace_block_instance(__FILE__, __LINE__, name)

/** Trace the execution of a line.
This macro outputs a trace of a source file line execution.
*/
#define PTRACE_LINE() \
    if (!PTrace::CanTrace(1)) ; else \
      PTrace::Begin(1, __FILE__, __LINE__) << __FILE__ << '(' << __LINE__ << ')' << PTrace::End

/** Output trace.
This macro outputs a trace of any information needed, using standard stream
output operators. The output is only made if the trace level set by the
#PSetTraceLevel# function is greater than or equal to the #level# argument.
*/
#define PTRACE(level, args) \
    if (!PTrace::CanTrace(level)) ; else \
      PTrace::Begin(level, __FILE__, __LINE__) << args << PTrace::End

/** Output trace on condition.
This macro outputs a trace of any information needed, using standard stream
output operators. The output is only made if the trace level set by the
#PSetTraceLevel# function is greater than or equal to the #level# argument
and the conditional is TRUE. Note the conditional is only evaluated if the
trace level is sufficient.
*/
#define PTRACE_IF(level, cond, args) \
    if (!(PTrace::CanTrace(level)  && (cond))) ; else \
      PTrace::Begin(level, __FILE__, __LINE__) << args << PTrace::End

#else // PTRACING

#define PTRACE_PARAM(param)
#define PTRACE_BLOCK(n)
#define PTRACE_LINE()
#define PTRACE(level, arg)
#define PTRACE_IF(level, cond, args)

#endif // PTRACING



#if PMEMORY_CHECK || (defined(_MSC_VER) && defined(_DEBUG) && !defined(_WIN32_WCE)) 

#define PMEMORY_HEAP 1

/** Memory heap checking class.
This class implements the memory heap checking and validation functions. It
maintains lists of allocated block so that memory leaks can be detected. It
also initialises memory on allocation and deallocation to help catch errors
involving the use of dangling pointers.
*/
class PMemoryHeap {
  public:
    /// Initialise the memory checking subsystem.
    PMemoryHeap();

    // Clear up the memory checking subsystem, dumping memory leaks.
    ~PMemoryHeap();

    /** Allocate a memory block.
       This allocates a new memory block and keeps track of it. The memory
       block is filled with the value in the #allocFillChar# member variable
       to help detect uninitialised structures.
       @return pointer to newly allocated memory block.
     */
    static void * Allocate(
      size_t nSize,           ///< Number of bytes to allocate.
      const char * file,      ///< Source file name for allocating function.
      int line,               ///< Source file line for allocating function.
      const char * className  ///< Class name for allocating function.
    );
    /** Allocate a memory block.
       This allocates a new memory block and keeps track of it. The memory
       block is filled with the value in the #allocFillChar# member variable
       to help detect uninitialised structures.
       @return pointer to newly allocated memory block.
     */
    static void * Allocate(
      size_t count,       ///< Number of items to allocate.
      size_t iSize,       ///< Size in bytes of each item.
      const char * file,  ///< Source file name for allocating function.
      int line            ///< Source file line for allocating function.
    );

    /** Change the size of an allocated memory block.
       This allocates a new memory block and keeps track of it. The memory
       block is filled with the value in the #allocFillChar# member variable
       to help detect uninitialised structures.
      @return pointer to reallocated memory block. Note this may
      {\em not} be the same as the pointer passed into the function.
     */
    static void * Reallocate(
      void * ptr,         ///< Pointer to memory block to reallocate.
      size_t nSize,       ///< New number of bytes to allocate.
      const char * file,  ///< Source file name for allocating function.
      int line            ///< Source file line for allocating function.
    );

    /** Free a memory block.
      The memory is deallocated, a warning is displayed if it was never
      allocated. The block of memory is filled with the value in the
      #freeFillChar# member variable.
     */
    static void Deallocate(
      void * ptr,             ///< Pointer to memory block to deallocate.
      const char * className  ///< Class name for deallocating function.
    );

    /** Validation result.
     */
    enum Validation {
      Ok, Bad, Trashed
    };
    /** Validate the memory pointer.
        The #ptr# parameter is validated as a currently allocated heap
        variable.
        @return Ok for pointer is in heap, Bad for pointer is not in the heap
        or Trashed if the pointer is in the heap but has overwritten the guard
        bytes before or after the actual data part of the memory block.
     */
    static Validation Validate(
      const void * ptr,       ///< Pointer to memory block to check
      const char * className, ///< Class name it should be.
      ostream * error         ///< Stream to receive error message (may be NULL)
    );

    /** Validate all objects in memory.
       This effectively calls Validate() on every object in the heap.
        @return TRUE if every object in heap is Ok.
     */
    static BOOL ValidateHeap(
      ostream * error = NULL  ///< Stream to output, use default if NULL
    );

    /** Ignore/Monitor allocations.
       Set internal flag so that allocations are not included in the memory
       leak check on program termination.
       Returns the previous state.
     */
    static BOOL SetIgnoreAllocations(
      BOOL ignore  ///< New flag for allocation ignoring.
    );

    /** Get memory check system statistics.
        Dump statistics output to the default stream.
     */
    static void DumpStatistics();
    /** Get memory check system statistics.
        Dump statistics output to the specified stream.
     */
    static void DumpStatistics(ostream & strm /** Stream to output to */);

#if PMEMORY_CHECK
    struct State {
      DWORD allocationNumber;
    };
#else
	typedef _CrtMemState State;
#endif

    /* Get memory state.
      This returns a state that may be used to determine where to start dumping
      objects from.
     */
    static void GetState(
      State & state  ///< Memory state
    );

    /** Dump allocated objects.
       Dump ojects allocated and not deallocated since the specified object
       number. This would be a value returned by the #GetAllocationRequest()#
       function.

       Output is to the default stream.
     */
    static void DumpObjectsSince(
      const State & when    ///< Memory state to begin dump from.
    );

    /** Dump allocated objects.
       Dump ojects allocated and not deallocated since the specified object
       number. This would be a value returned by the #GetAllocationRequest()#
       function.
     */
    static void DumpObjectsSince(
      const State & when,   ///< Memory state to begin dump from.
      ostream & strm        ///< Stream to output dump
    );

    /** Set break point allocation number.
      Set the allocation request number to cause an assert. This allows a
      developer to cause a halt in a debugger on a certain allocation allowing
      them to determine memory leaks allocation point.
     */
    static void SetAllocationBreakpoint(
      DWORD point   ///< Allocation number to stop at.
    );

#if PMEMORY_CHECK

  protected:
    void * InternalAllocate(
      size_t nSize,           // Number of bytes to allocate.
      const char * file,      // Source file name for allocating function.
      int line,               // Source file line for allocating function.
      const char * className  // Class name for allocating function.
    );
    Validation InternalValidate(
      const void * ptr,       // Pointer to memory block to check
      const char * className, // Class name it should be.
      ostream * error         // Stream to receive error message (may be NULL)
    );
    void InternalDumpStatistics(ostream & strm);
    void InternalDumpObjectsSince(DWORD objectNumber, ostream & strm);

    class Wrapper {
      public:
        Wrapper();
        ~Wrapper();
        PMemoryHeap * operator->() const { return instance; }
      private:
        PMemoryHeap * instance;
    };
    friend class Wrapper;

    enum Flags {
      NoLeakPrint = 1
    };

#pragma pack(1)
    struct Header {
      enum {
        // Assure that the Header struct is aligned to 8 byte boundary
        NumGuardBytes = 16 - (sizeof(Header *) +
                              sizeof(Header *) +
                              sizeof(const char *) +
                              sizeof(const char *) +
                              sizeof(size_t) +
                              sizeof(DWORD) +
                              sizeof(WORD) +
                              sizeof(BYTE)
#ifdef P_LINUX
                              + sizeof(pthread_t)
#endif
                              )%8
      };

      Header     * prev;
      Header     * next;
      const char * className;
      const char * fileName;
      size_t       size;
      DWORD        request;
      WORD         line;
      BYTE         flags;
#ifdef P_LINUX
      pthread_t    thread;
#endif
      char         guard[NumGuardBytes];

      static char GuardBytes[NumGuardBytes];
    };
#pragma pack()

    BOOL isDestroyed;

    Header * listHead;
    Header * listTail;

    static DWORD allocationBreakpoint;
    DWORD allocationRequest;
    DWORD firstRealObject;
    BYTE  flags;

    char  allocFillChar;
    char  freeFillChar;

    DWORD currentMemoryUsage;
    DWORD peakMemoryUsage;
    DWORD currentObjects;
    DWORD peakObjects;
    DWORD totalObjects;

    ostream * leakDumpStream;

#if defined(_WIN32)
    CRITICAL_SECTION mutex;
#elif defined(P_PTHREADS)
    pthread_mutex_t mutex;
#elif defined(P_VXWORKS)
    void * mutex;
#endif

#else

#define P_CLIENT_BLOCK (_CLIENT_BLOCK|(0x61<<16)) // This identifies a PObject derived class
    _CrtMemState initialState;

#endif // PMEMORY_CHECK
};


/** Allocate memory for the run time library.
This version of free is used for data that is not to be allocated using the
memory check system, ie will be free'ed inside the C run time library.
*/
inline void * runtime_malloc(size_t bytes /** Size of block to allocate */ ) { return malloc(bytes); }

/** Free memory allocated by run time library.
This version of free is used for data that is not allocated using the
memory check system, ie was malloc'ed inside the C run time library.
*/
inline void runtime_free(void * ptr /** Memory block to free */ ) { free(ptr); }


/** Override of system call for memory check system.
This macro is used to allocate memory via the memory check system selected
with the #PMEMORY_CHECK# compile time option. It will include the source file
and line into the memory allocation to allow the PMemoryHeap class to keep
track of the memory block.
*/
#define malloc(s) PMemoryHeap::Allocate(s, __FILE__, __LINE__, NULL)

/** Override of system call for memory check system.
This macro is used to allocate memory via the memory check system selected
with the #PMEMORY_CHECK# compile time option. It will include the source file
and line into the memory allocation to allow the PMemoryHeap class to keep
track of the memory block.
*/
#define calloc(n,s) PMemoryHeap::Allocate(n, s, __FILE__, __LINE__)

/** Override of system call for memory check system.
This macro is used to allocate memory via the memory check system selected
with the #PMEMORY_CHECK# compile time option. It will include the source file
and line into the memory allocation to allow the PMemoryHeap class to keep
track of the memory block.
*/
#define realloc(p,s) PMemoryHeap::Reallocate(p, s, __FILE__, __LINE__)


/** Override of system call for memory check system.
This macro is used to deallocate memory via the memory check system selected
with the #PMEMORY_CHECK# compile time option. It will include the source file
and line into the memory allocation to allow the PMemoryHeap class to keep
track of the memory block.
*/
#define free(p) PMemoryHeap::Deallocate(p, NULL)


/** Override of system call for memory check system.
This macro is used to deallocate memory via the memory check system selected
with the #PMEMORY_CHECK# compile time option. It will include the source file
and line into the memory allocation to allow the PMemoryHeap class to keep
track of the memory block.
*/
#define cfree(p) PMemoryHeap::Deallocate(p, NULL)


/** Macro for overriding system default #new# operator.
This macro is used to allocate memory via the memory check system selected
with the PMEMORY_CHECK compile time option. It will include the source file
and line into the memory allocation to allow the PMemoryHeap class to keep
track of the memory block.

This macro could be used instead of the system #new# operator. Or you can place
the line
\begin{verbatim}
  #define new PNEW
\end{verbatim}
at the begining of the source file, after all declarations that use the
PCLASSINFO macro.
*/
#define PNEW  new (__FILE__, __LINE__)

#if !defined(_MSC_VER) || _MSC_VER<1200
#define PSPECIAL_DELETE_FUNCTION
#else
#define PSPECIAL_DELETE_FUNCTION \
    void operator delete(void * ptr, const char *, int) \
      { PMemoryHeap::Deallocate(ptr, Class()); } \
    void operator delete[](void * ptr, const char *, int) \
      { PMemoryHeap::Deallocate(ptr, Class()); }
#endif

#define PNEW_AND_DELETE_FUNCTIONS \
    void * operator new(size_t nSize, const char * file, int line) \
      { return PMemoryHeap::Allocate(nSize, file, line, Class()); } \
    void * operator new(size_t nSize) \
      { return PMemoryHeap::Allocate(nSize, NULL, 0, Class()); } \
    void operator delete(void * ptr) \
      { PMemoryHeap::Deallocate(ptr, Class()); } \
    void * operator new(size_t, void * placement) \
      { return placement; } \
    void operator delete(void *, void *) \
      { } \
    void * operator new[](size_t nSize, const char * file, int line) \
      { return PMemoryHeap::Allocate(nSize, file, line, Class()); } \
    void * operator new[](size_t nSize) \
      { return PMemoryHeap::Allocate(nSize, NULL, 0, Class()); } \
    void operator delete[](void * ptr) \
      { PMemoryHeap::Deallocate(ptr, Class()); } \
    PSPECIAL_DELETE_FUNCTION


inline void * operator new(size_t nSize, const char * file, int line)
  { return PMemoryHeap::Allocate(nSize, file, line, NULL); }

inline void * operator new[](size_t nSize, const char * file, int line)
  { return PMemoryHeap::Allocate(nSize, file, line, NULL); }

#ifndef __GNUC__
void * operator new(size_t nSize);
void * operator new[](size_t nSize);

void operator delete(void * ptr);
void operator delete[](void * ptr);

#if defined(_MSC_VER) && _MSC_VER>=1200
inline void operator delete(void * ptr, const char *, int)
  { PMemoryHeap::Deallocate(ptr, NULL); }

inline void operator delete[](void * ptr, const char *, int)
  { PMemoryHeap::Deallocate(ptr, NULL); }
#endif
#endif


class PMemoryHeapIgnoreAllocationsForScope {
public:
  PMemoryHeapIgnoreAllocationsForScope() : previousIgnoreAllocations(PMemoryHeap::SetIgnoreAllocations(TRUE)) { }
  ~PMemoryHeapIgnoreAllocationsForScope() { PMemoryHeap::SetIgnoreAllocations(previousIgnoreAllocations); }
private:
  BOOL previousIgnoreAllocations;
};

#define PMEMORY_IGNORE_ALLOCATIONS_FOR_SCOPE PMemoryHeapIgnoreAllocationsForScope instance_PMemoryHeapIgnoreAllocationsForScope

class PMemoryAllocationBreakpoint {
public:
  PMemoryAllocationBreakpoint(DWORD point)
  {
    PMemoryHeap::SetAllocationBreakpoint(point);
  }
};

#define PMEMORY_ALLOCATION_BREAKPOINT(point) PMemoryAllocationBreakpoint PMemoryAllocationBreakpointInstance(point)


#else // PMEMORY_CHECK || (defined(_MSC_VER) && defined(_DEBUG))

#define PMEMORY_HEAP 0

#define PNEW new

#define PNEW_AND_DELETE_FUNCTIONS

#define runtime_malloc(s) malloc(s)
#define runtime_free(p) free(p)

#define PMEMORY_IGNORE_ALLOCATIONS_FOR_SCOPE
#define PMEMORY_ALLOCATION_BREAKPOINT(point)

#endif // PMEMORY_CHECK || (defined(_MSC_VER) && defined(_DEBUG))


/** Declare all the standard PWlib class information.
This macro is used to provide the basic run-time typing capability needed
by the library. All descendent classes from the #PObject# class require
these functions for correct operation. Either use this macro or the
#PDECLARE_CLASS# macro.

The use of the #PDECLARE_CLASS# macro is no longer recommended for reasons
of compatibility with documentation systems.
*/

#define PCLASSINFO(cls, par) \
  public: \
    static inline const char * Class() \
      { return #cls; } \
    virtual BOOL InternalIsDescendant(const char * clsName) const \
      { return strcmp(clsName, cls::Class()) == 0 || par::InternalIsDescendant(clsName); } \
    virtual const char * GetClass(unsigned ancestor = 0) const \
      { return ancestor > 0 ? par::GetClass(ancestor-1) : cls::Class(); } \
    virtual Comparison CompareObjectMemoryDirect(const PObject & obj) const \
      { return (Comparison)memcmp(this, &obj, sizeof(cls)); } \
    PNEW_AND_DELETE_FUNCTIONS


#if P_HAS_TYPEINFO

#define PIsDescendant(ptr, cls)    (dynamic_cast<const cls *>(ptr) != NULL) 
#define PIsDescendantStr(ptr, str) ((ptr)->InternalIsDescendant(str)) 

#define PRemoveConst(cls, ptr)  (const_cast<cls*>(ptr))

#if P_USE_ASSERTS
template<class BaseClass> inline BaseClass * PAssertCast(BaseClass * obj, const char * file, int line) 
  { if (obj == NULL) PAssertFunc(file, line, BaseClass::Class(), PInvalidCast); return obj; }
#define PDownCast(cls, ptr) PAssertCast<cls>(dynamic_cast<cls*>(ptr),__FILE__,__LINE__)
#else
#define PDownCast(cls, ptr) (dynamic_cast<cls*>(ptr))
#endif

#include <typeinfo>

#else // P_HAS_TYPEINFO

#define PIsDescendant(ptr, cls)    ((ptr)->InternalIsDescendant(cls::Class()))
#define PIsDescendantStr(ptr, str) ((ptr)->InternalIsDescendant(str))

#define PRemoveConst(cls, ptr)  ((cls*)(ptr))

#if P_USE_ASSERTS
template<class BaseClass> inline BaseClass * PAssertCast(PObject * obj, const char * file, int line) 
  { if (obj->InternalIsDescendant(BaseClass::Class()) return (BaseClass *)obj; PAssertFunc(file, line, BaseClass::Class(), PInvalidCast); return NULL; }
#define PDownCast(cls, ptr) PAssertCast<cls>((ptr),__FILE__,__LINE__)
#else
#define PDownCast(cls, ptr) ((cls*)(ptr))
#endif

#endif // P_HAS_TYPEINFO


/** Declare a class with PWLib class information.
This macro is used to declare a new class with a single public ancestor. It
starts the class declaration and then uses the #PCLASSINFO# macro to
get all the run-time type functions.

The use of this macro is no longer recommended for reasons of compatibility
with documentation systems.
*/
#define PDECLARE_CLASS(cls, par) class cls : public par { PCLASSINFO(cls, par)
#ifdef DOC_PLUS_PLUS
} Match previous opening brace in doc++
#endif

///////////////////////////////////////////////////////////////////////////////
// The root of all evil ... umm classes

/** Ultimate parent class for all objects in the class library.
This provides functionality provided to all classes, eg run-time types,
default comparison operations, simple stream I/O and serialisation support.
*/
class PObject {

  protected:
    /** Constructor for PObject, make protected so cannot ever create one on
       its own.
     */
    PObject() { }

  public:
    /* Destructor required to get the "virtual". A PObject really has nothing
       to destroy.
     */
    virtual ~PObject() { }

    /**@name Run Time Type functions */
  //@{
    /** Get the name of the class as a C string. This is a static function which
       returns the type of a specific class. 
       
       When comparing class names, always use the #strcmp()#
       function rather than comparing pointers. The pointers are not
       necessarily the same over compilation units depending on the compiler,
       platform etc.

       @return pointer to C string literal.
     */      
    static inline const char * Class()    { return "PObject"; }

    /** Get the current dynamic type of the object instance.

       When comparing class names, always use the #strcmp()#
       function rather than comparing pointers. The pointers are not
       necessarily the same over compilation units depending on the compiler,
       platform etc.

       The #PCLASSINFO# macro declares an override of this function for
       the particular class. The user need not implement it.

       @return pointer to C string literal.
     */
    virtual const char * GetClass(unsigned /*ancestor*/ = 0) const { return Class(); }

    BOOL IsClass(const char * cls) const 
    { return strcmp(cls, GetClass()) == 0; }

    /** Determine if the dynamic type of the current instance is a descendent of
       the specified class. The class name is usually provided by the
       #Class()# static function of the desired class.
    
       The #PCLASSINFO# macro declares an override of this function for
       the particular class. The user need not implement it.

       @return TRUE if object is descended from the class.
     */
    virtual BOOL InternalIsDescendant(
      const char * clsName    // Ancestor class name to compare against.
    ) const
    { return IsClass(clsName); }

  //@}

  /**@name Comparison functions */
  //@{
    /** Result of the comparison operation performed by the #Compare()#
       function.
      */
    enum Comparison {
      LessThan = -1,
      EqualTo = 0,
      GreaterThan = 1
    };

    /** Compare the two objects and return their relative rank. This function is
       usually overridden by descendent classes to yield the ranking according
       to the semantics of the object.
       
       The default function is to use the #CompareObjectMemoryDirect()#
       function to do a byte wise memory comparison of the two objects.

       @return
       #LessThan#, #EqualTo# or #GreaterThan#
       according to the relative rank of the objects.
     */
    virtual Comparison Compare(
      const PObject & obj   // Object to compare against.
    ) const;
    
    /** Determine the byte wise comparison of two objects. This is the default
       comparison operation for objects that do not explicitly override the
       #Compare()# function.
    
       The #PCLASSINFO# macro declares an override of this function for
       the particular class. The user need not implement it.

       @return
       #LessThan#, #EqualTo# or #GreaterThan#
       according to the result #memcpy()# function.
     */
    virtual Comparison CompareObjectMemoryDirect(
      const PObject & obj   // Object to compare against.
    ) const;

    /** Compare the two objects.
    
       @return
       TRUE if objects are equal.
     */
    bool operator==(
      const PObject & obj   // Object to compare against.
    ) const { return Compare(obj) == EqualTo; }

    /** Compare the two objects.
    
       @return
       TRUE if objects are not equal.
     */
    bool operator!=(
      const PObject & obj   // Object to compare against.
    ) const { return Compare(obj) != EqualTo; }

    /** Compare the two objects.
    
       @return
       TRUE if objects are less than.
     */
    bool operator<(
      const PObject & obj   // Object to compare against.
    ) const { return Compare(obj) == LessThan; }

    /** Compare the two objects.
    
       @return
       TRUE if objects are greater than.
     */
    bool operator>(
      const PObject & obj   // Object to compare against.
    ) const { return Compare(obj) == GreaterThan; }

    /** Compare the two objects.
    
       @return
       TRUE if objects are less than or equal.
     */
    bool operator<=(
      const PObject & obj   // Object to compare against.
    ) const { return Compare(obj) != GreaterThan; }

    /** Compare the two objects.
    
       @return
       TRUE if objects are greater than or equal.
     */
    bool operator>=(
      const PObject & obj   // Object to compare against.
    ) const { return Compare(obj) != LessThan; }
  //@}

  /**@name I/O functions */
  //@{
    /** Output the contents of the object to the stream. The exact output is
       dependent on the exact semantics of the descendent class. This is
       primarily used by the standard #operator<<# function.

       The default behaviour is to print the class name.
     */
    virtual void PrintOn(
      ostream &strm   // Stream to print the object into.
    ) const;

    /** Input the contents of the object from the stream. The exact input is
       dependent on the exact semantics of the descendent class. This is
       primarily used by the standard #operator>># function.

       The default behaviour is to do nothing.
     */
    virtual void ReadFrom(
      istream &strm   // Stream to read the objects contents from.
    );


    /** Global function for using the standard << operator on objects descended
       from PObject. This simply calls the objects #PrintOn()# function.
       
       @return the #strm# parameter.
     */
    inline friend ostream & operator<<(
      ostream &strm,       // Stream to print the object into.
      const PObject & obj  // Object to print to the stream.
    ) { obj.PrintOn(strm); return strm; }

    /** Global function for using the standard >> operator on objects descended
       from PObject. This simply calls the objects #ReadFrom()# function.

       @return the #strm# parameter.
     */
    inline friend istream & operator>>(
      istream &strm,   // Stream to read the objects contents from.
      PObject & obj    // Object to read inormation into.
    ) { obj.ReadFrom(strm); return strm; }


  /**@name Miscellaneous functions */
  //@{
    /** Create a copy of the class on the heap. The exact semantics of the
       descendent class determine what is required to make a duplicate of the
       instance. Not all classes can even {\bf do} a clone operation.
       
       The main user of the clone function is the #PDictionary# class as
       it requires copies of the dictionary keys.

       The default behaviour is for this function to assert.

       @return
       pointer to new copy of the class instance.
     */
    virtual PObject * Clone() const;

    /** This function yields a hash value required by the #PDictionary#
       class. A descendent class that is required to be the key of a dictionary
       should override this function. The precise values returned is dependent
       on the semantics of the class. For example, the #PString# class
       overrides it to provide a hash function for distinguishing text strings.

       The default behaviour is to return the value zero.

       @return
       hash function value for class instance.
     */
    virtual PINDEX HashFunction() const;
  //@}
};

///////////////////////////////////////////////////////////////////////////////
// Platform independent types

// All these classes encapsulate primitive types such that they may be
// transfered in a platform independent manner. In particular it is used to
// do byte swapping for little endien and big endien processor architectures
// as well as accommodating structure packing rules for memory structures.

#define PANSI_CHAR 1
#define PLITTLE_ENDIAN 2
#define PBIG_ENDIAN 3


#if 0
class PStandardType
/* Encapsulate a standard 8 bit character into a portable format. This would
   rarely need to do translation, only if the target platform uses EBCDIC
   would it do anything.

   The platform independent form here is always 8 bit ANSI.
 */
{
  public:
    PStandardType(
      type newVal   // Value to initialise data in platform dependent form.
    ) { data = newVal; }
    /* Create a new instance of the platform independent type using platform
       dependent data, or platform independent streams.
     */

    operator type() { return data; }
    /* Get the platform dependent value for the type.

       @return
       data for instance.
     */

    friend ostream & operator<<(ostream & strm, const PStandardType & val)
      { return strm << (type)val; }
    /* Output the platform dependent value for the type to the stream.

       @return
       the stream output was made to.
     */

    friend istream & operator>>(istream & strm, PStandardType & val)
      { type data; strm >> data; val = PStandardType(data); return strm; }
    /* Input the platform dependent value for the type from the stream.

       @return
       the stream input was made from.
     */


  private:
    type data;
};
#endif


#define PI_SAME(name, type) \
  struct name { \
    name() : data(0) { } \
    name(type value) : data(value) { } \
    name(const name & value) : data(value.data) { } \
    name & operator =(type value) { data = value; return *this; } \
    name & operator =(const name & value) { data = value.data; return *this; } \
    operator type() const { return data; } \
    friend ostream & operator<<(ostream & s, const name & v) { return s << v.data; } \
    friend istream & operator>>(istream & s, name & v) { return s >> v.data; } \
    private: type data; \
  }

#define PI_LOOP(src, dst) \
    BYTE *s = ((BYTE *)&src)+sizeof(src); BYTE *d = (BYTE *)&dst; \
    while (s != (BYTE *)&src) *d++ = *--s;

#define PI_DIFF(name, type) \
  struct name { \
    name() : data(0) { } \
    name(type value) : data(0) { operator=(value); } \
    name(const name & value) : data(value.data) { } \
    name & operator =(type value) { PI_LOOP(value, data); return *this; } \
    name & operator =(const name & value) { data = value.data; return *this; } \
    operator type() const { type value; PI_LOOP(data, value); return value; } \
    friend ostream & operator<<(ostream & s, const name & value) { return s << (type)value; } \
    friend istream & operator>>(istream & s, name & v) { type val; s >> val; v = val; return s; } \
    private: type data; \
  }

#ifndef PCHAR8
#define PCHAR8 PANSI_CHAR
#endif

#if PCHAR8==PANSI_CHAR
PI_SAME(PChar8, char);
#endif

PI_SAME(PInt8, signed char);

PI_SAME(PUInt8, unsigned char);

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_SAME(PInt16l, PInt16);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_DIFF(PInt16l, PInt16);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_DIFF(PInt16b, PInt16);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_SAME(PInt16b, PInt16);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_SAME(PUInt16l, WORD);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_DIFF(PUInt16l, WORD);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_DIFF(PUInt16b, WORD);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_SAME(PUInt16b, WORD);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_SAME(PInt32l, PInt32);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_DIFF(PInt32l, PInt32);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_DIFF(PInt32b, PInt32);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_SAME(PInt32b, PInt32);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_SAME(PUInt32l, DWORD);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_DIFF(PUInt32l, DWORD);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_DIFF(PUInt32b, DWORD);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_SAME(PUInt32b, DWORD);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_SAME(PInt64l, PInt64);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_DIFF(PInt64l, PInt64);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_DIFF(PInt64b, PInt64);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_SAME(PInt64b, PInt64);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_SAME(PUInt64l, PUInt64);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_DIFF(PUInt64l, PUInt64);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_DIFF(PUInt64b, PUInt64);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_SAME(PUInt64b, PUInt64);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_SAME(PFloat32l, float);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_DIFF(PFloat32l, float);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_DIFF(PFloat32b, float);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_SAME(PFloat32b, float);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_SAME(PFloat64l, double);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_DIFF(PFloat64l, double);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_DIFF(PFloat64b, double);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_SAME(PFloat64b, double);
#endif

#ifndef NO_LONG_DOUBLE // stupid OSX compiler
#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_SAME(PFloat80l, long double);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_DIFF(PFloat80l, long double);
#endif

#if PBYTE_ORDER==PLITTLE_ENDIAN
PI_DIFF(PFloat80b, long double);
#elif PBYTE_ORDER==PBIG_ENDIAN
PI_SAME(PFloat80b, long double);
#endif
#endif

#undef PI_LOOP
#undef PI_SAME
#undef PI_DIFF


///////////////////////////////////////////////////////////////////////////////
// Miscellaneous

/*$MACRO PARRAYSIZE(array)
   This macro is used to calculate the number of array elements in a static
   array.
 */
#define PARRAYSIZE(array) ((PINDEX)(sizeof(array)/sizeof(array[0])))

/*$MACRO PMIN(v1, v2)
   This macro is used to calculate the minimum of two values. As this is a
   macro the expression in #v1# or #v2# is executed
   twice so extreme care should be made in its use.
 */
#define PMIN(v1, v2) ((v1) < (v2) ? (v1) : (v2))

/*$MACRO PMAX(v1, v2)
   This macro is used to calculate the maximum of two values. As this is a
   macro the expression in #v1# or #v2# is executed
   twice so extreme care should be made in its use.
 */
#define PMAX(v1, v2) ((v1) > (v2) ? (v1) : (v2))

/*$MACRO PABS(val)
   This macro is used to calculate an absolute value. As this is a macro the
   expression in #val# is executed twice so extreme care should be
   made in its use.
 */
#define PABS(v) ((v) < 0 ? -(v) : (v))

#endif // _POBJECT_H

// End Of File ///////////////////////////////////////////////////////////////
