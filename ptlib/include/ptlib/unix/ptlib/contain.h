/*
 * contain.h
 *
 * Low level object and container definitions.
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
 * Revision 1.30  2007/04/25 09:21:20  csoutheren
 * Move unixODBC includes to a seperate namespace to avoid namespace conflicts
 *
 * Revision 1.28  2006/03/20 00:24:56  csoutheren
 * Applied patch #1446482
 * Thanks to Adam Butcher
 *
 * Revision 1.27  2005/11/30 12:47:38  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.26  2005/03/10 06:37:21  csoutheren
 * Removed use of typeid on WIndows to get class name because it is not threadsafe
 * In fact, lets just use #classname everywhere because that will always work
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.25  2004/05/12 04:36:18  csoutheren
 * Fixed problems with using sem_wait and friends on systems that do not
 * support atomic integers
 *
 * Revision 1.24  2004/04/11 13:34:51  csoutheren
 * Sigh. gcc needs a namespace directive in order to compile correctly :(
 *
 * Revision 1.23  2004/04/11 06:15:35  csoutheren
 * Modified to use Atomic_word if available
 *
 * Revision 1.22  2004/04/11 03:20:42  csoutheren
 * Added Unix implementation of PCriticalSection
 *
 * Revision 1.21  2004/04/09 11:54:47  csoutheren
 * Added configure.in check for STL streams, and tested with gcc 2.95.3,
 * gcc 3.3.1, and gcc 3.3.3
 *
 * Revision 1.20  2004/04/09 00:42:59  csoutheren
 * Changed Unix build to use slightly different method for
 * keep class names, as GCC does not use actual class names for typeinfo
 *
 * Revision 1.19  2004/04/03 08:57:41  csoutheren
 * Replaced pseudo-RTTI with real RTTI
 *
 * Revision 1.18  2004/04/03 07:14:51  csoutheren
 * Remove no-rtti compiler flag
 * Added __USE_STL__ to allow interopability with STL and iostreams
 *
 * Revision 1.17  2003/10/27 03:21:43  csoutheren
 * Added UINT type needed for QoS
 *
 * Revision 1.16  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.15  2002/06/06 09:27:26  robertj
 * Fixed PINDEX macros for being signed no (gcc3 port).
 * iAdded limits.h for Solaris compatibility.
 *
 * Revision 1.14  2002/06/05 12:29:15  craigs
 * Changes for gcc 3.1
 *
 * Revision 1.13  1999/05/01 11:29:19  robertj
 * Alpha linux port changes.
 *
 * Revision 1.12  1999/01/08 01:27:43  robertj
 * Moved include of pmachdep to earlier in inclusions.
 *
 * Revision 1.11  1998/11/30 22:06:35  robertj
 * New directory structure.
 *
 * Revision 1.10  1998/11/03 10:56:33  robertj
 * Removed unused extern for PErrorStream
 *
 * Revision 1.9  1998/09/24 04:11:31  robertj
 * Added open software license.
 *
 */

#include "pmachdep.h"
#include <unistd.h>
#include <ctype.h>
#include <limits.h>


///////////////////////////////////////////
//
//  define TRUE and FALSE for environments that don't have them
//

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

///////////////////////////////////////////
//
//  define a macro for declaring classes so we can bolt
//  extra things to class declarations
//

#define PEXPORT
#define PSTATIC


///////////////////////////////////////////
//
// define some basic types and their limits
//

typedef int                BOOL;
typedef unsigned char      BYTE;    // 1 byte
typedef signed short       PInt16;  // 16 bit
typedef unsigned short     WORD;

typedef signed int         PInt32;  // 32 bit
typedef unsigned int       DWORD;

#ifndef P_NEEDS_INT64
typedef   signed long long int PInt64;
typedef unsigned long long int PUInt64; // 64 bit
#endif

// Integer type that is same size as a pointer type.
#ifdef P_64BIT
typedef long          INT;
typedef unsigned long UINT;
#else
typedef int           INT;
typedef unsigned int  UINT;
#endif

#if defined (__FreeBSD__) && defined (P_64BIT)
  typedef long PINDEX;
# define P_MAX_INDEX LONG_MAX
#else
  typedef int PINDEX;
# define P_MAX_INDEX INT_MAX
#endif

inline PINDEX PABSINDEX(PINDEX idx) { return (idx < 0 ? -idx : idx)&P_MAX_INDEX; }
#define PASSERTINDEX(idx) PAssert((idx) >= 0, PInvalidArrayIndex)

///////////////////////////////////////////
//
// needed for STL
//
#if P_HAS_STL_STREAMS
#define __USE_STL__     1
// both gnu-c++ and stlport define __true_type normally this would be
// fine but until pwlib removes the evil using namespace std below,
// this is included here to ensure the types do not conflict.  Yes you
// get math when you don't want it but its one of the things in
// stlport that sources the native cmath and includes
// the gcc header bits/cpp_type_traits.h which has the conflicting type.
//
// the sooner the using namespace std below is removed the better.
// namespace pollution in headers is plain wrong!
// 
#include <cmath>
#endif

#define P_HAS_TYPEINFO  1

using namespace std;

