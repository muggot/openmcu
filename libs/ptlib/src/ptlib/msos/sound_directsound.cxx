/*
 * sound_directsound.cxx
 *
 * DirectX Sound driver implementation.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2006-2007 Novacom, a division of IT-Optics
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
 * The Initial Developer of the Original DirectSound Code is 
 * Vincent Luba <vincent.luba@novacom.be>
 *
 * Contributor(s): /
 *
 * $Log: sound_directsound.cxx,v $
 * Revision 1.5  2007/09/17 11:14:47  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.4  2007/08/21 10:33:32  rjongbloed
 * Improved build/configure of direct sound.
 *
 * Revision 1.3  2007/06/22 05:23:51  rjongbloed
 * Fixed logging to include "subsystem" name.
 *
 * Revision 1.2  2007/05/19 08:54:45  rjongbloed
 * Further integration of DirectSound plugin thanks to Vincent Luba.and NOVACOM (http://www.novacom.be).
 *
 * Revision 1.1  2007/05/15 21:39:26  dsandras
 * Added initial code for a DirectSound plugin thanks to Vincent Luba.
 * Code contributed by NOVACOM (http://www.novacom.be).
 *
 * Revision 1.1  2006/09/15 10:04:05  luba vincent
 * First implementation of DirectSound driver
 */

//#pragma implementation "sound_directsound.h"

#include <ptlib.h>

#ifdef P_DIRECTSOUND

#include <ptlib/msos/ptlib/sound_directsound.h>

#include <math.h>

//#include <dxerr9.h>  Doesn't seem to exist for me!
#define DXGetErrorString9(r) r


#pragma comment(lib, P_DIRECTSOUND_LIBRARY)


#define P_FORCE_STATIC_PLUGIN

#if defined(_WIN32) && !defined(P_FORCE_STATIC_PLUGIN)
#error "sound_directsound.cxx must be compiled without precompiled headers"
#endif

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }

/* Instantiate the PWLIBsound plugin*/ 
PCREATE_SOUND_PLUGIN(DirectSound, PSoundChannelDirectSound)

/* Callback function used to collect data from enumerated DirectX devices */
INT_PTR CALLBACK DSoundEnumCallback( GUID* pGUID, LPSTR strDesc, LPSTR strDrvName,
				     VOID* devices );

///////////////////////////////////////////////////////////////////////////////

/*
 * DESC	: Default Constructor	
 * BEHAVIOUR :
 * RETURN :
 */
PSoundChannelDirectSound::PSoundChannelDirectSound()
{
  Construct();
}

/*
 * DESC	:	PSoundChannelDirectSound Constructor. Creates the object and Initialise the device 
 * BEHAVIOUR :
 * RETURN :
 */
PSoundChannelDirectSound::PSoundChannelDirectSound (const PString &device,
						    Directions dir,
						    unsigned numChannels,
						    unsigned sampleRate,
						    unsigned bitsPerSample)
{
  Construct();
  Open (device, dir, numChannels, sampleRate, bitsPerSample);
}

/*
 * DESC	:	DirectSound Object destructor	
 * BEHAVIOUR :  Close any opened channels
 * RETURN :
 */

PSoundChannelDirectSound::~PSoundChannelDirectSound()
{
  PTRACE (4, "dsound\t (" << ((mDirection == Player) ? "Playback" : "Recording") << " ~PSoundChannelDirectSound");
  Close();
}
	
/*
 * DESC	:	Initialize Object variable	
 * BEHAVIOUR :	Every member set to NULL
 * RETURN :
 */
void 
PSoundChannelDirectSound::Construct()
{

  mAudioPlaybackBuffer = NULL;
  mAudioPrimaryPlaybackBuffer = NULL;
  sAudioPlaybackDevice = NULL;

  sAudioCaptureDevice = NULL;
  mAudioCaptureBuffer = NULL;
  
  mDXBufferSize = 0;
  mStreaming = true;
  mOutburst = 0;
  mVolume = 0;
}

/*
 * DESC	:	Opens a device with format specifications
 * BEHAVIOUR : Fetch the requested device GUID, then initialize the Device.
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::Open (const PString & _device,
				     Directions _dir,
				     unsigned _numChannels,
				     unsigned _sampleRate,
				     unsigned _bitsPerSample)
{
  mDirection = _dir;
  mNumChannels = _numChannels;
  mSampleRate = _sampleRate;
  mBitsPerSample = _bitsPerSample;
 
  GUID deviceGUID;

  if (!GetDeviceID (_device, &deviceGUID))
    return false;
  
  PTRACE (4, "dsound\tOpen " << ((mDirection == Player) ? "Playback" : "Recording") << " Channel\n"
          << "  --> mNumChannels " << mNumChannels << '\n'
          << "  --> mSampleRate" << mSampleRate << '\n'
          << "  --> mBitsPerSample" << mBitsPerSample);
  
  Close();

  SetFormat (_numChannels,
             _sampleRate,
             _bitsPerSample);

  BOOL result = (mDirection == Recorder) ? 
    				InitCaptureDevice (&deviceGUID) : 
				InitPlaybackDevice (&deviceGUID);

  if (!result) {
    PTRACE(4, "dsound\tCould not open device " << ((mDirection == Player) ? "Playback" : "Recording") << " failed");
  }

  return result;
}

/*
 * DESC	:	Provides Default device names
 * BEHAVIOUR :  Defines 'Default' as the default interface's name
 * RETURN :	PString
 */
PString 
PSoundChannelDirectSound::GetDefaultDevice (Directions /*dir*/)
{
  return PString ("Default");
}

/*
 * DESC	:
 * BEHAVIOUR :
 * RETURN :
 */
DirectSoundDevices 
PSoundChannelDirectSound::DevicesEnumerators (Directions dir)
{

  DirectSoundDevices devices;
  if (dir == Recorder)
    DirectSoundCaptureEnumerate( (LPDSENUMCALLBACK)DSoundEnumCallback,
				 (void*) &devices);
  else
    DirectSoundEnumerate( (LPDSENUMCALLBACK)DSoundEnumCallback,
			  (void*) &devices);

  if (devices.names.GetSize () > 1)
    {
      GUID pTemp  = (dir == Player) ? DSDEVID_DefaultPlayback  : DSDEVID_DefaultCapture;
      devices.guids [devices.names.GetSize ()] = pTemp;
      devices.names += "Default";
    }
  
  return devices;
}

/*
 * DESC	:
 * BEHAVIOUR :
 * RETURN :
 */
BOOL 
PSoundChannelDirectSound::GetDeviceID (PString deviceName, GUID *pGUID)
{

  PTRACE (4, "dsound\tGet " << ((mDirection == Player) ? "Playback" : "Recording") << " Device ID for " << deviceName);

  *pGUID = (mDirection == Player) ? DSDEVID_DefaultPlayback  : DSDEVID_DefaultCapture;
  DirectSoundDevices devices = DevicesEnumerators (mDirection);
  
  PINDEX idx = devices.names.GetStringsIndex (deviceName);
  
  if (idx != P_MAX_INDEX) 
    *pGUID = devices.guids[idx];
  
  return true;
}

/* 
 * DESC : Provides a list of detected devices human readable names
 * BEHAVIOUR : Returns the names array of enumerated devices
 * RETURN : Names as PStringArray
 */

PStringArray 
PSoundChannelDirectSound::GetDeviceNames (Directions dir)
{
 
  PTRACE (4, "dsound\tGetDeviceNames " << ((dir == Player) ? "Playback" : "Recording") << " device Name");
  DirectSoundDevices devices = DevicesEnumerators (dir);
  return devices.names; 
}

/*
 * DESC	:
 * BEHAVIOUR :
 * RETURN :
 */
INT_PTR CALLBACK 
DSoundEnumCallback( GUID* pGUID, LPSTR strDesc, LPSTR /*strDrvName*/,
				     void* device)
{
  DirectSoundDevices* devices_array = (DirectSoundDevices *) device;

  if( pGUID )
    {
  
      if (devices_array->names.GetSize () < 20)
	{
	  GUID *pTemp  = & (*devices_array).guids [devices_array->names.GetSize ()];
	  memcpy( pTemp, pGUID, sizeof(GUID) );
	  devices_array->names += strDesc;
	  PTRACE (4, "dsound\tDevice -->  " << strDesc );
	} else
	  return TRUE;

    }

  return TRUE;
}



/*
 * DESC	:
 * BEHAVIOUR :
 * RETURN :
 */
BOOL 
PSoundChannelDirectSound::InitPlaybackDevice(GUID *pGUID) {

  HRESULT hr;

  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " InitPlaybackDevice");
  hr = DirectSoundCreate8 (pGUID,
			   &sAudioPlaybackDevice,
			   NULL);
  if (FAILED (hr)) 
    {
      PTRACE (4, "dsound\tCould not create playback device " << DXGetErrorString9 (hr));
      return false;
    }

  HWND hWnd = GetForegroundWindow();
  
  if (hWnd == NULL)
      hWnd = GetDesktopWindow();
  
  hr = sAudioPlaybackDevice->SetCooperativeLevel (hWnd,
						  DSSCL_PRIORITY);

  if (FAILED (hr)) 
    {
      PTRACE (4, "dsound\tCould not set cooperative level " << DXGetErrorString9 (hr));
      return false;
    }
  
  DSBUFFERDESC dsbd;
  ZeroMemory(&dsbd, sizeof(dsbd));
  dsbd.dwSize = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
  dsbd.dwBufferBytes = 0;
  dsbd.lpwfxFormat = NULL;

  if ( FAILED(sAudioPlaybackDevice->CreateSoundBuffer(&dsbd, 
						      &mAudioPrimaryPlaybackBuffer, 
						      NULL)) ) 
    {
      PTRACE (4, "dsound\tCould not create primary buffer " << DXGetErrorString9 (hr));
      return false;
    }
  return TRUE;
}

/*
 * DESC	:
 * BEHAVIOUR :
 * RETURN :
 */
BOOL 
PSoundChannelDirectSound::InitCaptureDevice(GUID *pGUID) {

  HRESULT hr;
  hr = DirectSoundCaptureCreate8 (pGUID,
				  &sAudioCaptureDevice,
				  NULL);
  if (FAILED (hr)) 
    {
      PTRACE (4, "dsound\tCould not create Capture device " << DXGetErrorString9 (hr));
      return false;
    }
  return TRUE;
}

/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */
BOOL 
PSoundChannelDirectSound::Setup()
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " Setup");
  BOOL no_error = TRUE;

  return no_error;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::Close()
{

  PTRACE (4, "dsound\tClosing " << ((mDirection == Player) ? "Playback" : "Recording") << ") Channel");
  
  PWaitAndSignal mutex(bufferMutex);
  switch (mDirection) 
    {

    case Player:
      if (mAudioPlaybackBuffer)
	mAudioPlaybackBuffer->Stop ();
      SAFE_RELEASE(mAudioPlaybackBuffer);
      SAFE_RELEASE(mAudioPrimaryPlaybackBuffer);
      SAFE_RELEASE(sAudioPlaybackDevice);
      break;

    case Recorder:
      if (mAudioCaptureBuffer)
	mAudioCaptureBuffer->Stop ();
      SAFE_RELEASE(mAudioCaptureBuffer);
      SAFE_RELEASE(sAudioCaptureDevice);
      break;
    }
  isInitialised = false;
  return TRUE;
}

/*
 * DESC	:	Compute the freeSpace in a DirectX Circular Buffer
 * BEHAVIOUR :
 * RETURN 	: the size
 */

DWORD 
PSoundChannelDirectSound::GetDXBufferFreeSpace ()
{
  DWORD dwCursor = 0;
  PINDEX freeSpace;

  if (mDirection == Player)
      mAudioPlaybackBuffer->GetCurrentPosition (&dwCursor, NULL);
  else
      mAudioCaptureBuffer->GetCurrentPosition (&dwCursor, NULL);
  
  freeSpace = mDXBufferSize - (mDXOffset - dwCursor);
  if (freeSpace > mDXBufferSize) 
    freeSpace -= mDXBufferSize; // write_offset < play_offset

  return freeSpace;
}

/* 
 * DESC : 	Write Method is called by the playback device
 * BEHAVIOUR: 	It writes (len) bytes of input data (*buf) into the device circular buffer.  
 * 		In case data to write are bigger than the free space left in the buffer, it writes them than sleep while directx cursor move forward to leave 
 * RETURN :	TRUE if successful and FALSE otherwise.	
 */

BOOL 
PSoundChannelDirectSound::Write (const void *buf, 
				 PINDEX len)
{

  PINDEX to_write = len, written = 0;
  BYTE * input_buffer = (BYTE*) buf;

  if (!isInitialised)
    {
      PTRACE (4, "dsound\tWrite Failed: Device not initialised :");
      return false;
    }

  //Wait for Mutex signal
  PWaitAndSignal mutex(bufferMutex);

  lastWriteCount = 0;

  while (to_write > mOutburst)
    {

      /*
      //Adjust to blockalign
      if (mOutburst != 0)
      write_in_cycle = (write_in_cycle / mOutburst ) * mOutburst;
      */

      /* Write data from buf to circular buffer */
      written = WriteToDXBuffer (input_buffer, 
				 PMIN ((PINDEX)GetDXBufferFreeSpace (), to_write));

      if (HasPlayCompleted ()) 
	{
	  mAudioPlaybackBuffer->Play (0, 
				      0, 
				      mStreaming ? DSBPLAY_LOOPING : 0L);
	}

      //Move the cursor into the data buffer
      input_buffer += written;
      //Update the written buffer count for PWLIB usage
      lastWriteCount += written;
      //Set the count of buffers left to write
      to_write -= written;

      /* Wait as buffer is played */
      FlushBuffer ();
    }
  return true;

}

/* 
 * DESC : 	Read Method is called by the recording device
 * BEHAVIOUR: 	It reads (len) bytes from the device circular buffer to an input data (*buf).  
 * RETURN :	TRUE if successful and FALSE otherwise.	
 */

BOOL 
PSoundChannelDirectSound::Read (void * buf, PINDEX len)
{

  PINDEX read = 0, to_read = len;
  BYTE * output_buffer = (BYTE*) buf;
  
  PWaitAndSignal mutex(bufferMutex);

  if (!isInitialised) 
    {
      PTRACE (4, "dsound\tRead : Device not initialised ");
      return false;
    }
 
  lastReadCount = 0;

  while (to_read > mOutburst) 
    {
     
      /* Will read from device buffer minimum between the data left to read, and the available space */
      read = ReadFromDXBuffer (output_buffer,
			       PMIN ((PINDEX)GetDXBufferFreeSpace (), to_read));
      to_read -= read;
      lastReadCount += read;
      output_buffer += read; /* Increment the buffer pointer */
      
      /* Wait as buffer is being played */
      FlushBuffer ();
    }

  return true;
}

/*
 * DESC:   Writes (len) bytes from the buffer (*buf) to DirectX sound device buffer
 * BEHAVIOUR :  Locks the buffer on the requested size; In case buffer was lost, tries to restore it.
 * 	  	Copies the data into the buffer
 * 	  	Unlock the buffer
 * RETURN    : Returns the size actually written
 */

PINDEX 
PSoundChannelDirectSound::WriteToDXBuffer (const void *buf, 
					   PINDEX len) 
{

  HRESULT hr;
  LPVOID lpvWrite1, lpvWrite2;
  DWORD dwLength1, dwLength2;
  PINDEX written = 0;

  /***  Lock the buffer   ***/
  hr = mAudioPlaybackBuffer->Lock (mDXOffset,
				   len,
				   &lpvWrite1,
				   &dwLength1,
				   &lpvWrite2,
				   &dwLength2,
				   0L);

  if (hr == DSERR_BUFFERLOST) 
    {
      //Buffer was lost, need to restore it
      PTRACE (4, "dsound\tPlayback buffer was lost, Need to restore.");
      mAudioPlaybackBuffer->Restore ();
      hr = mAudioPlaybackBuffer->Lock (mDXOffset,
				       len,
				       &lpvWrite1,
				       &dwLength1,
				       &lpvWrite2,
				       &dwLength2,
				       0L);
    }

  if (!FAILED (hr)) 
    {
      /***  Copy memory into buffer  ***/
      memcpy (lpvWrite1, buf, dwLength1);
      if (lpvWrite2 != NULL)
	memcpy (lpvWrite2, (BYTE *) buf + dwLength1, dwLength2);

      written = dwLength1 + dwLength2;
      
      /***  Unlock the buffer   ***/
      mAudioPlaybackBuffer->Unlock (lpvWrite1,
				    dwLength1,
				    lpvWrite2,
				    dwLength2);
      //Move write cursor
      mDXOffset += written;
      mDXOffset %= mDXBufferSize;
    } 
  else 
      PTRACE (4, "dsound\tWriteToDXBuffer Failed : " << DXGetErrorString9 (hr));

  return written;
}

/*
 * DESC:   Reads (len) bytes from the buffer (*buf) from DirectX sound capture device buffer
 * BEHAVIOUR :  Locks the buffer on the requested size; In case buffer was lost, tries to restore it.
 * 	  	Copies the data into the buffer
 * 	  	Unlock the buffer
 * RETURN    : Returns the size actually read
 */
PINDEX 
PSoundChannelDirectSound::ReadFromDXBuffer (const void * buf, 
					    PINDEX len)
{
  HRESULT hr;
  LPVOID lpvRead1, lpvRead2;
  DWORD dwLength1, dwLength2;
  PINDEX read = 0;

  /***  Lock the buffer   ***/
  hr = mAudioCaptureBuffer->Lock (mDXOffset,
				  len,
				  &lpvRead1,
				  &dwLength1,
				  &lpvRead2,
				  &dwLength2,
				  0L);

  if (!FAILED(hr)) 
    {
      /***  Copy memory into buffer  ***/
      memcpy ((BYTE *)buf + lastReadCount, lpvRead1, dwLength1);

      if (lpvRead2 != NULL)
	memcpy ((BYTE *) buf + dwLength1 +  lastReadCount, lpvRead2, dwLength2);

      read = dwLength1 + dwLength2;
      mDXOffset += read;
      mDXOffset %= mDXBufferSize;
      
      /***  Unlock the buffer   ***/
      mAudioCaptureBuffer->Unlock (lpvRead1,
				   dwLength1,
				   lpvRead2,
				   dwLength2);

    } 
  else 
    PTRACE (4, "dsound\tReadFromDXBuffer Lock failed: " << DXGetErrorString9 (hr));

  return read;
}


/*
 * DESC	: 	Suspend the treatment in order to let the buffer flush itself 
 * BEHAVIOUR :	Compute the time needed to play the remaining data in buffer. 
 * 		In case it exceed 600 ms, sleeps for 500 ms.
 * RETURN :	/
 */
void 
PSoundChannelDirectSound::FlushBuffer ()
{
  DWORD sleep_time = (mDXBufferSize - GetDXBufferFreeSpace ()) * 8000 / ( mWFX.wBitsPerSample * mWFX.nSamplesPerSec);

  if (sleep_time > 600) 
      Sleep((DWORD)(sleep_time-500));
}

/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::SetFormat (unsigned numChannels,
					  unsigned sampleRate,
					  unsigned bitsPerSample)
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " SetFormat\n"
          << "   -->  nChannels  :" << mWFX.nChannels << '\n'
          << "   -->  nSamplesPerSec  :" << mWFX.nSamplesPerSec << '\n'
          << "   -->  wBitsPerSample  :" << mWFX.wBitsPerSample << '\n'
          << "   -->  nBlockAlign  :" << mWFX.nBlockAlign << '\n'
          << "   -->  nAvgBytesPerSec  :" << mWFX.nAvgBytesPerSec << '\n'
          << "   -->  mOutburst  :" << mOutburst);
  
  memset (&mWFX, 0, sizeof (mWFX)); 
  mWFX.wFormatTag = WAVE_FORMAT_PCM;
  mWFX.nChannels = (WORD)numChannels;
  mWFX.nSamplesPerSec = sampleRate;
  mWFX.wBitsPerSample = (WORD)bitsPerSample;
  mWFX.nBlockAlign = mWFX.nChannels * (mWFX.wBitsPerSample / 8);
  mWFX.nAvgBytesPerSec = mWFX.nSamplesPerSec * mWFX.nBlockAlign;
  mWFX.cbSize = 0; //ignored

  mOutburst = mWFX.nBlockAlign*8;

  
  return TRUE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

unsigned 
PSoundChannelDirectSound::GetChannels()   const
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " GetChannels");
  return mNumChannels;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

unsigned 
PSoundChannelDirectSound::GetSampleRate() const
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " GetSampleRate");
  return mSampleRate;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

unsigned 
PSoundChannelDirectSound::GetSampleSize() const
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " GetSampleSize");
  return mBitsPerSample;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::SetBuffers (PINDEX size, PINDEX count)
{

  //kept for records
  mBufferCount = count;
  mBufferSize = size; 

  mDXBufferSize = mWFX.nAvgBytesPerSec;

  PTRACE (4, "dsound\tSetBuffers ==>  size = " << size << "  |  count = " << count << "  | DXBufferSize = " << mDXBufferSize);
  if (mDirection == Player)
    InitPlaybackBuffer ();
  else
    InitCaptureBuffer ();
  return TRUE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::GetBuffers(PINDEX & size, PINDEX & count)
{
  count = mBufferCount;
  size = mBufferSize;
  return TRUE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::InitCaptureBuffer() 
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " InitCaptureBuffer");

  if (isInitialised)
    return false;
 
  HRESULT hr; 
  DSCBUFFERDESC dscbdesc;
  LPDIRECTSOUNDCAPTUREBUFFER pDscb = NULL;

  memset(&dscbdesc, 0, sizeof(DSCBUFFERDESC)); 
  dscbdesc.dwSize = sizeof(DSCBUFFERDESC); 
  dscbdesc.dwFlags = DSCBCAPS_WAVEMAPPED; 
  dscbdesc.dwBufferBytes = mDXBufferSize; 
  dscbdesc.lpwfxFormat = &mWFX; 

  hr = sAudioCaptureDevice->CreateCaptureBuffer(&dscbdesc, 
						&pDscb, 
						NULL); 
  if (SUCCEEDED(hr)) 
    { 

      hr = pDscb->QueryInterface(IID_IDirectSoundCaptureBuffer8, (LPVOID*) &mAudioCaptureBuffer);
      pDscb->Release();
      mDXOffset = 0;
      mAudioCaptureBuffer->Start (DSCBSTART_LOOPING);
      isInitialised = true;
    }
  else
    PTRACE (4, "dsound\tInitCaptureBuffer : Create Sound Buffer Failed " << DXGetErrorString9 (hr));


  return isInitialised;
}

/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::InitPlaybackBuffer() 
{

  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " InitPlaybackBuffer");

  if (isInitialised)
    return false;

  DSBUFFERDESC dsbdesc; 
  HRESULT hr; 
  LPDIRECTSOUNDBUFFER pDsb = NULL;

  memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
  dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
  dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2; 
  dsbdesc.dwBufferBytes = mDXBufferSize; 
  dsbdesc.lpwfxFormat = &mWFX; 

  hr = sAudioPlaybackDevice->CreateSoundBuffer(&dsbdesc, 
					       &pDsb, 
					       NULL); 
  if (SUCCEEDED(hr)) 
    { 

      hr = pDsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*) &mAudioPlaybackBuffer);
      pDsb->Release();
      mDXOffset = 0;

      //fill buffer with silence
      PBYTEArray silence(mDXBufferSize);
      memset (silence.GetPointer(), (mWFX.wBitsPerSample == 8) ? 128 : 0, mDXBufferSize);
      WriteToDXBuffer (silence, mDXBufferSize);

      mAudioPlaybackBuffer->SetCurrentPosition (0);
      isInitialised = true;
    } 
  else
    PTRACE (4, "dsound\tInitPlaybackBuffer : Create Sound Buffer Failed " << DXGetErrorString9 (hr));

  return isInitialised;
}

/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::PlaySound(const PSound & sound, BOOL wait)
{
  mStreaming = false;
  if (!Write((const void *)sound, sound.GetSize()))
    return FALSE;

  if (wait)
    return WaitForPlayCompletion();
  return TRUE;
}

/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::PlayFile(const PFilePath & filename, BOOL wait)
{
  BYTE buffer [512];
  mStreaming = false;

  PFile file (filename, PFile::ReadOnly);

  if (!file.IsOpen())
    return FALSE;

  for (;;) 
    {

    if (!file.Read (buffer, 512))
      break;

    PINDEX len = file.GetLastReadCount();
    if (len == 0)
      break;
    if (!Write(buffer, len))
      break;
  }

  file.Close();

  if (wait)
    return WaitForPlayCompletion();

  return TRUE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::HasPlayCompleted()
{

  DWORD dwStatus;
  if (mAudioPlaybackBuffer != NULL) 
    {

      mAudioPlaybackBuffer->GetStatus (&dwStatus);
      if (dwStatus & DSBSTATUS_PLAYING)  
	return FALSE;

    }
  return TRUE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::WaitForPlayCompletion()
{
  while (!HasPlayCompleted()) 
    Sleep (50);

  return TRUE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */
BOOL 
PSoundChannelDirectSound::RecordSound(PSound & /*sound*/)
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " RecordSound");
  return FALSE;
}

/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::RecordFile(const PFilePath & /*filename*/)
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " RecordFile");
  return FALSE;
}

/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */
BOOL 
PSoundChannelDirectSound::StartRecording()
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " StartRecording");
  return FALSE;
}

/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */
BOOL PSoundChannelDirectSound::IsRecordBufferFull()
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " IsRecordBufferFull");
  return TRUE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */
BOOL PSoundChannelDirectSound::AreAllRecordBuffersFull()
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " AreAllRecordBuffersFull");
  return TRUE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */
BOOL PSoundChannelDirectSound::WaitForRecordBufferFull()
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " WaitForRecordBufferFull");
  return TRUE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */
BOOL PSoundChannelDirectSound::WaitForAllRecordBuffersFull()
{

  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " WaitForAllRecordBuffersFull");
  return FALSE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */
BOOL 
PSoundChannelDirectSound::Abort()
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " Abort");
  return TRUE;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::SetVolume (unsigned newVal)
{

  BOOL no_error=TRUE;
  HRESULT hr;
  
  long volume = (long) (log10((double)newVal) * 5000.0) - 10000;
  switch (mDirection) {

  case Player:
    if (mAudioPrimaryPlaybackBuffer) 
      {

	if (FAILED (hr = mAudioPlaybackBuffer->SetVolume( volume ))) 
	  {
	    PTRACE (4, "PSoundChannelDirectSound::SetVolume Failed " << DXGetErrorString9 (hr));
	    no_error = FALSE;
	  }
      }
    else
      PTRACE (4, "PSoundChannelDirectSound::SetVolume Failed mAudioPlaybackBuffer is NULLL (huh?)");
    break;


  case Recorder:
    // DirectX does not let you change the capture buffer volume
    mVolume = newVal;
    break;
  }
  return no_error;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL  
PSoundChannelDirectSound::GetVolume(unsigned &devVol)
{

  BOOL no_error=TRUE;
  HRESULT hr;
  long volume = 100;
  switch (mDirection) 
    {

    case Player:

      if (mAudioPlaybackBuffer) 
	{

	  if (FAILED (hr = mAudioPlaybackBuffer->GetVolume( &volume ))) 
	    {
	      PTRACE (4, "PSoundChannelDirectSound::GetVolume Failed " << DXGetErrorString9 (hr));
	      no_error = FALSE;
	    }
	  devVol = (unsigned int) pow(10.0, (float)(volume+10000) / 5000.0);
	}
      break;

    case Recorder:
      // DirectX does not let you change the capture buffer volume
      devVol = mVolume;
      break;

    }
  return no_error;
}


/*
 * DESC	:	
 * BEHAVIOUR :
 * RETURN :
 */

BOOL 
PSoundChannelDirectSound::IsOpen () const
{
  PTRACE (4, "dsound\t" << ((mDirection == Player) ? "Playback" : "Recording") << " IsOpen");
  return isOpen;
}


#endif // P_DIRECTSOUND
