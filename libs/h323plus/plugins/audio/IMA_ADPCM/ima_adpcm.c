/*
 * IMA-ADPC plugin codec for OpenH323/OPAL
 *
 * Copyright (C) 2004 Post Increment, All Rights Reserved
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
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: ima_adpcm.c,v $
 * Revision 1.1  2007/10/22 07:40:47  shorne
 * *** empty log message ***
 *
 * Revision 1.2  2006/08/01 13:06:49  rjongbloed
 * Added a raft of unvalidated audio codecs from OpenH323 tree
 *
 * Revision 1.1.2.1  2006/07/22 13:42:49  rjongbloed
 * Added more plug ins
 *
 * Revision 1.1.2.1  2006/05/08 13:49:57  rjongbloed
 * Imported all the audio codec plug ins from OpenH323
 *
 * Revision 1.6  2005/07/15 10:08:59  rogerhardiman
 * Fix SF bug 1237507. Windows uses malloc.h. Linux and FreeBSD uses stdlib.h
 * Wrap #include with _WIN32 to be consistent with malloc.h in pwlib.
 *
 * Revision 1.5  2004/12/20 23:18:00  csoutheren
 * Added stdlib.h to all plugins to keep FreeBSD happy
 * Thanks to Kevin Oberman
 *
 * Revision 1.4  2004/06/17 22:04:55  csoutheren
 * Changed codec version number to be sensible rather than string $Ver$
 *
 * Revision 1.3  2004/05/26 03:56:02  csoutheren
 * Fixed codecs using compare functions for non-standard capability matching
 *
 * Revision 1.2  2004/05/11 14:07:58  csoutheren
 * Fixed problems with non-standard codecs using comparefunc
 *
 * Revision 1.1  2004/05/10 12:36:41  csoutheren
 * New implementation
 *
 */

#include <codec/opalplugin.h>

PLUGIN_CODEC_IMPLEMENT("IMA-ADPCM")

#include <stdlib.h>
#ifdef _WIN32
#include <malloc.h>
#endif
#include <string.h>

//By LH, Microsoft IMA ADPCM CODEC Capability
#define IMA_MAX_PACKET_SIZE 		1
#define IMA_DESIRED_TRANSMIT_SIZE	1

#define IMA_SAMPLES_PER_FRAME		505
#define IMA_BYTES_PER_FRAME		256

#define IMA_NS_PER_FRAME    63100

#define PREF_FRAMES_PER_PACKET 1
#define MAX_FRAMES_PER_PACKET  1

#define IMA_BITS_SECOND 32443

static const unsigned char msIMAHeader[] = {

  // unknown data
  0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xf9, 0x01,
  0x00, 0x00, 0xf9, 0x01,
  0x01, 0x00, 0x04, 0x00,
  0x00, 0x00, 0x00, 0x00,

#define IMA_FIXED_START			20

  // standard MS waveformatex structure follows
  0x11, 0x00,                 //    WORD    wFormatTag;        /* format type */
  0x01, 0x00,                 //    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
  0x40, 0x1f, 0x00, 0x00,     //    DWORD   nSamplesPerSec;    /* sample rate */
  0xd7, 0x0f, 0x00, 0x00,     //    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
  0x00, 0x01,                 //    WORD    nBlockAlign;       /* block size of data */
  0x04, 0x00,                 //    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
  0x02, 0x00,                 //    WORD    cbSize;            /* The count in bytes of the size of

#define IMA_FIXED_LEN			18

  // extra IMA information
  0xf9, 0x01, 		      //    WORD    numberOfSamples    /* 505 */

  // unknown data
  0x00, 0x00 

};

struct adpcm_state {
  short valprev;        /* Previous output value */
  char  index;          /* Index into stepsize table */
};

/* Intel ADPCM step variation table */
static int indexTable[16] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};

static int stepsizeTable[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

static void adpcm_coder(short indata[], char outdata[], int len, struct adpcm_state *state)
{
    short *inp;                 /* Input buffer pointer */
    signed char *outp;          /* output buffer pointer */
    int val;                    /* Current input sample value */
    int sign;                   /* Current adpcm sign bit */
    int delta;                  /* Current adpcm output value */
    int diff;                   /* Difference between val and valprev */
    int step;                   /* Stepsize */
    int valpred;                /* Predicted output value */
    int vpdiff;                 /* Current change to valpred */
    char index;                 /* Current step change index */
    int outputbuffer = 0;       /* place to keep previous 4-bit value */
    int bufferstep;             /* toggle between outputbuffer/output */

    outp = (signed char *)outdata;
    inp = indata;

    //create header
    valpred = *inp;
    memcpy(outp, (char *)inp, 2);
    inp++;
    outp += sizeof(short);

    index = state->index;
    memcpy(outp, (char *)&index, 1);
    inp++;
    outp++;
    *outp = 0;
    outp++;
    //create header ends

    len--;

    step = stepsizeTable[(int)index];

    bufferstep = 1;

    for ( ; len > 0 ; len-- ) {
        val = *inp++;

        /* Step 1 - compute difference with previous value */
        diff = val - valpred;
        sign = (diff < 0) ? 8 : 0;
        if ( sign ) diff = (-diff);

        /* Step 2 - Divide and clamp */
        /* Note:
        ** This code *approximately* computes:
        **    delta = diff*4/step;
        **    vpdiff = (delta+0.5)*step/4;
        ** but in shift step bits are dropped. The net result of this is
        ** that even if you have fast mul/div hardware you cannot put it to
        ** good use since the fixup would be too expensive.
        */
        delta = 0;
        vpdiff = (step >> 3);

        if ( diff >= step ) {
            delta = 4;
            diff -= step;
            vpdiff += step;
        }
        step >>= 1;
        if ( diff >= step  ) {
            delta |= 2;
            diff -= step;
            vpdiff += step;
        }
        step >>= 1;
        if ( diff >= step ) {
            delta |= 1;
            vpdiff += step;
        }

        /* Step 3 - Update previous value */
        if ( sign )
          valpred -= vpdiff;
        else
          valpred += vpdiff;

        /* Step 4 - Clamp previous value to 16 bits */
        if ( valpred > 32767 )
          valpred = 32767;
        else if ( valpred < -32768 )
          valpred = -32768;

        /* Step 5 - Assemble value, update index and step values */
        delta |= sign;
        index = (char)(index + indexTable[delta]);
        if ( index < 0 ) index = 0;
        if ( index > 88 ) index = 88;
        step = stepsizeTable[(int)index];

        /* Step 6 - Output value */
        if ( bufferstep ) {
            outputbuffer = (delta << 4) & 0xf0;
        } else {
            *outp++ = (char)((delta & 0x0f) | outputbuffer);
        }
        bufferstep = !bufferstep;
    }

    /* Output last step, if needed */
    if ( !bufferstep )
      *outp++ = (char)outputbuffer;

    state->valprev = (short)valpred;
    state->index = index;

}

static void adpcm_decoder(char indata[], short outdata[], int len)
{
    signed char *inp;           /* Input buffer pointer */
    short *outp;                /* output buffer pointer */
    int sign;                   /* Current adpcm sign bit */
    int delta;                  /* Current adpcm output value */
    int step;                   /* Stepsize */
    int valpred;                /* Predicted value */
    int vpdiff;                 /* Current change to valpred */
    int index;                  /* Current step change index */
    int inputbuffer = 0;        /* place to keep next 4-bit value */
    int bufferstep;             /* toggle between inputbuffer/input */

    outp = outdata;
    inp = (signed char *)indata;

    valpred = 0;
    index = 0;
    memcpy((char *)&valpred, (char *)inp, 2);
    inp += 2; //skip first 16 bits sample
    index = (int)(unsigned char)*inp;
    inp += 2; //skip index

    step = stepsizeTable[index];
    len -= 4; //skip header

    bufferstep = 0;

    len *= 2;

    for ( ; len > 0 ; len-- ) {
        /* Step 1 - get the delta value */
        if ( bufferstep ) {
            delta = inputbuffer & 0xf;
        } else {
            inputbuffer = *inp++;
            delta = (inputbuffer >> 4) & 0xf;
        }
        bufferstep = !bufferstep;

        /* Step 2 - Find new index value (for later) */
        index += indexTable[delta];
        if ( index < 0 ) index = 0;
        if ( index > 88 ) index = 88;

        /* Step 3 - Separate sign and magnitude */
        sign = delta & 8;
        delta = delta & 7;
        /* Step 4 - Compute difference and new predicted value */
        /*
        ** Computes 'vpdiff = (delta+0.5)*step/4', but see comment
        ** in adpcm_coder.
        */
        vpdiff = step >> 3;
        if ( delta & 4 ) vpdiff += step;
        if ( delta & 2 ) vpdiff += step>>1;
        if ( delta & 1 ) vpdiff += step>>2;

        if ( sign )
          valpred -= vpdiff;
        else
          valpred += vpdiff;

        /* Step 5 - clamp output value */
        if ( valpred > 32767 )
          valpred = 32767;
        else if ( valpred < -32768 )
          valpred = -32768;

        /* Step 6 - Update step value */
        step = stepsizeTable[index];

        /* Step 7 - Output value */
        *outp++ = (char)valpred;
    }
}

/////////////////////////////////////////////////////////////////////////////

static void * create_codec(const struct PluginCodec_Definition * codec)
{
  struct adpcm_state * s_adpcm = malloc(sizeof (struct adpcm_state));

  s_adpcm->valprev = 0;
  s_adpcm->index   = 0;

  return s_adpcm;
}

static int codec_encoder(const struct PluginCodec_Definition * codec,
                                           void * _context,
                                     const void * from,
                                       unsigned * fromLen,
                                           void * to,
                                       unsigned * toLen,
                                   unsigned int * flag)
{ 
  struct adpcm_state * s_adpcm = (struct adpcm_state *)_context;

  if (*fromLen < (IMA_SAMPLES_PER_FRAME*2) || (*toLen < IMA_BYTES_PER_FRAME))
    return 0;

  adpcm_coder((short *)from, (char *)to, IMA_SAMPLES_PER_FRAME, s_adpcm);

  return 1;
}

static int codec_decoder(const struct PluginCodec_Definition * codec,
                                           void * _context,
                                     const void * from,
                                       unsigned * fromLen,
                                           void * to,
                                       unsigned * toLen,
                                   unsigned int * flag)
{ 
  //struct adpcm_state * s_adpcm = (struct adpcm_state *)_context;

  if (*toLen < (IMA_SAMPLES_PER_FRAME*2) || (*fromLen < IMA_BYTES_PER_FRAME))
    return 0;

  adpcm_decoder((char *)from, (short *)to, IMA_BYTES_PER_FRAME);

  return 1;
}


static void destroy_codec(const struct PluginCodec_Definition * codec, void * _context)
{
  free(_context);
}

/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_information licenseInfo = {
  1084190703,                              // timestamp = Mon 10 May 2004 12:05:03 PM UTC = 

  "Craig Southeren, Post Increment",                           // source code author
  "1.0",                                                       // source code version
  "craigs@postincrement.com",                                  // source code email
  "http://www.postincrement.com",                              // source code URL
  "Copyright (C) 2004 by Post Increment, All Rights Reserved", // source code copyright
  "MPL 1.0",                                                   // source code license
  PluginCodec_License_MPL,                                     // source code license
  
  NULL,                                                        // codec description
  NULL,                                                        // codec author
  NULL,                                                        // codec version
  NULL,                                                        // codec email
  NULL,                                                        // codec URL
  NULL,                                                        // codec copyright information
  NULL,                                                        // codec license
  PluginCodec_Licence_None                                     // codec license code
};

/////////////////////////////////////////////////////////////////////////////

static const char L16Desc[]  = { "L16" };

static const char imaADPCM[] = "MS-IMA-ADPCM";

/////////////////////////////////////////////////////////////////////////////

#define	MICROSOFT_COUNTRY_CODE	181
#define	MICROSOFT_T35EXTENSION	0
#define	MICROSOFT_MANUFACTURER	21324

int imaCompareFunc(struct PluginCodec_H323NonStandardCodecData * data)
{
  int val;
  if (data == NULL || data->objectId != NULL || data->data == NULL)
    return -1;

  val = MICROSOFT_COUNTRY_CODE - data->t35CountryCode;
  if (val != 0)
    return val;

  val = MICROSOFT_T35EXTENSION - data->t35Extension;
  if (val != 0)
    return val;

  val = MICROSOFT_MANUFACTURER - data->manufacturerCode;
  if (val != 0)
    return val;

  val = sizeof(msIMAHeader) - data->dataLength;
  if (val != 0)
    return val;

  return memcmp(data->data+IMA_FIXED_START, msIMAHeader+IMA_FIXED_START, IMA_FIXED_LEN);
}

static struct PluginCodec_H323NonStandardCodecData imaADPCM_Cap =
{
  NULL,
  MICROSOFT_COUNTRY_CODE, MICROSOFT_T35EXTENSION, MICROSOFT_MANUFACTURER,
  msIMAHeader, sizeof(msIMAHeader),
  imaCompareFunc
};

static struct PluginCodec_Definition imaADPCMCodecDefn[] = {

{ 
  // encoder
  PLUGIN_CODEC_VERSION,                 // codec API version
  &licenseInfo,                         // license information

  PluginCodec_MediaTypeAudio |          // audio codec
  PluginCodec_InputTypeRaw |            // raw input data
  PluginCodec_OutputTypeRaw |           // raw output data
  PluginCodec_RTPTypeExplicit,          // specified RTP type

  imaADPCM,                             // text decription
  L16Desc,                              // source format
  imaADPCM,                             // destination format

  0,                                    // user data

  8000,                                 // samples per second
  IMA_BITS_SECOND,                      // raw bits per second
  IMA_NS_PER_FRAME,                     // nanoseconds per frame
  IMA_SAMPLES_PER_FRAME,                // samples per frame
  IMA_BYTES_PER_FRAME,                  // bytes per frame
  PREF_FRAMES_PER_PACKET,               // recommended number of frames per packet
  MAX_FRAMES_PER_PACKET,                // maximum number of frames per packe
  0,                                    // no IANA RTP payload code
  NULL,                                 // RTP payload name

  create_codec,                         // create codec function
  destroy_codec,                        // destroy codec
  codec_encoder,                        // encode/decode
  NULL,                                 // codec controls

  PluginCodec_H323Codec_nonStandard,    // h323CapabilityType 
  &imaADPCM_Cap                         // h323CapabilityData
},

{ 
  // decoder
  PLUGIN_CODEC_VERSION,                 // codec API version
  &licenseInfo,                         // license information

  PluginCodec_MediaTypeAudio |          // audio codec
  PluginCodec_InputTypeRaw |            // raw input data
  PluginCodec_OutputTypeRaw |           // raw output data
  PluginCodec_RTPTypeDynamic,           // dynamic RTP type

  imaADPCM,                             // text decription
  imaADPCM,                             // source format
  L16Desc,                              // destination format

  0,                                    // user data

  8000,                                 // samples per second
  IMA_BITS_SECOND,                      // raw bits per second
  IMA_NS_PER_FRAME,                     // nanoseconds per frame
  IMA_SAMPLES_PER_FRAME,                // samples per frame
  IMA_BYTES_PER_FRAME,                  // bytes per frame
  PREF_FRAMES_PER_PACKET,               // recommended number of frames per packet
  MAX_FRAMES_PER_PACKET,                // maximum number of frames per packe
  0,                                    // no IANA RTP payload code
  NULL,                                 // RTP payload name

  create_codec,                         // create codec function
  destroy_codec,                        // destroy codec
  codec_decoder,                        // encode/decode
  NULL,                                 // codec controls

  PluginCodec_H323Codec_nonStandard,    // h323CapabilityType 
  &imaADPCM_Cap                         // h323CapabilityData
},


};

#define NUM_DEFNS   (sizeof(imaADPCMCodecDefn) / sizeof(struct PluginCodec_Definition))

/////////////////////////////////////////////////////////////////////////////

PLUGIN_CODEC_DLL_API struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned version)
{
  *count = NUM_DEFNS;
  return imaADPCMCodecDefn;
}
