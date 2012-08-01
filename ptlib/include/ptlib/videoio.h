/*
 * videoio.h
 *
 * Classes to support streaming video input (grabbing) and output.
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
 * Contributor(s): Mark Cooke (mpc@star.sr.bham.ac.uk)
 *
 * $Log: videoio.h,v $
 * Revision 1.59  2007/09/26 03:43:09  rjongbloed
 * Added ability to get last position of window video output device.
 *
 * Revision 1.58  2007/05/19 09:33:29  rjongbloed
 * Fixed compiler warnings.
 *
 * Revision 1.57  2007/05/16 07:54:21  csoutheren
 * Fix problems created by gcc 4.2.0
 *
 * Revision 1.56  2007/05/01 21:13:46  dsandras
 * Added extra variables to store the number of frames received.
 *
 * Revision 1.55  2007/04/20 06:53:06  csoutheren
 * Fix problem with precompiled headers
 *
 * Revision 1.54  2007/04/20 06:47:02  csoutheren
 * Really disable video code when video is turned off
 *
 * Revision 1.53  2007/04/16 01:59:48  rjongbloed
 * Added function to video info class to parse standard size strings
 *   to width/height, eg "CIF", "QCIF", "VGA" etc
 *
 * Revision 1.52  2007/04/13 23:15:19  shorne
 * added backward compatible SetFrameSizeConverter
 *
 * Revision 1.51  2007/04/13 07:13:13  rjongbloed
 * Major update of video subsystem:
 *   Abstracted video frame info (width, height etc) into separate class.
 *   Changed devices, converter and video file to use above.
 *   Enhanced video file hint detection for frame rate and more
 *     flexible formats.
 *   Fixed issue if need to convert both colour format and size, had to do
 *     colour format first or it didn't convert size.
 *   Win32 video output device can be selected by "MSWIN" alone.
 *
 * Revision 1.50  2007/04/05 01:53:00  rjongbloed
 * Changed PVideoOutputDevice::CreateDeviceByName() to include driverName parameter so symmetric with PVideoInputDevice.
 *
 * Revision 1.49  2007/04/03 12:09:38  rjongbloed
 * Fixed various "file video device" issues:
 *   Remove filename from PVideoDevice::OpenArgs (use deviceName)
 *   Added driverName to PVideoDevice::OpenArgs (so can select YUVFile)
 *   Added new statics to create correct video input/output device object
 *     given a PVideoDevice::OpenArgs structure.
 *   Fixed begin able to write to YUVFile when YUV420P colour format
 *     is not actually selected.
 *   Fixed truncating output video file if overwriting.
 *
 * Revision 1.48  2006/10/31 04:10:40  csoutheren
 * Make sure PVidFileDev class is loaded, and make it work with OPAL
 *
 * Revision 1.47  2006/10/25 11:04:38  shorne
 * fix for devices having same name for different drivers.
 *
 * Revision 1.46  2006/06/21 05:38:58  csoutheren
 * Fixed plugin compile for latest pwlib
 *
 * Revision 1.45  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.44  2006/03/17 06:56:22  csoutheren
 * Exposed video fonts to external access
 *
 * Revision 1.43  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.42  2005/08/09 09:08:09  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.41.4.1  2005/07/17 09:27:04  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.41  2005/01/04 07:44:03  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.40  2004/04/18 12:49:22  csoutheren
 * Patches to video code thanks to Guilhem Tardy (hope I get it right this time :)
 *
 * Revision 1.39  2004/01/18 14:23:30  dereksmithies
 * Add new function to make opening of video input devices easier.
 *
 * Revision 1.38  2004/01/02 23:30:18  rjongbloed
 * Removed extraneous static function for getting input device names that has been deprecated during the plug ins addition.
 *
 * Revision 1.37  2003/12/14 10:01:02  rjongbloed
 * Resolved issue with name space conflict os static and virtual forms of GetDeviceNames() function.
 *
 * Revision 1.36  2003/12/03 03:47:56  dereksmithies
 * Add fix so video output devices compile and run correctly.
 * Thanks to Craig Southeren.
 *
 * Revision 1.35  2003/11/19 04:29:02  csoutheren
 * Changed to support video output plugins
 *
 * Revision 1.34  2003/11/18 10:39:06  csoutheren
 * Fixed warnings regarding calling virtual Close in destructors
 *
 * Revision 1.33  2003/11/18 06:46:15  csoutheren
 * Changed to support video input plugins
 *
 * Revision 1.32  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.31  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.30  2003/03/17 08:10:00  robertj
 * Fixed GNU warning
 *
 * Revision 1.29  2003/03/17 07:51:07  robertj
 * Added OpenFull() function to open with all video parameters in one go.
 * Made sure vflip variable is set in converter even if converter has not
 *   been set yet, should not depend on the order of functions!
 * Removed canCaptureVideo variable as this is really a virtual function to
 *   distinguish PVideoOutputDevice from PVideoInputDevice, it is not dynamic.
 * Made significant enhancements to PVideoOutputDevice class.
 * Added PVideoOutputDevice descendants for NULL and PPM files.
 *
 * Revision 1.28  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.27  2002/04/12 08:25:12  robertj
 * Added text string output for tracing video format.
 *
 * Revision 1.26  2002/04/05 06:41:54  rogerh
 * Apply video changes from Damien Sandras <dsandras@seconix.com>.
 * The Video Channel and Format are no longer set in Open(). Instead
 * call the new SetVideoChannelFormat() method. This makes video capture
 * and GnomeMeeting more stable with certain Linux video capture devices.
 *
 * Revision 1.25  2002/02/20 02:37:26  dereks
 * Initial release of Firewire camera support for linux.
 * Many thanks to Ryutaroh Matsumoto <ryutaroh@rmatsumoto.org>.
 *
 * Revision 1.24  2002/01/16 07:51:06  robertj
 * MSVC compatibilty changes
 *
 * Revision 1.23  2002/01/16 03:51:20  dereks
 * Move flip methods in PVideoInputDevice  to  PVideoDevice
 *
 * Revision 1.22  2002/01/14 02:59:54  robertj
 * Added preferred colour format selection, thanks Walter Whitlock
 *
 * Revision 1.21  2002/01/04 04:11:45  dereks
 * Add video flip code from Walter Whitlock, which flips code at the grabber.
 *
 * Revision 1.20  2001/11/28 00:07:32  dereks
 * Locking added to PVideoChannel, allowing reader/writer to be changed mid call
 * Enabled adjustment of the video frame rate
 * New fictitous image, a blank grey area
 *
 * Revision 1.19  2001/08/06 06:12:45  rogerh
 * Fix comments
 *
 * Revision 1.18  2001/08/03 04:21:51  dereks
 * Add colour/size conversion for YUV422->YUV411P
 * Add Get/Set Brightness,Contrast,Hue,Colour for PVideoDevice,  and
 * Linux PVideoInputDevice.
 * Add lots of PTRACE statement for debugging colour conversion.
 * Add support for Sony Vaio laptop under linux. Requires 2.4.7 kernel.
 *
 * Revision 1.17  2001/05/22 23:38:45  robertj
 * Fixed bug in PVideoOutputDevice, removed redundent SetFrameSize.
 *
 * Revision 1.16  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.15  2001/03/20 02:21:57  robertj
 * More enhancements from Mark Cooke
 *
 * Revision 1.14  2001/03/08 23:04:19  robertj
 * Fixed up some documentation.
 *
 * Revision 1.13  2001/03/08 08:31:34  robertj
 * Numerous enhancements to the video grabbing code including resizing
 *   infrastructure to converters. Thanks a LOT, Mark Cooke.
 *
 * Revision 1.12  2001/03/07 01:42:59  dereks
 * miscellaneous video fixes. Works on linux now. Add debug statements
 * (at PTRACE level of 1)
 *
 * Revision 1.11  2001/03/06 23:34:20  robertj
 * Added static function to get input device names.
 * Moved some inline virtuals to non-inline.
 *
 * Revision 1.10  2001/03/03 05:06:31  robertj
 * Major upgrade of video conversion and grabbing classes.
 *
 * Revision 1.9  2001/02/28 01:47:14  robertj
 * Removed function from ancestor and is not very useful, thanks Thorsten Westheider.
 *
 * Revision 1.8  2000/12/19 22:20:26  dereks
 * Add video channel classes to connect to the PwLib PVideoInputDevice class.
 * Add PFakeVideoInput class to generate test images for video.
 *
 * Revision 1.7  2000/11/09 00:20:38  robertj
 * Added qcif size constants
 *
 * Revision 1.6  2000/07/30 03:41:31  robertj
 * Added more colour formats to video device enum.
 *
 * Revision 1.5  2000/07/26 03:50:49  robertj
 * Added last error variable to video device.
 *
 * Revision 1.4  2000/07/26 02:13:46  robertj
 * Added some more "common" bounds checking to video device.
 *
 * Revision 1.3  2000/07/25 13:38:25  robertj
 * Added frame rate parameter to video frame grabber.
 *
 * Revision 1.2  2000/07/25 13:14:05  robertj
 * Got the video capture stuff going!
 *
 * Revision 1.1  2000/07/15 09:47:34  robertj
 * Added video I/O device classes.
 *
 */


#ifndef _PVIDEOIO
#define _PVIDEOIO

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#if P_VIDEO

#include <ptlib/plugin.h>
#include <ptlib/pluginmgr.h>

class PColourConverter;


class PVideoFrameInfo : public PObject
{
  PCLASSINFO(PVideoFrameInfo, PObject);

  public:
      PVideoFrameInfo();

    enum ResizeMode
    {
        eScale,
        eCropCentre,
        eCropTopLeft,
        eMaxResizeMode
    };

    enum StandardSizes {
      CIF16Width = 1408, CIF16Height = 1152,
      CIF4Width  = 704,  CIF4Height  = 576,
      CIFWidth   = 352,  CIFHeight   = 288,
      QCIFWidth  = 176,  QCIFHeight  = 144,
      SQCIFWidth = 144,  SQCIFHeight = 96,
    };

    /**Set the frame size to be used.

       Default behaviour sets the frameWidth and frameHeight variables and
       returns TRUE.
    */
    virtual BOOL SetFrameSize(
      unsigned width,   ///< New width of frame
      unsigned height   ///< New height of frame
    );

    /**Get the frame size being used.

       Default behaviour returns the value of the frameWidth and frameHeight
       variable and returns TRUE.
    */
    virtual BOOL GetFrameSize(
      unsigned & width,
      unsigned & height
    ) const;

    /** Get the width of the frame being used.

        Default behaviour returns the value of the frameWidth variable
    */
    virtual unsigned GetFrameWidth() const;

    /** Get the height of the frame being used.

        Default behaviour returns the value of the frameHeight variable
    */
    virtual unsigned GetFrameHeight() const;

    /**Set the video frame rate to be used on the device.

       Default behaviour sets the value of the frameRate variable and then
       returns TRUE.
    */
    virtual BOOL SetFrameRate(
      unsigned rate  ///< Frames  per second
    );

    /**Get the video frame rate used on the device.

       Default behaviour returns the value of the frameRate variable.
    */
    virtual unsigned GetFrameRate() const;

    /**Set the colour format to be used.

       Default behaviour sets the value of the colourFormat variable and then
       returns TRUE if not an empty string.
    */
    virtual BOOL SetColourFormat(
      const PString & colourFormat // New colour format for device.
    );

    /**Get the colour format to be used.

       Default behaviour returns the value of the colourFormat variable.
    */
    virtual const PString & GetColourFormat() const;

    /**Set the resize mode to be used.
    */
    void SetResizeMode(
      ResizeMode mode
    ) { if (resizeMode < eMaxResizeMode) resizeMode = mode; }

    /**Get the resize mode to be used.
    */
    ResizeMode GetResizeMode() const { return resizeMode; }

    /** Get the number of bytes of an image, given a particular width, height and colour format.
      */
    PINDEX CalculateFrameBytes() const { return CalculateFrameBytes(frameWidth, frameHeight, colourFormat); }
    static PINDEX CalculateFrameBytes(
      unsigned width,               ///< WIdth of frame
      unsigned height,              ///< Height of frame
      const PString & colourFormat  ///< Colour format of frame
    );

    /** Parse the standard size string names ("qcif", "cif" etc)
      */
    static BOOL ParseSize(
      const PString & str,  ///< String to parse
      unsigned & width,     ///< Resultant width
      unsigned & height     ///< Resulatant height
    );

  protected:
    unsigned   frameWidth;
    unsigned   frameHeight;
    unsigned   frameRate;
    PString    colourFormat;
    ResizeMode resizeMode;
};


/**This class defines a video device.
   This class is used to abstract the few parameters that are common to both\
   input and output devices.

   Example colour formats are:

     "Grey"     Simple 8 bit linear grey scale
     "Gray"     Synonym for Grey
     "RGB32"    32 bit RGB
     "RGB24"    24 bit RGB
     "RGB565"   16 bit RGB (6 bit green)
     "RGB555"   15 bit RGB
     "YUV422"   YUV 4:2:2 packed
     "YUV422P"  YUV 4:2:2 planar
     "YUV411"   YUV 4:1:1 packed
     "YUV411P"  YUV 4:1:1 planar
     "YUV420"   YUV 4:2:0 packed
     "YUV420P"  YUV 4:2:0 planar
     "YUV410"   YUV 4:1:0 packed
     "YUV410P"  YUV 4:1:0 planar
     "MJPEG"    Motion JPEG
     "UYVY422"  YUV 4:2:2 packed as U Y V Y U Y V Y ...
     "UYV444    YUV 4:4:4 packed as U Y V   U Y V   ...
                They are used in IEEE 1394 digital cameras. The specification
                is found at
http://www.1394ta.org/Download/Technology/Specifications/2000/IIDC_Spec_v1_30.pdf

 */
class PVideoDevice : public PVideoFrameInfo
{
  PCLASSINFO(PVideoDevice, PVideoFrameInfo);

  protected:
    /** Create a new video device (input or output).
     */
    PVideoDevice();


  public:
    /** Delete structures created by PVideoDevice(); 
     */
    virtual ~PVideoDevice();

    enum VideoFormat {
      PAL,
      NTSC,
      SECAM,
      Auto,
      NumVideoFormats
    };

    /**Get the device name of the open device.
      */
    const PString & GetDeviceName() const
      { return deviceName; }

    /**Get a list of all of the drivers available.
      */
    virtual PStringList GetDeviceNames() const = 0;

    struct OpenArgs {
      OpenArgs();

      PPluginManager * pluginMgr;
      PString     driverName;
      PString     deviceName;
      VideoFormat videoFormat;
      int         channelNumber;
      PString     colourFormat;
      bool        convertFormat;
      unsigned    rate;
      unsigned    width;
      unsigned    height;
      bool        convertSize;
      ResizeMode  resizeMode;
      bool        flip;
      int         brightness;
      int         whiteness;
      int         contrast;
      int         colour;
      int         hue;
    };

    /**Open the device given the device name.
      */
    virtual BOOL OpenFull(
      const OpenArgs & args,      ///< Parameters to set on opened device
      BOOL startImmediate = TRUE  ///< Immediately start device
    );

    /**Open the device given the device name.
      */
    virtual BOOL Open(
      const PString & deviceName,   ///< Device name to open
      BOOL startImmediate = TRUE    ///< Immediately start device
    ) = 0;

    /**Determine if the device is currently open.
      */
    virtual BOOL IsOpen() = 0;

    /**Close the device.
      */
    virtual BOOL Close() = 0;

    /**Start the video device I/O capture.
      */
    virtual BOOL Start() = 0;

    /**Stop the video device I/O capture.
      */
    virtual BOOL Stop() = 0;


#if PTRACING
    friend ostream & operator<<(ostream &, VideoFormat);
#endif

    /**Set the video format to be used.

       Default behaviour sets the value of the videoFormat variable and then
       returns TRUE.
    */
    virtual BOOL SetVideoFormat(
      VideoFormat videoFormat   ///< New video format
    );

    /**Get the video format being used.

       Default behaviour returns the value of the videoFormat variable.
    */
    virtual VideoFormat GetVideoFormat() const;

    /**Get the number of video channels available on the device.

       Default behaviour returns 1.
    */
    virtual int GetNumChannels();

    /**Set the video channel to be used on the device.
       The channel number is an integer from 0 to GetNumChannels()-1. The
       special value of -1 will find the first working channel number.

       Default behaviour sets the value of the channelNumber variable and then
       returns TRUE.
    */
    virtual BOOL SetChannel(
      int channelNumber  ///< New channel number for device.
    );

    /**Get the video channel to be used on the device.

       Default behaviour returns the value of the channelNumber variable.
    */
    virtual int GetChannel() const;

    /**Set the colour format to be used, trying converters if available.

       This function will set the colour format on the device to one that
       is compatible with a registered converter, and install that converter
       so that the correct format is used.
    */
    virtual BOOL SetColourFormatConverter(
      const PString & colourFormat // New colour format for device.
    );

    /**Get the video conversion vertical flip state.
       Default action is to return FALSE.
     */
    virtual BOOL GetVFlipState();

    /**Set the video conversion vertical flip state.
       Default action is to return FALSE.
     */
    virtual BOOL SetVFlipState(
      BOOL newVFlipState    ///< New vertical flip state
    );

    /**Get the minimum & maximum size of a frame on the device.

       Default behaviour returns the value 1 to UINT_MAX for both and returns
       FALSE.
    */
    virtual BOOL GetFrameSizeLimits(
      unsigned & minWidth,   ///< Variable to receive minimum width
      unsigned & minHeight,  ///< Variable to receive minimum height
      unsigned & maxWidth,   ///< Variable to receive maximum width
      unsigned & maxHeight   ///< Variable to receive maximum height
    ) ;


    /**Set the frame size to be used, trying converters if available.

       If the device does not support the size, a set of alternate resolutions
       are attempted.  A converter is setup if possible.
    */
    virtual BOOL SetFrameSizeConverter(
      unsigned width,  ///< New width of frame
      unsigned height, ///< New height of frame
      ResizeMode resizeMode = eMaxResizeMode ///< Mode to use if resizing is required.
    );

    /**Set the frame size to be used, trying converters if available.
       Function used for Backward compatibility only.
       If the device does not support the size, a set of alternate resolutions
       are attempted.  A converter is setup if possible.
    */
    virtual BOOL SetFrameSizeConverter(
      unsigned width,                   ///< New width of frame
      unsigned height,                  ///< New height of frame
	  BOOL  /*bScaleNotCrop*/           ///< Not used.
	  )  { return SetFrameSizeConverter(width,height,eScale); }


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

    /**Get the frame size being used.

       Default behaviour returns the value of the frameWidth and frameHeight
       variable and returns TRUE.
    */
    virtual BOOL GetFrameSize(
      unsigned & width,
      unsigned & height
    ) const;

    /**Get the maximum frame size in bytes.

       Note a particular device may be able to provide variable length
       frames (eg motion JPEG) so will be the maximum size of all frames.
      */
    virtual PINDEX GetMaxFrameBytes() = 0;

    
    /**Get the last error code. This is a platform dependent number.
      */
    int GetLastError() const { return lastError; }


    /** Is the device a camera, and obtain video
     */
    virtual BOOL CanCaptureVideo() const = 0;

    /**Get the brightness of the image. 0xffff-Very bright. -1 is unknown.
     */
    virtual int GetBrightness();

    /**Set brightness of the image. 0xffff-Very bright.
     */
    virtual BOOL SetBrightness(unsigned newBrightness);


    /**Get the whiteness of the image. 0xffff-Very white. -1 is unknown.
     */
    virtual int GetWhiteness();

    /**Set whiteness of the image. 0xffff-Very white.
     */
    virtual BOOL SetWhiteness(unsigned newWhiteness);


    /**Get the colour of the image. 0xffff-lots of colour. -1 is unknown.
     */
    virtual int GetColour();

    /**Set colour of the image. 0xffff-lots of colour.
     */
    virtual BOOL SetColour(unsigned newColour);


    /**Get the contrast of the image. 0xffff-High contrast. -1 is unknown.
     */
    virtual int GetContrast();

    /**Set contrast of the image. 0xffff-High contrast.
     */
    virtual BOOL SetContrast(unsigned newContrast);


    /**Get the hue of the image. 0xffff-High hue. -1 is unknown.
     */
    virtual int GetHue();

    /**Set hue of the image. 0xffff-High hue.
     */
    virtual BOOL SetHue(unsigned newHue);
    
    
    /**Return whiteness, brightness, colour, contrast and hue in one call.
     */
    virtual BOOL GetParameters(
      int *whiteness,
      int *brightness,
      int *colour,
      int *contrast,
      int *hue
    );

    
    /** Set VideoFormat and VideoChannel in one ioctl
     */
    virtual BOOL SetVideoChannelFormat (
      int channelNumber, 
      VideoFormat videoFormat
    );


    /**Set preferred native colour format from video capture device.
       Note empty == no preference.
     */
    void SetPreferredColourFormat(const PString & colourFmt) { preferredColourFormat = colourFmt; }

    /**Get preferred native colour format from video capture device.
       Returns empty == no preference
     */
    const PString & GetPreferredColourFormat() { return preferredColourFormat; }
    
    int GetNumberOfFrames () { return numberOfFrames; }
    
  protected:
    PINDEX GetMaxFrameBytesConverted(PINDEX rawFrameBytes) const;

    PString      deviceName;
    int          lastError;
    VideoFormat  videoFormat;
    int          channelNumber;
    // Preferred native colour format from video input device, empty == no preference
    PString      preferredColourFormat;
    BOOL         nativeVerticalFlip;

    PColourConverter * converter;
    PBYTEArray         frameStore;

    int          frameBrightness; // 16 bit entity, -1 is no value
    int          frameWhiteness;
    int          frameContrast;
    int          frameColour;
    int          frameHue;
    int          numberOfFrames;
};


/**This class defines a video output device.- typically, a window.
 */
class PVideoOutputDevice : public PVideoDevice
{
  PCLASSINFO(PVideoOutputDevice, PVideoDevice);

  public:
    /** Create a new video output device.
     */
    PVideoOutputDevice();
    
    /**Close the video output device on destruction.
      */
    virtual ~PVideoOutputDevice() { Close(); };      

    /**Get the list of available video output drivers (plug-ins)
    */
    static PStringList GetDriverNames(
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Get video output devices that correspond to the specified driver name.
       If driverName is an empty string or the value "*" then this will return
       a list of unique device names across all of the available drivers. If
       two drivers have identical names for devices, then the string returned
       will be of the form driver+'\t'+device.
    */
    static PStringList GetDriversDeviceNames(
      const PString & driverName,         ///< Name of driver
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Create the video output device that corresponds to the specified driver name.
    */
    static PVideoOutputDevice * CreateDevice(
      const PString & driverName,         ///< Name of driver
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /* Create the matching video output device that corresponds to the device name.

       This is typically used with the return values from GetDriversDeviceNames().
     */
    static PVideoOutputDevice *CreateDeviceByName(
      const PString & deviceName,         ///< Name of device
      const PString & driverName = PString::Empty(),  ///< Name of driver (if any)
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Create an opened video output device that corresponds to the specified names.
       If the driverName parameter is an empty string or "*" then CreateDeviceByName
       is used with the deviceName parameter which is assumed to be a value returned
       from GetDriversDeviceNames().
    */
    static PVideoOutputDevice *CreateOpenedDevice(
      const PString & driverName,         ///< Name of driver
      const PString & deviceName,         ///< Name of device
      BOOL startImmediate = TRUE,         ///< Immediately start display
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Create an opened video output device that corresponds to the specified arguments.
    */
    static PVideoOutputDevice *CreateOpenedDevice(
      const OpenArgs & args,              ///< Parameters to set on opened device
      BOOL startImmediate = TRUE          ///< Immediately start display
    );

    /**Close the device.
      */
    virtual BOOL Close() { return TRUE; }

    /**Start the video device I/O display.
      */
    virtual BOOL Start() { return TRUE; }

    /**Stop the video device I/O display.
      */
    virtual BOOL Stop() { return TRUE; }

    /** Is the device a camera, and obtain video
     */
    virtual BOOL CanCaptureVideo() const;

    /**Set a section of the output frame buffer.
      */
    virtual BOOL SetFrameData(
      unsigned x,
      unsigned y,
      unsigned width,
      unsigned height,
      const BYTE * data,
      BOOL endFrame = TRUE
    ) = 0;

    /**Get the position of the output device, where relevant. For devices such as
       files, this always returns zeros. For devices such as Windows, this is the
       position of the window on the screen.
       
       Returns: TRUE if the position is available.
      */
    virtual BOOL GetPosition(
      int & x,  // X position of device surface
      int & y   // Y position of device surface
    ) const;
};


/**This class defines a video output device for RGB in a frame store.
 */
class PVideoOutputDeviceRGB : public PVideoOutputDevice
{
  PCLASSINFO(PVideoOutputDeviceRGB, PVideoOutputDevice);

  public:
    /** Create a new video output device.
     */
    PVideoOutputDeviceRGB();

    /**Set the colour format to be used.
       Note that this function does not do any conversion. If it returns TRUE
       then the video device does the colour format in native mode.

       To utilise an internal converter use the SetColourFormatConverter()
       function.

       Default behaviour sets the value of the colourFormat variable and then
       returns TRUE.
    */
    virtual BOOL SetColourFormat(
      const PString & colourFormat // New colour format for device.
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

    /**Indicate frame may be displayed.
      */
    virtual BOOL FrameComplete() = 0;

  protected:
    PMutex     mutex;
    PINDEX     bytesPerPixel;
    PINDEX     scanLineWidth;
    bool       swappedRedAndBlue;
};


#ifdef SHOULD_BE_MOVED_TO_PLUGIN

/**This class defines a video output device which outputs to a series of PPM files.
 */
class PVideoOutputDevicePPM : public PVideoOutputDeviceRGB
{
  PCLASSINFO(PVideoOutputDevicePPM, PVideoOutputDeviceRGB);

  public:
    /** Create a new video output device.
     */
    PVideoOutputDevicePPM();

    /**Open the device given the device name.
      */
    virtual BOOL Open(
      const PString & deviceName,   ///< Device name (filename base) to open
      BOOL startImmediate = TRUE    ///< Immediately start device
    );

    /**Determine if the device is currently open.
      */
    virtual BOOL IsOpen();

    /**Close the device.
      */
    virtual BOOL Close();

    /**Get a list of all of the drivers available.
      */
    virtual PStringList GetDeviceNames() const;

    /**Indicate frame may be displayed.
      */
    virtual BOOL EndFrame();

  protected:
    unsigned   frameNumber;
};

#endif // SHOULD_BE_MOVED_TO_PLUGIN


/**This class defines a video input device.
 */
class PVideoInputDevice : public PVideoDevice
{
  PCLASSINFO(PVideoInputDevice, PVideoDevice);

  public:
    /** Create a new video input device.
     */
    //PVideoInputDevice();

    /**Close the video input device on destruction.
      */
    ~PVideoInputDevice() { Close(); }

    /**Get the list of available video input drivers (plug-ins)
    */
    static PStringList GetDriverNames(
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Get video input devices that correspond to the specified driver name.
       If driverName is an empty string or the value "*" then this will return
       a list of unique device names across all of the available drivers. If
       two drivers have identical names for devices, then the string returned
       will be of the form driver+'\t'+device.
    */
    static PStringList GetDriversDeviceNames(
      const PString & driverName,         ///< Name of driver
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Create the video input device that corresponds to the specified driver name.
    */
    static PVideoInputDevice *CreateDevice(
      const PString & driverName,         ///< Name of driver
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /* Create the matching video input device that corresponds to the device name.
       So, for "fake" return a device that will generate fake video.
       For "Phillips 680 webcam" (eg) will return appropriate grabber.
       Note that Phillips will return the appropriate grabber also.

       This is typically used with the return values from GetDriversDeviceNames().
     */
    static PVideoInputDevice *CreateDeviceByName(
      const PString & deviceName,         ///< Name of device
      const PString & driverName = PString::Empty(),  ///< Name of driver (if any)
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Create an opened video input device that corresponds to the specified names.
       If the driverName parameter is an empty string or "*" then CreateDeviceByName
       is used with the deviceName parameter which is assumed to be a value returned
       from GetDriversDeviceNames().
    */
    static PVideoInputDevice *CreateOpenedDevice(
      const PString & driverName,         ///< Name of driver
      const PString & deviceName,         ///< Name of device
      BOOL startImmediate = TRUE,         ///< Immediately start grabbing
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Create an opened video output device that corresponds to the specified arguments.
    */
    static PVideoInputDevice *CreateOpenedDevice(
      const OpenArgs & args,              ///< Parameters to set on opened device
      BOOL startImmediate = TRUE          ///< Immediately start display
    );

    /**Open the device given the device name.
      */
    virtual BOOL Open(
      const PString & deviceName,   ///< Device name to open
      BOOL startImmediate = TRUE    ///< Immediately start device
    ) = 0;

    virtual BOOL Close(
    ) { return TRUE; }

    /** Is the device a camera, and obtain video
     */
    virtual BOOL CanCaptureVideo() const;
 
    /**Determine if the video device I/O capture is in progress.
      */
    virtual BOOL IsCapturing() = 0;

    /**Grab a frame.
      */
    virtual BOOL GetFrame(
      PBYTEArray & frame
    );

    /**Grab a frame, after a delay as specified by the frame rate.
      */
    virtual BOOL GetFrameData(
      BYTE * buffer,                 ///< Buffer to receive frame
      PINDEX * bytesReturned = NULL  ///< OPtional bytes returned.
    ) = 0;

    /**Grab a frame. Do not delay according to the current frame rate parameter.
      */
    virtual BOOL GetFrameDataNoDelay(
      BYTE * buffer,                 ///< Buffer to receive frame
      PINDEX * bytesReturned = NULL  ///< OPtional bytes returned.
    ) = 0;

    /**Try all known video formats & see which ones are accepted by the video driver
     */
    virtual BOOL TestAllFormats() = 0;
};


////////////////////////////////////////////////////////
//
// declare macros and structures needed for video input plugins
//

template <class className> class PVideoInputPluginServiceDescriptor : public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject *   CreateInstance(int /*userData*/) const { return new className; }
    virtual PStringList GetDeviceNames(int /*userData*/) const { return className::GetInputDeviceNames(); }
};

#define PCREATE_VIDINPUT_PLUGIN(name) \
  static PVideoInputPluginServiceDescriptor<PVideoInputDevice_##name> PVideoInputDevice_##name##_descriptor; \
  PCREATE_PLUGIN(name, PVideoInputDevice, &PVideoInputDevice_##name##_descriptor)

////////////////////////////////////////////////////////
//
// declare macros and structures needed for video output plugins
//

template <class className> class PVideoOutputPluginServiceDescriptor : public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject *   CreateInstance(int /*userData*/) const { return new className; }
    virtual PStringList GetDeviceNames(int /*userData*/) const { return className::GetOutputDeviceNames(); }
};

#define PCREATE_VIDOUTPUT_PLUGIN(name) \
  static PVideoOutputPluginServiceDescriptor<PVideoOutputDevice_##name> PVideoOutputDevice_##name##_descriptor; \
  PCREATE_PLUGIN(name, PVideoOutputDevice, &PVideoOutputDevice_##name##_descriptor)

////////////////////////////////////////////////////////
//
// declare classes needed for access to simple video font
//

class PVideoFont : public PObject
{
  PCLASSINFO(PVideoFont, PObject);
  public:
    enum {
      MAX_L_HEIGHT = 11
    };
    struct LetterData {
      char ascii;
      const char *line[MAX_L_HEIGHT];
    };

    static const LetterData * GetLetterData(char ascii);
};

#endif // P_VIDEO

#endif   // _PVIDEOIO

// End Of File ///////////////////////////////////////////////////////////////
