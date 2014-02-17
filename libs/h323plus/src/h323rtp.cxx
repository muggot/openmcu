/*
 * h323rtp.cxx
 *
 * H.323 RTP protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1999-2000 Equivalence Pty. Ltd.
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h323rtp.cxx,v $
 * Revision 1.5  2008/02/06 02:52:59  shorne
 * Added support for Standards based NAT Traversal
 *
 * Revision 1.4  2007/11/16 22:09:43  shorne
 * Added ability to disable H.245 QoS for NetMeeting Interop
 *
 * Revision 1.3  2007/10/19 19:54:18  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.2  2007/10/16 17:08:57  shorne
 * Qos capability negotiation
 *
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.34.2.5  2007/09/05 04:14:40  rjongbloed
 * Back ported from OPAL media packetization in TCS
 *
 * Revision 1.34.2.4  2007/09/03 09:44:44  rjongbloed
 * Back ported from OPAL the H.323 media packetization support
 *
 * Revision 1.34.2.3  2007/05/23 06:58:02  shorne
 * Nat Support for EP's nested behind same NAT
 *
 * Revision 1.34.2.2  2007/02/11 00:45:20  shorne
 * Added ability to disable NAT method on a call by call basis
 *
 * Revision 1.34.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.34  2006/01/26 03:38:54  shorne
 * Added transport capability exchange
 *
 * Revision 1.33  2005/06/21 06:46:35  csoutheren
 * Add ability to create capabilities without codecs for external RTP interface
 *
 * Revision 1.32  2005/01/03 14:03:42  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.31  2005/01/03 06:26:09  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.30  2004/11/25 07:38:59  csoutheren
 * Ensured that external TCP address translation is performed when using STUN to handle UDP
 *
 * Revision 1.29  2004/07/03 05:47:54  rjongbloed
 * Added virtual function for determining RTP payload type used in an H.323 channel,
 *    also some added bullet proofing for exception conditions, thanks Guilhem Tardy
 *
 * Revision 1.28  2004/04/03 08:28:07  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.27  2003/10/27 06:03:39  csoutheren
 * Added support for QoS
 *   Thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.26  2003/02/07 00:28:24  robertj
 * Changed function to virtual to help in using external multiicast RTP stacks.
 *
 * Revision 1.25  2003/02/05 06:32:10  robertj
 * Fixed non-stun symmetric NAT support recently broken.
 *
 * Revision 1.24  2003/02/05 01:55:14  robertj
 * Fixed setting of correct address in OLC's when STUN is used.
 *
 * Revision 1.23  2003/02/04 07:06:41  robertj
 * Added STUN support.
 *
 * Revision 1.22  2002/11/19 01:47:26  robertj
 * Included canonical name in RTP statistics returned in IRR
 *
 * Revision 1.21  2002/10/08 13:08:21  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.20  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.19  2002/07/02 10:02:32  robertj
 * Added H323TransportAddress::GetIpAddress() so don't have to provide port
 *   when you don't need it as in GetIpAndPort(),.
 *
 * Revision 1.18  2002/06/24 08:07:49  robertj
 * Fixed setting of H.225.0 logical channel parameter silenceSuppression field
 *   to correctly indicate if codec is going to stop sending RTP on silence.
 *
 * Revision 1.17  2002/05/28 06:27:23  robertj
 * Split UDP (for RAS) from RTP port bases.
 * Added current port variable so cycles around the port range specified which
 *   fixes some wierd problems on some platforms, thanks Federico Pinna
 *
 * Revision 1.16  2001/10/02 02:06:23  robertj
 * Fixed CIsco IOS compatibility, yet again!.
 *
 * Revision 1.15  2001/10/02 01:53:53  robertj
 * Fixed CIsco IOS compatibility, again.
 *
 * Revision 1.14  2001/08/06 03:08:57  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.13  2001/02/09 05:13:56  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.12  2001/01/25 07:27:16  robertj
 * Major changes to add more flexible OpalMediaFormat class to normalise
 *   all information about media types, especially codecs.
 *
 * Revision 1.11  2000/12/18 08:59:20  craigs
 * Added ability to set ports
 *
 * Revision 1.10  2000/09/22 00:32:34  craigs
 * Added extra logging
 * Fixed problems with no fastConnect with tunelling
 *
 * Revision 1.9  2000/08/31 08:15:41  robertj
 * Added support for dynamic RTP payload types in H.245 OpenLogicalChannel negotiations.
 *
 * Revision 1.8  2000/08/23 14:27:04  craigs
 * Added prototype support for Microsoft GSM codec
 *
 * Revision 1.7  2000/07/12 13:06:49  robertj
 * Removed test for sessionID in OLC, just trace a warning instead of abandoning connection.
 *
 * Revision 1.6  2000/07/11 19:36:43  robertj
 * Fixed silenceSuppression field in OLC only to be included on transmitter.
 *
 * Revision 1.5  2000/05/23 12:57:37  robertj
 * Added ability to change IP Type Of Service code from applications.
 *
 * Revision 1.4  2000/05/02 04:32:27  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.3  2000/04/05 03:17:32  robertj
 * Added more RTP statistics gathering and H.245 round trip delay calculation.
 *
 * Revision 1.2  2000/01/20 05:57:46  robertj
 * Added extra flexibility in receiving incorrectly formed OpenLogicalChannel PDU's
 *
 * Revision 1.1  1999/12/23 23:02:36  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h323rtp.h"
#endif

#include "h323rtp.h"

#include "h323ep.h"
#include "h323pdu.h"


#define new PNEW

// #undef P_HAS_QOS


/////////////////////////////////////////////////////////////////////////////

H323_RTP_Session::H323_RTP_Session(const H323Connection & conn)
  : connection(conn)
{
}


void H323_RTP_Session::OnTxStatistics(const RTP_Session & session) const
{
  connection.OnRTPStatistics(session);
}


void H323_RTP_Session::OnRxStatistics(const RTP_Session & session) const
{
  connection.OnRTPStatistics(session);
}

void H323_RTP_Session::OnFinalStatistics(const RTP_Session & session) const
{
  connection.OnRTPFinalStatistics(session);
}


/////////////////////////////////////////////////////////////////////////////

H323_RTP_UDP::H323_RTP_UDP(const H323Connection & conn,
                                        RTP_UDP & rtp_udp,
                                        RTP_QOS * rtpQos)
  : H323_RTP_Session(conn),
    rtp(rtp_udp)
{
  const H323Transport & transport = connection.GetControlChannel();
  PIPSocket::Address localAddress;
  transport.GetLocalAddress().GetIpAddress(localAddress);

  H323EndPoint & endpoint = connection.GetEndPoint();

  PIPSocket::Address remoteAddress;
  transport.GetRemoteAddress().GetIpAddress(remoteAddress);

#ifdef P_STUN
  PNatMethod * meth = NULL;
  if (conn.HasNATSupport()) {
      meth = endpoint.GetPreferedNatMethod(remoteAddress);
	  if (meth != NULL) {
	     PTRACE(4, "RTP\tNAT Method " << meth->GetName()[0] << " selected for call.");
	  }
  }
#endif

  WORD firstPort = endpoint.GetRtpIpPortPair();
  WORD nextPort = firstPort;
  while (!rtp.Open(localAddress,
                   nextPort, nextPort,
                   endpoint.GetRtpIpTypeofService(),
				   conn,
#ifdef P_STUN
                   meth,
#else
		           NULL,
#endif
                   rtpQos)) {
    nextPort = endpoint.GetRtpIpPortPair();
    if (nextPort == firstPort)
      return;
  }

  localAddress = rtp.GetLocalAddress();
  endpoint.InternalTranslateTCPAddress(localAddress, remoteAddress, &conn);
  rtp.SetLocalAddress(localAddress);
}


BOOL H323_RTP_UDP::OnSendingPDU(const H323_RTPChannel & channel,
                                H245_H2250LogicalChannelParameters & param) const
{
  PTRACE(3, "RTP\tOnSendingPDU");

  param.m_sessionID = rtp.GetSessionID();

  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaGuaranteedDelivery);
  param.m_mediaGuaranteedDelivery = FALSE;

  // unicast must have mediaControlChannel
  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel);
  H323TransportAddress mediaControlAddress(rtp.GetLocalAddress(), rtp.GetLocalControlPort());
  mediaControlAddress.SetPDU(param.m_mediaControlChannel);

  if (channel.GetDirection() == H323Channel::IsReceiver) {
    // set mediaChannel
    param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
    H323TransportAddress mediaAddress(rtp.GetLocalAddress(), rtp.GetLocalDataPort());
    mediaAddress.SetPDU(param.m_mediaChannel);
  }

  H323Codec * codec = channel.GetCodec();

#ifndef NO_H323_AUDIO_CODECS
  // Set flag for we are going to stop sending audio on silence
  if (codec != NULL &&
      PIsDescendant(codec, H323AudioCodec) &&
      channel.GetDirection() != H323Channel::IsReceiver) {
    param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_silenceSuppression);
    param.m_silenceSuppression = ((H323AudioCodec*)codec)->GetSilenceDetectionMode() != H323AudioCodec::NoSilenceDetection;
  }
#endif

  // Set dynamic payload type, if is one
  RTP_DataFrame::PayloadTypes rtpPayloadType = channel.GetRTPPayloadType();
  if (rtpPayloadType >= RTP_DataFrame::DynamicBase && rtpPayloadType <= RTP_DataFrame::MaxPayloadType) {
    param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_dynamicRTPPayloadType);
    param.m_dynamicRTPPayloadType = rtpPayloadType;
  }

  // Set the media packetization field if have an option to describe it.
  if (codec != NULL) {
    param.m_mediaPacketization.SetTag(H245_H2250LogicalChannelParameters_mediaPacketization::e_rtpPayloadType);
    if (H323SetRTPPacketization(param.m_mediaPacketization, codec->GetMediaFormat(), rtpPayloadType))
//     if(connection.GetRemoteApplication().Find("NetMeeting") == P_MAX_INDEX) // netmeeting reject mp parameters
      param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaPacketization);
  }

  // GQoS
#if P_HAS_QOS
  if (connection.H245QoSEnabled() && WriteTransportCapPDU(param.m_transportCapability,channel)) {
		param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_transportCapability);
  }
#endif
  return TRUE;
}

BOOL H323_RTP_UDP::OnSendingAltPDU(const H323_RTPChannel & channel,
				H245_ArrayOf_GenericInformation & generic) const
{
	if (connection.isSameNAT()) 
        return connection.OnSendingRTPAltInformation(*this,generic);
	else
		return connection.OnSendingOLCGenericInformation(*this,generic);
}

void H323_RTP_UDP::OnSendingAckPDU(const H323_RTPChannel & channel,
                                   H245_H2250LogicalChannelAckParameters & param) const
{
  PTRACE(3, "RTP\tOnSendingAckPDU");

  // set mediaControlChannel
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaControlChannel);
  H323TransportAddress mediaControlAddress(rtp.GetLocalAddress(), rtp.GetLocalControlPort());
  mediaControlAddress.SetPDU(param.m_mediaControlChannel);

  // set mediaChannel
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
  H323TransportAddress mediaAddress(rtp.GetLocalAddress(), rtp.GetLocalDataPort());
  mediaAddress.SetPDU(param.m_mediaChannel);

  // Set dynamic payload type, if is one
  int rtpPayloadType = channel.GetRTPPayloadType();
  if (rtpPayloadType >= RTP_DataFrame::DynamicBase && rtpPayloadType <= RTP_DataFrame::MaxPayloadType) {
    param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_dynamicRTPPayloadType);
    param.m_dynamicRTPPayloadType = rtpPayloadType;
  }
}

void H323_RTP_UDP::OnSendOpenAckAlt(const H323_RTPChannel & channel, 
	  H245_ArrayOf_GenericInformation & alternate) const
{
   connection.OnSendingRTPAltInformation(*this,alternate);
}

BOOL H323_RTP_UDP::ExtractTransport(const H245_TransportAddress & pdu,
                                    BOOL isDataPort,
                                    unsigned & errorCode)
{
  if (pdu.GetTag() != H245_TransportAddress::e_unicastAddress) {
    PTRACE(1, "RTP_UDP\tOnly unicast supported at this time");
    errorCode = H245_OpenLogicalChannelReject_cause::e_multicastChannelNotAllowed;
    return FALSE;
  }

  H323TransportAddress transAddr = pdu;

  PIPSocket::Address ip;
  WORD port;
  if (transAddr.GetIpAndPort(ip, port))
    return rtp.SetRemoteSocketInfo(ip, port, isDataPort);

  return FALSE;
}


BOOL H323_RTP_UDP::OnReceivedPDU(H323_RTPChannel & channel,
                                 const H245_H2250LogicalChannelParameters & param,
                                 unsigned & errorCode)
{
  if (param.m_sessionID != rtp.GetSessionID()) {
    PTRACE(1, "RTP_UDP\tOpen of " << channel << " with invalid session: " << param.m_sessionID);
    errorCode = H245_OpenLogicalChannelReject_cause::e_invalidSessionID;
    return FALSE;
  }

  BOOL ok = FALSE;

  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel)) {
    if (!ExtractTransport(param.m_mediaControlChannel, FALSE, errorCode)) {
      PTRACE(1, "RTP_UDP\tFailed to extract mediaControl transport for " << channel);
      return FALSE;
    }
    ok = TRUE;
  }

  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaChannel)) {
    if (ok && channel.GetDirection() == H323Channel::IsReceiver)
      PTRACE(3, "RTP_UDP\tIgnoring media transport for " << channel);
    else if (!ExtractTransport(param.m_mediaChannel, TRUE, errorCode)) {
      PTRACE(1, "RTP_UDP\tFailed to extract media transport for " << channel);
      return FALSE;
    }
    ok = TRUE;
  }

  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_dynamicRTPPayloadType))
    channel.SetDynamicRTPPayloadType(param.m_dynamicRTPPayloadType);

  H323Codec * codec = channel.GetCodec();

  if (codec != NULL &&
      param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaPacketization) &&
      param.m_mediaPacketization.GetTag() == H245_H2250LogicalChannelParameters_mediaPacketization::e_rtpPayloadType)
    H323GetRTPPacketization(codec->GetWritableMediaFormat(), param.m_mediaPacketization);

  // GQoS
#if P_HAS_QOS
  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_transportCapability) && connection.H245QoSEnabled()) {
	 H245_TransportCapability trans = param.m_transportCapability;
		ReadTransportCapPDU(trans,channel);
  }
#endif

  if (ok)
    return TRUE;

  PTRACE(1, "RTP_UDP\tNo mediaChannel or mediaControlChannel specified for " << channel);
  errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
  return FALSE;
}

BOOL H323_RTP_UDP::OnReceivedAltPDU(H323_RTPChannel & channel, 
	  const H245_ArrayOf_GenericInformation & alternate)
{
	return connection.OnReceiveRTPAltInformation(*this,alternate);
}

BOOL H323_RTP_UDP::OnReceivedAckPDU(H323_RTPChannel & channel,
                                    const H245_H2250LogicalChannelAckParameters & param)
{
  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_sessionID)) {
    PTRACE(1, "RTP_UDP\tNo session specified");
  }

  if (param.m_sessionID != rtp.GetSessionID()) {
    PTRACE(1, "RTP_UDP\tAck for invalid session: " << param.m_sessionID);
  }

  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaControlChannel)) {
    PTRACE(1, "RTP_UDP\tNo mediaControlChannel specified");
    return FALSE;
  }

  unsigned errorCode;
  if (!ExtractTransport(param.m_mediaControlChannel, FALSE, errorCode))
    return FALSE;

  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel)) {
    PTRACE(1, "RTP_UDP\tNo mediaChannel specified");
    return FALSE;
  }

  if (!ExtractTransport(param.m_mediaChannel, TRUE, errorCode))
    return FALSE;

  if (param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_dynamicRTPPayloadType))
    channel.SetDynamicRTPPayloadType(param.m_dynamicRTPPayloadType);

  return TRUE;
}

BOOL H323_RTP_UDP::OnReceivedAckAltPDU(H323_RTPChannel & channel,
	  const H245_ArrayOf_GenericInformation & alternate)
{
	return connection.OnReceiveRTPAltInformation(*this,alternate);
}

void H323_RTP_UDP::OnSendRasInfo(H225_RTPSession & info)
{
  info.m_sessionId = rtp.GetSessionID();
  info.m_ssrc = rtp.GetSyncSourceOut();
  info.m_cname = rtp.GetCanonicalName();

  const H323Transport & transport = connection.GetControlChannel();

  transport.SetUpTransportPDU(info.m_rtpAddress.m_recvAddress, rtp.GetLocalDataPort());
  H323TransportAddress ta1(rtp.GetRemoteAddress(), rtp.GetRemoteDataPort());
  ta1.SetPDU(info.m_rtpAddress.m_sendAddress);

  transport.SetUpTransportPDU(info.m_rtcpAddress.m_recvAddress, rtp.GetLocalControlPort());
  H323TransportAddress ta2(rtp.GetRemoteAddress(), rtp.GetRemoteDataPort());
  ta2.SetPDU(info.m_rtcpAddress.m_sendAddress);
}

#if P_HAS_QOS
BOOL H323_RTP_UDP::WriteTransportCapPDU(H245_TransportCapability & cap, 
											const H323_RTPChannel & channel) const
{
 
    PQoS & qos = rtp.GetQOS();
	cap.IncludeOptionalField(H245_TransportCapability::e_qOSCapabilities);
	H245_ArrayOf_QOSCapability & QoSs = cap.m_qOSCapabilities;

	 H245_QOSCapability Cap = H245_QOSCapability();
	  Cap.IncludeOptionalField(H245_QOSCapability::e_localQoS);
	   PASN_Boolean & localqos = Cap.m_localQoS;
	   localqos.SetValue(TRUE);

	  Cap.IncludeOptionalField(H245_QOSCapability::e_dscpValue);
	   PASN_Integer & dscp = Cap.m_dscpValue;
	   dscp = qos.GetDSCP();

	if (PUDPSocket::SupportQoS(rtp.GetLocalAddress())) {		
	  Cap.IncludeOptionalField(H245_QOSCapability::e_rsvpParameters);
	  H245_RSVPParameters & rsvp = Cap.m_rsvpParameters; 

	  if (channel.GetDirection() == H323Channel::IsReceiver) {   /// If Reply don't have to send body
		  rtp.EnableGQoS(TRUE);
		  return TRUE;
	  }
	  rsvp.IncludeOptionalField(H245_RSVPParameters::e_qosMode); 
		H245_QOSMode & mode = rsvp.m_qosMode;
		  if (qos.GetServiceType() == SERVICETYPE_GUARANTEED) 
			 mode.SetTag(H245_QOSMode::e_guaranteedQOS); 
		  else 
			 mode.SetTag(H245_QOSMode::e_controlledLoad); 
		
	  rsvp.IncludeOptionalField(H245_RSVPParameters::e_tokenRate); 
		   rsvp.m_tokenRate = qos.GetTokenRate();
	  rsvp.IncludeOptionalField(H245_RSVPParameters::e_bucketSize);
		   rsvp.m_bucketSize = qos.GetTokenBucketSize();
	  rsvp.HasOptionalField(H245_RSVPParameters::e_peakRate);
		   rsvp.m_peakRate = qos.GetPeakBandwidth();
	}
	QoSs.SetSize(1);
	QoSs[0] = Cap;
	return TRUE;
}

void H323_RTP_UDP::ReadTransportCapPDU(const H245_TransportCapability & cap,
													H323_RTPChannel & channel)
{
	if (!cap.HasOptionalField(H245_TransportCapability::e_qOSCapabilities)) 
		return;	


	const H245_ArrayOf_QOSCapability QoSs = cap.m_qOSCapabilities;
	for (PINDEX i =0; i < QoSs.GetSize(); i++) {
	  PQoS & qos = rtp.GetQOS();
	  const H245_QOSCapability & QoS = QoSs[i];
//		if (QoS.HasOptionalField(H245_QOSCapability::e_localQoS)) {
//	       PASN_Boolean & localqos = QoS.m_localQoS;
//		}
		if (QoS.HasOptionalField(H245_QOSCapability::e_dscpValue)) {
	        const PASN_Integer & dscp = QoS.m_dscpValue;
	        qos.SetDSCP(dscp);
		}

		if (PUDPSocket::SupportQoS(rtp.GetLocalAddress())) {
			if (!QoS.HasOptionalField(H245_QOSCapability::e_rsvpParameters)) {
				PTRACE(4,"TRANS\tDisabling GQoS");
				rtp.EnableGQoS(FALSE);  
				return;
			}
		
			const H245_RSVPParameters & rsvp = QoS.m_rsvpParameters; 
			if (channel.GetDirection() != H323Channel::IsReceiver) {
				rtp.EnableGQoS(TRUE);
				return;
			}	  
			if (rsvp.HasOptionalField(H245_RSVPParameters::e_qosMode)) {
					const H245_QOSMode & mode = rsvp.m_qosMode;
					if (mode.GetTag() == H245_QOSMode::e_guaranteedQOS) {
						qos.SetWinServiceType(SERVICETYPE_GUARANTEED);
						qos.SetDSCP(PQoS::guaranteedDSCP);
					} else {
						qos.SetWinServiceType(SERVICETYPE_CONTROLLEDLOAD);
						qos.SetDSCP(PQoS::controlledLoadDSCP);
					}
			}
			if (rsvp.HasOptionalField(H245_RSVPParameters::e_tokenRate)) 
				qos.SetAvgBytesPerSec(rsvp.m_tokenRate);
			if (rsvp.HasOptionalField(H245_RSVPParameters::e_bucketSize))
				qos.SetMaxFrameBytes(rsvp.m_bucketSize);
			if (rsvp.HasOptionalField(H245_RSVPParameters::e_peakRate))
				qos.SetPeakBytesPerSec(rsvp.m_peakRate);	
		}
	}
}
#endif
/////////////////////////////////////////////////////////////////////////////
