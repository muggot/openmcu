/****************************************************************************
 *
 * AMR-WB (ITU G.722.2) Plugin codec for OpenH323/OPAL
 *
 * Copyright (c) 2009 Nimajin Software Consulting, All Rights Reserved
 * Copyright (C) 2010 Vox Lucida Pty. Ltd.
 *
 * Permission to copy, use, sell and distribute this file is granted
 * provided this copyright notice appears in all copies.
 * Permission to modify the code herein and to distribute modified code is
 * granted provided this copyright notice appears in all copies, and a
 * notice that the code was modified is included with the copyright notice.
 *
 * This software and information is provided "as is" without express or im-
 * plied warranty, and with no claim as to its suitability for any purpose.
 *
 ****************************************************************************
 *
 * AMR WB ACELP wideband decoder described in 3GPP TS 26.171, 26.190, 26.201
 * AMR-WB is, in fact, ITU G.722.2.
 * G.722.2 Annex F specifies usage in H.245.
 * There is newer but conflicting H.245 info in H.245 v13 Annex R.
 * This code follows the G.722.2 H.245 signaling.
 * Its payload format & SDP parameters are described in IETF RFC 3267
 * This implementation employs 3GPP TS 26.173 fixed point reference code.
 * It implements Adaptive Multi Rate Wideband speech transcoder (3GPP TS 
 * 26.190), Voice Activity Detection (3GPP TS 26.194), and comfort noise (3GPP
 * TS 26.192)
 * This build has not defined IF2
 * Encodes to octet-aligned format only (not bandwidth efficient)
 * Encodes to 23.85 kbps only (no mode change support)
 * Mode adaption not supported (needs work to link decoder to encoder)
 * Decoder handles only 1 frame (no multichannel or interleaving)
 * Decoder can handle any mode, and both octet-aligned and bandwidth efficient
 * AMR-WB is patented. Use requires licence from VoiceAge.
 *
 * Initial development by: Ted Szoczei, Nimajin Software Consulting, 09-12-31
 * Portions developed by: Robert Jongbloed, Vox Lucida Pty. Ltd.
 *
 * 06-04-03 Revise to conform to RFC 3267: use correct packet sizes, handle 
 *            missing CMR, detect use of bandwidth efficient mode, pass quality
 *            indicator to decoder. /tsz
 ****************************************************************************/


#define _CRT_NONSTDC_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdlib.h>

#ifndef PLUGIN_CODEC_DLL_EXPORTS
#include "opal/plugin-config.h"
#endif

#include "opal/opalplugin.h"
#include "g7222.h"

/////////////////////////////////////////////////////////////////////////////

static const char G7222Description[]  = "ITU-T G.722.2 (AMR-WB) - Wideband Adaptive Multirate Codec";

PLUGINCODEC_LICENSE(
  "Ted Szoczei, Nimajin Software Consulting",                  // source code author
  "1.0",                                                       // source code version
  "ted.szoczei@nimajin.com",                                   // source code email
  "http://www.nimajin.com",                                    // source code URL
  "Copyright (c) 2009 Nimajin Software Consulting",            // source code copyright
  "None",                                                      // source code license
  PluginCodec_License_None,                                    // source code license
  
  G7222Description,                                            // codec description
  "3rd Generation Partnership Project (3GPP)",                 // codec author
  "TS 26.173 V6.0.0 2004-12",                                  // codec version
  NULL,                                                        // codec email
  "http://www.3gpp.org",                                       // codec URL
  "",                                                          // codec copyright information
  "",                                                          // codec license
  PluginCodec_License_RoyaltiesRequired                        // codec license code
);


/////////////////////////////////////////////////////////////////////////////

extern "C" {
#include "AMR-WB/enc_if.h"
#include "AMR-WB/dec_if.h"
};

enum ConversionMode {   // these equate to modes in enc_main.c
  AMRWB_7k = 0,         //  6.60 kbit/s 
  AMRWB_9k,             //  8.85 kbit/s 
  AMRWB_12k,            // 12.65 kbit/s 
  AMRWB_14k,            // 14.25 kbit/s 
  AMRWB_16k,            // 15.85 kbit/s 
  AMRWB_18k,            // 18.25 kbit/s 
  AMRWB_20k,            // 19.85 kbit/s 
  AMRWB_23k,            // 23.05 kbit/s 
  AMRWB_24k,            // 23.85 kbit/s default
  AMRWB_MODES,
  AMRWB_SID = 9,        // dtx
  AMRWB_LOST = 14,
  AMRWB_NODATA = 15
};

// This stuff copied from AMR-WB/if_rom.c
// AMR-WB did not extern their block size table, they only use it to clear data before encoding.

// One encoded frame (bytes): includes TOC, but not header
// AMRWB_core_block_bits is the number of bits for each core frame
// AMRWB_block_size_octet is the number of octets for each core frame and ToC for RFC 3267 octet-aligned packing

const UWord16 AMRWB_core_block_bits[16]= { 132, 177, 253, 285, 317, 365, 397, 461, 477, 40, 40, 0, 0, 0, 0, 0 };
const UWord16 AMRWB_efficient_ToC_bits = 6;
const UWord16 AMRWB_efficient_CMR_bits = 4;

const UWord8 AMRWB_block_size_octet[16]= { 18, 24, 33, 37, 41, 47, 51, 59, 61, 6, 6, 0, 0, 0, 1, 1 };

// RFC 3267 octet-aligned ToC adds 8 bits to each core speech bits frame and 8 per packet for header
// RFC 3267 bandwidth-efficient adds 6 bits to each core speech bits frame and 4 per packet for header
// 3GPP IF1 mode adds 24 bits to core speech bits
// 3GPP IF2 mode adds 5 bits to core speech bits

#define AMRWB_ALIGNED_BPS(mode) ((AMRWB_block_size_octet[(mode)] + 1) * 50 * 8)

// All formats except 3GPP IF1 pad the core frame to fill the last octet (even when there are multiple
// frames within the packet).
// This build has not defined IF2 
// RFC 3267 bandwidth-efficient format does not pad between multiple ToC entries.
// LOST & NODATA packets have no core data, Thus the rounding-up for block size only counts for 
// their ToC entries.



/////////////////////////////////////////////////////////////////////////////
// Convert PCM16-16KHZ to AMR-WB
// Convert 320 samples of audio (mode 0 produces 19 bytes output, mode 8, 62)
// Output packed according to RFC 3267, section 4.4 (octet aligned)
// Supports only single 20 ms frame per packet.

// this is what we hand back when we are asked to create an encoder
typedef struct
{
  void    * state;              // Encoder interface's opaque state
  unsigned  mode;               // current mode
  int       vad;                // silence suppression 1/0
} AMRWBEncoderContext;


static void * MyCreateEncoder(const struct PluginCodec_Definition * codec)
{
  AMRWBEncoderContext * Context = (AMRWBEncoderContext *) malloc (sizeof(AMRWBEncoderContext));
  if (Context == NULL)
    return NULL;

  Context->mode = AMRWB_24k;    // start off in 23.85kbps mode
  Context->vad = 0;             // with no VAD/DTX/CN

  Context->state = E_IF_init ();
  if (Context->state == NULL)
  {
    free (Context);
    return NULL;
  }
  return Context;
}


static void MyDestroyEncoder(const struct PluginCodec_Definition * codec, void * context)
{
  AMRWBEncoderContext * Context = (AMRWBEncoderContext *)context;
  E_IF_exit (Context->state);
  free (Context);
}


static int MyEncodeAudio(const struct PluginCodec_Definition * codec, 
                                                        void * context,
                                                  const void * fromPtr, 
                                                    unsigned * fromLen,
                                                        void * toPtr,         
                                                    unsigned * toLen,
                                                unsigned int * flag)
{ 
  AMRWBEncoderContext * Context = (AMRWBEncoderContext *)context;
  if (*fromLen != G7222_SAMPLES_PER_FRAME * sizeof(short))
  {
    //PTRACE(2, "Codec\tAMR-WB encoder: Audio data of size " << *fromLen << " did not match expected " << G7222_SAMPLES_PER_FRAME * sizeof(short));
    return 0;
  }
  if (*toLen < (unsigned) AMRWB_block_size_octet[Context->mode] + 1)
  {
    //PTRACE(2,"Codec\tAMR-WB encoder: Output buffer of size " << *toLen << " too short for mode " << mode);
    return 0;
  }
  // First byte is CMR (change mode request). 0xF0 means we'll take anything.
  UWord8 * Dest = (UWord8 *) toPtr;
  *Dest++ = 0x80;//0xF0;  / 0x80 means we want 24kbps only (BT phone won't accept 0xF0)

  // Next byte is TOC (containing frame type), then follows encoded data
  // The TOC and data are both written by E_IF_encode.
  int ByteCount = E_IF_encode (Context->state, (Word16) Context->mode, (short *) fromPtr, Dest, Context->vad);
  if (ByteCount < 1)
  {
    *toLen = 0;
    return 0;   // Bad mode
  }
  *toLen = ByteCount + 1;
  return 1;
}


/////////////////////////////////////////////////////////////////////////////
// Convert AMR-WB to PCM16-16KHZ
// Convert encoded source to 320 samples of audio.
// Allows only input packed according to RFC 3267, section 4.4 (octet aligned),
// but header byte (change mode request) may be missing.
// this code does not handle multiple frame packets!


static void * MyCreateDecoder(const struct PluginCodec_Definition * codec)
{
  return D_IF_init ();
}


static void MyDestroyDecoder(const struct PluginCodec_Definition * codec, void * context)
{
  D_IF_exit (context);
}


// AMRWB_7k = 0     0x04 frame_type = 0, fqi = 1
// AMRWB_9k,        0x0C frame_type = 1, fqi = 1
// AMRWB_12k,       0x14 frame_type = 2, fqi = 1
// AMRWB_14k,       0x1C frame_type = 3, fqi = 1
// AMRWB_16k,       0x24 frame_type = 4, fqi = 1
// AMRWB_18k,       0x2C frame_type = 5, fqi = 1
// AMRWB_20k,       0x34 frame_type = 6, fqi = 1
// AMRWB_23k,       0x3C frame_type = 7, fqi = 1
// AMRWB_24k,       0x44 frame_type = 8, fqi = 1
// AMRWB_SID = 9,   0x4C frame_type = 9, fqi = 1  DTX/VAD/CN
// AMRWB_LOST = 14,
// AMRWB_NODATA = 15

static int AMRWBTypeGet (const UWord8 byte)
{
  if ((byte & 0x03) != 0)               // pad bits must be 0
    return -1;

  int Type = (byte >> 3) & 0x0F;
  if (Type > AMRWB_SID && Type != AMRWB_LOST && Type != AMRWB_NODATA)
    return -1;

  return Type;
}

int AMRWBIsBandWidthEfficient (const unsigned short word, const unsigned int packetSize)
{
  int RequestedMode = (word >> 12) & 0x0F;
  if (RequestedMode > AMRWB_24k && RequestedMode != AMRWB_NODATA)
    return 0;

  int Type = (word >> 7) & 0x0F;
  if (Type > AMRWB_SID && Type != AMRWB_LOST && Type != AMRWB_NODATA)
    return 0;

  // this code does not handle multiple frame packets!
  unsigned int ExpectedSize = (AMRWB_core_block_bits[Type] + AMRWB_efficient_ToC_bits + AMRWB_efficient_CMR_bits + 7) / 8;
  return (ExpectedSize == packetSize)? 1 : 0;
}


// Effects of PluginCodec_DecodeSilence when stream detects silence:
// Encoder
// DecodeSilence=1: Transcode will pass us 0 for fromPtr & fromLen
//                  Typically we will use this to fill output with comfort noise frame
// DecodeSilence=0: Transcode will pass us input of inputBytesPerFrame 0's
// Decoder
// DecodeSilence=1: Transcode will pass us 0 for fromPtr & fromLen and set PluginCodec_CoderSilenceFrame flag
//                  Typically we will use this to fill output with comfort noise
// DecodeSilence=0: Transcode will not call us, and pass along an outputBytesPerFrame 0-filled frame
// AMR-WB encoder supports VAD if dtx flag is set. Then encoder will send SIDs during silence.
// But encoder requires full frame to detect silence, so leave PluginCodec_DecodeSilence off encoder.
// AMR-WB decoder will generate something if told frame was lost, so use PluginCodec_DecodeSilence on decoder
// to indicate lost frame with PluginCodec_CoderSilenceFrame flag.

static int MyDecodeAudio(const struct PluginCodec_Definition * codec,
                                                       void * context,
                                                 const void * fromPtr,
                                                   unsigned * fromLen,
                                                       void * toPtr,
                                                   unsigned * toLen,
                                               unsigned int * flag)
{
  if (fromPtr == NULL || fromLen == NULL || (*flag & PluginCodec_CoderSilenceFrame) != 0)
  {                                     // no data: return comfort noise
      D_IF_decode (context, NULL, (Word16 *) toPtr, _no_frame);
  }
  else
  {
    if (*fromLen < 1)
    {
      //PTRACE(2,"Codec\tAMR-WB decoder: No input");
      return 0;
    }
    if (*toLen < G7222_SAMPLES_PER_FRAME * sizeof(short))
    {
      //PTRACE(2,"Codec\tAMR-WB decoder: Output buffer of size " << *toLen << " less than " << G7222_SAMPLES_PER_FRAME * sizeof(short) << " required");
      return 0;
    }
    // test the input packet
    UWord8 * Src = (UWord8 *) fromPtr;
    int Quality = ((*(Src + 1) & 0x04) == 0)? _bad_frame : _good_frame;
    int Followed = (*(Src + 1) >> 7) & 0x01;
    int FrameType = AMRWBTypeGet (*(Src + 1));
    int RequestedMode = -1;
    int ValidPacket = -1;               // -3=bad CMR octet, -2=bad size, -1=bad frametype, 0=bad CMR value, 1=good
    int Input = 1;                      // offset into Src of start of data (Toc) for decoding
    if (FrameType >= 0)
    {                                   // check for valid header & ToC pair
      ValidPacket = (*fromLen == AMRWB_block_size_octet[FrameType] + 1)? 1 : -2;
      if (ValidPacket > 0)
      {                                 // size is right
        if ((*Src & 0x0F) != 0)         // test CMR byte structure
          ValidPacket = -3;
        else
        {                               // test CMR value
          RequestedMode = (*Src >> 4) & 0x0F;
          if (RequestedMode < AMRWB_7k
          || (RequestedMode > AMRWB_24k && RequestedMode != 0x0F))
            ValidPacket = 0;            // probably proper octet-aligned packet but possibly bad, check alternatives
        }
      }
    }
    if (ValidPacket <= 0)
    {                                   // not a valid 2-octet header & TOC, try just ToC
      int TOCFrameType = AMRWBTypeGet (*Src);
      if (TOCFrameType >= 0 && *fromLen == AMRWB_block_size_octet[TOCFrameType])
      {
        ValidPacket = 1;
        FrameType = TOCFrameType;
        Quality = ((*Src & 0x04) == 0)? _bad_frame : _good_frame;
        Followed = (*Src & 0x80) >> 7;
        Input = 0;
        //PTRACE(2, "Codec\tAMR-WB decoder: Received packet without header (CMR) octet - contrary to RFC 3267, processed anyway");
        //PTRACE(2, "Codec\tAMR-WB decoder: First octet value was ToC 0x"
        //           << hex << setfill('0') << setprecision(2) << (unsigned) *Src << dec << setfill(' ')
        //           << " (F=" << Followed  << " FT=" << FrameType << " Q=" << (Quality ^ 1) << ')');
      }
    }                                     // check if invalid octet-aligned packet is bandwidth efficient type
    if (ValidPacket < 0 && AMRWBIsBandWidthEfficient ((*Src << 8) + *(Src + 1), *fromLen))
    {
      //PTRACE(2, "Codec\tAMR-WB decoder: Received packet appeared to be packed in RFC 3267 bandwidth efficient mode. Unsupported.");
      return 0;
    }
    switch (ValidPacket)
    {                                   // it's octet-aligned but bad
    case -1:
      //PTRACE(2, "Codec\tAMR-WB decoder: Received packet with invalid ToC octet 0x"
      //          << hex << setfill('0') << setprecision(2) << (unsigned)*(Src + 1) << dec << setfill(' '));
      return 0;

    case -2:
      //PTRACE(2, "Codec\tAMR-WB decoder: Packet size " << *fromLen << " did not match expected " << (unsigned)(AMRWB_block_size_octet[FrameType] + 1) << " for frame type " << FrameType);
      return 0;

    case -3:
      //PTRACE(2, "Codec\tAMR-WB decoder: Received packet with invalid header octet 0x"
      //        << hex << setfill('0') << setprecision(2) << (unsigned) *Src << dec << setfill(' '));
      return 0;
    }
    // Aw.. we did all that work and can't change encoder's mode because send & receive contexts are not attached!
    //if (ValidPacket == 0)
    //  PTRACE(2, "Codec\tAMR-WB decoder: Received packet with invalid requested mode " << RequestedMode << ", mode change ignored");

    //else if (RequestedMode != 0x0F)       // 0x0F means no change requested, so we'll just stick to sending whatever the last mode was
    //  mode = (ConversionMode) RequestedMode;

    //if (Followed)
    //  PTRACE(2, "Codec\tAMR-WB decoder: Received packet indicated multiple frames. Unsupported. Audio was lost.");

    D_IF_decode (context, Src + Input, (Word16 *) toPtr, Quality);

    // return the number of decoded bytes to the caller
    *fromLen = AMRWB_block_size_octet[FrameType] + Input; // Actual bytes consumed
  }
  *toLen = G7222_SAMPLES_PER_FRAME * sizeof(short);
  return 1;
}


/////////////////////////////////////////////////////////////////////////////

// limit frames per packet to 1

static struct PluginCodec_Option const OptionInitialMode =
{
  PluginCodec_IntegerOption,          // PluginCodec_OptionTypes
  G7222InitialModeName,               // Generic (human readable) option name
  false,                              // User Read/Only flag
  PluginCodec_MinMerge,               // Merge mode
  STRINGIZE(G7222_MODE_INITIAL_VALUE),// Initial value
  G7222InitialModeFMTPName,           // SIP/SDP FMTP name
  STRINGIZE(G7222_MODE_INITIAL_VALUE),// SIP/SDP FMTP default value (option not included in FMTP if have this value)
  G7222_H245_REQUEST_MODE,            // H.245 generic capability code and bit mask
  "0",                                // Minimum value
  STRINGIZE(G7222_MAX_MODES)          // Maximum value
};

static struct PluginCodec_Option const OptionRxFramesPerPacket =
{
  PluginCodec_IntegerOption,          // PluginCodec_OptionTypes
  PLUGINCODEC_OPTION_RX_FRAMES_PER_PACKET, // Generic (human readable) option name
  false,                              // User Read/Only flag
  PluginCodec_MinMerge,               // Merge mode
  "1",                                // Initial value
  NULL,                               // SIP/SDP FMTP name
  NULL,                               // SIP/SDP FMTP default value (option not included in FMTP if have this value)
  G7222_H245_MAXAL_SDUFRAMES_RX,      // H.245 generic capability code and bit mask
  "1",                                // Minimum value
  "1"                                 // Maximum value    // Do not change!! See above.
};

static struct PluginCodec_Option const OptionTxFramesPerPacket =
{
  PluginCodec_IntegerOption,          // Option type
  PLUGINCODEC_OPTION_TX_FRAMES_PER_PACKET, // User visible name
  false,                              // User Read/Only flag
  PluginCodec_MinMerge,               // Merge mode
  "1",                                // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  G7222_H245_MAXAL_SDUFRAMES_TX,      // H.245 generic capability code and bit mask
  "1",                                // Minimum value
  "1"                                 // Maximum value
};

// this is here so FMTP always adds 'octet-align=1'
// this option is indicated in fmtp by its presence

static struct PluginCodec_Option const OptionOctetAlign =
{
  PluginCodec_BoolOption,             // Option type
  G7222AlignmentOptionName,           // User visible name
  true,                               // User Read/Only flag
  PluginCodec_EqualMerge,             // Merge mode
  "1",                                // Initial value
  G7222AlignmentFMTPName,             // FMTP option name
  "0",                                // FMTP default value
  G7222_H245_OCTET_ALIGNED            // H.245 generic capability code and bit mask
};

static struct PluginCodec_Option const ModeSetG7222 =
{
  PluginCodec_IntegerOption,          // Option type
  G7222ModeSetOptionName,             // User visible name
  true,                               // User Read/Only flag
  PluginCodec_EqualMerge,             // Merge mode
  STRINGIZE(G7222_MODE_SET_INITIAL_VALUE), // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  G7222_H245_MODE_SET                 // H.245 generic capability code and bit mask
};

static struct PluginCodec_Option const ModeChangePeriodG7222 =
{
  PluginCodec_IntegerOption,          // Option type
  G7222ModeChangePeriodOptionName,    // User visible name
  false,                              // User Read/Only flag
  PluginCodec_MinMerge,               // Merge mode
  "0",                                // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  G7222_H245_MODE_CHANGE_PERIOD,      // H.245 generic capability code and bit mask
  "0",                                // Minimum value
  "1000"                              // Maximum value
};

static struct PluginCodec_Option const ModeChangeNeighbourG7222 =
{
  PluginCodec_BoolOption,             // Option type
  G7222ModeChangeNeighbourOptionName, // User visible name
  false,                              // User Read/Only flag
  PluginCodec_AndMerge,               // Merge mode
  "0",                                // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  G7222_H245_MODE_CHANGE_NEIGHBOUR    // H.245 generic capability code and bit mask
};

static struct PluginCodec_Option const CRC_G7222 =
{
  PluginCodec_BoolOption,             // Option type
  G7222CRCOptionName,                 // User visible name
  true,                               // User Read/Only flag
  PluginCodec_AndMerge,               // Merge mode
  "0",                                // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  G7222_H245_CRC                      // H.245 generic capability code and bit mask
};

static struct PluginCodec_Option const RobustSortingG7222 =
{
  PluginCodec_BoolOption,             // Option type
  G7222RobustSortingOptionName,       // User visible name
  true,                               // User Read/Only flag
  PluginCodec_AndMerge,               // Merge mode
  "0",                                // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  G7222_H245_ROBUST_SORTING           // H.245 generic capability code and bit mask
};

static struct PluginCodec_Option const InterleavingG7222 =
{
  PluginCodec_BoolOption,             // Option type
  G7222InterleavingOptionName,        // User visible name
  true,                               // User Read/Only flag
  PluginCodec_AndMerge,               // Merge mode
  "0",                                // Initial value
  NULL,                               // FMTP option name
  NULL,                               // FMTP default value
  G7222_H245_INTERLEAVING             // H.245 generic capability code and bit mask
};

static struct PluginCodec_Option const MediaPacketizationRFC3267 =
{
  PluginCodec_StringOption,           // Option type
  PLUGINCODEC_MEDIA_PACKETIZATION,    // User visible name
  true,                               // User Read/Only flag
  PluginCodec_NoMerge,                // Merge mode
  "RFC3267"                           // Initial value
};

#ifdef PLUGINCODEC_MEDIA_PACKETIZATIONS
static struct PluginCodec_Option const MediaPacketizationsRFC3267 =
{
  PluginCodec_StringOption,           // Option type
  PLUGINCODEC_MEDIA_PACKETIZATIONS,   // User visible name
  true,                               // User Read/Only flag
  PluginCodec_NoMerge,                // Merge mode
  "RFC3267,RFC4867"                   // Initial value
};
#endif

static struct PluginCodec_Option const * const OptionsTable[] = {
  &OptionInitialMode,
  &OptionRxFramesPerPacket,
  &OptionTxFramesPerPacket,
  &MediaPacketizationRFC3267,
#ifdef PLUGINCODEC_MEDIA_PACKETIZATIONS
  &MediaPacketizationsRFC3267,
#endif
  &OptionOctetAlign,
  //&ModeSetG7222,
  &ModeChangePeriodG7222,
  &ModeChangeNeighbourG7222,
  &CRC_G7222,
  &RobustSortingG7222,
  &InterleavingG7222,
  NULL
};


static int GetOptions(const struct PluginCodec_Definition * defn,
                                                     void * context, 
                                               const char * name,
                                                     void * parm,
                                                 unsigned * parmLen)
{
  if (parm == NULL || parmLen == NULL || *parmLen != sizeof(struct PluginCodec_Option **))
    return 0;

  *(struct PluginCodec_Option const * const * *)parm = OptionsTable;
  return 1;
}


static struct PluginCodec_ControlDefn MyControlsTable[] =
{
  { "get_codec_options", GetOptions },
  { NULL }
};


/////////////////////////////////////////////////////////////////////////////

// Ref. Table F.1/G.722.2
static const struct PluginCodec_H323GenericCodecData G7222Capability =
{
  OpalPluginCodec_Identifer_G7222,      // capability identifier
};


/////////////////////////////////////////////////////////////////////////////

PLUGINCODEC_ONE_AUDIO_CODEC(
  G7222FormatName,
  G7222EncodingName,
  G7222Description,
  G7222_SAMPLE_RATE,
  G7222_MAX_BIT_RATE,
  G7222_SAMPLES_PER_FRAME,
  1, 1, 0, 0,
  PluginCodec_H323Codec_generic, &G7222Capability
);

extern "C"
{
  PLUGIN_CODEC_IMPLEMENT_ALL(G7222, CodecDefinitionTable, PLUGIN_CODEC_VERSION_OPTIONS)
};

/////////////////////////////////////////////////////////////////////////////
