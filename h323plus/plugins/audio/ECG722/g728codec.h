/* g728codec.h
 *
 * Easy G728 codec for OpenH323/OPAL
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

#include "EasyG728/EasyG728.h"

/////////////////////////////////////////////////////////////////////////////

//#define Ecodec EasyG728
static const char		EasyG728_Desc[] = { "Easy G728" };	// text decription  
static const char		EasyG728_L16Desc[]= { "L16" };			// source format 
static const char		EasyG728_MediaFmt[] = { "G.728-16k[e]" };	// destination format  
static unsigned int		EasyG728_SamplePerSec = 8000;          // samples per second  
static unsigned int		EasyG728_BitsPerSec = 16000;			// raw bits per second  
static unsigned int		EasyG728_NsPerFrame = 10000;			// nanoseconds per frame
static unsigned int		EasyG728_SamplesPerFrame = L_G728_FRAME; // samples per frame 
static unsigned int		EasyG728_BytesPerFrame = L_G728_FRAME_COMPRESSED; // Bytes per frame
static unsigned int		EasyG728_RecFramesPerPacket = 1;        //rec number of frames per packet 
static unsigned int		EasyG728_MaxFramesPerPacket = 1;       // max number of frames per packet
#define	EasyG728_IANACode  15									// IANA RTP payload code  
static const char		EasyG728_IANAName[] = { "G728" };		// RTP payload name 
static unsigned char	EasyG728_CapType = PluginCodec_H323AudioCodec_g728;	// h323CapabilityType 


class G728_EasyCodec : public EasyCodec
{ 
  public: 
	unsigned long init_encoder() 
		{ return EasyG728_init_encoder();} 
	bool encoder(unsigned long hEncoder, short *speech, unsigned char *bitstream) 
		{ return EasyG728_encoder(hEncoder, speech, bitstream);} 
	bool release_encoder(unsigned long hEncoder)
		{ return EasyG728_release_encoder(hEncoder);} 
	unsigned long init_decoder() 
		{ return EasyG728_init_decoder();} 
	bool decoder(unsigned long hDecoder, unsigned char *bitstream, short *synth_short) 
		{ return EasyG728_decoder(hDecoder, bitstream, synth_short);} 
	bool release_decoder(unsigned long hDecoder) 
		{ return EasyG728_release_decoder(hDecoder);}  
}; 

#pragma data_seg(".G728_SHARED")				// define the segment
#pragma comment(lib,"EasyG728/EasyG728.lib")
static G728_EasyCodec * m_G728codec = NULL; 	// specify the data and initialise at Instantiation
#pragma data_seg()
#pragma comment(linker, "/section:.G728_SHARED,rws") // Link the segment
