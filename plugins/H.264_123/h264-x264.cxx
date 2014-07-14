/*
 * H.264 Plugin codec for OpenH323/OPAL
 *
 * Copyright (C) Matthias Schneider, All Rights Reserved
 *
 * This code is based on the file h261codec.cxx from the OPAL project released
 * under the MPL 1.0 license which contains the following:
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
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
 * Contributor(s): Matthias Schneider (ma30002000@yahoo.de)
 *                 Michele Piccini (michele@piccini.com)
 *                 Derek Smithies (derek@indranet.co.nz)
 *
 *
 */

/*
  Notes
  -----

 */
 
#include <stdio.h>
#include <stdarg.h>

#include "h264-x264.h"

#ifdef _MSC_VER
 #include "../common/dyna.h"
 #include "../common/trace.h"
 #include "shared/rtpframe.h"
#else
 #include "trace.h"
 #include "rtpframe.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32) || defined(_WIN32_WCE)
  #include <malloc.h>
  #define STRCMPI  _strcmpi
#else
  #include <semaphore.h>
  #define STRCMPI  strcasecmp
#endif
#include <string.h>

#ifndef X264_LINK_STATIC
extern X264Library X264Lib;
#endif

static void logCallbackFFMPEG (void* v, int level, const char* fmt , va_list arg) {
  char buffer[512];
  int severity = 0;
  if (v) {
    switch (level)
    {
      case AV_LOG_QUIET: severity = 0; break;
      case AV_LOG_ERROR: severity = 1; break;
      case AV_LOG_INFO:  severity = 4; break;
      case AV_LOG_DEBUG: severity = 4; break;
      default:           severity = 4; break;
    }
    sprintf(buffer, "H264\tFFMPEG\t");
    vsprintf(buffer + strlen(buffer), fmt, arg);
    if (strlen(buffer) > 0)
      buffer[strlen(buffer)-1] = 0;
    if (severity == 4)
      { TRACE_UP (severity, buffer); }
    else
      { TRACE (severity, buffer); }
  }
}

static char * num2str(int num)
{
  char buf[20];
  sprintf(buf, "%i", num);
  return strdup(buf);
}

H264EncoderContext::H264EncoderContext()
{
  contextProfile = 66;
  constraints = 0;
  contextLevel = 51;
  width = 352;
  height = 288;
  frameTime = 3600;
  targetBitrate = 20000000;
  cpb = 0;
  maxBr = 0;
  x264 = new X264EncoderContext();
}

H264EncoderContext::~H264EncoderContext()
{
  WaitAndSignal m(_mutex);
  delete x264;
}

void H264EncoderContext::ApplyOptions()
{
  x264->ApplyOptions ();
}

void H264EncoderContext::SetMaxRTPFrameSize(unsigned size)
{
  x264->SetMaxRTPFrameSize (size);
}

void H264EncoderContext::SetTargetBitrate(unsigned rate, unsigned maxBr, unsigned cpb)
{
  x264->SetTargetBitrate (rate,maxBr,cpb);
}

void H264EncoderContext::SetFrameWidth(unsigned width)
{
  x264->SetFrameWidth (width);
}

void H264EncoderContext::SetFrameHeight(unsigned height)
{
  x264->SetFrameHeight (height);
}

void H264EncoderContext::SetFrameRate(unsigned rate)
{
  x264->SetFrameRate (rate);
}

void H264EncoderContext::SetTSTO (unsigned tsto)
{
  x264->SetTSTO (tsto);
}

void H264EncoderContext::SetMaxKeyFramePeriod (const char *keyint)
{
  x264->SetMaxKeyFramePeriod (keyint);
}

void H264EncoderContext::SetProfileLevel (unsigned profile, unsigned constraints, unsigned level)
{
  unsigned profileLevel = (profile << 16) + (constraints << 8) + level;
  x264->SetProfileLevel (profileLevel);
}

void H264EncoderContext::SetQuality (unsigned quality)
{
  x264->SetQuality (quality);
}

void H264EncoderContext::SetThreads(unsigned threads)
{
  x264->SetThreads (threads);
}

int H264EncoderContext::EncodeFrames(const u_char * src, unsigned & srcLen, u_char * dst, unsigned & dstLen, unsigned int & flags)
{
  WaitAndSignal m(_mutex);

  int ret=0;
//  unsigned int headerLen;

//  RTPFrame dstRTP(dst, dstLen);
//  headerLen = dstRTP.GetHeaderSize();

  ret = x264->EncodeFrames( src,  srcLen, dst, dstLen, flags);
//  H264EncCtxInstance.call(ENCODE_FRAMES, src, srcLen, dst, dstLen, headerLen, flags, ret);

  return ret;
}

void H264EncoderContext::Lock ()
{
  _mutex.Wait();
}

void H264EncoderContext::Unlock ()
{
  _mutex.Signal();
}

#define	SQN_CHECK_INTERVAL	31 

H264DecoderContext::H264DecoderContext()
{
  _lastSQN = 0;
  _lostFrameCounter = 0;

  freezeVideo = false;
  _gotIFrame = false;
  _gotAGoodFrame = false;
  _frameCounter = 0; 
  _skippedFrameCounter = 0;
  _rxH264Frame = new H264Frame();

  if ((_codec = avcodec_find_decoder_by_name("h264")) == NULL) {
    cout << "H264\tDecoder\tCodec not found for decoder\n";
    return;
  }

#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT(53,8,0)
  _context = avcodec_alloc_context3(_codec);
#else
  _context = avcodec_alloc_context();
#endif
  if (_context == NULL) {
    cout << "H264\tDecoder\tFailed to allocate context for decoder\n";
    return;
  }

  _outputFrame = avcodec_alloc_frame();
  if (_outputFrame == NULL) {
    cout << "H264\tDecoder\tFailed to allocate frame for encoder\n";
    return;
  }

  av_init_packet(&_pkt);

#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT(53,8,0)
  if (avcodec_open2(_context, _codec, NULL) < 0) {
#else
  if (avcodec_open(_context, _codec) < 0) {
#endif
    cout << "H264\tDecoder\tFailed to open H.264 decoder\n";
    return;
  }
  else
  {
    cout << "H264\tDecoder\tDecoder successfully opened\n";
  }
}

H264DecoderContext::~H264DecoderContext()
{
 if (_context != NULL)
 {
  if (_context->codec != NULL)
  {
   avcodec_close(_context);
   cout << "H264\tDecoder\tClosed H.264 decoder, decoded " << _frameCounter << " Frames, skipped " << _skippedFrameCounter << " Frames\n";
  }
 }
 if (_context != NULL) av_free(_context);
 if (_outputFrame != NULL) av_free(_outputFrame);
 if (_rxH264Frame) delete _rxH264Frame;
}

void H264DecoderContext::SetSpropParameter(const char *value)
{
  // from libavformat/rtpdec_h264.c
  _context->extradata_size = 0;
  av_freep(&_context->extradata);

  uint8_t start_sequence[] = { 0, 0, 0, 1 };

  while(*value)
  {
    char base64packet[1024];
    uint8_t decoded_packet[1024];
    int packet_size;
    char *dst = base64packet;

    while(*value && *value != ',' && (dst - base64packet) < sizeof(base64packet) - 1)
    {
      if(*value == '"') { value++; continue; }
      *dst++ = *value++;
    }
    *dst++ = '\0';

    if(*value == ',')
      value++;

    packet_size = av_base64_decode(decoded_packet, base64packet, sizeof(decoded_packet));
    if(packet_size > 0)
    {
      uint8_t *dest = (uint8_t *)av_malloc(packet_size + sizeof(start_sequence) +
                                          _context->extradata_size +
                                          FF_INPUT_BUFFER_PADDING_SIZE);
      if(!dest)
      {
        av_log(_context, AV_LOG_ERROR, "Decoder unable to allocate memory for extradata!\n");
        return;
      }
      if(_context->extradata_size)
      {
        memcpy(dest, _context->extradata, _context->extradata_size);
        av_free(_context->extradata);
      }

      memcpy(dest + _context->extradata_size, start_sequence, sizeof(start_sequence));
      memcpy(dest + _context->extradata_size + sizeof(start_sequence), decoded_packet, packet_size);
      memset(dest + _context->extradata_size + sizeof(start_sequence) + packet_size, 0, FF_INPUT_BUFFER_PADDING_SIZE);

      _context->extradata = dest;
      _context->extradata_size += sizeof(start_sequence) + packet_size;
    }
  }
  av_log(_context, AV_LOG_INFO, "Decoder extradata set to %p (size: %d)!\n", _context->extradata, _context->extradata_size);

  avcodec_close(_context);
#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT(53,8,0)
  avcodec_open2(_context, _codec, NULL);
#else
  avcodec_open(_context, _codec);
#endif
}

int H264DecoderContext::DecodeFrames(const u_char * src, unsigned & srcLen, u_char * dst, unsigned & dstLen, unsigned int & flags)
{
  // create RTP frame from source buffer
  RTPFrame srcRTP(src, srcLen);

  unsigned int curSQN = srcRTP.GetSequenceNumber();
  if(_lastSQN != 0 && curSQN > _lastSQN+1) _lostFrameCounter++;
  _lastSQN = curSQN;

  // create RTP frame from destination buffer
  RTPFrame dstRTP(dst, dstLen, 0);
  dstLen = 0;

  if(flags!=freezeVideo)
  {
   if(flags)
   {
    if(srcRTP.GetMarker()) freezeVideo=flags; // last packet of current frame received, stop decoding
   }
   else
   {
    freezeVideo=flags; // start decoding
    flags = requestIFrame;
    return 1;
   } 
  }
  else if(freezeVideo) { flags = 0; return 1; }

  flags = 0;

  if (!_rxH264Frame->SetFromRTPFrame(srcRTP, flags)) {
    _rxH264Frame->BeginNewFrame();
    flags = (_gotAGoodFrame ? requestIFrame : 0);
    _gotAGoodFrame = false;
    return 1;
  }

  if (srcRTP.GetMarker()==0)
  {
    return 1;
  } 

  if (_rxH264Frame->GetFrameSize()==0)
  {
    _rxH264Frame->BeginNewFrame();
    TRACE(4, "H264\tDecoder\tGot an empty frame - skipping");
    _skippedFrameCounter++;
    flags = (_gotAGoodFrame ? requestIFrame : 0);
    _gotAGoodFrame = false;
    return 1;
  }

  TRACE_UP(4, "H264\tDecoder\tDecoding " << _rxH264Frame->GetFrameSize()  << " bytes");

  // look and see if we have read an I frame.
  if (_gotIFrame == 0)
  {
    if (!_rxH264Frame->IsSync())
    {
      TRACE(1, "H264\tDecoder\tWaiting for an I-Frame");
      _rxH264Frame->BeginNewFrame();
      flags = (_gotAGoodFrame ? requestIFrame : 0);
      _gotAGoodFrame = false;
      return 1;
    }
    _gotIFrame = 1;
    _lostFrameCounter = 0;
  }

  int gotPicture = 0;
  uint32_t bytesUsed = 0;
//  int bytesDecoded = avcodec_decode_video(_context, _outputFrame, &gotPicture, _rxH264Frame->GetFramePtr() + bytesUsed, _rxH264Frame->GetFrameSize() - bytesUsed);
  _pkt.data = _rxH264Frame->GetFramePtr() + bytesUsed;
  _pkt.size = _rxH264Frame->GetFrameSize() - bytesUsed;
  int bytesDecoded = avcodec_decode_video2(_context, _outputFrame, &gotPicture, &_pkt);
  
  if(_lostFrameCounter > 0 && (curSQN&SQN_CHECK_INTERVAL)==0)
  {
   _lostFrameCounter=0;
//   flags |= PluginCodec_ReturnCoderRequestIFrame;
  }

  _rxH264Frame->BeginNewFrame();
  if (!gotPicture) 
  {
    TRACE(1, "H264\tDecoder\tDecoded "<< bytesDecoded << " bytes without getting a Picture..."); 
    _skippedFrameCounter++;
    flags |= (_gotAGoodFrame ? requestIFrame : 0);
    _gotAGoodFrame = false;
    return 1;
  }

  TRACE_UP(4, "H264\tDecoder\tDecoded " << bytesDecoded << " bytes"<< ", Resolution: " << _context->width << "x" << _context->height);
  int frameBytes = (_context->width * _context->height * 3) / 2;
  PluginCodec_Video_FrameHeader * header = (PluginCodec_Video_FrameHeader *)dstRTP.GetPayloadPtr();
  header->x = header->y = 0;
  header->width = _context->width;
  header->height = _context->height;

  int size = _context->width * _context->height;
  if (_outputFrame->data[1] == _outputFrame->data[0] + size
      && _outputFrame->data[2] == _outputFrame->data[1] + (size >> 2))
  {
    memcpy(OPAL_VIDEO_FRAME_DATA_PTR(header), _outputFrame->data[0], frameBytes);
  }
  else 
  {
    unsigned char *dstData = OPAL_VIDEO_FRAME_DATA_PTR(header);
    for (int i=0; i<3; i ++)
    {
      unsigned char *srcData = _outputFrame->data[i];
      int dst_stride = i ? _context->width >> 1 : _context->width;
      int src_stride = _outputFrame->linesize[i];
      int h = i ? _context->height >> 1 : _context->height;

      if (src_stride==dst_stride)
      {
        memcpy(dstData, srcData, dst_stride*h);
        dstData += dst_stride*h;
      }
      else
      {
        while (h--)
        {
          memcpy(dstData, srcData, dst_stride);
          dstData += dst_stride;
          srcData += src_stride;
        }
      }
    }
  }

  dstRTP.SetPayloadSize(sizeof(PluginCodec_Video_FrameHeader) + frameBytes);
  dstRTP.SetTimestamp(srcRTP.GetTimestamp());
  dstRTP.SetMarker(1);
  dstLen = dstRTP.GetFrameLen();

  flags |= PluginCodec_ReturnCoderLastFrame;

  _frameCounter++;
  _gotAGoodFrame = true;
  return 1;
}

/////////////////////////////////////////////////////////////////////////////

static int get_codec_options(const struct PluginCodec_Definition * codec,
                                                  void *,
                                                  const char *,
                                                  void * parm,
                                                  unsigned * parmLen)
{
    if (parmLen == NULL || parm == NULL || *parmLen != sizeof(struct PluginCodec_Option **))
        return 0;

    *(const void **)parm = codec->userData;
    *parmLen = 0; //FIXME
    return 1;
}

static int free_codec_options ( const struct PluginCodec_Definition *, void *, const char *, void * parm, unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char ***))
    return 0;

  char ** strings = (char **) parm;
  for (char ** string = strings; *string != NULL; string++)
    free(*string);
  free(strings);
  return 1;
}

static int int_from_string(std::string str)
{
  if (str.find_first_of("\"") != std::string::npos)
    return (atoi( str.substr(1, str.length()-2).c_str()));

  return (atoi( str.c_str()));
}

static void profile_level_from_string  (std::string profileLevelString, unsigned & profile, unsigned & constraints, unsigned & level)
{

  if (profileLevelString.find_first_of("\"") != std::string::npos)
    profileLevelString = profileLevelString.substr(1, profileLevelString.length()-2);

  unsigned profileLevelInt = strtoul(profileLevelString.c_str(), NULL, 16);

  if (profileLevelInt == 0) {
#ifdef DEFAULT_TO_FULL_CAPABILITIES
    // Baseline, Level 3
    profileLevelInt = 0x42C01E;
#else
    // Default handling according to RFC 3984
    // Baseline, Level 1
    profileLevelInt = 0x42C00A;
#endif  
  }

  profile     = (profileLevelInt & 0xFF0000) >> 16;
  constraints = (profileLevelInt & 0x00FF00) >> 8;
  level       = (profileLevelInt & 0x0000FF);
}

static int valid_for_protocol (const struct PluginCodec_Definition * codec,
                                                  void *,
                                                  const char *,
                                                  void * parm,
                                                  unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char *))
    return 0;

  if (codec->h323CapabilityType != PluginCodec_H323Codec_NoH323)
    return (STRCMPI((const char *)parm, "h.323") == 0 ||
            STRCMPI((const char *)parm, "h323") == 0) ? 1 : 0;	        
   else 
    return (STRCMPI((const char *)parm, "sip") == 0) ? 1 : 0;
}

static int adjust_bitrate_to_level (unsigned & targetBitrate, unsigned level, int idx = -1)
{
  int i = 0;
  if (idx == -1) { 
    while (h264_levels[i].level_idc) {
      if (h264_levels[i].level_idc == level)
        break;
    i++; 
    }
  
    if (!h264_levels[i].level_idc) {
      TRACE(1, "H264\tCap\tIllegal Level negotiated");
      return 0;
    }
  }
  else
    i = idx;

// Correct Target Bitrate
  TRACE(4, "H264\tCap\tBitrate: " << targetBitrate << "(" << h264_levels[i].bitrate << ")");
  if (targetBitrate > h264_levels[i].bitrate)
    targetBitrate = h264_levels[i].bitrate;

  return 1;
}

static int adjust_to_level (unsigned & width, unsigned & height, unsigned & frameTime, unsigned & targetBitrate, unsigned level, unsigned maxMbps, unsigned maxFs, unsigned &cpb, unsigned &maxBr)
{
  int i = 0;
  while (h264_levels[i].level_idc) {
    if (h264_levels[i].level_idc == level)
      break;
   i++; 
  }

  if (!h264_levels[i].level_idc) {
    TRACE(1, "H264\tCap\tIllegal Level negotiated");
    return 0;
  }

  if(maxBr==0) maxBr=targetBitrate;
  else if(maxBr < h264_levels[i].bitrate * 1000) maxBr = h264_levels[i].bitrate * 1000;
  cpb = maxBr*1.0/(h264_levels[i].bitrate * 1000)*h264_levels[i].cpb*1000;

  if(maxBr > targetBitrate*1.25) maxBr = targetBitrate*1.25;
  
  if(maxFs == 0) maxFs = h264_levels[i].frame_size;
  unsigned j = 0;
  TRACE(4, "H264\tCap\tFrame Size: " <<  maxFs );
  while (h264_resolutions[j].width) 
  {
   if(h264_resolutions[j].macroblocks <= maxFs) break;
   j++; 
  }
  if (!h264_resolutions[j].width) 
  {
   TRACE(1, "H264\tCap\tNo Resolution found that has number of macroblocks <=" << h264_levels[i].frame_size);
   return 0;
  }
  width  = h264_resolutions[j].width;
  height = h264_resolutions[j].height;

// Correct macroblocks per second
  uint32_t nbMBsPerSecond = width * height / 256 * (H264_CLOCKRATE / frameTime);
  if(maxMbps == 0) maxMbps = h264_levels[i].mbps;
  TRACE(4, "H264\tCap\tMB/s: " << nbMBsPerSecond << "(" << maxMbps << ")");
  if (nbMBsPerSecond > maxMbps)
    frameTime =  (unsigned) (H264_CLOCKRATE / 256 * width  * height / maxMbps );

  return 1;
}

/////////////////////////////////////////////////////////////////////////////

static void * create_encoder(const struct PluginCodec_Definition * /*codec*/)
{
  return new H264EncoderContext;
}

static void destroy_encoder(const struct PluginCodec_Definition * /*codec*/, void * _context)
{
  H264EncoderContext * context = (H264EncoderContext *)_context;
  delete context;
}

static int codec_encoder(const struct PluginCodec_Definition * ,
                                           void * _context,
                                     const void * from,
                                       unsigned * fromLen,
                                           void * to,
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  int res;
  H264EncoderContext * context = (H264EncoderContext *)_context;
  res=context->EncodeFrames((const u_char *)from, *fromLen, (u_char *)to, *toLen, *flag);
/* 
  for(int i=0;i<*toLen;i++)
  {
   printf("%02x ",((u_char *)to)[i]);
  }
   printf("\n");
*/
  return res;
}

static int to_normalised_options(const struct PluginCodec_Definition *, void * _context, const char *, void * parm, unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char ***))
    return 0;

  H264EncoderContext * context = (H264EncoderContext *)_context;
  unsigned profile = 66;
  unsigned level = 51;

  for (const char * const * option = *(const char * const * *)parm; *option != NULL; option += 2) {
      if (STRCMPI(option[0], "CAP RFC3894 Profile Level") == 0) {
        profile_level_from_string(option[1], profile, context->constraints, level);
      }
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_WIDTH) == 0)
        context->width = atoi(option[1]);
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_HEIGHT) == 0)
        context->height = atoi(option[1]);
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_TIME) == 0)
        context->frameTime = atoi(option[1]);
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_TARGET_BIT_RATE) == 0)
        context->targetBitrate = atoi(option[1]);
  }

  TRACE(4, "H264\tCap\tProfile and Level: " << profile << ";" << context->constraints << ";" << level);

  // Though this is not a strict requirement we enforce 
  //it here in order to obtain optimal compression results
  context->width -= context->width % 16;
  context->height -= context->height % 16;

  if (!adjust_to_level (context->width, context->height, context->frameTime, context->targetBitrate, level, 0, 0, context->cpb, context->maxBr))
    return 0;

  char ** options = (char **)calloc(9, sizeof(char *));
  *(char ***)parm = options;
  if (options == NULL)
    return 0;

  options[0] = strdup(PLUGINCODEC_OPTION_FRAME_WIDTH);
  options[1] = num2str(context->width);
  options[2] = strdup(PLUGINCODEC_OPTION_FRAME_HEIGHT);
  options[3] = num2str(context->height);
  options[4] = strdup(PLUGINCODEC_OPTION_FRAME_TIME);
  options[5] = num2str(context->frameTime);
  options[6] = strdup(PLUGINCODEC_OPTION_TARGET_BIT_RATE);
  options[7] = num2str(context->targetBitrate);

  return 1;
}


static int to_customised_options(const struct PluginCodec_Definition *, void *, const char *, void * parm, unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char ***))
    return 0;

  for (const char * const * option = *(const char * const * *)parm; *option != NULL; option += 2) {
/*      if (STRCMPI(option[0], PLUGINCODEC_OPTION_MIN_RX_FRAME_WIDTH) == 0)
        h263MPIList.setMinWidth(atoi(option[1]));
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_MIN_RX_FRAME_HEIGHT) == 0)
        h263MPIList.setMinHeight(atoi(option[1]));
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH) == 0)
        h263MPIList.setMaxWidth(atoi(option[1]));
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT) == 0)
        h263MPIList.setMaxHeight(atoi(option[1]));*/
  }


  char ** options = (char **)calloc(3, sizeof(char *));
  *(char ***)parm = options;
  if (options == NULL)
    return 0;

//   options[0] = strdup(PLUGINCODEC_QCIF_MPI);
//   options[1] = num2str(qcif_mpi);

  return 1;
}

static int encoder_set_options(
      const struct PluginCodec_Definition *, 
      void * _context, 
      const char *, 
      void * parm, 
      unsigned * parmLen)
{
  H264EncoderContext * context = (H264EncoderContext *)_context;

  if (parmLen == NULL || *parmLen != sizeof(const char **)) 
    return 0;

  context->Lock();

  int custom_resolution = 0;

  if (parm != NULL) {
    const char ** options = (const char **)parm;
    int i;
    for (i = 0; options[i] != NULL; i += 2) {
    printf("%s %s\n",options[i],options[i+1]);
      if (STRCMPI(options[i], "Encoding Quality") == 0)
         context->SetQuality (atoi(options[i+1]));
      if (STRCMPI(options[i], "Encoding Threads") == 0)
         context->SetThreads (atoi(options[i+1]));
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_MAX_BIT_RATE) == 0)
      {
         context->maxBr = atoi(options[i+1]);
         context->targetBitrate = context->maxBr * 90 / 100; // very approx. :(
      }
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_TIME) == 0)
         context->frameTime = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_HEIGHT) == 0)
         context->height = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_WIDTH) == 0)
         context->width = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_MAX_FRAME_SIZE) == 0)
         context->SetMaxRTPFrameSize(atoi(options[i+1]));
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_TEMPORAL_SPATIAL_TRADE_OFF) == 0)
        context->SetTSTO (atoi(options[i+1]));
      if (STRCMPI(options[i], "Generic Parameter 41") == 0)
         { context->contextProfile = atoi(options[i+1]); }
      if (STRCMPI(options[i], "Generic Parameter 42") == 0)
         { context->contextLevel = atoi(options[i+1]); }
      if (STRCMPI(options[i], "Generic Parameter 3") == 0)
         context->maxMbps = atoi(options[i+1]) * 500;
      if (STRCMPI(options[i], "Generic Parameter 4") == 0)
         context->maxFs = atoi(options[i+1]) * 256;
      if (STRCMPI(options[i], "Generic Parameter 6") == 0)
         context->maxBr = atoi(options[i+1]) * 25000;
      if (STRCMPI(options[i], "Custom Resolution") == 0)
         custom_resolution = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_TX_KEY_FRAME_PERIOD) == 0)
         context->SetMaxKeyFramePeriod(options[i+1]);
    }

    unsigned profile = context->contextProfile;
    unsigned level = context->contextLevel;

    if(profile&31!=0) profile=88; //extended & high
    else if(profile&32!=0) profile=77; //main
    else profile=66; //baseline

    for(i=0;h241_to_x264_levels[i].h241!=0;i++)
    {
     if(h241_to_x264_levels[i].h241==level) { level=h241_to_x264_levels[i].idc; break; }
    }
    if(h241_to_x264_levels[i].h241==0) level=13;

    TRACE(4, "H264\tCap\tProfile and Level: " << profile << ";" << context->constraints << ";" << level);

    if(!custom_resolution)
    {
      if(!adjust_to_level(context->width, context->height, context->frameTime, context->targetBitrate, level, context->maxMbps, context->maxFs, context->cpb, context->maxBr))
      {
        context->Unlock();
        return 0;
      }
    }

    cout << "Profile/level: " << profile << "/" << level << "\n";
    cout << "Size: " << context->width << "x" << context->height << "\n";
    cout << "frameRate: " << (int)(H264_CLOCKRATE / context->frameTime) << "\n";
    cout << "targetBitrate: " << context->targetBitrate << "bit/sec\n";
    cout << "maxBitrate: " << context->maxBr << "bit/sec\n";
    cout << "vbv_buffer: " << context->cpb << "bit\n";
    TRACE(1,"Profile/level: " << profile << "/" << level);
    TRACE(1,"Size: " << context->width << "x" << context->height);
    TRACE(1,"frameRate: " << (int)(H264_CLOCKRATE / context->frameTime));
    TRACE(1,"targetBitrate: " << context->targetBitrate << "bit/sec");
    TRACE(1,"maxBitrate: " << context->maxBr << "bit/sec");
    TRACE(1,"vbv_buffer: " << context->cpb << "bit");

    context->SetFrameHeight(context->height);
    context->SetFrameWidth(context->width);
    context->SetFrameRate((int)(H264_CLOCKRATE / context->frameTime));
    context->SetTargetBitrate(context->targetBitrate/1024, context->maxBr/1024, context->cpb/1024);
    context->SetProfileLevel(profile, context->constraints, level);
    context->ApplyOptions();
    context->Unlock();

    // Write back the option list the changed information to the level
    if (parm != NULL)
    {
      char ** options = (char **)parm;
      if (options == NULL) return 0;
      for (int i = 0; options[i] != NULL; i += 2)
      {
        if (STRCMPI(options[i], PLUGINCODEC_OPTION_TARGET_BIT_RATE) == 0)
        options[i+1] = num2str(context->targetBitrate);
        if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_TIME) == 0)
        options[i+1] = num2str(context->frameTime);
        if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_HEIGHT) == 0)
        options[i+1] = num2str(context->height);
        if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_WIDTH) == 0)
        options[i+1] = num2str(context->width);
//        if (STRCMPI(options[i], PLUGINCODEC_OPTION_LEVEL) == 0)
//        options[i+1] = num2str(level);
      }
    }
  }
  return 1;
}

static int encoder_get_output_data_size(const PluginCodec_Definition *, void *, const char *, void *, unsigned *)
{
  return 1400; //FIXME
}
/////////////////////////////////////////////////////////////////////////////

static int decoder_set_options(const struct PluginCodec_Definition *, void * _context, const char *, void * parm, unsigned * parmLen)
{
  H264DecoderContext * context = (H264DecoderContext *)_context;

  if(parm == NULL || parmLen == NULL || *parmLen != sizeof(const char **))
    return 0;

  context->Lock();

  const char ** options = (const char **)parm;
  for(int i = 0; options[i] != NULL; i += 2)
  {
    if(STRCMPI(options[i], "sprop-parameter-sets") == 0)
    {
      std::string s(options[i+1]);
      if(s[0] == '"')          s.erase(0,1);
      if(s[s.size()-1] == '"') s.erase(s.size()-1);
      if(strcasecmp(s.c_str(), "") != 0)
        context->SetSpropParameter(s.c_str());
    }
  }

  context->Unlock();
  return 1;
}

static void * create_decoder(const struct PluginCodec_Definition *)
{
  return new H264DecoderContext;
}

static void destroy_decoder(const struct PluginCodec_Definition * /*codec*/, void * _context)
{
  H264DecoderContext * context = (H264DecoderContext *)_context;
  delete context;
}

static int codec_decoder(const struct PluginCodec_Definition *, 
                                           void * _context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  const unsigned char *src = (const unsigned char *) from;

  H264DecoderContext * context = (H264DecoderContext *)_context;
  return context->DecodeFrames((const u_char *)from, *fromLen, (u_char *)to, *toLen, *flag);
}

static int decoder_get_output_data_size(const PluginCodec_Definition * codec, void *, const char *, void *, unsigned *)
{
  return sizeof(PluginCodec_Video_FrameHeader) + ((codec->parm.video.maxFrameWidth * codec->parm.video.maxFrameHeight * 3) / 2);
}

/////////////////////////////////////////////////////////////////////////////
static int merge_profile_level_h264(char ** result, const char * dst, const char * src)
{
  // c0: obeys A.2.1 Baseline
  // c1: obeys A.2.2 Main
  // c2: obeys A.2.3, Extended
  // c3: if profile_idc profile = 66, 77, 88, and level =11 and c3: obbeys annexA for level 1b

  unsigned srcProfile, srcConstraints, srcLevel;
  unsigned dstProfile, dstConstraints, dstLevel;
  profile_level_from_string(src, srcProfile, srcConstraints, srcLevel);
  profile_level_from_string(dst, dstProfile, dstConstraints, dstLevel);

  switch (srcLevel) {
    case 10:
      srcLevel = 8;
      break;
    default:
      break;
  }

  switch (dstLevel) {
    case 10:
      dstLevel = 8;
      break;
    default:
      break;
  }

  if (dstProfile > srcProfile) {
    dstProfile = srcProfile;
  }

  dstConstraints |= srcConstraints;

  if (dstLevel > srcLevel)
    dstLevel = srcLevel;

  switch (dstLevel) {
    case 8:
      dstLevel = 10;
      break;
    default:
      break;
  }


  char buffer[10];
  sprintf(buffer, "%x", (dstProfile<<16)|(dstConstraints<<8)|(dstLevel));
  
  *result = strdup(buffer);

  TRACE(4, "H264\tCap\tCustom merge profile-level: " << src << " and " << dst << " to " << *result);

  return true;
}

static int merge_packetization_mode(char ** result, const char * dst, const char * src)
{
  unsigned srcInt = int_from_string (src); 
  unsigned dstInt = int_from_string (dst); 

  if (srcInt == 5) {
#ifdef DEFAULT_TO_FULL_CAPABILITIES
    srcInt = 1;
#else
    // Default handling according to RFC 3984
    srcInt = 0;
#endif
  }

  if (dstInt == 5) {
#ifdef DEFAULT_TO_FULL_CAPABILITIES
    dstInt = 1;
#else
    // Default handling according to RFC 3984
    dstInt = 0;
#endif
  }

  if (dstInt > srcInt)
    dstInt = srcInt;

  char buffer[10];
  sprintf(buffer, "%d", dstInt);
  
  *result = strdup(buffer);

  TRACE(4, "H264\tCap\tCustom merge packetization-mode: " << src << " and " << dst << " to " << *result);

//  if (dstInt > 0)
    return 1;

//  return 0;
}

static void free_string(char * str)
{
  free(str);
}

/////////////////////////////////////////////////////////////////////////////

extern "C" {

PLUGIN_CODEC_IMPLEMENT(H264)

PLUGIN_CODEC_DLL_API struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned version)
{

  char * debug_level = getenv ("PTLIB_TRACE_CODECS");
  if (debug_level!=NULL) {
    Trace::SetLevel(atoi(debug_level));
  } 
  else {
    Trace::SetLevel(0);
  }

  debug_level = getenv ("PTLIB_TRACE_CODECS_USER_PLANE");
  if (debug_level!=NULL) {
    Trace::SetLevelUserPlane(atoi(debug_level));
  } 
  else {
    Trace::SetLevelUserPlane(0);
  }

  int status;
//  x264 = NULL;
//  dstLen = 1400;

#ifndef X264_LINK_STATIC
  if (X264Lib.Load()) 
    status = 1;
  else 
    status = 0;
#else
  status = 1;
#endif

  avcodec_register_all();

  if (version < PLUGIN_CODEC_VERSION_OPTIONS) {
    *count = 0;
    printf("H264\tCodec\tDisabled1\n");
    TRACE(1, "H264\tCodec\tDisabled - plugin version mismatch");
    return NULL;
  }
  else {
    *count = sizeof(h264CodecDefn) / sizeof(struct PluginCodec_Definition);
    printf("H264\tCodec\tEnabled\n");
    TRACE(1, "H264\tCodec\tEnabled");
    return h264CodecDefn;
  }
  
}
};
