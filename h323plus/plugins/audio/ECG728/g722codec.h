/* g722codec.h
 *
 * Easy G722 codec for OpenH323/OPAL
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

#include "EasyG722/EasyG722.h"

/////////////////////////////////////////////////////////////////////////////

//#define Ecodec EasyG722
static const char		EasyG722_Desc[] = { "Easy G722" };		// text decription  
static const char		EasyG722_L16Desc[]= { "L16" };			// source format 
static const char		EasyG722_MediaFmt[] = { "G.722-64k[e]" }; // destination format  
static unsigned int		EasyG722_SamplePerSec = 16000;          // samples per second  
static unsigned int		EasyG722_BitsPerSec = 64000;			// raw bits per second  
static unsigned int		EasyG722_NsPerFrame = 10000;			// nanoseconds per frame
static unsigned int		EasyG722_SamplesPerFrame = L_G722_FRAME; // samples per frame 
static unsigned int		EasyG722_BytesPerFrame = L_G722_FRAME_COMPRESSED; // Bytes per frame Compressed
static unsigned int		EasyG722_RecFramesPerPacket = 1;        //rec number of frames per packet 
static unsigned int		EasyG722_MaxFramesPerPacket = 1;        // max number of frames per packet
#define	EasyG722_IANACode  9									// IANA RTP payload code  
static const char		EasyG722_IANAName[] = { "G722" };		// RTP payload name 
static unsigned char	EasyG722_CapType = PluginCodec_H323AudioCodec_g722_64k;	// h323CapabilityType 



class G722_EasyCodec : public EasyCodec
{ 
  public: 
	unsigned long init_encoder() 
		{ return EasyG722_init_encoder();} 
	bool encoder(unsigned long hEncoder, short *speech, unsigned char *bitstream) 
		{ return EasyG722_encoder(hEncoder, speech, bitstream);} 
	bool release_encoder(unsigned long hEncoder)
		{ return EasyG722_release_encoder(hEncoder);} 
	unsigned long init_decoder() 
		{ return EasyG722_init_decoder();} 
	bool decoder(unsigned long hDecoder, unsigned char *bitstream, short *synth_short) 
		{ return EasyG722_decoder(hDecoder, bitstream, synth_short);} 
	bool release_decoder(unsigned long hDecoder) 
		{ return EasyG722_release_decoder(hDecoder);}  
}; 

#pragma data_seg(".G722_SHARED")    // Define the Segment
#pragma comment(lib,"EasyG722/EasyG722.lib")  // load Library into shared Memory
static G722_EasyCodec * m_G722codec = NULL;   // specify the data and initialise at Instantiation
#pragma data_seg()
#pragma comment(linker, "/section:.G722_SHARED,rws") // link it
