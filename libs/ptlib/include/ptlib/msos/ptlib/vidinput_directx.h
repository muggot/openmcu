/*
 * vidinput_directx.h
 *
 * Classes to support streaming video input (grabbing) and output.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2007 Luc Saillard <luc@saillard.org>
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
 * The Initial Developer of the Original Code is Luc Saillard <luc@saillard.org>
 *
 * Contributor(s): Matthias Schneider <ma30002000@yahoo.de>
 */

#ifndef __DIRECTSHOW_H__
#define __DIRECTSHOW_H__


#include <ptlib.h>
#include <ptbuildopts.h>

#ifdef P_DIRECTSHOW
#include <ptlib/videoio.h>
#include <ptlib/vconvert.h>
#include <ptlib/pluginmgr.h>
#include <ptclib/delaychan.h>

#include <mingw_dshow_port.h>
#include <windows.h>
#include <ddraw.h>
#include <dshow.h>
#include <uuids.h>
#include <control.h>

#ifdef __MINGW32__
static void DeleteMediaType(AM_MEDIA_TYPE *pmt);

// 30323449-0000-0010-8000-00AA00389B71            MEDIASUBTYPE_None
static GUID MEDIASUBTYPE_I420 = {0x30323449, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};

#undef INTERFACE
#define INTERFACE ISampleGrabberCB
DECLARE_INTERFACE_(ISampleGrabberCB, IUnknown)
{
    STDMETHOD_(HRESULT, SampleCB)(THIS_ double, IMediaSample *) PURE;
    STDMETHOD_(HRESULT, BufferCB)(THIS_ double, BYTE *, long) PURE;
};

#undef INTERFACE
#define INTERFACE ISampleGrabber
DECLARE_INTERFACE_(ISampleGrabber,IUnknown)
{
    STDMETHOD_(HRESULT, SetOneShot)(THIS_ BOOL) PURE;
    STDMETHOD_(HRESULT, SetMediaType)(THIS_ AM_MEDIA_TYPE *) PURE;
    STDMETHOD_(HRESULT, GetConnectedMediaType)(THIS_ AM_MEDIA_TYPE *) PURE;
    STDMETHOD_(HRESULT, SetBufferSamples)(THIS_ BOOL) PURE;
    STDMETHOD_(HRESULT, GetCurrentBuffer)(THIS_ long *, long *) PURE;
    STDMETHOD_(HRESULT, GetCurrentSample)(THIS_ IMediaSample *) PURE;
    STDMETHOD_(HRESULT, SetCallback)(THIS_ ISampleGrabberCB *, long) PURE;
};
extern "C" {
    extern const CLSID CLSID_SampleGrabber;
    extern const IID IID_ISampleGrabber;
    extern const CLSID CLSID_SampleGrabberCB;
    extern const IID IID_ISampleGrabberCB;
    extern const CLSID CLSID_NullRenderer;
};

#else
#include <Mtype.h>
#include <qedit.h>
#endif

/**This class defines a video input device.
 */
class PVideoInputDevice_DirectShow : public PVideoInputDevice
{
  PCLASSINFO(PVideoInputDevice_DirectShow, PVideoInputDevice);

  public:
    /** Create a new video input device.
     */
    PVideoInputDevice_DirectShow();

    /**Close the video input device on destruction.
      */
    ~PVideoInputDevice_DirectShow();

    /**Open the device given the device name.
      */
    BOOL Open(
      const PString & deviceName,   /// Device name to open
      BOOL startImmediate = TRUE    /// Immediately start device
    );

    /** Is the device a camera, and obtain video
     */
    static PStringList GetInputDeviceNames();

    PStringList GetDeviceNames() const
      { return GetInputDeviceNames(); }


    /**Determine if the device is currently open.
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

    /**Set the colour format to be used.
       Note that this function does not do any conversion. If it returns TRUE
       then the video device does the colour format in native mode.

       To utilise an internal converter use the SetColourFormatConverter()
       function.

       Default behaviour sets the value of the colourFormat variable and then
       returns TRUE.
    */
    BOOL SetColourFormat(
      const PString & colourFormat // New colour format for device.
    );

    /**Set the video frame rate to be used on the device.

       Default behaviour sets the value of the frameRate variable and then
       returns TRUE.
    */
    BOOL SetFrameRate(
      unsigned rate  /// Frames  per second
    );

    /**Set the frame size to be used.

       Note that devices may not be able to produce the requested size, and
       this function will fail.  See SetFrameSizeConverter().

       Default behaviour sets the frameWidth and frameHeight variables and
       returns TRUE.
    */
    BOOL SetFrameSize(
      unsigned width,   /// New width of frame
      unsigned height   /// New height of frame
    );

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

    /**Get the brightness of the image. 0xffff-Very bright. -1 is unknown.
     */
    int GetBrightness();

    /**Set brightness of the image. 0xffff-Very bright.
     */
    BOOL SetBrightness(unsigned newBrightness);


    /**Get the whiteness of the image. 0xffff-Very white. -1 is unknown.
     */
    int GetWhiteness();

    /**Set whiteness of the image. 0xffff-Very white.
     */
    BOOL SetWhiteness(unsigned newWhiteness);


    /**Get the colour of the image. 0xffff-lots of colour. -1 is unknown.
     */
    int GetColour();

    /**Set colour of the image. 0xffff-lots of colour.
     */
    BOOL SetColour(unsigned newColour);


    /**Get the contrast of the image. 0xffff-High contrast. -1 is unknown.
     */
    int GetContrast();

    /**Set contrast of the image. 0xffff-High contrast.
     */
    BOOL SetContrast(unsigned newContrast);


    /**Get the hue of the image. 0xffff-High hue. -1 is unknown.
     */
    int GetHue();

    /**Set hue of the image. 0xffff-High hue.
     */
    BOOL SetHue(unsigned newHue);

    /**Return whiteness, brightness, colour, contrast and hue in one call.
     */
    BOOL GetParameters(
      int *whiteness,
      int *brightness,
      int *colour,
      int *contrast,
      int *hue
    );

    /**Try all known video formats & see which ones are accepted by the video driver
     */
    BOOL TestAllFormats() { return TRUE; }



  protected:

    HRESULT Initialize_Interfaces();
    BOOL SetControlCommon(long control, int newValue);
    BOOL GetControlCommon(long control, int *newValue);
    BOOL SetFormat(const PString &format, int width, int height, int fps);
    BOOL GetDefaultFormat();
    BOOL ListSupportedFormats();
    BOOL InitialiseCapture();
    void FlipVertical(BYTE *buffer);

    char *tempFrame;			/* Buffer used when a converter is needed */
    unsigned int frameBytes;		/* Size of a frame in Bytes */
    int  capturing_duration;
    BOOL flipVertical;
    PAdaptiveDelay m_pacing;
    
    BOOL          isCapturingNow;

    IBaseFilter   * pSrcFilter;
    IBaseFilter   * pGrabberFilter;
    IBaseFilter   * pNullFilter;
    IGraphBuilder * pGraph;
    IMediaControl * pMC;
    IMediaEventEx * pME;
    ICaptureGraphBuilder2 * pCapture;
    ISampleGrabber * pGrabber;

};

#endif /*P_DIRECTSHOW*/

#endif  /* __DIRECTSHOW_H__ */
