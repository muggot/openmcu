/*
 * pglobalstatic.cxx
 *
 * Various global statics that need to be instantiated upon startup
 *
 * Portable Windows Library
 *
 * Copyright (C) 2004 Post Increment
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
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: pglobalstatic.cxx,v $
 * Revision 1.11  2007/09/30 17:34:40  dsandras
 * Killed GCC 4.2 warnings.
 *
 * Revision 1.10  2007/08/17 08:46:01  csoutheren
 * Remove unnecessary inclusions of <iostream>
 *
 * Revision 1.9  2007/06/09 17:25:48  dsandras
 * Integrated DirectShow support from Luc Saillard <luc saillard org> with
 * the help of Matthias Schneider <ma30002000 yahoo de>.
 *
 * Revision 1.8  2007/05/13 10:05:18  dsandras
 * Fixed misplaced #if P_VIDFILE thanks to Matthias Schneider
 * <ma30002000 yahoo de>. Thanks a lot!
 *
 * Revision 1.7  2007/05/02 17:44:32  hfriederich
 * Fix linking if PVideoFile is disabled
 *
 * Revision 1.6  2007/04/13 07:19:24  rjongbloed
 * Removed separate Win32 solution for "plug in static loading" issue,
 *   and used the PLOAD_FACTORY() mechanism for everything.
 * Slight clean up of the PLOAD_FACTORY macro.
 *
 * Revision 1.5  2005/08/09 09:08:11  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.4.6.1  2005/07/17 09:27:08  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.4  2005/01/31 08:05:41  csoutheren
 * More patches for MacOSX, thanks to Hannes Friederich
 *
 * Revision 1.3  2005/01/11 06:57:15  csoutheren
 * Fixed namespace collisions with plugin starup factories
 *
 * Revision 1.2  2005/01/04 08:09:42  csoutheren
 * Fixed Linux configure problems
 *
 * Revision 1.1  2005/01/04 07:44:03  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 */

#ifndef _PGLOBALSTATIC_CXX
#define _PGLOBALSTATIC_CXX

#include <ptbuildopts.h>
#include <ptlib/plugin.h>
#include <iostream>


//
// Load static sound modules as required 
//
#if defined(P_AUDIO)

  #if defined(_WIN32) 
    PWLIB_STATIC_LOAD_PLUGIN(WindowsMultimedia, PSoundChannel);
  #elif defined(__BEOS__)
    PWLIB_STATIC_LOAD_PLUGIN(BeOS, PSoundChannel);
  #endif

  #if defined(P_WAVFILE)
    PWLIB_STATIC_LOAD_PLUGIN(WAVFile, PSoundChannel)
  #endif

#endif

//
// Load static video modules as required 
//
#if defined(P_VIDEO)

  #include <ptlib/videoio.h>

  #if defined(_WIN32) 
    PWLIB_STATIC_LOAD_PLUGIN(Window, PVideoOutputDevice);
    #if ! defined(NO_VIDEO_CAPTURE)
      #if defined(P_VFW_CAPTURE) 
        PWLIB_STATIC_LOAD_PLUGIN(VideoForWindows, PVideoInputDevice);
      #endif /*P_VFW_CAPTURE*/
      #if defined(P_DIRECTSHOW) 
        PWLIB_STATIC_LOAD_PLUGIN(DirectShow, PVideoInputDevice)
      #endif /*P_DIRECTSHOW*/
    #endif
  #endif

  PWLIB_STATIC_LOAD_PLUGIN(FakeVideo, PVideoInputDevice);
  PWLIB_STATIC_LOAD_PLUGIN(NULLOutput, PVideoOutputDevice);

  #if P_VIDFILE
  PWLIB_STATIC_LOAD_PLUGIN(YUVFile, PVideoInputDevice)
  PWLIB_STATIC_LOAD_PLUGIN(YUVFile, PVideoOutputDevice)
  PLOAD_FACTORY(PVideoFile, PDefaultPFactoryKey)
  #endif

#endif

//
// instantiate text to speech factory
//
#if defined(P_TTS)
  PLOAD_FACTORY(PTextToSpeech, PString)
#endif

//
// instantiate WAV file factory
//
#if defined(P_WAVFILE)
  PLOAD_FACTORY(PWAVFileConverter, unsigned)
  PLOAD_FACTORY(PWAVFileFormat,    unsigned)
#endif

//
// instantiate URL factory
//
#if defined(P_HTTP)
  PLOAD_FACTORY(PURLScheme, PString)
#endif


//
//  instantiate startup factory
//
#if defined(P_HAS_PLUGINS)
  PLOAD_FACTORY(PluginLoaderStartup, PString)
#endif


#endif // _PGLOBALSTATIC_CXX
