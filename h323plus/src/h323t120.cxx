/*
 * h323t120.cxx
 *
 * H.323 T.120 logical channel establishment
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
 * $Log: h323t120.cxx,v $
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.12  2002/11/10 08:10:43  robertj
 * Moved constants for "well known" ports to better place (OPAL change).
 *
 * Revision 1.11  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.10  2002/07/02 10:02:32  robertj
 * Added H323TransportAddress::GetIpAddress() so don't have to provide port
 *   when you don't need it as in GetIpAndPort(),.
 *
 * Revision 1.9  2002/06/28 03:34:28  robertj
 * Fixed issues with address translation on gatekeeper RAS channel.
 *
 * Revision 1.8  2002/05/10 05:48:13  robertj
 * Added the max bit rate field to the data channel capability class.
 * Added session ID to the data logical channel class.
 *
 * Revision 1.7  2002/02/01 01:47:18  robertj
 * Some more fixes for T.120 channel establishment, more to do!
 *
 * Revision 1.6  2002/01/09 00:21:40  robertj
 * Changes to support outgoing H.245 RequstModeChange.
 *
 * Revision 1.5  2001/12/22 03:22:29  robertj
 * Added create protocol function to H323Connection.
 *
 * Revision 1.4  2001/09/12 07:48:05  robertj
 * Fixed various problems with tracing.
 *
 * Revision 1.3  2001/08/06 03:08:57  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.2  2001/07/18 02:27:24  robertj
 * Fixed GNU warning.
 *
 * Revision 1.1  2001/07/17 04:44:32  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h323t120.h"
#endif

#include "h323t120.h"

#include "h323ep.h"
#include "h245.h"
#include "t120proto.h"
#include "x224.h"


#define new PNEW

#define T120_MAX_BIT_RATE 825000


/////////////////////////////////////////////////////////////////////////////

H323_T120Capability::H323_T120Capability()
  : H323DataCapability(T120_MAX_BIT_RATE)
{
  dynamicPortCapability = TRUE;
}


PObject * H323_T120Capability::Clone() const
{
  return new H323_T120Capability(*this);
}


unsigned H323_T120Capability::GetSubType() const
{
  return H245_DataApplicationCapability_application::e_t120;
}


PString H323_T120Capability::GetFormatName() const
{
  return "T.120";
}


H323Channel * H323_T120Capability::CreateChannel(H323Connection & connection,
                                                 H323Channel::Directions direction,
                                                 unsigned sessionID,
                                const H245_H2250LogicalChannelParameters *) const
{
  return new H323_T120Channel(connection, *this, direction, sessionID);
}


BOOL H323_T120Capability::OnSendingPDU(H245_DataApplicationCapability & pdu) const
{
  pdu.m_application.SetTag(H245_DataApplicationCapability_application::e_t120);
  return OnSendingPDU((H245_DataProtocolCapability &)pdu.m_application);
}


BOOL H323_T120Capability::OnSendingPDU(H245_DataMode & pdu) const
{
  pdu.m_application.SetTag(H245_DataMode_application::e_t120);
  return OnSendingPDU((H245_DataProtocolCapability &)pdu.m_application);
}


BOOL H323_T120Capability::OnSendingPDU(H245_DataProtocolCapability & pdu) const
{
  pdu.SetTag(H245_DataProtocolCapability::e_separateLANStack);
  return TRUE;
}


BOOL H323_T120Capability::OnReceivedPDU(const H245_DataApplicationCapability & cap)
{
  if (cap.m_application.GetTag() != H245_DataApplicationCapability_application::e_t120)
    return FALSE;

  const H245_DataProtocolCapability & dataCap = cap.m_application;

  return dataCap.GetTag() == H245_DataProtocolCapability::e_separateLANStack;
}


/////////////////////////////////////////////////////////////////////////////

H323_T120Channel::H323_T120Channel(H323Connection & connection,
                                   const H323Capability & capability,
                                   Directions direction,
                                   unsigned id)
  : H323DataChannel(connection, capability, direction, id)
{
  t120handler = NULL;
  PTRACE(3, "H323T120\tCreated logical channel for T.120");
}


void H323_T120Channel::Receive()
{
  HandleChannel();
}


void H323_T120Channel::Transmit()
{
  HandleChannel();
}


void H323_T120Channel::HandleChannel()
{
  PTRACE(2, "H323T120\tThread started.");

  if (t120handler == NULL) {
    PTRACE(1, "H323T120\tNo protocol handler, aborting thread.");
  }
  else if (transport == NULL && listener == NULL) {
    PTRACE(1, "H323T120\tNo listener or transport, aborting thread.");
  }
  else if (listener != NULL) {
    if ((transport = listener->Accept(30000)) != NULL)  // 30 second wait for connect back
      t120handler->Answer(*transport);
    else {
      PTRACE(1, "H323T120\tAccept failed, aborting thread.");
    }
  }
  else if (transport->IsOpen())
    t120handler->Originate(*transport);
  else {
    PTRACE(1, "H323T120\tConnect failed, aborting thread.");
  }

  connection.CloseLogicalChannelNumber(number);

  PTRACE(2, "H323T120\tThread ended");
}


BOOL H323_T120Channel::OnSendingPDU(H245_OpenLogicalChannel & open) const
{
  if (!H323DataChannel::OnSendingPDU(open))
    return FALSE;

  if (!((H323_T120Channel*)this)->CreateListener()) {
    PTRACE(1, "H323T120\tCould not create listener");
    return FALSE;
  }

  PTRACE(3, "H323T120\tOnSendingPDU");

  open.IncludeOptionalField(H245_OpenLogicalChannel::e_separateStack);
  open.m_separateStack.IncludeOptionalField(H245_NetworkAccessParameters::e_distribution);
  open.m_separateStack.m_distribution.SetTag(H245_NetworkAccessParameters_distribution::e_unicast);
  open.m_separateStack.m_networkAddress.SetTag(H245_NetworkAccessParameters_networkAddress::e_localAreaAddress);
  H245_TransportAddress & address = open.m_separateStack.m_networkAddress;
  return listener->SetUpTransportPDU(address, connection.GetControlChannel());
}


void H323_T120Channel::OnSendOpenAck(const H245_OpenLogicalChannel & /*open*/,
                                     H245_OpenLogicalChannelAck & ack) const
{
  PTRACE(3, "H323T120\tOnSendOpenAck");

  if (listener != NULL || transport != NULL) {
    ack.IncludeOptionalField(H245_OpenLogicalChannelAck::e_separateStack);
    ack.m_separateStack.IncludeOptionalField(H245_NetworkAccessParameters::e_distribution);
    ack.m_separateStack.m_distribution.SetTag(H245_NetworkAccessParameters_distribution::e_unicast);
    ack.m_separateStack.m_networkAddress.SetTag(H245_NetworkAccessParameters_networkAddress::e_localAreaAddress);
    H245_TransportAddress & address = ack.m_separateStack.m_networkAddress;
    if (listener != NULL)
      listener->SetUpTransportPDU(address, connection.GetControlChannel());
    else
      transport->SetUpTransportPDU(address, H323Transport::UseLocalTSAP);
  }
}


BOOL H323_T120Channel::OnReceivedPDU(const H245_OpenLogicalChannel & open,
                                     unsigned & errorCode)
{
  number = H323ChannelNumber(open.m_forwardLogicalChannelNumber, TRUE);

  PTRACE(3, "H323T120\tOnReceivedPDU for channel: " << number);

  H323EndPoint & endpoint = connection.GetEndPoint();

  t120handler = connection.CreateT120ProtocolHandler();
  if (t120handler == NULL) {
    PTRACE(1, "H323T120\tCould not create protocol handler");
    errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotAvailable;
    return FALSE;
  }

  BOOL listen = connection.HadAnsweredCall();

  H323TransportAddress address;
  if (open.HasOptionalField(H245_OpenLogicalChannel::e_separateStack) &&
    open.m_separateStack.m_networkAddress.GetTag() == H245_NetworkAccessParameters_networkAddress::e_localAreaAddress) {
    address = (const H245_TransportAddress &)open.m_separateStack.m_networkAddress;
    if (open.m_separateStack.HasOptionalField(H245_NetworkAccessParameters::e_t120SetupProcedure))
      listen = open.m_separateStack.m_t120SetupProcedure.GetTag() ==
                            H245_NetworkAccessParameters_t120SetupProcedure::e_waitForCall;
  }

  if (listen) {
    if (!address)
      listener = address.CreateCompatibleListener(endpoint);
    else {
      // No address specified, assume same IP as the transport and use default port
      PIPSocket::Address ip;
      if (!connection.GetControlChannel().GetLocalAddress().GetIpAddress(ip)) {
        PTRACE(1, "H323T120\tOnly IPv4 supported");
        errorCode = H245_OpenLogicalChannelReject_cause::e_separateStackEstablishmentFailed;
        return FALSE;
      }
      listener = new H323ListenerTCP(endpoint, ip, OpalT120Protocol::DefaultTcpPort, FALSE);
    }

    if (!listener->Open()) {
      PTRACE(1, "H323T120\tCould not open listener");
      errorCode = H245_OpenLogicalChannelReject_cause::e_separateStackEstablishmentFailed;
      return FALSE;
    }

    PTRACE(2, "H323T120\tCreated listener on " << listener->GetTransportAddress());
  }
  else {
    if (address.IsEmpty()) {
      // No address specified, assume same IP as the transport and use default port
      PIPSocket::Address ip;
      if (!connection.GetControlChannel().GetRemoteAddress().GetIpAddress(ip)) {
        PTRACE(1, "H323T120\tOnly IPv4 supported");
        errorCode = H245_OpenLogicalChannelReject_cause::e_separateStackEstablishmentFailed;
        return FALSE;
      }
      address = H323TransportAddress(ip, OpalT120Protocol::DefaultTcpPort);
    }

    transport = address.CreateTransport(endpoint);
    if (transport == NULL) {
      PTRACE(1, "H323T120\tCould not create transport");
      errorCode = H245_OpenLogicalChannelReject_cause::e_separateStackEstablishmentFailed;
      return FALSE;
    }

    transport->SetReadTimeout(10000); // 10 second wait for connect
    if (!transport->ConnectTo(address)) {
      PTRACE(1, "H323T120\tCould not connect to remote address: " << address);
      errorCode = H245_OpenLogicalChannelReject_cause::e_separateStackEstablishmentFailed;
      return FALSE;
    }

    PTRACE(2, "H323T120\tCreated transport from "
           << transport->GetLocalAddress() << " to " << transport->GetRemoteAddress());
  }

  return TRUE;
}


BOOL H323_T120Channel::OnReceivedAckPDU(const H245_OpenLogicalChannelAck & /*ack*/)
{
  PTRACE(3, "H323T120\tOnReceivedAckPDU");

  t120handler = connection.CreateT120ProtocolHandler();
  if (t120handler == NULL) {
    PTRACE(1, "H323T120\tCould not create protocol handler");
    return FALSE;
  }

  return TRUE;
}

    
/////////////////////////////////////////////////////////////////////////////
