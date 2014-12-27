
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

  OpalFactoryCodec *fcodec = CreateOpalFactoryCodec(capability, direction);
  if(fcodec)
  {
    PluginCodec_Definition * defn = (PluginCodec_Definition *)fcodec->GetDefinition();
    if(capability->GetMainType() == MCUCapability::e_Audio)
      return new MCUFramedAudioCodec(capability->GetMediaFormat(), direction, defn);
    else
      return new MCUVideoCodec(capability->GetMediaFormat(), direction, defn);
  }

  MCUTRACE(1, "MCUCapability\tWarning! Create default codec " << formatName);
  return capability->CreateCodec(direction);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

OpalFactoryCodec * MCUCapability::CreateOpalFactoryCodec(const H323Capability * capability, MCUCodec::Direction direction)
{
  if(capability == NULL)
    return NULL;

#if MCU_OPENH323_VERSION < MCU_OPENH323_VERSION_INT(1,23,0)
  typedef PFactory<OpalFactoryCodec, PDefaultPFactoryKey> OpalPluginCodecFactory;
#else
  typedef PFactory<OpalFactoryCodec, PString> OpalPluginCodecFactory;
#endif

  const OpalMediaFormat & mediaFormat = capability->GetMediaFormat();
  PString formatName = mediaFormat;
  formatName.Replace("{sw}", "");

  OpalFactoryCodec *fcodec = NULL;

  if(capability->GetMainType() == MCUCapability::e_Audio)
  {
    if(direction == MCUCodec::Decoder)
      fcodec = OpalPluginCodecFactory::CreateInstance(formatName + "|L16");
    else
      fcodec = OpalPluginCodecFactory::CreateInstance("L16|" + formatName);
  }
  else
  {
    if(direction == MCUCodec::Decoder)
      fcodec = OpalPluginCodecFactory::CreateInstance(formatName + "|YUV420P");
    else
      fcodec = OpalPluginCodecFactory::CreateInstance("YUV420P|" + formatName);
  }

  return fcodec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
