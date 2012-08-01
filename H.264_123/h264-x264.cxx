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

void H264EncoderContext::SetProfileLevel (unsigned profile, unsigned constraints, unsigned level)
{
  unsigned profileLevel = (profile << 16) + (constraints << 8) + level;
  x264->SetProfileLevel (profileLevel);
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

  if ((_codec = avcodec_find_decoder(CODEC_ID_H264)) == NULL) {
    cout << "H264\tDecoder\tCodec not found for decoder\n";
    return;
  }

  _context = avcodec_alloc_context();
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

  if (avcodec_open(_context, _codec) < 0) {
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

static int to_normalised_options(const struct PluginCodec_Definition *, void *, const char *, void * parm, unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char ***))
    return 0;

  unsigned profile = 66;
  unsigned constraints = 0;
  unsigned level = 51;
  unsigned width = 352;
  unsigned height = 288;
  unsigned frameTime = 3600;
  unsigned targetBitrate = 20000000;
  unsigned cpb = 0;
  unsigned maxBr = 0;

  for (const char * const * option = *(const char * const * *)parm; *option != NULL; option += 2) {
      if (STRCMPI(option[0], "CAP RFC3894 Profile Level") == 0) {
        profile_level_from_string(option[1], profile, constraints, level);
      }
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_WIDTH) == 0)
        width = atoi(option[1]);
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_HEIGHT) == 0)
        height = atoi(option[1]);
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_TIME) == 0)
        frameTime = atoi(option[1]);
      if (STRCMPI(option[0], PLUGINCODEC_OPTION_TARGET_BIT_RATE) == 0)
        targetBitrate = atoi(option[1]);
  }

  TRACE(4, "H264\tCap\tProfile and Level: " << profile << ";" << constraints << ";" << level);

  // Though this is not a strict requirement we enforce 
  //it here in order to obtain optimal compression results
  width -= width % 16;
  height -= height % 16;

  if (!adjust_to_level (width, height, frameTime, targetBitrate, level, 0, 0, cpb, maxBr))
    return 0;

  char ** options = (char **)calloc(9, sizeof(char *));
  *(char ***)parm = options;
  if (options == NULL)
    return 0;

  options[0] = strdup(PLUGINCODEC_OPTION_FRAME_WIDTH);
  options[1] = num2str(width);
  options[2] = strdup(PLUGINCODEC_OPTION_FRAME_HEIGHT);
  options[3] = num2str(height);
  options[4] = strdup(PLUGINCODEC_OPTION_FRAME_TIME);
  options[5] = num2str(frameTime);
  options[6] = strdup(PLUGINCODEC_OPTION_TARGET_BIT_RATE);
  options[7] = num2str(targetBitrate);

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
  unsigned profile = 64;
  unsigned constraints = 129;
  unsigned level = 36;
  unsigned width = 352;
  unsigned height = 288;
  unsigned frameTime = 3000;
  unsigned maxMbps = 0;
  unsigned maxFs = 0;
  unsigned targetBitrate = 2000000;
  unsigned cpb = 0;
  unsigned maxBr = 0;

  if (parm != NULL) {
    const char ** options = (const char **)parm;
    int i;
    for (i = 0; options[i] != NULL; i += 2) {
    printf("%s %s\n",options[i],options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_MAX_BIT_RATE) == 0)
         targetBitrate = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_TIME) == 0)
         frameTime = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_HEIGHT) == 0)
         height = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_WIDTH) == 0)
         width = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_MAX_FRAME_SIZE) == 0)
         context->SetMaxRTPFrameSize(atoi(options[i+1]));
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_TEMPORAL_SPATIAL_TRADE_OFF) == 0)
        context->SetTSTO (atoi(options[i+1]));
      if (STRCMPI(options[i], "Generic Parameter 41") == 0)
         profile = atoi(options[i+1]);
      if (STRCMPI(options[i], "Generic Parameter 42") == 0)
         level = atoi(options[i+1]);
      if (STRCMPI(options[i], "Generic Parameter 3") == 0)
         maxMbps = atoi(options[i+1]) * 500;
      if (STRCMPI(options[i], "Generic Parameter 4") == 0)
         maxFs = atoi(options[i+1]) * 256;
      if (STRCMPI(options[i], "Generic Parameter 6") == 0)
         maxBr = atoi(options[i+1]) * 25000;
    }

    if(profile&31!=0) profile=88; //extended & high
    else if(profile&32!=0) profile=77; //main
    else profile=66; //baseline

    for(i=0;h241_to_x264_levels[i].h241!=0;i++)
    {
     if(h241_to_x264_levels[i].h241==level) { level=h241_to_x264_levels[i].idc; break; }
    }
    if(h241_to_x264_levels[i].h241==0) level=13;

    TRACE(4, "H264\tCap\tProfile and Level: " << profile << ";" << constraints << ";" << level);

    if (!adjust_to_level (width, height, frameTime, targetBitrate, level, maxMbps, maxFs, cpb, maxBr)) {
      context->Unlock();
      return 0;
    }


    cout << "Profile/level: " << profile << "/" << level << "\n";
    cout << "Size: " << width << "x" << height << "\n";
    cout << "frameRate: " << (int)(H264_CLOCKRATE / frameTime) << "\n";
    cout << "targetBitrate: " << targetBitrate << "bit/sec\n";
    cout << "maxBitrate: " << maxBr << "bit/sec\n";
    cout << "vbv_buffer: " << cpb << "bit\n";

    context->SetFrameHeight(height);
    context->SetFrameWidth(width);
    context->SetFrameRate((int)(H264_CLOCKRATE / frameTime));
    context->SetTargetBitrate(targetBitrate/1024, maxBr/1024, cpb/1024);
    context->SetProfileLevel(profile, constraints, level);
    context->ApplyOptions();
    context->Unlock();


    char ** topt = (char **)calloc(7, sizeof(char *)); if (topt == NULL) return 0;
//    free(*(char ***)parm); // memory leak ?;

    topt[0] = strdup(PLUGINCODEC_OPTION_FRAME_WIDTH);
    topt[1] = num2str(width);
    topt[2] = strdup(PLUGINCODEC_OPTION_FRAME_HEIGHT);
    topt[3] = num2str(height);
    topt[4] = strdup(PLUGINCODEC_OPTION_FRAME_TIME);
    topt[5] = num2str(1000*frameTime/H264_CLOCKRATE);
    
    int hlen=7*sizeof(char *);
    unsigned len = strlen(topt[0])+1+strlen(topt[1])+1+
		   strlen(topt[2])+1+strlen(topt[3])+1+
		   strlen(topt[4])+1+strlen(topt[5])+1;
    char *opt=(char *)parm;
    char ** hopt=(char **)parm;
    hopt[0]=&opt[hlen]; strcpy(&opt[hlen],topt[0]); hlen+=strlen(topt[0])+1;
    hopt[1]=&opt[hlen]; strcpy(&opt[hlen],topt[1]); hlen+=strlen(topt[1])+1;
    hopt[2]=&opt[hlen]; strcpy(&opt[hlen],topt[2]); hlen+=strlen(topt[2])+1;
    hopt[3]=&opt[hlen]; strcpy(&opt[hlen],topt[3]); hlen+=strlen(topt[3])+1;
    hopt[4]=&opt[hlen]; strcpy(&opt[hlen],topt[4]); hlen+=strlen(topt[4])+1;
    hopt[5]=&opt[hlen]; strcpy(&opt[hlen],topt[5]);
    hopt[6]=NULL;
    
    free(topt[0]); free(topt[1]); free(topt[2]); free(topt[3]); 
    free(topt[4]); free(topt[5]); free(topt);
//    *(char ***)parm = hopt;
    
  }
  return 1;
}

static int encoder_get_output_data_size(const PluginCodec_Definition *, void *, const char *, void *, unsigned *)
{
  return 1400; //FIXME
}
/////////////////////////////////////////////////////////////////////////////

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
/* 
  for(int i=0;i<*fromLen;i++)
  {
   printf("%02x ",src[i]);
  }
   printf("\n");
*/

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
