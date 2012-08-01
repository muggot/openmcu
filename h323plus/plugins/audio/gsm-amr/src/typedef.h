/*
 * ===================================================================
 *  TS 26.104
 *  REL-5 V5.4.0 2004-03
 *  REL-6 V6.1.0 2004-03
 *  3GPP AMR Floating-point Speech Codec
 * ===================================================================
 *
 */

/* This is valid for PC */

#ifndef _TYPEDEF_H
#define _TYPEDEF_H

// By Default we use VAD2 for Silence supression
#define VAD2 1

typedef char Word8;
typedef unsigned char UWord8;
typedef float Float32;
typedef double Float64;
#if defined(__unix__) | defined(_VXWORKS) | defined(_RTEMS) | defined(_PSOS)
    // identifier structure modified to fix different platform compilation
typedef short Word16;
typedef int   Word32;
#else
typedef short Word16;
typedef long Word32;
#endif

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#endif // !_TYPEDEF_H
