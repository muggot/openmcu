/*
 * H.264 Plugin codec for OpenH323/OPAL
 *
 * Copyright (C) 2007 Matthias Schneider, All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include <stdio.h>
#include <stdarg.h>

#include "plugin-config.h"

#include "enc-ctx.h"
#include "trace.h"
#include "rtpframe.h"

#include <stdlib.h>
#include <math.h>
#if defined(_WIN32) || defined(_WIN32_WCE)
  #include <malloc.h>
  #define STRCMPI  _strcmpi
#else
  #include <semaphore.h>
  #define STRCMPI  strcasecmp
#endif
#include <string.h>

#ifndef X264_LINK_STATIC
X264Library X264Lib;
#endif

static void logCallbackX264 (void * /*priv*/, int level, const char *fmt, va_list arg) {
//  return;
  char buffer[512];
  int severity = 0;
  switch (level) {
    case X264_LOG_NONE:    severity = 1; break;
    case X264_LOG_ERROR:   severity = 1; break;
    case X264_LOG_WARNING: severity = 3; break;
    case X264_LOG_INFO:    severity = 4; break;
    case X264_LOG_DEBUG:   severity = 4; break;
    default:               severity = 4; break;
  }
  sprintf(buffer, "H264\tx264\t"); 
  vsprintf(buffer + strlen (buffer), fmt, arg);
  if (strlen(buffer) > 0)
    buffer[strlen(buffer)-1] = 0;
  if (severity == 4)
    { TRACE_UP (severity, buffer); }
  else
    { TRACE (severity, buffer); }
  printf("%s\n",buffer);
}

X264EncoderContext::X264EncoderContext()
{
  _frameCounter=0;

  _txH264Frame = new H264Frame();
  _txH264Frame->SetMaxPayloadSize(H264_PAYLOAD_SIZE);


   memset(&_inputFrame,0,sizeof(_inputFrame));
  _inputFrame.i_type = X264_TYPE_AUTO;
  _inputFrame.i_qpplus1 = 0;
  _inputFrame.img.i_csp = X264_CSP_I420;
  _inputFrame.param = NULL;
 
   X264_PARAM_DEFAULT(&_context);

  // Default
  // ABR with bit rate tolerance = 1 is CBR...
//  _context.rc.i_rc_method = X264_RC_ABR;
//  _context.rc.f_rate_tolerance = 1;

  // No aspect ratio correction
  _context.vui.i_sar_width = 0;
  _context.vui.i_sar_height = 0;

  // Enable logging
  _context.pf_log = logCallbackX264;
  _context.i_log_level = X264_LOG_DEBUG;
  _context.p_log_private = NULL;

  // Auto detect number of CPUs
  _context.i_threads = 0;
  int f_size;
  char *preset,*tune;
  FILE *fs;

  fs=fopen("x264.init","r"); 
  if(fs)
  {
   fseek(fs,0L,SEEK_END); f_size=ftell(fs); rewind(fs);
   preset=(char *)malloc(f_size+1); tune=(char *)malloc(f_size+1);
   preset[f_size]=0; tune[f_size]=0;
   fscanf(fs,"%s\n%s",preset,tune);
   fclose(fs);
  }
  else
  {
   preset=strdup("veryfast"); tune=strdup("zerolatency,psnr");
  }
  
  printf("X264 configured using <%s> preset and <%s> tune options\n",preset,tune);

  X264_PARAM_DEFAULT_PRESET( &_context, preset, tune );
  free(preset); free(tune);

  X264_PARAM_PARSE(&_context,"slice-max-size","1024");
  X264_PARAM_PARSE(&_context,"intra-refresh","1");
  X264_PARAM_PARSE(&_context,"keyint","125");

  SetFrameWidth       (CIF_WIDTH);
  SetFrameHeight      (CIF_HEIGHT);
//  SetFrameRate        (H264_FRAME_RATE);
  SetTargetBitrate    ((unsigned)(H264_BITRATE / 1000),(unsigned)(H264_BITRATE / 1000),80);
  SetProfileLevel     (H264_PROFILE_LEVEL);
  SetTSTO             (H264_TSTO);

  _codec = X264_ENCODER_OPEN(&_context);
  
  if (_codec == NULL) {
    printf("H264\tEncoder\tCouldn't init x264 encoder\n");
  } 
  else
  {
    printf("H264\tEncoder\tx264 encoder successfully opened\n");
  }
}

X264EncoderContext::~X264EncoderContext()
{
 if (_codec != NULL)
 {
  X264_ENCODER_CLOSE(_codec); _codec = NULL;
  TRACE(4, "H264\tEncoder\tClosed H.264 encoder, encoded " << _frameCounter << " Frames" );
 }
 if (_txH264Frame) delete _txH264Frame;
}

void X264EncoderContext::SetMaxRTPFrameSize(unsigned size)
{
  _txH264Frame->SetMaxPayloadSize(size);
}

void X264EncoderContext::SetTargetBitrate(unsigned rate, unsigned maxBr, unsigned cpb)
{
  _context.rc.i_rc_method = X264_RC_ABR;
  _context.rc.i_bitrate = rate;
  _context.rc.i_vbv_max_bitrate = maxBr;
  _context.rc.i_vbv_buffer_size = cpb;
  if(_context.rc.i_vbv_buffer_size == 0)
    _context.rc.i_vbv_buffer_size = _context.rc.i_vbv_max_bitrate;
}

void X264EncoderContext::SetFrameWidth(unsigned width)
{
  _context.i_width = width;
}

void X264EncoderContext::SetFrameHeight(unsigned height)
{
  _context.i_height = height;
}

void X264EncoderContext::SetFrameRate(unsigned rate)
{
// return;
//  _context.i_fps_num = (int)((rate + .5) * 1000);
//  _context.i_fps_den = 1000;
  _context.i_fps_num = rate;
  _context.i_fps_den = 1;
}

void X264EncoderContext::SetTSTO (unsigned tsto)
{
    _context.rc.i_qp_min = H264_MIN_QUANT;
    _context.rc.i_qp_max = round ( (double)(51 - H264_MIN_QUANT) / 31 * tsto + H264_MIN_QUANT);
    _context.rc.i_qp_step = 4;
}

void X264EncoderContext::SetMaxKeyFramePeriod (const char *keyint)
{
  X264_PARAM_PARSE(&_context, "keyint", keyint);
}

void X264EncoderContext::SetProfileLevel (unsigned profileLevel)
{
  unsigned profile = (profileLevel & 0xff0000) >> 16;
  if(profile==66) X264_PARAM_APPLY_PROFILE(&_context, "baseline");
  else if(profile==77) X264_PARAM_APPLY_PROFILE(&_context, "main");
  else X264_PARAM_APPLY_PROFILE(&_context, "high");
  
//  bool constraint0 = (profileLevel & 0x008000) ? true : false;
//  bool constraint1 = (profileLevel & 0x004000) ? true : false;
//  bool constraint2 = (profileLevel & 0x002000) ? true : false;
//  bool constraint3 = (profileLevel & 0x001000) ? true : false;
  unsigned level   = (profileLevel & 0x0000ff);

  int i = 0;
  while (h264_levels[i].level_idc) {
    if (h264_levels[i].level_idc == level)
      break;
   i++; 
  }

  if (!h264_levels[i].level_idc) {
    printf("H264\tCap\tIllegal Level negotiated\n");
    return;
  }

  _context.i_level_idc = level;
}

void X264EncoderContext::SetQuality(unsigned quality)
{
  return;
}

void X264EncoderContext::SetThreads(unsigned threads)
{
  _context.i_threads = threads;
}

void X264EncoderContext::ApplyOptions()
{
  X264_ENCODER_CLOSE(_codec);
  _codec = X264_ENCODER_OPEN(&_context);
  if (_codec == NULL) {
    printf("H264\tEncoder\tCouldn't init x264 encoder\n");
  } 
  else
  {
    printf("H264\tEncoder\tx264 encoder successfully opened\n");
  }
}

int X264EncoderContext::EncodeFrames(const unsigned char * src, unsigned & srcLen, unsigned char * dst, unsigned & dstLen, unsigned int & flags)
{

  // create RTP frame from source buffer
  RTPFrame srcRTP(src, srcLen);

  // create RTP frame from destination buffer
  RTPFrame dstRTP(dst, dstLen);

  dstLen = 0;

  // from here, we are encoding a new frame
  if ((!_codec) || (!_txH264Frame))
  {
    return 0;
  }

  // if there are RTP packets to return, return them
  if  (_txH264Frame->HasRTPFrames())
  {
    _txH264Frame->GetRTPFrame(dstRTP, flags);
    dstLen = dstRTP.GetFrameLen();
    return 1;
  }

//  cout << "here1\n";


  if (srcRTP.GetPayloadSize() < sizeof(frameHeader))
  {
   TRACE(1, "H264\tEncoder\tVideo grab too small, Close down video transmission thread");
   return 0;
  }

//  cout << "here2\n";

  frameHeader * header = (frameHeader *)srcRTP.GetPayloadPtr();
  if (header->x != 0 || header->y != 0)
  {
    TRACE(1, "H264\tEncoder\tVideo grab of partial frame unsupported, Close down video transmission thread");
    return 0;
  }

//  cout << "here3\n";

  // do a validation of size
  // if the incoming data has changed size, tell the encoder
  if ((unsigned)_context.i_width != header->width || (unsigned)_context.i_height != header->height)
  {
    X264_ENCODER_CLOSE(_codec);
    _context.i_width = header->width;
    _context.i_height = header->height;
    _codec = X264_ENCODER_OPEN(&_context);
    if (_codec == NULL) {
      printf("H264\tEncoder\tCouldn't init x264 encoder\n");
    } 
    else
    {
      printf("H264\tEncoder\tx264 encoder successfully opened\n");
    }
  } 

  bool wantIFrame = false;
  x264_nal_t* NALs;
  int numberOfNALs = 0;
  x264_picture_t dummyOutput;

  // Prepare the frame to be encoded
  _inputFrame.img.plane[0] = (uint8_t *)(((unsigned char *)header) + sizeof(frameHeader));
  _inputFrame.img.i_stride[0] = header->width;
  _inputFrame.img.plane[1] = (uint8_t *)((((unsigned char *)header) + sizeof(frameHeader)) 
                           + (int)(_inputFrame.img.i_stride[0]*header->height));
  _inputFrame.img.i_stride[1] = 
  _inputFrame.img.i_stride[2] = (int) ( header->width / 2 );
  _inputFrame.img.plane[2] = (uint8_t *)(_inputFrame.img.plane[1] + (int)(_inputFrame.img.i_stride[1] *header->height/2));
  _inputFrame.i_type = X264_TYPE_AUTO;
//  next line enables sending i-frame to response for incoming fast update needed signal
  _inputFrame.i_type = (flags && forceIFrame) ? X264_TYPE_IDR : X264_TYPE_AUTO;

//  cout << "here6\n";

  while (numberOfNALs==0) { // workaround for first 2 packets being 0
    if (X264_ENCODER_ENCODE(_codec, &NALs, &numberOfNALs, &_inputFrame, &dummyOutput) < 0) {
      TRACE (1,"H264\tEncoder\tEncoding failed");
      return 0;
    } 
//    cout << "Empty\n";
  }
//  cout << "frame type " << _inputFrame.i_type << "\n";
//    cout << "Enc1\n";

//  cout << "here7\n";
  
  _txH264Frame->BeginNewFrame();
  _txH264Frame->SetFromFrame(NALs, numberOfNALs);
  _txH264Frame->SetTimestamp(srcRTP.GetTimestamp());
  _frameCounter++; 

//  cout << "here8\n";

  if (_txH264Frame->HasRTPFrames())
  {
    _txH264Frame->GetRTPFrame(dstRTP, flags);
    dstLen = dstRTP.GetFrameLen();
    return 1;
  }
//  cout << "here9\n";
  
  return 1;
}
