
#ifndef _MCU_CAPS_H
#define _MCU_CAPS_H

#include "config.h"

#include <ptlib.h>
#include <ptlib/pluginmgr.h>

#include <h323caps.h>
#include <codec/opalplugin.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUCapability : public H323Capability
{
  public:

    static MCUCapability * Create(const PString & name)
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
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_CAPS_H
