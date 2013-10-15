/*
 * H.263 Plugin codec for OpenH323/OPAL
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
 * Copyright (C) 2007 Matthias Schneider
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
 * Contributor(s): Matthias Schneider (ma30002000@yahoo.de)
 *                 Guilhem Tardy (gtardy@salyens.com)
 *                 Craig Southeren (craigs@postincrement.com)
 *
 */

/*
  Notes
  -----

 */

#ifndef __H263P_1998_H__
#define __H263P_1998_H__ 1

#include "opalplugin.h"
#include "h263pframe.h"
#include "rfc2190.h"
#include "critsect.h"

#if TRACE_FILE
#include "tracer.h"
#endif

typedef unsigned char BYTE;

#define H263P_CLOCKRATE        90000
#define H263P_BITRATE         327600
#define H263P_PAYLOAD_SIZE       1200
#define H263P_FRAME_RATE          25
#define H263P_KEY_FRAME_INTERVAL 125
#define H263P_MIN_QUANT            2

#define H263_CLOCKRATE         90000
#define H263_BITRATE          327600
#define H263_PAYLOAD_SIZE        1200
#define RTP_RFC2190_PAYLOAD       34
#define H263_KEY_FRAME_INTERVAL  125

#define CIF_WIDTH       352
#define CIF_HEIGHT      288

#define CIF4_WIDTH      (CIF_WIDTH*2)
#define CIF4_HEIGHT     (CIF_HEIGHT*2)

#define CIF16_WIDTH     (CIF_WIDTH*4)
#define CIF16_HEIGHT    (CIF_HEIGHT*4)

#define QCIF_WIDTH     (CIF_WIDTH/2)
#define QCIF_HEIGHT    (CIF_HEIGHT/2)

#define QCIF4_WIDTH     (CIF4_WIDTH/2)
#define QCIF4_HEIGHT    (CIF4_HEIGHT/2)

#define SQCIF_WIDTH     128
#define SQCIF_HEIGHT    96

#define MAX_YUV420P_FRAME_SIZE (((CIF16_WIDTH * CIF16_HEIGHT * 3) / 2) + (FF_INPUT_BUFFER_PADDING_SIZE*2))
enum Annex {
    D,
    F,
    I,
    K,
    J,
    S,
    T,
    N,
    P
};

class H263_Base_EncoderContext
{
  public:
    H263_Base_EncoderContext(const char * prefix);
    virtual ~H263_Base_EncoderContext();

    virtual bool Open() = 0;
    virtual bool Open(const char *name);
    virtual int EncodeFrames(const BYTE * src, unsigned & srcLen, BYTE * dst, unsigned & dstLen, unsigned int & flags) = 0;
    void SetMaxKeyFramePeriod (unsigned period);
    void SetTargetBitrate (unsigned rate);
    void SetFrameWidth (unsigned width);
    void SetFrameHeight (unsigned height);
    void SetTSTO (unsigned tsto);
    void SetQuality (unsigned quality);
    void SetFrameTime (unsigned frameTime);
    void EnableAnnex (Annex annex);
    void DisableAnnex (Annex annex);
    bool OpenCodec();
    void CloseCodec();

    void Lock();
    void Unlock();

    virtual void SetMaxRTPFrameSize (unsigned size) = 0;

    int m_targetBitRate;

  protected:
    virtual bool InitContext() = 0;

    unsigned char * _inputFrameBuffer;
    unsigned char * _memInputFrameBuffer;
    AVCodec        *_codec;
    AVCodecContext *_context;
    AVFrame        *_inputFrame;

    int _frameCount;
    int _width, _height;
    CriticalSection _mutex;
    const char * prefix;
#if TRACE_FILE
    Tracer tracer;
#endif
};

////////////////////////////////////////////////////////////////////////////

class H263_RFC2190_EncoderContext : public H263_Base_EncoderContext
{
  public:
    H263_RFC2190_EncoderContext();
    ~H263_RFC2190_EncoderContext();
    bool Open();
    bool InitContext();
    void SetMaxRTPFrameSize (unsigned size);
    int EncodeFrames(const BYTE * src, unsigned & srcLen, BYTE * dst, unsigned & dstLen, unsigned int & flags);
    void RTPCallBack(struct AVCodecContext *avctx, void * _data, int size, int mbCount);
  protected:
    bool Init();
    RFC2190Packetizer packetizer;
    unsigned currentMb;
    unsigned currentBytes;
};

////////////////////////////////////////////////////////////////////////////

class H263_RFC2429_EncoderContext : public H263_Base_EncoderContext
{
  public:
    H263_RFC2429_EncoderContext();
    ~H263_RFC2429_EncoderContext();

    bool Open();
    bool InitContext();
    void SetMaxRTPFrameSize (unsigned size);
    int EncodeFrames(const BYTE * src, unsigned & srcLen, BYTE * dst, unsigned & dstLen, unsigned int & flags);
  protected:
    bool Init();
    H263PFrame * _txH263PFrame;
};

////////////////////////////////////////////////////////////////////////////

class H263_Base_DecoderContext
{
  public:
    H263_Base_DecoderContext(const char * prefix);
    ~H263_Base_DecoderContext();

    virtual bool DecodeFrames(const BYTE * src, unsigned & srcLen, BYTE * dst, unsigned & dstLen, unsigned int & flags) = 0;

  protected:
    bool OpenCodec();
    void CloseCodec();

    AVCodec        *_codec;
    AVCodecContext *_context;
    AVFrame        *_outputFrame;
    AVPacket       _pkt;

    int _frameCount;
    bool freezeVideo;
    int _lastSQN;
    int _lostFrameCounter;
    
    CriticalSection _mutex;
    const char * prefix;
#if TRACE_FILE
    Tracer tracer;
#endif
};

////////////////////////////////////////////////////////////////////////////

class H263_RFC2190_DecoderContext : public H263_Base_DecoderContext
{
  public:
    H263_RFC2190_DecoderContext();
    ~H263_RFC2190_DecoderContext();
    bool DecodeFrames(const BYTE * src, unsigned & srcLen, BYTE * dst, unsigned & dstLen, unsigned int & flags);

  protected:
    RFC2190Depacketizer depacketizer;
};

////////////////////////////////////////////////////////////////////////////

class H263_RFC2429_DecoderContext : public H263_Base_DecoderContext
{
  public:
     H263_RFC2429_DecoderContext();
     ~H263_RFC2429_DecoderContext();

     bool DecodeFrames(const BYTE * src, unsigned & srcLen, BYTE * dst, unsigned & dstLen, unsigned int & flags);
  protected:
    unsigned int _skippedFrameCounter;
    bool _gotIFrame;
    bool _gotAGoodFrame;
    H263PFrame* _rxH263PFrame;
};

////////////////////////////////////////////////////////////////////////////

static int valid_for_protocol    ( const struct PluginCodec_Definition *, void *, const char *,
                                   void * parm, unsigned * parmLen);
static int get_codec_options     ( const struct PluginCodec_Definition * codec, void *, const char *, 
                                   void * parm, unsigned * parmLen);
static int free_codec_options    ( const struct PluginCodec_Definition *, void *, const char *, 
                                   void * parm, unsigned * parmLen);

static void * create_encoder     ( const struct PluginCodec_Definition *);
static void destroy_encoder      ( const struct PluginCodec_Definition *, void * _context);
static int codec_encoder         ( const struct PluginCodec_Definition *, void * _context,
                                   const void * from, unsigned * fromLen,
                                   void * to, unsigned * toLen,
                                   unsigned int * flag);
static int to_normalised_options ( const struct PluginCodec_Definition *, void *, const char *,
                                   void * parm, unsigned * parmLen);
static int to_customised_options ( const struct PluginCodec_Definition *, void *, const char *, 
                                   void * parm, unsigned * parmLen);
static int encoder_set_options   ( const struct PluginCodec_Definition *, void * _context, const char *, 
                                   void * parm, unsigned * parmLen);
static int encoder_get_output_data_size ( const PluginCodec_Definition *, void *, const char *,
                                   void *, unsigned *);

static void * create_decoder     ( const struct PluginCodec_Definition *);
static void destroy_decoder      ( const struct PluginCodec_Definition *, void * _context);
static int codec_decoder         ( const struct PluginCodec_Definition *, void * _context, 
                                   const void * from, unsigned * fromLen,
                                   void * to, unsigned * toLen,
                                   unsigned int * flag);
static int decoder_get_output_data_size ( const PluginCodec_Definition * codec, void *, const char *,
                                   void *, unsigned *);
/////////////////////////////////////////////////////////////////////////////

#endif /* __H263P_1998_H__ */
