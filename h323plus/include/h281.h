/*
 * h281.h
 *
 * H.281 PDU implementation for the OpenH323 Project.
 *
 * Copyright (c) 2006 Network for Educational Technology, ETH Zurich.
 * Written by Hannes Friederich.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: h281.h,v $
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.1  2006/06/22 11:07:22  shorne
 * Backport of FECC (H.224) from Opal
 *
 * Revision 1.2  2006/04/23 18:52:19  dsandras
 * Removed warnings when compiling with gcc on Linux.
 *
 * Revision 1.1  2006/04/20 16:48:17  hfriederich
 * Initial version of H.224/H.281 implementation.
 *
 */

#ifndef __OPAL_H281_H
#define __OPAL_H281_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>
#include <h224.h>

class H281_Frame : public H224_Frame
{
  PCLASSINFO(H281_Frame, H224_Frame);
	
public:
	
  enum RequestType {
	IllegalRequest		= 0x00,
	StartAction			= 0x01,
	ContinueAction		= 0x02,
	StopAction			= 0x03,
	SelectVideoSource	= 0x04,
	VideoSourceSwitched = 0x05,
	StoreAsPreset		= 0x07,
	ActivatePreset		= 0x08
  };
	
  enum PanDirection {
	NoPan		= 0x00,
	IllegalPan	= 0x40,
	PanLeft		= 0x80,
	PanRight	= 0xc0,
  };
	
  enum TiltDirection {
    NoTilt		= 0x00,
	IllegalTilt = 0x10,
	TiltDown	= 0x20,
	TiltUp		= 0x30,
  };
	
  enum ZoomDirection {
    NoZoom		= 0x00,
	IllegalZoom = 0x04,
	ZoomOut		= 0x08,
	ZoomIn		= 0x0c
  };
	
  enum FocusDirection {
    NoFocus			= 0x00,
	IllegalFocus	= 0x01,
	FocusOut		= 0x02,
	FocusIn			= 0x03
  };
	
  enum VideoMode {	
    MotionVideo					= 0x00,
	IllegalVideoMode			= 0x01,
	NormalResolutionStillImage	= 0x02,
	DoubleResolutionStillImage	= 0x03
  };
	
  H281_Frame();
  ~H281_Frame();
	
  RequestType GetRequestType() const { return (RequestType)(GetClientDataPtr())[0]; }
  void SetRequestType(RequestType requestType);
	
  // The following methods are only valid when
  // request type is either StartAction, ContinueAction or StopAction	
  PanDirection GetPanDirection() const;
  void SetPanDirection(PanDirection direction);
	
  TiltDirection GetTiltDirection() const;
  void SetTiltDirection(TiltDirection direction);

  ZoomDirection GetZoomDirection() const;
  void SetZoomDirection(ZoomDirection direction);
	
  FocusDirection GetFocusDirection() const;
  void SetFocusDirection(FocusDirection direction);
	
  // Only valid when RequestType is StartAction
  BYTE GetTimeout() const;
  void SetTimeout(BYTE timeout);
	
  // Only valid when RequestType is SelectVideoSource or VideoSourceSwitched
  BYTE GetVideoSourceNumber() const;
  void SetVideoSourceNumber(BYTE videoSourceNumber);
	
  VideoMode GetVideoMode() const;
  void SetVideoMode(VideoMode videoMode);
	
  // Only valid when RequestType is StoreAsPreset or ActivatePreset
  BYTE GetPresetNumber() const;
  void SetPresetNumber(BYTE presetNumber);
};

#endif // __OPAL_H281_H

