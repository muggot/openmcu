
#ifndef _MCU_PRECOMPILE_H
#define _MCU_PRECOMPILE_H

// msvc warnings
#ifdef _WIN32
#pragma warning(disable:4100)
#pragma warning(disable:4786)
#pragma warning(disable:4805) // long == true/false
#endif

// config
#include <ptlib.h>
#include "config.h"
#include "version.h"

// ptlib
#include <ptlib/video.h>
#include <ptlib/vconvert.h>
#include <ptclib/guid.h>
#include <ptlib/videoio.h>
#include <ptlib/sound.h>
#include <ptclib/delaychan.h>
#include <ptlib/sound.h>
#include <ptlib/pluginmgr.h>
#include <ptclib/cypher.h>
#include <ptclib/pstun.h>
#include <opalwavfile.h>
#if P_SSL
  #include <ptclib/shttpsvc.h>
#else
  #include <ptclib/httpsvc.h>
#endif
#ifdef RC_INVOKED
  #include <winver.h>
#endif

// h323plus
#include <h323.h>
#include <h323ep.h>
#include <h323con.h>
#include <h323pdu.h>
#include <h323caps.h>
#include <h323rtp.h>
#include <h245.h>
#include <codecs.h>
#include <channels.h>
#include <h323.h>
#include <gkserver.h>
#include <gkclient.h>
#include <h323pluginmgr.h>
#include <codec/opalplugin.h>

// sofia-sip
#include <sofia-sip/su.h>
#include <sofia-sip/nta.h>
#include <sofia-sip/nta_stateless.h>
#include <sofia-sip/stun_tag.h>
#include <sofia-sip/sip_status.h>
#include <sofia-sip/sip_header.h>
#include <sofia-sip/sip_util.h>
#include <sofia-sip/su_log.h>
#include <sofia-sip/auth_digest.h>
#include <sofia-sip/sofia_features.h>
#include <sofia-sip/nta_tport.h>
#include <sofia-sip/tport.h>
#include <sofia-sip/sdp.h>
#include <sofia-sip/msg_addr.h>
#include <sofia-sip/sres_sip.h>
#include <sofia-sip/su_uniqueid.h>

// ffmpeg
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
#include "libavutil/mem.h"
#include "libavutil/mathematics.h"
#include "libavutil/audioconvert.h"
#if USE_SWSCALE
  #include "libswscale/swscale.h"
#endif
#if USE_SWRESAMPLE
  #include <libswresample/swresample.h>
#elif USE_AVRESAMPLE
  #include <libavresample/avresample.h>
  #include <libavutil/samplefmt.h>
#elif USE_LIBSAMPLERATE
  #include <samplerate.h>
#endif
}

// libyuv
#if USE_LIBYUV
  #include <libyuv/scale.h>
#endif

// libjpeg
#if USE_LIBJPEG
extern "C" {
  #include <jpeglib.h>
};
#endif

// freetype
#if USE_FREETYPE
  #include <ft2build.h>
  #include FT_FREETYPE_H
#endif

// srtp
#if MCUSIP_SRTP
extern "C" {
  #include "srtp.h"
  #include "crypto_kernel.h"
};
#endif

// zrtp
#if MCUSIP_ZRTP
  #include "zrtp.h"
#endif

// sys
#include <math.h>
#include <stdio.h>
#include <deque>
#include <set>
#include <map>
#include <time.h>
#include <numeric>
#include <algorithm>
#include <typeinfo>
#include <sys/types.h>
#ifndef _WIN32
  #include <unistd.h>
#endif
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <sys/socket.h>
#endif
#ifdef __FreeBSD__
  #include <signal.h>
#endif


#endif // _MCU_PRECOMPILE_H
