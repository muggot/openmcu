/*
 * Common ffmpeg code for OPAL
 *
 * This code is based on the following files from the OPAL project which
 * have been removed from the current build and distributions but are still
 * available in the CVS "attic"
 * 
 *    src/codecs/h263codec.cxx 
 *    include/codecs/h263codec.h 

 * The original files, and this version of the original code, are released under the same 
 * MPL 1.0 license. Substantial portions of the original code were contributed
 * by Salyens and March Networks and their right to be identified as copyright holders
 * of the original code portions and any parts now included in this new copy is asserted through 
 * their inclusion in the copyright notices below.
 *
 * Copyright (C) 2006 Post Increment
 * Copyright (C) 2005 Salyens
 * Copyright (C) 2001 March Networks Corporation
 * Copyright (C) 1999-2000 Equivalence Pty. Ltd.
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): Guilhem Tardy (gtardy@salyens.com)
 *                 Craig Southeren (craigs@postincrement.com)
 *                 Matthias Schneider (ma30002000@yahoo.de)
 */

#ifndef __FFMPEG_H__
#define __FFMPEG_H__ 1

#include "opalplugin.h"

#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN32_WCE)
  #include <windows.h>
  #include <malloc.h>
  #define STRCMPI  _strcmpi
#else
  #include <semaphore.h>
  #include <dlfcn.h>
  #define STRCMPI  strcasecmp
  typedef unsigned char BYTE;
#endif

#include <string.h>
#include "critsect.h"
#include "trace.h"


extern "C" {
#if defined (_WIN32) && defined (_MSC_VER)
#include "vs-stdint.h"
#define LIBAVCODEC_HEADER   "libavcodec\avcodec.h"
#pragma warning(push)
#pragma warning(disable:4244 4996)
#define round(d)  ((int)((double)(d)+0.5))
#define strdup(s) _strdup(s)
#else
#include "plugin-config.h"
#include <stdint.h>
#endif

#include LIBAVCODEC_HEADER
#include "libavutil/mem.h"

#ifdef _WIN32
#pragma warning(pop)
#endif
};

#ifndef LIBAVCODEC_VERSION_INT
#error Libavcodec include is not correct
#endif

#include <vector>

// Compile time version checking
#if LIBAVCODEC_VERSION_INT < ((51<<16)+(11<<8)+0)
#error Libavcodec LIBAVCODEC_VERSION_INT too old.
#endif


#ifdef LIBAVCODEC_STACKALIGN_HACK
/*
* Some combination of gcc 3.3.5, glibc 2.3.5 and PWLib 1.11.3 is throwing
* off stack alignment for ffmpeg when it is dynamically loaded by PWLib.
* Wrapping all ffmpeg calls in this macro should ensure the stack is aligned
* when it reaches ffmpeg. ffmpeg still needs to be compiled with a more
* recent gcc (we used 4.1.1) to ensure it preserves stack boundaries
* internally.
*
* This macro comes from FFTW 3.1.2, kernel/ifft.h. See:
*     http://www.fftw.org/fftw3_doc/Stack-alignment-on-x86.html
* Used with permission.
*/
#define WITH_ALIGNED_STACK(what)                               \
{                                                              \
    (void)__builtin_alloca(16);                                \
     __asm__ __volatile__ ("andl $-16, %esp");                 \
							       \
    what						       \
}
#else
#define WITH_ALIGNED_STACK(what) what
#endif

#ifdef  _WIN32
# define DIR_SEPARATOR "\\"
# define DIR_TOKENISER ";"
#else
# define DIR_SEPARATOR "/"
# define DIR_TOKENISER ":"
#endif

#endif // __FFMPEG_H__
