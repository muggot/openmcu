/*
 * h501pdu.cxx
 *
 * H.501 PDU definitions
 *
 * Open H323 Library
 *
 * Copyright (c) 2003 Equivalence Pty. Ltd.
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
 * $Log: h501pdu.cxx,v $
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.11  2003/04/10 01:00:12  craigs
 * Changed to allow reply address to be a list
 *
 * Revision 1.10  2003/03/28 05:34:06  robertj
 * Added implementation for GetRequestInProgressDelay
 *
 * Revision 1.9  2003/03/28 05:08:39  robertj
 * Added delay parameter to BuildRequestInProgress
 *
 * Revision 1.8  2003/03/25 04:56:21  robertj
 * Fixed issues to do with multiple inheritence in transaction reply cache.
 *
 * Revision 1.7  2003/03/20 01:51:12  robertj
 * More abstraction of H.225 RAS and H.501 protocols transaction handling.
 *
 * Revision 1.6  2003/03/18 13:57:53  craigs
 * More H.501 implementation
 *
 * Revision 1.5  2003/03/17 13:19:31  craigs
 * More H501 implementation
 *
 * Revision 1.4  2003/03/14 06:01:16  craigs
 * More updates
 *
 * Revision 1.3  2003/03/01 00:22:10  craigs
 * New PeerElement implementation
 *
 * Revision 1.2  2003/02/25 06:48:19  robertj
 * More work on PDU transaction abstraction.
 *
 * Revision 1.1  2003/02/21 05:27:06  craigs
 * Initial version
 *
 */


#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h501pdu.h"
#endif

#include "h501.h"
#include "h501pdu.h"
#include "h323pdu.h"



H501PDU::H501PDU()
{
}


PObject * H501PDU::Clone() const
{
  return new H501PDU(*this);
}


const PASN_Object & H501PDU::GetPDU() const
{
  return *this;
}


const PASN_Choice & H501PDU::GetChoice() const
{
  return m_body;
}


PASN_Object & H501PDU::GetPDU()
{
  return *this;
}


PASN_Choice & H501PDU::GetChoice()
{
  return m_body;
}


unsigned H501PDU::GetSequenceNumber() const
{
  return m_common.m_sequenceNumber;
}


unsigned H501PDU::GetRequestInProgressDelay() const
{
  if (m_body.GetTag() != H501_MessageBody::e_requestInProgress)
    return 0;

  const H501_RequestInProgress & pdu_body = m_body;
  return pdu_body.m_delay;
}


#if PTRACING
const char * H501PDU::GetProtocolName() const
{
  return "H501";
}
#endif


H323TransactionPDU * H501PDU::ClonePDU() const
{
  return new H501PDU(*this);
}


void H501PDU::DeletePDU()
{
  delete this;
}


void H501PDU::BuildPDU(unsigned tag, unsigned seqnum)
{
  m_body.SetTag(tag);
  m_common.m_sequenceNumber = seqnum;
  m_common.m_hopCount       = 10;
  m_common.m_annexGversion.SetValue("0.0.8.2250.1.7.0.2"); //  {itu-t(0) recommendation(0) h(8) h-225-0(2250) annex(1) g(7) version(0) 2}
  m_common.m_version.SetValue("0.0.8.501.0.1");            // {itu-t(0) recommendation(0) h(8) 501 version(0) 1} 
}


void H501PDU::BuildRequest(unsigned tag, unsigned seqnum, const H323TransportAddressArray & replyAddr)
{
  BuildPDU(tag, seqnum);
  m_common.IncludeOptionalField(H501_MessageCommonInfo::e_replyAddress);
  PINDEX i;
  m_common.m_replyAddress.SetSize(replyAddr.GetSize());
  for (i = 0; i < replyAddr.GetSize(); i++)
    replyAddr[i].SetPDU(m_common.m_replyAddress[i]);
}


H501_ServiceRequest & H501PDU::BuildServiceRequest(unsigned seqnum, const H323TransportAddressArray & reply)
{
  BuildRequest(H501_MessageBody::e_serviceRequest, seqnum, reply);
  H501_ServiceRequest & pdu_body = m_body;
  return pdu_body;
}


H501_ServiceConfirmation & H501PDU::BuildServiceConfirmation(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_serviceConfirmation, seqnum);
  H501_ServiceConfirmation & pdu_body = m_body;
  return pdu_body;
}


H501_ServiceRejection & H501PDU::BuildServiceRejection(unsigned seqnum, unsigned reason)
{
  BuildPDU(H501_MessageBody::e_serviceRejection, seqnum);
  H501_ServiceRejection & pdu_body = m_body;
  pdu_body.m_reason.SetTag(reason);
  return pdu_body;
}


H501_ServiceRelease & H501PDU::BuildServiceRelease(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_serviceRelease, seqnum);
  H501_ServiceRelease & pdu_body = m_body;
  return pdu_body;
}


H501_DescriptorRequest & H501PDU::BuildDescriptorRequest(unsigned seqnum, const H323TransportAddressArray & reply)
{
  BuildRequest(H501_MessageBody::e_descriptorRequest, seqnum, reply);
  H501_DescriptorRequest & pdu_body = m_body;
  return pdu_body;
}


H501_DescriptorConfirmation & H501PDU::BuildDescriptorConfirmation(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_descriptorConfirmation, seqnum);
  H501_DescriptorConfirmation & pdu_body = m_body;
  return pdu_body;
}


H501_DescriptorRejection & H501PDU::BuildDescriptorRejection(unsigned seqnum, unsigned reason)
{
  BuildPDU(H501_MessageBody::e_descriptorRejection, seqnum);
  H501_DescriptorRejection & pdu_body = m_body;
  pdu_body.m_reason.SetTag(reason);
  return pdu_body;
}


H501_DescriptorIDRequest & H501PDU::BuildDescriptorIDRequest(unsigned seqnum, const H323TransportAddressArray & reply)
{
  BuildRequest(H501_MessageBody::e_descriptorIDRequest, seqnum, reply);
  H501_DescriptorIDRequest & pdu_body = m_body;
  return pdu_body;
}


H501_DescriptorIDConfirmation & H501PDU::BuildDescriptorIDConfirmation(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_descriptorIDConfirmation, seqnum);
  H501_DescriptorIDConfirmation & pdu_body = m_body;
  return pdu_body;
}


H501_DescriptorIDRejection & H501PDU::BuildDescriptorIDRejection(unsigned seqnum, unsigned reason)
{
  BuildPDU(H501_MessageBody::e_descriptorIDRejection, seqnum);
  H501_DescriptorIDRejection & pdu_body = m_body;
  pdu_body.m_reason.SetTag(reason);
  return pdu_body;
}


H501_DescriptorUpdate & H501PDU::BuildDescriptorUpdate(unsigned seqnum, const H323TransportAddressArray & reply)
{
  BuildRequest(H501_MessageBody::e_descriptorUpdate, seqnum, reply);
  m_common.m_sequenceNumber = seqnum;
  H501_DescriptorUpdate & pdu_body = m_body;
  return pdu_body;
}


H501_DescriptorUpdateAck & H501PDU::BuildDescriptorUpdateAck(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_descriptorUpdateAck, seqnum);
  H501_DescriptorUpdateAck & pdu_body = m_body;
  return pdu_body;
}


H501_AccessRequest & H501PDU::BuildAccessRequest(unsigned seqnum, const H323TransportAddressArray & reply)
{
  BuildRequest(H501_MessageBody::e_accessRequest, seqnum, reply);
  H501_AccessRequest & pdu_body = m_body;
  return pdu_body;
}


H501_AccessConfirmation & H501PDU::BuildAccessConfirmation(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_accessConfirmation, seqnum);
  H501_AccessConfirmation & pdu_body = m_body;
  return pdu_body;
}


H501_AccessRejection & H501PDU::BuildAccessRejection(unsigned seqnum, int reason)
{
  BuildPDU(H501_MessageBody::e_accessRejection, seqnum);
  H501_AccessRejection & pdu_body = m_body;
  pdu_body.m_reason.SetTag(reason);
  return pdu_body;
}


H501_RequestInProgress & H501PDU::BuildRequestInProgress(unsigned seqnum, unsigned delay)
{
  BuildPDU(H501_MessageBody::e_requestInProgress, seqnum);
  H501_RequestInProgress & pdu_body = m_body;
  pdu_body.m_delay = delay;
  return pdu_body;
}


H501_NonStandardRequest & H501PDU::BuildNonStandardRequest(unsigned seqnum, const H323TransportAddressArray & reply)
{
  BuildRequest(H501_MessageBody::e_nonStandardRequest, seqnum, reply);
  m_common.m_sequenceNumber = seqnum;
  H501_NonStandardRequest & pdu_body = m_body;
  return pdu_body;
}


H501_NonStandardConfirmation & H501PDU::BuildNonStandardConfirmation(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_nonStandardConfirmation, seqnum);
  H501_NonStandardConfirmation & pdu_body = m_body;
  return pdu_body;
}


H501_NonStandardRejection & H501PDU::BuildNonStandardRejection(unsigned seqnum, unsigned reason)
{
  BuildPDU(H501_MessageBody::e_nonStandardRejection, seqnum);
  H501_NonStandardRejection & pdu_body = m_body;
  pdu_body.m_reason.SetTag(reason);
  return pdu_body;
}


H501_UnknownMessageResponse & H501PDU::BuildUnknownMessageResponse(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_unknownMessageResponse, seqnum);
  H501_UnknownMessageResponse & pdu_body = m_body;
  return pdu_body;
}


H501_UsageRequest & H501PDU::BuildUsageRequest(unsigned seqnum, const H323TransportAddressArray & reply)
{
  BuildRequest(H501_MessageBody::e_usageRequest, seqnum, reply);
  m_common.m_sequenceNumber = seqnum;
  H501_UsageRequest & pdu_body = m_body;
  return pdu_body;
}


H501_UsageConfirmation & H501PDU::BuildUsageConfirmation(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_usageConfirmation, seqnum);
  H501_UsageConfirmation & pdu_body = m_body;
  return pdu_body;
}


H501_UsageIndicationConfirmation & H501PDU::BuildUsageIndicationConfirmation(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_usageIndicationConfirmation, seqnum);
  H501_UsageIndicationConfirmation & pdu_body = m_body;
  return pdu_body;
}


H501_UsageIndicationRejection & H501PDU::BuildUsageIndicationRejection(unsigned seqnum, unsigned reason)
{
  BuildPDU(H501_MessageBody::e_usageIndicationRejection, seqnum);
  H501_UsageIndicationRejection & pdu_body = m_body;
  pdu_body.m_reason.SetTag(reason);
  return pdu_body;
}


H501_UsageRejection & H501PDU::BuildUsageRejection(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_usageRejection, seqnum);
  H501_UsageRejection & pdu_body = m_body;
  return pdu_body;
}


H501_ValidationRequest & H501PDU::BuildValidationRequest(unsigned seqnum, const H323TransportAddressArray & reply)
{
  BuildRequest(H501_MessageBody::e_validationRequest, seqnum, reply);
  m_common.m_sequenceNumber = seqnum;
  H501_ValidationRequest & pdu_body = m_body;
  return pdu_body;
}


H501_ValidationConfirmation & H501PDU::BuildValidationConfirmation(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_validationConfirmation, seqnum);
  H501_ValidationConfirmation & pdu_body = m_body;
  return pdu_body;
}


H501_ValidationRejection & H501PDU::BuildValidationRejection(unsigned seqnum, unsigned reason)
{
  BuildPDU(H501_MessageBody::e_validationRejection, seqnum);
  H501_ValidationRejection & pdu_body = m_body;
  pdu_body.m_reason.SetTag(reason);
  return pdu_body;
}


H501_AuthenticationRequest & H501PDU::BuildAuthenticationRequest(unsigned seqnum, const H323TransportAddressArray & reply)
{
  BuildRequest(H501_MessageBody::e_authenticationRequest, seqnum, reply);
  m_common.m_sequenceNumber = seqnum;
  H501_AuthenticationRequest & pdu_body = m_body;
  return pdu_body;
}


H501_AuthenticationConfirmation & H501PDU::BuildAuthenticationConfirmation(unsigned seqnum)
{
  BuildPDU(H501_MessageBody::e_authenticationConfirmation, seqnum);
  H501_AuthenticationConfirmation & pdu_body = m_body;
  return pdu_body;
}


H501_AuthenticationRejection & H501PDU::BuildAuthenticationRejection(unsigned seqnum, unsigned reason)
{
  BuildPDU(H501_MessageBody::e_authenticationRejection, seqnum);
  H501_AuthenticationRejection & pdu_body = m_body;
  pdu_body.m_reason.SetTag(reason);
  return pdu_body;
}


/////////////////////////////////////////////////////////////////////////////
