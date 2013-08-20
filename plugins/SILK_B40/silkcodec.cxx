/****************************************************************************
* Fixed Point SILK SDK 1.0.2 beta source code package
* Copyright 2010 (c), Skype Limited
* https://developer.skype.com/silk/
* Author of wrapper: Ravi Kalluri
 ****************************************************************************/
#define _CRT_NONSTDC_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER) 
 #define STRCMPI  _strcmpi
 #include "opal\critsect.h"
#else
 #define STRCMPI  strcasecmp
 #include "opal/critsect.h"
 #include <unistd.h>
#endif

#ifndef PLUGIN_CODEC_DLL_EXPORTS
#include "opal/plugin-config.h"
#endif

#include "opal/opalplugin.h"


#include "silkSrc/SKP_Silk_SDK_API.h"


static struct PluginCodec_information licenseInfo =
{
    1273081733,           // version timestamp = Wed May  5 10:49:24 PDT 2010

    "Skype Limited",                            // source code author
    "1.0.8",                                    // source code version
    NULL,                                       // source code email
    "https://developer.skype.com/silk/",        // source code URL
    "Copyright 2010 (c), Skype Limited",        // source code copyright
    "None",                                     // source code license
    PluginCodec_License_None,                   // source code license

    "SILK",                                     // codec description
    "Skype Limited",                            // codec author
    "1.0.8",				                    // codec version
    NULL,                                       // codec email
    "https://developer.skype.com/silk/",        // codec URL
    "Copyright 2010 (c), Skype Limited",        // codec copyright information
    "ITU-T General Public License (G.191)",     // codec license
    PluginCodec_License_NoRoyalties             // codec license code
};

/////////////////////////////////////////////////////////////////////////////

#include "silkSrc/SKP_Silk_SDK_API.h"
#include "silkSrc/SKP_Silk_control.h"
#include "silkSrc/SKP_Silk_typedef.h" 
#include "silkSrc/SKP_Silk_errors.h"

// 20 ms frame
#define SILK_FRAME_SAMPLES_8K   160
#define SILK_FRAME_SAMPLES_12K  240
#define SILK_FRAME_SAMPLES_16K  320
#define SILK_FRAME_SAMPLES_24K  480

// required bandwidth options in bits per second
#define SILK_8K_SAMP_RATE       8000  
#define SILK_12K_SAMP_RATE     12000  
#define SILK_16K_SAMP_RATE     16000  
#define SILK_24K_SAMP_RATE     24000  

#define SILK_8K_MAX_BIT_RATE   20000  
#define SILK_12K_MAX_BIT_RATE  25000  
#define SILK_16K_MAX_BIT_RATE  30000  
#define SILK_24K_MAX_BIT_RATE  40000  


/////////////////////////////////////////////////////////////////////////////
// Compress audio for transport


struct SILKEncoderControl
{
    SKP_SILK_SDK_EncControlStruct  control;
    void *                         context;
    CriticalSection *mutex;
};

static void * SILKEncoderCreate (const struct PluginCodec_Definition * codec)
{
    
    SKP_int ret;
    SKP_int32 encSizeBytes;
    
    struct SILKEncoderControl * ctxt;
   
    /* Create Encoder */
    ret = SKP_Silk_SDK_Get_Encoder_Size( &encSizeBytes );
    if( ret ) {
        printf( "SKP_Silk_create_encoder returned %d", ret );
    }
    
    ctxt = (struct SILKEncoderControl*)malloc(1*(sizeof(struct SILKEncoderControl)+encSizeBytes));
    ctxt->context = malloc( encSizeBytes );
    ctxt->mutex = new CriticalSection();
    
   // printf("\nbitrate in SILKEncoderCreate %d\n", codec->bitsPerSec);
    
   // printf( "\nSilky Encoder created");

    ret = SKP_Silk_SDK_InitEncoder(ctxt->context, &ctxt->control );

    if( ret ) {
        printf( "\nSKP_Silk_reset_encoder returned %d", ret );
    }  
    else {
        ctxt->control.API_sampleRate       = codec->sampleRate;
        ctxt->control.packetSize           = codec->parm.audio.samplesPerFrame;
	ctxt->control.maxInternalSampleRate = codec->sampleRate;
        ctxt->control.packetLossPercentage = 0;
        ctxt->control.useInBandFEC         = 0;
        ctxt->control.useDTX               = 0;
        ctxt->control.complexity           = 2;
        ctxt->control.bitRate              = codec->bitsPerSec;
    }
    return ctxt;
}

static void SILKEncoderDestroy (const struct PluginCodec_Definition * codec, void * context)
{
  struct SILKEncoderControl * ctxt =  (struct SILKEncoderControl *)context;
  ctxt->mutex->Wait();
  free (ctxt->context); ctxt->context = NULL;
  ctxt->mutex->Signal();
#if defined(_MSC_VER) 
  Sleep(100);
#else
  usleep(100000); // timeout for transmit thread termination
#endif
  delete(ctxt->mutex);
  free(ctxt);
}


static int SILKEncode (const struct PluginCodec_Definition * codec, 
                                      void * context,
                                      const void * fromPtr, 
                                      unsigned * fromLen,
                                      void * toPtr,         
                                      unsigned * toLen,
                                      unsigned int * flag)
{

    SKP_int ret;
    struct SILKEncoderControl * ctxt =  (struct SILKEncoderControl *)context;
    WaitAndSignal m(*ctxt->mutex);

    if(!ctxt->context) return 0;
 
    /* Silk Encoder */
    ret = SKP_Silk_SDK_Encode( ctxt->context, &ctxt->control, (const short *)fromPtr, (SKP_int16)*fromLen/2, (unsigned char *)toPtr, (short *)toLen);
    if( ret ) {
        printf( "\nSKP_Silk_Encode returned %i", ret );
    }
   
    return 1;
}

/////////////////////////////////////////////////////////////////////////////
// Decoder


struct SILKDecoderControl
{
    SKP_SILK_SDK_DecControlStruct  control;
    void *                         context;
    CriticalSection *mutex;
};

                                                                    
static void * SILKDecoderCreate (const struct PluginCodec_Definition * codec)
{
    SKP_int16 ret;
    SKP_int32 decSizeBytes;

    struct SILKDecoderControl * ctxt;
    
    /* Create decoder */
    ret = SKP_Silk_SDK_Get_Decoder_Size( &decSizeBytes );
    if( ret ) {
        printf( "\nSKP_Silk_SDK_Get_Decoder_Size returned %d", ret );
    }


    ctxt = (struct SILKDecoderControl*)malloc(1*(sizeof(struct SILKDecoderControl)+decSizeBytes));
    ctxt->context = malloc( decSizeBytes );
    ctxt->mutex = new CriticalSection();

    ctxt->control.framesPerPacket = codec->parm.audio.maxFramesPerPacket;
    ctxt->control.API_sampleRate = codec->sampleRate;
    ctxt->control.frameSize = codec->parm.audio.samplesPerFrame;
    ctxt->control.inBandFECOffset = 0;
    ctxt->control.moreInternalDecoderFrames = 0;

    ret = SKP_Silk_SDK_InitDecoder( ctxt->context );
    
    if( ret ) {
        printf( "\nSKP_Silk_InitDecoder returned %d", ret );
    }

    return ctxt;
}

static void SILKDecoderDestroy (const struct PluginCodec_Definition * codec, void * context)
{
 struct SILKDecoderControl * ctxt =  (struct SILKDecoderControl *)context;
 ctxt->mutex->Wait();
 free (ctxt->context); ctxt->context = NULL;
 ctxt->mutex->Signal();
#if defined(_MSC_VER) 
 Sleep(100);
#else
 usleep(100000); // timeout for receive thread termination
#endif
 delete(ctxt->mutex);
 free(ctxt);
}

static int SILKDecode (const struct PluginCodec_Definition * codec, 
                                                       void * context,
                                                 const void * fromPtr, 
                                                   unsigned * fromLen,
                                                       void * toPtr,         
                                                   unsigned * toLen,
                                               unsigned int * flag)
{
    SKP_int16 ret=0;
    SKP_int16 len, tot_len;
    SKP_int16 *outPtr;
    struct SILKDecoderControl * ctxt =  (struct SILKDecoderControl *)context;
    WaitAndSignal m(*ctxt->mutex);

	if(!ctxt->context) return 0;

    SKP_int   frames;
    outPtr = (short *)toPtr;
         
    frames = 0;
    tot_len = 0;
    len = 0; 
        
    do {
        ret = SKP_Silk_SDK_Decode( ctxt->context, &ctxt->control, 0, (const unsigned char *)fromPtr, (SKP_uint8)*fromLen, (short *)toPtr, &len );       
        if( ret ) {
           printf( "\nSKP_Silk_SDK_Decode returned %d", ret );
        }
        tot_len = len;
        
    } while( ctxt->control.moreInternalDecoderFrames );

    *toLen = tot_len*2;
    
    return 1;
}

/////////////////////////////////////////////////////////////////////////////
// bitrate is a required SDP parameter in RFC 3047/5577
static const char BitRateOptionName[] = "BitRate";

static struct PluginCodec_Option BitRateOption =
{
    PluginCodec_IntegerOption,  // PluginCodec_OptionTypes
    BitRateOptionName,          // Generic (human readable) option name
    1,                          // Read Only flag
    PluginCodec_EqualMerge,     // Merge mode
    "24000",                    // Initial value
    "bitrate",                  // SIP/SDP FMTP name
    "0",                        // SIP/SDP FMTP default value (option not included in FMTP if have this value)
    0,                          // H.245 Generic Capability number and scope bits
    "12000",                    // Minimum value (enum values separated by ':')
    "40000"                     // Maximum value
};

static struct PluginCodec_Option * const SILK_OptionTable[] =
{
    &BitRateOption,
    NULL
};

static int get_codec_options (const struct PluginCodec_Definition * defn,
                                                             void * context, 
                                                       const char * name,
                                                             void * parm,
                                                         unsigned * parmLen)
{
    if (context == NULL)
        return 0;
/*
    if (defn->sampleRate == SILK_8K_SAMP_RATE)  *(struct PluginCodec_Option const * const * *)parm = OptionTable8k;
    else if (defn->sampleRate == SILK_12K_SAMP_RATE)  *(struct PluginCodec_Option const * const * *)parm = OptionTable12k;
    else if (defn->sampleRate == SILK_16K_SAMP_RATE)  *(struct PluginCodec_Option const * const * *)parm = OptionTable16k;
    else *(struct PluginCodec_Option const * const * *)parm = OptionTable24k; */
    *parmLen = 0;
    return 1;
}


static int encoder_set_options (const struct PluginCodec_Definition * defn,
                                                             void * context, 
                                                       const char * name,
                                                             void * parm,
                                                         unsigned * parmLen)
{
  int i;
  struct SILKEncoderControl * ctxt =  (struct SILKEncoderControl *)context;
  
/*
  char ** option = (char **)parm;
    for (i = 0; option[i] != NULL; i += 2) {
      if (ctxt && STRCMPI(option[0], PLUGINCODEC_OPTION_MAX_BIT_RATE) == 0)
        ctxt->control.bitRate = atoi(option[1]);
      if (ctxt && STRCMPI(option[0], PLUGINCODEC_OPTION_MAX_FRAME_SIZE) == 0)
        ctxt->control.sampleRate = atoi(option[1]); 
    }
*/
  return 1;
}
/*
static struct PluginCodec_ControlDefn SILKEncodeControls[] =
{
  { PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS, get_codec_options },
  { PLUGINCODEC_CONTROL_SET_CODEC_OPTIONS, encoder_set_options },
  { NULL }
};

static struct PluginCodec_ControlDefn SILKDecodeControls[] =
{
  { PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS, get_codec_options },
  { NULL }
};
*/
#define OpalPluginCodec_Identifer_SILK_Id16K "1.3.6.1.4.1.17091.1.3"
#define OpalPluginCodec_Identifer_SILK_Id24K "1.3.6.1.4.1.17091.1.3.24"

#define OpalPluginCodec_Identifer_SILK_24K   OpalPluginCodec_Identifer_SILK_Id24K
#define SILK_24K_Desc                        "SILK_B40_24K"
#define SILK_24K_MediaFmt                    "SILK_B40_24K"
#define SILK_24K_PayloadName                 "SILK"
static unsigned int SILK_24K_ClockRate       =24000;
static unsigned int SILK_24K_MaxBitRate      =SILK_24K_MAX_BIT_RATE;
static unsigned int SILK_24K_Samples         =SILK_FRAME_SAMPLES_24K;
static unsigned int SILK_24K_BytesPerFrame   =SILK_24K_MAX_BIT_RATE/200;
static unsigned int SILK_24K_PayloadCode     =100;
static unsigned int SILK_24K_FRAMES          =1;
static unsigned int SILK_24K_SUPPORTMODE     =240;  // 8/12/16/24 k

#define OpalPluginCodec_Identifer_SILK_16K   OpalPluginCodec_Identifer_SILK_Id16K
#define SILK_16K_Desc                        "SILK_B40"
#define SILK_16K_MediaFmt                    "SILK_B40"
#define SILK_16K_PayloadName                 "SILK"
static unsigned int SILK_16K_ClockRate       =16000;
static unsigned int SILK_16K_MaxBitRate      =SILK_24K_MAX_BIT_RATE;
static unsigned int SILK_16K_Samples         =SILK_FRAME_SAMPLES_16K;
static unsigned int SILK_16K_BytesPerFrame   =SILK_24K_MAX_BIT_RATE/200;
static unsigned int SILK_16K_PayloadCode     =101;
static unsigned int SILK_16K_FRAMES          =1;
static unsigned int SILK_16K_SUPPORTMODE     =120;  // 8/12/16/24 k

#define SILKPLUGIN_CODEC(prefix) \
static const struct PluginCodec_H323GenericParameterDefinition prefix##_h323params[] = \
{ \
   {{1,0,0,0,0},1, PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsignedMin, {prefix##_FRAMES}}, \
   {{1,0,0,0,0},2, PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_booleanArray, {prefix##_SUPPORTMODE}}, \
    NULL \
}; \
static struct PluginCodec_H323GenericCodecData prefix##_Cap = \
{ \
    OpalPluginCodec_Identifer_##prefix, \
    prefix##_MaxBitRate, \
    2, \
    prefix##_h323params \
}; 

SILKPLUGIN_CODEC(SILK_16K);
SILKPLUGIN_CODEC(SILK_24K);


#define DECLARE_H323PARAM(prefix) \
{ \
  /* encoder */ \
  PLUGIN_CODEC_VERSION_OPTIONS,	      /* codec API version */ \
  &licenseInfo,                       /* license information */ \
  PluginCodec_MediaTypeAudio |        /*  audio codec */ \
  PluginCodec_InputTypeRaw |          /* raw input data */ \
  PluginCodec_OutputTypeRaw |         /* raw output data */ \
  PluginCodec_RTPTypeExplicit |       /*  specified RTP type */ \
  PluginCodec_RTPTypeShared,          /* RTP type shared */ \
  prefix##_Desc,                      /* text decription */ \
  "L16",                              /* source format */ \
  prefix##_MediaFmt,                  /* destination format */ \
  SILK_OptionTable,			          /* user data */ \
  prefix##_ClockRate,                 /* samples per second */ \
  prefix##_MaxBitRate,				  /* raw bits per second */ \
  20000,                              /* nanoseconds per frame */ \
  {{ \
     prefix##_Samples,                /* samples per frame */ \
     prefix##_BytesPerFrame,		  /* bytes per frame */ \
     1,                               /* recommended number of frames per packet */ \
     1,                               /* maximum number of frames per packet  */ \
  }}, \
  prefix##_PayloadCode,               /* IANA RTP payload code */ \
  prefix##_PayloadName,               /* RTP payload name */ \
  SILKEncoderCreate,                  /* create codec function */ \
  SILKEncoderDestroy,                 /* destroy codec */ \
  SILKEncode,                         /* encode/decode */ \
  NULL,                 /* codec controls */ \
  PluginCodec_H323Codec_generic,      /* h323CapabilityType */ \
  (struct PluginCodec_H323GenericCodecData *)&prefix##_Cap /* h323CapabilityData */ \
}, \
{  \
  /* decoder */ \
  PLUGIN_CODEC_VERSION_OPTIONS,	      /* codec API version */ \
  &licenseInfo,                       /* license information */ \
  PluginCodec_MediaTypeAudio |        /*  audio codec */ \
  PluginCodec_InputTypeRaw |          /* raw input data */ \
  PluginCodec_OutputTypeRaw |         /* raw output data */ \
  PluginCodec_RTPTypeExplicit |       /*  specified RTP type */ \
  PluginCodec_RTPTypeShared,          /* RTP type shared */ \
  prefix##_Desc,                      /* text decription */ \
  prefix##_MediaFmt,                  /* source format */ \
  "L16",                              /* destination format */ \
  SILK_OptionTable,			          /* user data */ \
  prefix##_ClockRate,                 /* samples per second */ \
  prefix##_MaxBitRate,				  /* raw bits per second */ \
  20000,                              /* nanoseconds per frame */ \
  {{ \
     prefix##_Samples,                /* samples per frame */ \
     prefix##_BytesPerFrame,		  /* bytes per frame */ \
     1,                               /* recommended number of frames per packet */ \
     1,                               /* maximum number of frames per packet  */ \
  }}, \
  prefix##_PayloadCode,               /* IANA RTP payload code */ \
  prefix##_PayloadName,               /* RTP payload name */ \
  SILKDecoderCreate,                  /* create codec function */ \
  SILKDecoderDestroy,                 /* destroy codec */ \
  SILKDecode,                         /* encode/decode */ \
  NULL,                 /* decode controls */ \
  PluginCodec_H323Codec_generic,      /* h323CapabilityType */ \
  (struct PluginCodec_H323GenericCodecData *)&prefix##_Cap /* h323CapabilityData */ \
} \


static struct PluginCodec_Definition SILKCodecDefn[] =
{
    DECLARE_H323PARAM(SILK_16K)
    ,DECLARE_H323PARAM(SILK_24K)
};

extern "C" {
PLUGIN_CODEC_IMPLEMENT_ALL(SILK_B40, SILKCodecDefn, PLUGIN_CODEC_VERSION_OPTIONS)
}
