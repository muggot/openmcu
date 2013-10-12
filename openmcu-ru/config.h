#ifndef _OpenMCU_CONFIG_H
#define _OpenMCU_CONFIG_H

// specify the path executable files
#define SYS_BIN_DIR "/usr/local/bin"
// specify the path for .conf files
#define SYS_CONFIG_DIR "/usr/local/share/openmcu-ru/config"
// specify the path for audio&video export via named pipes (linux, freebsd)
#define SYS_PIPE_DIR "/usr/local/share/openmcu-ru/pipe"
// specify the path for built-in web server resources
#define SYS_RESOURCE_DIR "/usr/local/share/openmcu-ru/resource"
// specify server logs folder
#define SERVER_LOGS "/usr/local/share/openmcu-ru/log"
// specify true font directory
#define TRUETYPE_FONT_DIR "/usr/local/share/openmcu-ru/font"
// specify records directory
#define RECORDS_DIR "/usr/local/share/openmcu-ru/records"

// specify ffmpeg path
#define FFMPEG_PATH	"/usr/bin/ffmpeg"

// enable test rooms for video mixer
#define ENABLE_TEST_ROOMS   1

// enable echo room for video mixer
#define ENABLE_ECHO_MIXER   1

// default log/trace level
#define DEFAULT_LOG_LEVEL   0
#define DEFAULT_TRACE_LEVEL 0

// enable video if OpenH323 has been compiled with video support
#include <h323.h>
#ifndef NO_H323_VIDEO
#define OPENMCU_VIDEO   1
#endif

// maximum Video frame rate (for outgoing video)
#ifndef MAX_FRAME_RATE
#define MAX_FRAME_RATE  999
#endif

// enable/disable swresample usage
#define USE_SWRESAMPLE  1
// enable/disable avresample usage
#define USE_AVRESAMPLE  0
// enable/disable libsamplerate usage
#define USE_LIBSAMPLERATE  0

// enable/disable freetype2 (rendering names)
#define USE_FREETYPE	1

// enable/disable libjpeg (live video frames in Room Control Page)
#define USE_LIBJPEG	1

// enable/disable libyuv (optimized frame resize)
#define USE_LIBYUV	1

// libyuv filtering type: kFilterNone|kFilterBilinear|kFilterBox
#define LIBYUV_FILTER	kFilterBilinear

#ifdef _WIN32
#  ifdef SYS_CONFIG_DIR
#    define CONFIG_PATH PString(SYS_CONFIG_DIR)+"\\openmcu.ini"
#  else
#    define CONFIG_PATH ".\\openmcu.ini"
#  endif
#else
#  ifdef SYS_CONFIG_DIR
#    define CONFIG_PATH PString(SYS_CONFIG_DIR)+"/openmcu.ini"
#  else
#    define CONFIG_PATH "./openmcu.ini"
#  endif
#endif


#undef P_SSL

#ifdef _WIN32
#  undef  USE_SWRESAMPLE
#  define USE_SWRESAMPLE     0
#  undef  USE_LIBSAMPLERATE
#  define USE_LIBSAMPLERATE  0
#  undef  USE_AVRESAMPLE
#  define USE_AVRESAMPLE     0
#  undef  SYS_BIN_DIR
#  define SYS_BIN_DIR       "."
#  undef  SYS_CONFIG_DIR
#  define SYS_CONFIG_DIR    "."
#  undef  CONFIG_PATH
#  define CONFIG_PATH       "openmcu.ini"
#  undef  SYS_PIPE_DIR
#  define SYS_PIPE_DIR      "."
#  undef  SYS_RESOURCE_DIR
#  define SYS_RESOURCE_DIR  "."
#  undef  SERVER_LOGS
#  define SERVER_LOGS       "."
#  undef  TRUETYPE_FONT_DIR
#  define TRUETYPE_FONT_DIR "."
#  undef  RECORDS_DIR
#  define RECORDS_DIR       "records"
#endif

#endif // _OpenMCU-ru_CONFIG_H

