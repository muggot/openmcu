/*
 * iLBC Plugin codec for OpenH323/OPAL
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
 * $Log: ilbccodec.c,v $
 * Revision 1.1  2007/10/22 07:41:12  shorne
 * *** empty log message ***
 *
 * Revision 1.5  2007/09/05 07:49:07  csoutheren
 * Set output buffers correctly and check input buffers correctly
 *
 * Revision 1.4  2007/09/05 06:22:32  rjongbloed
 * Update iLBC plug in to latest API so get FMTP parameter for mode.
 *
 * Revision 1.3  2006/08/28 01:21:54  csoutheren
 * Disable 15k for SIP
 *
 * Revision 1.2  2006/07/31 09:09:19  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.2  2006/04/08 06:09:09  rjongbloed
 * Fix correct directory for OPAL headers
 *
 * Revision 1.1.2.1  2006/04/06 01:20:05  csoutheren
 * Ported audio codec plugins from OpenH323 to OPAL
 *
 * Revision 1.8  2005/07/15 10:09:00  rogerhardiman
 * Fix SF bug 1237507. Windows uses malloc.h. Linux and FreeBSD uses stdlib.h
 * Wrap #include with _WIN32 to be consistent with malloc.h in pwlib.
 *
 * Revision 1.7  2004/12/20 23:18:01  csoutheren
 * Added stdlib.h to all plugins to keep FreeBSD happy
 * Thanks to Kevin Oberman
 *
 * Revision 1.6  2004/11/29 06:29:58  csoutheren
 * Added flag to reuse RTP payload types rather than allocaing new ones for each codec
 *  variant
 *
 * Revision 1.5  2004/06/17 22:04:57  csoutheren
 * Changed codec version number to be sensible rather than string $Ver$
 *
 * Revision 1.4  2004/04/09 12:24:19  csoutheren
 * Renamed h323plugin.h to opalplugin.h, and modified everything else
 * as required
 *
 * Revision 1.3  2004/04/04 12:43:59  csoutheren
 * Added file headers and fixd formatting
 *
 */

#include "codec/opalplugin.h"

#include <stdlib.h>
#ifdef _WIN32
  #define _CRT_SECURE_NO_DEPRECATE
  #include <malloc.h>
  #define STRCMPI  _strcmpi
#else
  #define STRCMPI  strcasecmp
#endif

#include "iLBC/iLBC_encode.h" 
#include "iLBC/iLBC_decode.h" 
#include "iLBC/iLBC_define.h" 

#define	SPEED_30MS	NO_OF_BYTES_30MS*8*8000/BLOCKL_30MS
#define	SPEED_20MS	NO_OF_BYTES_20MS*8*8000/BLOCKL_20MS


/////////////////////////////////////////////////////////////////////////////

static void * create_encoder(const struct PluginCodec_Definition * codec)
{
  struct iLBC_Enc_Inst_t_ * context = (struct iLBC_Enc_Inst_t_ *)malloc((unsigned)sizeof(struct iLBC_Enc_Inst_t_));
  initEncode(context, (codec->bitsPerSec) == SPEED_30MS ? 30 : 20); 
  return context;
}

static void * create_decoder(const struct PluginCodec_Definition * codec)
{
  struct iLBC_Dec_Inst_t_ * context = (struct iLBC_Dec_Inst_t_ *)malloc((unsigned)sizeof(struct iLBC_Dec_Inst_t_));
  initDecode(context, (codec->bitsPerSec) == SPEED_30MS ? 30 : 20, 0); 
  return context;
}

static void destroy_context(const struct PluginCodec_Definition * codec, void * context)
{
  free(context);
}

static int codec_encoder(const struct PluginCodec_Definition * codec, 
                                           void * context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  float block[BLOCKL_MAX];
  int i;

  struct iLBC_Enc_Inst_t_ * encoder = (struct iLBC_Enc_Inst_t_ *)context;
  const short * sampleBuffer = (const short *)from;

  if ((*fromLen)/2 < (unsigned)encoder->blockl)
    return 0;

  /* convert signal to float */
  for (i = 0; i < encoder->blockl; i++)
    block[i] = (float)sampleBuffer[i];

  /* do the actual encoding */
  iLBC_encode(to, block, encoder);

  // set output length
  *toLen = encoder->no_of_bytes;

  return 1; 
}

static int codec_decoder(const struct PluginCodec_Definition * codec, 
                                           void * context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  int i;
  float block[BLOCKL_MAX];

  struct iLBC_Dec_Inst_t_ * decoder = (struct iLBC_Dec_Inst_t_ *)context;
  short * sampleBuffer = (short *)to;

  if (*fromLen < (unsigned)decoder->no_of_bytes)
    return 0;

  /* do actual decoding of block */ 
  iLBC_decode(block, (unsigned char *)from, decoder, 1);

  if (*toLen < (unsigned)decoder->blockl*2)
    return 0;

  /* convert to short */     
  for (i = 0; i < decoder->blockl; i++) {
    float tmp = block[i];
    if (tmp < MIN_SAMPLE)
      tmp = MIN_SAMPLE;
    else if (tmp > MAX_SAMPLE)
      tmp = MAX_SAMPLE;
    sampleBuffer[i] = (short)tmp;
  }

  *toLen = decoder->blockl*2;

  return 1;
}

static int valid_for_h323(
      const struct PluginCodec_Definition * codec, 
      void * context , 
      const char * key, 
      void * parm , 
      unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char *))
    return 0;

  return (STRCMPI((const char *)parm, "h.323") == 0 ||
          STRCMPI((const char *)parm, "h323") == 0) ? 1 : 0;

}

static int valid_for_sip_or_h323(
      const struct PluginCodec_Definition * codec, 
      void * context , 
      const char * key, 
      void * parm , 
      unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char *))
    return 0;

  return (STRCMPI((const char *)parm, "sip") == 0 ||
          STRCMPI((const char *)parm, "h.323") == 0 ||
          STRCMPI((const char *)parm, "h323") == 0) ? 1 : 0;
}

static struct PluginCodec_Option const ilbc13k3_mode =
  { PluginCodec_IntegerOption, "Preferred Mode", 0, PluginCodec_NoMerge,  "30", "mode", NULL, 0, "0", "30" };
static struct PluginCodec_Option const ilbc15k2_mode =
  { PluginCodec_IntegerOption, "Preferred Mode", 0, PluginCodec_NoMerge,  "20", "mode", NULL, 0, "0", "20" };

static struct PluginCodec_Option const * const ilbc13k3_OptionTable[] = {
  &ilbc13k3_mode,
  NULL
};
static struct PluginCodec_Option const * const ilbc15k2_OptionTable[] = {
  &ilbc15k2_mode,
  NULL
};

static int get_codec_options(const struct PluginCodec_Definition * defn,
                                                            void * context, 
                                                      const char * name,
                                                            void * parm,
                                                        unsigned * parmLen)
{
  if (parm == NULL || parmLen == NULL || *parmLen != sizeof(struct PluginCodec_Option **))
    return 0;

  if (defn->usPerFrame == 30000)  *(struct PluginCodec_Option const * const * *)parm = ilbc13k3_OptionTable;
  else if (defn->usPerFrame == 20000)  *(struct PluginCodec_Option const * const * *)parm = ilbc15k2_OptionTable;
  return 1;
}


static int set_codec_options(const struct PluginCodec_Definition * defn,
                                                            void * context,
                                                      const char * name, 
                                                            void * parm, 
                                                        unsigned * parmLen)
{
  const char * const * option;
  struct iLBC_Enc_Inst_t_ * encoder;

  if (context == NULL || parm == NULL || parmLen == NULL || *parmLen != sizeof(const char **))
    return 0;

  encoder = (struct iLBC_Enc_Inst_t_ *)context;

//  for (option = (const char * const *)parm; *option != NULL; option += 2) {
//    if (STRCMPI(option[0], "Preferred Mode") == 0)
//      initEncode(context, atoi(option[1]) == SPEED_30MS ? 30 : 20); 
//  }

  return 1;
}


static struct PluginCodec_ControlDefn h323CoderControls[] = {
  { "valid_for_protocol", valid_for_h323 },
  { "get_codec_options",  get_codec_options },
  { "set_codec_options",  set_codec_options },
  { NULL }
};

static struct PluginCodec_ControlDefn h323AndSIPCoderControls[] = {
  { "valid_for_protocol", valid_for_sip_or_h323 },
  { "get_codec_options",  get_codec_options },
  { "set_codec_options",  set_codec_options },
  { NULL }
};


/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_information licenseInfo = {
  //1073187324,				                       // timestamp = Sun 04 Jan 2004 03:35:24 AM UTC =
  1101695533,                            // Mon 29 Nov 2004 12:32:13 PM EST

  "Craig Southeren, Post Increment",                           // source code author
  "1.1",                                                       // source code version
  "craigs@postincrement.com",                                  // source code email
  "http://www.postincrement.com",                              // source code URL
  "Copyright (C) 2004 by Post Increment, All Rights Reserved", // source code copyright
  "MPL 1.0",                                                   // source code license
  PluginCodec_License_MPL,                                     // source code license

  "iLBC (internet Low Bitrate Codec)",                         // codec description
  "Global IP Sound, Inc.",                                     // codec author
  NULL,                                                        // codec version
  "info@globalipsound.com",                                    // codec email
  "http://www.ilbcfreeware.org",                               // codec URL
  "Global IP Sound AB. Portions Copyright (C) 1999-2002, All Rights Reserved",          // codec copyright information
  "Global IP Sound iLBC Freeware Public License, IETF Version, Limited Commercial Use", // codec license
  PluginCodec_License_Freeware                                // codec license code
};

static const char L16Desc[]  = { "L16" };

static const char iLBC13k3[] = { "iLBC-13k3" };
static const char iLBC15k2[] = { "iLBC-15k2" };

static const char sdpILBC[]  = { "iLBC" };

#define	EQUIVALENCE_COUNTRY_CODE            9
#define	EQUIVALENCE_EXTENSION_CODE          0
#define	EQUIVALENCE_MANUFACTURER_CODE       61

static struct PluginCodec_H323NonStandardCodecData ilbc13k3Cap =
{
  NULL, 
  EQUIVALENCE_COUNTRY_CODE, 
  EQUIVALENCE_EXTENSION_CODE, 
  EQUIVALENCE_MANUFACTURER_CODE,
  iLBC13k3, sizeof(iLBC13k3)-1,
  NULL
};

static struct PluginCodec_H323NonStandardCodecData ilbc15k2Cap =
{
  NULL, 
  EQUIVALENCE_COUNTRY_CODE, 
  EQUIVALENCE_EXTENSION_CODE, 
  EQUIVALENCE_MANUFACTURER_CODE,
  iLBC15k2, sizeof(iLBC15k2)-1,
  NULL
};

static struct PluginCodec_Definition iLBCCodecDefn[4] = {

{ 
  // encoder
  PLUGIN_CODEC_VERSION_OPTIONS,               // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeShared |         // share RTP code 
  PluginCodec_RTPTypeDynamic,         // dynamic RTP type

  iLBC13k3,                           // text decription
  L16Desc,                            // source format
  iLBC13k3,                           // destination format

  (void *)NULL,                       // user data

  8000,                               // samples per second
  SPEED_30MS,                         // raw bits per second
  30000,                              // nanoseconds per frame
  BLOCKL_30MS,                        // samples per frame
  NO_OF_BYTES_30MS,                   // bytes per frame
  1,                                  // recommended number of frames per packet
  1,                                  // maximum number of frames per packe
  0,                                  // IANA RTP payload code
  sdpILBC,                            // RTP payload name

  create_encoder,                     // create codec function
  destroy_context,                    // destroy codec
  codec_encoder,                      // encode/decode
  h323AndSIPCoderControls,            // codec controls

  PluginCodec_H323Codec_nonStandard,  // h323CapabilityType
  &ilbc13k3Cap                        // h323CapabilityData
},

{ 
  // decoder
  PLUGIN_CODEC_VERSION_OPTIONS,               // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeShared |         // share RTP code 
  PluginCodec_RTPTypeDynamic,         // dynamic RTP type

  iLBC13k3,                           // text decription
  iLBC13k3,                           // source format
  L16Desc,                            // destination format

  (const void *)NULL,                       // user data

  8000,                               // samples per second
  SPEED_30MS,                         // raw bits per second
  30000,                              // nanoseconds per frame
  BLOCKL_30MS,                        // samples per frame
  NO_OF_BYTES_30MS,                   // bytes per frame
  1,                                  // recommended number of frames per packet
  1,                                  // maximum number of frames per packe
  0,                                  // IANA RTP payload code
  sdpILBC,                            // RTP payload name

  create_decoder,                     // create codec function
  destroy_context,                    // destroy codec
  codec_decoder,                      // encode/decode
  h323AndSIPCoderControls,            // codec controls

  PluginCodec_H323Codec_nonStandard,  // h323CapabilityType 
  &ilbc13k3Cap                        // h323CapabilityData
},

{ 
  // encoder
  PLUGIN_CODEC_VERSION_OPTIONS,               // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeShared |         // share RTP code 
  PluginCodec_RTPTypeDynamic,         // dynamic RTP type

  iLBC15k2,                           // text decription
  L16Desc,                            // source format
  iLBC15k2,                           // destination format

  (void *)NULL,                       // user data

  8000,                               // samples per second
  SPEED_20MS,                         // raw bits per second
  20000,                              // nanoseconds per frame
  BLOCKL_20MS,                        // samples per frame
  NO_OF_BYTES_20MS,                   // bytes per frame
  1,                                  // recommended number of frames per packet
  1,                                  // maximum number of frames per packe
  0,                                  // IANA RTP payload code
  sdpILBC,                            // RTP payload name

  create_encoder,                     // create codec function
  destroy_context,                    // destroy codec
  codec_encoder,                      // encode/decode
  h323CoderControls,                  // codec controls

  PluginCodec_H323Codec_nonStandard,  // h323CapabilityType 
  &ilbc15k2Cap                        // h323CapabilityData
},

{ 
  // decoder
  PLUGIN_CODEC_VERSION_OPTIONS,               // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeShared |         // share RTP code 
  PluginCodec_RTPTypeDynamic,         // dynamic RTP type

  iLBC15k2,                           // text decription
  iLBC15k2,                           // source format
  L16Desc,                            // destination format

  (void *)NULL,                       // user data

  8000,                               // samples per second
  SPEED_20MS,                         // raw bits per second
  20000,                              // nanoseconds per frame
  BLOCKL_20MS,                        // samples per frame
  NO_OF_BYTES_20MS,                   // bytes per frame
  1,                                  // recommended number of frames per packet
  1,                                  // maximum number of frames per packe
  0,                                  // IANA RTP payload code
  sdpILBC,                            // RTP payload name

  create_decoder,                     // create codec function
  destroy_context,                    // destroy codec
  codec_decoder,                      // encode/decode
  h323CoderControls,                  // codec controls

  PluginCodec_H323Codec_nonStandard,  // h323CapabilityType 
  &ilbc15k2Cap                        // h323CapabilityData
}
};


/////////////////////////////////////////////////////////////////////////////

PLUGIN_CODEC_IMPLEMENT_ALL("iLBC", iLBCCodecDefn, PLUGIN_CODEC_VERSION_OPTIONS)
