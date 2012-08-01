/*
 * main.cxx
 *
 * PWLib application source file for PluginTest
 *
 * Main program entry point.
 *
 * Copyright 2003 Equivalence
 *
 * $Log: main.cxx,v $
 * Revision 1.9  2006/06/23 04:39:58  dereksmithies
 * Fixes, to get it to compile following the recent include file upgrade.
 *
 * Revision 1.8  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.7  2005/04/20 06:42:22  csoutheren
 * Added -S option to check bug 1031626
 *
 * Revision 1.6  2004/11/08 04:10:36  csoutheren
 * Fixed handling of sound driver and device names
 *
 * Revision 1.5  2004/08/17 04:59:28  csoutheren
 * Cleaned up help message and added factory based routines
 *
 * Revision 1.4  2004/08/16 06:41:57  csoutheren
 * Added options to get access to devices via the abstract factory interface
 *
 * Revision 1.3  2003/11/12 07:00:09  csoutheren
 * Changed make compile under Windows
 *
 * Revision 1.2  2003/11/12 06:35:12  csoutheren
 * Initial main version
 *
 * Revision 1.1.2.6  2003/10/20 21:15:33  dereksmithies
 * Tidy up text output. Fix calls to Usage() function.
 *
 * Revision 1.1.2.5  2003/10/20 03:22:46  dereksmithies
 * Add checks on validity of function returned.
 *
 * Revision 1.1.2.4  2003/10/13 02:46:02  dereksmithies
 * Now generates sound through dynamically loaded OSS sound channel.
 *
 * Revision 1.1.2.3  2003/10/12 21:22:12  dereksmithies
 * Add ability to play sample sound out PSoundChannel - illustrating operation of plugins.
 *
 * Revision 1.1.2.2  2003/10/08 03:55:54  dereksmithies
 * Add lots of debug statements, fix option parsing, improve Usage() function.
 *
 * Revision 1.1.2.1  2003/10/07 01:52:39  csoutheren
 * Test program for plugins
 *
 * Revision 1.3  2003/04/22 23:25:13  craigs
 * Changed help message for SRV records
 *
 * Revision 1.2  2003/04/15 08:15:16  craigs
 * Added single string form of GetSRVRecords
 *
 * Revision 1.1  2003/04/15 04:12:38  craigs
 * Initial version
 *
 */

#include <ptlib.h>
#include <ptlib/pprocess.h>

#include <ptlib/pluginmgr.h>
#include <ptlib/sound.h>
#include <ptlib/video.h>
#include "main.h"

#include <math.h>

#ifndef M_PI
#define M_PI  3.1415926
#endif

PCREATE_PROCESS(PluginTest);

#define SAMPLES 64000  

PluginTest::PluginTest()
  : PProcess("Equivalence", "PluginTest", 1, 0, AlphaCode, 1)
{
}

void Usage()
{
  PError << "usage: plugintest [options]\n \n"
            "  -d dir      : Set the directory from which plugins are loaded\n"
            "  -s          : show the list of available PSoundChannel drivers\n"
            "  -l          : list all plugin drivers\n"
            "  -L          : list all plugin drivers using abstract factory interface\n"
            "  -a driver   : play test sound using specified driver and default device\n"
            "                Use \"default\" as driver to use default (first) driver\n"
            "                Can also specify device as first arg, or use \"list\" to list all devices\n"
            "  -A driver   : same as -a but uses abstract factory based routines\n"
            "  -t          : set trace level (can be set more than once)\n"
            "  -o fn       : write trace output to file\n"
            "  -h          : display this help message\n";
   return;
}

ostream & operator << (ostream & strm, const std::vector<PString> & vec)
{
  char separator = strm.fill();
  int width = strm.width();
  for (std::vector<PString>::const_iterator r = vec.begin(); r != vec.end(); ++r) {
    if (r != vec.begin() && separator != '\0')
      strm << separator;
    strm.width(width);
    strm << *r;
  }
  if (separator == '\n')
    strm << '\n';

  return strm;
}

template <class DeviceType>
void DisplayPluginTypes(const PString & type)
{
  cout << "   " << type << " : ";
  std::vector<PString> services = PFactory<DeviceType>::GetKeyList();
  if (services.size() == 0)
    cout << "None available" << endl;
  else
    cout << setfill(',') << services << setfill(' ') << endl;
}


void PluginTest::Main()
{
  PArgList & args = GetArguments();

  args.Parse(
       "t-trace."              
       "o-output:"             
       "h-help."               
       "l-list."               
       "L-List."               
       "s-service:"   
       "S-sounddefault:"
       "a-audio:"
       "A-Audio:"
       "d-directory:"          
       );

  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
         PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);

  if (args.HasOption('d')) {
    PPluginManager & pluginMgr = PPluginManager::GetPluginManager();
    pluginMgr.LoadPluginDirectory(args.GetOptionString('d'));
  }

  if (args.HasOption('h')) {
    Usage();
    return;
  }

  if (args.HasOption('l')) {
    cout << "List available plugin types" << endl;
    PPluginManager & pluginMgr = PPluginManager::GetPluginManager();
    PStringList plugins = pluginMgr.GetPluginTypes();
    if (plugins.GetSize() == 0)
      cout << "No plugins loaded" << endl;
    else {
      cout << plugins.GetSize() << " plugin types available:" << endl;
      for (int i = 0; i < plugins.GetSize(); i++) {
        cout << "   " << plugins[i] << " : ";
        PStringList services = pluginMgr.GetPluginsProviding(plugins[i]);
        if (services.GetSize() == 0)
          cout << "None available" << endl;
        else
          cout << setfill(',') << services << setfill(' ') << endl;
      }
    }
    return;
  }

  if (args.HasOption('L')) {
    DisplayPluginTypes<PSoundChannel>("PSoundChannel");
    DisplayPluginTypes<PVideoInputDevice>("PVideoInputDevice");
    DisplayPluginTypes<PVideoOutputDevice>("PVideoOutputDevice");
    return;
  }

  if (args.HasOption('s')) {
    cout << "Available " << args.GetOptionString('s') << " :" <<endl;
    cout << "Sound plugin names = " << setfill(',') << PSoundChannel::GetDriverNames() << setfill(' ') << endl;
    return;
  }

  if (args.HasOption('S')) {
    cout << "Default sound device is \"" << PSoundChannel::GetDefaultDevice(PSoundChannel::Player) << "\"" << endl;
    return;
  }

  if (args.HasOption('a') || args.HasOption('A')) {
    PString driver;
    BOOL useFactory = FALSE;
    if (args.HasOption('a'))
      driver = args.GetOptionString('a');
    else {
      driver = args.GetOptionString('A');
      useFactory = TRUE;
    }

    PStringList driverList;
    if (useFactory)
      driverList = PStringList::container<std::vector<PString> >(PFactory<PSoundChannel>::GetKeyList());
    else
      driverList = PSoundChannel::GetDriverNames();

    if (driver *= "default") {
      if (driverList.GetSize() == 0) {
        cout << "No sound device drivers available\n";
        return;
      }
      driver = driverList[0];
    }
    else if (driver *= "list") {
      cout << "Drivers: " << setfill('\n') << driverList << endl;
      return;
    }


    PStringList deviceList = PSoundChannel::GetDeviceNames(driver, PSoundChannel::Player);
    if (deviceList.GetSize() == 0) {
      cout << "No devices for sound driver " << driver << endl;
      return;
    }

    PString device;

    if (args.GetCount() > 0) {
      device = args[0];
      if (driver *= "list") {
        cout << "Devices = " << deviceList << endl;
        return;
      }
    }
    else {
      device = deviceList[0];
    }
    
    cout << "Using sound driver" << driver << " with device " << device << endl;

    PSoundChannel * snd = PSoundChannel::CreateChannel(driver);
    if (snd == NULL) {
      cout << "Failed to create sound channel with " << driver << endl;
      return;
    }

    cout << "Opening sound driver " << driver << " with device " << device << endl;

    if (!snd->Open(device, PSoundChannel::Player)) {
      cout << "Failed to open sound driver " << driver  << " with device " << device << endl;
      return;
    }

    if (!snd->IsOpen()) {
      cout << "Sound device " << device << " not open" << endl;
      return;
    }

    if (!snd->SetBuffers(SAMPLES, 2)) {
      cout << "Failed to set samples to " << SAMPLES << " and 2 buffers. End program now." << endl;
      return;
    }

    snd->SetVolume(100);

    PWORDArray audio(SAMPLES);
    int i, pointsPerCycle = 8;
    int volume = 80;
    double angle;

    for (i = 0; i < SAMPLES; i++) {
      angle = M_PI * 2 * (double)(i % pointsPerCycle)/pointsPerCycle;
      if ((i % 4000) < 3000)
        audio[i] = (unsigned short) ((16384 * cos(angle) * volume)/100);
      else
        audio[i] = 0;
    }

    if (!snd->Write((unsigned char *)audio.GetPointer(), SAMPLES * 2)) {
      cout << "Failed to write  " << SAMPLES/8000  << " seconds of beep beep. End program now." << endl;
      return;
    }

    snd->WaitForPlayCompletion();
  }
}

// End of File ///////////////////////////////////////////////////////////////
