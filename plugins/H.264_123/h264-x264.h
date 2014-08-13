/*
 * H.264 Plugin codec for OpenH323/OPAL
 *
 * Copyright (C) Matthias Schneider, All Rights Reserved
 *
 * This code is based on the file h261codec.cxx from the OPAL project released
 * under the MPL 1.0 license which contains the following:
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
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
 * Contributor(s): Matthias Schneider (ma30002000@yahoo.de)
 *                 Michele Piccini (michele@piccini.com)
 *                 Derek Smithies (derek@indranet.co.nz)
 *                 Simon Horne (s.horne@packetizer.com)
 *
 *
 */

/*
  Notes
  -----

 */
#define H323_H264_TEST

#ifndef __H264_X264_H__
#define __H264_X264_H__ 1

#include "plugin-config.h"

#include <stdarg.h>
#include "opalplugin.h"

#ifdef _MSC_VER
 #include "../common/vs-stdint.h"
 #include "../common/critsect.h"
#else
 #include <stdint.h>
 #include "critsect.h"
#endif

//#include "shared/h264frame.h"



extern "C" {
#ifdef _MSC_VER
  #include "libavcodec/avcodec.h"
#else
  #include LIBAVCODEC_HEADER
#endif
  #include "libavutil/mem.h"
  #include "libavutil/base64.h"
};

#define FHD_WIDTH 1600
#define FHD_HEIGHT 1200
#define HD_WIDTH 1280
#define HD_HEIGHT 1024
#define SD_WIDTH 1280
#define SD_HEIGHT 720
#define CIF4_WIDTH 704
#define CIF4_HEIGHT 576
#define CIF_WIDTH 352
#define CIF_HEIGHT 288
#define QCIF_WIDTH 176
#define QCIF_HEIGHT 144
#define SQCIF_WIDTH 128
#define SQCIF_HEIGHT 96
#define IT_QCIF 0
#define IT_CIF 1

#ifndef X264_LINK_STATIC
#include "x264loader_unix.h"
#endif

#include "enc-ctx.h"

typedef unsigned char u_char;

static void logCallbackFFMPEG (void* v, int level, const char* fmt , va_list arg);

class H264EncoderContext 
{
  public:
    H264EncoderContext ();
    ~H264EncoderContext ();

    int EncodeFrames (const u_char * src, unsigned & srcLen, u_char * dst, unsigned & dstLen, unsigned int & flags);

    void SetMaxRTPFrameSize (unsigned size);
    void SetTargetBitrate (unsigned rate, unsigned maxBr, unsigned cpb);
    void SetFrameWidth (unsigned width);
    void SetFrameHeight (unsigned height);
    void SetFrameRate (unsigned rate);
    void SetTSTO (unsigned tsto);
    void SetMaxKeyFramePeriod (const char *keyint);
    void SetProfileLevel (unsigned profile, unsigned constraints, unsigned level);
    void SetQuality (unsigned quality);
    void SetThreads (unsigned threads);
    void ApplyOptions ();
    void Lock ();
    void Unlock ();

  unsigned contextProfile, constraints, contextLevel, width, height, frameTime, maxMbps, maxFs, targetBitrate, cpb, maxBr;

  protected:
    CriticalSection _mutex;
    X264EncoderContext *x264;
};

class H264DecoderContext
{
  public:
    H264DecoderContext();
    ~H264DecoderContext();

    int DecodeFrames(const u_char * src, unsigned & srcLen, u_char * dst, unsigned & dstLen, unsigned int & flags);

    void SetSpropParameter(const char *value);

    void Lock() { _mutex.Wait(); }
    void Unlock() { _mutex.Signal(); }

  protected:
    CriticalSection _mutex;

    AVCodec* _codec;
    AVCodecContext* _context;
    AVFrame* _outputFrame;
    AVPacket _pkt;
    H264Frame* _rxH264Frame;

    bool freezeVideo;
    bool _gotIFrame;
    bool _gotAGoodFrame;
    int _frameCounter;
    int _skippedFrameCounter;
    int _lastSQN;
    int _lostFrameCounter;
};

static int valid_for_protocol    ( const struct PluginCodec_Definition *, void *, const char *,
                                   void * parm, unsigned * parmLen);
static int get_codec_options     ( const struct PluginCodec_Definition * codec, void *, const char *, 
                                   void * parm, unsigned * parmLen);
static int free_codec_options    ( const struct PluginCodec_Definition *, void *, const char *, 
                                   void * parm, unsigned * parmLen);

static void * create_encoder     ( const struct PluginCodec_Definition *);
static void destroy_encoder      ( const struct PluginCodec_Definition *, void * _context);
static int codec_encoder         ( const struct PluginCodec_Definition *, void * _context,
                                   const void * from, unsigned * fromLen,
                                   void * to, unsigned * toLen,
                                   unsigned int * flag);
static int to_normalised_options ( const struct PluginCodec_Definition *, void *, const char *,
                                   void * parm, unsigned * parmLen);
static int to_customised_options ( const struct PluginCodec_Definition *, void *, const char *, 
                                   void * parm, unsigned * parmLen);
static int encoder_set_options   ( const struct PluginCodec_Definition *, void * _context, const char *, 
                                   void * parm, unsigned * parmLen);
static int encoder_get_output_data_size ( const PluginCodec_Definition *, void *, const char *,
                                   void *, unsigned *);
static int decoder_set_options   ( const struct PluginCodec_Definition *, void * _context, const char *, 
                                   void * parm, unsigned * parmLen);

static void * create_decoder     ( const struct PluginCodec_Definition *);
static void destroy_decoder      ( const struct PluginCodec_Definition *, void * _context);
static int codec_decoder         ( const struct PluginCodec_Definition *, void * _context, 
                                   const void * from, unsigned * fromLen,
                                   void * to, unsigned * toLen,
                                   unsigned int * flag);
static int decoder_get_output_data_size ( const PluginCodec_Definition * codec, void *, const char *,
                                   void *, unsigned *);

///////////////////////////////////////////////////////////////////////////////
static int merge_profile_level_h264(char ** result, const char * dest, const char * src);
static int merge_packetization_mode(char ** result, const char * dest, const char * src);
static void free_string(char * str);
///////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_information licenseInfo = {
  1143692893,                                                   // timestamp = Thu 30 Mar 2006 04:28:13 AM UTC

  "Matthias Schneider",                                         // source code author
  "1.0",                                                        // source code version
  "ma30002000@yahoo.de",                                        // source code email
  "",                                                           // source code URL
  "Copyright (C) 2006 by Matthias Schneider",                   // source code copyright
  "MPL 1.0",                                                    // source code license
  PluginCodec_License_MPL,                                      // source code license

  "x264 / ffmpeg H.264",                                        // codec description
  "x264: Laurent Aimar, ffmpeg: Michael Niedermayer",           // codec author
  "", 							        // codec version
  "fenrir@via.ecp.fr, ffmpeg-devel-request@mplayerhq.hu",       // codec email
  "http://developers.videolan.org/x264.html, \
   http://ffmpeg.mplayerhq.hu", 				// codec URL
  "x264: Copyright (C) 2003 Laurent Aimar, \
   ffmpeg: Copyright (c) 2002-2003 Michael Niedermayer",        // codec copyright information
  "x264: GNU General Public License as published Version 2, \
   ffmpeg: GNU Lesser General Public License, Version 2.1",     // codec license
  PluginCodec_License_GPL                                       // codec license code
};

static const char YUV420PDesc[]  = { "YUV420P" };

static PluginCodec_ControlDefn EncoderControls[] = {
  { PLUGINCODEC_CONTROL_VALID_FOR_PROTOCOL,    valid_for_protocol },
  { PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS,     get_codec_options },
  { PLUGINCODEC_CONTROL_FREE_CODEC_OPTIONS,    free_codec_options },
  { PLUGINCODEC_CONTROL_TO_NORMALISED_OPTIONS, to_normalised_options },
  { PLUGINCODEC_CONTROL_TO_CUSTOMISED_OPTIONS, to_customised_options },
  { PLUGINCODEC_CONTROL_SET_CODEC_OPTIONS,     encoder_set_options },
  { PLUGINCODEC_CONTROL_GET_OUTPUT_DATA_SIZE,  encoder_get_output_data_size },
  { NULL }
};

static PluginCodec_ControlDefn DecoderControls[] = {
  { PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS,     get_codec_options },
  { PLUGINCODEC_CONTROL_GET_OUTPUT_DATA_SIZE,  decoder_get_output_data_size },
  { PLUGINCODEC_CONTROL_SET_CODEC_OPTIONS,     decoder_set_options },
  { NULL }
};

/////////////////////////////////////////////////////////////////////////////

static const char h264Desc[]      = { "H.264" };
static const char sdpH264[]       = { "h264" };

#define H264_CLOCKRATE        90000
#define H264_BITRATE         768000
#define H264_PAYLOAD_SIZE      1400

#ifdef H323_H264_TEST
///////////////////////////////////////////////////////////////////////////
// H.323 Definitions

static struct PluginCodec_Option const mediaPacketization =
  { PluginCodec_StringOption,  "Media Packetization",  0, PluginCodec_EqualMerge, "0.0.8.241.0.0.0.0" };

static struct PluginCodec_Option const tsto =
  { PluginCodec_IntegerOption,  PLUGINCODEC_OPTION_TEMPORAL_SPATIAL_TRADE_OFF,  0, PluginCodec_AlwaysMerge, "31" };

static struct PluginCodec_Option const encodingThreads =
  { PluginCodec_IntegerOption,  "Encoding Threads", 0, PluginCodec_AlwaysMerge, "0" };

static struct PluginCodec_Option const encodingQuality =
  { PluginCodec_IntegerOption,  "Encoding Quality",  0, PluginCodec_AlwaysMerge, "31" };

static struct PluginCodec_Option const profileLevelId =
  { PluginCodec_StringOption, "profile-level-id", 0, PluginCodec_EqualMerge, "" };

static struct PluginCodec_Option const spropParameterSets =
  { PluginCodec_StringOption, "sprop-parameter-sets", 0, PluginCodec_EqualMerge, "" };

static struct PluginCodec_Option const customResolution =
  { PluginCodec_IntegerOption, "Custom Resolution", 0, PluginCodec_NoMerge, 0 };

static struct PluginCodec_Option const txkeyFramePeriod =
  { PluginCodec_IntegerOption,   PLUGINCODEC_OPTION_TX_KEY_FRAME_PERIOD, 0, PluginCodec_EqualMerge, "125" };


// MEGA MACRO to set options
#define DECLARE_GENERIC_OPTIONS(prefix) \
static struct PluginCodec_Option const * const prefix##_OptionTable[] = { \
  &mediaPacketization, \
  &tsto, \
  &encodingThreads, \
  &encodingQuality, \
  &profileLevelId, \
  &spropParameterSets, \
  &customResolution, \
  &txkeyFramePeriod, \
  NULL \
}; \
static const struct PluginCodec_H323GenericParameterDefinition prefix##_h323params[] = \
{ \
	{{1},41, PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_BooleanArray,{prefix##_Profile}}, \
	{{1},42, PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsignedMin,{prefix##_Level}}, \
	{{1},3, PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsignedMin,{0}}, \
	{{1},4, PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsignedMin,{0}}, \
	{{1},5, PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsignedMin,{0}}, \
	{{1},6, PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsignedMin,{0}}, \
	0 \
}; \
static struct PluginCodec_H323GenericCodecData prefix##_h323GenericData[] = { \
	OpalPluginCodec_Identifer_H264_Generic, \
	prefix##_MaxBitRate, \
	6, \
	prefix##_h323params \
}; \


#define DECLARE_H323PARAM(prefix) \
{ \
  /* encoder */ \
  PLUGIN_CODEC_VERSION_OPTIONS,	      /* codec API version */ \
  &licenseInfo,                       /* license information */ \
  prefix##_VideoType |                /* video type */ \
  PluginCodec_RTPTypeShared |         /* specified RTP type */ \
  PluginCodec_RTPTypeDynamic,         /* specified RTP type */ \
  prefix##_Desc,                      /* text decription */ \
  YUV420PDesc,                        /* source format */ \
  prefix##_MediaFmt,                  /* destination format */ \
  prefix##_OptionTable,			      /* user data */ \
  H264_CLOCKRATE,                     /* samples per second */ \
  H264_BITRATE,				          /* raw bits per second */ \
  20000,                              /* nanoseconds per frame */ \
  prefix##_FrameWidth,			      /* bytes per frame */ \
  prefix##_FrameHeight,               /* samples per frame */ \
  10,                                 /* recommended number of frames per packet */ \
  60,                                 /* maximum number of frames per packet  */ \
  0,                                  /* IANA RTP payload code */ \
  sdpH264,                               /* RTP payload name */ \
  create_encoder,                     /* create codec function */ \
  destroy_encoder,                    /* destroy codec */ \
  codec_encoder,                      /* encode/decode */ \
  EncoderControls,                    /* codec controls */ \
  PluginCodec_H323Codec_generic,      /* h323CapabilityType */ \
  (struct PluginCodec_H323GenericCodecData *)&prefix##_h323GenericData /* h323CapabilityData */ \
}, \
{  \
  /* decoder */ \
  PLUGIN_CODEC_VERSION_OPTIONS,	      /* codec API version */ \
  &licenseInfo,                       /* license information */ \
  PluginCodec_MediaTypeVideo |        /* audio codec */ \
  PluginCodec_RTPTypeShared |         /* specified RTP type */ \
  PluginCodec_RTPTypeDynamic,         /* specified RTP type */ \
  prefix##_Desc,                      /* text decription */ \
  prefix##_MediaFmt,                  /* source format */ \
  YUV420PDesc,                        /* destination format */ \
  prefix##_OptionTable,			      /* user data */ \
  H264_CLOCKRATE,                     /* samples per second */ \
  H264_BITRATE,				          /* raw bits per second */ \
  20000,                              /* nanoseconds per frame */ \
  prefix##_FrameWidth,			      /* bytes per frame */ \
  prefix##_FrameHeight,               /* samples per frame */ \
  10,                                 /* recommended number of frames per packet */ \
  60,                                 /* maximum number of frames per packet  */ \
  0,                                  /* IANA RTP payload code */ \
  sdpH264,                               /* RTP payload name */ \
  create_decoder,                     /* create codec function */ \
  destroy_decoder,                    /* destroy codec */ \
  codec_decoder,                      /* encode/decode */ \
  DecoderControls,                  /* codec controls */ \
  PluginCodec_H323Codec_generic,      /* h323CapabilityType */ \
  (struct PluginCodec_H323GenericCodecData *)&prefix##_h323GenericData /* h323CapabilityData */ \
} \


/////////////////////////////////////////////////////////////////////////////
// Codec Definitions

// SIP 42E015 is 
//  Profile : H264_PROFILE_BASE + H264_PROFILE_MAIN 
//  Level : 2:1  compatible H.323 codec is 4CIF.

#define H264_PROFILE_BASE      64   // only do base and main at this stage
#define H264_PROFILE_MAIN      32
#define H264_PROFILE_EXTENDED  16
#define H264_PROFILE_HIGH       8   

// NOTE: All these values are subject to change Values need to be confirmed!
#define H264_LEVEL1           15      // SQCIF  30 fps
#define H264_LEVEL1_MBPS      640
#define H264_LEVEL1b          19      // QCIF 15 fps
#define H264_LEVEL1b_MBPS     1280
#define H264_LEVEL11          22      //  CIF 15 fps / QCIF 30 fps 
#define H264_LEVEL11_MBPS     1920
#define H264_LEVEL12          29      // 4CIF 15fps / cif 30fps - To Match SIP 42E015
#define H264_LEVEL12_MBPS     3840
#define H264_LEVEL13          36      // 720P 30fps ? Need to confirm
#define H264_LEVEL13_MBPS     7680
#define H264_LEVEL20          43      // SQCIF  30 fps
#define H264_LEVEL20_MBPS     20000
#define H264_LEVEL21          50      // QCIF 15 fps
#define H264_LEVEL21_MBPS     40000
#define H264_LEVEL22          57      //  CIF 15 fps / QCIF 30 fps 
#define H264_LEVEL22_MBPS     40000
#define H264_LEVEL30          64      // 4CIF 15fps / cif 30fps - To Match SIP 42E015
#define H264_LEVEL30_MBPS     100000
#define H264_LEVEL31          71      // 720P 30fps ? Need to confirm
#define H264_LEVEL31_MBPS     140000
#define H264_LEVEL32          78      // 720P 30fps ? Need to confirm
#define H264_LEVEL32_MBPS     200000
#define H264_LEVEL40          85      // 720P 30fps ? Need to confirm
#define H264_LEVEL40_MBPS     200000
#define H264_LEVEL41          92      // 720P 30fps ? Need to confirm
#define H264_LEVEL41_MBPS     500000
#define H264_LEVEL42          99      // 720P 30fps ? Need to confirm
#define H264_LEVEL42_MBPS     500000
#define H264_LEVEL50          106      // 720P 30fps ? Need to confirm
#define H264_LEVEL50_MBPS     1350000
#define H264_LEVEL51          113      // 720P 30fps ? Need to confirm
#define H264_LEVEL51_MBPS     2400000


static const char     H264_Desc[]          = { "H.264" };
static const char     H264_MediaFmt[]      = { "H.264" };
static unsigned int   H264_FrameHeight     = CIF_HEIGHT;
static unsigned int   H264_FrameWidth      = CIF_WIDTH;
static unsigned int   H264_Profile         = H264_PROFILE_BASE;
static unsigned int   H264_Level           = H264_LEVEL12;
static unsigned int   H264_MaxBitRate      = 0; // set this using option "Max Bit Rate"
static unsigned int   H264_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264)

static const char     H264SQCIF_Desc[]          = { "H.264-SQCIF" };
static const char     H264SQCIF_MediaFmt[]      = { "H.264-SQCIF" };                             
static unsigned int   H264SQCIF_FrameHeight     = SQCIF_HEIGHT;               
static unsigned int   H264SQCIF_FrameWidth      = SQCIF_WIDTH; 
static unsigned int   H264SQCIF_Profile         = H264_PROFILE_BASE;
static unsigned int   H264SQCIF_Level           = H264_LEVEL1;
static unsigned int   H264SQCIF_MaxBitRate      = H264_LEVEL1_MBPS;
static unsigned int   H264SQCIF_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264SQCIF)

static const char     H264QCIF_Desc[]          = { "H.264-QCIF1" };
static const char     H264QCIF_MediaFmt[]      = { "H.264-QCIF1" };                             
static unsigned int   H264QCIF_FrameHeight     = QCIF_HEIGHT;               
static unsigned int   H264QCIF_FrameWidth      = QCIF_WIDTH;
static unsigned int   H264QCIF_Profile         = H264_PROFILE_BASE;
static unsigned int   H264QCIF_Level           = H264_LEVEL1b;
static unsigned int   H264QCIF_MaxBitRate      = H264_LEVEL1b_MBPS;
static unsigned int   H264QCIF_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264QCIF)

static const char     H264QCIF2_Desc[]          = { "H.264-QCIF2" };
static const char     H264QCIF2_MediaFmt[]      = { "H.264-QCIF2" };                             
static unsigned int   H264QCIF2_FrameHeight     = QCIF_HEIGHT;               
static unsigned int   H264QCIF2_FrameWidth      = QCIF_WIDTH; 
static unsigned int   H264QCIF2_Profile         = H264_PROFILE_BASE;
static unsigned int   H264QCIF2_Level           = H264_LEVEL11;
static unsigned int   H264QCIF2_MaxBitRate      = H264_LEVEL11_MBPS;
static unsigned int   H264QCIF2_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264QCIF2)

static const char     H264CIF_Desc[]          = { "H.264-CIF1" };
static const char     H264CIF_MediaFmt[]      = { "H.264-CIF1" };                             
static unsigned int   H264CIF_FrameHeight     = CIF_HEIGHT;               
static unsigned int   H264CIF_FrameWidth      = CIF_WIDTH; 
static unsigned int   H264CIF_Profile         = H264_PROFILE_BASE;
static unsigned int   H264CIF_Level           = H264_LEVEL12;
static unsigned int   H264CIF_MaxBitRate      = H264_LEVEL12_MBPS;
static unsigned int   H264CIF_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264CIF)

static const char     H264CIF2_Desc[]          = { "H.264-CIF2" };
static const char     H264CIF2_MediaFmt[]      = { "H.264-CIF2" };                             
static unsigned int   H264CIF2_FrameHeight     = CIF_HEIGHT;               
static unsigned int   H264CIF2_FrameWidth      = CIF_WIDTH; 
static unsigned int   H264CIF2_Profile         = H264_PROFILE_BASE;
static unsigned int   H264CIF2_Level           = H264_LEVEL13;
static unsigned int   H264CIF2_MaxBitRate      = H264_LEVEL13_MBPS;
static unsigned int   H264CIF2_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264CIF2)

static const char     H264CIF3_Desc[]          = { "H.264-CIF3" };
static const char     H264CIF3_MediaFmt[]      = { "H.264-CIF3" };                             
static unsigned int   H264CIF3_FrameHeight     = CIF_HEIGHT;               
static unsigned int   H264CIF3_FrameWidth      = CIF_WIDTH; 
static unsigned int   H264CIF3_Profile         = H264_PROFILE_BASE;
static unsigned int   H264CIF3_Level           = H264_LEVEL20;
static unsigned int   H264CIF3_MaxBitRate      = H264_LEVEL20_MBPS;
static unsigned int   H264CIF3_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264CIF3)

static const char     H264CIF4_Desc[]          = { "H.264-4CIF1" };
static const char     H264CIF4_MediaFmt[]      = { "H.264-4CIF1" };                             
static unsigned int   H264CIF4_FrameHeight     = CIF4_HEIGHT;               
static unsigned int   H264CIF4_FrameWidth      = CIF4_WIDTH;
static unsigned int   H264CIF4_Profile         = H264_PROFILE_BASE;
static unsigned int   H264CIF4_Level           = H264_LEVEL21;
static unsigned int   H264CIF4_MaxBitRate      = H264_LEVEL21_MBPS;
static unsigned int   H264CIF4_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264CIF4)

static const char     H264CIF42_Desc[]          = { "H.264-4CIF2" };
static const char     H264CIF42_MediaFmt[]      = { "H.264-4CIF2" };                             
static unsigned int   H264CIF42_FrameHeight     = CIF4_HEIGHT;               
static unsigned int   H264CIF42_FrameWidth      = CIF4_WIDTH;
static unsigned int   H264CIF42_Profile         = H264_PROFILE_BASE;
static unsigned int   H264CIF42_Level           = H264_LEVEL22;
static unsigned int   H264CIF42_MaxBitRate      = H264_LEVEL22_MBPS;
static unsigned int   H264CIF42_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264CIF42)

static const char     H264CIF43_Desc[]          = { "H.264-4CIF3" };
static const char     H264CIF43_MediaFmt[]      = { "H.264-4CIF3" };                             
static unsigned int   H264CIF43_FrameHeight     = CIF4_HEIGHT;               
static unsigned int   H264CIF43_FrameWidth      = CIF4_WIDTH;
static unsigned int   H264CIF43_Profile         = H264_PROFILE_BASE;
static unsigned int   H264CIF43_Level           = H264_LEVEL30;
static unsigned int   H264CIF43_MaxBitRate      = H264_LEVEL30_MBPS;
static unsigned int   H264CIF43_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264CIF43)

static const char     H264SD_Desc[]          = { "H.264-SD" };
static const char     H264SD_MediaFmt[]      = { "H.264-SD" };                             
static unsigned int   H264SD_FrameHeight     = SD_HEIGHT;               
static unsigned int   H264SD_FrameWidth      = SD_WIDTH; 
static unsigned int   H264SD_Profile         = H264_PROFILE_BASE;
static unsigned int   H264SD_Level           = H264_LEVEL31;
static unsigned int   H264SD_MaxBitRate      = H264_LEVEL31_MBPS;
static unsigned int   H264SD_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264SD)

static const char     H264HD_Desc[]          = { "H.264-HD" };
static const char     H264HD_MediaFmt[]      = { "H.264-HD" };                             
static unsigned int   H264HD_FrameHeight     = HD_HEIGHT;               
static unsigned int   H264HD_FrameWidth      = HD_WIDTH; 
static unsigned int   H264HD_Profile         = H264_PROFILE_BASE;
static unsigned int   H264HD_Level           = H264_LEVEL32;
static unsigned int   H264HD_MaxBitRate      = H264_LEVEL32_MBPS;
static unsigned int   H264HD_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264HD)

static const char     H264FHD_Desc[]          = { "H.264-FHD" };
static const char     H264FHD_MediaFmt[]      = { "H.264-FHD" };                             
static unsigned int   H264FHD_FrameHeight     = FHD_HEIGHT;               
static unsigned int   H264FHD_FrameWidth      = FHD_WIDTH; 
static unsigned int   H264FHD_Profile         = H264_PROFILE_BASE;
static unsigned int   H264FHD_Level           = H264_LEVEL40;
static unsigned int   H264FHD_MaxBitRate      = H264_LEVEL40_MBPS;
static unsigned int   H264FHD_VideoType       = PluginCodec_MediaTypeVideo;
DECLARE_GENERIC_OPTIONS(H264FHD)

#endif

/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition h264CodecDefn[] = {
#ifdef H323_H264_TEST
  DECLARE_H323PARAM(H264),
//  DECLARE_H323PARAM(H264SQCIF),
//  DECLARE_H323PARAM(H264QCIF),
//  DECLARE_H323PARAM(H264QCIF2),
//  DECLARE_H323PARAM(H264CIF),
//  DECLARE_H323PARAM(H264CIF2),
//  DECLARE_H323PARAM(H264CIF3),
//  DECLARE_H323PARAM(H264CIF4),
//  DECLARE_H323PARAM(H264CIF42),
//  DECLARE_H323PARAM(H264CIF43),
//  DECLARE_H323PARAM(H264SD),
//  DECLARE_H323PARAM(H264HD),
//  DECLARE_H323PARAM(H264FHD)
#endif
};

#endif /* __H264-X264_H__ */
