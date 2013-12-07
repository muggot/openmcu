/*
 * G.726 plugin codec for OpenH323/OPAL
 *
 * Copyright (C) 2004 Post Increment, All Rights Reserved
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
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: g726codec.c,v $
 * Revision 1.1  2007/10/22 07:40:42  shorne
 * *** empty log message ***
 *
 * Revision 1.5  2006/08/28 01:37:36  csoutheren
 * Normalised case of SDP names
 *
 * Revision 1.4  2006/08/28 01:19:22  csoutheren
 * Changed payload types and SDP names
 *
 * Revision 1.3  2006/08/28 01:17:15  csoutheren
 * Changed payload types and SDP names
 *
 * Revision 1.2  2006/08/01 13:06:48  rjongbloed
 * Added a raft of unvalidated audio codecs from OpenH323 tree
 *
 * Revision 1.1.2.1  2006/07/22 13:42:48  rjongbloed
 * Added more plug ins
 *
 * Revision 1.1.2.1  2006/05/08 13:49:57  rjongbloed
 * Imported all the audio codec plug ins from OpenH323
 *
 * Revision 1.12  2005/07/15 10:08:58  rogerhardiman
 * Fix SF bug 1237507. Windows uses malloc.h. Linux and FreeBSD uses stdlib.h
 * Wrap #include with _WIN32 to be consistent with malloc.h in pwlib.
 *
 * Revision 1.11  2005/03/19 03:22:11  csoutheren
 * Removed warnings from gcc snapshot 4.1-20050313
 *
 * Revision 1.10  2004/12/20 23:17:44  csoutheren
 * Added stdlib.h to all plugins to keep FreeBSD happy
 * Thanks to Kevin Oberman
 *
 * Revision 1.9  2004/06/17 22:04:40  csoutheren
 * Changed codec version number to be sensible rather than string $Ver$
 *
 * Revision 1.8  2004/05/27 10:52:10  csoutheren
 * Changed samples per frame to 8, thanks to David Parr
 *
 * Revision 1.7  2004/05/26 23:41:44  csoutheren
 * Fixed problem with incorrect types on encode and decode functions
 *
 * Revision 1.6  2004/05/12 12:32:44  csoutheren
 * More name fixes
 *
 * Revision 1.5  2004/05/12 12:09:59  csoutheren
 * Fixed accidental use of dynamic RTP types
 *
 * Revision 1.4  2004/05/10 11:57:06  csoutheren
 * Changed function to be static
 *
 * Revision 1.3  2004/05/10 11:36:29  csoutheren
 * Compiled under Windows
 *
 * Revision 1.2  2004/05/10 11:20:59  csoutheren
 * Compiled under Windows
 *
 * Revision 1.1  2004/05/09 14:58:34  csoutheren
 * Initial version - not yet finished
 *
 */

#include "codec/opalplugin.h"

PLUGIN_CODEC_IMPLEMENT("G.726")

#include <stdlib.h>
#ifdef _WIN32
#include <malloc.h>
#endif

#include "g726/g72x.h"

#define SAMPLES_PER_FRAME       8

#define MAX_FRAMES_PER_PACKET   240
#define PREF_FRAMES_PER_PACKET  80

#define PAYLOAD_CODE            0  // used to be 2, uses dynamic payload type (RFC 3551)

/////////////////////////////////////////////////////////////////////////////

static void * create_codec(const struct PluginCodec_Definition * codec)
{
  struct g726_state_s * g726 = malloc(sizeof (struct g726_state_s));
  g726_init_state(g726);
  return g726;
}

static void destroy_codec(const struct PluginCodec_Definition * codec, void * _context)
{
  free(_context);
}

/////////////////////////////////////////////////////////////////////////////

#define define_encoder(func, coder) \
static int func(const struct PluginCodec_Definition * codec, \
                                           void * _context, \
                                     const void * from, \
                                       unsigned * fromLen, \
                                           void * to, \
                                       unsigned * toLen, \
                                   unsigned int * flag) \
{ \
  int sample = *(short *)from; \
  *(int *)to = coder(sample, AUDIO_ENCODING_LINEAR, (struct g726_state_s *)_context); \
  return 1; \
} \

#define define_decoder(func, coder) \
static int func(const struct PluginCodec_Definition * codec, \
                                           void * _context, \
                                     const void * from, \
                                       unsigned * fromLen, \
                                           void * to, \
                                       unsigned * toLen, \
                                   unsigned int * flag) \
{ \
  int sample = coder(*(int *)from, AUDIO_ENCODING_LINEAR, (struct g726_state_s *)_context); \
  *(short *)to = (short)sample; \
  return 1; \
} \

define_encoder(encoder_40, g726_40_encoder)
define_decoder(decoder_40, g726_40_decoder)

define_encoder(encoder_32, g726_32_encoder)
define_decoder(decoder_32, g726_32_decoder)

define_encoder(encoder_24, g726_24_encoder)
define_decoder(decoder_24, g726_24_decoder)

define_encoder(encoder_16, g726_16_encoder)
define_decoder(decoder_16, g726_16_decoder)

/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_information licenseInfo = {
  1084181196,                              // timestamp = Mon 10 May 2004 09:26:36 AM UTC

  "Craig Southeren, Post Increment",                           // source code author
  "1.0",                                                       // source code version
  "craigs@postincrement.com",                                  // source code email
  "http://www.postincrement.com",                              // source code URL
  "Copyright (C) 2004 by Post Increment, All Rights Reserved", // source code copyright
  "MPL 1.0",                                                   // source code license
  PluginCodec_License_MPL,                                     // source code license
  
  "CCITT Recommendation G.721",                                // codec description
  "Sun Microsystems, Inc.",                                    // codec author
  NULL,                                                        // codec version
  NULL,                                                        // codec email
  NULL,                                                        // codec URL
  "This source code is a product of Sun Microsystems, Inc. and is provided\n"
  "for unrestricted use.  Users may copy or modify this source code without charge",  // codec copyright information
  NULL,                                                        // codec license
  PluginCodec_Licence_None                                     // codec license code
};

/////////////////////////////////////////////////////////////////////////////

static const char L16Desc[]  = { "L16" };

static const char sdpG726_16[]  = { "G726-16" };
static const char sdpG726_24[]  = { "G726-24" };
static const char sdpG726_32[]  = { "G726-32" };
static const char sdpG726_40[]  = { "G726-40" };

static const char g726_40[] = "G.726-40k";
static const char g726_32[] = "G.726-32k";
static const char g726_24[] = "G.726-24k";
static const char g726_16[] = "G.726-16k";

/////////////////////////////////////////////////////////////////////////////

#define	EQUIVALENCE_COUNTRY_CODE            9
#define	EQUIVALENCE_EXTENSION_CODE          0
#define	EQUIVALENCE_MANUFACTURER_CODE       61


static struct PluginCodec_H323NonStandardCodecData g726_40_Cap =
{
  NULL, 
  EQUIVALENCE_COUNTRY_CODE, 
  EQUIVALENCE_EXTENSION_CODE, 
  EQUIVALENCE_MANUFACTURER_CODE,
  (const unsigned char *)g726_40, sizeof(g726_40)-1,
  NULL
};

static struct PluginCodec_H323NonStandardCodecData g726_32_Cap =
{
  NULL, 
  EQUIVALENCE_COUNTRY_CODE, 
  EQUIVALENCE_EXTENSION_CODE, 
  EQUIVALENCE_MANUFACTURER_CODE,
  (const unsigned char *)g726_32, sizeof(g726_32)-1,
  NULL
};

static struct PluginCodec_H323NonStandardCodecData g726_24_Cap =
{
  NULL, 
  EQUIVALENCE_COUNTRY_CODE, 
  EQUIVALENCE_EXTENSION_CODE, 
  EQUIVALENCE_MANUFACTURER_CODE,
  (const unsigned char *)g726_24, sizeof(g726_24)-1,
  NULL
};

static struct PluginCodec_H323NonStandardCodecData g726_16_Cap =
{
  NULL, 
  EQUIVALENCE_COUNTRY_CODE, 
  EQUIVALENCE_EXTENSION_CODE, 
  EQUIVALENCE_MANUFACTURER_CODE,
  (const unsigned char *)g726_16, sizeof(g726_16)-1,
  NULL
};

static struct PluginCodec_Definition g726CodecDefn[] = {

{ 
  // encoder 40k
  PLUGIN_CODEC_VERSION,                 // codec API version
  &licenseInfo,                         // license information

  PluginCodec_MediaTypeAudioStreamed |  // audio codec
  (5 << PluginCodec_BitsPerSamplePos) | // bits per sample
  PluginCodec_InputTypeRaw |            // raw input data
  PluginCodec_OutputTypeRaw |           // raw output data
  PluginCodec_RTPTypeDynamic,           // dynamic RTP type

  g726_40,                              // text decription
  L16Desc,                              // source format
  g726_40,                              // destination format

  0,                                    // user data

  8000,                                 // samples per second
  40000,                                // raw bits per second
  1000,                                 // nanoseconds per frame
  SAMPLES_PER_FRAME,                    // samples per frame
  5,                                    // bytes per frame
  PREF_FRAMES_PER_PACKET,               // recommended number of frames per packet
  MAX_FRAMES_PER_PACKET,                // maximum number of frames per packe
  PAYLOAD_CODE,                         // IANA RTP payload code
  sdpG726_40,                           // RTP payload name

  create_codec,                         // create codec function
  destroy_codec,                        // destroy codec
  encoder_40,                           // encode/decode
  NULL,                                 // codec controls

  PluginCodec_H323Codec_nonStandard,    // h323CapabilityType 
  &g726_40_Cap                          // h323CapabilityData
},

{ 
  // decoder 40k
  PLUGIN_CODEC_VERSION,                 // codec API version
  &licenseInfo,                         // license information

  PluginCodec_MediaTypeAudioStreamed |  // audio codec
  (5 << PluginCodec_BitsPerSamplePos) | // bits per sample
  PluginCodec_InputTypeRaw |            // raw input data
  PluginCodec_OutputTypeRaw |           // raw output data
  PluginCodec_RTPTypeDynamic,           // dynamic RTP type

  g726_40,                              // text decription
  g726_40,                              // source format
  L16Desc,                              // destination format

  0,                                    // user data

  8000,                                 // samples per second
  40000,                                // raw bits per second
  1000,                                 // nanoseconds per frame
  SAMPLES_PER_FRAME,                    // samples per frame
  5,                                    // bytes per frame
  PREF_FRAMES_PER_PACKET,               // recommended number of frames per packet
  MAX_FRAMES_PER_PACKET,                // maximum number of frames per packe
  PAYLOAD_CODE,                         // IANA RTP payload code
  sdpG726_40,                           // RTP payload name

  create_codec,                         // create codec function
  destroy_codec,                        // destroy codec
  decoder_40,                           // encode/decode
  NULL,                                 // codec controls

  PluginCodec_H323Codec_nonStandard,    // h323CapabilityType 
  &g726_40_Cap                          // h323CapabilityData
},

////////////////////////////////////

{
  // encoder 32k
  PLUGIN_CODEC_VERSION,                 // codec API version
  &licenseInfo,                         // license information

  PluginCodec_MediaTypeAudioStreamed |  // audio codec
  (4 << PluginCodec_BitsPerSamplePos) | // bits per sample
  PluginCodec_InputTypeRaw |            // raw input data
  PluginCodec_OutputTypeRaw |           // raw output data
  PluginCodec_RTPTypeDynamic,           // dynamic RTP type

  g726_32,                              // text decription
  L16Desc,                              // source format
  g726_32,                              // destination format

  0,                                    // user data

  8000,                                 // samples per second
  32000,                                // raw bits per second
  1000,                                 // nanoseconds per frame
  SAMPLES_PER_FRAME,                    // samples per frame
  4,                                    // bytes per frame
  PREF_FRAMES_PER_PACKET,               // recommended number of frames per packet
  MAX_FRAMES_PER_PACKET,                // maximum number of frames per packe
  PAYLOAD_CODE,                         // IANA RTP payload code
  sdpG726_32,                           // RTP payload name

  create_codec,                         // create codec function
  destroy_codec,                        // destroy codec
  encoder_32,                           // encode/decode
  NULL,                                 // codec controls

  PluginCodec_H323Codec_nonStandard,    // h323CapabilityType 
  &g726_32_Cap                          // h323CapabilityData
},

{ 
  // decoder 32k
  PLUGIN_CODEC_VERSION,                 // codec API version
  &licenseInfo,                         // license information

  PluginCodec_MediaTypeAudioStreamed |  // audio codec
  (4 << PluginCodec_BitsPerSamplePos) | // bits per sample
  PluginCodec_InputTypeRaw |            // raw input data
  PluginCodec_OutputTypeRaw |           // raw output data
  PluginCodec_RTPTypeDynamic,           // dynamic RTP type

  g726_32,                              // text decription
  g726_32,                              // source format
  L16Desc,                              // destination format

  0,                                    // user data

  8000,                                 // samples per second
  32000,                                // raw bits per second
  1000,                                 // nanoseconds per frame
  SAMPLES_PER_FRAME,                    // samples per frame
  4,                                    // bytes per frame
  PREF_FRAMES_PER_PACKET,               // recommended number of frames per packet
  MAX_FRAMES_PER_PACKET,                // maximum number of frames per packe
  PAYLOAD_CODE,                         // IANA RTP payload code
  sdpG726_32,                           // RTP payload name

  create_codec,                         // create codec function
  destroy_codec,                        // destroy codec
  decoder_32,                           // encode/decode
  NULL,                                 // codec controls

  PluginCodec_H323Codec_nonStandard,    // h323CapabilityType 
  &g726_32_Cap                          // h323CapabilityData
},

////////////////////////////////////

{
  // encoder 24k
  PLUGIN_CODEC_VERSION,                 // codec API version
  &licenseInfo,                         // license information

  PluginCodec_MediaTypeAudioStreamed |  // audio codec
  (3 << PluginCodec_BitsPerSamplePos) | // bits per sample
  PluginCodec_InputTypeRaw |            // raw input data
  PluginCodec_OutputTypeRaw |           // raw output data
  PluginCodec_RTPTypeDynamic,           // dynamic RTP type

  g726_24,                              // text decription
  L16Desc,                              // source format
  g726_24,                              // destination format

  0,                                    // user data

  8000,                                 // samples per second
  24000,                                // raw bits per second
  1000,                                 // nanoseconds per frame
  SAMPLES_PER_FRAME,                    // samples per frame
  3,                                    // bytes per frame
  PREF_FRAMES_PER_PACKET,               // recommended number of frames per packet
  MAX_FRAMES_PER_PACKET,                // maximum number of frames per packe
  PAYLOAD_CODE,                         // IANA RTP payload code
  sdpG726_24,                           // RTP payload name

  create_codec,                         // create codec function
  destroy_codec,                        // destroy codec
  encoder_24,                           // encode/decode
  NULL,                                 // codec controls

  PluginCodec_H323Codec_nonStandard,    // h323CapabilityType 
  &g726_24_Cap                          // h323CapabilityData
},

{ 
  // decoder 24k
  PLUGIN_CODEC_VERSION,                 // codec API version
  &licenseInfo,                         // license information

  PluginCodec_MediaTypeAudioStreamed |  // audio codec
  (3 << PluginCodec_BitsPerSamplePos) | // bits per sample
  PluginCodec_InputTypeRaw |            // raw input data
  PluginCodec_OutputTypeRaw |           // raw output data
  PluginCodec_RTPTypeDynamic,           // dynamic RTP type

  g726_24,                              // text decription
  g726_24,                              // source format
  L16Desc,                              // destination format

  0,                                    // user data

  8000,                                 // samples per second
  24000,                                // raw bits per second
  1000,                                 // nanoseconds per frame
  SAMPLES_PER_FRAME,                    // samples per frame
  3,                                    // bytes per frame
  PREF_FRAMES_PER_PACKET,               // recommended number of frames per packet
  MAX_FRAMES_PER_PACKET,                // maximum number of frames per packe
  PAYLOAD_CODE,                         // IANA RTP payload code
  sdpG726_24,                           // RTP payload name

  create_codec,                         // create codec function
  destroy_codec,                        // destroy codec
  decoder_24,                           // encode/decode
  NULL,                                 // codec controls

  PluginCodec_H323Codec_nonStandard,    // h323CapabilityType 
  &g726_24_Cap                          // h323CapabilityData
},

////////////////////////////////////

{
  // encoder 16k
  PLUGIN_CODEC_VERSION,                 // codec API version
  &licenseInfo,                         // license information

  PluginCodec_MediaTypeAudioStreamed |  // audio codec
  (2 << PluginCodec_BitsPerSamplePos) | // bits per sample
  PluginCodec_InputTypeRaw |            // raw input data
  PluginCodec_OutputTypeRaw |           // raw output data
  PluginCodec_RTPTypeDynamic,           // dynamic RTP type

  g726_16,                              // text decription
  L16Desc,                              // source format
  g726_16,                              // destination format

  0,                                    // user data

  8000,                                 // samples per second
  16000,                                // raw bits per second
  1000,                                 // nanoseconds per frame
  SAMPLES_PER_FRAME,                    // samples per frame
  2,                                    // bytes per frame
  PREF_FRAMES_PER_PACKET,               // recommended number of frames per packet
  MAX_FRAMES_PER_PACKET,                // maximum number of frames per packe
  PAYLOAD_CODE,                         // IANA RTP payload code
  sdpG726_16,                           // RTP payload name

  create_codec,                         // create codec function
  destroy_codec,                        // destroy codec
  encoder_16,                           // encode/decode
  NULL,                                 // codec controls

  PluginCodec_H323Codec_nonStandard,    // h323CapabilityType 
  &g726_16_Cap                          // h323CapabilityData
},

{ 
  // decoder 16k
  PLUGIN_CODEC_VERSION,                 // codec API version
  &licenseInfo,                         // license information

  PluginCodec_MediaTypeAudioStreamed |  // audio codec
  (2 << PluginCodec_BitsPerSamplePos) | // bits per sample
  PluginCodec_InputTypeRaw |            // raw input data
  PluginCodec_OutputTypeRaw |           // raw output data
  PluginCodec_RTPTypeDynamic,           // dynamic RTP type

  g726_16,                              // text decription
  g726_16,                              // source format
  L16Desc,                              // destination format

  0,                                    // user data

  8000,                                 // samples per second
  16000,                                // raw bits per second
  1000,                                 // nanoseconds per frame
  SAMPLES_PER_FRAME,                    // samples per frame
  2,                                    // bytes per frame
  PREF_FRAMES_PER_PACKET,               // recommended number of frames per packet
  MAX_FRAMES_PER_PACKET,                // maximum number of frames per packe
  PAYLOAD_CODE,                         // IANA RTP payload code
  sdpG726_16,                           // RTP payload name

  create_codec,                         // create codec function
  destroy_codec,                        // destroy codec
  decoder_16,                           // encode/decode
  NULL,                                 // codec controls

  PluginCodec_H323Codec_nonStandard,    // h323CapabilityType 
  &g726_16_Cap                          // h323CapabilityData
},


};

#define NUM_DEFNS   (sizeof(g726CodecDefn) / sizeof(struct PluginCodec_Definition))

/////////////////////////////////////////////////////////////////////////////

PLUGIN_CODEC_DLL_API struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned version)
{
  *count = NUM_DEFNS;
  return g726CodecDefn;
}
