/* 
 *
 * Easy codecs for OpenH323/OPAL
 *
 * Copyright (c) 2004 ISVO (Asia) Pte Ltd. All Rights Reserved.
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
 * Portions of this Code as Copyright Imtelephone.com. All rights reserved.
 *
 * The Original Code is derived from and used in conjunction with the 
 * OpenH323/OPAL Project (www.openh323.org/)
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte Ltd.
 *
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision: 22675 $
 * $Author: rjongbloed $
 * $Date: 2009-05-20 04:23:29 +0000 (Wed, 20 May 2009) $
 */

#include <codec/opalplugin.h>

#if _MSC_VER > 1000
#pragma once
#endif 

// All of PWLib/OpenH323 use MSVCRT.LIB or MSVCRTD.LIB, but EasyCodecs use
// libc.lib, so we need to tell the linker to ignore it, can't have two
// Run Time libraries!
#pragma comment(linker, "/NODEFAULTLIB:libc.lib")

////////////////////////////////////////////////////////////////////////////
// Definitions.

enum {
	Plugin_EasyG722		=1,
	Plugin_EasyG729A	=2,
	Plugin_EasyG728		=3,
	Plugin_EasyG7231_63	=4
  };

////////////////////////////////////////////////////////////////////////////
// Information

static struct PluginCodec_information licenseInfo = {
  1192510548,                              // timestamp = Tue 16 Oct 2007 04:55:48 AM UTC 

  "Simon Horne	ISVO(Asia) Pte Ltd",                           // source code author
  "1.0",                                                       // source code version
  "shorne@isvo.net",		                                   // source code email
  "http://www.isvo.net",									   // source code URL
  "Copyright (C) 2005 by ISVO (Asia), All Rights Reserved",	   // source code copyright
  "MPL 1.0",                                                   // source code license
  PluginCodec_License_MPL,                                     // source code license

  "Easy Codecs",                                               // codec description
  "imtelephone.com",										   // codec author
  NULL,                                                        // codec version
  NULL,                                                        // codec email
  "http://www.imtelephone.com",                                // codec URL
  "Copyright (C) 2003-2004 imtelephone.com",					   // codec copyright information
  "NULL",                                                      // codec license
  PluginCodec_License_ResearchAndDevelopmentUseOnly            // codec license code
};


// Codec Information Template

#define DECLARE_EASY_PARAM(prefix) \
{ \
  /* encoder */ \
  PLUGIN_CODEC_VERSION_WIDEBAND,	  /* codec API version */ \
  &licenseInfo,                       /* license information */ \
  PluginCodec_MediaTypeAudio |        /* audio codec */ \
  PluginCodec_InputTypeRaw |          /* raw input data */ \
  PluginCodec_OutputTypeRaw |         /* raw output data */ \
  PluginCodec_RTPTypeExplicit,        /* dynamic RTP type */ \
  prefix##_Desc,                      /* text decription */ \
  prefix##_L16Desc,                   /* source format */ \
  prefix##_MediaFmt,                  /* destination format */ \
  (void*)Plugin_##prefix,			  /* user data */ \
  prefix##_SamplePerSec,              /* samples per second */ \
  prefix##_BitsPerSec,				  /* raw bits per second */ \
  prefix##_NsPerFrame,                /* nanoseconds per frame */ \
  prefix##_SamplesPerFrame,           /* samples per frame */ \
  prefix##_BytesPerFrame,			  /* bytes per frame */ \
  prefix##_RecFramesPerPacket,        /* recommended number of frames per packet */ \
  prefix##_MaxFramesPerPacket,        /* maximum number of frames per packet  */ \
  prefix##_IANACode,                  /* IANA RTP payload code */ \
  prefix##_IANAName,                  /* RTP payload name */ \
  create_encoder,                     /* create codec function */ \
  destroy_encoder,                    /* destroy codec */ \
  codec_encoder,                      /* encode/decode */ \
  NULL,                               /* codec controls */ \
  prefix##_CapType,					  /* h323CapabilityType */ \
  NULL								  /* h323CapabilityData */ \
}, \
{  \
  /* decoder */ \
  PLUGIN_CODEC_VERSION_WIDEBAND,	  /* codec API version */ \
  &licenseInfo,                       /* license information */ \
  PluginCodec_MediaTypeAudio |        /* audio codec */ \
  PluginCodec_InputTypeRaw |          /* raw input data */ \
  PluginCodec_OutputTypeRaw |         /* raw output data */ \
  PluginCodec_RTPTypeExplicit,         /* dynamic RTP type */ \
  prefix##_Desc,                      /* text decription */ \
  prefix##_MediaFmt,				  /* source format */ \
  prefix##_L16Desc,                   /* destination format */ \
  (void*)Plugin_##prefix,			  /* user data */ \
  prefix##_SamplePerSec,              /* samples per second */ \
  prefix##_BitsPerSec,				  /* raw bits per second */ \
  prefix##_NsPerFrame,                /* nanoseconds per frame */ \
  prefix##_SamplesPerFrame,           /* samples per frame */ \
  prefix##_BytesPerFrame,			  /* bytes per frame */ \
  prefix##_RecFramesPerPacket,        /* recommended number of frames per packet */ \
  prefix##_MaxFramesPerPacket,        /* maximum number of frames per packet  */ \
  prefix##_IANACode,                  /* IANA RTP payload code */ \
  prefix##_IANAName,                  /* RTP payload name */ \
  create_decoder,                     /* create codec function */ \
  destroy_decoder,                    /* destroy codec */ \
  codec_decoder,                      /* encode/decode */ \
  NULL,                               /* codec controls */ \
  prefix##_CapType,					  /* h323CapabilityType */ \
  NULL								  /* h323CapabilityData */ \
} \


////////////////////////////////////////////////////////////////////////////////////////////
// Base Codec class to build Single instances of the Easy Libraries 

class EasyCodec 
{ 
  public:
  /// Contructor/Deconstructor
	EasyCodec() 
	{ 
	};

	~EasyCodec() 
	{ 
	};
 
 /// Single Instance Plugin Calls (not threadsafe)
	unsigned long init_enc() 
	{	
			unsigned long ret = init_encoder();
		return ret;
	};
	bool enc(unsigned long hEncoder, short *speech, unsigned char *bitstream) 
	{	
			bool ret = encoder(hEncoder, speech, bitstream);		
		return ret;
	};
	bool release_enc(unsigned long hEncoder) 
	{	
			bool ret = release_encoder(hEncoder);
		return ret;
	};
	unsigned long init_dec() 
	{	
			unsigned long ret = init_decoder();
		return ret;
	};
	bool dec(unsigned long hDecoder, unsigned char *bitstream, short *synth_short) 
	{	
			bool ret = decoder(hDecoder, bitstream, synth_short); 
		return ret;
	}; 
	bool release_dec(unsigned long hDecoder) 
	{	
			bool ret = release_decoder(hDecoder); 	
		return ret;
	}; 

 /// Thread Safe Single Instance Codec calls
	virtual unsigned long init_encoder() =0;
	virtual bool encoder(unsigned long hEncoder, short *speech, unsigned char *bitstream) =0;
	virtual bool release_encoder(unsigned long hEncoder) =0;
	virtual unsigned long init_decoder() =0;
	virtual bool decoder(unsigned long hDecoder, unsigned char *bitstream, short *synth_short) =0; 
	virtual bool release_decoder(unsigned long hDecoder) =0; 

}; 


// Session information to pass back with coder handle and reference Easy Library instance
struct EasySession
{
	EasyCodec * easy;
	unsigned long hEcoder;
	unsigned long hDcoder;
};




