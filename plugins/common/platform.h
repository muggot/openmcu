/*
 * Common platform code for OPAL
 *
 * The original files, and this version of the original code, are released under the same 
 * MPL 1.0 license. Substantial portions of the original code were contributed
 * by Salyens and March Networks and their right to be identified as copyright holders
 * of the original code portions and any parts now included in this new copy is asserted through 
 * their inclusion in the copyright notices below.
 *
 * Copyright (C) 2011 Vox Lucida Pty. Ltd.
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
 *                 Robert Jongbloed (robertj@voxlucida.com.au)
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__ 1

#define __STDC_CONSTANT_MACROS 1

#include <stdint.h>

#if defined (_MSC_VER)
  #define _CRT_NONSTDC_NO_DEPRECATE 1
  #define _CRT_SECURE_NO_WARNINGS 1

  #include "stdint.h"

  #include <windows.h>
  #undef min
  #undef max

  #include <malloc.h>
  #include <stdio.h>

  #define round(d)  ((int)((double)(d)+0.5))
  #define strdup(s) _strdup(s)
  #define STRCMPI  _strcmpi
  #define snprintf  _snprintf
  #define vsnprintf _vsnprintf

  #pragma warning(disable:4101 4244 4996)
  #pragma pack(16)
#elif defined(_WIN32)
  #include "stdint.h"
#else
  #include "plugin_config.h"

  #if defined HAVE_STDINT_H
    #include <stdint.h>
  #elif defined HAVE_INTTYPES_H
    #include <inttypes.h>
  #endif

  #include <semaphore.h>
  #include <dlfcn.h>

  #define STRCMPI  strcasecmp
  typedef uint8_t BYTE;

#endif

#ifdef  _WIN32
# define DIR_SEPARATOR "\\"
# define DIR_TOKENISER ";"
#else
# define DIR_SEPARATOR "/"
# define DIR_TOKENISER ":"
#endif


#endif // __PLATFORM_H__
