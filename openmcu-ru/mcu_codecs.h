
#include "precompile.h"

#ifndef _MCU_CODECS_H
#define _MCU_CODECS_H

#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static const char GET_CODEC_OPTIONS_CONTROL[]    = "get_codec_options";
static const char FREE_CODEC_OPTIONS_CONTROL[]   = "free_codec_options";
static const char GET_OUTPUT_DATA_SIZE_CONTROL[] = "get_output_data_size";
static const char SET_CODEC_OPTIONS_CONTROL[]    = "set_codec_options";
static const char EVENT_CODEC_CONTROL[]          = "event_codec";

////////////////////////////////////////////////////////////////////////////////////////////////////

inline static BOOL CallCodecControl(PluginCodec_Definition * defn, void * context, const char * name, void * parm, unsigned int * parmLen, int & retVal)
{
  PluginCodec_ControlDefn * codecControls = defn->codecControls;
  if (codecControls == NULL)
    return FALSE;

  while (codecControls->name != NULL) {
    if (strcasecmp(codecControls->name, name) == 0) {
      retVal = (*codecControls->control)(defn, context, name, parm, parmLen);
      return TRUE;
    }
    codecControls++;
  }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline static PluginCodec_ControlDefn * GetCodecControl(const PluginCodec_Definition * defn, const char * name)
{
  PluginCodec_ControlDefn * codecControls = defn->codecControls;
  if (codecControls == NULL)
    return NULL;

  while (codecControls->name != NULL) {
    if (strcasecmp(codecControls->name, name) == 0)
      return codecControls;
    codecControls++;
  }

  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline static BOOL SetCodecControl(const PluginCodec_Definition * defn, void * context, const char * name, const char * parm, const char * value)
{
  PluginCodec_ControlDefn * codecControls = GetCodecControl(defn, name);
  if(codecControls == NULL)
    return FALSE;

  char const * options[3] = { parm, value, NULL };
  unsigned optionsLen = sizeof(const char **);

  return (*codecControls->control)(defn, context, SET_CODEC_OPTIONS_CONTROL, options, &optionsLen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline static BOOL SetCodecControl(const PluginCodec_Definition * defn, void * context, const char * name, const char * parm, int value)
{
  return SetCodecControl(defn, context, name, parm, PString(PString::Signed, value));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline static BOOL EventCodecControl(PluginCodec_Definition * defn, void * context, const char * name, const char * parm )
{
  PluginCodec_ControlDefn * codecControls = defn->codecControls;

  if(codecControls == NULL)
    return FALSE;

  return FALSE;
}

//////////////////////////////////////////////////////////////////////////////

#if PTRACING
inline static int PluginLogFunction(unsigned level, const char * file, unsigned line, const char * section, const char * log)
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

////////////////////////////////////////////////////////////////////////////////////////////////////

inline void PopulateMediaFormatOptions(PluginCodec_Definition * defn, OpalMediaFormat & format)
{
  char ** _options = NULL;
  unsigned int optionsLen = sizeof(_options);
  int retVal;

  if(!CallCodecControl(defn, NULL, GET_CODEC_OPTIONS_CONTROL, &_options, &optionsLen, retVal) || _options == NULL)
  {
    PTRACE(6, "Plugin\tUnable to read options from plugin " << format);
    return;
  }

  if(defn->version >= PLUGIN_CODEC_VERSION_OPTIONS)
  {
    struct PluginCodec_Option const * const * options = (struct PluginCodec_Option const * const *)_options;
    PTRACE_IF(6, options != NULL, "Plugin\tAdding plugin options to format " << format);
    while(*options != NULL)
    {
      struct PluginCodec_Option const * option = *options++;
      OpalMediaOption * newOption;
      switch(option->m_type)
      {
        case PluginCodec_StringOption :
          newOption = new OpalMediaOptionString(option->m_name, option->m_readOnly != 0, option->m_value);
          break;

        case PluginCodec_BoolOption :
          newOption = new OpalMediaOptionBoolean(option->m_name, option->m_readOnly != 0,
                                                  (OpalMediaOption::MergeType)option->m_merge,
                                                  option->m_value != NULL && *option->m_value == 'T');
          break;

        case PluginCodec_IntegerOption :
          newOption = new OpalMediaOptionUnsigned(option->m_name, option->m_readOnly != 0,
                                                  (OpalMediaOption::MergeType)option->m_merge,
                                                  PString(option->m_value).AsInteger(),
                                                  PString(option->m_minimum).AsInteger(),
                                                  PString(option->m_maximum).AsInteger());
          break;

        case PluginCodec_RealOption :
          newOption = new OpalMediaOptionReal(option->m_name, option->m_readOnly != 0,
                                                  (OpalMediaOption::MergeType)option->m_merge,
                                                  PString(option->m_value).AsReal(),
                                                  PString(option->m_minimum).AsReal(),
                                                  PString(option->m_maximum).AsReal());
          break;
        case PluginCodec_EnumOption :
        {
          PStringArray valueTokens = PString(option->m_minimum).Tokenise(':');
          char ** enumValues = valueTokens.ToCharArray();
          newOption = new OpalMediaOptionEnum(option->m_name, option->m_readOnly != 0,
                                                  enumValues, valueTokens.GetSize(),
                                                  (OpalMediaOption::MergeType)option->m_merge,
                                                  valueTokens.GetStringsIndex(option->m_value));
          free(enumValues);
          break;
        }
        case PluginCodec_OctetsOption :
          newOption = new OpalMediaOptionOctets(option->m_name, option->m_readOnly != 0, option->m_minimum != NULL); // Use minimum to indicate Base64
          newOption->FromString(option->m_value);
          break;

        default :
          continue;
      }

      newOption->SetFMTPName(option->m_FMTPName);
      newOption->SetFMTPDefault(option->m_FMTPDefault);

      OpalMediaOption::H245GenericInfo genericInfo;
      genericInfo.ordinal = option->m_H245Generic & PluginCodec_H245_OrdinalMask;
      if(option->m_H245Generic & PluginCodec_H245_Collapsing)
        genericInfo.mode = OpalMediaOption::H245GenericInfo::Collapsing;
      else if(option->m_H245Generic&PluginCodec_H245_NonCollapsing)
        genericInfo.mode = OpalMediaOption::H245GenericInfo::NonCollapsing;
      else
        genericInfo.mode = OpalMediaOption::H245GenericInfo::None;
      if(option->m_H245Generic&PluginCodec_H245_Unsigned32)
        genericInfo.integerType = OpalMediaOption::H245GenericInfo::Unsigned32;
      else if(option->m_H245Generic&PluginCodec_H245_BooleanArray)
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
  CallCodecControl(defn, NULL, FREE_CODEC_OPTIONS_CONTROL, _options, &optionsLen, retVal);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUCodec : public H323Codec
{
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUFramedAudioCodec : public H323AudioCodec
{
  PCLASSINFO(MCUFramedAudioCodec, H323AudioCodec);
  public:
    MCUFramedAudioCodec(const OpalMediaFormat & fmtName, Direction direction, PluginCodec_Definition * _codec);
    ~MCUFramedAudioCodec();

    virtual BOOL Open(H323Connection & connection);

    virtual BOOL Read(BYTE * buffer, unsigned & length, RTP_DataFrame & rtpFrame);
    virtual BOOL Write(const BYTE * buffer, unsigned length, const RTP_DataFrame & rtpFrame, unsigned & written);

    virtual BOOL EncodeFrame(BYTE * buffer, unsigned int & length);
    virtual BOOL DecodeFrame(const BYTE * buffer, unsigned length, unsigned & written, unsigned & bytesDecoded);

    virtual unsigned GetAverageSignalLevel();
    virtual BOOL DetectSilence();
    virtual void DecodeSilenceFrame(void * buffer, unsigned length);

    virtual unsigned GetSampleRate()
    { return codec->sampleRate; }

    MCU_RTPChannel * GetLogicalChannel()
    { return (MCU_RTPChannel *)logicalChannel; }

  protected:
    void * context;
    PluginCodec_Definition * codec;

    PShortArray sampleBuffer;
    unsigned bytesPerFrame;
    unsigned sampleRate;
    unsigned channels;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUStreamedAudioCodec : public MCUFramedAudioCodec
{
  PCLASSINFO(MCUStreamedAudioCodec, MCUFramedAudioCodec);
  public:
    MCUStreamedAudioCodec(const OpalMediaFormat & mediaFormat, Direction direction, unsigned samples, unsigned bits, PluginCodec_Definition * _codec);

    virtual BOOL EncodeFrame(BYTE * buffer, unsigned & length);
    virtual BOOL DecodeFrame(const BYTE * buffer, unsigned length, unsigned & written, unsigned & samples);
    virtual int Encode(short sample) const;
    virtual short Decode(int sample) const;

  protected:
    unsigned bitsPerSample;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUVideoCodec : public H323VideoCodec
{
  PCLASSINFO(MCUVideoCodec, H323VideoCodec);
  public:
    MCUVideoCodec(const OpalMediaFormat & fmt, Direction direction, PluginCodec_Definition * _codec);
    ~MCUVideoCodec();

    BOOL Read(BYTE * buffer, unsigned & length, RTP_DataFrame & dst, unsigned & flags);
    virtual BOOL Read(BYTE * buffer, unsigned & length, RTP_DataFrame & dst);
    virtual BOOL Write(const BYTE * buffer, unsigned length, const RTP_DataFrame & src, unsigned & written);

    virtual BOOL SetFrameSize(int width, int height);

    virtual unsigned GetFrameRate() const 
    { return lastFrameTimeRTP; }

    virtual void OnFastUpdatePicture()
    {
      EventCodecControl(codec, context, EVENT_CODEC_CONTROL, "on_fast_update");
      sendIntra = true;
    }

    BOOL RenderFrame(const BYTE * buffer);

    MCU_RTPChannel * GetLogicalChannel()
    { return (MCU_RTPChannel *)logicalChannel; }

  protected:
    void * context;
    PluginCodec_Definition * codec;

    RTP_DataFrame bufferRTP;
    PColourConverter * converter;

    unsigned     bytesPerFrame;
    unsigned     lastFrameTimeRTP;
    unsigned     targetFrameTimeMs;
    int          maxWidth;
    int          maxHeight;
    bool         sendIntra;
    bool         lastPacketSent;

    mutable PTimeInterval lastFrameTick;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_CODECS_H
