/*
 * h323pluginmgr.cxx
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
 * $Log: h323pluginmgr.cxx,v $
 * Revision 1.14  2008/02/12 07:59:31  shorne
 * fix for the correct SetMaxBitRate being called
 *
 * Revision 1.13  2008/02/10 23:11:33  shorne
 * Fix to compile H323plus without Video
 *
 * Revision 1.12  2007/11/28 06:03:37  shorne
 * Video capability merge. Thx again Jan Willamowius
 *
 * Revision 1.11  2007/11/23 04:29:30  shorne
 * small correction with video codec definitions reading. Only a naming map issue.
 *
 * Revision 1.10  2007/11/22 22:37:09  willamowius
 * fix from H.263 frame size negotiation
 *
 * Revision 1.9  2007/11/14 19:34:51  willamowius
 * use dynamic RTP frame allocation, static allocation had a memory leak of 2 frames per call
 *
 * Revision 1.8  2007/11/06 17:43:36  shorne
 * added i480 standard framesize
 *
 * Revision 1.7  2007/11/01 20:17:33  shorne
 * updates for H.239 support
 *
 * Revision 1.6  2007/10/30 04:23:45  shorne
 * Corrections and Improvements for H.239 support
 *
 * Revision 1.5  2007/10/25 21:08:04  shorne
 * Added support for HD Video devices
 *
 * Revision 1.4  2007/10/19 19:54:17  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.3  2007/08/20 20:19:52  shorne
 * Moved opalplugin.h to codec directory to be plugin compile compatible with Opal
 *
 * Revision 1.2  2007/08/20 19:13:29  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.60.2.26  2007/10/03 04:22:24  rjongbloed
 * Added transmission of VideoFastUpdate request on decoder error.
 *
 * Revision 1.60.2.25  2007/09/26 05:16:05  rjongbloed
 * Fixed correct size of buffers for plug in.
 *
 * Revision 1.60.2.24  2007/09/11 07:14:05  rjongbloed
 * Added log for when I-Frame is sent in response to VFU
 *
 * Revision 1.60.2.23  2007/09/07 08:53:55  rjongbloed
 * Fixed passing back adjusted media options to codec after H.245 negotiations.
 * Fixed telling plug in that a new I-Frame is required in response to VideoFastUpdate.
 *
 * Revision 1.60.2.22  2007/09/03 09:45:47  rjongbloed
 * Fixed failure to propagate meda format options to codec.
 *
 * Revision 1.60.2.21  2007/09/01 05:25:27  rjongbloed
 * Fixed video RTP timestamp reference point
 *
 * Revision 1.60.2.20  2007/08/31 09:29:56  rjongbloed
 * Back ported OPAL handling of H.261/H.263 xCIF MPI values.
 * Fixed missing initialisation of video frame size header when passing to encoder.
 *
 * Revision 1.60.2.19  2007/08/30 12:44:18  rjongbloed
 * Fixed correct setting of RTP timestamp for video codecs.
 *
 * Revision 1.60.2.18  2007/08/17 08:38:23  rjongbloed
 * Back ported OPAL meda options based plug ins and H.323 generic capabilties.
 *
 * Revision 1.60.2.17  2007/02/19 20:11:06  shorne
 * Added Baseline H.239 Support
 *
 * Revision 1.60.2.16  2007/02/17 15:32:54  shorne
 * Altered the buffer RTP_DataFrame memory
 *  space to be statically allocated on H323PluginVideoCodec constructor to ensure memory
 *  is not deleted prior to the class destructor which causes a segfault.
 *
 * Revision 1.60.2.14  2007/02/11 00:43:48  shorne
 * More code tidy-ups
 *
 * Revision 1.60.2.13  2007/02/05 05:17:00  shorne
 * Added ability to send general codec options to Video Plugins
 *
 * Revision 1.60.2.12  2007/02/04 08:36:48  shorne
 * CallCodecControl fixes affected by previous tidy up patch
 *
 * Revision 1.60.2.11  2007/02/03 06:15:50  shorne
 * Fix to ensure options are loaded for all Video types
 *
 * Revision 1.60.2.10  2007/02/03 05:49:52  shorne
 * Extended set FrameSize to generic video plugins
 *
 * Revision 1.60.2.9  2007/02/02 22:12:44  shorne
 * Added ability to set FrameSize for video plugins
 *
 * Revision 1.60.2.8  2007/02/02 08:23:54  shorne
 * Tidy up to video rendering and SetFrameSize
 *
 * Revision 1.60.2.7  2007/01/31 14:05:19  shorne
 * fix for crash on Linux when loading audio codecs
 *
 * Revision 1.60.2.6  2007/01/30 20:25:07  shorne
 * Fixed merge error
 *
 * Revision 1.60.2.5  2007/01/30 18:14:09  shorne
 * Fix for Linux compile
 *
 * Revision 1.60.2.4  2007/01/30 14:39:55  shorne
 * Corrections and Improvements patch supplied by Guilhem Tardy
 *
 * Revision 1.60.2.3  2007/01/17 06:17:08  shorne
 * Fix for partial frame decoding thx Guilhem Tardy
 *
 * Revision 1.60.2.2  2006/12/24 05:25:04  shorne
 * small fix to avoid copying memory onto itself
 *
 * Revision 1.60.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.59  2006/07/18 07:24:27  csoutheren
 * Fix G.723.1 capabilty comparison
 *
 * Revision 1.58  2005/08/05 17:11:03  csoutheren
 * Fixed gcc 4.0.1 warning
 *
 * Revision 1.57  2005/06/27 00:31:19  csoutheren
 * Fixed problem with uninitialised variable when using codec-free capability constructor
 * Fixed mismatch with G.711 codec capability table for plugins
 * Both fixes thanks to Benni Badham
 *
 * Revision 1.56  2005/06/21 06:46:35  csoutheren
 * Add ability to create capabilities without codecs for external RTP interface
 *
 * Revision 1.55  2005/06/07 03:22:24  csoutheren
 * Added patch 1198741 with support for plugin codecs with generic capabilities
 * Added patch 1198754 with support for setting quality level on audio codecs
 * Added patch 1198760 with GSM-AMR codec support
 * Many thanks to Richard van der Hoff for his work
 *
 * Revision 1.54  2005/05/03 12:23:05  csoutheren
 * Unlock connection list when creating connection
 * Remove chance of race condition with H.245 negotiation timer
 * Unregister OpalPluginMediaFormat when instance is destroyed
 * Thank to Paul Cadach
 *
 * Revision 1.53  2005/04/28 03:22:17  csoutheren
 * Fixed problem with toLen not being set n factory-based G.711 routines
 * Thanks to Derek Smithies
 *
 * Revision 1.52  2005/01/04 12:20:12  csoutheren
 * Fixed more problems with global statics
 *
 * Revision 1.51  2005/01/04 08:08:46  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.50  2005/01/03 14:03:42  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.49  2005/01/03 06:26:09  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.48  2005/01/03 02:53:22  csoutheren
 * Fixed problem compiling without OpenSSL
 *
 * Revision 1.47  2004/12/20 23:30:21  csoutheren
 * Added plugin support for packet loss concealment frames
 *
 * Revision 1.46  2004/12/08 02:03:59  csoutheren
 * Fixed problem with detection of non-FFH.263
 *
 * Revision 1.45  2004/11/29 06:30:54  csoutheren
 * Added support for wideband codecs
 *
 * Revision 1.44  2004/11/20 22:00:50  csoutheren
 * Added hacks for linker problem
 *
 * Revision 1.43  2004/11/12 06:04:45  csoutheren
 * Changed H235Authentiators to use PFactory
 *
 * Revision 1.42  2004/09/03 08:06:42  csoutheren
 * G.711 codecs are not singleton
 *
 * Revision 1.41  2004/09/03 07:13:47  csoutheren
 * Fixed typo returning wrong value for RTP payload code in factory codecs for G.711
 *
 * Revision 1.40  2004/08/26 11:04:33  csoutheren
 * Fixed factory bootstrap on Linux
 *
 * Revision 1.39  2004/08/26 08:41:33  csoutheren
 * Fixed problem compiling on Linux
 *
 * Revision 1.38  2004/08/26 08:05:04  csoutheren
 * Codecs now appear in abstract factory system
 * Fixed Windows factory bootstrap system (again)
 *
 * Revision 1.37  2004/08/24 14:23:11  csoutheren
 * Fixed problem with plugin codecs using capability compare functions
 *
 * Revision 1.36  2004/08/09 11:11:33  csoutheren
 * Added stupid windows hack to force opalwavfile factories to register
 *
 * Revision 1.35  2004/08/02 23:56:15  csoutheren
 * Fixed problem when using --enable-embeddedgsm
 *
 * Revision 1.34  2004/07/07 08:04:56  csoutheren
 * Added video codecs to default codec list, but H.263 is only loaded if the .so/DLL is found
 *
 * Revision 1.33  2004/07/03 06:51:37  rjongbloed
 * Added PTRACE_PARAM() macro to fix warnings on parameters used in PTRACE
 *  macros only.
 *
 * Revision 1.32  2004/06/30 12:31:16  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.31  2004/06/16 04:00:00  csoutheren
 * Fixed problems with T35 information in plugin codecs
 *
 * Revision 1.30  2004/06/09 13:18:48  csoutheren
 * Fixed compile errors and warnings when --disable-video and --disable-audio
 * used, thanks to Paul Rolland
 *
 * Revision 1.29  2004/06/03 23:20:47  csoutheren
 * Fixed compile problem on some gcc variants
 *
 * Revision 1.28  2004/06/03 13:32:01  csoutheren
 * Renamed INSTANTIATE_FACTORY
 *
 * Revision 1.27  2004/06/03 12:48:35  csoutheren
 * Decomposed PFactory declarations to hopefully avoid problems with DLLs
 *
 * Revision 1.26  2004/06/01 10:29:45  csoutheren
 * Fixed problems on Linux
 *
 * Revision 1.25  2004/06/01 07:30:27  csoutheren
 * Removed accidental cut & paste in new code that removed capabilities
 *
 * Revision 1.24  2004/06/01 05:49:27  csoutheren
 * Added code to cleanup some allocated memory upon shutdown
 *
 * Revision 1.23  2004/05/26 23:44:36  csoutheren
 * Fixed problem with incorrect return value on streamed codec functions
 *
 * Revision 1.22  2004/05/23 12:49:20  rjongbloed
 * Tidied some of the OpalMediaFormat usage after abandoning some previous
 *   code due to MSVC6 compiler bug.
 *
 * Revision 1.21  2004/05/19 07:38:23  csoutheren
 * Changed OpalMediaFormat handling to use abstract factory method functions
 *
 * Revision 1.20  2004/05/18 23:12:24  csoutheren
 * Fixed problem with plugins for predefined formats not appearing in default media format list
 *
 * Revision 1.19  2004/05/18 22:26:28  csoutheren
 * Initial support for embedded codecs
 * Fixed problems with streamed codec support
 * Updates for abstract factory loading methods
 *
 * Revision 1.18  2004/05/18 06:02:30  csoutheren
 * Deferred plugin codec loading until after main has executed by using abstract factory classes
 *
 * Revision 1.17  2004/05/13 12:48:03  rjongbloed
 * Fixed correct usage of the subtle distinction between the capability
 *   name (with {sw} etc) and the media format name.
 *
 * Revision 1.16  2004/05/12 13:41:26  csoutheren
 * Added support for getting lists of media formats from plugin manager
 *
 * Revision 1.15  2004/05/09 14:44:36  csoutheren
 * Added support for streamed plugin audio codecs
 *
 * Revision 1.14  2004/05/09 10:08:36  csoutheren
 * Changed new DecodeFrame to return bytes decoded rather than samples decoded
 * Added support for new DecodeFrame to plugin manager
 *
 * Revision 1.13  2004/05/06 12:54:41  rjongbloed
 * Fixed Clone() functions in plug in capabilities so uses copy constructor and
 *   thus copies all fields and all ancestors fields.
 *   Thanks Gustavo García Bernardo Telefónica R&D
 *
 * Revision 1.12  2004/05/04 03:33:33  csoutheren
 * Added guards against comparing certain kinds of Capabilities
 *
 * Revision 1.11  2004/05/02 08:24:57  rjongbloed
 * Fixed loading of plug ins when multiple plug in class sets used. Especially H.323 codecs.
 *
 * Revision 1.10  2004/04/29 15:04:07  ykiryanov
 * Added #ifndef NO_H323_VIDEO around video codec code
 *
 * Revision 1.9  2004/04/22 22:35:00  csoutheren
 * Fixed mispelling of Guilhem Tardy - my apologies to him
 *
 * Revision 1.8  2004/04/22 14:22:21  csoutheren
 * Added RFC 2190 H.263 code as created by Guilhem Tardy and AliceStreet
 * Many thanks to them for their contributions.
 *
 * Revision 1.7  2004/04/14 08:14:41  csoutheren
 * Changed to use generic plugin manager
 *
 * Revision 1.6  2004/04/09 13:28:38  rjongbloed
 * Fixed conversion of plug ins from OpenH323 to OPAL naming convention.
 *
 * Revision 1.5  2004/04/05 13:33:48  csoutheren
 * Fixed typo in GSM capability creation
 *
 * Revision 1.4  2004/04/04 00:41:09  csoutheren
 * Fixed MSVC compile warning
 *
 * Revision 1.3  2004/04/03 12:17:07  csoutheren
 * Updated plugin changes for RTTI changes and added missing include
 *
 * Revision 1.2  2004/04/03 10:38:25  csoutheren
 * Added in initial cut at codec plugin code. Branches are for wimps :)
 *
 * Revision 1.1.2.1  2004/03/31 11:03:16  csoutheren
 * Initial public version
 *
 * Revision 1.15  2004/02/03 06:14:43  craigs
 * Fixed compile warnings under Linux
 *
 * Revision 1.14  2004/01/28 13:29:55  craigs
 * Fixed compile warning under Linux
 *
 * Revision 1.13  2004/01/27 14:55:46  craigs
 * Implemented static linking of new codecs
 *
 * Revision 1.12  2004/01/25 09:08:15  craigs
 * Removed compile warnings
 *
 * Revision 1.11  2004/01/25 04:38:59  craigs
 * Fixed lengths and other parameters
 *
 * Revision 1.10  2004/01/24 22:26:36  craigs
 * Fixed RTP payload problems
 *
 * Revision 1.9  2004/01/23 05:21:15  craigs
 * Updated for changes to the codec plugin interface
 *
 * Revision 1.8  2004/01/13 03:19:11  craigs
 * Fixed problems on Linux
 *
 * Revision 1.7  2004/01/09 11:27:46  craigs
 * Plugin codec audio now works :)
 *
 * Revision 1.6  2004/01/09 07:32:22  craigs
 * More fixes for capability problems
 *
 * Revision 1.5  2004/01/08 06:36:57  craigs
 * Added creation of media format
 *
 * Revision 1.4  2004/01/06 12:50:04  craigs
 * More plugin fixes
 *
 * Revision 1.3  2004/01/06 10:25:28  craigs
 * Implementation of codec plugins
 *
 * Revision 1.2  2004/01/06 07:05:03  craigs
 * Changed to support plugin codecs
 *
 * Revision 1.1  2004/01/04 13:38:25  craigs
 * Implementation of codec plugins
 *
 *
 */

#ifdef __GNUC__
#pragma implementation "h323pluginmgr.h"
#endif

#include <ptlib.h>
#include <ptlib/video.h>
#include <ptlib/vconvert.h>
#include <h323.h>
#include <h323pluginmgr.h>
#include <codec/opalplugin.h>
#include <opalwavfile.h>
#include <h323caps.h>
#include <h245.h>
#include <rtp.h>
#include <mediafmt.h>

#define H323CAP_TAG_PREFIX    "h323"
static const char GET_CODEC_OPTIONS_CONTROL[]    = "get_codec_options";
static const char FREE_CODEC_OPTIONS_CONTROL[]   = "free_codec_options";
static const char GET_OUTPUT_DATA_SIZE_CONTROL[] = "get_output_data_size";
static const char SET_CODEC_OPTIONS_CONTROL[]    = "set_codec_options";
static const char EVENT_CODEC_CONTROL[]          = "event_codec";

#ifdef H323_VIDEO

#define CIF_WIDTH         352
#define CIF_HEIGHT        288

#define CIF4_WIDTH        (CIF_WIDTH*2)
#define CIF4_HEIGHT       (CIF_HEIGHT*2)

#define CIF16_WIDTH       (CIF_WIDTH*4)
#define CIF16_HEIGHT      (CIF_HEIGHT*4)

#define QCIF_WIDTH        (CIF_WIDTH/2)
#define QCIF_HEIGHT       (CIF_HEIGHT/2)

#define SQCIF_WIDTH       128
#define SQCIF_HEIGHT      96



static const char * sqcifMPI_tag                          = "SQCIF MPI";
static const char * qcifMPI_tag                           = "QCIF MPI";
static const char * cifMPI_tag                            = "CIF MPI";
static const char * cif4MPI_tag                           = "CIF4 MPI";
static const char * cif16MPI_tag                          = "CIF16 MPI";
//static const char * i480MPI_tag                           = "I480 MPI";
//static const char * p720MPI_tag                           = "720P MPI";
static const char * i1080MPI_tag                          = "I1080 MPI";

// H.261 only
static const char * h323_stillImageTransmission_tag            = H323CAP_TAG_PREFIX "_stillImageTransmission";

// H.261/H.263/H.264 tags
static const char * h323_qcifMPI_tag                           = H323CAP_TAG_PREFIX "_qcifMPI";
static const char * h323_cifMPI_tag                            = H323CAP_TAG_PREFIX "_cifMPI";

// H.263/H.264 tags
static const char * h323_sqcifMPI_tag                          = H323CAP_TAG_PREFIX "_sqcifMPI";
static const char * h323_cif4MPI_tag                           = H323CAP_TAG_PREFIX "_cif4MPI";
static const char * h323_cif16MPI_tag                          = H323CAP_TAG_PREFIX "_cif16MPI";
//static const char * h323_slowSqcifMPI_tag                      = H323CAP_TAG_PREFIX "_slowSqcifMPI";
//static const char * h323_slowQcifMPI_tag                       = H323CAP_TAG_PREFIX "_slowQcifMPI";
//static const char * h323_slowCifMPI_tag                        = H323CAP_TAG_PREFIX "slowCifMPI";
//static const char * h323_slowCif4MPI_tag                       = H323CAP_TAG_PREFIX "_slowCif4MPI";
//static const char * h323_slowCif16MPI_tag                      = H323CAP_TAG_PREFIX "_slowCif16MPI";

// H.263 only
static const char * h323_temporalSpatialTradeOffCapability_tag = H323CAP_TAG_PREFIX "_temporalSpatialTradeOffCapability";
static const char * h323_unrestrictedVector_tag                = H323CAP_TAG_PREFIX "_unrestrictedVector";
static const char * h323_arithmeticCoding_tag                  = H323CAP_TAG_PREFIX "_arithmeticCoding";      
static const char * h323_advancedPrediction_tag                = H323CAP_TAG_PREFIX "_advancedPrediction";
static const char * h323_pbFrames_tag                          = H323CAP_TAG_PREFIX "_pbFrames";
static const char * h323_hrdB_tag                              = H323CAP_TAG_PREFIX "_hrdB";
static const char * h323_bppMaxKb_tag                          = H323CAP_TAG_PREFIX "_bppMaxKb";
static const char * h323_errorCompensation_tag                 = H323CAP_TAG_PREFIX "_errorCompensation";

inline static bool IsValidMPI(int mpi) {
  return (mpi > 0); // && (mpi < 5);
}

#endif // H323_VIDEO

class OpalPluginCodec : public OpalFactoryCodec {
  PCLASSINFO(OpalPluginCodec, PObject)
  public:
    OpalPluginCodec(PluginCodec_Definition * _codecDefn)
      : codecDefn(_codecDefn)
    { 
      if (codecDefn->createCodec == NULL)
        context = NULL;
      else
        context = (codecDefn->createCodec)(codecDefn);
    }

    ~OpalPluginCodec()
    {
      (codecDefn->destroyCodec)(codecDefn, context);
    }

    const struct PluginCodec_Definition * GetDefinition()
    { return codecDefn; }

    PString GetInputFormat() const
    { return codecDefn->sourceFormat; }

    PString GetOutputFormat() const
    { return codecDefn->destFormat; }

    int Encode(const void * from, unsigned * fromLen, void * to,   unsigned * toLen, unsigned int * flag)
    { return (*codecDefn->codecFunction)(codecDefn, context, from, fromLen, to, toLen, flag); }

    unsigned int GetSampleRate() const
    { return codecDefn->sampleRate; }

    unsigned int GetBitsPerSec() const
    { return codecDefn->bitsPerSec; }

    unsigned int GetFrameTime() const
    { return codecDefn->usPerFrame; }

    unsigned int GetSamplesPerFrame() const
    { return codecDefn->parm.audio.samplesPerFrame; }

    unsigned int GetBytesPerFrame() const
    { return codecDefn->parm.audio.bytesPerFrame; }

    unsigned int GetRecommendedFramesPerPacket() const
    { return codecDefn->parm.audio.recommendedFramesPerPacket; }

    unsigned int GetMaxFramesPerPacket() const
    { return codecDefn->parm.audio.maxFramesPerPacket; }

    BYTE GetRTPPayload() const
    { return (BYTE)codecDefn->rtpPayload; }

    PString GetSDPFormat() const 
    { return codecDefn->sampleRate; }

  protected:
    PluginCodec_Definition * codecDefn;
    void * context;
};

class OpalPluginCodecFactory : public PFactory<OpalFactoryCodec>
{
  public:
    class Worker : public PFactory<OpalFactoryCodec>::WorkerBase 
    {
      public:
        Worker(const PString & key, PluginCodec_Definition * _codecDefn)
          : PFactory<OpalFactoryCodec>::WorkerBase(TRUE), codecDefn(_codecDefn)
        { PFactory<OpalFactoryCodec>::Register(key, this); }

      protected:
        virtual OpalFactoryCodec * Create(const PDefaultPFactoryKey &) const
        { return new OpalPluginCodec(codecDefn); }

        PluginCodec_Definition * codecDefn;
    };
};

/////////////////////////////////////////////////////////////////////////////

#ifndef NO_H323_AUDIO_CODECS

extern "C" {
  unsigned char linear2ulaw(int pcm_val);
  int ulaw2linear(unsigned char u_val);
  unsigned char linear2alaw(int pcm_val);
  int alaw2linear(unsigned char u_val);
};

#define DECLARE_FIXED_CODEC(name, format, bps, frameTime, samples, bytes, fpp, maxfpp, payload, sdp) \
class name##_Base : public OpalFactoryCodec { \
  PCLASSINFO(name##_Base, OpalFactoryCodec) \
  public: \
    name##_Base() \
    { } \
    unsigned int GetSampleRate() const                 { return 8000; } \
    unsigned int GetBitsPerSec() const                 { return bps; } \
    unsigned int GetFrameTime() const                  { return frameTime; } \
    unsigned int GetSamplesPerFrame() const            { return samples; } \
    unsigned int GetBytesPerFrame() const              { return bytes; } \
    unsigned int GetRecommendedFramesPerPacket() const { return fpp; } \
    unsigned int GetMaxFramesPerPacket() const         { return maxfpp; } \
    BYTE GetRTPPayload() const                         { return payload; } \
    PString GetSDPFormat() const                       { return sdp; } \
}; \
class name##_Encoder : public name##_Base { \
  PCLASSINFO(name##_Encoder, name##_Base) \
  public: \
    name##_Encoder() \
    { } \
    virtual PString GetInputFormat() const \
    { return format; }  \
    virtual PString GetOutputFormat() const \
    { return "L16"; }  \
    static PString GetFactoryName() \
    { return PString("L16") + "|" + format; } \
    int Encode(const void * from, unsigned * fromLen, void * to,   unsigned * toLen, unsigned int * flag); \
}; \
class name##_Decoder : public name##_Base { \
PCLASSINFO(name##_Decoder, name##_Base) \
  public: \
    name##_Decoder() \
    { } \
    virtual PString GetInputFormat() const \
    { return "L16"; }  \
    virtual PString GetOutputFormat() const \
    { return format; } \
    static PString GetFactoryName() \
    { return PString(format) + "|" + "L16"; } \
    int Encode(const void * from, unsigned * fromLen, void * to,   unsigned * toLen, unsigned int * flag); \
}; \

DECLARE_FIXED_CODEC(OpalG711ALaw64k, OpalG711ALaw64k, 64000, 30000, 240, 240, 30, 30, RTP_DataFrame::PCMA, "PCMA")

int OpalG711ALaw64k_Encoder::Encode(const void * _from, unsigned * fromLen, void * _to,   unsigned * toLen, unsigned int * )
{
  if (*fromLen/2 > *toLen)
    return 0;

  const short * from = (short *)_from;
  BYTE * to          = (BYTE *)_to;

  unsigned count = *fromLen / 2;
  *toLen         = count;

  while (count-- > 0)
    *to++ = linear2alaw(*from++);

  return 1;
}

int OpalG711ALaw64k_Decoder::Encode(const void * _from, unsigned * fromLen, void * _to,   unsigned * toLen, unsigned int * )
{
  if (*fromLen*2 > *toLen)
    return 0;

  const BYTE * from = (BYTE *)_from;
  short * to        = (short *)_to;

  unsigned count = *fromLen;
  *toLen         = count * 2;

  while (count-- > 0)
    *to++ = (short)alaw2linear(*from++);

  return 1;
}

DECLARE_FIXED_CODEC(OpalG711uLaw64k, OpalG711uLaw64k, 64000, 30000, 240, 240, 30, 30, RTP_DataFrame::PCMU, "PCMU")

int OpalG711uLaw64k_Encoder::Encode(const void * _from, unsigned * fromLen, void * _to,   unsigned * toLen, unsigned int * )
{
  if (*fromLen/2 > *toLen)
    return 0;

  const short * from = (short *)_from;
  BYTE * to          = (BYTE *)_to;

  unsigned count = *fromLen / 2;
  *toLen         = count;

  while (count-- > 0)
    *to++ = linear2ulaw(*from++);

  return 1;
}

int OpalG711uLaw64k_Decoder::Encode(const void * _from, unsigned * fromLen, void * _to,   unsigned * toLen, unsigned int * )
{
  if (*fromLen*2 > *toLen)
    return 0;

  const BYTE * from = (BYTE *)_from;
  short * to        = (short *)_to;

  unsigned count = *fromLen;
  *toLen         = count * 2;

  while (count-- > 0)
    *to++ = (short)ulaw2linear(*from++);

  return 1;
}

#endif // NO_H323_AUDIO_CODECS

//////////////////////////////////////////////////////////////////////////////
//
// Helper functions for codec control operators
//

static PluginCodec_ControlDefn * GetCodecControl(const PluginCodec_Definition * codec, const char * name)
{
  PluginCodec_ControlDefn * codecControls = codec->codecControls;
  if (codecControls == NULL)
    return NULL;

  while (codecControls->name != NULL) {
    if (strcasecmp(codecControls->name, name) == 0)
      return codecControls;
    codecControls++;
  }

  return NULL;
}

static BOOL SetCodecControl(const PluginCodec_Definition * codec, 
                                                    void * context,
                                              const char * name,
                                              const char * parm, 
                                              const char * value)
{
 if(codec == NULL) return FALSE;
  PluginCodec_ControlDefn * codecControls = GetCodecControl(codec, name);
  if (codecControls == NULL)
    return FALSE;

///////////////////// ZAZ /////////////////////////
  char const * options[3] = { parm, value, NULL };
  unsigned optionsLen = sizeof(const char **);
  //char const * options[2] = { parm, value };
  //unsigned optionsLen = 2;
  return (*codecControls->control)(codec, context, SET_CODEC_OPTIONS_CONTROL, options, &optionsLen);
}

static BOOL SetCodecControl(const PluginCodec_Definition * codec, 
                                                    void * context,
                                              const char * name,
                                              const char * parm, 
                                                       int value)
{
  return SetCodecControl(codec, context, name, parm, PString(PString::Signed, value));
}

#ifdef H323_VIDEO

static BOOL EventCodecControl(PluginCodec_Definition * codec, 
                                               void * context,
                                         const char * name,
                                         const char * parm )
{
 if(codec == NULL) return FALSE;
  PluginCodec_ControlDefn * codecControls = codec->codecControls;
  if (codecControls == NULL)
    return FALSE;

  // Still to be finalised how to write back an event to the plugin codec 
  // in a meaningful way

  return FALSE;
}

static BOOL CallCodecControl(PluginCodec_Definition * codec, 
                                               void * context,
                                         const char * name,
                                               void * parm, 
                                       unsigned int * parmLen,
                                                int & retVal)
{
  PluginCodec_ControlDefn * codecControls = codec->codecControls;
  if (codecControls == NULL)
    return FALSE;

  while (codecControls->name != NULL) {
    if (strcasecmp(codecControls->name, name) == 0) {
      retVal = (*codecControls->control)(codec, context, name, parm, parmLen);
      return TRUE;
    }
    codecControls++;
  }

  return FALSE;
}

//////////////////////////////////////////////////////////////////////////////

#if PTRACING
static int PluginLogFunction(unsigned level, const char * file, unsigned line, const char * section, const char * log)
{
  if(level > PTrace::GetLevel())
    return false;

  if(log == NULL)
    return true;

  if(section == NULL)
    section = "Plugin";

  PTrace::Begin(level, file, line) << section << '\t' << log << PTrace::End;
  return true;
}
#endif

//////////////////////////////////////////////////////////////////////////////

static void PopulateMediaFormatOptions(PluginCodec_Definition * _encoderCodec, OpalMediaFormat & format)
{
  char ** _options = NULL;
  unsigned int optionsLen = sizeof(_options);
  int retVal;

  if (CallCodecControl(_encoderCodec, NULL, GET_CODEC_OPTIONS_CONTROL, &_options, &optionsLen, retVal) && (_options != NULL)) {
    if (_encoderCodec->version < PLUGIN_CODEC_VERSION_OPTIONS) {
      PTRACE(3, "OpalPlugin\tAdding options to OpalMediaFormat " << format << " using old style method");
      // Old scheme
      char ** options = _options;

      while (options[0] != NULL && options[1] != NULL && options[2] != NULL) {
        const char * key = options[0];
        // Backward compatibility tests
        if (strcasecmp(key, h323_qcifMPI_tag) == 0)
          key = qcifMPI_tag;
        else if (strcasecmp(key, h323_cifMPI_tag) == 0)
          key = cifMPI_tag;
        else if (strcasecmp(key, h323_sqcifMPI_tag) == 0)
          key = sqcifMPI_tag;
        else if (strcasecmp(key, h323_cif4MPI_tag) == 0)
          key = cif4MPI_tag;
        else if (strcasecmp(key, h323_cif16MPI_tag) == 0)
          key = cif16MPI_tag;
        const char * val = options[1];
        const char * type = options[2];
        OpalMediaOption::MergeType op = OpalMediaOption::NoMerge;
        if (val != NULL && val[0] != '\0' && val[1] != '\0') {
          switch (val[0]) {
            case '<':
              op = OpalMediaOption::MinMerge;
              ++val;
              break;
            case '>':
              op = OpalMediaOption::MaxMerge;
              ++val;
              break;
            case '=':
              op = OpalMediaOption::EqualMerge;
              ++val;
              break;
            case '!':
              op = OpalMediaOption::NotEqualMerge;
              ++val;
              break;
            case '*':
              op = OpalMediaOption::AlwaysMerge;
              ++val;
              break;
            default:
              break;
          }
        }
        if (type != NULL && type[0] != '\0') {
          PStringArray tokens = PString(val+1).Tokenise(':', FALSE);
          char ** array = tokens.ToCharArray();
          switch (toupper(type[0])) {
            case 'E':
              if (format.HasOption(key))
                format.SetOptionEnum(key,tokens.GetStringsIndex(val));
              else
                format.AddOption(new OpalMediaOptionEnum(key, false, array, tokens.GetSize(), op, tokens.GetStringsIndex(val)));
              break;
            case 'B':
              if (format.HasOption(key))
                  format.SetOptionBoolean(key, val != NULL && (val[0] == '1' || toupper(val[0] == 'T')));
              else
                format.AddOption(new OpalMediaOptionBoolean(key, false, op, val != NULL && (val[0] == '1' || toupper(val[0] == 'T'))));
              break;
            case 'R':
                if (format.HasOption(key))
                format.SetOptionReal(key, PString(val).AsReal());
                else if (tokens.GetSize() < 2)
                format.AddOption(new OpalMediaOptionReal(key, false, op, PString(val).AsReal()));
                else
                format.AddOption(new OpalMediaOptionReal(key, false, op, PString(val).AsReal(), tokens[0].AsReal(), tokens[1].AsReal()));
              break;
            case 'I':
                if (format.HasOption(key))
                    format.SetOptionInteger(key,PString(val).AsInteger());
                else if (tokens.GetSize() < 2) 
                    format.AddOption(new OpalMediaOptionInteger(key, false, op, PString(val).AsInteger()));
                else
                    format.AddOption(new OpalMediaOptionInteger(key, false, op, PString(val).AsInteger(), tokens[0].AsInteger(), tokens[1].AsInteger()));
              break;
            case 'S':
            default:
                if (format.HasOption(key))
                  format.SetOptionString(key, val);
                else
                  format.AddOption(new OpalMediaOptionString(key, false, val));
              break;
          }
          free(array);
        }
        options += 3;
      }
    }
    else {
      // New scheme
      struct PluginCodec_Option const * const * options = (struct PluginCodec_Option const * const *)_options;
      PTRACE_IF(5, options != NULL, "Adding options to OpalMediaFormat " << format << " using new style method");
      while (*options != NULL) {
        struct PluginCodec_Option const * option = *options++;
        OpalMediaOption * newOption;
        switch (option->m_type) {
          case PluginCodec_StringOption :
            newOption = new OpalMediaOptionString(option->m_name,
                                                  option->m_readOnly != 0,
                                                  option->m_value);
            break;
          case PluginCodec_BoolOption :
            newOption = new OpalMediaOptionBoolean(option->m_name,
                                                   option->m_readOnly != 0,
                                                   (OpalMediaOption::MergeType)option->m_merge,
                                                   option->m_value != NULL && *option->m_value == 'T');
            break;
          case PluginCodec_IntegerOption :
            newOption = new OpalMediaOptionUnsigned(option->m_name,
                                                    option->m_readOnly != 0,
                                                    (OpalMediaOption::MergeType)option->m_merge,
                                                    PString(option->m_value).AsInteger(),
                                                    PString(option->m_minimum).AsInteger(),
                                                    PString(option->m_maximum).AsInteger());
            break;
          case PluginCodec_RealOption :
            newOption = new OpalMediaOptionReal(option->m_name,
                                                option->m_readOnly != 0,
                                                (OpalMediaOption::MergeType)option->m_merge,
                                                PString(option->m_value).AsReal(),
                                                PString(option->m_minimum).AsReal(),
                                                PString(option->m_maximum).AsReal());
            break;
          case PluginCodec_EnumOption :
            {
              PStringArray valueTokens = PString(option->m_minimum).Tokenise(':');
              char ** enumValues = valueTokens.ToCharArray();
              newOption = new OpalMediaOptionEnum(option->m_name,
                                                  option->m_readOnly != 0,
                                                  enumValues,
                                                  valueTokens.GetSize(),
                                                  (OpalMediaOption::MergeType)option->m_merge,
                                                  valueTokens.GetStringsIndex(option->m_value));
              free(enumValues);
            }
            break;
          case PluginCodec_OctetsOption :
            newOption = new OpalMediaOptionOctets(option->m_name, option->m_readOnly != 0, option->m_minimum != NULL); // Use minimum to indicate Base64
            newOption->FromString(option->m_value);
            break;
          default : // Huh?
            continue;
        }

        newOption->SetFMTPName(option->m_FMTPName);
        newOption->SetFMTPDefault(option->m_FMTPDefault);

        OpalMediaOption::H245GenericInfo genericInfo;
        genericInfo.ordinal = option->m_H245Generic&PluginCodec_H245_OrdinalMask;
        if (option->m_H245Generic&PluginCodec_H245_Collapsing)
          genericInfo.mode = OpalMediaOption::H245GenericInfo::Collapsing;
        else if (option->m_H245Generic&PluginCodec_H245_NonCollapsing)
          genericInfo.mode = OpalMediaOption::H245GenericInfo::NonCollapsing;
        else
          genericInfo.mode = OpalMediaOption::H245GenericInfo::None;
        if (option->m_H245Generic&PluginCodec_H245_Unsigned32)
          genericInfo.integerType = OpalMediaOption::H245GenericInfo::Unsigned32;
        else if (option->m_H245Generic&PluginCodec_H245_BooleanArray)
          genericInfo.integerType = OpalMediaOption::H245GenericInfo::BooleanArray;
        else
          genericInfo.integerType = OpalMediaOption::H245GenericInfo::UnsignedInt;
        genericInfo.excludeTCS = (option->m_H245Generic&PluginCodec_H245_TCS) == 0;
        genericInfo.excludeOLC = (option->m_H245Generic&PluginCodec_H245_OLC) == 0;
        genericInfo.excludeReqMode = (option->m_H245Generic&PluginCodec_H245_ReqMode) == 0;
        newOption->SetH245Generic(genericInfo);

        format.AddOption(newOption, TRUE);
      }
    }

    CallCodecControl(_encoderCodec, NULL, FREE_CODEC_OPTIONS_CONTROL, _options, &optionsLen, retVal);
  } else {
      PTRACE(4,"PLUGIN\tUnable to read default options");
  }
  
}

static void SetDefaultVideoOptions(OpalMediaFormat & mediaFormat)
{
  mediaFormat.AddOption(new OpalMediaOptionInteger(qcifMPI_tag,  false, OpalMediaOption::MinMerge, 0));
  mediaFormat.AddOption(new OpalMediaOptionInteger(cifMPI_tag,   false, OpalMediaOption::MinMerge, 0));
  mediaFormat.AddOption(new OpalMediaOptionInteger(sqcifMPI_tag, false, OpalMediaOption::MinMerge, 0));
  mediaFormat.AddOption(new OpalMediaOptionInteger(cif4MPI_tag,  false, OpalMediaOption::MinMerge, 0));
  mediaFormat.AddOption(new OpalMediaOptionInteger(cif16MPI_tag, false, OpalMediaOption::MinMerge, 0));

  mediaFormat.AddOption(new OpalMediaOptionInteger(OpalVideoFormat::FrameWidthOption,          true,  OpalMediaOption::MinMerge, CIF4_WIDTH, 11, 32767));
  mediaFormat.AddOption(new OpalMediaOptionInteger(OpalVideoFormat::FrameHeightOption,         true,  OpalMediaOption::MinMerge, CIF4_HEIGHT, 9, 32767));
  mediaFormat.AddOption(new OpalMediaOptionInteger(OpalVideoFormat::EncodingQualityOption,     false, OpalMediaOption::MinMerge, 15,          1, 31));
  mediaFormat.AddOption(new OpalMediaOptionInteger(OpalVideoFormat::TargetBitRateOption,       false, OpalMediaOption::MinMerge, 64000,    1000));
  mediaFormat.AddOption(new OpalMediaOptionInteger(OpalVideoFormat::MaxBitRateOption,          false, OpalMediaOption::MinMerge, mediaFormat.GetBandwidth(), 1000));
  mediaFormat.AddOption(new OpalMediaOptionBoolean(OpalVideoFormat::DynamicVideoQualityOption, false, OpalMediaOption::NoMerge,  false));
  mediaFormat.AddOption(new OpalMediaOptionBoolean(OpalVideoFormat::AdaptivePacketDelayOption, false, OpalMediaOption::NoMerge,  false));
  mediaFormat.AddOption(new OpalMediaOptionInteger(OpalVideoFormat::FrameTimeOption,           false, OpalMediaOption::NoMerge,  3600));

  mediaFormat.AddOption(new OpalMediaOptionBoolean(h323_temporalSpatialTradeOffCapability_tag, false, OpalMediaOption::NoMerge,  false));
  mediaFormat.AddOption(new OpalMediaOptionBoolean(h323_stillImageTransmission_tag           , false, OpalMediaOption::NoMerge,  false));
}

#endif  // #ifdef H323_VIDEO

static void PopulateMediaFormatFromGenericData(OpalMediaFormat & mediaFormat, const PluginCodec_H323GenericCodecData * genericData)
{
  const PluginCodec_H323GenericParameterDefinition *ptr = genericData->params;
  for (unsigned i = 0; i < genericData->nParameters; i++, ptr++) {
    OpalMediaOption::H245GenericInfo generic;
    generic.ordinal = ptr->id;
    generic.mode = ptr->collapsing ? OpalMediaOption::H245GenericInfo::Collapsing : OpalMediaOption::H245GenericInfo::NonCollapsing;
    generic.excludeTCS = ptr->excludeTCS;
    generic.excludeOLC = ptr->excludeOLC;
    generic.excludeReqMode = ptr->excludeReqMode;
    generic.integerType = OpalMediaOption::H245GenericInfo::UnsignedInt;

    PString name(PString::Printf, "Generic Parameter %u", ptr->id);

    OpalMediaOption * mediaOption;
    switch (ptr->type) {
      case PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_logical :
        mediaOption = new OpalMediaOptionBoolean(name, ptr->readOnly, OpalMediaOption::NoMerge, ptr->value.integer != 0);
        break;

      case PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_booleanArray :
        generic.integerType = OpalMediaOption::H245GenericInfo::BooleanArray;
        mediaOption = new OpalMediaOptionUnsigned(name, ptr->readOnly, OpalMediaOption::AndMerge, ptr->value.integer, 0, 255);
        break;

      case PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsigned32Min :
        generic.integerType = OpalMediaOption::H245GenericInfo::Unsigned32;
        // Do next case

      case PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsignedMin :
        mediaOption = new OpalMediaOptionUnsigned(name, ptr->readOnly, OpalMediaOption::MinMerge, ptr->value.integer);
        break;

      case PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsigned32Max :
        generic.integerType = OpalMediaOption::H245GenericInfo::Unsigned32;
        // Do next case

      case PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_unsignedMax :
        mediaOption = new OpalMediaOptionUnsigned(name, ptr->readOnly, OpalMediaOption::MaxMerge, ptr->value.integer);
        break;

      case PluginCodec_H323GenericParameterDefinition::PluginCodec_GenericParameter_octetString :
        mediaOption = new OpalMediaOptionString(name, ptr->readOnly, ptr->value.octetstring);
        break;

      default :
        mediaOption = NULL;
    }

    if (mediaOption != NULL) {
      mediaOption->SetH245Generic(generic);
      mediaFormat.AddOption(mediaOption);
    }
  }
}

template <typename CodecClass>
class OpalFixedCodecFactory : public PFactory<OpalFactoryCodec>
{
  public:
    class Worker : public PFactory<OpalFactoryCodec>::WorkerBase 
    {
      public:
        Worker(const PString & key)
          : PFactory<OpalFactoryCodec>::WorkerBase()
        { PFactory<OpalFactoryCodec>::Register(key, this); }

      protected:
        virtual OpalFactoryCodec * Create(const PString &) const
        { return new CodecClass(); }
    };
};


static PString CreateCodecName(PluginCodec_Definition * codec, BOOL addSW)
{
  PString str;
  if (codec->destFormat != NULL)
    str = codec->destFormat;
  else
    str = PString(codec->descr);
  if (addSW)
    str += "{sw}";
  return str;
}

static PString CreateCodecName(const PString & baseName, BOOL addSW)
{
  PString str(baseName);
  if (addSW)
    str += "{sw}";
  return str;
}

class OpalPluginAudioMediaFormat : public OpalMediaFormat
{
  public:
    friend class H323PluginCodecManager;

    OpalPluginAudioMediaFormat(
      PluginCodec_Definition * _encoderCodec,
      unsigned defaultSessionID,  /// Default session for codec type
      BOOL     needsJitter,   /// Indicate format requires a jitter buffer
      unsigned frameTime,     /// Time for frame in RTP units (if applicable)
      unsigned timeUnits,     /// RTP units for frameTime (if applicable)
      time_t timeStamp        /// timestamp (for versioning)
    )
    : OpalMediaFormat(
      CreateCodecName(_encoderCodec, FALSE),
      defaultSessionID,
      (RTP_DataFrame::PayloadTypes)(((_encoderCodec->flags & PluginCodec_RTPTypeMask) == PluginCodec_RTPTypeDynamic) ? RTP_DataFrame::DynamicBase : _encoderCodec->rtpPayload),
      needsJitter,
      _encoderCodec->bitsPerSec,
      _encoderCodec->parm.audio.bytesPerFrame,
      frameTime,
      timeUnits,
      timeStamp
    )
    , encoderCodec(_encoderCodec)
    {
      // manually register the new singleton type, as we do not have a concrete type
      OpalMediaFormatFactory::Register(*this, this);
    }
    ~OpalPluginAudioMediaFormat()
    {
      OpalMediaFormatFactory::Unregister(*this);
    }
    PluginCodec_Definition * encoderCodec;
};

#ifndef NO_H323_AUDIO_CODECS

static H323Capability * CreateG7231Cap(
  PluginCodec_Definition * encoderCodec, 
  PluginCodec_Definition * decoderCodec,
  int subType
);

static H323Capability * CreateGenericAudioCap(
  PluginCodec_Definition * encoderCodec, 
  PluginCodec_Definition * decoderCodec,
  int subType
);

static H323Capability * CreateNonStandardAudioCap(
  PluginCodec_Definition * encoderCodec, 
  PluginCodec_Definition * decoderCodec,
  int subType
);

static H323Capability * CreateGSMCap(
  PluginCodec_Definition * encoderCodec, 
  PluginCodec_Definition * decoderCodec,
  int subType
);

#endif // NO_H323_AUDIO

/////////////////////////////////////////////////////

#ifdef H323_VIDEO

class OpalPluginVideoMediaFormat : public OpalVideoFormat
{
  public:
    friend class OpalPluginCodecManager;

    OpalPluginVideoMediaFormat(
      PluginCodec_Definition * _encoderCodec,
      const char * /*rtpEncodingName*/, /// rtp encoding name. Not required
      time_t timeStamp              /// timestamp (for versioning)
    )
    : OpalVideoFormat(
      CreateCodecName(_encoderCodec, FALSE),
      (RTP_DataFrame::PayloadTypes)(((_encoderCodec->flags & PluginCodec_RTPTypeMask) == PluginCodec_RTPTypeDynamic) ? RTP_DataFrame::DynamicBase : _encoderCodec->rtpPayload),
      _encoderCodec->parm.video.maxFrameWidth,
      _encoderCodec->parm.video.maxFrameHeight,
      _encoderCodec->parm.video.maxFrameRate,
      _encoderCodec->bitsPerSec,
      timeStamp
    )
    , encoderCodec(_encoderCodec)
    {
       SetDefaultVideoOptions(*this);

       rtpPayloadType = (RTP_DataFrame::PayloadTypes)(((_encoderCodec->flags & PluginCodec_RTPTypeMask) == PluginCodec_RTPTypeDynamic) ? RTP_DataFrame::DynamicBase : _encoderCodec->rtpPayload);
       frameTime = (VideoTimeUnits * encoderCodec->usPerFrame) / 1000;
       timeUnits = encoderCodec->sampleRate / 1000; 

      // manually register the new singleton type, as we do not have a concrete type
      OpalMediaFormatFactory::Register(*this, this);
    }
    ~OpalPluginVideoMediaFormat()
    {
      OpalMediaFormatFactory::Unregister(*this);
    }

    PObject * Clone() const
    { return new OpalPluginVideoMediaFormat(*this); }
    
    PluginCodec_Definition * encoderCodec;
};

static H323Capability * CreateNonStandardVideoCap(
  PluginCodec_Definition * encoderCodec, 
  PluginCodec_Definition * decoderCodec,
  int subType
);

static H323Capability * CreateGenericVideoCap(
  PluginCodec_Definition * encoderCodec, 
  PluginCodec_Definition * decoderCodec,
  int subType
);

static H323Capability * CreateH261Cap(
  PluginCodec_Definition * encoderCodec, 
  PluginCodec_Definition * decoderCodec,
  int subType
);

static H323Capability * CreateH263Cap(
  PluginCodec_Definition * encoderCodec, 
  PluginCodec_Definition * decoderCodec,
  int subType
);

#endif // H323_VIDEO


/*
//////////////////////////////////////////////////////////////////////////////
//
// Class to auto-register plugin capabilities
//

class H323CodecPluginCapabilityRegistration : public PObject
{
  public:
    H323CodecPluginCapabilityRegistration(
       PluginCodec_Definition * _encoderCodec,
       PluginCodec_Definition * _decoderCodec
    );

    H323Capability * Create(H323EndPoint & ep) const;
  
    static H323Capability * CreateG7231Cap           (H323EndPoint & ep, int subType) const;
    static H323Capability * CreateNonStandardAudioCap(H323EndPoint & ep, int subType) const;
    //H323Capability * CreateNonStandardVideoCap(H323EndPoint & ep, int subType) const;
    static H323Capability * CreateGSMCap             (H323EndPoint & ep, int subType) const;
    static H323Capability * CreateH261Cap            (H323EndPoint & ep, int subType) const;

  protected:
    PluginCodec_Definition * encoderCodec;
    PluginCodec_Definition * decoderCodec;
};

*/

class H323CodecPluginCapabilityMapEntry {
  public:
    int pluginCapType;
    int h323SubType;
    H323Capability * (* createFunc)(PluginCodec_Definition * encoderCodec, PluginCodec_Definition * decoderCodec, int subType);
};

#ifndef NO_H323_AUDIO_CODECS

static H323CodecPluginCapabilityMapEntry audioMaps[] = {
  { PluginCodec_H323Codec_nonStandard,              H245_AudioCapability::e_nonStandard,         &CreateNonStandardAudioCap },
  { PluginCodec_H323AudioCodec_gsmFullRate,         H245_AudioCapability::e_gsmFullRate,         &CreateGSMCap },
  { PluginCodec_H323AudioCodec_gsmHalfRate,         H245_AudioCapability::e_gsmHalfRate,         &CreateGSMCap },
  { PluginCodec_H323AudioCodec_gsmEnhancedFullRate, H245_AudioCapability::e_gsmEnhancedFullRate, &CreateGSMCap },
  { PluginCodec_H323AudioCodec_g711Alaw_64k,        H245_AudioCapability::e_g711Alaw64k },
  { PluginCodec_H323AudioCodec_g711Alaw_56k,        H245_AudioCapability::e_g711Alaw56k },
  { PluginCodec_H323AudioCodec_g711Ulaw_64k,        H245_AudioCapability::e_g711Ulaw64k },
  { PluginCodec_H323AudioCodec_g711Ulaw_56k,        H245_AudioCapability::e_g711Ulaw56k },
  { PluginCodec_H323AudioCodec_g7231,               H245_AudioCapability::e_g7231,               &CreateG7231Cap },
  { PluginCodec_H323AudioCodec_g729,                H245_AudioCapability::e_g729 },
  { PluginCodec_H323AudioCodec_g729AnnexA,          H245_AudioCapability::e_g729AnnexA },
  { PluginCodec_H323AudioCodec_g728,                H245_AudioCapability::e_g728 }, 
  { PluginCodec_H323AudioCodec_g722_64k,            H245_AudioCapability::e_g722_64k },
  { PluginCodec_H323AudioCodec_g722_56k,            H245_AudioCapability::e_g722_56k },
  { PluginCodec_H323AudioCodec_g722_48k,            H245_AudioCapability::e_g722_48k },
  { PluginCodec_H323AudioCodec_g729wAnnexB,         H245_AudioCapability::e_g729wAnnexB }, 
  { PluginCodec_H323AudioCodec_g729AnnexAwAnnexB,   H245_AudioCapability::e_g729AnnexAwAnnexB },
  { PluginCodec_H323Codec_generic,                  H245_AudioCapability::e_genericAudioCapability, &CreateGenericAudioCap },

  // not implemented
  //{ PluginCodec_H323AudioCodec_g729Extensions,      H245_AudioCapability::e_g729Extensions,   0 },
  //{ PluginCodec_H323AudioCodec_g7231AnnexC,         H245_AudioCapability::e_g7231AnnexCMode   0 },
  //{ PluginCodec_H323AudioCodec_is11172,             H245_AudioCapability::e_is11172AudioMode, 0 },
  //{ PluginCodec_H323AudioCodec_is13818Audio,        H245_AudioCapability::e_is13818AudioMode, 0 },

  { -1 }
};

#endif

#ifdef H323_VIDEO

static H323CodecPluginCapabilityMapEntry videoMaps[] = {
  // video codecs
  { PluginCodec_H323Codec_nonStandard,              H245_VideoCapability::e_nonStandard, &CreateNonStandardVideoCap },
  { PluginCodec_H323VideoCodec_h261,                H245_VideoCapability::e_h261VideoCapability, &CreateH261Cap },
  { PluginCodec_H323VideoCodec_h263,                H245_VideoCapability::e_h263VideoCapability,    &CreateH263Cap },
  { PluginCodec_H323Codec_generic,                  H245_VideoCapability::e_genericVideoCapability, &CreateGenericVideoCap },
/*
  PluginCodec_H323VideoCodec_h262,                // not yet implemented
  PluginCodec_H323VideoCodec_is11172,             // not yet implemented
*/

  { -1 }
};

#endif  // H323_VIDEO


//////////////////////////////////////////////////////////////////////////////
//
// Plugin streamed audio codec classes
//

#ifndef NO_H323_AUDIO_CODECS

class H323StreamedPluginAudioCodec : public H323StreamedAudioCodec
{
  PCLASSINFO(H323StreamedPluginAudioCodec, H323StreamedAudioCodec);
  public:
    H323StreamedPluginAudioCodec(
      const OpalMediaFormat & fmtName, 
      H323Codec::Direction direction, 
      unsigned samplesPerFrame,  /// Number of samples in a frame
      unsigned bits,             /// Bits per sample
      PluginCodec_Definition * _codec
    )
      : H323StreamedAudioCodec(fmtName, direction, samplesPerFrame, bits), codec(_codec)
    { if (codec != NULL && codec->createCodec != NULL) context = (*codec->createCodec)(codec); else context = NULL; }

    ~H323StreamedPluginAudioCodec()
    { if (codec != NULL && codec->destroyCodec != NULL) (*codec->destroyCodec)(codec, context); }

    int Encode(short sample) const
    {
      if (codec == NULL || direction != Encoder)
        return 0;
      unsigned int fromLen = sizeof(sample);
      int to;
      unsigned toLen = sizeof(to);
      unsigned flags = 0;
      (codec->codecFunction)(codec, context, 
                                 (const unsigned char *)&sample, &fromLen,
                                 (unsigned char *)&to, &toLen,
                                 &flags);
      return to;
    }

    short Decode(int sample) const
    {
      if (codec == NULL || direction != Decoder)
        return 0;
      unsigned fromLen = sizeof(sample);
      short to;
      unsigned toLen   = sizeof(to);
      unsigned flags = 0;
      (codec->codecFunction)(codec, context, 
                                 (const unsigned char *)&sample, &fromLen,
                                 (unsigned char *)&to, &toLen,
                                 &flags);
      return to;
    }

    virtual void SetTxQualityLevel(int qlevel)
    { SetCodecControl(codec, context, SET_CODEC_OPTIONS_CONTROL, "set_quality", qlevel); }

  protected:
    void * context;
    PluginCodec_Definition * codec;
};

#endif //  NO_H323_AUDIO_CODECS

//////////////////////////////////////////////////////////////////////////////

#define FRAME_MASK	0xFFFFFF00
#define FRAME_BUF_SIZE	0x2000
#define FRAME_OFFSET	0x100

class unit 
{
    public:
	unit() {};
	~unit() {};
	
	void PutFrame(RTP_DataFrame &srcFrame);
	void GetFrame(RTP_DataFrame &dstFrame);
	int lock;
	unsigned int len;
    private:
	RTP_DataFrame frame;
};

typedef std::map<unsigned int, unit *> RTPCacheMap;

class cache
{
    public:
	cache() { seqN = FRAME_BUF_SIZE; lastN = 0; iframeN = 0; uN = 0; fastUpdate = 0; terminate = FALSE; }
	~cache()
        {
          for(RTPCacheMap::iterator r = rtpCaches.begin(); r != rtpCaches.end(); )
          { unit *u = r->second; rtpCaches.erase(r++); if(u) delete u; }
        }

	void PutFrame(RTP_DataFrame &frame, unsigned int len, unsigned int flags);
	void GetFrame(RTP_DataFrame &frame, unsigned int &toLen, unsigned int &num, unsigned int &flags);
	unsigned int GetLastFrameNum();
	void GetFastUpdate(unsigned int &flags);
        unsigned int uN;
        BOOL fastUpdate;
        BOOL terminate;
    private:
        unsigned int seqN;
        unsigned int lastN;
        unsigned int iframeN;
        RTPCacheMap rtpCaches;
};

// New 1
class cacheArray
{
   public:
       cacheArray() { n = 0; }
       ~cacheArray() { }


        cache *GetCache();
        int n;
        cache c[3];
   private:
};

typedef std::map<PString, cacheArray *> CacheMap;
CacheMap caches;

void unit::PutFrame(RTP_DataFrame &srcFrame)
{
  int sz = srcFrame.GetHeaderSize()+srcFrame.GetPayloadSize();
  frame.SetMinSize(sz);
  memcpy(frame.GetPointer(),srcFrame.GetPointer(),sz);
  frame.SetPayloadSize(srcFrame.GetPayloadSize());
}

void unit::GetFrame(RTP_DataFrame &dstFrame)
{
  int sz = frame.GetHeaderSize()+frame.GetPayloadSize();
  dstFrame.SetMinSize(sz);
  memcpy(dstFrame.GetPointer(),frame.GetPointer(),sz);
  dstFrame.SetPayloadSize(frame.GetPayloadSize());
}

bool GetMarker (unsigned char *pkt) 
{
  return (pkt[1] & 0x80);
}

cache *CreateCacheRTP(PString key)
{
  CacheMap::iterator r = caches.find(key);
  cacheArray *newCacheArray;
  if(r == caches.end())
  {
    newCacheArray = new cacheArray();
    caches.insert(CacheMap::value_type(key, newCacheArray));
  }
  else newCacheArray = r->second;
  if(newCacheArray->n >= 1) return NULL;
  newCacheArray->n++;
  cout << "new cache " << newCacheArray->n << "\n";
  return &(newCacheArray->c[newCacheArray->n-1]);
}

void GetFastUpdate(PString *key, unsigned int &flags, cache *(&srcCache))
{
  if(srcCache) srcCache->GetFastUpdate(flags);
}

void cache::GetFastUpdate(unsigned int &flags)
{
  if(!fastUpdate) return;
  if(seqN-iframeN <= FRAME_OFFSET*10) return;
  cout << "FastUpdate needed\n";
  flags|=PluginCodec_CoderForceIFrame;
  fastUpdate = FALSE;
  iframeN = (seqN&FRAME_MASK)+FRAME_OFFSET;
}

unsigned int cache::GetLastFrameNum()
{
  RTPCacheMap::reverse_iterator r = rtpCaches.rbegin();
  if(r != rtpCaches.rend()) return r->first;
  return 0;
}

void PutCacheRTP(PString *key, RTP_DataFrame &frame, unsigned int len, unsigned int flags, cache *(&dstCache))
{
  if(dstCache) { dstCache->PutFrame(frame,len,flags); return; }
  dstCache = CreateCacheRTP(*key); // needs for modes without patently creation of caches
  if(dstCache) dstCache->PutFrame(frame,len,flags);
}

void cache::PutFrame(RTP_DataFrame &frame, unsigned int len, unsigned int flags)
{
  unit *cu;
  unsigned int l;

  //cout << "Frame seqN/lastN " << seqN << "/" << lastN << "\n";

  RTPCacheMap::iterator r = rtpCaches.find(lastN);
  while (r == rtpCaches.end() && seqN-lastN >= FRAME_BUF_SIZE)
  {
    lastN=(lastN&FRAME_MASK)+FRAME_OFFSET;
    r = rtpCaches.find(lastN);
  };

  if(r == rtpCaches.end()) { cu = new unit(); }
  else { cu = r->second; rtpCaches.erase(r); lastN++; }
  cu->PutFrame(frame); cu->len=len; cu->lock=0;
  rtpCaches.insert(RTPCacheMap::value_type(seqN, cu));

  //if(flags&PluginCodec_ReturnCoderIFrame) 
  //{ fastUpdate = FALSE; iframeN = seqN;  cout << "IFrame found\n"; }

  if(GetMarker(frame.GetPointer()))
  {
    seqN=(seqN&FRAME_MASK)+FRAME_OFFSET;
    l=seqN-FRAME_BUF_SIZE/2;
    do // lock all frame
    {
      r=rtpCaches.find(l); l++;
      if(r != rtpCaches.end()) r->second->lock = 1;
    }
    while (r != rtpCaches.end());
  }
  else seqN++;
}

cache *FindCacheRTP(PString *key)
{
  CacheMap::iterator r = caches.find(*key);
  if(r == caches.end()) return NULL;
  if(r->second->c[0].terminate) { PThread::Sleep(50); return NULL; }
  int i=(int)(1.0*r->second->n*(rand()/(RAND_MAX+1.0)));
  return &r->second->c[i];
}

void GetCacheRTP(PString *key, RTP_DataFrame &frame, unsigned int &toLen, unsigned int &seqN, unsigned int &flags, cache *(&srcCache))
{
  if(!srcCache)
  {
    return; // ???
    srcCache = FindCacheRTP(key);
    if(!srcCache) { seqN = 0xFFFFFFFF; cout << "No cache\n"; return; }
  }
  if(srcCache->terminate) { srcCache = NULL; return; }

  if(flags&PluginCodec_CoderForceIFrame) srcCache->fastUpdate = TRUE;
  srcCache->GetFrame(frame,toLen,seqN,flags);
}

void cache::GetFrame(RTP_DataFrame &frame, unsigned int &toLen, unsigned int &num, unsigned int &flags)
{
  while(num>=seqN)
  {
    if(terminate) return;
    PThread::Sleep(10);
  }
  RTPCacheMap::iterator r = rtpCaches.find(num);
  int i=0; // for debug
  while ((r == rtpCaches.end() || r->second->lock) && num<seqN)
  {
    if(terminate) return;
    PTRACE_IF(3, i>0, "H323READ\t Lost Packet " << i << " " << num); //for debug
    PTRACE_IF(3, (r!=rtpCaches.end() && r->second->lock), "H323READ\t Lost Packet " << i << " " << num << " " << r->second->lock); //for debug
    num=(num&FRAME_MASK)+FRAME_OFFSET; // may be lost frames, fix it
    r = rtpCaches.find(num);
    i++; // for debug
  }
  while (r == rtpCaches.end())
  {
    if(terminate) return;
    PThread::Sleep(10);
    r = rtpCaches.find(num);
  }

  r->second->GetFrame(frame); toLen=r->second->len;
  flags=0;
  if(GetMarker(frame.GetPointer())) flags|=PluginCodec_ReturnCoderLastFrame;
  if(num==iframeN) flags|=PluginCodec_ReturnCoderIFrame;
  num++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NO_H323_AUDIO_CODECS

class H323PluginFramedAudioCodec : public H323FramedAudioCodec
{
  PCLASSINFO(H323PluginFramedAudioCodec, H323FramedAudioCodec);
  public:
    H323PluginFramedAudioCodec(const OpalMediaFormat & fmtName, Direction direction, PluginCodec_Definition * _codec);
    ~H323PluginFramedAudioCodec();

    BOOL EncodeFrame(
      BYTE * buffer,        /// Buffer into which encoded bytes are placed
      unsigned int & toLen  /// Actual length of encoded data buffer
    );

    BOOL DecodeFrame(
      const BYTE * buffer,    /// Buffer from which encoded data is found
      unsigned length,        /// Length of encoded data buffer
      unsigned & written,     /// Number of bytes used from data buffer
      unsigned & bytesDecoded /// Number of bytes output from frame
    );

    void DecodeSilenceFrame(
      void * buffer,        /// Buffer from which encoded data is found
      unsigned length       /// Length of encoded data buffer
    )
    {
      if ((codec->flags & PluginCodec_DecodeSilence) == 0)
        memset(buffer, 0, length);
      else {
        unsigned flags = PluginCodec_CoderSilenceFrame;
        (codec->codecFunction)(codec, context,
                                 NULL, NULL,
                                 buffer, &length,
                                 &flags);
      }
    }

    virtual BOOL Read(
      BYTE * buffer,            ///< Buffer of encoded data
      unsigned & length,        ///< Actual length of encoded data buffer
      RTP_DataFrame & dst       ///< RTP data frame
    );

    virtual void SetTxQualityLevel(int qlevel)
    { SetCodecControl(codec, context, SET_CODEC_OPTIONS_CONTROL, "set_quality", qlevel); }

    virtual unsigned GetSampleRate() { return codec->sampleRate; }

    virtual int CheckCacheRTP()
    {
      cout << "CheckCacheRTP " << formatString << "\n";
      if(FindCacheRTP(&formatString)) return 1; else return 0;
    }

    virtual void AttachCacheRTP()
    {
      cout << "AttachCacheRTP " << formatString << "\n";
      codecCache = FindCacheRTP(&formatString);
      if(!codecCache) return;
      encoderSeqN = codecCache->GetLastFrameNum();
      cout << "SeqN=" << encoderSeqN << "\n";
      codecCache->uN++;
      cacheMode = 2;
      if(codec != NULL && codec->destroyCodec != NULL)
      {
        (*codec->destroyCodec)(codec, context);
        codec = NULL;
      }
    }

    virtual unsigned int GetEncoderSeqN()
    {
      return codecCache->GetLastFrameNum();
    }

    virtual void DeleteCacheRTP()
    {
      CacheMap::iterator r = caches.find(formatString);
      if(r != caches.end())
      {
        cout << "DeleteCacheRTP " << formatString << "\n";
        cacheArray *ca = r->second;
        ca->c[0].terminate = TRUE;
        ca->c[1].terminate = TRUE;
        ca->c[2].terminate = TRUE;
        PThread::Sleep(50);
        caches.erase(r);
        delete ca;
        ca = NULL;
      }
    }

    virtual void DetachCacheRTP()
    {
      cacheMode = 0;
      if(!codecCache || !FindCacheRTP(&formatString)) { cout << "DetachCacheRTP " << formatString << ", cache is not found\n"; return; }
      cout << "DetachCacheRTP " << formatString << "\n";
      codecCache->uN--;
      codecCache = NULL;
    }

    virtual void NewCacheRTP()
    {
      cout << "NewCacheRTP " << formatString << "\n";
      codecCache = CreateCacheRTP(formatString);
    }

    virtual int GetCacheUsersNumber()
    {
      if(codecCache == NULL)
        return 0;
      return codecCache->uN;
    }

  protected:
    cache *codecCache;

    void * context;
    PluginCodec_Definition * codec;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

H323PluginFramedAudioCodec::H323PluginFramedAudioCodec(const OpalMediaFormat & fmtName, Direction direction, PluginCodec_Definition * _codec)
  : H323FramedAudioCodec(fmtName, direction), codec(_codec)
{
  if(codec != NULL && codec->createCodec != NULL)
    context = (*codec->createCodec)(codec);
  else
    context = NULL;
  if(context)
  {
    PluginCodec_ControlDefn * ctl = GetCodecControl(codec, SET_CODEC_OPTIONS_CONTROL);
    if(ctl != NULL)
    {
      PStringArray list;
      for(PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++)
      {
        const OpalMediaOption & option = mediaFormat.GetOption(i);
        list += option.GetName();
        list += option.AsString();
        PTRACE(5, "OpalPlugin\tSetting codec option '" << option.GetName() << "'=" << option.AsString());
      }
      char ** _options = list.ToCharArray();
      unsigned int optionsLen = sizeof(_options);
      (*ctl->control)(codec, context, SET_CODEC_OPTIONS_CONTROL, _options, &optionsLen);
      free(_options);
    }
#if PTRACING
    PTRACE(6,"Codec Options");
    OpalMediaFormat::DebugOptionList(mediaFormat);
#endif
  }

  encoderSeqN = 0;
  encoderCacheKey = 0;
  cacheMode = 0;
  codecCache = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323PluginFramedAudioCodec::~H323PluginFramedAudioCodec()
{
  if(cacheMode == 2)
    DetachCacheRTP();

  if(codec != NULL && codec->destroyCodec != NULL)
    (*codec->destroyCodec)(codec, context);
  codec = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL H323PluginFramedAudioCodec::EncodeFrame(BYTE * buffer, unsigned int & toLen)
{
  if(codec == NULL || direction != Encoder)
    return FALSE;
  unsigned int fromLen = codec->parm.audio.samplesPerFrame*2*codecChannels;
  toLen                = codec->parm.audio.bytesPerFrame;
  unsigned flags = 0;
  return (codec->codecFunction)(codec, context,
                                 (const unsigned char *)sampleBuffer.GetPointer(), &fromLen,
                                 buffer, &toLen,
                                 &flags) != 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL H323PluginFramedAudioCodec::DecodeFrame(const BYTE * buffer, unsigned length, unsigned & written, unsigned & bytesDecoded)
{
  if(codec == NULL || direction != Decoder)
    return FALSE;
  unsigned flags = 0;
  if((codec->codecFunction)(codec, context, buffer, &length, (unsigned char *)sampleBuffer.GetPointer(), &bytesDecoded, &flags) == 0)
    return FALSE;

  written = length;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL H323PluginFramedAudioCodec::Read(BYTE * buffer, unsigned & length, RTP_DataFrame & dst)
{
  PWaitAndSignal mutex(rawChannelMutex);

  if(cacheMode == 0 || encoderSeqN == 0xFFFFFFFF)
    return H323FramedAudioCodec::Read(buffer, length, dst);

  unsigned flags = 0;
  BOOL retval = FALSE;

  if(cacheMode == 1)
  {
    // get buffer from the encoder
    retval = H323FramedAudioCodec::Read(dst.GetPayloadPtr(), length, dst);
    if(retval == FALSE)
      length = 0;

    // increase the buffer size to the size of the frame
    length += dst.GetHeaderSize();

    // put frame in cache
    PutCacheRTP(&formatString, dst, length, flags, codecCache);
  }

  if(cacheMode == 2)
  {
    GetCacheRTP(&formatString, dst, length, encoderSeqN, flags, codecCache);
    retval = TRUE;
  }

  if(retval == FALSE && codec != NULL)
  {
    PTRACE(3, "PLUGIN\tError encoding frame from plugin " << codec->descr << " cacheMode " << cacheMode);
    length = 0;
    return FALSE;
  }

  if(length > (unsigned)dst.GetHeaderSize())
    length = length - dst.GetHeaderSize();
  else
    length = 0;

  return TRUE;
};

#endif //  NO_H323_AUDIO_CODECS

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef H323_VIDEO

class H323PluginVideoCodec : public H323VideoCodec
{
  PCLASSINFO(H323PluginVideoCodec, H323VideoCodec);
  public:
    H323PluginVideoCodec(const OpalMediaFormat & fmt, Direction direction, PluginCodec_Definition * _codec);

    ~H323PluginVideoCodec();

    virtual BOOL Read(
      BYTE * buffer,            ///< Buffer of encoded data
      unsigned & length,        ///< Actual length of encoded data buffer
      RTP_DataFrame & dst       ///< RTP data frame
    );

    virtual BOOL Write(
      const BYTE * buffer,        ///< Buffer of encoded data
      unsigned length,            ///< Length of encoded data buffer
      const RTP_DataFrame & src,  ///< RTP data frame
      unsigned & written          ///< Number of bytes used from data buffer
    );

    BOOL RenderFrame(
      const BYTE * buffer         ///< Buffer of data to render
    );
 
    virtual unsigned GetFrameRate() const 
    { return lastFrameTimeRTP; }

    virtual unsigned GetTargetFrameRate() const { if(targetFrameTimeMs==0) return 30; else return 1000/targetFrameTimeMs; }

    BOOL SetTargetFrameTimeMs(unsigned ms)  // Requires implementing
    {  
     targetFrameTimeMs = ms; 
     mediaFormat.SetOptionInteger(OpalVideoFormat::FrameTimeOption, (int)(90*ms));
     return TRUE; 
    }

    virtual BOOL SetFrameSize(int width, int height);

    void SetTxQualityLevel(int qlevel)
    { SetCodecControl(codec, context, SET_CODEC_OPTIONS_CONTROL, "Encoding Quality", qlevel); }
 
    void SetTxMinQuality(int qlevel)
    { SetCodecControl(codec, context, SET_CODEC_OPTIONS_CONTROL, "set_min_quality", qlevel); }

    void SetTxMaxQuality(int qlevel)
    { SetCodecControl(codec, context, SET_CODEC_OPTIONS_CONTROL, "set_max_quality", qlevel); }

    void SetBackgroundFill(int fillLevel)
    { SetCodecControl(codec, context, SET_CODEC_OPTIONS_CONTROL, "set_background_fill", fillLevel); }

    unsigned GetMaxBitRate() const
    { return mediaFormat.GetOptionInteger(OpalVideoFormat::MaxBitRateOption); }

    BOOL SetMaxBitRate(unsigned bitRate) 
    { return SetCodecControl(codec, context, SET_CODEC_OPTIONS_CONTROL, "Max Bit Rate", bitRate); }

    void SetGeneralCodecOption(const char * opt, int val)
    { SetCodecControl(codec, context, SET_CODEC_OPTIONS_CONTROL, opt, val);}

    unsigned GetVideoMode(void);

    void SetVideoMode(int mode);

    void ConvertCIF4ToCIF(const void * _src, void * _dst);

    // The following require implementation in the plugin codec
    virtual void OnFastUpdatePicture()
    {
      EventCodecControl(codec, context, EVENT_CODEC_CONTROL, "on_fast_update");
      sendIntra = true;
    }

    virtual void OnLostPartialPicture()
    { EventCodecControl(codec, context, EVENT_CODEC_CONTROL, "on_lost_partial"); }

    virtual void OnLostPicture()
    { EventCodecControl(codec, context, EVENT_CODEC_CONTROL, "on_lost_picture"); } 

    virtual void OnFreezeVideo(int disable)
    { 
     freezeVideo = disable;
    } 

    virtual int CheckCacheRTP()
    {
      cout << "CheckCacheRTP " << formatString << "\n";
      if(FindCacheRTP(&formatString)) return 1; else return 0;
    }

    virtual void AttachCacheRTP()
    {
      cout << "AttachCacheRTP " << formatString << "\n";
      codecCache = FindCacheRTP(&formatString); 
      if(!codecCache) return;
      encoderSeqN = codecCache->GetLastFrameNum();
      cout << "SeqN=" << encoderSeqN << "\n";
      codecCache->uN++; 
      cacheMode=2; 
      if(codec != NULL && codec->destroyCodec != NULL)
        { (*codec->destroyCodec)(codec, context); codec = NULL; }
    }

    virtual unsigned int GetEncoderSeqN()
    {
      return codecCache->GetLastFrameNum();
    }

    virtual void DeleteCacheRTP()
    {
      CacheMap::iterator r = caches.find(formatString);
      if(r != caches.end())
      {
        cout << "DeleteCacheRTP " << formatString << "\n";
        cacheArray *ca = r->second;
        ca->c[0].terminate = TRUE;
        ca->c[1].terminate = TRUE;
        ca->c[2].terminate = TRUE;
        PThread::Sleep(50);
        caches.erase(r);
        delete ca;
        ca = NULL;
      }
    }

    virtual void DetachCacheRTP()
    {
      cacheMode=0;
      if(!codecCache || !FindCacheRTP(&formatString)) { cout << "DetachCacheRTP " << formatString << ", cache is not found\n"; return; }
      cout << "DetachCacheRTP " << formatString << "\n";
      codecCache->uN--;
      codecCache = NULL;
    }

    virtual void NewCacheRTP()
    {
      cout << "NewCacheRTP " << formatString << "\n";
      codecCache = CreateCacheRTP(formatString);
    }

    virtual int GetCacheUsersNumber()
    {
      if(codecCache==NULL)
        return 0;
      return codecCache->uN;
    }

  protected:
    void *       context;
    PluginCodec_Definition * codec;
    RTP_DataFrame bufferRTP;
    BOOL         lastPacketSent;
    PColourConverter * converter;

    unsigned     bytesPerFrame;
    unsigned     lastFrameTimeRTP;
    unsigned     targetFrameTimeMs;
    int          maxWidth; 
    int          maxHeight;
    bool         sendIntra;
    bool	 freezeVideo;

    cache *	 codecCache;

    mutable PTimeInterval lastFrameTick;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

H323PluginVideoCodec::H323PluginVideoCodec(const OpalMediaFormat & fmt, Direction direction, PluginCodec_Definition * _codec)
      : H323VideoCodec(fmt, direction), codec(_codec)
{
    if (codec != NULL && codec->createCodec != NULL)
        context = (*codec->createCodec)(codec);
    else
        context = NULL;

    sendIntra = TRUE;
    freezeVideo = FALSE;

    encoderSeqN	= 0;
    encoderCacheKey = 0;
    cacheMode = 0;
    codecCache = NULL;

    lastPacketSent = TRUE;
    lastFrameTimeRTP = 0;
    frameWidth = maxWidth = mediaFormat.GetOptionInteger(OpalVideoFormat::FrameWidthOption); 
    frameHeight = maxHeight = mediaFormat.GetOptionInteger(OpalVideoFormat::FrameHeightOption);

    maxWidth = 2048;   maxHeight = 2048;
    converter = NULL;
/*
    if (codec->parm.video.recommendedFrameRate != 0)
      targetFrameTimeMs = 1000 / codec->parm.video.recommendedFrameRate;
    else
      targetFrameTimeMs = mediaFormat.GetOptionInteger(OpalVideoFormat::FrameTimeOption);
*/
    targetFrameTimeMs = 40;
    // Need to allocate buffer to the maximum framesize statically
    // and clear the memory in the destructor to avoid segfault in destructor
    bytesPerFrame = (maxHeight * maxWidth * 3)/2;
    bufferRTP = RTP_DataFrame(sizeof(PluginCodec_Video_FrameHeader) + bytesPerFrame, TRUE);

    PluginCodec_ControlDefn * ctl = GetCodecControl(codec, SET_CODEC_OPTIONS_CONTROL);
    if (ctl != NULL) {
      PStringArray list;
      for (PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++) {
        const OpalMediaOption & option = mediaFormat.GetOption(i);
        list += option.GetName();
        list += option.AsString();
        PTRACE(5, "OpalPlugin\tSetting codec option '" << option.GetName() << "'=" << option.AsString());
      }
      char ** _options = list.ToCharArray();
      unsigned int optionsLen = sizeof(_options);
      (*ctl->control)(codec, context, SET_CODEC_OPTIONS_CONTROL, _options, &optionsLen);

      for(int i=0;_options[i]!=NULL;i+=2)
      {
       if (strcasecmp(_options[i], "Frame Width") == 0)
         frameWidth = atoi(_options[i+1]);
       if (strcasecmp(_options[i], "Frame Height") == 0)
         frameHeight = atoi(_options[i+1]);
       if (strcasecmp(_options[i], "Frame Time") == 0)
         targetFrameTimeMs = atoi(_options[i+1]);
//         cout << "Answer opt:" << _options[i] << "=" << _options[i+1] << "\n";
      }

      free(_options);

      //OpalMediaFormat cmf;
      //PopulateMediaFormatOptions(codec, cmf);
      //PString extradata = cmf.GetOptionString("extradata");
      //GetWritableMediaFormat().SetOptionString("extradata", extradata);

    }
   if(targetFrameTimeMs>1000) targetFrameTimeMs=40; // for h.263 codecs
   formatString = mediaFormat + "@" + PString(frameWidth) + "x" + PString(frameHeight) + ":" + PString(mediaFormat.GetOptionInteger(OpalVideoFormat::MaxBitRateOption)) + "x";
#if PTRACING
   PTRACE(6,"Codec Options");
     OpalMediaFormat::DebugOptionList(mediaFormat);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323PluginVideoCodec::~H323PluginVideoCodec()
{
    PWaitAndSignal mutex(videoHandlerActive);

    // Set the buffer memory to zero to prevent
    // memory leak
    bufferRTP.SetSize(0);
    if(cacheMode==2) DetachCacheRTP();

    if (codec != NULL && codec->destroyCodec != NULL)
		(*codec->destroyCodec)(codec, context);
    codec = NULL;
//    if (converter!=NULL) delete converter;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void H323PluginVideoCodec::ConvertCIF4ToCIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;

  // copy Y
  for (y = CIF_HEIGHT; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    for (x = CIF_WIDTH; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = (unsigned char)val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }

  // copy U
  for (y = CIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    for (x = CIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = (unsigned char)val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }

  // copy V
  for (y = CIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    for (x = CIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = (unsigned char)val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL H323PluginVideoCodec::Read(BYTE * buffer, unsigned & length, RTP_DataFrame & dst)
{
    PWaitAndSignal mutex(videoHandlerActive);

    if (direction != Encoder) {
        PTRACE(1, "Plugin\tAttempt to decode from encoder");
        return FALSE;
    }

    if (rawDataChannel == NULL) {
        PTRACE(1, "PLUGIN\tNo channel to grab from, close down video transmission thread");
        return FALSE;
    }

    PVideoChannel *videoIn = (PVideoChannel *)rawDataChannel;
    int vw = videoIn->GetGrabWidth();
    int vh = videoIn->GetGrabHeight();
    if (vw == 0 || vh == 0) {
      PTRACE(1,"PLUGIN\tVideo grab dimension is 0, close down video transmission thread");
      videoIn->EnableAccess();
      return FALSE;
    }

    PluginCodec_Video_FrameHeader * frameHeader = (PluginCodec_Video_FrameHeader *)bufferRTP.GetPayloadPtr();
    frameHeader->x = 0;
    frameHeader->y = 0;

/*
    if ((encoderCacheKey&0x000000FE)==0)
    {
     if(strstr(codec->descr,"263p")!=NULL) encoderCacheKey|=64;
     else if(strstr(codec->descr,"263P")!=NULL) encoderCacheKey|=64;
     else if(strstr(codec->descr,"263")!=NULL) encoderCacheKey|=48;
     else if(strstr(codec->descr,"264")!=NULL) encoderCacheKey|=80;
     else if(strstr(codec->descr,"261")!=NULL) encoderCacheKey|=32;
     else encoderCacheKey|=16;
     if(frameWidth==CIF_WIDTH) encoderCacheKey|=8;
     else if(frameWidth==CIF4_WIDTH) encoderCacheKey|=10;
     else if(frameWidth==CIF16_WIDTH) encoderCacheKey|=12;
     else if(frameWidth==QCIF_WIDTH) encoderCacheKey|=6;
     else if(frameWidth==SQCIF_WIDTH) encoderCacheKey|=4;
     else encoderCacheKey|=2;
     cout << "Key " << encoderCacheKey << "\n";
    }
*/
    if(lastPacketSent)
    {
      if(cacheMode == 3)
      {
        encoderCacheKey++; if(encoderCacheKey >= 125) sendIntra = TRUE;
        if(sendIntra == TRUE) encoderCacheKey = 0;
      }

      if(cacheMode == 0 || cacheMode == 1 || cacheMode == 3 || encoderSeqN == 0 || encoderSeqN == 0xFFFFFFFF)
      {
        videoIn->RestrictAccess();

        if (!videoIn->IsGrabberOpen()) {
            PTRACE(1, "PLUGIN\tVideo grabber is not initialised, close down video transmission thread");
            videoIn->EnableAccess();
            return FALSE;
        }

        if (!SetFrameSize(frameWidth, frameHeight)) {
            PTRACE(1, "PLUGIN\tFailed to resize, close down video transmission thread");
            videoIn->EnableAccess();
            return FALSE;
        }

	if(frameWidth > vw && frameHeight > vh)
	  SetFrameSize(vw, vh);

        unsigned char * data = OPAL_VIDEO_FRAME_DATA_PTR(frameHeader);
        unsigned byteRead = (vw * vh * 3)/2;
        if (!rawDataChannel->Read(data, byteRead)) {
            PTRACE(3, "PLUGIN\tFailed to read data from video grabber");
            videoIn->EnableAccess();
            length=0;
            dst.SetPayloadSize(0);
            return TRUE; // and hope the error condition will fix itself
        }

        videoIn->EnableAccess();

        RenderFrame(data);

        if(converter==NULL && (vw!=frameWidth || vh!=frameHeight))
        {
         if(vw==CIF4_WIDTH && vh==CIF4_HEIGHT && frameWidth==CIF_WIDTH && frameHeight==CIF_HEIGHT)
         {
          ConvertCIF4ToCIF(data,data);
         }
         else
         {
          converter = PColourConverter::Create("YUV420P", "YUV420P", vw, vh);
          converter->SetDstFrameSize(frameWidth, frameHeight, TRUE);
          int w=frameWidth;
          int h=frameHeight;
          frameWidth=0; frameHeight=0;
          if (!SetFrameSize(w, h)) {
             PTRACE(1, "PLUGIN\tFailed to resize, close down video transmission thread");
             videoIn->EnableAccess(); 
             return FALSE; 
          }
         }
        }
        if(converter!=NULL)
        {
         PINDEX bytesReturned = (frameWidth * frameHeight * 3)/2;
         PINDEX amount = (vw * vh * 3)/2;
         converter->Convert(data, data, amount, &bytesReturned);
        }

      }
      PTimeInterval now = PTimer::Tick();
      if (lastFrameTick != 0)
        lastFrameTimeRTP = (now - lastFrameTick).GetInterval()*90;
      lastFrameTick = now;
    }
    else
      lastFrameTimeRTP = 0;

    frameHeader->width = frameWidth;
    frameHeader->height = frameHeight;

    // get the size of the output buffer
    int outputDataSize;
    if (codec == NULL || !CallCodecControl(codec, context, GET_OUTPUT_DATA_SIZE_CONTROL, NULL, NULL, outputDataSize))
      outputDataSize = 1518-14-4-8-20-16; // Max Ethernet packet (1518 bytes) minus 802.3/CRC, 802.3, IP, UDP headers

    dst.SetMinSize(outputDataSize);

    unsigned int fromLen = bufferRTP.GetHeaderSize() + bufferRTP.GetPayloadSize();
    unsigned int toLen = outputDataSize;
    unsigned int flags = sendIntra ? PluginCodec_CoderForceIFrame : 0;
    int retval = 0;

    if(cacheMode == 1)
    {
      GetFastUpdate(&formatString,flags,codecCache);
      sendIntra = (flags&PluginCodec_CoderForceIFrame)? 1 : 0;
    }

    if(cacheMode == 0 || cacheMode == 1 || cacheMode == 3)// || encoderSeqN == 0xFFFFFFFF)
      retval = (codec->codecFunction)(codec, context, bufferRTP.GetPointer(), &fromLen, dst.GetPointer(), &toLen, &flags);

    if(cacheMode == 1)
      PutCacheRTP(&formatString,dst,toLen,flags,codecCache);

    if(cacheMode == 2 && encoderSeqN != 0xFFFFFFFF)
    {
      GetCacheRTP(&formatString,dst,toLen,encoderSeqN,flags,codecCache);
      retval = 1;
    }

    if (retval == 0 && codec != NULL) {
        PTRACE(3,"PLUGIN\tError encoding frame from plugin " << codec->descr << " cacheMode=" << cacheMode);
        length = 0;
        return FALSE;
    }

    if ((flags & PluginCodec_ReturnCoderIFrame) != 0) {
        //PTRACE(sendIntra ? 3 : 5,"PLUGIN\tSent I-Frame" << (sendIntra ? ", in response to VideoFastUpdate" : ""));
        sendIntra = false;
    }

    if (toLen > 0)
        length = toLen - dst.GetHeaderSize();
    else
        length = 0;

    lastPacketSent = (flags & PluginCodec_ReturnCoderLastFrame);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL H323PluginVideoCodec::Write(const BYTE * buffer, unsigned length, const RTP_DataFrame & src, unsigned & written)
{
  PWaitAndSignal mutex(videoHandlerActive);

  if (direction != Decoder) {
    PTRACE(1, "PLUGIN\tAttempt to decode from decoder");
    return FALSE;
  }

  if (rawDataChannel == NULL) {
    PTRACE(1, "PLUGIN\tNo channel to render to, close down video reception thread");
    return FALSE;
  }

  if (length == 0) {
    written = length;
    return TRUE;
  }

  // get the size of the output buffer
  int outputDataSize;
  if (!CallCodecControl(codec, context, GET_OUTPUT_DATA_SIZE_CONTROL, NULL, NULL, outputDataSize))
    return FALSE;

  bufferRTP.SetMinSize(outputDataSize);

  unsigned int fromLen = src.GetHeaderSize() + src.GetPayloadSize();
  unsigned int toLen = bufferRTP.GetSize();
  unsigned int flags = freezeVideo;

  PTRACE(9, "PLUGIN\t" << formatString << ", Write " << fromLen << " " << src.GetHeaderSize() << "+" << src.GetPayloadSize());

  int retval = (codec->codecFunction)(codec, context,
                                      (const BYTE *)src, &fromLen,
                                      bufferRTP.GetPointer(toLen), &toLen,
                                      &flags);

  if (retval == 0) {
    PTRACE(3,"PLUGIN\tError decoding frame from plugin " << codec->descr);
    return FALSE;
  }

  PluginCodec_Video_FrameHeader * h = (PluginCodec_Video_FrameHeader *)(bufferRTP.GetPayloadPtr());

  if (flags & PluginCodec_ReturnCoderRequestIFrame) {
    PTRACE(6,"PLUGIN\tIFrame Request Decoder: Unimplemented.");
    logicalChannel->SendMiscCommand(H245_MiscellaneousCommand_type::e_videoFastUpdatePicture);
  }

  if (toLen < (unsigned)bufferRTP.GetHeaderSize()) {
    PTRACE(9,"PLUGIN\tPartial Frame received " << codec->descr << " Ignoring rendering.");
    written = length;
    return TRUE;
  }

  if(h->width != (unsigned int)frameWidth || h->height != (unsigned int)frameHeight)
  {
    PVideoChannel *videoOut = (PVideoChannel *)rawDataChannel;
    SetFrameSize(h->width,h->height);
    videoOut->SetRenderFrameSize(frameWidth, frameHeight);
  }

  if (flags & PluginCodec_ReturnCoderLastFrame) {
    SetFrameSize(h->width,h->height);
    RenderFrame(OPAL_VIDEO_FRAME_DATA_PTR(h));
  }

  written = length;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL H323PluginVideoCodec::RenderFrame(const BYTE * buffer)
{
    PVideoChannel *videoOut = (PVideoChannel *)rawDataChannel; // guaranteed to be non-NULL when called from Read() or Write()

    if (!videoOut->IsRenderOpen())
        return TRUE;

//    videoOut->SetRenderFrameSize(frameWidth, frameHeight);
//    videoOut->SetRenderFrameSize(videoOut->GetGrabWidth(), videoOut->GetGrabHeight());

    PTRACE(9, "PLUGIN\tWrite data to video renderer");
    return videoOut->Write(buffer, 0 /*unused parameter*/);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL H323PluginVideoCodec::SetFrameSize(int _width, int _height)
{
    if ((frameWidth == _width) && (frameHeight == _height))
        return TRUE;
        
    if ((_width == 0) || (_height == 0))
        return FALSE;
        
    if ((_width > maxWidth) || (_height > maxHeight)) {
        PTRACE(3, "PLUGIN\tERROR: Frame Size " << _width << "x" << _height  << " exceeds codec limits " << maxWidth << "x" << maxHeight); 
        return FALSE;
    }

    frameWidth = _width;
    frameHeight = _height;

    PTRACE(3,"PLUGIN\tResize to w:" << frameWidth << " h:" << frameHeight); 

    mediaFormat.SetOptionInteger(OpalVideoFormat::FrameWidthOption,frameWidth); 
    mediaFormat.SetOptionInteger(OpalVideoFormat::FrameHeightOption,frameHeight); 

    bytesPerFrame = (frameHeight * frameWidth * 3)/2;

    if (direction == Encoder) {
        bufferRTP.SetPayloadSize(sizeof(PluginCodec_Video_FrameHeader) + bytesPerFrame);
        PluginCodec_Video_FrameHeader * header = 
                    (PluginCodec_Video_FrameHeader *)(bufferRTP.GetPayloadPtr());
        header->x = header->y = 0;
        header->width = frameWidth;
        header->height = frameHeight;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned H323PluginVideoCodec::GetVideoMode(void) 
{ 
   if (mediaFormat.GetOptionBoolean(OpalVideoFormat::DynamicVideoQualityOption))
      return H323VideoCodec::DynamicVideoQuality;
   else if (mediaFormat.GetOptionBoolean(OpalVideoFormat::AdaptivePacketDelayOption))
      return H323VideoCodec::AdaptivePacketDelay;
   else
      return H323VideoCodec::None;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void H323PluginVideoCodec::SetVideoMode(int mode) 
{ 

    switch (mode) {
      case H323VideoCodec::DynamicVideoQuality : 
        SetCodecControl(codec, context, SET_CODEC_OPTIONS_CONTROL, "Dynamic Video Quality", mode); 
        break; 
      case H323VideoCodec::AdaptivePacketDelay :
        SetCodecControl(codec, context, SET_CODEC_OPTIONS_CONTROL, "Adaptive Packet Delay", mode);
        break;
      default:
        break;
     }
}

#endif // H323_VIDEO

////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// Helper class for handling plugin capabilities
//

class H323PluginCapabilityInfo
{
  public:
    H323PluginCapabilityInfo(PluginCodec_Definition * _encoderCodec,
                             PluginCodec_Definition * _decoderCodec);

    H323PluginCapabilityInfo(const PString & _mediaFormat, 
                             const PString & _baseName);

    const PString & GetFormatName() const
    { return capabilityFormatName; }

    H323Codec * CreateCodec(const OpalMediaFormat & mediaFormat, H323Codec::Direction direction) const;

  protected:
    PluginCodec_Definition * encoderCodec;
    PluginCodec_Definition * decoderCodec;
    PString                  capabilityFormatName;
    PString                  mediaFormatName;
};

#ifndef NO_H323_AUDIO

//////////////////////////////////////////////////////////////////////////////
//
// Class for handling most audio plugin capabilities
//

class H323AudioPluginCapability : public H323AudioCapability,
                                  public H323PluginCapabilityInfo
{
  PCLASSINFO(H323AudioPluginCapability, H323AudioCapability);
  public:
    H323AudioPluginCapability(PluginCodec_Definition * _encoderCodec,
                         PluginCodec_Definition * _decoderCodec,
                         unsigned _pluginSubType)
      : H323AudioCapability(_decoderCodec->parm.audio.maxFramesPerPacket, _encoderCodec->parm.audio.recommendedFramesPerPacket), 
        H323PluginCapabilityInfo(_encoderCodec, _decoderCodec),
        pluginSubType(_pluginSubType)
      { }

    // this constructor is only used when creating a capability without a codec
    H323AudioPluginCapability(const PString & _mediaFormat, const PString & _baseName,
                         unsigned maxFramesPerPacket, unsigned recommendedFramesPerPacket,
                         unsigned _pluginSubType)
      : H323AudioCapability(maxFramesPerPacket, recommendedFramesPerPacket), 
        H323PluginCapabilityInfo(_mediaFormat, _baseName),
        pluginSubType(_pluginSubType)
      { 
        for (PINDEX i = 0; audioMaps[i].pluginCapType >= 0; i++) {
          if (audioMaps[i].pluginCapType == (int)_pluginSubType) { 
            h323subType = audioMaps[i].h323SubType;
            break;
          }
        }
        rtpPayloadType = OpalMediaFormat(_mediaFormat).GetPayloadType();
      }

    virtual PObject * Clone() const
    { return new H323AudioPluginCapability(*this); }

    virtual PString GetFormatName() const
    { return H323PluginCapabilityInfo::GetFormatName();}

    virtual H323Codec * CreateCodec(H323Codec::Direction direction) const
    { return H323PluginCapabilityInfo::CreateCodec(GetMediaFormat(), direction); }

    virtual unsigned GetSubType() const
    { return pluginSubType; }

  protected:
    unsigned pluginSubType;
    unsigned h323subType;   // only set if using capability without codec
};

#ifndef NO_H323_AUDIO_CODECS

//////////////////////////////////////////////////////////////////////////////
//
// Class for handling non standard audio capabilities
//

class H323CodecPluginNonStandardAudioCapability : public H323NonStandardAudioCapability,
                                                  public H323PluginCapabilityInfo
{
  PCLASSINFO(H323CodecPluginNonStandardAudioCapability, H323NonStandardAudioCapability);
  public:
    H323CodecPluginNonStandardAudioCapability(
                                   PluginCodec_Definition * _encoderCodec,
                                   PluginCodec_Definition * _decoderCodec,
                                   H323NonStandardCapabilityInfo::CompareFuncType compareFunc,
                                   const unsigned char * data, unsigned dataLen);

    H323CodecPluginNonStandardAudioCapability(
                                   PluginCodec_Definition * _encoderCodec,
                                   PluginCodec_Definition * _decoderCodec,
                                   const unsigned char * data, unsigned dataLen);

    virtual PObject * Clone() const
    { return new H323CodecPluginNonStandardAudioCapability(*this); }

    virtual PString GetFormatName() const
    { return H323PluginCapabilityInfo::GetFormatName();}

    virtual H323Codec * CreateCodec(H323Codec::Direction direction) const
    { return H323PluginCapabilityInfo::CreateCodec(GetMediaFormat(), direction); }
};


//////////////////////////////////////////////////////////////////////////////
//
// Class for handling generic audio capabilities
//

class H323CodecPluginGenericAudioCapability : public H323GenericAudioCapability,
                          public H323PluginCapabilityInfo
{
  PCLASSINFO(H323CodecPluginGenericAudioCapability, H323GenericAudioCapability);
  public:
    H323CodecPluginGenericAudioCapability(
                                   const PluginCodec_Definition * _encoderCodec,
                                   const PluginCodec_Definition * _decoderCodec,
                   const PluginCodec_H323GenericCodecData * data );

    virtual PObject * Clone() const
    {
      return new H323CodecPluginGenericAudioCapability(*this);
    }

    virtual PString GetFormatName() const
    { return H323PluginCapabilityInfo::GetFormatName();}

    virtual H323Codec * CreateCodec(H323Codec::Direction direction) const
    { return H323PluginCapabilityInfo::CreateCodec(GetMediaFormat(), direction); }
};

//////////////////////////////////////////////////////////////////////////////
//
// Class for handling G.723.1 codecs
//

class H323PluginG7231Capability : public H323AudioPluginCapability
{
  PCLASSINFO(H323PluginG7231Capability, H323AudioPluginCapability);
  public:
    H323PluginG7231Capability(PluginCodec_Definition * _encoderCodec,
                               PluginCodec_Definition * _decoderCodec,
                               BOOL _annexA = TRUE)
      : H323AudioPluginCapability(_encoderCodec, _decoderCodec, H245_AudioCapability::e_g7231),
        annexA(_annexA)
      { }

    Comparison Compare(const PObject & obj) const
    {
      if (!PIsDescendant(&obj, H323PluginG7231Capability))
        return LessThan;

      Comparison result = H323AudioCapability::Compare(obj);
      if (result != EqualTo)
        return result;

      BOOL otherAnnexA = ((const H323PluginG7231Capability &)obj).annexA;
      if (annexA == otherAnnexA)
        return EqualTo;

      if (annexA)
        return GreaterThan;

      return EqualTo;
    }

    virtual PObject * Clone() const
    { 
      return new H323PluginG7231Capability(*this);
    }

    virtual BOOL OnSendingPDU(H245_AudioCapability & cap, unsigned packetSize) const
    {
      cap.SetTag(H245_AudioCapability::e_g7231);
      H245_AudioCapability_g7231 & g7231 = cap;
      g7231.m_maxAl_sduAudioFrames = packetSize;
      g7231.m_silenceSuppression = annexA;
      return TRUE;
    }

    virtual BOOL OnReceivedPDU(const H245_AudioCapability & cap,  unsigned & packetSize)
    {
      if (cap.GetTag() != H245_AudioCapability::e_g7231)
        return FALSE;
      const H245_AudioCapability_g7231 & g7231 = cap;
      packetSize = g7231.m_maxAl_sduAudioFrames;
      annexA = g7231.m_silenceSuppression;
      return TRUE;
    }

  protected:
    BOOL annexA;
};

//////////////////////////////////////////////////////////////////////////////
//
// Class for handling GSM plugin capabilities
//

class H323GSMPluginCapability : public H323AudioPluginCapability
{
  PCLASSINFO(H323GSMPluginCapability, H323AudioPluginCapability);
  public:
    H323GSMPluginCapability(PluginCodec_Definition * _encoderCodec,
                            PluginCodec_Definition * _decoderCodec,
                            int _pluginSubType, int _comfortNoise, int _scrambled)
      : H323AudioPluginCapability(_encoderCodec, _decoderCodec, _pluginSubType),
        comfortNoise(_comfortNoise), scrambled(_scrambled)
    { }

    Comparison Compare(const PObject & obj) const;

    virtual PObject * Clone() const
    {
      return new H323GSMPluginCapability(*this);
    }

    virtual BOOL OnSendingPDU(
      H245_AudioCapability & pdu,  /// PDU to set information on
      unsigned packetSize          /// Packet size to use in capability
    ) const;

    virtual BOOL OnReceivedPDU(
      const H245_AudioCapability & pdu,  /// PDU to get information from
      unsigned & packetSize              /// Packet size to use in capability
    );
  protected:
    int comfortNoise;
    int scrambled;
};

#endif // NO_H323_AUDIO_CODECS

#endif // NO_H323_AUDIO

#ifdef  H323_VIDEO

//////////////////////////////////////////////////////////////////////////////
//
// Class for handling most video plugin capabilities
//

class H323VideoPluginCapability : public H323VideoCapability,
                                  public H323PluginCapabilityInfo
{
  PCLASSINFO(H323VideoPluginCapability, H323VideoCapability);
  public:
    H323VideoPluginCapability(PluginCodec_Definition * _encoderCodec,
                              PluginCodec_Definition * _decoderCodec,
                              unsigned _pluginSubType)
      : H323VideoCapability(), 
           H323PluginCapabilityInfo(_encoderCodec, _decoderCodec),
        pluginSubType(_pluginSubType)
      { 
        SetCommonOptions(GetWritableMediaFormat(),encoderCodec->parm.video.maxFrameWidth, encoderCodec->parm.video.maxFrameHeight, encoderCodec->parm.video.recommendedFrameRate);
        PopulateMediaFormatOptions(encoderCodec,GetWritableMediaFormat());

        rtpPayloadType = (RTP_DataFrame::PayloadTypes)(((_encoderCodec->flags & PluginCodec_RTPTypeMask) == PluginCodec_RTPTypeDynamic) ? RTP_DataFrame::DynamicBase : _encoderCodec->rtpPayload);
      }

#if 0
    // this constructor is only used when creating a capability without a codec
    H323VideoPluginCapability(const PString & _mediaFormat, const PString & _baseName,
                         unsigned maxFramesPerPacket, unsigned /*recommendedFramesPerPacket*/,
                         unsigned _pluginSubType)
      : H323VideoCapability(), 
        H323PluginCapabilityInfo(_mediaFormat, _baseName),
        pluginSubType(_pluginSubType)
      { 
        for (PINDEX i = 0; audioMaps[i].pluginCapType >= 0; i++) {
          if (videoMaps[i].pluginCapType == (int)_pluginSubType) { 
            h323subType = audioMaps[i].h323SubType;
            break;
          }
        }
        rtpPayloadType = OpalMediaFormat(_mediaFormat).GetPayloadType();
      }
#endif

    virtual PString GetFormatName() const
    { return H323PluginCapabilityInfo::GetFormatName();}

    virtual unsigned GetSubType() const
    { return pluginSubType; }


    static BOOL SetCommonOptions(OpalMediaFormat & mediaFormat, int frameWidth, int frameHeight, int frameRate)
    {
        if (!mediaFormat.SetOptionInteger(OpalVideoFormat::FrameWidthOption, frameWidth)) {
           PTRACE(3,"PLUGIN Error setting " << OpalVideoFormat::FrameWidthOption << " to " << frameWidth);
           return FALSE;
        }

        if (!mediaFormat.SetOptionInteger(OpalVideoFormat::FrameHeightOption, frameHeight)) {
           PTRACE(3,"PLUGIN Error setting " << OpalVideoFormat::FrameHeightOption << " to " << frameHeight);
           return FALSE;
        }

        if (!mediaFormat.SetOptionInteger(OpalVideoFormat::FrameTimeOption, (int)(OpalMediaFormat::VideoTimeUnits * 1000 * 100 * frameRate / 2997))){
           PTRACE(3,"PLUGIN Error setting " << OpalVideoFormat::FrameTimeOption << " to " << (int)(OpalMediaFormat::VideoTimeUnits * 100 * frameRate / 2997));
           return FALSE;
        } 

      return TRUE;
    }

     virtual BOOL SetMaxFrameSize(CapabilityFrameSize framesize, int frameunits = 1)
     {
         PString param;
         switch (framesize) {
             case sqcifMPI : param = sqcifMPI_tag; break;
             case  qcifMPI : param = qcifMPI_tag; break;
             case   cifMPI : param = cifMPI_tag; break;
             case  cif4MPI : param = cif4MPI_tag; break;
             case cif16MPI : param = cif16MPI_tag; break;
			 case  i480MPI : param = cif4MPI_tag; break;
             case  p720MPI : param = cif16MPI_tag; break;
             case i1080MPI : param = i1080MPI_tag; break;
             default: return FALSE;
         }

         SetCodecControl(encoderCodec, NULL, SET_CODEC_OPTIONS_CONTROL, param, frameunits);
         SetCodecControl(decoderCodec, NULL, SET_CODEC_OPTIONS_CONTROL, param, frameunits);
         PopulateMediaFormatOptions(encoderCodec,GetWritableMediaFormat());
         return TRUE;
     }
   
    virtual H323Codec * CreateCodec(H323Codec::Direction direction) const
    { return H323PluginCapabilityInfo::CreateCodec(GetMediaFormat(), direction); }

  protected:
    unsigned pluginSubType;
    unsigned h323subType;   // only set if using capability without codec
};

//////////////////////////////////////////////////////////////////////////////
//
// Class for handling non standard video capabilities
//

class H323CodecPluginNonStandardVideoCapability : public H323NonStandardVideoCapability,
                                                  public H323PluginCapabilityInfo
{
  PCLASSINFO(H323CodecPluginNonStandardVideoCapability, H323NonStandardVideoCapability);
  public:
    H323CodecPluginNonStandardVideoCapability(
                                   PluginCodec_Definition * _encoderCodec,
                                   PluginCodec_Definition * _decoderCodec,
                                   H323NonStandardCapabilityInfo::CompareFuncType compareFunc,
                                   const unsigned char * data, unsigned dataLen);

    H323CodecPluginNonStandardVideoCapability(
                                   PluginCodec_Definition * _encoderCodec,
                                   PluginCodec_Definition * _decoderCodec,
                                   const unsigned char * data, unsigned dataLen);

    virtual PObject * Clone() const
    { return new H323CodecPluginNonStandardVideoCapability(*this); }

    virtual PString GetFormatName() const
    { return H323PluginCapabilityInfo::GetFormatName();}
    
    virtual H323Codec * CreateCodec(H323Codec::Direction direction) const
    { return H323PluginCapabilityInfo::CreateCodec(GetMediaFormat(), direction); }
};

//////////////////////////////////////////////////////////////////////////////
//
// Class for handling generic video capabilities ie H.264 / MPEG4 part 2
//

class H323CodecPluginGenericVideoCapability : public H323GenericVideoCapability,
                                              public H323PluginCapabilityInfo
{
  PCLASSINFO(H323CodecPluginGenericVideoCapability, H323GenericVideoCapability);
  public:
    H323CodecPluginGenericVideoCapability(
                                   const PluginCodec_Definition * _encoderCodec,
                                   const PluginCodec_Definition * _decoderCodec,
                                   const PluginCodec_H323GenericCodecData * data );

    virtual PObject * Clone() const
    { return new H323CodecPluginGenericVideoCapability(*this); }

    virtual PString GetFormatName() const
    { return H323PluginCapabilityInfo::GetFormatName();}
    
    virtual H323Codec * CreateCodec(H323Codec::Direction direction) const
    { return H323PluginCapabilityInfo::CreateCodec(GetMediaFormat(), direction); }

    static BOOL SetCommonOptions(OpalMediaFormat & mediaFormat, int frameWidth, int frameHeight, int frameRate)
    {
        if (!mediaFormat.SetOptionInteger(OpalVideoFormat::FrameWidthOption, frameWidth)) {
           PTRACE(3,"PLUGIN Error setting " << OpalVideoFormat::FrameWidthOption << " to " << frameWidth);
           return FALSE;
        }

        if (!mediaFormat.SetOptionInteger(OpalVideoFormat::FrameHeightOption, frameHeight)) {
           PTRACE(3,"PLUGIN Error setting " << OpalVideoFormat::FrameHeightOption << " to " << frameHeight);
           return FALSE;
        }

        if (!mediaFormat.SetOptionInteger(OpalVideoFormat::FrameTimeOption, (int)(OpalMediaFormat::VideoTimeUnits * 1000 * 100 * frameRate / 2997))){
           PTRACE(3,"PLUGIN Error setting " << OpalVideoFormat::FrameTimeOption << " to " << (int)(OpalMediaFormat::VideoTimeUnits * 100 * frameRate / 2997));
           return FALSE;
        } 

      return TRUE;
    }

    virtual void LoadGenericData(const PluginCodec_H323GenericCodecData *ptr);

    virtual BOOL SetMaxFrameSize(CapabilityFrameSize framesize, int frameunits = 1);
};

//////////////////////////////////////////////////////////////////////////////
//
// Class for handling H.261 plugin capabilities
//

class H323H261PluginCapability : public H323VideoPluginCapability
{
  PCLASSINFO(H323H261PluginCapability, H323VideoPluginCapability);
  public:
    H323H261PluginCapability(PluginCodec_Definition * _encoderCodec,
                             PluginCodec_Definition * _decoderCodec);

    Comparison Compare(const PObject & obj) const;

    virtual PObject * Clone() const
    { 
      return new H323H261PluginCapability(*this); 
    }

    virtual BOOL OnSendingPDU(
      H245_VideoCapability & pdu  /// PDU to set information on
    ) const;

    virtual BOOL OnSendingPDU(
      H245_VideoMode & pdu
    ) const;

    virtual BOOL OnReceivedPDU(
      const H245_VideoCapability & pdu  /// PDU to get information from
    );

    PluginCodec_Definition * enc;
};

//////////////////////////////////////////////////////////////////////////////
//
// Class for handling H.263 plugin capabilities
//

class H323H263PluginCapability : public H323VideoPluginCapability
{
  PCLASSINFO(H323H263PluginCapability, H323VideoPluginCapability);
  public:
    H323H263PluginCapability(PluginCodec_Definition * _encoderCodec,
                             PluginCodec_Definition * _decoderCodec);

    Comparison Compare(const PObject & obj) const;

    virtual PObject * Clone() const
    { return new H323H263PluginCapability(*this); }

    virtual BOOL OnSendingPDU(
      H245_VideoCapability & pdu  /// PDU to set information on
    ) const;

    virtual BOOL OnSendingPDU(
      H245_VideoMode & pdu
    ) const;

    virtual BOOL OnReceivedPDU(
      const H245_VideoCapability & pdu  /// PDU to get information from
    );
};

#endif //  H323_VIDEO

/////////////////////////////////////////////////////////////////////////////

class H323StaticPluginCodec
{
  public:
    virtual ~H323StaticPluginCodec() { }
    virtual PluginCodec_GetAPIVersionFunction Get_GetAPIFn() = 0;
    virtual PluginCodec_GetCodecFunction Get_GetCodecFn() = 0;
};

PMutex & H323PluginCodecManager::GetMediaFormatMutex()
{
  static PMutex mutex;
  return mutex;
}

H323PluginCodecManager::H323PluginCodecManager(PPluginManager * _pluginMgr)
 : PPluginModuleManager(PLUGIN_CODEC_GET_CODEC_FN_STR, _pluginMgr)
{
  // instantiate all of the media formats
  {
    OpalMediaFormatFactory::KeyList_T keyList = OpalMediaFormatFactory::GetKeyList();
    OpalMediaFormatFactory::KeyList_T::const_iterator r;
    for (r = keyList.begin(); r != keyList.end(); ++r) {
      OpalMediaFormat * instance = OpalMediaFormatFactory::CreateInstance(*r);
      if (instance == NULL) {
        PTRACE(4, "H323PLUGIN\tCannot instantiate opal media format " << *r);
      } else {
        PTRACE(4, "H323PLUGIN\tCreating media format " << *r);
      }
    }
  }
  
  // instantiate all of the static codecs
  {
    PFactory<H323StaticPluginCodec>::KeyList_T keyList = PFactory<H323StaticPluginCodec>::GetKeyList();
    PFactory<H323StaticPluginCodec>::KeyList_T::const_iterator r;
    for (r = keyList.begin(); r != keyList.end(); ++r) {
      H323StaticPluginCodec * instance = PFactory<H323StaticPluginCodec>::CreateInstance(*r);
      if (instance == NULL) {
        PTRACE(4, "H323PLUGIN\tCannot instantiate static codec plugin " << *r);
      } else {
        PTRACE(4, "H323PLUGIN\tLoading static codec plugin " << *r);
        RegisterStaticCodec(r->c_str(), instance->Get_GetAPIFn(), instance->Get_GetCodecFn());
      }
    }
  }

  // cause the plugin manager to load all dynamic plugins
  pluginMgr->AddNotifier(PCREATE_NOTIFIER(OnLoadModule), TRUE);
}

H323PluginCodecManager::~H323PluginCodecManager()
{
}

void H323PluginCodecManager::OnShutdown()
{
  // unregister the plugin media formats
  OpalMediaFormatFactory::UnregisterAll();
#ifdef H323_VIDEO
#ifdef H323_H239
  H323ExtendedVideoFactory::UnregisterAll();
#endif
#endif
  // unregister the plugin capabilities
  H323CapabilityFactory::UnregisterAll();
}

void H323PluginCodecManager::OnLoadPlugin(PDynaLink & dll, INT code)
{
  PDynaLink::Function getCodecs; //  PluginCodec_GetCodecFunction
  if (!dll.GetFunction(PString(signatureFunctionName), getCodecs))
  {
    PTRACE(3, "H323PLUGIN\tPlugin Codec DLL " << dll.GetName() << " is not a plugin codec");
    return;
  }

  unsigned int count;
  PluginCodec_Definition * codecs = (*((PluginCodec_GetCodecFunction)getCodecs))(&count, PLUGIN_CODEC_VERSION_OPTIONS);
  if (codecs == NULL || count == 0) {
    PTRACE(3, "H323PLUGIN\tPlugin Codec DLL " << dll.GetName() << " contains no codec definitions");
    return;
  } 

  PTRACE(3, "H323PLUGIN\tLoading plugin codec " << dll.GetName());

  switch (code) {

    // plugin loaded
    case 0:
      RegisterCodecs(count, codecs);
      break;

    // plugin unloaded
    case 1:
      UnregisterCodecs(count, codecs);
      break;

    default:
      break;
  }
}

void H323PluginCodecManager::RegisterStaticCodec(
      const char * name,
      PluginCodec_GetAPIVersionFunction /*getApiVerFn*/,
      PluginCodec_GetCodecFunction getCodecFn)
{
  unsigned int count;
  PluginCodec_Definition * codecs = (*getCodecFn)(&count, PLUGIN_CODEC_VERSION);
  if (codecs == NULL || count == 0) {
    PTRACE(3, "H323PLUGIN\tStatic codec " << name << " contains no codec definitions");
    return;
  } 

  RegisterCodecs(count, codecs);
}

void H323PluginCodecManager::RegisterCodecs(unsigned int count, void * _codecList)
{
  // make sure all non-timestamped codecs have the same concept of "now"
  static time_t codecNow = ::time(NULL);

  PluginCodec_Definition * codecList = (PluginCodec_Definition *)_codecList;
  unsigned i, j ;
  for (i = 0; i < count; i++) {

    PluginCodec_Definition & encoder = codecList[i];

    BOOL videoSupported = encoder.version >= PLUGIN_CODEC_VERSION_VIDEO;

    // for every encoder, we need a decoder
    BOOL found = FALSE;
    BOOL isEncoder = FALSE;
    if (encoder.h323CapabilityType != PluginCodec_H323Codec_undefined && (
         (
           ((encoder.flags & PluginCodec_MediaTypeMask) == PluginCodec_MediaTypeAudio) &&
            (strcmp(encoder.sourceFormat, "L16") == 0 ||
            strstr(encoder.sourceFormat, "PCM-16-") != NULL)
         ) ||
         (
           ((encoder.flags & PluginCodec_MediaTypeMask) == PluginCodec_MediaTypeAudioStreamed) &&
            (strcmp(encoder.sourceFormat, "L16") == 0 ||
            strstr(encoder.sourceFormat, "PCM-16-") != NULL)
         ) ||
         (
           videoSupported &&
           ((encoder.flags & PluginCodec_MediaTypeMask) == PluginCodec_MediaTypeVideo) && 
           strcmp(encoder.sourceFormat, "YUV420P") == 0
        )
      )) {
      isEncoder = TRUE;
      for (j = 0; j < count; j++) {

        PluginCodec_Definition & decoder = codecList[j];
        if (
            (decoder.h323CapabilityType == encoder.h323CapabilityType) &&
            ((decoder.flags & PluginCodec_MediaTypeMask) == (encoder.flags & PluginCodec_MediaTypeMask)) &&
            (strcmp(decoder.sourceFormat, encoder.destFormat) == 0) &&
            (strcmp(decoder.destFormat,   encoder.sourceFormat) == 0)
            )
          { 

          // deal with codec having no info, or timestamp in future
          time_t timeStamp = codecList[i].info == NULL ? codecNow : codecList[i].info->timestamp;
          if (timeStamp > codecNow)
            timeStamp = codecNow;

          // create the capability and media format associated with this plugin
          CreateCapabilityAndMediaFormat(&encoder, &decoder);
          found = TRUE;
          
          PTRACE(2, "H323PLUGIN\tPlugin codec " << encoder.descr << " defined");
          break;
        }
      }
    }
    if (!found && isEncoder) {
      PTRACE(2, "H323PLUGIN\tCannot find decoder for plugin encoder " << encoder.descr);
    }
  }
}

void H323PluginCodecManager::UnregisterCodecs(unsigned int /*count*/, void * /*codec*/)
{
}

void H323PluginCodecManager::AddFormat(OpalMediaFormat * fmt)
{
  PWaitAndSignal m(H323PluginCodecManager::GetMediaFormatMutex());
  H323PluginCodecManager::GetMediaFormatList().Append(fmt);
}

void H323PluginCodecManager::AddFormat(const OpalMediaFormat & fmt)
{
  PWaitAndSignal m(H323PluginCodecManager::GetMediaFormatMutex());
  H323PluginCodecManager::GetMediaFormatList().Append(new OpalMediaFormat(fmt));
}

OpalMediaFormat::List H323PluginCodecManager::GetMediaFormats() 
{
  PWaitAndSignal m(H323PluginCodecManager::GetMediaFormatMutex());
  OpalMediaFormat::List & list = H323PluginCodecManager::GetMediaFormatList();
  OpalMediaFormat::List copy;
  for (PINDEX i = 0; i < list.GetSize(); i++)
    copy.Append(new OpalMediaFormat(list[i]));
  return copy;
}

OpalMediaFormat::List & H323PluginCodecManager::GetMediaFormatList()
{
  static OpalMediaFormat::List mediaFormatList;
  return mediaFormatList;
}


void H323PluginCodecManager::CreateCapabilityAndMediaFormat(
       PluginCodec_Definition * encoderCodec,
       PluginCodec_Definition * decoderCodec
) 
{
  // make sure all non-timestamped codecs have the same concept of "now"
  static time_t mediaNow = time(NULL);

  // deal with codec having no info, or timestamp in future
  time_t timeStamp = encoderCodec->info == NULL ? mediaNow : encoderCodec->info->timestamp;
  if (timeStamp > mediaNow)
    timeStamp = mediaNow;

  unsigned defaultSessionID = 0;
  BOOL jitter = FALSE;
#ifdef H323_VIDEO
  BOOL extended = FALSE;
#endif
  unsigned frameTime = 0;
  unsigned timeUnits = 0;
  switch (encoderCodec->flags & PluginCodec_MediaTypeMask) {
#ifdef H323_VIDEO
    case PluginCodec_MediaTypeVideo:
      defaultSessionID = OpalMediaFormat::DefaultVideoSessionID;
	  if ((encoderCodec->flags & PluginCodec_MediaExtensionMask) == PluginCodec_MediaTypeExtVideo)
		    extended = TRUE;
      jitter = FALSE;
      break;
#endif
#ifndef NO_H323_AUDIO
    case PluginCodec_MediaTypeAudio:
    case PluginCodec_MediaTypeAudioStreamed:
      defaultSessionID = OpalMediaFormat::DefaultAudioSessionID;
      jitter = TRUE;
      frameTime = (OpalMediaFormat::AudioTimeUnits * encoderCodec->usPerFrame) / 1000;
      timeUnits = encoderCodec->sampleRate / 1000; // OpalMediaFormat::AudioTimeUnits;
      break;
#endif
    default:
      break;
  }

  // add the media format
  if (defaultSessionID == 0) {
    PTRACE(3, "H323PLUGIN\tCodec DLL provides unknown media format " << (int)(encoderCodec->flags & PluginCodec_MediaTypeMask));
  } else {
    PString fmtName = CreateCodecName(encoderCodec, FALSE);
    OpalMediaFormat existingFormat(fmtName, TRUE);
    if (existingFormat.IsValid()) {
      PTRACE(3, "H323PLUGIN\tMedia format " << fmtName << " already exists");
      H323PluginCodecManager::AddFormat(existingFormat);
    } else {
      PTRACE(3, "H323PLUGIN\tCreating new media format " << fmtName);

      OpalMediaFormat * mediaFormat = NULL;

      // manually register the new singleton type, as we do not have a concrete type
      switch (encoderCodec->flags & PluginCodec_MediaTypeMask) {
#ifdef H323_VIDEO
        case PluginCodec_MediaTypeVideo:
          mediaFormat = new OpalPluginVideoMediaFormat(
                                  encoderCodec, 
                                  encoderCodec->sdpFormat,
                                  timeStamp);
          break;
#endif
#ifndef NO_H323_AUDIO
        case PluginCodec_MediaTypeAudio:
        case PluginCodec_MediaTypeAudioStreamed:
          mediaFormat = new OpalPluginAudioMediaFormat(
                                   encoderCodec,
                                   defaultSessionID,
                                   jitter,
                                   frameTime,
                                   timeUnits,
                                   timeStamp);
          break;
#endif
        default:
          break;
      }
      // if the codec has been flagged to use a shared RTP payload type, then find a codec with the same SDP name
      // and use that RTP code rather than creating a new one. That prevents codecs (like Speex) from consuming
      // dozens of dynamic RTP types
      if ((encoderCodec->flags & PluginCodec_RTPTypeShared) != 0) {
        PWaitAndSignal m(H323PluginCodecManager::GetMediaFormatMutex());
        OpalMediaFormat::List & list = H323PluginCodecManager::GetMediaFormatList();
        for (PINDEX i = 0; i < list.GetSize(); i++) {
          OpalMediaFormat * opalFmt = &list[i];
#ifndef NO_H323_AUDIO
         {
          OpalPluginAudioMediaFormat * fmt = dynamic_cast<OpalPluginAudioMediaFormat *>(opalFmt);
          if (
               (encoderCodec->sdpFormat != NULL) &&
               (fmt != NULL) && 
               (fmt->encoderCodec->sdpFormat != NULL) &&
               (strcasecmp(encoderCodec->sdpFormat, fmt->encoderCodec->sdpFormat) == 0)
              ) {
            mediaFormat->SetPayloadType(fmt->GetPayloadType());
            break;
                }
            }
#endif
#ifdef H323_VIDEO
          {
            OpalPluginVideoMediaFormat * fmt = dynamic_cast<OpalPluginVideoMediaFormat *>(opalFmt);
            if (fmt != NULL)
            {
            
  }
            if (
                (fmt != NULL) && 
                (encoderCodec->sampleRate == fmt->encoderCodec->sampleRate) &&
                (fmt->encoderCodec->sdpFormat != NULL) &&
                (strcasecmp(encoderCodec->sdpFormat, fmt->encoderCodec->sdpFormat) == 0)
                ) {
              mediaFormat->SetPayloadType(fmt->GetPayloadType());
              break;
            }
          }
#endif
    }
  }

      // save the format
      H323PluginCodecManager::AddFormat(mediaFormat);
    }
  }

  // add the capability
  H323CodecPluginCapabilityMapEntry * map = NULL;

  switch (encoderCodec->flags & PluginCodec_MediaTypeMask) {
#ifndef NO_H323_AUDIO_CODECS
    case PluginCodec_MediaTypeAudio:
    case PluginCodec_MediaTypeAudioStreamed:
      map = audioMaps;
      break;
#endif

#ifdef H323_VIDEO
    case PluginCodec_MediaTypeVideo:
      map = videoMaps;
      break;
#endif

    default:
      break;
  }

  if (map == NULL) {
    PTRACE(3, "H323PLUGIN\tCannot create capability for unknown plugin codec media format " << (int)(encoderCodec->flags & PluginCodec_MediaTypeMask));
  } else {
    for (PINDEX i = 0; map[i].pluginCapType >= 0; i++) {
      if (map[i].pluginCapType == encoderCodec->h323CapabilityType) {
        H323Capability * cap = NULL;
        if (map[i].createFunc != NULL)
        {
          cap = (*map[i].createFunc)(encoderCodec, decoderCodec, map[i].h323SubType);
        }
        else {
          switch (encoderCodec->flags & PluginCodec_MediaTypeMask) {
#ifndef NO_H323_AUDIO
            case PluginCodec_MediaTypeAudio:
            case PluginCodec_MediaTypeAudioStreamed:
              cap = new H323AudioPluginCapability(encoderCodec, decoderCodec, map[i].h323SubType);
              break;
#endif // NO_H323_AUDIO

#ifdef H323_VIDEO
            case PluginCodec_MediaTypeVideo:
              // all video caps are created using the create functions
              break;
#endif // H323_VIDEO

            default:
              break;
          }
        }

        // manually register the new singleton type, as we do not have a concrete type
        if (cap != NULL){
          H323CapabilityFactory::Register(CreateCodecName(encoderCodec, TRUE), cap);
          cout << CreateCodecName(encoderCodec, TRUE) << "\n";
#ifdef H323_VIDEO
#ifdef H323_H239
             if (extended)
                H323ExtendedVideoFactory::Register(CreateCodecName(encoderCodec, FALSE), (H323VideoCapability *)cap);
#endif
#endif
        }
        break;
      }
    }
  }

  // create the factories for the codecs 
  new OpalPluginCodecFactory::Worker(PString(encoderCodec->sourceFormat) + "|" + encoderCodec->destFormat, encoderCodec);
  new OpalPluginCodecFactory::Worker(PString(decoderCodec->sourceFormat) + "|" + decoderCodec->destFormat, decoderCodec);
}
/*
H323Capability * H323PluginCodecManager::CreateCapability(
          const PString & _mediaFormat, 
          const PString & _baseName,
                 unsigned maxFramesPerPacket, 
                 unsigned recommendedFramesPerPacket,
                 unsigned _pluginSubType
  )
{
  return new H323PluginCapability(_mediaFormat, _baseName,
                                  maxFramesPerPacket, recommendedFramesPerPacket, _pluginSubType);
}
*/
/////////////////////////////////////////////////////////////////////////////



#ifndef NO_H323_AUDIO_CODECS

H323Capability * CreateNonStandardAudioCap(
  PluginCodec_Definition * encoderCodec,  
  PluginCodec_Definition * decoderCodec,
  int /*subType*/) 
{
  PluginCodec_H323NonStandardCodecData * pluginData =  (PluginCodec_H323NonStandardCodecData *)encoderCodec->h323CapabilityData;
  if (pluginData == NULL) {
    return new H323CodecPluginNonStandardAudioCapability(
                             encoderCodec, decoderCodec,
                             (const unsigned char *)encoderCodec->descr, 
                             strlen(encoderCodec->descr));
  }

  else if (pluginData->capabilityMatchFunction != NULL) 
    return new H323CodecPluginNonStandardAudioCapability(encoderCodec, decoderCodec,
                             (H323NonStandardCapabilityInfo::CompareFuncType)pluginData->capabilityMatchFunction,
                             pluginData->data, pluginData->dataLength);
  else
    return new H323CodecPluginNonStandardAudioCapability(
                             encoderCodec, decoderCodec,
                             pluginData->data, pluginData->dataLength);
}

H323Capability *CreateGenericAudioCap(
  PluginCodec_Definition * encoderCodec,  
  PluginCodec_Definition * decoderCodec,
  int /*subType*/) 
{
    PluginCodec_H323GenericCodecData * pluginData = (PluginCodec_H323GenericCodecData *)encoderCodec->h323CapabilityData;

    if(pluginData == NULL ) {
    PTRACE(1, "Generic codec information for codec '"<<encoderCodec->descr<<"' has NULL data field");
    return NULL;
    }
    return new H323CodecPluginGenericAudioCapability(encoderCodec, decoderCodec, pluginData);
}

H323Capability * CreateG7231Cap(
  PluginCodec_Definition * encoderCodec,  
  PluginCodec_Definition * decoderCodec,
  int /*subType*/) 
{
  return new H323PluginG7231Capability(encoderCodec, decoderCodec, decoderCodec->h323CapabilityData != 0);
}


H323Capability * CreateGSMCap(
  PluginCodec_Definition * encoderCodec,  
  PluginCodec_Definition * decoderCodec,
  int subType) 
{
  PluginCodec_H323AudioGSMData * pluginData =  (PluginCodec_H323AudioGSMData *)encoderCodec->h323CapabilityData;
  return new H323GSMPluginCapability(encoderCodec, decoderCodec, subType, pluginData->comfortNoise, pluginData->scrambled);
}

#endif

#ifdef H323_VIDEO

H323Capability * CreateNonStandardVideoCap(
  PluginCodec_Definition * encoderCodec,  
  PluginCodec_Definition * decoderCodec,
  int /*subType*/) 
{
  PluginCodec_H323NonStandardCodecData * pluginData =  (PluginCodec_H323NonStandardCodecData *)encoderCodec->h323CapabilityData;
  if (pluginData == NULL) {
    return new H323CodecPluginNonStandardVideoCapability(
                             encoderCodec, decoderCodec,
                             (const unsigned char *)encoderCodec->descr, 
                             strlen(encoderCodec->descr));
  }

  else if (pluginData->capabilityMatchFunction != NULL)
    return new H323CodecPluginNonStandardVideoCapability(encoderCodec, decoderCodec,
                             (H323NonStandardCapabilityInfo::CompareFuncType)pluginData->capabilityMatchFunction,
                             pluginData->data, pluginData->dataLength);
  else
    return new H323CodecPluginNonStandardVideoCapability(
                             encoderCodec, decoderCodec,
                             pluginData->data, pluginData->dataLength);
}

H323Capability *CreateGenericVideoCap(
  PluginCodec_Definition * encoderCodec,  
  PluginCodec_Definition * decoderCodec,
  int /*subType*/) 
{
  PluginCodec_H323GenericCodecData * pluginData = (PluginCodec_H323GenericCodecData *)encoderCodec->h323CapabilityData;

  if (pluginData == NULL ) {
      PTRACE(1, "Generic codec information for codec '"<<encoderCodec->descr<<"' has NULL data field");
      return NULL;
  }
  return new H323CodecPluginGenericVideoCapability(encoderCodec, decoderCodec, pluginData);
}


H323Capability * CreateH261Cap(
  PluginCodec_Definition * encoderCodec, 
  PluginCodec_Definition * decoderCodec,
  int /*subType*/) 
{
  return new H323H261PluginCapability(encoderCodec, decoderCodec);
}

H323Capability * CreateH263Cap(
  PluginCodec_Definition * encoderCodec, 
  PluginCodec_Definition * decoderCodec,
  int /*subType*/) 
{
  return new H323H263PluginCapability(encoderCodec, decoderCodec);
}

#endif // H323_VIDEO

/////////////////////////////////////////////////////////////////////////////

H323Codec * H323PluginCapabilityInfo::CreateCodec(const OpalMediaFormat & mediaFormat, H323Codec::Direction direction) const
{  
  // allow use of this class for external codec capabilities
  if (encoderCodec == NULL || decoderCodec == NULL)
    return NULL;

  PluginCodec_Definition * codec = (direction == H323Codec::Encoder) ? encoderCodec : decoderCodec;

#if PTRACING
  int retVal;
  unsigned parmLen = sizeof(PluginCodec_LogFunction);
  CallCodecControl(codec, NULL, PLUGINCODEC_CONTROL_SET_LOG_FUNCTION, (void *)PluginLogFunction, &parmLen, retVal);
#endif

  switch (codec->flags & PluginCodec_MediaTypeMask) {

    case PluginCodec_MediaTypeAudio:
#ifndef NO_H323_AUDIO_CODECS
      PTRACE(3, "H323PLUGIN\tCreating framed audio codec " << mediaFormatName << " from plugin");
      return new H323PluginFramedAudioCodec(mediaFormat, direction, codec);
#endif  // NO_H323_AUDIO_CODECS

    case PluginCodec_MediaTypeAudioStreamed:
#ifdef NO_H323_AUDIO_CODECS
      PTRACE(3, "H323PLUGIN\tAudio plugins disabled");
      return NULL;
#else
      {
        PTRACE(3, "H323PLUGIN\tCreating audio codec " << mediaFormatName << " from plugin");
        int bitsPerSample = (codec->flags & PluginCodec_BitsPerSampleMask) >> PluginCodec_BitsPerSamplePos;
        if (bitsPerSample == 0)
          bitsPerSample = 16;
        return new H323StreamedPluginAudioCodec(
                                mediaFormat, 
                                direction, 
                                codec->parm.audio.samplesPerFrame,
                                bitsPerSample,
                                codec);
      }
#endif  // NO_H323_AUDIO_CODECS

    case PluginCodec_MediaTypeVideo:
#ifndef H323_VIDEO
      PTRACE(3, "H323PLUGIN\tVideo plugins disabled");
      return NULL;
#else
      if (((codec->flags & PluginCodec_MediaTypeMask) != PluginCodec_MediaTypeVideo) ||
          (((codec->flags & PluginCodec_RTPTypeMask) != PluginCodec_RTPTypeExplicit) &&
           (codec->flags & PluginCodec_RTPTypeMask) != PluginCodec_RTPTypeDynamic)) {
             PTRACE(3, "H323PLUGIN\tVideo codec " << mediaFormatName << " has incorrect format types");
             return NULL;
      } 

      PTRACE(3, "H323PLUGIN\tCreating video codec " << mediaFormatName << " from plugin");
      return new H323PluginVideoCodec(mediaFormat, direction, codec);

#endif // H323_VIDEO
    default:
      break;
  }

  PTRACE(3, "H323PLUGIN\tCannot create codec for unknown plugin codec media format " << (int)(codec->flags & PluginCodec_MediaTypeMask));
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////

H323PluginCapabilityInfo::H323PluginCapabilityInfo(PluginCodec_Definition * _encoderCodec,
                                                   PluginCodec_Definition * _decoderCodec)
 : encoderCodec(_encoderCodec),
   decoderCodec(_decoderCodec),
   capabilityFormatName(CreateCodecName(_encoderCodec, TRUE)),
   mediaFormatName(CreateCodecName(_encoderCodec, FALSE))
{
} 

H323PluginCapabilityInfo::H323PluginCapabilityInfo(const PString & _mediaFormat, const PString & _baseName)
 : encoderCodec(NULL),
   decoderCodec(NULL),
   capabilityFormatName(CreateCodecName(_baseName, TRUE)),
   mediaFormatName(_mediaFormat)
{
}

#ifndef NO_H323_AUDIO_CODECS

/////////////////////////////////////////////////////////////////////////////

H323CodecPluginNonStandardAudioCapability::H323CodecPluginNonStandardAudioCapability(
    PluginCodec_Definition * _encoderCodec,
    PluginCodec_Definition * _decoderCodec,
    H323NonStandardCapabilityInfo::CompareFuncType compareFunc,
    const unsigned char * data, unsigned dataLen)
 : H323NonStandardAudioCapability(_decoderCodec->parm.audio.maxFramesPerPacket,
                                  _encoderCodec->parm.audio.maxFramesPerPacket,
                                  compareFunc,
                                  data, dataLen), 
   H323PluginCapabilityInfo(_encoderCodec, _decoderCodec)
{
  PopulateMediaFormatOptions(encoderCodec,GetWritableMediaFormat());
  PluginCodec_H323NonStandardCodecData * nonStdData = (PluginCodec_H323NonStandardCodecData *)_encoderCodec->h323CapabilityData;
  if (nonStdData->objectId != NULL) {
    oid = PString(nonStdData->objectId);
  } else {
    t35CountryCode   = nonStdData->t35CountryCode;
    t35Extension     = nonStdData->t35Extension;
    manufacturerCode = nonStdData->manufacturerCode;
  }
}

H323CodecPluginNonStandardAudioCapability::H323CodecPluginNonStandardAudioCapability(
    PluginCodec_Definition * _encoderCodec,
    PluginCodec_Definition * _decoderCodec,
    const unsigned char * data, unsigned dataLen)
 : H323NonStandardAudioCapability(_decoderCodec->parm.audio.maxFramesPerPacket,
                                  _encoderCodec->parm.audio.maxFramesPerPacket,
                                  data, dataLen), 
   H323PluginCapabilityInfo(_encoderCodec, _decoderCodec)
{
  PopulateMediaFormatOptions(encoderCodec,GetWritableMediaFormat());
  PluginCodec_H323NonStandardCodecData * nonStdData = (PluginCodec_H323NonStandardCodecData *)_encoderCodec->h323CapabilityData;
  if (nonStdData->objectId != NULL) {
    oid = PString(nonStdData->objectId);
  } else {
    t35CountryCode   = nonStdData->t35CountryCode;
    t35Extension     = nonStdData->t35Extension;
    manufacturerCode = nonStdData->manufacturerCode;
  }
}

/////////////////////////////////////////////////////////////////////////////

H323CodecPluginGenericAudioCapability::H323CodecPluginGenericAudioCapability(
    const PluginCodec_Definition * _encoderCodec,
    const PluginCodec_Definition * _decoderCodec,
    const PluginCodec_H323GenericCodecData *data )
    : H323GenericAudioCapability(_decoderCodec->parm.audio.maxFramesPerPacket,
                     _encoderCodec->parm.audio.maxFramesPerPacket,
                     data -> standardIdentifier, data -> maxBitRate),
      H323PluginCapabilityInfo((PluginCodec_Definition *)_encoderCodec,
                   (PluginCodec_Definition *) _decoderCodec)
{
  PopulateMediaFormatOptions(encoderCodec,GetWritableMediaFormat());
  PopulateMediaFormatFromGenericData(GetWritableMediaFormat(), data);
  rtpPayloadType = (RTP_DataFrame::PayloadTypes)(((_encoderCodec->flags & PluginCodec_RTPTypeMask) == PluginCodec_RTPTypeDynamic) ? RTP_DataFrame::DynamicBase : _encoderCodec->rtpPayload);
}

/////////////////////////////////////////////////////////////////////////////

PObject::Comparison H323GSMPluginCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323GSMPluginCapability))
    return LessThan;

  Comparison result = H323AudioCapability::Compare(obj);
  if (result != EqualTo)
    return result;

  const H323GSMPluginCapability& other = (const H323GSMPluginCapability&)obj;
  if (scrambled < other.scrambled)
    return LessThan;
  if (comfortNoise < other.comfortNoise)
    return LessThan;
  return EqualTo;
}


BOOL H323GSMPluginCapability::OnSendingPDU(H245_AudioCapability & cap, unsigned packetSize) const
{
  cap.SetTag(pluginSubType);
  H245_GSMAudioCapability & gsm = cap;
  gsm.m_audioUnitSize = packetSize * encoderCodec->parm.audio.bytesPerFrame;
  gsm.m_comfortNoise  = comfortNoise;
  gsm.m_scrambled     = scrambled;

  return TRUE;
}


BOOL H323GSMPluginCapability::OnReceivedPDU(const H245_AudioCapability & cap, unsigned & packetSize)
{
  const H245_GSMAudioCapability & gsm = cap;
  packetSize   = gsm.m_audioUnitSize / encoderCodec->parm.audio.bytesPerFrame;
  if (packetSize == 0)
    packetSize = 1;

  scrambled    = gsm.m_scrambled;
  comfortNoise = gsm.m_comfortNoise;

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

#endif   // H323_AUDIO_CODECS

#ifdef H323_VIDEO

/////////////////////////////////////////////////////////////////////////////

H323H261PluginCapability::H323H261PluginCapability(PluginCodec_Definition * _encoderCodec,
                                                   PluginCodec_Definition * _decoderCodec)
  : H323VideoPluginCapability(_encoderCodec, _decoderCodec, H245_VideoCapability::e_h261VideoCapability),
  enc(_encoderCodec)
{ 
}

PObject::Comparison H323H261PluginCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323H261PluginCapability))
    return LessThan;

  Comparison result = H323Capability::Compare(obj);
  if (result != EqualTo)
    return result;

  const H323H261PluginCapability & other = (const H323H261PluginCapability &)obj;

  const OpalMediaFormat & myFormat = GetMediaFormat();
  int qcifMPI = myFormat.GetOptionInteger(qcifMPI_tag);
  int  cifMPI = myFormat.GetOptionInteger(cifMPI_tag);

  const OpalMediaFormat & otherFormat = other.GetMediaFormat();
  int other_qcifMPI = otherFormat.GetOptionInteger(qcifMPI_tag);
  int other_cifMPI  = otherFormat.GetOptionInteger(cifMPI_tag);

  if (((qcifMPI > 0) && (other_qcifMPI > 0)) ||
      ((cifMPI  > 0) && (other_cifMPI > 0)))
    return EqualTo;

  if (qcifMPI > 0)
    return LessThan;

  return GreaterThan;
}


BOOL H323H261PluginCapability::OnSendingPDU(H245_VideoCapability & cap) const
{
  cap.SetTag(H245_VideoCapability::e_h261VideoCapability);

  H245_H261VideoCapability & h261 = cap;

  const OpalMediaFormat & fmt = GetMediaFormat();

  int qcifMPI = fmt.GetOptionInteger(qcifMPI_tag, 0);

  if (qcifMPI > 0) {
    h261.IncludeOptionalField(H245_H261VideoCapability::e_qcifMPI);
    h261.m_qcifMPI = qcifMPI;
  }

  int cifMPI = fmt.GetOptionInteger(cifMPI_tag);
  if (cifMPI > 0 || qcifMPI == 0) {
    h261.IncludeOptionalField(H245_H261VideoCapability::e_cifMPI);
    h261.m_cifMPI = cifMPI;
  }

  h261.m_temporalSpatialTradeOffCapability = fmt.GetOptionBoolean(h323_temporalSpatialTradeOffCapability_tag, FALSE);
  h261.m_maxBitRate                        = (fmt.GetOptionInteger(OpalVideoFormat::MaxBitRateOption, 621700)+50)/100;
  h261.m_stillImageTransmission            = fmt.GetOptionBoolean(h323_stillImageTransmission_tag, FALSE);

  return TRUE;
}


BOOL H323H261PluginCapability::OnSendingPDU(H245_VideoMode & pdu) const
{
  pdu.SetTag(H245_VideoMode::e_h261VideoMode);
  H245_H261VideoMode & mode = pdu;

  const OpalMediaFormat & fmt = GetMediaFormat();

  int qcifMPI = fmt.GetOptionInteger(qcifMPI_tag);

  mode.m_resolution.SetTag(qcifMPI > 0 ? H245_H261VideoMode_resolution::e_qcif
                                       : H245_H261VideoMode_resolution::e_cif);

  mode.m_bitRate                = (fmt.GetOptionInteger(OpalVideoFormat::MaxBitRateOption, 621700) + 50) / 1000;
  mode.m_stillImageTransmission = fmt.GetOptionBoolean(h323_stillImageTransmission_tag, FALSE);

  return TRUE;
}

BOOL H323H261PluginCapability::OnReceivedPDU(const H245_VideoCapability & cap)
{
  if (cap.GetTag() != H245_VideoCapability::e_h261VideoCapability)
    return FALSE;

  OpalMediaFormat & fmt = GetWritableMediaFormat();

  const H245_H261VideoCapability & h261 = cap;

  if (h261.HasOptionalField(H245_H261VideoCapability::e_qcifMPI)) {
    if (!fmt.SetOptionInteger(qcifMPI_tag, h261.m_qcifMPI))
      return FALSE;

     if (!H323VideoPluginCapability::SetCommonOptions(fmt, QCIF_WIDTH, QCIF_HEIGHT, h261.m_qcifMPI)) 
         return FALSE;  
  }

  if (h261.HasOptionalField(H245_H261VideoCapability::e_cifMPI)) {
    if (!fmt.SetOptionInteger(cifMPI_tag, h261.m_cifMPI))
      return FALSE;

    if (!H323VideoPluginCapability::SetCommonOptions(fmt, CIF_WIDTH, CIF_HEIGHT, h261.m_cifMPI))
      return FALSE;
  }

  fmt.SetOptionInteger(OpalVideoFormat::MaxBitRateOption,          h261.m_maxBitRate*100);
  fmt.SetOptionBoolean(h323_temporalSpatialTradeOffCapability_tag, h261.m_temporalSpatialTradeOffCapability);
  fmt.SetOptionBoolean(h323_stillImageTransmission_tag,            h261.m_stillImageTransmission);

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

H323H263PluginCapability::H323H263PluginCapability(PluginCodec_Definition * _encoderCodec,
                                                   PluginCodec_Definition * _decoderCodec)
  : H323VideoPluginCapability(_encoderCodec, _decoderCodec, H245_VideoCapability::e_h263VideoCapability)
{ 
}

PObject::Comparison H323H263PluginCapability::Compare(const PObject & obj) const
{

  if (!PIsDescendant(&obj, H323H263PluginCapability))
    return LessThan;

  Comparison result = H323Capability::Compare(obj);
  if (result != EqualTo)
    return result;

  const H323H263PluginCapability & other = (const H323H263PluginCapability &)obj;

  const OpalMediaFormat & myFormat = GetMediaFormat();
  int sqcifMPI = myFormat.GetOptionInteger(sqcifMPI_tag);
  int qcifMPI  = myFormat.GetOptionInteger(qcifMPI_tag);
  int cifMPI   = myFormat.GetOptionInteger(cifMPI_tag);
  int cif4MPI  = myFormat.GetOptionInteger(cif4MPI_tag);
  int cif16MPI = myFormat.GetOptionInteger(cif16MPI_tag);

  const OpalMediaFormat & otherFormat = other.GetMediaFormat();
  int other_sqcifMPI = otherFormat.GetOptionInteger(sqcifMPI_tag);
  int other_qcifMPI  = otherFormat.GetOptionInteger(qcifMPI_tag);
  int other_cifMPI   = otherFormat.GetOptionInteger(cifMPI_tag);
  int other_cif4MPI  = otherFormat.GetOptionInteger(cif4MPI_tag);
  int other_cif16MPI = otherFormat.GetOptionInteger(cif16MPI_tag);

  if ((IsValidMPI(sqcifMPI) && IsValidMPI(other_sqcifMPI)) ||
      (IsValidMPI(qcifMPI) && IsValidMPI(other_qcifMPI)) ||
      (IsValidMPI(cifMPI) && IsValidMPI(other_cifMPI)) ||
      (IsValidMPI(cif4MPI) && IsValidMPI(other_cif4MPI)) ||
      (IsValidMPI(cif16MPI) && IsValidMPI(other_cif16MPI)))
           return EqualTo;

  if ((!IsValidMPI(cif16MPI) && IsValidMPI(other_cif16MPI)) ||
      (!IsValidMPI(cif4MPI) && IsValidMPI(other_cif4MPI)) ||
      (!IsValidMPI(cifMPI) && IsValidMPI(other_cifMPI)) ||
      (!IsValidMPI(qcifMPI) && IsValidMPI(other_qcifMPI)) ||
      (!IsValidMPI(sqcifMPI) && IsValidMPI(other_sqcifMPI)))
    return LessThan;

  return GreaterThan;
}

static void SetTransmittedCap(const OpalMediaFormat & mediaFormat,
                              H245_H263VideoCapability & h263,
                              const char * mpiTag,
                              int mpiEnum,
                              PASN_Integer & mpi,
                              int slowMpiEnum,
                              PASN_Integer & slowMpi)
{
  int mpiVal = mediaFormat.GetOptionInteger(mpiTag);
  if (mpiVal > 0) {
    h263.IncludeOptionalField(mpiEnum);
    mpi = mpiVal;
  }
  else if (mpiVal < 0) {
    h263.IncludeOptionalField(slowMpiEnum);
    slowMpi = -mpiVal;
  }
}


BOOL H323H263PluginCapability::OnSendingPDU(H245_VideoCapability & cap) const
{

  cap.SetTag(H245_VideoCapability::e_h263VideoCapability);
  H245_H263VideoCapability & h263 = cap;

  const OpalMediaFormat & fmt = GetMediaFormat();

  SetTransmittedCap(fmt, cap, sqcifMPI_tag, H245_H263VideoCapability::e_sqcifMPI, h263.m_sqcifMPI, H245_H263VideoCapability::e_slowSqcifMPI, h263.m_slowSqcifMPI);
  SetTransmittedCap(fmt, cap, qcifMPI_tag,  H245_H263VideoCapability::e_qcifMPI,  h263.m_qcifMPI,  H245_H263VideoCapability::e_slowQcifMPI,  h263.m_slowQcifMPI);
  SetTransmittedCap(fmt, cap, cifMPI_tag,   H245_H263VideoCapability::e_cifMPI,   h263.m_cifMPI,   H245_H263VideoCapability::e_slowCifMPI,   h263.m_slowCifMPI);
  SetTransmittedCap(fmt, cap, cif4MPI_tag,  H245_H263VideoCapability::e_cif4MPI,  h263.m_cif4MPI,  H245_H263VideoCapability::e_slowCif4MPI,  h263.m_slowCif4MPI);
  SetTransmittedCap(fmt, cap, cif16MPI_tag, H245_H263VideoCapability::e_cif16MPI, h263.m_cif16MPI, H245_H263VideoCapability::e_slowCif16MPI, h263.m_slowCif16MPI);

  h263.m_maxBitRate                        = (fmt.GetOptionInteger(OpalVideoFormat::MaxBitRateOption, 327600) + 50) / 100;
  h263.m_temporalSpatialTradeOffCapability = fmt.GetOptionBoolean(h323_temporalSpatialTradeOffCapability_tag, FALSE);
  h263.m_unrestrictedVector                   = fmt.GetOptionBoolean(h323_unrestrictedVector_tag, FALSE);
  h263.m_arithmeticCoding                     = fmt.GetOptionBoolean(h323_arithmeticCoding_tag, FALSE);
  h263.m_advancedPrediction                   = fmt.GetOptionBoolean(h323_advancedPrediction_tag, FALSE);
  h263.m_pbFrames                             = fmt.GetOptionBoolean(h323_pbFrames_tag, FALSE);
  h263.m_errorCompensation                 = fmt.GetOptionBoolean(h323_errorCompensation_tag, FALSE);

  {
    int hrdB = fmt.GetOptionInteger(h323_hrdB_tag, -1);
    if (hrdB >= 0) {
      h263.IncludeOptionalField(H245_H263VideoCapability::e_hrd_B);
        h263.m_hrd_B = hrdB;
    }
  }

  {
    int bppMaxKb = fmt.GetOptionInteger(h323_bppMaxKb_tag, -1);
    if (bppMaxKb >= 0) {
      h263.IncludeOptionalField(H245_H263VideoCapability::e_bppMaxKb);
        h263.m_bppMaxKb = bppMaxKb;
    }
  }

  int plusOpt = fmt.GetOptionInteger("plusOpt", -1);
  if(plusOpt >= 0)
  {
   h263.IncludeOptionalField(H245_H263VideoCapability::e_h263Options);
  }
  
  return TRUE;
}


BOOL H323H263PluginCapability::OnSendingPDU(H245_VideoMode & pdu) const
{
  pdu.SetTag(H245_VideoMode::e_h263VideoMode);
  H245_H263VideoMode & mode = pdu;

  const OpalMediaFormat & fmt = GetMediaFormat();

  int qcifMPI  = fmt.GetOptionInteger(qcifMPI_tag);
  int cifMPI   = fmt.GetOptionInteger(cifMPI_tag);
  int cif4MPI  = fmt.GetOptionInteger(cif4MPI_tag);
  int cif16MPI = fmt.GetOptionInteger(cif16MPI_tag);

  mode.m_resolution.SetTag(cif16MPI ? H245_H263VideoMode_resolution::e_cif16
              :(cif4MPI ? H245_H263VideoMode_resolution::e_cif4
               :(cifMPI ? H245_H263VideoMode_resolution::e_cif
                :(qcifMPI ? H245_H263VideoMode_resolution::e_qcif
            : H245_H263VideoMode_resolution::e_sqcif))));

  mode.m_bitRate              = (fmt.GetOptionInteger(OpalVideoFormat::MaxBitRateOption, 327600) + 50) / 100;
  mode.m_unrestrictedVector   = fmt.GetOptionBoolean(h323_unrestrictedVector_tag, FALSE);
  mode.m_arithmeticCoding     = fmt.GetOptionBoolean(h323_arithmeticCoding_tag, FALSE);
  mode.m_advancedPrediction   = fmt.GetOptionBoolean(h323_advancedPrediction_tag, FALSE);
  mode.m_pbFrames             = fmt.GetOptionBoolean(h323_pbFrames_tag, FALSE);
  mode.m_errorCompensation    = fmt.GetOptionBoolean(h323_errorCompensation_tag, FALSE);

  return TRUE;
}

static BOOL SetReceivedH263Cap(OpalMediaFormat & mediaFormat, 
                               const H245_H263VideoCapability & h263, 
                               const char * mpiTag,
                               int mpiEnum,
                               const PASN_Integer & mpi,
                               int slowMpiEnum,
                               const PASN_Integer & slowMpi,
                               int frameWidth, int frameHeight,
                               BOOL & formatDefined)
{
  if (h263.HasOptionalField(mpiEnum)) {
    if (!mediaFormat.SetOptionInteger(mpiTag, mpi))
      return FALSE;
    if (!H323VideoPluginCapability::SetCommonOptions(mediaFormat, frameWidth, frameHeight, mpi))
      return FALSE;
    formatDefined = TRUE;
  } else if (h263.HasOptionalField(slowMpiEnum)) {
    if (!mediaFormat.SetOptionInteger(mpiTag, -(signed)slowMpi))
      return FALSE;
    if (!H323VideoPluginCapability::SetCommonOptions(mediaFormat, frameWidth, frameHeight, -(signed)slowMpi))
      return FALSE;
    formatDefined = TRUE;
  } 
  else mediaFormat.SetOptionInteger(mpiTag, 0);

  return TRUE;
}


BOOL H323H263PluginCapability::OnReceivedPDU(const H245_VideoCapability & cap)
{
  if (cap.GetTag() != H245_VideoCapability::e_h263VideoCapability)
    return FALSE;

  OpalMediaFormat & fmt = GetWritableMediaFormat();

  BOOL formatDefined = FALSE;

  const H245_H263VideoCapability & h263 = cap;

  if (!SetReceivedH263Cap(fmt, cap, sqcifMPI_tag, H245_H263VideoCapability::e_sqcifMPI, h263.m_sqcifMPI, H245_H263VideoCapability::e_slowSqcifMPI, h263.m_slowSqcifMPI, SQCIF_WIDTH, SQCIF_HEIGHT, formatDefined))
    return FALSE;

  if (!SetReceivedH263Cap(fmt, cap, qcifMPI_tag,  H245_H263VideoCapability::e_qcifMPI,  h263.m_qcifMPI,  H245_H263VideoCapability::e_slowQcifMPI,  h263.m_slowQcifMPI,  QCIF_WIDTH,  QCIF_HEIGHT,  formatDefined))
    return FALSE;

  if (!SetReceivedH263Cap(fmt, cap, cifMPI_tag,   H245_H263VideoCapability::e_cifMPI,   h263.m_cifMPI,   H245_H263VideoCapability::e_slowCifMPI,   h263.m_slowCifMPI,   CIF_WIDTH,   CIF_HEIGHT,   formatDefined))
    return FALSE;

  if (!SetReceivedH263Cap(fmt, cap, cif4MPI_tag,  H245_H263VideoCapability::e_cif4MPI,  h263.m_cif4MPI,  H245_H263VideoCapability::e_slowCif4MPI,  h263.m_slowCif4MPI,  CIF4_WIDTH,  CIF4_HEIGHT,  formatDefined))
    return FALSE;

  if (!SetReceivedH263Cap(fmt, cap, cif16MPI_tag, H245_H263VideoCapability::e_cif16MPI, h263.m_cif16MPI, H245_H263VideoCapability::e_slowCif16MPI, h263.m_slowCif16MPI, CIF16_WIDTH, CIF16_HEIGHT, formatDefined))
    return FALSE;

  if (!fmt.SetOptionInteger(OpalVideoFormat::MaxBitRateOption, h263.m_maxBitRate*100))
    return FALSE;

  fmt.SetOptionBoolean(h323_unrestrictedVector_tag,      h263.m_unrestrictedVector);
  fmt.SetOptionBoolean(h323_arithmeticCoding_tag,        h263.m_arithmeticCoding);
  fmt.SetOptionBoolean(h323_advancedPrediction_tag,      h263.m_advancedPrediction);
  fmt.SetOptionBoolean(h323_pbFrames_tag,                h263.m_pbFrames);
  fmt.SetOptionBoolean(h323_errorCompensation_tag,       h263.m_errorCompensation);

  if (h263.HasOptionalField(H245_H263VideoCapability::e_hrd_B))
    fmt.SetOptionInteger(h323_hrdB_tag, h263.m_hrd_B);

  if (h263.HasOptionalField(H245_H263VideoCapability::e_bppMaxKb))
    fmt.SetOptionInteger(h323_bppMaxKb_tag, h263.m_bppMaxKb);
    
  if(h263.HasOptionalField(H245_H263VideoCapability::e_h263Options))
  {
   PString mediaPacketization = fmt.GetOptionString("Media Packetization");
   if(mediaPacketization.NumCompare("RFC2429") != PObject::EqualTo) return FALSE;
  }
  else
  {
   PString mediaPacketization = fmt.GetOptionString("Media Packetization");
   if(mediaPacketization.NumCompare("RFC2429") == PObject::EqualTo) return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

H323CodecPluginNonStandardVideoCapability::H323CodecPluginNonStandardVideoCapability(
    PluginCodec_Definition * _encoderCodec,
    PluginCodec_Definition * _decoderCodec,
    H323NonStandardCapabilityInfo::CompareFuncType compareFunc,
    const unsigned char * data, unsigned dataLen)
 : H323NonStandardVideoCapability(data, dataLen), 
   H323PluginCapabilityInfo(_encoderCodec, _decoderCodec)
{
  PluginCodec_H323NonStandardCodecData * nonStdData = (PluginCodec_H323NonStandardCodecData *)_encoderCodec->h323CapabilityData;
  if (nonStdData->objectId != NULL) {
    oid = PString(nonStdData->objectId);
  } else {
    t35CountryCode   = nonStdData->t35CountryCode;
    t35Extension     = nonStdData->t35Extension;
    manufacturerCode = nonStdData->manufacturerCode;
  }

  PopulateMediaFormatOptions(encoderCodec,GetWritableMediaFormat());
  rtpPayloadType = (RTP_DataFrame::PayloadTypes)(((_encoderCodec->flags & PluginCodec_RTPTypeMask) == PluginCodec_RTPTypeDynamic) ? RTP_DataFrame::DynamicBase : _encoderCodec->rtpPayload);
}

H323CodecPluginNonStandardVideoCapability::H323CodecPluginNonStandardVideoCapability(
    PluginCodec_Definition * _encoderCodec,
    PluginCodec_Definition * _decoderCodec,
    const unsigned char * data, unsigned dataLen)
 : H323NonStandardVideoCapability(data, dataLen), 
   H323PluginCapabilityInfo(_encoderCodec, _decoderCodec)
{
  PluginCodec_H323NonStandardCodecData * nonStdData = (PluginCodec_H323NonStandardCodecData *)_encoderCodec->h323CapabilityData;
  if (nonStdData->objectId != NULL) {
    oid = PString(nonStdData->objectId);
  } else {
    t35CountryCode   = nonStdData->t35CountryCode;
    t35Extension     = nonStdData->t35Extension;
    manufacturerCode = nonStdData->manufacturerCode;
  }

  rtpPayloadType = (RTP_DataFrame::PayloadTypes)(((_encoderCodec->flags & PluginCodec_RTPTypeMask) == PluginCodec_RTPTypeDynamic) ? RTP_DataFrame::DynamicBase : _encoderCodec->rtpPayload);
}

/////////////////////////////////////////////////////////////////////////////

H323CodecPluginGenericVideoCapability::H323CodecPluginGenericVideoCapability(
    const PluginCodec_Definition * _encoderCodec,
    const PluginCodec_Definition * _decoderCodec,
    const PluginCodec_H323GenericCodecData *data )
    : H323GenericVideoCapability(data -> standardIdentifier, data -> maxBitRate),
      H323PluginCapabilityInfo((PluginCodec_Definition *)_encoderCodec, (PluginCodec_Definition *) _decoderCodec)
{
  LoadGenericData(data);
  rtpPayloadType = (RTP_DataFrame::PayloadTypes)(((_encoderCodec->flags & PluginCodec_RTPTypeMask) == PluginCodec_RTPTypeDynamic) ? RTP_DataFrame::DynamicBase : _encoderCodec->rtpPayload);
}

void H323CodecPluginGenericVideoCapability::LoadGenericData(const PluginCodec_H323GenericCodecData *data)
{

//  SetCommonOptions(GetWritableMediaFormat(),encoderCodec->parm.video.maxFrameWidth, encoderCodec->parm.video.maxFrameHeight, encoderCodec->parm.video.recommendedFrameRate);

  PopulateMediaFormatOptions(encoderCodec,GetWritableMediaFormat());
  PopulateMediaFormatFromGenericData(GetWritableMediaFormat(), data);
}

BOOL H323CodecPluginGenericVideoCapability::SetMaxFrameSize(CapabilityFrameSize framesize, int frameunits)
{
    PString param;
    switch (framesize) {
        case sqcifMPI  : param = sqcifMPI_tag; break;
        case  qcifMPI  : param =  qcifMPI_tag; break;
        case   cifMPI  : param =   cifMPI_tag; break;
        case   cif4MPI : param =   cif4MPI_tag; break;
        case   cif16MPI: param =   cif16MPI_tag; break;
		case  i480MPI  : param =   cif4MPI_tag; break;
        case  p720MPI  : param =   cif16MPI_tag; break;
        case i1080MPI  : param =  i1080MPI_tag; break; 
        default: return FALSE;
    }

    SetCodecControl(encoderCodec, NULL, SET_CODEC_OPTIONS_CONTROL, param,frameunits);
    SetCodecControl(decoderCodec, NULL, SET_CODEC_OPTIONS_CONTROL, param, frameunits);
    LoadGenericData((PluginCodec_H323GenericCodecData *)encoderCodec->h323CapabilityData);
    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////

#endif  // H323_VIDEO

/////////////////////////////////////////////////////////////////////////////

H323DynaLink::H323DynaLink(const char * _baseName, const char * _reason)
  : baseName(_baseName), reason(_reason)
{
  isLoadedOK = FALSE;
}

void H323DynaLink::Load()
{
  PStringArray dirs = PPluginManager::GetPluginDirs();
  PINDEX i;
  for (i = 0; !PDynaLink::IsLoaded() && i < dirs.GetSize(); i++)
    PLoadPluginDirectory<H323DynaLink>(*this, dirs[i]);
  
  if (!PDynaLink::IsLoaded()) {
    cerr << "Cannot find " << baseName << " as required for " << ((reason != NULL) ? reason : " a code module") << "." << endl
         << "This function may appear to be installed, but will not operate correctly." << endl
         << "Please put the file " << baseName << PDynaLink::GetExtension() << " into one of the following directories:" << endl
         << "     " << setfill(',') << dirs << setfill(' ') << endl
         << "This list of directories can be set using the PWLIBPLUGINDIR environment variable." << endl;
    return;
  }
}

BOOL H323DynaLink::LoadPlugin(const PString & filename)
{
    return PDynaLink::Open(filename);
}

/////////////////////////////////////////////////////////////////////////////

static PAtomicInteger bootStrapCount(0);

void H323PluginCodecManager::Bootstrap()
{
  if (++bootStrapCount != 1)
    return;

#if defined(H323_AUDIO_CODECS)
  OpalMediaFormat::List & mediaFormatList = H323PluginCodecManager::GetMediaFormatList();

  mediaFormatList.Append(new OpalMediaFormat(OpalG711uLaw));
  mediaFormatList.Append(new OpalMediaFormat(OpalG711ALaw));

  new OpalFixedCodecFactory<OpalG711ALaw64k_Encoder>::Worker(OpalG711ALaw64k_Encoder::GetFactoryName());
  new OpalFixedCodecFactory<OpalG711ALaw64k_Decoder>::Worker(OpalG711ALaw64k_Decoder::GetFactoryName());

  new OpalFixedCodecFactory<OpalG711uLaw64k_Encoder>::Worker(OpalG711uLaw64k_Encoder::GetFactoryName());
  new OpalFixedCodecFactory<OpalG711uLaw64k_Decoder>::Worker(OpalG711uLaw64k_Decoder::GetFactoryName());
#endif

}

/////////////////////////////////////////////////////////////////////////////

#define INCLUDE_STATIC_CODEC(name) \
extern "C" { \
extern unsigned int Opal_StaticCodec_##name##_GetAPIVersion(); \
extern struct PluginCodec_Definition * Opal_StaticCodec_##name##_GetCodecs(unsigned *,unsigned); \
}; \
class H323StaticPluginCodec_##name : public H323StaticPluginCodec \
{ \
  public: \
    PluginCodec_GetAPIVersionFunction Get_GetAPIFn() \
    { return &Opal_StaticCodec_##name##_GetAPIVersion; } \
    PluginCodec_GetCodecFunction Get_GetCodecFn() \
    { return &Opal_StaticCodec_##name##_GetCodecs; } \
}; \
static PFactory<H323StaticPluginCodec>::Worker<H323StaticPluginCodec_##name > static##name##CodecFactory( #name ); \

#ifdef H323_EMBEDDED_GSM

INCLUDE_STATIC_CODEC(GSM_0610)

#endif


