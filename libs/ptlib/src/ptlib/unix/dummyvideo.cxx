/*
 * dummyvideo.cxx
 *
 * Classes to support streaming video input (grabbing) and output.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2001 Equivalence Pty. Ltd.
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
 * Contributor(s): Roger Hardiman <roger@freebsd.org>
 *
 * $Log: dummyvideo.cxx,v $
 * Revision 1.8  2004/01/02 23:30:18  rjongbloed
 * Removed extraneous static function for getting input device names that has been deprecated during the plug ins addition.
 *
 * Revision 1.7  2002/04/10 08:40:36  rogerh
 * Simplify the SetVideoChannelFormat() code. Use the implementation in the
 * ancestor class.
 *
 * Revision 1.6  2002/04/05 06:41:54  rogerh
 * Apply video changes from Damien Sandras <dsandras@seconix.com>.
 * The Video Channel and Format are no longer set in Open(). Instead
 * call the new SetVideoChannelFormat() method. This makes video capture
 * and GnomeMeeting more stable with certain Linux video capture devices.
 *
 * Revision 1.5  2002/01/14 11:52:44  rogerh
 * Add TestAllFormats
 *
 * Revision 1.4  2001/12/05 08:34:41  rogerh
 * Add more dummy functions
 *
 * Revision 1.3  2001/09/24 09:11:07  rogerh
 * Add dummy methods, submitted by Andreas Wrede <awrede@mac.com>
 *
 * Revision 1.2  2001/06/27 17:43:32  rogerh
 * MakeOpen to return FALSE. Also change every things else to return failure.
 *
 * Revision 1.1  2001/06/27 17:27:23  rogerh
 * Dummy video allows code dependent on PVideoInputDevice to compile and link.
 * It has no functionality.
 *
 */

#pragma implementation "videoio.h"

#include <ptlib.h>
#include <ptlib/videoio.h>
#include <ptlib/vfakeio.h>
#include <ptlib/vconvert.h>

///////////////////////////////////////////////////////////////////////////////
// PVideoInputDevice

PVideoInputDevice::PVideoInputDevice()
{
}


BOOL PVideoInputDevice::Open(const PString & devName, BOOL startImmediate)
{
  return FALSE;    
}


BOOL PVideoInputDevice::IsOpen() 
{
  return FALSE;    
}


BOOL PVideoInputDevice::Close()
{
  return FALSE;    
}


BOOL PVideoInputDevice::Start()
{
  return FALSE;
}


BOOL PVideoInputDevice::Stop()
{
  return FALSE;
}


BOOL PVideoInputDevice::IsCapturing()
{
  return FALSE;
}


BOOL PVideoInputDevice::SetVideoFormat(VideoFormat newFormat)
{
  return FALSE;
}


int PVideoInputDevice::GetBrightness()
{
  return -1;
}


BOOL PVideoInputDevice::SetBrightness(unsigned newBrightness)
{
  return FALSE;
}


int PVideoInputDevice::GetHue()
{
  return -1;
}


BOOL PVideoInputDevice::SetHue(unsigned newHue)
{
  return FALSE;
}


int PVideoInputDevice::GetContrast()
{
  return -1;
}


BOOL PVideoInputDevice::SetContrast(unsigned newContrast)
{
  return FALSE;
}


BOOL PVideoInputDevice::GetParameters (int *whiteness, int *brightness,
                                       int *colour, int *contrast, int *hue)
{
  return FALSE;
}


int PVideoInputDevice::GetNumChannels() 
{
  return 0;
}


BOOL PVideoInputDevice::SetChannel(int newChannel)
{
  return FALSE;
}


BOOL PVideoInputDevice::SetColourFormat(const PString & newFormat)
{
  return FALSE;
}


BOOL PVideoInputDevice::SetFrameRate(unsigned rate)
{
  return FALSE;
}


BOOL PVideoInputDevice::GetFrameSizeLimits(unsigned & minWidth,
                                           unsigned & minHeight,
                                           unsigned & maxWidth,
                                           unsigned & maxHeight) 
{
  return FALSE;
}


BOOL PVideoInputDevice::SetFrameSize(unsigned width, unsigned height)
{
  return FALSE;
}


PINDEX PVideoInputDevice::GetMaxFrameBytes()
{
  return 0;
}



BOOL PVideoInputDevice::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{
  return FALSE;
}


BOOL PVideoInputDevice::GetFrameDataNoDelay(BYTE * buffer, PINDEX * bytesReturned)
{
  return FALSE;
}


void PVideoInputDevice::ClearMapping()
{
}

BOOL PVideoInputDevice::VerifyHardwareFrameSize(unsigned width,
                                                unsigned height)
{
	// Assume the size is valid
	return TRUE;
}

BOOL PVideoInputDevice::TestAllFormats()
{
  return TRUE;
}
    
// End Of File ///////////////////////////////////////////////////////////////
