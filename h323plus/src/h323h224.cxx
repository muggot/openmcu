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
 * $Log: h323h224.cxx,v $
 * Revision 1.5  2008/01/24 08:09:48  shorne
 * Fix for H224 capability exchange Thx mohammad alam
 *
 * Revision 1.4  2008/01/23 22:08:12  shorne
 * added missing const
 *
 * Revision 1.3  2008/01/22 01:11:22  shorne
 * Fix H.224 Capability exchange
 *
 * Revision 1.2  2007/11/01 14:34:50  willamowius
 * add newline at end of file
 *
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.2  2006/06/23 06:02:44  csoutheren
 * Added missing declarations for H.224 backport
 *
 * Revision 1.1  2006/06/22 11:07:23  shorne
 * Backport of FECC (H.224) from Opal
 *
 * Revision 1.4  2006/06/07 08:02:22  hfriederich
 * Fixing crashes when creating the RTP session failed
 *
 * Revision 1.3  2006/05/01 10:29:50  csoutheren
 * Added pragams for gcc < 4
 *
 * Revision 1.2  2006/04/24 12:53:50  rjongbloed
 * Port of H.224 Far End Camera Control to DevStudio/Windows
 *
 * Revision 1.1  2006/04/20 16:48:17  hfriederich
 * Initial version of H.224/H.281 implementation.
 *
 */

#include <ptlib.h>
#include <h323.h>

#ifdef H323_H224

#ifdef __GNUC__
#pragma implementation "h323h224.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include <h323h224.h>

H323_H224Capability::H323_H224Capability()
: H323DataCapability(640)
{
//  SetPayloadType((RTP_DataFrame::PayloadTypes)100);
}

H323_H224Capability::~H323_H224Capability()
{
}

PObject::Comparison H323_H224Capability::Compare(const PObject & obj) const
{
  Comparison result = H323DataCapability::Compare(obj);

  if(result != EqualTo)	{
    return result;
  }
	
  PAssert(PIsDescendant(&obj, H323_H224Capability), PInvalidCast);
	
  return EqualTo;
}

PObject * H323_H224Capability::Clone() const
{
  return new H323_H224Capability(*this);
}

unsigned H323_H224Capability::GetSubType() const
{
  return H245_DataApplicationCapability_application::e_h224;
}

PString H323_H224Capability::GetFormatName() const
{
  return "H.224";
}

H323Channel * H323_H224Capability::CreateChannel(H323Connection & connection,
                                                 H323Channel::Directions direction,
                                                 unsigned int sessionID,
                                                 const H245_H2250LogicalChannelParameters * /*params*/) const
{
 
	RTP_Session *session;
    H245_TransportAddress addr;
    connection.GetControlChannel().SetUpTransportPDU(addr, H323Transport::UseLocalTSAP);
    session = connection.UseSession(sessionID, addr, direction);
	
  if(session == NULL) {
    return NULL;
  } 
  
  return new H323_H224Channel(connection, *this, direction, (RTP_UDP &)*session, sessionID);
}

BOOL H323_H224Capability::OnSendingPDU(H245_DataApplicationCapability & pdu) const
{
  pdu.m_maxBitRate = maxBitRate;
  pdu.m_application.SetTag(H245_DataApplicationCapability_application::e_h224);
	
  H245_DataProtocolCapability & dataProtocolCapability = (H245_DataProtocolCapability &)pdu.m_application;
  dataProtocolCapability.SetTag(H245_DataProtocolCapability::e_hdlcFrameTunnelling);
	
  return TRUE;
}

BOOL H323_H224Capability::OnSendingPDU(H245_DataMode & pdu) const
{
  pdu.m_bitRate = maxBitRate;
  pdu.m_application.SetTag(H245_DataMode_application::e_h224);
	
  return TRUE;
}

BOOL H323_H224Capability::OnReceivedPDU(const H245_DataApplicationCapability & pdu)
{

  if (pdu.m_application.GetTag() != H245_DataApplicationCapability_application::e_h224)
	  return FALSE;

  const H245_DataProtocolCapability & dataProtocolCapability = pdu.m_application;
  if (dataProtocolCapability.GetTag() != H245_DataProtocolCapability::e_hdlcFrameTunnelling)
	  return FALSE;

  maxBitRate = pdu.m_maxBitRate;
  return TRUE;
}

H323_H224Channel::H323_H224Channel(H323Connection & connection,
                                   const H323Capability & capability,
                                   H323Channel::Directions theDirection,
                                   RTP_UDP & theSession,
                                   unsigned theSessionID)
: H323Channel(connection, capability),
  rtpSession(theSession),
  rtpCallbacks(*(H323_RTP_Session *)theSession.GetUserData())
{
  direction = theDirection;
  sessionID = theSessionID;
	
  h224Handler = NULL;
	
  rtpPayloadType = (RTP_DataFrame::PayloadTypes)100;
}

H323_H224Channel::~H323_H224Channel()
{
  // h224Handler is deleted by OpalConnection
}

H323Channel::Directions H323_H224Channel::GetDirection() const
{
  return direction;
}

BOOL H323_H224Channel::SetInitialBandwidth()
{
  return TRUE;
}

BOOL H323_H224Channel::Open()
{
  BOOL result = H323Channel::Open();
	
  if(result == FALSE) {
    return FALSE;
  }
	
  return TRUE;
}

BOOL H323_H224Channel::Start()
{
  if(!Open()) {
    return FALSE;
  }
	
  if(h224Handler == NULL) {
	  h224Handler = connection.CreateH224ProtocolHandler(sessionID);
  }
	
  if(direction == H323Channel::IsReceiver) {
    h224Handler->StartReceive();
  }	else {
    h224Handler->StartTransmit();
  }
	
  return TRUE;
}

void H323_H224Channel::Close()
{
  if(terminating) {
    return;
  }
	
  if(h224Handler != NULL) {
	
    if(direction == H323Channel::IsReceiver) {
      h224Handler->StopReceive();
    } else {
      h224Handler->StopTransmit();
    }
  }
	
 // H323Channel::Close();
}

void H323_H224Channel::Receive()
{

}
void H323_H224Channel::Transmit()
{

}

BOOL H323_H224Channel::OnSendingPDU(H245_OpenLogicalChannel & open) const
{
  open.m_forwardLogicalChannelNumber = (unsigned)number;
		
  if(open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters)) {
	  
	open.m_reverseLogicalChannelParameters.IncludeOptionalField(
		H245_OpenLogicalChannel_reverseLogicalChannelParameters::e_multiplexParameters);
			
    open.m_reverseLogicalChannelParameters.m_multiplexParameters.SetTag(
		H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters);
			
    return OnSendingPDU(open.m_reverseLogicalChannelParameters.m_multiplexParameters);
	
  }	else {
	  
    open.m_forwardLogicalChannelParameters.m_multiplexParameters.SetTag(
		H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters);
		
    return OnSendingPDU(open.m_forwardLogicalChannelParameters.m_multiplexParameters);
  }
}

void H323_H224Channel::OnSendOpenAck(const H245_OpenLogicalChannel & openPDU, 
										H245_OpenLogicalChannelAck & ack) const
{
  // set forwardMultiplexAckParameters option
  ack.IncludeOptionalField(H245_OpenLogicalChannelAck::e_forwardMultiplexAckParameters);
	
  // select H225 choice
  ack.m_forwardMultiplexAckParameters.SetTag(
    H245_OpenLogicalChannelAck_forwardMultiplexAckParameters::e_h2250LogicalChannelAckParameters);
	
  // get H225 params
  H245_H2250LogicalChannelAckParameters & param = ack.m_forwardMultiplexAckParameters;
	
  // set session ID
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_sessionID);
  const H245_H2250LogicalChannelParameters & openparam =
	  openPDU.m_forwardLogicalChannelParameters.m_multiplexParameters;
	
  unsigned sessionID = openparam.m_sessionID;
  param.m_sessionID = sessionID;
	
  OnSendOpenAck(param);
}

BOOL H323_H224Channel::OnReceivedPDU(const H245_OpenLogicalChannel & open,
									 unsigned & errorCode)
{
  if(direction == H323Channel::IsReceiver) {
    number = H323ChannelNumber(open.m_forwardLogicalChannelNumber, TRUE);
  }
	
  BOOL reverse = open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters);
  const H245_DataType & dataType = reverse ? open.m_reverseLogicalChannelParameters.m_dataType
										   : open.m_forwardLogicalChannelParameters.m_dataType;
	
  if (!capability->OnReceivedPDU(dataType, direction)) {
	  
    errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotSupported;
    return FALSE;
  }
	
  if (reverse) {
    if (open.m_reverseLogicalChannelParameters.m_multiplexParameters.GetTag() ==
			H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters) 
	{
      return OnReceivedPDU(open.m_reverseLogicalChannelParameters.m_multiplexParameters, errorCode);
    }
	  
  } else {
    if (open.m_forwardLogicalChannelParameters.m_multiplexParameters.GetTag() ==
			H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters)
    {
      return OnReceivedPDU(open.m_forwardLogicalChannelParameters.m_multiplexParameters, errorCode);
    }
  }

  errorCode = H245_OpenLogicalChannelReject_cause::e_unsuitableReverseParameters;
  return FALSE;
}

BOOL H323_H224Channel::OnReceivedAckPDU(const H245_OpenLogicalChannelAck & ack)
{
  if (!ack.HasOptionalField(H245_OpenLogicalChannelAck::e_forwardMultiplexAckParameters)) {
    return FALSE;
  }
	
  if (ack.m_forwardMultiplexAckParameters.GetTag() !=
	H245_OpenLogicalChannelAck_forwardMultiplexAckParameters::e_h2250LogicalChannelAckParameters)
  {
	return FALSE;
  }
	
  return OnReceivedAckPDU(ack.m_forwardMultiplexAckParameters);
}

BOOL H323_H224Channel::OnSendingPDU(H245_H2250LogicalChannelParameters & param) const
{
  param.m_sessionID = sessionID;
	
  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaGuaranteedDelivery);
  param.m_mediaGuaranteedDelivery = FALSE;
	
  // unicast must have mediaControlChannel
  H323TransportAddress mediaControlAddress(rtpSession.GetLocalAddress(), rtpSession.GetLocalControlPort());
  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel);
  mediaControlAddress.SetPDU(param.m_mediaControlChannel);
	
  if (direction == H323Channel::IsReceiver) {
    // set mediaChannel
    H323TransportAddress mediaAddress(rtpSession.GetLocalAddress(), rtpSession.GetLocalDataPort());
    param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
    mediaAddress.SetPDU(param.m_mediaChannel);
	
  }	else{

  }
	
  // Set dynamic payload type, if is one
  int rtpPayloadType = GetDynamicRTPPayloadType();
  
  if (rtpPayloadType >= RTP_DataFrame::DynamicBase && rtpPayloadType < RTP_DataFrame::IllegalPayloadType) {
    param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_dynamicRTPPayloadType);
    param.m_dynamicRTPPayloadType = rtpPayloadType;
  }
	
  return TRUE;
}

void H323_H224Channel::OnSendOpenAck(H245_H2250LogicalChannelAckParameters & param) const
{
  // set mediaControlChannel
  H323TransportAddress mediaControlAddress(rtpSession.GetLocalAddress(), rtpSession.GetLocalControlPort());
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaControlChannel);
  mediaControlAddress.SetPDU(param.m_mediaControlChannel);
	
  // set mediaChannel
  H323TransportAddress mediaAddress(rtpSession.GetLocalAddress(), rtpSession.GetLocalDataPort());
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
  mediaAddress.SetPDU(param.m_mediaChannel);
	
  // Set dynamic payload type, if is one
  int rtpPayloadType = GetDynamicRTPPayloadType();
  if (rtpPayloadType >= RTP_DataFrame::DynamicBase && rtpPayloadType < RTP_DataFrame::IllegalPayloadType) {
    param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_dynamicRTPPayloadType);
    param.m_dynamicRTPPayloadType = rtpPayloadType;
  }
}

BOOL H323_H224Channel::OnReceivedPDU(const H245_H2250LogicalChannelParameters & param,
						   unsigned & errorCode)
{
  if (param.m_sessionID != sessionID) {
	errorCode = H245_OpenLogicalChannelReject_cause::e_invalidSessionID;
	return FALSE;
  }
	
  BOOL ok = FALSE;
	
  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel)) {
		
	if (!ExtractTransport(param.m_mediaControlChannel, FALSE, errorCode)) {
	  return FALSE;
	}
	
	ok = TRUE;
  }
	
  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaChannel)) {
	if (ok && direction == H323Channel::IsReceiver) {
		
	} else if (!ExtractTransport(param.m_mediaChannel, TRUE, errorCode)) {
      return FALSE;
    }
    
    ok = TRUE;
  }
	
  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_dynamicRTPPayloadType)) {
    SetDynamicRTPPayloadType(param.m_dynamicRTPPayloadType);
  }
	
  if (ok) {
    return TRUE;
  }
	
  errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
  return FALSE;
}

BOOL H323_H224Channel::OnReceivedAckPDU(const H245_H2250LogicalChannelAckParameters & param)
{
  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_sessionID)) {
  }
	
  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaControlChannel)) {
    return FALSE;
  }
	
  unsigned errorCode;
  if (!ExtractTransport(param.m_mediaControlChannel, FALSE, errorCode)) {
    return FALSE;
  }
	
  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel)) {
    return FALSE;
  }
	
  if (!ExtractTransport(param.m_mediaChannel, TRUE, errorCode)) {
    return FALSE;
  }
	
  if (param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_dynamicRTPPayloadType)) {
    SetDynamicRTPPayloadType(param.m_dynamicRTPPayloadType);
  }
	
  return TRUE;
}

BOOL H323_H224Channel::SetDynamicRTPPayloadType(int newType)
{
  if(newType == -1) {
    return TRUE;
  }
	
  if(newType < RTP_DataFrame::DynamicBase || newType >= RTP_DataFrame::IllegalPayloadType) {
    return FALSE;
  }
	
  if(rtpPayloadType < RTP_DataFrame::DynamicBase) {
    return FALSE;
  }
	
  rtpPayloadType = (RTP_DataFrame::PayloadTypes)newType;
	
  return TRUE;
}
/*
OpalMediaStream * H323_H224Channel::GetMediaStream() const
{
  // implemented since declared as an abstract method in H323Channel
  return NULL;
}
*/
BOOL H323_H224Channel::ExtractTransport(const H245_TransportAddress & pdu,
										BOOL isDataPort,
										unsigned & errorCode)
{
  if (pdu.GetTag() != H245_TransportAddress::e_unicastAddress) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_multicastChannelNotAllowed;
    return FALSE;
  }
	
  H323TransportAddress transAddr = pdu;
	
  PIPSocket::Address ip;
  WORD port;
  if (transAddr.GetIpAndPort(ip, port)) {
    return rtpSession.SetRemoteSocketInfo(ip, port, isDataPort);
  }
	
  return FALSE;
}

#endif

