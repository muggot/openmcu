/*
 * Common Plugin code for OpenH323/OPAL
 *
 * This code is based on the following files from the OPAL project which
 * have been removed from the current build and distributions but are still
 * available in the CVS "attic"
 *
 *    src/codecs/h263codec.cxx 
 *    include/codecs/h263codec.h 

 * The original files, and this version of the original code, are released under the same 
 * MPL 1.0 license. Substantial portions of the original code were contributed
 * by Salyens and March Networks and their right to be identified as copyright holders
 * of the original code portions and any parts now included in this new copy is asserted through 
 * their inclusion in the copyright notices below.
 *
 * Copyright (C) 2006 Post Increment
 * Copyright (C) 2005 Salyens
 * Copyright (C) 2001 March Networks Corporation
 * Copyright (C) 1999-2000 Equivalence Pty. Ltd.
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
 * Contributor(s): Guilhem Tardy (gtardy@salyens.com)
 *                 Craig Southeren (craigs@postincrement.com)
 *                 Matthias Schneider (ma30002000@yahoo.de)
 */

#include <stdio.h>
#include <iomanip>

#include "ffmpeg.h"


#ifdef PLUGINCODEC_TRACING
  #undef PLUGINCODEC_TRACING
#endif
#define PLUGINCODEC_TRACING 0

#if PLUGINCODEC_TRACING
static void logCallbackFFMPEG(void * avcl, int severity, const char* fmt , va_list arg)
{
  unsigned level;
  if (severity <= AV_LOG_FATAL)
    level = 0;
  else if (severity <= AV_LOG_ERROR)
    level = 1;
  else if (severity <= AV_LOG_WARNING)
    level = 2;
  else if (severity <= AV_LOG_INFO)
    level = 3;
  else if (severity <= AV_LOG_VERBOSE)
    level = 4;
  else
    level = 5;

  if (level > 1 && !PTRACE_CHECK(level))
    return;

  char buffer[512];
  int len = vsnprintf(buffer, sizeof(buffer), fmt, arg);
  if (len <= 0)
    return;

  // Drop trailing white space, in particular line feed, if present
  while (len > 0 && isspace(buffer[len-1]))
    buffer[--len] = '\0';

  // Nothing to log
  if (buffer[0] == '\0')
    return;

  // Check for bogus errors, everything works so what do these mean? Bump up level so don't get the noise
  if (strstr(buffer, "Frame num gap") != NULL ||
      strstr(buffer, "Too many slices") != NULL ||
      (len == 2 && isxdigit(buffer[1])))
    level = 6;

  if (avcl != NULL && strcmp((*(AVClass**)avcl)->class_name, "AVCodecContext") == 0)
    static_cast<FFMPEGCodec *>(static_cast<AVCodecContext *>(avcl)->opaque)->ErrorCallback(level, buffer);
  else
    PTRACE(level, "FFMPEG", buffer);
}
#endif


///////////////////////////////////////////////////////////////////////////////

FFMPEGCodec::FFMPEGCodec(const char * prefix, EncodedFrame * fullFrame)
  : m_prefix(prefix)
  , m_codec(NULL)
  , m_context(NULL)
  , m_picture(NULL)
  , m_alignedInputYUV(NULL)
  , m_alignedInputSize(0)
  , m_fullFrame(fullFrame)
  , m_errorCount(0)
{
  avcodec_register_all();

  av_init_packet(&m_packet);

#if PLUGINCODEC_TRACING
  av_log_set_level(AV_LOG_DEBUG);
  av_log_set_callback(&logCallbackFFMPEG);
#endif
}


FFMPEGCodec::~FFMPEGCodec()
{
  CloseCodec();

  if (m_context != NULL)
    av_free(m_context);
  if (m_picture != NULL)
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54, 0, 0)
    av_free(m_picture);
#else
    avcodec_free_frame(&m_picture);
#endif
  if (m_alignedInputYUV != NULL)
    free(m_alignedInputYUV);

  delete m_fullFrame;

  PTRACE(4, m_prefix, "Codec closed");
}


bool FFMPEGCodec::InitContext()
{
#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT(52, 25, 0)
  m_context = avcodec_alloc_context2(m_codec->type);
#else
  m_context = avcodec_alloc_context3(m_codec);
#endif
  if (m_context == NULL) {
    PTRACE(1, m_prefix, "Failed to allocate context for encoder");
    return false;
  }

  m_picture = avcodec_alloc_frame();
  if (m_picture == NULL) {
    PTRACE(1, m_prefix, "Failed to allocate frame for encoder");
    return false;
  }

  m_context->pix_fmt = PIX_FMT_YUV420P;
  m_context->workaround_bugs = FF_BUG_AUTODETECT;

  // debugging flags
#if PLUGINCODEC_TRACING
  if (PTRACE_CHECK(4))
    m_context->debug |= FF_DEBUG_ER;
  if (PTRACE_CHECK(5))
    m_context->debug |= FF_DEBUG_PICT_INFO | FF_DEBUG_RC;
  if (PTRACE_CHECK(6))
    m_context->debug |= FF_DEBUG_BUGS | FF_DEBUG_BUFFERS;
#endif

  m_context->opaque = this;
  return true;
}


// This is called after an Favcodec_encode_frame() call.  This populates the 
// m_packetSizes deque with offsets, no greated than max_rtp which we will use
// to create RTP packets.
static void StaticRTPCallBack(AVCodecContext * ctx, void * data, int size, int numMB)
{
  static_cast<FFMPEGCodec *>(ctx->opaque)->GetEncodedFrame()->RTPCallBack(data, size, numMB);
}

bool FFMPEGCodec::InitEncoder(const char *codecId)
{
  PTRACE(5, m_prefix, "Opening encoder");

  m_codec = avcodec_find_encoder_by_name(codecId);
  if (m_codec == NULL) {
    PTRACE(1, m_prefix, "Codec not found for encoder");
    return false;
  }

  if (!InitContext())
    return false;

  m_context->rtp_callback = &StaticRTPCallBack;

  m_context->flags = CODEC_FLAG_EMU_EDGE   // don't draw edges
                    | CODEC_FLAG_TRUNCATED  // Possible missing packets
                    ;

  m_context->mb_decision = FF_MB_DECISION_SIMPLE;    // high quality off

  m_context->qblur = 0.3f; // Reduce the difference in quantization between frames.

  // X-Lite does not like Custom Picture frequency clocks... stick to 29.97Hz
  m_context->time_base.num = 100;
  m_context->time_base.den = 2997;

  m_context->gop_size = 132;

  PTRACE(4, m_prefix, "Encoder created");

  return true;
}


bool FFMPEGCodec::InitDecoder(const char *codecId)
{
  if ((m_codec = avcodec_find_decoder_by_name(codecId)) == NULL) {
    PTRACE(1, m_prefix, "Codec not found for decoder");
    return false;
  }

  if (!InitContext())
    return false;

  m_picture->quality = -1;

#ifdef FF_ER_AGGRESSIVE
  m_context->error_recognition = FF_ER_AGGRESSIVE;
#endif
  m_context->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;

  return true;
}


bool FFMPEGCodec::OpenCodec()
{
  if (m_codec == NULL || m_context == NULL || m_picture == NULL) {
    PTRACE(1, m_prefix, "Codec not initialized");
    return false;
  }

#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT(52, 25, 0)
  int result = avcodec_open(m_context, m_codec);
#else
  AVDictionary * options = NULL;
  int result = avcodec_open2(m_context, m_codec, &options);
  av_dict_free(&options);
#endif

  if (result < 0) {
    PTRACE(1, m_prefix, "Failed to open codec");
    return false;
  }

  PTRACE(4, m_prefix, "Codec opened");
  return true;
}


void FFMPEGCodec::CloseCodec()
{
  if (m_context != NULL && m_context->codec != NULL)
    avcodec_close(m_context);
}


bool FFMPEGCodec::SetResolution(unsigned width, unsigned height)
{
  bool wasOpen = m_context->codec != NULL;
  if (wasOpen) {
    PTRACE(3, m_prefix, "Resolution has changed - reopening codec");
    CloseCodec();
  }

  if (m_context != NULL) {
    if (width > 352)
      m_context->flags &= ~CODEC_FLAG_EMU_EDGE; // Totally bizarre! FFMPEG crashes if on for CIF4

    avcodec_set_dimensions(m_context, width, height);
  }

  if (m_picture != NULL) {
    // YUV420P input
    m_picture->linesize[0] = width;
    m_picture->linesize[1] = m_picture->linesize[2] = width/2;
  }

  if (m_fullFrame != NULL && !m_fullFrame->SetResolution(width, height)) {
    PTRACE(1, m_prefix, "Frame handler SetResolution failed");
    return false;
  }


  if (wasOpen && !OpenCodec()) {
    PTRACE(1, m_prefix, "Reopening codec failed");
    return false;
  }

  PTRACE(5, m_prefix, "Resolution set to " << width << 'x' << height);
  return true;
}


void FFMPEGCodec::SetEncoderOptions(unsigned frameTime,
                                    unsigned maxBitRate,
                                    unsigned maxRTPSize,
                                    unsigned tsto,
                                    unsigned keyFramePeriod)
{
  m_context->time_base.den = 2997;
  m_context->time_base.num = frameTime*m_context->time_base.den/PLUGINCODEC_VIDEO_CLOCK;

  m_context->bit_rate = m_context->rc_min_rate = m_context->rc_max_rate = maxBitRate;

  // Ratecontrol buffer size, in bits. Usually 0.5-2 second worth. We have 2.
  // 224 kbyte is what VLC uses, and it seems to fix the quantization pulse (at Level 5)
  m_context->rc_buffer_size = maxBitRate*2;

  // In MEncoder this defaults to 1/4 buffer size, but in ffmpeg.c it
  // defaults to 3/4. I think the buffer is supposed to stabilize at
  // about half full. Note that setting this after avcodec_open() has
  // no effect.
  m_context->rc_initial_buffer_occupancy = m_context->rc_buffer_size * 1/2;

  // And this is set to 1.
  // It seems to affect how aggressively the library will raise and lower
  // quantization to keep bandwidth constant. Except it's in reference to
  // the "vbv buffer", not bits per second, so nobody really knows how
  // it works.
  m_context->rc_buffer_aggressivity = 1.0f;

  // This is set to 0 in ffmpeg.c, the command-line utility.
  m_context->rc_initial_cplx = 0.0f;

  // FFMPEG requires bit rate tolerance to be at least one frame size
  m_context->bit_rate_tolerance = maxBitRate/10;
  int oneFrameBits = (int)((int64_t)m_context->bit_rate*m_context->time_base.num/m_context->time_base.den) + 1;
  if (m_context->bit_rate_tolerance < oneFrameBits) {
    PTRACE(4, m_prefix, "Limited bit_rate_tolerance "
            "(" << m_context->bit_rate_tolerance << ") to size of one frame (" << oneFrameBits << ')');
    m_context->bit_rate_tolerance = oneFrameBits;
  }

  m_context->i_quant_factor = (float)-0.6;  // qscale factor between p and i frames
  m_context->i_quant_offset = (float)0.0;   // qscale offset between p and i frames
  m_context->max_qdiff = 10;  // was 3      // max q difference between frames
  m_context->qcompress = 0.5;               // qscale factor between easy & hard scenes (0.0-1.0)

  // Lagrange multipliers - this is how the context defaults do it:
  m_context->lmin = m_context->qmin * FF_QP2LAMBDA;
  m_context->lmax = m_context->qmax * FF_QP2LAMBDA; 

  m_context->rtp_payload_size = maxRTPSize;

  m_context->qmax = tsto;
  if (m_context->qmax <= m_context->qmin)
    m_context->qmax = m_context->qmin+1;

  m_context->gop_size = keyFramePeriod;

  // Dump info
  PTRACE(5, m_prefix, "Size is " << m_context->width << "x" << m_context->height);
  PTRACE(5, m_prefix, "GOP is " << m_context->gop_size);
  PTRACE(5, m_prefix, "time_base set to " << m_context->time_base.num << '/' << m_context->time_base.den
          << " (" << std::setprecision(2) << ((double)m_context->time_base.den/m_context->time_base.num) << "fps)");
  PTRACE(5, m_prefix, "bit_rate set to " << m_context->bit_rate);
  PTRACE(5, m_prefix, "rc_max_rate is " <<  m_context->rc_max_rate);
  PTRACE(5, m_prefix, "rc_min_rate set to " << m_context->rc_min_rate);
  PTRACE(5, m_prefix, "bit_rate_tolerance set to " <<m_context->bit_rate_tolerance);
  PTRACE(5, m_prefix, "qmin set to " << m_context->qmin);
  PTRACE(5, m_prefix, "qmax set to " << m_context->qmax);
  PTRACE(5, m_prefix, "payload size set to " << m_context->rtp_payload_size);

}


bool FFMPEGCodec::EncodeVideoPacket(const PluginCodec_RTP & in, PluginCodec_RTP & out, unsigned & flags)
{
  if (m_codec == NULL) {
    PTRACE(1, m_prefix, "Encoder did not open");
    return false;
  }

  bool forceIFrame = (flags & PluginCodec_CoderForceIFrame) != 0;
  flags = 0;

  out.SetTimestamp(in.GetTimestamp());

  if (m_fullFrame != NULL && m_fullFrame->GetPacket(out, flags))
    return true;

  PluginCodec_Video_FrameHeader * header = in.GetVideoHeader();
  if (header->x != 0 || header->y != 0) {
    PTRACE(2, m_prefix, "Video grab of partial frame unsupported, closing down video transmission thread.");
    return false;
  }

  // if this is the first frame, or the frame size has changed, deal wth it
  if ((m_context->width !=  (int)header->width || m_context->height != (int)header->height) &&
                                                !SetResolution(header->width, header->height)) {
    PTRACE(3, m_prefix, "Could not adjust output buffer to " << header->width << 'x' << header->height);
    return false;
  }

  size_t planeSize = m_context->width*m_context->height;

  // May need to copy to local buffer to guarantee 16 byte alignment
  uint8_t * yuv = OPAL_VIDEO_FRAME_DATA_PTR(header);
  if ((((intptr_t)yuv)&0xf) != 0) {
    size_t frameSize = planeSize*3/2;
    size_t alignedSize = frameSize + 16;
    if (m_alignedInputSize < alignedSize) {
      if (m_alignedInputYUV != NULL)
        free(m_alignedInputYUV);
      if ((m_alignedInputYUV = (uint8_t *)malloc(alignedSize)) == NULL) {
        PTRACE(1, m_prefix, "Unable to allocate memory for aligned buffer");
        return false;
      }
      m_alignedInputSize = alignedSize;
    }

    yuv = m_alignedInputYUV + 16 - (((intptr_t)m_alignedInputYUV) & 0xf);
    memcpy(yuv, OPAL_VIDEO_FRAME_DATA_PTR(header), frameSize);
  }

  m_picture->data[0] = yuv;
  m_picture->data[1] = m_picture->data[0] + planeSize;
  m_picture->data[2] = m_picture->data[1] + planeSize/4;

  /*
  m_picture->pts = (int64_t)srcRTP.GetTimestamp()*m_context->time_base.den/m_context->time_base.num/PLUGINCODEC_VIDEO_CLOCK;

  It would be preferable to use the above line which would get the correct bit rate if
  the grabber is actually slower that the desired frame rate. But due to rounding
  errors, this could end up with two consecutive frames with the same pts and FFMPEG
  then spits the dummy and returns -1 error, which kills the stream. Sigh.

  So, we just assume the frame rate is actually correct and use it for bit rate control.
  */
  m_picture->pts = AV_NOPTS_VALUE;

  m_picture->pict_type = forceIFrame ? AV_PICTURE_TYPE_I : (AVPictureType)0;
  m_picture->key_frame = 0;

  if (m_fullFrame == NULL)
    return EncodeVideoFrame(out.GetPayloadPtr(), out.GetMaxSize()-out.GetHeaderSize(), flags) >= 0;

  int result = EncodeVideoFrame(m_fullFrame->GetBuffer(), m_fullFrame->GetMaxSize(), flags);
  if (result < 0)
    return false;

  m_fullFrame->Reset(result);

  if (m_fullFrame->IsIntraFrame())
    flags |= PluginCodec_ReturnCoderIFrame;

  return m_fullFrame->GetPacket(out, flags);
}


int FFMPEGCodec::EncodeVideoFrame(uint8_t * frame, size_t length, unsigned & flags)
{
#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT(54, 0, 0)
  int result = avcodec_encode_video(m_context, frame, length, m_picture);
  int gotPacket = result > 0;
#else
  m_packet.data = frame;
  m_packet.size = length;
  int gotPacket = 0;
  int result = avcodec_encode_video2(m_context, &m_packet, m_picture, &gotPacket);
#endif

  if (result < 0) {
    PTRACE(1, m_prefix, "Encoder failed");
    return result;
  }

  if (m_picture->key_frame)
    flags |= PluginCodec_ReturnCoderIFrame;

  if (gotPacket)
    return m_packet.size;

  PTRACE(3, m_prefix, "Encoder returned no data");
  flags |= PluginCodec_ReturnCoderLastFrame;
  return 0;
}


bool FFMPEGCodec::DecodeVideoPacket(const PluginCodec_RTP & in, unsigned & flags)
{
  if (m_codec == NULL) {
    PTRACE(1, m_prefix, "Decoder did not open");
    return false;
  }

  flags = 0;

  if (m_fullFrame == NULL)
    return DecodeVideoFrame(in.GetPayloadPtr(), in.GetPayloadSize(), flags);

  if (in.GetMarker())
    flags |= PluginCodec_ReturnCoderLastFrame;

  if (in.GetPayloadSize() > 0 && !m_fullFrame->AddPacket(in, flags))
    return false;

  if ((flags&PluginCodec_ReturnCoderLastFrame) == 0)
    return true;

  bool result = DecodeVideoFrame(m_fullFrame->GetBuffer(), m_fullFrame->GetLength(), flags);
  m_fullFrame->Reset();
  return result;
}


bool FFMPEGCodec::DecodeVideoFrame(const uint8_t * frame, size_t length, unsigned & flags)
{
  PTRACE(5, m_prefix, "Decoding " << length << " bytes");

  int errorsBefore = m_errorCount;
#ifdef FFMPEG_HAS_DECODE_ERROR_COUNT
  errorsBefore += m_context->decode_error_count
#endif

  m_picture->pict_type = (AVPictureType)0;
  int gotPicture = 0;
#if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT(52, 25, 0)
  int bytesDecoded = avcodec_decode_video(m_context, m_picture, &gotPicture, frame, length);
#else
  m_packet.data = (uint8_t *)frame;
  m_packet.size = length;
  int bytesDecoded = avcodec_decode_video2(m_context, m_picture, &gotPicture, &m_packet);
#endif

  int errorsAfter = m_errorCount;
#ifdef FFMPEG_HAS_DECODE_ERROR_COUNT
  errorsAfter += m_context->decode_error_count
#endif

  // if error occurred, tell the other end to send another I-frame and hopefully we can resync
  if (errorsAfter > errorsBefore)
    flags |= PluginCodec_ReturnCoderRequestIFrame;

  if (gotPicture) {
    bool isIntra = m_fullFrame != NULL ? m_fullFrame->IsIntraFrame() : (m_picture->pict_type == AV_PICTURE_TYPE_I);
    if (isIntra)
      flags |= PluginCodec_ReturnCoderIFrame;

    PTRACE((size_t)bytesDecoded == length ? 5 : 4, m_prefix,
           "Decoded " << bytesDecoded << " of " << length << " bytes, " <<
           (isIntra ? 'I' : 'P') << "-Frame at " << m_context->width << "x" << m_context->height);
  }
  else {
    flags &= ~PluginCodec_ReturnCoderLastFrame;

    PTRACE(4, m_prefix, "Decoded " << bytesDecoded << " of " << length << " bytes without an output frame");
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////

FFMPEGCodec::EncodedFrame::EncodedFrame()
  : m_length(0)
  , m_maxSize(0)
  , m_buffer(NULL)
  , m_maxPayloadSize(PluginCodec_RTP_MaxPayloadSize)
{
}


FFMPEGCodec::EncodedFrame::~EncodedFrame()
{
  if (m_buffer != NULL)
    free(m_buffer);
}


bool FFMPEGCodec::EncodedFrame::SetResolution(unsigned width, unsigned height)
{
  return SetMaxSize(width*height*2);
}


bool FFMPEGCodec::EncodedFrame::SetMaxSize(size_t newSize)
{
  if (newSize <= m_maxSize)
    return true;

  m_buffer = (uint8_t *)realloc(m_buffer, newSize);
  if (m_buffer == NULL)
    return false;

  m_maxSize = newSize;
  return true;
}


bool FFMPEGCodec::EncodedFrame::Append(const uint8_t * data, size_t len)
{
  if (!SetMaxSize(m_length + len))
    return false;

  memcpy(m_buffer+m_length, data, len);
  m_length += len;
  return true;
}


bool FFMPEGCodec::EncodedFrame::Reset(size_t len)
{
  if (len > m_maxSize)
    return false;

  m_length = len;
  return true;
}


void FFMPEGCodec::EncodedFrame::RTPCallBack(void *, int, int)
{
}


void FFMPEGCodec::ErrorCallback(unsigned level, const char * msg)
{
  PTRACE(level, m_prefix, msg);

  if (level < 2)
    ++m_errorCount;
}

