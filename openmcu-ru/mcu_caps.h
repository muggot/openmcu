
#ifndef _MCU_CAPS_H
#define _MCU_CAPS_H

#include "config.h"
#include "mcu_codecs.h"

#include <ptlib.h>
#include <ptlib/pluginmgr.h>

#include <h323caps.h>
#include <codec/opalplugin.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUCapability : public H323Capability
{
  public:
    static MCUCapability * Create(const PString & name);
    static H323Codec * CreateCodec(const H323Capability * capability, MCUCodec::Direction direction);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::map<PString, PluginCodec_Definition *> PluginCodecListType;
class MCUPluginCodecManager : public PPluginModuleManager
{
  PCLASSINFO(MCUPluginCodecManager, PPluginModuleManager);

  public:
    MCUPluginCodecManager(PPluginManager * pluginMgr = NULL);

    static PluginCodec_Definition * GetPluginCodec(const PString & key);
    static PluginCodec_Definition * GetPluginCodec(const H323Capability * capability, MCUCodec::Direction direction);

    static void PopulateMediaFormats();

  protected:
    static PluginCodecListType & GetPluginCodecList()
    {
      static PluginCodecListType pluginCodecList;
      return pluginCodecList;
    }

    static PMutex & GetPluginCodecListMutex()
    {
      static PMutex pluginCodecListMutex;
      return pluginCodecListMutex;
    }

    PDECLARE_NOTIFIER(PDynaLink, MCUPluginCodecManager, OnLoadPlugin);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_CAPS_H
