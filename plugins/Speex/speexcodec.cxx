/*
 * Speex audio codec Plugin codec for OPAL
 *
 * Copyright (C) 2013 by Andrey Burbovskiy, OpenMCU-ru Team, All Rights Reserved
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
 * The Original Code is OPAL Library.
 *
 * The Initial Developer of the Original Code is Vox Lucida Pty Ltd
 *
 * Added with funding from Requestec, Inc.
 *
 * Contributor(s):  Andrey Burbovskiy (andrewb@yandex.ru)
 *
 */

#define PTRACE(level,category,text)
#define CODEC_LOG "SPEEX"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codec/opalplugin.hpp"
#include "codec/plugin-config.h"

#include <speex/speex.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

PLUGINCODEC_LICENSE(
    "Andrey Burbovskiy",                                         // source code author
    "0.1",                                                       // source code version
    "andrewb@yandex.ru",                                         // source code email
    "http://openmcu.ru/",                                        // source code URL
    "Copyright (C) 2013 by Andrey Burbovskiy for OpenMCU-ru",    // source code copyright
    "MPL 1.0",                                                   // source code license
    PluginCodec_License_MPL,                                     // source code license

    "Speex",                                                     // codec description
    "Jean-Marc Valin, Xiph Foundation.",                         // codec author
    "1.2rc1",                                                    // codec version
    "jmvalin@jmvalin.ca",                                        // codec email
    "http://www.speex.org     ",                                 // codec URL
    "(C) 2003 Xiph.Org Foundation, All Rights Reserved",         // codec copyright information
    "Xiph BSD license",                                          // codec license
    PluginCodec_License_BSD                                      // codec license code
);

////////////////////////////////////////////////////////////////////////////////////////////////////

class CODEC { };

class AudioFormat : public PluginCodec_AudioFormat<CODEC>
{
  private:
    typedef PluginCodec_AudioFormat<CODEC> BaseClass;

  public:
    AudioFormat(const char * formatName,
    		const char * payloadName,
    		const char * description,
    		unsigned     samplesPerFrame,
    		unsigned     bytesPerFrame,
    		unsigned     sampleRate,
    		OptionsTable options)
      : BaseClass(formatName, payloadName, description, samplesPerFrame, bytesPerFrame, sampleRate, options)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Encoder : public PluginCodec<CODEC>
{
  protected:
    SpeexBits speexBits;
    void     *m_state;
    unsigned m_samplesPerFrame;
    unsigned m_bytesPerFrame;
    unsigned m_sampleRate;
    unsigned m_signalSampleSize;
    unsigned m_vbr;
    unsigned m_mode;
    int      ret;

  public:
    Encoder(const PluginCodec_Definition * defn)
      : PluginCodec<CODEC>(defn)
      , m_samplesPerFrame(m_definition->parm.audio.samplesPerFrame)
      , m_bytesPerFrame(m_definition->parm.audio.bytesPerFrame)
      , m_sampleRate(m_definition->sampleRate)
      , m_signalSampleSize(sizeof(short))
    {
    }

    ~Encoder()
    {
      speex_encoder_destroy(m_state); 
    }

    virtual bool Construct()
    {
      GetInitOptions();
      if (m_sampleRate == 8000)
        m_state = speex_encoder_init(&speex_nb_mode);
      else if(m_sampleRate == 16000)
        m_state = speex_encoder_init(&speex_wb_mode);
      else if(m_sampleRate == 32000)
        m_state = speex_encoder_init(&speex_uwb_mode);

      speex_encoder_ctl(m_state, SPEEX_SET_VBR, &m_vbr);
      speex_encoder_ctl(m_state, SPEEX_SET_MODE, &m_mode);
      speex_encoder_ctl(m_state, SPEEX_SET_SAMPLING_RATE, &m_sampleRate);

      return true;
    }

    virtual void GetInitOptions()
    {
      for (PluginCodec_Option ** options = (PluginCodec_Option **)((MediaFormat *)m_definition->userData)->GetOptionsTable(); *options != NULL; ++options)
      {
        if (strcmp((*options)->m_name, "vbr") == 0)
        {
          if(strcmp((*options)->m_value, "on") == 0)
            m_vbr = 1;
          else
            m_vbr = 0;
        }
        if (strcmp((*options)->m_name, "mode") == 0)
          m_mode = atoi((*options)->m_value);
      }
    }

    virtual bool SetOption(const char * optionName, const char * optionValue)
    {
      if (strcasecmp(optionName, "vbr") == 0)
      {
        int tmp = 0;
        if(strcmp(optionValue, "on") == 0)
          tmp = 1;
        ret = speex_encoder_ctl(m_state, SPEEX_SET_VBR, &tmp);
        return true;
      }
      if (strcasecmp(optionName, "mode") == 0)
      {
        int tmp = atoi(optionValue);
        ret = speex_encoder_ctl(m_state, SPEEX_SET_MODE, &tmp);
        return true;
      }
      return PluginCodec<CODEC>::SetOption(optionName, optionValue);
    }

    virtual bool Transcode(const void * fromPtr,
                             unsigned & fromLen,
                                 void * toPtr,
                             unsigned & toLen,
                             unsigned & flags)
    {
      if (fromLen < m_samplesPerFrame*m_signalSampleSize)
        return false;

      speex_bits_init(&speexBits);

      speex_encode_int(m_state, (spx_int16_t *)fromPtr, &speexBits);
      speex_bits_insert_terminator(&speexBits);
      toLen = speex_bits_write(&speexBits, (char *)toPtr, toLen);

      speex_bits_destroy(&speexBits);

      return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Decoder : public PluginCodec<CODEC>
{
  protected:
    SpeexBits speexBits;
    void     *m_state;
    unsigned m_samplesPerFrame;
    unsigned m_bytesPerFrame;
    unsigned m_sampleRate;
    unsigned m_signalSampleSize;
    unsigned m_decoderChannels;
    int ret;

  public:
    Decoder(const PluginCodec_Definition * defn)
      : PluginCodec<CODEC>(defn)
      , m_samplesPerFrame(m_definition->parm.audio.samplesPerFrame)
      , m_bytesPerFrame(m_definition->parm.audio.bytesPerFrame)
      , m_sampleRate(m_definition->sampleRate)
      , m_signalSampleSize(sizeof(short))
    {
    }

    ~Decoder()
    {
      speex_decoder_destroy(m_state); 
    }

    virtual bool Construct()
    {
      if (m_sampleRate == 8000)
        m_state = speex_decoder_init(&speex_nb_mode);
      else if(m_sampleRate == 16000)
        m_state = speex_decoder_init(&speex_wb_mode);
      else if(m_sampleRate == 32000)
        m_state = speex_decoder_init(&speex_uwb_mode);
      return true;
    }

    virtual bool Transcode(const void * fromPtr,
                             unsigned & fromLen,
                                 void * toPtr,
                             unsigned & toLen,
                             unsigned & flags)
    {
      if (toLen < m_samplesPerFrame*m_signalSampleSize)
        return false;

      speex_bits_init(&speexBits);

      if (flags&PluginCodec_CoderSilenceFrame) {
        ret = speex_decode_int(m_state, NULL, (spx_int16_t *)toPtr);
      } else {
        speex_bits_read_from(&speexBits, (char *)fromPtr, fromLen);
        ret = speex_decode_int(m_state, &speexBits, (spx_int16_t *)toPtr);
      }

      speex_bits_destroy(&speexBits);

      if(ret != 0)
        return false;

      return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define SPEEX_8K_MediaFmt                           "Speex_8K"
#define SPEEX_8K_PayloadName                        "speex"
#define SPEEX_8K_PayloadType                         0
#define SPEEX_8K_Vbr                                "off"
#define SPEEX_8K_Mode                               "3"
static unsigned int SPEEX_8K_SampleRate             =8000;
static unsigned int SPEEX_8K_Samples                =160;
static unsigned int SPEEX_8K_MaxBytesPerFrame       =4000;
static unsigned int SPEEX_8K_FramesPerPacket        =1;

#define SPEEX_16K_MediaFmt                          "Speex_16K"
#define SPEEX_16K_PayloadName                       "speex"
#define SPEEX_16K_PayloadType                        0
#define SPEEX_16K_Vbr                               "off"
#define SPEEX_16K_Mode                              "6"
static unsigned int SPEEX_16K_SampleRate            =16000;
static unsigned int SPEEX_16K_Samples               =320;
static unsigned int SPEEX_16K_MaxBytesPerFrame      =4000;
static unsigned int SPEEX_16K_FramesPerPacket       =1;

#define SPEEX_32K_MediaFmt                          "Speex_32K"
#define SPEEX_32K_PayloadName                       "speex"
#define SPEEX_32K_PayloadType                        0
#define SPEEX_32K_Vbr                               "off"
#define SPEEX_32K_Mode                              "6"
static unsigned int SPEEX_32K_SampleRate            =32000;
static unsigned int SPEEX_32K_Samples               =640;
static unsigned int SPEEX_32K_MaxBytesPerFrame      =4000;
static unsigned int SPEEX_32K_FramesPerPacket       =1;

////////////////////////////////////////////////////////////////////////////////////////////////////

#define XIPH_COUNTRY_CODE                   181
#define XIPH_EXTENSION_CODE                 0
#define XIPH_MANUFACTURER_CODE              38

static const unsigned char SPEEX_8K_XIPH_Str[] = "speex sr=8000;mode=3;vbr=off;cng=off";
static const unsigned char SPEEX_16K_XIPH_Str[] = "speex sr=16000;mode=6;vbr=off;cng=off";
static const unsigned char SPEEX_32K_XIPH_Str[] = "speex sr=32000;mode=6;vbr=off;cng=off";

////////////////////////////////////////////////////////////////////////////////////////////////////

#define PLUGIN_CODEC(nameprefix, prefix, manuf) \
static struct PluginCodec_Option const nameprefix##prefix##_vbr = \
{ \
  PluginCodec_EnumOption,             /* Option type */ \
  "vbr",                              /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  prefix##_Vbr,                       /* Initial value */ \
  "vbr",                              /* FMTP option name */ \
  "off",                              /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "off:on"                            /* Enumeration */ \
}; \
static struct PluginCodec_Option const nameprefix##prefix##_mode = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "mode",                             /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  prefix##_Mode,                      /* Initial value */ \
  "mode",                             /* FMTP option name */ \
  "6",                                /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "0",                                /* Minimum value */ \
  "10"                                /* Maximum value */ \
}; \
static struct PluginCodec_Option const * nameprefix##prefix##_OptionTable[] = \
{ \
  &prefix##_vbr, \
  &prefix##_mode, \
  NULL \
}; \
static struct PluginCodec_H323NonStandardCodecData nameprefix##prefix##_Cap = \
{ \
  NULL, \
  manuf##_COUNTRY_CODE, manuf##_EXTENSION_CODE, manuf##_MANUFACTURER_CODE, \
  (const unsigned char *)prefix##_##manuf##_Str, sizeof(prefix##_##manuf##_Str)-1, \
  NULL \
}; \
class nameprefix##prefix##_AudioFormat : public AudioFormat \
{ \
  public: \
    nameprefix##prefix##_AudioFormat() \
      : AudioFormat( \
        #nameprefix prefix##_MediaFmt, \
        prefix##_PayloadName, \
        #nameprefix prefix##_MediaFmt, \
        prefix##_Samples, \
        prefix##_MaxBytesPerFrame, \
        prefix##_SampleRate, \
        prefix##_OptionTable) \
    { \
      m_payloadType = prefix##_PayloadType; \
      m_recommendedFramesPerPacket = prefix##_FramesPerPacket; \
      m_maxFramesPerPacket = prefix##_FramesPerPacket; \
      m_h323CapabilityType = PluginCodec_H323Codec_nonStandard; \
      m_h323CapabilityData = (PluginCodec_H323NonStandardCodecData *)&prefix##_Cap; \
    } \
}; \
static nameprefix##prefix##_AudioFormat nameprefix##prefix##_AudioFormatInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////

PLUGIN_CODEC(,SPEEX_8K, XIPH);
PLUGIN_CODEC(,SPEEX_16K, XIPH);
PLUGIN_CODEC(,SPEEX_32K, XIPH);

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition CodecDefinition[] =
{
  PLUGINCODEC_AUDIO_CODEC_CXX(SPEEX_8K_AudioFormatInfo, Encoder, Decoder)
  ,PLUGINCODEC_AUDIO_CODEC_CXX(SPEEX_16K_AudioFormatInfo, Encoder, Decoder)
  ,PLUGINCODEC_AUDIO_CODEC_CXX(SPEEX_32K_AudioFormatInfo, Encoder, Decoder)
};

PLUGIN_CODEC_IMPLEMENT_CXX(CODEC, CodecDefinition);

////////////////////////////////////////////////////////////////////////////////////////////////////
