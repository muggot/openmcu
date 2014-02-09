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
 * $Revision: 26686 $
 * $Author: rjongbloed $
 * $Date: 2011-11-23 20:22:20 -0600 (Wed, 23 Nov 2011) $
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

    bool SBGGR8toYUV420P(
     const BYTE * srgb,
      BYTE * rgb,
      PINDEX * bytesReturned
    );
    bool SBGGR8toRGB(
      const BYTE * srgb,
      BYTE * rgb,
      PINDEX * bytesReturned
    ) const;
    void GreytoYUV420PSameSize(
      const BYTE * rgb,
      BYTE * yuv
    ) const;
    void GreytoYUV420PWithCrop(
      const BYTE * rgb,
      BYTE * yuv
    ) const;
    bool GreytoYUV420P(
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
    void RGBtoYUV420PWithCrop(
      const BYTE * rgb,
      BYTE * yuv,
      unsigned rgbIncrement,
      unsigned redOffset,
      unsigned blueOffset
    ) const;
    bool RGBtoYUV420P(
      const BYTE * rgb,
      BYTE * yuv,
      PINDEX * bytesReturned,
      unsigned rgbIncrement,
      unsigned redOffset,
      unsigned blueOffset
    );
    bool YUV420PtoRGB(
      const BYTE * yuv,
      BYTE * rgb,
      PINDEX * bytesReturned,
      unsigned rgbIncrement,
      unsigned redOffset,
      unsigned blueOffset
    ) const;
    BOOL YUV420PtoRGB565(
      const BYTE * yuv,
      BYTE * rgb,
      PINDEX * bytesReturned
    ) const;
    bool SwapRedAndBlue(
      const BYTE * src,
      BYTE * dst,
      PINDEX * bytesReturned,
      unsigned srcIncrement,
      unsigned dstIncrement
    ) const;
    void UYVY422WithCrop(
      const BYTE *src_uyvy,
      BYTE *dst_uyvy
    ) const;
    void YUV422WithCrop(
      const BYTE * src,
      BYTE * dest,
      bool centred
    ) const;
    void UYVY422toYUV420PSameSize(
      const BYTE *uyvy,
      BYTE *yuv420p
    ) const;
    void UYVY422toYUV420PWithCrop(
      const BYTE *uyvy,
      BYTE *yuv420p
    ) const;
    void YUY2toYUV420PSameSize(
      const BYTE *yuy2,
      BYTE *yuv420p
    ) const;
    void YUY2toYUV420PWithGrow(
      const BYTE *yuy2,
      BYTE *yuv420p
    ) const;
    void YUY2toYUV420PWithShrink(
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


#define BLACK_Y 0
#define BLACK_U 128
#define BLACK_V 128


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


PColourConverter::PColourConverter(const PString & srcColourFmt,
                                   const PString & dstColourFmt,
                                   unsigned width,
                                   unsigned height)
{
  Construct(PVideoFrameInfo(width, height, srcColourFmt),
            PVideoFrameInfo(width, height, dstColourFmt));
}

PColourConverter::PColourConverter(const PVideoFrameInfo & src,
                                   const PVideoFrameInfo & dst)
{
  Construct(src, dst);
}


void PColourConverter::Construct(const PVideoFrameInfo & src, const PVideoFrameInfo & dst)
{
#ifndef P_MACOSX
  jdec = NULL;
#endif

  srcColourFormat = src.GetColourFormat();
  src.GetFrameSize(srcFrameWidth, srcFrameHeight);
  srcFrameBytes = src.CalculateFrameBytes();

  dstColourFormat = dst.GetColourFormat();
  dst.GetFrameSize(dstFrameWidth, dstFrameHeight);
  dstFrameBytes = dst.CalculateFrameBytes();

  resizeMode = dst.GetResizeMode();

  verticalFlip = false;

  PTRACE(4,"PColCnv\tPColourConverter constructed: " << src << " -> " << dst);
}


void PColourConverter::PrintOn(ostream & strm) const
{
  strm << srcColourFormat << ':' << srcFrameWidth << 'x' << srcFrameHeight << "->"
       << dstColourFormat << ':' << dstFrameWidth << 'x' << dstFrameHeight << '/'
       << resizeMode;
}


BOOL PColourConverter::SetFrameSize(unsigned width, unsigned height)
{
  bool ok1 = SetSrcFrameSize(width, height);
  bool ok2 = SetDstFrameSize(width, height);
  PTRACE(2,"PColCnv\tSetFrameSize: " << width << 'x' << height
         << (ok1 && ok2 ? " OK" : " Failed"));
  return ok1 && ok2;
}


BOOL PColourConverter::SetSrcFrameInfo(const PVideoFrameInfo & info)
{
  if (!PAssert(info.GetColourFormat() != GetSrcColourFormat(), "Cannot change colour format"))
    return false;

  unsigned w, h;
  return info.GetFrameSize(w, h) && SetSrcFrameSize(w, h);
}


BOOL PColourConverter::SetDstFrameInfo(const PVideoFrameInfo & info)
{
  if (!PAssert(info.GetColourFormat() != GetDstColourFormat(), "Cannot change colour format"))
    return false;

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
    return true;

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

BOOL PColourConverter::SetDstFrameSize(unsigned width, unsigned height, BOOL bScale)
{
  if (!SetDstFrameSize(width, height))
    return false;

  if (bScale)
    SetResizeMode(PVideoFrameInfo::eScale);
  else
    SetResizeMode(PVideoFrameInfo::eCropCentre);

  return true;
}

BOOL PColourConverter::GetSrcFrameSize(unsigned &width, unsigned &height) const
{
  width = srcFrameWidth;
  height = srcFrameHeight;
  return true;
}


BOOL PColourConverter::GetDstFrameSize(unsigned &width, unsigned &height) const
{
  width = dstFrameWidth;
  height = dstFrameHeight;
  return true;
}


BOOL PColourConverter::ConvertInPlace(BYTE * frameBuffer,
                                      PINDEX * bytesReturned,
                                      BOOL noIntermediateFrame)
{
  if (Convert(frameBuffer, frameBuffer, bytesReturned))
    return true;

  if (noIntermediateFrame) {
    PTRACE(2,"PColCnv\tError in ConvertInPlace, no intermediate frame available.");
    return false;
  }

  BYTE * intermediate = intermediateFrameStore.GetPointer(dstFrameBytes);
  PINDEX bytes;
  if (!Convert(frameBuffer, intermediate, &bytes))
    return false;

  memcpy(frameBuffer, intermediate, bytes);
  if (bytesReturned != NULL)
    *bytesReturned = bytes;
  return true;
}


#define RGB2Y(r, g, b, y) \
  y=(BYTE)(((int)257*(r)  +(int)504*(g) +(int)98*(b))/1000)

#define RGB2YUV(r, g, b, y, cb, cr) \
  RGB2Y(r, g, b, y); \
  cb=(BYTE)((-148*(r)  -291*(g) +439*(b))/1000 + 128); \
  cr=(BYTE)(( 439*(r)  -368*(g) - 71*(b))/1000 + 128)


void PColourConverter::RGBtoYUV(unsigned   r, unsigned   g, unsigned   b,
                                unsigned & y, unsigned & u, unsigned & v)
{
  RGB2YUV(r, g, b, y, u, v);
}


void PColourConverter::RGBtoYUV(unsigned r, unsigned g, unsigned b,
                                BYTE   & y, BYTE   & u, BYTE   & v)
{
  RGB2YUV(r, g, b, y, u, v);
}


// Consider a YUV420P image of 4x4 pixels.
//
// A plane of Y values    A B C D
//                        E F G H
//                        I J K L 
//                        M N O P
//
// A plane of U values    1 . 2 . 
//                           . . . .
//                           3 . 4 .
//                        . . . .
//
// A plane of V values    1 . 2 .
//                           . . . .
//                        3 . 4 .
//                        . . . .
// 
// YUV420P is stored as all Y (w*h), then U (w*h/4), then V
//   thus, a 4x4 image requires 24 bytes of storage.
//
// Grow and Shrink utilise the Variable Duty Cycle algorithm, currently
// no interpolation is used, just pixel dropping or doubling

static void GrowYUV420P(unsigned srcX, unsigned srcY, unsigned srcWidth, unsigned srcHeight,
                        unsigned srcFrameWidth, const BYTE * srcYUV,
                        unsigned dstX, unsigned dstY, unsigned dstWidth, unsigned dstHeight,
                        unsigned dstFrameWidth, BYTE * dstYUV)
{
  const BYTE * srcPtr = srcYUV + srcY * srcFrameWidth + srcX;
  BYTE * dstPtr = dstYUV + dstY * dstFrameWidth + dstX;

  unsigned repeatRow = 0;
  for (unsigned y = 0; y < srcHeight; y++) {

    const BYTE * srcPixel = srcPtr;
    BYTE * dstPixel = dstPtr;
    unsigned repeatPixel = 0;

    for (unsigned x = 0; x < srcWidth; x++) {
      do {
        *dstPixel++ = *srcPixel;
        repeatPixel += srcWidth;
      } while (repeatPixel < dstWidth);
      repeatPixel -= dstWidth;

      srcPixel++;
    }

    BYTE * repeatPtr = dstPtr;

    repeatRow += srcHeight;
    while (repeatRow < dstHeight) {
      dstPtr += dstFrameWidth;
      memcpy(dstPtr, repeatPtr, dstWidth);
      repeatRow += srcHeight;
    }
    repeatRow -= dstHeight;

    srcPtr += srcFrameWidth;
    dstPtr += dstFrameWidth;
  }
}


static void ShrinkYUV420P(unsigned srcX, unsigned srcY, unsigned srcWidth, unsigned srcHeight,
                          unsigned srcFrameWidth, const BYTE * srcYUV,
                          unsigned dstX, unsigned dstY, unsigned dstWidth, unsigned dstHeight,
                          unsigned dstFrameWidth, BYTE * dstYUV)
{
  const BYTE * srcPtr = srcYUV + srcY * srcFrameWidth + srcX;
  BYTE * dstPtr = dstYUV + dstY * dstFrameWidth + dstX;

  unsigned repeatRow = 0;
  for (unsigned y = 0; y < dstHeight; y++) {

    const BYTE * srcPixel = srcPtr;
    BYTE * dstPixel = dstPtr;
    unsigned repeatPixel = 0;

    for (unsigned x = 0; x < dstWidth; x++) {
      *dstPixel++ = *srcPixel;

      do {
        srcPixel++;
        repeatPixel += dstWidth;
      } while (repeatPixel < srcWidth);
      repeatPixel -= srcWidth;
    }

    do {
      srcPtr += srcFrameWidth;
      repeatRow += dstHeight;
    } while (repeatRow < srcHeight);
    repeatRow -= srcHeight;

    dstPtr += dstFrameWidth;
  }
}


static void CropYUV420P(unsigned srcX, unsigned srcY, unsigned srcWidth, unsigned srcHeight,
                          unsigned srcFrameWidth, const BYTE * srcYUV,
                          unsigned dstX, unsigned dstY, unsigned , unsigned ,
                          unsigned dstFrameWidth, BYTE * dstYUV)
{
  const BYTE * srcPtr = srcYUV + srcY * srcFrameWidth + srcX;
  BYTE * dstPtr = dstYUV + dstY * dstFrameWidth + dstX;
  for (unsigned y = 0; y < srcHeight; y++) {
    memcpy(dstPtr, srcPtr, srcWidth);
    srcPtr += srcFrameWidth;
    dstPtr += dstFrameWidth;
  }
}


static bool ValidateDimensions(unsigned srcFrameWidth, unsigned srcFrameHeight, unsigned dstFrameWidth, unsigned dstFrameHeight)
{
  if (srcFrameWidth == 0 || dstFrameWidth == 0 || srcFrameHeight == 0 || dstFrameHeight == 0) {
    PTRACE(2,"PColCnv\tDimensions cannot be zero: "
           << srcFrameWidth << 'x' << srcFrameHeight << " -> " << dstFrameWidth << 'x' << dstFrameHeight);
    return false;
  }

  if ((srcFrameWidth | dstFrameWidth | srcFrameHeight | dstFrameHeight) & 1) {
    PTRACE(2,"PColCnv\tDimensions must be even: "
           << srcFrameWidth << 'x' << srcFrameHeight << " -> " << dstFrameWidth << 'x' << dstFrameHeight);
    return false;
  }

  if (srcFrameWidth <= dstFrameWidth && srcFrameHeight <= dstFrameHeight)
    return true;

  if (srcFrameWidth >= dstFrameWidth && srcFrameHeight >= dstFrameHeight)
    return true;

  PTRACE(2,"PColCnv\tCannot do one dimension shrinking and the other one growing: "
         << srcFrameWidth << 'x' << srcFrameHeight << " -> " << dstFrameWidth << 'x' << dstFrameHeight);
  return false;
}


bool PColourConverter::CopyYUV420P(unsigned srcX, unsigned srcY, unsigned srcWidth, unsigned srcHeight,
                                   unsigned srcFrameWidth, unsigned srcFrameHeight, const BYTE * srcYUV,
                                   unsigned dstX, unsigned dstY, unsigned dstWidth, unsigned dstHeight,
                                   unsigned dstFrameWidth, unsigned dstFrameHeight, BYTE * dstYUV,
                                   PVideoFrameInfo::ResizeMode resizeMode)
{
  if (srcX == 0 && srcY == 0 && dstX == 0 && dstY == 0 &&
      srcWidth == dstWidth && srcHeight == dstHeight &&
      srcFrameWidth == dstFrameWidth && srcFrameHeight == dstFrameHeight &&
      srcWidth == srcFrameWidth && srcHeight == srcFrameHeight) {
    memcpy(dstYUV, srcYUV, srcFrameWidth*srcFrameHeight*3/2);
    return true;
  }

  if (srcFrameWidth == 0 || srcFrameHeight == 0 ||
      dstFrameWidth == 0 || dstFrameHeight == 0 ||
      !ValidateDimensions(srcWidth, srcHeight, dstWidth, dstHeight) ||
      srcX + srcWidth > srcFrameWidth ||
      srcY + srcHeight > srcFrameHeight ||
      dstX + dstWidth > dstFrameWidth ||
      dstY + dstHeight > dstFrameHeight) {
    PAssertAlways(PInvalidParameter);
    return false;
  }

  void (*rowFunction)(unsigned srcX, unsigned srcY, unsigned srcWidth, unsigned srcHeight,
                      unsigned srcFrameWidth, const BYTE * srcYUV,
                      unsigned dstX, unsigned dstY, unsigned dstWidth, unsigned dstHeight,
                      unsigned dstFrameWidth, BYTE * dstYUV) = CropYUV420P;

  switch (resizeMode) {
    case PVideoFrameInfo::eScale :
      if (srcWidth > dstWidth)
        rowFunction = ShrinkYUV420P;
      else if (srcWidth < dstWidth)
        rowFunction = GrowYUV420P;
      break;

    case PVideoFrameInfo::eCropTopLeft :
      if (srcWidth < dstWidth) {
        FillYUV420P(dstX + srcWidth, dstY, dstWidth - srcWidth, dstHeight, dstFrameWidth, dstFrameHeight, dstYUV, 0, 0, 0);
        if (srcHeight < dstHeight)
          FillYUV420P(dstX, dstY + srcHeight, dstWidth, dstHeight - srcHeight, dstFrameWidth, dstFrameHeight, dstYUV, 0, 0, 0);
        dstWidth = srcWidth;
        dstHeight = srcHeight;
      }
      else {
        srcWidth = dstWidth;
        srcHeight = dstHeight;
      }
      break;

    case PVideoFrameInfo::eCropCentre :
      if (srcWidth < dstWidth) {
        unsigned deltaX = (dstWidth - srcWidth)/2;
        unsigned deltaY = (dstHeight - srcHeight)/2;
        FillYUV420P(dstX, dstY, deltaX, dstHeight, dstFrameWidth, dstFrameHeight, dstYUV, 0, 0, 0);
        FillYUV420P(dstX+deltaX+srcWidth, dstY, deltaX, dstHeight, dstFrameWidth, dstFrameHeight, dstYUV, 0, 0, 0);
        if (srcHeight < dstHeight) {
          FillYUV420P(dstX+deltaX, dstY, srcWidth, deltaY, dstFrameWidth, dstFrameHeight, dstYUV, 0, 0, 0);
          FillYUV420P(dstX+deltaX, dstY+deltaY+srcHeight, srcWidth, deltaY, dstFrameWidth, dstFrameHeight, dstYUV, 0, 0, 0);
        }
        dstX += deltaX;
        dstY += deltaY;
        dstWidth = srcWidth;
        dstHeight = srcHeight;
      }
      else {
        srcX += (srcWidth - dstWidth)/2;
        srcY += (srcHeight - dstHeight)/2;
        srcWidth = dstWidth;
        srcHeight = dstHeight;
      }
      break;

    default :
      PAssertAlways(PInvalidParameter);
      return false;
  }

  // Copy plane Y
  rowFunction(srcX, srcY, srcWidth, srcHeight, srcFrameWidth, srcYUV,
              dstX, dstY, dstWidth, dstHeight, dstFrameWidth, dstYUV);

  srcYUV += srcFrameWidth*srcFrameHeight;
  dstYUV += dstFrameWidth*dstFrameHeight;

  // U & V planes half size
  srcX /= 2;
  srcY /= 2;
  dstX /= 2;
  dstY /= 2;
  srcWidth /= 2;
  srcHeight /= 2;
  dstWidth /= 2;
  dstHeight /= 2;
  srcFrameWidth /= 2;
  srcFrameHeight /= 2;
  dstFrameWidth /= 2;
  dstFrameHeight /= 2;

  // Copy plane U
  rowFunction(srcX, srcY, srcWidth, srcHeight, srcFrameWidth, srcYUV,
              dstX, dstY, dstWidth, dstHeight, dstFrameWidth, dstYUV);

  srcYUV += srcFrameWidth*srcFrameHeight;
  dstYUV += dstFrameWidth*dstFrameHeight;

  // Copy plane V
  rowFunction(srcX, srcY, srcWidth, srcHeight, srcFrameWidth, srcYUV,
              dstX, dstY, dstWidth, dstHeight, dstFrameWidth, dstYUV);
  return true;
}


bool PColourConverter::FillYUV420P(unsigned x, unsigned y, int width, int height,
                                   unsigned frameWidth, unsigned frameHeight, BYTE * yuv,
                                   unsigned r, unsigned g, unsigned b)
{
  if (frameWidth == 0 || frameHeight == 0 || x + width > frameWidth || y + height > frameHeight) {
    PAssertAlways(PInvalidParameter);
    return false;
  }

  unsigned Y, Cb, Cr;
  PColourConverter::RGBtoYUV(r, g, b, Y, Cb, Cr);

  x &= 0xfffffffe; // Make sure is even

  int offset       = ( y * frameWidth ) + x;
  int colourOffset = ( (y * frameWidth) >> 2) + (x >> 1);

  unsigned char * Yptr  = yuv + offset;
  unsigned char * CbPtr = yuv + (frameWidth * frameHeight) + colourOffset;
  unsigned char * CrPtr = yuv + (frameWidth * frameHeight) + (frameWidth * frameHeight/4)  + colourOffset;

  int halfRectWidth  = width/2;
  int halfFrameWidth = frameWidth/2;
  
  for (int dy = 0; dy < height; dy += 2) {
    memset(Yptr, Y, width);
    Yptr += frameWidth;
    memset(Yptr, Y, width);
    Yptr += frameWidth;

    memset(CbPtr, Cb, halfRectWidth);
    memset(CrPtr, Cr, halfRectWidth);

    CbPtr += halfFrameWidth;
    CrPtr += halfFrameWidth;
  }

  return true;
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
    PTRACE(2,"PColCnv\tCannot do synonym conversion, source and destination size not equal: " << *this);
    return false;
  }

  if (verticalFlip) {
    PINDEX rowSize = dstFrameBytes/srcFrameHeight;
    if (rowSize*srcFrameHeight != dstFrameBytes) {
      PTRACE(2,"PColCnv\tCannot do synonym conversion, frame does not have equal scan lines: " << *this);
      return false;
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

  return true;
}

///////////////////////////////////////////////////////////////////////////////

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
void PStandardColourConverter::GreytoYUV420PWithCrop(const BYTE * grey, BYTE * yuv) const
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


bool PStandardColourConverter::GreytoYUV420P(const BYTE * grey, BYTE * yuv, PINDEX * bytesReturned) const
{
  if (grey == yuv) {
    PTRACE(2,"PColCnv\tCannot do in-place conversion, not implemented.");
    return false;
  }

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) 
    GreytoYUV420PSameSize(grey, yuv);
  else
    GreytoYUV420PWithCrop(grey, yuv);

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return true;
}


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
void PStandardColourConverter::RGBtoYUV420PWithCrop(const BYTE * rgb,
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


bool PStandardColourConverter::RGBtoYUV420P(const BYTE * rgb,
                                            BYTE * yuv,
                                            PINDEX * bytesReturned,
                                            unsigned rgbIncrement,
                                            unsigned redOffset,
                                            unsigned blueOffset)
{
  if (rgb == yuv) {
    PTRACE(2,"PColCnv\tCannot do in-place conversion, not implemented.");
    return false;
  }

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) 
    RGBtoYUV420PSameSize(rgb, yuv, rgbIncrement, redOffset, blueOffset);
  else if (resizeMode == PVideoFrameInfo::eCropTopLeft)
    RGBtoYUV420PWithCrop(rgb, yuv, rgbIncrement, redOffset, blueOffset);
  else {
    unsigned intermediateSize = PVideoFrameInfo::CalculateFrameBytes(srcFrameWidth, srcFrameHeight, dstColourFormat);
    RGBtoYUV420PSameSize(rgb, intermediateFrameStore.GetPointer(intermediateSize), rgbIncrement, redOffset, blueOffset);
    CopyYUV420P(0, 0, srcFrameWidth, srcFrameHeight, srcFrameWidth, srcFrameHeight, intermediateFrameStore,
                0, 0, dstFrameWidth, dstFrameHeight, dstFrameWidth, dstFrameHeight, yuv, resizeMode);
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return true;
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
 * NOTE: This algorithm works only if the width and the height are even numbers.
 */
void PStandardColourConverter::YUY2toYUV420PWithGrow(const BYTE *yuy2, BYTE *yuv420p) const
{
  const BYTE *s;
  BYTE *y, *u, *v;
  unsigned int x, h;  
  unsigned int npixels = dstFrameWidth * dstFrameHeight;

  s = yuy2;
  y = yuv420p;
  u = yuv420p + npixels;
  v = u + npixels/4;

  // dest is bigger than the source. No subsampling.
  // Place the src in the middle of the destination.
  unsigned int yOffset = (dstFrameHeight - srcFrameHeight)/2;
  unsigned int xOffset = (dstFrameWidth - srcFrameWidth)/2;
  unsigned int bpixels = yOffset * dstFrameWidth;

  /* Top border */
  memset(y, BLACK_Y, bpixels);   y += bpixels;
  memset(u, BLACK_U, bpixels/4); u += bpixels/4;
  memset(v, BLACK_V, bpixels/4); v += bpixels/4;

  for (h=0; h<srcFrameHeight; h+=2)
  {
    /* Left border */
    memset(y, BLACK_Y, xOffset);   y += xOffset;
    memset(u, BLACK_U, xOffset/2); u += xOffset/2;
    memset(v, BLACK_V, xOffset/2); v += xOffset/2;

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
    memset(y, BLACK_Y, xOffset);        y += xOffset;
    memset(u, BLACK_U, xOffset/2);        u += xOffset/2;
    memset(v, BLACK_V, xOffset/2);        v += xOffset/2;
  }
  memset(y, BLACK_Y, bpixels);
  memset(u, BLACK_U, bpixels/4);
  memset(v, BLACK_V, bpixels/4);
}


void PStandardColourConverter::YUY2toYUV420PWithShrink(const BYTE *yuy2, BYTE *yuv420p) const
{
  const BYTE *s;
  BYTE *y, *u, *v;
  unsigned int x, h;  
  unsigned int npixels = dstFrameWidth * dstFrameHeight;

  s = yuy2;
  y = yuv420p;
  u = yuv420p + npixels;
  v = u + npixels/4;

  // source is bigger than the destination
  // We are doing linear interpolation to find value.
  // Note this algorithm only works if dst is an even multple of src
  unsigned int dx = srcFrameWidth/dstFrameWidth;
  unsigned int dy = srcFrameHeight/dstFrameHeight;
  unsigned int fy, fx;

  for (fy=0, h=0; h<dstFrameHeight; h+=2, fy+=dy*2)
  {
    /* Copy the first line with U&V */
    unsigned int yy = fy;
    unsigned int yy2 = (fy+dy);
    const unsigned char *line1, *line2;
    unsigned char lastU, lastV;

    line1 = s + (yy*2*srcFrameWidth);
    line2 = s + (yy2*2*srcFrameWidth);
    lastU = line1[1];
    lastV = line1[3];
    for (fx=0, x=0; x<dstFrameWidth; x+=2, fx+=dx*2)
    {
      unsigned int xx = fx*2;
      *y++ = line1[xx];
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
      
      xx = (fx+dx);
      *y++ = line1[xx];
      if ( (xx&2) == 0)
        lastU = (line1[xx+1] + line2[xx+1])/2;
      else
        lastV = (line1[xx+3] + line2[xx+3])/2;
    }

    /* Copy the second line without U&V */
    for (fx=0, x=0; x<dstFrameWidth; x++, fx+=dx)
    {
      unsigned int xx = fx*2;
      *y++ = line2[xx];
    }
  } /* end of for (fy=0, h=0; h<dstFrameHeight; h+=2, fy+=dy*2) */
}


PSTANDARD_COLOUR_CONVERTER(YUY2,YUV420P)
{
  if (!ValidateDimensions(srcFrameWidth, srcFrameHeight, dstFrameWidth, dstFrameHeight))
    return false;

  if (dstFrameWidth == srcFrameWidth)
    YUY2toYUV420PSameSize(srcFrameBuffer, dstFrameBuffer);
  else if (dstFrameWidth < srcFrameWidth)
    YUY2toYUV420PWithShrink(srcFrameBuffer, dstFrameBuffer);
  else
    YUY2toYUV420PWithGrow(srcFrameBuffer, dstFrameBuffer);

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return true;
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

void PStandardColourConverter::YUV422WithCrop(const BYTE * src, BYTE * dest, bool centred) const
{
  DWORD *result = (DWORD *)dest;
  DWORD black   = (DWORD)(BLACK_U<<24) + (BLACK_Y<<16) + (BLACK_U<<8) + BLACK_Y;
  unsigned maxIndex    = dstFrameWidth*dstFrameHeight/2;

  if ( (dstFrameWidth*dstFrameHeight) > (srcFrameWidth*srcFrameHeight) ) { 
    for (unsigned i = 0; i < maxIndex; i++) 
      *result++ = black;

    //dest is bigger than the source. No subsampling.
    //Place the src in the middle of the destination.
    unsigned yOffset = centred ? dstFrameHeight - srcFrameHeight : 0;
    unsigned xOffset = centred ? dstFrameWidth - srcFrameWidth : 0;

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
    return true;

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) 
    memcpy(dstFrameBuffer,srcFrameBuffer,srcFrameWidth*srcFrameHeight*2);
  else
    YUV422WithCrop(srcFrameBuffer, dstFrameBuffer, resizeMode == PVideoFrameInfo::eCropCentre);

  return true;
}


PSTANDARD_COLOUR_CONVERTER(YUV420P,YUV420P)
{
  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  
  if (srcFrameBuffer == dstFrameBuffer) {
    if (srcFrameWidth == dstFrameWidth && srcFrameHeight == dstFrameHeight) 
      return true;
    if(srcFrameWidth < dstFrameWidth || srcFrameHeight < dstFrameHeight) {
      PTRACE(2,"PColCnv\tCannot do in place conversion, increasing image size.");
      return false;
    }
  }

  return CopyYUV420P(0, 0, srcFrameWidth, srcFrameHeight, srcFrameWidth, srcFrameHeight, srcFrameBuffer,
                     0, 0, dstFrameWidth, dstFrameHeight, dstFrameWidth, dstFrameHeight, dstFrameBuffer,
                     resizeMode);
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
  if (!ValidateDimensions(srcFrameWidth, srcFrameHeight, dstFrameWidth, dstFrameHeight))
    return false;

  if (dstFrameWidth == srcFrameWidth)
    YUY2toYUV420PSameSize(srcFrameBuffer, dstFrameBuffer);
  else if (dstFrameWidth < srcFrameWidth)
    YUY2toYUV420PWithShrink(srcFrameBuffer, dstFrameBuffer);
  else
    YUY2toYUV420PWithGrow(srcFrameBuffer, dstFrameBuffer);

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return true;
}


#define LIMIT(x) (unsigned char) ((x > 255) ? 255 : ((x < 0) ? 0 : x ))
static inline int clip(int a, int limit) {
  return a<limit?a:limit;
}

bool PStandardColourConverter::SBGGR8toYUV420P(const BYTE * src, BYTE * dst, PINDEX * bytesReturned)
{
#define USE_SBGGR8_NATIVE 1 // set to 0 to use the double conversion algorithm (Bayer->RGB->YUV420P)
  
#if USE_SBGGR8_NATIVE
if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight))
{
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
}
else
#endif //USE_SBGGR8_NATIVE
{
  // shortest but less efficient (one malloc per conversion!)
  BYTE * tempDest=(BYTE*)malloc(3*srcFrameWidth*srcFrameHeight);
  SBGGR8toRGB(src, tempDest, NULL);
  bool r = RGBtoYUV420P(tempDest, dst, bytesReturned, 3, 0, 2);
  free(tempDest);
  return r;
}
}

bool PStandardColourConverter::SBGGR8toRGB(const BYTE * src,
                                           BYTE       * dst,
                                           PINDEX     * bytesReturned) const
{
  if (src == dst) {
    PTRACE(2,"PColCnv\tCannot do in-place conversion, not implemented.");
    return false;
  }

  if (verticalFlip) {
    PTRACE(2,"PColCnv\tCannot do vertical flip, not implemented.");
    return false;
  }

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

  return true;
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
bool PStandardColourConverter::YUV420PtoRGB(const BYTE * srcFrameBuffer,
                                            BYTE * dstFrameBuffer,
                                            PINDEX * bytesReturned,
                                            unsigned rgbIncrement,
                                            unsigned redOffset,
                                            unsigned blueOffset) const
{
  if (srcFrameBuffer == dstFrameBuffer) {
    PTRACE(2,"PColCnv\tCannot do in-place conversion, not implemented.");
    return false;
  }

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
    dstPixpos[0] = dstPixpos[2];
    dstPixpos[1] = dstPixpos[3];
    dstPixpos[2] = 0;
    dstPixpos[3] = rgbIncrement;
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
        int yvalue = yplane[srcPixpos[p]];

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

  return true;
}


BOOL PStandardColourConverter::YUV420PtoRGB565(const BYTE * srcFrameBuffer,
                                            BYTE * dstFrameBuffer,
                                            PINDEX * bytesReturned) const
{
  if (srcFrameBuffer == dstFrameBuffer) {
    PTRACE(2,"PColCnv\tCannot do in-place conversion, not implemented.");
    return false;
  }

  static const unsigned rgbIncrement = 2;

  unsigned height = PMIN(srcFrameHeight, dstFrameHeight)&(UINT_MAX-1); // Must be even
  unsigned width = PMIN(srcFrameWidth, dstFrameWidth)&(UINT_MAX-1);

  unsigned    yplanesize = srcFrameWidth*srcFrameHeight;
  const BYTE *yplane     = srcFrameBuffer;        // 1 byte Y (luminance) for each pixel
  const BYTE *uplane     = yplane+yplanesize;     // 1 byte U for a block of 4 pixels
  const BYTE *vplane     = uplane+(yplanesize/4); // 1 byte V for a block of 4 pixels

  BYTE * dstScanLine   = dstFrameBuffer;

  unsigned int srcPixpos[4] = { 0, 1, srcFrameWidth, srcFrameWidth + 1 };
  unsigned int dstPixpos[4] = { 0, rgbIncrement, dstFrameWidth*rgbIncrement, (dstFrameWidth+1)*rgbIncrement };

  if (verticalFlip) {
    dstScanLine += (dstFrameHeight - 2) * dstFrameWidth * rgbIncrement;
    dstPixpos[0] = dstFrameWidth*rgbIncrement;
    dstPixpos[1] = (dstFrameWidth +1)*rgbIncrement;
    dstPixpos[2] = 0;
    dstPixpos[3] = 1*rgbIncrement;
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
        WORD r16,g16,b16;
        WORD color;
        WORD *colorptr=NULL;

        r16 = ( (LIMIT(r)) >> 3) & 0x001f;
        g16 = ( (LIMIT(g)) >> 2) & 0x003f;
        b16 = ( (LIMIT(b)) >> 3) & 0x001f;
        color = ((r16 << 11) & (0xf800))
          | ((g16 << 5 ) & (0x07e0))
          | ((b16 ) & (0x001f));
        colorptr = (WORD *)(rgpPtr);
        *colorptr = color;
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

  return true;
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


PSTANDARD_COLOUR_CONVERTER(YUV420P,RGB565)
{
  return YUV420PtoRGB565(srcFrameBuffer, dstFrameBuffer, bytesReturned);
}

PSTANDARD_COLOUR_CONVERTER(YUV420P,RGB16)
{
  return YUV420PtoRGB565(srcFrameBuffer, dstFrameBuffer, bytesReturned);
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

bool PStandardColourConverter::SwapRedAndBlue(const BYTE * srcFrameBuffer,
                                              BYTE * dstFrameBuffer,
                                              PINDEX * bytesReturned,
                                              unsigned srcIncrement,
                                              unsigned dstIncrement) const
{
  if ((dstFrameWidth != srcFrameWidth) || (dstFrameHeight != srcFrameHeight)) {
    PTRACE(2,"PColCnv\tCannot do different sized RGB swap, not implemented.");
    return false;
  }

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
  return true;
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
  if ((dstFrameWidth != srcFrameWidth) || (dstFrameHeight != srcFrameHeight)) {
    PTRACE(2,"PColCnv\tCannot do RGB 24/32 conversion on different sized image, not implemented.");
    return false;
  }

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
  return true;
}


PSTANDARD_COLOUR_CONVERTER(RGB32,RGB24)
{
  if ((dstFrameWidth != srcFrameWidth) || (dstFrameHeight != srcFrameHeight)) {
    PTRACE(2,"PColCnv\tCannot do RGB 32/24 conversion on different sized image, not implemented.");
    return false;
  }

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
  return true;
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
  if (srcFrameBuffer == dstFrameBuffer) {
    PTRACE(2,"PColCnv\tCannot do in-place conversion, not implemented.");
    return false;
  }

  if ((dstFrameWidth != srcFrameWidth) || (dstFrameHeight != srcFrameHeight)) {
    PTRACE(2,"PColCnv\tCannot do YUV 420/411 conversion on different sized image, not implemented.");
    return false;
  }

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
  
  return true;
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
  if (srcFrameBuffer == dstFrameBuffer) {
    PTRACE(2,"PColCnv\tCannot do in-place conversion, not implemented.");
    return false;
  }

  if ((dstFrameWidth != srcFrameWidth) || (dstFrameHeight != srcFrameHeight)) {
    PTRACE(2,"PColCnv\tCannot do YUV 411/420 conversion on different sized image, not implemented.");
    return false;
  }

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

  return true;
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
      return true;
    if(srcFrameWidth < dstFrameWidth || srcFrameHeight < dstFrameHeight) {
      PTRACE(2,"PColCnv\tCannot do in place conversion, increasing image size.");
      return false;
    }
  }

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) 
    memcpy(dstFrameBuffer,srcFrameBuffer,srcFrameWidth*srcFrameHeight*2);
  else
    UYVY422WithCrop(srcFrameBuffer, dstFrameBuffer);

  return true;
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
void PStandardColourConverter::UYVY422WithCrop(const BYTE *src_uyvy, BYTE *dst_uyvy) const
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
void PStandardColourConverter::UYVY422toYUV420PWithCrop(const BYTE *uyvy, BYTE *yuv420p) const
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
     memset(y, BLACK_Y, bpixels);        y += bpixels;
     memset(u, BLACK_U, bpixels/4);        u += bpixels/4;
     memset(v, BLACK_V, bpixels/4);        v += bpixels/4;

     for (h=0; h<srcFrameHeight; h+=2)
      {
        /* Left border */
        memset(y, BLACK_Y, xOffset);        y += xOffset;
        memset(u, BLACK_U, xOffset/2);        u += xOffset/2;
        memset(v, BLACK_V, xOffset/2);        v += xOffset/2;

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
        memset(y, BLACK_Y, xOffset);        y += xOffset;
        memset(u, BLACK_U, xOffset/2);        u += xOffset/2;
        memset(v, BLACK_V, xOffset/2);        v += xOffset/2;
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
  if (srcFrameBuffer == dstFrameBuffer) {
    PTRACE(2,"PColCnv\tCannot do in-place conversion, not implemented.");
    return false;
  }

  if ((srcFrameWidth==dstFrameWidth) && (srcFrameHeight==dstFrameHeight))
    UYVY422toYUV420PSameSize(srcFrameBuffer, dstFrameBuffer);
  else
    UYVY422toYUV420PWithCrop(srcFrameBuffer, dstFrameBuffer);

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;

  return true;
}

PSTANDARD_COLOUR_CONVERTER(UYV444,YUV420P)
{
  if (srcFrameBuffer == dstFrameBuffer) {
    PTRACE(2,"PColCnv\tCannot do in-place conversion, not implemented.");
    return false;
  }

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
  return true;
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

  struct jdec_private *jdec;
 
  components[0] = rgb;
 
  jdec = tinyjpeg_init();

  if (jdec == NULL) {
     PTRACE(2, "PColCnv\tJpeg error: Can't allocate memory");
     return false;
  }
  tinyjpeg_set_flags(jdec, TINYJPEG_FLAGS_MJPEG_TABLE);
  tinyjpeg_set_components(jdec, components, 1);
  if (tinyjpeg_parse_header(jdec, mjpeg, srcFrameBytes) < 0) {
     PTRACE(2, "PColCnv\tJpeg error: " << tinyjpeg_get_errorstring(jdec));
     free(jdec);
     return false;
  }
  if (tinyjpeg_decode(jdec, format) < 0) {
     PTRACE(2, "PColCnv\tJpeg error: " << tinyjpeg_get_errorstring(jdec));
     free(jdec);
     return false;
  }

  free(jdec);
  return true;
}


bool PStandardColourConverter::MJPEGtoXXX(const BYTE *mjpeg,
                                          BYTE *output_data,
                                          PINDEX *bytesReturned,
                                          int format)
{
  if ((srcFrameWidth | dstFrameWidth | srcFrameHeight | dstFrameHeight) & 0xf) {
    PTRACE(2,"PColCnv\tError MJPEG decoder need width and height to be a multiple of 16");
    return false;
  }

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) {
     if (MJPEGtoXXXSameSize(mjpeg, output_data, format) == false)
       return false;
  } else {
     /* not efficient (convert then resize) */
     /* TODO: */
     return false;
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  
  return true;

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
  struct jdec_private *jdec;

  int npixels = srcFrameWidth * srcFrameHeight;

  components[0] = yuv420p;
  components[1] = yuv420p + npixels;
  components[2] = yuv420p + npixels + npixels/4;
  components[3] = NULL;
 
  jdec = tinyjpeg_init();

  if (jdec == NULL) {
    PTRACE(2, "PColCnv\tJpeg error: Can't allocate memory");
    return false;
  }
  tinyjpeg_set_flags(jdec, TINYJPEG_FLAGS_MJPEG_TABLE);
  tinyjpeg_set_components(jdec, components, 4);
  if (tinyjpeg_parse_header(jdec, mjpeg, srcFrameBytes) < 0) {
     PTRACE(2, "PColCnv\tJpeg error: " << tinyjpeg_get_errorstring(jdec));
     free(jdec);
     return false;
  }
  if (tinyjpeg_decode(jdec, TINYJPEG_FMT_YUV420P) < 0) {
     PTRACE(2, "PColCnv\tJpeg error: " << tinyjpeg_get_errorstring(jdec));
     free(jdec);
     return false;
  }

  free(jdec);
  return true;
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
    return false;
  }

  if ((srcFrameWidth == dstFrameWidth) && (srcFrameHeight == dstFrameHeight)) {

     PTRACE(2,"PColCnv\tMJPEG to YUV420P\n");
     if (MJPEGtoYUV420PSameSize(mjpeg, yuv420p) == false)
       return false;

  } else {
     /* Very not efficient */
     unsigned int frameBytes = srcFrameWidth * srcFrameHeight * 3 / 2;
     BYTE *intermed = intermediateFrameStore.GetPointer(frameBytes);
     MJPEGtoYUV420PSameSize(mjpeg, intermed);
     CopyYUV420P(0, 0, srcFrameWidth, srcFrameHeight, srcFrameWidth, srcFrameHeight, intermed,
                 0, 0, dstFrameWidth, dstFrameHeight, dstFrameWidth, dstFrameHeight, yuv420p,
                     resizeMode);
  }

  if (bytesReturned != NULL)
    *bytesReturned = dstFrameBytes;
  
  return true;
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
