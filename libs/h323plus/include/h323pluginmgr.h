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
 * $Log: h323pluginmgr.h,v $
 * Revision 1.2  2007/08/20 20:19:52  shorne
 * Moved opalplugin.h to codec directory to be plugin compile compatible with Opal
 *
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.24.4.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.24  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.23  2005/06/21 06:46:35  csoutheren
 * Add ability to create capabilities without codecs for external RTP interface
 *
 * Revision 1.22  2005/04/28 04:00:15  dereksmithies
 * Add documentation to the OpalFactoryCodec. Thanks to Craig Southeren for clarifying
 * the meaning of the toLen field.
 *
 * Revision 1.21  2005/01/04 08:08:39  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.20  2005/01/03 14:03:20  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.19  2004/12/08 02:03:58  csoutheren
 * Fixed problem with detection of non-FFH.263
 *
 * Revision 1.18  2004/11/29 06:30:51  csoutheren
 * Added support for wideband codecs
 *
 * Revision 1.17  2004/08/26 08:05:02  csoutheren
 * Codecs now appear in abstract factory system
 * Fixed Windows factory bootstrap system (again)
 *
 * Revision 1.16  2004/06/30 12:31:09  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.15  2004/06/03 23:20:46  csoutheren
 * Fixed compile problem on some gcc variants
 *
 * Revision 1.14  2004/06/01 07:30:27  csoutheren
 * Removed accidental cut & paste in new code that removed capabilities
 *
 * Revision 1.13  2004/06/01 05:49:27  csoutheren
 * Added code to cleanup some allocated memory upon shutdown
 *
 * Revision 1.12  2004/05/23 12:40:34  rjongbloed
 * Simplified call for getting H323 plug manager instance
 *
 * Revision 1.11  2004/05/18 23:03:10  csoutheren
 * Added empty destructor to prevent gcc 3.3.3 from getting link errors.
 *
 * Revision 1.10  2004/05/18 22:32:54  csoutheren
 * Added pragma inteface
 *
 * Revision 1.9  2004/05/18 22:26:28  csoutheren
 * Initial support for embedded codecs
 * Fixed problems with streamed codec support
 * Updates for abstract factory loading methods
 *
 * Revision 1.8  2004/05/18 06:02:25  csoutheren
 * Deferred plugin codec loading until after main has executed by using abstract factory classes
 *
 * Revision 1.7  2004/05/12 13:41:26  csoutheren
 * Added support for getting lists of media formats from plugin manager
 *
 * Revision 1.6  2004/04/22 22:35:00  csoutheren
 * Fixed mispelling of Guilhem Tardy - my apologies to him
 *
 * Revision 1.5  2004/04/22 14:22:20  csoutheren
 * Added RFC 2190 H.263 code as created by Guilhem Tardy and AliceStreet
 * Many thanks to them for their contributions.
 *
 * Revision 1.4  2004/04/14 08:14:40  csoutheren
 * Changed to use generic plugin manager
 *
 * Revision 1.3  2004/04/09 12:24:18  csoutheren
 * Renamed h323plugin.h to opalplugin.h, and modified everything else
 * as required
 *
 * Revision 1.2  2004/04/03 10:38:24  csoutheren
 * Added in initial cut at codec plugin code. Branches are for wimps :)
 *
 * Revision 1.1.2.1  2004/03/31 11:03:16  csoutheren
 * Initial public version
 *
 * Revision 1.2  2004/01/27 14:55:46  craigs
 * Implemented static linking of new codecs
 *
 * Revision 1.1  2004/01/04 13:37:51  craigs
 * Implementation of codec plugins
 *
 *
 */

#ifndef __OPAL_H323PLUGINMGR_H
#define __OPAL_H323PLUGINMGR_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/pluginmgr.h>
#include <codec/opalplugin.h>
#include <mediafmt.h>
#include <ptlib/pfactory.h>

#include <h323caps.h>

class H323Capability;

class H323PluginCodecManager : public PPluginModuleManager
{
  PCLASSINFO(H323PluginCodecManager, PPluginModuleManager);
  public:
    H323PluginCodecManager(PPluginManager * pluginMgr = NULL);
    ~H323PluginCodecManager();

    void RegisterStaticCodec(const char * name,
                             PluginCodec_GetAPIVersionFunction getApiVerFn,
                             PluginCodec_GetCodecFunction getCodecFn);

    void OnLoadPlugin(PDynaLink & dll, INT code);

    static OpalMediaFormat::List GetMediaFormats();
    static void AddFormat(const OpalMediaFormat & fmt);
    static void AddFormat(OpalMediaFormat * fmt);

    virtual void OnShutdown();

    static void Bootstrap();
/*
    H323Capability * CreateCapability(
          const PString & _mediaFormat, 
          const PString & _baseName,
                 unsigned maxFramesPerPacket, 
                 unsigned recommendedFramesPerPacket,
                 unsigned _pluginSubType);
*/
  protected:
    void CreateCapabilityAndMediaFormat(
      PluginCodec_Definition * _encoderCodec,
      PluginCodec_Definition * _decoderCodec
    );

    static OpalMediaFormat::List & GetMediaFormatList();
    static PMutex & GetMediaFormatMutex();

    void RegisterCodecs  (unsigned int count, void * codecList);
    void UnregisterCodecs(unsigned int count, void * codecList);

    void RegisterCapability(PluginCodec_Definition * encoderCodec, PluginCodec_Definition * decoderCodec);
    struct CapabilityListCreateEntry {
      CapabilityListCreateEntry(PluginCodec_Definition * e, PluginCodec_Definition * d)
        : encoderCodec(e), decoderCodec(d) { }
      PluginCodec_Definition * encoderCodec;
      PluginCodec_Definition * decoderCodec;
    };
    typedef vector<CapabilityListCreateEntry> CapabilityCreateListType;
    CapabilityCreateListType capabilityCreateList;
    
};

class H323DynaLink : public PDynaLink
{
  PCLASSINFO(H323DynaLink, PDynaLink)
    
 public:
  H323DynaLink(const char * basename, const char * reason = NULL);

  virtual void Load();
  virtual BOOL IsLoaded()
  { PWaitAndSignal m(processLock); return isLoadedOK; }
  virtual BOOL LoadPlugin (const PString & fileName);

protected:
  PMutex processLock;
  BOOL isLoadedOK;
  const char * baseName;
  const char * reason;
};

//////////////////////////////////////////////////////
//
//  this is the base class for codecs accesible via the abstract factory functions
//

/**Class for codcs which is accessible via the abstract factor functions.
   The code would be :

      PFactory<OpalFactoryCodec>::CreateInstance(conversion);

  to create an instance, where conversion is (eg) "L16:G.711-uLaw-64k"
*/
class OpalFactoryCodec : public PObject {
  PCLASSINFO(OpalFactoryCodec, PObject)
  public:
  /** Return the PluginCodec_Definition, which describes this codec */
    virtual const struct PluginCodec_Definition * GetDefinition()
    { return NULL; }

  /** Return the sourceFormat field of PluginCodec_Definition for this codec*/
    virtual PString GetInputFormat() const = 0;

  /** Return the destFormat field of PluginCodec_Definition for this codec*/
    virtual PString GetOutputFormat() const = 0;

    /** Take the supplied data and apply the conversion specified by CreateInstance call (above). When this method returns, toLen contains the number of bytes placed in the destination buffer. */
    virtual int Encode(const void * from,      ///< pointer to the source data
                         unsigned * fromLen,   ///< number of bytes in the source data to process
                             void * to,        ///< pointer to the destination buffer, which contains the output of the  conversion
		                 unsigned * toLen,     ///< Number of available bytes in the destination buffer
                     unsigned int * flag       ///< Typically, this is not used.
		       ) = 0;  

    /** Return the  sampleRate field of PluginCodec_Definition for this codec*/
    virtual unsigned int GetSampleRate() const = 0;

    /** Return the  bitsPerSec field of PluginCodec_Definition for this codec*/
    virtual unsigned int GetBitsPerSec() const = 0;

    /** Return the  nmPerFrame field of PluginCodec_Definition for this codec*/
    virtual unsigned int GetFrameTime() const = 0;

    /** Return the samplesPerFrame  field of PluginCodec_Definition for this codec*/
    virtual unsigned int GetSamplesPerFrame() const = 0;

    /** Return the  bytesPerFrame field of PluginCodec_Definition for this codec*/
    virtual unsigned int GetBytesPerFrame() const = 0;

    /** Return the recommendedFramesPerPacket field of PluginCodec_Definition for this codec*/
    virtual unsigned int GetRecommendedFramesPerPacket() const = 0;

    /** Return the maxFramesPerPacket field of PluginCodec_Definition for this codec*/
    virtual unsigned int GetMaxFramesPerPacket() const = 0;

    /** Return the  rtpPayload field of PluginCodec_Definition for this codec*/
    virtual BYTE         GetRTPPayload() const = 0;

    /** Return the  sampleRate field of PluginCodec_Definition for this codec*/
    virtual PString      GetSDPFormat() const = 0;
};

#endif
