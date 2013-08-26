typedef unsigned char BYTE;
#include <iostream>
using namespace std;

#include "opal/opalplugin.h"
#include "opal/ffmpeg.h"
#include "opal/critsect.h"

static const char		G7231_Desc[] = { "G7231-6.3" };	// text decription  
static const char		G7231_L16Desc[]= { "L16" };			// source format 
static const char		G7231_MediaFmt[] = { "G.7231-6.3k[e]" };	// destination format  
static unsigned int		G7231_SamplePerSec = 8000;			// samples per second  
static unsigned int		G7231_BitsPerSec = 6300;			// raw bits per second  
static unsigned int		G7231_NsPerFrame = 30000;			// nanoseconds per frame
static unsigned int		G7231_SamplesPerFrame = 240;			// samples per frame 
static unsigned int		G7231_BytesPerFrame = 24;			// Bytes per frame
static unsigned int		G7231_RecFramesPerPacket = 1;        //rec number of frames per packet 
static unsigned int		G7231_MaxFramesPerPacket = 1;       // max number of frames per packet
#define	G7231_IANACode  4	// IANA RTP payload code  
static const char G7231_IANAName[] = { "G723" }; // RTP payload name 
static unsigned char G7231_CapType = PluginCodec_H323AudioCodec_g7231; // h323CapabilityType 


static struct PluginCodec_information licenseInfo = {
  1192510548,                                         // timestamp = Tue 16 Oct 2007 04:55:48 AM UTC
  "Varnavskiy Andrey",                                // source code author
  "1.0",                                              // source code version
  "nuggot@mail.ru",                                   // source code email
  "http://openmcu.ru",                                // source code URL
  "Copyright (C) 2012 Varnavskiy Andrey",             // source code copyright
  "MPL 1.0",                                          // source code license
  PluginCodec_License_MPL,                            // source code license

  "G7231 ffmpeg codec",                               // codec description
  "ffmpeg.org",                                       // codec author
  NULL,                                               // codec version
  NULL,                                               // codec email
  "http://ffmpeg.org",                                // codec URL
  NULL,                                               // codec copyright information
  "NULL",                                             // codec license
  PluginCodec_License_LGPL                            // codec license code
};

#define DECLARE_G7231_PARAM(prefix) \
{ \
  /* encoder */ \
  PLUGIN_CODEC_VERSION_WIDEBAND,      /* codec API version */ \
  &licenseInfo,                       /* license information */ \
  PluginCodec_MediaTypeAudio |        /* audio codec */ \
  PluginCodec_InputTypeRaw |          /* raw input data */ \
  PluginCodec_OutputTypeRaw |         /* raw output data */ \
  PluginCodec_RTPTypeExplicit,        /* dynamic RTP type */ \
  prefix##_Desc,                      /* text decription */ \
  prefix##_L16Desc,                   /* source format */ \
  prefix##_MediaFmt,                  /* destination format */ \
  NULL,             /* user data */ \
  prefix##_SamplePerSec,              /* samples per second */ \
  prefix##_BitsPerSec,                /* raw bits per second */ \
  prefix##_NsPerFrame,                /* nanoseconds per frame */ \
  prefix##_SamplesPerFrame,           /* samples per frame */ \
  prefix##_BytesPerFrame,             /* bytes per frame */ \
  prefix##_RecFramesPerPacket,        /* recommended number of frames per packet */ \
  prefix##_MaxFramesPerPacket,        /* maximum number of frames per packet  */ \
  prefix##_IANACode,                  /* IANA RTP payload code */ \
  prefix##_IANAName,                  /* RTP payload name */ \
  create_encoder,                     /* create codec function */ \
  destroy_encoder,                    /* destroy codec */ \
  codec_encoder,                      /* encode/decode */ \
  NULL,                               /* codec controls */ \
  prefix##_CapType,                   /* h323CapabilityType */ \
  NULL                                /* h323CapabilityData */ \
}, \
{  \
  /* decoder */ \
  PLUGIN_CODEC_VERSION_WIDEBAND,      /* codec API version */ \
  &licenseInfo,                       /* license information */ \
  PluginCodec_MediaTypeAudio |        /* audio codec */ \
  PluginCodec_InputTypeRaw |          /* raw input data */ \
  PluginCodec_OutputTypeRaw |         /* raw output data */ \
  PluginCodec_RTPTypeExplicit,        /* dynamic RTP type */ \
  prefix##_Desc,                      /* text decription */ \
  prefix##_MediaFmt,                  /* source format */ \
  prefix##_L16Desc,                   /* destination format */ \
  NULL,             /* user data */ \
  prefix##_SamplePerSec,              /* samples per second */ \
  prefix##_BitsPerSec,                /* raw bits per second */ \
  prefix##_NsPerFrame,                /* nanoseconds per frame */ \
  prefix##_SamplesPerFrame,           /* samples per frame */ \
  prefix##_BytesPerFrame,             /* bytes per frame */ \
  prefix##_RecFramesPerPacket,        /* recommended number of frames per packet */ \
  prefix##_MaxFramesPerPacket,        /* maximum number of frames per packet  */ \
  prefix##_IANACode,                  /* IANA RTP payload code */ \
  prefix##_IANAName,                  /* RTP payload name */ \
  create_decoder,                     /* create codec function */ \
  destroy_decoder,                    /* destroy codec */ \
  codec_decoder,                      /* encode/decode */ \
  NULL,                               /* codec controls */ \
  prefix##_CapType,                   /* h323CapabilityType */ \
  NULL                                /* h323CapabilityData */ \
} \


class G7231_EncoderContext
{
  public:
    G7231_EncoderContext();
    virtual ~G7231_EncoderContext();

    int EncodeFrames(const BYTE * src, BYTE * dst);
    bool OpenCodec();
    void CloseCodec();

  protected:
    AVCodec        *_codec;
    AVCodecContext *_context;
    AVFrame        *_inputFrame;
    AVPacket       _pkt;
    AVDictionary   *_dct;
    CriticalSection _mutex;

    int _frameCount;
};

class G7231_DecoderContext
{
  public:
    G7231_DecoderContext();
    virtual ~G7231_DecoderContext();

    int DecodeFrames(const BYTE * src, BYTE * dst);
    bool OpenCodec();
    void CloseCodec();

  protected:
    AVCodec        *_codec;
    AVCodecContext *_context;
    AVFrame        *_outputFrame;
    AVPacket       _pkt;
    AVDictionary   *_dct;
    CriticalSection _mutex;

    int _frameCount;
};