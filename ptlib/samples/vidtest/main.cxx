/*
 * main.cxx
 *
 * PWLib application source file for vidtest
 *
 * Main program entry point.
 *
 * Copyright (c) 2003 Equivalence Pty. Ltd.
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
 * $Log: main.cxx,v $
 * Revision 1.17  2007/09/30 18:26:11  dsandras
 * Reverted accidental commit. Sorry.
 *
 * Revision 1.16  2007/09/30 17:34:40  dsandras
 * Killed GCC 4.2 warnings.
 *
 * Revision 1.15  2007/04/16 01:59:48  rjongbloed
 * Added function to video info class to parse standard size strings
 *   to width/height, eg "CIF", "QCIF", "VGA" etc
 *
 * Revision 1.14  2007/04/13 07:14:05  rjongbloed
 * Added command line parameter to set video frame resizing mode.
 * Default size and frame rate to what grabbebr is using.
 *
 * Revision 1.13  2007/04/03 12:09:38  rjongbloed
 * Fixed various "file video device" issues:
 *   Remove filename from PVideoDevice::OpenArgs (use deviceName)
 *   Added driverName to PVideoDevice::OpenArgs (so can select YUVFile)
 *   Added new statics to create correct video input/output device object
 *     given a PVideoDevice::OpenArgs structure.
 *   Fixed begin able to write to YUVFile when YUV420P colour format
 *     is not actually selected.
 *   Fixed truncating output video file if overwriting.
 *
 * Revision 1.12  2006/06/06 23:00:13  dereksmithies
 * Fix erroneous help message.
 *
 * Revision 1.11  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.10  2005/08/09 09:08:11  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.9.4.1  2005/07/17 09:25:30  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.9  2005/03/10 08:10:57  dereksmithies
 * Use a more descriptive name for the manufacturer.
 *
 * Revision 1.8  2004/04/20 00:19:31  dereksmithies
 * Add code to generate compile time error if SDL is not installed.
 *
 * Revision 1.7  2004/01/18 14:20:26  dereksmithies
 * Opening of video devices for plugins works now.
 *
 * Revision 1.6  2003/12/14 10:01:25  rjongbloed
 * Resolved issue with name space conflict os static and virtual forms of GetDeviceNames() function.
 *
 * Revision 1.5  2003/12/08 01:28:52  dereksmithies
 * Compiles now with new video plugins.
 *
 * Revision 1.4  2003/11/04 03:21:26  dereksmithies
 * Fix compile on windows OS.
 *
 * Revision 1.3  2003/04/29 00:57:21  dereks
 * Add user interface, option setting for format/input/fake. Works on Linux.
 *
 * Revision 1.2  2003/04/28 14:30:21  craigs
 * Started rearranging code
 *
 * Revision 1.1  2003/04/28 08:18:42  craigs
 * Initial version
 *
 */

#include "precompile.h"
#include "main.h"
#include "version.h"


PCREATE_PROCESS(VidTest);

#include  <ptlib/video.h>
#include  <ptclib/vsdl.h>


VidTest::VidTest()
  : PProcess("PwLib Video Example", "vidtest", 1, 0, ReleaseCode, 0)
{
  grabber = NULL;
  display = NULL;
}


void VidTest::Main()
{
  PArgList & args = GetArguments();

  args.Parse("h-help."
             "-input-driver:"
             "I-input-device:"
             "-input-format:"
             "-input-channel:"
             "-output-driver:"
             "O-output-device:"
             "F-colour-format:"
             "S-frame-size:"
             "R-frame-rate:"
             "C-crop."
#if PTRACING
             "o-output:"             "-no-output."
             "t-trace."              "-no-trace."
#endif
       );

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
         PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif

  if (args.HasOption('h')) {
    PError << "Available options are: " << endl
           << endl
           <<    "--help                 : print this help message.\n"
           <<    "--input-driver  drv    : video grabber driver.\n"
           <<    "-I --input-device dev  : video grabber device.\n"
           <<    "--input-format  fmt    : video grabber format (\"pal\"/\"ntsc\")\n"
           <<    "--input-channel num    : video grabber channel.\n"
           <<    "--output-driver drv    : video display driver to use.\n"
           <<    "-O --output-device dev : video display device to use.\n"
           <<    "-F --colour-format fmt : video colour size (\"rgb24\", \"yuv420\", etc)\n"
           <<    "-S --frame-size size   : video frame size (\"qcif\", \"cif\", WxH)\n"
           <<    "-R --frame-rate size   : video frame rate (frames/second)\n"
           <<    "-C --crop              : crop rather than scale if resizing\n"
#if PTRACING
           <<    "-o or --output file   : file name for output of log messages\n"       
           <<    "-t or --trace         : degree of verbosity in error log (more times for more detail)\n"     
#endif
           << endl
           << " e.g. ./vidtest --input-device fake --input-channel 2" << endl << endl;
    return;
  }

  PINDEX i;

  PString inputDriverName = args.GetOptionString("input-driver");
  if (!inputDriverName.IsEmpty()) {
    grabber = PVideoInputDevice::CreateDevice(inputDriverName);
    if (grabber == NULL) {
      cerr << "Cannot use input driver name \"" << inputDriverName << "\", must be one of:\n";
      PStringList drivers = PVideoInputDevice::GetDriverNames();
      for (i = 0; i < drivers.GetSize(); i++)
        cerr << "  " << drivers[i] << '\n';
      cerr << endl;
      return;
    }
  }

  PStringList devices = PVideoInputDevice::GetDriversDeviceNames(inputDriverName);
  if (devices.IsEmpty()) {
    cerr << "No video input devices present";
    if (!inputDriverName.IsEmpty())
      cerr << " for driver \"" << inputDriverName << '"';
    cerr << endl;
    return;
  }

  PString inputDeviceName = args.GetOptionString("input-device");
  if (inputDeviceName.IsEmpty())
    inputDeviceName = devices[0];
    
  if (grabber == NULL)
    grabber = PVideoInputDevice::CreateDeviceByName(inputDeviceName);

  if (grabber == NULL || !grabber->Open(inputDeviceName, false)) {
    cerr << "Cannot use input device name \"" << inputDeviceName << "\", must be one of:\n";
    PStringList drivers = PVideoInputDevice::GetDriverNames();
    for (i = 0; i < drivers.GetSize(); i++) {
      devices = PVideoInputDevice::GetDriversDeviceNames(drivers[i]);
      for (PINDEX j = 0; j < devices.GetSize(); j++)
        cerr << "  " << setw(20) << setiosflags(ios::left) << drivers[i] << devices[j] << '\n';
    }
    cerr << endl;
    return;
  }

  cout << "Grabber ";
  if (!inputDriverName.IsEmpty())
    cout << "driver \"" << inputDriverName << "\" and ";
  cout << "device \"" << grabber->GetDeviceName() << "\" opened." << endl;

  if (args.HasOption("input-format")) {
    PVideoDevice::VideoFormat format;
    PCaselessString formatString = args.GetOptionString("input-format");
    if (formatString == "PAL")
      format = PVideoDevice::PAL;
    else if (formatString == "NTSC")
      format = PVideoDevice::NTSC;
    else if (formatString == "SECAM")
      format = PVideoDevice::SECAM;
    else if (formatString == "Auto")
      format = PVideoDevice::Auto;
    else {
      cerr << "Illegal video input format name \"" << formatString << '"' << endl;
    return;
  }
    if (!grabber->SetVideoFormat(format)) {
      cerr << "Video input device could not be set to format \"" << formatString << '"' << endl;
      return;
    }
  }
  cout << "Grabber input format set to " << grabber->GetVideoFormat() << endl;

  if (args.HasOption("input-channel")) {
    int videoInput = args.GetOptionString("input-channel").AsInteger();
    if (!grabber->SetChannel(videoInput)) {
      cerr << "Video input device could not be set to channel " << videoInput << endl;
    return;
  }
  }
  cout << "Grabber input channel set to " << grabber->GetChannel() << endl;

  
  int frameRate;
  if (args.HasOption("frame-rate"))
    frameRate = args.GetOptionString("frame-rate").AsInteger();
  else
    frameRate = grabber->GetFrameRate();

  if (!grabber->SetFrameRate(frameRate)) {
    cerr << "Video input device could not be set to frame rate " << frameRate << endl;
    return;
  }
  cout << "Grabber frame rate set to " << grabber->GetFrameRate() << endl;


  PString outputDriverName = args.GetOptionString("output-driver");
  if (!outputDriverName.IsEmpty()) {
    display = PVideoOutputDevice::CreateDevice(outputDriverName);
    if (display == NULL) {
      cerr << "Cannot use output driver name \"" << inputDriverName << "\", must be one of:\n";
      PStringList drivers = PVideoOutputDevice::GetDriverNames();
      for (i = 0; i < drivers.GetSize(); i++)
        cerr << "  " << drivers[i] << '\n';
      cerr << endl;
    return;
    }
  }

  devices = PVideoOutputDevice::GetDriversDeviceNames(outputDriverName);
  if (devices.IsEmpty()) {
    cerr << "No video output devices present";
    if (!outputDriverName.IsEmpty())
      cerr << " for driver \"" << outputDriverName << '"';
    cerr << endl;
    return;
  }

  PString outputDeviceName = args.GetOptionString("output-device");
  if (outputDeviceName.IsEmpty()) {
    outputDeviceName = devices[0];
    if (outputDeviceName == "NULL" && devices.GetSize() > 1)
      outputDeviceName = devices[1];
  }

  if (display == NULL)
    display = PVideoOutputDevice::CreateDeviceByName(outputDeviceName);

  if (display == NULL || !display->Open(outputDeviceName, false)) {
    cerr << "Cannot use output device name \"" << outputDeviceName << "\", must be one of:\n";
    PStringList drivers = PVideoOutputDevice::GetDriverNames();
    for (i = 0; i < drivers.GetSize(); i++) {
      devices = PVideoOutputDevice::GetDriversDeviceNames(drivers[i]);
      for (PINDEX j = 0; j < devices.GetSize(); j++)
        cerr << "  " << setw(20) << setiosflags(ios::left) << drivers[i] << devices[j] << '\n';
    }
    cerr << endl;
    return;
  }

  cout << "Display ";
  if (!outputDriverName.IsEmpty())
    cout << "driver \"" << outputDriverName << "\" and ";
  cout << "device \"" << display->GetDeviceName() << "\" opened." << endl;


  unsigned width, height;
  if (args.HasOption("frame-size")) {
    PString sizeString = args.GetOptionString("frame-size");
    if (!PVideoFrameInfo::ParseSize(sizeString, width, height)) {
      cerr << "Illegal video frame size \"" << sizeString << '"' << endl;
      return;
    }
  }
  else {
    grabber->GetFrameSize(width, height);
  }

  PVideoFrameInfo::ResizeMode resizeMode = args.HasOption("crop") ? PVideoFrameInfo::eCropCentre : PVideoFrameInfo::eScale;
  if (!grabber->SetFrameSizeConverter(width, height, resizeMode)) {
    cerr << "Video input device could not be set to size " << width << 'x' << height << endl;
    return;
  }
  cout << "Grabber frame size set to " << grabber->GetFrameWidth() << 'x' << grabber->GetFrameHeight() << endl;

  if  (!display->SetFrameSizeConverter(width, height, resizeMode)) {
    cerr << "Video output device could not be set to size " << width << 'x' << height << endl;
    return;
  }

  cout << "Display frame size set to " << display->GetFrameWidth() << 'x' << display->GetFrameHeight() << endl;


  PCaselessString colourFormat = args.GetOptionString("colour-format", "RGB24").ToUpper();
  if (!grabber->SetColourFormatConverter(colourFormat) ) {
    cerr << "Video input device could not be set to colour format \"" << colourFormat << '"' << endl;
    return;
  }

  cout << "Grabber colour format set to " << grabber->GetColourFormat() << " (";
  if (colourFormat == grabber->GetColourFormat())
    cout << "native";
  else
    cout << "converted to " << colourFormat;
  cout << ')' << endl;

  if (!display->SetColourFormatConverter(colourFormat)) {
    cerr << "Video output device could not be set to colour format \"" << colourFormat << '"' << endl;
    return;
 }

  cout << "Diaplay colour format set to " << display->GetColourFormat() << " (";
  if (colourFormat == display->GetColourFormat())
    cout << "native";
  else
    cout << "converted from " << colourFormat;
  cout << ')' << endl;


  PThread::Create(PCREATE_NOTIFIER(GrabAndDisplay), 0,
                  PThread::NoAutoDeleteThread, PThread::NormalPriority,
                  "GrabAndDisplay");

  // command line
  for (;;) {

    // display the prompt
    cout << "vidtest> " << flush;
    PCaselessString cmd;
    cin >> cmd;

    if (cmd == "q" || cmd == "x" || cmd == "quit" || cmd == "exit")
      break;

    if (cmd == "fg") {
      if (!grabber->SetVFlipState(!grabber->GetVFlipState()))
        cout << "\nCould not toggle Vflip state of video input device" << endl;
      continue;
    }

    if (cmd == "fd") {
      if (!display->SetVFlipState(!display->GetVFlipState()))
        cout << "\nCould not toggle Vflip state of video output device" << endl;
      continue;
    }

    unsigned width, height;
    if (PVideoFrameInfo::ParseSize(cmd, width, height)) {
      pauseGrabAndDisplay.Signal();
      if  (!grabber->SetFrameSizeConverter(width, height))
        cout << "Video input device could not be set to size " << width << 'x' << height << endl;
      if  (!display->SetFrameSizeConverter(width, height))
        cout << "Video output device could not be set to size " << width << 'x' << height << endl;
      resumeGrabAndDisplay.Signal();
      continue;
    }

    cout << "Select:\n"
            "  fg     : Flip video input top to bottom\n"
            "  fd     : Flip video output top to bottom\n"
            "  qcif   : Set size of grab & display to qcif\n"
            "  cif    : Set size of grab & display to cif\n"
            "  WxH    : Set size of grab & display W by H\n"
            "  Q or X : Exit program\n" << endl;
  } // end for

  cout << "Exiting." << endl;
  exitGrabAndDisplay.Signal();
}


void VidTest::GrabAndDisplay(PThread &, INT)
{
  PBYTEArray frame;
  unsigned frameCount = 0;
  bool oldGrabberState = true;
  bool oldDisplayState = true;

  grabber->Start();
  display->Start();

  PTimeInterval startTick = PTimer::Tick();
  while (!exitGrabAndDisplay.Wait(0)) {

    bool grabberState = grabber->GetFrame(frame);
    if (oldGrabberState != grabberState) {
      oldGrabberState = grabberState;
      cerr << "Frame grab " << (grabberState ? "restored." : "failed!") << endl;
    }

    bool displayState = display->SetFrameData(0, 0, grabber->GetFrameWidth(), grabber->GetFrameHeight(), frame);
    if (oldDisplayState != displayState)
    {
      oldDisplayState = displayState;
      cerr << "Frame display " << (displayState ? "restored." : "failed!") << endl;
    }

    if (pauseGrabAndDisplay.Wait(0)) {
      pauseGrabAndDisplay.Acknowledge();
      resumeGrabAndDisplay.Wait();
    }

    frameCount++;
  }

  PTimeInterval duration = PTimer::Tick() - startTick;
  cout << frameCount << " frames over " << duration << " seconds at " << (frameCount*1000.0/duration.GetMilliSeconds()) << " fps." << endl;
  exitGrabAndDisplay.Acknowledge();
}



// End of File ///////////////////////////////////////////////////////////////
