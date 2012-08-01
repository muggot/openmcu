/*
 * vidinput_directx.cxx
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

#define P_FORCE_STATIC_PLUGIN

#include <ptlib.h>

#ifdef P_DIRECTSHOW

#include "ptlib/msos/ptlib/vidinput_directx.h"

#if defined(_WIN32) && !defined(P_FORCE_STATIC_PLUGIN)
#error "vidinput_directx.cxx must be compiled without precompiled headers"
#endif

#ifdef _MSC_VER
#ifndef _WIN32_WCE
#pragma comment(lib,"strmbase.lib")
#pragma comment(lib,"strmiids.lib")
#endif
#endif

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

static HRESULT SetDevice(const PString & devName, IBaseFilter ** ppSrcFilter);
static const char *ErrorMessage(HRESULT hr);
static char *BSTR_to_ANSI(BSTR pSrc);
static GUID pwlib_format_to_media_format(const char *format);
static const char *media_format_to_pwlib_format(const GUID guid);
static char *guid_to_string(const GUID guid);

PCREATE_VIDINPUT_PLUGIN(DirectShow);

PVideoInputDevice_DirectShow::PVideoInputDevice_DirectShow()
{
  PTRACE(1,"PVidDirectShow\tPVideoInputDevice_DirectShow: constructor" );

  ::CoInitialize(NULL);

  tempFrame = NULL;

  pSrcFilter = NULL;
  pGrabberFilter = NULL;
  pNullFilter = NULL;
  pGraph = NULL;
  pMC = NULL;
  pME = NULL;
  pCapture = NULL;
  pGrabber = NULL;

  flipVertical = FALSE;
  isCapturingNow = FALSE;
  capturing_duration = 10000; // arbitrary large value suffices
}

PVideoInputDevice_DirectShow::~PVideoInputDevice_DirectShow()
{
    Close();
    ::CoUninitialize();
}

HRESULT PVideoInputDevice_DirectShow::Initialize_Interfaces()
{
    HRESULT hr;

    PTRACE(1,"PVidDirectShow\tInitialize_Interfaces()");

    // Create the filter graph
    hr = CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                           IID_IGraphBuilder, (void **) &pGraph);
    if (FAILED(hr))
    {
        PTRACE(1,"PVidDirectShow\tFailed to create instance FilterGraph: " << ErrorMessage(hr));
        return hr;
    }

    // Create the capture graph builder
    hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC_SERVER,
                           IID_ICaptureGraphBuilder2, (void **) &pCapture);
    if (FAILED(hr))
    {
        PTRACE(1,"PVidDirectShow\tFailed to create instance CaptureGraphBuilder2: " << ErrorMessage(hr));
        return hr;
    }

    // Create the Sample Grabber Filter.
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
			  IID_IBaseFilter, (void**) &pGrabberFilter);
    if (FAILED(hr))
    {
        PTRACE(1,"PVidDirectShow\tFailed to create instance SampleGrabber: " << ErrorMessage(hr));
        return hr;
    }

    // Create the Null Renderer Filter.
    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
			  IID_IBaseFilter, (void**) &pNullFilter);
    if (FAILED(hr))
    {
        PTRACE(1,"PVidDirectShow\tFailed to create instance SampleGrabber: " << ErrorMessage(hr));
        return hr;
    }

    // Obtain interfaces for media control and Video Window
    hr = pGraph->QueryInterface(IID_IMediaControl,(LPVOID *) &pMC);
    if (FAILED(hr))
    {
        PTRACE(1,"PVidDirectShow\tFailed to query interface MediaControl: " << ErrorMessage(hr));
        return hr;
    }

    hr = pGraph->QueryInterface(IID_IMediaEvent, (LPVOID *) &pME);
    if (FAILED(hr))
    {
        PTRACE(1,"PVidDirectShow\tFailed to query interface MediaEvent: " << ErrorMessage(hr));
        return hr;
    }

    // Attach the filter graph to the capture graph
    hr = pCapture->SetFiltergraph(pGraph);
    if (FAILED(hr))
    {
        PTRACE(1,"PVidDirectShow\tFailed to set capture filter graph: " << ErrorMessage(hr));
        return hr;
    }

    //Add the filter to the graph
    hr = pGraph->AddFilter(pGrabberFilter, L"Sample Grabber");
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tCouldn't add the grabber filter to the graph: " << ErrorMessage(hr));
	return hr;
    }

    // Obtain interfaces for Sample Grabber
    pGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
    hr = pGrabber->SetBufferSamples(TRUE);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tFailed to SetBufferSamples: " << ErrorMessage(hr));
	return hr;
    }

    hr = pGrabber->SetOneShot(FALSE);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tFailed to SetOneShot: " << ErrorMessage(hr));
	return hr;
    }

    //Set the Sample Grabber callback
    //0: SampleCB (the buffer is the original buffer, not a copy)
    //1: BufferCB (the buffer is a copy of the original buffer)
#if 0
    hr = pGrabber->SetCallback(this, 0);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tFailed to SetCallback: " << ErrorMessage(hr));
	return hr;
    }
#endif

    return hr;
}

BOOL PVideoInputDevice_DirectShow::InitialiseCapture()
{
    HRESULT hr;

    PTRACE(1,"PVidDirectShow\tInitializeCapture()");

    hr = Initialize_Interfaces();
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tFailed to initialize interfaces: " << ErrorMessage(hr));
	return FALSE;
    }
    hr = SetDevice(deviceName, &pSrcFilter);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tFailed to select a device: " << ErrorMessage(hr));
	return FALSE;
    }

    // Add Capture filter to our graph.
    hr = pGraph->AddFilter(pSrcFilter, L"Video Capture");
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tCouldn't add the capture filter to the graph: " << ErrorMessage(hr));
        return FALSE;
    }

    // Add the filter to our graph
    hr = pGraph->AddFilter(pNullFilter, L"Null Renderer");
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tCouldn't add the grabber filter to the graph: " << ErrorMessage(hr));
	return hr;
    }

    return TRUE;
}


PStringList PVideoInputDevice_DirectShow::GetInputDeviceNames()
{
    HRESULT hr;
    IBaseFilter * pSrc = NULL;
    IMoniker *pMoniker =NULL;
    ICreateDevEnum *pDevEnum =NULL;
    IEnumMoniker *pClassEnum = NULL;
    ULONG cFetched;
    PStringList list;

    PTRACE(1,"PVidDirectShow\tGetInputDeviceNames()");

    ::CoInitialize(NULL);

    // Create the system device enumerator
    hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                           IID_ICreateDevEnum, (void **) &pDevEnum);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tCouldn't create system enumerator. " << ErrorMessage(hr));
	::CoUninitialize();
        return list;
    }

    // Create an enumerator for the video capture devices
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tGetInputDeviceNames() Couldn't create class enumerator. " << ErrorMessage(hr));
	::CoUninitialize();
        return list;
    }

    if (pClassEnum == NULL)
    {
        PTRACE(1, "PVidDirectShow\tGetInputDeviceNames() No video capture device was detected.");
	::CoUninitialize();
        return list;
    }

    while (1)
    {
	// Get the next device
	hr = pClassEnum->Next(1, &pMoniker, &cFetched);
	if (hr != S_OK)
	{
	    PTRACE(4, "PVidDirectShow\tGetInputDeviceNames() No more video capture device");
	    break;
	}

	// Get the property bag
	IPropertyBag *pPropBag;

	hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
	if (FAILED(hr))
	{
	    pMoniker->Release();
	    continue;
	}

	// Find the description or friendly name.
	VARIANT DeviceName;
	DeviceName.vt = VT_BSTR;
	hr = pPropBag->Read(L"Description", &DeviceName, NULL);
	if (FAILED(hr))
	    hr = pPropBag->Read(L"FriendlyName", &DeviceName, NULL);
	if (SUCCEEDED(hr))
	{
	    char *pDeviceName = BSTR_to_ANSI(DeviceName.bstrVal);
	    if (pDeviceName)
	    {
		PTRACE(4, "PVidDirectShow\tGetInputDeviceNames() Found this capture device '"<< pDeviceName <<"'");
		list.AppendString(pDeviceName);
		free(pDeviceName);
	    }
	}

	pPropBag->Release();
	pMoniker->Release();
	// Next Device
    }

    ::CoUninitialize();
    return list;
}

BOOL PVideoInputDevice_DirectShow::Open(const PString & devName, BOOL startImmediate)
{
    PTRACE(1,"PVidDirectShow\tOpen("<<devName<<"," << startImmediate<<")");

    /* FIXME: If the device is already open, close it */
    if (IsOpen())
	Close();

    deviceName = devName;

    if (!InitialiseCapture())
	return FALSE;

    ListSupportedFormats();
    GetDefaultFormat();

    if (startImmediate)
	return Start();

    return TRUE;
}

BOOL PVideoInputDevice_DirectShow::IsOpen()
{
    PTRACE(1,"PVidDirectShow\tIsOpen()");

    return pCapture != NULL;
}

BOOL PVideoInputDevice_DirectShow::Close()
{
    HRESULT hr;

    if (!IsOpen())
	return FALSE;

    hr = pGrabber->SetCallback(NULL, 0);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tFailed to remove the callback: " << ErrorMessage(hr));
	return hr;
    }

    if (pMC)
	pMC->StopWhenReady();

    SAFE_RELEASE(pMC);
    SAFE_RELEASE(pME);
    SAFE_RELEASE(pNullFilter)
    SAFE_RELEASE(pGrabberFilter)
    SAFE_RELEASE(pSrcFilter)
    SAFE_RELEASE(pGraph);
    SAFE_RELEASE(pCapture);
    SAFE_RELEASE(pGrabber);


    return TRUE;
}

BOOL PVideoInputDevice_DirectShow::Start()
{
    HRESULT hr;
    long evCode;
    unsigned int count;

    PTRACE(1,"PVidDirectShow\tStart()");

    if (IsCapturing())
	return TRUE;

    // http://msdn2.microsoft.com/en-us/library/ms784859.aspx
    hr = pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                pSrcFilter,	/* Source Filter */
				NULL,		/* Intermediate Filter */
				pGrabberFilter	/* Sink Filter */
				);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tCouldn't render the video capture stream: " << ErrorMessage(hr));
        return hr;
    }

    // Start previewing video data
    hr = pMC->Run();
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tCouldn't run the graph: " << ErrorMessage(hr));
        return FALSE;
    }

    hr = pME->WaitForCompletion(INFINITE, &evCode);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tCouldn't wait for completion: " << ErrorMessage(hr));
        return FALSE;
    }

    /*
     * Even after a WaitForCompletion, the webcam is not available, so wait
     * until the server give us a frame
     */
    count = 0;
    while (count < 100)
    {
	long cbBuffer;
	hr = pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
	if (hr == S_OK && cbBuffer > 0)
	    break;
	else if (hr == VFW_E_WRONG_STATE)
	{
	    /* Not available */
	    PThread::Current()->Sleep(100);
	}
	else
	{
	    PTRACE(1, "PVidDirectShow\tWhile waiting the webcam to be ready, we have got this error: " << ErrorMessage(hr));
	    PThread::Current()->Sleep(10);
	}
	count++;
    }

#if 0
    SetHue((unsigned)-1);
    SetWhiteness((unsigned)-1);
    SetColour((unsigned)-1);
    SetContrast((unsigned)-1);
    SetBrightness((unsigned)-1);
#endif

    isCapturingNow = TRUE;

    return TRUE;
}

BOOL PVideoInputDevice_DirectShow::Stop()
{
    HRESULT hr;

    PTRACE(1,"PVidDirectShow\tStop()");

    if (IsCapturing())
	return FALSE;

    if (pMC)
	pMC->StopWhenReady();

    isCapturingNow = FALSE;

    return TRUE;
}


BOOL PVideoInputDevice_DirectShow::IsCapturing()
{
    return isCapturingNow;
}

/*
 * Flip an image vertically in place
 *
 */
void PVideoInputDevice_DirectShow::FlipVertical(BYTE *buffer)
{
    unsigned int bytesPerLine = frameBytes / frameHeight;
    BYTE templine[bytesPerLine];
    BYTE *s = buffer;
    BYTE *d = buffer + frameBytes - bytesPerLine;
    unsigned int i;

    for (i=0; i<frameHeight/2; i++)
    {
	memcpy(templine, s, bytesPerLine);
	memcpy(s, d, bytesPerLine);
	memcpy(d, templine, bytesPerLine);
	s += bytesPerLine;
	d -= bytesPerLine;
    }
}

/*
 *
 *
 */
BOOL PVideoInputDevice_DirectShow::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{
  m_pacing.Delay(1000/GetFrameRate());
  return GetFrameDataNoDelay(buffer, bytesReturned);
}

BOOL PVideoInputDevice_DirectShow::GetFrameDataNoDelay(BYTE *destFrame, PINDEX * bytesReturned)
{
    HRESULT hr;
    long cbBuffer = frameBytes;

    if (converter != NULL)
    {
	if (tempFrame == NULL)
	    return FALSE;

	hr = pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
	if (FAILED(hr))
	{
	    PTRACE(1, "PVidDirectShow\tFailed to get the current buffer size: " << ErrorMessage(hr));
	    return FALSE;
	}
	if (frameBytes < cbBuffer)
	{
	    PTRACE(1, "PVidDirectShow\tBuffer too short (needed "<< cbBuffer << "  got " << frameBytes);
	    return FALSE;
	}

	hr = pGrabber->GetCurrentBuffer(&cbBuffer, (long*)tempFrame);
	if (FAILED(hr))
	{
	    PTRACE(1, "PVidDirectShow\tFailed to get the current buffer: " << ErrorMessage(hr));
	    return FALSE;
	}

	if (flipVertical)
	    FlipVertical((BYTE *)tempFrame);
	converter->Convert((BYTE *)tempFrame, destFrame, cbBuffer, bytesReturned);
    }
    else
    {
	hr = pGrabber->GetCurrentBuffer(&cbBuffer, (long*)destFrame);
	if (FAILED(hr))
	{
	    PTRACE(1, "PVidDirectShow\tFailed to get the current buffer: " << ErrorMessage(hr));
	    return FALSE;
	}

	if (flipVertical)
	    FlipVertical(destFrame);

	*bytesReturned = cbBuffer;

    }

    return TRUE;
}

PINDEX PVideoInputDevice_DirectShow::GetMaxFrameBytes()
{
 //   PTRACE(1,"PVidDirectShow\tGetMaxFrameBytes() return " << GetMaxFrameBytesConverted(frameBytes));
    return GetMaxFrameBytesConverted(frameBytes);
}

/*
 *
 * Set the FrameRate, FrameSize, ...
 *
 *
 */
/*
 * Change Colourspace AND FrameSize by looking if the resolution is supported by the hardware.
 *
 * For example a Logitech Pro 4000:
 *   Fmt[0] = (RGB24, 320x240, 30fps)
 *   Fmt[1] = (RGB24, 640x480, 15fps)
 *   Fmt[2] = (RGB24, 352x288, 30fps)
 *   Fmt[3] = (RGB24, 176x144, 30fps)
 *   Fmt[4] = (RGB24, 160x120, 30fps)
 *   Fmt[5] = (YUV420P, 320x240, 30fps)
 *   Fmt[6] = (YUV420P, 640x480, 15fps)
 *   Fmt[7] = (YUV420P, 352x288, 30fps)
 *   Fmt[8] = (YUV420P, 176x144, 30fps)
 *   Fmt[9] = (YUV420P, 160x120, 30fps)
 *   Fmt[10] = (IYUV, 320x240, 30fps)
 *   Fmt[11] = (IYUV, 640x480, 15fps)
 *   Fmt[12] = (IYUV, 352x288, 30fps)
 *   Fmt[13] = (IYUV, 176x144, 30fps)
 *   Fmt[14] = (IYUV, 160x120, 30fps)
 *
 * For example a Logitech Fusion that support MPJEG in hardware, doesn't return a MJPEG format :(
 *  Fmt[0] = (RGB24, 320x240, 15fps)
 *  Fmt[1] = (RGB24, 176x144, 30fps)
 *  Fmt[2] = (RGB24, 160x120, 30fps)
 *  Fmt[3] = (RGB24, 352x288, 30fps)
 *  Fmt[4] = (RGB24, 432x240, 30fps)
 *  Fmt[5] = (RGB24, 480x360, 30fps)
 *  Fmt[6] = (RGB24, 512x288, 30fps)
 *  Fmt[7] = (RGB24, 640x360, 30fps)
 *  Fmt[8] = (RGB24, 640x480, 15fps)
 *  Fmt[9] = (RGB24, 704x576, 15fps)
 *  Fmt[10] = (RGB24, 864x480, 15fps)
 *  Fmt[11] = (RGB24, 960x720, 15fps)
 *  Fmt[12] = (RGB24, 1024x576, 10fps)
 *  Fmt[13] = (RGB24, 1280x960, 7.5fps)
 *  Fmt[14] = (YUV420P, 320x240, 15fps)
 *  Fmt[15] = (YUV420P, 176x144, 30fps)
 *  Fmt[16] = (YUV420P, 160x120, 30fps)
 *  Fmt[17] = (YUV420P, 352x288, 30fps)
 *  Fmt[18] = (YUV420P, 432x240, 30fps)
 *  Fmt[19] = (YUV420P, 480x360, 30fps)
 *  Fmt[20] = (YUV420P, 512x288, 30fps)
 *  Fmt[21] = (YUV420P, 640x360, 30fps)
 *  Fmt[22] = (YUV420P, 640x480, 15fps)
 *  Fmt[23] = (YUV420P, 704x576, 15fps)
 *  Fmt[24] = (YUV420P, 864x480, 15fps)
 *  Fmt[25] = (YUV420P, 960x720, 15fps)
 *  Fmt[26] = (YUV420P, 1024x576, 10fps)
 *  Fmt[27] = (YUV420P, 1280x960, 7.5fps)
 */
BOOL PVideoInputDevice_DirectShow::SetFormat(const PString &wanted_format, int width, int height, int fps)
{
    HRESULT hr;
    IAMStreamConfig *pStreamConfig;
    AM_MEDIA_TYPE *pMediaFormat;
    int iCount, iSize;
    VIDEO_STREAM_CONFIG_CAPS scc;
    unsigned int i;
    BOOL was_capturing = FALSE;
    OAFilterState filterState;

    PTRACE(4, "PVidDirectShow\tSetFormat(\""
	      << (wanted_format.IsEmpty()?"Not changed":wanted_format) <<"\", "
	      << width<<"x"<<height <<", "
	      << fps <<"fps)");

    const GUID wanted_guid_format = pwlib_format_to_media_format(wanted_format);
    if (!wanted_format.IsEmpty() && wanted_guid_format == MEDIATYPE_NULL)
    {
	PTRACE(4, "PVidDirectShow\tColorspace not supported ("<< wanted_format << ")");
	return FALSE;
    }

    hr = pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
	    			 pSrcFilter, IID_IAMStreamConfig, (void **)&pStreamConfig);

    if (FAILED(hr))
    {
	PTRACE(1, "PVidDirectShow\tFailed to find StreamConfig Video interface: " << ErrorMessage(hr));
	return FALSE;
    }

    hr = pStreamConfig->GetNumberOfCapabilities(&iCount, &iSize);
    if (FAILED(hr))
    {
	PTRACE(1, "PVidDirectShow\tFailed to GetNumberOfCapabilities: " << ErrorMessage(hr));
	pStreamConfig->Release();
	return FALSE;
    }

    /* Sanity check: just to be sure that the Streamcaps is a VIDEOSTREAM and not AUDIOSTREAM */
    if (sizeof(scc) != iSize)
    {
	PTRACE(1, "PVidDirectShow\tBad Capapabilities (not a  VIDEO_STREAM_CONFIG_CAPS)");
	pStreamConfig->Release();
	return FALSE;
    }

    for (i=0; i<iCount; i++, DeleteMediaType(pMediaFormat))
    {
	pMediaFormat = NULL;
	hr = pStreamConfig->GetStreamCaps(i, &pMediaFormat, (BYTE *)&scc);
	if (FAILED(hr))
	{
	    PTRACE(1, "PVidDirectShow\tFailed to GetStreamCaps(" << i <<"): " << ErrorMessage(hr));
	    continue;
	}

	if (!((pMediaFormat->formattype == FORMAT_VideoInfo)     &&
             (pMediaFormat->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
	     (pMediaFormat->pbFormat != NULL)))
	    continue;

	VIDEOINFOHEADER *VideoInfo = (VIDEOINFOHEADER *)pMediaFormat->pbFormat;
	BITMAPINFOHEADER *BitmapInfo = &(VideoInfo->bmiHeader);
	const char *current_format = media_format_to_pwlib_format(pMediaFormat->subtype);
	const int maxfps = (int)(10000000.0/VideoInfo->AvgTimePerFrame);

	if (!wanted_format.IsEmpty() && (wanted_guid_format != pMediaFormat->subtype))
	    continue;

	if (width && BitmapInfo->biWidth != width)
	    continue;

	if (width && BitmapInfo->biHeight != height)
	    continue;

	if (fps && fps <= maxfps)
	    VideoInfo->AvgTimePerFrame = (LONGLONG) (10000000.0 / (double)fps);

	/* We have match a goo format, Use it to change the format */
	PTRACE(1,"PVidDirectShow\tUsing setting ["<< i << "] = ("
		<< current_format << ", "
		<< BitmapInfo->biWidth << "x" << BitmapInfo->biHeight << ", "
		<< fps << "fps, max:" << maxfps << "fps)");

#if 1

        if (pMC)
        {
	    hr = pMC->GetState(1000, &filterState);
	    if (FAILED(hr))
	        PTRACE(1, "PVidDirectShow\tGetState failed: " << ErrorMessage(hr));
	    pMC->StopWhenReady();
        }

        hr = pStreamConfig->SetFormat(pMediaFormat);
	if (FAILED(hr))
	{
	    PTRACE(1, "PVidDirectShow\tFailed to setFormat: " << ErrorMessage(hr));
	    if (hr != VFW_E_INVALIDMEDIATYPE)
		continue;

	    PTRACE(1, "PVidDirectShow\tRetrying ...");
	    was_capturing = isCapturingNow;
	    Close();
	    Open(deviceName, FALSE);
	    hr = pStreamConfig->SetFormat(pMediaFormat);
	    if (FAILED(hr))
	    {
		PTRACE(1, "PVidDirectShow\tFailed to setFormat (Try #2 graph deconnected): " << ErrorMessage(hr));
		continue;
	    }
	    if (was_capturing)
		Start();
	}

        if (pMC)  {
            if (filterState==State_Running)
            {
                pMC->Run();
            } 
            else if (filterState==State_Paused)
            {
                pMC->Pause();
            }
        }
        PTRACE(1, "PVidDirectShow\tOk");
	
        if (pMediaFormat->subtype == MEDIASUBTYPE_RGB32 ||
	    pMediaFormat->subtype == MEDIASUBTYPE_RGB24 ||
	    pMediaFormat->subtype == MEDIASUBTYPE_RGB565 ||
	    pMediaFormat->subtype == MEDIASUBTYPE_RGB555)
	{
	    flipVertical = TRUE;
	}
	else
	{
	    flipVertical = FALSE;
	}
#endif

#if 0
	hr = pGrabber->SetMediaType(pMediaFormat);
	if (FAILED(hr))
	{
	    PTRACE(1, "PVidDirectShow\tFailed to setFormat on pGrabber: " << ErrorMessage(hr));
	}
#endif

	DeleteMediaType(pMediaFormat);
	pStreamConfig->Release();
	return TRUE;
    }

    pStreamConfig->Release();
    return FALSE;
}


BOOL PVideoInputDevice_DirectShow::SetFrameSize(unsigned width, unsigned height)
{
    PTRACE(1,"PVidDirectShow\tSetFrameSize(" << width << ", " << height << ")");

    if (!SetFormat(colourFormat, width, height, frameRate))
	return FALSE;

    PTRACE(1,"PVidDirectShow\tSetFrameSize " << width << "x" << height << " is suported in hardware");

    if (!PVideoDevice::SetFrameSize(width, height))
	return FALSE;

    frameBytes = CalculateFrameBytes(frameWidth, frameHeight, colourFormat);

    if (tempFrame)
	free(tempFrame);
    tempFrame = (char *)malloc(frameBytes);
    if (tempFrame == NULL)
    {
	PTRACE(1,"PVidDirectShow\tNot enought memory to allocate tempFrame ("<<frameBytes<<")");
	return FALSE;
    }

    PTRACE(4,"PVidDirectShow\tset frame size " << width << "x" << height << "  frameBytes="<<frameBytes);
    return TRUE;
}

BOOL PVideoInputDevice_DirectShow::SetFrameRate(unsigned rate)
{
    PTRACE(1,"PVidDirectShow\tSetFrameRate("<<rate<<"fps)");

    if (rate < 1)
	rate = 1;
    else if (rate > 50)
	rate = 50;

    if (!SetFormat(colourFormat, frameWidth, frameHeight, rate))
	return FALSE;

    return PVideoDevice::SetFrameRate(rate);
}

BOOL PVideoInputDevice_DirectShow::SetColourFormat(const PString & colourFmt)
{
    PTRACE(1,"PVidDirectShow\tSetColourFormat("<<colourFmt<<")");

    if (!SetFormat(colourFmt, frameWidth, frameHeight, frameRate))
	return FALSE;

    if (!PVideoDevice::SetColourFormat(colourFmt))
	return FALSE;

    return TRUE;
}



/*
 *
 * Get brightness, contrast, hue, saturation
 *
 *
 */
BOOL PVideoInputDevice_DirectShow::GetControlCommon(long control, int *newValue)
{
    IAMVideoProcAmp *pVideoProcAmp;
    long Min, Max, Stepping, Def, CapsFlags, Val;
    HRESULT hr;

    hr = pSrcFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pVideoProcAmp);
    if (FAILED(hr))
    {
	PTRACE(4, "PVidDirectShow\tFailed to find VideoProcAmp interface: " << ErrorMessage(hr));
	return FALSE;
    }

    hr = pVideoProcAmp->GetRange(control, &Min, &Max, &Stepping, &Def, &CapsFlags);
    if (FAILED(hr))
    {
	PTRACE(4, "PVidDirectShow\tFailed to getRange interface on " << control << " : " << ErrorMessage(hr));
	pVideoProcAmp->Release();
	return FALSE;
    }

    hr = pVideoProcAmp->Get(control, &Val, &CapsFlags);
    if (FAILED(hr))
    {
	PTRACE(4, "PVidDirectShow\tFailed to setRange interface on " << control << " : " << ErrorMessage(hr));
	Val = Def;
    }

    if (CapsFlags == VideoProcAmp_Flags_Auto)
	*newValue = -1;
    else
	*newValue = ((Val - Min) * 65536) / ((Max-Min));

    pVideoProcAmp->Release();
    return TRUE;
}

int PVideoInputDevice_DirectShow::GetBrightness()
{
  return GetControlCommon(VideoProcAmp_Brightness, &frameBrightness);
}

int PVideoInputDevice_DirectShow::GetWhiteness()
{
  return GetControlCommon(VideoProcAmp_Gamma, &frameWhiteness);
}

int PVideoInputDevice_DirectShow::GetColour()
{
  return GetControlCommon(VideoProcAmp_Saturation, &frameColour);
}

int PVideoInputDevice_DirectShow::GetContrast()
{
  return GetControlCommon(VideoProcAmp_Contrast, &frameContrast);
}

int PVideoInputDevice_DirectShow::GetHue()
{
  return GetControlCommon(VideoProcAmp_Hue, &frameHue);
}

BOOL PVideoInputDevice_DirectShow::GetParameters(int *whiteness, int *brightness, int *colour, int *contrast, int *hue)
{
  if (!IsOpen())
    return FALSE;

  frameWhiteness = -1;
  frameBrightness = -1;
  frameColour = -1;
  frameContrast = -1;
  frameHue = -1;
  GetWhiteness();
  GetBrightness();
  GetColour();
  GetContrast();
  GetHue();

  PTRACE(4, "PVidDirectShow\tGetWhiteness() = " << frameWhiteness);
  PTRACE(4, "PVidDirectShow\tGetBrighness() = " << frameBrightness);
  PTRACE(4, "PVidDirectShow\tGetColour() = " << frameColour);
  PTRACE(4, "PVidDirectShow\tGetContrast() = " << frameContrast);
  PTRACE(4, "PVidDirectShow\tGetHue() = " << frameHue);

  *whiteness  = frameWhiteness;
  *brightness = frameBrightness;
  *colour     = frameColour;
  *contrast   = frameContrast;
  *hue        = frameHue;

  return TRUE;
}


/*
 *
 * Set brightness, contrast, hue, saturation
 *
 *
 */
BOOL PVideoInputDevice_DirectShow::SetControlCommon(long control, int newValue)
{
    IAMVideoProcAmp *pVideoProcAmp;
    long Min, Max, Stepping, Def, CapsFlags;
    HRESULT hr;

    PTRACE(1, "PVidDirectShow\tSetControl() = " << newValue);

    hr = pSrcFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pVideoProcAmp);
    if (FAILED(hr))
    {
	PTRACE(4, "PVidDirectShow\tFailed to find VideoProcAmp interface: " << ErrorMessage(hr));
	return FALSE;
    }

    hr = pVideoProcAmp->GetRange(control, &Min, &Max, &Stepping, &Def, &CapsFlags);
    if (FAILED(hr))
    {
	PTRACE(4, "PVidDirectShow\tFailed to getRange interface on " << control << " : " << ErrorMessage(hr));
	pVideoProcAmp->Release();
	return FALSE;
    }

    if (newValue == -1)
	hr = pVideoProcAmp->Set(control, 0, VideoProcAmp_Flags_Auto);
    else
    {
	long ValScaled = Min + ((Max-Min) * newValue) / 65536;
	hr = pVideoProcAmp->Set(control, ValScaled, VideoProcAmp_Flags_Manual);
    }
    if (FAILED(hr))
	PTRACE(4, "PVidDirectShow\tFailed to setRange interface on " << control << " : " << ErrorMessage(hr));

    pVideoProcAmp->Release();
    return TRUE;
}

BOOL PVideoInputDevice_DirectShow::SetBrightness(unsigned newBrightness)
{
    if (!SetControlCommon(VideoProcAmp_Brightness, newBrightness))
	return FALSE;

    frameBrightness = newBrightness;

    return TRUE;
}

BOOL PVideoInputDevice_DirectShow::SetColour(unsigned newColour)
{
    if (!SetControlCommon(VideoProcAmp_Saturation, newColour))
	return FALSE;

    frameColour = newColour;

    return TRUE;
}

BOOL PVideoInputDevice_DirectShow::SetContrast(unsigned newContrast)
{
    if (!SetControlCommon(VideoProcAmp_Contrast, newContrast))
	return FALSE;

    frameContrast = newContrast;

    return TRUE;
}

BOOL PVideoInputDevice_DirectShow::SetHue(unsigned newHue)
{
    if (!SetControlCommon(VideoProcAmp_Hue, newHue))
	return FALSE;

    frameHue = newHue;

    return TRUE;
}

BOOL PVideoInputDevice_DirectShow::SetWhiteness(unsigned newWhiteness)
{
    if (!SetControlCommon(VideoProcAmp_Gamma, newWhiteness))
	return FALSE;

    frameWhiteness = newWhiteness;

    return TRUE;
}


/*
 *
 *
 */
BOOL PVideoInputDevice_DirectShow::ListSupportedFormats()
{
    HRESULT hr;
    IAMStreamConfig *pStreamConfig;
    AM_MEDIA_TYPE *pMediaFormat;
    int iCount, iSize;
    VIDEO_STREAM_CONFIG_CAPS scc;
    unsigned int i;

    PTRACE(1, "PVidDirectShow\tListSupportedFormats()");

    hr = pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
	    			 pSrcFilter, IID_IAMStreamConfig, (void **)&pStreamConfig);
    if (FAILED(hr))
    {
	PTRACE(4, "PVidDirectShow\tFailed to find StreamConfig Video interface: " << ErrorMessage(hr));
	return FALSE;
    }

    hr = pStreamConfig->GetNumberOfCapabilities(&iCount, &iSize);
    if (FAILED(hr))
    {
	PTRACE(1, "PVidDirectShow\tFailed to GetNumberOfCapabilities: " << ErrorMessage(hr));
	pStreamConfig->Release();
	return FALSE;
    }

    /* Sanity check: just to be sure that the Streamcaps is a VIDEOSTREAM and not AUDIOSTREAM */
    if (sizeof(scc) != iSize)
    {
	PTRACE(1, "PVidDirectShow\tBad Capapabilities (not a  VIDEO_STREAM_CONFIG_CAPS)");
	pStreamConfig->Release();
	return FALSE;
    }

    for (i=0; i<iCount; i++)
    {
	pMediaFormat = NULL;
	hr = pStreamConfig->GetStreamCaps(i, &pMediaFormat, (BYTE *)&scc);
	if (FAILED(hr))
	{
	    PTRACE(1, "PVidDirectShow\tFailed to GetStreamCaps(" << i <<"): " << ErrorMessage(hr));
	    continue;
	}

	if ((pMediaFormat->formattype == FORMAT_VideoInfo)     &&
            (pMediaFormat->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
	    (pMediaFormat->pbFormat != NULL))
	{
	    VIDEOINFOHEADER *VideoInfo = (VIDEOINFOHEADER *)pMediaFormat->pbFormat;
	    BITMAPINFOHEADER *BitmapInfo = &(VideoInfo->bmiHeader);

	    PTRACE(1,"PVidDirectShow\tFmt["<< i << "] = ("
		    << media_format_to_pwlib_format(pMediaFormat->subtype) << ", "
		    << BitmapInfo->biWidth << "x" << BitmapInfo->biHeight << ", "
		    << (10000000.0/VideoInfo->AvgTimePerFrame) << "fps)");
	}

	DeleteMediaType(pMediaFormat);
    }

    pStreamConfig->Release();

    return TRUE;
}

/*
 *
 *
 */
BOOL PVideoInputDevice_DirectShow::GetDefaultFormat()
{
    HRESULT hr;
    IAMStreamConfig *pStreamConfig;
    AM_MEDIA_TYPE *pMediaFormat;

    PTRACE(4, "PVidDirectShow\tGetDefaultFormat()");

    hr = pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
	    			 pSrcFilter, IID_IAMStreamConfig, (void **)&pStreamConfig);

    if (FAILED(hr))
    {
	PTRACE(4, "PVidDirectShow\tFailed to find StreamConfig Video interface: " << ErrorMessage(hr));
	return FALSE;
    }

    hr = pStreamConfig->GetFormat(&pMediaFormat);
    if (FAILED(hr))
    {
	PTRACE(1, "PVidDirectShow\tFailed to getFormat: " << ErrorMessage(hr));
	pStreamConfig->Release();
	return FALSE;
    }

    if ((pMediaFormat->formattype == FORMAT_VideoInfo)     &&
        (pMediaFormat->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
	(pMediaFormat->pbFormat != NULL))
    {
	VIDEOINFOHEADER *VideoInfo = (VIDEOINFOHEADER *)pMediaFormat->pbFormat;
	BITMAPINFOHEADER *BitmapInfo = &(VideoInfo->bmiHeader);
	const char *format = media_format_to_pwlib_format(pMediaFormat->subtype);
	int fps = (int)(10000000.0/VideoInfo->AvgTimePerFrame);

	PTRACE(1,"PVidDirectShow\tDefault format is: "
		<< format << ", "
		<< BitmapInfo->biWidth << "x" << BitmapInfo->biHeight << ", "
		<< fps << "fps)");

	colourFormat = format;
	frameWidth = BitmapInfo->biWidth;
	frameHeight = BitmapInfo->biHeight;
	frameRate = fps;

    }

    DeleteMediaType(pMediaFormat);
    pStreamConfig->Release();
}


/*
 *
 *
 *
 */
static char *BSTR_to_ANSI(BSTR pSrc)
{
    unsigned int cb, cwch;
    char *szOut = NULL;

    if(!pSrc)
	return NULL;

    cwch = SysStringLen(pSrc);

    /* Count the number of character needed to allocate */
    cb = WideCharToMultiByte(CP_ACP, 0, pSrc, cwch + 1, NULL, 0, 0, 0);
    if (cb == 0)
	return NULL;

    szOut = (char *)calloc(cb+1, 1);
    if (szOut == NULL)
	return NULL;

    cb = WideCharToMultiByte(CP_ACP, 0, pSrc, cwch + 1, szOut, cb, 0, 0);
    if (cb == 0)
    {
	free(szOut);
	return NULL;
    }

    return szOut;
}


static const char *ErrorMessage(HRESULT hr)
{
    static char string[1024];
    DWORD dwMsgLen;

    memset(string, 0, sizeof(string));
    dwMsgLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM |
			      FORMAT_MESSAGE_IGNORE_INSERTS,
			      NULL,
			      hr,
			      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			      (LPSTR)string,
			      sizeof(string)-1,
			      NULL);
    if (dwMsgLen)
	return string;

    memset(string, 0, sizeof(string));
    dwMsgLen = AMGetErrorTextA(hr, string, sizeof(string));
    if (dwMsgLen)
	return string;

    snprintf(string, sizeof(string), "0x%8.8x", hr);
    return string;
}

static HRESULT SetDevice(const PString & devName, IBaseFilter ** ppSrcFilter)
{
    HRESULT hr;
    IBaseFilter *pSrc = NULL;
    IMoniker *pMoniker = NULL;
    ICreateDevEnum *pDevEnum = NULL;
    IEnumMoniker *pClassEnum = NULL;
    ULONG cFetched;

    PTRACE(4,"PVidDirectShow\tSetDevice(" << devName << ")");

    // Create the system device enumerator

    hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                           IID_ICreateDevEnum, (void **) &pDevEnum);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tCouldn't create system enumerator: " << ErrorMessage(hr));
        return hr;
    }

    // Create an enumerator for the video capture devices

    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
    if (FAILED(hr))
    {
        PTRACE(1, "PVidDirectShow\tCouldn't create class enumerator: " << ErrorMessage(hr));
        return hr;
    }

    if (pClassEnum == NULL)
    {
        PTRACE(1, "PVidDirectShow\tSetDevice() No video capture device was detected");
        return hr;
    }

    pClassEnum->Reset();

    *ppSrcFilter = NULL;
    while (*ppSrcFilter == NULL)
    {
	// Get the next device
	hr = pClassEnum->Next(1, &pMoniker, &cFetched);
	if (hr != S_OK)
	{
	    PTRACE(4, "PVidDirectShow\tSetDevice() No more video capture device");
	    hr = ERROR_DEVICE_NOT_CONNECTED;
	    break;
	}

	// Get the property bag
	IPropertyBag *pPropBag;

	hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
	if (FAILED(hr))
	{
	    pMoniker->Release();
	    continue;
	}

	// Find the description or friendly name.
	VARIANT DeviceName;
	DeviceName.vt = VT_BSTR;

	hr = pPropBag->Read(L"Description", &DeviceName, 0);
	if (FAILED(hr))
	    hr = pPropBag->Read(L"FriendlyName", &DeviceName, 0);
	if (SUCCEEDED(hr))
	{
	    char *pDeviceName = BSTR_to_ANSI(DeviceName.bstrVal);
	    PTRACE(4, "PVidDirectShow\tSetDevice() current capture device '"<< pDeviceName << "'");

	    if (pDeviceName && PString(pDeviceName) == devName)
	    {
		// Bind Moniker to a filter object
		hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pSrc);
		if (FAILED(hr))
		{
		    PTRACE(1, "PVidDirectShow\tSetDevice() Couldn't bind moniker to filter object: " << ErrorMessage(hr));
		    break;
		}
		PTRACE(4, "PVidDirectShow\tSetDevice() This one is kept '"<< pDeviceName << "'");
		*ppSrcFilter = pSrc;
	    }
	    if (pDeviceName)
		free(pDeviceName);
	}

	pPropBag->Release();
	pMoniker->Release();
	// Next Device
    }
    /* If no device was found ppSrcFilter is NULL */

    SAFE_RELEASE(pDevEnum);
    SAFE_RELEASE(pClassEnum);

    return hr;
}


struct pwlib_fmt
{
    char *pwlib_format;
    GUID  media_format;
};

static struct pwlib_fmt formats[] =
{
    {(char*) "Grey",    MEDIASUBTYPE_RGB8 },
    {(char*) "BGR32",   MEDIASUBTYPE_RGB32}, /* Microsoft assumes that we are in little endian */
    {(char*) "BGR24",   MEDIASUBTYPE_RGB24},
    {(char*) "RGB565",  MEDIASUBTYPE_RGB565},
    {(char*) "RGB555",  MEDIASUBTYPE_RGB555},
    {(char*) "YUV420P", MEDIASUBTYPE_I420},
    {(char*) "YUV422P", MEDIASUBTYPE_YUYV},
    {(char*) "YUV411",  MEDIASUBTYPE_Y411},
    {(char*) "YUV411P", MEDIASUBTYPE_Y41P},
    {(char*) "YUV410P", MEDIASUBTYPE_YVU9},
    {(char*) "YUY2",    MEDIASUBTYPE_YUY2},
    {(char*) "MJPEG",   MEDIASUBTYPE_MJPG},
    {(char*) "UYVY422", MEDIASUBTYPE_UYVY},
};

static GUID pwlib_format_to_media_format(const char *format)
{
    unsigned int i;

    for (i=0; i<sizeof(formats)/sizeof(formats[0]); i++)
    {
	if (strcmp(formats[i].pwlib_format, format) == 0)
	    return formats[i].media_format;
    }
    return MEDIATYPE_NULL;
}

static const char *media_format_to_pwlib_format(const GUID guid)
{
    unsigned int i;

    for (i=0; i<sizeof(formats)/sizeof(formats[0]); i++)
    {
	if (guid == formats[i].media_format)
	    return formats[i].pwlib_format;
    }
    if (guid == MEDIASUBTYPE_CLPL)
	return "CLPL";
    else if (guid == MEDIASUBTYPE_YUYV)
	return "YUYV";
    else if (guid == MEDIASUBTYPE_IYUV)
	return "IYUV";
    else if (guid == MEDIASUBTYPE_YVU9)
	return "YVU9";
    else if (guid == MEDIASUBTYPE_Y411)
	return "Y411";
    else if (guid == MEDIASUBTYPE_Y41P)
	return "Y41P";
    else if (guid == MEDIASUBTYPE_YUY2)
	return "YUY2";
    else if (guid == MEDIASUBTYPE_YVYU)
	return "YVYU";
    else if (guid == MEDIASUBTYPE_UYVY)
	return "UYVY";
    else if (guid == MEDIASUBTYPE_Y211)
	return "Y211";
    else if (guid == MEDIASUBTYPE_YV12)
	return "YV12";
    else if (guid == MEDIASUBTYPE_CLJR)
	return "CLJR";
    else if (guid == MEDIASUBTYPE_IF09)
	return "IF09";
    else if (guid == MEDIASUBTYPE_CPLA)
	return "CPLA";
    else if (guid == MEDIASUBTYPE_MJPG)
	return "MJPG";
    else if (guid == MEDIASUBTYPE_TVMJ)
	return "TVMJ";
    else if (guid == MEDIASUBTYPE_WAKE)
	return "WAKE";
    else if (guid == MEDIASUBTYPE_CFCC)
	return "CFCC";
    else if (guid == MEDIASUBTYPE_IJPG)
	return "IJPG";
    else if (guid == MEDIASUBTYPE_Plum)
	return "Plum";
    else if (guid == MEDIASUBTYPE_DVCS)
	return "DVCS";
    else if (guid == MEDIASUBTYPE_DVSD)
	return "DVSD";
    else if (guid == MEDIASUBTYPE_MDVF)
	return "MDVF";
    else if (guid == MEDIASUBTYPE_RGB1)
	return "RGB1";
    else if (guid == MEDIASUBTYPE_RGB4)
	return "RGB4";
    else if (guid == MEDIASUBTYPE_RGB8)
	return "RGB8";
    else if (guid == MEDIASUBTYPE_RGB565)
	return "RGB565";
    else if (guid == MEDIASUBTYPE_RGB555)
	return "RGB555";
    else if (guid == MEDIASUBTYPE_RGB24)
	return "BGR24";
    else if (guid == MEDIASUBTYPE_RGB32)
	return "BGR32";
    else if (guid == MEDIASUBTYPE_I420)
	return "I420";
    else
	return guid_to_string(guid); /* FIXME: memory leak */
}

static char *guid_to_string(const GUID guid)
{
    wchar_t guid_wchar[256];
    char guid_string[256];
    int guid_wcharlen;

    guid_wcharlen = StringFromGUID2(guid, guid_wchar, sizeof(guid_wchar));
    WideCharToMultiByte(CP_ACP, 0,
	                guid_wchar, guid_wcharlen+1,
			guid_string, sizeof(guid_string),
			0, 0);

    return strdup(guid_string);
}

#ifdef __MINGW32__

static void DeleteMediaType(AM_MEDIA_TYPE *pmt)
{
    if (pmt == NULL)
	return;

    if (pmt->cbFormat != 0)
    {
	CoTaskMemFree((PVOID)pmt->pbFormat);
        pmt->cbFormat = 0;
        pmt->pbFormat = NULL;
    }
    if (pmt->pUnk != NULL)
    {
        // Uncessessary because pUnk should not be used, but safest.
        pmt->pUnk->Release();
        pmt->pUnk = NULL;
    }

    CoTaskMemFree(pmt);
}

#endif

#endif /*P_DIRECTSHOW*/
