/*
    Opus codec plugin for Opal
    Author: Xak
    OpenMCU-ru
*/

#define PTRACE(level,category,text)
#define CODEC_LOG "OPUS"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codec/opalplugin.hpp"
#include "codec/plugin-config.h"

#include "src/include/opus.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

PLUGINCODEC_LICENSE(
    "Andrey Burbovskiy",                                         // source code author
    "0.3",                                                       // source code version
    "xak-altsp@yandex.ru",                                       // source code email
    "http://openmcu.ru/",                                        // source code URL
    "Copyright (C) 2013 by Andrey Burbovskiy for OpenMCU-ru",    // source code copyright
    "BSD license",                                               // source code license
    PluginCodec_License_BSD,                                     // source code license

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

static struct PluginCodec_Option const UseInBandFEC =
{
  PluginCodec_IntegerOption,          /* Option type */
  "useinbandfec",                     /* User visible name */
  false,                              /* User Read/Only flag */
  PluginCodec_NoMerge,                /* Merge mode */
  "0",                                /* Initial value */
  "useinbandfec",                     /* FMTP option name */
  "0",                                /* FMTP default value */
  0,                                  /* H.245 generic capability code and bit mask */
  "0",                                /* Minimum value */
  "1"                                 /* Maximum value */
};
static struct PluginCodec_Option const UseDTX =
{
  PluginCodec_IntegerOption,          /* Option type */
  "usedtx",                           /* User visible name */
  false,                              /* User Read/Only flag */
  PluginCodec_NoMerge,                /* Merge mode */
  "0",                                /* Initial value */
  "usedtx",                           /* FMTP option name */
  "0",                                /* FMTP default value */
  0,                                  /* H.245 generic capability code and bit mask */
  "0",                                /* Minimum value */
  "1"                                 /* Maximum value */
};

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
    unsigned m_channels;
    int error;
    int data_len;

  public:
    Encoder(const PluginCodec_Definition * defn)
      : PluginCodec<CODEC>(defn)
      , m_samplesPerFrame(m_definition->parm.audio.samplesPerFrame)
      , m_bytesPerFrame(m_definition->parm.audio.bytesPerFrame)
      , m_sampleRate(m_definition->sampleRate)
      , m_signalSampleSize(2)
      , m_channels(1)
    {
    }

    ~Encoder()
    {
      opus_encoder_destroy(m_state);
    }

    virtual bool Construct()
    {
      int size = opus_encoder_get_size(m_channels);
      m_state = (OpusEncoder*)malloc(size);
      if (m_state == NULL)
        return false;

      error = opus_encoder_init(m_state, m_sampleRate, m_channels, OPUS_APPLICATION_VOIP);
      //m_state = opus_encoder_create(m_sampleRate, 1, OPUS_APPLICATION_VOIP, &error);
      if (error != OPUS_OK || m_state == NULL) {
        PTRACE(1, CODEC_LOG, "Error opus_encoder_init: " << opus_strerror(error));
        return false;
      }

      return true;
    }


    virtual bool SetOption(const char * optionName, const char * optionValue)
    {
      if (strcasecmp(optionName, UseInBandFEC.m_name) == 0) {
        error = opus_encoder_ctl(m_state, OPUS_SET_INBAND_FEC(atoi(optionValue)));
        return true;
      }
      if (strcasecmp(optionName, UseDTX.m_name) == 0) {
        error = opus_encoder_ctl(m_state, OPUS_SET_DTX(atoi(optionValue)));
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

      if (toLen < 3)
        return false;

      data_len = opus_encode(m_state, (const opus_int16 *)fromPtr, fromLen/2, (unsigned char *)toPtr, toLen);
      if (data_len < 0) {
        return false;
      }

      toLen = data_len;
      fromLen = m_samplesPerFrame*m_channels*m_signalSampleSize;
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
    unsigned m_channels;
    int error;
    int frame_size;

  public:
    Decoder(const PluginCodec_Definition * defn)
      : PluginCodec<CODEC>(defn)
      , m_samplesPerFrame(m_definition->parm.audio.samplesPerFrame)
      , m_bytesPerFrame(m_definition->parm.audio.bytesPerFrame)
      , m_sampleRate(m_definition->sampleRate)
      , m_signalSampleSize(2)
      , m_channels(1)
    {
    }

    ~Decoder()
    {
      opus_decoder_destroy(m_state);
    }

    virtual bool Construct()
    {
      int size = opus_decoder_get_size(m_channels);
      m_state = (OpusDecoder*)malloc(size);
      if (m_state == NULL)
        return false;

      error = opus_decoder_init(m_state, m_sampleRate, m_channels);
      //m_state = opus_decoder_create(m_sampleRate, m_channels, &error);
      if (error != OPUS_OK || m_state == NULL) {
        PTRACE(1, CODEC_LOG, "Error opus_decoder_init: " << opus_strerror(error));
        return false;
      }
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

      if (fromLen < 3)
        return false;

      frame_size = opus_decode(m_state, (const unsigned char *)fromPtr, fromLen, (opus_int16 *)toPtr, m_bytesPerFrame, 0);
      if (frame_size < 0) {
        return false;
      }

      toLen = frame_size*m_channels*m_signalSampleSize;
      return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define OpalPluginCodec_Identifer_OPUS_8K          "1.3.6.1.4.1.17091.1.10.8"
#define OPUS_8K_MediaFmt                           "OPUS_8K"
static unsigned int OPUS_8K_ClockRate              =8000;
static unsigned int OPUS_8K_MaxBitRate             =12000;
static unsigned int OPUS_8K_DataPerFrame           =20000;
static unsigned int OPUS_8K_Samples                =160;
static unsigned int OPUS_8K_MaxBytesPerFrame       =4000;
static unsigned int OPUS_8K_FramesPerPacket        =1;
static unsigned int OPUS_8K_MaxFramesPerPacket     =1;

#define OpalPluginCodec_Identifer_OPUS_16K         "1.3.6.1.4.1.17091.1.10.16"
#define OPUS_16K_MediaFmt                          "OPUS_16K"
static unsigned int OPUS_16K_ClockRate             =16000;
static unsigned int OPUS_16K_MaxBitRate            =20000;
static unsigned int OPUS_16K_DataPerFrame          =20000;
static unsigned int OPUS_16K_Samples               =320;
static unsigned int OPUS_16K_MaxBytesPerFrame      =4000;
static unsigned int OPUS_16K_FramesPerPacket       =1;
static unsigned int OPUS_16K_MaxFramesPerPacket    =1;

#define OpalPluginCodec_Identifer_OPUS_48K         "1.3.6.1.4.1.17091.1.10.48"
#define OPUS_48K_MediaFmt                          "OPUS_48K"
static unsigned int OPUS_48K_ClockRate             =48000;
static unsigned int OPUS_48K_MaxBitRate            =64000;
static unsigned int OPUS_48K_DataPerFrame          =20000;
static unsigned int OPUS_48K_Samples               =960;
static unsigned int OPUS_48K_MaxBytesPerFrame      =4000;
static unsigned int OPUS_48K_FramesPerPacket       =1;
static unsigned int OPUS_48K_MaxFramesPerPacket    =1;

#define PLUGIN_CODEC(prefix) \
static struct PluginCodec_Option const * prefix##_OptionTable[] = \
{ \
  &UseInBandFEC, \
  &UseDTX, \
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
        prefix##_MediaFmt, \
        prefix##_MediaFmt, \
        prefix##_Samples, \
        prefix##_MaxBytesPerFrame, \
        prefix##_ClockRate, \
        prefix##_OptionTable) \
    { \
      m_recommendedFramesPerPacket = prefix##_FramesPerPacket; \
      m_maxFramesPerPacket = prefix##_MaxFramesPerPacket; \
      m_h323CapabilityType = PluginCodec_H323Codec_generic; \
      m_h323CapabilityData = (struct PluginCodec_H323GenericCodecData *)&prefix##_Cap; \
    } \
}; \
static prefix##_AudioFormat prefix##_AudioFormatInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////

PLUGIN_CODEC(OPUS_16K);
PLUGIN_CODEC(OPUS_48K);

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition CodecDefinition[] =
{
  PLUGINCODEC_AUDIO_CODEC_CXX(OPUS_16K_AudioFormatInfo, Encoder, Decoder)
  ,PLUGINCODEC_AUDIO_CODEC_CXX(OPUS_48K_AudioFormatInfo, Encoder, Decoder)
};

PLUGIN_CODEC_IMPLEMENT_CXX(CODEC, CodecDefinition);

////////////////////////////////////////////////////////////////////////////////////////////////////
