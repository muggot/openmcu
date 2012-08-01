/*
 * h323h224.h
 *
 * H.323 H.224 logical channel establishment implementation for the 
 * OpenH323 Project.
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
 * $Log: h323h224.h,v $
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.1  2006/06/22 11:07:22  shorne
 * Backport of FECC (H.224) from Opal
 *
 * Revision 1.2  2006/04/30 09:25:08  csoutheren
 * Remove warning about missing newline
 *
 * Revision 1.1  2006/04/20 16:48:17  hfriederich
 * Initial version of H.224/H.281 implementation.
 *
 */

#ifndef __OPAL_H323H224_H
#define __OPAL_H323H224_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>

#include <h224.h>
#include <h224handler.h>

#define H323_H224_CAPABILITY_NAME "H.224"

/** This class describes the H.224 capability
 */
class H323_H224Capability : public H323DataCapability
{
  PCLASSINFO(H323_H224Capability, H323DataCapability);
	
public:
	
  H323_H224Capability();
  ~H323_H224Capability();
	
  Comparison Compare(const PObject & obj) const;
	
  virtual PObject * Clone() const;
	
  virtual unsigned GetSubType() const;
	
  virtual PString GetFormatName() const;
	
  virtual H323Channel * CreateChannel(H323Connection & connection,
									  H323Channel::Directions dir,
									  unsigned sesionID,
									  const H245_H2250LogicalChannelParameters * param) const;
	
  virtual BOOL OnSendingPDU(H245_DataApplicationCapability & pdu) const;
  virtual BOOL OnSendingPDU(H245_DataMode & pdu) const;
  virtual BOOL OnReceivedPDU(const H245_DataApplicationCapability & pdu);
	
};

/** This class implements a H.224 logical channel
 */
class H323_H224Channel : public H323Channel
{
  PCLASSINFO(H323_H224Channel, H323Channel);
	
public:
  H323_H224Channel(H323Connection & connection,
				   const H323Capability & capability,
				   Directions direction,
				   RTP_UDP & session,
				   unsigned sessionID);
  ~H323_H224Channel();
	
  virtual H323Channel::Directions GetDirection() const;
  virtual BOOL SetInitialBandwidth();

  virtual void Receive();
  virtual void Transmit();
		
  virtual BOOL Open();
  virtual BOOL Start();
  virtual void Close();
	
  virtual BOOL OnSendingPDU(H245_OpenLogicalChannel & openPDU) const;
  virtual void OnSendOpenAck(const H245_OpenLogicalChannel & openPDU, 
							 H245_OpenLogicalChannelAck & ack) const;
  virtual BOOL OnReceivedPDU(const H245_OpenLogicalChannel & pdu, unsigned & errorCode);
  virtual BOOL OnReceivedAckPDU(const H245_OpenLogicalChannelAck & pdu);
	
  virtual BOOL OnSendingPDU(H245_H2250LogicalChannelParameters & param) const;
  virtual void OnSendOpenAck(H245_H2250LogicalChannelAckParameters & param) const;
  virtual BOOL OnReceivedPDU(const H245_H2250LogicalChannelParameters & param,
							 unsigned & errorCode);
  virtual BOOL OnReceivedAckPDU(const H245_H2250LogicalChannelAckParameters & param);
	
  virtual BOOL SetDynamicRTPPayloadType(int newType);
  RTP_DataFrame::PayloadTypes GetDynamicRTPPayloadType() const { return rtpPayloadType; }
	
//  virtual OpalMediaStream * GetMediaStream() const;
	
  OpalH224Handler * GetHandler() const { return h224Handler; }
	
protected:
		
  virtual BOOL ExtractTransport(const H245_TransportAddress & pdu,
								BOOL isDataPort,
								unsigned & errorCode);
	
  unsigned sessionID;
  Directions direction;
  RTP_UDP & rtpSession;
  H323_RTP_Session & rtpCallbacks;
  OpalH224Handler *h224Handler;
  RTP_DataFrame::PayloadTypes rtpPayloadType;
  
};

#endif // __OPAL_H323H224_H

