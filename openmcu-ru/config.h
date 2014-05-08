#ifndef _MCU_CONFIG_H
#define _MCU_CONFIG_H

#ifndef PTLIB_VERSION_INT
  #define PTLIB_VERSION_INT(major,minor,build) ((major<<24)|(minor<<16)|build)
#endif
#define PTLIB_VER PTLIB_VERSION_INT(PTLIB_MAJOR, PTLIB_MINOR, PTLIB_BUILD)

#if PTLIB_VER > PTLIB_VERSION_INT(2,0,1)
  #define BOOL PBoolean
#endif

#define MCUSIP_SRTP 1
#define MCUSIP_ZRTP 1


#ifdef _WIN32
#  define PATH_SEPARATOR "\\"
#else
#  define PATH_SEPARATOR "/"
#endif


// specify the path executable files
#define SYS_BIN_DIR "/opt/openmcu-ru/bin"
// specify the path for .conf files
#define SYS_CONFIG_DIR "/opt/openmcu-ru/config"
// specify the path for audio&video export via named pipes (linux, freebsd)
#define SYS_PIPE_DIR "/opt/openmcu-ru/pipe"
// specify the path for built-in web server resources
#define SYS_RESOURCE_DIR "/opt/openmcu-ru/resource"
// specify server logs folder
#define SERVER_LOGS "/opt/openmcu-ru/log"
// specify true font directory
#define TRUETYPE_FONT_DIR "/opt/openmcu-ru/font"
// specify records directory
#define RECORDS_DIR "/opt/openmcu-ru/records"
// specify certificates directory
#define SYS_SSL_DIR "/opt/openmcu-ru/ssl"

// specify ffmpeg path
#define FFMPEG_PATH	"/opt/bin/ffmpeg"

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
#define MCU_VIDEO   1
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
#  undef  USE_SWRESAMPLE
#  define USE_SWRESAMPLE     0
#  undef  USE_LIBSAMPLERATE
#  define USE_LIBSAMPLERATE  0
#  undef  USE_AVRESAMPLE
#  define USE_AVRESAMPLE     0
#  undef  SYS_CONFIG_DIR
#  define SYS_CONFIG_DIR    "etc"
#  undef  SYS_RESOURCE_DIR
#  define SYS_RESOURCE_DIR  "share"
#  undef  SERVER_LOGS
#  define SERVER_LOGS       "log"
#  undef  TRUETYPE_FONT_DIR
#  define TRUETYPE_FONT_DIR "font"
#  undef  RECORDS_DIR
#  define RECORDS_DIR       "records"
#  undef  SYS_SSL_DIR
#  define SYS_SSL_DIR       "ssl"
#endif

#ifndef SYS_CONFIG_DIR
#  define SYS_CONFIG_DIR "."
#endif
#define CONFIG_PATH PString(SYS_CONFIG_DIR)+PATH_SEPARATOR+"openmcu.ini"


#endif // _MCU_CONFIG_H

