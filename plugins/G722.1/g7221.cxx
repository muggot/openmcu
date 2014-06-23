/*
 * G.722.1 audio codec Plugin codec for OpenH323/OPAL
 *
 * Copyright (C) 2014 OpenMCU-ru Team, All Rights Reserved
 *
 * Software distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.
 *
 * The Original Code is OPAL Library.
 * The Initial Developer of the Original Code is Vox Lucida Pty Ltd
 *
 * Contributor(s):  Andrey Burbovskiy (andrewb@yandex.ru)
 *
 */

#include "../common/opalplugin.hpp"

#include "g722_1.h"


PLUGINCODEC_CONTROL_LOG_FUNCTION_DEF

////////////////////////////////////////////////////////////////////////////////////////////////////

PLUGINCODEC_LICENSE(
    "Andrey Burbovskiy",                                         // source code author
    "0.2",                                                       // source code version
    "andrewb@yandex.ru",                                         // source code email
    "http://openmcu.ru",                                         // source code URL
    "Copyright (C) 2014 OpenMCU-ru Team",                        // source code copyright
    "none",                                                      // source code license
    PluginCodec_License_None,                                    // source code license

    "ITU-T G.722.1 - 7/14kHz Audio Codec",                       // codec description
    "Polycom, Inc.",                                             // codec author
    "2.1  2008-06-26",			                         // codec version
    NULL,                                                        // codec email
    "http://www.itu.int/rec/T-REC-G.722.1/e",                    // codec URL
    "(c) 2005 Polycom, Inc. All rights reserved.",               // codec copyright information
    "ITU-T General Public License (G.191)",                      // codec license
    PluginCodec_License_NoRoyalties                              // codec license code
);

////////////////////////////////////////////////////////////////////////////////////////////////////

class CODEC { };

class AudioFormat : public PluginCodec_AudioFormat<CODEC>
{
  private:
    typedef PluginCodec_AudioFormat<CODEC> BaseClass;

  public:
    AudioFormat(const char * formatName, const char * payloadName, const char * description, unsigned samplesPerFrame, unsigned bytesPerFrame, unsigned sampleRate, OptionsTable options)
      : BaseClass(formatName, payloadName, description, samplesPerFrame, bytesPerFrame, sampleRate, options)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Encoder : public PluginCodec<CODEC>
{
  protected:
    g722_1_encode_state_t *m_state;
    const char *m_description;
    unsigned m_sampleRate;
    unsigned m_bitsPerSec;

  public:
    Encoder(const PluginCodec_Definition * defn)
      : PluginCodec<CODEC>(defn)
      , m_state(NULL)
      , m_description(m_definition->descr)
      , m_bitsPerSec(m_definition->bitsPerSec)
      , m_sampleRate(m_definition->sampleRate)
    {
    }

    ~Encoder()
    {
      if(m_state != NULL)
        g722_1_encode_release(m_state);
    }

    virtual bool Construct()
    {
      m_state = g722_1_encode_init(NULL, m_bitsPerSec, m_sampleRate);
      if(m_state == NULL)
      {
        PTRACE(1, m_description, "error: g722_1_encode_init(m_state, " << m_bitsPerSec << ", " <<  m_sampleRate << ")");
        return false;
      }
      return true;
    }

    virtual bool SetOption(const char * optionName, const char * optionValue)
    {
      if(strcmp(optionName, PLUGINCODEC_OPTION_MAX_BIT_RATE) == 0)
      {
        return true;
      }
      if(strcmp(optionName, "Generic Parameter 2") == 0)
      {
        unsigned value = atoi(optionValue);
        unsigned bitsPerSec = m_bitsPerSec;
        if(value == 64)                      bitsPerSec = 24000;
        else if(value == 96 || value == 32)  bitsPerSec = 32000;
        else if(value == 112 || value == 16) bitsPerSec = 48000;
        if(bitsPerSec != m_bitsPerSec)
        {
          if(g722_1_encode_set_rate(m_state, bitsPerSec) == -1)
          {
            PTRACE(1, m_description, "error: g722_1_encode_set_rate(m_state, " << bitsPerSec << ")");
            return false;
          }
          m_bitsPerSec = bitsPerSec;
        }
        return true;
      }
      return PluginCodec<CODEC>::SetOption(optionName, optionValue);
    }

    virtual bool Transcode(const void * fromPtr, unsigned & fromLen, void * toPtr, unsigned & toLen, unsigned & flags)
    {
      if(m_state == NULL)
        return false;

      toLen = g722_1_encode(m_state, (uint8_t *)toPtr, (const int16_t *)fromPtr, fromLen/2);
      return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Decoder : public PluginCodec<CODEC>
{
  protected:
    g722_1_decode_state_t *m_state;
    const char *m_description;
    unsigned m_sampleRate;
    unsigned m_bitsPerSec;

  public:
    Decoder(const PluginCodec_Definition * defn)
      : PluginCodec<CODEC>(defn)
      , m_state(NULL)
      , m_description(m_definition->descr)
      , m_bitsPerSec(m_definition->bitsPerSec)
      , m_sampleRate(m_definition->sampleRate)
    {
    }

    ~Decoder()
    {
      if(m_state != NULL)
        g722_1_decode_release(m_state);
    }

    virtual bool Construct()
    {
      m_state = g722_1_decode_init(NULL, m_bitsPerSec, m_sampleRate);
      if(m_state == NULL)
      {
        PTRACE(1, m_description, "error: g722_1_decode_init(m_state, " << m_bitsPerSec << ", " <<  m_sampleRate << ")");
        return false;
      }
      return true;
    }

    virtual bool SetOption(const char * optionName, const char * optionValue)
    {
      if(strcmp(optionName, PLUGINCODEC_OPTION_MAX_BIT_RATE) == 0)
      {
        return true;
      }
      if(strcmp(optionName, "Generic Parameter 2") == 0)
      {
        unsigned value = atoi(optionValue);
        unsigned bitsPerSec = m_bitsPerSec;
        if(value == 64)                      bitsPerSec = 24000;
        else if(value == 96 || value == 32)  bitsPerSec = 32000;
        else if(value == 112 || value == 16) bitsPerSec = 48000;
        if(bitsPerSec != m_bitsPerSec)
        {
          if(g722_1_decode_set_rate(m_state, bitsPerSec) == -1)
          {
            PTRACE(1, m_description, "error: g722_1_decode_set_rate(m_state, " << bitsPerSec << ")");
            return false;
          }
          m_bitsPerSec = bitsPerSec;
        }
        return true;
      }
      return PluginCodec<CODEC>::SetOption(optionName, optionValue);
    }


    virtual bool Transcode(const void * fromPtr, unsigned & fromLen, void * toPtr, unsigned & toLen, unsigned & flags)
    {
      if(m_state == NULL)
        return false;

      if(fromLen != m_state->bytes_per_frame && fromLen != m_state->bytes_per_frame*2)
      {
        PTRACE(1, m_description, "decoder received " << fromLen << " bytes, ignoring frame");
        return false;
      }

      toLen = g722_1_decode(m_state, (int16_t *)toPtr, (const uint8_t *)fromPtr, fromLen);
      toLen = toLen*2;
      return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static const char   PayloadName[]                     = "g7221";
static unsigned int PayloadType                       = 0;
static unsigned int MaxBytesPerFrame                  = 4000;
static unsigned int FramesPerPacket                   = 1;
static unsigned int MaxFramesPerPacket                = 2;

static const char   G7221_24K_Identifier[]            = OpalPluginCodec_Identifer_G7221;
static const char   G7221_24K_MediaFmt[]              = "G.722.1-24K";
static const char   G7221_24K_BitRate[]               = "24000";
static unsigned int G7221_24K_MaxBitRate              = 24000;
static unsigned int G7221_24K_SampleRate              = 16000;
static unsigned int G7221_24K_Samples                 = 320;

static const char   G7221_32K_Identifier[]            = OpalPluginCodec_Identifer_G7221;
static const char   G7221_32K_MediaFmt[]              = "G.722.1-32K";
static const char   G7221_32K_BitRate[]               = "32000";
static unsigned int G7221_32K_MaxBitRate              = 32000;
static unsigned int G7221_32K_SampleRate              = 16000;
static unsigned int G7221_32K_Samples                 = 320;

static const char   G7221C_24K_Identifier[]           = OpalPluginCodec_Identifer_G7221ext;
static const char   G7221C_24K_MediaFmt[]             = "G.722.1C-24K";
static const char   G7221C_24K_BitRate[]              = "24000";
static unsigned int G7221C_24K_MaxBitRate             = 24000;
static unsigned int G7221C_24K_SampleRate             = 32000;
static unsigned int G7221C_24K_Samples                = 640;
static unsigned int G7221C_24K_SupportedExtendedModes = 64;

static const char   G7221C_32K_Identifier[]           = OpalPluginCodec_Identifer_G7221ext;
static const char   G7221C_32K_MediaFmt[]             = "G.722.1C-32K";
static const char   G7221C_32K_BitRate[]              = "32000";
static unsigned int G7221C_32K_MaxBitRate             = 32000;
static unsigned int G7221C_32K_SampleRate             = 32000;
static unsigned int G7221C_32K_Samples                = 640;
static unsigned int G7221C_32K_SupportedExtendedModes = 96;

static const char   G7221C_48K_Identifier[]           = OpalPluginCodec_Identifer_G7221ext;
static const char   G7221C_48K_MediaFmt[]             = "G.722.1C-48K";
static const char   G7221C_48K_BitRate[]              = "48000";
static unsigned int G7221C_48K_MaxBitRate             = 48000;
static unsigned int G7221C_48K_SampleRate             = 32000;
static unsigned int G7221C_48K_Samples                = 640;
static unsigned int G7221C_48K_SupportedExtendedModes = 112;

////////////////////////////////////////////////////////////////////////////////////////////////////

#define CAPABILITY_G7221(prefix) \
static const struct PluginCodec_H323GenericParameterDefinition prefix##_h323params[] = \
{ \
  {{1},1,PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsignedMin,{MaxFramesPerPacket}}, \
  NULL \
}; \
static struct PluginCodec_H323GenericCodecData prefix##_Capability = \
{ \
  prefix##_Identifier, \
  prefix##_MaxBitRate, \
  1, \
  prefix##_h323params \
}; \

////////////////////////////////////////////////////////////////////////////////////////////////////

#define CAPABILITY_G7221C(prefix) \
static const struct PluginCodec_H323GenericParameterDefinition prefix##_h323params[] = \
{ \
  {{1},1,PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsignedMin,{MaxFramesPerPacket}}, \
  {{1},2,PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_booleanArray,{prefix##_SupportedExtendedModes}}, \
  NULL \
}; \
static struct PluginCodec_H323GenericCodecData prefix##_Capability = \
{ \
  prefix##_Identifier, \
  prefix##_MaxBitRate/100, \
  2, \
  prefix##_h323params \
}; \

////////////////////////////////////////////////////////////////////////////////////////////////////

#define PLUGIN_CODEC(prefix) \
static struct PluginCodec_Option const prefix##_RxFramesPerPacketOption = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  PLUGINCODEC_OPTION_RX_FRAMES_PER_PACKET, /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_MinMerge,               /* Merge mode */ \
  "2",                                /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "1",                                /* Minimum value */ \
  "8"                                 /* Maximum value */ \
}; \
static struct PluginCodec_Option const prefix##_BitRateOption = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  PLUGINCODEC_OPTION_MAX_BIT_RATE,    /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_EqualMerge,             /* Merge mode */ \
  prefix##_BitRate,                   /* Initial value */ \
  "bitrate",                          /* FMTP option name */ \
  "0",                                /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "24000",                            /* Minimum value */ \
  "48000"                             /* Maximum value */ \
}; \
static struct PluginCodec_Option const * prefix##_OptionsTable[] = \
{ \
  &prefix##_RxFramesPerPacketOption, \
  &prefix##_BitRateOption, \
  NULL \
}; \
class prefix##_AudioFormat : public AudioFormat \
{ \
  public: \
    prefix##_AudioFormat() \
      : AudioFormat( \
        prefix##_MediaFmt, \
        PayloadName, \
        prefix##_MediaFmt, \
        prefix##_Samples, \
        MaxBytesPerFrame, \
        prefix##_SampleRate, \
        prefix##_OptionsTable) \
    { \
      m_maxBandwidth = prefix##_MaxBitRate; \
      m_recommendedFramesPerPacket = FramesPerPacket; \
      m_maxFramesPerPacket = MaxFramesPerPacket; \
      m_payloadType = PayloadType; \
      m_h323CapabilityType = PluginCodec_H323Codec_generic; \
      m_h323CapabilityData = (struct PluginCodec_H323GenericCodecData *)&prefix##_Capability; \
    } \
}; \
static prefix##_AudioFormat prefix##_AudioFormatInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////

CAPABILITY_G7221(G7221_24K);
CAPABILITY_G7221(G7221_32K);
CAPABILITY_G7221C(G7221C_24K);
CAPABILITY_G7221C(G7221C_32K);
CAPABILITY_G7221C(G7221C_48K);

PLUGIN_CODEC(G7221_24K);
PLUGIN_CODEC(G7221_32K);
PLUGIN_CODEC(G7221C_24K);
PLUGIN_CODEC(G7221C_32K);
PLUGIN_CODEC(G7221C_48K);

DECLARE_CONTROLS_TABLE(Encoder, Decoder);

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition CodecDefinition[] =
{
  PLUGINCODEC_AUDIO_CODEC_CXX(G7221_24K_AudioFormatInfo, Encoder, Decoder)
  ,PLUGINCODEC_AUDIO_CODEC_CXX(G7221_32K_AudioFormatInfo, Encoder, Decoder)
  ,PLUGINCODEC_AUDIO_CODEC_CXX(G7221C_24K_AudioFormatInfo, Encoder, Decoder)
  ,PLUGINCODEC_AUDIO_CODEC_CXX(G7221C_32K_AudioFormatInfo, Encoder, Decoder)
  ,PLUGINCODEC_AUDIO_CODEC_CXX(G7221C_48K_AudioFormatInfo, Encoder, Decoder)
};

PLUGIN_CODEC_IMPLEMENT_CXX(CODEC, CodecDefinition);

////////////////////////////////////////////////////////////////////////////////////////////////////
