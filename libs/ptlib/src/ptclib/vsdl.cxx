/*
 * vsdl.cxx
 *
 * Classes to support video output via SDL
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2000 Equivalence Pty. Ltd.
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
 * $Log: vsdl.cxx,v $
 * Revision 1.21  2007/09/27 03:36:07  rjongbloed
 * Fixed Linux compatibility.
 *
 * Revision 1.20  2007/09/27 03:27:56  rjongbloed
 * Use posix version of function.
 *
 * Revision 1.19  2007/09/26 03:40:03  rjongbloed
 * Added ability to set position and title of SDL window.
 *
 * Revision 1.18  2007/04/04 01:51:38  rjongbloed
 * Reviewed and adjusted PTRACE log levels
 *   Now follows 1=error,2=warn,3=info,4+=debug
 *
 * Revision 1.17  2007/04/02 05:29:54  rjongbloed
 * Tidied some trace logs to assure all have a category (bit before a tab character) set.
 *
 * Revision 1.16  2006/06/21 04:20:07  csoutheren
 * Fixes for VS.net
 *
 * Revision 1.15  2006/06/09 04:43:04  dereksmithies
 * Add patch from Ben Lear to reduce the cpu consumption. Previously, the SDL
 * display thread was using 100% of the CPU time. Many thanks.
 *
 * Revision 1.14  2005/08/09 09:08:11  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.13.12.4  2005/08/04 08:21:58  dsandras
 * Added static plugin flag.
 *
 * Revision 1.13.12.3  2005/07/26 17:07:03  dsandras
 * Fix to make gcc happy.
 *
 * Revision 1.13.12.2  2005/07/17 12:58:15  rjongbloed
 * Sorted out the ordering or Red. Blue, Cr and Cb in RGB/BGR/YUV420 formats
 *
 * Revision 1.13.12.1  2005/07/17 09:27:07  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.13  2004/04/09 06:52:17  rjongbloed
 * Removed #pargma linker command for /delayload of DLL as documentations sais that
 *   you cannot do this.
 *
 * Revision 1.12  2004/02/23 23:52:20  csoutheren
 * Added pragmas to avoid every Windows application needing to include libs explicitly
 *
 * Revision 1.11  2003/12/12 05:11:56  rogerhardiman
 * Add SDL support on FreeBSD. Header files live in SDL11 directory
 *
 * Revision 1.10  2003/11/06 09:13:20  rjongbloed
 * Improved the Windows configure system to allow multiple defines based on file existence. Needed for SDL support of two different distros.
 *
 * Revision 1.9  2003/07/22 22:55:20  dereksmithies
 * Add memory allocation feature.
 *
 * Revision 1.8  2003/05/21 03:59:10  dereksmithies
 * Fix close down bug.
 *
 * Revision 1.7  2003/05/17 03:21:26  rjongbloed
 * Removed need to do strange things with main() function.
 *
 * Revision 1.6  2003/05/14 02:34:53  dereksmithies
 * Make SDL display work if only one of two display areas in use.
 *
 * Revision 1.5  2003/05/07 02:40:58  dereks
 * Fix to allow it to exit when the ::Terminate method called.
 *
 * Revision 1.4  2003/04/28 14:30:02  craigs
 * Started rearranging code
 *
 * Revision 1.3  2003/04/28 08:33:00  craigs
 * Linux SDL includes are in a SDL directory, but Windows is not
 *
 * Revision 1.2  2003/04/28 07:27:15  craigs
 * Added missed functions
 *
 * Revision 1.1  2003/04/28 07:03:55  craigs
 * Initial version from ohphone
 *
 */

#ifdef __GNUC__
#pragma implementation "vsdl.h"
#endif

#define P_FORCE_STATIC_PLUGIN 

#include <ptlib.h>
#include <ptlib/vconvert.h>
#include <ptlib/pluginmgr.h>
#include <ptclib/vsdl.h>

#define new PNEW

#if P_SDL

extern "C" {

#if defined(P_FREEBSD)
#include <SDL11/SDL.h>
#else
#include <SDL/SDL.h>
#endif

};

#ifdef _MSC_VER
#pragma comment(lib, P_SDL_LIBRARY)
#endif


class PVideoOutputDevice_SDL_PluginServiceDescriptor : public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject *   CreateInstance(int /*userData*/) const { return new PVideoOutputDevice_SDL; }
    virtual PStringList GetDeviceNames(int /*userData*/) const { return PStringList("SDL"); }
    virtual bool        ValidateDeviceName(const PString & deviceName, int /*userData*/) const { return deviceName.Find("SDL") == 0; }
} PVideoOutputDevice_SDL_descriptor;

PCREATE_PLUGIN(SDL, PVideoOutputDevice, &PVideoOutputDevice_SDL_descriptor);


///////////////////////////////////////////////////////////////////////

PVideoOutputDevice_SDL::PVideoOutputDevice_SDL()
{
  colourFormat = "YUV420P";

  sdlThread = NULL;
  updateOverlay = false;
  screen = NULL;
  overlay = NULL;
}


PVideoOutputDevice_SDL::~PVideoOutputDevice_SDL()
{ 
  Close();
}


PStringList PVideoOutputDevice_SDL::GetDeviceNames() const
{
  return PStringList("SDL");
}


BOOL PVideoOutputDevice_SDL::Open(const PString & name, BOOL /*startImmediate*/)
{
  Close();

  deviceName = name;

  sdlThread = PThread::Create(PCREATE_NOTIFIER(SDLThreadMain), 0,
                               PThread::NoAutoDeleteThread,
                               PThread::LowPriority,
                               "SDL:%x");

  sdlStarted.Wait();

  return screen != NULL;
}


BOOL PVideoOutputDevice_SDL::IsOpen()
{
  return screen != NULL && overlay != NULL;
}


BOOL PVideoOutputDevice_SDL::Close()
{
  if (IsOpen()) {
    sdlStop.Signal();
    sdlThread->WaitForTermination(1000);
    delete sdlThread;
  }

  return TRUE;
}


BOOL PVideoOutputDevice_SDL::SetColourFormat(const PString & colourFormat)
{
  if (colourFormat *= "YUV420P")
    return PVideoOutputDevice::SetColourFormat(colourFormat);

  return FALSE;
}


BOOL PVideoOutputDevice_SDL::SetFrameSize(unsigned width, unsigned height)
{
  {
    PWaitAndSignal m(mutex);

    if (width == frameWidth && height == frameHeight)
      return TRUE;

    if (!PVideoOutputDevice::SetFrameSize(width, height))
      return FALSE;
  }

  adjustSize.Signal();
  return IsOpen();
}


PINDEX PVideoOutputDevice_SDL::GetMaxFrameBytes()
{
  PWaitAndSignal m(mutex);
  return GetMaxFrameBytesConverted(CalculateFrameBytes(frameWidth, frameHeight, colourFormat));
}


BOOL PVideoOutputDevice_SDL::SetFrameData(unsigned x, unsigned y,
                                          unsigned width, unsigned height,
                                          const BYTE * data,
                                          BOOL endFrame) 
{
  PWaitAndSignal m(mutex);

  if (!IsOpen())
    return FALSE;

  if (x != 0 || y != 0 || width != frameWidth || height != frameHeight || !endFrame)
    return FALSE;

  ::SDL_LockYUVOverlay(overlay);

  PAssert(frameWidth == (unsigned)overlay->w && frameHeight == (unsigned)overlay->h, PLogicError);
  PINDEX pixelsFrame = frameWidth * frameHeight;
  PINDEX pixelsQuartFrame = pixelsFrame >> 2;

  const BYTE * dataPtr = data;

  PBYTEArray tempStore;
  if (converter != NULL) {
    converter->Convert(data, tempStore.GetPointer(pixelsFrame+2*pixelsQuartFrame));
    dataPtr = tempStore;
  }

  memcpy(overlay->pixels[0], dataPtr,                                  pixelsFrame);
  memcpy(overlay->pixels[1], dataPtr + pixelsFrame,                    pixelsQuartFrame);
  memcpy(overlay->pixels[2], dataPtr + pixelsFrame + pixelsQuartFrame, pixelsQuartFrame);

  ::SDL_UnlockYUVOverlay(overlay);

  updateOverlay = true;

  return TRUE;
}


bool PVideoOutputDevice_SDL::InitialiseSDL()
{
  // initialise the SDL library
  if (::SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE) < 0 ) {
    PTRACE(1, "VSDL\tCouldn't initialize SDL: " << ::SDL_GetError());
    return false;
  }

#ifdef _WIN32
  SDL_SetModuleHandle(GetModuleHandle(NULL));
#endif

  PString title = "Video Output";
  PINDEX pos = deviceName.Find("TITLE=\"");
  if (pos != P_MAX_INDEX) {
    pos += 6;
    PINDEX quote = deviceName.FindLast('"');
    PString quotedTitle = deviceName(pos, quote > pos ? quote : P_MAX_INDEX);
    title = PString(PString::Literal, quotedTitle);
  }
  ::SDL_WM_SetCaption(title, NULL);

  pos = deviceName.Find("X=");
  int x = pos != P_MAX_INDEX ? atoi(&deviceName[pos+2]) : 0;

  pos = deviceName.Find("Y=");
  int y = pos != P_MAX_INDEX ? atoi(&deviceName[pos+2]) : 0;

  PString winpos(PString::Printf, "SDL_VIDEO_WINDOW_POS=%i,%i", x, y);
  putenv(winpos.GetPointer());

  screen = ::SDL_SetVideoMode(frameWidth, frameHeight, 0, SDL_SWSURFACE /* | SDL_RESIZABLE */);
  if (screen == NULL) {
    PTRACE(1, "VSDL\tCouldn't create SDL screen: " << ::SDL_GetError());
    return false;
  }

  overlay = ::SDL_CreateYUVOverlay(frameWidth, frameHeight, SDL_IYUV_OVERLAY, screen);
  if (overlay == NULL) {
    PTRACE(1, "VSDL\tCouldn't create SDL overlay: " << ::SDL_GetError());
    return false;
  }

  return true;
}


bool PVideoOutputDevice_SDL::ProcessSDLEvents()
{
  if (screen == NULL || overlay == NULL) {
    PTRACE(6, "VSDL\t Screen and/or overlay not open, so dont process events");
    return false;
  }

  SDL_Event event;  
  while (::SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT : //User selected cross
        PTRACE(3, "VSDL\t user selected cross on window, close window");
        return false;

      case SDL_VIDEORESIZE :
        PTRACE(4, "VSDL\t Resize window to " << event.resize.w << " x " << event.resize.h);
    }
  }
  // Sleep for 25 milliseconds
  SDL_Delay(25);

  return true;
}


void PVideoOutputDevice_SDL::SDLThreadMain(PThread &, INT)
{
  InitialiseSDL();

  sdlStarted.Signal();

  PTRACE(4, "VSDL\tMain loop is underway, with SDL screen initialised");

  while (ProcessSDLEvents()) {
    if (sdlStop.Wait(0))
      break;

    PWaitAndSignal m(mutex);

    if (adjustSize.Wait(0)) {
      ::SDL_FreeYUVOverlay(overlay);
      overlay = NULL;

      screen = ::SDL_SetVideoMode(frameWidth, frameHeight, 0, SDL_SWSURFACE /* | SDL_RESIZABLE */);
      if (screen != NULL)
        overlay = ::SDL_CreateYUVOverlay(frameWidth, frameHeight, SDL_IYUV_OVERLAY, screen);

      adjustSize.Acknowledge();
    }

    if (updateOverlay) {
      SDL_Rect rect;
      rect.x = 0;
      rect.y = 0;
      rect.w = (Uint16)frameWidth;
      rect.h = (Uint16)frameHeight;
      ::SDL_DisplayYUVOverlay(overlay, &rect);
      updateOverlay = true;
    }
  }

  if (overlay != NULL) {
    ::SDL_FreeYUVOverlay(overlay);
    overlay = NULL;
  }

  if (screen != NULL) {
    ::SDL_FreeSurface(screen);
    screen = NULL;
  }

  ::SDL_Quit();

  sdlStop.Acknowledge();

  PTRACE(4, "VSDL\tEnd of sdl display loop");
}


#endif // P_SDL


// End of file ////////////////////////////////////////////////////////////////
