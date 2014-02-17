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
 * Revision 1.1  2000/06/26 11:17:19  robertj
 * Nucleus++ port (incomplete).
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

#ifndef _PCONTAIN
#define _PCONTAIN

// Wos ere.  Moved so that types are defined properly in the right order.
//#include "pmachdep.h"

#ifdef __NUCLEUS_MNT__
#pragma message ("<unistd.h> not included")
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_     /* _WINSOCKAPI_ must be defined to keep the      */
#endif                   /* winsock.h file from being included.  If it is */
                         /* included some of the typedefs will conflict   */
                         /* with those in Nucleus NET.                    */

#pragma message ("Include VNET stuff")
extern "C"
  {
#include "net\inc\externs.h"  // Nucleus (V)NET unofficial
#include "control.h"  // Nucleus (V)NET official
#include "net\inc\Sockext.h"
  }

// This really is for VNET only, because they've used INT16 in NET and not
// even bothered to define it in NET.
//typedef int16 INT16;

#else
#include <unistd.h>
#endif
#include <ctype.h>


///////////////////////////////////////////
//
//  define TRUE and FALSE for environments that don't have them
//

#ifndef	TRUE
#define	TRUE		1
#define	FALSE		0
#endif

///////////////////////////////////////////
//
//  define a macro for declaring classes so we can bolt
//  extra things to class declarations
//

#define	PEXPORT
#define	PSTATIC


///////////////////////////////////////////
//
// define some basic types and their limits
//

#ifdef __NUCLEUS_MNT__
typedef int	           BOOL;
#endif
typedef unsigned char	   BYTE;	// 1 byte

typedef	signed short	   PInt16;	// 16 bit
typedef unsigned short	   WORD;

typedef	signed int         PInt32;	// 32 bit
#ifndef __NUCLEUS_MNT__
typedef unsigned int 	   DWORD;
#else // __NUCLEUS_MNT__
typedef unsigned long 	   DWORD;
typedef long int 	   int32;
#if 0 // Shouldn't be necessary now we're VNET 4.2
typedef int16 INT16;
typedef uint16 UINT16;
#endif
#endif

#define P_HAS_INT64

#ifdef _MSC_VER  // MS compiler should set MSC_VER
typedef signed __int64 PInt64;
typedef unsigned __int64 PUInt64;

#include <iostream>
//class ostream;
//class istream;

ostream & operator<<(ostream & s, PInt64 v);
ostream & operator<<(ostream & s, PUInt64 v);

istream & operator>>(istream & s, PInt64 & v);
istream & operator>>(istream & s, PUInt64 & v);
#endif           // Diab compiler uses long long for 64-bit
#ifdef __GNUC__
typedef   signed long long int PInt64;
typedef unsigned long long int PUInt64;

#include <istream>
#include <ostream>

ostream & operator<<(ostream & s, PInt64 v);
ostream & operator<<(ostream & s, PUInt64 v);

istream & operator>>(istream & s, PInt64 & v);
istream & operator>>(istream & s, PUInt64 & v);
#endif
#ifdef __DIAB
typedef   signed long long int PInt64;
typedef unsigned long long int PUInt64;	// 64 bit
#endif

// Integer type that is same size as a pointer type.
#ifdef P_64BIT
typedef long INT;
#else
typedef int  INT;
#endif

// MSVC problem requires a PINDEX to be #defined (specifically) to an int
// (specifically)
#ifdef __NUCLEUS_MNT__
#define PINDEX int
#else
typedef size_t PINDEX;
#endif

#define P_MAX_INDEX 		0x7fffffff
#define PABSINDEX(idx) 		(idx)		// careful - size_t may be signed!
#define PASSERTINDEX(idx)

#include "pmachdep.h"


///////////////////////////////////////////
//
//  include common declarations
//

#include "../../contain.h"

#endif
