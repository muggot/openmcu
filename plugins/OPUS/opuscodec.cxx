/*
 * OPUS audio codec Plugin codec for OPAL
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
#define CODEC_LOG "OPUS"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codec/opalplugin.hpp"
#include "codec/plugin-config.h"

#ifdef LINK_OPUS_FLAG
#  include <opus/opus.h>
#else
#  include "opus.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

PLUGINCODEC_LICENSE(
    "Andrey Burbovskiy",                                         // source code author
    "0.3",                                                       // source code version
    "andrewb@yandex.ru",                                         // source code email
    "http://openmcu.ru/",                                        // source code URL
    "Copyright (C) 2013 by Andrey Burbovskiy for OpenMCU-ru",    // source code copyright
    "MPL 1.0",                                                   // source code license
    PluginCodec_License_MPL,                                     // source code license

    "OPUS (ultra-low delay audio codec)",                        // codec description
    "Jean-Marc Valin, Xiph Foundation.",                         // codec author
    "1.0.2",                                                     // codec version
    "jmvalin@jmvalin.ca",                                        // codec email
    "http://www.opus-codec.org",                                 // codec URL
    "(C) 2013 Xiph.Org Foundation, All Rights Reserved",         // codec copyright information
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
    OpusEncoder *m_state;
    unsigned m_samplesPerFrame;
    unsigned m_bytesPerFrame;
    unsigned m_sampleRate;
    unsigned m_signalSampleSize;
    unsigned m_encoderMode;
    unsigned m_encoderChannels;
    unsigned m_forceChannels;
    unsigned m_cbr;
    int error;
    int data_len;

  public:
    Encoder(const PluginCodec_Definition * defn)
      : PluginCodec<CODEC>(defn)
      , m_samplesPerFrame(m_definition->parm.audio.samplesPerFrame)
      , m_bytesPerFrame(m_definition->parm.audio.bytesPerFrame)
      , m_sampleRate(m_definition->sampleRate)
      , m_signalSampleSize(2)
      , m_encoderMode(OPUS_APPLICATION_VOIP)
      , m_encoderChannels(1)
      , m_forceChannels(0)
    {
    }

    ~Encoder()
    {
      opus_encoder_destroy(m_state);
    }

    virtual bool Construct()
    {
      GetInitOptions();
      int size = opus_encoder_get_size(m_encoderChannels);
      m_state = (OpusEncoder*)malloc(size);
      if (m_state == NULL)
        return false;

      error = opus_encoder_init(m_state, m_sampleRate, m_encoderChannels, m_encoderMode);
      if (error != OPUS_OK || m_state == NULL) {
        PTRACE(1, CODEC_LOG, "opus_encoder_init error: " << error << " " << opus_strerror(error));
        return false;
      }

      if (m_forceChannels > 0) {
        error = opus_encoder_ctl(m_state, OPUS_SET_FORCE_CHANNELS(m_forceChannels));
        if (error != OPUS_OK)
          PTRACE(1, CODEC_LOG, "Error force: " << opus_strerror(error));
      }

      opus_encoder_ctl(m_state, OPUS_SET_VBR(m_cbr==0?1:0));

      return true;
    }


    virtual void GetInitOptions()
    {
      for (PluginCodec_Option ** options = (PluginCodec_Option **)((MediaFormat *)m_definition->userData)->GetOptionsTable(); *options != NULL; ++options) {
        if (strcmp((*options)->m_name, "Encoder Channels") == 0) {
          m_encoderChannels = atoi((*options)->m_value);
        }
        if (strcmp((*options)->m_name, "Force Channels") == 0) {
          m_forceChannels = atoi((*options)->m_value);
        }
        if (strcmp((*options)->m_name, "Encoder Mode") == 0) {
          if (strcmp((*options)->m_value, "VOIP") == 0)
            m_encoderMode = OPUS_APPLICATION_VOIP;
          else if (strcmp((*options)->m_value, "AUDIO") == 0)
            m_encoderMode = OPUS_APPLICATION_AUDIO;
        }
        if (strcmp((*options)->m_name, "cbr") == 0) {
          m_cbr = atoi((*options)->m_value);
        }
      }
    }

    virtual bool SetOption(const char * optionName, const char * optionValue)
    {
      if (strcasecmp(optionName, "useinbandfec") == 0) {
        error = opus_encoder_ctl(m_state, OPUS_SET_INBAND_FEC(atoi(optionValue)));
        return true;
      }
      if (strcasecmp(optionName, "usedtx") == 0) {
        error = opus_encoder_ctl(m_state, OPUS_SET_DTX(atoi(optionValue)));
        return true;
      }
      if (strcasecmp(optionName, "cbr") == 0) {
        error = opus_encoder_ctl(m_state, OPUS_SET_VBR(atoi(optionValue)==0?1:0));
        return true;
      }
      if (strcasecmp(optionName, "maxaveragebitrate") == 0) {
        if(atoi(optionValue) >= 500 && atoi(optionValue) <= 512000)
          error = opus_encoder_ctl(m_state, OPUS_SET_BITRATE(atoi(optionValue)));
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
      if (fromLen < m_samplesPerFrame*m_signalSampleSize*m_encoderChannels)
        return false;

      if (toLen < 3)
        return false;

      data_len = opus_encode(m_state, (const opus_int16 *)fromPtr, m_samplesPerFrame, (unsigned char *)toPtr, m_bytesPerFrame);
      if (data_len < 0) {
        PTRACE(1, CODEC_LOG, "opus_encode error: " << data_len << " " << opus_strerror(data_len));
        return false;
      }

      toLen = data_len;
      return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Decoder : public PluginCodec<CODEC>
{
  protected:
    OpusDecoder *m_state;
    unsigned m_samplesPerFrame;
    unsigned m_bytesPerFrame;
    unsigned m_sampleRate;
    unsigned m_signalSampleSize;
    unsigned m_decoderChannels;
    int error;
    int frame_size;

  public:
    Decoder(const PluginCodec_Definition * defn)
      : PluginCodec<CODEC>(defn)
      , m_samplesPerFrame(m_definition->parm.audio.samplesPerFrame)
      , m_bytesPerFrame(m_definition->parm.audio.bytesPerFrame)
      , m_sampleRate(m_definition->sampleRate)
      , m_signalSampleSize(2)
      , m_decoderChannels(1)
    {
    }

    ~Decoder()
    {
      opus_decoder_destroy(m_state);
    }

    virtual bool Construct()
    {
      GetInitOptions();
      int size = opus_decoder_get_size(m_decoderChannels);
      m_state = (OpusDecoder*)malloc(size);
      if (m_state == NULL)
        return false;

      error = opus_decoder_init(m_state, m_sampleRate, m_decoderChannels);
      if (error != OPUS_OK || m_state == NULL) {
        PTRACE(1, CODEC_LOG, "opus_decoder_init error: " << error << " " << opus_strerror(error));
        return false;
      }
      return true;
    }

    virtual void GetInitOptions()
    {
      for (PluginCodec_Option ** options = (PluginCodec_Option **)((MediaFormat *)m_definition->userData)->GetOptionsTable(); *options != NULL; ++options) {
        if (strcmp((*options)->m_name, "Decoder Channels") == 0) {
          m_decoderChannels = atoi((*options)->m_value);
        }
      }
    }

    virtual bool Transcode(const void * fromPtr,
                             unsigned & fromLen,
                                 void * toPtr,
                             unsigned & toLen,
                             unsigned & flags)
    {
      if (toLen < m_samplesPerFrame*m_signalSampleSize)
        return false;

      if (fromLen < 3)
        return false;

      frame_size = opus_decode(m_state, (const unsigned char *)fromPtr, fromLen, (opus_int16 *)toPtr, m_bytesPerFrame, 0);
      if (frame_size < 0) {
        PTRACE(1, CODEC_LOG, "opus_decode error: " << frame_size << " " << opus_strerror(frame_size));
        return false;
      }

      //toLen = frame_size*m_signalSampleSize*m_decoderChannels;
      return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define OpalPluginCodec_Identifer_OPUS_8K          "1.3.6.1.4.1.17091.1.10.8"
#define OPUS_8K_MediaFmt                           "OPUS_8K"
#define OPUS_8K_PayloadName                        "opus"
#define OPUS_8K_PayloadType                         0
#define OPUS_8K_EncMode                            "VOIP"
#define OPUS_8K_EncoderChannelsNum                 "1"
#define OPUS_8K_DecoderChannelsNum                 "1"
#define OPUS_8K_ForceChannelsNum                   "0"
static unsigned int OPUS_8K_ClockRate              =8000;
static unsigned int OPUS_8K_MaxBitRate             =12000;
static unsigned int OPUS_8K_DataPerFrame           =20000;
static unsigned int OPUS_8K_Samples                =160;
static unsigned int OPUS_8K_MaxBytesPerFrame       =4000;
static unsigned int OPUS_8K_FramesPerPacket        =1;
static unsigned int OPUS_8K_MaxFramesPerPacket     =1;

#define OpalPluginCodec_Identifer_OPUS_16K         "1.3.6.1.4.1.17091.1.10.16"
#define OPUS_16K_MediaFmt                          "OPUS_16K"
#define OPUS_16K_PayloadName                       "opus"
#define OPUS_16K_PayloadType                        0
#define OPUS_16K_EncMode                           "VOIP"
#define OPUS_16K_EncoderChannelsNum                "1"
#define OPUS_16K_DecoderChannelsNum                "1"
#define OPUS_16K_ForceChannelsNum                  "0"
static unsigned int OPUS_16K_ClockRate             =16000;
static unsigned int OPUS_16K_MaxBitRate            =20000;
static unsigned int OPUS_16K_DataPerFrame          =20000;
static unsigned int OPUS_16K_Samples               =320;
static unsigned int OPUS_16K_MaxBytesPerFrame      =4000;
static unsigned int OPUS_16K_FramesPerPacket       =1;
static unsigned int OPUS_16K_MaxFramesPerPacket    =1;

#define OpalPluginCodec_Identifer_OPUS_48K         "1.3.6.1.4.1.17091.1.10.48"
#define OPUS_48K_MediaFmt                          "OPUS_48K"
#define OPUS_48K_PayloadName                       "opus"
#define OPUS_48K_PayloadType                        0
#define OPUS_48K_EncMode                           "VOIP"
#define OPUS_48K_EncoderChannelsNum                "1"
#define OPUS_48K_DecoderChannelsNum                "1"
#define OPUS_48K_ForceChannelsNum                  "0"
static unsigned int OPUS_48K_ClockRate             =48000;
static unsigned int OPUS_48K_MaxBitRate            =64000;
static unsigned int OPUS_48K_DataPerFrame          =20000;
static unsigned int OPUS_48K_Samples               =960;
static unsigned int OPUS_48K_MaxBytesPerFrame      =4000;
static unsigned int OPUS_48K_FramesPerPacket       =1;
static unsigned int OPUS_48K_MaxFramesPerPacket    =1;

#define OpalPluginCodec_Identifer_OPUS_48K2        "1.3.6.1.4.1.17091.1.10.482"
#define OPUS_48K2_MediaFmt                         "OPUS_48K2"
#define OPUS_48K2_PayloadName                      "opus"
#define OPUS_48K2_PayloadType                       0
#define OPUS_48K2_EncMode                          "VOIP"
#define OPUS_48K2_EncoderChannelsNum               "2"
#define OPUS_48K2_DecoderChannelsNum               "2"
#define OPUS_48K2_ForceChannelsNum                 "0"
static unsigned int OPUS_48K2_ClockRate            =48000;
static unsigned int OPUS_48K2_MaxBitRate           =128000;
static unsigned int OPUS_48K2_DataPerFrame         =20000;
static unsigned int OPUS_48K2_Samples              =960;
static unsigned int OPUS_48K2_MaxBytesPerFrame     =4000;
static unsigned int OPUS_48K2_FramesPerPacket      =1;
static unsigned int OPUS_48K2_MaxFramesPerPacket   =1;

#define PLUGIN_CODEC(prefix) \
static struct PluginCodec_Option const prefix##_EncoderChannels = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "Encoder Channels",                 /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  prefix##_EncoderChannelsNum,        /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "1",                                /* Minimum value */ \
  "2"                                 /* Maximum value */ \
}; \
static struct PluginCodec_Option const prefix##_DecoderChannels = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "Decoder Channels",                 /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  prefix##_DecoderChannelsNum,        /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "1",                                /* Minimum value */ \
  "2"                                 /* Maximum value */ \
}; \
static struct PluginCodec_Option const prefix##_ForceChannels = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "Force Channels",                   /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  prefix##_ForceChannelsNum,          /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "1",                                /* Minimum value */ \
  "2"                                 /* Maximum value */ \
}; \
static struct PluginCodec_Option const prefix##_EncoderMode = \
{ \
  PluginCodec_EnumOption,             /* Option type */ \
  "Encoder Mode",                     /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  prefix##_EncMode,                   /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "VOIP:AUDIO"                        /* Enumeration */ \
}; \
static struct PluginCodec_Option const prefix##_UseInBandFEC = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "useinbandfec",                     /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  "0",                                /* Initial value */ \
  "useinbandfec",                     /* FMTP option name */ \
  "0",                                /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "0",                                /* Minimum value */ \
  "1"                                 /* Maximum value */ \
}; \
static struct PluginCodec_Option const prefix##_UseDTX = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "usedtx",                           /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  "0",                                /* Initial value */ \
  "usedtx",                           /* FMTP option name */ \
  "0",                                /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "0",                                /* Minimum value */ \
  "1"                                 /* Maximum value */ \
}; \
static struct PluginCodec_Option const prefix##_EncoderCBR = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "cbr",                              /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  "0",                                /* Initial value */ \
  "cbr",                              /* FMTP option name */ \
  "0",                                /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "0",                                /* Minimum value */ \
  "1"                                 /* Maximum value */ \
}; \
static struct PluginCodec_Option const prefix##_MaxAverageBitrate = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "maxaveragebitrate",                /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  "0",                                /* Initial value */ \
  "maxaveragebitrate",                /* FMTP option name */ \
  "0",                                /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  "500",                              /* Minimum value */ \
  "512000"                            /* Maximum value */ \
}; \
static struct PluginCodec_Option const * prefix##_OptionTable[] = \
{ \
  &prefix##_EncoderChannels, \
  &prefix##_DecoderChannels, \
  &prefix##_ForceChannels, \
  &prefix##_EncoderMode, \
  &prefix##_UseInBandFEC, \
  &prefix##_UseDTX, \
  &prefix##_EncoderCBR, \
  &prefix##_MaxAverageBitrate, \
  NULL \
}; \
static const struct PluginCodec_H323GenericParameterDefinition prefix##_h323params[] = \
{ \
    NULL \
}; \
static struct PluginCodec_H323GenericCodecData prefix##_Cap = \
{ \
    OpalPluginCodec_Identifer_##prefix, \
    0, \
    0, \
    prefix##_h323params \
}; \
class prefix##_AudioFormat : public AudioFormat \
{ \
  public: \
    prefix##_AudioFormat() \
      : AudioFormat( \
        prefix##_MediaFmt, \
        prefix##_PayloadName, \
        prefix##_MediaFmt, \
        prefix##_Samples, \
        prefix##_MaxBytesPerFrame, \
        prefix##_ClockRate, \
        prefix##_OptionTable) \
    { \
      m_payloadType = prefix##_PayloadType; \
      m_recommendedFramesPerPacket = prefix##_FramesPerPacket; \
      m_maxFramesPerPacket = prefix##_MaxFramesPerPacket; \
      m_h323CapabilityType = PluginCodec_H323Codec_generic; \
      m_h323CapabilityData = (struct PluginCodec_H323GenericCodecData *)&prefix##_Cap; \
    } \
}; \
static prefix##_AudioFormat prefix##_AudioFormatInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////

PLUGIN_CODEC(OPUS_8K);
PLUGIN_CODEC(OPUS_16K);
PLUGIN_CODEC(OPUS_48K);
PLUGIN_CODEC(OPUS_48K2);

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition CodecDefinition[] =
{
  PLUGINCODEC_AUDIO_CODEC_CXX(OPUS_8K_AudioFormatInfo, Encoder, Decoder)
  ,PLUGINCODEC_AUDIO_CODEC_CXX(OPUS_16K_AudioFormatInfo, Encoder, Decoder)
  ,PLUGINCODEC_AUDIO_CODEC_CXX(OPUS_48K_AudioFormatInfo, Encoder, Decoder)
  ,PLUGINCODEC_AUDIO_CODEC_CXX(OPUS_48K2_AudioFormatInfo, Encoder, Decoder)
};

PLUGIN_CODEC_IMPLEMENT_CXX(CODEC, CodecDefinition);

////////////////////////////////////////////////////////////////////////////////////////////////////
