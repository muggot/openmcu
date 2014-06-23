/*
 * MPEG4 Plugin codec for OpenMCU-ru
 *
 * Copyright (C) 2014 by Andrey Burbovskiy, OpenMCU-ru Team, All Rights Reserved
 *
 * Contributor(s):  Andrey Burbovskiy (andrewb@yandex.ru)
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * MPEG4 Plugin codec for OpenH323/OPAL
 *
 * This codec implements an MPEG4 encoder/decoder using the ffmpeg library
 * (http://ffmpeg.mplayerhq.hu/).  Plugin infrastructure is based off of the
 * H.263 plugin and MPEG4 codec functions originally created by Michael Smith.
 *
 * The original files, and this version of the original code, are released under the same 
 * MPL 1.0 license. Substantial portions of the original code were contributed
 * by Salyens and March Networks and their right to be identified as copyright holders
 * of the original code portions and any parts now included in this new copy is asserted through 
 * their inclusion in the copyright notices below.
 *
 * Copyright (C) 2007 Canadian Bank Note, Limited
 * Copyright (C) 2006 Post Increment
 * Copyright (C) 2005 Salyens
 * Copyright (C) 2001 March Networks Corporation
 * Copyright (C) 1999-2000 Equivalence Pty. Ltd.
 * Copyright (C) 2010 Vox Lucida Pty. Ltd.
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
 * Contributor(s): Josh Mahonin (jmahonin@cbnco.com)
 *                 Michael Smith (msmith@cbnco.com)
 *                 Guilhem Tardy (gtardy@salyens.com)
 *                 Craig Southeren (craigs@postincrement.com)
 *                 Matthias Schneider (ma30002000@yahoo.de)
 *                 Robert Jongbloed (robertj@voxlucida.com.au)
 */

extern "C"
{
  #include "libavcodec/avcodec.h"
  #include "libavutil/mem.h"
}

#include <vector>
#include <deque>

#include "../common/opalplugin.hpp"
#include "../common/ffmpeg.h"
#include "../common/utils.h"

const unsigned MaxBitRate = 4000000;


PLUGINCODEC_CONTROL_LOG_FUNCTION_DEF

///////////////////////////////////////////////////////////////////////////////

PLUGINCODEC_LICENSE(
  "Andrey Burbovskiy",                                          // source code author
  "1.1",                                                        // source code version
  "andrewb@yandex.ru",                                          // source code email
  "http://www.openmcu.ru",                                      // source code URL
  "Copyright (C) 2014 by OpenMCU-ru Team",                      // source code copyright
  "MPL 1.0",                                                    // source code license
  PluginCodec_License_MPL,                                      // source code license

  "MPEG4 Video Codec",                                          // codec description
  "Michael Niedermayer, Fabrice Bellard",                       // codec author
  "4.7.1",                                                      // codec version
  "ffmpeg-devel-request@ mplayerhq.hu",                         // codec email
  "http://sourceforge.net/projects/ffmpeg/",                    // codec URL
  "Copyright (c) 2000-2001 Fabrice Bellard"                     // codec copyright information
  ", Copyright (c) 2002-2003 Michael Niedermayer",
  "GNU LESSER GENERAL PUBLIC LICENSE, Version 2.1, February 1999", // codec license
  PluginCodec_License_LGPL                                         // codec license code
);

///////////////////////////////////////////////////////////////////////////////

class CODEC { };

class VideoFormat : public PluginCodec_VideoFormat<CODEC>
{
  private:
    typedef PluginCodec_VideoFormat<CODEC> BaseClass;

  public:
    VideoFormat(const char * formatName, const char * payloadName, const char * description, OptionsTable options)
      : BaseClass(formatName, payloadName, description, MaxBitRate, options)
    {
    }
};

///////////////////////////////////////////////////////////////////////////////

class MPEG4_EncodedFrame : public FFMPEGCodec::EncodedFrame
{
  protected:
    // packet sizes generating in RtpCallback
    std::deque<size_t> m_packetSizes;
    unsigned m_fragmentationOffset;

  public:
    MPEG4_EncodedFrame()
      : m_fragmentationOffset(0)
    {
    }

    virtual bool GetPacket(PluginCodec_RTP & rtp, unsigned & flags)
    {
      if(m_packetSizes.empty())
      {
        m_fragmentationOffset = 0;
        return false;
      }

      if(!rtp.SetPayloadSize(std::min(m_packetSizes.front(), m_maxPayloadSize)))
        rtp.SetPayloadSize(rtp.GetMaxSize()-rtp.GetHeaderSize());
      size_t payloadSize = rtp.GetPayloadSize();

      // Remove, or adjust the current packet size
      if(payloadSize == m_packetSizes.front())
        m_packetSizes.pop_front();
      else
        m_packetSizes.front() -= payloadSize;

      // Copy the encoded data from the buffer into the outgoign RTP
      memcpy(rtp.GetPayloadPtr(), GetBuffer()+m_fragmentationOffset, payloadSize);
      m_fragmentationOffset += payloadSize;

      // If there are no more packet sizes left, we've reached the last packet
      // for the frame, set the marker bit and flags
      if(m_packetSizes.empty())
      {
        rtp.SetMarker(true);
        flags |= PluginCodec_ReturnCoderLastFrame;
      }
      else
        rtp.SetMarker(false);

      return true;
    }

    virtual bool AddPacket(const PluginCodec_RTP & rtp, unsigned & flags)
    {
      if(!Append(rtp.GetPayloadPtr(), rtp.GetPayloadSize()))
        return false;

      if(rtp.GetMarker())
        flags |= PluginCodec_ReturnCoderLastFrame;

      return true;
    }

    virtual bool IsIntraFrame() const
    {
      unsigned i = 0;
      while((i+4)<= m_length)
      {
        if((m_buffer[i] == 0) && (m_buffer[i+1] == 0) && (m_buffer[i+2] == 1))
        {
          if(m_buffer[i+3] == 0xb6)
          {
            unsigned vop_coding_type = (unsigned) ((m_buffer[i+4] & 0xC0) >> 6);
            if(vop_coding_type == 0)
              return true;
          }
        }
        i++;
      }

      return false;
    }

    virtual void RTPCallBack(void *, int size, int)
    {
      m_packetSizes.push_back(size);
    }
};

///////////////////////////////////////////////////////////////////////////////

class Encoder : public PluginVideoEncoder<CODEC>, public FFMPEGCodec
{
  private:
    typedef PluginVideoEncoder<CODEC> BaseClass;

  protected:
    const char *m_description;
    unsigned m_profileLevel;
    unsigned m_profile;
    unsigned m_level;

  public:
    Encoder(const PluginCodec_Definition * defn)
      : BaseClass(defn)
      , FFMPEGCodec(m_definition->descr, new MPEG4_EncodedFrame)
      , m_description(m_definition->descr)
      , m_profileLevel(1)
      , m_profile(1)
      , m_level(1)
    {
    }

    ~Encoder()
    {
    }

    virtual bool Construct()
    {
      return InitEncoder("mpeg4");
    }

    void SetEncoderOptions(unsigned frameTime, unsigned maxBitRate, unsigned maxRTPSize, unsigned tsto, unsigned keyFramePeriod)
    {
      m_context->time_base.den = 2997;
      m_context->time_base.num = frameTime*m_context->time_base.den/PLUGINCODEC_VIDEO_CLOCK;

      m_context->rc_min_rate = 0;
      m_context->rc_max_rate = maxBitRate;
      m_context->bit_rate = m_context->rc_max_rate*7/8;

      m_context->rc_buffer_size = m_context->rc_max_rate*2;
      // rc buffer underflow
      if(m_context->rc_buffer_size < m_width*m_height*16)
        m_context->rc_buffer_size = m_width*m_height*16;

      m_context->bit_rate_tolerance = m_context->rc_max_rate/10;
      int tolerance_min =  m_context->bit_rate*m_context->time_base.num/m_context->time_base.den + 1; // one frame bits
      if(m_context->bit_rate_tolerance < tolerance_min)
        m_context->bit_rate_tolerance = tolerance_min;

      // limit q by clipping
      m_context->rc_qsquish = 0;

      // rate control equation
      m_context->rc_eq = strdup("1");

      // default libavcodec settings
      m_context->max_qdiff = 3;                 // max q difference between frames
      m_context->qcompress = 0.5;               // qscale factor between easy & hard scenes (0.0-1.0)
      m_context->i_quant_factor = (float)-0.8;  // qscale factor between p and i frames
      m_context->i_quant_offset = (float)0.0;   // qscale offset between p and i frames
      m_context->me_subpel_quality = 8;

      m_context->qmax = tsto;
      if(m_context->qmax <= m_context->qmin)
        m_context->qmax = m_context->qmin+1;

      // Lagrange multipliers - this is how the context defaults do it:
      m_context->lmin = m_context->qmin * FF_QP2LAMBDA;
      m_context->lmax = m_context->qmax * FF_QP2LAMBDA;

      m_context->rtp_payload_size = maxRTPSize;
      m_context->gop_size = 125;

      m_context->max_b_frames = 0;
      m_context->flags |= CODEC_FLAG_PASS1;
      m_context->flags |= CODEC_FLAG_AC_PRED;
    }

    virtual bool SetOption(const char * name, const char * value)
    {
      if(strcasecmp(name, PLUGINCODEC_OPTION_MAX_BIT_RATE) == 0)
      {
        return SetOptionUnsigned(m_maxBitRate, value, 1, m_definition->bitsPerSec);
      }
      if(strcasecmp(name, PLUGINCODEC_OPTION_TARGET_BIT_RATE) == 0)
      {
        return true; // 64000 !!!
      }
      if(strcasecmp(name, "profile-level-id") == 0)
      {
        m_profileLevel = atoi(value);
        return true;
      }
      if(strcasecmp(name, "profile") == 0)
      {
        m_profile = atoi(value);
        return true;
      }
      if(strcasecmp(name, "level") == 0)
      {
        m_level = atoi(value);
        return true;
      }

      return BaseClass::SetOption(name, value);
    }

    virtual bool OnChangedOptions()
    {
      CloseCodec();

      m_context->profile = m_profile;
      m_context->level = m_level;

      SetResolution(m_width, m_height);
      SetEncoderOptions(m_frameTime, m_maxBitRate, m_maxRTPSize, m_tsto, m_keyFramePeriod);

      return OpenCodec();
    }

    virtual bool Transcode(const void * fromPtr, unsigned & fromLen, void * toPtr, unsigned & toLen, unsigned & flags)
    {
      PluginCodec_RTP dstRTP(toPtr, toLen);
      if(!EncodeVideoPacket(PluginCodec_RTP(fromPtr, fromLen), dstRTP, flags))
        return false;

      toLen = dstRTP.GetHeaderSize() + dstRTP.GetPayloadSize();
      return true;
    }

};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Decoder : public PluginVideoDecoder<CODEC>
{
  private:
    typedef PluginVideoDecoder<CODEC> BaseClass;

  protected:
    AVCodec             *m_codec;
    AVCodecContext      *m_context;
    AVFrame             *m_outputFrame;
    AVPacket             m_pkt;
    const char          *m_description;
    std::vector<uint8_t> m_fullFrame;
    bool                 m_intraFrame;

  public:
    Decoder(const PluginCodec_Definition * defn)
      : BaseClass(defn)
      , m_description(m_definition->descr)
      , m_intraFrame(false)
    {
      memset(&m_codec, 0, sizeof(m_codec));
      m_fullFrame.reserve(10000);
    }

    ~Decoder()
    {
      if(m_context != NULL)
        if(m_context->codec != NULL)
          avcodec_close(m_context);
      if(m_context != NULL) av_free(m_context);
      if(m_outputFrame != NULL) av_free(m_outputFrame);
    }

    virtual bool Construct()
    {
      m_codec = avcodec_find_decoder_by_name("mpeg4");
      if(m_codec == NULL)
      {
        PTRACE(1, m_description, "Codec not found");
        return false;
      }

#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT(53,8,0)
      m_context = avcodec_alloc_context();
#else
      m_context = avcodec_alloc_context3(m_codec);
#endif
      if(m_context == NULL)
      {
        PTRACE(1, m_description, "Failed to allocate context");
        return false;
      }

      m_outputFrame = avcodec_alloc_frame();
      if(m_outputFrame == NULL)
      {
        PTRACE(1, m_description, "Failed to allocate frame");
        return false;
      }

#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT(53,8,0)
      int result = avcodec_open(m_context, m_codec);
#else
      AVDictionary * options = NULL;
      int result = avcodec_open2(m_context, m_codec, NULL);
      av_dict_free(&options);
#endif
      if(result < 0)
      {
        PTRACE(1, m_description, "Failed to open codec");
        return false;
      }

      av_init_packet(&m_pkt);

      return true;
    }

    virtual bool SetOption(const char * name, const char * value)
    {
      if(strcasecmp(name, "config") == 0)
      {
        std::string s(value);
        if(s[0] == '"')          s.erase(0,1);
        if(s[s.size()-1] == '"') s.erase(s.size()-1);
        if(strcasecmp(s.c_str(), "") != 0)
          SetConfig(s.c_str());
        return true;
      }

      return BaseClass::SetOption(name, value);
    }

    void SetConfig(const char *value)
    {
      // from ffmpeg
      // decode the hexa encoded parameter
      int len = ff_hex_to_data(NULL, value);
      av_free(m_context->extradata);
      m_context->extradata = (uint8_t *)av_mallocz(len + FF_INPUT_BUFFER_PADDING_SIZE);
      if(!m_context->extradata)
      {
        av_log(m_context, AV_LOG_ERROR, "Decoder unable to allocate memory for extradata!\n");
        PTRACE(1, m_description, "Decoder unable to allocate memory for extradata!");
        return;
      }
      m_context->extradata_size = len;
      ff_hex_to_data(m_context->extradata, value);
      av_log(m_context, AV_LOG_INFO, "Decoder extradata set to %p (size: %d)!\n", m_context->extradata, m_context->extradata_size);
      PTRACE(1, m_description, "Decoder extradata set to " << m_context->extradata << " size: " << m_context->extradata_size);
    }

    virtual bool Transcode(const void * fromPtr, unsigned & fromLen, void * toPtr, unsigned & toLen, unsigned & flags)
    {
      flags = m_intraFrame ? PluginCodec_ReturnCoderIFrame : 0;

      PluginCodec_RTP srcRTP(fromPtr, fromLen);
      if(!Unpacketise(srcRTP))
      {
        flags |= PluginCodec_ReturnCoderRequestIFrame;
        m_fullFrame.clear();
        return true;
      }

      if(!srcRTP.GetMarker() || m_fullFrame.empty())
        return true;

      int got_picture_ptr = 0;
      m_pkt.data = &m_fullFrame[0];
      m_pkt.size = m_fullFrame.size();
      int bytesDecoded = avcodec_decode_video2(m_context, m_outputFrame, &got_picture_ptr, &m_pkt);

      if(bytesDecoded < 0 || got_picture_ptr == 0)
      {
        flags |= PluginCodec_ReturnCoderRequestIFrame;
        m_fullFrame.clear();
        return true;
      }
      m_fullFrame.clear();

      PluginCodec_RTP dstRTP(toPtr, toLen);
      toLen = OutputImage(m_outputFrame->data, m_outputFrame->linesize, m_context->width, m_context->height, dstRTP, flags);
      return true;
    }


    void Accumulate(const unsigned char * fragmentPtr, size_t fragmentLen)
    {
      size_t size = m_fullFrame.size();
      m_fullFrame.reserve(size+fragmentLen*2);
      m_fullFrame.resize(size+fragmentLen);
      memcpy(&m_fullFrame[size], fragmentPtr, fragmentLen);
    }

    virtual bool Unpacketise(const PluginCodec_RTP & rtp)
    {
      size_t fragmentLen = rtp.GetPayloadSize();
      if(fragmentLen < 12)
        return false;

      Accumulate(rtp.GetPayloadPtr(), fragmentLen);
      return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static const char   PayloadName[]                     = "MP4V-ES";
static unsigned int PayloadType                       = 0;

static const char   MPEG4_MediaFmt[]                  = "MP4V-ES";
static const char   MPEG4_PROFILE[]                   = "1";
static const char   MPEG4_LEVEL[]                     = "3";
static const char   MPEG4_WIDTH[]                     = "352";
static const char   MPEG4_HEIGHT[]                    = "288";

////////////////////////////////////////////////////////////////////////////////////////////////////

#define MPEG4PLUGIN_CODEC(prefix) \
static const struct PluginCodec_H323GenericParameterDefinition prefix##_h323params[] = \
{ \
    NULL \
}; \
static struct PluginCodec_H323GenericCodecData prefix##_Cap = \
{ \
    OpalPluginCodec_Identifer_MPEG4, \
    0, \
    0, \
    prefix##_h323params \
}; \
static struct PluginCodec_Option const prefix##_FrameWidth = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  PLUGINCODEC_OPTION_FRAME_WIDTH,     /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_AlwaysMerge,            /* Merge mode */ \
  prefix##_WIDTH,                     /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
}; \
static struct PluginCodec_Option const prefix##_FrameHeight = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  PLUGINCODEC_OPTION_FRAME_HEIGHT,    /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  prefix##_HEIGHT,                    /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
}; \
static struct PluginCodec_Option const prefix##_Profile = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "profile",                          /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  prefix##_PROFILE,                   /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
}; \
static struct PluginCodec_Option const prefix##_Level = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "level",                            /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  prefix##_LEVEL,                     /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
}; \
static struct PluginCodec_Option const prefix##_DecoderConfig = \
{ \
  PluginCodec_StringOption,           /* Option type */ \
  "config",                           /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_NoMerge,                /* Merge mode */ \
  "",                                 /* Initial value */ \
  "config",                           /* FMTP option name */ \
  ""                                  /* FMTP default value */ \
}; \
static struct PluginCodec_Option const * prefix##_OptionTable[] = \
{ \
  &prefix##_FrameWidth, \
  &prefix##_FrameHeight, \
  &prefix##_Profile, \
  &prefix##_Level, \
  &prefix##_DecoderConfig, \
  NULL \
}; \
class prefix##_VideoFormat : public VideoFormat \
{ \
  public: \
    prefix##_VideoFormat() \
      : VideoFormat(prefix##_MediaFmt, PayloadName, prefix##_MediaFmt, prefix##_OptionTable) \
    { \
      m_payloadType = PayloadType; \
      m_h323CapabilityType = PluginCodec_H323Codec_generic; \
      m_h323CapabilityData = (struct PluginCodec_H323GenericCodecData *)&prefix##_Cap; \
    } \
}; \
static prefix##_VideoFormat prefix##_MediaFormatInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////

MPEG4PLUGIN_CODEC(MPEG4);

DECLARE_CONTROLS_TABLE(Encoder, Decoder);

///////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition CodecDefinition[] =
{
  PLUGINCODEC_VIDEO_CODEC_CXX(MPEG4_MediaFormatInfo, Encoder, Decoder)
};

PLUGIN_CODEC_IMPLEMENT_CXX(CODEC, CodecDefinition);


/////////////////////////////////////////////////////////////////////////////
