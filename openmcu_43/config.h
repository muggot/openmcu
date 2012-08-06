
#ifndef _OpenMCU_CONFIG_H
#define _OpenMCU_CONFIG_H

// enable test rooms for video mixer
#define ENABLE_TEST_ROOMS   1

// enable echo room for video mixer
#define ENABLE_ECHO_MIXER   1

// enable video if OpenH323 has been compiled with video support
#include <h323.h>
#ifndef NO_H323_VIDEO
#define OPENMCU_VIDEO   1
#endif

// enable freetype2, libjpeg, libyuv for video
#if OPENMCU_VIDEO
#define USE_FREETYPE	1
#define USE_LIBJPEG	1
#define USE_LIBYUV	0
//#define LIBYUV_FILTER	libyuv::kFilterNone
//#define LIBYUV_FILTER	libyuv::kFilterBilinear
#define LIBYUV_FILTER	libyuv::kFilterBox
#endif

#undef P_SSL

#endif // _OpenMCU_CONFIG_H

