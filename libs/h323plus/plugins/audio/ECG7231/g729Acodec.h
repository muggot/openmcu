/* g729Acodec.h
 *
 * Easy G729A codec for OpenH323/OPAL
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
#include "easycodecs.h"

#include "EasyG729A/EasyG729A.h"

/////////////////////////////////////////////////////////////////////////////

//#define Ecodec EasyG729A
static const char		EasyG729A_Desc[] = { "Easy G729A" };	// text decription  
static const char		EasyG729A_L16Desc[]= { "L16" };			// source format 
static const char		EasyG729A_MediaFmt[] = { "G.729A-8k[e]" };	// destination format  
static unsigned int		EasyG729A_SamplePerSec = 8000;          // samples per second  
static unsigned int		EasyG729A_BitsPerSec = 8000;			// raw bits per second  
static unsigned int		EasyG729A_NsPerFrame = 10000;			// nanoseconds per frame
static unsigned int		EasyG729A_SamplesPerFrame = L_G729A_FRAME; // samples per frame 
static unsigned int		EasyG729A_BytesPerFrame = L_G729A_FRAME_COMPRESSED; // Bytes per frame
static unsigned int		EasyG729A_RecFramesPerPacket = 1;        //rec number of frames per packet 
static unsigned int		EasyG729A_MaxFramesPerPacket = 1;       // max number of frames per packet
#define	EasyG729A_IANACode  18									// IANA RTP payload code  
static const char		EasyG729A_IANAName[] = { "G729A" };		// RTP payload name 
static unsigned char	EasyG729A_CapType = PluginCodec_H323AudioCodec_g729AnnexA;	// h323CapabilityType 


class G729A_EasyCodec : public EasyCodec
{ 
  public: 
	unsigned long init_encoder() 
		{ return EasyG729A_init_encoder();} 
	bool encoder(unsigned long hEncoder, short *speech, unsigned char *bitstream) 
		{ return EasyG729A_encoder(hEncoder, speech, bitstream);} 
	bool release_encoder(unsigned long hEncoder)
		{ return EasyG729A_release_encoder(hEncoder);} 
	unsigned long init_decoder() 
		{ return EasyG729A_init_decoder();} 
	bool decoder(unsigned long hDecoder, unsigned char *bitstream, short *synth_short) 
		{ return EasyG729A_decoder(hDecoder, bitstream, synth_short);} 
	bool release_decoder(unsigned long hDecoder) 
		{ return EasyG729A_release_decoder(hDecoder);}  
}; 

#pragma data_seg(".G729A_SHARED")				// define the segment
#pragma comment(lib,"EasyG729A/EasyG729A.lib")
static G729A_EasyCodec * m_G729Acodec = NULL; 	// specify the data and initialise at Instantiation
#pragma data_seg()
#pragma comment(linker, "/section:.G729A_SHARED,rws") // Link the segment