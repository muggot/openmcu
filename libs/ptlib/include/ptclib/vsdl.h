/*
 * vsdl.h
 *
 * Classes to support video output via SDL
 *
 * Copyright (c) 1999-2000 Equivalence Pty. Ltd.
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): Derek J Smithies (derek@indranet.co.nz)
 *
 * $Log: vsdl.h,v $
 * Revision 1.11  2006/06/21 04:20:07  csoutheren
 * Fixes for VS.net
 *
 * Revision 1.10  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.9  2005/08/09 09:08:08  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.8.2.1  2005/07/17 09:26:46  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.8  2005/07/13 12:50:07  csoutheren
 * Backported changes from isvo branch
 *
 * Revision 1.7.8.1  2005/04/25 13:27:26  shorne
 * Added support for capture SDL output to redirect to existing surface
 *
 * Revision 1.7  2004/05/31 01:26:58  dereksmithies
 * Fix the "no newline at end of file" warning.
 *
 * Revision 1.6  2004/05/27 04:47:05  csoutheren
 * Added include guard to file
 *
 * Revision 1.5  2003/12/12 05:11:56  rogerhardiman
 * Add SDL support on FreeBSD. Header files live in SDL11 directory
 *
 * Revision 1.4  2003/05/17 03:20:48  rjongbloed
 * Removed need to do strange things with main() function.
 *
 * Revision 1.3  2003/04/28 14:29:45  craigs
 * Started rearranging code
 *
 * Revision 1.2  2003/04/28 08:44:42  craigs
 * Fixed problem with include on linux
 *
 * Revision 1.1  2003/04/28 07:04:20  craigs
 * Initial version from ohphone
 *
 * Revision 1.8  2003/03/21 00:47:47  dereks
 * Remove surplus PTRACE statements.
 *
 * Revision 1.7  2003/03/20 23:50:41  dereks
 * Fixups resulting from the new PVideoOutputDevice class code.
 *
 * Revision 1.6  2002/12/03 21:45:05  dereks
 * Fixes from Walter Whitlock to cure warnings about u_chars. Thanks!
 *
 * Revision 1.5  2002/06/27 02:17:40  robertj
 * Renamed video format 411 to the correct 420P, thanks Mark Cooke
 *
 * Revision 1.4  2002/04/29 03:51:55  dereks
 * Documentation tidy up. Thanks to Walter Whitlock.
 *
 * Revision 1.3  2002/04/26 03:33:32  dereks
 * Major upgrade. All calls to SDL library are now done by one thread.
 *
 * Revision 1.2  2001/05/25 01:14:44  dereks
 * Alter SetFrameSize & OpenWindo to use unsigned variables. Change type of
 * other variables to eliminate compiler warnings.
 *
 * Revision 1.1  2001/03/07 01:47:45  dereks
 * Initial release of SDL (Simple DirectMedia Layer, a cross-platform multimedia library),
 * a video library code.
 *
 *
 */

#ifndef _PVSDL
#define _PVSDL

#if P_SDL

#include <ptlib.h>
#if defined(P_FREEBSD)
#include <SDL11/SDL.h>
#else
#include <SDL/SDL.h>
#endif

#include <ptlib/videoio.h>

#undef main


/**Display data to the SDL screen.
  */
class PVideoOutputDevice_SDL : public PVideoOutputDevice
{
    PCLASSINFO(PVideoOutputDevice_SDL, PVideoOutputDevice);
  
  public:
    /**Constructor. Does not make a window.
      */
    PVideoOutputDevice_SDL();
  
      /**Destructor.  Closes window if necessary, (which initializes all variables)
    */
    ~PVideoOutputDevice_SDL();
  
    /**Get a list of all of the devices available.
    */
    virtual PStringList GetDeviceNames() const;
  
    /**Open the device given the device name.
    */
    virtual BOOL Open(
      const PString & /*deviceName*/,   ///< Device name to open
      BOOL /*startImmediate*/ = TRUE    ///< Immediately start device
    );
  
    /**Synonymous with the destructor.
    */
    virtual BOOL Close();
  
    /**Global test function to determine if this video rendering
    class is open.*/
    virtual BOOL IsOpen();
  
    /**Set the colour format to be used.
       Note that this function does not do any conversion. If it returns TRUE
       then the video device does the colour format in native mode.

       To utilise an internal converter use the SetColourFormatConverter()
       function.

       Default behaviour sets the value of the colourFormat variable and then
       returns TRUE.
    */
    virtual BOOL SetColourFormat(
      const PString & colourFormat ///< New colour format for device.
    );

    /**Set the frame size to be used.

       Note that devices may not be able to produce the requested size, and
       this function will fail.  See SetFrameSizeConverter().

       Default behaviour sets the frameWidth and frameHeight variables and
       returns TRUE.
    */
    virtual BOOL SetFrameSize(
      unsigned width,   ///< New width of frame
      unsigned height   ///< New height of frame
    );

    /**Get the maximum frame size in bytes.

       Note a particular device may be able to provide variable length
       frames (eg motion JPEG) so will be the maximum size of all frames.
      */
    virtual PINDEX GetMaxFrameBytes();

    /**Set a section of the output frame buffer.
      */
    virtual BOOL SetFrameData(
      unsigned x,
      unsigned y,
      unsigned width,
      unsigned height,
      const BYTE * data,
      BOOL endFrame = TRUE
    );

  protected:
    PDECLARE_NOTIFIER(PThread, PVideoOutputDevice_SDL, SDLThreadMain);
    bool InitialiseSDL();
    bool ProcessSDLEvents();

    PThread     * sdlThread;
    PSyncPoint    sdlStarted;
    PSyncPointAck sdlStop;
    PSyncPointAck adjustSize;
    bool          updateOverlay;
    PMutex        mutex;

    SDL_Surface * screen;
    SDL_Overlay * overlay;
  };


typedef PVideoOutputDevice_SDL PSDLVideoDevice; // Backward compatibility


PWLIB_STATIC_LOAD_PLUGIN(SDL, PVideoOutputDevice);

#endif    // P_SDL

#endif

