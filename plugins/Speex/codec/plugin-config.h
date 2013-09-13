/* plugin-config.h.  Generated from plugin-config.h.in by configure.  */
#ifndef _PLUGIN_CONFIG_H_
#define _PLUGIN_CONFIG_H_

/* For the win32 build */
/* #undef PLUGIN_CODEC_DLL_EXPORTS */

/* Directory with libavcodec source code, for MPEG4 rate control correction */
/* #undef LIBAVCODEC_HAVE_SOURCE_DIR */

/* Directory of the libavcodec header files */
#define LIBAVCODEC_HEADER "libavcodec/avcodec.h"

/* Stack alignment hack for libavcodec library */
/* #undef LIBAVCODEC_STACKALIGN_HACK */

/* Assume full capabilities at empty fmtp lines */
#define DEFAULT_TO_FULL_CAPABILITIES 1

/* Statically link x264 to the plugin. Default for win32. */
/* #undef X264_LINK_STATIC */

#endif /* _PLUGIN_CONFIG_H_ */
