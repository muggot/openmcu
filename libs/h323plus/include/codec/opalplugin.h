/*
 * h323plugins.h
 *
 * H.323 codec plugins handler
 *
 * Open H323 Library
 *
 * Copyright (C) 2004 Post Increment
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
 * $Log: opalplugin.h,v $
 * Revision 1.5  2007/11/06 17:45:38  shorne
 * Added h323pluslib definition
 *
 * Revision 1.4  2007/11/06 17:43:33  shorne
 * added i480 standard framesize
 *
 * Revision 1.3  2007/10/30 04:23:43  shorne
 * Corrections and Improvements for H.239 support
 *
 * Revision 1.2  2007/10/19 19:53:43  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.1  2007/08/20 20:19:53  shorne
 * Moved opalplugin.h to codec directory to be plugin compile compatible with Opal
 *
 * Revision 1.2  2007/08/20 19:13:28  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.10.2.5  2007/09/26 05:14:28  rjongbloed
 * Added some extra RTP magic numbers: min header size, max packet size etc
 *
 * Revision 1.10.2.4  2007/08/17 08:38:22  rjongbloed
 * Back ported OPAL meda options based plug ins and H.323 generic capabilties.
 *
 * Revision 1.10.2.3  2007/02/19 20:12:45  shorne
 * added H.239 support
 *
 * Revision 1.10.2.2  2007/02/19 14:19:36  shorne
 * Added H.239 OIDs
 *
 * Revision 1.10.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.10  2006/05/16 11:26:06  shorne
 * Added more hid key input mask types
 *
 * Revision 1.9  2005/11/21 21:04:10  shorne
 * Added more HID input switches
 *
 * Revision 1.8  2005/08/23 08:13:06  shorne
 * Added HID plugin volume & LCD display support
 *
 * Revision 1.7  2005/07/03 13:54:23  shorne
 * Added Initial LID Plugin Support
 *
 * Revision 1.6  2005/06/07 03:22:22  csoutheren
 * Added patch 1198741 with support for plugin codecs with generic capabilities
 * Added patch 1198754 with support for setting quality level on audio codecs
 * Added patch 1198760 with GSM-AMR codec support
 * Many thanks to Richard van der Hoff for his work
 *
 * Revision 1.5  2004/12/20 23:30:20  csoutheren
 * Added plugin support for packet loss concealment frames
 *
 * Revision 1.4  2004/11/29 06:30:53  csoutheren
 * Added support for wideband codecs
 *
 * Revision 1.3  2004/05/18 22:26:28  csoutheren
 * Initial support for embedded codecs
 * Fixed problems with streamed codec support
 * Updates for abstract factory loading methods
 *
 * Revision 1.2  2004/05/09 14:44:36  csoutheren
 * Added support for streamed plugin audio codecs
 *
 * Revision 1.1  2004/04/09 12:25:25  csoutheren
 * Renamed from h323plugin.h
 *
 * Revision 1.2  2004/04/03 10:38:24  csoutheren
 * Added in initial cut at codec plugin code. Branches are for wimps :)
 *
 * Revision 1.1.2.1  2004/03/31 11:03:16  csoutheren
 * Initial public version
 *
 * Revision 1.8  2004/02/23 13:17:32  craigs
 * Fixed problems with codec interface functions
 *
 * Revision 1.7  2004/02/23 13:04:09  craigs
 * Removed warnings when compliing plugins
 *
 * Revision 1.6  2004/01/27 14:55:46  craigs
 * Implemented static linking of new codecs
 *
 * Revision 1.5  2004/01/23 05:21:15  craigs
 * Updated for changes to the codec plugin interface
 *
 * Revision 1.4  2004/01/09 11:27:46  craigs
 * Plugin codec audio now works :)
 *
 * Revision 1.3  2004/01/09 07:32:22  craigs
 * More fixes for capability problems
 *
 * Revision 1.2  2004/01/06 07:05:03  craigs
 * Changed to support plugin codecs
 *
 * Revision 1.1  2004/01/04 13:37:51  craigs
 * Implementation of codec plugins
 *
 *
 */

#ifndef __OPAL_CODECPLUGIN_H
#define __OPAL_CODECPLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

// indicator the this is h323plus version of the plugin codec
// Most unfortunate!
#define h323pluslib    1  

#ifdef _MSC_VER
#pragma warning(disable:4201)
#endif

#ifdef _WIN32
#  ifdef PLUGIN_CODEC_DLL_EXPORTS
#    define PLUGIN_CODEC_DLL_API __declspec(dllexport)
#  else
#    define PLUGIN_CODEC_DLL_API __declspec(dllimport)
#  endif

#else

#define PLUGIN_CODEC_DLL_API

#endif

#ifdef PWLIB_PLUGIN_API_VERSION
#undef PWLIB_PLUGIN_API_VERSION
#endif
#define PWLIB_PLUGIN_API_VERSION 1

#define	PLUGIN_CODEC_VERSION            1    // initial version
#define	PLUGIN_CODEC_VERSION_WIDEBAND   2    // added wideband
#define	PLUGIN_CODEC_VERSION_VIDEO      3    // added video
#define PLUGIN_CODEC_VERSION_FAX        4    // added fax
#define PLUGIN_CODEC_VERSION_OPTIONS    5    // added options handling

#define PLUGIN_CODEC_API_VER_FN       PWLibPlugin_GetAPIVersion
#define PLUGIN_CODEC_API_VER_FN_STR   "PWLibPlugin_GetAPIVersion"

#define PLUGIN_CODEC_GET_CODEC_FN     OpalCodecPlugin_GetCodecs
#define PLUGIN_CODEC_GET_CODEC_FN_STR "OpalCodecPlugin_GetCodecs"

#define PLUGIN_CODEC_API_VER_FN_DECLARE \
PLUGIN_CODEC_DLL_API unsigned int PLUGIN_CODEC_API_VER_FN() \
{ return PWLIB_PLUGIN_API_VERSION; }

enum {
  PluginCodec_License_None                           = 0,
  PluginCodec_Licence_None = PluginCodec_License_None,        // allow for old code with misspelled constant
  PluginCodec_License_GPL                            = 1,
  PluginCodec_License_MPL                            = 2,
  PluginCodec_License_Freeware                       = 3,
  PluginCodec_License_ResearchAndDevelopmentUseOnly  = 4,
  PluginCodec_License_BSD                            = 5,
  PluginCodec_License_LGPL                           = 6,

  PluginCodec_License_NoRoyalties                    = 0x7f,

  // any license codes above here require royalty payments
  PluginCodec_License_RoyaltiesRequired              = 0x80
};

struct PluginCodec_information {
  // start of version 1 fields
  time_t timestamp;                     // codec creation time and date - obtain with command: date -u "+%c = %s"

  const char * sourceAuthor;            // source code author
  const char * sourceVersion;           // source code version
  const char * sourceEmail;             // source code email contact information
  const char * sourceURL;               // source code web site
  const char * sourceCopyright;         // source code copyright
  const char * sourceLicense;           // source code license
  unsigned char sourceLicenseCode;      // source code license

  const char * codecDescription;        // codec description
  const char * codecAuthor;             // codec author
  const char * codecVersion;            // codec version
  const char * codecEmail;              // codec email contact information
  const char * codecURL;                // codec web site
  const char * codecCopyright;          // codec copyright information
  const char * codecLicense;            // codec license
  unsigned short codecLicenseCode;      // codec license code
  // end of version 1 fields

};

enum PluginCodec_Flags {
  PluginCodec_MediaTypeMask          = 0x000f,
  PluginCodec_MediaTypeAudio         = 0x0000,
  PluginCodec_MediaTypeVideo         = 0x0001,
  PluginCodec_MediaTypeAudioStreamed = 0x0002,
  PluginCodec_MediaTypeFax           = 0x0003,

  PluginCodec_InputTypeMask          = 0x0010,
  PluginCodec_InputTypeRaw           = 0x0000,
  PluginCodec_InputTypeRTP           = 0x0010,

  PluginCodec_OutputTypeMask         = 0x0020,
  PluginCodec_OutputTypeRaw          = 0x0000,
  PluginCodec_OutputTypeRTP          = 0x0020,

  PluginCodec_RTPTypeMask            = 0x0040,
  PluginCodec_RTPTypeDynamic         = 0x0000,
  PluginCodec_RTPTypeExplicit        = 0x0040,

  PluginCodec_RTPSharedMask          = 0x0080,
  PluginCodec_RTPTypeNotShared       = 0x0000,
  PluginCodec_RTPTypeShared          = 0x0080,

  PluginCodec_DecodeSilenceMask      = 0x0100,
  PluginCodec_NoDecodeSilence        = 0x0000,
  PluginCodec_DecodeSilence          = 0x0100,

  PluginCodec_EncodeSilenceMask      = 0x0200,
  PluginCodec_NoEncodeSilence        = 0x0000,
  PluginCodec_EncodeSilence          = 0x0200,

  PluginCodec_MediaExtensionMask     = 0x0400,
  PluginCodec_MediaTypeExtVideo      = 0x0400,

  PluginCodec_BitsPerSamplePos       = 12,
  PluginCodec_BitsPerSampleMask      = 0xf000,
};

enum PluginCodec_CoderFlags {
  PluginCodec_CoderSilenceFrame      = 1,    // request audio codec to create silence frame
  PluginCodec_CoderForceIFrame       = 2     // request video codec to force I frame
};

enum PluginCodec_ReturnCoderFlags {
  PluginCodec_ReturnCoderLastFrame     = 1,    // indicates when video codec returns last data for frame
  PluginCodec_ReturnCoderIFrame        = 2,    // indicates when video returns I frame
  PluginCodec_ReturnCoderRequestIFrame = 4     // indicates when video decoder request I frame for resync
};

struct PluginCodec_Definition;

// Control function names

#define PLUGINCODEC_CONTROL_VALID_FOR_PROTOCOL    "valid_for_protocol"
#define PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS     "get_codec_options"
#define PLUGINCODEC_CONTROL_FREE_CODEC_OPTIONS    "free_codec_options"
#define PLUGINCODEC_CONTROL_GET_OUTPUT_DATA_SIZE  "get_output_data_size"
#define PLUGINCODEC_CONTROL_SET_CODEC_OPTIONS     "set_codec_options"
#define PLUGINCODEC_CONTROL_GET_ACTIVE_OPTIONS    "get_active_options"
#define PLUGINCODEC_CONTROL_TO_NORMALISED_OPTIONS "to_normalised_options"
#define PLUGINCODEC_CONTROL_TO_CUSTOMISED_OPTIONS "to_customised_options"
#define PLUGINCODEC_CONTROL_SET_INSTANCE_ID       "set_instance_id"
#define PLUGINCODEC_CONTROL_SET_LOG_FUNCTION      "set_log_function"
#define PLUGINCODEC_CONTROL_GET_STATISTICS        "get_statistics"
#define PLUGINCODEC_CONTROL_TERMINATE_CODEC       "terminate_codec"


/* Log function, plug in gets a pointer to this function which allows
   it to use the standard OPAL logging system. The function returns 0 if
   no logging was performed due to the log level. Note if log == NULL
   then this return state is all that happens, so this may be executed
   first to prevent lengthy logs that would not result in any output. */
typedef int (*PluginCodec_LogFunction)(unsigned level,
                                       const char * file,
                                       unsigned line,
                                       const char * section,
                                       const char * log);


struct PluginCodec_ControlDefn {
  const char * name;
  int (*control)(const struct PluginCodec_Definition * codec, void * context, 
                 const char * name, void * parm, unsigned * parmLen);
 
};

enum PluginCodec_OptionTypes {
  PluginCodec_StringOption,
  PluginCodec_BoolOption,
  PluginCodec_IntegerOption,
  PluginCodec_RealOption,
  PluginCodec_EnumOption,
  PluginCodec_OctetsOption,
  PluginCodec_NumOptionTypes,
};

enum PluginCodec_OptionMerge {
  PluginCodec_NoMerge,
  PluginCodec_MinMerge,
  PluginCodec_MaxMerge,
  PluginCodec_EqualMerge,
  PluginCodec_NotEqualMerge,
  PluginCodec_AlwaysMerge,
  PluginCodec_NumOptionMerge,

  PluginCodec_AndMerge = PluginCodec_MaxMerge,
  PluginCodec_OrMerge  = PluginCodec_MinMerge,
  PluginCodec_XorMerge = PluginCodec_NotEqualMerge,
  PluginCodec_NotXorMerge = PluginCodec_EqualMerge
};

#define PluginCodec_H245_Collapsing    0x40000000
#define PluginCodec_H245_NonCollapsing 0x20000000
#define PluginCodec_H245_Unsigned32    0x10000000
#define PluginCodec_H245_BooleanArray  0x08000000
#define PluginCodec_H245_TCS           0x04000000
#define PluginCodec_H245_OLC           0x02000000
#define PluginCodec_H245_ReqMode       0x01000000
#define PluginCodec_H245_OrdinalMask   0x0000ffff

struct PluginCodec_Option {
  // start of version 4 fields
  enum PluginCodec_OptionTypes m_type;
  const char *                 m_name;
  unsigned                     m_readOnly;
  enum PluginCodec_OptionMerge m_merge;
  const char *                 m_value;
  const char *                 m_FMTPName;
  const char *                 m_FMTPDefault;
  int                          m_H245Generic;
  const char *                 m_minimum;
  const char *                 m_maximum;
};


struct PluginCodec_Definition {
  unsigned int version;			               // codec structure version

  // start of version 1 fields
  struct PluginCodec_information * info;   // license information

  unsigned int flags;                      // b0-3: 0 = audio,        1 = video
                                           // b4:   0 = raw input,    1 = RTP input
                                           // b5:   0 = raw output,   1 = RTP output
                                           // b6:   0 = dynamic RTP,  1 = explicit RTP
                                           // b7:   0 = no share RTP, 1 = share RTP

  const char * descr;    		               // text decription

  const char * sourceFormat;               // source format
  const char * destFormat;                 // destination format

  const void * userData;                   // user data value

  unsigned int sampleRate;                 // samples per second
  unsigned int bitsPerSec;     		       // raw bits per second
  unsigned int usPerFrame;                 // microseconds per frame

  union _parm {
    struct _audio {
      unsigned int samplesPerFrame;            // audio: samples per frame
      unsigned int bytesPerFrame;              // audio: max bytes per frame
      unsigned int recommendedFramesPerPacket; // audio: recommended number of frames per packet
      unsigned int maxFramesPerPacket;         // audio: maximum number of frames per packet
    } audio;
    struct _video {
      unsigned int maxFrameWidth;              // video: frame width
      unsigned int maxFrameHeight;             // video: frame height
      unsigned int recommendedFrameRate;       // video: recommended frame rate
      unsigned int maxFrameRate;               // video: max frame rate
    } video;
  } parm;

  unsigned char rtpPayload;    		         // IANA RTP payload code (if defined)
  const char * sdpFormat;                  // SDP format string (or NULL, if no SDP format)

  void * (*createCodec)(const struct PluginCodec_Definition * codec);	                  // create codec 
  void (*destroyCodec) (const struct PluginCodec_Definition * codec,  void * context); 	// destroy codec
  int (*codecFunction) (const struct PluginCodec_Definition * codec,  void * context,   // do codec function
                                  const void * from, unsigned * fromLen,
                                        void * to,   unsigned * toLen,
                                        unsigned int * flag);
  struct PluginCodec_ControlDefn * codecControls;
 
  // H323 specific fields
  unsigned char h323CapabilityType;
  const void  * h323CapabilityData;

  // end of version 1 fields

  // NOTE!!!!! Due to an error in judgement, you cannot add ANY more fields
  // to this structure without an API version change!!!!
};

typedef struct PluginCodec_Definition * (* PluginCodec_GetCodecFunction)(unsigned int *, unsigned int);
typedef unsigned (* PluginCodec_GetAPIVersionFunction)();

///////////////////////////////////////////////////////////////////
//
//  H.323 specific values
//


struct PluginCodec_H323CapabilityExtension {
  unsigned int index;
  void * data;
  unsigned dataLength;
};

struct PluginCodec_H323NonStandardCodecData {
  const char * objectId;
  unsigned char  t35CountryCode;
  unsigned char  t35Extension;
  unsigned short manufacturerCode;
  const unsigned char * data;
  unsigned int dataLength;
  int (*capabilityMatchFunction)(struct PluginCodec_H323NonStandardCodecData *);
};


struct PluginCodec_H323GenericParameterDefinition
{
  /* The following used to be a simple integer for the collapsing flag in
     version 3 and earlier. We hope that all those implementations just used
     zero and one (a good bet) and thus the below bit fields will be backward
     compatible, putting the parameter in all three PDU types.
   */ 
  struct {
    int collapsing:1; /* boolean */
    int excludeTCS:1;
    int excludeOLC:1;
    int excludeReqMode:1;
    int readOnly:1;
  };

    unsigned int id;

    enum PluginCodec_H323GenericParameterType {
	/* these need to be in the same order as the choices in
	   H245_ParameterValue::Choices, as the value is just cast to that type
	*/
	PluginCodec_GenericParameter_Logical = 0,
	PluginCodec_GenericParameter_BooleanArray,
	PluginCodec_GenericParameter_UnsignedMin,
	PluginCodec_GenericParameter_UnsignedMax,
	PluginCodec_GenericParameter_Unsigned32Min,
	PluginCodec_GenericParameter_Unsigned32Max,
	PluginCodec_GenericParameter_OctetString,
	PluginCodec_GenericParameter_GenericParameter,
	
    PluginCodec_GenericParameter_logical = 0,
    PluginCodec_GenericParameter_booleanArray,
    PluginCodec_GenericParameter_unsignedMin,
    PluginCodec_GenericParameter_unsignedMax,
    PluginCodec_GenericParameter_unsigned32Min,
    PluginCodec_GenericParameter_unsigned32Max,
    PluginCodec_GenericParameter_octetString,
    PluginCodec_GenericParameter_genericParameter
    } type;

    union {
	unsigned long integer;
    const char * octetstring;
	struct PluginCodec_H323GenericParameterDefinition *genericparameter;
    } value;
};

struct PluginCodec_H323GenericCodecData
{
    // some cunning structures & lists, and associated logic in 
    // H323CodecPluginGenericAudioCapability::H323CodecPluginGenericAudioCapability()
    const char * standardIdentifier;
    unsigned int maxBitRate; // Zero means use value from OpalMediaFormat

    /* parameters; these are the parameters which are set in the
       'TerminalCapabilitySet' and 'OpenLogicalChannel' requests */
    unsigned int nParameters;
    /* an array of nParameters parameter definitions */
    const struct PluginCodec_H323GenericParameterDefinition *params; 
};
    

struct PluginCodec_H323AudioGSMData {
  int comfortNoise:1;
  int scrambled:1;
};

struct  PluginCodec_H323AudioG7231AnnexC {
  unsigned char maxAl_sduAudioFrames;
  int silenceSuppression:1;
  int highRateMode0:6;  	      // INTEGER (27..78),	-- units octets
  int	highRateMode1:6;	        // INTEGER (27..78),	-- units octets
  int	lowRateMode0:6;		        // INTEGER (23..66),	-- units octets
  int	lowRateMode1:6;		        // INTEGER (23..66),	-- units octets
  int	sidMode0:4;		            // INTEGER (6..17),	-- units octets
  int	sidMode1:4;		            // INTEGER (6..17),	-- units octets
};


enum {
  PluginCodec_H323Codec_undefined,			// must be zero, so empty struct is undefined
  PluginCodec_H323Codec_programmed,			// H323ProgrammedCapability
  PluginCodec_H323Codec_nonStandard,		// H323NonStandardData
  PluginCodec_H323Codec_generic,            // H323GenericCodecData

  // audio codecs
  PluginCodec_H323AudioCodec_g711Alaw_64k,		    // int
  PluginCodec_H323AudioCodec_g711Alaw_56k,		    // int
  PluginCodec_H323AudioCodec_g711Ulaw_64k,		    // int
  PluginCodec_H323AudioCodec_g711Ulaw_56k,		    // int
  PluginCodec_H323AudioCodec_g722_64k,			      // int
  PluginCodec_H323AudioCodec_g722_56k,			      // int
  PluginCodec_H323AudioCodec_g722_48k,			      // int
  PluginCodec_H323AudioCodec_g7231,			          // H323AudioG7231Data
  PluginCodec_H323AudioCodec_g728,			          // int
  PluginCodec_H323AudioCodec_g729,			          // int
  PluginCodec_H323AudioCodec_g729AnnexA,		      // int
  PluginCodec_H323AudioCodec_is11172,             // not yet implemented
  PluginCodec_H323AudioCodec_is13818Audio,        // not yet implemented
  PluginCodec_H323AudioCodec_g729wAnnexB,		      // int
  PluginCodec_H323AudioCodec_g729AnnexAwAnnexB,	  // int
  PluginCodec_H323AudioCodec_g7231AnnexC,         // H323AudioG7231AnnexC
  PluginCodec_H323AudioCodec_gsmFullRate,		      // H323AudioGSMData
  PluginCodec_H323AudioCodec_gsmHalfRate,		      // H323AudioGSMData
  PluginCodec_H323AudioCodec_gsmEnhancedFullRate,	// H323AudioGSMData
  PluginCodec_H323AudioCodec_g729Extensions,      // not yet implemented

  // video codecs
  PluginCodec_H323VideoCodec_h261,                // implemented 
  PluginCodec_H323VideoCodec_h262,                // not yet implemented
  PluginCodec_H323VideoCodec_h263,                // implemented
  PluginCodec_H323VideoCodec_is11172,             // not yet implemented

  // other capabilities
  PluginCodec_H323VideoCodec_Extended,            // implemented (for use with H.239)
  PluginCodec_H323T38Codec,                       // not yet implemented

  // special codes
  PluginCodec_H323Codec_NoH323 = 0xff,            // used for SIP-only codecs
};

/////////////////
//
// Generic Codec Standard Identifiers
//

// Audio Capabilities
// AMR (as defined in H.245v13 Annex I)
#define OpalPluginCodec_Identifer_AMR             "0.0.8.245.1.1.1"

// AMR-NB\WB  (as defined in H.245v13 Annex R)
#define OpalPluginCodec_Identifer_AMR_NB          "0.0.8.245.1.1.9"
#define OpalPluginCodec_Identifer_AMR_WB          "0.0.8.245.1.1.10"

// G.722.2 (aka AMR-WB)
#define OpalPluginCodec_Identifer_G7222           "0.0.7.7222.1.0"

// iLBC (as defined in H.245v13 Annex S)
#define OpalPluginCodec_Identifer_iLBC            "0.0.8.245.1.1.11"


// Video Capabilities

// H264 (as defined in H.241)
#define OpalPluginCodec_Identifer_H264            "0.0.8.241.0.0.0.0"
#define OpalPluginCodec_Identifer_H264_RFC3984    "0.0.8.241.0.0.0.1"
#define OpalPluginCodec_Identifer_H264_RFC3984Int "0.0.8.241.0.0.0.2"
#define OpalPluginCodec_Identifer_H264_Generic    "0.0.8.241.0.0.1"

// ISO/IEC 14496-2 MPEG4 part 2 (as defined in H.245v13 Annex E)
#define OpalPluginCodec_Identifer_MPEG4           "0.0.8.245.1.0.0"

// Extended Video Capability (as defined in H.239)
#define OpalPluginCodec_Identifer_H239            "0.0.8.239.1.1"
#define OpalPluginCodec_Identifer_H239_Video      "0.0.8.239.1.2"
#define OpalPluginCodec_Identifer_H239_Generic    "0.0.8.239.2"

/////////////////
//
// RTP specific definitions
//

#define PluginCodec_RTP_MaxPacketSize  (1518-14-4-8-20-16)  // Max Ethernet packet (1518 bytes) minus 802.3/CRC, 802.3, IP, UDP headers
#define PluginCodec_RTP_MinHeaderSize  (12)
#define PluginCodec_RTP_MaxPayloadSize (PluginCodec_RTP_MaxPacketSize - PluginCodec_RTP_MinHeaderSize)

#define PluginCodec_RTP_GetHeaderLength(ptr)      ((((BYTE*)(ptr))[0] & 0x0f)*4 + PluginCodec_RTP_MinHeaderSize)
#define PluginCodec_RTP_GetPayloadPtr(ptr)          ((BYTE*)(ptr) + PluginCodec_RTP_GetHeaderLength(ptr))
#define PluginCodec_RTP_GetPayloadType(ptr)        (((BYTE*)(ptr))[1] & 0x7f)
#define PluginCodec_RTP_SetPayloadType(ptr, type)  (((BYTE*)(ptr))[1] = (((BYTE*)(ptr))[1] & 0x80) | (type & 0x7f))
#define PluginCodec_RTP_GetMarker(ptr)            ((((BYTE*)(ptr))[1] & 0x80) != 0)
#define PluginCodec_RTP_SetMarker(ptr, mark)       (((BYTE*)(ptr))[1] = (((BYTE*)(ptr))[1] & 0x7f) | (mark != 0 ? 0x80 : 0))
#define PluginCodec_RTP_GetTimestamp(ptr)         ((((BYTE*)(ptr))[4] << 24) | (((BYTE*)(ptr))[5] << 16) | (((BYTE*)(ptr))[6] << 8) | ((BYTE*)(ptr))[7])
#define PluginCodec_RTP_SetTimestamp(ptr, ts)     ((((BYTE*)(ptr))[4] = ((ts) >> 24)),(((BYTE*)(ptr))[5] = ((ts) >> 16)),(((BYTE*)(ptr))[6] = ((ts) >> 8)),(((BYTE*)(ptr))[7] = (ts)))


/////////////////
//
// video specific definitions
//

struct PluginCodec_Video_FrameHeader {
  unsigned int  x;
  unsigned int  y;
  unsigned int  width;
  unsigned int  height;
};

#ifdef __cplusplus
};

inline unsigned char * OPAL_VIDEO_FRAME_DATA_PTR(struct PluginCodec_Video_FrameHeader * base)
{ return (((unsigned char *)base) + sizeof(PluginCodec_Video_FrameHeader)); }

inline unsigned char * OPAL_VIDEO_FRAME_DATA_PTR(const PluginCodec_Video_FrameHeader * base)
{ return (((unsigned char *)base) + sizeof(PluginCodec_Video_FrameHeader)); }

extern "C" {
#endif

#define PLUGIN_CODEC_VIDEO_SET_FRAME_SIZE_FN    "set_frame_size"    // argument is struct PluginCodec_VideoSetFrameInfo
struct PluginCodec_Video_SetFrameInfo {
  int width;
  int height;
};


/////////////////
//
// experimental definitions for statically linking codecs
//

#ifdef OPAL_STATIC_CODEC

#  undef PLUGIN_CODEC_DLL_API
#  define PLUGIN_CODEC_DLL_API static
#  define PLUGIN_CODEC_IMPLEMENT(name) \
unsigned int Opal_StaticCodec_##name##_GetAPIVersion() \
{ return PWLIB_PLUGIN_API_VERSION; } \
static struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned /*version*/); \
struct PluginCodec_Definition * Opal_StaticCodec_##name##_GetCodecs(unsigned * p1, unsigned p2) \
{ return PLUGIN_CODEC_GET_CODEC_FN(p1,p2); } \

#else

#  define PLUGIN_CODEC_IMPLEMENT(name) \
PLUGIN_CODEC_DLL_API unsigned int PLUGIN_CODEC_API_VER_FN() \
{ return PWLIB_PLUGIN_API_VERSION; } \

#  define PLUGIN_CODEC_IMPLEMENT_ALL(name, table, ver) \
PLUGIN_CODEC_IMPLEMENT(name) \
PLUGIN_CODEC_DLL_API struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned version) \
{ *count = sizeof(table)/sizeof(struct PluginCodec_Definition); return version < ver ? NULL : table; }


#endif

#ifdef __cplusplus
};
#endif


#ifdef _MSC_VER
#pragma warning(default:4201)
#endif

#endif // __OPAL_CODECPLUGIN_H
