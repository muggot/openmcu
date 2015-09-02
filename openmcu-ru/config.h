#ifndef _MCU_CONFIG_H
#define _MCU_CONFIG_H

////////////////////////////////////////////////////////////////////////////////////////////////////
// PTLib

#define MCU_PTLIB_VERSION_INT(major,minor,build) ((major<<24)|(minor<<16)|build)
#define MCU_PTLIB_VERSION MCU_PTLIB_VERSION_INT(PTLIB_MAJOR, PTLIB_MINOR, PTLIB_BUILD)
#if MCU_PTLIB_VERSION > MCU_PTLIB_VERSION_INT(2,0,1)
  #define BOOL PBoolean
#endif

#define MCU_PLUGIN_DIR "/opt/openmcu-ru/lib/ptlib"

////////////////////////////////////////////////////////////////////////////////////////////////////
// H323Plus

#define MCU_OPENH323_VERSION_INT(major,minor,build) ((major<<24)|(minor<<16)|build)
#define MCU_OPENH323_VERSION MCU_OPENH323_VERSION_INT(OPENH323_MAJOR, OPENH323_MINOR, OPENH323_BUILD)

////////////////////////////////////////////////////////////////////////////////////////////////////

#define MCUSIP_SRTP 1
#define MCUSIP_ZRTP 0

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#  define PATH_SEPARATOR "\\"
#else
#  define PATH_SEPARATOR "/"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

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

// defalt values
#ifndef MCU_MAX_FRAME_RATE
  #define MCU_MAX_FRAME_RATE 999
#endif
#ifndef MCU_MIN_BIT_RATE
  #define MCU_MIN_BIT_RATE 64000
#endif
#ifndef MCU_MAX_BIT_RATE
  #define MCU_MAX_BIT_RATE 25000000
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
#define USE_LIBJPEG	0

// enable/disable libyuv (optimized frame resize)
#define USE_LIBYUV	1

// enable/disable ffmpeg scale
#define USE_SWSCALE	1

// SCALE_FILTER
// 1  libyuv|kFilterNone
// 2  libyuv|kFilterBilinear
// 3  libyuv|kFilterBox
// 4  swscale|SWS_FAST_BILINEAR
// 5  swscale|SWS_BILINEAR
// 6  swscale|SWS_BICUBIC
// 7  swscale|SWS_X
// 8  swscale|SWS_POINT
// 9  swscale|SWS_AREA
// 10 swscale|SWS_BICUBLIN
// 11 swscale|SWS_GAUSS
// 12 swscale|SWS_SINC
// 13 swscale|SWS_LANCZOS
// 14 swscale|SWS_SPLINE
#if USE_LIBYUV
  #define LIBYUV_FILTER kFilterBilinear
#elif USE_SWSCALE
  #define SCALE_FILTER 5
#else
  #define SCALE_FILTER -1
#endif


#ifdef _WIN32
#  undef  USE_SWRESAMPLE
#  define USE_SWRESAMPLE     1
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
#  undef  MCU_PLUGIN_DIR
#  define MCU_PLUGIN_DIR    "."
#endif

#define CONFIG_PATH PString(SYS_CONFIG_DIR)+PATH_SEPARATOR+"openmcu.ini"


#endif // _MCU_CONFIG_H

