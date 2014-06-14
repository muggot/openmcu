/*
 * H.261 Plugin codec for OpenH323/OPAL
 *
 * Copyright (C) 2005 Post Increment, All Rights Reserved
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
 * Contributor(s): Michele Piccini (michele@piccini.com)
 *                 Derek Smithies (derek@indranet.co.nz)
 *
 * $Revision: 1.6 $
 * $Author: willamowius $
 * $Date: 2012/09/13 08:26:24 $
 */

/*
  Notes
  -----

  This codec implements a H.261 encoder and decoder with RTP packaging as per 
  RFC 2032 "RTP Payload Format for H.261 Video Streams". As per this specification,
  The RTP payload code is always set to 31

  The encoder only creates I-frames

  The decoder can accept either I-frames or P-frames

  There are seperate encoder/decoder pairs which ensures that the encoder/decoder
  always knows whether it will be receiving CIF or QCIF data, and a third pair
  which will determine the size from the received data stream.
 */

//#define DEBUG_OUTPUT 1

#define _CRT_NONSTDC_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1

#ifdef _WIN32
//#include <openh323buildopts.h>
#if H323_STATIC_H261
  #define OPAL_STATIC_CODEC 1
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#ifndef PLUGIN_CODEC_DLL_EXPORTS
#include "opal/plugin-config.h"
#endif

#include "opal/opalplugin.h"


#if defined (_WIN32) || defined (_WIN32_WCE)
  #ifndef _WIN32_WCE
    #define STRCMPI  _strcmpi
  #else
    #define STRCMPI  _stricmp
    #define strdup _strdup
  #endif
#else
  #define STRCMPI  strcasecmp
  #include <unistd.h>
  #include <semaphore.h>
#endif

#include <math.h>

#include "opal/critsect.h"
#include "opal/rtpframe.h"

#ifdef _MSC_VER
#pragma warning(disable:4800)
#endif

#define BEST_ENCODER_QUALITY   1
#define WORST_ENCODER_QUALITY 31
#define DEFAULT_ENCODER_QUALITY 10

#define DEFAULT_FILL_LEVEL     5

#define RTP_RFC2032_PAYLOAD   31
#define RTP_DYNAMIC_PAYLOAD   96

#define H261_CLOCKRATE    90000
#define H261_BITRATE      621700

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;

#include "vic/p64.h"
#include "vic/p64encoder.h"

#ifdef _WIN32
  #undef min
  #undef max
#endif

#ifdef _MSC_VER
   #include "opal/trace.h"
#else
   #include "opal/trace.h"
#endif


#if DEBUG_OUTPUT
#include <stdio.h>

#ifdef _WIN32
#include <io.h>
#define CREAT(f) _open(f, _O_WRONLY | _O_TRUNC | _O_CREAT | _O_BINARY, 0600 )
#else
#include <sys/types.h>
#include <sys/stat.h>
#define CREAT(f) ::creat(f, 0666)
#endif
#include <fcntl.h>

static void debug_write_data(int & fd, const char * title, const char * filename, const void * buffer, size_t writeLen)
{
  if (fd == -1) {
   if ((fd = CREAT(filename)) < 0) 
    printf("cannot open %s file\n", title);
  }
  if (fd >= 0) {
    size_t len = write(fd, buffer, writeLen);
    if (len != writeLen) 
      printf("failed to write %s buffer - %i != %i\n", title, len, writeLen);
    else
      printf("wrote %i bytes to %s\n", writeLen, title);
  }
}

#endif

#define RTP_MIN_HEADER_SIZE 12

/////////////////////////////////////////////////////////////////////////////

class H261EncoderContext 
{
  public:
    P64Encoder * videoEncoder;
    unsigned frameWidth;
    unsigned frameHeight;

    bool forceIFrame;
    int videoQuality;
    unsigned long lastTimeStamp;
    CriticalSection mutex;
  
    H261EncoderContext()
    {
      frameWidth = frameHeight = 0;
      videoEncoder = new P64Encoder(DEFAULT_ENCODER_QUALITY, DEFAULT_FILL_LEVEL);
      forceIFrame = false;
      videoQuality = DEFAULT_ENCODER_QUALITY;
    }

    ~H261EncoderContext()
    {
      delete videoEncoder;
    }

    void SetFrameSize(int w, int h)
    {
      frameWidth = w;
      frameHeight = h;
      videoEncoder->SetSize(frameWidth, frameHeight);
    }

    void GetFrameSize(int & w, int & h)
    {
      w = frameWidth;
      h = frameHeight;
    }

    /* The only way to influence the amount of data that the H.261 codec produces is 
    to change the "quality" variable that specifies the static quantisation level for
    BACKGROUND blocks. This "quality" value divided by 2 is used as quantisation level
    for IDLE blocks. MOTION blocks are always transmitted with quantisation level 1.
    One could map the TSTO values 1-to-1 to the quality, but that would lead to sending
    with lower quality than possible with higher bitrates. The following two mean-looking 
    functions scale the TSTO value in dependence of the bitrate and resolution (CIF or QCIF).
      
    At QCIF resolution, TSTO values 0..31 will be mapped to quality values 1..31 at
    bitrates <=128kbit/s. At higher bitrates, the same TSTO values will be 
    mapped to lower quality values, e.g. to 1..19 at 256kbit/s and 1..5 at 512kbit/s. Above 
    approx. 656kbit/s, all TSTO values will be mapped to a quality of 1.
      
    At CIF, TSTO values 0..31 will be mapped to quality values 1..31 at up to 224kbit/s, to 1..11 
    at 512kbit/s and to 1 at bitrates >=912kbit/s. 
      
    The basis of these functions is a nonlinear regression based on the observation of the 
    effectve framrate obtained at specific quality settings and bitrates. */
    
    void SetQualityFromTSTO (int tsto, unsigned bitrate, int width, int height)
    {
      if (tsto == -1) 
        return;

      if ((width==CIF_WIDTH) && (height==CIF_HEIGHT)) {
        double bitrate_d = std::max((int)bitrate, 128000);
        double factor =  std::max (   0.0031 * pow ((double) (bitrate_d/64000), 4)
	                            - 0.0758 * pow ((double) (bitrate_d/64000), 3)
			            + 0.6518 * pow ((double) (bitrate_d/64000), 2)
			            - 1.9377 * (double) (bitrate_d/64000)
			            + 2.5342
			            , 1.0);
        videoQuality = std::max ((int)( floor ( tsto / factor)), 1);
      } 
      else if ((width==QCIF_WIDTH) && (height==QCIF_HEIGHT)) {
        double bitrate_d = std::max((int)bitrate, 64000);
        double factor =  std::max ( 0.0036 * pow ((double) (bitrate_d / 64000), 4)
	                          - 0.0462 * pow ((double) (bitrate_d / 64000), 3)
				  + 0.2792 * pow ((double) (bitrate_d / 64000), 2)
				  - 0.5321 * (double) (bitrate_d / 64000)
				  + 1.3438    -0.0844
				  , 1.0);
	videoQuality = std::max ((int)( floor ( tsto / factor)), 1); 
      } 
      TRACE(4, "H261\tf(tsto=" << tsto << ", bitrate=" << bitrate << ", width=" << width <<", height=" << height << ")=" << videoQuality);
    }

    int EncodeFrames(const u_char * src, unsigned & srcLen, u_char * dst, unsigned & dstLen, unsigned int & flags)
    {
      WaitAndSignal m(mutex);

      // create RTP frame from source buffer
      RTPFrame srcRTP(src, srcLen);

      // create RTP frame from destination buffer
      RTPFrame dstRTP(dst, dstLen, RTP_RFC2032_PAYLOAD);
      dstLen = 0;

      // return more pending data frames, if any
      if (videoEncoder->MoreToIncEncode()) {
        unsigned payloadLength = 0;
        videoEncoder->IncEncodeAndGetPacket((u_char *)dstRTP.GetPayloadPtr(), payloadLength); //get next packet on list
        dstLen = SetEncodedPacket(dstRTP, !videoEncoder->MoreToIncEncode(), RTP_RFC2032_PAYLOAD, lastTimeStamp, payloadLength, flags);
#if DEBUG_OUTPUT
static int encoderOutput = -1;
debug_write_data(encoderOutput, "encoder output", "encoder.output", dstRTP.GetPacketPtr(), dstLen);
#endif
        return 1;
      }

      //
      // from here on, this is encoding a new frame
      //

      // get the timestamp we will be using for the rest of the frame
      lastTimeStamp = srcRTP.GetTimestamp();

      // update the video quality
      videoEncoder->SetQualityLevel(videoQuality);

      // get and validate header
      if (srcRTP.GetPayloadSize() < sizeof(PluginCodec_Video_FrameHeader)) {
        TRACE(1,"H261\tVideo grab too small");
        return 0;
      } 

      PluginCodec_Video_FrameHeader * header = (PluginCodec_Video_FrameHeader *)srcRTP.GetPayloadPtr();
      if (header->x != 0 && header->y != 0) {
        TRACE(1,"H261\tVideo grab of partial frame unsupported");
        return 0;
      }

      // make sure the incoming frame is big enough for the specified frame size
      if (srcRTP.GetPayloadSize() < (int)(sizeof(PluginCodec_Video_FrameHeader) + frameWidth*frameHeight*12/8)) {
        TRACE(1,"H261\tPayload of grabbed frame too small for full frame");
        return 0;
      }

      if ((header->width  != 176 && header->width  != 352) ||
          (header->height != 144 && header->height != 288)) {
        TRACE(1,"H261\tInvalid frame size");
        return 0;
      }

      // if the incoming data has changed size, tell the encoder
      if (frameWidth != header->width || frameHeight != header->height) {
        frameWidth = header->width;
        frameHeight = header->height;
        videoEncoder->SetSize(frameWidth, frameHeight);
      }

#if DEBUG_OUTPUT
static int encoderYUV = -1;
debug_write_data(encoderYUV, "encoder input", "encoder.yuv", OPAL_VIDEO_FRAME_DATA_PTR(header),
                                                             srcRTP.GetPayloadSize() - sizeof(PluginCodec_Video_FrameHeader));
#endif

      // "grab" the frame
      memcpy(videoEncoder->GetFramePtr(), OPAL_VIDEO_FRAME_DATA_PTR(header), frameWidth*frameHeight*12/8);

      // force I-frame, if requested
      if (forceIFrame || (flags & PluginCodec_CoderForceIFrame) != 0) {
        videoEncoder->FastUpdatePicture();
        forceIFrame = false;
      }

      // preprocess the data
      videoEncoder->PreProcessOneFrame();

      // get next frame from list created by preprocessor
      if (!videoEncoder->MoreToIncEncode())
        dstLen = 0;
      else {
        unsigned payloadLength = 0;
        videoEncoder->IncEncodeAndGetPacket((u_char *)dstRTP.GetPayloadPtr(), payloadLength); 
        dstLen = SetEncodedPacket(dstRTP, !videoEncoder->MoreToIncEncode(), RTP_RFC2032_PAYLOAD, lastTimeStamp, payloadLength, flags);

#if DEBUG_OUTPUT
debug_write_data(encoderOutput, "encoder output", "encoder.output", dstRTP.GetPacketPtr(), dstLen);
#endif

      }

      return 1;
    }

  protected:
    unsigned SetEncodedPacket(RTPFrame & dstRTP, bool isLast, unsigned char payloadCode, unsigned long lastTimeStamp, unsigned payloadLength, unsigned & flags);
};



unsigned H261EncoderContext::SetEncodedPacket(RTPFrame & dstRTP, bool isLast, unsigned char payloadCode, unsigned long lastTimeStamp, unsigned payloadLength, unsigned & flags)
{
  dstRTP.SetPayloadSize(payloadLength);
  dstRTP.SetMarker(isLast);
  dstRTP.SetPayloadType(payloadCode);
  dstRTP.SetTimestamp(lastTimeStamp);

  flags = 0;
  flags |= isLast ? PluginCodec_ReturnCoderLastFrame : 0;  // marker bit on last frame of video
  flags |= PluginCodec_ReturnCoderIFrame;                       // sadly, this encoder *always* returns I-frames :(

  return dstRTP.GetFrameLen();
}

static void * create_encoder(const struct PluginCodec_Definition * /*codec*/)
{
  return new H261EncoderContext;
}

static int encoder_set_options(const PluginCodec_Definition *, 
                               void * _context,
                               const char * , 
                               void * parm, 
                               unsigned * parmLen)
{
  int width=0;
  int height=0;
  int tsto=-1;
  unsigned bitrate=H261_BITRATE;

  H261EncoderContext * context = (H261EncoderContext *)_context;
  if (parmLen == NULL || *parmLen != sizeof(const char **))
    return 0;
  
  context->GetFrameSize (width, height);	// fetch old value, needed for quality computation
  if (parm != NULL) {
    const char ** options = (const char **)parm;
    int i;
    for (i = 0; options[i] != NULL; i += 2) {
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_HEIGHT) == 0)
        height = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_FRAME_WIDTH) == 0)
        width = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_TARGET_BIT_RATE) == 0)
        bitrate = atoi(options[i+1]);
      if (STRCMPI(options[i], PLUGINCODEC_OPTION_TEMPORAL_SPATIAL_TRADE_OFF) == 0)
        tsto = atoi(options[i+1]);
    }
  }
  context->SetFrameSize (width, height);
  context->SetQualityFromTSTO (tsto, bitrate, width, height);

  return 1;
}

static void destroy_encoder(const struct PluginCodec_Definition * /*codec*/, void * _context)
{
  H261EncoderContext * context = (H261EncoderContext *)_context;
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
  H261EncoderContext * context = (H261EncoderContext *)_context;
  return context->EncodeFrames((const u_char *)from, *fromLen, (u_char *)to, *toLen, *flag);
}

static int encoder_get_output_data_size(const PluginCodec_Definition *, void *, const char *, void *, unsigned *)
{
  return RTP_MTU + RTP_MIN_HEADER_SIZE;
}

/////////////////////////////////////////////////////////////////////////////

class H261DecoderContext
{
  public:
    u_char * rvts;
    P64Decoder * videoDecoder;
    u_short expectedSequenceNumber;
    int ndblk, nblk;
    int now;
    bool packetReceived;
    unsigned frameWidth;
    unsigned frameHeight;

    CriticalSection mutex;

  public:
    H261DecoderContext()
    {
      rvts = NULL;

      videoDecoder = new FullP64Decoder();
      videoDecoder->marks(rvts);

      expectedSequenceNumber = 0;
      nblk = ndblk = 0;
      now = 1;
      packetReceived = false;
      frameWidth = frameHeight = 0;

      // Create the actual decoder
    }

    ~H261DecoderContext()
    {
      if (rvts)
        delete [] rvts;
      delete videoDecoder;
    }

    int DecodeFrames(const u_char * src, unsigned & srcLen, u_char * dst, unsigned & dstLen, unsigned int & flags)
    {
      WaitAndSignal m(mutex);

      // create RTP frame from source buffer
      RTPFrame srcRTP(src, srcLen);

      // create RTP frame from destination buffer
      RTPFrame dstRTP(dst, dstLen, 0);
      dstLen = 0;
      flags = 0;

      // Check for lost packets to help decoder
      bool lostPreviousPacket = false;
      if ((expectedSequenceNumber == 0) || (expectedSequenceNumber != srcRTP.GetSequenceNumber())) {
        lostPreviousPacket = true;
        TRACE(3,"H261\tDetected loss of one video packet. "
    	      << expectedSequenceNumber << " != "
              << srcRTP.GetSequenceNumber() << " Will recover.");
      }
      expectedSequenceNumber = (u_short)(srcRTP.GetSequenceNumber()+1);

#if DEBUG_OUTPUT
static int decoderInput = -1;
debug_write_data(decoderInput, "decoder input", "decoder.input", srcRTP.GetPacketPtr(), srcRTP.GetFrameLen());
#endif

      videoDecoder->mark(now);
      if (!videoDecoder->decode(srcRTP.GetPayloadPtr(), srcRTP.GetPayloadSize(), lostPreviousPacket)) {
        flags = PluginCodec_ReturnCoderRequestIFrame;
        return 1;
      }

      //Check for a resize - can change at any time!
      if (frameWidth  != (unsigned)videoDecoder->width() ||
          frameHeight != (unsigned)videoDecoder->height()) {

        frameWidth = videoDecoder->width();
        frameHeight = videoDecoder->height();

        nblk = (frameWidth * frameHeight) / 64;
        delete [] rvts;
        rvts = new u_char[nblk];
        memset(rvts, 0, nblk);
        videoDecoder->marks(rvts);
      }

      // Have not built an entire frame yet
      if (!srcRTP.GetMarker())
        return 1;

      videoDecoder->sync();
      ndblk = videoDecoder->ndblk();

      int wraptime = now ^ 0x80;
      u_char * ts = rvts;
      int k;
      for (k = nblk; --k >= 0; ++ts) {
        if (*ts == wraptime)
          *ts = (u_char)now;
      }

      now = (now + 1) & 0xff;

      int frameBytes = (frameWidth * frameHeight * 12) / 8;
      dstRTP.SetPayloadSize(sizeof(PluginCodec_Video_FrameHeader) + frameBytes);
      dstRTP.SetPayloadType(RTP_DYNAMIC_PAYLOAD);
      dstRTP.SetMarker(true);

      PluginCodec_Video_FrameHeader * frameHeader = (PluginCodec_Video_FrameHeader *)dstRTP.GetPayloadPtr();
      frameHeader->x = frameHeader->y = 0;
      frameHeader->width = frameWidth;
      frameHeader->height = frameHeight;
      memcpy(OPAL_VIDEO_FRAME_DATA_PTR(frameHeader), videoDecoder->GetFramePtr(), frameBytes);

      videoDecoder->resetndblk();

      dstLen = dstRTP.GetFrameLen();

#if DEBUG_OUTPUT
static int decoderOutput = -1;
debug_write_data(decoderOutput, "decoder output", "decoder.yuv", OPAL_VIDEO_FRAME_DATA_PTR(frameHeader),
                                                                 dstRTP.GetPayloadSize() - sizeof(PluginCodec_Video_FrameHeader));
#endif
      flags = PluginCodec_ReturnCoderLastFrame | PluginCodec_ReturnCoderIFrame;   // TODO: THIS NEEDS TO BE CHANGED TO DO CORRECT I-FRAME DETECTION
      return 1;
    }
};


/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_information licenseInfo = {
  1143692893,                                                   // timestamp = Thu 30 Mar 2006 04:28:13 AM UTC

  "Craig Southeren, Post Increment",                            // source code author
  "1.0",                                                        // source code version
  "craigs@postincrement.com",                                   // source code email
  "http://www.postincrement.com",                               // source code URL
  "Copyright (C) 2004 by Post Increment, All Rights Reserved",  // source code copyright
  "MPL 1.0",                                                    // source code license
  PluginCodec_License_MPL,                                      // source code license
  
  "VIC H.261",                                                   // codec description
  "",                                                            // codec author
  "",                                                            // codec version
  "",                                                            // codec email
  "",                                                            // codec URL
  "Copyright (c) 1994 Regents of the University of California",  // codec copyright information
  NULL,                                                          // codec license
  PluginCodec_License_BSD                                        // codec license code
};

static const char YUV420PDesc[]  = { "YUV420P" };

static const char h261QCIFDesc[]  = { "H.261-QCIF" };
static const char h261CIFDesc[]   = { "H.261-CIF" };
static const char h261720Desc[]   = { "H.261-720" };
static const char h261Desc[]      = { "H.261" };

static const char sdpH261[]   = { "h261" };

static const char QCIF_MPI[]  = PLUGINCODEC_QCIF_MPI;
static const char  CIF_MPI[]  =  PLUGINCODEC_CIF_MPI;


static void * create_decoder(const struct PluginCodec_Definition *)
{
  return new H261DecoderContext;
}


static int decoder_set_options(
      const struct PluginCodec_Definition *, 
      void * _context, 
      const char *, 
      void * parm, 
      unsigned * parmLen)
{
  H261DecoderContext * context = (H261DecoderContext *)_context;

  if (parmLen == NULL || *parmLen != sizeof(const char **) || parm == NULL)
    return 0;

  // get the "frame width" media format parameter to use as a hint for the encoder to start off
  for (const char * const * option = (const char * const *)parm; *option != NULL; option += 2) {
    if (STRCMPI(option[0], "Frame Width") == 0) {
      context->videoDecoder->fmt_ = (atoi(option[1]) == QCIF_WIDTH) ? IT_QCIF : IT_CIF;
      context->videoDecoder->init();
    }
  }

  return 1;
}


static void destroy_decoder(const struct PluginCodec_Definition * /*codec*/, void * _context)
{
  H261DecoderContext * context = (H261DecoderContext *)_context;
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
  H261DecoderContext * context = (H261DecoderContext *)_context;
  return context->DecodeFrames((const u_char *)from, *fromLen, (u_char *)to, *toLen, *flag);
}


static int decoder_get_output_data_size(const PluginCodec_Definition * codec, void *, const char *, void *, unsigned *)
{
  // this is really frame height * frame width;
  return sizeof(PluginCodec_Video_FrameHeader) + ((codec->parm.video.maxFrameWidth * codec->parm.video.maxFrameHeight * 3) / 2);
}


static int get_codec_options(const struct PluginCodec_Definition * codec,
                                 void *, 
                                 const char *,
                                 void * parm,
                                 unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(struct PluginCodec_Option **))
    return 0;

  *(const void **)parm = codec->userData;
  *parmLen = 0;
  return 1;
}


static char * num2str(int num)
{
  char buf[20];
  sprintf(buf, "%i", num);
  return strdup(buf);
}


static int ClampSize(int pixels, int maximum)
{
  // Set to nearest of the two possible sizes
  return pixels < maximum ? maximum/2 : maximum;
}


/* Convert the custom options for the codec to normalised options.
   For H.261 the custom options are "QCIF MPI" and "CIF MPI" which will
   restrict the min/max width/height and maximum frame rate.
 */
static int to_normalised_options(const struct PluginCodec_Definition *, void *, const char *, void * parm, unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char ***))
    return 0;

  int qcif_mpi = PLUGINCODEC_MPI_DISABLED;
  int cif_mpi = PLUGINCODEC_MPI_DISABLED;
  int frameWidth = 352;
  int frameHeight = 288;
  for (const char * const * option = *(const char * const * *)parm; *option != NULL; option += 2) {
    if (STRCMPI(option[0], PLUGINCODEC_QCIF_MPI) == 0)
      qcif_mpi = atoi(option[1]);
    else if (STRCMPI(option[0], PLUGINCODEC_CIF_MPI) == 0)
      cif_mpi = atoi(option[1]);
    else if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_WIDTH) == 0)
      frameWidth = ClampSize(atoi(option[1]), 352);
    else if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_HEIGHT) == 0)
      frameHeight = ClampSize(atoi(option[1]), 288);
  }

  int minWidth = 176;
  int minHeight = 144;
  int maxWidth = 352;
  int maxHeight = 288;
  int frameTime = 3003;
  if (qcif_mpi != PLUGINCODEC_MPI_DISABLED && cif_mpi != PLUGINCODEC_MPI_DISABLED)
    frameTime = 3003*(qcif_mpi > cif_mpi ? qcif_mpi : cif_mpi);
  else if (qcif_mpi != PLUGINCODEC_MPI_DISABLED) {
    maxWidth = 176;
    maxHeight = 144;
    frameTime = 3003*qcif_mpi;
  }
  else if (cif_mpi != PLUGINCODEC_MPI_DISABLED) {
    minWidth = 352;
    minHeight = 288;
    frameTime = 3003*cif_mpi;
  }
  else {
#ifdef DEFAULT_TO_FULL_CAPABILITIES
    maxWidth = 352;
    maxHeight = 288;
    frameTime = 3003*1;
#else
    // Default handling according to RFC 4587 (QCIF=1)
    maxWidth = 176;
    maxHeight = 144;
    frameTime = 3003*1;
#endif
  }

  char ** options = (char **)calloc(15, sizeof(char *));
  *(char ***)parm = options;
  if (options == NULL)
    return 0;

  options[ 0] = strdup(PLUGINCODEC_OPTION_FRAME_WIDTH);
  options[ 1] = num2str(frameWidth);
  options[ 2] = strdup(PLUGINCODEC_OPTION_FRAME_HEIGHT);
  options[ 3] = num2str(frameHeight);
  options[ 4] = strdup(PLUGINCODEC_OPTION_MIN_RX_FRAME_WIDTH);
  options[ 5] = num2str(minWidth);
  options[ 6] = strdup(PLUGINCODEC_OPTION_MIN_RX_FRAME_HEIGHT);
  options[ 7] = num2str(minHeight);
  options[ 8] = strdup(PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH);
  options[ 9] = num2str(maxWidth);
  options[10] = strdup(PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT);
  options[11] = num2str(maxHeight);
  options[12] = strdup(PLUGINCODEC_OPTION_FRAME_TIME);
  options[13] = num2str(frameTime);

  return 1;
}


/* Convert the normalised options to the codec custom options.
   For H.261 the custom options are "QCIF MPI" and "CIF MPI" which are
   set according to the min/max width/height and frame time.
 */
static int to_customised_options(const struct PluginCodec_Definition *, void *, const char *, void * parm, unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char ***))
    return 0;

  int frameWidth = 352;
  int frameHeight = 288;
  int minWidth = 176;
  int minHeight = 144;
  int maxWidth = 352;
  int maxHeight = 288;
  int mpi = 1;
  for (const char * const * option = *(const char * const * *)parm; *option != NULL; option += 2) {
    if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_WIDTH) == 0)
      frameWidth = ClampSize(atoi(option[1]), 352);
    else if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_HEIGHT) == 0)
      frameHeight = ClampSize(atoi(option[1]), 288);
    else if (STRCMPI(option[0], PLUGINCODEC_OPTION_MIN_RX_FRAME_WIDTH) == 0)
      minWidth = ClampSize(atoi(option[1]), 352);
    else if (STRCMPI(option[0], PLUGINCODEC_OPTION_MIN_RX_FRAME_HEIGHT) == 0)
      minHeight = ClampSize(atoi(option[1]), 288);
    else if (STRCMPI(option[0], PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH) == 0)
      maxWidth = ClampSize(atoi(option[1]), 352);
    else if (STRCMPI(option[0], PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT) == 0)
      maxHeight = ClampSize(atoi(option[1]), 288);
    else if (STRCMPI(option[0], PLUGINCODEC_OPTION_FRAME_TIME) == 0)
      mpi = atoi(option[1])/3003;
  }

  int qcif_mpi = 1;
  int cif_mpi = 1;
  if (minWidth > 176 || minHeight > 144)
    qcif_mpi = PLUGINCODEC_MPI_DISABLED;
  if (maxWidth < 352 || maxHeight < 288)
    cif_mpi = PLUGINCODEC_MPI_DISABLED;

  if (mpi < 1)
    mpi = 1;
  else if (mpi > 4)
    mpi = 4;
  if (qcif_mpi < PLUGINCODEC_MPI_DISABLED && mpi > qcif_mpi)
    qcif_mpi = mpi;
  if (cif_mpi < PLUGINCODEC_MPI_DISABLED && mpi > cif_mpi)
    cif_mpi = mpi;

  char ** options = (char **)calloc(17, sizeof(char *));
  *(char ***)parm = options;
  if (options == NULL)
    return 0;

  options[ 0] = strdup(PLUGINCODEC_OPTION_FRAME_WIDTH);
  options[ 1] = num2str(frameWidth);
  options[ 2] = strdup(PLUGINCODEC_OPTION_FRAME_HEIGHT);
  options[ 3] = num2str(frameHeight);
  options[ 4] = strdup(PLUGINCODEC_OPTION_MIN_RX_FRAME_WIDTH);
  options[ 5] = num2str(minWidth);
  options[ 6] = strdup(PLUGINCODEC_OPTION_MIN_RX_FRAME_HEIGHT);
  options[ 7] = num2str(minHeight);
  options[ 8] = strdup(PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH);
  options[ 9] = num2str(maxWidth);
  options[10] = strdup(PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT);
  options[11] = num2str(maxHeight);
  options[12] = strdup(PLUGINCODEC_QCIF_MPI);
  options[13] = num2str(qcif_mpi);
  options[14] = strdup(PLUGINCODEC_CIF_MPI);
  options[15] = num2str(cif_mpi);

  return 1;
}
  

static int free_codec_options(const struct PluginCodec_Definition *, void *, const char *, void * parm, unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char ***))
    return 0;

  char ** strings = (char **) parm;
  for (char ** string = strings; *string != NULL; string++)
    free(*string);
  free(strings);
  return 1;
}


static int valid_for_protocol(const struct PluginCodec_Definition *, void *, const char *, void * parm, unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char *))
    return 0;

  return (STRCMPI((const char *)parm, "h.323") == 0 ||
          STRCMPI((const char *)parm, "h323") == 0) ? 1 : 0;

}


static PluginCodec_ControlDefn h323EncoderControls[] = {
  { PLUGINCODEC_CONTROL_VALID_FOR_PROTOCOL,    valid_for_protocol },
  { PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS,     get_codec_options },
  { PLUGINCODEC_CONTROL_TO_NORMALISED_OPTIONS, to_normalised_options },
  { PLUGINCODEC_CONTROL_TO_CUSTOMISED_OPTIONS, to_customised_options },
  { PLUGINCODEC_CONTROL_FREE_CODEC_OPTIONS,    free_codec_options },
  { PLUGINCODEC_CONTROL_SET_CODEC_OPTIONS,     encoder_set_options },
  { PLUGINCODEC_CONTROL_GET_OUTPUT_DATA_SIZE,  encoder_get_output_data_size },
  { NULL }
};

static PluginCodec_ControlDefn h323DecoderControls[] = {
  { PLUGINCODEC_CONTROL_VALID_FOR_PROTOCOL,    valid_for_protocol },
  { PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS,     get_codec_options },
  { PLUGINCODEC_CONTROL_TO_NORMALISED_OPTIONS, to_normalised_options },
  { PLUGINCODEC_CONTROL_TO_CUSTOMISED_OPTIONS, to_customised_options },
  { PLUGINCODEC_CONTROL_FREE_CODEC_OPTIONS,    free_codec_options },
  { PLUGINCODEC_CONTROL_SET_CODEC_OPTIONS,     decoder_set_options },
  { PLUGINCODEC_CONTROL_GET_OUTPUT_DATA_SIZE,  decoder_get_output_data_size },
  { NULL }
};

static PluginCodec_ControlDefn EncoderControls[] = {
  { PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS,     get_codec_options },
  { PLUGINCODEC_CONTROL_TO_NORMALISED_OPTIONS, to_normalised_options },
  { PLUGINCODEC_CONTROL_TO_CUSTOMISED_OPTIONS, to_customised_options },
  { PLUGINCODEC_CONTROL_FREE_CODEC_OPTIONS,    free_codec_options },
  { PLUGINCODEC_CONTROL_SET_CODEC_OPTIONS,     encoder_set_options },
  { PLUGINCODEC_CONTROL_GET_OUTPUT_DATA_SIZE,  encoder_get_output_data_size },
  { NULL }
};

static PluginCodec_ControlDefn DecoderControls[] = {
  { PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS,     get_codec_options },
  { PLUGINCODEC_CONTROL_TO_NORMALISED_OPTIONS, to_normalised_options },
  { PLUGINCODEC_CONTROL_TO_CUSTOMISED_OPTIONS, to_customised_options },
  { PLUGINCODEC_CONTROL_FREE_CODEC_OPTIONS,    free_codec_options },
  { PLUGINCODEC_CONTROL_SET_CODEC_OPTIONS,     decoder_set_options },
  { PLUGINCODEC_CONTROL_GET_OUTPUT_DATA_SIZE,  decoder_get_output_data_size },
  { NULL }
};

/* The annex below is turned off and set to read/only because this
   implementation does not support them. It's presence here is so that if
   someone out there does a different implementation of the codec and copies
   this file as a template, they will get them and hopefully notice that they
   can just make it read/write and/or turned on.
 */
static struct PluginCodec_Option const annexD =
{
  PluginCodec_BoolOption,               // Option type
  "Annex D",                            // User visible name
  true,                                 // User Read/Only flag
  PluginCodec_AndMerge,                 // Merge mode
  "0",                                  // Initial value
  "D",                                  // FMTP option name
  "0"                                   // FMTP default value
};

#define DECLARE_OPTIONS(prefix) \
static struct PluginCodec_Option const prefix##_sqcifMPI = \
  { PluginCodec_IntegerOption, "SQCIF MPI", false, PluginCodec_MaxMerge, prefix##_SQCIF_MPI, "SQCIF", "0", 0, "0", "4" }; \
static struct PluginCodec_Option const prefix##_qcifMPI = \
  { PluginCodec_IntegerOption, "QCIF MPI", false, PluginCodec_MaxMerge, prefix##_QCIF_MPI, "QCIF", "0", 0, "0", "4" }; \
static struct PluginCodec_Option const prefix##_cifMPI = \
  { PluginCodec_IntegerOption, "CIF MPI",  false, PluginCodec_MaxMerge, prefix##_CIF_MPI, "CIF",  "0", 0, "0", "4" }; \
static struct PluginCodec_Option const prefix##_cif4MPI = \
  { PluginCodec_IntegerOption, "CIF4 MPI",  false, PluginCodec_MaxMerge, prefix##_CIF4_MPI, "CIF4", "0", 0, "0", "4" }; \
static struct PluginCodec_Option const prefix##_cif16MPI = \
  { PluginCodec_IntegerOption, "CIF16 MPI", false, PluginCodec_MaxMerge, prefix##_CIF16_MPI, "CIF16", "0", 0, "0", "4" }; \
static struct PluginCodec_Option const prefix##_minRxFrameWidth = \
  { PluginCodec_IntegerOption, PLUGINCODEC_OPTION_MIN_RX_FRAME_WIDTH,  true, PluginCodec_NoMerge, prefix##_MinMaxFrameWidth, NULL, NULL, 0, prefix##_MinMaxFrameWidth, prefix##_MinMaxFrameWidth }; \
static struct PluginCodec_Option const prefix##_minRxFrameHeight = \
  { PluginCodec_IntegerOption, PLUGINCODEC_OPTION_MIN_RX_FRAME_HEIGHT, true, PluginCodec_NoMerge, prefix##_MinMaxFrameHeight, NULL, NULL, 0, prefix##_MinMaxFrameHeight, prefix##_MinMaxFrameHeight }; \
static struct PluginCodec_Option const prefix##_maxRxFrameWidth = \
  { PluginCodec_IntegerOption, PLUGINCODEC_OPTION_MAX_RX_FRAME_WIDTH,  true, PluginCodec_NoMerge, prefix##_MinMaxFrameWidth, NULL, NULL, 0, prefix##_MinMaxFrameWidth, prefix##_MinMaxFrameWidth }; \
static struct PluginCodec_Option const prefix##_maxRxFrameHeight = \
  { PluginCodec_IntegerOption, PLUGINCODEC_OPTION_MAX_RX_FRAME_HEIGHT, true, PluginCodec_NoMerge, prefix##_MinMaxFrameHeight, NULL, NULL, 0, prefix##_MinMaxFrameHeight, prefix##_MinMaxFrameHeight }; \
 \
static struct PluginCodec_Option const * const prefix##_OptionTable[] = { \
  &prefix##_sqcifMPI, \
  &prefix##_qcifMPI, \
  &prefix##_cifMPI, \
  &prefix##_cif4MPI, \
  &prefix##_cif16MPI, \
  &annexD, \
  &prefix##_minRxFrameWidth, \
  &prefix##_minRxFrameHeight, \
  &prefix##_maxRxFrameWidth, \
  &prefix##_maxRxFrameHeight, \
  NULL \
}; \


/////////////////////////////////////////////////////////////////////////////

#define DECLARE_PARAM(prefix) \
{ \
  /* encoder */ \
  PLUGIN_CODEC_VERSION_OPTIONS,	      /* codec API version */ \
  &licenseInfo,                       /* license information */ \
  PluginCodec_MediaTypeVideo |        /* audio codec */ \
  PluginCodec_RTPTypeExplicit,        /* specified RTP type */ \
  prefix##_MediaFmt,                  /* text decription */ \
  YUV420PDesc,                        /* source format */ \
  prefix##_MediaFmt,                  /* destination format */ \
  prefix##_OptionTable,		      /* user data */ \
  H261_CLOCKRATE,                     /* samples per second */ \
  H261_BITRATE,	                      /* raw bits per second */ \
  20000,                              /* nanoseconds per frame */ \
  prefix##_FrameWidth,		      /* bytes per frame */ \
  prefix##_FrameHeight,               /* samples per frame */ \
  10,                                 /* recommended number of frames per packet */ \
  60,                                 /* maximum number of frames per packet  */ \
  RTP_RFC2032_PAYLOAD,                /* IANA RTP payload code */ \
  sdpH261,                            /* RTP payload name */ \
  create_encoder,                     /* create codec function */ \
  destroy_encoder,                    /* destroy codec */ \
  codec_encoder,                      /* encode/decode */ \
  EncoderControls,                    /* codec controls */ \
  PluginCodec_H323VideoCodec_h261,    /* h323CapabilityType */ \
  NULL                                /* h323CapabilityData */ \
}, \
{  \
  /* decoder */ \
  PLUGIN_CODEC_VERSION_OPTIONS,	      /* codec API version */ \
  &licenseInfo,                       /* license information */ \
  PluginCodec_MediaTypeVideo |        /* audio codec */ \
  PluginCodec_RTPTypeExplicit,        /* specified RTP type */ \
  prefix##_MediaFmt,                  /* text decription */ \
  prefix##_MediaFmt,                  /* source format */ \
  YUV420PDesc,                        /* destination format */ \
  prefix##_OptionTable,		      /* user data */ \
  H261_CLOCKRATE,                     /* samples per second */ \
  H261_BITRATE, 	              /* raw bits per second */ \
  20000,                              /* nanoseconds per frame */ \
  prefix##_FrameWidth,		      /* bytes per frame */ \
  prefix##_FrameHeight,               /* samples per frame */ \
  10,                                 /* recommended number of frames per packet */ \
  60,                                 /* maximum number of frames per packet  */ \
  RTP_RFC2032_PAYLOAD,                /* IANA RTP payload code */ \
  sdpH261,                            /* RTP payload name */ \
  create_decoder,                     /* create codec function */ \
  destroy_decoder,                    /* destroy codec */ \
  codec_decoder,                      /* encode/decode */ \
  DecoderControls,                    /* codec controls */ \
  PluginCodec_H323VideoCodec_h261,    /* h323CapabilityType */ \
  NULL                                /* h323CapabilityData */ \
} \

/////////////////////////////////////////////////////////////////////////////

static const char *   H261_MediaFmt             = "H.261";
static const char *   H261_SQCIF_MPI            = "0";
static const char *   H261_QCIF_MPI             = "0";
static const char *   H261_CIF_MPI              = "1";
static const char *   H261_CIF4_MPI             = "0";
static const char *   H261_CIF16_MPI            = "0";
static const char *   H261_MinMaxFrameWidth  = "352";
static const char *   H261_MinMaxFrameHeight = "288";
static unsigned int   H261_FrameWidth           = 352;
static unsigned int   H261_FrameHeight          = 288;
DECLARE_OPTIONS(H261)

static const char *   H261_QCIF_MediaFmt        = "H.261-QCIF";
static const char *   H261_QCIF_SQCIF_MPI       = "0";
static const char *   H261_QCIF_QCIF_MPI        = "1";
static const char *   H261_QCIF_CIF_MPI         = "0";
static const char *   H261_QCIF_CIF4_MPI        = "0";
static const char *   H261_QCIF_CIF16_MPI       = "0";
static const char *   H261_QCIF_MinMaxFrameWidth  = "176";
static const char *   H261_QCIF_MinMaxFrameHeight = "144";
static unsigned int   H261_QCIF_FrameWidth      = 176;
static unsigned int   H261_QCIF_FrameHeight     = 144;
DECLARE_OPTIONS(H261_QCIF)

static const char *   H261_CIF_MediaFmt         = "H.261-CIF";
static const char *   H261_CIF_SQCIF_MPI        = "0";
static const char *   H261_CIF_QCIF_MPI         = "0";
static const char *   H261_CIF_CIF_MPI          = "1";
static const char *   H261_CIF_CIF4_MPI         = "0";
static const char *   H261_CIF_CIF16_MPI        = "0";
static const char *   H261_CIF_MinMaxFrameWidth  = "352";
static const char *   H261_CIF_MinMaxFrameHeight = "288";
static unsigned int   H261_CIF_FrameWidth       = 352;
static unsigned int   H261_CIF_FrameHeight      = 288;
DECLARE_OPTIONS(H261_CIF)

/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition CodecDefn[] = {
  DECLARE_PARAM(H261),
//  DECLARE_PARAM(H261_QCIF),
//  DECLARE_PARAM(H261_CIF),
};

/////////////////////////////////////////////////////////////////////////////

extern "C" {
  PLUGIN_CODEC_IMPLEMENT(VIC_H261)

  PLUGIN_CODEC_DLL_API struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned /*version*/)
  {
#ifndef _WIN32_WCE
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
#else
    Trace::SetLevel(0);
    Trace::SetLevelUserPlane(0);
#endif

    *count = sizeof(CodecDefn) / sizeof(struct PluginCodec_Definition);
    return CodecDefn;
  }

};

/////////////////////////////////////////////////////////////////////////////
