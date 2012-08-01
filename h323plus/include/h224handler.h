/*
 * h224handler.h
 *
 * H.224 protocol handler implementation for the OpenH323 Project.
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
 * $Log: h224handler.h,v $
 * Revision 1.1  2007/08/06 20:50:48  shorne
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

#ifndef __H323_H224HANDLER_H
#define __H323_H224HANDLER_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>
//#include <opal/connection.h>
//#include <transports.h>
#include <rtp.h>
#include <h281handler.h>

#define H281_CLIENT_ID 0x01

class H224_Frame;
class OpalH224Handler;

class OpalH224ReceiverThread : public PThread
{
  PCLASSINFO(OpalH224ReceiverThread, PThread);
	
public:
	
  OpalH224ReceiverThread(OpalH224Handler *h224Handler, RTP_Session & rtpSession);
  ~OpalH224ReceiverThread();
	
  virtual void Main();
	
  void Close();
	
private:
		
  OpalH224Handler *h224Handler;
  mutable PMutex inUse;
  unsigned timestamp;
  RTP_Session & rtpSession;
  BOOL terminate;
};

class OpalH281Handler;
class H323Connection;

class OpalH224Handler : public PObject
{
  PCLASSINFO(OpalH224Handler, PObject);
	
public:
	
  OpalH224Handler(H323Connection & connection, unsigned sessionID);
  ~OpalH224Handler();
	
  virtual void StartTransmit();
  virtual void StopTransmit();
  virtual void StartReceive();
  virtual void StopReceive();
	
  BOOL SendClientList();
  BOOL SendExtraCapabilities();
  BOOL SendClientListCommand();
  BOOL SendExtraCapabilitiesCommand(BYTE clientID);

  BOOL SendExtraCapabilitiesMessage(BYTE clientID, BYTE *data, PINDEX length);

  BOOL TransmitClientFrame(BYTE clientID, H224_Frame & frame);
	
  virtual BOOL OnReceivedFrame(H224_Frame & frame);
  virtual BOOL OnReceivedCMEMessage(H224_Frame & frame);
  virtual BOOL OnReceivedClientList(H224_Frame & frame);
  virtual BOOL OnReceivedClientListCommand();
  virtual BOOL OnReceivedExtraCapabilities(H224_Frame & frame);
  virtual BOOL OnReceivedExtraCapabilitiesCommand();
	
  PMutex & GetTransmitMutex() { return transmitMutex; }
	
  RTP_Session * GetSession() const { return session; }
	
  virtual OpalH224ReceiverThread * CreateH224ReceiverThread();
	
  OpalH281Handler *GetH281Handler() { return h281Handler; }
	
protected:

  RTP_Session * session;

  BOOL canTransmit;
  PMutex transmitMutex;
  RTP_DataFrame *transmitFrame;
  BYTE transmitBitIndex;
  PTime *transmitStartTime;
	
  OpalH224ReceiverThread *receiverThread;
	
  OpalH281Handler *h281Handler;
	
private:
		
  void TransmitFrame(H224_Frame & frame);
	
};

#endif // __H323_H224HANDLER_H

