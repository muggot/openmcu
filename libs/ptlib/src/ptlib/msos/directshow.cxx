/*
 * directshow.cxx
 *
 * DirectShow Implementation for the H323Plus/OPAL Project.
 *
 * Copyright (c) 2009 ISVO (Asia) Pte Ltd. All Rights Reserved.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the General Public License (the  "GNU License"), in which case the
 * provisions of GNU License are applicable instead of those
 * above. If you wish to allow use of your version of this file only
 * under the terms of the GNU License and not to allow others to use
 * your version of this file under the MPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and
 * other provisions required by the GNU License. If you do not delete
 * the provisions above, a recipient may use your version of this file
 * under either the MPL or the GNU License."
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * Initial work sponsored by Requestec Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision: 27570 $
 * $Author: ededu $
 * $Date: 2012-05-03 10:00:22 -0500 (Thu, 03 May 2012) $
 */


#include <ptlib.h>

#ifdef P_DIRECTSHOW

#include <ptlib/videoio.h>
#include <ptlib/vconvert.h>

#include <ptlib/msos/ptlib/pt_atl.h>

#ifdef _WIN32_WCE

  static const GUID MEDIASUBTYPE_IYUV = { 0x56555949, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 };
  #define CLSID_CaptureGraphBuilder2 CLSID_CaptureGraphBuilder

  #pragma comment(lib, "strmbase.lib")
  #pragma comment(lib, "mmtimer.lib")

  #ifdef DEBUG
    /* Only the release version is provided as a .lib file, so we need to
       make sure that the compilation does NOT have the extra fields/functions
       that are added when DEBUG version. */
    #undef DEBUG
    #include <streams.h>
    #define DEBUG
  #else
    #include <streams.h>
  #endif

  #include <dshow.h>
  #include <initguid.h>

  class PSampleGrabber : public CBaseVideoRenderer
  {
    public:
      PSampleGrabber(HRESULT * hr);

      virtual HRESULT CheckMediaType(const CMediaType *media);
      virtual HRESULT ShouldDrawSampleNow(IMediaSample *sample, REFERENCE_TIME *start, REFERENCE_TIME *stop);
      virtual HRESULT DoRenderSample(IMediaSample *sample);

      PMutex m_sampleMutex;
      long   m_sampleSize;
      BYTE * m_sampleData;
  };

  #ifdef _MSC_VER
    #pragma comment(lib, "ddraw.lib")
  #endif

#else // _WIN32_WCE

  /* workaround a compile error with mingw-w64 on sprintf member function
     below. Even though the member function is not the same thing as the
     global function which _is_ deprecated.

     Also applies to a warning in MSVC.

     Need to define this before dshow.h inclusion.
   */
  #define STRSAFE_NO_DEPRECATE

  #include <dshow.h>
  #include <initguid.h>
  #include <ks.h>
  #include <ksmedia.h>

  class PVideoInputControl_DirectShow : public PVideoInputControl
  {
      PCLASSINFO(PVideoInputControl_DirectShow, PVideoInputControl);
    public:
      PVideoInputControl_DirectShow(IAMCameraControl * _pCC);

      // overrides
      virtual BOOL Pan(long value, bool absolute = false);
      virtual BOOL Tilt(long value, bool absolute = false);
      virtual BOOL Zoom(long value, bool absolute = false);

    protected:
      IAMCameraControl * t_pCC;
  };


  #ifdef P_DIRECTSHOW_QEDIT_H

    // Use this to avoid compile error in Qedit.h with DirectX SDK
    #define __IDxtCompositor_INTERFACE_DEFINED__
    #define __IDxtAlphaSetter_INTERFACE_DEFINED__
    #define __IDxtJpeg_INTERFACE_DEFINED__
    #define __IDxtKey_INTERFACE_DEFINED__

    #pragma include_alias("dxtrans.h", "ptlib/msos/dxtrans.h")

    #include <rpcsal.h>
    #include P_DIRECTSHOW_QEDIT_H

  #else

    extern "C" {
      extern const CLSID CLSID_SampleGrabber;
      extern const IID IID_ISampleGrabber;
      extern const IID IID_ISampleGrabberCB;
      extern const CLSID CLSID_NullRenderer;
      extern const CLSID CLSID_CameraName;
    };


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

  #endif // P_DIRECTSHOW_QEDIT_H

  #ifdef _MSC_VER
    #pragma comment(lib, "quartz.lib")
  #endif

#endif // _WIN32_WCE

#ifdef _MSC_VER
  #pragma comment(lib, "strmiids.lib")
  #pragma message("Direct Show video support enabled")
#endif


//////////////////////////////////////////////////////////////////////
// Video Input device

class PVideoInputDevice_DirectShow : public PVideoInputDevice
{
    PCLASSINFO(PVideoInputDevice_DirectShow, PVideoInputDevice);

  public:
    PVideoInputDevice_DirectShow();
    ~PVideoInputDevice_DirectShow();


    static PStringList GetInputDeviceNames();
    virtual PStringList GetDeviceNames() const;
    static BOOL GetDeviceCapabilities(const PString & deviceName, Capabilities * capabilities);
    virtual bool GetDeviceCapabilities(Capabilities * capabilities) const;
#ifndef _WIN32_WCE
    virtual PVideoInputControl * GetVideoInputControls();
#endif

    virtual BOOL Open(const PString & deviceName, BOOL startImmediate);
    virtual BOOL IsOpen();
    virtual BOOL Close();
    virtual BOOL Start();
    virtual BOOL Stop();
    virtual BOOL IsCapturing();
    virtual BOOL SetColourFormat(const PString & colourFormat);
    virtual BOOL SetFrameRate(unsigned rate);
    virtual BOOL SetFrameSize(unsigned width, unsigned height);
    virtual PINDEX GetMaxFrameBytes();
    virtual BOOL GetFrameData(BYTE * buffer, PINDEX * bytesReturned);
    virtual BOOL GetFrameDataNoDelay(BYTE * buffer, PINDEX * bytesReturned);
    virtual bool FlowControl(const void * flowData);
    virtual int GetBrightness();
    virtual BOOL SetBrightness(unsigned newBrightness);
    virtual int GetWhiteness();
    virtual BOOL SetWhiteness(unsigned newWhiteness);
    virtual int GetColour();
    virtual BOOL SetColour(unsigned newColour);
    virtual int GetContrast();
    virtual BOOL SetContrast(unsigned newContrast);
    virtual int GetHue();
    virtual BOOL SetHue(unsigned newHue);
    virtual BOOL GetParameters(int *whiteness, int *brightness, int *colour, int *contrast, int *hue);


  protected:
    bool BindCaptureDevice(const PString & devName);
    bool PlatformOpen();
    PINDEX GetCurrentBufferSize();
    bool GetCurrentBufferData(BYTE * data);
    bool SetPinFormat(unsigned useDefaultColourOrSize = 0);
    BOOL SetControlCommon(long control, int newValue);
    int GetControlCommon(long control);


    // DirectShow 
    CComPtr<IGraphBuilder>         m_pGraphBuilder;
    CComPtr<ICaptureGraphBuilder2> m_pCaptureBuilder;
    CComPtr<IBaseFilter>           m_pCaptureFilter;
    CComPtr<IPin>                  m_pCameraOutPin; // Camera output out -> Transform Input pin
    GUID                           m_selectedGUID;

#ifdef _WIN32_WCE
    PSampleGrabber               * m_pSampleGrabber;
#else
    CComPtr<ISampleGrabber>        m_pSampleGrabber;
    CComPtr<ISampleGrabberCB>      m_pSampleGrabberCB;
    CComPtr<IAMCameraControl>      m_pCameraControls;
#endif
    CComPtr<IBaseFilter>           m_pNullRenderer;
    CComPtr<IMediaControl>         m_pMediaControl;

    PINDEX     m_maxFrameBytes;
    PBYTEArray m_tempFrame;
    PMutex     m_lastFrameMutex;
};


PCREATE_VIDINPUT_PLUGIN(DirectShow);


////////////////////////////////////////////////////////////////////

static struct {
    const char * m_colourFormat;
    GUID         m_guid;
} const ColourFormat2GUID[] =
{
    { "Grey",    MEDIASUBTYPE_RGB8   },
    { "BGR32",   MEDIASUBTYPE_RGB32  }, /* Microsoft assumes that we are in little endian */
    { "BGR24",   MEDIASUBTYPE_RGB24  },
    { "RGB565",  MEDIASUBTYPE_RGB565 },
    { "RGB555",  MEDIASUBTYPE_RGB555 },
    { "YUV420P", MEDIASUBTYPE_IYUV   },  // aka I420
    { "YUV420P", MEDIASUBTYPE_YV12   },
    { "YUV411",  MEDIASUBTYPE_Y411   },
    { "YUV411P", MEDIASUBTYPE_Y41P   },
    { "YUV410P", MEDIASUBTYPE_YVU9   },
    { "YUY2",    MEDIASUBTYPE_YUY2   },
    { "MJPEG",   MEDIASUBTYPE_MJPG   },
    { "UYVY422", MEDIASUBTYPE_UYVY   }
};

static PString GUID2Format(GUID guid)
{
   for (int j = 0; j < sizeof(ColourFormat2GUID)/sizeof(ColourFormat2GUID[0]); j++) {
    if (guid == ColourFormat2GUID[j].m_guid)
      return ColourFormat2GUID[j].m_colourFormat;
   }

   wchar_t guidName[256];
   if (StringFromGUID2(guid, guidName, sizeof(guidName)) <= 0)
       return "UNKNOWN"; // Can't use this entry!

   char buff[256];
   sprintf(buff, "%S", guidName);
   return buff;
}


////////////////////////////////////////////////////////////////////
// Some support functions/macros

#if PTRACING
static PString ErrorMessage(HRESULT hr)
{
  PString msg;
  DWORD dwMsgLen = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                                 FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,
                                 hr,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 msg.GetPointer(1000), 999,
                                 NULL);
  if (dwMsgLen > 0)
    return msg;

#ifndef _WIN32_WCE
  dwMsgLen = AMGetErrorTextA(hr, msg.GetPointer(1000), 1000);
  if (dwMsgLen > 0)
    return msg;
#endif

  char hex[20];
  _snprintf(hex, sizeof(hex), "0x%08x", hr);
  return hex;
}
#endif // PTRACING


#if PTRACING
static bool CheckError(HRESULT hr, const char * fn)
{
  if (SUCCEEDED(hr))
    return false;

  PTRACE(1, "DShow\tFunction \"" << fn << "\" failed : " << ErrorMessage(hr));
  return true;
}

#define CHECK_ERROR(fn, action) if (CheckError(fn, #fn)) action
#else
#define CHECK_ERROR(fn, action) if (FAILED(fn)) action
#endif

#define CHECK_ERROR_RETURN(fn) CHECK_ERROR(fn, return false)


//////////////////////////////////////////////////////////////////////////////////////////////////

class MediaTypePtr
{
    AM_MEDIA_TYPE * pointer;
  public:
    MediaTypePtr()
      : pointer(NULL)
    {
    }

    ~MediaTypePtr()
    {
      Release();
    }

    void Release()
    {
      if (pointer == NULL)
        return;

      if (pointer->cbFormat != 0) {
        CoTaskMemFree(pointer->pbFormat);
        pointer->cbFormat = 0;
        pointer->pbFormat = NULL;
      }

      if (pointer->pUnk != NULL) {
        // Uncessessary because pUnk should not be used, but safest.
        pointer->pUnk->Release();
        pointer->pUnk = NULL;
      }

      CoTaskMemFree(pointer);
    }

    operator AM_MEDIA_TYPE *()        const { return  pointer; }
    AM_MEDIA_TYPE & operator*()       const { return *pointer; }
    AM_MEDIA_TYPE** operator&()             { return &pointer; }
	AM_MEDIA_TYPE* operator->()       const { return  pointer; }

  private:
    MediaTypePtr(const MediaTypePtr &) { }
    void operator=(const MediaTypePtr &) { }
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// Bubble Sorting functions
// This is used to sort the video frame sizes from largest to smallest (removing duplicates)
// The optimum frame size can be selected by sorting down the list for a given bandwidth.
// If there is a better way to do this then great but it very important in order the framesizes. - SH

class PVideoFrameInfoSort
{
  public:
    bool operator()(PVideoFrameInfo* const& f1, PVideoFrameInfo* const& f2)
    {
      if (f1->GetFrameWidth() > f2->GetFrameWidth())
        return true;
      if (f1->GetFrameWidth() == f2->GetFrameWidth() && f1->GetFrameHeight() > f2->GetFrameHeight())
        return true;
      return false;
    }
};

class PVideoFrameInfoMatch
{
  public:
    bool operator()(PVideoFrameInfo*& f1, PVideoFrameInfo*& f2)
    {
      return (f1->GetFrameWidth()  == f2->GetFrameWidth()) &&
             (f1->GetFrameHeight() == f2->GetFrameHeight());
    }
};

class PVideoFrameInfoArray : public std::vector<PVideoFrameInfo*>
{
  public:
    ~PVideoFrameInfoArray()
    {
      for (iterator pItem=begin(); pItem != end(); ++pItem)
        delete *pItem;
    }

    void ReIndex()
    {
      sort(begin(), end(), PVideoFrameInfoSort());
      unique(begin(), end(), PVideoFrameInfoMatch());

      unsigned lastWidth = P_MAX_INDEX;
      int pos = 0;
      iterator r = begin();
      while (r != end()) {
        const PVideoFrameInfo* frame = *r;
        if (frame->GetFrameWidth() > lastWidth)
          break;
        lastWidth = frame->GetFrameWidth();
        ++pos; ++r;
      }

      int sz = size();
      PINDEX i = sz-1;
      while (i > pos-1) {
        erase(begin()+i);
        --i;
      }
    }
};


//////////////////////////////////////////////////////////////////////////////////////////////////
// Input device

PVideoInputDevice_DirectShow::PVideoInputDevice_DirectShow()
  : m_maxFrameBytes(0)
#ifdef _WIN32_WCE
  , m_pSampleGrabber(NULL)
#endif
{
  PTRACE(4, "DShow\tVideo Device Instance");

  CoInitializeEx(NULL,COINIT_MULTITHREADED);
}


PVideoInputDevice_DirectShow::~PVideoInputDevice_DirectShow()
{
  Close();
}


PStringList PVideoInputDevice_DirectShow::GetInputDeviceNames()
{
  PVideoInputDevice_DirectShow instance;
  return instance.GetDeviceNames();
}


BOOL PVideoInputDevice_DirectShow::GetDeviceCapabilities(const PString & deviceName,
                                                             Capabilities * capabilities)
{
  PVideoInputDevice_DirectShow instance;
  return instance.Open(deviceName, false) && instance.GetDeviceCapabilities(capabilities);
}


bool PVideoInputDevice_DirectShow::GetDeviceCapabilities(Capabilities * caps) const
{
  CComPtr<IAMStreamConfig> pStreamConfig;
#ifdef __MINGW32__
  CHECK_ERROR_RETURN(m_pCameraOutPin->QueryInterface(IID_IAMStreamConfig, (void**)&pStreamConfig));
#else
  CHECK_ERROR_RETURN(m_pCameraOutPin->QueryInterface(&pStreamConfig));
#endif

  int iCount, iSize;
  CHECK_ERROR_RETURN(pStreamConfig->GetNumberOfCapabilities(&iCount, &iSize));

  /* Sanity check: just to be sure that the Streamcaps is a VIDEOSTREAM and not AUDIOSTREAM */
  VIDEO_STREAM_CONFIG_CAPS scc;
  if (sizeof(scc) != iSize) {
    PTRACE(1, "DShow\tBad Capapabilities (not a VIDEO_STREAM_CONFIG_CAPS)");
    return false;
  }

  PVideoFrameInfoArray fsizes;
  for (int iFormat = 0; iFormat < iCount; iFormat++) {
    MediaTypePtr pMediaFormat;
    if (SUCCEEDED(pStreamConfig->GetStreamCaps(iFormat, &pMediaFormat, (BYTE *)&scc)) &&
        pMediaFormat->majortype == MEDIATYPE_Video &&
        pMediaFormat->formattype == FORMAT_VideoInfo &&
        pMediaFormat->pbFormat != NULL &&
        pMediaFormat->cbFormat >= sizeof(VIDEOINFOHEADER))
      fsizes.push_back(new PVideoFrameInfo(scc.MaxOutputSize.cx,
                                           scc.MaxOutputSize.cy,
                                           GUID2Format(pMediaFormat->subtype),
                                           10000000/(unsigned)scc.MinFrameInterval));
  }

  if (fsizes.empty())
    return false;

  // Sort so we have unique sizes from largest to smallest
  fsizes.ReIndex();
  caps->framesizes.clear();
  for (std::vector<PVideoFrameInfo*>::iterator it = fsizes.begin(); it != fsizes.end(); ++it) {
    PTRACE(5, "DShow\tFormat["<< caps->framesizes.size() << "] = (" << **it);
    caps->framesizes.push_back(**it);
  }

#ifndef _WIN32_WCE

  PVideoControlInfo panInfo;
  PVideoControlInfo tiltInfo;
  PVideoControlInfo zoomInfo;

  // Query for camera controls
  if (m_pCameraControls != NULL) {
    // Retrieve information about the pan and tilt controls
    panInfo.type= PVideoControlInfo::ControlPan;
    HRESULT hr = m_pCameraControls->GetRange(CameraControl_Pan, &panInfo.min, &panInfo.max, &panInfo.step, &panInfo.def, &panInfo.flags);
    if (FAILED(hr)) {
      PTRACE(4, "DShow\tCamera " << deviceName << " does not support Pan.");
    } else {
      caps->controls.push_back(panInfo);
    }

    tiltInfo.type= PVideoControlInfo::ControlTilt;
    hr = m_pCameraControls->GetRange(CameraControl_Tilt, &tiltInfo.min, &tiltInfo.max, &tiltInfo.step, &tiltInfo.def, &tiltInfo.flags);
    if (FAILED(hr)) {
      PTRACE(4, "DShow\tCamera " << deviceName << " does not support Tilt.");
    } else {
      caps->controls.push_back(tiltInfo);
    }

    zoomInfo.type= PVideoControlInfo::ControlZoom;
    hr = m_pCameraControls->GetRange(CameraControl_Zoom, &zoomInfo.min, &zoomInfo.max, &zoomInfo.step, &zoomInfo.def, &zoomInfo.flags);
    if (FAILED(hr)) {
      PTRACE(4, "DShow\tCamera " << deviceName << " does not support zoom.");
    } else {
      caps->controls.push_back(zoomInfo);
    }
  }

#endif

  return true;
}


bool PVideoInputDevice_DirectShow::SetPinFormat(unsigned useDefaultColourOrSize)
{
  if (m_pCameraOutPin == NULL) {
    PTRACE(2, "DShow\tCamera output pin is NULL!");
    return false;
  }

  CComPtr<IAMStreamConfig> pStreamConfig;
#ifdef __MINGW32__
  CHECK_ERROR_RETURN(m_pCameraOutPin->QueryInterface(IID_IAMStreamConfig, (void**)&pStreamConfig));
#else
  CHECK_ERROR_RETURN(m_pCameraOutPin->QueryInterface(&pStreamConfig));
#endif

  int iCount = 0, iSize=0;
  CHECK_ERROR_RETURN(pStreamConfig->GetNumberOfCapabilities(&iCount, &iSize));

  if (iSize != sizeof(VIDEO_STREAM_CONFIG_CAPS))
    return false;

  for (int iFormat = 0; iFormat < iCount; iFormat++) {
    VIDEO_STREAM_CONFIG_CAPS scc;
    MediaTypePtr pMediaFormat;
    if (SUCCEEDED(pStreamConfig->GetStreamCaps(iFormat, &pMediaFormat, (BYTE *)&scc)) &&
        pMediaFormat->majortype == MEDIATYPE_Video &&
        pMediaFormat->formattype == FORMAT_VideoInfo &&
        pMediaFormat->pbFormat != NULL &&
        pMediaFormat->cbFormat >= sizeof(VIDEOINFOHEADER) &&
        (
          useDefaultColourOrSize >= 2 ||
          (
            scc.MaxOutputSize.cx == (LONG)frameWidth &&
            scc.MaxOutputSize.cy == (LONG)frameHeight &&
            (
              useDefaultColourOrSize >= 1 ||
              GUID2Format(pMediaFormat->subtype) == colourFormat
            )
          )
        )) {

      bool running = IsCapturing();
      if (running)
        CHECK_ERROR_RETURN(m_pMediaControl->Stop());

      VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *)pMediaFormat->pbFormat;
      pVih->AvgTimePerFrame = 10000000 / frameRate;
      CHECK_ERROR_RETURN(pStreamConfig->SetFormat(pMediaFormat));

      if (useDefaultColourOrSize >= 1) {
        colourFormat = GUID2Format(pMediaFormat->subtype);
        if (useDefaultColourOrSize >= 2) {
          frameWidth = scc.MaxOutputSize.cx;
          frameHeight = scc.MaxOutputSize.cy;
        }
      }

      m_selectedGUID = pMediaFormat->subtype;
      m_maxFrameBytes = CalculateFrameBytes(frameWidth, frameHeight, colourFormat);

      if (running)
        CHECK_ERROR_RETURN(m_pMediaControl->Run());

      PTRACE(4, "DShow\tCamera format set to " << *this);
      return true;
    }
  }

  PTRACE(2, "DShow\tCamera formats available could not be matched to " << *this);
  return false;
}


BOOL PVideoInputDevice_DirectShow::Open(const PString & devName,
                                            BOOL        startImmediate)
{
  Close();

  if (devName.IsEmpty()) {
    PTRACE(2, "DShow\tUnable to Bind to empty device");
    return false;
  }

  // Get the interface for DirectShow's GraphBuilder
  CHECK_ERROR_RETURN(CoCreateInstance(CLSID_FilterGraph,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      IID_IGraphBuilder,
                                      (LPVOID *)&m_pGraphBuilder));

  // Create the capture graph builder
  CHECK_ERROR_RETURN(CoCreateInstance(CLSID_CaptureGraphBuilder2,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      IID_ICaptureGraphBuilder2,
                                      (LPVOID *)&m_pCaptureBuilder));

  // Bind the Camera Input
  if (!BindCaptureDevice(devName))
    return false;

  // Add Capture filter to our graph.
  CHECK_ERROR_RETURN(m_pGraphBuilder->AddFilter(m_pCaptureFilter, L"Video Capture"));

  // Attach the filter graph to the capture graph
  CHECK_ERROR_RETURN(m_pCaptureBuilder->SetFiltergraph(m_pGraphBuilder));

  // Obtain interfaces for media control (start/stop capture)
  CHECK_ERROR_RETURN(m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl));

  // Get the camera output Pin 
  CComPtr<IEnumPins> pEnum;
  CHECK_ERROR_RETURN(m_pCaptureFilter->EnumPins(&pEnum));

  CHECK_ERROR_RETURN(pEnum->Reset());
  CHECK_ERROR_RETURN(pEnum->Next(1, &m_pCameraOutPin, NULL));

  // Set the format of the Output pin of the camera
  if (!(SetPinFormat(0) || SetPinFormat(1) || SetPinFormat(2)))
    return false;

  if (!PlatformOpen())
    return false;

  if (startImmediate) 
    Start();

  PTRACE(4, "DShow\tDevice " << devName << " open.");
  deviceName = devName;
  return true;
}


BOOL PVideoInputDevice_DirectShow::IsOpen()
{
  return m_pGraphBuilder != NULL;
}


BOOL PVideoInputDevice_DirectShow::Close()
{
  if (!IsOpen())
    return false;

  PTRACE(4, "DShow\tTorn Down.");

  // Stop Camera Graph
  Stop();

  // Release filters
#ifdef _WIN32_WCE
  if (m_pSampleGrabber != NULL) {
    m_pSampleGrabber->Release();
    delete m_pSampleGrabber;
  }
#else
  m_pNullRenderer.Release();
  m_pSampleGrabberCB.Release();
  m_pSampleGrabber.Release();
  m_pCameraControls.Release();
#endif

  // Release the Camera and interfaces
  m_pMediaControl.Release();
  m_pCameraOutPin.Release(); 
  m_pCaptureBuilder.Release();
  m_pCaptureFilter.Release();

  // Relase DirectShow Graph
  m_pGraphBuilder.Release(); 

  return true;
}


BOOL PVideoInputDevice_DirectShow::Start()
{
  if (!IsOpen()) {
    PTRACE(3, "DShow\tNot open.");
    return false;
  }

  if (IsCapturing())
    return true;

  CHECK_ERROR_RETURN(m_pMediaControl->Run());

  PTRACE(4, "DShow\tVideo Started.");
  return true;
}


BOOL PVideoInputDevice_DirectShow::Stop()
{
  if (!IsOpen()) {
    PTRACE(3, "DShow\tNot open.");
    return false;
  }

  if (!IsCapturing())
    return true;

  // Use Pause() not Stop() as the latter is to much of a stop and takes too long to restart
  CHECK_ERROR_RETURN(m_pMediaControl->Pause());

  PTRACE(3, "DShow\tVideo Stopped.");
  return true;
}


BOOL PVideoInputDevice_DirectShow::IsCapturing()
{
  OAFilterState state;
  CHECK_ERROR_RETURN(m_pMediaControl->GetState(0, &state));
  return state == State_Running;
}


BOOL PVideoInputDevice_DirectShow::SetColourFormat(const PString & newColourFormat)
{
  if (colourFormat == newColourFormat)
    return true;

  PString oldColourFormat = colourFormat;

  if (!PVideoDevice::SetColourFormat(newColourFormat))
    return false;

  if (SetPinFormat())
    return true;

  PVideoDevice::SetColourFormat(oldColourFormat);
  return false;
}


BOOL PVideoInputDevice_DirectShow::SetFrameRate(unsigned newRate)
{
  if (frameRate == newRate)
    return true;

  unsigned oldRate = frameRate;

  if (!PVideoDevice::SetFrameRate(newRate))
    return false;

  if (SetPinFormat())
    return true;

  PVideoDevice::SetFrameRate(oldRate);
  return false;
}


BOOL PVideoInputDevice_DirectShow::SetFrameSize(unsigned width, unsigned height)
{
  if (frameWidth == width && frameHeight == height)
    return true;

  unsigned oldWidth = frameWidth;
  unsigned oldHeight = frameHeight;

  if (!PVideoDevice::SetFrameSize(width, height))
    return false;

  if (SetPinFormat())
    return true;

  PVideoDevice::SetFrameSize(oldWidth, oldHeight);
  return false;
}


bool PVideoInputDevice_DirectShow::FlowControl(const void * flowData)
{
    const PStringArray & options = *(const PStringArray *)flowData;
    
    int w=0; int h=0; int r=0;
    for (PINDEX i=0; i < options.GetSize(); i+=2) {
      if (options[i] == "Frame Width")
            w = options[i+1].AsInteger();
      else if (options[i] == "Frame Height")
            h = options[i+1].AsInteger();
      else if (options[i] ==  "Frame Time")
            r =  90000/options[i+1].AsInteger();
    }

    PTRACE(4, "DShow\tAdjusting to new H: " << h << " W: " << w << " R: " << r);
    m_lastFrameMutex.Wait();
    SetFrameSize(w,h);
    SetFrameRate(r);
    m_lastFrameMutex.Signal();

    return true;
}

PINDEX PVideoInputDevice_DirectShow::GetMaxFrameBytes()
{
  return GetMaxFrameBytesConverted(CalculateFrameBytes(frameWidth, frameHeight, colourFormat));
}

BOOL PVideoInputDevice_DirectShow::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{
  return GetFrameDataNoDelay(buffer,bytesReturned);
}

BOOL PVideoInputDevice_DirectShow::GetFrameDataNoDelay(BYTE * destFrame, PINDEX * bytesReturned)
{
  PWaitAndSignal mutex(m_lastFrameMutex);

  PTRACE(6, "DShow\tGrabbing Frame");

  PINDEX bufferSize = GetCurrentBufferSize();
  if (converter != NULL) {
    if (!GetCurrentBufferData(m_tempFrame.GetPointer(bufferSize)))
      return false;
    if (!converter->Convert(m_tempFrame, destFrame, bufferSize, bytesReturned))
      return false;
  }
  else {
    if (!PAssert(bufferSize <= m_maxFrameBytes, PLogicError))
      return false;
    if (!GetCurrentBufferData(destFrame))
      return false;
    if (bytesReturned != NULL)
      *bytesReturned = bufferSize;
  }

  return true;
}


int PVideoInputDevice_DirectShow::GetControlCommon(long control)
{
  CComPtr<IAMVideoProcAmp> pVideoProcAmp;
  CHECK_ERROR(m_pCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pVideoProcAmp), return -1);

  long minimum, maximum, stepping, def, flags;
  CHECK_ERROR(pVideoProcAmp->GetRange(control, &minimum, &maximum, &stepping, &def, &flags), return -1);

  long value;
  CHECK_ERROR(pVideoProcAmp->Get(control, &value, &flags), return -1);

  if (flags == VideoProcAmp_Flags_Auto)
    return -1;

  return ((value - minimum) * 65536) / (maximum-minimum);
}


int PVideoInputDevice_DirectShow::GetBrightness()
{
  return GetControlCommon(VideoProcAmp_Brightness);
}

int PVideoInputDevice_DirectShow::GetWhiteness()
{
  return GetControlCommon(VideoProcAmp_Gamma);
}

int PVideoInputDevice_DirectShow::GetColour()
{
  return GetControlCommon(VideoProcAmp_Saturation);
}

int PVideoInputDevice_DirectShow::GetContrast()
{
  return GetControlCommon(VideoProcAmp_Contrast);
}

int PVideoInputDevice_DirectShow::GetHue()
{
  return GetControlCommon(VideoProcAmp_Hue);
}

BOOL PVideoInputDevice_DirectShow::GetParameters(int *whiteness, int *brightness, int *colour, int *contrast, int *hue)
{
  if (!IsOpen())
    return false;

  *whiteness  = GetWhiteness();
  *brightness = GetBrightness();
  *colour     = GetColour();
  *contrast   = GetContrast();
  *hue        = GetHue();

  return true;
}


BOOL PVideoInputDevice_DirectShow::SetControlCommon(long control, int newValue)
{
  PTRACE(4, "DShow\tSetControl() = " << newValue);

  CComPtr<IAMVideoProcAmp> pVideoProcAmp;
  CHECK_ERROR_RETURN(m_pCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pVideoProcAmp));

  long minimum, maximum, stepping, def, flags;
  CHECK_ERROR_RETURN(pVideoProcAmp->GetRange(control, &minimum, &maximum, &stepping, &def, &flags));

  HRESULT hr;
  if (newValue == -1)
    hr = pVideoProcAmp->Set(control, 0, VideoProcAmp_Flags_Auto);
  else
  {
    long scaled = minimum + ((maximum-minimum) * newValue) / 65536;
    hr = pVideoProcAmp->Set(control, scaled, VideoProcAmp_Flags_Manual);
  }
  PTRACE_IF(2, FAILED(hr), "DShow\tFailed to setRange interface on " << control << " : " << ErrorMessage(hr));

  return true;
}

BOOL PVideoInputDevice_DirectShow::SetBrightness(unsigned newBrightness)
{
  return SetControlCommon(VideoProcAmp_Brightness, newBrightness);
}

BOOL PVideoInputDevice_DirectShow::SetColour(unsigned newColour)
{
  return SetControlCommon(VideoProcAmp_Saturation, newColour);
}

BOOL PVideoInputDevice_DirectShow::SetContrast(unsigned newContrast)
{
  return SetControlCommon(VideoProcAmp_Contrast, newContrast);
}

BOOL PVideoInputDevice_DirectShow::SetHue(unsigned newHue)
{
  return SetControlCommon(VideoProcAmp_Hue, newHue);
}

BOOL PVideoInputDevice_DirectShow::SetWhiteness(unsigned newWhiteness)
{
  return SetControlCommon(VideoProcAmp_Gamma, newWhiteness);
}


///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WCE

class CPropertyBag : public IPropertyBag
{  
    struct VAR_LIST
    {
      VARIANT var;
      VAR_LIST *pNext;
      BSTR pBSTRName;
    }  * m_pVarList;
    LONG m_refCount;

  public:
    CPropertyBag()
       : m_refCount(1), m_pVarList(0)
    {
    }

    ~CPropertyBag()
    {
      VAR_LIST *pTemp = m_pVarList;
      while (pTemp != NULL) {
        VariantClear(&pTemp->var);
        SysFreeString(pTemp->pBSTRName);

        VAR_LIST * pDel = pTemp;
        pTemp = pTemp->pNext;
        delete pDel;
      }
    }

    HRESULT Read(LPCOLESTR pszPropName, VARIANT *pVar, IErrorLog *pErrorLog)
    {
      VAR_LIST *pTemp = m_pVarList;
      while (pTemp != NULL) {
        if (0 == wcscmp(pszPropName, pTemp->pBSTRName))
          return VariantCopy(pVar, &pTemp->var);
        pTemp = pTemp->pNext;
      }

      return S_FALSE;
    }

    HRESULT Write(LPCOLESTR pszPropName, VARIANT *pVar)
    {
      VAR_LIST *pTemp = new VAR_LIST();
      if (pTemp == NULL)
        return E_OUTOFMEMORY;

      pTemp->pNext = m_pVarList;
      m_pVarList = pTemp;

      pTemp->pBSTRName = SysAllocString(pszPropName);

      VariantInit(&pTemp->var);
      return VariantCopy(&pTemp->var, pVar);
    }

    ULONG AddRef()
    {
      return InterlockedIncrement(&m_refCount);
    }

    ULONG Release()
    {
      ULONG ret = InterlockedDecrement(&m_refCount);
      if (ret == 0)
        delete this; 
      return ret;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv)
    {
      if (ppv == NULL) 
        return E_POINTER;

      if (riid != IID_IPropertyBag) {
        *ppv = 0;
        return E_NOINTERFACE;
      }

      *ppv = static_cast<IPropertyBag*>(this);	
      AddRef();
      return S_OK;
    }
};


PStringList PVideoInputDevice_DirectShow::GetDeviceNames() const
{
  PStringList devices;

  GUID guidCamera = { 0xCB998A05, 0x122C, 0x4166, 0x84, 0x6A, 0x93, 0x3E, 0x4D, 0x7E, 0x3C, 0x86 };
  // Note about the above: The driver material doesn't ship as part of the SDK. This GUID is hardcoded
  // here to be able to enumerate the camera drivers and pass the name of the driver to the video capture filter

  DEVMGR_DEVICE_INFORMATION devInfo;
  devInfo.dwSize = sizeof(devInfo);

  HANDLE handle = FindFirstDevice(DeviceSearchByGuid, &guidCamera, &devInfo);
  if (handle == NULL) {
    PTRACE(1, "DShow\tFindFirstDevice failed, error=" << ::GetLastError());
    return devices;
  }

  do {
    if (devInfo.hDevice != NULL) {
      PString devName(devInfo.szLegacyName);
      devices.AppendString(devName);
      PTRACE(3, "DShow\tFound capture device \""<< devName <<'"');
    }
  } while (FindNextDevice(handle, &devInfo));

  FindClose(handle);

  PTRACE_IF(2, devices.IsEmpty(), "DShow\tNo video capture devices available.");

  return devices;
}


/* As WinCE does not have the ISampleGrabber component we have to fake it
   using a custom renderer. */

struct __declspec(  uuid("{71771540-2017-11cf-ae26-0020afd79767}")  ) CLSID_MySampleGrabber;


PSampleGrabber::PSampleGrabber(HRESULT * hr)
  : CBaseVideoRenderer(__uuidof(CLSID_MySampleGrabber), NAME("Frame Sample Grabber"), NULL, hr)
  , m_sampleSize(0)
  , m_sampleData(NULL)
{
}


HRESULT PSampleGrabber::CheckMediaType(const CMediaType *media)
{
  return *media->FormatType() == FORMAT_VideoInfo && IsEqualGUID(*media->Type(), MEDIATYPE_Video) ? S_OK : E_FAIL;
}


HRESULT PSampleGrabber::ShouldDrawSampleNow(IMediaSample *sample, REFERENCE_TIME *start, REFERENCE_TIME *stop)
{
  return S_OK; // disable dropping of frames
}


HRESULT PSampleGrabber::DoRenderSample(IMediaSample *sample)
{
  m_sampleMutex.Wait();

  m_sampleSize = sample->GetActualDataLength();
  sample->GetPointer(&m_sampleData);

  m_sampleMutex.Signal();

  return  S_OK;
}


PINDEX PVideoInputDevice_DirectShow::GetCurrentBufferSize()
{
  return m_pSampleGrabber->m_sampleSize;
}


bool PVideoInputDevice_DirectShow::GetCurrentBufferData(BYTE * pData)
{
  PWaitAndSignal mutex(m_pSampleGrabber->m_sampleMutex);

  if (pData == NULL)
    return false;

  memcpy(pData, m_pSampleGrabber->m_sampleData, m_pSampleGrabber->m_sampleSize);
  return true;
}


bool PVideoInputDevice_DirectShow::BindCaptureDevice(const PString & devName)
{
  // Create an instance of the video capture filter
  CHECK_ERROR_RETURN(CoCreateInstance(CLSID_VideoCapture,
                                      NULL,
                                      CLSCTX_INPROC,
                                      IID_IBaseFilter,
                                      (LPVOID *)&m_pCaptureFilter));

  CComPtr<IPersistPropertyBag> pPropertyBag;
  CHECK_ERROR_RETURN(m_pCaptureFilter->QueryInterface(&pPropertyBag));

  VARIANT varName;
  varName.vt = VT_BSTR;
  varName.bstrVal = ::SysAllocString(devName.AsUCS2());

  CPropertyBag propBag;
  CHECK_ERROR_RETURN(propBag.Write(_T("VCapName"), &varName));
  CHECK_ERROR_RETURN(pPropertyBag->Load(&propBag, NULL));

  VariantClear(&varName);
  return true;
}


bool PVideoInputDevice_DirectShow::PlatformOpen()
{
  HRESULT hr = S_OK;
  PSampleGrabber * grabber = new PSampleGrabber(&hr);
  if (FAILED(hr)) {
    delete grabber;
    return false;
  }

  m_pSampleGrabber = grabber;

  CHECK_ERROR_RETURN(m_pGraphBuilder->AddFilter(dynamic_cast<IBaseFilter *>(grabber), L"Sampler"));

  // Find the source's output pin and the renderer's input pin
  CComPtr<IPin> pCapturePinOut;
  CHECK_ERROR_RETURN(m_pCaptureFilter->FindPin(L"Capture", &pCapturePinOut));

  CComPtr<IPin> pGrabberPinIn;
  CHECK_ERROR_RETURN(m_pSampleGrabber->FindPin(L"In", &pGrabberPinIn));

  // Connect these two filters pins
  CHECK_ERROR_RETURN(m_pGraphBuilder->Connect(pCapturePinOut, pGrabberPinIn));

  return true;
}


///////////////////////////////////////////////////////////////////////////////

#else // _WIN32_WCE

///////////////////////////////////////////////////////////////////////////////


class PComEnumerator
{
    CComPtr<ICreateDevEnum> m_pDevEnum;
    CComPtr<IEnumMoniker>   m_pClassEnum;
    CComPtr<IMoniker>       m_pMoniker;
  public:
    PComEnumerator()
    {
      // Create the system device enumerator
      CHECK_ERROR(CoCreateInstance(CLSID_SystemDeviceEnum,
                                   NULL,
                                   CLSCTX_INPROC,
                                   IID_ICreateDevEnum,
                                   (LPVOID *)&m_pDevEnum), return);
      // Create an enumerator for the video capture devices
      CHECK_ERROR(m_pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &m_pClassEnum, 0), return);

      PTRACE_IF(2, m_pClassEnum == NULL, "DShow\tNo video capture device was detected.");
    }

    bool Next()
    {
      if (m_pClassEnum == NULL)
        return false;

      m_pMoniker.Release();

      ULONG cFetched;
      return m_pClassEnum->Next(1, &m_pMoniker, &cFetched) == S_OK;
    }

    IMoniker * GetMoniker() const { return m_pMoniker; }

    PString GetMonikerName()
    {
      CComPtr<IPropertyBag> pPropBag;
      CHECK_ERROR(m_pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag), return PString::Empty());

      // Find the description or friendly name.
      VARIANT varName;
      varName.vt = VT_BSTR;
      HRESULT hr = pPropBag->Read(L"Description", &varName, NULL);
      if (FAILED(hr))
        hr = pPropBag->Read(L"FriendlyName", &varName, NULL);
      if (FAILED(hr))
        return PString::Empty();

	  char buff[512];
	  WideCharToMultiByte(CP_ACP, 0, (WCHAR *)varName.bstrVal, -1, (char *)buff, 512, 0, 0);
	  PString name(buff);
      VariantClear(&varName);
      return name;
    }
};

PStringList PVideoInputDevice_DirectShow::GetDeviceNames() const
{
  PTRACE(4, "DShow\tEnumerating Device Names");

  PStringList devices;
  unsigned duplicate = 0;

  PComEnumerator enumerator;
  while (enumerator.Next()) {
    PString name = enumerator.GetMonikerName();
    if (!name.IsEmpty()) {
      if (devices.GetValuesIndex(name) != P_MAX_INDEX)
        name.sprintf("{%u}", ++duplicate);
      devices.AppendString(name);
    }
  }

  return devices;
}


PVideoInputControl_DirectShow::PVideoInputControl_DirectShow(IAMCameraControl * _pCC)
  : t_pCC(_pCC)
{
  HRESULT  hr;
  PVideoControlInfo panInfo;
  PVideoControlInfo tiltInfo;
  PVideoControlInfo zoomInfo;

  // Retrieve information about the PTZ
  panInfo.type= ControlPan;
  hr = t_pCC->GetRange(CameraControl_Pan, &panInfo.min, &panInfo.max, &panInfo.step, &panInfo.def, &panInfo.flags);
  panInfo.current = panInfo.def;
  if (SUCCEEDED(hr)) m_info.push_back(panInfo);

  tiltInfo.type= ControlTilt;
  hr = t_pCC->GetRange(CameraControl_Tilt, &tiltInfo.min, &tiltInfo.max, &tiltInfo.step, &tiltInfo.def, &tiltInfo.flags);
  tiltInfo.current = tiltInfo.def;
  if (SUCCEEDED(hr)) m_info.push_back(tiltInfo);

  zoomInfo.type= ControlZoom;
  hr = t_pCC->GetRange(CameraControl_Zoom, &zoomInfo.min, &zoomInfo.max, &zoomInfo.step, &zoomInfo.def, &zoomInfo.flags);
  zoomInfo.current = zoomInfo.def;
  if (SUCCEEDED(hr)) m_info.push_back(zoomInfo);

}

BOOL PVideoInputControl_DirectShow::Pan(long value, bool absolute)
{
  PWaitAndSignal m(ccmutex);

  HRESULT  hr;
  PVideoControlInfo control;
  if (GetVideoControlInfo(PVideoControlInfo::ControlPan, control)) {
    long flags;
    if (absolute)
      flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
    else
      flags = KSPROPERTY_CAMERACONTROL_FLAGS_RELATIVE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;

    hr = t_pCC->Set(CameraControl_Pan, value, flags);
    if SUCCEEDED(hr) {
      hr = t_pCC->Get(CameraControl_Pan, &control.current, &flags);
      PTRACE(5, "DShow\tSet Pan to " << control.current);
      return true;
    }
    PTRACE(2, "DShow\tFailed to pan to " << value);
  }
  return false;
}

BOOL PVideoInputControl_DirectShow::Tilt(long value, bool absolute)
{
  PWaitAndSignal m(ccmutex);

  HRESULT  hr;
  PVideoControlInfo control;
  if (GetVideoControlInfo(PVideoControlInfo::ControlTilt, control)) {
    long flags;
    if (absolute)
      flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;
    else
      flags = KSPROPERTY_CAMERACONTROL_FLAGS_RELATIVE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;

    hr = t_pCC->Set(CameraControl_Tilt, value, flags);
    if SUCCEEDED(hr) {
      hr = t_pCC->Get(CameraControl_Tilt, &control.current, &flags);
      PTRACE(5, "DShow\tSet tilt to " << control.current);
      return true;
    }
    PTRACE(2, "DShow\tFailed to tilt to " << value);
  }    
  return false;
}

BOOL PVideoInputControl_DirectShow::Zoom(long value, bool absolute)
{
  PWaitAndSignal m(ccmutex);

  // 50 is 100% and 200 is 4x zoom
  if (absolute && ((value < 50) || (value > 200))) {
    PTRACE(2, "DShow\tWrong zoom value received: " << value << " must be between 50 (1x) and 200 (4x).");
    return false;
  }

  HRESULT  hr;
  PVideoControlInfo control;
  if (GetVideoControlInfo(PVideoControlInfo::ControlZoom, control)) {
    long flags;

    flags = KSPROPERTY_CAMERACONTROL_FLAGS_ABSOLUTE | KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL;

    if (!absolute) {
      control.current = control.current + value;
    }

    hr = t_pCC->Set(CameraControl_Zoom, control.current, flags);
    if SUCCEEDED(hr) {
      PTRACE(5, "DShow\tSet Zoom to " << control.current);
      SetCurrentPosition(PVideoControlInfo::ControlZoom, control.current);
      return true;
    }
    PTRACE(2, "DShow\tFailed to zoom to " << value);
  }
  return false;
}


// Implementation of CSampleGrabberCB object
class CSampleGrabberCB : public ISampleGrabberCB 
{
public:
  CSampleGrabberCB()
  {
    bufferSize = 0;
    pBuffer = NULL; 
    cInd = 0;
  }

  ~CSampleGrabberCB()
  {
    bufferSize = 0;
    delete pBuffer;
    frameready.Signal();
  }

  // Fake out any COM ref counting
  //
  STDMETHODIMP_(ULONG) AddRef() { return 2; }
  STDMETHODIMP_(ULONG) Release() { return 1; }

  // Fake out any COM QI'ing
  //
  STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
  {

    if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
    {
      *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
      return NOERROR;
    }    

    return E_NOINTERFACE;
  }


  // We don't implement this one
  //
  STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample )
  {
    return 0;
  }

  // The sample grabber is calling us back on its deliver thread.
  // This is NOT the main app thread!
  //
  STDMETHODIMP BufferCB( double dblSampleTime, BYTE * buffer, long size )
  {
    PTRACE(6, "DShow\tBuffer callback: time=" << dblSampleTime
           << ", buf=" << (void *)buffer << ", size=" << size);

    static int skipFrames = 4;
    if (cInd < skipFrames) {
      cInd++;
      return S_OK;
    }

    PWaitAndSignal m(mbuf);

    if (size == 0)
      return S_OK;

    if (bufferSize != size) {
      bufferSize = size;
      delete pBuffer;
      pBuffer = new BYTE[bufferSize];
      memset(pBuffer,0,bufferSize);
    }

    if (!buffer)
      return E_POINTER;

    memcpy(pBuffer,buffer, bufferSize);

    frameready.Signal();

    return S_OK;
  }


  long bufferSize;
  BYTE * pBuffer;

  PMutex mbuf;
  PSyncPoint frameready;
  PINDEX cInd;

};


static HRESULT GetUnconnectedPin(IBaseFilter *pFilter,   // Pointer to the filter.
                                 PIN_DIRECTION PinDir,   // Direction of the pin to find.
                                 IPin **ppPin)           // Receives a pointer to the pin.
{
  *ppPin = 0;
  IEnumPins *pEnum = 0;
  IPin *pPin = 0;
  HRESULT hr = pFilter->EnumPins(&pEnum);
  if (FAILED(hr))
    return hr;

  while (pEnum->Next(1, &pPin, NULL) == S_OK)
  {
    PIN_DIRECTION ThisPinDir;
    pPin->QueryDirection(&ThisPinDir);
    if (ThisPinDir == PinDir)
    {
      IPin *pTmp = 0;
      hr = pPin->ConnectedTo(&pTmp);
      if (SUCCEEDED(hr))  // Already connected, not the pin we want.
      {
        pTmp->Release();
      }
      else  // Unconnected, this is the pin we want.
      {
        pEnum->Release();
        *ppPin = pPin;
        return S_OK;
      }
    }
    pPin->Release();
  }

  pEnum->Release();

  // Did not find a matching pin.
  return E_FAIL;
}


static HRESULT ConnectFilters(IGraphBuilder *pGraph, // Filter Graph Manager.
                              IPin *pOut,            // Output pin on the upstream filter.
                              IBaseFilter *pDest)    // Downstream filter.
{
  if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))
    return E_POINTER;

  // Find an input pin on the downstream filter.
  IPin *pIn = 0;
  HRESULT hr = GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
  if (FAILED(hr))
    return hr;

  // Try to connect them.
  hr = pGraph->Connect(pOut, pIn);
  pIn->Release();
  return hr;
}


static HRESULT ConnectFilters(IGraphBuilder *pGraph, 
                              IBaseFilter *pSrc, 
                              IBaseFilter *pDest)
{
  if ((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL))
    return E_POINTER;

  // Find an output pin on the first filter.
  IPin *pOut = 0;
  HRESULT hr = GetUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
  if (FAILED(hr)) 
    return hr;

  hr = ConnectFilters(pGraph, pOut, pDest);
  pOut->Release();
  return hr;
}


bool PVideoInputDevice_DirectShow::BindCaptureDevice(const PString & devName)
{
  // Bind Device Filter.  We know the device because the id was passed in
  int deviceNumber = 0;
  PString devSearch = devName;
  PINDEX brace = devName.Find('{');
  if (brace != P_MAX_INDEX) {
    deviceNumber = devName.Mid(brace+1).AsInteger();
    devSearch = devName.Left(brace).Trim();
  }

  PComEnumerator enumerator;
  while (enumerator.Next()) {
    PString name = enumerator.GetMonikerName();
    if (name == devSearch && deviceNumber-- == 0) {
      enumerator.GetMoniker()->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pCaptureFilter);
      return true;
    }
  }

  return false;
}


bool PVideoInputDevice_DirectShow::PlatformOpen()
{
  // Buid the Camera Sample Grabber
  PTRACE(5, "DShow\tBuilding Sample Grabber");

  CComPtr<IBaseFilter> pGrab; 
  CHECK_ERROR_RETURN(CoCreateInstance(CLSID_SampleGrabber,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      IID_IBaseFilter,
                                      (LPVOID *)&pGrab));
  CHECK_ERROR_RETURN(m_pGraphBuilder->AddFilter(pGrab, L"Sample Grabber"));
  CHECK_ERROR_RETURN(pGrab->QueryInterface(IID_ISampleGrabber, (void **)&m_pSampleGrabber));

  AM_MEDIA_TYPE mt;
  ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
  mt.majortype = MEDIATYPE_Video;
  mt.subtype = m_selectedGUID;
  CHECK_ERROR_RETURN(m_pSampleGrabber->SetMediaType(&mt));

  CHECK_ERROR_RETURN(m_pSampleGrabber->SetBufferSamples(true));

  PTRACE(5, "DShow\tSetting Sample Grabber Callback");
  m_pSampleGrabberCB = new CSampleGrabberCB();
  CHECK_ERROR_RETURN(m_pSampleGrabber->SetCallback(m_pSampleGrabberCB, 1));

  PTRACE(5, "DShow\tConnect sample grabber to camera");
  CHECK_ERROR_RETURN(ConnectFilters(m_pGraphBuilder, m_pCameraOutPin, pGrab));

  // Set the NULL Renderer
  PTRACE(5, "DShow\tBuilding NULL output filter");
  CHECK_ERROR_RETURN(CoCreateInstance(CLSID_NullRenderer,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      IID_IBaseFilter,
                                      (LPVOID *)&m_pNullRenderer));

  CHECK_ERROR_RETURN(m_pGraphBuilder->AddFilter(m_pNullRenderer, L"NULL Output"));

  PTRACE(5, "DShow\tConnect Null Render to Grab filter Pin");
  CHECK_ERROR_RETURN(ConnectFilters(m_pGraphBuilder, pGrab, m_pNullRenderer));

  PTRACE(5, "DShow\tChecking image flip");
  CHECK_ERROR_RETURN(m_pSampleGrabber->GetConnectedMediaType(&mt));

  VIDEOINFOHEADER * pvih = (VIDEOINFOHEADER *)mt.pbFormat;
  if (pvih->bmiHeader.biHeight > 0) {
    nativeVerticalFlip = true;
    PTRACE(3, "DShow\tImage up side down");
  }

  // Query for camera controls
  PVideoControlInfo info;
  if (FAILED(m_pCaptureFilter->QueryInterface(IID_IAMCameraControl, (void **)&m_pCameraControls))) {
    PTRACE(3, "DShow\tCamera " << deviceName << " does not support Camera Controls.");
    m_pCameraControls = NULL;
  }
  else {
    if (FAILED(m_pCameraControls->GetRange(CameraControl_Pan, &info.min, &info.max, &info.step, &info.def, &info.flags))) {
      PTRACE(4, "DShow\tCamera " << deviceName << " does not support Pan. Controls DISABLED");
    } 
    if (FAILED(m_pCameraControls->GetRange(CameraControl_Tilt, &info.min, &info.max, &info.step, &info.def, &info.flags))) {
      PTRACE(4, "DShow\tCamera " << deviceName << " does not support Tilt. Controls DISABLED");
    } 
    if (FAILED(m_pCameraControls->GetRange(CameraControl_Zoom, &info.min, &info.max, &info.step, &info.def, &info.flags))) {
      PTRACE(4, "DShow\tCamera " << deviceName << " does not support zoom. Controls DISABLED");
    }

    PTRACE(3, "DShow\tCamera " << deviceName << " supports Camera Controls. Controls ENABLED");
  }

  return true;
}


PINDEX PVideoInputDevice_DirectShow::GetCurrentBufferSize()
{
  return m_maxFrameBytes;
}


bool PVideoInputDevice_DirectShow::GetCurrentBufferData(BYTE * data)
{
  CSampleGrabberCB * cb = (CSampleGrabberCB *)&*m_pSampleGrabberCB;

  if (!cb->frameready.Wait(2000)) {
    PTRACE(1, "DShow\tTimeout awaiting next frame");
    return false;
  }

  if (PAssertNULL(cb->pBuffer) == NULL)
    return false;

  if (cb->bufferSize <= m_maxFrameBytes) {
    cb->mbuf.Wait();
    memcpy(data, cb->pBuffer, cb->bufferSize);
    cb->mbuf.Signal();
  } else
    PTRACE(1, "DShow\tNot copying, since bufferSize is greater than m_maxFrameBytes (" << cb->bufferSize << " > " << m_maxFrameBytes << ")");

  return true;
}


PVideoInputControl * PVideoInputDevice_DirectShow::GetVideoInputControls()
{
  if (m_pCameraControls != NULL)
    return new PVideoInputControl_DirectShow(m_pCameraControls);
  return NULL; 
}


#endif  // _WIN32_WCE


#else

  #ifdef _MSC_VER
    #pragma message("Direct Show video support DISABLED")
  #endif

#endif  // P_DIRECTSHOW
