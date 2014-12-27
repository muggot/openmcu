
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
    static OpalFactoryCodec * CreateOpalFactoryCodec(const H323Capability * capability, MCUCodec::Direction direction);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_CAPS_H
