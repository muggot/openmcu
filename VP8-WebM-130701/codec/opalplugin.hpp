/*
 * opalplugins.hpp
 *
 * OPAL codec plugins handler (C++ version)
 *
 * Open Phone Abstraction Library (OPAL)
 * Formally known as the Open H323 project.
 *
 * Copyright (C) 2010 Vox Lucida
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.

 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#ifndef OPAL_CODEC_OPALPLUGIN_HPP
#define OPAL_CODEC_OPALPLUGIN_HPP

#include "opalplugin.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <map>
#include <string>


/////////////////////////////////////////////////////////////////////////////

#ifndef PLUGINCODEC_TRACING
  #define PLUGINCODEC_TRACING 1
#endif

#if PLUGINCODEC_TRACING
  extern PluginCodec_LogFunction PluginCodec_LogFunctionInstance;
  extern int PluginCodec_SetLogFunction(const PluginCodec_Definition *, void *, const char *, void * parm, unsigned * len);

#define PLUGINCODEC_CONTROL_LOG_FUNCTION_DEF \
  PluginCodec_LogFunction PluginCodec_LogFunctionInstance; \
  int PluginCodec_SetLogFunction(const PluginCodec_Definition *, void *, const char *, void * parm, unsigned * len) \
  { \
    if (len == NULL || *len != sizeof(PluginCodec_LogFunction)) \
      return false; \
 \
    PluginCodec_LogFunctionInstance = (PluginCodec_LogFunction)parm; \
    if (PluginCodec_LogFunctionInstance != NULL) \
      PluginCodec_LogFunctionInstance(4, __FILE__, __LINE__, "Plugin", "Started logging."); \
 \
    return true; \
  } \

  #define PLUGINCODEC_CONTROL_LOG_FUNCTION_INC { PLUGINCODEC_CONTROL_SET_LOG_FUNCTION, PluginCodec_SetLogFunction },
#else
  #define PLUGINCODEC_CONTROL_LOG_FUNCTION_DEF
  #define PLUGINCODEC_CONTROL_LOG_FUNCTION_INC
#endif

#if !defined(PTRACE)
  #if PLUGINCODEC_TRACING
    #include <sstream>
    #define PTRACE_CHECK(level) \
        (PluginCodec_LogFunctionInstance != NULL && PluginCodec_LogFunctionInstance(level, NULL, 0, NULL, NULL))
    #define PTRACE(level, section, args) \
      if (PTRACE_CHECK(level)) { \
        std::ostringstream strm; strm << args; \
        PluginCodec_LogFunctionInstance(level, __FILE__, __LINE__, section, strm.str().c_str()); \
      } else (void)0
  #else
    #define PTRACE_CHECK(level)
    #define PTRACE(level, section, expr)
  #endif
#endif


/////////////////////////////////////////////////////////////////////////////

class PluginCodec_RTP
{
    unsigned char * m_packet;
    unsigned        m_maxSize;
    unsigned        m_headerSize;
    unsigned        m_payloadSize;

  public:
    PluginCodec_RTP(const void * packet, unsigned size)
      : m_packet((unsigned char *)packet)
      , m_maxSize(size)
      , m_headerSize(PluginCodec_RTP_GetHeaderLength(m_packet))
      , m_payloadSize(size - m_headerSize)
    {
    }

    __inline unsigned GetMaxSize() const           { return m_maxSize; }
    __inline unsigned GetPacketSize() const        { return m_headerSize+m_payloadSize; }
    __inline unsigned GetHeaderSize() const        { return m_headerSize; }
    __inline unsigned GetPayloadSize() const       { return m_payloadSize; }
    __inline bool     SetPayloadSize(unsigned size)
    {
      if (m_headerSize+size > m_maxSize)
        return false;
      m_payloadSize = size;
      return true;
    }

    __inline unsigned GetPayloadType() const         { return PluginCodec_RTP_GetPayloadType(m_packet);        }
    __inline void     SetPayloadType(unsigned type)  {        PluginCodec_RTP_SetPayloadType(m_packet, type);  }
    __inline bool     GetMarker() const              { return PluginCodec_RTP_GetMarker(m_packet);             }
    __inline void     SetMarker(bool mark)           {        PluginCodec_RTP_SetMarker(m_packet, mark);       }
    __inline unsigned GetTimestamp() const           { return PluginCodec_RTP_GetTimestamp(m_packet);          }
    __inline void     SetTimestamp(unsigned ts)      {        PluginCodec_RTP_SetTimestamp(m_packet, ts);      }
    __inline unsigned GetSequenceNumber() const      { return PluginCodec_RTP_GetSequenceNumber(m_packet);     }
    __inline void     SetSequenceNumber(unsigned sn) {        PluginCodec_RTP_SetSequenceNumber(m_packet, sn); }
    __inline unsigned GetSSRC() const                { return PluginCodec_RTP_GetSSRC(m_packet);               }
    __inline void     SetSSRC(unsigned ssrc)         {        PluginCodec_RTP_SetSSRC(m_packet, ssrc);         }

    __inline unsigned char * SetExtended(unsigned id, unsigned len)
    {
      m_packet[0] |= 0x10;

      unsigned char * ptr = m_packet + PluginCodec_RTP_GetCSRCHdrLength(m_packet);
      switch (id >> 16) {
        case 0 :
          PluginCodec_RTP_SetWORD(ptr, 0, id);
          PluginCodec_RTP_SetWORD(ptr, 2, (len+3)/4);
          ptr += 4;
          break;

        case 1 :
          *ptr++ = 0xbe;
          *ptr++ = 0xde;
          PluginCodec_RTP_SetWORD(ptr, 0, (len+7)/4); ptr += 2;
          *ptr++ = (unsigned char)(((id&0xf) << 4)|(len-1));
          break;

        case 2 :
          *ptr++ = 0x10;
          *ptr++ = 0x00;
          PluginCodec_RTP_SetWORD(ptr, 0, (len+8)/4); ptr += 2;
          *ptr++ = (unsigned char)(id&0xff);
          *ptr++ = (unsigned char)(len&0xff);
      }

      m_headerSize = PluginCodec_RTP_GetHeaderLength(m_packet);
      return ptr;
    }

    __inline unsigned char * GetExtendedHeader(unsigned & id, size_t & len) const
    {
      if ((m_packet[0]&0x10) == 0)
        return NULL;

      unsigned char * ptr = m_packet + PluginCodec_RTP_GetCSRCHdrLength(m_packet);
      id = PluginCodec_RTP_GetWORD(ptr, 0);

      if (id == 0xbede) {
        id = (0x10000|(ptr[4] >> 4));
        len = (ptr[4] & 0xf)+1;
        return ptr + 5;
      }

      if ((id&0xfff0) == 0x1000) {
        id = 0x20000 | ptr[4];
        len = ptr[5];
        return ptr + 6;
      }

      len = PluginCodec_RTP_GetWORD(ptr, 2)*4;
      return ptr + 4;
    }

    __inline unsigned char * GetPayloadPtr() const   { return m_packet + m_headerSize; }
    __inline unsigned char & operator[](size_t offset) { return m_packet[m_headerSize + offset]; }
    __inline unsigned const char & operator[](size_t offset) const { return m_packet[m_headerSize + offset]; }
    __inline bool CopyPayload(const void * data, size_t size, size_t offset = 0)
    {
      if (!SetPayloadSize(offset + size))
        return false;
      memcpy(GetPayloadPtr()+offset, data, size);
      return true;
    }

    __inline PluginCodec_Video_FrameHeader * GetVideoHeader() const { return (PluginCodec_Video_FrameHeader *)GetPayloadPtr(); }
    __inline unsigned char * GetVideoFrameData() const { return m_packet + m_headerSize + sizeof(PluginCodec_Video_FrameHeader); }
};


/////////////////////////////////////////////////////////////////////////////

typedef std::map<std::string, std::string> PluginCodec_OptionMapBase;

class PluginCodec_Utilities
{
  public:
   static unsigned String2Unsigned(const std::string & str)
    {
      return strtoul(str.c_str(), NULL, 10);
    }


    static void AppendUnsigned2String(unsigned value, std::string & str)
    {
      // Not very efficient, but really, really simple
      if (value > 9)
        AppendUnsigned2String(value/10, str);
      str += (char)(value%10 + '0');
    }


    static void Unsigned2String(unsigned value, std::string & str)
    {
      str.clear();
      AppendUnsigned2String(value,str);
    }


    static void Change(const char * value,
                       PluginCodec_OptionMapBase & original,
                       PluginCodec_OptionMapBase & changed,
                       const char * option)
    {
      PluginCodec_OptionMapBase::iterator it = original.find(option);
      if (it != original.end() && it->second != value)
        changed[option] = value;
    }


    static void Change(unsigned     value,
                       PluginCodec_OptionMapBase  & original,
                       PluginCodec_OptionMapBase  & changed,
                       const char * option)
    {
      if (String2Unsigned(original[option]) != value)
        Unsigned2String(value, changed[option]);
    }


    static void ClampMax(unsigned     maximum,
                         PluginCodec_OptionMapBase  & original,
                         PluginCodec_OptionMapBase  & changed,
                         const char * option,
                         bool forceIfZero = false)
    {
      unsigned value = String2Unsigned(original[option]);
      if (value > maximum || (forceIfZero && value == 0))
        Unsigned2String(maximum, changed[option]);
    }


    static void ClampMin(unsigned     minimum,
                         PluginCodec_OptionMapBase  & original,
                         PluginCodec_OptionMapBase  & changed,
                         const char * option)
    {
      unsigned value = String2Unsigned(original[option]);
      if (value < minimum)
        Unsigned2String(minimum, changed[option]);
    }


    static unsigned GetMacroBlocks(unsigned width, unsigned height)
    {
      return ((width+15)/16) * ((height+15)/16);
    }


    static bool ClampResolution(
      unsigned & width,
      unsigned & height,
      unsigned & maxFrameSize)
    {
      static struct {
        unsigned m_width;
        unsigned m_height;
        unsigned m_macroblocks;
      } MaxVideoResolutions[] = {
        #define OPAL_PLUGIN_CLAMPED_RESOLUTION(width, height) { width, height, ((width+15)/16) * ((height+15)/16) }
        OPAL_PLUGIN_CLAMPED_RESOLUTION(2816, 2304),
        OPAL_PLUGIN_CLAMPED_RESOLUTION(1920, 1080),
        OPAL_PLUGIN_CLAMPED_RESOLUTION(1408, 1152),
        OPAL_PLUGIN_CLAMPED_RESOLUTION(1280,  720),
        OPAL_PLUGIN_CLAMPED_RESOLUTION( 704,  576),
        OPAL_PLUGIN_CLAMPED_RESOLUTION( 640,  480),
        OPAL_PLUGIN_CLAMPED_RESOLUTION( 352,  288),
        OPAL_PLUGIN_CLAMPED_RESOLUTION( 320,  240),
        OPAL_PLUGIN_CLAMPED_RESOLUTION( 176,  144),
        OPAL_PLUGIN_CLAMPED_RESOLUTION( 128,   96)
      };
      static size_t const LastMaxVideoResolutions = sizeof(MaxVideoResolutions)/sizeof(MaxVideoResolutions[0]) - 1;
      static unsigned const MinWidth = 4*16;  // Four macroblocks wide
      static unsigned const MinHeight = 3*16; // Three macroblocks high

      unsigned maxWidth  = maxFrameSize*16*16/MinHeight;
      unsigned maxHeight = maxFrameSize*16*16/MinWidth;

      // Check if total frame size below threshold total of macroblocks.
      unsigned macroBlocks = GetMacroBlocks(width, height);
      if (macroBlocks <= maxFrameSize &&
          width  >= MinWidth  && width  <= maxWidth &&
          height >= MinHeight && height <= maxHeight)
        return false;

      size_t i = 0;
      while (i < LastMaxVideoResolutions &&
              (MaxVideoResolutions[i].m_macroblocks > maxFrameSize ||
              MaxVideoResolutions[i].m_width > maxWidth ||
              MaxVideoResolutions[i].m_height > maxHeight))
        ++i;
      width = MaxVideoResolutions[i].m_width;
      height = MaxVideoResolutions[i].m_height;
      macroBlocks = MaxVideoResolutions[i].m_macroblocks;
      return true;
    }
};


class PluginCodec_OptionMap : public PluginCodec_OptionMapBase, public PluginCodec_Utilities
{
  public:
    PluginCodec_OptionMap(const char * const * * options = NULL)
    {
      if (options != NULL) {
        for (const char * const * option = *options; *option != NULL; option += 2)
          insert(value_type(option[0], option[1]));
      }
    }


    unsigned GetUnsigned(const char * key, unsigned dflt = 0) const
    {
      const_iterator it = find(key);
      return it == end() ? dflt : String2Unsigned(it->second);
    }

    void SetUnsigned(unsigned value, const char * key)
    {
      Unsigned2String(value, operator[](key));
    }


    char ** GetOptions() const
    {
      char ** options = (char **)calloc(size()*2+1, sizeof(char *));
      if (options == NULL) {
        PTRACE(1, "Plugin", "Could not allocate new option lists.");
        return NULL;
      }

      char ** opt = options;
      for (const_iterator it = begin(); it != end(); ++it) {
        *opt++ = strdup(it->first.c_str());
        *opt++ = strdup(it->second.c_str());
      }

      return options;
    }
};


template<typename NAME>
class PluginCodec_MediaFormat : public PluginCodec_Utilities
{
  public:
    typedef struct PluginCodec_Option const * const * OptionsTable;
    typedef PluginCodec_OptionMap OptionMap;

  protected:
    const char * m_formatName;
    const char * m_payloadName;
    unsigned     m_payloadType;
    const char * m_description;
    unsigned     m_maxBandwidth;
    unsigned     m_h323CapabilityType;
    const void * m_h323CapabilityData;
    unsigned     m_flags;
    OptionsTable m_options;

  protected:
    PluginCodec_MediaFormat(
        const char * formatName,
        const char * payloadName,
        const char * description,
        unsigned     maxBandwidth,
        OptionsTable options)
      : m_formatName(formatName)
      , m_payloadName(payloadName)
      , m_payloadType(0)
      , m_description(description)
      , m_maxBandwidth(maxBandwidth)
      , m_h323CapabilityType(PluginCodec_H323Codec_NoH323)
      , m_h323CapabilityData(NULL)
      , m_flags(PluginCodec_RTPTypeDynamic)
      , m_options(options)
    {
    }

  public:
    virtual ~PluginCodec_MediaFormat()
    {
    }


    __inline const char *  GetFormatName() const   { return this->m_formatName; }
    __inline const char *  GetPayloadName() const  { return this->m_payloadName; }
    __inline unsigned char GetPayloadType() const  { return (unsigned char)this->m_payloadType; }
    __inline const char *  GetDescription() const  { return this->m_description; }
    __inline unsigned      GetMaxBandwidth() const { return this->m_maxBandwidth; }
    __inline unsigned char GetH323CapabilityType() const { return (unsigned char)this->m_h323CapabilityType; }
    __inline const void *  GetH323CapabilityData() const { return this->m_h323CapabilityData; }
    __inline unsigned      GetFlags() const { return this->m_flags; }
    __inline const void *  GetOptionsTable() const { return this->m_options; }


    /// Determine if codec is valid for the specified protocol
    virtual bool IsValidForProtocol(const char * /*protocol*/)
    {
      return true;
    }


    /// Utility function to adjust option strings, used by ToNormalised()/ToCustomised().
    bool AdjustOptions(void * parm, unsigned * parmLen, bool (PluginCodec_MediaFormat:: * adjuster)(OptionMap & original, OptionMap & changed))
    {
      if (parmLen == NULL || parm == NULL || *parmLen != sizeof(char ***)) {
        PTRACE(1, "Plugin", "Invalid parameters to AdjustOptions.");
        return false;
      }

      OptionMap originalOptions((const char * const * *)parm);
      OptionMap changedOptions;
      if (!(this->*adjuster)(originalOptions, changedOptions)) {
        PTRACE(1, "Plugin", "Could not normalise/customise options.");
        return false;
      }

      return (*(char ***)parm = changedOptions.GetOptions()) != NULL;
    }


    /// Adjust normalised options calculated from codec specific options.
    virtual bool ToNormalised(OptionMap & /*original*/, OptionMap & /*changed*/)
    {
      return true;
    }


    // Adjust codec specific options calculated from normalised options.
    virtual bool ToCustomised(OptionMap & /*original*/, OptionMap & /*changed*/)
    {
      return true;
    }


    virtual void AdjustForVersion(unsigned version, const PluginCodec_Definition * /*definition*/)
    {
      if (version < PLUGIN_CODEC_VERSION_INTERSECT) {
        for (PluginCodec_Option ** options = (PluginCodec_Option **)m_options; *options != NULL; ++options) {
          if (strcmp((*options)->m_name, PLUGINCODEC_MEDIA_PACKETIZATIONS) == 0) {
            *options = NULL;
            break;
          }
        }
      }
    }


    static void AdjustAllForVersion(unsigned version, const PluginCodec_Definition * definitions, size_t size)
    {
      while (size-- > 0) {
        PluginCodec_MediaFormat * info = (PluginCodec_MediaFormat *)definitions->userData;
        if (info != NULL)
          info->AdjustForVersion(version, definitions);
        ++definitions;
      }
    }
};


template<typename NAME>
class PluginCodec_AudioFormat : public PluginCodec_MediaFormat<NAME>
{
  protected:
    unsigned m_samplesPerFrame;
    unsigned m_bytesPerFrame;
    unsigned m_sampleRate;
    unsigned m_recommendedFramesPerPacket;
    unsigned m_maxFramesPerPacket;

  public:
    typedef PluginCodec_MediaFormat<NAME> Parent;
    typedef typename Parent::OptionsTable OptionsTable;

    PluginCodec_AudioFormat(
      const char * formatName,
      const char * payloadName,
      const char * description,
      unsigned     samplesPerFrame,
      unsigned     bytesPerFrame,
      unsigned     sampleRate,
      OptionsTable options
    ) : Parent(formatName, payloadName, description, bytesPerFrame*8 * samplesPerFrame*1000000/sampleRate, options)
      , m_samplesPerFrame(samplesPerFrame)
      , m_bytesPerFrame(bytesPerFrame)
      , m_sampleRate(sampleRate)
      , m_recommendedFramesPerPacket((50*sampleRate)/(1000*samplesPerFrame))
      , m_maxFramesPerPacket((120*sampleRate)/(1000*samplesPerFrame))
    {
      this->m_flags = PluginCodec_MediaTypeAudio /* audio codec */
                    | PluginCodec_InputTypeRaw   /* raw input data */
                    | PluginCodec_OutputTypeRaw; /* raw output data */

    }


    __inline unsigned GetSamplesPerFrame() const { return this->m_samplesPerFrame; }
    __inline unsigned GetBytesPerFrame() const { return this->m_bytesPerFrame; }
    __inline unsigned GetSampleRate() const { return this->m_sampleRate; }
    __inline unsigned GetFrameTime() const { return this->m_samplesPerFrame*1000000/this->m_sampleRate; }
    __inline unsigned GetRecommendedFramesPerPacket() const { return this->m_recommendedFramesPerPacket; }
    __inline unsigned GetMaxFramesPerPacket() const { return this->m_maxFramesPerPacket; }
};


template<typename NAME>
class PluginCodec_VideoFormat : public PluginCodec_MediaFormat<NAME>
{
  protected:
    unsigned m_maxWidth;
    unsigned m_maxHeight;

  public:
    typedef PluginCodec_MediaFormat<NAME> Parent;
    typedef typename Parent::OptionsTable OptionsTable;

    PluginCodec_VideoFormat(
      const char * formatName,
      const char * payloadName,
      const char * description,
      unsigned     maxBandwidth,
      OptionsTable options
    ) : Parent(formatName, payloadName, description, maxBandwidth, options)
      , m_maxWidth(1920)
      , m_maxHeight(1200)
    {
      this->m_flags = PluginCodec_MediaTypeVideo /* audio codec */
                    | PluginCodec_InputTypeRTP   /* raw input data */
                    | PluginCodec_OutputTypeRTP; /* raw output data */
    }


    __inline unsigned GetMaxWidth() const { return this->m_maxWidth; }
    __inline unsigned GetMaxHeight() const { return this->m_maxHeight; }
};


/////////////////////////////////////////////////////////////////////////////

template<typename NAME>
class PluginCodec : public PluginCodec_Utilities
{
  protected:
    PluginCodec(const PluginCodec_Definition * defn)
      : m_definition(defn)
      , m_optionsSame(false)
      , m_maxBitRate(defn->bitsPerSec)
      , m_frameTime((defn->sampleRate/1000*defn->usPerFrame)/1000) // Odd way of calculation to avoid 32 bit integer overflow
    {
      PTRACE(3, "Plugin", "Codec created: \"" << defn->descr
             << "\", \"" << defn->sourceFormat << "\" -> \"" << defn->destFormat << '"');
    }


  public:
    virtual ~PluginCodec()
    {
    }


    /// Complete construction of the plug in codec.
    virtual bool Construct()
    {
      return true;
    }


    /** Terminate operation of plug in codec.
        This is generally not needed but sometimes (e.g. fax) there is some
        clean up required to be done on completion of the codec run.
      */
    static bool Terminate()
    {
      return true;
    }


    /// Convert from one media format to another.
    virtual bool Transcode(const void * fromPtr,
                             unsigned & fromLen,
                                 void * toPtr,
                             unsigned & toLen,
                             unsigned & flags) = 0;


    /// Gather any statistics as a string into the provide buffer.
    virtual bool GetStatistics(char * /*bufferPtr*/, unsigned /*bufferSize*/)
    {
      return true;
    }


    /// Get the required output buffer size to be passed into Transcode.
    virtual size_t GetOutputDataSize()
    {
      return 576-20-16; // Max safe MTU size (576 bytes as per RFC879) minus IP & UDP headers
    }


    /** Set the instance ID for the codec.
        This is used to match up the encode and decoder pairs of instances for
        a given call. While most codecs like G.723.1 are purely unidirectional,
        some a bidirectional and have information flow between encoder and
        decoder.
      */
    virtual bool SetInstanceID(const char * /*idPtr*/, unsigned /*idLen*/)
    {
      return true;
    }


    /// Get options that are "active" and may be different from the last SetOptions() call.
    virtual bool GetActiveOptions(PluginCodec_OptionMap & /*options*/)
    {
      return false;
    }


    /// Set all the options for the codec.
    virtual bool SetOptions(const char * const * options)
    {
      this->m_optionsSame = true;

      // get the media format options after adjustment from protocol negotiation
      for (const char * const * option = options; *option != NULL; option += 2) {
        if (!this->SetOption(option[0], option[1])) {
          PTRACE(1, "Plugin", "Could not set option \"" << option[0] << "\" to \"" << option[1] << '"');
          return false;
        }
      }

      if (this->m_optionsSame)
        return true;

      return this->OnChangedOptions();
    }


    /// Callback for if any options are changed.
    virtual bool OnChangedOptions()
    {
      return true;
    }


    /// Set an individual option of the given name.
    virtual bool SetOption(const char * optionName, const char * optionValue)
    {
      if (strcasecmp(optionName, PLUGINCODEC_OPTION_TARGET_BIT_RATE) == 0)
        return this->SetOptionUnsigned(this->m_maxBitRate, optionValue, 1, this->m_definition->bitsPerSec);

      if (strcasecmp(optionName, PLUGINCODEC_OPTION_FRAME_TIME) == 0)
        return this->SetOptionUnsigned(this->m_frameTime, optionValue,
                                       this->m_definition->sampleRate/1000, this->m_definition->sampleRate); // 1ms to 1 second

      return true;
    }


    template <typename T>
    bool SetOptionUnsigned(T & oldValue, const char * optionValue, unsigned minimum, unsigned maximum = UINT_MAX)
    {
      unsigned newValue = oldValue;
      if (!this->SetOptionUnsigned(newValue, optionValue, minimum, maximum))
        return false;
      oldValue = (T)newValue;
      return true;
    }


    bool SetOptionUnsigned(unsigned & oldValue, const char * optionValue, unsigned minimum, unsigned maximum = UINT_MAX)
    {
      char * end;
      unsigned newValue = strtoul(optionValue, &end, 10);
      if (*end != '\0')
        return false;

      if (newValue < minimum)
        newValue = minimum;
      else if (newValue > maximum)
        newValue = maximum;

      if (oldValue != newValue) {
        oldValue = newValue;
        this->m_optionsSame = false;
      }

      return true;
    }


    template <typename T>
    bool SetOptionBoolean(T & oldValue, const char * optionValue)
    {
      bool opt = oldValue != 0;
      if (!this->SetOptionBoolean(opt, optionValue))
        return false;
      oldValue = (T)opt;
      return true;
    }


    bool SetOptionBoolean(bool & oldValue, const char * optionValue)
    {
      bool newValue;
      if (     strcasecmp(optionValue, "0") == 0 ||
               strcasecmp(optionValue, "n") == 0 ||
               strcasecmp(optionValue, "f") == 0 ||
               strcasecmp(optionValue, "no") == 0 ||
               strcasecmp(optionValue, "false") == 0)
        newValue = false;
      else if (strcasecmp(optionValue, "1") == 0 ||
               strcasecmp(optionValue, "y") == 0 ||
               strcasecmp(optionValue, "t") == 0 ||
               strcasecmp(optionValue, "yes") == 0 ||
               strcasecmp(optionValue, "true") == 0)
        newValue = true;
      else
        return false;

      if (oldValue != newValue) {
        oldValue = newValue;
        this->m_optionsSame = false;
      }

      return true;
    }


    bool SetOptionBit(int & oldValue, unsigned bit, const char * optionValue)
    {
      return this->SetOptionBit((unsigned &)oldValue, bit, optionValue);
    }


    bool SetOptionBit(unsigned & oldValue, unsigned bit, const char * optionValue)
    {
      bool newValue;
      if (strcmp(optionValue, "0") == 0)
        newValue = false;
      else if (strcmp(optionValue, "1") == 0)
        newValue = true;
      else
        return false;

      if (((oldValue&bit) != 0) != newValue) {
        if (newValue)
          oldValue |= bit;
        else
          oldValue &= ~bit;
        this->m_optionsSame = false;
      }

      return true;
    }


    template <class CodecClass> static void * Create_s(const PluginCodec_Definition * defn)
    {
      CodecClass * codec = new CodecClass(defn);
      if (codec != NULL && codec->Construct())
        return codec;

      PTRACE(1, "Plugin", "Could not open codec, no context being returned.");
      delete codec;
      return NULL;
    }


    static void Destroy_s(const PluginCodec_Definition * /*defn*/, void * context)
    {
      delete (PluginCodec *)context;
    }


    static int Transcode_s(const PluginCodec_Definition * /*defn*/,
                                                   void * context,
                                             const void * fromPtr,
                                               unsigned * fromLen,
                                                   void * toPtr,
                                               unsigned * toLen,
                                           unsigned int * flags)
    {
      if (context != NULL && fromPtr != NULL && fromLen != NULL && toPtr != NULL && toLen != NULL && flags != NULL)
        return ((PluginCodec *)context)->Transcode(fromPtr, *fromLen, toPtr, *toLen, *flags);

      PTRACE(1, "Plugin", "Invalid parameter to Transcode.");
      return false;
    }


    static int GetOutputDataSize_s(const PluginCodec_Definition *, void * context, const char *, void *, unsigned *)
    {
      return context != NULL ? ((PluginCodec *)context)->GetOutputDataSize() : 0;
    }

    typedef PluginCodec_MediaFormat<NAME> MediaFormat;

    static int ToNormalised_s(const PluginCodec_Definition * defn, void *, const char *, void * parm, unsigned * len)
    {
      return defn->userData != NULL ? ((MediaFormat *)defn->userData)->AdjustOptions(parm, len, &MediaFormat::ToNormalised) : -1;
    }


    static int ToCustomised_s(const PluginCodec_Definition * defn, void *, const char *, void * parm, unsigned * len)
    {
      return defn->userData != NULL ? ((MediaFormat *)defn->userData)->AdjustOptions(parm, len, &MediaFormat::ToCustomised) : -1;
    }


    static int GetActiveOptions_s(const PluginCodec_Definition *, void * context, const char *, void * parm, unsigned * parmLen)
    {
      if (context == NULL || parmLen == NULL || parm == NULL || *parmLen != sizeof(char ***)) {
        PTRACE(1, "Plugin", "Invalid parameters to GetActiveOptions.");
        return false;
      }

      PluginCodec_OptionMap activeOptions;
      if (!((PluginCodec *)context)->GetActiveOptions(activeOptions))
        return false;

      return (*(char ***)parm = activeOptions.GetOptions()) != NULL;
    }


    static int FreeOptions_s(const PluginCodec_Definition *, void *, const char *, void * parm, unsigned * len)
    {
      if (parm == NULL || len == NULL || *len != sizeof(char ***))
        return false;

      char ** strings = (char **)parm;
      for (char ** string = strings; *string != NULL; string++)
        free(*string);
      free(strings);
      return true;
    }


    static int GetOptions_s(const struct PluginCodec_Definition * codec, void *, const char *, void * parm, unsigned * len)
    {
      if (parm == NULL || len == NULL || *len != sizeof(struct PluginCodec_Option **))
        return false;

      *(const void **)parm = codec->userData != NULL ? ((MediaFormat *)codec->userData)->GetOptionsTable() : NULL;
      *len = 0;
      return true;
    }


    static int SetOptions_s(const PluginCodec_Definition *, void * context, const char *, void * parm, unsigned * len)
    {
      PluginCodec * codec = (PluginCodec *)context;
      return len != NULL && *len == sizeof(const char **) && parm != NULL &&
             codec != NULL && codec->SetOptions((const char * const *)parm);
    }

    static int ValidForProtocol_s(const PluginCodec_Definition * defn, void *, const char *, void * parm, unsigned * len)
    {
      return len != NULL && *len == sizeof(const char *) && parm != NULL && defn->userData != NULL &&
                              ((MediaFormat *)defn->userData)->IsValidForProtocol((const char *)parm);
    }

    static int SetInstanceID_s(const PluginCodec_Definition *, void * context, const char *, void * parm, unsigned * len)
    {
      PluginCodec * codec = (PluginCodec *)context;
      return len != NULL && parm != NULL &&
             codec != NULL && codec->SetInstanceID((const char *)parm, *len);
    }

    static int GetStatistics_s(const PluginCodec_Definition *, void * context, const char *, void * parm, unsigned * len)
    {
      PluginCodec * codec = (PluginCodec *)context;
      return len != NULL && parm != NULL &&
             codec != NULL && codec->GetStatistics((char *)parm, *len);
    }

    static int Terminate_s(const PluginCodec_Definition *, void * context, const char *, void *, unsigned *)
    {
      PluginCodec * codec = (PluginCodec *)context;
      return codec != NULL && codec->Terminate();
    }

    static struct PluginCodec_ControlDefn * GetControls()
    {
      static PluginCodec_ControlDefn ControlsTable[] = {
        { PLUGINCODEC_CONTROL_GET_OUTPUT_DATA_SIZE,  PluginCodec::GetOutputDataSize_s },
        { PLUGINCODEC_CONTROL_TO_NORMALISED_OPTIONS, PluginCodec::ToNormalised_s },
        { PLUGINCODEC_CONTROL_TO_CUSTOMISED_OPTIONS, PluginCodec::ToCustomised_s },
        { PLUGINCODEC_CONTROL_SET_CODEC_OPTIONS,     PluginCodec::SetOptions_s },
        { PLUGINCODEC_CONTROL_GET_ACTIVE_OPTIONS,    PluginCodec::GetActiveOptions_s },
        { PLUGINCODEC_CONTROL_GET_CODEC_OPTIONS,     PluginCodec::GetOptions_s },
        { PLUGINCODEC_CONTROL_FREE_CODEC_OPTIONS,    PluginCodec::FreeOptions_s },
        { PLUGINCODEC_CONTROL_VALID_FOR_PROTOCOL,    PluginCodec::ValidForProtocol_s },
        { PLUGINCODEC_CONTROL_SET_INSTANCE_ID,       PluginCodec::SetInstanceID_s },
        { PLUGINCODEC_CONTROL_GET_STATISTICS,        PluginCodec::GetStatistics_s },
        { PLUGINCODEC_CONTROL_TERMINATE_CODEC,       PluginCodec::Terminate_s },
        PLUGINCODEC_CONTROL_LOG_FUNCTION_INC
        { NULL }
      };
      return ControlsTable;
    }

  protected:
    const PluginCodec_Definition * m_definition;

    bool     m_optionsSame;
    unsigned m_maxBitRate;
    unsigned m_frameTime;
};


/////////////////////////////////////////////////////////////////////////////

template<typename NAME>
class PluginVideoCodec : public PluginCodec<NAME>
{
    typedef PluginCodec<NAME> BaseClass;

  public:
    enum {
      DefaultWidth = 352, // CIF size
      DefaultHeight = 288,
      MaxWidth = DefaultWidth*8,
      MaxHeight = DefaultHeight*8
    };


    PluginVideoCodec(const PluginCodec_Definition * defn)
      : BaseClass(defn)
    {
    }


    virtual size_t GetRawFrameSize(unsigned width, unsigned height)
    {
      return width*height*3/2; // YUV420P
    }
};


/////////////////////////////////////////////////////////////////////////////

template<typename NAME>
class PluginVideoEncoder : public PluginVideoCodec<NAME>
{
    typedef PluginVideoCodec<NAME> BaseClass;

  protected:
    unsigned m_width;
    unsigned m_height;
    unsigned m_maxRTPSize;
    unsigned m_tsto;
    unsigned m_keyFramePeriod;

  public:
    PluginVideoEncoder(const PluginCodec_Definition * defn)
      : BaseClass(defn)
      , m_width(BaseClass::DefaultWidth)
      , m_height(BaseClass::DefaultHeight)
      , m_maxRTPSize(PluginCodec_RTP_MaxPacketSize)
      , m_tsto(31)
      , m_keyFramePeriod(0) // Indicates auto/default
    {
    }


    virtual bool SetOption(const char * optionName, const char * optionValue)
    {
      if (strcasecmp(optionName, PLUGINCODEC_OPTION_FRAME_WIDTH) == 0)
        return this->SetOptionUnsigned(this->m_width, optionValue, 16, BaseClass::MaxWidth);

      if (strcasecmp(optionName, PLUGINCODEC_OPTION_FRAME_HEIGHT) == 0)
        return this->SetOptionUnsigned(this->m_height, optionValue, 16, BaseClass::MaxHeight);

      if (strcasecmp(optionName, PLUGINCODEC_OPTION_MAX_TX_PACKET_SIZE) == 0)
        return this->SetOptionUnsigned(this->m_maxRTPSize, optionValue, 256, 8192);

      if (strcasecmp(optionName, PLUGINCODEC_OPTION_TEMPORAL_SPATIAL_TRADE_OFF) == 0)
        return this->SetOptionUnsigned(this->m_tsto, optionValue, 1, 31);

      if (strcasecmp(optionName, PLUGINCODEC_OPTION_TX_KEY_FRAME_PERIOD) == 0)
        return this->SetOptionUnsigned(this->m_keyFramePeriod, optionValue, 0);

      // Base class sets bit rate and frame time
      return BaseClass::SetOption(optionName, optionValue);
    }


    /// Get options that are "active" and may be different from the last SetOptions() call.
    virtual bool GetActiveOptions(PluginCodec_OptionMap & options)
    {
      options.SetUnsigned(this->m_frameTime, PLUGINCODEC_OPTION_FRAME_TIME);
      return true;
    }


    virtual size_t GetPacketSpace(const PluginCodec_RTP & rtp, size_t total)
    {
      size_t space = rtp.GetMaxSize();
      if (space > this->m_maxRTPSize)
        space = this->m_maxRTPSize;
      space -= rtp.GetHeaderSize();
      if (space > total)
        space = total;
      return space;
    }
};


/////////////////////////////////////////////////////////////////////////////

template<typename NAME>
class PluginVideoDecoder : public PluginVideoCodec<NAME>
{
    typedef PluginVideoCodec<NAME> BaseClass;

  protected:
    size_t m_outputSize;

  public:
    PluginVideoDecoder(const PluginCodec_Definition * defn)
      : BaseClass(defn)
      , m_outputSize(BaseClass::DefaultWidth*BaseClass::DefaultHeight*3/2 + sizeof(PluginCodec_Video_FrameHeader) + PluginCodec_RTP_MinHeaderSize)
    {
    }


    virtual size_t GetOutputDataSize()
    {
      return this->m_outputSize;
    }


    virtual bool CanOutputImage(unsigned width, unsigned height, PluginCodec_RTP & rtp, unsigned & flags)
    {
      size_t newSize = this->GetRawFrameSize(width, height) + sizeof(PluginCodec_Video_FrameHeader) + rtp.GetHeaderSize();
      if (newSize > rtp.GetMaxSize() || !rtp.SetPayloadSize(newSize)) {
        m_outputSize = newSize;
        flags |= PluginCodec_ReturnCoderBufferTooSmall;
        return false;
      }

      PluginCodec_Video_FrameHeader * videoHeader = rtp.GetVideoHeader();
      videoHeader->x = 0;
      videoHeader->y = 0;
      videoHeader->width = width;
      videoHeader->height = height;

      flags |= PluginCodec_ReturnCoderLastFrame;
      rtp.SetMarker(true);
      return true;
    }


    struct OutputImagePlaneInfo
    {
      unsigned        m_width;
      unsigned        m_height;
      unsigned        m_raster;
      unsigned char * m_source;
      unsigned char * m_destination;

      void Copy()
      {
        for (unsigned y = 0; y < m_height; ++y) {
          memcpy(m_destination, m_source, m_width);
          this->m_source += m_raster;
          this->m_destination += m_width;
        }
      }
    };

    virtual unsigned OutputImage(unsigned char * planes[3], int raster[3],
                                 unsigned width, unsigned height, PluginCodec_RTP & rtp, unsigned & flags)
    {
      if (!CanOutputImage(width, height, rtp, flags))
        return 0;

      size_t ySize = width*height;
      size_t uvSize = ySize/4;
      if (planes[1] == planes[0]+ySize && planes[2] == planes[1]+uvSize)
        memcpy(rtp.GetVideoFrameData(), planes[0], ySize+uvSize*2);
      else {
        OutputImagePlaneInfo planeInfo[3] = {
          { width,   height,   raster[0], planes[0], rtp.GetVideoFrameData()     },
          { width/2, height/2, raster[1], planes[1], planeInfo[0].m_destination + ySize  },
          { width/2, height/2, raster[2], planes[2], planeInfo[1].m_destination + uvSize }
        };

        for (unsigned plane = 0; plane < 3; ++plane)
          planeInfo[plane].Copy();
      }

      return rtp.GetPacketSize();
    }
};


/////////////////////////////////////////////////////////////////////////////

/// Declare an audio codec using C++ support classes
#define PLUGINCODEC_AUDIO_CODEC_CXX(MediaFormat,     /**< PluginCodec_VideoFormat instance */ \
                                    EncoderClass,    /**< Encoder class name */ \
                                    DecoderClass     /**< Decoder class name */ \
                                    ) \
             PLUGINCODEC_CODEC_PAIR(MediaFormat.GetFormatName(), \
                                    MediaFormat.GetPayloadName(), \
                                    MediaFormat.GetDescription(), \
                                    MediaFormat.GetSampleRate(), \
                                    MediaFormat.GetMaxBandwidth(), \
                                    MediaFormat.GetFrameTime(), \
                                    MediaFormat.GetSamplesPerFrame(), \
                                    MediaFormat.GetBytesPerFrame(), \
                                    MediaFormat.GetRecommendedFramesPerPacket(), \
                                    MediaFormat.GetMaxFramesPerPacket(), \
                                    MediaFormat.GetPayloadType(), \
                                    MediaFormat.GetH323CapabilityType(), \
                                    MediaFormat.GetH323CapabilityData(), \
                                    EncoderClass::Create_s<EncoderClass>, \
                                    EncoderClass::Destroy_s, \
                                    EncoderClass::Transcode_s, \
                                    DecoderClass::Create_s<DecoderClass>, \
                                    DecoderClass::Destroy_s, \
                                    DecoderClass::Transcode_s, \
                                    DecoderClass::GetControls(), /* Note doesn't matter if encoder or decoder */ \
                                    MediaFormat.GetFlags(), \
                                    PLUGINCODEC_RAW_AUDIO, \
                                    &MediaFormat)

/// Declare a video codec using C++ support classes
#define PLUGINCODEC_VIDEO_CODEC_CXX(MediaFormat,     /**< PluginCodec_VideoFormat instance */ \
                                    EncoderClass,    /**< Encoder class name */ \
                                    DecoderClass     /**< Decoder class name */ \
                                    ) \
             PLUGINCODEC_CODEC_PAIR(MediaFormat.GetFormatName(), \
                                    MediaFormat.GetPayloadName(), \
                                    MediaFormat.GetDescription(), \
                                    PLUGINCODEC_VIDEO_CLOCK, \
                                    MediaFormat.GetMaxBandwidth(), \
                                    1000000/PLUGINCODEC_MAX_FRAME_RATE, \
                                    MediaFormat.GetMaxWidth(), \
                                    MediaFormat.GetMaxHeight(), \
                                    0,PLUGINCODEC_MAX_FRAME_RATE, \
                                    MediaFormat.GetPayloadType(), \
                                    MediaFormat.GetH323CapabilityType(), \
                                    MediaFormat.GetH323CapabilityData(), \
                                    EncoderClass::Create_s<EncoderClass>, \
                                    EncoderClass::Destroy_s, \
                                    EncoderClass::Transcode_s, \
                                    DecoderClass::Create_s<DecoderClass>, \
                                    DecoderClass::Destroy_s, \
                                    DecoderClass::Transcode_s, \
                                    DecoderClass::GetControls(), /* Note doesn't matter if encoder or decoder */ \
                                    MediaFormat.GetFlags(), \
                                    PLUGINCODEC_RAW_VIDEO, \
                                    &MediaFormat)


#define PLUGIN_CODEC_IMPLEMENT_CXX(NAME, table) \
  extern "C" { \
    PLUGIN_CODEC_IMPLEMENT(NAME) \
    PLUGIN_CODEC_DLL_API struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned version) { \
      if (version < PLUGIN_CODEC_VERSION_OPTIONS) return NULL; \
      *count = sizeof(table)/sizeof(struct PluginCodec_Definition); \
      PluginCodec_MediaFormat<NAME>::AdjustAllForVersion(version, table, *count); \
      return table; \
    } \
  }


#endif // OPAL_CODEC_OPALPLUGIN_HPP
