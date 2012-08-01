/*
 * vconvert.cxx
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
 *   Thorsten Westheider (thorsten.westheider@teleos-web.de)
 *   Mark Cooke (mpc@star.sr.bham.ac.uk)
 *
 * $Log: vconvert.cxx,v $
 * Revision 1.70  2007/09/08 11:34:29  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.69  2007/06/18 19:23:40  dsandras
 * Fixed previous commit.
 *
 * Revision 1.68  2007/06/18 18:23:01  dsandras
 * Added patch from Elaine Xiong <elaine xiong sun com> to optimize
 * color space conversions using MediaLib.
 *
 * Revision 1.67  2007/06/10 06:31:46  rjongbloed
 * Fixed compiler warnings
 *
 * Revision 1.66  2007/06/09 17:23:28  dsandras
 * Added UYVY422 resizing method thanks to Luc Saillard <luc saillard org>.
 * Thanks!
 *
 * Revision 1.65  2007/04/24 08:28:52  csoutheren
 * Add backwards compatible API
 *
 * Revision 1.64  2007/04/20 06:47:48  csoutheren
 * Really disable video code when video is turned off
 *
 * Revision 1.63  2007/04/20 06:11:37  csoutheren
 * Add backwards compatible API for PColourConverter
 *
 * Revision 1.62  2007/04/20 05:40:49  csoutheren
 * Add backwards compatible API for PColourConverter
 *
 * Revision 1.61  2007/04/16 01:37:13  rjongbloed
 * Added simple crop to YUV420P to RGB converter.
 *
 * Revision 1.60  2007/04/14 07:08:55  rjongbloed
 * Major update of video subsystem:
 *   Abstracted video frame info (width, height etc) into separate class.
 *   Changed devices, converter and video file to use above.
 *   Enhanced video file hint detection for frame rate and more
 *     flexible formats.
 *   Fixed issue if need to convert both colour format and size, had to do
 *     colour format first or it didn't convert size.
 *   Win32 video output device can be selected by "MSWIN" alone.
 *
 * Revision 1.59  2007/04/13 07:13:14  rjongbloed
 * Major update of video subsystem:
 *   Abstracted video frame info (width, height etc) into separate class.
 *   Changed devices, converter and video file to use above.
 *   Enhanced video file hint detection for frame rate and more
 *     flexible formats.
 *   Fixed issue if need to convert both colour format and size, had to do
 *     colour format first or it didn't convert size.
 *   Win32 video output device can be selected by "MSWIN" alone.
 *
 * Revision 1.58  2006/12/07 21:32:41  dominance
 * make sure can fit the jpeg image into the buffer for MJPEG and JPEG cams.
 * Thanks goes to Luc Saillard for this patch.
 *
 * Revision 1.57  2006/11/01 17:47:10  dsandras
 * Added patch from Brian Lu <brian lu sun com> to fix compilation on
 * OpenSolaris.
 *
 * Revision 1.56  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.55  2006/06/20 05:39:40  csoutheren
 * Applied patch 1501428
 * Only shortcut YUV420P inplace resizing if sizes are the same
 * Thanks to raaf123
 *
 * Revision 1.54  2006/05/16 11:10:36  shorne
 * Removed warning message MSVC 6
 *
 * Revision 1.53  2006/04/30 21:26:46  dsandras
 * Fixed green color problem when converting images in some of the color formats
 * used by the iSight thanks to Luc Saillard <luc saillard org>.
 *
 * Revision 1.52  2006/04/23 20:20:49  dsandras
 * Added resizing capability for VYUY YUYV formats thanks to Luc Saillard.
 * Many thanks!
 *
 * Revision 1.51  2006/04/19 04:09:37  csoutheren
 * Add special case conversion from QCIF to CIF
 *
 * Revision 1.50  2006/03/12 11:09:53  dsandras
 * Applied patch from Luc Saillard to fix problems with MJPEG. Thanks!
 *
 * Revision 1.49  2006/03/07 20:53:51  dsandras
 * Added support for JPEG based webcams, thanks to Luc Saillard <luc saillard org>.
 *
 * Revision 1.48  2006/02/22 11:17:53  csoutheren
 * Applied patch #1425825
 * MaxOSX compatibility
 *
 * Revision 1.47  2006/01/31 03:24:43  csoutheren
 * Removed MJPEG capabilities when compiling with Microsoft compilers
 *
 * Revision 1.46  2006/01/29 22:46:38  csoutheren
 * Added support for cameras that return MJPEG streams
 * Thanks to Luc Saillard and Damien Sandras
 *
 * Revision 1.45  2006/01/16 20:18:01  dsandras
 * Applied patch from Luc Saillard <luc saillard org> to improve conversion
 * routines. Thanks!!
 *
 * Revision 1.44  2006/01/09 18:19:12  dsandras
 * Add YUY2 (or YUV420) format. Resizing to YUV420P is done, but it's not very
 * efficient.
 * Fix the gray border when doing padding for YUV420P (change it to black).
 * Logitech webcam fusion export only big format in yuy2.
 * Patches provided by Luc Saillard <luc _AT____ saillard.org>. Many thanks!
 *
 * Revision 1.43  2006/01/07 13:33:02  dsandras
 * Added code allowing real resizing on YUV420P streams thanks to Luc Sailard <luc ___AT_-_ saillard.org>. Thank you very much for that nice patch Luc!
 *
 * Revision 1.42  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.41  2005/08/09 09:08:11  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.40.6.2  2005/07/17 12:58:15  rjongbloed
 * Sorted out the ordering or Red. Blue, Cr and Cb in RGB/BGR/YUV420 formats
 *
 * Revision 1.40.6.1  2005/07/17 09:27:08  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.40  2005/01/25 06:35:28  csoutheren
 * Removed warnings under MSVC
 *
 * Revision 1.39  2005/01/04 07:44:03  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.38  2004/11/07 06:28:00  rjongbloed
 * Removed warnings about unused parameters in SBGGR8 conversion functions. Left one
 *   in (flip vertical) as it should be implemented but wasn't.
 *
 * Revision 1.37  2004/10/27 09:24:18  dsandras
 * Added patch from Nicola Orru' to convert from SBGGR8 to YUV420P. Thanks!
 *
 * Revision 1.36  2004/09/21 13:01:08  dsandras
 * Added conversion from sbggr to rgb thanks to an anonymous patcher.
 *
 * Revision 1.35  2003/11/23 22:17:35  dsandras
 * Added YUV420P to BGR24 and BGR32 conversion.
 *
 * Revision 1.34  2003/06/14 02:57:36  rjongbloed
 * REmoved redundent parameter, grey scale does not have rgb increment!
 *
 * Revision 1.33  2003/06/09 22:37:24  dereksmithies
 * Fix from Clive Nicolson to make b/w colour conversions work (i.e. grey palette).
 * many thanks!
 *
 * Revision 1.32  2003/04/03 09:28:37  robertj
 * Added reversed RGB byte order versions (BGR24), thanks Damien Sandras
 *
 * Revision 1.31  2003/03/31 11:30:14  rogerh
 * make 'cb' and 'cr' contain the values that their name implies.
 *
 * Revision 1.30  2002/09/01 23:00:05  dereks
 * Fix noise in flipped RGB image. Thanks Alex Phtahov.
 *
 * Revision 1.29  2002/02/26 02:23:21  dereks
 * Reduced verbosity in PTRACE output for when video is enabled.
 *
 * Revision 1.28  2002/02/20 02:37:26  dereks
 * Initial release of Firewire camera support for linux.
 * Many thanks to Ryutaroh Matsumoto <ryutaroh@rmatsumoto.org>.
 *
 * Revision 1.27  2002/02/03 19:55:57  dereks
 * *** empty log message ***
 *
 * Revision 1.26  2002/01/08 01:32:50  robertj
 * Tidied up some PTRACE debug output.
 *
 * Revision 1.25  2002/01/04 04:11:45  dereks
 * Add video flip code from Walter Whitlock, which flips code at the grabber.
 *
 * Revision 1.24  2001/12/08 00:33:11  robertj
 * Changed some (unsigned int *) to (DWORD *) as the latter is assured to be a
 *   pointer to a 32 bit integer and the former is not.
 *
 * Revision 1.23  2001/12/06 22:14:45  dereks
 * Improve YUV 422 resize routine so it now subsamples as required.
 *
 * Revision 1.22  2001/12/03 02:21:50  dereks
 * Add YUV420P to RGB24F, RGB32F converters.
 *
 * Revision 1.21  2001/12/02 21:53:56  dereks
 * Additional debug information
 *
 * Revision 1.20  2001/11/28 04:43:10  robertj
 * Added synonym colour class for equivalent colour format strings.
 * Allowed for setting ancestor classes in PCOLOUR_CONVERTER() macro.
 * Moved static functions into internal class to avoid pasing lots of parameters.
 * Added conversions for flipped RGB colour formats.
 *
 * Revision 1.19  2001/09/06 02:06:36  robertj
 * Fixed bug in detecting size mismatch, thanks Vjacheslav Andrejev
 *
 * Revision 1.18  2001/08/22 02:14:08  robertj
 * Fixed MSVC compatibility.
 *
 * Revision 1.17  2001/08/22 02:06:17  robertj
 * Resolved confusion with YUV411P and YUV420P video formats, thanks Mark Cooke.
 *
 * Revision 1.16  2001/08/20 07:01:26  robertj
 * Fixed wierd problems with YUV411P and YUV420P formats, thanks Mark Cooke.
 *
 * Revision 1.15  2001/08/16 23:17:29  robertj
 * Added 420P to 411P converter, thanks Mark Cooke.
 *
 * Revision 1.14  2001/08/03 10:13:56  robertj
 * Changes to previous check in to support MSVC.
 *
 * Revision 1.13  2001/08/03 04:21:51  dereks
 * Add colour/size conversion for YUV422->YUV411P
 * Add Get/Set Brightness,Contrast,Hue,Colour for PVideoDevice,  and
 * Linux PVideoInputDevice.
 * Add lots of PTRACE statement for debugging colour conversion.
 * Add support for Sony Vaio laptop under linux. Requires 2.4.7 kernel.
 *
 * Revision 1.12  2001/07/20 05:23:51  robertj
 * Added YUV411P to RGB24 converter.
 *
 * Revision 1.11  2001/05/14 05:10:38  robertj
 * Fixed problems with video colour converters registration, could not rely
 *   on static PList being initialised before all registration instances.
 *
 * Revision 1.10  2001/03/20 02:21:57  robertj
 * More enhancements from Mark Cooke
 *
 * Revision 1.9  2001/03/08 23:36:03  robertj
 * Added backward compatibility SetFrameSize() function.
 * Added internal SimpleConvert() function for same type converters.
 * Fixed some documentation.
 *
 * Revision 1.8  2001/03/08 08:31:34  robertj
 * Numerous enhancements to the video grabbing code including resizing
 *   infrastructure to converters. Thanks a LOT, Mark Cooke.
 *
 * Revision 1.7  2001/03/07 01:39:56  dereks
 * Fix image flip (top to bottom) in YUV411P to RGB24 conversion
 *
 * Revision 1.6  2001/03/06 23:48:32  robertj
 * Fixed naming convention on video converter classes.
 *
 * Revision 1.5  2001/03/03 23:25:07  robertj
 * Fixed use of video conversion function, returning bytes in destination frame.
 *
 * Revision 1.4  2001/03/03 06:13:01  robertj
 * Major upgrade of video conversion and grabbing classes.
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
 */

#include <ptlib.h>

#if P_VIDEO

#include <ptlib/video.h>

#ifdef __GNUC__
#pragma implementation "vconvert.h"
#endif

#include <ptlib/vconvert.h>

#if  defined(__GNUC__) || defined(__sun) 
#include "tinyjpeg.h"
#endif


#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif


#ifdef P_MEDIALIB
#include <mlib.h>
#endif

static PColourConverterRegistration * RegisteredColourConvertersListHead = NULL;

PSYNONYM_COLOUR_CONVERTER(SBGGR8, SBGGR8);
PSYNONYM_COLOUR_CONVERTER(Grey,   Grey);
PSYNONYM_COLOUR_CONVERTER(RGB24,  RGB24);
PSYNONYM_COLOUR_CONVERTER(BGR24,  BGR24);
PSYNONYM_COLOUR_CONVERTER(RGB32,  RGB32);
PSYNONYM_COLOUR_CONVERTER(BGR32,  BGR32);
PSYNONYM_COLOUR_CONVERTER(UYVY422,UYVY422);
PSYNONYM_COLOUR_CONVERTER(YUV411P,YUV411P);
PSYNONYM_COLOUR_CONVERTER(YUV420P,IYUV);
PSYNONYM_COLOUR_CONVERTER(IYUV,   YUV420P);
PSYNONYM_COLOUR_CONVERTER(YUV420P,I420);
PSYNONYM_COLOUR_CONVERTER(I420,   YUV420P);


class PStandardColourConverter : public PColourConverter
{
  PCLASSINFO(PStandardColourConverter, PColourConverter);

  protected:
    PStandardColourConverter(
      const PVideoFrameInfo & src,
      const PVideoFrameInfo & dst
    ) : PColourConverter(src, dst) { }

    BOOL SBGGR8toYUV420P(
     const BYTE * srgb,
      BYTE * rgb,
      PINDEX * bytesReturned
    ) const;
    BOOL SBGGR8toRGB(
      const BYTE * srgb,
      BYTE * rgb,
      PINDEX * bytesReturned
    ) const;
    void GreytoYUV420PSameSize(
      const BYTE * rgb,
      BYTE * yuv
    ) const;
    void GreytoYUV420PWithResize(
      const BYTE * rgb,
      BYTE * yuv
    ) const;
    BOOL GreytoYUV420P(
      const BYTE * rgb,
      BYTE * yuv,
      PINDEX * bytesReturned
    ) const;
    void RGBtoYUV420PSameSize(
      const BYTE * rgb,
      BYTE * yuv,
      unsigned rgbIncrement,
      unsigned redOffset,
      unsigned blueOffset
    ) const;
    void RGBtoYUV420PWithResize(
      const BYTE * rgb,
      BYTE * yuv,
      unsigned rgbIncrement,
      unsigned redOffset,
      unsigned blueOffset
    ) const;
    BOOL RGBtoYUV420P(
      const BYTE * rgb,
      BYTE * yuv,
      PINDEX * bytesReturned,
      unsigned rgbIncrement,
      unsigned redOffset,
      unsigned blueOffset
    ) const;
    BOOL YUV420PtoRGB(
      const BYTE * yuv,
      BYTE * rgb,
      PINDEX * bytesReturned,
      unsigned rgbIncrement,
      unsigned redOffset,
      unsigned blueOffset
    ) const;
    BOOL SwapRedAndBlue(
      const BYTE * src,
      BYTE * dst,
      PINDEX * bytesReturned,
      unsigned srcIncrement,
      unsigned dstIncrement
    ) const;
    void ResizeUYVY422(
      const BYTE *src_uyvy,
      BYTE *dst_uyvy
    ) const;
    void ResizeYUV422(
      const BYTE * src,
      BYTE * dest
    ) const;
    void ResizeYUV420P(
      const BYTE * src,
      BYTE * dest
    ) const;
    void UYVY422toYUV420PSameSize(
      const BYTE *uyvy,
      BYTE *yuv420p
    ) const;
    void UYVY422toYUV420PWithResize(
      const BYTE *uyvy,
      BYTE *yuv420p
    ) const;
    void YUY2toYUV420PSameSize(
      const BYTE *yuy2,
      BYTE *yuv420p
    ) const;
    void YUY2toYUV420PWithResize(
      const BYTE *yuy2,
      BYTE *yuv420p
    ) const;
    bool MJPEGtoYUV420PSameSize(
      const BYTE *yuy2,
      BYTE *yuv420p
    );
#if defined (__GNUC__) || defined (__sun)
    bool MJPEGtoXXX(
      const BYTE *mjpeg,
	    BYTE *output_data,
	    PINDEX *bytesReturned,
	    int format
    );
    bool MJPEGtoYUV420P(
      const BYTE *mjpeg,
      BYTE *yuv420p,
      PINDEX *bytesReturned
    );
    bool MJPEGtoXXXSameSize(
      const BYTE *yuy2,
      BYTE *rgb,
      int format
    );
#endif
};


#define PSTANDARD_COLOUR_CONVERTER(from,to) \
  PCOLOUR_CONVERTER2(P_##from##_##to,PStandardColourConverter,#from,#to)


#define new PNEW


///////////////////////////////////////////////////////////////////////////////
// PColourConverter

PColourConverterRegistration::PColourConverterRegistration(const PString & srcColourFormat,
                                                           const PString & destColourFormat)
  : PCaselessString(srcColourFormat+'\t'+destColourFormat)
{
  PColourConverterRegistration * test = RegisteredColourConvertersListHead;
  while (test != NULL) {
    if (*test == *this)
      return;
    test = test->link;
  }

  link = RegisteredColourConvertersListHead;
  RegisteredColourConvertersListHead = this;
}


PColourConverter * PColourConverter::Create(const PVideoFrameInfo & src,
                                            const PVideoFrameInfo & dst)
{
  PString converterName = src.GetColourFormat() + '\t' + dst.GetColourFormat();

  PColourConverterRegistration * find = RegisteredColourConvertersListHead;
  while (find != NULL) {
    if (*find == converterName) {
      return find->Create(src, dst);
    }
    find = find->link;
  }

  PTRACE(2,"PColCnv\tCreate error. Did not find " << src.GetColourFormat() << "->" << dst.GetColourFormat());
  return NULL;
}

PColourConverter * PColourConverter::Create(const PString & srcColourFormat,
                                            const PString & destColourFormat,
                                            unsigned width,
                                            unsigned height)
{
  PVideoFrameInfo src;
  src.SetColourFormat(srcColourFormat);
  src.SetFrameSize(width, height);

  PVideoFrameInfo dst;
  dst.SetColourFormat(destColourFormat);

  return Create(src, dst);
}


PColourConverter::PColourConverter(
      const PString & _srcColourFormat,  ///< Name of source colour format
      const PString & _dstColourFormat,  ///< Name of destination colour format
      unsigned width,   ///< Width of frame
      unsigned height   ///< Height of frame
)
  : verticalFlip(FALSE)
#ifndef P_MACOSX
  , jdec(NULL)
#endif
{
  PVideoFrameInfo src;
  src.SetColourFormat(_srcColourFormat);
  src.SetFrameSize(width, height);

  PVideoFrameInfo dst;
  dst.SetColourFormat(_dstColourFormat);

  srcColourFormat = src.GetColourFormat();
  dstColourFormat = dst.GetColourFormat();
  resizeMode = dst.GetResizeMode();

  src.GetFrameSize(srcFrameWidth, srcFrameHeight);
  srcFrameBytes = src.CalculateFrameBytes();
  dst.GetFrameSize(dstFrameWidth, dstFrameHeight);
  dstFrameBytes = dst.CalculateFrameBytes();
  PTRACE(6,"PColCnv\tPColourConverter constructed: " << srcColourFormat << ' ' << srcFrameWidth << 'x'<< srcFrameHeight
                                           << " -> " << dstColourFormat << ' ' << dstFrameWidth << 'x'<< dstFrameHeight);
}

PColourConverter::PColourConverter(const PVideoFrameInfo & src,
                                   const PVideoFrameInfo & dst)
  : srcColourFormat(src.GetColourFormat())
  , dstColourFormat(dst.GetColourFormat())
  , resizeMode(dst.GetResizeMode())
  , verticalFlip(FALSE)
#ifndef P_MACOSX
  , jdec(NULL)
#endif
{
  src.GetFrameSize(srcFrameWidth, srcFrameHeight);
  srcFrameBytes = src.CalculateFrameBytes();
  dst.GetFrameSize(dstFrameWidth, dstFrameHeight);
  dstFrameBytes = dst.CalculateFrameBytes();
  PTRACE(6,"PColCnv\tPColourConverter constructed: " << srcColourFormat << ' ' << srcFrameWidth << 'x'<< srcFrameHeight
                                           << " -> " << dstColourFormat << ' ' << dstFrameWidth << 'x'<< dstFrameHeight);
}


BOOL PColourConverter::SetFrameSize(unsigned width, unsigned height)
{
  BOOL ok1 = SetSrcFrameSize(width, height);
  BOOL ok2 = SetDstFrameSize(width, height);
  PTRACE(2,"PColCnv\tSetFrameSize: " << width << 'x' << height
         << (ok1 && ok2 ? " OK" : " Failed"));
  return ok1 && ok2;
}


BOOL PColourConverter::SetSrcFrameInfo(const PVideoFrameInfo & info)
{
  if (info.GetColourFormat() != GetSrcColourFormat())
    return FALSE;

  unsigned w, h;
  return info.GetFrameSize(w, h) && SetSrcFrameSize(w, h);
}


BOOL PColourConverter::SetDstFrameInfo(const PVideoFrameInfo & info)
{
  if (info.GetColourFormat() != GetDstColourFormat())
    return FALSE;

  SetResizeMode(info.GetResizeMode());

  unsigned w, h;
  return info.GetFrameSize(w, h) && SetDstFrameSize(w, h);
}


void PColourConverter::GetSrcFrameInfo(PVideoFrameInfo & info)
{
  info.SetColourFormat(GetSrcColourFormat());
  info.SetFrameSize(srcFrameWidth, srcFrameHeight);
}


void PColourConverter::GetDstFrameInfo(PVideoFrameInfo & info)
{
  info.SetColourFormat(GetDstColourFormat());
  info.SetFrameSize(dstFrameWidth, dstFrameHeight);
}


BOOL PColourConverter::SetSrcFrameSize(unsigned width, unsigned height)
{
  if (srcFrameWidth == width && srcFrameHeight == height)
    return TRUE;

  srcFrameWidth = width;
  srcFrameHeight = height;
  srcFrameBytes = PVideoDevice::CalculateFrameBytes(srcFrameWidth, srcFrameHeight, srcColourFormat);
  PTRACE(srcFrameBytes != 0 ? 6 : 2, "PColCnv\tSetSrcFrameSize "
         << ((srcFrameBytes != 0) ? "Succeed": "Fail") << "ed, "
         << srcColourFormat << ' ' << srcFrameWidth << 'x' << srcFrameHeight
         << ", " << srcFrameBytes << " bytes.");

  return srcFrameBytes != 0;
}


BOOL PColourConverter::SetDstFrameSize(unsigned width, unsigned height)
{
  dstFrameWidth  = width;
  dstFrameHeight = height;

  dstFrameBytes = PVideoDevice::CalculateFrameBytes(dstFrameWidth, dstFrameHeight, dstColourFormat);

  PTRACE(dstFrameBytes != 0 ? 6 : 2, "PColCnv\tSetDstFrameSize "
         << ((dstFrameBytes != 0) ? "Succeed": "Fail") << "ed, "
         << dstColourFormat << ' ' << dstFrameWidth << 'x' << dstFrameHeight
         << ", " << dstFrameBytes << " bytes.");

  return dstFrameBytes != 0;
}

BOOL PColourConverter::SetDstFrameSize(
      unsigned width, 
      unsigned height,
      BOOL bScale
)
{
  if (!SetDstFrameSize(width, height))
    return FALSE;

  if (bScale)
    SetResizeMode(PVideoFrameInfo::eScale);
  else
    SetResizeMode(PVideoFrameInfo::eCropCentre);

  return TRUE;
}

BOOL PColourConverter::GetSrcFrameSize(unsigned &width, unsigned &height) const
{
  width = srcFrameWidth;
  height = srcFrameHeight;
  return TRUE;
}


BOOL PColourConverter::GetDstFrameSize(unsigned &width, unsigned &height) const
{
  width = dstFrameWidth;
  height = dstFrameHeight;
  return TRUE;
}


BOOL PColourConverter::ConvertInPlace(BYTE * frameBuffer,
                                      PINDEX * bytesReturned,
                                      BOOL noIntermediateFrame)
{
  if (Convert(frameBuffer, frameBuffer, bytesReturned))
    return TRUE;

  if (noIntermediateFrame) {
    PTRACE(2,"PColCnv\tError in ConvertInPlace, no intermediate frame available.");
    return FALSE;
  }

  BYTE * intermediate = intermediateFrameStore.GetPointer(dstFrameBytes);
  PINDEX bytes;
  if (!Convert(frameBuffer, intermediate, &bytes))
    return FALSE;

  memcpy(frameBuffer, intermediate, bytes);
  if (bytesReturned != NULL)
    *bytesReturned = bytes;
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////

PSynonymColourRegistration::PSynonymColourRegistration(const char * srcFmt,
                                                       const char * dstFmt)
  : PColourConverterRegistration(srcFmt,dstFmt)
{
}


PColourConverter * PSynonymColourRegistration::Create(const PVideoFrameInfo & src,
                                                      const PVideoFrameInfo & dst) const
{
  return new PSynonymColour(src, dst);
}

BOOL PSynonymColour::Convert(const BYTE *srcFrameBuffer,
                             BYTE *dstFrameBuffer,
			     unsigned int __srcFrameBytes,
                             PINDEX * bytesReturned)
{
  srcFrameBytes = __srcFrameBytes;
  return Convert(srcFrameBuffer, dstFrameBuffer, bytesReturned);
}

BOOL PSynonymColour::Convert(const BYTE *srcFrameBuffer,
                             BYTE *dstFrameBuffer,
                             PINDEX * bytesReturned)
{
  if ((srcFrameWidth != dstFrameWidth) || (srcFrameHeight != dstFrameHeight)) {
    PTRACE(2,"PColCnv\tCannot do synonym colour conversion, source and destination frame size not equal.");
    return FALSE;
  }

  if (verticalFlip) {
    PINDEX rowSize = dstFrameBytes/srcFrameHeight;
    if (rowSize*srcFrameHeight != dstFrameBytes) {
      PTRACE(2,"PColCnv\tCannot do synonym colour conversion, frame does not have equal sized scan lines.");
      return FALSE;
    }

    if (srcFrameBuffer != dstFrameBuffer) {
      const BYTE * srcRowPtr = srcFrameBuffer;
      BYTE * dstRowPtr = dstFrameBuffer + srcFrameHeight*rowSize;
      for (unsigned y = 0; y < srcFrameHeight; y++) {
        dstRowPtr -= rowSize;
        memcpy(dstRowPtr, srcRowPtr, rowSize);
        srcRowPtr += rowSize;
      }
    }
    else {
      BYTE * rowPtr1 = dstFrameBuffer;
      BYTE * rowPtr2 = dstFrameBuffer + srcFrameHeight*rowSize;
      PBYTEArray temp(rowSize);
      for (unsigned y = 0; y < srcFrameHeight; y += 2) {
        rowPtr2 -= rowSize;
        memcpy(temp.GetPointer(), rowPtr1, rowSize);
        memcpy(rowPtr1, rowPtr2, rowSize);
        memcpy(rowPtr2, temp.GetPointer(), rowSize);
        rowPtr1 += rowSize;
      }
    }
  }
  else {
    if (srcFrameBuffer != dstFrameBuffer)
      memcpy(dstFrameBuffer, srcFrameBuffer, dstFrameBytes);
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

#define BLACK_Y 0
#define BLACK_U 128
#define BLACK_V 128

#define greytoy(r, y) y=r
#define greytoyuv(r, y, u, v) greytoy(r,y); u=BLACK_U; v=BLACK_V

void PStandardColourConverter::GreytoYUV420PSameSize(const BYTE * grey, BYTE * yuv) const
{
  const unsigned planeSize = srcFrameWidth*srcFrameHeight;
  const unsigned halfWidth = srcFrameWidth >> 1;

  // get pointers to the data
  BYTE * yplane  = yuv;
  BYTE * uplane  = yuv + planeSize;
  BYTE * vplane  = yuv + planeSize + (planeSize >> 2);
  const BYTE * greyIndex = grey;

  for (unsigned y = 0; y < srcFrameHeight; y++) {
    BYTE * yline  = yplane + (y * srcFrameWidth);
    BYTE * uline  = uplane + ((y >> 1) * halfWidth);
    BYTE * vline  = vplane + ((y >> 1) * halfWidth);

    if (verticalFlip)
      greyIndex = grey + srcFrameWidth*(srcFrameHeight-1-y);

    for (unsigned x = 0; x < srcFrameWidth; x+=2) {
      greytoy(*greyIndex, *yline);
      greyIndex++;
      yline++;
      greytoyuv(*greyIndex, *yline, *uline, *vline);
      greyIndex++;
      yline++;
      uline++;
      vline++;
    }
  }
}


// Simple crop/pad version.  Image aligned to top-left
// and cropped / padded with black borders as required.
void PStandardColourConverter::GreytoYUV420PWithResize(const BYTE * grey, BYTE * yuv) const
{
  int planeSize = dstFrameWidth*dstFrameHeight;
  const int halfWidth = dstFrameWidth >> 1;
  unsigned min_width, min_height;

  min_width  = (dstFrameWidth  < srcFrameWidth)  ? dstFrameWidth  : srcFrameWidth;
  min_height = (dstFrameHeight < srcFrameHeight) ? dstFrameHeight : srcFrameHeight;

  // get pointers to the data
  BYTE * yplane  = yuv;
  BYTE * uplane  = yuv + planeSize;
  BYTE * vplane  = yuv + planeSize + (planeSize >> 2);
  const BYTE * greyIndex = grey;

  for (unsigned y = 0; y < min_height; y++) 
  {
    BYTE * yline  = yplane + (y * dstFrameWidth);
    BYTE * uline  = uplane + ((y >> 1) * halfWidth);
    BYTE * vline  = vplane + ((y >> 1) * halfWidth);

    if (verticalFlip)
      greyIndex = grey + srcFrameWidth*(min_height-1-y);

    for (unsigned x = 0; x < min_width; x+=2) 
    {
      greytoy(*greyIndex, *yline);
      greyIndex++;
      yline++;
      greytoyuv(*greyIndex, *yline, *uline, *vline);
      greyIndex++;
      yline++;
      uline++;
      vline++;
    }

    // Crop if source width > dest width
    if (srcFrameWidth > dstFrameWidth)
      greyIndex += srcFrameWidth - dstFrameWidth;

    // Pad if dest width < source width
    if (dstFrameWidth > srcFrameWidth) {
      memset(yline, BLACK_Y, dstFrameWidth - srcFrameWidth);
      memset(uline, BLACK_U, (dstFrameWidth - srcFrameWidth)>>1);
      memset(vline, BLACK_V, (dstFrameWidth - srcFrameWidth)>>1);
    }
  }

  // Pad if dest height > source height
  if (dstFrameHeight > srcFrameHeight) {
    BYTE * yline  = yplane + (srcFrameHeight * dstFrameWidth);
    BYTE * uline  = uplane + ((srcFrameHeight >> 1) * halfWidth);
    BYTE * vline  = vplane + ((srcFrameHeight >> 1) * halfWidth);
    unsigned fill = (dstFrameHeight - srcFrameHeight) * dstFrameWidth;

    memset(yline, BLACK_Y, fill);
    memset(uline, BLACK_U, fill >> 2);
    memset(vline, BLACK_V, fill >> 2);
  }
}


BOOL PStandardColourConverter::GreytoYUV420P(const BYTE * grey, BYTE * yuv, PINDEX * bytesReturned) const
{
  if (grey == yuv)
    return FALSE; // Cannot do in place conversion

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) 
    GreytoYUV420PSameSize(grey, yuv);
  else
    GreytoYUV420PWithResize(grey, yuv);

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return TRUE;
}

#define RGB2Y(r, g, b, y) \
  y=(BYTE)(((int)257*(r)  +(int)504*(g) +(int)98*(b))/1000)

#define RGB2YUV(r, g, b, y, cb, cr) \
  RGB2Y(r, g, b, y); \
  cb=(BYTE)((-148*(r)  -291*(g) +439*(b))/1000 + 128); \
  cr=(BYTE)(( 439*(r)  -368*(g) - 71*(b))/1000 + 128)

void PStandardColourConverter::RGBtoYUV420PSameSize(const BYTE * rgb,
                                                    BYTE * yuv,
                                                    unsigned rgbIncrement,
                                                    unsigned redOffset,
                                                    unsigned blueOffset) const
{
  const unsigned planeSize = srcFrameWidth*srcFrameHeight;
  const unsigned halfWidth = srcFrameWidth >> 1;

  // get pointers to the data
  BYTE * yplane  = yuv;
  BYTE * uplane  = yuv + planeSize;
  BYTE * vplane  = yuv + planeSize + (planeSize >> 2);
  const BYTE * rgbIndex = rgb;

  for (unsigned y = 0; y < srcFrameHeight; y++) {
    BYTE * yline  = yplane + (y * srcFrameWidth);
    BYTE * uline  = uplane + ((y >> 1) * halfWidth);
    BYTE * vline  = vplane + ((y >> 1) * halfWidth);

    if (verticalFlip)
      rgbIndex = rgb + (srcFrameWidth*(srcFrameHeight-1-y)*rgbIncrement);

    for (unsigned x = 0; x < srcFrameWidth; x+=2) {
      RGB2Y(rgbIndex[redOffset], rgbIndex[1], rgbIndex[blueOffset], *yline);
      rgbIndex += rgbIncrement;
      yline++;
      RGB2YUV(rgbIndex[redOffset], rgbIndex[1], rgbIndex[blueOffset], *yline, *uline, *vline);
      rgbIndex += rgbIncrement;
      yline++;
      uline++;
      vline++;
    }
  }
}


// Simple crop/pad version.  Image aligned to top-left
// and cropped / padded with black borders as required.
void PStandardColourConverter::RGBtoYUV420PWithResize(const BYTE * rgb,
                                                      BYTE * yuv,
                                                      unsigned rgbIncrement,
                                                      unsigned redOffset,
                                                      unsigned blueOffset) const
{
  int planeSize = dstFrameWidth*dstFrameHeight;
  const int halfWidth = dstFrameWidth >> 1;
  unsigned min_width, min_height;

  min_width  = (dstFrameWidth  < srcFrameWidth)  ? dstFrameWidth  : srcFrameWidth;
  min_height = (dstFrameHeight < srcFrameHeight) ? dstFrameHeight : srcFrameHeight;

  // get pointers to the data
  BYTE * yplane  = yuv;
  BYTE * uplane  = yuv + planeSize;
  BYTE * vplane  = yuv + planeSize + (planeSize >> 2);
  const BYTE * rgbIndex = rgb;

  for (unsigned y = 0; y < min_height; y++) 
  {
    BYTE * yline  = yplane + (y * dstFrameWidth);
    BYTE * uline  = uplane + ((y >> 1) * halfWidth);
    BYTE * vline  = vplane + ((y >> 1) * halfWidth);

    if (verticalFlip)
      rgbIndex = rgb + (srcFrameWidth*(min_height-1-y)*rgbIncrement); 

    for (unsigned x = 0; x < min_width; x+=2) {
      RGB2Y(rgbIndex[redOffset], rgbIndex[1], rgbIndex[blueOffset], *yline);
      rgbIndex += rgbIncrement;
      yline++;
      RGB2YUV(rgbIndex[redOffset], rgbIndex[1], rgbIndex[blueOffset], *yline, *uline, *vline);
      rgbIndex += rgbIncrement;
      yline++;
      uline++;
      vline++;
    }

    // Crop if source width > dest width
    if (srcFrameWidth > dstFrameWidth)
      rgbIndex += rgbIncrement * (srcFrameWidth - dstFrameWidth);

    // Pad if dest width < source width
    if (dstFrameWidth > srcFrameWidth) {
      memset(yline, BLACK_Y, dstFrameWidth - srcFrameWidth);
      memset(uline, BLACK_U, (dstFrameWidth - srcFrameWidth)>>1);
      memset(vline, BLACK_V, (dstFrameWidth - srcFrameWidth)>>1);
    }
  }

  // Pad if dest height > source height
  if (dstFrameHeight > srcFrameHeight) {
    BYTE * yline  = yplane + (srcFrameHeight * dstFrameWidth);
    BYTE * uline  = uplane + ((srcFrameHeight >> 1) * halfWidth);
    BYTE * vline  = vplane + ((srcFrameHeight >> 1) * halfWidth);
    unsigned fill = (dstFrameHeight - srcFrameHeight) * dstFrameWidth;

    memset(yline, BLACK_Y, fill);
    memset(uline, BLACK_U, fill >> 2);
    memset(vline, BLACK_V, fill >> 2);
  }
}


BOOL PStandardColourConverter::RGBtoYUV420P(const BYTE * rgb,
                                            BYTE * yuv,
                                            PINDEX * bytesReturned,
                                            unsigned rgbIncrement,
                                            unsigned redOffset,
                                            unsigned blueOffset) const
{
  if (rgb == yuv)
    return FALSE; // Cannot do in place conversion

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) 
    RGBtoYUV420PSameSize(rgb, yuv, rgbIncrement, redOffset, blueOffset);
  else
    RGBtoYUV420PWithResize(rgb, yuv, rgbIncrement, redOffset, blueOffset);

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return TRUE;
}


PSTANDARD_COLOUR_CONVERTER(Grey,YUV420P)
{
  return GreytoYUV420P(srcFrameBuffer, dstFrameBuffer, bytesReturned);
}


PSTANDARD_COLOUR_CONVERTER(RGB24,YUV420P)
{
  return RGBtoYUV420P(srcFrameBuffer, dstFrameBuffer, bytesReturned, 3,  0, 2);
}


PSTANDARD_COLOUR_CONVERTER(BGR24,YUV420P)
{
  return RGBtoYUV420P(srcFrameBuffer, dstFrameBuffer, bytesReturned, 3,  2, 0);
}


PSTANDARD_COLOUR_CONVERTER(RGB32,YUV420P)
{
  return RGBtoYUV420P(srcFrameBuffer, dstFrameBuffer, bytesReturned, 4, 0, 2);
}


PSTANDARD_COLOUR_CONVERTER(BGR32,YUV420P)
{
  return RGBtoYUV420P(srcFrameBuffer, dstFrameBuffer, bytesReturned, 4, 2, 0);
}

/*
 * Format YUY2 or YUV422(non planar):
 *
 * off: 0  Y00 U00 Y01 V00 Y02 U01 Y03 V01
 * off: 8  Y10 U10 Y11 V10 Y12 U11 Y13 V11
 * off:16  Y20 U20 Y21 V20 Y22 U21 Y23 V21
 * off:24  Y30 U30 Y31 V30 Y32 U31 Y33 V31
 * length:32 bytes
 *
 * Format YUV420P:
 * off: 00  Y00 Y01 Y02 Y03
 * off: 04  Y10 Y11 Y12 Y13
 * off: 08  Y20 Y21 Y22 Y23
 * off: 12  Y30 Y31 Y32 Y33
 * off: 16  U00 U02 U20 U22
 * off: 20  V00 V02 V20 V22
 * 
 * So, we loose some bit of information when converting YUY2 to YUV420 
 *
 * NOTE: This algorithm works only if the width and the height is pair.
 */
void  PStandardColourConverter::YUY2toYUV420PSameSize(const BYTE *yuy2, BYTE *yuv420p) const
{
  const BYTE *s;
  BYTE *y, *u, *v;
  unsigned int x, h;  
  int npixels = srcFrameWidth * srcFrameHeight;

  s = yuy2;
  y = yuv420p;
  u = yuv420p + npixels;
  v = u + npixels/4;

  for (h=0; h<srcFrameHeight; h+=2) {

     /* Copy the first line keeping all information */
     for (x=0; x<srcFrameWidth; x+=2) {
	*y++ = *s++;
	*u++ = *s++;
	*y++ = *s++;
	*v++ = *s++;
     }
     /* Copy the second line discarding u and v information */
     for (x=0; x<srcFrameWidth; x+=2) {
	*y++ = *s++;
	s++;
	*y++ = *s++;
	s++;
     }
  }
}

/*
 * Format YUY2 or YUV422(non planar):
 *
 * off: 0  Y00 U00 Y01 V00 Y02 U01 Y03 V01
 * off: 8  Y10 U10 Y11 V10 Y12 U11 Y13 V11
 * off:16  Y20 U20 Y21 V20 Y22 U21 Y23 V21
 * off:24  Y30 U30 Y31 V30 Y32 U31 Y33 V31
 * length:32 bytes
 *
 * Format YUV420P:
 * off: 00  Y00 Y01 Y02 Y03
 * off: 04  Y10 Y11 Y12 Y13
 * off: 08  Y20 Y21 Y22 Y23
 * off: 12  Y30 Y31 Y32 Y33
 * off: 16  U00 U02 U20 U22
 * off: 20  V00 V02 V20 V22
 * 
 * So, we loose some bit of information when converting YUY2 to YUV420 
 *
 * NOTE: This algorithm works only if the width and the height is pair.
 */
void PStandardColourConverter::YUY2toYUV420PWithResize(const BYTE *yuy2, BYTE *yuv420p) const
{
  const BYTE *s;
  BYTE *y, *u, *v;
  unsigned int x, h;  
  unsigned int npixels = srcFrameWidth * srcFrameHeight;

  s = yuy2;
  y = yuv420p;
  u = yuv420p + npixels;
  v = u + npixels/4;

  if ( (dstFrameWidth * dstFrameHeight) > npixels ) {

     // dest is bigger than the source. No subsampling.
     // Place the src in the middle of the destination.
     unsigned int yOffset = (dstFrameHeight - srcFrameHeight)/2;
     unsigned int xOffset = (dstFrameWidth - srcFrameWidth)/2;
     unsigned int bpixels = yOffset * dstFrameWidth;

     /* Top border */
     memset(y, BLACK_Y, bpixels);	y += bpixels;
     memset(u, BLACK_U, bpixels/4);	u += bpixels/4;
     memset(v, BLACK_V, bpixels/4);	v += bpixels/4;

     for (h=0; h<srcFrameHeight; h+=2)
      {
        /* Left border */
        memset(y, BLACK_Y, xOffset);	y += xOffset;
        memset(u, BLACK_U, xOffset/2);	u += xOffset/2;
        memset(v, BLACK_V, xOffset/2);	v += xOffset/2;

        /* Copy the first line keeping all information */
        for (x=0; x<srcFrameWidth; x+=2)
         {
           *y++ = *s++;
           *u++ = *s++;
           *y++ = *s++;
           *v++ = *s++;
         }
        /* Right and Left border */
        for (x=0; x<xOffset*2; x++)
          *y++ = BLACK_Y;

        /* Copy the second line discarding u and v information */
        for (x=0; x<srcFrameWidth; x+=2)
         {
           *y++ = *s++;
           s++;
           *y++ = *s++;
           s++;
         }
        /* Fill the border with black (right side) */
        memset(y, BLACK_Y, xOffset);	y += xOffset;
        memset(u, BLACK_U, xOffset/2);	u += xOffset/2;
        memset(v, BLACK_V, xOffset/2);	v += xOffset/2;
      }
     memset(y, BLACK_Y, bpixels);
     memset(u, BLACK_U, bpixels/4);
     memset(v, BLACK_V, bpixels/4);


  } else {

     // source is bigger than the destination
     // We are doing linear interpolation to find value.
#define FIX_FLOAT       12
     unsigned int dx = (srcFrameWidth<<FIX_FLOAT)/dstFrameWidth;
     unsigned int dy = (srcFrameHeight<<FIX_FLOAT)/dstFrameHeight;
     unsigned int fy, fx;

     for (fy=0, h=0; h<dstFrameHeight; h+=2, fy+=dy*2)
      {
	/* Copy the first line with U&V */
	unsigned int yy = fy>>FIX_FLOAT;
	unsigned int yy2 = (fy+dy)>>FIX_FLOAT;
	const unsigned char *line1, *line2;
	unsigned char lastU, lastV;

	line1 = s + (yy*2*srcFrameWidth);
	line2 = s + (yy2*2*srcFrameWidth);
	lastU = line1[0];
	lastV = line1[2];
	for (fx=0, x=0; x<dstFrameWidth; x+=2, fx+=dx*2)
	 {
	   unsigned int xx = (fx>>FIX_FLOAT)*2;
	   *y++ = line1[xx+1];
	   if ( (xx&2) == 0)
	    {
	      *u++ = lastU = (line1[xx+1] + line2[xx+1])/2;
	      *v++ = lastV = (line1[xx+3] + line2[xx+3])/2;
	    }
	   else
	    {
	      *u++ = lastU;
	      *v++ = lastV = (line1[xx+1] + line2[xx+1])/2;
	    }
	   xx = ((fx+dx)>>FIX_FLOAT)*2;
	   *y++ = line1[xx+1];
	   if ( (xx&2) == 0)
	     lastU = (line1[xx+1] + line2[xx+1])/2;
	   else
	     lastV = (line1[xx+1] + line2[xx+1])/2;
	 }

	/* Copy the second line without U&V */
        for (fx=0, x=0; x<dstFrameWidth; x++, fx+=dx)
         {
           unsigned int xx = (fx>>FIX_FLOAT)*2;
           *y++ = line2[xx];
         }
      } /* end of for (fy=0, h=0; h<dstFrameHeight; h+=2, fy+=dy*2) */

   }

}


PSTANDARD_COLOUR_CONVERTER(YUY2,YUV420P)
{
  const BYTE *yuy2 = srcFrameBuffer;
  BYTE *yuv420p = dstFrameBuffer;

  if ((srcFrameWidth | dstFrameWidth | srcFrameHeight | dstFrameHeight) & 1) {
    PTRACE(2,"PColCnv\tError in YUY2 to YUV420P converter, All size need to be pair.");
    return FALSE;
  }

  if ((srcFrameWidth == dstFrameWidth) || (srcFrameHeight == dstFrameHeight)) {
     YUY2toYUV420PSameSize(yuy2, yuv420p);
  } else {
     YUY2toYUV420PWithResize(yuy2, yuv420p);
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return TRUE;
}

// Consider a YUV422P image of 8x2 pixels.
//
// A plane of Y values    A B C D E F G H
//                        I J K L M N O P
//
// A plane of U values    1 . 2 . 3 . 4 .
//                        5 . 6 . 7 . 8 .
//
// A plane of V values    1 . 2 . 3 . 4 .
//                        5 . 6 . 7 . 8 .
// 
// YUV422 is stored as Y U Y V 
//   thus, a 4x4 image requires 32 bytes of storage.
//
// Image has two possible transformations.
//        padded                 (src smaller than dst)      
//        subsampled and padded  (src bigger than dst)  

void PStandardColourConverter::ResizeYUV422(const BYTE * src, BYTE * dest) const
{
  DWORD *result = (DWORD *)dest;
  DWORD black   = (DWORD)(BLACK_U<<24) + (BLACK_Y<<16) + (BLACK_U<<8) + BLACK_Y;
  unsigned maxIndex    = dstFrameWidth*dstFrameHeight/2;

  if ( (dstFrameWidth*dstFrameHeight) > (srcFrameWidth*srcFrameHeight) ) { 
    for (unsigned i = 0; i < maxIndex; i++) 
      *result++ = black;

    //dest is bigger than the source. No subsampling.
    //Place the src in the middle of the destination.
    unsigned yOffset = dstFrameHeight - srcFrameHeight;
    unsigned xOffset = dstFrameWidth - srcFrameWidth;

    BYTE *s_ptr,*d_ptr;
    d_ptr = (yOffset * dstFrameWidth) + xOffset + dest;
    s_ptr = (BYTE *)src;
    for (unsigned y = 0; y < srcFrameHeight; y++) {
      memcpy(d_ptr,s_ptr, srcFrameWidth*2);
      d_ptr += 2*dstFrameWidth;
      s_ptr += 2*srcFrameWidth;
    }
  } else {  
    // source is bigger than the destination.
    //
    unsigned subSample  = 1 + (srcFrameHeight/dstFrameHeight) ;
    unsigned yOffset    = dstFrameHeight - (srcFrameHeight/subSample);
    unsigned xOffset    = dstFrameWidth - (srcFrameWidth/subSample);
    unsigned subSample2 = subSample*2;

    DWORD *s_ptr = (DWORD * )src;
    DWORD *d_ptr = (DWORD *) dest + ((yOffset * dstFrameWidth) + xOffset)/4 ;
    DWORD *sl_ptr, *dl_ptr;

    for (unsigned y = 0; y < srcFrameHeight; y+= subSample) {
      sl_ptr = s_ptr;
      dl_ptr = d_ptr;
      for (unsigned x = 0; x < srcFrameWidth; x+= subSample2) {
        *dl_ptr++ = *sl_ptr;
        sl_ptr += subSample;
      }
      d_ptr += dstFrameWidth/2;
      s_ptr += srcFrameWidth*subSample/2;
    }
  }
}


PSTANDARD_COLOUR_CONVERTER(YUV422,YUV422)
{
  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  
  if (srcFrameBuffer == dstFrameBuffer)
    return TRUE;

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) 
    memcpy(dstFrameBuffer,srcFrameBuffer,srcFrameWidth*srcFrameHeight*2);
  else
    ResizeYUV422(srcFrameBuffer, dstFrameBuffer);

  return TRUE;
}


// copied from OpenMCU
static void ConvertQCIFToCIF(const void * _src, void * _dst)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst;

  //BYTE * dstEnd = dst + CIF_SIZE;
  int y, x;
  BYTE * srcRow;

  // copy Y
  for (y = 0; y < PVideoFrameInfo::QCIFHeight; y++) {
    srcRow = src;
    for (x = 0; x < PVideoFrameInfo::QCIFWidth; x++) {
      dst[0] = dst[1] = *srcRow++;
      dst += 2;
    }
    srcRow = src;
    for (x = 0; x < PVideoFrameInfo::QCIFWidth; x++) {
      dst[0] = dst[1] = *srcRow++;
      dst += 2;
    }
    src += PVideoFrameInfo::QCIFWidth;
  }

  // copy U
  for (y = 0; y < PVideoFrameInfo::QCIFHeight/2; y++) {
    srcRow = src;
    for (x = 0; x < PVideoFrameInfo::QCIFWidth/2; x++) {
      dst[0] = dst[1] = *srcRow++;
      dst += 2;
    }
    srcRow = src;
    for (x = 0; x < PVideoFrameInfo::QCIFWidth/2; x++) {
      dst[0] = dst[1] = *srcRow++;
      dst += 2;
    }
    src += PVideoFrameInfo::QCIFWidth/2;
  }

  // copy V
  for (y = 0; y < PVideoFrameInfo::QCIFHeight/2; y++) {
    srcRow = src;
    for (x = 0; x < PVideoFrameInfo::QCIFWidth/2; x++) {
      dst[0] = dst[1] = *srcRow++;
      dst += 2;
    }
    srcRow = src;
    for (x = 0; x < PVideoFrameInfo::QCIFWidth/2; x++) {
      dst[0] = dst[1] = *srcRow++;
      dst += 2;
    }
    src += PVideoFrameInfo::QCIFWidth/2;
  }
}


// Consider a YUV420P image of 4x4 pixels.
//
// A plane of Y values    A B C D
//                        E F G H
//                        I J K L 
//                        M N O P
//
// A plane of U values    1 . 2 . 
// 			  . . . .
// 			  3 . 4 .
//                        . . . .
//
// A plane of V values    1 . 2 .
// 			  . . . .
//                        3 . 4 .
//                        . . . .
// 
// YUV420P is stored as all Y (w*h), then U (w*h/4), then V
//   thus, a 4x4 image requires 24 bytes of storage.
//
// Image has two possible transformations.
//        padded                 (src smaller than dst)      
//        subsampled and padded  (src bigger than dst)  

void PStandardColourConverter::ResizeYUV420P(const BYTE * src, BYTE * dest) const
{
  unsigned int i, y, x, npixels;
  BYTE *d;
  const BYTE *s;

  npixels = dstFrameWidth * dstFrameHeight;
  if ( (dstFrameWidth*dstFrameHeight) > (srcFrameWidth*srcFrameHeight) ) { 

    if (srcFrameWidth  == PVideoFrameInfo::QCIFWidth && 
        srcFrameHeight == PVideoFrameInfo::QCIFHeight &&
        dstFrameWidth  == PVideoFrameInfo::CIFWidth && 
        dstFrameHeight == PVideoFrameInfo::CIFHeight) {
      ConvertQCIFToCIF(src, dest);
    } 
    else {

      // dest is bigger than the source. No subsampling.
      // Place the src in the middle of the destination.
      unsigned int yOffset = (dstFrameHeight - srcFrameHeight)/2;
      unsigned int xOffset = (dstFrameWidth - srcFrameWidth)/2;

      d = dest;
      for (i=0; i < npixels; i++) 
        *d++ = BLACK_Y;
      for (i=0; i < npixels/4; i++)
        *d++ = BLACK_U;
      for (i=0; i < npixels/4; i++)
        *d++ = BLACK_V;

      // Copy plane Y
      d = dest + yOffset * dstFrameWidth + xOffset;
      s = src;
      for (y = 0; y < srcFrameHeight; y++) {
        memcpy(d, s, srcFrameWidth);
        s += srcFrameWidth;
        d += dstFrameWidth;
      }

      // Copy plane U
      d = dest + npixels + (yOffset*dstFrameWidth/4) + xOffset/2;
      for (y = 0; y < srcFrameHeight/2; y++) {
        memcpy(d, s, srcFrameWidth/2);
        s += srcFrameWidth/2;
        d += dstFrameWidth/2;
      }

      // Copy plane V
      d = dest + npixels + npixels/4 + (yOffset*dstFrameWidth/4) + xOffset/2;
      for (y = 0; y < srcFrameHeight/2; y++) {
        memcpy(d, s, srcFrameWidth/2);
        s += srcFrameWidth/2;
        d += dstFrameWidth/2;
      }
    }

  } else {  
    // source is bigger than the destination.
    //
#define FIX_FLOAT	12
    unsigned int dx = (srcFrameWidth<<FIX_FLOAT)/dstFrameWidth;
    unsigned int dy = (srcFrameHeight<<FIX_FLOAT)/dstFrameHeight;
    unsigned int fy, fx;

    s = src;
    d = dest;

    /* Copy Plane Y */
    for (fy=0, y=0; y<dstFrameHeight; y++, fy+=dy) {
       s = src + (fy>>FIX_FLOAT) * srcFrameWidth;
       for (fx=0, x=0; x<dstFrameWidth; x++, fx+=dx) {
	  *d++ = s[fx>>FIX_FLOAT];
       }
    }

    /* Copy Plane U */
    src += srcFrameWidth*srcFrameHeight;
    for (fy=0, y=0; y<dstFrameHeight/2; y++, fy+=dy) {
       s = src + (fy>>FIX_FLOAT) * srcFrameWidth/2;
       for (fx=0, x=0; x<dstFrameWidth/2; x++, fx+=dx) {
	  *d++ = s[fx>>FIX_FLOAT];
       }
    }

    /* Copy Plane V */
    src += srcFrameWidth*srcFrameHeight/4;
    for (fy=0, y=0; y<dstFrameHeight/2; y++, fy+=dy) {
       s = src + (fy>>FIX_FLOAT) * srcFrameWidth/2;
       for (fx=0, x=0; x<dstFrameWidth/2; x++, fx+=dx) {
	  *d++ = s[fx>>FIX_FLOAT];
       }
    }

  }

}

PSTANDARD_COLOUR_CONVERTER(YUV420P,YUV420P)
{
  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  
  if (srcFrameBuffer == dstFrameBuffer) {
    if (srcFrameWidth == dstFrameWidth && srcFrameHeight == dstFrameHeight) 
      return TRUE;
    if(srcFrameWidth < dstFrameWidth || srcFrameHeight < dstFrameHeight)
      return FALSE;
  }

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) {
    memcpy(dstFrameBuffer,srcFrameBuffer,srcFrameWidth*srcFrameHeight*3/2);
    return TRUE;
  }

  switch (resizeMode) {
    case PVideoFrameInfo::eScale :
      ResizeYUV420P(srcFrameBuffer, dstFrameBuffer);
      return TRUE;

    //case PVideoFrameInfo::eCropTopLeft :
    //  break;

    default :
      return FALSE;
  }
}

/*
 * Format YUY2 or YUV422(non planar):
 *
 * off: 0  Y00 U00 Y01 V00 Y02 U01 Y03 V01
 * off: 8  Y10 U10 Y11 V10 Y12 U11 Y13 V11
 * off:16  Y20 U20 Y21 V20 Y22 U21 Y23 V21
 * off:24  Y30 U30 Y31 V30 Y32 U31 Y33 V31
 * length:32 bytes
 *
 * Format YUV420P:
 * off: 00  Y00 Y01 Y02 Y03
 * off: 04  Y10 Y11 Y12 Y13
 * off: 08  Y20 Y21 Y22 Y23
 * off: 12  Y30 Y31 Y32 Y33
 * off: 16  U00 U02 U20 U22
 * off: 20  V00 V02 V20 V22
 * 
 * So, we loose some bit of information when converting YUY2 to YUV420 
 *
 */
PSTANDARD_COLOUR_CONVERTER(YUV422,YUV420P)
{
  if ((srcFrameWidth | dstFrameWidth | srcFrameHeight | dstFrameHeight) & 1) {
    PTRACE(2,"PColCnv\tError in YUV422 to YUV420P converter, All size need to be pair.");
    return FALSE;
  }

  if ((srcFrameWidth==dstFrameWidth) && (srcFrameHeight==dstFrameHeight))
    YUY2toYUV420PSameSize(srcFrameBuffer, dstFrameBuffer);
  else
    YUY2toYUV420PWithResize(srcFrameBuffer, dstFrameBuffer);

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return TRUE;
}


#define LIMIT(x) (unsigned char) ((x > 255) ? 255 : ((x < 0) ? 0 : x ))
static inline int clip(int a, int limit) {
  return a<limit?a:limit;
}

BOOL PStandardColourConverter::SBGGR8toYUV420P(const BYTE * src, BYTE * dst, PINDEX * bytesReturned) const
{
#define USE_SBGGR8_NATIVE 1 // set to 0 to use the double conversion algorithm (Bayer->RGB->YUV420P)
  
#if USE_SBGGR8_NATIVE

  // kernels for Y conversion, normalised by 2^16
  const int kR[]={1802,9667,1802,9667,19661,9667,1802,9667,1802}; 
  const int kG1[]={7733,9830,7733,3604,7733,3604,7733,9830,7733};
  const int kG2[]={7733,3604,7733,9830,7733,9830,7733,3604,7733};
  const int kB[]={4915,9667,4915,9667,7209,9667,4915,9667,4915};
  //  const int kID[]={0,0,0,0,65536,0,0,0,0}; identity kernel, use to test

  int B, G, G1, G2, R;
  const int stride = srcFrameWidth;
  unsigned const int hSize =srcFrameHeight/2;
  unsigned const int vSize =srcFrameWidth/2;
  unsigned const int lastRow=srcFrameHeight-1;
  unsigned const int lastCol=srcFrameWidth-1;
  unsigned int i,j;
  const BYTE *sBayer = src;

  //  Y = round( 0.256788 * R + 0.504129 * G + 0.097906 * B) +  16;
  //  Y = round( 0.30 * R + 0.59 * G + 0.11 * B ) use this!
  //  U = round(-0.148223 * R - 0.290993 * G + 0.439216 * B) + 128;
  //  V = round( 0.439216 * R - 0.367788 * G - 0.071427 * B) + 128;

  // Compute U and V planes using EXACT values, reading 2x2 pixels at a time
  BYTE *dU = dst+srcFrameHeight*srcFrameWidth;
  BYTE *dV = dU+hSize*vSize;
  for (i=0; i<hSize; i++) {      
    for (j=0; j<vSize; j++) {
      B=sBayer[0];
      G1=sBayer[1];
      G2=sBayer[stride];
      R=sBayer[stride+1];
      G=G1+G2;
      *dU = (BYTE)( ( (-19428 * R -19071*G +57569 * B) >> 17) + 128 );
      *dV = (BYTE)( ( ( 57569 * R -24103*G -9362 * B) >> 17) + 128 );
      sBayer+=2;
      dU++;
      dV++;
    }
    sBayer+=stride; // skip odd lines
  }
  // Compute Y plane
  BYTE *dY = dst;
  sBayer=src;
  const int * k; // kernel pointer
  int dxLeft, dxRight; // precalculated offsets, needed for first and last column
  const BYTE *sBayerTop, *sBayerBottom;
  for (i=0; i<srcFrameHeight; i++) {
    // Pointer to previous row, to the next if we are on the first one
    sBayerTop=sBayer+(i?(-stride):stride);
    // Pointer to next row, to the previous one if we are on the last
    sBayerBottom=sBayer+((i<lastRow)?stride:(-stride));
    // offset to previous column, to the next if we are on the first col
    dxLeft=1;
    for (j=0; j<srcFrameWidth; j++) {
      // offset to next column, to previous if we are on the last one
      dxRight=j<lastCol?1:(-1);
      // find the proper kernel according to the current pixel color
      if ( (i ^ j) & 1)  k=(j&1)?kG1:kG2; // green 1 or green 2
      else if (!(i & 1))  k=kB; // blue
      else /* if (!(j & 1)) */ k=kR; // red
      
      // apply the proper kernel to this pixel and surrounding ones
      *dY= (BYTE)(clip( (k[0])*(int)sBayerTop[dxLeft]+
      (k[1])*(int)(*sBayerTop)+
      (k[2])*(int)sBayerTop[dxRight]+
      (k[3])*(int)sBayer[dxLeft]+
      (k[4])*(int)(*sBayer)+
      (k[5])*(int)sBayer[dxRight]+
      (k[6])*(int)sBayerBottom[dxLeft]+
      (k[7])*(int)(*sBayerBottom)+
      (k[8])*(int)sBayerBottom[dxRight], (1<<24)) >> 16);
      dY++;
      sBayer++;
      sBayerTop++;
      sBayerBottom++;
      dxLeft=-1;
    }
  }

  if (bytesReturned)
    *bytesReturned = srcFrameHeight*srcFrameWidth+2*hSize*vSize;

  return true;

#else //USE_SBGGR8_NATIVE

  // shortest but less efficient (one malloc per conversion!)
  BYTE * tempDest=(BYTE*)malloc(3*srcFrameWidth*srcFrameHeight);
  SBGGR8toRGB(src, tempDest, NULL);
  BOOL r = RGBtoYUV420P(tempDest, dst, bytesReturned, 3, 2, 0);
  free(tempDest);
  return r;

#endif //USE_SBGGR8_NATIVE
}

BOOL PStandardColourConverter::SBGGR8toRGB(const BYTE * src,
                                           BYTE       * dst,
                                           PINDEX     * bytesReturned) const
{
  if (src == dst || verticalFlip)
    return FALSE;

  long int i;
  const BYTE *rawpt;
  BYTE *scanpt;
  long int size;

  rawpt = src;
  scanpt = dst;
  long int WIDTH = srcFrameWidth, HEIGHT = srcFrameHeight;
  size = WIDTH*HEIGHT;

  for ( i = 0; i < size; i++ ) {
    if ( (i/WIDTH) % 2 == 0 ) {
      if ( (i % 2) == 0 ) {
        /* B */
        if ( (i > WIDTH) && ((i % WIDTH) > 0) ) {
          *scanpt++ = (BYTE) ((*(rawpt-WIDTH-1)+*(rawpt-WIDTH+1)+ *(rawpt+WIDTH-1)+*(rawpt+WIDTH+1))/4);  /* R */
          *scanpt++ = (BYTE) ((*(rawpt-1)+*(rawpt+1)+ *(rawpt+WIDTH)+*(rawpt-WIDTH))/4);  /* G */
          *scanpt++ = *rawpt;         /* B */
        } else {
          /* first line or left column */
          *scanpt++ = *(rawpt+WIDTH+1);   /* R */
          *scanpt++ = (BYTE) ((*(rawpt+1)+*(rawpt+WIDTH))/2); /* G */
          *scanpt++ = *rawpt;       /* B */
        }
      } else {
        /* (B)G */
        if ( (i > WIDTH) && ((i % WIDTH) < (WIDTH-1)) ) {
          *scanpt++ = (BYTE) ((*(rawpt+WIDTH)+*(rawpt-WIDTH))/2); /* R */
          *scanpt++ = *rawpt;         /* G */
          *scanpt++ = (BYTE) ((*(rawpt-1)+*(rawpt+1))/2);   /* B */
        } else {
          /* first line or right column */
          *scanpt++ = *(rawpt+WIDTH); /* R */
          *scanpt++ = *rawpt;   /* G */
          *scanpt++ = *(rawpt-1); /* B */
        }
      }
    } else {
      if ( (i % 2) == 0 ) {
        /* G(R) */
        if ( (i < (WIDTH*(HEIGHT-1))) && ((i % WIDTH) > 0) ) {
          *scanpt++ = (BYTE) ((*(rawpt-1)+*(rawpt+1))/2);   /* R */
          *scanpt++ = *rawpt;         /* G */
          *scanpt++ = (BYTE) ((*(rawpt+WIDTH)+*(rawpt-WIDTH))/2); /* B */
        } else {
          /* bottom line or left column */
          *scanpt++ = *(rawpt+1);   /* R */
          *scanpt++ = *rawpt;         /* G */
          *scanpt++ = *(rawpt-WIDTH);   /* B */
        }
      } else {
        /* R */
        if ( i < (WIDTH*(HEIGHT-1)) && ((i % WIDTH) < (WIDTH-1)) ) {
          *scanpt++ = *rawpt;         /* R */
          *scanpt++ = (BYTE) ((*(rawpt-1)+*(rawpt+1)+*(rawpt-WIDTH)+*(rawpt+WIDTH))/4);  /* G */
          *scanpt++ = (BYTE) ((*(rawpt-WIDTH-1)+*(rawpt-WIDTH+1)+*(rawpt+WIDTH-1)+*(rawpt+WIDTH+1))/4);  /* B */
        } else {
          /* bottom line or right column */
          *scanpt++ = *rawpt;        /* R */
          *scanpt++ = (BYTE) ((*(rawpt-1)+*(rawpt-WIDTH))/2);  /* G */
          *scanpt++ = *(rawpt-WIDTH-1);    /* B */
        }
      }
    }
    rawpt++;
  }

  if (bytesReturned)
    *bytesReturned = scanpt - dst;

  return TRUE;
}

#define SCALEBITS 12
#define ONE_HALF  (1UL << (SCALEBITS - 1))
#define FIX(x)    ((int) ((x) * (1UL<<SCALEBITS) + 0.5))

/* 
 * Please note when converting colorspace from YUV to RGB.
 * Not all YUV have the same colorspace. 
 *
 * For instance Jpeg use this formula
 * YCbCr is defined per CCIR 601-1, except that Cb and Cr are
 * normalized to the range 0..MAXJSAMPLE rather than -0.5 .. 0.5.
 * The conversion equations to be implemented are therefore
 *      Y  =  0.29900 * R + 0.58700 * G + 0.11400 * B
 *      Cb = -0.16874 * R - 0.33126 * G + 0.50000 * B  + CENTERJSAMPLE
 *      Cr =  0.50000 * R - 0.41869 * G - 0.08131 * B  + CENTERJSAMPLE
 * (These numbers are derived from TIFF 6.0 section 21, dated 3-June-92.)
 * So
 * R = Y + 1.402 (Cr-128)
 * G = Y - 0.34414 (Cb-128) - 0.71414 (Cr-128)
 * B = Y + 1.772 (Cb-128)
 * 
 */
BOOL PStandardColourConverter::YUV420PtoRGB(const BYTE * srcFrameBuffer,
                                            BYTE * dstFrameBuffer,
                                            PINDEX * bytesReturned,
                                            unsigned rgbIncrement,
                                            unsigned redOffset,
                                            unsigned blueOffset) const
{
  if (srcFrameBuffer == dstFrameBuffer)
    return FALSE; // Cannot do in-place conversion

  static const unsigned greenOffset = 1;

  unsigned height = PMIN(srcFrameHeight, dstFrameHeight)&(UINT_MAX-1); // Must be even
  unsigned width = PMIN(srcFrameWidth, dstFrameWidth)&(UINT_MAX-1);

  unsigned    yplanesize = srcFrameWidth*srcFrameHeight;
  const BYTE *yplane     = srcFrameBuffer;        // 1 byte Y (luminance) for each pixel
  const BYTE *uplane     = yplane+yplanesize;     // 1 byte U for a block of 4 pixels
  const BYTE *vplane     = uplane+(yplanesize/4); // 1 byte V for a block of 4 pixels

  BYTE * dstScanLine   = dstFrameBuffer;

#ifdef P_MEDIALIB
  const BYTE *y0;
  const BYTE *y1;
  const BYTE *cb;
  const BYTE *cr;
  unsigned int   x,p;

  for(int i = 0; i < srcFrameHeight; i += 2) {
    p = i*srcFrameWidth;
    x = p/4;
    y0 = yplane + p;
    y1 = y0 + srcFrameWidth;
    cb = uplane + x;
    cr = vplane + x;
    mlib_VideoColorJFIFYCC2RGB420_Nearest(dstFrameBuffer,
                                          dstFrameBuffer+3*dstFrameWidth, 
                                          y0, y1, cb, cr,
                                          srcFrameWidth);
    dstFrameBuffer += 6*dstFrameWidth;
  }
#else

  unsigned int srcPixpos[4] = { 0, 1, srcFrameWidth, srcFrameWidth + 1 };
  unsigned int dstPixpos[4] = { 0, rgbIncrement, dstFrameWidth*rgbIncrement, (dstFrameWidth+1)*rgbIncrement };

  if (verticalFlip) {
    dstScanLine += (dstFrameHeight - 2) * dstFrameWidth * rgbIncrement;
    dstPixpos[0] = dstFrameWidth;
    dstPixpos[1] = dstFrameWidth +1;
    dstPixpos[2] = 0;
    dstPixpos[3] = 1;
  }

  for (unsigned y = 0; y < height; y += 2)
  {
    BYTE * dstPixelGroup = dstScanLine;
    for (unsigned x = 0; x < width; x += 2)
    {
      // The RGB value without luminance
      long cb = *uplane-128;
      long cr = *vplane-128;
      long rd = FIX(1.40200) * cr + ONE_HALF;
      long gd = -FIX(0.34414) * cb -FIX(0.71414) * cr + ONE_HALF;
      long bd = FIX(1.77200) * cb + ONE_HALF;

      // Add luminance to each of the 4 pixels

      for (unsigned p = 0; p < 4; p++)
      {
        int yvalue = *(yplane + srcPixpos[p]);

        int l = yvalue << SCALEBITS;

        int r = (l+rd)>>SCALEBITS;
        int g = (l+gd)>>SCALEBITS;
        int b = (l+bd)>>SCALEBITS;

        BYTE * rgpPtr = dstPixelGroup + dstPixpos[p];
        rgpPtr[redOffset]   = LIMIT(r);
        rgpPtr[greenOffset] = LIMIT(g);
        rgpPtr[blueOffset]  = LIMIT(b);
        if (rgbIncrement == 4)
          rgpPtr[3] = 0;
      }

      yplane += 2;
      dstPixelGroup += rgbIncrement*2;

      uplane++;
      vplane++;
    }
 
    yplane += srcFrameWidth;

    dstScanLine += (verticalFlip?-2:2)*rgbIncrement*dstFrameWidth;
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
#endif

  return TRUE;
}

PSTANDARD_COLOUR_CONVERTER(SBGGR8,RGB24)
{
  return SBGGR8toRGB(srcFrameBuffer, dstFrameBuffer, bytesReturned);
}

PSTANDARD_COLOUR_CONVERTER(SBGGR8,YUV420P)
{
  return SBGGR8toYUV420P(srcFrameBuffer, dstFrameBuffer, bytesReturned);
}

PSTANDARD_COLOUR_CONVERTER(YUV420P,RGB24)
{
  return YUV420PtoRGB(srcFrameBuffer, dstFrameBuffer, bytesReturned, 3, 0, 2);
}

PSTANDARD_COLOUR_CONVERTER(YUV420P,BGR24)
{
  return YUV420PtoRGB(srcFrameBuffer, dstFrameBuffer, bytesReturned, 3, 2, 0);
}

PSTANDARD_COLOUR_CONVERTER(YUV420P,RGB32)
{
  return YUV420PtoRGB(srcFrameBuffer, dstFrameBuffer, bytesReturned, 4, 0, 2);
}

PSTANDARD_COLOUR_CONVERTER(YUV420P,BGR32)
{
  return YUV420PtoRGB(srcFrameBuffer, dstFrameBuffer, bytesReturned, 4, 2, 0);
}


static void SwapRedAndBlueRow(const BYTE * srcRowPtr,
                              BYTE * dstRowPtr,
                              unsigned width,
                              unsigned srcIncrement,
                              unsigned dstIncrement)
{
  for (unsigned x = 0; x < width; x++) {
    BYTE temp = srcRowPtr[0]; // Do it this way in case src and dst are same buffer
    dstRowPtr[0] = srcRowPtr[2];
    dstRowPtr[1] = srcRowPtr[1];
    dstRowPtr[2] = temp;

    srcRowPtr += srcIncrement;
    dstRowPtr += dstIncrement;
  }
}

BOOL PStandardColourConverter::SwapRedAndBlue(const BYTE * srcFrameBuffer,
                                              BYTE * dstFrameBuffer,
                                              PINDEX * bytesReturned,
                                              unsigned srcIncrement,
                                              unsigned dstIncrement) const
{
  if ((dstFrameWidth != srcFrameWidth) || (dstFrameHeight != srcFrameHeight))
    return FALSE;

  unsigned srcRowSize = srcFrameBytes/srcFrameHeight;
  const BYTE * srcRowPtr = srcFrameBuffer;

  unsigned dstRowSize = dstFrameBytes/dstFrameHeight;
  BYTE * dstRowPtr = dstFrameBuffer;

  if (verticalFlip) {
    dstRowPtr += dstFrameHeight*dstRowSize;

    if (srcFrameBuffer == dstFrameBuffer) {
      PBYTEArray tempRow(PMAX(srcRowSize, dstRowSize));
      unsigned halfHeight = (srcFrameHeight+1)/2;
      for (unsigned y = 0; y < halfHeight; y++) {
        dstRowPtr -= dstRowSize;
        SwapRedAndBlueRow(dstRowPtr, tempRow.GetPointer(), dstFrameWidth, srcIncrement, dstIncrement);
        SwapRedAndBlueRow(srcRowPtr, dstRowPtr, srcFrameWidth, srcIncrement, dstIncrement);
        memcpy((BYTE *)srcRowPtr, tempRow, srcRowSize);
        srcRowPtr += srcRowSize;
      }
    }
    else {
      for (unsigned y = 0; y < srcFrameHeight; y++) {
        dstRowPtr -= dstRowSize;
        SwapRedAndBlueRow(srcRowPtr, dstRowPtr, srcFrameWidth, srcIncrement, dstIncrement);
        srcRowPtr += srcRowSize;
      }
    }
  }
  else {
    for (unsigned y = 0; y < srcFrameHeight; y++) {
      SwapRedAndBlueRow(srcRowPtr, dstRowPtr, srcFrameWidth, srcIncrement, dstIncrement);
      srcRowPtr += srcRowSize;
      dstRowPtr += dstRowSize;
    }
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  return TRUE;
}


PSTANDARD_COLOUR_CONVERTER(RGB24,BGR24)
{
  return SwapRedAndBlue(srcFrameBuffer, dstFrameBuffer, bytesReturned, 3, 3);
}


PSTANDARD_COLOUR_CONVERTER(BGR24,RGB24)
{
  return SwapRedAndBlue(srcFrameBuffer, dstFrameBuffer, bytesReturned, 3, 3);
}


PSTANDARD_COLOUR_CONVERTER(RGB24,BGR32)
{
  return SwapRedAndBlue(srcFrameBuffer, dstFrameBuffer, bytesReturned, 3, 4);
}


PSTANDARD_COLOUR_CONVERTER(BGR24,RGB32)
{
  return SwapRedAndBlue(srcFrameBuffer, dstFrameBuffer, bytesReturned, 3, 4);
}


PSTANDARD_COLOUR_CONVERTER(RGB32,BGR24)
{
  return SwapRedAndBlue(srcFrameBuffer, dstFrameBuffer, bytesReturned, 4, 3);
}


PSTANDARD_COLOUR_CONVERTER(BGR32,RGB24)
{
  return SwapRedAndBlue(srcFrameBuffer, dstFrameBuffer, bytesReturned, 4, 3);
}


PSTANDARD_COLOUR_CONVERTER(RGB32,BGR32)
{
  return SwapRedAndBlue(srcFrameBuffer, dstFrameBuffer, bytesReturned, 4, 4);
}


PSTANDARD_COLOUR_CONVERTER(BGR32,RGB32)
{
  return SwapRedAndBlue(srcFrameBuffer, dstFrameBuffer, bytesReturned, 4, 4);
}


PSTANDARD_COLOUR_CONVERTER(RGB24,RGB32)
{
  if ((dstFrameWidth != srcFrameWidth) || (dstFrameHeight != srcFrameHeight))
    return FALSE;

  // Go from bottom to top so can do in place conversion
  const BYTE * src = srcFrameBuffer+srcFrameBytes-1;
  BYTE * dst = dstFrameBuffer+dstFrameBytes-1;

  for (unsigned x = 0; x < srcFrameWidth; x++) {
    for (unsigned y = 0; y < srcFrameHeight; y++) {
      *dst-- = 0;
      for (unsigned p = 0; p < 3; p++)
        *dst-- = *src--;
    }
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  return TRUE;
}


PSTANDARD_COLOUR_CONVERTER(RGB32,RGB24)
{
  if ((dstFrameWidth != srcFrameWidth) || (dstFrameHeight != srcFrameHeight))
    return FALSE;

  const BYTE * src = srcFrameBuffer;
  BYTE * dst = dstFrameBuffer;

  for (unsigned x = 0; x < srcFrameWidth; x++) {
    for (unsigned y = 0; y < srcFrameHeight; y++) {
      for (unsigned p = 0; p < 3; p++)
        *dst++ = *src++;
      src++;
    }
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  return TRUE;
}


// Consider a YUV420P image of 8x2 pixels.
//
// A plane of Y values    A B C D E F G H
//                        I J K L M N O P
//
// A plane of U values    1   2   3   4 
// A plane of V values    1   2   3   4 ....
//
// The U1/V1 samples correspond to the ABIJ pixels.
//     U2/V2 samples correspond to the CDKL pixels.
//
// Consider a YUV411P image of 8x2 pixels.
//
// A plane of Y values as before.
//
// A plane of U values    1       2
//                        3       4
//
// A plane of V values    1       2
//                        3       4
//
// The U1/V1 samples correspond to the ABCD pixels.
//     U2/V2 samples correspond to the EFGH pixels.
//
// I choose to reoganize the U and V samples by using
// using U1 for ABCD, U3 for EFGH, U2 for IJKL, U4 for MNOP
//
// Possibly discarding U2/U4 completely, or using the
// average of U1 and U2 might be easier for compression
//
// TODO:
//
// - Inplace converter
// - Resizing / padding / scaling converter
//
PSTANDARD_COLOUR_CONVERTER(YUV420P,YUV411P)
{
  if (srcFrameBuffer == dstFrameBuffer)
    return FALSE;

  if ((dstFrameWidth != srcFrameWidth) || (dstFrameHeight != srcFrameHeight))
    return FALSE;

  // Copy over the Y plane.
  memcpy(dstFrameBuffer, srcFrameBuffer, srcFrameWidth*srcFrameHeight);

  unsigned linewidth = dstFrameWidth / 4;

  // Source data is the start of the U plane
  const BYTE* src = srcFrameBuffer + srcFrameWidth * srcFrameHeight;

  // Two output lines at a time
  BYTE *dst0 = dstFrameBuffer + dstFrameWidth * dstFrameHeight;
  BYTE *dst1 = dst0 + linewidth;

  unsigned x, y;

  // U plane
  for (y = 0; y < dstFrameHeight; y += 2) {
    for (x = 0; x < dstFrameWidth; x += 4) {
      *dst0++ = *src++;
      *dst1++ = *src++;
    }

    // Skip over the 2nd line we already did.
    dst0 += linewidth;
    dst1 = dst0 + linewidth;
  }

  // Source data is the start of the U plane
  src = srcFrameBuffer + srcFrameWidth * srcFrameHeight * 5 / 4;

  // Two output lines at a time
  dst0 = dstFrameBuffer + dstFrameWidth * dstFrameHeight * 5 / 4;
  dst1 = dst0 + linewidth;

  // V plane
  for (y = 0; y < dstFrameHeight; y += 2) {
    for (x = 0; x < dstFrameWidth; x += 4) {
      *dst0++ = *src++;
      *dst1++ = *src++;
    }

    // Skip over the 2nd line we already did.
    dst0 += linewidth;
    dst1 = dst0 + linewidth;
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  
  return TRUE;
}


// YUV411P to YUV420P conversion
//
// Consider YUV411P U plane (. = pixel) :
//
// A... B... C... D...
// E... F... G... H...
// I... J... K... L...
// M... N... O... P...
//
// We map this to a YUV420P plane by
// discarding odd rows, and doubling up
// the even row samples:
//
// A.A. B.B. C.C. D.D.
// .... .... .... ....
// I.I. J.J. K.K. L.L.
// .... .... .... ....
//
// TODO:
//
// - Inplace converter
// - Resizing / padding / scaling converter
//
PSTANDARD_COLOUR_CONVERTER(YUV411P,YUV420P)
{
  if (srcFrameBuffer == dstFrameBuffer)
    return FALSE;

  if ((dstFrameWidth != srcFrameWidth) || (dstFrameHeight != srcFrameHeight))
    return FALSE;

  // Copy over the Y plane.
  memcpy(dstFrameBuffer, srcFrameBuffer, srcFrameWidth*srcFrameHeight);

  unsigned linewidth = dstFrameWidth / 4;

  // Source data is the start of the U plane
  const BYTE* src = srcFrameBuffer + srcFrameWidth * srcFrameHeight;

  // Output line
  BYTE *dst0 = dstFrameBuffer + dstFrameWidth * dstFrameHeight;

  unsigned x, y;

  // U plane
  for (y = 0; y < dstFrameHeight; y += 2) {
    for (x = 0; x < dstFrameWidth; x += 4) {

      // Double up the horizontal samples
      *dst0++ = *src;
      *dst0++ = *src++;
    }

    // Skip over the 2nd line we are decimating
    src += linewidth;
  }

  // Source data is the start of the U plane
  src = srcFrameBuffer + srcFrameWidth * srcFrameHeight * 5 / 4;

  // Output line
  dst0 = dstFrameBuffer + dstFrameWidth * dstFrameHeight * 5 / 4;

  // V plane
  for (y = 0; y < dstFrameHeight; y += 2) {
    for (x = 0; x < dstFrameWidth; x += 4) {

      // Double up the samples horizontal samples
      *dst0++ = *src;
      *dst0++ = *src++;
    }

    // Skip over the 2nd source line we already did.
    src += linewidth;
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return TRUE;
}


/*
 * Format UYVY or UYVY422(non planar) 4x4
 *
 * off: 0  U00 Y00 V01 Y00 U02 Y01 V03 Y01
 * off: 8  U10 Y10 V11 Y10 U12 Y11 V13 Y11
 * off:16  U20 Y20 V21 Y20 U22 Y21 V23 Y21
 * off:24  U30 Y30 V31 Y30 U32 Y31 V33 Y31
 * length:32 bytes
 */
PSTANDARD_COLOUR_CONVERTER(UYVY422, UYVY422)
{
  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  
  if (srcFrameBuffer == dstFrameBuffer) {
	if (srcFrameWidth == dstFrameWidth && srcFrameHeight == dstFrameHeight) 
		return TRUE;
	else if(srcFrameWidth < dstFrameWidth || srcFrameHeight < dstFrameHeight)
		return FALSE;
  }

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) 
    memcpy(dstFrameBuffer,srcFrameBuffer,srcFrameWidth*srcFrameHeight*2);
  else
    ResizeUYVY422(srcFrameBuffer, dstFrameBuffer);

  return TRUE;
}

/*
 * Format UYVY (or UYVY422) non planar (4x4)
 *
 * off: 0  U00 Y00 V01 Y00 U02 Y01 V03 Y01
 * off: 8  U10 Y10 V11 Y10 U12 Y11 V13 Y11
 * off:16  U20 Y20 V21 Y20 U22 Y21 V23 Y21
 * off:24  U30 Y30 V31 Y30 U32 Y31 V33 Y31
 * length:32 bytes
 *
 * NOTE: This algorithm works only if the width and the height is pair.
 */
void PStandardColourConverter::ResizeUYVY422(const BYTE *src_uyvy, BYTE *dst_uyvy) const
{
  const BYTE *s;
  BYTE *d;
  unsigned int x, h;  
  unsigned int npixels = dstFrameWidth * dstFrameHeight;

  s = src_uyvy;
  d = dst_uyvy;

  if ( (srcFrameWidth * srcFrameHeight) < npixels ) {

     // dest is bigger than the source. No subsampling.
     // Place the src in the middle of the destination.
     unsigned int yOffset = (dstFrameHeight - srcFrameHeight)/2;
     unsigned int xOffset = (dstFrameWidth - srcFrameWidth)/2;

     /* Top border */
     for (h=0; h<yOffset; h++)
      {
	for (x=0; x<dstFrameWidth/2; x++)
	 {
	   *d++ = BLACK_U;
	   *d++ = BLACK_Y;
	   *d++ = BLACK_V;
	   *d++ = BLACK_Y;
	 }
      }

     for (h=0; h<srcFrameHeight; h+=2)
      {
        /* Left border */
       	for (x=0; x<xOffset/2; x++)
	 {
	   *d++ = BLACK_U;
	   *d++ = BLACK_Y;
	   *d++ = BLACK_V;
	   *d++ = BLACK_Y;
	 }

        /* Copy the first line keeping all information */
	memcpy(d, s, srcFrameWidth*2);
	d += srcFrameWidth*2;
        /* Right and Left border */
       	for (x=0; x<xOffset/2; x++)
	 {
	   *d++ = BLACK_U;
	   *d++ = BLACK_Y;
	   *d++ = BLACK_V;
	   *d++ = BLACK_Y;
	 }
      }
     for (h=0; h<yOffset; h++)
      {
	for (x=0; x<dstFrameWidth/2; x++)
	 {
	   *d++ = BLACK_U;
	   *d++ = BLACK_Y;
	   *d++ = BLACK_V;
	   *d++ = BLACK_Y;
	 }
      }

  } else {

     /* FIXME */

   }

}


/*
 * Format UYVY or UYVY422(non planar) 4x4
 *
 * off: 0  U00 Y00 V01 Y00 U02 Y01 V03 Y01
 * off: 8  U10 Y10 V11 Y10 U12 Y11 V13 Y11
 * off:16  U20 Y20 V21 Y20 U22 Y21 V23 Y21
 * off:24  U30 Y30 V31 Y30 U32 Y31 V33 Y31
 * length:32 bytes
 *
 * Format YUV420P:
 * off: 00  Y00 Y01 Y02 Y03
 * off: 04  Y10 Y11 Y12 Y13
 * off: 08  Y20 Y21 Y22 Y23
 * off: 12  Y30 Y31 Y32 Y33
 * off: 16  U00 U02 U20 U22
 * off: 20  V00 V02 V20 V22
 * 
 * So, we loose some bit of information when converting UYVY to YUV420 
 *
 * NOTE: This algorithm works only if the width and the height is pair.
 */
void  PStandardColourConverter::UYVY422toYUV420PSameSize(const BYTE *uyvy, BYTE *yuv420p) const
{
  const BYTE *s;
  BYTE *y, *u, *v;
  unsigned int x, h;  
  int npixels = srcFrameWidth * srcFrameHeight;

  s = uyvy;
  y = yuv420p;
  u = yuv420p + npixels;
  v = u + npixels/4;

  for (h=0; h<srcFrameHeight; h+=2) {

     /* Copy the first line keeping all information */
     for (x=0; x<srcFrameWidth; x+=2) {
	*u++ = *s++;
	*y++ = *s++;
	*v++ = *s++;
	*y++ = *s++;
     }
     /* Copy the second line discarding u and v information */
     for (x=0; x<srcFrameWidth; x+=2) {
	s++;
	*y++ = *s++;
	s++;
	*y++ = *s++;
     }
  }
}


/*
 * Format UYVY (or UYVY422) non planar (4x4)
 *
 * off: 0  U00 Y00 V01 Y00 U02 Y01 V03 Y01
 * off: 8  U10 Y10 V11 Y10 U12 Y11 V13 Y11
 * off:16  U20 Y20 V21 Y20 U22 Y21 V23 Y21
 * off:24  U30 Y30 V31 Y30 U32 Y31 V33 Y31
 * length:32 bytes
 *
 * Format YUV420P:
 * off: 00  Y00 Y01 Y02 Y03
 * off: 04  Y10 Y11 Y12 Y13
 * off: 08  Y20 Y21 Y22 Y23
 * off: 12  Y30 Y31 Y32 Y33
 * off: 16  U00 U02 U20 U22
 * off: 20  V00 V02 V20 V22
 * 
 * So, we loose some bit of information when converting YUY2 to YUV420 
 *
 * NOTE: This algorithm works only if the width and the height is pair.
 */
void PStandardColourConverter::UYVY422toYUV420PWithResize(const BYTE *uyvy, BYTE *yuv420p) const
{
  const BYTE *s;
  BYTE *y, *u, *v;
  unsigned int x, h;  
  unsigned int npixels = dstFrameWidth * dstFrameHeight;

  s = uyvy;
  y = yuv420p;
  u = yuv420p + npixels;
  v = u + npixels/4;

  if ( (srcFrameWidth * srcFrameHeight) < npixels ) {

     // dest is bigger than the source. No subsampling.
     // Place the src in the middle of the destination.
     unsigned int yOffset = (dstFrameHeight - srcFrameHeight)/2;
     unsigned int xOffset = (dstFrameWidth - srcFrameWidth)/2;
     unsigned int bpixels = yOffset * dstFrameWidth;

     /* Top border */
     memset(y, BLACK_Y, bpixels);	y += bpixels;
     memset(u, BLACK_U, bpixels/4);	u += bpixels/4;
     memset(v, BLACK_V, bpixels/4);	v += bpixels/4;

     for (h=0; h<srcFrameHeight; h+=2)
      {
        /* Left border */
        memset(y, BLACK_Y, xOffset);	y += xOffset;
        memset(u, BLACK_U, xOffset/2);	u += xOffset/2;
        memset(v, BLACK_V, xOffset/2);	v += xOffset/2;

        /* Copy the first line keeping all information */
        for (x=0; x<srcFrameWidth; x+=2)
         {
           *u++ = *s++;
           *y++ = *s++;
           *v++ = *s++;
           *y++ = *s++;
         }
        /* Right and Left border */
        for (x=0; x<xOffset*2; x++)
          *y++ = BLACK_Y;

        /* Copy the second line discarding u and v information */
        for (x=0; x<srcFrameWidth; x+=2)
         {
           s++;
           *y++ = *s++;
           s++;
           *y++ = *s++;
         }
        /* Fill the border with black (right side) */
        memset(y, BLACK_Y, xOffset);	y += xOffset;
        memset(u, BLACK_U, xOffset/2);	u += xOffset/2;
        memset(v, BLACK_V, xOffset/2);	v += xOffset/2;
      }
     memset(y, BLACK_Y, bpixels);
     memset(u, BLACK_U, bpixels/4);
     memset(v, BLACK_V, bpixels/4);


  } else {

     // source is bigger than the destination
     // We are doing linear interpolation to find value.
#define FIX_FLOAT       12
     unsigned int dx = (srcFrameWidth<<FIX_FLOAT)/dstFrameWidth;
     unsigned int dy = (srcFrameHeight<<FIX_FLOAT)/dstFrameHeight;
     unsigned int fy, fx;

     for (fy=0, h=0; h<dstFrameHeight; h+=2, fy+=dy*2)
      {
	/* Copy the first line with U&V */
	unsigned int yy = fy>>FIX_FLOAT;
	unsigned int yy2 = (fy+dy)>>FIX_FLOAT;
	const unsigned char *line1, *line2;
	unsigned char lastU, lastV;

	line1 = s + (yy*2*srcFrameWidth);
	line2 = s + (yy2*2*srcFrameWidth);
	lastU = line1[0];
	lastV = line1[2];
	for (fx=0, x=0; x<dstFrameWidth; x+=2, fx+=dx*2)
	 {
	   unsigned int xx = (fx>>FIX_FLOAT)*2;
	   if ( (xx&2) == 0)
	    {
	      *u++ = lastU = (line1[xx+0] + line2[xx+0])/2;
	      *v++ = lastV = (line1[xx+2] + line2[xx+2])/2;
	    }
	   else
	    {
	      *u++ = lastU;
	      *v++ = lastV = (line1[xx+0] + line2[xx+0])/2;
	    }
	   *y++ = line1[xx+1];
	   xx = ((fx+dx)>>FIX_FLOAT)*2;
	   if ( (xx&2) == 0)
	     lastU = (line1[xx+0] + line2[xx+0])/2;
	   else
	     lastV = (line1[xx+0] + line2[xx+0])/2;
	   *y++ = line1[xx+1];
	 }

	/* Copy the second line without U&V */
        for (fx=0, x=0; x<dstFrameWidth; x++, fx+=dx)
         {
           unsigned int xx = (fx>>FIX_FLOAT)*2;
           *y++ = line2[xx+1];
         }
      } /* end of for (fy=0, h=0; h<dstFrameHeight; h+=2, fy+=dy*2) */

   }

}


/*
 * The following functions converts video from IEEE 1394 cameras into
 * YUV420P format. The video format of IEEE 1394 cameras can be found
 *  at Section 2.1.3 of
http://www.1394ta.org/Download/Technology/Specifications/2000/IIDC_Spec_v1_30.pdf
 * 320x240 and 160x120 resolutions are used.
 *
 */
PSTANDARD_COLOUR_CONVERTER(UYVY422,YUV420P)
{
  if (srcFrameBuffer == dstFrameBuffer)
    return FALSE;

  if ((srcFrameWidth==dstFrameWidth) && (srcFrameHeight==dstFrameHeight))
    UYVY422toYUV420PSameSize(srcFrameBuffer, dstFrameBuffer);
  else
    UYVY422toYUV420PWithResize(srcFrameBuffer, dstFrameBuffer);

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return TRUE;
}

PSTANDARD_COLOUR_CONVERTER(UYV444,YUV420P)
{
  if (srcFrameBuffer == dstFrameBuffer)
    return FALSE;

  unsigned int row,column;
  unsigned char *y = dstFrameBuffer;  //Initialise y,u,v here, to stop compiler warnings.
  unsigned char *u = dstFrameBuffer + dstFrameWidth*dstFrameHeight;
  unsigned char *v = dstFrameBuffer + dstFrameWidth*(dstFrameHeight + dstFrameHeight/4);
  const unsigned char *src = srcFrameBuffer;

  for(row=0; row < PMIN(srcFrameHeight, dstFrameHeight); row+=2) {
    y = dstFrameBuffer + dstFrameWidth*row;
    u = dstFrameBuffer + dstFrameWidth*dstFrameHeight + dstFrameWidth*row/4;
    v = dstFrameBuffer + dstFrameWidth*(dstFrameHeight + dstFrameHeight/4) + dstFrameWidth*row/4;
    src = srcFrameBuffer + row*srcFrameWidth*3;
    for(column=0; column < PMIN(srcFrameWidth, dstFrameWidth); column+=2) {
      *(u++) = (unsigned char)(((unsigned int)src[0] + src[3] + src[srcFrameWidth*3] + src[3+srcFrameWidth*3])/4);
      *(y++) = src[1];
      *(v++) = (unsigned char)(((unsigned int)src[2] + src[5] + src[srcFrameWidth*3] +src[3+srcFrameWidth*3])/4);
      *(y++) = src[4];
      src += 6;
    }
    for(column = PMIN(srcFrameWidth, dstFrameWidth);
  column < dstFrameWidth; column+=2) {
      *(u++) = BLACK_U;
      *(y++) = BLACK_Y;
      *(v++) = BLACK_V;
      *(y++) = BLACK_Y;
    }
    y = dstFrameBuffer + dstFrameWidth*(row+1);
    src = srcFrameBuffer + (row+1)*srcFrameWidth*3;
    for(column=0; column < PMIN(srcFrameWidth, dstFrameWidth); column++) {
      src++;
      *(y++) = *(src++);
      src++;
    }
    for(column = PMIN(srcFrameWidth, dstFrameWidth);
  column < dstFrameWidth; column++)
      *(y++) = BLACK_Y;
  }
  for(row = PMIN(srcFrameHeight, dstFrameHeight);
      row<dstFrameHeight; row+=2) {
    for(column = 0; column < dstFrameWidth; column+=2) {
      *(u++) = BLACK_U;
      *(y++) = BLACK_Y;
      *(v++) = BLACK_V;
      *(y++) = BLACK_Y;
    }
    for(column = 0; column < dstFrameWidth; column+=2) {
      *(y++) = BLACK_Y;
      *(y++) = BLACK_Y;
    }
  }
  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  return TRUE;
}

#if  defined (__GNUC__) || defined (__sun)
#ifndef P_MACOSX
/*
 * Convert a MJPEG Buffer to one plane pixel format (RGB24, BGR24, GRAY)
 * image need to be same size.
 */
bool PStandardColourConverter::MJPEGtoXXXSameSize(const BYTE *mjpeg, BYTE *rgb, int format)
{
  BYTE *components[1];

  components[0] = rgb;

  if (jdec == NULL) {
    jdec = tinyjpeg_init();
    if (jdec == NULL) {
      PTRACE(2, "PColCnv\tJpeg error: Can't allocate memory");
	    return FALSE;
    }
    tinyjpeg_set_flags(jdec, TINYJPEG_FLAGS_MJPEG_TABLE);
  }
  tinyjpeg_set_components(jdec, components, 1);
  if (tinyjpeg_parse_header(jdec, mjpeg, srcFrameBytes) < 0) {
     PTRACE(2, "PColCnv\tJpeg error: " << tinyjpeg_get_errorstring(jdec));
     return FALSE;
  }
  if (tinyjpeg_decode(jdec, format) < 0) {
     PTRACE(2, "PColCnv\tJpeg error: " << tinyjpeg_get_errorstring(jdec));
     return FALSE;
  }

  return TRUE;
}


bool PStandardColourConverter::MJPEGtoXXX(const BYTE *mjpeg,
                                          BYTE *output_data,
                                          PINDEX *bytesReturned,
					  int format)
{
  if ((srcFrameWidth | dstFrameWidth | srcFrameHeight | dstFrameHeight) & 0xf) {
    PTRACE(2,"PColCnv\tError MJPEG decoder need width and height to be a multiple of 16");
    return FALSE;
  }

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) {
     if (MJPEGtoXXXSameSize(mjpeg, output_data, format) == FALSE)
       return FALSE;
  } else {
     /* not efficient (convert then resize) */
     /* TODO: */
     return FALSE;
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  
  return TRUE;

}

PSTANDARD_COLOUR_CONVERTER(MJPEG,RGB24)
{
  return MJPEGtoXXX(srcFrameBuffer, dstFrameBuffer, bytesReturned, TINYJPEG_FMT_RGB24);
}

PSTANDARD_COLOUR_CONVERTER(MJPEG,BGR24)
{
  return MJPEGtoXXX(srcFrameBuffer, dstFrameBuffer, bytesReturned, TINYJPEG_FMT_BGR24);
}

PSTANDARD_COLOUR_CONVERTER(MJPEG,Grey)
{
  return MJPEGtoXXX(srcFrameBuffer, dstFrameBuffer, bytesReturned, TINYJPEG_FMT_GREY);
}

PSTANDARD_COLOUR_CONVERTER(JPEG,RGB24)
{
  return MJPEGtoXXX(srcFrameBuffer, dstFrameBuffer, bytesReturned, TINYJPEG_FMT_RGB24);
}

PSTANDARD_COLOUR_CONVERTER(JPEG,BGR24)
{
  return MJPEGtoXXX(srcFrameBuffer, dstFrameBuffer, bytesReturned, TINYJPEG_FMT_BGR24);
}

PSTANDARD_COLOUR_CONVERTER(JPEG,Grey)
{
  return MJPEGtoXXX(srcFrameBuffer, dstFrameBuffer, bytesReturned, TINYJPEG_FMT_GREY);
}

/*
 * Convert a MJPEG Buffer to YUV420P
 * image need to be same size.
 */
bool PStandardColourConverter::MJPEGtoYUV420PSameSize(const BYTE *mjpeg, BYTE *yuv420p)
{
  BYTE *components[4];

  int npixels = srcFrameWidth * srcFrameHeight;

  components[0] = yuv420p;
  components[1] = yuv420p + npixels;
  components[2] = yuv420p + npixels + npixels/4;

  if (jdec == NULL) {
    PTRACE(2, "PColCnv\tJpeg: Allocating Jpeg decoder private structure");
    jdec = tinyjpeg_init();
    if (jdec == NULL) {
      PTRACE(2, "PColCnv\tJpeg error: Can't allocate memory");
      return FALSE;
    }
    tinyjpeg_set_flags(jdec, TINYJPEG_FLAGS_MJPEG_TABLE);
  }
  tinyjpeg_set_components(jdec, components, 4);
  if (tinyjpeg_parse_header(jdec, mjpeg, srcFrameBytes) < 0) {
     PTRACE(2, "PColCnv\tJpeg error: " << tinyjpeg_get_errorstring(jdec));
     return FALSE;
  }
  if (tinyjpeg_decode(jdec, TINYJPEG_FMT_YUV420P) < 0) {
     PTRACE(2, "PColCnv\tJpeg error: " << tinyjpeg_get_errorstring(jdec));
     return FALSE;
  }

  return TRUE;
}

/*
 * Convert a MJPEG or JPEG buffer to YUV420P
 *
 */
bool PStandardColourConverter::MJPEGtoYUV420P(const BYTE *mjpeg,
                                              BYTE *yuv420p,
					      PINDEX *bytesReturned)
{
  if ((srcFrameWidth | dstFrameWidth | srcFrameHeight | dstFrameHeight) & 0xf) {
    PTRACE(2,"PColCnv\tError in MJPEG to YUV420P converter, All size need to be a multiple of 16.");
    return FALSE;
  }

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) {

     PTRACE(2,"PColCnv\tMJPEG to YUV420P\n");
     if (MJPEGtoYUV420PSameSize(mjpeg, yuv420p) == FALSE)
       return FALSE;

  } else {
     /* Very not efficient */
     unsigned int frameBytes = srcFrameWidth * srcFrameHeight * 3 / 2;
     BYTE *intermed = intermediateFrameStore.GetPointer(frameBytes);
     MJPEGtoYUV420PSameSize(mjpeg, intermed);
     ResizeYUV420P(intermed, yuv420p);
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  
  return TRUE;
}

/*
 * MJPEG to YUV420P
 */
PSTANDARD_COLOUR_CONVERTER(MJPEG,YUV420P)
{
  return MJPEGtoYUV420P(srcFrameBuffer, dstFrameBuffer, bytesReturned);
}

/*
 * JPEG to YUV420P
 */
PSTANDARD_COLOUR_CONVERTER(JPEG,YUV420P)
{
  return MJPEGtoYUV420P(srcFrameBuffer, dstFrameBuffer, bytesReturned);
}


#endif // P_MACOSX
#endif // __GNUC__

#ifdef _MSC_VER
#pragma warning(default : 4244)
#endif

#endif

// End Of File ///////////////////////////////////////////////////////////////
