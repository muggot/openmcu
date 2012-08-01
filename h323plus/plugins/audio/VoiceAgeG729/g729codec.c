/*
 * Voice Age G.729A codec for OpenH323/OPAL
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
 * $Log: g729codec.c,v $
 * Revision 1.1  2007/10/22 07:41:08  shorne
 * *** empty log message ***
 *
 * Revision 1.4  2007/10/09 10:30:17  rjongbloed
 * Cosmetic change.
 *
 * Revision 1.3  2006/10/26 08:39:25  rjongbloed
 * Updated to latest Voice Age download library
 *
 * Revision 1.2  2006/08/01 13:06:53  rjongbloed
 * Added a raft of unvalidated audio codecs from OpenH323 tree
 *
 * Revision 1.1.2.1  2006/07/22 13:42:49  rjongbloed
 * Added more plug ins
 *
 * Revision 1.1.2.1  2006/05/08 13:49:58  rjongbloed
 * Imported all the audio codec plug ins from OpenH323
 *
 * Revision 1.2  2004/06/17 22:04:56  csoutheren
 * Changed codec version number to be sensible rather than string $Ver$
 *
 * Revision 1.1  2004/05/02 08:59:56  rjongbloed
 * Added VoiceAge G.729 codec plug in.
 *
 */

#include <codec/opalplugin.h>

PLUGIN_CODEC_IMPLEMENT("VoiceAgeG729")

// All of PWLib/OpenH323 use MSVCRT.LIB or MSVCRTD.LIB, but vag729a.lib uses
// libcmt.lib, so we need to tell the linker to ignore it, can't have two
// Run Time libraries!
#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")

#include <stdlib.h>
#include <malloc.h>
#include "va_g729/va_g729.h"

#define SAMPLES_PER_FRAME    L_FRAME
#define BYTES_PER_FRAME      L_FRAME_COMPRESSED
#define MICROSECONDSPERFRAME 10000
#define BITS_PER_SECOND      8000

/////////////////////////////////////////////////////////////////////////////

static int encoderInUse = 0;
static int decoderInUse = 0;

static void * create_encoder(const struct PluginCodec_Definition * codec)
{
  //if (encoderInUse)
  //  return NULL;

  va_g729a_init_encoder();
  encoderInUse = 1;

  return (void *)1;
}

static int codec_encoder(const struct PluginCodec_Definition * codec, 
                                           void * context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  //if (!encoderInUse)
  //  return 0;

  if (*fromLen < SAMPLES_PER_FRAME*2 || *toLen < L_FRAME_COMPRESSED)
    return 0;

  va_g729a_encoder((short *)from, (unsigned char *)to);

  *fromLen = SAMPLES_PER_FRAME*2;
  *toLen   = L_FRAME_COMPRESSED;

  return 1; 
}

static void destroy_encoder(const struct PluginCodec_Definition * codec, void * context)
{
  encoderInUse = 0;
}

static void * create_decoder(const struct PluginCodec_Definition * codec)
{
  //if (decoderInUse)
  //  return NULL;

  decoderInUse = 1;

  va_g729a_init_decoder();

  return (void *)1;
}

static int codec_decoder(const struct PluginCodec_Definition * codec, 
                                           void * context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  //if (!decoderInUse)
  //  return 0;

  if (*fromLen < L_FRAME_COMPRESSED || *toLen < SAMPLES_PER_FRAME*2)
    return 0;

  va_g729a_decoder((unsigned char *)from, (short *)to, 0);

  *fromLen = L_FRAME_COMPRESSED;
  *toLen   = SAMPLES_PER_FRAME * 2;

  return 1;
}

static void destroy_decoder(const struct PluginCodec_Definition * codec, void * context)
{
  decoderInUse = 0;
}


/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_information licenseInfo = {
  1073619586,                              // timestamp = Fri 09 Jan 2004 03:39:46 AM UTC = 

  "Craig Southeren, Post Increment",                           // source code author
  "1.0",                                                       // source code version
  "craigs@postincrement.com",                                  // source code email
  "http://www.postincrement.com",                              // source code URL
  "Copyright (C) 2004 by Post Increment, All Rights Reserved", // source code copyright
  "MPL 1.0",                                                   // source code license
  PluginCodec_License_MPL,                                     // source code license

  "G.729A",                                                    // codec description
  "VoiceAge Corporation",                                      // codec author
  NULL,                                                        // codec version
  NULL,                                                        // codec email
  "http://www.voiceage.com",                                   // codec URL
  "Copyright (C) 1995-2001 VoiceAge Corporation. All Rights Reserved",  // codec copyright information
  "NULL",                                                      // codec license
  PluginCodec_License_ResearchAndDevelopmentUseOnly            // codec license code
};

static const char L16Desc[]  = { "L16" };

static const char g729Descr[]  = { "VoiceAge G729" };
static const char g729ADescr[] = { "VoiceAge G729A" };

static const char g729MediaFmt[]  = { "G.729" };
static const char g729AMediaFmt[] = { "G.729A" };

static const char ianaName[]   = { "G729" };
#define IANACODE  18

static struct PluginCodec_Definition g729CodecDefn[4] = {

{ 
  // encoder
  PLUGIN_CODEC_VERSION,               // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeExplicit,        // explicit RTP type

  g729Descr,                          // text decription
  L16Desc,
  g729MediaFmt,

  NULL,                               // user data

  8000,                               // samples per second
  BITS_PER_SECOND,                    // raw bits per second
  MICROSECONDSPERFRAME,               // microseconds per frame
  SAMPLES_PER_FRAME,                  // samples per frame
  10,                                 // bytes per frame
  6,                                  // recommended number of frames per packet
  24,                                 // maximum number of frames per packet
  IANACODE,                           // IANA RTP payload code
  ianaName,                           // RTP Payload name

  create_encoder,                     // create codec function
  destroy_encoder,                    // destroy codec
  codec_encoder,                      // encode/decode
  NULL,                                // codec controls

  PluginCodec_H323AudioCodec_g729,    // h323CapabilityType
  NULL                                // h323CapabilityData
},

{ 
  // decoder
  PLUGIN_CODEC_VERSION,               // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeExplicit,        // explicit RTP type

  g729Descr,                          // text decription
  g729MediaFmt,
  L16Desc,

  NULL,                               // user data

  8000,                               // samples per second
  BITS_PER_SECOND,                    // raw bits per second
  MICROSECONDSPERFRAME,               // microseconds per frame
  SAMPLES_PER_FRAME,                  // samples per frame
  BYTES_PER_FRAME,                    // bytes per frame
  6,                                  // recommended number of frames per packet
  24,                                 // maximum number of frames per packet
  IANACODE,                           // IANA RTP payload code
  ianaName,                           // RTP Payload name

  create_decoder,                     // create codec function
  destroy_decoder,                    // destroy codec
  codec_decoder,                      // encode/decode
  NULL,                                // codec controls

  PluginCodec_H323AudioCodec_g729,    // h323CapabilityType 
  NULL                                // h323CapabilityData
},

{ 
  // encoder
  PLUGIN_CODEC_VERSION,                   // codec API version
  &licenseInfo,                           // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeExplicit,        // explicit RTP type

  g729ADescr,                          // text decription
  L16Desc,
  g729AMediaFmt,

  NULL,                               // user data

  8000,                                   // samples per second
  BITS_PER_SECOND,                        // raw bits per second
  MICROSECONDSPERFRAME,                   // microseconds per frame
  SAMPLES_PER_FRAME,                      // samples per frame
  BYTES_PER_FRAME,                        // bytes per frame
  6,                                      // recommended number of frames per packet
  24,                                     // maximum number of frames per packet
  IANACODE,                           // IANA RTP payload code
  ianaName,                           // RTP Payload name

  create_encoder,                         // create codec function
  destroy_encoder,                        // destroy codec
  codec_encoder,                          // encode/decode
  NULL,                                // codec controls

  PluginCodec_H323AudioCodec_g729AnnexA,  // h323CapabilityType
  NULL                                    // h323CapabilityData
},

{ 
  // decoder
  PLUGIN_CODEC_VERSION,                   // codec API version
  &licenseInfo,                           // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeExplicit,        // explicit RTP type

  g729ADescr,                          // text decription
  g729AMediaFmt,
  L16Desc,

  NULL,                               // user data

  8000,                                   // samples per second
  BITS_PER_SECOND,                        // raw bits per second
  MICROSECONDSPERFRAME,                   // microseconds per frame
  SAMPLES_PER_FRAME,                      // samples per frame
  BYTES_PER_FRAME,                        // bytes per frame
  6,                                      // recommended number of frames per packet
  24,                                     // maximum number of frames per packet
  IANACODE,                           // IANA RTP payload code
  ianaName,                           // RTP Payload name

  create_decoder,                         // create codec function
  destroy_decoder,                        // destroy codec
  codec_decoder,                          // encode/decode
  NULL,                                // codec controls

  PluginCodec_H323AudioCodec_g729AnnexA,  // h323CapabilityType 
  NULL                                    // h323CapabilityData
}

};

#define NUM_DEFNS   (sizeof(g729CodecDefn) / sizeof(struct PluginCodec_Definition))

/////////////////////////////////////////////////////////////////////////////

PLUGIN_CODEC_DLL_API struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned version)
{
  *count = NUM_DEFNS;
  return g729CodecDefn;
}
