/*
 * opalplugins.h
 *
 * OPAL codec plugins handler
 *
 * Open Phone Abstraction Library (OPAL)
 * Formally known as the Open H323 project.
 *
 * Copyright (C) 2004-2011 Post Increment
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.

 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#ifndef OPAL_CODEC_OPALPLUGIN_H
#define OPAL_CODEC_OPALPLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <time.h>

#ifdef _MSC_VER
#pragma warning(disable:4201)
#endif

#ifdef _MSC_VER
#  ifdef PLUGIN_CODEC_DLL_EXPORTS
#    define PLUGIN_CODEC_DLL_API __declspec(dllexport)
#  else
#    define PLUGIN_CODEC_DLL_API __declspec(dllimport)
#  endif

#if !defined(strcasecmp) && !defined(_WIN32_WCE)
#define strcasecmp stricmp
#endif

#else

#define PLUGIN_CODEC_DLL_API

#endif

#ifdef PWLIB_PLUGIN_API_VERSION
#undef PWLIB_PLUGIN_API_VERSION
#endif
#define PWLIB_PLUGIN_API_VERSION 1

#define  PLUGIN_CODEC_VERSION_FIRST     1    // initial version
#define  PLUGIN_CODEC_VERSION_WIDEBAND  2    // added wideband
#define  PLUGIN_CODEC_VERSION_VIDEO     3    // added video
#define  PLUGIN_CODEC_VERSION_FAX       4    // added fax
#define  PLUGIN_CODEC_VERSION_OPTIONS   5    // added options handling
#define  PLUGIN_CODEC_VERSION_INTERSECT 6    // added media option intersection merge functionality
#define  PLUGIN_CODEC_VERSION_H245_DEF_GEN_PARAM 7 // added suppression of H.245 generic parameters via default

#define  PLUGIN_CODEC_VERSION PLUGIN_CODEC_VERSION_H245_DEF_GEN_PARAM // Always latest version

#define PLUGIN_CODEC_API_VER_FN       PWLibPlugin_GetAPIVersion
#define PLUGIN_CODEC_API_VER_FN_STR   "PWLibPlugin_GetAPIVersion"

#define PLUGIN_CODEC_GET_CODEC_FN     OpalCodecPlugin_GetCodecs
#define PLUGIN_CODEC_GET_CODEC_FN_STR "OpalCodecPlugin_GetCodecs"

#define PLUGIN_CODEC_API_VER_FN_DECLARE \
PLUGIN_CODEC_DLL_API unsigned int PLUGIN_CODEC_API_VER_FN() \
{ return PWLIB_PLUGIN_API_VERSION; }

enum {
  PluginCodec_License_None                           = 0,
  PluginCodec_Licence_None = PluginCodec_License_None,        // allow for old code with misspelled constant
  PluginCodec_License_GPL                            = 1,
  PluginCodec_License_MPL                            = 2,
  PluginCodec_License_Freeware                       = 3,
  PluginCodec_License_ResearchAndDevelopmentUseOnly  = 4,
  PluginCodec_License_BSD                            = 5,
  PluginCodec_License_LGPL                           = 6,

  PluginCodec_License_NoRoyalties                    = 0x7f,

  // any license codes above here require royalty payments
  PluginCodec_License_RoyaltiesRequired              = 0x80
};

struct PluginCodec_information {
  // start of version 1 fields
  time_t timestamp_deprecated;

  const char * sourceAuthor;            // source code author
  const char * sourceVersion;           // source code version
  const char * sourceEmail;             // source code email contact information
  const char * sourceURL;               // source code web site
  const char * sourceCopyright;         // source code copyright
  const char * sourceLicense;           // source code license
  unsigned char sourceLicenseCode;      // source code license

  const char * codecDescription;        // codec description
  const char * codecAuthor;             // codec author
  const char * codecVersion;            // codec version
  const char * codecEmail;              // codec email contact information
  const char * codecURL;                // codec web site
  const char * codecCopyright;          // codec copyright information
  const char * codecLicense;            // codec license
  unsigned short codecLicenseCode;      // codec license code
  // end of version 1 fields

  const char * timestamp;               // String form of timestamp for plug in, generally as provided by __TIMESTAMP__
};

enum PluginCodec_Flags {
  PluginCodec_MediaTypeMask          = 0x000f,
  PluginCodec_MediaTypeAudio         = 0x0000,
  PluginCodec_MediaTypeVideo         = 0x0001,
  PluginCodec_MediaTypeAudioStreamed = 0x0002,
  PluginCodec_MediaTypeFax           = 0x0003,

  PluginCodec_InputTypeMask          = 0x0010,
  PluginCodec_InputTypeRaw           = 0x0000, // Note video is always RTP
  PluginCodec_InputTypeRTP           = 0x0010,

  PluginCodec_OutputTypeMask         = 0x0020,
  PluginCodec_OutputTypeRaw          = 0x0000, // Note video is always RTP
  PluginCodec_OutputTypeRTP          = 0x0020,

  PluginCodec_RTPTypeMask            = 0x0040,
  PluginCodec_RTPTypeDynamic         = 0x0000,
  PluginCodec_RTPTypeExplicit        = 0x0040,

  PluginCodec_RTPSharedMask          = 0x0080,
  PluginCodec_RTPTypeNotShared       = 0x0000,
  PluginCodec_RTPTypeShared          = 0x0080,

  PluginCodec_DecodeSilenceMask      = 0x0100,
  PluginCodec_NoDecodeSilence        = 0x0000,
  PluginCodec_DecodeSilence          = 0x0100,

  PluginCodec_EncodeSilenceMask      = 0x0200,
  PluginCodec_NoEncodeSilence        = 0x0000,
  PluginCodec_EncodeSilence          = 0x0200,

  PluginCodec_MediaExtensionMask     = 0x0400,
  PluginCodec_MediaTypeExtVideo      = 0x0400,

  PluginCodec_ComfortNoiseMask       = 0x0800,  // Audio only
  PluginCodec_ComfortNoise           = 0x0800,

  PluginCodec_ErrorConcealmentMask   = 0x0800,  // Video only
  PluginCodec_ErrorConcealment       = 0x0800,

  PluginCodec_EmptyPayloadMask       = 0x1000,
  PluginCodec_EmptyPayload           = 0x1000,

  PluginCodec_OtherPayloadMask       = 0x2000,
  PluginCodec_OtherPayload           = 0x2000,

  PluginCodec_BitsPerSamplePos       = 12,
  PluginCodec_BitsPerSampleMask      = 0xf000,

  PluginCodec_ChannelsPos            = 16,
  PluginCodec_ChannelsMask           = 0x003f0000
};

#define PluginCodec_SetChannels(n) (((n-1)<<PluginCodec_ChannelsPos)&PluginCodec_ChannelsMask)


enum PluginCodec_CoderFlags {
  PluginCodec_CoderSilenceFrame      = 1,    // request audio codec to create silence frame
  PluginCodec_CoderForceIFrame       = 2,    // request video codec to force I frame
  PluginCodec_CoderPacketLoss        = 4     // indicate to video codec packets were lost
};

enum PluginCodec_ReturnCoderFlags {
  PluginCodec_ReturnCoderLastFrame      = 1,    // indicates when video codec returns last data for frame
  PluginCodec_ReturnCoderIFrame         = 2,    // indicates when video returns I frame
  PluginCodec_ReturnCoderRequestIFrame  = 4,    // indicates when video decoder request I frame for resync
  PluginCodec_ReturnCoderBufferTooSmall = 8     // indicates when output buffer is not large enough to receive
                                                // the data, another call to get_output_data_size is required
};

struct PluginCodec_Definition;

// Control function names

#define PLUGINCODEC_CONTROL_VALID_FOR_PROTOCOL    "valid_for_protocol"
#define PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS     "get_codec_options"
#define PLUGINCODEC_CONTROL_FREE_CODEC_OPTIONS    "free_codec_options"
#define PLUGINCODEC_CONTROL_GET_OUTPUT_DATA_SIZE  "get_output_data_size"
#define PLUGINCODEC_CONTROL_SET_CODEC_OPTIONS     "set_codec_options"
#define PLUGINCODEC_CONTROL_GET_ACTIVE_OPTIONS    "get_active_options"
#define PLUGINCODEC_CONTROL_TO_NORMALISED_OPTIONS "to_normalised_options"
#define PLUGINCODEC_CONTROL_TO_CUSTOMISED_OPTIONS "to_customised_options"
#define PLUGINCODEC_CONTROL_SET_INSTANCE_ID       "set_instance_id"
#define PLUGINCODEC_CONTROL_SET_LOG_FUNCTION      "set_log_function"
#define PLUGINCODEC_CONTROL_GET_STATISTICS        "get_statistics"
#define PLUGINCODEC_CONTROL_TERMINATE_CODEC       "terminate_codec"


/* Log function, plug in gets a pointer to this function which allows
   it to use the standard OPAL logging system. The function returns 0 if
   no logging was performed due to the log level. Note if log == NULL
   then this return state is all that happens, so this may be executed
   first to prevent lengthy logs that would not result in any output. */
typedef int (*PluginCodec_LogFunction)(unsigned level,
                                       const char * file,
                                       unsigned line,
                                       const char * section,
                                       const char * log);


struct PluginCodec_ControlDefn {
  const char * name;
  int (*control)(const struct PluginCodec_Definition * codec, void * context,
                 const char * name, void * parm, unsigned * parmLen);

};

enum PluginCodec_OptionTypes {
  PluginCodec_StringOption,
  PluginCodec_BoolOption,
  PluginCodec_IntegerOption,
  PluginCodec_RealOption,
  PluginCodec_EnumOption,
  PluginCodec_OctetsOption,
  PluginCodec_NumOptionTypes,
};

enum PluginCodec_OptionMerge {
  PluginCodec_NoMerge,
  PluginCodec_MinMerge,
  PluginCodec_MaxMerge,
  PluginCodec_EqualMerge,
  PluginCodec_NotEqualMerge,
  PluginCodec_AlwaysMerge,
  PluginCodec_CustomMerge,
  PluginCodec_IntersectionMerge,

  PluginCodec_AndMerge = PluginCodec_MinMerge,
  PluginCodec_OrMerge  = PluginCodec_MaxMerge
};

#define PluginCodec_H245_Collapsing    0x40000000
#define PluginCodec_H245_NonCollapsing 0x20000000
#define PluginCodec_H245_Unsigned32    0x10000000
#define PluginCodec_H245_BooleanArray  0x08000000
#define PluginCodec_H245_TCS           0x04000000
#define PluginCodec_H245_OLC           0x02000000
#define PluginCodec_H245_ReqMode       0x01000000
#define PluginCodec_H245_OrdinalMask   0x0000ffff
#define PluginCodec_H245_PositionMask  0x00ff0000
#define PluginCodec_H245_PositionShift 16

typedef int (*PluginCodec_MergeFunction)(char ** result, const char * dest, const char * src);
typedef void (*PluginCodec_FreeFunction)(char * string);

struct PluginCodec_Option {
  // start of version 4 fields
  enum PluginCodec_OptionTypes m_type;
  const char *                 m_name;
  unsigned                     m_readOnly;
  enum PluginCodec_OptionMerge m_merge;
  const char *                 m_value;
  const char *                 m_FMTPName;
  const char *                 m_FMTPDefault;
  int                          m_H245Generic;
  const char *                 m_minimum;
  const char *                 m_maximum;
  PluginCodec_MergeFunction    m_mergeFunction; // Used if m_merge==PluginCodec_CustomMerge
  PluginCodec_FreeFunction     m_freeFunction;
  const char *                 m_H245Default;
};


// Normalised option names
#define PLUGINCODEC_OPTION_NEEDS_JITTER               "Needs Jitter"
#define PLUGINCODEC_OPTION_CLOCK_RATE                 "Clock Rate"
#define PLUGINCODEC_OPTION_CHANNELS                   "Channels"
#define PLUGINCODEC_OPTION_FRAME_TIME                 "Frame Time"
#define PLUGINCODEC_OPTION_MAX_FRAME_SIZE             "Max Frame Size"
#define PLUGINCODEC_OPTION_MAX_TX_PACKET_SIZE         "Max Tx Packet Size"   /* Really max PAYLOAD size */
#define PLUGINCODEC_OPTION_MAX_BIT_RATE               "Max Bit Rate"
#define PLUGINCODEC_OPTION_TARGET_BIT_RATE            "Target Bit Rate"
#define PLUGINCODEC_OPTION_RATE_CONTROL_PERIOD        "Rate Control Period"
#define PLUGINCODEC_OPTION_RX_FRAMES_PER_PACKET       "Rx Frames Per Packet"
#define PLUGINCODEC_OPTION_TX_FRAMES_PER_PACKET       "Tx Frames Per Packet"
#define PLUGINCODEC_OPTION_FRAME_WIDTH                "Frame Width"
#define PLUGINCODEC_OPTION_FRAME_HEIGHT               "Frame Height"
#define PLUGINCODEC_OPTION_MIN_RX_FRAME_WIDTH         "Min Rx Frame Width"
#define PLUGINCODEC_OPTION_MIN_RX_FRAME_HEIGHT        "Min Rx Frame Height"
#define PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH         "Max Rx Frame Width"
#define PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT        "Max Rx Frame Height"
#define PLUGINCODEC_OPTION_TEMPORAL_SPATIAL_TRADE_OFF "Temporal Spatial Trade Off"
#define PLUGINCODEC_OPTION_TX_KEY_FRAME_PERIOD        "Tx Key Frame Period"

#define PLUGINCODEC_OPTION_PROTOCOL      "Protocol"
#define PLUGINCODEC_OPTION_PROTOCOL_H323 "H.323"
#define PLUGINCODEC_OPTION_PROTOCOL_SIP  "SIP"


// Full definition of the codec

struct PluginCodec_Definition {
  unsigned int version;                     // codec structure version

  // start of version 1 fields
  const struct PluginCodec_information * info;   // license information

  unsigned int flags;                      // b0-3: 0 = audio,        1 = video
                                           // b4:   0 = raw input,    1 = RTP input
                                           // b5:   0 = raw output,   1 = RTP output
                                           // b6:   0 = dynamic RTP,  1 = explicit RTP
                                           // b7:   0 = no share RTP, 1 = share RTP

  const char * descr;                       // text decription

  const char * sourceFormat;               // source format
  const char * destFormat;                 // destination format

  const void * userData;                   // user data value

  unsigned int sampleRate;                 // samples per second
  unsigned int bitsPerSec;                // raw bits per second
  unsigned int usPerFrame;                 // microseconds per frame

  union _parm {
    struct _audio {
      unsigned int samplesPerFrame;            // audio: samples per frame
      unsigned int bytesPerFrame;              // audio: max bytes per frame
      unsigned int recommendedFramesPerPacket; // audio: recommended number of frames per packet
      unsigned int maxFramesPerPacket;         // audio: maximum number of frames per packet
    } audio;
    struct _video {
      unsigned int maxFrameWidth;              // video: frame width
      unsigned int maxFrameHeight;             // video: frame height
      unsigned int recommendedFrameRate;       // video: recommended frame rate
      unsigned int maxFrameRate;               // video: max frame rate
    } video;
  } parm;

  unsigned char rtpPayload;                 // IANA RTP payload code (if defined)
  const char * sdpFormat;                  // SDP format string (or NULL, if no SDP format)

  void * (*createCodec)(const struct PluginCodec_Definition * codec);                    // create codec
  void (*destroyCodec) (const struct PluginCodec_Definition * codec,  void * context);   // destroy codec
  int (*codecFunction) (const struct PluginCodec_Definition * codec,  void * context,   // do codec function
                                  const void * from, unsigned * fromLen,
                                        void * to,   unsigned * toLen,
                                        unsigned int * flag);
  struct PluginCodec_ControlDefn * codecControls;

  // H323 specific fields
  unsigned char h323CapabilityType;
  const void  * h323CapabilityData;

  // end of version 1 fields

  // NOTE!!!!! Due to an error in judgement, you cannot add ANY more fields
  // to this structure without an API version change!!!!
};

typedef const struct PluginCodec_Definition * (* PluginCodec_GetCodecFunction)(unsigned int *, unsigned int);
typedef unsigned (* PluginCodec_GetAPIVersionFunction)();


///////////////////////////////////////////////////////////////////

#define PLUGINCODEC_RAW_AUDIO "L16"
#define PLUGINCODEC_RAW_VIDEO "YUV420P"
#define PLUGINCODEC_VIDEO_CLOCK 90000
#define PLUGINCODEC_MAX_FRAME_RATE 30

#define PLUGINCODEC_CIF_WIDTH     352
#define PLUGINCODEC_CIF_HEIGHT    288

#define PLUGINCODEC_CIF4_WIDTH   (PLUGINCODEC_CIF_WIDTH*2)
#define PLUGINCODEC_CIF4_HEIGHT  (PLUGINCODEC_CIF_HEIGHT*2)

#define PLUGINCODEC_CIF16_WIDTH  (PLUGINCODEC_CIF_WIDTH*4)
#define PLUGINCODEC_CIF16_HEIGHT (PLUGINCODEC_CIF_HEIGHT*4)

#define PLUGINCODEC_QCIF_WIDTH   (PLUGINCODEC_CIF_WIDTH/2)
#define PLUGINCODEC_QCIF_HEIGHT  (PLUGINCODEC_CIF_HEIGHT/2)

#define PLUGINCODEC_QCIF4_WIDTH  (PLUGINCODEC_CIF4_WIDTH/2)
#define PLUGINCODEC_QCIF4_HEIGHT (PLUGINCODEC_CIF4_HEIGHT/2)

#define PLUGINCODEC_SQCIF_WIDTH   128
#define PLUGINCODEC_SQCIF_HEIGHT   96


#ifndef __TIMESTAMP__
#define __TIMESTAMP__ "0"
#endif

#define PLUGINCODEC_LICENSE(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15) \
  static struct PluginCodec_information MyLicenseInfo = { 0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,__TIMESTAMP__ }


/// Declare a pair of plug in definition entries for a codec
#define PLUGINCODEC_CODEC_PAIR(MediaFormat,    /**< Media Format */ \
                               PayloadName,    /**< IANA RTP payload code */ \
                               Description,    /**< Description text */ \
                               SampleRate,     /**< Sample rate */ \
                               BitsPerSecond,  /**< Maximum bits per second */ \
                               FrameTime,      /**< Microseconds per frame */ \
                               p1,p2,p3,p4, \
                               PayloadType,    /**< IANA RTP payload type code */ \
                               H323type,       /**< h323CapabilityType enumeration */ \
                               H323data,       /**< Data to go with h323CapabilityType */ \
                               CreateEncoder,  /**< Create encoder function */ \
                               DestroyEncoder, /**< Destroy encoder function */ \
                               EncodeMedia,    /**< Encode media function */ \
                               CreateDecoder,  /**< Create decoder function */ \
                               DestroyDecoder, /**< Destroy decoder function */ \
                               DecodeMedia,    /**< Decode media function */ \
                               ControlsTable,  /**< Codec controls tables */ \
                               Flags,          /**< Flags */ \
                               RawFormat,      /**< Raw format */ \
                               UserData        /**< User Data */ \
                               ) \
  { \
    PLUGIN_CODEC_VERSION, &MyLicenseInfo, Flags, Description, RawFormat, MediaFormat, UserData, \
    SampleRate, BitsPerSecond, FrameTime, {{ p1,p2,p3,p4 }}, PayloadType, PayloadName, \
    CreateEncoder, DestroyEncoder, EncodeMedia, ControlsTable, H323type, H323data \
  }, \
  { \
    PLUGIN_CODEC_VERSION, &MyLicenseInfo, Flags, Description, MediaFormat, RawFormat, UserData, \
    SampleRate, BitsPerSecond, FrameTime, {{ p1,p2,p3,p4 }}, PayloadType, PayloadName, \
    CreateDecoder, DestroyDecoder, DecodeMedia, ControlsTable, H323type, H323data \
  }

#define PLUGINCODEC_AUDIO_CODEC(MediaFormat,     /**< Media Format */ \
                                PayloadName,     /**< IANA RTP payload code */ \
                                Description,     /**< Description text */ \
                                SampleRate,      /**< Sample rate */ \
                                BitsPerSecond,   /**< Maximum bits per second */ \
                                SamplesPerFrame, /**< Samples per audio frame */ \
                                RecFramesPerPacket, /**< Recommended frames per packet */ \
                                MaxFramesPerPacket, /**< Maximum frames per packet */ \
                                RtpFlags,         /**< Extra flags typically if RTP payload type is fixed */ \
                                PayloadType,      /**< IANA RTP payload type code */ \
                                H323type,         /**< h323CapabilityType enumeration */ \
                                H323data,         /**< Data to go with h323CapabilityType */ \
                                CreateEncoder,    /**< Create encoder function */ \
                                DestroyEncoder,   /**< Destroy encoder function */ \
                                EncodeAudio,      /**< Encode media function */ \
                                CreateDecoder,    /**< Create decoder function */ \
                                DestroyDecoder,   /**< Destroy decoder function */ \
                                DecodeAudio,      /**< Decode media function */ \
                                ControlsTable     /**< Codec controls tables */ \
                                ) \
         PLUGINCODEC_CODEC_PAIR(MediaFormat, \
                                PayloadName, \
                                Description, \
                                SampleRate, \
                                BitsPerSecond, \
                                (SamplesPerFrame)*1000000/(SampleRate), \
                                SamplesPerFrame, \
                                (BitsPerSecond)*(SampleRate)/(SamplesPerFrame)/8, \
                                RecFramesPerPacket, \
                                MaxFramesPerPacket, \
                                PayloadType, \
                                H323type, \
                                H323data, \
                                CreateEncoder, \
                                DestroyEncoder, \
                                EncodeAudio, \
                                CreateDecoder, \
                                DestroyDecoder, \
                                DecodeAudio, \
                                ControlsTable, \
                                PluginCodec_MediaTypeAudio | /* audio codec */ \
                                PluginCodec_InputTypeRaw |   /* raw input data */ \
                                PluginCodec_OutputTypeRaw |  /* raw output data */ \
                                (RtpFlags), \
                                PLUGINCODEC_RAW_AUDIO, \
                                NULL)

#define PLUGINCODEC_ONE_AUDIO_CODEC(MediaFormat,     /**< Media Format */ \
                                    PayloadName,     /**< IANA RTP payload code */ \
                                    Description,     /**< Description text */ \
                                    SampleRate,      /**< Sample rate */ \
                                    BitsPerSecond,   /**< Maximum bits per second */ \
                                    SamplesPerFrame, /**< Samples per audio frame */ \
                                    RecFramesPerPacket, /**< Recommended frames per packet */ \
                                    MaxFramesPerPacket, /**< Maximum frames per packet */ \
                                    RtpFlags,         /**< Extra flags typically if RTP payload type is fixed */ \
                                    PayloadType,      /**< IANA RTP payload type code */ \
                                    H323type,         /**< h323CapabilityType enumeration */ \
                                    H323data          /**< Data to go with h323CapabilityType */ \
                                ) \
    static struct PluginCodec_Definition CodecDefinitionTable[] = { \
            PLUGINCODEC_AUDIO_CODEC(MediaFormat, \
                                    PayloadName, \
                                    Description, \
                                    SampleRate, \
                                    BitsPerSecond, \
                                    SamplesPerFrame, \
                                    RecFramesPerPacket, \
                                    MaxFramesPerPacket, \
                                    RtpFlags, \
                                    PayloadType, \
                                    H323type, \
                                    H323data, \
                                    MyCreateEncoder, \
                                    MyDestroyEncoder, \
                                    MyEncodeAudio, \
                                    MyCreateDecoder, \
                                    MyDestroyDecoder, \
                                    MyDecodeAudio, \
                                    MyControlsTable \
                                    ) \
    }

#define PLUGINCODEC_VIDEO_CODEC(MediaFormat,     /**< Media Format */ \
                                PayloadName,     /**< IANA RTP payload code */ \
                                Description,     /**< Description text */ \
                                BitsPerSecond,   /**< Maximum bits per second */ \
                                MaxWidth,        /**< Max resolution (width) */ \
                                MaxHeight,       /**< Max resolution (height) */ \
                                RtpFlags,        /**< Extra flags typically if RTP payload type is fixed */ \
                                PayloadType,     /**< IANA RTP payload type code */ \
                                H323type,        /**< h323CapabilityType enumeration */ \
                                H323data,        /**< Data to go with h323CapabilityType */ \
                                CreateEncoder,   /**< Create encoder function */ \
                                DestroyEncoder,  /**< Destroy encoder function */ \
                                EncodeVideo,     /**< Encode media function */ \
                                CreateDecoder,   /**< Create decoder function */ \
                                DestroyDecoder,  /**< Destroy decoder function */ \
                                DecodeVideo,     /**< Decode media function */ \
                                ControlsTable    /**< Codec controls tables */ \
                                ) \
         PLUGINCODEC_CODEC_PAIR(MediaFormat, \
                                PayloadName, \
                                Description, \
                                SampleRate, \
                                BitsPerSecond, \
                                PLUGINCODEC_VIDEO_CLOCK, \
                                BitsPerSecond, \
                                1000000/PLUGINCODEC_MAX_FRAME_RATE, \
                                MaxWidth, \
                                MaxHeight, \
                                0,PLUGINCODEC_MAX_FRAME_RATE, \
                                PayloadType, \
                                H323type, \
                                H323data, \
                                CreateEncoder, \
                                DestroyEncoder, \
                                EncodeVideo, \
                                CreateDecoder, \
                                DestroyDecoder, \
                                DecodeVideo, \
                                ControlsTable, \
                                PluginCodec_MediaTypeVideo | /* video codec */ \
                                PluginCodec_InputTypeRTP |   /* RTP input data */ \
                                PluginCodec_OutputTypeRTP |  /* RTP output data */ \
                                (RtpFlags), \
                                PLUGINCODEC_RAW_VIDEO, \
                                NULL)

#define PLUGINCODEC_ONE_VIDEO_CODEC(MediaFormat,     /**< Media Format */ \
                                    PayloadName,     /**< IANA RTP payload code */ \
                                    Description,     /**< Description text */ \
                                    BitsPerSecond,   /**< Maximum bits per second */ \
                                    MaxWidth,        /**< Max resolution (width) */ \
                                    MaxHeight,       /**< Max resolution (height) */ \
                                    RtpFlags,        /**< Extra flags typically if RTP payload type is fixed */ \
                                    PayloadType,     /**< IANA RTP payload type code */ \
                                    H323type,        /**< h323CapabilityType enumeration */ \
                                    H323data         /**< Data to go with h323CapabilityType */ \
                                ) \
    static struct PluginCodec_Definition CodecDefinitionTable[] = { \
            PLUGINCODEC_VIDEO_CODEC(MediaFormat, \
                                    PayloadName, \
                                    Description, \
                                    BitsPerSecond, \
                                    MaxWidth, \
                                    MaxHeight, \
                                    RtpFlags, \
                                    PayloadType, \
                                    H323type, \
                                    H323data, \
                                    CreateEncoder, \
                                    DestroyEncoder, \
                                    EncodeAudio, \
                                    CreateDecoder, \
                                    DestroyDecoder, \
                                    DecodeAudio, \
                                    ControlsTable \
                                    ) \
    }


//////////////////////////////////////////////////////////////////
//
//  H.323 specific values
//


struct PluginCodec_H323CapabilityExtension {
  unsigned int index;
  void * data;
  unsigned dataLength;
};

struct PluginCodec_H323NonStandardCodecData {
  const char * objectId;
  unsigned char  t35CountryCode;
  unsigned char  t35Extension;
  unsigned short manufacturerCode;
  const unsigned char * data;
  unsigned int dataLength;
  int (*capabilityMatchFunction)(struct PluginCodec_H323NonStandardCodecData *);
};


struct PluginCodec_H323GenericParameterDefinition
{
  /* The following used to be a simple integer for the collapsing flag in
     version 3 and earlier. We hope that all those implementations just used
     zero and one (a good bet) and thus the below bit fields will be backward
     compatible, putting the parameter in all three PDU types.
   */ 
#ifndef SOLARIS   
  struct {
#endif  
    int collapsing:1; /* boolean */
    int excludeTCS:1;
    int excludeOLC:1;
    int excludeReqMode:1;
    int readOnly:1;
#ifndef SOLARIS    
  };
#endif  

  unsigned int id;

  enum PluginCodec_H323GenericParameterType {
    /* these need to be in the same order as the choices in
      H245_ParameterValue::Choices, as the value is just cast to that type
    */
    PluginCodec_GenericParameter_Logical = 0,
    PluginCodec_GenericParameter_BooleanArray,
    PluginCodec_GenericParameter_UnsignedMin,
    PluginCodec_GenericParameter_UnsignedMax,
    PluginCodec_GenericParameter_Unsigned32Min,
    PluginCodec_GenericParameter_Unsigned32Max,
    PluginCodec_GenericParameter_OctetString,
    PluginCodec_GenericParameter_GenericParameter,

    PluginCodec_GenericParameter_logical = 0,
    PluginCodec_GenericParameter_booleanArray,
    PluginCodec_GenericParameter_unsignedMin,
    PluginCodec_GenericParameter_unsignedMax,
    PluginCodec_GenericParameter_unsigned32Min,
    PluginCodec_GenericParameter_unsigned32Max,
    PluginCodec_GenericParameter_octetString,
    PluginCodec_GenericParameter_genericParameter
  } type;

  union {
    unsigned long integer;
    const char * octetstring;
    struct PluginCodec_H323GenericParameterDefinition *genericparameter;
  } value;
};

struct PluginCodec_H323GenericCodecData
{
  // some cunning structures & lists, and associated logic in
  // H323CodecPluginGenericAudioCapability::H323CodecPluginGenericAudioCapability()
  const char * standardIdentifier;
  unsigned int maxBitRate; // Zero means use value from OpalMediaFormat

  /* parameters; these are the parameters which are set in the
     'TerminalCapabilitySet' and 'OpenLogicalChannel' requests */
  unsigned int nParameters;
  /* an array of nParameters parameter definitions */
  const struct PluginCodec_H323GenericParameterDefinition *params;
};


struct PluginCodec_H323AudioGSMData {
  int comfortNoise:1;
  int scrambled:1;
};

struct  PluginCodec_H323AudioG7231AnnexC {
  unsigned char maxAl_sduAudioFrames;
  int silenceSuppression:1;
  int highRateMode0:6;          // INTEGER (27..78),  -- units octets
  int  highRateMode1:6;          // INTEGER (27..78),  -- units octets
  int  lowRateMode0:6;            // INTEGER (23..66),  -- units octets
  int  lowRateMode1:6;            // INTEGER (23..66),  -- units octets
  int  sidMode0:4;                // INTEGER (6..17),  -- units octets
  int  sidMode1:4;                // INTEGER (6..17),  -- units octets
};


enum {
  PluginCodec_H323Codec_undefined,      // must be zero, so empty struct is undefined
  PluginCodec_H323Codec_programmed,      // H323ProgrammedCapability
  PluginCodec_H323Codec_nonStandard,    // H323NonStandardData
  PluginCodec_H323Codec_generic,            // H323GenericCodecData

  // audio codecs
  PluginCodec_H323AudioCodec_g711Alaw_64k,        // int
  PluginCodec_H323AudioCodec_g711Alaw_56k,        // int
  PluginCodec_H323AudioCodec_g711Ulaw_64k,        // int
  PluginCodec_H323AudioCodec_g711Ulaw_56k,        // int
  PluginCodec_H323AudioCodec_g722_64k,            // int
  PluginCodec_H323AudioCodec_g722_56k,            // int
  PluginCodec_H323AudioCodec_g722_48k,            // int
  PluginCodec_H323AudioCodec_g7231,                // H323AudioG7231Data
  PluginCodec_H323AudioCodec_g728,                // int
  PluginCodec_H323AudioCodec_g729,                // int
  PluginCodec_H323AudioCodec_g729AnnexA,          // int
  PluginCodec_H323AudioCodec_is11172,             // not yet implemented
  PluginCodec_H323AudioCodec_is13818Audio,        // not yet implemented
  PluginCodec_H323AudioCodec_g729wAnnexB,          // int
  PluginCodec_H323AudioCodec_g729AnnexAwAnnexB,    // int
  PluginCodec_H323AudioCodec_g7231AnnexC,         // H323AudioG7231AnnexC
  PluginCodec_H323AudioCodec_gsmFullRate,          // H323AudioGSMData
  PluginCodec_H323AudioCodec_gsmHalfRate,          // H323AudioGSMData
  PluginCodec_H323AudioCodec_gsmEnhancedFullRate,  // H323AudioGSMData
  PluginCodec_H323AudioCodec_g729Extensions,      // not yet implemented

  // video codecs
  PluginCodec_H323VideoCodec_h261,                // implemented
  PluginCodec_H323VideoCodec_h262,                // not yet implemented
  PluginCodec_H323VideoCodec_h263,                // implemented
  PluginCodec_H323VideoCodec_is11172,             // not yet implemented

  // other capabilities
  PluginCodec_H323VideoCodec_Extended,            // implemented (for use with H.239)
  PluginCodec_H323T38Codec,                       // not yet implemented

  // special codes
  PluginCodec_H323Codec_NoH323 = 0xff,            // used for SIP-only codecs
};

/////////////////
//
// Generic Codec Standard Identifiers
//

// Audio Capabilities
// AMR (as defined in H.245v13 Annex I)
#define OpalPluginCodec_Identifer_AMR             "0.0.8.245.1.1.1"

// AMR-NB\WB  (as defined in H.245v13 Annex R)
#define OpalPluginCodec_Identifer_AMR_NB          "0.0.8.245.1.1.9"
#define OpalPluginCodec_Identifer_AMR_WB          "0.0.8.245.1.1.10"

// G.722.1
#define OpalPluginCodec_Identifer_G7221           "0.0.7.7221.1.0"
#define OpalPluginCodec_Identifer_G7221ext        "0.0.7.7221.1.1.0"

// G.722.2 (aka AMR-WB)
#define OpalPluginCodec_Identifer_G7222           "0.0.7.7222.1.0"

// iLBC (as defined in H.245v13 Annex S)
#define OpalPluginCodec_Identifer_iLBC            "0.0.8.245.1.1.11"


// Video Capabilities

// H264 (as defined in H.241)
#define OpalPluginCodec_Identifer_H264_Aligned        "0.0.8.241.0.0.0.0"
#define OpalPluginCodec_Identifer_H264_NonInterleaved "0.0.8.241.0.0.0.1"
#define OpalPluginCodec_Identifer_H264_Interleaved    "0.0.8.241.0.0.0.2"
#define OpalPluginCodec_Identifer_H264_Generic        "0.0.8.241.0.0.1"

// ISO/IEC 14496-2 MPEG4 part 2 (as defined in H.245v13 Annex E)
#define OpalPluginCodec_Identifer_MPEG4           "0.0.8.245.1.0.0"


/////////////////
//
// Predefined options for H.323 codecs
//

#define PLUGINCODEC_SQCIF_MPI   "SQCIF MPI"
#define PLUGINCODEC_QCIF_MPI     "QCIF MPI"
#define PLUGINCODEC_CIF_MPI       "CIF MPI"
#define PLUGINCODEC_CIF4_MPI     "CIF4 MPI"
#define PLUGINCODEC_CIF16_MPI   "CIF16 MPI"
#define PLUGINCODEC_CUSTOM_MPI "Custom MPI"

#define PLUGINCODEC_MPI_DISABLED 33

#define PLUGINCODEC_MEDIA_PACKETIZATION  "Media Packetization"
#define PLUGINCODEC_MEDIA_PACKETIZATIONS "Media Packetizations"

#define H261_ANNEX_D "Annex D - Still Image Transmit"
#define H263_ANNEX_D "Annex D - Unrestricted Motion Vector"
#define H263_ANNEX_F "Annex F - Advanced Prediction"
#define H263_ANNEX_I "Annex I - Advanced INTRA Coding"
#define H263_ANNEX_J "Annex J - Deblocking Filter"
#define H263_ANNEX_K "Annex K - Slice Structure"
#define H263_ANNEX_N "Annex N - Reference Picture Selection"
#define H263_ANNEX_S "Annex S - Alternative INTER VLC"
#define H263_ANNEX_T "Annex T - Modified Quantization"

#ifndef STRINGIZE
#define __INTERNAL_STRINGIZE__(v) #v
#define STRINGIZE(v) __INTERNAL_STRINGIZE__(v)
#endif


/////////////////
//
// RTP specific definitions
//

#define PluginCodec_RTP_MaxPacketSize  (1518-14-4-8-20-16)  // Max Ethernet packet (1518 bytes) minus 802.3/CRC, 802.3, IP, UDP headers
#define PluginCodec_RTP_MinHeaderSize  (12)
#define PluginCodec_RTP_MaxPayloadSize (PluginCodec_RTP_MaxPacketSize - PluginCodec_RTP_MinHeaderSize)

#define PluginCodec_RTP_GetWORD(ptr, off)       ((((unsigned char*)(ptr))[off] << 8) | ((unsigned char*)(ptr))[off+1])

#define PluginCodec_RTP_GetDWORD(ptr, off)      ((((unsigned char*)(ptr))[off  ] << 24)|\
                                                 (((unsigned char*)(ptr))[off+1] << 16)|\
                                                 (((unsigned char*)(ptr))[off+2] << 8 )|\
                                                  ((unsigned char*)(ptr))[off+3])

#define PluginCodec_RTP_SetWORD(ptr, off, val)  ((((unsigned char*)(ptr))[off  ] = (unsigned char)((val) >> 8 )),\
                                                 (((unsigned char*)(ptr))[off+1] = (unsigned char) (val)      ))

#define PluginCodec_RTP_SetDWORD(ptr, off, val) ((((unsigned char*)(ptr))[off  ] = (unsigned char)((val) >> 24)),\
                                                 (((unsigned char*)(ptr))[off+1] = (unsigned char)((val) >> 16)),\
                                                 (((unsigned char*)(ptr))[off+2] = (unsigned char)((val) >> 8 )),\
                                                 (((unsigned char*)(ptr))[off+3] = (unsigned char) (val)      ))

#define PluginCodec_RTP_GetCSRCHdrLength(ptr)      ((((unsigned char*)(ptr))[0] & 0x0f)*4 + PluginCodec_RTP_MinHeaderSize)
#define PluginCodec_RTP_GetExtHdrLength(ptr)       ((((unsigned char*)(ptr))[0] & 0x10) ? (PluginCodec_RTP_GetWORD(ptr, PluginCodec_RTP_GetCSRCHdrLength(ptr)+2)*4+4) : 0)

#define PluginCodec_RTP_GetHeaderLength(ptr)       (PluginCodec_RTP_GetCSRCHdrLength(ptr) + PluginCodec_RTP_GetExtHdrLength(ptr))
#define PluginCodec_RTP_GetPayloadPtr(ptr)           ((unsigned char*)(ptr) + PluginCodec_RTP_GetHeaderLength(ptr))
#define PluginCodec_RTP_GetPayloadType(ptr)         (((unsigned char*)(ptr))[1] & 0x7f)
#define PluginCodec_RTP_SetPayloadType(ptr, type)   (((unsigned char*)(ptr))[1] = (unsigned char)((((unsigned char*)(ptr))[1] & 0x80) | (type & 0x7f)))
#define PluginCodec_RTP_GetMarker(ptr)             ((((unsigned char*)(ptr))[1] & 0x80) != 0)
#define PluginCodec_RTP_SetMarker(ptr, mark)        (((unsigned char*)(ptr))[1] = (unsigned char)((((unsigned char*)(ptr))[1] & 0x7f) | (mark != 0 ? 0x80 : 0)))
#define PluginCodec_RTP_GetTimestamp(ptr)          PluginCodec_RTP_GetDWORD(ptr, 4)
#define PluginCodec_RTP_SetTimestamp(ptr, ts)      PluginCodec_RTP_SetDWORD(ptr, 4, ts)
#define PluginCodec_RTP_GetSequenceNumber(ptr)     PluginCodec_RTP_GetWORD(ptr, 2)
#define PluginCodec_RTP_SetSequenceNumber(ptr, sn) PluginCodec_RTP_SetWORD(ptr, 2, sn)
#define PluginCodec_RTP_GetSSRC(ptr)               PluginCodec_RTP_GetDWORD(ptr, 8)
#define PluginCodec_RTP_SetSSRC(ptr, ssrc)         PluginCodec_RTP_SetDWORD(ptr, 8, ssrc)



/////////////////
//
// video specific definitions
//

struct PluginCodec_Video_FrameHeader {
  unsigned int  x;
  unsigned int  y;
  unsigned int  width;
  unsigned int  height;
};

#ifdef __cplusplus
};

inline unsigned char * OPAL_VIDEO_FRAME_DATA_PTR(struct PluginCodec_Video_FrameHeader * base)
{ return (((unsigned char *)base) + sizeof(PluginCodec_Video_FrameHeader)); }

inline unsigned char * OPAL_VIDEO_FRAME_DATA_PTR(const PluginCodec_Video_FrameHeader * base)
{ return (((unsigned char *)base) + sizeof(PluginCodec_Video_FrameHeader)); }

extern "C" {
#endif


/////////////////
//
// experimental definitions for statically linking codecs
//

#ifdef OPAL_STATIC_CODEC

#  undef PLUGIN_CODEC_DLL_API
#  define PLUGIN_CODEC_DLL_API static
#  define PLUGIN_CODEC_IMPLEMENT(name) \
unsigned int Opal_StaticCodec_##name##_GetAPIVersion() \
{ return PWLIB_PLUGIN_API_VERSION; } \
static const struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned /*version*/); \
struct const PluginCodec_Definition * Opal_StaticCodec_##name##_GetCodecs(unsigned * p1, unsigned p2) \
{ return PLUGIN_CODEC_GET_CODEC_FN(p1,p2); } \

#  define PLUGIN_CODEC_IMPLEMENT_ALL(name, table, ver) \
unsigned int Opal_StaticCodec_##name##_GetAPIVersion() \
{ return PWLIB_PLUGIN_API_VERSION; } \
PLUGIN_CODEC_DLL_API const struct PluginCodec_Definition * Opal_StaticCodec_##name##_GetCodecs(unsigned * count, unsigned version) \
{ *count = sizeof(table)/sizeof(struct PluginCodec_Definition); return version < ver ? NULL : table; }


#else

#  define PLUGIN_CODEC_IMPLEMENT(name) \
PLUGIN_CODEC_DLL_API unsigned int PLUGIN_CODEC_API_VER_FN() \
{ return PWLIB_PLUGIN_API_VERSION; } \

#  define PLUGIN_CODEC_IMPLEMENT_ALL(name, table, ver) \
PLUGIN_CODEC_IMPLEMENT(name) \
PLUGIN_CODEC_DLL_API const struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned version) \
{ *count = sizeof(table)/sizeof(struct PluginCodec_Definition); return version < ver ? NULL : table; }


#endif

#ifdef __cplusplus
};
#endif


#ifdef _MSC_VER
#pragma warning(default:4201)
#endif

#endif // OPAL_CODEC_OPALPLUGIN_H
