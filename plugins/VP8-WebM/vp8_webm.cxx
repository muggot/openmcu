/*
 * VP8 (WebM) video codec Plugin codec for OPAL
 *
 * Copyright (C) 2012 Vox Lucida Pty Ltd, All Rights Reserved
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
 * Contributor(s): Robert Jongbloed (robertj@voxlucida.com.au)
 *
 * $Revision: 29380 $
 * $Author: rjongbloed $
 * $Date: 2013-03-29 04:58:30 +0400 (Пт, 29 мар 2013) $
 */

#ifndef PLUGIN_CODEC_DLL_EXPORTS
#include "plugin-config.h"
#endif

#include <codec/opalplugin.hpp>

#include <vector>

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

#define VPX_CODEC_DISABLE_COMPAT 0
#include <vpx/vpx_encoder.h>
#include <vpx/vp8cx.h>
#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>


//#define INCLUDE_OM_CUSTOM_PACKETIZATION 1

#define MY_CODEC_LOG "VP8"
class VP8_CODEC { };

PLUGINCODEC_CONTROL_LOG_FUNCTION_DEF


PLUGINCODEC_LICENSE(
  "Robert Jongbloed, Vox Lucida Pty.Ltd.",                      // source code author
  "1.0",                                                        // source code version
  "robertj@voxlucida.com.au",                                   // source code email
  "http://www.voxlucida.com.au",                                // source code URL
  "Copyright (C) 2011 by Vox Lucida Pt.Ltd., All Rights Reserved", // source code copyright
  "MPL 1.0",                                                    // source code license
  PluginCodec_License_MPL,                                      // source code license
  
  "VP8 Video Codec",                                            // codec description
  "James Bankoski, John Koleszar, Lou Quillio, Janne Salonen, "
  "Paul Wilkins, Yaowu Xu, all from Google Inc.",               // codec author
  "8", 							        // codec version
  "jimbankoski@google.com, jkoleszar@google.com, "
  "louquillio@google.com, jsalonen@google.com, "
  "paulwilkins@google.com, yaowu@google.com",                   // codec email
  "http://google.com",                                          // codec URL
  "Copyright (c) 2010, 2011, Google Inc.",                      // codec copyright information
  "Copyright (c) 2010, 2011, Google Inc.",                      // codec license
  PluginCodec_License_BSD                                       // codec license code
);


///////////////////////////////////////////////////////////////////////////////

const unsigned MaxBitRate = 4000000;

static struct PluginCodec_Option const MaxFR =
{
  PluginCodec_IntegerOption,          // Option type
  "max-fr",                           // User visible name
  false,                              // User Read/Only flag
  PluginCodec_MinMerge,               // Merge mode
  "30",                               // Initial value
  "max-fr",                           // FMTP option name
  "30",                               // FMTP default value
  0,                                  // H.245 generic capability code and bit mask
  "1",                                // Minimum value
  "30"                                // Maximum value
};

static struct PluginCodec_Option const MaxFS =
{
  PluginCodec_IntegerOption,          // Option type
  "max-fs",                           // User visible name
  false,                              // User Read/Only flag
  PluginCodec_MinMerge,               // Merge mode
  "0",                                // Initial value
  "max-fs",                           // FMTP option name
  "0",                                // FMTP default value
  0,                                  // H.245 generic capability code and bit mask
  "48",                               // Minimum value
  "65535"                             // Maximum value
};

#if INCLUDE_OM_CUSTOM_PACKETIZATION
static struct PluginCodec_Option const MaxFrameSizeOM =
{
  PluginCodec_StringOption,           // Option type
  "SIP/SDP Max Frame Size",           // User visible name
  true,                               // User Read/Only flag
  PluginCodec_NoMerge,                // Merge mode
  "",                                 // Initial value
  "x-mx-max-size",                    // FMTP option name
  ""                                  // FMTP default value (as per RFC)
};
#endif

static struct PluginCodec_Option const TemporalSpatialTradeOff =
{
  PluginCodec_IntegerOption,          // Option type
  PLUGINCODEC_OPTION_TEMPORAL_SPATIAL_TRADE_OFF, // User visible name
  false,                              // User Read/Only flag
  PluginCodec_AlwaysMerge,            // Merge mode
  "31",                               // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  0,                                  // H.245 generic capability code and bit mask
  "1",                                // Minimum value
  "31"                                // Maximum value
};

static struct PluginCodec_Option const SpatialResampling =
{
  PluginCodec_BoolOption,             // Option type
  "Spatial Resampling",               // User visible name
  false,                              // User Read/Only flag
  PluginCodec_AndMerge,               // Merge mode
  "0",                                // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  0                                   // H.245 generic capability code and bit mask
};

static struct PluginCodec_Option const SpatialResamplingUp =
{
  PluginCodec_IntegerOption,          // Option type
  "Spatial Resampling Up",            // User visible name
  false,                              // User Read/Only flag
  PluginCodec_AlwaysMerge,            // Merge mode
  "100",                              // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  0,                                  // H.245 generic capability code and bit mask
  "0",                                // Minimum value
  "100"                               // Maximum value
};

static struct PluginCodec_Option const SpatialResamplingDown =
{
  PluginCodec_IntegerOption,          // Option type
  "Spatial Resampling Down",          // User visible name
  false,                              // User Read/Only flag
  PluginCodec_AlwaysMerge,            // Merge mode
  "0",                                // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  0,                                  // H.245 generic capability code and bit mask
  "0",                                // Minimum value
  "100"                               // Maximum value
};

static struct PluginCodec_Option const PictureID =
{
  PluginCodec_EnumOption,             // Option type
  "Picture ID Size",                  // User visible name
  false,                              // User Read/Only flag
  PluginCodec_AlwaysMerge,            // Merge mode
  "None",                             // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  0,                                  // H.245 generic capability code and bit mask
  "None:Byte:Word"                    // Enumeration
};

static struct PluginCodec_Option const OutputPartition =
{
  PluginCodec_BoolOption,             // Option type
  "Output Partition",                 // User visible name
  false,                              // User Read/Only flag
  PluginCodec_AndMerge,               // Merge mode
  "1",                                // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  0                                   // H.245 generic capability code and bit mask
};

static struct PluginCodec_Option const * OptionTableRFC[] = {
  &MaxFR,
  &MaxFS,
  &PictureID,
  &OutputPartition,
  &TemporalSpatialTradeOff,
  &SpatialResampling,
  &SpatialResamplingUp,
  &SpatialResamplingDown,
  NULL
};


#if INCLUDE_OM_CUSTOM_PACKETIZATION
static struct PluginCodec_Option const * OptionTableOM[] = {
  &MaxFrameSizeOM,
  &TemporalSpatialTradeOff,
  &SpatialResampling,
  &SpatialResamplingUp,
  &SpatialResamplingDown,
  NULL
};
#endif


///////////////////////////////////////////////////////////////////////////////

class VP8Format : public PluginCodec_VideoFormat<VP8_CODEC>
{
  private:
    typedef PluginCodec_VideoFormat<VP8_CODEC> BaseClass;

  protected:
    bool m_partition;

  public:
    VP8Format(const char * formatName, const char * payloadName, const char * description, OptionsTable options)
      : BaseClass(formatName, payloadName, description, MaxBitRate, options)
      , m_partition(false)
    {
#ifdef VPX_CODEC_USE_ERROR_CONCEALMENT
      if ((vpx_codec_get_caps(vpx_codec_vp8_dx()) & VPX_CODEC_CAP_ERROR_CONCEALMENT) != 0)
        m_flags |= PluginCodec_ErrorConcealment; // Prevent video update request on packet loss
#endif
#ifdef VPX_CODEC_CAP_OUTPUT_PARTITION
      if ((vpx_codec_get_caps(vpx_codec_vp8_dx()) & VPX_CODEC_CAP_OUTPUT_PARTITION) != 0)
        m_partition = true;
#endif
    }
};


class VP8FormatRFC : public VP8Format
{
  public:
    VP8FormatRFC()
      : VP8Format("VP8-WebM", "VP8", "VP8 Video Codec (RFC)", OptionTableRFC)
    {
    }


    virtual bool ToNormalised(OptionMap & original, OptionMap & changed)
    {
      OptionMap::iterator it = original.find(MaxFS.m_name);
      if (it != original.end() && !it->second.empty()) {
        unsigned maxFrameSize = String2Unsigned(it->second);
        ClampResolution(m_maxWidth, m_maxHeight, maxFrameSize);
        Change(maxFrameSize,  original, changed, MaxFS.m_name);
        ClampMax(m_maxWidth,  original, changed, PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH);
        ClampMax(m_maxHeight, original, changed, PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT);
        ClampMax(m_maxWidth,  original, changed, PLUGINCODEC_OPTION_MIN_RX_FRAME_WIDTH);
        ClampMax(m_maxHeight, original, changed, PLUGINCODEC_OPTION_MIN_RX_FRAME_HEIGHT);
      }

      it = original.find(MaxFR.m_name);
      if (it != original.end() && !it->second.empty())
        ClampMin(PLUGINCODEC_VIDEO_CLOCK/String2Unsigned(it->second), original, changed, PLUGINCODEC_OPTION_FRAME_TIME);

      if (!m_partition)
        ClampMax(0, original, changed, OutputPartition.m_name);

      return true;
    }


    virtual bool ToCustomised(OptionMap & original, OptionMap & changed)
    {
      Change(GetMacroBlocks(original.GetUnsigned(PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH),
                            original.GetUnsigned(PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT)),
             original, changed, MaxFS.m_name);

      Change(PLUGINCODEC_VIDEO_CLOCK/original.GetUnsigned(PLUGINCODEC_OPTION_FRAME_TIME),
             original, changed, MaxFR.m_name);

      if (!m_partition)
        ClampMax(0, original, changed, OutputPartition.m_name);

      return true;
    }
};

static VP8FormatRFC VP8MediaFormatInfoRFC;


///////////////////////////////////////////////////////////////////////////////

#if INCLUDE_OM_CUSTOM_PACKETIZATION

class VP8FormatOM : public VP8Format
{
  public:
    VP8FormatOM()
      : VP8Format("VP8-OM", "X-MX-VP8", "VP8 Video Codec (Open Market)", OptionTableOM)
    {
    }


    virtual bool ToNormalised(OptionMap & original, OptionMap & changed)
    {
      OptionMap::iterator it = original.find(MaxFrameSizeOM.m_name);
      if (it != original.end() && !it->second.empty()) {
        std::stringstream strm(it->second);
        unsigned maxWidth, maxHeight;
        char x;
        strm >> maxWidth >> x >> maxHeight;
        if (maxWidth < 32 || maxHeight < 32) {
          PTRACE(1, MY_CODEC_LOG, "Invalid " << MaxFrameSizeOM.m_name << ", was \"" << it->second << '"');
          return false;
        }
        ClampMax(maxWidth,  original, changed, PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH);
        ClampMax(maxHeight, original, changed, PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT);
        ClampMax(maxWidth,  original, changed, PLUGINCODEC_OPTION_MIN_RX_FRAME_WIDTH);
        ClampMax(maxHeight, original, changed, PLUGINCODEC_OPTION_MIN_RX_FRAME_HEIGHT);
      }
      return true;
    }


    virtual bool ToCustomised(OptionMap & original, OptionMap & changed)
    {
      unsigned maxWidth = original.GetUnsigned(PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH);
      unsigned maxHeight = original.GetUnsigned(PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT);
      std::stringstream strm;
      strm << maxWidth << 'x' << maxHeight;
      Change(strm.str().c_str(), original, changed, MaxFrameSizeOM.m_name);
      return true;
    }
};

static VP8FormatOM VP8MediaFormatInfoOM;

#endif // INCLUDE_OM_CUSTOM_PACKETIZATION


///////////////////////////////////////////////////////////////////////////////

static bool IsError(vpx_codec_err_t err, const char * fn)
{
  if (err == VPX_CODEC_OK)
    return false;

  PTRACE(1, MY_CODEC_LOG, "Error " << err << " in " << fn << " - " << vpx_codec_err_to_string(err));
  return true;
}

#define IS_ERROR(fn, args) IsError(fn args, #fn)


enum Orientation {
  PortraitLeft,
  LandscapeUp,
  PortraitRight,
  LandscapeDown,
  OrientationMask = 3,

  OrientationExtHdrShift = 4,
  OrientationPktHdrShift = 5
};
#include <iostream>


///////////////////////////////////////////////////////////////////////////////

class VP8Encoder : public PluginVideoEncoder<VP8_CODEC>
{
  private:
    typedef PluginVideoEncoder<VP8_CODEC> BaseClass;

  protected:
    vpx_codec_enc_cfg_t        m_config;
    unsigned                   m_initFlags;
    vpx_codec_ctx_t            m_codec;
    vpx_codec_iter_t           m_iterator;
    const vpx_codec_cx_pkt_t * m_packet;
    size_t                     m_offset;
    unsigned                   m_encodingQuality;

  public:
    VP8Encoder(const PluginCodec_Definition * defn)
      : BaseClass(defn)
      , m_initFlags(0)
      , m_iterator(NULL)
      , m_packet(NULL)
      , m_offset(0)
      , m_encodingQuality(30)
    {
      memset(&m_codec, 0, sizeof(m_codec));
    }


    ~VP8Encoder()
    {
      vpx_codec_destroy(&m_codec);
    }


    virtual bool Construct()
    {
      if (IS_ERROR(vpx_codec_enc_config_default, (vpx_codec_vp8_cx(), &m_config, 0)))
        return false;

      m_maxBitRate = m_config.rc_target_bitrate*1000;

      m_config.g_w = 0; // Forces OnChangedOptions to initialise encoder
      m_config.g_h = 0;
      m_config.g_error_resilient = 1;
      m_config.g_pass = VPX_RC_ONE_PASS;
      m_config.g_lag_in_frames = 0;
      m_config.rc_end_usage = VPX_CBR;
      m_config.g_timebase.num = 1;
      m_config.g_timebase.den = PLUGINCODEC_VIDEO_CLOCK;

      if (!OnChangedOptions())
        return false;

      PTRACE(4, MY_CODEC_LOG, "Encoder opened: " << vpx_codec_version_str() << ", revision $Revision$");
      return true;
    }


    virtual bool SetOption(const char * optionName, const char * optionValue)
    {
      if (strcasecmp(optionName, "Encoding Quality") == 0)
        return SetOptionUnsigned(m_encodingQuality, optionValue, 1, 30);

      if (strcasecmp(optionName, PLUGINCODEC_OPTION_MAX_BIT_RATE) == 0)
        return SetOptionUnsigned(m_maxBitRate, optionValue, 1, m_definition->bitsPerSec);

      //if (strcasecmp(optionName, PLUGINCODEC_OPTION_TARGET_BIT_RATE) == 0)
      //  return SetOptionUnsigned(m_maxBitRate, optionValue, 1, m_definition->bitsPerSec);

      if (strcasecmp(optionName, SpatialResampling.m_name) == 0)
        return SetOptionBoolean(m_config.rc_resize_allowed, optionValue);

      if (strcasecmp(optionName, SpatialResamplingUp.m_name) == 0)
        return SetOptionUnsigned(m_config.rc_resize_up_thresh, optionValue, 0, 100);

      if (strcasecmp(optionName, SpatialResamplingDown.m_name) == 0)
        return SetOptionUnsigned(m_config.rc_resize_down_thresh, optionValue, 0, 100);

      return BaseClass::SetOption(optionName, optionValue);
    }


    virtual bool OnChangedOptions()
    {
      m_config.kf_mode = VPX_KF_AUTO;
      if (m_keyFramePeriod != 0)
        m_config.kf_min_dist = m_config.kf_max_dist = m_keyFramePeriod;
      else {
        m_config.kf_min_dist = 0;
        m_config.kf_max_dist = 10*PLUGINCODEC_VIDEO_CLOCK/m_frameTime;  // No slower than once every 10 seconds
      }

      m_config.rc_target_bitrate = m_maxBitRate/1000;

      // Take simple temporal/spatial trade off and set multiple variables
      m_config.rc_dropframe_thresh = (31-m_tsto)*2; // m_tsto==31 is maintain frame rate, so threshold is zero
      m_config.rc_resize_allowed = m_tsto < 16;
      m_config.rc_max_quantizer = 32 + m_tsto;

      if (m_config.g_w == m_width && m_config.g_h == m_height)
        return !IS_ERROR(vpx_codec_enc_config_set, (&m_codec, &m_config));

      m_config.g_w = m_width;
      m_config.g_h = m_height;
      vpx_codec_destroy(&m_codec);
      return !IS_ERROR(vpx_codec_enc_init, (&m_codec, vpx_codec_vp8_cx(), &m_config, m_initFlags));
    }


    bool NeedEncode()
    {
      if (m_packet != NULL)
        return false;

      while ((m_packet = vpx_codec_get_cx_data(&m_codec, &m_iterator)) != NULL) {
        if (m_packet->kind == VPX_CODEC_CX_FRAME_PKT)
          return false;
      }

      m_iterator = NULL;
      return true;
    }


    virtual bool Transcode(const void * fromPtr,
                             unsigned & fromLen,
                                 void * toPtr,
                             unsigned & toLen,
                             unsigned & flags)
    {
      while (NeedEncode()) {
        PluginCodec_RTP srcRTP(fromPtr, fromLen);
        PluginCodec_Video_FrameHeader * video = srcRTP.GetVideoHeader();

        if (m_width != video->width || m_height != video->height) {
          PTRACE(4, MY_CODEC_LOG, "Changing resolution from " <<
                 m_width << 'x' << m_height << " to " << video->width << 'x' << video->height);
          m_width = video->width;
          m_height = video->height;
          OnChangedOptions();
        }

        vpx_image_t image;
        vpx_img_wrap(&image, VPX_IMG_FMT_I420, video->width, video->height, 4, srcRTP.GetVideoFrameData());

        if(m_encodingQuality != 0)
          vpx_codec_control(&m_codec, VP8E_SET_CPUUSED, 15-(m_encodingQuality/2));

        if (IS_ERROR(vpx_codec_encode, (&m_codec, &image,
                                        srcRTP.GetTimestamp(), m_frameTime,
                                        (flags&PluginCodec_CoderForceIFrame) != 0 ? VPX_EFLAG_FORCE_KF : 0,
                                        VPX_DL_REALTIME)))
          return false;
      }

      flags = 0;
      if ((m_packet->data.frame.flags&VPX_FRAME_IS_KEY) != 0)
        flags |= PluginCodec_ReturnCoderIFrame;

      PluginCodec_RTP dstRTP(toPtr, toLen);
      Packetise(dstRTP);
      toLen = dstRTP.GetPacketSize();

      if (m_offset >= m_packet->data.frame.sz) {
        flags |= PluginCodec_ReturnCoderLastFrame;
        dstRTP.SetMarker(true);
        m_packet = NULL;
        m_offset = 0;
      }
      else
        dstRTP.SetMarker(false);

      return true;
    }


    virtual void Packetise(PluginCodec_RTP & rtp) = 0;
};


class VP8EncoderRFC : public VP8Encoder
{
  protected:
    unsigned m_pictureId;
    unsigned m_pictureIdSize;

  public:
    VP8EncoderRFC(const PluginCodec_Definition * defn)
      : VP8Encoder(defn)
      , m_pictureId(rand()&0x7fff)
      , m_pictureIdSize(0)
    {
    }


    virtual bool SetOption(const char * optionName, const char * optionValue)
    {
      if (strcasecmp(optionName, PictureID.m_name) == 0) {
        if (strcasecmp(optionValue, "Byte") == 0) {
          if (m_pictureIdSize != 0x80) {
            m_pictureIdSize = 0x80;
            m_optionsSame = false;
          }
          return true;
        }

        if (strcasecmp(optionValue, "Word") == 0) {
          if (m_pictureIdSize != 0x8000) {
            m_pictureIdSize = 0x8000;
            m_optionsSame = false;
          }
          return true;
        }

        if (strcasecmp(optionValue, "None") == 0) {
          if (m_pictureIdSize != 0) {
            m_pictureIdSize = 0;
            m_optionsSame = false;
          }
          return true;
        }

        PTRACE(2, MY_CODEC_LOG, "Unknown picture ID size: \"" << optionValue << '"');
        return false;
      }

#ifdef VPX_CODEC_USE_OUTPUT_PARTITION
      if (strcasecmp(optionName, OutputPartition.m_name) == 0)
        return SetOptionBit(m_initFlags, VPX_CODEC_USE_OUTPUT_PARTITION, optionValue);
#endif

      return VP8Encoder::SetOption(optionName, optionValue);
    }


    virtual void Packetise(PluginCodec_RTP & rtp)
    {
      size_t headerSize = 1;

      rtp[0] = 0;

      if (m_offset == 0)
        rtp[0] |= 0x10; // Add S bit if start of partition

#ifdef VPX_CODEC_USE_OUTPUT_PARTITION
      if (m_packet->data.frame.partition_id >= 0)
        rtp[0] |= (uint8_t)(m_packet->data.frame.partition_id&0x0f);
#endif

      if ((m_packet->data.frame.flags&VPX_FRAME_IS_DROPPABLE) != 0)
        rtp[0] |= 0x20; // Add N bit for non-reference frame

      if (m_pictureIdSize > 0) {
        headerSize += 2;
        rtp[0] |= 0x80; // Add X bit for X (extension) bit mask byte
        rtp[1] |= 0x80; // Add I bit for picture ID
        if (m_pictureId < 128)
          rtp[2] = (uint8_t)m_pictureId;
        else {
          ++headerSize;
          rtp[2] = (uint8_t)(0x80 | (m_pictureId >> 8));
          rtp[3] = (uint8_t)m_pictureId;
        }

        if (m_offset == 0 && ++m_pictureId >= m_pictureIdSize)
          m_pictureId = 0;
      }

      size_t fragmentSize = GetPacketSpace(rtp, m_packet->data.frame.sz - m_offset + headerSize) - headerSize;
      rtp.CopyPayload((char *)m_packet->data.frame.buf+m_offset, fragmentSize, headerSize);
      m_offset += fragmentSize;
    }
};


#if INCLUDE_OM_CUSTOM_PACKETIZATION

class VP8EncoderOM : public VP8Encoder
{
  protected:
    unsigned m_currentGID;

  public:
    VP8EncoderOM(const PluginCodec_Definition * defn)
      : VP8Encoder(defn)
      , m_currentGID(0)
    {
    }


    virtual void Packetise(PluginCodec_RTP & rtp)
    {
      size_t headerSize;
      if (m_offset != 0) {
        headerSize = 1;
        rtp[0] = (uint8_t)m_currentGID; // No start bit or header extension bit
      }
      else {
        headerSize = 2;

        rtp[0] = 0x40; // Start bit

        unsigned type = UINT_MAX;
        size_t len;
        unsigned char * ext = rtp.GetExtendedHeader(type, len);
        Orientation orientation = type == 0x10001 ? (Orientation)(*ext >> OrientationExtHdrShift) : LandscapeUp;
        rtp[1] = (unsigned char)(orientation << OrientationPktHdrShift);

        if ((m_packet->data.frame.flags&VPX_FRAME_IS_KEY) != 0) {
          rtp[1] |= 0x80; // Indicate is golden frame
          m_currentGID = (m_currentGID+1)&0x3f;
        }

        rtp[0] |= m_currentGID;
      }

      size_t fragmentSize = GetPacketSpace(rtp, m_packet->data.frame.sz - m_offset + headerSize) - headerSize;
      rtp.CopyPayload((char *)m_packet->data.frame.buf+m_offset, fragmentSize, headerSize);
      m_offset += fragmentSize;
    }
};

#endif //INCLUDE_OM_CUSTOM_PACKETIZATION


///////////////////////////////////////////////////////////////////////////////

class VP8Decoder : public PluginVideoDecoder<VP8_CODEC>
{
  private:
    typedef PluginVideoDecoder<VP8_CODEC> BaseClass;

  protected:
    vpx_codec_iface_t  * m_iface;
    vpx_codec_ctx_t      m_codec;
    vpx_codec_flags_t    m_flags;
    vpx_codec_iter_t     m_iterator;
    std::vector<uint8_t> m_fullFrame;
    bool                 m_intraFrame;
    bool                 m_ignoreTillKeyFrame;

  public:
    VP8Decoder(const PluginCodec_Definition * defn)
      : BaseClass(defn)
      , m_iface(vpx_codec_vp8_dx())
      , m_flags(0)
      , m_iterator(NULL)
      , m_intraFrame(false)
      , m_ignoreTillKeyFrame(false)
    {
      memset(&m_codec, 0, sizeof(m_codec));
      m_fullFrame.reserve(10000);

#ifdef VPX_CODEC_USE_ERROR_CONCEALMENT
      if ((vpx_codec_get_caps(m_iface) & VPX_CODEC_CAP_ERROR_CONCEALMENT) != 0)
        m_flags |= VPX_CODEC_USE_ERROR_CONCEALMENT;
#endif
    }


    ~VP8Decoder()
    {
      vpx_codec_destroy(&m_codec);
    }


    virtual bool Construct()
    {
      if (IS_ERROR(vpx_codec_dec_init, (&m_codec, m_iface, NULL, m_flags)))
        return false;

      PTRACE(4, MY_CODEC_LOG, "Decoder opened: " << vpx_codec_version_str() << ", revision $Revision$");
      return true;
    }


    virtual bool Transcode(const void * fromPtr,
                             unsigned & fromLen,
                                 void * toPtr,
                             unsigned & toLen,
                             unsigned & flags)
    {
      vpx_image_t * image;

      flags = m_intraFrame ? PluginCodec_ReturnCoderIFrame : 0;

      if ((image = vpx_codec_get_frame(&m_codec, &m_iterator)) == NULL) {

        PluginCodec_RTP srcRTP(fromPtr, fromLen);
        if (!Unpacketise(srcRTP)) {
          flags |= PluginCodec_ReturnCoderRequestIFrame;
          m_fullFrame.clear();
          return true;
        }

        if (!srcRTP.GetMarker() || m_fullFrame.empty())
          return true;

        vpx_codec_err_t err = vpx_codec_decode(&m_codec, &m_fullFrame[0], m_fullFrame.size(), NULL, 0);
        switch (err) {
          case VPX_CODEC_OK :
            break;
          // Non fatal errors
          case VPX_CODEC_UNSUP_BITSTREAM :
          case VPX_CODEC_CORRUPT_FRAME :
            IsError(err, "vpx_codec_decode");
            flags |= PluginCodec_ReturnCoderRequestIFrame;
            m_fullFrame.clear();
            m_ignoreTillKeyFrame = true;
            return true;
          case VPX_CODEC_UNSUP_FEATURE :
          default :
            IsError(err, "vpx_codec_decode");
            return true;
        }
#if 1
        /* Prefer to use vpx_codec_get_stream_info() here, but it doesn't
           work, it always returns key frame! The vpx_codec_peek_stream_info()
           function is also useless, it return VPX_CODEC_UNSUP_FEATURE for
           anything that isn't an I-Frame. Luckily it appears that the low bit
           of the first byte is the I-Frame indicator.
        */
        if ((m_fullFrame[0]&1) == 0) {
          flags |= PluginCodec_ReturnCoderIFrame;
          m_intraFrame = true;
        }
#else
        vpx_codec_stream_info_t info;
        info.sz = sizeof(info);
        if (IS_ERROR(vpx_codec_get_stream_info, (&m_codec, &info)))
          flags |= PluginCodec_ReturnCoderRequestIFrame;

        if (info.is_kf) {
          flags |= PluginCodec_ReturnCoderIFrame;
          m_intraFrame = true;
        }
#endif

        m_fullFrame.clear();

        m_iterator = NULL;
        if ((image = vpx_codec_get_frame(&m_codec, &m_iterator)) == NULL)
          return true;
      }

      if (image->fmt != VPX_IMG_FMT_I420) {
        PTRACE(1, MY_CODEC_LOG, "Unsupported image format from decoder.");
        return false;
      }

      PluginCodec_RTP dstRTP(toPtr, toLen);
      OutputImage(image->planes, image->stride, image->d_w, image->d_h, dstRTP, flags);
      toLen = dstRTP.GetPacketSize();

      if (flags&PluginCodec_ReturnCoderLastFrame)
        m_intraFrame = false;

      return true;
    }


    void Accumulate(const unsigned char * fragmentPtr, size_t fragmentLen)
    {
      if (fragmentLen == 0)
        return;

      size_t size = m_fullFrame.size();
      m_fullFrame.reserve(size+fragmentLen*2);
      m_fullFrame.resize(size+fragmentLen);
      memcpy(&m_fullFrame[size], fragmentPtr, fragmentLen);
    }


    virtual bool Unpacketise(const PluginCodec_RTP & rtp) = 0;
};


class VP8DecoderRFC : public VP8Decoder
{
  public:
    VP8DecoderRFC(const PluginCodec_Definition * defn)
      : VP8Decoder(defn)
    {
    }


    virtual bool Unpacketise(const PluginCodec_RTP & rtp)
    {
      if (rtp.GetPayloadSize() == 0)
        return true;

      if (rtp.GetPayloadSize() < 3)
        return false;

      size_t headerSize = 1;
      if ((rtp[0]&0x80) != 0) { // Check X bit
        ++headerSize;           // Allow for X byte

        if ((rtp[1]&0x80) != 0) { // Check I bit
          ++headerSize;           // Allow for I field
          if ((rtp[2]&0x80) != 0) // > 7 bit picture ID
            ++headerSize;         // Allow for extra bits of I field
        }

        if ((rtp[1]&0x40) != 0) // Check L bit
          ++headerSize;         // Allow for L byte

        if ((rtp[1]&0x30) != 0) // Check T or K bit
          ++headerSize;         // Allow for T/K byte
      }

/*
      if ((rtp[0]&0x10) != 0 && !m_fullFrame.empty()) {
        PTRACE(3, MY_CODEC_LOG, "Missing start to frame, ignoring till next key frame.");
        m_ignoreTillKeyFrame = true;
        return false;
      }
*/

      Accumulate(rtp.GetPayloadPtr()+headerSize, rtp.GetPayloadSize()-headerSize);
      return true;
    }
};


#if INCLUDE_OM_CUSTOM_PACKETIZATION

class VP8DecoderOM : public VP8Decoder
{
  protected:
    unsigned m_expectedGID;
    Orientation m_orientation;

  public:
    VP8DecoderOM(const PluginCodec_Definition * defn)
      : VP8Decoder(defn)
      , m_expectedGID(UINT_MAX)
      , m_orientation(LandscapeUp)
    {
    }


    struct RotatePlaneInfo : public OutputImagePlaneInfo
    {
      RotatePlaneInfo(unsigned width, unsigned height, int raster, unsigned char * src, unsigned char * dst)
      {
        m_width = width;
        m_height = height;
        m_raster = raster;
        m_source = src;
        m_destination = dst;
      }

      void CopyPortraitLeft()
      {
        for (int y = m_height-1; y >= 0; --y) {
          unsigned char * src = m_source;
          unsigned char * dst = m_destination + y;
          for (int x = m_width; x > 0; --x) {
            *dst = *src++;
            dst += m_height;
          }
          m_source += m_raster;
        }
      }

      void CopyPortraitRight()
      {
        m_destination += m_width*m_height;
        for (int y = m_height; y > 0; --y) {
          unsigned char * src = m_source;
          unsigned char * dst = m_destination - y;
          for (int x = m_width; x > 0; --x) {
            *dst = *src++;
            dst -= m_height;
          }
          m_source += m_raster;
        }
      }

      void CopyLandscapeDown()
      {
        m_destination += m_width*(m_height-1);
        for (int y = m_height; y > 0; --y) {
          memcpy(m_destination, m_source, m_width);
          m_source += m_raster;
          m_destination -= m_width;
        }
      }
    };

    virtual unsigned OutputImage(unsigned char * planes[3], int raster[3],
                                 unsigned width, unsigned height, PluginCodec_RTP & rtp, unsigned & flags)
    {
      unsigned type;
      size_t len;
      unsigned char * ext = rtp.GetExtendedHeader(type, len);
      if (ext != NULL && type == 0x10001) {
        *ext = (unsigned char)(m_orientation << OrientationExtHdrShift);
        return VP8Decoder::OutputImage(planes, raster, width, height, rtp, flags);
      }

      switch (m_orientation) {
        case PortraitLeft :
        case PortraitRight :
          if (CanOutputImage(height, width, rtp, flags))
            break;
          return 0;

        case LandscapeUp :
        case LandscapeDown :
          if (CanOutputImage(width, height, rtp, flags))
            break;
          return 0;

        default :
          return 0;
      }

      RotatePlaneInfo planeInfo[3] = {
        RotatePlaneInfo(width,   height,   raster[0], planes[0], rtp.GetVideoFrameData()),
        RotatePlaneInfo(width/2, height/2, raster[1], planes[1], planeInfo[0].m_destination + width*height),
        RotatePlaneInfo(width/2, height/2, raster[2], planes[2], planeInfo[1].m_destination + width*height/4)
      };

      for (unsigned p = 0; p < 3; ++p) {
        switch (m_orientation) {
          case PortraitLeft :
            planeInfo[p].CopyPortraitLeft();
            break;

          case LandscapeUp :
            planeInfo[p].Copy();
            break;

          case PortraitRight :
            planeInfo[p].CopyPortraitRight();
            break;

          case LandscapeDown :
            planeInfo[p].CopyLandscapeDown();
            break;
        }
      }

      return rtp.GetPacketSize();
    }


    virtual bool Unpacketise(const PluginCodec_RTP & rtp)
    {
      size_t paylaodSize = rtp.GetPayloadSize();
      if (paylaodSize < 2)
        return paylaodSize == 0;

      bool first = (rtp[0]&0x40) != 0;
      if (first)
        m_orientation = (Orientation)((rtp[1] >> OrientationPktHdrShift) & OrientationMask);

      size_t headerSize = first ? 2 : 1;
      if ((rtp[0]&0x80) != 0) {
        while ((rtp[headerSize]&0x80) != 0)
          ++headerSize;
        ++headerSize;
      }

      if (m_ignoreTillKeyFrame) {
        if (!first || (rtp[headerSize]&1) != 0)
          return false;
        m_ignoreTillKeyFrame =  false;
        PTRACE(3, MY_CODEC_LOG, "Found next start of key frame.");
      }
      else {
        if (!first && m_fullFrame.empty()) {
          PTRACE(3, MY_CODEC_LOG, "Missing start to frame, ignoring till next key frame.");
          m_ignoreTillKeyFrame = true;
          return false;
        }
      }

      Accumulate(rtp.GetPayloadPtr()+headerSize, paylaodSize-headerSize);

      unsigned gid = rtp[0]&0x3f;
      bool expected = m_expectedGID == UINT_MAX || m_expectedGID == gid;
      m_expectedGID = gid;
      return expected || first;
    }
};

#endif //INCLUDE_OM_CUSTOM_PACKETIZATION


///////////////////////////////////////////////////////////////////////////////
// Xak
///////////////////////////////////////////////////////////////////////////////
#define OpalPluginCodec_Identifer_VP8       "1.3.6.1.4.1.17091.1.9"

#define OpalPluginCodec_Identifer_VP8_QCIF  "1.3.6.1.4.1.17091.1.9.0"
#define VP8_QCIF_MediaFmt                   "VP8-QCIF"
#define VP8_QCIF_WIDTH                      "176"
#define VP8_QCIF_HEIGHT                     "144"
#define VP8_QCIF_TARGETBITRATE              "64000"

#define OpalPluginCodec_Identifer_VP8_CIF   "1.3.6.1.4.1.17091.1.9.1"
#define VP8_CIF_MediaFmt                    "VP8-CIF"
#define VP8_CIF_WIDTH                       "352"
#define VP8_CIF_HEIGHT                      "288"
#define VP8_CIF_TARGETBITRATE               "64000"

#define OpalPluginCodec_Identifer_VP8_4CIF  "1.3.6.1.4.1.17091.1.9.2"
#define VP8_4CIF_MediaFmt                   "VP8-4CIF"
#define VP8_4CIF_WIDTH                      "704"
#define VP8_4CIF_HEIGHT                     "576"
#define VP8_4CIF_TARGETBITRATE              "128000"

#define OpalPluginCodec_Identifer_VP8_240P  "1.3.6.1.4.1.17091.1.9.21"
#define VP8_240P_MediaFmt                   "VP8-240P"
#define VP8_240P_WIDTH                      "424"
#define VP8_240P_HEIGHT                     "240"
#define VP8_240P_TARGETBITRATE              "60000"

#define OpalPluginCodec_Identifer_VP8_360P  "1.3.6.1.4.1.17091.1.9.22"
#define VP8_360P_MediaFmt                   "VP8-360P"
#define VP8_360P_WIDTH                      "640"
#define VP8_360P_HEIGHT                     "360"
#define VP8_360P_TARGETBITRATE              "80000"

#define OpalPluginCodec_Identifer_VP8_480P  "1.3.6.1.4.1.17091.1.9.10"
#define VP8_480P_MediaFmt                   "VP8-480P"
#define VP8_480P_WIDTH                      "852"
#define VP8_480P_HEIGHT                     "480"
#define VP8_480P_TARGETBITRATE              "128000"

#define OpalPluginCodec_Identifer_VP8_720P  "1.3.6.1.4.1.17091.1.9.11"
#define VP8_720P_MediaFmt                   "VP8-720P"
#define VP8_720P_WIDTH                      "1280"
#define VP8_720P_HEIGHT                     "720"
#define VP8_720P_TARGETBITRATE              "256000"

#define OpalPluginCodec_Identifer_VP8_768P  "1.3.6.1.4.1.17091.1.9.23"
#define VP8_768P_MediaFmt                   "VP8-768P"
#define VP8_768P_WIDTH                      "1364"
#define VP8_768P_HEIGHT                     "768"
#define VP8_768P_TARGETBITRATE              "200000"

#define OpalPluginCodec_Identifer_VP8_1080P "1.3.6.1.4.1.17091.1.9.12"
#define VP8_1080P_MediaFmt                  "VP8-1080P"
#define VP8_1080P_WIDTH                     "1920"
#define VP8_1080P_HEIGHT                    "1080"
#define VP8_1080P_TARGETBITRATE             "512000"

#define VP8PLUGIN_CODEC(prefix) \
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
static struct PluginCodec_Option const prefix##_FrameWidth = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "Frame Width",                      /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_AlwaysMerge,            /* Merge mode */ \
  prefix##_WIDTH,                     /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  prefix##_WIDTH,                     /* Minimum value */ \
  prefix##_WIDTH                      /* Maximum value */ \
}; \
static struct PluginCodec_Option const prefix##_FrameHeight = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "Frame Height",                     /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_AlwaysMerge,            /* Merge mode */ \
  prefix##_HEIGHT,                    /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  prefix##_HEIGHT,                    /* Minimum value */ \
  prefix##_HEIGHT                     /* Maximum value */ \
}; \
static struct PluginCodec_Option const prefix##_TargetBitRate = \
{ \
  PluginCodec_IntegerOption,          /* Option type */ \
  "Target Bit Rate",                  /* User visible name */ \
  false,                              /* User Read/Only flag */ \
  PluginCodec_AlwaysMerge,            /* Merge mode */ \
  prefix##_TARGETBITRATE,             /* Initial value */ \
  NULL,                               /* FMTP option name */ \
  NULL,                               /* FMTP default value */ \
  0,                                  /* H.245 generic capability code and bit mask */ \
  prefix##_TARGETBITRATE,             /* Minimum value */ \
  prefix##_TARGETBITRATE              /* Maximum value */ \
}; \
static struct PluginCodec_Option const * prefix##_OptionTable[] = \
{ \
  &prefix##_FrameWidth, \
  &prefix##_FrameHeight, \
  &prefix##_TargetBitRate, \
  &MaxFR, \
  &MaxFS, \
  &PictureID, \
  &OutputPartition, \
  &TemporalSpatialTradeOff, \
  &SpatialResampling, \
  &SpatialResamplingUp, \
  &SpatialResamplingDown, \
  NULL \
}; \
class prefix##_Format : public VP8Format \
{ \
  public: \
    prefix##_Format() \
      : VP8Format(prefix##_MediaFmt, "VP8", "VP8 Video Codec (RFC)", prefix##_OptionTable) \
    { \
      m_h323CapabilityType = PluginCodec_H323Codec_generic; \
      m_h323CapabilityData = (struct PluginCodec_H323GenericCodecData *)&prefix##_Cap; \
    } \
    \
    virtual bool ToNormalised(OptionMap & original, OptionMap & changed) \
    { \
      OptionMap::iterator it = original.find(MaxFS.m_name); \
      if (it != original.end() && !it->second.empty()) { \
        unsigned maxFrameSize = String2Unsigned(it->second); \
        ClampResolution(m_maxWidth, m_maxHeight, maxFrameSize); \
        Change(maxFrameSize,  original, changed, MaxFS.m_name); \
        ClampMax(m_maxWidth,  original, changed, PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH); \
        ClampMax(m_maxHeight, original, changed, PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT); \
        ClampMax(m_maxWidth,  original, changed, PLUGINCODEC_OPTION_MIN_RX_FRAME_WIDTH);  \
        ClampMax(m_maxHeight, original, changed, PLUGINCODEC_OPTION_MIN_RX_FRAME_HEIGHT); \
      } \
    \
      it = original.find(MaxFR.m_name); \
      if (it != original.end() && !it->second.empty()) \
        ClampMin(PLUGINCODEC_VIDEO_CLOCK/String2Unsigned(it->second), original, changed, PLUGINCODEC_OPTION_FRAME_TIME); \
    \
      if (!m_partition) \
        ClampMax(0, original, changed, OutputPartition.m_name); \
    \
      return true; \
    } \
    \
    virtual bool ToCustomised(OptionMap & original, OptionMap & changed) \
    { \
      Change(GetMacroBlocks(original.GetUnsigned(PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH), \
                            original.GetUnsigned(PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT)), \
             original, changed, MaxFS.m_name); \
    \
      Change(PLUGINCODEC_VIDEO_CLOCK/original.GetUnsigned(PLUGINCODEC_OPTION_FRAME_TIME), \
             original, changed, MaxFR.m_name); \
    \
      if (!m_partition) \
        ClampMax(0, original, changed, OutputPartition.m_name); \
    \
      return true; \
    } \
}; \
static prefix##_Format prefix##_MediaFormatInfo;

VP8PLUGIN_CODEC(VP8_QCIF);
VP8PLUGIN_CODEC(VP8_CIF);
VP8PLUGIN_CODEC(VP8_4CIF);
VP8PLUGIN_CODEC(VP8_240P);
VP8PLUGIN_CODEC(VP8_360P);
VP8PLUGIN_CODEC(VP8_480P);
VP8PLUGIN_CODEC(VP8_720P);
VP8PLUGIN_CODEC(VP8_768P);
VP8PLUGIN_CODEC(VP8_1080P);

//#include <iostream>
//#include <sstream>
//#include <stdio.h>
//#define PTRACE(level,category,text) {std::stringstream t;t<<time(0)<<"\t"__FILE__<<":"<<__LINE__<<"\t"<<"\t"<<category<<"\t"<<text<<"\n";FILE *f;f=fopen("/opt/openmcu-ru/vp8_trace.txt","a");fputs(t.str().c_str(),f);fclose(f);}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition VP8CodecDefinition[] =
{
  // Xak
  PLUGINCODEC_VIDEO_CODEC_CXX(VP8_QCIF_MediaFormatInfo, VP8EncoderRFC, VP8DecoderRFC),
  PLUGINCODEC_VIDEO_CODEC_CXX(VP8_CIF_MediaFormatInfo, VP8EncoderRFC, VP8DecoderRFC),
  PLUGINCODEC_VIDEO_CODEC_CXX(VP8_4CIF_MediaFormatInfo, VP8EncoderRFC, VP8DecoderRFC),
  PLUGINCODEC_VIDEO_CODEC_CXX(VP8_240P_MediaFormatInfo, VP8EncoderRFC, VP8DecoderRFC),
  PLUGINCODEC_VIDEO_CODEC_CXX(VP8_360P_MediaFormatInfo, VP8EncoderRFC, VP8DecoderRFC),
  PLUGINCODEC_VIDEO_CODEC_CXX(VP8_480P_MediaFormatInfo, VP8EncoderRFC, VP8DecoderRFC),
  PLUGINCODEC_VIDEO_CODEC_CXX(VP8_720P_MediaFormatInfo, VP8EncoderRFC, VP8DecoderRFC),
  PLUGINCODEC_VIDEO_CODEC_CXX(VP8_768P_MediaFormatInfo, VP8EncoderRFC, VP8DecoderRFC),
  PLUGINCODEC_VIDEO_CODEC_CXX(VP8_1080P_MediaFormatInfo, VP8EncoderRFC, VP8DecoderRFC),
  //
//  PLUGINCODEC_VIDEO_CODEC_CXX(VP8MediaFormatInfoRFC, VP8EncoderRFC, VP8DecoderRFC),
#if INCLUDE_OM_CUSTOM_PACKETIZATION
  PLUGINCODEC_VIDEO_CODEC_CXX(VP8MediaFormatInfoOM,  VP8EncoderOM,  VP8DecoderOM)
#endif
};

PLUGIN_CODEC_IMPLEMENT_CXX(VP8_CODEC, VP8CodecDefinition);


/////////////////////////////////////////////////////////////////////////////
