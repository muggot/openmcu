/*
 * h323t38.cxx
 *
 * H.323 T.38 logical channel establishment
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
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
 * $Log: h323t38.cxx,v $
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.27  2004/08/24 14:23:11  csoutheren
 * Fixed problem with plugin codecs using capability compare functions
 *
 * Revision 1.26  2004/05/04 03:33:33  csoutheren
 * Added guards against comparing certain kinds of Capabilities
 *
 * Revision 1.25  2004/04/03 08:28:07  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.24  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.23  2002/07/02 10:02:32  robertj
 * Added H323TransportAddress::GetIpAddress() so don't have to provide port
 *   when you don't need it as in GetIpAndPort(),.
 *
 * Revision 1.22  2002/05/21 06:37:55  robertj
 * Fixed crash if CreateT38Handler returns NULL.
 *
 * Revision 1.21  2002/05/15 23:29:31  robertj
 * Backed out delete of t38 handler, causes race conditions.
 *
 * Revision 1.20  2002/05/15 01:31:23  robertj
 * Added missing delete of t38 handler, thanks thsuk@digitalsis.com.
 * Changed to allow the T.35 information to be adjusted so it will work for
 *    various vendors version of the non-standard capability.
 *
 * Revision 1.19  2002/05/14 08:41:31  robertj
 * Fixed incorrect class type check for finding reverse T.38 channel and also
 *   fixed possible race condition on channel close, thanks Vyacheslav Frolov
 *
 * Revision 1.18  2002/05/10 05:50:02  robertj
 * Added the max bit rate field to the data channel capability class.
 * Added session ID to the data logical channel class.
 * Added capability for old pre-v3 non-standard T.38.
 *
 * Revision 1.17  2002/04/17 00:49:04  robertj
 * Fixed problems with T.38 start up, thanks Vyacheslav Frolov.
 *
 * Revision 1.16  2002/02/13 07:41:45  robertj
 * Fixed missing setting of transport on second H323Channel, thanks Vyacheslav Frolov
 *
 * Revision 1.15  2002/02/09 04:39:05  robertj
 * Changes to allow T.38 logical channels to use single transport which is
 *   now owned by the OpalT38Protocol object instead of H323Channel.
 *
 * Revision 1.14  2002/01/10 04:08:42  robertj
 * Fixed missing bit rate in mode request PDU.
 *
 * Revision 1.13  2002/01/09 00:21:40  robertj
 * Changes to support outgoing H.245 RequstModeChange.
 *
 * Revision 1.12  2002/01/01 23:27:50  craigs
 * Added CleanupOnTermination functions
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.11  2001/12/22 03:21:58  robertj
 * Added create protocol function to H323Connection.
 *
 * Revision 1.10  2001/12/22 01:55:06  robertj
 * Removed vast quatities of redundent code that is done by ancestor class.
 *
 * Revision 1.9  2001/12/19 09:15:43  craigs
 * Added changes from Vyacheslav Frolov
 *
 * Revision 1.8  2001/12/14 08:36:36  robertj
 * More implementation of T.38, thanks Adam Lazur
 *
 * Revision 1.7  2001/11/20 03:04:30  robertj
 * Added ability to reuse t38 channels with same session ID.
 *
 * Revision 1.6  2001/11/09 05:39:54  craigs
 * Added initial T.38 support thanks to Adam Lazur
 *
 * Revision 1.5  2001/09/12 07:48:05  robertj
 * Fixed various problems with tracing.
 *
 * Revision 1.4  2001/08/06 03:08:57  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.3  2001/07/24 02:26:24  robertj
 * Added UDP, dual TCP and single TCP modes to T.38 capability.
 *
 * Revision 1.2  2001/07/19 10:48:20  robertj
 * Fixed bandwidth
 *
 * Revision 1.1  2001/07/17 04:44:32  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h323t38.h"
#endif

#include "h323t38.h"

#include "h323con.h"
#include "h245.h"
#include "t38proto.h"
#include "t38.h"
#include "h323ep.h"


#define new PNEW

#define FAX_BIT_RATE 144  //14.4k



/////////////////////////////////////////////////////////////////////////////

H323_T38Capability::H323_T38Capability(TransportMode m)
  : H323DataCapability(FAX_BIT_RATE),
    mode(m)
{
}


PObject::Comparison H323_T38Capability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323_T38Capability))
    return LessThan;

  Comparison result = H323DataCapability::Compare(obj);
  if (result != EqualTo)
    return result;

  PAssert(PIsDescendant(&obj, H323_T38Capability), PInvalidCast);
  const H323_T38Capability & other = (const H323_T38Capability &)obj;

  if (mode < other.mode)
    return LessThan;

  if (mode > other.mode)
    return GreaterThan;

  return EqualTo;
}


PObject * H323_T38Capability::Clone() const
{
  return new H323_T38Capability(*this);
}


unsigned H323_T38Capability::GetSubType() const
{
  return H245_DataApplicationCapability_application::e_t38fax;
}


PString H323_T38Capability::GetFormatName() const
{
  static const char * const modes[NumTransportModes] = {
    "UDP", "TCP2", "TCP"
  };
  return PString("T.38-") + modes[mode];
}


H323Channel * H323_T38Capability::CreateChannel(H323Connection & connection,
                                                H323Channel::Directions direction,
                                                unsigned int sessionID,
                             const H245_H2250LogicalChannelParameters * /*params*/) const
{
  PTRACE(1, "H323T38\tCreateChannel, sessionID=" << sessionID << " direction=" << direction);

  return new H323_T38Channel(connection, *this, direction, sessionID, mode);
}


BOOL H323_T38Capability::OnSendingPDU(H245_DataApplicationCapability & pdu) const
{
  PTRACE(3, "H323T38\tOnSendingPDU for capability");

  pdu.m_maxBitRate = FAX_BIT_RATE;
  pdu.m_application.SetTag(H245_DataApplicationCapability_application::e_t38fax);
  H245_DataApplicationCapability_application_t38fax & fax = pdu.m_application;
  return OnSendingPDU(fax.m_t38FaxProtocol, fax.m_t38FaxProfile);
}


BOOL H323_T38Capability::OnSendingPDU(H245_DataMode & pdu) const
{
  pdu.m_bitRate = FAX_BIT_RATE;
  pdu.m_application.SetTag(H245_DataMode_application::e_t38fax);
  H245_DataMode_application_t38fax & fax = pdu.m_application;
  return OnSendingPDU(fax.m_t38FaxProtocol, fax.m_t38FaxProfile);
}


BOOL H323_T38Capability::OnSendingPDU(H245_DataProtocolCapability & proto,
                                      H245_T38FaxProfile & profile) const
{
  if (mode == e_UDP) {
    proto.SetTag(H245_DataProtocolCapability::e_udp);
    profile.m_t38FaxRateManagement.SetTag(H245_T38FaxRateManagement::e_transferredTCF); // recommended for UDP

    profile.IncludeOptionalField(H245_T38FaxProfile::e_t38FaxUdpOptions);
    profile.m_t38FaxUdpOptions.IncludeOptionalField(H245_T38FaxUdpOptions::e_t38FaxMaxBuffer);
    profile.m_t38FaxUdpOptions.m_t38FaxMaxBuffer = 200;
    profile.m_t38FaxUdpOptions.IncludeOptionalField(H245_T38FaxUdpOptions::e_t38FaxMaxDatagram);
    profile.m_t38FaxUdpOptions.m_t38FaxMaxDatagram = 72;
    profile.m_t38FaxUdpOptions.m_t38FaxUdpEC.SetTag(H245_T38FaxUdpOptions_t38FaxUdpEC::e_t38UDPRedundancy);
  }
  else {
    proto.SetTag(H245_DataProtocolCapability::e_tcp);
    profile.m_t38FaxRateManagement.SetTag(H245_T38FaxRateManagement::e_localTCF); // recommended for TCP

    profile.IncludeOptionalField(H245_T38FaxProfile::e_t38FaxTcpOptions);
    profile.m_t38FaxTcpOptions.m_t38TCPBidirectionalMode = mode == e_SingleTCP;
  }

  return TRUE;
}


BOOL H323_T38Capability::OnReceivedPDU(const H245_DataApplicationCapability & cap)
{
  PTRACE(3, "H323T38\tOnRecievedPDU for capability");

  if (cap.m_application.GetTag() != H245_DataApplicationCapability_application::e_t38fax)
    return FALSE;

  const H245_DataApplicationCapability_application_t38fax & fax = cap.m_application;
  const H245_DataProtocolCapability & proto = fax.m_t38FaxProtocol;

  if (proto.GetTag() == H245_DataProtocolCapability::e_udp)
    mode = e_UDP;
  else {
    const H245_T38FaxProfile & profile = fax.m_t38FaxProfile;
    if (profile.m_t38FaxTcpOptions.m_t38TCPBidirectionalMode)
      mode = e_SingleTCP;
    else
      mode = e_DualTCP;
  }

  return TRUE;
}


//////////////////////////////////////////////////////////////

static const char T38NonStandardCapabilityName[] = "T38FaxUDP";

H323_T38NonStandardCapability::H323_T38NonStandardCapability(BYTE country,
                                                             BYTE extension,
                                                             WORD manufacturer)
  : H323NonStandardDataCapability(FAX_BIT_RATE,
                                  country, extension, manufacturer,
                                  (const BYTE *)T38NonStandardCapabilityName,
                                  (PINDEX)sizeof(T38NonStandardCapabilityName)-1)
{
}


PObject * H323_T38NonStandardCapability::Clone() const
{
  return new H323_T38NonStandardCapability(*this);
}


class PString H323_T38NonStandardCapability::GetFormatName() const
{
  return T38NonStandardCapabilityName;
}


H323Channel * H323_T38NonStandardCapability::CreateChannel(H323Connection & connection,
                                                H323Channel::Directions direction,
                                                unsigned int sessionID,
                             const H245_H2250LogicalChannelParameters * /*params*/) const
{
  PTRACE(1, "H323T38\tCreateChannel, sessionID=" << sessionID << " direction=" << direction);

  return new H323_T38Channel(connection, *this, direction, sessionID, H323_T38Capability::e_UDP);
}


//////////////////////////////////////////////////////////////

H323_T38Channel::H323_T38Channel(H323Connection & connection,
                                 const H323Capability & capability,
                                 H323Channel::Directions dir,
                                 unsigned sessionID,
                                 H323_T38Capability::TransportMode mode)
  : H323DataChannel(connection, capability, dir, sessionID)
{
  PTRACE(3, "H323T38\tH323 channel created");

  // Transport will be owned by OpalT38Protocol
  autoDeleteTransport = FALSE;

  separateReverseChannel = mode != H323_T38Capability::e_SingleTCP;
  usesTCP = mode != H323_T38Capability::e_UDP;

  t38handler = NULL;

  H323Channel * chan = connection.FindChannel(sessionID, dir == H323Channel::IsTransmitter);
  if (chan != NULL) {
    if (PIsDescendant(chan, H323_T38Channel)) {
      PTRACE(3, "H323T38\tConnected to existing T.38 handler");
      t38handler = ((H323_T38Channel *)chan)->GetHandler();
    }
    else
      PTRACE(1, "H323T38\tCreateChannel, channel " << *chan << " is not H323_T38Channel");
  }

  if (t38handler == NULL) {
    PTRACE(3, "H323T38\tCreating new T.38 handler");
    t38handler = connection.CreateT38ProtocolHandler();
  }

  if (t38handler != NULL) {
    transport = t38handler->GetTransport();

    if (transport == NULL && !usesTCP && CreateTransport())
      t38handler->SetTransport(transport, TRUE);
  }
}


H323_T38Channel::~H323_T38Channel()
{
}


void H323_T38Channel::CleanUpOnTermination()
{
  if (terminating)
    return;

  PTRACE(3, "H323T38\tCleanUpOnTermination");
    
  if (t38handler != NULL) 
    t38handler->CleanUpOnTermination();

  H323DataChannel::CleanUpOnTermination();
}


BOOL H323_T38Channel::OnSendingPDU(H245_OpenLogicalChannel & open) const
{
  if (t38handler != NULL)
    return H323DataChannel::OnSendingPDU(open);

  PTRACE(1, "H323T38\tNo protocol handler, aborting OpenLogicalChannel.");
  return FALSE;
}


BOOL H323_T38Channel::OnReceivedPDU(const H245_OpenLogicalChannel & open,
                                    unsigned & errorCode)
{
  if (t38handler != NULL)
    return H323DataChannel::OnReceivedPDU(open, errorCode);

  errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
  PTRACE(1, "H323T38\tNo protocol handler, refusing OpenLogicalChannel.");
  return FALSE;
}


void H323_T38Channel::Receive()
{
  PTRACE(2, "H323T38\tReceive thread started.");

  if (t38handler != NULL) {
    if (listener != NULL) {
      transport = listener->Accept(30000);  // 30 second wait for connect back
      t38handler->SetTransport(transport);
    }

    if (transport != NULL)
      t38handler->Answer();
    else {
      PTRACE(1, "H323T38\tNo transport, aborting thread.");
    }
  }
  else {
    PTRACE(1, "H323T38\tNo protocol handler, aborting thread.");
  }

  if (!terminating)
    connection.CloseLogicalChannelNumber(number);

  PTRACE(2, "H323T38\tReceive thread ended");
}


void H323_T38Channel::Transmit()
{
  if (terminating)
    return;

  PTRACE(2, "H323T38\tTransmit thread starting");

  if (t38handler != NULL)
    t38handler->Originate();
  else {
    PTRACE(1, "H323T38\tTransmit no proto handler");
  }

  if (!terminating)
    connection.CloseLogicalChannelNumber(number);

  PTRACE(2, "H323T38\tTransmit thread terminating");
}


BOOL H323_T38Channel::CreateTransport()
{
  if (transport != NULL)
    return TRUE;

  if (usesTCP)
    return H323DataChannel::CreateTransport();

  PIPSocket::Address ip;
  if (!connection.GetControlChannel().GetLocalAddress().GetIpAddress(ip)) {
    PTRACE(2, "H323T38\tTrying to use UDP when base transport is not IP");
    PIPSocket::GetHostAddress(ip);
  }

  transport = new H323TransportUDP(connection.GetEndPoint(), ip);
  PTRACE(3, "H323T38\tCreated transport: " << *transport);
  return TRUE;
}


BOOL H323_T38Channel::CreateListener()
{
  if (listener != NULL)
    return TRUE;

  if (usesTCP) {
    return H323DataChannel::CreateListener();
    PTRACE(3, "H323T38\tCreated listener " << *listener);
  }

  return CreateTransport();
}


/////////////////////////////////////////////////////////////////////////////
