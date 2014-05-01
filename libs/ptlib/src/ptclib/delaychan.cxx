/*
 * delaychan.cxx
 *
 * Class for controlling the timing of data passing through it.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2001 Equivalence Pty. Ltd.
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
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: delaychan.cxx,v $
 * Revision 1.9  2007/04/02 05:29:54  rjongbloed
 * Tidied some trace logs to assure all have a category (bit before a tab character) set.
 *
 * Revision 1.8  2006/07/21 01:03:12  csoutheren
 * Fixed to PAdaptiveDelay
 * Thanks to Paolo Amadini
 *
 * Revision 1.7  2006/07/19 06:03:35  csoutheren
 * Add extension PAdaptiveDelay to set maximum and minimum delay times
 * Thanks to Paolo Amadini
 *
 * Revision 1.6  2006/06/20 12:44:02  csoutheren
 * Added new constructor for PDelayChannel
 * Thanks to Frederic Heem
 *
 * Revision 1.5  2003/02/20 08:43:44  rogerh
 * On Mac OS X, the thread sleep() (which uses select) is not as fine grained
 * as usleep. So use usleep(). Tested by Shawn.
 *
 * Revision 1.4  2002/02/26 00:42:13  robertj
 * Fixed MSVC warning.
 *
 * Revision 1.3  2002/02/25 11:05:02  rogerh
 * New Delay code which solves the accumulated error problem. Based on ideas
 * by Tomasz Motylewski <T.Motylewski@bfad.de>, Roger and Craig.
 *
 * Revision 1.2  2002/01/15 03:56:03  craigs
 * Added PAdaptiveDelay class
 *
 * Revision 1.1  2001/07/10 03:07:07  robertj
 * Added queue channel and delay channel classes to ptclib.
 *
 */

#ifdef __GNUC__
#pragma implementation "delaychan.h"
#endif

#include <ptlib.h>
#include <ptclib/delaychan.h>

/////////////////////////////////////////////////////////

PAdaptiveDelay::PAdaptiveDelay(unsigned _maximumSlip, unsigned _minimumDelay)
  : jitterLimit(_maximumSlip), minimumDelay(_minimumDelay)
{
  firstTime = TRUE;
}

void PAdaptiveDelay::Restart()
{
  firstTime = TRUE;
}

BOOL PAdaptiveDelay::Delay(int frameTime)
{
  if (firstTime) {
    firstTime = FALSE;
    targetTime = PTime();   // targetTime is the time we want to delay to
    return FALSE;
  }

  // Set the new target
  targetTime += frameTime;

  // Calculate the sleep time so we delay until the target time
  PTimeInterval delay = targetTime - PTime();
  int sleep_time = (int)delay.GetMilliSeconds();

  // Catch up if we are too late and the featue is enabled
  if (jitterLimit > 0 && sleep_time < -jitterLimit.GetMilliSeconds())
    targetTime = PTime();

  // Else sleep only if necessary
  if (sleep_time > minimumDelay.GetMilliSeconds())
#if defined(P_LINUX) || defined(P_MACOSX)
    usleep(sleep_time * 1000);
#else
    PThread::Current()->Sleep(sleep_time);
#endif

  return sleep_time <= -frameTime;
}

/////////////////////////////////////////////////////////

PDelayChannel::PDelayChannel(Mode m,
                             unsigned delay,
                             PINDEX size,
                             unsigned max,
                             unsigned min)
{
  mode = m;
  frameDelay = delay;
  frameSize = size;
  maximumSlip = -PTimeInterval(max);
  minimumDelay = min;
}

PDelayChannel::PDelayChannel(PChannel &channel,
                             Mode m,
                             unsigned delay,
                             PINDEX size,
                             unsigned max,
                             unsigned min) :
   mode(m), 
   frameDelay(delay),
   frameSize(size),
   minimumDelay(min)
{
  maximumSlip = -PTimeInterval(max);
  if(Open(channel) == FALSE){
    PTRACE(1,"Delay\tPDelayChannel cannot open channel");
  }
  PTRACE(5,"Delay\tdelay = " << frameDelay << ", size = " << frameSize);
}

BOOL PDelayChannel::Read(void * buf, PINDEX count)
{
  if (mode != DelayWritesOnly)
    Wait(count, nextReadTick);
  return PIndirectChannel::Read(buf, count);
}


BOOL PDelayChannel::Write(const void * buf, PINDEX count)
{
  if (mode != DelayReadsOnly)
    Wait(count, nextWriteTick);
  return PIndirectChannel::Write(buf, count);
}


void PDelayChannel::Wait(PINDEX count, PTimeInterval & nextTick)
{
  PTimeInterval thisTick = PTimer::Tick();

  if (nextTick == 0)
    nextTick = thisTick;

  PTimeInterval delay = nextTick - thisTick;
  if (delay > maximumSlip)
    PTRACE(6, "Delay\t" << delay);
  else {
    PTRACE(6, "Delay\t" << delay << " ignored, too large");
    nextTick = thisTick;
    delay = 0;
  }

  if (frameSize > 0)
    nextTick += count*frameDelay/frameSize;
  else
    nextTick += frameDelay;

  if (delay > minimumDelay)
    PThread::Current()->Sleep(delay);
}


// End of File ///////////////////////////////////////////////////////////////
