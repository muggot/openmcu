/*
 * vconvert.h
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
 * Contributor(s): Derek Smithies (derek@indranet.co.nz)
 *                 Thorsten Westheider (thorsten.westheider@teleos-web.de)
 *                 Mark Cooke (mpc@star.sr.bham.ac.uk)
 *
 * $Log: vconvert.h,v $
 * Revision 1.25  2007/04/24 08:28:52  csoutheren
 * Add backwards compatible API
 *
 * Revision 1.24  2007/04/20 06:47:02  csoutheren
 * Really disable video code when video is turned off
 *
 * Revision 1.23  2007/04/20 06:11:22  csoutheren
 * Add backwards compatible API for PColourConverter
 *
 * Revision 1.22  2007/04/20 05:40:33  csoutheren
 * Add backwards compatible API for PColourConverter
 *
 * Revision 1.21  2007/04/13 07:13:13  rjongbloed
 * Major update of video subsystem:
 *   Abstracted video frame info (width, height etc) into separate class.
 *   Changed devices, converter and video file to use above.
 *   Enhanced video file hint detection for frame rate and more
 *     flexible formats.
 *   Fixed issue if need to convert both colour format and size, had to do
 *     colour format first or it didn't convert size.
 *   Win32 video output device can be selected by "MSWIN" alone.
 *
 * Revision 1.20  2006/03/12 11:09:48  dsandras
 * Applied patch from Luc Saillard to fix problems with MJPEG. Thanks!
 *
 * Revision 1.19  2006/02/22 11:17:53  csoutheren
 * Applied patch #1425825
 * MaxOSX compatibility
 *
 * Revision 1.18  2006/02/20 06:12:10  csoutheren
 * Added guard defines
 *
 * Revision 1.17  2006/01/29 22:46:41  csoutheren
 * Added support for cameras that return MJPEG streams
 * Thanks to Luc Saillard and Damien Sandras
 *
 * Revision 1.16  2005/11/30 12:47:38  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.15  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.14  2005/08/09 09:08:09  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.13.14.1  2005/07/17 09:27:04  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.13  2003/03/17 07:44:20  robertj
 * Removed redundant toggle function.
 *
 * Revision 1.12  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.11  2002/01/04 04:11:45  dereks
 * Add video flip code from Walter Whitlock, which flips code at the grabber.
 *
 * Revision 1.10  2001/11/28 04:41:28  robertj
 * Added synonym colour class for equivalent colour format strings.
 * Allowed for setting ancestor classes in PCOLOUR_CONVERTER() macro.
 *
 * Revision 1.9  2001/05/14 05:10:38  robertj
 * Fixed problems with video colour converters registration, could not rely
 *   on static PList being initialised before all registration instances.
 *
 * Revision 1.8  2001/03/20 02:21:57  robertj
 * More enhancements from Mark Cooke
 *
 * Revision 1.7  2001/03/08 23:36:02  robertj
 * Added backward compatibility SetFrameSize() function.
 * Added internal SimpleConvert() function for same type converters.
 * Fixed some documentation.
 *
 * Revision 1.6  2001/03/08 08:31:34  robertj
 * Numerous enhancements to the video grabbing code including resizing
 *   infrastructure to converters. Thanks a LOT, Mark Cooke.
 *
 * Revision 1.5  2001/03/07 01:42:59  dereks
 * miscellaneous video fixes. Works on linux now. Add debug statements
 * (at PTRACE level of 1)
 *
 * Revision 1.4  2001/03/03 23:25:07  robertj
 * Fixed use of video conversion function, returning bytes in destination frame.
 *
 * Revision 1.3  2001/03/03 05:06:31  robertj
 * Major upgrade of video conversion and grabbing classes.
 *
 * Revision 1.2  2000/12/19 23:58:14  robertj
 * Fixed MSVC compatibility issues.
 *
 * Revision 1.1  2000/12/19 22:20:26  dereks
 * Add video channel classes to connect to the PwLib PVideoInputDevice class.
 * Add PFakeVideoInput class to generate test images for video.
 *
 *
 */

#ifndef _PCONVERT
#define _PCONVERT

#ifdef P_USE_PRAGMA
#ifndef P_MACOSX
#pragma interface
#endif
#endif

#include <ptbuildopts.h>

#if P_VIDEO

#include <ptlib/videoio.h>

struct jdec_private;


/**This class registers a colour conversion class.
   There should be one and one only instance of this class for each pair of
   srcColurFormat and dstColourFormat strings. Use the
   PCOLOUR_CONVERTER_REGISTRATION macro to do this.
 */
class PColourConverterRegistration : public PCaselessString
{
    PCLASSINFO(PColourConverterRegistration, PCaselessString);
  public:
    PColourConverterRegistration(
      const PString & srcColourFormat,  ///< Name of source colour format
      const PString & destColourFormat  ///< Name of destination colour format
    );

  protected:
    virtual PColourConverter * Create(
      const PVideoFrameInfo & src, ///< Source frame info (colour formet, size etc)
      const PVideoFrameInfo & dst  ///< Destination frame info
    ) const = 0;

    PColourConverterRegistration * link;

  friend class PColourConverter;
};


/**This class defines a means to convert an image from one colour format to another.
   It is an ancestor class for the individual formatting functions.
 */
class PColourConverter : public PObject
{
    PCLASSINFO(PColourConverter, PObject);
  public:
    /**Create a new converter.
      */
    PColourConverter(
      const PString & srcColourFormat,  ///< Name of source colour format
      const PString & dstColourFormat,  ///< Name of destination colour format
      unsigned width,   ///< Width of frame
      unsigned height   ///< Height of frame
    );
    PColourConverter(
      const PVideoFrameInfo & src, ///< Source frame info (colour formet, size etc)
      const PVideoFrameInfo & dst  ///< Destination frame info
    );

    /**Get the video conversion vertical flip state
     */
    BOOL GetVFlipState() 
      { return verticalFlip; }
    
    /**Set the video conversion vertical flip state
     */
    void SetVFlipState(BOOL vFlipState) 
      { verticalFlip = vFlipState; }
    
    /**Set the frame size to be used.

       Default behaviour calls SetSrcFrameSize() and SetDstFrameSize().
    */
    virtual BOOL SetFrameSize(
      unsigned width,   ///< New width of frame
      unsigned height   ///< New height of frame
    );

    /**Set the source frame info to be used.

       Default behaviour sets the srcFrameWidth and srcFrameHeight variables and
       recalculates the frame buffer size in bytes then returns TRUE if the size
       was calculated correctly.

       Returns FALSE if the colour formats do not agree.
    */
    virtual BOOL SetSrcFrameInfo(
      const PVideoFrameInfo & info   ///< New info for frame
    );

    /**Set the destination frame info to be used.

       Default behaviour sets the dstFrameWidth and dstFrameHeight variables,
       and the scale / crop preference. It then recalculates the frame buffer
       size in bytes then returns TRUE if the size was calculated correctly.

       Returns FALSE if the colour formats do not agree.
    */
    virtual BOOL SetDstFrameInfo(
      const PVideoFrameInfo & info  ///< New info for frame
    );

    /**Get the source frame info to be used.
    */
    virtual void GetSrcFrameInfo(
      PVideoFrameInfo & info   ///< New info for frame
    );

    /**Get the destination frame info to be used.
    */
    virtual void GetDstFrameInfo(
      PVideoFrameInfo & info  ///< New info for frame
    );

    /**Set the source frame size to be used.

       Default behaviour sets the srcFrameWidth and srcFrameHeight variables and
       recalculates the frame buffer size in bytes then returns TRUE if the size
       was calculated correctly.
    */
    virtual BOOL SetSrcFrameSize(
      unsigned width,   ///< New width of frame
      unsigned height   ///< New height of frame
    );

    /**Set the destination frame size to be used.

       Default behaviour sets the dstFrameWidth and dstFrameHeight variables,
       and the scale / crop preference. It then recalculates the frame buffer
       size in bytes then returns TRUE if the size was calculated correctly.
    */
    virtual BOOL SetDstFrameSize(
      unsigned width,  ///< New width of target frame
      unsigned height ///< New height of target frame
    );
    virtual BOOL SetDstFrameSize(
      unsigned width,  ///< New width of target frame
      unsigned height, ///< New height of target frame
      BOOL bScale
    );

    /**Get the source colour format.
      */
    const PString & GetSrcColourFormat() { return srcColourFormat; }

    /**Get the destination colour format.
      */
    const PString & GetDstColourFormat() { return dstColourFormat; }

    /**Get the maximum frame size in bytes for source frames.

       Note a particular device may be able to provide variable length
       frames (eg motion JPEG) so will be the maximum size of all frames.
      */
    PINDEX GetMaxSrcFrameBytes() { return srcFrameBytes; }

    /**Get the maximum frame size in bytes for destination frames.

       Note a particular device may be able to provide variable length
       frames (eg motion JPEG) so will be the maximum size of all frames.
      */
    PINDEX GetMaxDstFrameBytes() { return dstFrameBytes; }


    /**Convert from one colour format to another.
       This version will copy the data from one frame buffer to another.
       An implementation of this function should allow for the case of
       where srcFrameBuffer and dstFrameBuffer are the same, if the conversion
       algorithm allows for that to occur without an intermediate frame store.

       The function should return FALSE if srcFrameBuffer and dstFrameBuffer
       are the same and that form pf conversion is not allowed
    */
    virtual BOOL Convert(
      const BYTE * srcFrameBuffer,  ///< Frame store for source pixels
      BYTE * dstFrameBuffer,        ///< Frame store for destination pixels
      PINDEX * bytesReturned = NULL ///< Bytes written to dstFrameBuffer
    ) = 0;

    virtual BOOL Convert(
      const BYTE * srcFrameBuffer,  ///< Frame store for source pixels
      BYTE * dstFrameBuffer,        ///< Frame store for destination pixels
      unsigned int srcFrameBytes,
      PINDEX * bytesReturned = NULL ///< Bytes written to dstFrameBuffer
    ) = 0;

    /**Convert from one colour format to another.
       This version will copy the data from one frame buffer to the same frame
       buffer. Not all conversions can do this so an intermediate store and
       copy may be required. If the noIntermediateFrame parameter is TRUE
       and the conversion cannot be done in place then the function returns
       FALSE. If the in place conversion can be done then that parameter is
       ignored.

       Note that the frame should be large enough to take the destination
       pixels.

       Default behaviour calls Convert() from the frameBuffer to itself, and
       if that returns FALSE then calls it again (provided noIntermediateFrame
       is FALSE) using an intermediate store, copying the intermediate store
       back to the original frame store.
    */
    virtual BOOL ConvertInPlace(
      BYTE * frameBuffer,               ///< Frame buffer to translate data
      PINDEX * bytesReturned = NULL,    ///< Bytes written to frameBuffer
      BOOL noIntermediateFrame = FALSE  ///< Flag to use intermediate store
    );


    /**Create an instance of a colour conversion function.
       Returns NULL if there is no registered colour converter between the two
       named formats.
      */
    static PColourConverter * Create(
      const PVideoFrameInfo & src, ///< Source frame info (colour formet, size etc)
      const PVideoFrameInfo & dst  ///< Destination frame info
    );
    static PColourConverter * Create(
      const PString & srcColourFormat,
      const PString & destColourFormat,
      unsigned width,
      unsigned height
    );

    /**Get the output frame size.
      */
    BOOL GetDstFrameSize(
      unsigned & width, ///< Width of destination frame
      unsigned & height ///< Height of destination frame
    ) const;

    /**Get the input frame size.
      */
    BOOL GetSrcFrameSize(
      unsigned & width, ///< Width of source frame
      unsigned & height ///< Height of source frame
    ) const;

    unsigned GetSrcFrameWidth()  const { return srcFrameWidth;  }
    unsigned GetSrcFrameHeight() const { return srcFrameHeight; }
    unsigned GetDstFrameWidth()  const { return dstFrameWidth;  }
    unsigned GetDstFrameHeight() const { return dstFrameHeight; }

    /**Set the resize mode to be used.
    */
    void SetResizeMode(
      PVideoFrameInfo::ResizeMode mode
    ) { if (mode < PVideoFrameInfo::eMaxResizeMode) resizeMode = mode; }

    /**Get the resize mode to be used.
    */
    PVideoFrameInfo::ResizeMode GetResizeMode() const { return resizeMode; }

  protected:
    PString  srcColourFormat;
    PString  dstColourFormat;
    unsigned srcFrameWidth;
    unsigned srcFrameHeight;
    unsigned srcFrameBytes;

    // Needed for resizing
    unsigned dstFrameWidth;
    unsigned dstFrameHeight;
    unsigned dstFrameBytes;

    PVideoFrameInfo::ResizeMode resizeMode;
     
    BOOL     verticalFlip;

    PBYTEArray intermediateFrameStore;

#ifndef P_MACOSX
      /* Use by the jpeg decompressor */
    struct jdec_private *jdec;
#endif

  friend class PColourConverterRegistration;
};


/**Declare a colour converter class with Convert() function.
   This should only be used once and at the global scope level for each
   converter. It declares everything needs so only the body of the Convert()
   function need be added.
  */
#define PCOLOUR_CONVERTER2(cls,ancestor,srcFmt,dstFmt) \
class cls : public ancestor { \
  public: \
  cls(const PVideoFrameInfo & src, const PVideoFrameInfo & dst) \
    : ancestor(src, dst) { } \
  virtual BOOL Convert(const BYTE *, BYTE *, PINDEX * = NULL); \
  virtual BOOL Convert(const BYTE *, BYTE *, unsigned int , PINDEX * = NULL); \
}; \
static class cls##_Registration : public PColourConverterRegistration { \
  public: cls##_Registration() \
    : PColourConverterRegistration(srcFmt,dstFmt) { } \
  protected: virtual PColourConverter * Create(const PVideoFrameInfo & src, const PVideoFrameInfo & dst) const; \
} p_##cls##_registration_instance; \
PColourConverter * cls##_Registration::Create(const PVideoFrameInfo & src, const PVideoFrameInfo & dst) const \
  { return new cls(src, dst); } \
BOOL cls::Convert(const BYTE *srcFrameBuffer, BYTE *dstFrameBuffer, unsigned int __srcFrameBytes, PINDEX * bytesReturned) \
  { srcFrameBytes = __srcFrameBytes;return Convert(srcFrameBuffer, dstFrameBuffer, bytesReturned); } \
BOOL cls::Convert(const BYTE *srcFrameBuffer, BYTE *dstFrameBuffer, PINDEX * bytesReturned)


/**Declare a colour converter class with Convert() function.
   This should only be used once and at the global scope level for each
   converter. It declares everything needs so only the body of the Convert()
   function need be added.
  */
#define PCOLOUR_CONVERTER(cls,src,dst) \
        PCOLOUR_CONVERTER2(cls,PColourConverter,src,dst)



/**Define synonym colour format converter.
   This is a class that defines for which no conversion is required between
   the specified colour format names.
  */
class PSynonymColour : public PColourConverter {
  public:
    PSynonymColour(
      const PVideoFrameInfo & src,
      const PVideoFrameInfo & dst
    ) : PColourConverter(src, dst) { }
    virtual BOOL Convert(const BYTE *, BYTE *, PINDEX * = NULL);
    virtual BOOL Convert(const BYTE *, BYTE *, unsigned int , PINDEX * = NULL);
};


/**Define synonym colour format registration.
   This is a class that defines for which no conversion is required between
   the specified colour format names.
  */
class PSynonymColourRegistration : public PColourConverterRegistration {
  public:
    PSynonymColourRegistration(
      const char * srcFmt,
      const char * dstFmt
    );

  protected:
    virtual PColourConverter * Create(const PVideoFrameInfo & src, const PVideoFrameInfo & dst) const;
};


/**Define synonym colour format.
   This is a class that defines for which no conversion is required between
   the specified colour format names.
  */
#define PSYNONYM_COLOUR_CONVERTER(from,to) \
  static PSynonymColourRegistration p_##from##_##to##_registration_instance(#from,#to)

#endif // P_VIDEO


#endif // _PCONVERT

// End of file ///////////////////////////////////////////////////////////////
