/*
 * vfw.cxx
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
 * Contributor(s): ______________________________________.
 *                 Walter H Whitlock (twohives@nc.rr.com)
 *
 * $Revision: 27514 $
 * $Author: rjongbloed $
 * $Date: 2012-04-25 21:09:35 -0500 (Wed, 25 Apr 2012) $
 */

#include <ptlib.h>

#if P_VIDEO

#include <ptlib/videoio.h>
#include <ptlib/vconvert.h>
#include <ptlib/pluginmgr.h>
#include <ptclib/delaychan.h>

#if P_VFW_CAPTURE

#ifdef _MSC_VER
#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "gdi32.lib")
#endif


#include <vfw.h>

#ifdef __MINGW32__

#define VHDR_DONE       0x00000001
#define VHDR_KEYFRAME   0x00000008

extern "C" {
#ifdef _MSC_VER
BOOL VFWAPI capGetDriverDescriptionA (WORD wDriverIndex, LPSTR lpszName,
              int cbName, LPSTR lpszVer, int cbVer);
#endif
}

#endif // __MINGW32



/**This class defines a video input device.
 */
class PVideoInputDevice_VideoForWindows : public PVideoInputDevice
{
  PCLASSINFO(PVideoInputDevice_VideoForWindows, PVideoInputDevice);

  public:
    /** Create a new video input device.
     */
    PVideoInputDevice_VideoForWindows();

    /**Close the video input device on destruction.
      */
    ~PVideoInputDevice_VideoForWindows();


    /** Is the device a camera, and obtain video
     */
    static PStringList GetInputDeviceNames();

    virtual PStringList GetDeviceNames() const
      { return GetInputDeviceNames(); }

    /**Open the device given the device name.
      */
    virtual BOOL Open(
      const PString & deviceName,   /// Device name to open
      BOOL startImmediate = true    /// Immediately start device
    );

    /**Determine if the device is currently open.
      */
    virtual BOOL IsOpen();

    /**Close the device.
      */
    virtual BOOL Close();

    /**Retrieve a list of Device Capabilities
      */
    bool GetDeviceCapabilities(
      Capabilities * /*caps*/         ///< List of supported capabilities
    );
    static BOOL GetDeviceCapabilities(const PString & deviceName, Capabilities * capabilities);

    /**Start the video device I/O.
      */
    virtual BOOL Start();

    /**Stop the video device I/O capture.
      */
    virtual BOOL Stop();

    /**Determine if the video device I/O capture is in progress.
      */
    virtual BOOL IsCapturing();

    /**Set the colour format to be used.
       Note that this function does not do any conversion. If it returns true
       then the video device does the colour format in native mode.

       To utilise an internal converter use the SetColourFormatConverter()
       function.

       Default behaviour sets the value of the colourFormat variable and then
       returns true.
    */
    virtual BOOL SetColourFormat(
      const PString & colourFormat // New colour format for device.
    );

    /**Set the video frame rate to be used on the device.

       Default behaviour sets the value of the frameRate variable and then
       returns true.
    */
    virtual BOOL SetFrameRate(
      unsigned rate  /// Frames  per second
    );

    /**Set the frame size to be used.

       Note that devices may not be able to produce the requested size, and
       this function will fail.  See SetFrameSizeConverter().

       Default behaviour sets the frameWidth and frameHeight variables and
       returns true.
    */
    virtual BOOL SetFrameSize(
      unsigned width,   /// New width of frame
      unsigned height   /// New height of frame
    );

    /**Get the maximum frame size in bytes.

       Note a particular device may be able to provide variable length
       frames (eg motion JPEG) so will be the maximum size of all frames.
      */
    virtual PINDEX GetMaxFrameBytes();

    /**Grab a frame, after a delay as specified by the frame rate.
      */
    virtual BOOL GetFrameData(
      BYTE * buffer,                 /// Buffer to receive frame
      PINDEX * bytesReturned = NULL  /// OPtional bytes returned.
    );

    /**Grab a frame. Do not delay according to the current frame rate parameter.
      */
    virtual BOOL GetFrameDataNoDelay(
      BYTE * buffer,                 /// Buffer to receive frame
      PINDEX * bytesReturned = NULL  /// OPtional bytes returned.
    );


    virtual bool SetCaptureMode(unsigned mode);
    virtual int GetCaptureMode() const { return useVideoMode; }


  protected:
    BOOL VerifyHardwareFrameSize(unsigned width, unsigned height);

    PDECLARE_NOTIFIER(PThread, PVideoInputDevice_VideoForWindows, HandleCapture);

    static LRESULT CALLBACK ErrorHandler(HWND hWnd, int id, LPCSTR err);
    LRESULT HandleError(int id, LPCSTR err);
    static LRESULT CALLBACK VideoHandler(HWND hWnd, LPVIDEOHDR vh);
    LRESULT HandleVideo(LPVIDEOHDR vh);
    BOOL InitialiseCapture();

    PThread     * captureThread;
    PSyncPoint    threadStarted;

    HWND          hCaptureWindow;
    PMutex        operationMutex;

    PSyncPoint    frameAvailable;
    bool          useVideoMode;
    LPBYTE        lastFrameData;
    unsigned      lastFrameSize;
    PMutex        lastFrameMutex;
    bool          isCapturingNow;
    PAdaptiveDelay m_Pacing;
};


///////////////////////////////////////////////////////////////////////////////

class PCapStatus : public CAPSTATUS
{
  public:
    PCapStatus(HWND hWnd);
    BOOL IsOK()
       { return uiImageWidth != 0; }
};

///////////////////////////////////////////////////////////////////////////////

static struct FormatTableEntry { 
  const char * colourFormat; 
  WORD  bitCount;
  BOOL  negHeight; // MS documentation suggests that negative height will request
                  // top down scan direction from video driver
                  // HOWEVER, not all drivers honor this request
  DWORD compression; 
} FormatTable[] = {
  { "BGR32",   32, TRUE,  BI_RGB },
  { "BGR24",   24, TRUE,  BI_RGB },
  { "Grey",     8, TRUE,  BI_RGB },
  { "Gray",     8, TRUE,  BI_RGB },

  { "RGB565",  16, TRUE,  BI_BITFIELDS },
  { "RGB555",  15, TRUE,  BI_BITFIELDS },

  // http://support.microsoft.com/support/kb/articles/q294/8/80.asp
  { "YUV420P", 12, FALSE, mmioFOURCC('I','Y','U','V') },
  { "IYUV",    12, FALSE, mmioFOURCC('I','Y','U','V') }, // Synonym for IYUV
  { "I420",    12, FALSE, mmioFOURCC('I','4','2','0') }, // Synonym for IYUV
  { "YV12",    12, FALSE, mmioFOURCC('Y','V','1','2') }, // same as IYUV except that U and V planes are switched
  { "YUV422",  16, FALSE, mmioFOURCC('Y','U','Y','2') },
  { "YUY2",    16, FALSE, mmioFOURCC('Y','U','Y','2') },
  { "UYVY",    16, FALSE, mmioFOURCC('U','Y','V','Y') }, // Like YUY2 except for ordering
  { "YVYU",    16, FALSE, mmioFOURCC('Y','V','Y','U') }, // Like YUY2 except for ordering
  { "YVU9",    16, FALSE, mmioFOURCC('Y','V','U','9') },
  { "MJPEG",   12, FALSE, mmioFOURCC('M','J','P','G') },
  { NULL },
};


static struct {
    unsigned device_width, device_height;
} winTestResTable[] = {
    { 176, 144 },
    { 352, 288 },
    { 320, 240 },
    { 160, 120 },
    { 640, 480 },
    { 704, 576 },
    {1024, 768 },
};

///////////////////////////////////////////////////////////////////////////////

class PVideoDeviceBitmap : PBYTEArray
{
  public:
    // the following method is replaced by PVideoDeviceBitmap(HWND hWnd, WORD bpp)
    // PVideoDeviceBitmap(unsigned width, unsigned height, const PString & fmt);
    //returns object with gray color pallet if needed for 8 bpp formats
    PVideoDeviceBitmap(HWND hWnd, WORD bpp);
    // does not build color pallet
    PVideoDeviceBitmap(HWND hWnd); 
    // apply video format to capture device
    BOOL ApplyFormat(HWND hWnd, const FormatTableEntry & formatTableEntry);

    BITMAPINFO * operator->() const 
    { return (BITMAPINFO *)theArray; }
};

PVideoDeviceBitmap::PVideoDeviceBitmap(HWND hCaptureWindow)
{
  PINDEX sz = capGetVideoFormatSize(hCaptureWindow);
  SetSize(sz);
  if (!capGetVideoFormat(hCaptureWindow, theArray, sz)) { 
    PTRACE(1, "PVidInp\tcapGetVideoFormat(hCaptureWindow) failed - " << ::GetLastError());
    SetSize(0);
    return;
  }
}

PVideoDeviceBitmap::PVideoDeviceBitmap(HWND hCaptureWindow, WORD bpp)
{
  PINDEX sz = capGetVideoFormatSize(hCaptureWindow);
  SetSize(sz);
  if (!capGetVideoFormat(hCaptureWindow, theArray, sz)) { 
    PTRACE(1, "PVidInp\tcapGetVideoFormat(hCaptureWindow) failed - " << ::GetLastError());
    SetSize(0);
    return;
  }

  if (8 == bpp && bpp != ((BITMAPINFO*)theArray)->bmiHeader.biBitCount) {
    SetSize(sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256);
    RGBQUAD * bmiColors = ((BITMAPINFO*)theArray)->bmiColors;
    for (int i = 0; i < 256; i++)
      bmiColors[i].rgbBlue  = bmiColors[i].rgbGreen = bmiColors[i].rgbRed = (BYTE)i;
  }
  ((BITMAPINFO*)theArray)->bmiHeader.biBitCount = bpp;
}

BOOL PVideoDeviceBitmap::ApplyFormat(HWND hWnd, const FormatTableEntry & formatTableEntry)
{
  // NB it is necessary to set the biSizeImage value appropriate to frame size
  // assume bmiHeader.biBitCount has already been set appropriatly for format
  BITMAPINFO & bmi = *(BITMAPINFO*)theArray;
  bmi.bmiHeader.biPlanes = 1;

  int height = bmi.bmiHeader.biHeight<0 ? -bmi.bmiHeader.biHeight : bmi.bmiHeader.biHeight;
  bmi.bmiHeader.biSizeImage = height*4*((bmi.bmiHeader.biBitCount * bmi.bmiHeader.biWidth + 31)/32);

  // set .biHeight according to .negHeight value
  if (formatTableEntry.negHeight)
    bmi.bmiHeader.biHeight = -height; 

#if PTRACING
  PTimeInterval startTime = PTimer::Tick();
#endif

  if (capSetVideoFormat(hWnd, theArray, GetSize())) {
    PTRACE(3, "PVidInp\tcapSetVideoFormat succeeded: "
            << PString(formatTableEntry.colourFormat) << ' '
            << bmi.bmiHeader.biWidth << "x" << bmi.bmiHeader.biHeight
            << " sz=" << bmi.bmiHeader.biSizeImage << " time=" << (PTimer::Tick() - startTime));
    return true;
  }

  if (formatTableEntry.negHeight) {
    bmi.bmiHeader.biHeight = height; 
    if (capSetVideoFormat(hWnd, theArray, GetSize())) {
      PTRACE(3, "PVidInp\tcapSetVideoFormat succeeded: "
              << PString(formatTableEntry.colourFormat) << ' '
              << bmi.bmiHeader.biWidth << "x" << bmi.bmiHeader.biHeight
              << " sz=" << bmi.bmiHeader.biSizeImage << " time=" << (PTimer::Tick() - startTime));
      return true;
    }
  }

  PTRACE(1, "PVidInp\tcapSetVideoFormat failed: "
          << (formatTableEntry.colourFormat != NULL ? formatTableEntry.colourFormat : "NO-COLOUR-FORMAT") << ' '
          << bmi.bmiHeader.biWidth << "x" << bmi.bmiHeader.biHeight
          << " sz=" << bmi.bmiHeader.biSizeImage << " time=" << (PTimer::Tick() - startTime)
          << " - lastError=" << ::GetLastError());
  return false;
}

///////////////////////////////////////////////////////////////////////////////

PCapStatus::PCapStatus(HWND hWnd)
{
  memset(this, 0, sizeof(*this));
  if (capGetStatus(hWnd, this, sizeof(*this)))
    return;

  PTRACE(1, "PVidInp\tcapGetStatus: failed - " << ::GetLastError());
}


///////////////////////////////////////////////////////////////////////////////
// PVideoInputDevice_VideoForWindows

PCREATE_VIDINPUT_PLUGIN(VideoForWindows);

PVideoInputDevice_VideoForWindows::PVideoInputDevice_VideoForWindows()
{
  captureThread = NULL;
  hCaptureWindow = NULL;
  lastFrameSize = 0;
  isCapturingNow = false;

  useVideoMode    = false;
  lastFrameData   = NULL;
}


PVideoInputDevice_VideoForWindows::~PVideoInputDevice_VideoForWindows()
{
  if(lastFrameData)
    delete[] lastFrameData;
  Close();
}


bool PVideoInputDevice_VideoForWindows::SetCaptureMode(unsigned mode)
{
  useVideoMode = mode != 0;

  // Do nothing if we are currently capturing (we don't support switching between picture- and video-mode during a capture).
  if(IsCapturing())
    return false;

  // Set the callback function for complete frames
  BOOL result;
  if (useVideoMode)
    result = capSetCallbackOnVideoStream(hCaptureWindow, VideoHandler);
  else
    result = capSetCallbackOnFrame(hCaptureWindow, VideoHandler);

  if (!result) {
    lastError = ::GetLastError();
    PTRACE(1, "PVidInp\tFailed to set callback on VfW - " << lastError);
    return false;
  }

  CAPTUREPARMS parms;
  memset(&parms, 0, sizeof(parms));
  if (!capCaptureGetSetup(hCaptureWindow, &parms, sizeof(parms))) {
    lastError = ::GetLastError();
    PTRACE(1, "PVidInp\tcapCaptureGetSetup: failed - " << lastError);
    return false;
  }

  // For video mode we must tell VfW to work in a separate background thread, or our application will lock otherwise.
  if (useVideoMode) {
    parms.fYield = TRUE;
    parms.dwIndexSize = 324000;
  }
  else {
    parms.fYield = FALSE;
    parms.dwIndexSize = 0;
  }

  if (!capCaptureSetSetup(hCaptureWindow, &parms, sizeof(parms))) {
    lastError = ::GetLastError();
    PTRACE(1, "PVidInp\tcapCaptureSetSetup: failed - " << lastError);
    return false;
  }

  return true;
}


BOOL PVideoInputDevice_VideoForWindows::Open(const PString & devName, BOOL startImmediate)
{
  Close();

  operationMutex.Wait();

  deviceName = devName;

  captureThread = PThread::Create(PCREATE_NOTIFIER(HandleCapture), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "VidIn");

  operationMutex.Signal();
  threadStarted.Wait();

  PWaitAndSignal mutex(operationMutex);

  if (hCaptureWindow == NULL) {
    delete captureThread;
    captureThread = NULL;
    return false;
  }

  if (startImmediate)
    return Start();

  return true;
}


BOOL PVideoInputDevice_VideoForWindows::IsOpen() 
{
  return hCaptureWindow != NULL;
}


BOOL PVideoInputDevice_VideoForWindows::Close()
{
  PWaitAndSignal mutex(operationMutex);

  if (!IsOpen())
    return false;
 
  Stop();

  ::PostThreadMessage(captureThread->GetThreadId(), WM_QUIT, 0, 0L);

  // Some brain dead drivers may hang so we provide a timeout.
  if (!captureThread->WaitForTermination(5000))
  {
      // Two things may happen if we are forced to terminate the capture thread:
      // 1. As the VIDCAP window is associated to that thread the OS itself will 
      //    close the window and release the driver
      // 2. the driver will not be released and we will not have video until we 
      //    terminate the process
      // Any of the two ios better than just hanging
      captureThread->Terminate();
      hCaptureWindow = NULL;
      PTRACE(1, "PVidInp\tCapture thread failed to stop. Terminated");
  }

  delete captureThread;
  captureThread = NULL;

  return true;
}


BOOL PVideoInputDevice_VideoForWindows::GetDeviceCapabilities(const PString & deviceName,
                                                                  Capabilities * capabilities)
{
  PVideoInputDevice_VideoForWindows instance;
  return instance.Open(deviceName, false) && instance.GetDeviceCapabilities(capabilities);
}


bool PVideoInputDevice_VideoForWindows::GetDeviceCapabilities(Capabilities * caps)
{
  for (PINDEX prefFormatIdx = 0; FormatTable[prefFormatIdx].colourFormat != NULL; prefFormatIdx++) {
    PVideoDeviceBitmap bi(hCaptureWindow, FormatTable[prefFormatIdx].bitCount); 
    bi->bmiHeader.biCompression = FormatTable[prefFormatIdx].compression;
    for (PINDEX prefResizeIdx = 0; prefResizeIdx < PARRAYSIZE(winTestResTable); prefResizeIdx++) {
      bi->bmiHeader.biWidth = winTestResTable[prefResizeIdx].device_width;
      bi->bmiHeader.biHeight = winTestResTable[prefResizeIdx].device_height;
      if (bi.ApplyFormat(hCaptureWindow, FormatTable[prefFormatIdx]) && caps != NULL) {
        PVideoFrameInfo frameInfo;
        frameInfo.SetFrameSize(winTestResTable[prefResizeIdx].device_width,
                               winTestResTable[prefResizeIdx].device_height);
        caps->framesizes.push_back(frameInfo);
      }
    }
  }
  return !caps->framesizes.empty();
}


BOOL PVideoInputDevice_VideoForWindows::Start()
{
  PWaitAndSignal mutex(operationMutex);

  if (IsCapturing())
    return true;

  if (!useVideoMode) {
    isCapturingNow = true;
    return capGrabFrameNoStop(hCaptureWindow);
  }

  if (capCaptureSequenceNoFile(hCaptureWindow)) {
    PCapStatus status(hCaptureWindow);
    isCapturingNow = status.fCapturingNow;

    // As initializing the camera takes some time, and video-mode runs in a background thread, we need to wait for the first frame here.
    // Otherwise "GetFrameDataNoDelay" might time-out.
    frameAvailable.Wait();
    return isCapturingNow;
  }

  lastError = ::GetLastError();
  PTRACE(1, "PVidInp\tcapCaptureSequenceNoFile: failed - " << lastError);
  return false;
}


BOOL PVideoInputDevice_VideoForWindows::Stop()
{
  PWaitAndSignal mutex(operationMutex);

  if (!IsCapturing())
    return false;
  isCapturingNow = false;

  // If using the picture mode, we just need to wait for the very next frame ...
  if (!useVideoMode)
    return IsOpen() && frameAvailable.Wait(1000);

  // ... otherwise we need to explicitely stop capturing.
  if (capCaptureStop(hCaptureWindow))
    return true;

  lastError = ::GetLastError();
  PTRACE(1, "PVidInp\tcapCaptureStop: failed - " << lastError);
  return false;
}


BOOL PVideoInputDevice_VideoForWindows::IsCapturing()
{
  return isCapturingNow;
}


BOOL PVideoInputDevice_VideoForWindows::SetColourFormat(const PString & colourFmt)
{
  PWaitAndSignal mutex(operationMutex);

  if (!IsOpen())
    return PVideoDevice::SetColourFormat(colourFmt); // Not open yet, just set internal variables

  BOOL running = IsCapturing();
  if (running)
    Stop();

  PString oldFormat = colourFormat;

  if (!PVideoDevice::SetColourFormat(colourFmt))
    return false;

  PINDEX i = 0;
  while (FormatTable[i].colourFormat != NULL && !(colourFmt *= FormatTable[i].colourFormat))
    i++;

  PVideoDeviceBitmap bi(hCaptureWindow, FormatTable[i].bitCount);

  if (FormatTable[i].colourFormat != NULL)
    bi->bmiHeader.biCompression = FormatTable[i].compression;
  else if (colourFmt.GetLength() == 4)
    bi->bmiHeader.biCompression = mmioFOURCC(colourFmt[0],colourFmt[1],colourFmt[2],colourFmt[3]);
  else {
    bi->bmiHeader.biCompression = 0xffffffff; // Indicate invalid colour format
    PVideoDevice::SetColourFormat(oldFormat);
    return false;
  }

  // set frame width and height
  bi->bmiHeader.biWidth = frameWidth;
  bi->bmiHeader.biHeight = frameHeight;
  if (!bi.ApplyFormat(hCaptureWindow, FormatTable[i])) {
    lastError = ::GetLastError();
    PVideoDevice::SetColourFormat(oldFormat);
    return false;
  }

  // Didn't do top down, tell everything we are up side down
  nativeVerticalFlip = FormatTable[i].negHeight && bi->bmiHeader.biHeight > 0;

  if (running)
    return Start();

  return true;
}


BOOL PVideoInputDevice_VideoForWindows::SetFrameRate(unsigned rate)
{
  PWaitAndSignal mutex(operationMutex);

  if (!PVideoDevice::SetFrameRate(rate))
    return false;

  if (!IsOpen())
    return true; // Not open yet, just set internal variables

  BOOL running = IsCapturing();
  if (running)
    Stop();

  CAPTUREPARMS parms;
  memset(&parms, 0, sizeof(parms));

  if (!capCaptureGetSetup(hCaptureWindow, &parms, sizeof(parms))) {
    lastError = ::GetLastError();
    PTRACE(1, "PVidInp\tcapCaptureGetSetup: failed - " << lastError);
    return false;
  }

  // keep current (default) framerate if 0==frameRate   
  if (0 != frameRate)
    parms.dwRequestMicroSecPerFrame = 1000000 / frameRate;

  parms.fMakeUserHitOKToCapture = false;
  parms.wPercentDropForError = 100;
  parms.fCaptureAudio = false;
  parms.fAbortLeftMouse = false;
  parms.fAbortRightMouse = false;
  parms.fLimitEnabled = false;

  if (!capCaptureSetSetup(hCaptureWindow, &parms, sizeof(parms))) {
    lastError = ::GetLastError();
    PTRACE(1, "PVidInp\tcapCaptureSetSetup: failed - " << lastError);
    return false;
  }
    
  if (running)
    return Start();

  return true;
}


BOOL PVideoInputDevice_VideoForWindows::SetFrameSize(unsigned width, unsigned height)
{
  PWaitAndSignal mutex(operationMutex);

  if (!IsOpen())
    return PVideoDevice::SetFrameSize(width, height); // Not open yet, just set internal variables

  BOOL running = IsCapturing();
  if (running)
    Stop();

  PVideoDeviceBitmap bi(hCaptureWindow); 
  PTRACE(5, "PVidInp\tChanging frame size from "
         << bi->bmiHeader.biWidth << 'x' << bi->bmiHeader.biHeight << " to " << width << 'x' << height);

  PINDEX i = 0;
  while (FormatTable[i].colourFormat != NULL && !(colourFormat *= FormatTable[i].colourFormat))
    i++;

  bi->bmiHeader.biWidth = width;
  bi->bmiHeader.biHeight = height;
  if (!bi.ApplyFormat(hCaptureWindow, FormatTable[i])) {
    lastError = ::GetLastError();
    return false;
  }

  // Didn't do top down, tell everything we are up side down
  nativeVerticalFlip = FormatTable[i].negHeight && bi->bmiHeader.biHeight > 0;

  // verify that the driver really took the frame size
  if (!VerifyHardwareFrameSize(width, height)) 
    return false; 

  // frameHeight must be positive regardlesss of what the driver says
  if (0 > (int)height) 
    height = (unsigned)-(int)height;

  if (!PVideoDevice::SetFrameSize(width, height))
    return false;

  if (running)
    return Start();

  return true;
}


//return true if absolute value of height reported by driver 
//  is equal to absolute value of current frame height AND
//  width reported by driver is equal to current frame width
BOOL PVideoInputDevice_VideoForWindows::VerifyHardwareFrameSize(unsigned width, unsigned height)
{
  PCapStatus status(hCaptureWindow);

  if (!status.IsOK())
    return false;

  if (width != status.uiImageWidth)
    return false;

  if (0 > (int)height)
    height = (unsigned)-(int)height;

  if (0 > (int)status.uiImageHeight)
    status.uiImageHeight = (unsigned)-(int)status.uiImageHeight;

  return (height == status.uiImageHeight);
}


PStringList PVideoInputDevice_VideoForWindows::GetInputDeviceNames()
{
  PStringList devices;

  for (WORD devId = 0; devId < 10; devId++) {
    char name[100];
    char version[200];
    if (capGetDriverDescription(devId, name, sizeof(name), version, sizeof(version)))
      devices.AppendString(name);
  }

  return devices;
}


PINDEX PVideoInputDevice_VideoForWindows::GetMaxFrameBytes()
{
  PWaitAndSignal mutex(operationMutex);

  if (!IsOpen())
    return 0;

  return GetMaxFrameBytesConverted(PVideoDeviceBitmap(hCaptureWindow)->bmiHeader.biSizeImage);
}


BOOL PVideoInputDevice_VideoForWindows::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{
  // Some camera drivers ignore the frame rate set in the CAPTUREPARMS structure,
  // so we have a fail safe delay here.
  m_Pacing.Delay(1000/GetFrameRate());
  return GetFrameDataNoDelay(buffer, bytesReturned);
}


BOOL PVideoInputDevice_VideoForWindows::GetFrameDataNoDelay(BYTE * buffer, PINDEX * bytesReturned)
{
  if (!frameAvailable.Wait(1000)) {
    PTRACE(1, "PVidInp\tTimeout waiting for frame grab!");
    return false;
  }

  bool retval = false;

  lastFrameMutex.Wait();

  if (lastFrameData != NULL) {
    if (NULL != converter)
      retval = converter->Convert(lastFrameData, buffer, bytesReturned);
    else {
      memcpy(buffer, lastFrameData, lastFrameSize);
      if (bytesReturned != NULL)
        *bytesReturned = lastFrameSize;
      retval = true;
    }
  }

  lastFrameMutex.Signal();

  if (!useVideoMode && isCapturingNow)
    capGrabFrameNoStop(hCaptureWindow);

  return retval;
}


LRESULT CALLBACK PVideoInputDevice_VideoForWindows::ErrorHandler(HWND hWnd, int id, LPCSTR err)
{
  if (hWnd == NULL)
    return false;

  return ((PVideoInputDevice_VideoForWindows *)capGetUserData(hWnd))->HandleError(id, err);
}


LRESULT PVideoInputDevice_VideoForWindows::HandleError(int id, LPCSTR PTRACE_PARAM(err))
{
  if (id != 0) {
    PTRACE(1, "PVidInp\tErrorHandler: [id="<< id << "] " << err);
  }

  return true;
}


LRESULT CALLBACK PVideoInputDevice_VideoForWindows::VideoHandler(HWND hWnd, LPVIDEOHDR vh)
{
  if (hWnd == NULL || capGetUserData(hWnd) == NULL)
    return false;

  return ((PVideoInputDevice_VideoForWindows *)capGetUserData(hWnd))->HandleVideo(vh);
}


LRESULT PVideoInputDevice_VideoForWindows::HandleVideo(LPVIDEOHDR vh)
{
  if ((vh->dwFlags&(VHDR_DONE|VHDR_KEYFRAME)) != 0) {
    lastFrameMutex.Wait();

    /**
    * As in video mode VfW captures in background, and hence might override the buffer of the current frame,
    * we must copy the frame's data into a separate buffer.
    */

    // If the size of the current frame is same as of the old ...
    //    -> ... simply copy the data of the new frame into the buffer ...
    if(lastFrameSize == vh->dwBytesUsed)
      memcpy(lastFrameData, vh->lpData, lastFrameSize);
    else {
      // ... otherwise delete the old buffer ...
      if (lastFrameSize)
        delete[] lastFrameData;

      // ... and allocate a new one.
      lastFrameSize = vh->dwBytesUsed;
      lastFrameData = new BYTE[lastFrameSize];

      memcpy(lastFrameData, vh->lpData, lastFrameSize);
    }

    lastFrameMutex.Signal();
    frameAvailable.Signal();
  }

  return true;
}


BOOL PVideoInputDevice_VideoForWindows::InitialiseCapture()
{
  if ((hCaptureWindow = capCreateCaptureWindow("Capture Window",
                                               WS_POPUP | WS_CAPTION,
                                               CW_USEDEFAULT, CW_USEDEFAULT,
                                               frameWidth + GetSystemMetrics(SM_CXFIXEDFRAME),
                                               frameHeight + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME),
                                               (HWND)0,
                                               0)) == NULL) {
    lastError = ::GetLastError();
    PTRACE(1, "PVidInp\tcapCreateCaptureWindow failed - " << lastError);
    return false;
  }

  capSetCallbackOnError(hCaptureWindow, ErrorHandler);

  BOOL result = FALSE;
  if (useVideoMode)
    result = capSetCallbackOnVideoStream(hCaptureWindow, VideoHandler);
  else
    result = capSetCallbackOnFrame(hCaptureWindow, VideoHandler);

  if (!result) {
    lastError = ::GetLastError();
    PTRACE(1, "PVidInp\tFailed to set callback on VfW - " << lastError);
    return false;
  }

  WORD devId;

#if PTRACING
  if (PTrace::CanTrace(4)) { // list available video capture drivers
    ostream & trace = PTrace::Begin(5, __FILE__, __LINE__);
    trace << "PVidInp\tEnumerating available video capture drivers:\n";
    for (devId = 0; devId < 10; devId++) { 
      char name[100];
      char version[200];
      if (capGetDriverDescription(devId, name, sizeof(name), version, sizeof(version)) ) 
        trace << "  Video device[" << devId << "] = " << name << ", " << version << '\n';
    }
    trace << PTrace::End;
  }
#endif

  if (deviceName.GetLength() == 1 && isdigit(deviceName[0]))
    devId = (WORD)(deviceName[0] - '0');
  else {
    for (devId = 0; devId < 10; devId++) {
      char name[100];
      char version[200];
      if (capGetDriverDescription(devId, name, sizeof(name), version, sizeof(version)) &&
          (deviceName *= name))
        break;
    }
  }

  capSetUserData(hCaptureWindow, this);

  // Use first driver available.
  if (!capDriverConnect(hCaptureWindow, devId))// ZAZ глюк Винды (выводит окно выбора устройства)
  if (!capDriverConnect(hCaptureWindow, devId)){
    lastError = ::GetLastError();
    PTRACE(1, "PVidInp\tcapDriverConnect failed - " << lastError);
    return false;
  }

  CAPDRIVERCAPS driverCaps;
  memset(&driverCaps, 0, sizeof(driverCaps));
  if (!capDriverGetCaps(hCaptureWindow, &driverCaps, sizeof(driverCaps))) {
    lastError = ::GetLastError();
    PTRACE(1, "PVidInp\tcapGetDriverCaps failed - " << lastError);
    return false;
  }

  PTRACE(6, "PVidInp\tEnumerating CAPDRIVERCAPS values:\n"
            "  driverCaps.wDeviceIndex           = " << driverCaps.wDeviceIndex        << "\n"
            "  driverCaps.fHasOverlay            = " << driverCaps.fHasOverlay         << "\n"
            "  driverCaps.fHasDlgVideoSource     = " << driverCaps.fHasDlgVideoSource  << "\n"
            "  driverCaps.fHasDlgVideoFormat     = " << driverCaps.fHasDlgVideoFormat  << "\n"
            "  driverCaps.fHasDlgVideoDisplay    = " << driverCaps.fHasDlgVideoDisplay << "\n"
            "  driverCaps.fCaptureInitialized    = " << driverCaps.fCaptureInitialized << "\n"
            "  driverCaps.fDriverSuppliesPalettes= " << driverCaps.fDriverSuppliesPalettes);
  
/*
  if (driverCaps.fHasOverlay)
    capOverlay(hCaptureWindow, true);
  else {
    capPreviewRate(hCaptureWindow, 66);
    capPreview(hCaptureWindow, true);
  }
*/
   
  capPreview(hCaptureWindow, false);

#if PTRACING
  if (PTrace::CanTrace(6)) {
    // Display log for every format set
    for (PINDEX prefFormatIdx = 0; FormatTable[prefFormatIdx].colourFormat != NULL; prefFormatIdx++) {
      PVideoDeviceBitmap bi(hCaptureWindow, FormatTable[prefFormatIdx].bitCount); 
      bi->bmiHeader.biCompression = FormatTable[prefFormatIdx].compression;
      for (PINDEX prefResizeIdx = 0; prefResizeIdx < PARRAYSIZE(winTestResTable); prefResizeIdx++) {
        bi->bmiHeader.biWidth = winTestResTable[prefResizeIdx].device_width;
        bi->bmiHeader.biHeight = winTestResTable[prefResizeIdx].device_height;
        bi.ApplyFormat(hCaptureWindow, FormatTable[prefFormatIdx]);
      }
    }
  }
#endif
  
  return SetFrameRate(frameRate) && SetColourFormatConverter(colourFormat.IsEmpty() ? PString("YUV420P") : colourFormat);
}


void PVideoInputDevice_VideoForWindows::HandleCapture(PThread &, INT)
{
  BOOL initSucceeded = InitialiseCapture();

  if (initSucceeded) {
    threadStarted.Signal();

    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0))
      ::DispatchMessage(&msg);
  }

  PTRACE(5, "PVidInp\tDisconnecting driver");
  capDriverDisconnect(hCaptureWindow);
  capSetUserData(hCaptureWindow, NULL);

  capSetCallbackOnError(hCaptureWindow, NULL);
  capSetCallbackOnVideoStream(hCaptureWindow, NULL);

  PTRACE(5, "PVidInp\tDestroying VIDCAP window");
  DestroyWindow(hCaptureWindow);
  hCaptureWindow = NULL;

  // Signal the other thread we have completed, even if have error
  if (!initSucceeded)
    threadStarted.Signal();
}

#endif // P_VFW_CAPTURE


///////////////////////////////////////////////////////////////////////////////
// PVideoOutputDevice_Window

/**This class defines a video output device for RGB in a frame store.
 */
class PVideoOutputDevice_Window : public PVideoOutputDeviceRGB
{
  PCLASSINFO(PVideoOutputDevice_Window, PVideoOutputDeviceRGB);

  public:
    /** Create a new video output device.
     */
    PVideoOutputDevice_Window();

    /** Destroy a video output device.
     */
    ~PVideoOutputDevice_Window();

    /**Open the device given the device name.
      */
    virtual BOOL Open(
      const PString & deviceName,   /// Device name (filename base) to open
      BOOL startImmediate = true    /// Immediately start device
    );

    /**Determine if the device is currently open.
      */
    virtual BOOL IsOpen();

    /**Close the device.
      */
    virtual BOOL Close();

    /**Start the video device I/O display.
      */
    virtual BOOL Start();

    /**Stop the video device I/O display.
      */
    virtual BOOL Stop();

    /**Get a list of all of the devices available.
      */
    static PStringList GetOutputDeviceNames();

    /**Get a list of all of the devices available.
      */
    virtual PStringList GetDeviceNames() const
    { return GetOutputDeviceNames(); }

    /**Set the colour format to be used.
       Note that this function does not do any conversion. If it returns true
       then the video device does the colour format in native mode.

       To utilise an internal converter use the SetColourFormatConverter()
       function.

       Default behaviour sets the value of the colourFormat variable and then
       returns true.
    */
    virtual BOOL SetColourFormat(
      const PString & colourFormat // New colour format for device.
    );

    /**Get the video conversion vertical flip state.
       Default action is to return false.
     */
    virtual BOOL GetVFlipState();

    /**Set the video conversion vertical flip state.
       Default action is to return false.
     */
    virtual BOOL SetVFlipState(
      BOOL newVFlipState    /// New vertical flip state
    );

    /**Set the frame size to be used.

       Note that devices may not be able to produce the requested size, and
       this function will fail.  See SetFrameSizeConverter().

       Default behaviour sets the frameWidth and frameHeight variables and
       returns true.
    */
    virtual BOOL SetFrameSize(
      unsigned width,   /// New width of frame
      unsigned height   /// New height of frame
    );

    /**Set a section of the output frame buffer.
      */
    virtual BOOL FrameComplete();

    /**Get the position of the output device, where relevant. For devices such as
       files, this always returns zeros. For devices such as Windows, this is the
       position of the window on the screen.
      */
    virtual BOOL GetPosition(
      int & x,  // X position of device surface
      int & y   // Y position of device surface
    ) const;

    /**Set the position of the output device, where relevant. For devices such as
       files, this does nothing. For devices such as Windows, this sets the
       position of the window on the screen.
       
       Returns: TRUE if the position can be set.
      */
    virtual bool SetPosition(
      int x,  // X position of device surface
      int y   // Y position of device surface
    );

    LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

  protected:
    PDECLARE_NOTIFIER(PThread, PVideoOutputDevice_Window, HandleDisplay);
    void CreateDisplayWindow();
    void SetWindowSize();
    void Draw(HDC hDC);

    HWND       m_hWnd;
    PThread  * m_thread;
    PMutex     m_openCloseMutex;
    PSyncPoint m_started;
    BITMAPINFO m_bitmap;
    bool       m_flipped;
    POINT      m_lastPosition;
    SIZE       m_fixedSize;
};


#define DEFAULT_STYLE (WS_POPUP|WS_BORDER|WS_SYSMENU|WS_CAPTION)
#define DEFAULT_TITLE "Video Output"

static bool ParseWindowDeviceName(const PString & deviceName, DWORD * dwStylePtr = NULL, HWND * hWndParentPtr = NULL)
{
  if (deviceName.Find("MSWIN") != 0)
    return false;

  PINDEX pos = deviceName.Find("STYLE=");
  DWORD dwStyle = pos == P_MAX_INDEX ? DEFAULT_STYLE : strtoul(((const char *)deviceName)+pos+6, NULL, 0);
  if ((dwStyle&(WS_POPUP|WS_CHILD)) == 0) {
    PTRACE(1, "VidOut\tWindow must be WS_POPUP or WS_CHILD window.");
    return false;
  }

  HWND hWndParent = NULL;
  pos = deviceName.Find("PARENT=");
  if (pos != P_MAX_INDEX) {
    hWndParent = (HWND)strtoul(((const char *)deviceName)+pos+7, NULL, 0);
    if (!::IsWindow(hWndParent)) {
      PTRACE(2, "VidOut\tIllegal parent window " << hWndParent << " specified.");
      hWndParent = NULL;
    }
  }

  // Have parsed out style & parent, see if legal combination
  if (hWndParent == NULL && (dwStyle&WS_POPUP) == 0) {
    PTRACE(1, "VidOut\tWindow must be WS_POPUP if parent window not specified.");
    return false;
  }

  if (dwStylePtr != NULL)
    *dwStylePtr = dwStyle;

  if (hWndParentPtr != NULL)
    *hWndParentPtr = hWndParent;

  return true;
}


class PVideoOutputDevice_Window_PluginServiceDescriptor : public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject *    CreateInstance(int /*userData*/) const { return PNEW PVideoOutputDevice_Window; }
    virtual PStringList	GetDeviceNames(int /*userData*/) const { return PVideoOutputDevice_Window::GetOutputDeviceNames(); }
    virtual bool         ValidateDeviceName(const PString & deviceName, int /*userData*/) const { return ParseWindowDeviceName(deviceName); }
} PVideoOutputDevice_Window_descriptor;

PCREATE_PLUGIN(Window, PVideoOutputDevice, &PVideoOutputDevice_Window_descriptor);


///////////////////////////////////////////////////////////////////////////////
// PVideoOutputDeviceRGB

PVideoOutputDevice_Window::PVideoOutputDevice_Window()
{
  m_hWnd = NULL;
  m_thread = NULL;
  m_flipped = false;
  m_lastPosition.x = 0;
  m_lastPosition.y = 0;
  m_fixedSize.cx = 0;
  m_fixedSize.cy = 0;

  m_bitmap.bmiHeader.biSize = sizeof(m_bitmap.bmiHeader);
  m_bitmap.bmiHeader.biWidth = frameWidth;
  m_bitmap.bmiHeader.biHeight = -(int)frameHeight;
  m_bitmap.bmiHeader.biPlanes = 1;
  m_bitmap.bmiHeader.biBitCount = 32;
  m_bitmap.bmiHeader.biCompression = BI_RGB;
  m_bitmap.bmiHeader.biXPelsPerMeter = 0;
  m_bitmap.bmiHeader.biYPelsPerMeter = 0;
  m_bitmap.bmiHeader.biClrImportant = 0;
  m_bitmap.bmiHeader.biClrUsed = 0;
  m_bitmap.bmiHeader.biSizeImage = frameStore.GetSize();
}


PVideoOutputDevice_Window::~PVideoOutputDevice_Window()
{
  Close();
}


PStringList PVideoOutputDevice_Window::GetOutputDeviceNames()
{
  return psprintf("MSWIN STYLE=0x%08X TITLE=\"%s\"", DEFAULT_STYLE, DEFAULT_TITLE);
}


BOOL PVideoOutputDevice_Window::Open(const PString & name, BOOL startImmediate)
{
  Close();

  m_openCloseMutex.Wait();

  deviceName = name;

  if (deviceName.Find("PARENT") == P_MAX_INDEX) {
    m_thread = PThread::Create(PCREATE_NOTIFIER(HandleDisplay), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "VidOut");

    m_openCloseMutex.Signal();
    m_started.Wait();     
  }
  else {    
    /* Description: child windows should be created on the same thread as the
      parent window (well, not necessarily, but it is much less error prone
      that way) so I am adding another method to handle creating child
      windows which will be created in the calling thread.
      Date: 03/15/2012
      Author: Jonathan M. Henson
     */
    CreateDisplayWindow();
    m_openCloseMutex.Signal();
  }

  return startImmediate ? Start() : IsOpen();
}


BOOL PVideoOutputDevice_Window::IsOpen()
{
  return m_hWnd != NULL;
}


BOOL PVideoOutputDevice_Window::Close()
{
  PWaitAndSignal m(m_openCloseMutex);

  if (m_hWnd == NULL)
    return false;

  SendMessage(m_hWnd, WM_CLOSE, 0, 0);

  if (m_thread != NULL) {
    m_thread->WaitForTermination(3000);
    delete m_thread;
    m_thread = NULL;
  }

  return true;
}


BOOL PVideoOutputDevice_Window::Start()
{
  PWaitAndSignal m(m_openCloseMutex);

  if (m_hWnd == NULL)
    return false;
  
  ShowWindow(m_hWnd, SW_SHOW);
  return true;
}


BOOL PVideoOutputDevice_Window::Stop()
{
  PWaitAndSignal m(m_openCloseMutex);

  if (m_hWnd != NULL)
    return ShowWindow(m_hWnd, SW_HIDE);

  return false;
}


BOOL PVideoOutputDevice_Window::SetColourFormat(const PString & colourFormat)
{
  PWaitAndSignal m(mutex);

  if (((colourFormat *= "BGR24") || (colourFormat *= "BGR32")) &&
                PVideoOutputDeviceRGB::SetColourFormat(colourFormat)) {
    m_bitmap.bmiHeader.biBitCount = (WORD)(bytesPerPixel*8);
    m_bitmap.bmiHeader.biSizeImage = frameStore.GetSize();
    return true;
  }

  return false;
}


BOOL PVideoOutputDevice_Window::GetVFlipState()
{
  return m_flipped;
}


BOOL PVideoOutputDevice_Window::SetVFlipState(BOOL newVFlip)
{
  m_flipped = newVFlip;
  m_bitmap.bmiHeader.biHeight = m_flipped ? frameHeight : -(int)frameHeight;
  return true;
}


BOOL PVideoOutputDevice_Window::SetFrameSize(unsigned width, unsigned height)
{
  {
    PWaitAndSignal m(mutex);

    if (width == frameWidth && height == frameHeight)
      return true;

    if (!PVideoOutputDeviceRGB::SetFrameSize(width, height))
      return false;

    m_bitmap.bmiHeader.biWidth = frameWidth;
    m_bitmap.bmiHeader.biHeight = m_flipped ? frameHeight : -(int)frameHeight;
    m_bitmap.bmiHeader.biSizeImage = frameStore.GetSize();
  }

  // Must be outside of mutex
  SetWindowSize();

  return true;
}


void PVideoOutputDevice_Window::SetWindowSize()
{
  if (m_hWnd == NULL)
    return;

  RECT rect;
  rect.top = 0;
  rect.left = 0;
  rect.bottom = m_fixedSize.cy > 0 ? m_fixedSize.cy : frameHeight;
  rect.right = m_fixedSize.cx > 0 ? m_fixedSize.cx : frameWidth;
  ::AdjustWindowRectEx(&rect, GetWindowLong(m_hWnd, GWL_STYLE), false, GetWindowLong(m_hWnd, GWL_EXSTYLE));
  ::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, rect.right-rect.left, rect.bottom-rect.top, SWP_NOMOVE);
}


BOOL PVideoOutputDevice_Window::FrameComplete()
{
  PWaitAndSignal m(mutex);

  if (m_hWnd == NULL)
    return false;

  HDC hDC = GetDC(m_hWnd);
  Draw(hDC);
  ReleaseDC(m_hWnd, hDC);

  return true;
}


BOOL PVideoOutputDevice_Window::GetPosition(int & x, int & y) const
{
  x = m_lastPosition.x;
  y = m_lastPosition.y;
  return true;
}


bool PVideoOutputDevice_Window::SetPosition(int x, int y)
{
  if (m_hWnd != NULL) {
    RECT rect;
    rect.top = y;
    rect.left = x;
    rect.bottom = y;
    rect.right = x;
    ::AdjustWindowRectEx(&rect, GetWindowLong(m_hWnd, GWL_STYLE), false, GetWindowLong(m_hWnd, GWL_EXSTYLE));
    ::SetWindowPos(m_hWnd, HWND_TOP, x+(x-rect.left), y, 0, 0, SWP_NOSIZE);
  }

  return true;
}


void PVideoOutputDevice_Window::Draw(HDC hDC)
{
  RECT rect;
  GetClientRect(m_hWnd, &rect);

  int result;
  if (frameWidth == (unsigned)rect.right && frameHeight == (unsigned)rect.bottom)
    result = SetDIBitsToDevice(hDC,
                               0, 0, frameWidth, frameHeight,
                               0, 0, 0, frameHeight,
                               frameStore.GetPointer(), &m_bitmap, DIB_RGB_COLORS);
  else {
#ifdef _WIN32_WCE
    SetStretchBltMode(hDC, COLORONCOLOR);
#else
    SetStretchBltMode(hDC, STRETCH_DELETESCANS);
#endif
    result = StretchDIBits(hDC,
                           0, 0, rect.right, rect.bottom,
                           0, 0, frameWidth, frameHeight,
                           frameStore.GetPointer(), &m_bitmap, DIB_RGB_COLORS, SRCCOPY);
  }

  if (result == 0) {
    lastError = ::GetLastError();
    PTRACE(2, "VidOut\tDrawing image failed, error=" << lastError);
  }
}


static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_CREATE)
    SetWindowLong(hWnd, 0, (LONG)((LPCREATESTRUCT)lParam)->lpCreateParams);

  PVideoOutputDevice_Window * vodw = (PVideoOutputDevice_Window *)GetWindowLong(hWnd, 0);
  if (vodw != NULL)
    return vodw->WndProc(uMsg, wParam, lParam);

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


static int GetTokenValue(const PString & deviceName, const char * token, int defaultValue)
{
  PINDEX pos = deviceName.Find(token);
  if (pos == P_MAX_INDEX)
    return defaultValue;

  pos += strlen(token);
  return deviceName.Mid(pos).AsInteger();
}


void PVideoOutputDevice_Window::CreateDisplayWindow()
{
#ifndef _WIN32_WCE
  static char const wndClassName[] = "PVideoOutputDevice_Window";
#else
  static LPCWSTR const wndClassName = L"PVideoOutputDevice_Window";
#endif

  static bool needRegistration = true;
  if (needRegistration) {
    needRegistration = false;

    WNDCLASS wndClass;
    memset(&wndClass, 0, sizeof(wndClass));
    wndClass.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_PARENTDC;
    wndClass.lpszClassName = wndClassName;
    wndClass.lpszClassName = wndClassName;
    wndClass.lpfnWndProc = ::WndProc;
    wndClass.cbWndExtra = sizeof(this);
    PAssertOS(RegisterClass(&wndClass));
  }

  DWORD dwStyle;
  HWND hParent;

  if (!ParseWindowDeviceName(deviceName, &dwStyle, &hParent))
    return;

  PString title = DEFAULT_TITLE;
  PINDEX pos = deviceName.Find("TITLE=\"");
  if (pos != P_MAX_INDEX)
    title = PString(PString::Literal, deviceName.Mid(pos+6));

  m_lastPosition.x = GetTokenValue(deviceName, "X=", CW_USEDEFAULT);
  m_lastPosition.y = GetTokenValue(deviceName, "Y=", CW_USEDEFAULT);
  m_fixedSize.cx   = GetTokenValue(deviceName, "WIDTH=", 0);
  m_fixedSize.cy   = GetTokenValue(deviceName, "HEIGHT=", 0);

  if (m_lastPosition.x == CW_USEDEFAULT && m_lastPosition.y == CW_USEDEFAULT) {
    if (hParent != NULL) {
      RECT rect;
      GetWindowRect(hParent, &rect);
      m_lastPosition.x = (rect.right + rect.left - frameWidth)/2;
      m_lastPosition.y = (rect.bottom + rect.top - frameHeight)/2;
    }
    else {
      m_lastPosition.x = (GetSystemMetrics(SM_CXSCREEN) - frameWidth)/2;
      m_lastPosition.y = (GetSystemMetrics(SM_CYSCREEN) - frameHeight)/2;
    }
  }
  
  m_hWnd = CreateWindow(wndClassName,
                        title, 
                        dwStyle,
                        m_lastPosition.x , m_lastPosition.y, frameWidth, frameHeight,
                        hParent, NULL, GetModuleHandle(NULL), this);
  SetWindowSize();
}


void PVideoOutputDevice_Window::HandleDisplay(PThread &, INT)
{
  CreateDisplayWindow();

  m_started.Signal();

  if (m_hWnd != NULL) {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
      DispatchMessage(&msg);
  }
}


LRESULT PVideoOutputDevice_Window::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  PWaitAndSignal m(mutex);

  switch (uMsg) {
    case WM_PAINT :
      {
        PAINTSTRUCT paint;
        HDC hDC = BeginPaint(m_hWnd, &paint);
        Draw(hDC);
        EndPaint(m_hWnd, &paint);
        break;
      }

    case WM_MOVE :
      if (m_hWnd != NULL) {
        RECT rect;
        GetWindowRect(m_hWnd, &rect);
        m_lastPosition.x = rect.left;
        m_lastPosition.y = rect.top;
      }
      break;

    case WM_LBUTTONDBLCLK :
      if (m_fixedSize.cx < 10000 && m_fixedSize.cy < 10000) {
        m_fixedSize.cx = 2*(m_fixedSize.cx > 0 ? m_fixedSize.cx : frameWidth);
        m_fixedSize.cy = 2*(m_fixedSize.cy > 0 ? m_fixedSize.cy : frameHeight);
        SetWindowSize();
      }
      break;

    case WM_RBUTTONDBLCLK :
      if ((m_fixedSize.cx&1) == 0 && (m_fixedSize.cy&1) == 0) {
        m_fixedSize.cx = (m_fixedSize.cx > 0 ? m_fixedSize.cx : frameWidth)/2;
        m_fixedSize.cy = (m_fixedSize.cy > 0 ? m_fixedSize.cy : frameHeight)/2;
        SetWindowSize();
      }
      break;

    case WM_CLOSE :
      DestroyWindow(m_hWnd);
      m_hWnd = NULL;
      break;

    case WM_DESTROY:
      PostThreadMessage(GetCurrentThreadId(), WM_QUIT, 0, 0);
      break;
  }
  return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

#endif // P_VIDEO



// End Of File ///////////////////////////////////////////////////////////////
