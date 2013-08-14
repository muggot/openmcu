#ifndef _OpenMCU_CONFIG_H
#define _OpenMCU_CONFIG_H

// specify the path executable files
//#define SYS_BIN_DIR "/opt/openmcu-ru/bin"
#define SYS_BIN_DIR "/usr/local/share/openmcu-ru/bin"
// specify the path for .conf files
//#define SYS_CONFIG_DIR "/opt/openmcu-ru/config"
#define SYS_CONFIG_DIR "/usr/local/share/openmcu-ru/config"
// specify the path for audio&video export via named pipes (linux, freebsd)
//#define SYS_PIPE_DIR "/opt/openmcu-ru/pipe"
#define SYS_PIPE_DIR "/usr/local/share/openmcu-ru/pipe"
// specify the path for built-in web server resources
//#define SYS_RESOURCE_DIR "/opt/openmcu-ru/resource"
#define SYS_RESOURCE_DIR "/usr/local/share/openmcu-ru/resource"
// specify server logs folder
//#define SERVER_LOGS "/opt/openmcu-ru/log"
#define SERVER_LOGS "/usr/local/share/openmcu-ru/log"
// specify true font directory
//#define TRUETYPE_FONT_DIR "/opt/openmcu-ru/font"
#define TRUETYPE_FONT_DIR "/usr/local/share/openmcu-ru/font"
// specify records directory
//#define TRUETYPE_FONT_DIR "/opt/openmcu-ru/records"
#define RECORDS_DIR "/usr/local/share/openmcu-ru/records"

// enable test rooms for video mixer
#define ENABLE_TEST_ROOMS   1

// enable echo room for video mixer
#define ENABLE_ECHO_MIXER   1

// enable video if OpenH323 has been compiled with video support
#include <h323.h>
#ifndef NO_H323_VIDEO
#define OPENMCU_VIDEO   1
#endif

// enable/disable swresample usage
#define USE_SWRESAMPLE  1
// enable/disable avresample usage
#define USE_AVRESAMPLE  0
// enable/disable libsamplerate usage
#define USE_LIBSAMPLERATE  1

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

#endif // _OpenMCU-ru_CONFIG_H

