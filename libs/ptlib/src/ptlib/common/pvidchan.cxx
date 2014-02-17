/*
 * pvidchan.cxx
 *
 * Video Channel implementation.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): Derek Smithies (derek@indranet.co.nz)
 *
 * $Log: pvidchan.cxx,v $
 * Revision 1.21  2007/10/03 04:25:51  csoutheren
 * Fixed typo
 *
 * Revision 1.20  2007/09/14 01:14:25  csoutheren
 * Fix warnings under VS.net 2003
 *
 * Revision 1.19  2007/04/20 06:47:48  csoutheren
 * Really disable video code when video is turned off
 *
 * Revision 1.18  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.17  2006/02/20 06:14:41  csoutheren
 * Return FALSE if video channel read fails
 *
 * Revision 1.16  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.15  2003/05/27 04:22:54  dereksmithies
 * Test grabber size before issuing a grabber resize command.
 *
 * Revision 1.14  2003/04/14 21:18:41  dereks
 * Formatting change.
 *
 * Revision 1.13  2003/03/20 23:40:51  dereks
 * Fix minor problems with using null pointers.
 *
 * Revision 1.12  2003/03/17 07:47:42  robertj
 * Removed redundant "render now" function.
 * Made significant enhancements to PVideoOutputDevice class.
 *
 * Revision 1.11  2003/01/06 18:41:08  rogerh
 * Add NetBSD patches, taken from the NetBSD pkg patches.
 * Submitted by Andreas Wrede
 *
 * Revision 1.10  2002/05/08 22:38:53  dereks
 * Adjust formatting to the pwlib standard.
 *
 * Revision 1.9  2002/02/08 00:57:33  dereks
 * Modify PTRACE level to reduce debug information to reasonable level.
 *
 * Revision 1.8  2002/01/04 04:11:45  dereks
 * Add video flip code from Walter Whitlock, which flips code at the grabber.
 *
 * Revision 1.7  2001/12/03 03:44:52  dereks
 * Add method to retrive pointer to the attached video display class.
 *
 * Revision 1.6  2001/11/28 00:07:32  dereks
 * Locking added to PVideoChannel, allowing reader/writer to be changed mid call
 * Enabled adjustment of the video frame rate
 * New fictitous image, a blank grey area
 *
 * Revision 1.5  2001/10/23 02:11:00  dereks
 * Extend video channel so it can display raw data, using attached video devices.
 *
 * Revision 1.4  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.3  2001/06/19 00:51:57  dereks
 * The ::Write method now returns the result of mpOutput->Redraw(), rather than
 *   always true.
 *
 * Revision 1.2  2001/03/23 20:24:23  yurik
 * Got rid of "unknown pragma" for WinCE port
 *
 * Revision 1.1  2000/12/19 22:20:26  dereks
 * Add video channel classes to connect to the PwLib PVideoInputDevice class.
 * Add PFakeVideoInput class to generate test images for video.
 *
 *
 *
 */

#ifdef __GNUC__
#pragma implementation "video.h"
#endif

#include <ptlib.h>

#if P_VIDEO 

#include <ptlib/video.h>

PVideoChannel::PVideoChannel() 
{
  mpInput = NULL;
  mpOutput = NULL;
}


PVideoChannel::PVideoChannel(const PString & device,
                             Directions dir)
{
  mpInput = NULL;
  mpOutput = NULL;
  Open(device, dir);
}

PVideoChannel::~PVideoChannel()
{
  Close();
}


PStringList PVideoChannel::GetDeviceNames(Directions /*dir*/)
{
  PStringList list;

  list.AppendString("Video Channel Base");

  return list;
}


PString PVideoChannel::GetDefaultDevice(Directions /*dir*/)
{
#if defined(P_FREEBSD) || defined(P_OPENBSD)  || defined(P_NETBSD)
  return "/dev/bktr0";
#endif

#ifndef DEFAULT_VIDEO
     return "/dev/video0";
#else
  return DEFAULT_VIDEO;
#endif
}


BOOL PVideoChannel::Open(const PString & dev,
                         Directions dir)
{
  PWaitAndSignal m(accessMutex);

  Close();

  deviceName = dev;
  direction = dir;
  
  return TRUE;
}



BOOL PVideoChannel::Read(void * buf, PINDEX  len)
{
  PWaitAndSignal m(accessMutex);

  if (mpInput == NULL)  
    return FALSE;

  BYTE * dataBuf;
  PINDEX dataLen;
  dataBuf = (BYTE *)buf;
  dataLen = len;
  return mpInput->GetFrameData(dataBuf, &dataLen);

  // CHANGED  return TRUE;
}

BOOL PVideoChannel::Write(const void * buf,  //image data to be rendered
                          PINDEX      /* len */)
{
  PWaitAndSignal m(accessMutex);

  if (mpOutput == NULL)
    return FALSE;


  if (mpInput == NULL) {
    PTRACE(9,"PVC\t::Write, frame size is "
              << mpOutput->GetFrameWidth() << "x" << mpOutput->GetFrameHeight() << 
              " VideoGrabber is unavailabile");
    return mpOutput->SetFrameData(0, 0, mpOutput->GetFrameWidth(), mpOutput->GetFrameHeight(), (const BYTE *)buf, TRUE);
  }

  PTRACE(6,"PVC\t::Write, frame size is " 
               << mpInput->GetFrameWidth() << "x" << mpInput->GetFrameHeight() << 
               " VideoGrabber is source of size");
  return mpOutput->SetFrameData(0, 0,
        mpInput->GetFrameWidth(), mpInput->GetFrameHeight(),
           (const BYTE *)buf, TRUE);  
}

BOOL PVideoChannel::Close()
{
  PWaitAndSignal m(accessMutex);

  CloseVideoReader();
  CloseVideoPlayer();

  return TRUE;
}

/*returns true if either input or output is open */
BOOL PVideoChannel::IsOpen() const 
{
   PWaitAndSignal m(accessMutex);

   return (mpInput != NULL) || (mpOutput != NULL);
}


PString PVideoChannel::GetName() const
{
  return deviceName;
}

void PVideoChannel::AttachVideoPlayer(PVideoOutputDevice * device, BOOL keepCurrent)
{
  PWaitAndSignal m(accessMutex);

  if (mpOutput && keepCurrent)
    PAssertAlways("Error: Attempt to add video player while one is already defined");
  
  CloseVideoPlayer();
   
  mpOutput = device;
}

void PVideoChannel::AttachVideoReader(PVideoInputDevice * device, BOOL keepCurrent)
{
  PWaitAndSignal m(accessMutex);

  if ((mpInput != NULL) && keepCurrent)
    PAssertAlways("Error: Attempt to add video reader while one is already defined");
  
  CloseVideoReader();
  
  mpInput = device;
}

void PVideoChannel::CloseVideoPlayer()
{
  PWaitAndSignal m(accessMutex);

  if (mpOutput != NULL)
    delete mpOutput;
  
  mpOutput = NULL;
}

void PVideoChannel::CloseVideoReader()
{
  PWaitAndSignal m(accessMutex);

  if (mpInput != NULL)
    delete mpInput;
  
  mpInput = NULL;
}

PINDEX  PVideoChannel::GetGrabHeight() 
{
   PWaitAndSignal m(accessMutex);
   if (mpInput != NULL)
     return mpInput->GetFrameHeight();
   else
     return 0;
}


PINDEX  PVideoChannel::GetGrabWidth()
{
   PWaitAndSignal m(accessMutex);

   if (mpInput != NULL)
     return mpInput->GetFrameWidth();
   else
     return 0;
}

BOOL PVideoChannel::IsGrabberOpen()
{
  PWaitAndSignal m(accessMutex);

  if (mpInput != NULL)
    return mpInput->IsOpen();
  else
    return FALSE; 
}

BOOL PVideoChannel::IsRenderOpen()      
{
  PWaitAndSignal m(accessMutex);

  if (mpOutput != NULL)
    return mpOutput->IsOpen();
  else
    return FALSE; 
}

BOOL PVideoChannel::DisplayRawData(void *videoBuffer)
{
  PWaitAndSignal m(accessMutex);

  if ((mpOutput == NULL) || (mpInput == NULL))
    return FALSE;
  
  PINDEX length=0;

  int frameWidth  = GetGrabWidth();
  int frameHeight = GetGrabHeight();
  PTRACE(6,"Video\t data direct:: camera-->render, size " << frameWidth << "x" << frameHeight );
  
  SetRenderFrameSize(frameWidth, frameHeight);
  Read(videoBuffer, length);
  Write((const void *)videoBuffer, length);
  
  return TRUE;      
}

void  PVideoChannel::SetGrabberFrameSize(int _width, int _height)     
{ 
  PTRACE(6, "PVC\t Set Grabber frame size to " << _width << "x" << _height);
  PWaitAndSignal m(accessMutex);

  if (mpInput != NULL) {
    if ((GetGrabWidth() != _width) || (GetGrabHeight() != _height))
      mpInput->SetFrameSize((unsigned)_width, (unsigned)_height);
  } 
}

void  PVideoChannel::SetRenderFrameSize(int _width, int _height) 
{ 
  PTRACE(6, "PVC\t Set Renderer frame size to " << _width << "x" << _height);
  PWaitAndSignal m(accessMutex);

  if (mpOutput != NULL)
    mpOutput->SetFrameSize(_width, _height); 
}

PVideoInputDevice *PVideoChannel::GetVideoReader()
{
  return mpInput;
}

PVideoOutputDevice *PVideoChannel::GetVideoPlayer()
{
  return mpOutput;
}

BOOL  PVideoChannel::Redraw(const void * frame) 
{ 
  PTRACE(6,"PVC\t::Redraw a frame");
  return Write(frame, 0);
}

PINDEX   PVideoChannel::GetRenderWidth()
{ 
  PWaitAndSignal m(accessMutex);

  if (mpOutput != NULL)
    return mpOutput->GetFrameWidth(); 

  return 0;
}

PINDEX  PVideoChannel::GetRenderHeight()
{
  PWaitAndSignal m(accessMutex);

 if (mpOutput != NULL)
  return mpOutput->GetFrameHeight(); 

 return 0;
}


void PVideoChannel::RestrictAccess()
{
  accessMutex.Wait();
}

void PVideoChannel::EnableAccess()
{
  accessMutex.Signal();
}


BOOL PVideoChannel::ToggleVFlipInput()
{
  PWaitAndSignal m(accessMutex);

 if (mpOutput != NULL)
  return mpInput->SetVFlipState(mpInput->GetVFlipState()); 

 return FALSE;
}

#endif

///////////////////////////////////////////////////////////////////////////
// End of file

