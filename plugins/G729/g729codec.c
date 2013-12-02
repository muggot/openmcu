/*
 * ITU Reference G.729AB codec for OpenH323/OPAL
 *
 * Copyright (C) 2004 Nickolay V. Shmyrev, All Rights Reserved
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
 * The Initial Developer of the Original Code is Nickolay V. Shmyrev
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: g729codec.c,v $
 *
 * Revision 1.1  2004/05/02 08:59:56  nshmyrev
 * Added ITU Reference G.729 codec plug in.
 *
 */

#include <opalplugin.h>

PLUGIN_CODEC_IMPLEMENT("ITU-G729")

#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"
#include "tab_ld8a.h"
#include "util.h"
#include "pre_proc.h"

#define SAMPLES_PER_FRAME   L_FRAME
#define BYTES_PER_FRAME     10
#define NANOSECONDSPERFRAME 10000
#define BITS_PER_SECOND     8000

/////////////////////////////////////////////////////////////////////////////

static void *create_encoder(const struct PluginCodec_Definition *codec)
{
	CodState *coder;

	coder = Init_Coder_ld8a();
	Init_Pre_Process(coder);

	return (void *)coder;
}

static int codec_encoder(const struct PluginCodec_Definition *codec,
			 void *context,
			 const void *from,
			 unsigned *fromLen,
			 void *to, unsigned *toLen, unsigned int *flag)
{
	Word16 parm[PRM_SIZE];
	CodState *coder = (CodState *)context;

	if (*fromLen < SAMPLES_PER_FRAME * 2 || *toLen < BYTES_PER_FRAME)
		return 0;

	Copy ((Word16 *)from, coder->new_speech, SAMPLES_PER_FRAME);
	Pre_Process(coder, coder->new_speech, L_FRAME);
	Coder_ld8a(coder, parm);
	Store_Params(parm, to);

	*fromLen = SAMPLES_PER_FRAME * 2;
	*toLen = BYTES_PER_FRAME;

	return 1;
}

static void destroy_encoder(const struct PluginCodec_Definition *codec,
			    void *context)
{
        free (context);
	return;
}

static void *create_decoder(const struct PluginCodec_Definition *codec)
{
	DecState *decoder;	

	decoder = Init_Decod_ld8a();
	Init_Post_Filter(decoder);
	Init_Post_Process(decoder);
	
	return (void *)decoder;
}

static int codec_decoder(const struct PluginCodec_Definition *codec,
			 void *context,
			 const void *from,
			 unsigned *fromLen,
			 void *to, unsigned *toLen, unsigned int *flag)
{
	Word16 i;
	Word16 *synth;
	Word16 parm[PRM_SIZE + 1];

	DecState *decoder = (DecState *) context;

	if (*fromLen < BYTES_PER_FRAME || *toLen < SAMPLES_PER_FRAME * 2)
		return 0;

	Restore_Params(from, &parm[1]);

	synth = decoder->synth_buf + M;

	parm[0] = 1;
	for (i = 0; i < PRM_SIZE; i++) {
		if (parm[i + 1] != 0) {
			parm[0] = 0;
			break;
		}
	}

	parm[4] = Check_Parity_Pitch(parm[3], parm[4]);	

	Decod_ld8a(decoder, parm, synth, decoder->Az_dec, decoder->T2, &decoder->bad_lsf);
	Post_Filter(decoder, synth, decoder->Az_dec, decoder->T2);
	Post_Process(decoder, synth, L_FRAME);

	for (i = 0; i < SAMPLES_PER_FRAME; i++)
		((Word16 *) to)[i] = synth[i];

	*fromLen = BYTES_PER_FRAME;
	*toLen = SAMPLES_PER_FRAME * 2;

	return 1;
}

static void destroy_decoder(const struct PluginCodec_Definition *codec,
			    void *context)
{
	DecState *decoder = (DecState *) context;
	free (decoder);
	return;
}

/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_information licenseInfo = {
	1073619586,		// timestamp = Fri 09 Jan 2004 03:39:46 AM UTC = 

	"Nickolay V. Shmyrev",	// source code author
	"1.0",			// source code version
	"nshmyrev@yandex.ru",	// source code email
	"http://nshmyrev.narod.ru/g729/g729.html",	// source code URL
	"Copyright (C) 2005 by Nickolay V. Shmyrev, All Rights Reserved",	// source code copyright
	"MPL 1.0",		// source code license
	PluginCodec_License_MPL,	// source code license

	"G.729A",		// codec description
	"ITU",			// codec author
	NULL,			// codec version
	NULL,			// codec email
	"http://www.itu.int",	// codec URL
	"Copyright (C) 1995-2001 ITU Corporation. All Rights Reserved",	// codec copyright information
	"NULL",			// codec license
	PluginCodec_License_ResearchAndDevelopmentUseOnly	// codec license code
};

static const char L16Desc[] = { "L16" };

static const char g729Descr[] = { "ITU G729" };
static const char g729ADescr[] = { "ITU G729A" };

static const char g729MediaFmt[] = { "G.729" };
static const char g729AMediaFmt[] = { "G.729A" };

static const char ianaName[] = { "G729" };
#define IANACODE  18

static struct PluginCodec_Definition g729CodecDefn[4] = {

	{
	 // encoder
	 PLUGIN_CODEC_VERSION,	// codec API version
	 &licenseInfo,		// license information

	 PluginCodec_MediaTypeAudio |	// audio codec
	 PluginCodec_InputTypeRaw |	// raw input data
	 PluginCodec_OutputTypeRaw |	// raw output data
	 PluginCodec_RTPTypeExplicit,	// explicit RTP type

	 g729Descr,		// text decription
	 L16Desc,
	 g729MediaFmt,

	 NULL,			// user data

	 8000,			// samples per second
	 BITS_PER_SECOND,	// raw bits per second
	 NANOSECONDSPERFRAME,	// nanoseconds per frame
	 {{SAMPLES_PER_FRAME,	// samples per frame
	 BYTES_PER_FRAME,	// bytes per frame
	 6,			// recommended number of frames per packet
	 24}},			// maximum number of frames per packet
	 IANACODE,		// IANA RTP payload code
	 ianaName,		// RTP Payload name

	 create_encoder,	// create codec function
	 destroy_encoder,	// destroy codec
	 codec_encoder,		// encode/decode
	 NULL,			// codec controls

	 PluginCodec_H323AudioCodec_g729,	// h323CapabilityType
	 NULL			// h323CapabilityData
	 },

	{
	 // decoder
	 PLUGIN_CODEC_VERSION,	// codec API version
	 &licenseInfo,		// license information

	 PluginCodec_MediaTypeAudio |	// audio codec
	 PluginCodec_InputTypeRaw |	// raw input data
	 PluginCodec_OutputTypeRaw |	// raw output data
	 PluginCodec_RTPTypeExplicit,	// explicit RTP type

	 g729Descr,		// text decription
	 g729MediaFmt,
	 L16Desc,

	 NULL,			// user data

	 8000,			// samples per second
	 BITS_PER_SECOND,	// raw bits per second
	 NANOSECONDSPERFRAME,	// nanoseconds per frame
	 {{SAMPLES_PER_FRAME,	// samples per frame
	 BYTES_PER_FRAME,	// bytes per frame
	 6,			// recommended number of frames per packet
	 24}},			// maximum number of frames per packet
	 IANACODE,		// IANA RTP payload code
	 ianaName,		// RTP Payload name

	 create_decoder,	// create codec function
	 destroy_decoder,	// destroy codec
	 codec_decoder,		// encode/decode
	 NULL,			// codec controls

	 PluginCodec_H323AudioCodec_g729,	// h323CapabilityType 
	 NULL			// h323CapabilityData
	 },

	{
	 // encoder
	 PLUGIN_CODEC_VERSION,	// codec API version
	 &licenseInfo,		// license information

	 PluginCodec_MediaTypeAudio |	// audio codec
	 PluginCodec_InputTypeRaw |	// raw input data
	 PluginCodec_OutputTypeRaw |	// raw output data
	 PluginCodec_RTPTypeExplicit,	// explicit RTP type

	 g729ADescr,		// text decription
	 L16Desc,
	 g729AMediaFmt,

	 NULL,			// user data

	 8000,			// samples per second
	 BITS_PER_SECOND,	// raw bits per second
	 NANOSECONDSPERFRAME,	// nanoseconds per frame
	 {{SAMPLES_PER_FRAME,	// samples per frame
	 BYTES_PER_FRAME,	// bytes per frame
	 6,			// recommended number of frames per packet
	 24}},			// maximum number of frames per packet
	 IANACODE,		// IANA RTP payload code
	 ianaName,		// RTP Payload name

	 create_encoder,	// create codec function
	 destroy_encoder,	// destroy codec
	 codec_encoder,		// encode/decode
	 NULL,			// codec controls

	 PluginCodec_H323AudioCodec_g729AnnexA,	// h323CapabilityType
	 NULL			// h323CapabilityData
	 },

	{
	 // decoder
	 PLUGIN_CODEC_VERSION,	// codec API version
	 &licenseInfo,		// license information

	 PluginCodec_MediaTypeAudio |	// audio codec
	 PluginCodec_InputTypeRaw |	// raw input data
	 PluginCodec_OutputTypeRaw |	// raw output data
	 PluginCodec_RTPTypeExplicit,	// explicit RTP type

	 g729ADescr,		// text decription
	 g729AMediaFmt,
	 L16Desc,

	 NULL,			// user data

	 8000,			// samples per second
	 BITS_PER_SECOND,	// raw bits per second
	 NANOSECONDSPERFRAME,	// nanoseconds per frame
	 {{SAMPLES_PER_FRAME,	// samples per frame
	 BYTES_PER_FRAME,	// bytes per frame
	 6,			// recommended number of frames per packet
	 24}},			// maximum number of frames per packet
	 IANACODE,		// IANA RTP payload code
	 ianaName,		// RTP Payload name

	 create_decoder,	// create codec function
	 destroy_decoder,	// destroy codec
	 codec_decoder,		// encode/decode
	 NULL,			// codec controls

	 PluginCodec_H323AudioCodec_g729AnnexA,	// h323CapabilityType 
	 NULL			// h323CapabilityData
	 }

};

#define NUM_DEFNS   (sizeof(g729CodecDefn) / sizeof(struct PluginCodec_Definition))

/////////////////////////////////////////////////////////////////////////////

PLUGIN_CODEC_DLL_API struct PluginCodec_Definition
    *PLUGIN_CODEC_GET_CODEC_FN(unsigned *count, unsigned version)
{
	*count = NUM_DEFNS;
	return g729CodecDefn;
}
