/*
 * NucleusThread.cxx
 *
 * pwlib's PProcess as implemented for Nucleus++
 *
 * Copyright (c) 1999 ISDN Communications Ltd
 *
 * Author: Chris Wayman Purvis
 *
 */

#ifdef __GNUC__
#pragma message ("implementations")
#pragma implementation "svcproc.h"
#pragma implementation "config.h"
#pragma implementation "args.h"
#pragma implementation "syncpoint.h"
#pragma implementation "semaphor.h"
#pragma implementation "mutex.h"
#pragma implementation "channel.h"
#pragma implementation "syncthrd.h"
#pragma implementation "indchan.h"
#pragma implementation "pprocess.h"
#pragma implementation "thread.h"
#pragma implementation "timer.h"
#pragma implementation "pdirect.h"
#pragma implementation "file.h"
#pragma implementation "textfile.h"
#pragma implementation "conchan.h"
#pragma implementation "ptime.h"
#pragma implementation "timeint.h"
#pragma implementation "filepath.h"
#pragma implementation "lists.h"
#pragma implementation "pstring.h"
#pragma implementation "dict.h"
#pragma implementation "array.h"
#pragma implementation "object.h"
#pragma implementation "contain.h"
#endif

#include <ptlib.h>
#include <ptlib/svcproc.h>

#define new PNEW

// Return the effective user name of the process, eg "root" etc.
// Not really applicable as we're in a single-user system.
PString PProcess::GetUserName() const
  {
  return PString("route4");
  }

void PProcess::PXShowSystemWarning(PINDEX num, const PString & str)
{
  PProcess::Current()._PXShowSystemWarning(num, str);
}

void PProcess::_PXShowSystemWarning(PINDEX code, const PString & str)
{
  PError << "PWLib/Unix error #"
         << code
         << "-"
         << str
         << endl;
}

PString PProcess::GetOSClass()
  {
  return PString("ATI");
  }

PString PProcess::GetOSName()
  {
  return PString("Nucleus++");
  }
  
PString PProcess::GetOSHardware()
  {
#ifdef __NUCLEUS_MNT__
  return PString("MNT virtual system");
#else
  return PString("PPC_EMBEDDED");
#endif
  }

// We may need to get this direct from Nucleus's RLD_Release_String
// But it isn't const, so we may be OK.
PString PProcess::GetOSVersion()
  {
  return DevelopmentService::ReleaseInformation();
  }

PDirectory GetOSConfigDir()
  {
// No directory structure (nothing at all!)
#pragma message("No directory structure")
  PAssertAlways("No directory structure");
  return "N/A";
  }

void PProcess::Construct()
  {
// I don't believe we can turn off the various signals.  I don't understand
// why we want to (it's done in tlibthrd.cxx in PProcess::Construct, along
// with some filing system stuff, and in tlib.cxx in
// PProcess::CommonConstruct!).

// Normally in CommonConstruct
  CreateConfigFilesDictionary();

// Apparently housekeepingThread is undefined.  Might need one, but I don't
// know what to do with it!
//  housekeepingThread = NULL;

#pragma message("PProcess::Construct() looks a little light relative to the Unix version!")
  }

void PProcess::SignalTimerChange()
{
#if __NUCLEUS_PLUS__
#pragma message ("Do we need to do anything in PProcess::SignalTimerChange?")
#else
  if (housekeepingThread == NULL)
    housekeepingThread = PNEW HouseKeepingThread;
  else
    timerChangeSemaphore.Signal();
#endif
}

// stolen from tlibthrd
void PProcess::PXCheckSignals()
{
  if (pxSignals == 0)
    return;

  for (int sig = 0; sig < 32; sig++) {
    int bit = 1 << sig;
    if ((pxSignals&bit) != 0) {
      pxSignals &= ~bit;
      PXOnSignal(sig);
    }
  }
}

void PProcess::PXOnSignal(int /*sig*/)
{
}

void PProcess::DoArgs(void)
{
	PArgList & args = GetArguments();
	args.SetArgs(p_argc, p_argv);
}
