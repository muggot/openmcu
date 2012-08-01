/*
 * videoio1394avc.h
 *
 * This file is a based on videoio1394dc.h
 *
 * Portable Windows Library
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
 * Contributor(s): Georgi Georgiev <chutz@gg3.net>
 *
 * $Log: vidinput_avc.h,v $
 * Revision 1.8  2007/04/27 17:34:44  dsandras
 * Applied patch from Luc Saillard to fix things after the latest change
 * which broke all drivers. Thanks Luc <luc saillard org>.
 *
 * Revision 1.7  2007/04/14 07:08:55  rjongbloed
 * Major update of video subsystem:
 *   Abstracted video frame info (width, height etc) into separate class.
 *   Changed devices, converter and video file to use above.
 *   Enhanced video file hint detection for frame rate and more
 *     flexible formats.
 *   Fixed issue if need to convert both colour format and size, had to do
 *     colour format first or it didn't convert size.
 *   Win32 video output device can be selected by "MSWIN" alone.
 *
 * Revision 1.6  2005/08/09 09:08:09  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.5.8.2  2005/07/24 09:01:47  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.5.8.1  2005/07/17 09:27:04  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.5  2003/12/14 13:30:10  csoutheren
 * Added new function required for recent video changes
 *
 * Revision 1.4  2003/11/25 10:49:31  dsandras
 * Removed double declaration.
 *
 * Revision 1.3  2003/11/24 08:25:27  csoutheren
 * Patches from Snark to fix compile problems
 *
 * Revision 1.2  2003/11/23 22:11:04  dsandras
 * Added missing variable in the .h.
 *
 * Revision 1.1  2003/11/14 06:16:13  csoutheren
 * Initial version thanks to Damien and Snark
 *
 * Revision 1.1  2003/01/11 05:30:13  robertj
 * Added support for IEEE 1394 AV/C cameras, thanks Georgi Georgiev
 *
 */


#ifndef _PVIDEOIO1394AVC

#define _PVIDEOIO1394AVC

#ifdef __GNUC__
#pragma interface
#endif

#include <sys/utsname.h>
#include <libraw1394/raw1394.h>
#include <libavc1394/avc1394.h>
#include <libavc1394/rom1394.h>
#include <libraw1394/csr.h>
#include <libdv/dv.h>

#include <ptlib.h>
#include <ptlib/videoio.h>
#include <ptlib/vconvert.h>
#include <ptlib/file.h>
#if !P_USE_INLINES
#include <ptlib/contain.inl>
#endif
#include <ptclib/delaychan.h>


/** This class defines a video input device that
    generates fictitous image data.
*/

class PVideoInputDevice_1394AVC : public PVideoInputDevice
{
    PCLASSINFO(PVideoInputDevice_1394AVC, PVideoInputDevice);
 public:
  /** Create a new video input device.
   */
    PVideoInputDevice_1394AVC();

    /**Close the video input device on destruction.
      */
    ~PVideoInputDevice_1394AVC();

    /**Open the device given the device name.
      */
    BOOL Open(
      const PString & deviceName,   /// Device name to open
      BOOL startImmediate = TRUE    /// Immediately start device
    );

    /**Determine of the device is currently open.
      */
    BOOL IsOpen();

    /**Close the device.
      */
    BOOL Close();

    /**Start the video device I/O.
      */
    BOOL Start();

    /**Stop the video device I/O capture.
      */
    BOOL Stop();

    /**Determine if the video device I/O capture is in progress.
      */
    BOOL IsCapturing();

    /**Get a list of all of the drivers available.
      */
    static PStringList GetInputDeviceNames();

    PStringList GetDeviceNames() const
    { return GetInputDeviceNames(); }

    /**Get the maximum frame size in bytes.

       Note a particular device may be able to provide variable length
       frames (eg motion JPEG) so will be the maximum size of all frames.
      */
    PINDEX GetMaxFrameBytes();

    /**Grab a frame, after a delay as specified by the frame rate.
      */
    BOOL GetFrameData(
      BYTE * buffer,                 /// Buffer to receive frame
      PINDEX * bytesReturned = NULL  /// OPtional bytes returned.
    );

    /**Grab a frame. Do not delay according to the current frame rate parameter.
      */
    BOOL GetFrameDataNoDelay(
      BYTE * buffer,                 /// Buffer to receive frame
      PINDEX * bytesReturned = NULL  /// OPtional bytes returned.
    );


    /**Get the brightness of the image. 0xffff-Very bright.
     */
    int GetBrightness();

    /**Set brightness of the image. 0xffff-Very bright.
     */
    BOOL SetBrightness(unsigned newBrightness);


    /**Get the whiteness of the image. 0xffff-Very white.
     */
    int GetWhiteness();

    /**Set whiteness of the image. 0xffff-Very white.
     */
    BOOL SetWhiteness(unsigned newWhiteness);


    /**Get the colour of the image. 0xffff-lots of colour.
     */
    int GetColour();

    /**Set colour of the image. 0xffff-lots of colour.
     */
    BOOL SetColour(unsigned newColour);


    /**Get the contrast of the image. 0xffff-High contrast.
     */
    int GetContrast();

    /**Set contrast of the image. 0xffff-High contrast.
     */
    BOOL SetContrast(unsigned newContrast);


    /**Get the hue of the image. 0xffff-High hue.
     */
    int GetHue();

    /**Set hue of the image. 0xffff-High hue.
     */
    BOOL SetHue(unsigned newHue);
    
    
    /**Return whiteness, brightness, colour, contrast and hue in one call.
     */
    BOOL GetParameters (int *whiteness, int *brightness, 
				int *colour, int *contrast, int *hue);

    /**Get the minimum & maximum size of a frame on the device.
    */
    BOOL GetFrameSizeLimits(
      unsigned & minWidth,   /// Variable to receive minimum width
      unsigned & minHeight,  /// Variable to receive minimum height
      unsigned & maxWidth,   /// Variable to receive maximum width
      unsigned & maxHeight   /// Variable to receive maximum height
    ) ;

    void ClearMapping();

    int GetNumChannels();
    BOOL SetChannel(
         int channelNumber  /// New channel number for device.
    );
    BOOL SetFrameRate(
      unsigned rate  /// Frames  per second
    );
    BOOL SetVideoFormat(
      VideoFormat videoFormat   /// New video format
    );
    BOOL SetFrameSize(
      unsigned width,   /// New width of frame
      unsigned height   /// New height of frame
    );
    BOOL SetColourFormat(
      const PString & colourFormat   // New colour format for device.
    );


    /**Try all known video formats & see which ones are accepted by the video driver
     */
    BOOL TestAllFormats();


 protected:

    raw1394handle_t handle;
    BOOL is_capturing;
    BOOL UseDMA;
    dv_decoder_t * dv_decoder;
    PINDEX frameBytes;
    int port;
    PAdaptiveDelay m_pacing;

    BOOL SetupHandle();
};

int RawISOHandler (raw1394handle_t handle, int channel, size_t length, u_int32_t * data);

#endif


// End Of File ///////////////////////////////////////////////////////////////
