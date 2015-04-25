
#include "precompile.h"
#include "mcu.h"
#include "mcu_caps.h"
#include "mcu_codecs.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUCapability * MCUCapability::Create(const PString & name)
{
  MCUCapability * cap = (MCUCapability *)H323CapabilityFactory::CreateInstance(name);
  if(cap == NULL && name.Right(4) != "{sw}")
    cap = (MCUCapability *)H323CapabilityFactory::CreateInstance(name+"{sw}");
  if(cap == NULL && name.Right(4) == "{sw}" && name.GetLength() > 4)
    cap = (MCUCapability *)H323CapabilityFactory::CreateInstance(name.Left(name.GetLength()-4));
  if(cap == NULL)
    return NULL;
  return (MCUCapability *)cap->Clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323Codec * MCUCapability::CreateCodec(const H323Capability * capability, MCUCodec::Direction direction)
{
  if(capability == NULL)
    return NULL;

  PString formatName = capability->GetFormatName();

  // h323plus built-in
  if(formatName.Find("G711") != P_MAX_INDEX || formatName.Find("G.711") != P_MAX_INDEX)
    return capability->CreateCodec(direction);

  PluginCodec_Definition *defn = MCUPluginCodecManager::GetPluginCodec(capability, direction);
  if(defn)
  {
    if(capability->GetMainType() == MCUCapability::e_Audio)
    {
      if(formatName.Find("G726") != P_MAX_INDEX || formatName.Find("G.726") != P_MAX_INDEX)
      {
        int bitsPerSample = (defn->flags & PluginCodec_BitsPerSampleMask) >> PluginCodec_BitsPerSamplePos;
        if (bitsPerSample == 0)
          bitsPerSample = 16;
        return new MCUStreamedAudioCodec(capability->GetMediaFormat(), direction, defn->parm.audio.samplesPerFrame, bitsPerSample, defn);
      }
      else
        return new MCUFramedAudioCodec(capability->GetMediaFormat(), direction, defn);
    }
    else
      return new MCUVideoCodec(capability->GetMediaFormat(), direction, defn);
  }

  MCUTRACE(1, "MCUCapability\tWarning! Create default codec " << formatName);
  return capability->CreateCodec(direction);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUPluginCodecManager::MCUPluginCodecManager(PPluginManager * _pluginMgr)
  : PPluginModuleManager(PLUGIN_CODEC_GET_CODEC_FN_STR, _pluginMgr)
{
  PPluginManager & mgr = PPluginManager::GetPluginManager();
  mgr.AddNotifier(PCREATE_NOTIFIER(OnLoadPlugin));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PluginCodec_Definition * MCUPluginCodecManager::GetPluginCodec(const PString & key)
{
  PWaitAndSignal m(MCUPluginCodecManager::GetPluginCodecListMutex());
  PluginCodecListType & pluginCodecList = MCUPluginCodecManager::GetPluginCodecList();
  PluginCodecListType::iterator it = pluginCodecList.find(key);
  if(it != pluginCodecList.end())
    return it->second;
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PluginCodec_Definition * MCUPluginCodecManager::GetPluginCodec(const H323Capability * capability, MCUCodec::Direction direction)
{
  if(capability == NULL)
    return NULL;

  const OpalMediaFormat & mediaFormat = capability->GetMediaFormat();
  PString formatName = mediaFormat;
  formatName.Replace("{sw}", "");

  PluginCodec_Definition *defn = NULL;

  if(capability->GetMainType() == MCUCapability::e_Audio)
  {
    if(direction == MCUCodec::Decoder)
      defn = MCUPluginCodecManager::GetPluginCodec(formatName + "|L16");
    else
      defn = MCUPluginCodecManager::GetPluginCodec("L16|" + formatName);
  }
  else
  {
    if(direction == MCUCodec::Decoder)
      defn = MCUPluginCodecManager::GetPluginCodec(formatName + "|YUV420P");
    else
      defn = MCUPluginCodecManager::GetPluginCodec("YUV420P|" + formatName);
  }

  return defn;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUPluginCodecManager::OnLoadPlugin(PDynaLink & dll, INT code)
{
  PWaitAndSignal m(MCUPluginCodecManager::GetPluginCodecListMutex());

  PluginCodec_GetCodecFunction *GetCodecsDefn = new PluginCodec_GetCodecFunction;
  if(!dll.GetFunction(PLUGIN_CODEC_GET_CODEC_FN_STR, *(PDynaLink::Function *)GetCodecsDefn))
    return;

  unsigned int count;
  PluginCodec_Definition *codecsDefn = (*GetCodecsDefn)(&count, PLUGIN_CODEC_VERSION_OPTIONS);
  if(codecsDefn == NULL || count == 0)
    return;

  for(unsigned i = 0; i < count; ++i)
  {
    PluginCodec_Definition & defn = codecsDefn[i];
    PString key = PString(defn.sourceFormat) + "|" + PString(defn.destFormat);
    PluginCodecListType & pluginCodecList = MCUPluginCodecManager::GetPluginCodecList();
    if(code == 0)
    {
      PluginCodecListType::iterator it = pluginCodecList.find(key);
      if(it == pluginCodecList.end())
      {
#if PTRACING
        int retVal;
        unsigned parmLen = sizeof(PluginCodec_LogFunction);
        CallCodecControl(&defn, NULL, PLUGINCODEC_CONTROL_SET_LOG_FUNCTION, (void *)PluginLogFunction, &parmLen, retVal);
#endif
        pluginCodecList.insert(PluginCodecListType::value_type(key, &defn));
      }
    }
    else if(code == 1)
      pluginCodecList.erase(key);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUPluginCodecManager::PopulateMediaFormats()
{
  // Setup capabilities
  H323CapabilityFactory::KeyList_T stdCaps = H323CapabilityFactory::GetKeyList();
  for(H323CapabilityFactory::KeyList_T::const_iterator r = stdCaps.begin(); r != stdCaps.end(); ++r)
  {
    PString name = *r;
    H323Capability *capability = H323CapabilityFactory::CreateInstance(name);
    PluginCodec_Definition *defn = MCUPluginCodecManager::GetPluginCodec(capability, MCUCodec::Encoder);
    if(defn)
      PopulateMediaFormatOptions(defn, capability->GetWritableMediaFormat());
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
