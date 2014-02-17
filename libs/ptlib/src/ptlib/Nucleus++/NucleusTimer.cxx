/*
 * NucleusTimer.cxx
 *
 * pwlib's PTimer as implemented for Nucleus++
 *
 * Copyright (c) 1999 ISDN Communications Ltd
 *
 * Author: Chris Wayman Purvis
 *
 */

#include <ptlib.h>
#define new PNEW

///////////////////////////////////////////////////////////////////////////////
//
// timer

PTimeInterval PTimer::Tick()
  {
  return PTimeInterval((unsigned)SystemClock::Retrieve()*Resolution());
  }

