#ifndef _OpenMCU_CONFIG_H
#define _OpenMCU_CONFIG_H

// specify the path for .conf files
//#define SYS_CONFIG_DIR "/etc/openmcu"
#define SYS_CONFIG_DIR "."
// specify the path for audio&video export via named pipes (linux, freebsd)
//#define SYS_PIPE_DIR "/var/spool/openmcu"
#define SYS_PIPE_DIR "."
// specify the path for built-in web server resources
//#define SYS_RESOURCE_DIR "/usr/share/openmcu"
#define SYS_RESOURCE_DIR "."
// specify server logs folder
//#define SERVER_LOGS "/var/log"
#define SERVER_LOGS "."
// specify true font directory
//#define TRUETYPE_FONT_DIR "/usr/share/fonts/truetype"
#define TRUETYPE_FONT_DIR "."

// enable test rooms for video mixer
#define ENABLE_TEST_ROOMS   1

// enable echo room for video mixer
#define ENABLE_ECHO_MIXER   1

// enable video if OpenH323 has been compiled with video support
#include <h323.h>
#ifndef NO_H323_VIDEO
#define OPENMCU_VIDEO   1
#endif

// enable/disable swresample usage (resample 8<>16 KHz)
#define USE_SWRESAMPLE  1

// enable/disable avresample usage
#define USE_AVRESAMPLE  0

// enable/disable freetype2 (rendering names)
#define USE_FREETYPE	1

// enable/disable libjpeg (live video frames in Room Control Page)
#define USE_LIBJPEG	1

// enable/disable libyuv (optimized frame resize)
#define USE_LIBYUV	1

// libyuv filtering type: kFilterNone|kFilterBilinear|kFilterBox
#define LIBYUV_FILTER	kFilterBilinear


#undef P_SSL

#ifdef _WIN32
#undef USE_SWRESAMPLE
#define USE_SWRESAMPLE 0
#endif

#endif // _OpenMCU_CONFIG_H

