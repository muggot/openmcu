/* g7231codec.h
 *
 * Easy G7231 codec for OpenH323/OPAL
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

#include "EasyG7231/EasyG7231.h"



/////////////////////////////////////////////////////////////////////////////

//#define Ecodec EasyG7231
static const char		EasyG7231_63_Desc[] = { "Easy G7231-6.3" };	// text decription  
static const char		EasyG7231_63_L16Desc[]= { "L16" };			// source format 
static const char		EasyG7231_63_MediaFmt[] = { "G.7231-6.3k[e]" };	// destination format  
static unsigned int		EasyG7231_63_SamplePerSec = 8000;          // samples per second  
static unsigned int		EasyG7231_63_BitsPerSec = 6300;			// raw bits per second  
static unsigned int		EasyG7231_63_NsPerFrame = 30000;			// nanoseconds per frame
static unsigned int		EasyG7231_63_SamplesPerFrame = L_G7231_FRAME; // samples per frame 
static unsigned int		EasyG7231_63_BytesPerFrame = L_G7231_FRAME_COMPRESSED_63; // Bytes per frame
static unsigned int		EasyG7231_63_RecFramesPerPacket = 1;        //rec number of frames per packet 
static unsigned int		EasyG7231_63_MaxFramesPerPacket = 1;       // max number of frames per packet
#define	EasyG7231_63_IANACode  4									// IANA RTP payload code  
static const char		EasyG7231_63_IANAName[] = { "G723" };		// RTP payload name 
static unsigned char	EasyG7231_63_CapType = PluginCodec_H323AudioCodec_g7231;	// h323CapabilityType 



// Base Class 
class G7231_EasyCodec : public EasyCodec
{ 
  public: 
	virtual unsigned long init_encoder() 
		{ return EasyG7231_init_encoder(Is63);} 
	virtual bool encoder(unsigned long hEncoder, short *speech, unsigned char *bitstream) 
		{ return EasyG7231_encoder(hEncoder, speech, bitstream);} 
	virtual bool release_encoder(unsigned long hEncoder)
		{ return EasyG7231_release_encoder(hEncoder);} 
	virtual unsigned long init_decoder() 
		{ return EasyG7231_init_decoder(Is63);} 
	virtual bool decoder(unsigned long hDecoder, unsigned char *bitstream, short *synth_short) 
		{ return EasyG7231_decoder(hDecoder, bitstream, synth_short);} 
	virtual bool release_decoder(unsigned long hDecoder) 
		{ return EasyG7231_release_decoder(hDecoder);}  
  protected:
	bool Is63;
}; 

// Derived classes
class G7231_53_EasyCodec : public G7231_EasyCodec
{ 
  public:
	G7231_53_EasyCodec()
		{ Is63 = false; };
};

class G7231_63_EasyCodec : public G7231_EasyCodec
{ 
  public:
	G7231_63_EasyCodec()
		{ Is63 = true; };
};


//#pragma data_seg(".G7231_SHARED")				// define the segment
//#pragma comment(lib,"EasyG7231/EasyG7231.lib")
static G7231_63_EasyCodec * m_G7231_63_codec = NULL;	// specify the data and initialise at Instantiation
//#pragma data_seg()
//#pragma comment(linker, "/section:.G7231_SHARED,rws") // Link the segment
