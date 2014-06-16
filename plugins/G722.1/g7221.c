/****************************************************************************
 *
 * ITU G.722.1 wideband audio codec plugin for OpenH323/OPAL
 *
 * Copyright (C) 2009 Nimajin Software Consulting, All Rights Reserved
 * Copyright (C) 2010 Vox Lucida Pty. Ltd.
 *
 * Permission to copy, use, sell and distribute this file is granted
 * provided this copyright notice appears in all copies.
 * Permission to modify the code herein and to distribute modified code is
 * granted provided this copyright notice appears in all copies, and a
 * notice that the code was modified is included with the copyright notice.
 *
 * This software and information is provided "as is" without express or im-
 * plied warranty, and with no claim as to its suitability for any purpose.
 *
 ****************************************************************************
 *
 * ITU-T 7/14kHz Audio Coder (G.722.1) AKA Polycom 'Siren'
 * SDP usage described in RFC 5577
 * H.245 capabilities defined in G.722.1 Annex A & H.245 Appendix VIII
 * This implementation employs ITU-T G.722.1 (2005-05) fixed point reference 
 * code, release 2.1 (2008-06)
 * This implements only standard bit rates 24000 & 32000 at 16kHz (Siren7)
 * sampling rate, not extended modes or 32000 sampling rate (Siren14).
 * G.722.1 does not implement any silence handling (VAD/CNG)
 * Static variables are not used, so multiple instances can run simultaneously.
 * G.722.1 is patented by Polycom, but is royalty-free if you follow their 
 * license terms. See:
 * http://www.polycom.com/company/about_us/technology/siren14_g7221c/faq.html
 *
 * Initial development by: Ted Szoczei, Nimajin Software Consulting, 09-12-09
 * Portions developed by: Robert Jongbloed, Vox Lucida Pty. Ltd.
 * Major rewrite to use libg722_1 and support G.722.1 Annex C 10-Apr-2014
 *
 ****************************************************************************/

#define _CRT_NONSTDC_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1

#ifndef PLUGIN_CODEC_DLL_EXPORTS
#include "opal/plugin-config.h"
#endif

#include "opal/opalplugin.h"

#include "g722_1.h"
#include "g7221.h"

#ifdef strcasecmp
#undef strcasecmp
#endif



/////////////////////////////////////////////////////////////////////////////

static const char G7221Description[] = "ITU-T G.722.1 - 7/14kHz Audio Codec";

PLUGINCODEC_LICENSE(
  "Ted Szoczei, Nimajin Software Consulting",                  // source code author
  "1.0",                                                       // source code version
  "ted.szoczei@nimajin.com",                                   // source code email
  NULL,                                                        // source code URL
  "Copyright (c) 2009 Nimajin Software Consulting",            // source code copyright
  "None",                                                      // source code license
  PluginCodec_License_None,                                    // source code license
    
  G7221Description,                                            // codec description
  "Polycom, Inc.",                                             // codec author
  "2.1  2008-06-26",				                           // codec version
  NULL,                                                        // codec email
  "http://www.itu.int/rec/T-REC-G.722.1/e",                    // codec URL
  "(c) 2005 Polycom, Inc. All rights reserved.",               // codec copyright information
  "ITU-T General Public License (G.191)",                      // codec license
  PluginCodec_License_NoRoyalties                              // codec license code
);


static void * G7221EncoderCreate(const struct PluginCodec_Definition * defn)
{
  return g722_1_encode_init(NULL, defn->bitsPerSec, defn->sampleRate);
}


static void G7221EncoderDestroy (const struct PluginCodec_Definition * codec, void * context)
{
  if (context != NULL)
    g722_1_encode_release((g722_1_encode_state_t *)context);
}


static int G7221Encode(const struct PluginCodec_Definition * codec, 
                                                      void * context,
                                                const void * fromPtr, 
                                                  unsigned * fromLen,
                                                      void * toPtr,         
                                                  unsigned * toLen,
                                              unsigned int * flag)
{
  if (context == NULL)
    return 0;

  *toLen = g722_1_encode((g722_1_encode_state_t *)context, toPtr, (const int16_t *)fromPtr, *fromLen / 2);
  return 1;
}


static void * G7221DecoderCreate(const struct PluginCodec_Definition * defn)
{
  return g722_1_decode_init(NULL, defn->bitsPerSec, defn->sampleRate);
}


static void G7221DecoderDestroy (const struct PluginCodec_Definition * codec, void * context)
{
  g722_1_decode_release((g722_1_decode_state_t *)context);
}


static int G7221Decode(const struct PluginCodec_Definition * codec, 
                                                      void * context,
                                                const void * fromPtr, 
                                                  unsigned * fromLen,
                                                      void * toPtr,         
                                                  unsigned * toLen,
                                              unsigned int * flag)
{
  if (context == NULL)
    return 0;

  *toLen = g722_1_decode((g722_1_decode_state_t *)context, (int16_t *)toPtr, fromPtr, *fromLen)*2;
  return 1;
}


/////////////////////////////////////////////////////////////////////////////

// bitrate is a required SDP parameter in RFC 3047/5577

static struct PluginCodec_Option BitRateOption24k =
{
  PluginCodec_IntegerOption,        // PluginCodec_OptionTypes
  G7221BitRateOptionName,           // Generic (human readable) option name
  1,                                // Read Only flag
  PluginCodec_EqualMerge,           // Merge mode
  "24000",                          // Initial value
  G7221BitRateFMTPName,             // SIP/SDP FMTP name
  "0",                              // SIP/SDP FMTP default value (option not included in FMTP if have this value)
  0,                                // H.245 Generic Capability number and scope bits
  "24000",                          // Minimum value (enum values separated by ':')
  "48000"                           // Maximum value
};

static struct PluginCodec_Option BitRateOption32k =
{
  PluginCodec_IntegerOption,        // PluginCodec_OptionTypes
  G7221BitRateOptionName,           // Generic (human readable) option name
  1,                                // Read Only flag
  PluginCodec_EqualMerge,           // Merge mode
  "32000",                          // Initial value
  G7221BitRateFMTPName,             // SIP/SDP FMTP name
  "0",                              // SIP/SDP FMTP default value (option not included in FMTP if have this value)
  0,                                // H.245 Generic Capability number and scope bits
  "24000",                          // Minimum value (enum values separated by ':')
  "48000"                           // Maximum value
};

static struct PluginCodec_Option BitRateOption48k =
{
  PluginCodec_IntegerOption,        // PluginCodec_OptionTypes
  G7221BitRateOptionName,           // Generic (human readable) option name
  1,                                // Read Only flag
  PluginCodec_EqualMerge,           // Merge mode
  "48000",                          // Initial value
  G7221BitRateFMTPName,             // SIP/SDP FMTP name
  "0",                              // SIP/SDP FMTP default value (option not included in FMTP if have this value)
  0,                                // H.245 Generic Capability number and scope bits
  "24000",                          // Minimum value (enum values separated by ':')
  "48000"                           // Maximum value
};

static struct PluginCodec_Option RxFramesPerPacket =
{
  PluginCodec_IntegerOption,        // PluginCodec_OptionTypes
  PLUGINCODEC_OPTION_RX_FRAMES_PER_PACKET, // Generic (human readable) option name
  0,                                // Read Only flag
  PluginCodec_MinMerge,             // Merge mode
  "2",                              // Initial value
  NULL,                             // SIP/SDP FMTP name
  NULL,                             // SIP/SDP FMTP default value (option not included in FMTP if have this value)
  G7221_H241_RxFramesPerPacket,     // H.245 Generic Capability number and scope bits
  "1",                              // Minimum value (enum values separated by ':')
  "8"                               // Maximum value
};

static struct PluginCodec_Option const * const OptionTable24k[] =
{
  &BitRateOption24k,
  &RxFramesPerPacket,
  NULL
};

static struct PluginCodec_Option const * const OptionTable32k[] =
{
  &BitRateOption32k,
  &RxFramesPerPacket,
  NULL
};


static struct PluginCodec_Option const * const OptionTable48k[] =
{
  &BitRateOption48k,
  &RxFramesPerPacket,
  NULL
};


static int get_codec_options (const struct PluginCodec_Definition * defn,
                                                             void * context, 
                                                       const char * name,
                                                             void * parm,
                                                         unsigned * parmLen)
{
  if (parm == NULL || parmLen == NULL || *parmLen != sizeof(struct PluginCodec_Option **))
    return 0;

  switch (defn->sampleRate) {
    case 16000 :
      switch (defn->bitsPerSec) {
        case G7221_24K_BIT_RATE :
          *(struct PluginCodec_Option const * const * *)parm = OptionTable24k;
          break;
        case G7221_32K_BIT_RATE :
          *(struct PluginCodec_Option const * const * *)parm = OptionTable32k;
          break;
        default :
          return 0;
      }
      break;
    case 32000 :
      switch (defn->bitsPerSec) {
        case G7221C_24K_BIT_RATE :
          *(struct PluginCodec_Option const * const * *)parm = OptionTable24k;
          break;
        case G7221C_32K_BIT_RATE :
          *(struct PluginCodec_Option const * const * *)parm = OptionTable32k;
          break;
        case G7221C_48K_BIT_RATE :
          *(struct PluginCodec_Option const * const * *)parm = OptionTable48k;
          break;
        default :
          return 0;
      }
      break;
    default :
      return 0;
  }

  *parmLen = 0;
  return 1;
}


// Options are read-only, so set_codec_options not implemented
// get_codec_options returns pointers to statics, and toCustomized and 
// toNormalized are not implemented, so free_codec_options is not necessary

static struct PluginCodec_ControlDefn G7221Controls[] =
{
  { PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS, get_codec_options },
  { NULL }
};


/////////////////////////////////////////////////////////////////////////////

#define CAPABILITY(type) \
  static const struct PluginCodec_H323GenericCodecData type##_Capability = { type##_OID, type##_BIT_RATE }

CAPABILITY(G7221_24K);
CAPABILITY(G7221_32K);
CAPABILITY(G7221C_24K);
CAPABILITY(G7221C_32K);
CAPABILITY(G7221C_48K);


/////////////////////////////////////////////////////////////////////////////

#define G7221_PLUGINCODEC_AUDIO_CODEC(type) \
  PLUGINCODEC_AUDIO_CODEC( \
    type##_FormatName, \
    G7221EncodingName, \
    G7221Description, \
    type##_SAMPLE_RATE, \
    type##_BIT_RATE, \
    type##_SAMPLE_RATE*G7221_FRAME_MS/1000, \
    1,1, \
    0, \
    0, \
    PluginCodec_H323Codec_generic, &type##_Capability, \
    G7221EncoderCreate, G7221EncoderDestroy, G7221Encode, \
    G7221DecoderCreate, G7221DecoderDestroy, G7221Decode, \
    G7221Controls \
  )

static struct PluginCodec_Definition G7221CodecDefn[] =
{
  G7221_PLUGINCODEC_AUDIO_CODEC(G7221_24K),
  G7221_PLUGINCODEC_AUDIO_CODEC(G7221_32K),
  G7221_PLUGINCODEC_AUDIO_CODEC(G7221C_24K),
  G7221_PLUGINCODEC_AUDIO_CODEC(G7221C_32K),
  G7221_PLUGINCODEC_AUDIO_CODEC(G7221C_48K)
};


PLUGIN_CODEC_IMPLEMENT_ALL(G7221, G7221CodecDefn, PLUGIN_CODEC_VERSION_OPTIONS)


/////////////////////////////////////////////////////////////////////////////
