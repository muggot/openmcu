#ifndef _ABSTRACT_H
#define _ABSTRACT_H

#include <ptlib.h>
#include <ptlib/pluginmgr.h>

class MyAbstractClass 
{
  public:
    virtual PString Function() = 0;
};

#endif

