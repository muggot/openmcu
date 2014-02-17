/*
 * h323annexg.cxx
 *
 * Implementation of H.323 Annex G using H.501
 *
 * Open H323 Library
 *
 * Copyright (c) 2001 Equivalence Pty. Ltd.
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
 * $Log: h323annexg.cxx,v $
 * Revision 1.1  2007/08/06 20:51:06  shorne
 * First commit of h323plus
 *
 * Revision 1.12  2004/07/03 06:51:37  rjongbloed
 * Added PTRACE_PARAM() macro to fix warnings on parameters used in PTRACE
 *  macros only.
 *
 * Revision 1.11  2003/04/09 03:08:10  robertj
 * Fixed race condition in shutting down transactor (pure virtual call)
 *
 * Revision 1.10  2003/04/01 03:14:40  robertj
 * Fixed passing thru H.501 RIP packet to transactor to handle.
 *
 * Revision 1.9  2003/03/26 00:46:29  robertj
 * Had another go at making H323Transactor being able to be created
 *   without having a listener running.
 *
 * Revision 1.8  2003/03/21 05:27:34  robertj
 * Added setting of remote port in UDP transport constructor.
 * Added call backs for cached responses.
 *
 * Revision 1.7  2003/03/20 01:51:11  robertj
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
#pragma implementation "h323annexg.h"
#endif

#include <ptclib/random.h>

#include "h323annexg.h"
#include "h323ep.h"
#include "h323pdu.h"
#include "h501.h"


#define new PNEW


///////////////////////////////////////////////////////////////////////////////

H323_AnnexG::H323_AnnexG(H323EndPoint & ep, H323Transport * trans)
  : H323Transactor(ep, trans, DefaultUdpPort, DefaultUdpPort)
{
  Construct();
}

H323_AnnexG::H323_AnnexG(H323EndPoint & ep, const H323TransportAddress & addr)
  : H323Transactor(ep, addr, DefaultUdpPort, DefaultUdpPort)
{
  Construct();
}

void H323_AnnexG::Construct()
{
  lastRequest = NULL;
  requests.DisallowDeleteObjects();
}

H323_AnnexG::~H323_AnnexG()
{
  StopChannel();
}

void H323_AnnexG::PrintOn(ostream & strm) const
{
  strm << "H501@";
  H323Transactor::PrintOn(strm);
}


H323TransactionPDU * H323_AnnexG::CreateTransactionPDU() const
{
  return new H501PDU;
}


BOOL H323_AnnexG::HandleTransaction(const PASN_Object & rawPDU)
{
  const H501PDU & pdu = (const H501PDU &)rawPDU;

  switch (pdu.m_body.GetTag()) {
    case H501_MessageBody::e_serviceRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveServiceRequest(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_serviceConfirmation :
      return OnReceiveServiceConfirmation(pdu, pdu.m_body);

    case H501_MessageBody::e_serviceRejection :
      return OnReceiveServiceRejection(pdu, pdu.m_body);

    case H501_MessageBody::e_serviceRelease :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveServiceRelease(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_descriptorRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveDescriptorRequest(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_descriptorConfirmation :
      return OnReceiveDescriptorConfirmation(pdu, pdu.m_body);

    case H501_MessageBody::e_descriptorRejection :
      return OnReceiveDescriptorRejection(pdu, pdu.m_body);

    case H501_MessageBody::e_descriptorIDRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveDescriptorIDRequest(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_descriptorIDConfirmation :
      return OnReceiveDescriptorIDConfirmation(pdu, pdu.m_body);

    case H501_MessageBody::e_descriptorIDRejection :
      return OnReceiveDescriptorIDRejection(pdu, pdu.m_body);

    case H501_MessageBody::e_descriptorUpdate :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveDescriptorUpdate(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_descriptorUpdateAck :
      return OnReceiveDescriptorUpdateACK(pdu, pdu.m_body);

    case H501_MessageBody::e_accessRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveAccessRequest(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_accessConfirmation :
      return OnReceiveAccessConfirmation(pdu, pdu.m_body);

    case H501_MessageBody::e_accessRejection :
      return OnReceiveAccessRejection(pdu, pdu.m_body);

    case H501_MessageBody::e_requestInProgress :
      return OnReceiveRequestInProgress(pdu, pdu.m_body);

    case H501_MessageBody::e_nonStandardRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveNonStandardRequest(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_nonStandardConfirmation :
      return OnReceiveNonStandardConfirmation(pdu, pdu.m_body);

    case H501_MessageBody::e_nonStandardRejection :
      return OnReceiveNonStandardRejection(pdu, pdu.m_body);

    case H501_MessageBody::e_unknownMessageResponse :
      OnReceiveUnknownMessageResponse(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_usageRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveUsageRequest(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_usageConfirmation :
      return OnReceiveUsageConfirmation(pdu, pdu.m_body);

    case H501_MessageBody::e_usageIndication :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveUnknownMessageResponse(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_usageIndicationConfirmation :
      return OnReceiveUsageIndicationConfirmation(pdu, pdu.m_body);

    case H501_MessageBody::e_usageIndicationRejection :
      return OnReceiveUsageIndicationRejection(pdu, pdu.m_body);

    case H501_MessageBody::e_usageRejection :
      return OnReceiveUsageRejection(pdu, pdu.m_body);

    case H501_MessageBody::e_validationRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveValidationRequest(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_validationConfirmation :
      return OnReceiveValidationConfirmation(pdu, pdu.m_body);

    case H501_MessageBody::e_validationRejection :
      return OnReceiveValidationRejection(pdu, pdu.m_body);

    case H501_MessageBody::e_authenticationRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveAuthenticationRequest(pdu, pdu.m_body);
      break;

    case H501_MessageBody::e_authenticationConfirmation :
      return OnReceiveAuthenticationConfirmation(pdu, pdu.m_body);

    case H501_MessageBody::e_authenticationRejection :
      return OnReceiveAuthenticationRejection(pdu, pdu.m_body);

    default :
      OnReceiveUnknown(pdu);
  }

  return FALSE;
}


void H323_AnnexG::OnSendingPDU(PASN_Object & /*rawPDU*/)
{
}


BOOL H323_AnnexG::OnReceiveUnknown(const H501PDU &)
{
  H501PDU response;
  response.BuildUnknownMessageResponse(0);
  return response.Write(*transport);
}

BOOL H323_AnnexG::OnReceiveServiceRequest(const H501PDU & pdu, const H501_ServiceRequest & /*pduBody*/)
{
  PTRACE(3, "AnnexG\tOnReceiveServiceRequest - seq: " << pdu.m_common.m_sequenceNumber);
  H501PDU response;
  response.BuildServiceRejection(pdu.m_common.m_sequenceNumber, H501_ServiceRejectionReason::e_serviceUnavailable);
  return response.Write(*transport);
}

BOOL H323_AnnexG::OnReceiveServiceConfirmation(const H501PDU & pdu, const H501_ServiceConfirmation & /*pduBody*/)
{
  return CheckForResponse(H501_MessageBody::e_serviceRequest, pdu.m_common.m_sequenceNumber);
}

BOOL H323_AnnexG::OnReceiveServiceRejection(const H501PDU & pdu, const H501_ServiceRejection & pduBody)
{
  return CheckForResponse(H501_MessageBody::e_serviceRequest, pdu.m_common.m_sequenceNumber, &pduBody.m_reason);
}

BOOL H323_AnnexG::OnReceiveServiceRelease(const H501PDU & /*common*/, const H501_ServiceRelease & /*pdu*/)
{
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveDescriptorRequest(const H501PDU & PTRACE_PARAM(pdu), const H501_DescriptorRequest & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveDescriptorRequest - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveDescriptorConfirmation(const H501PDU & PTRACE_PARAM(pdu), const H501_DescriptorConfirmation & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveDescriptorConfirmation - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveDescriptorRejection(const H501PDU & PTRACE_PARAM(pdu), const H501_DescriptorRejection & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveDescriptorRejection - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveDescriptorIDRequest(const H501PDU & PTRACE_PARAM(pdu), const H501_DescriptorIDRequest & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveDescriptorIDRequest - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveDescriptorIDConfirmation(const H501PDU & PTRACE_PARAM(pdu), const H501_DescriptorIDConfirmation & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveDescriptorIDConfirmation - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveDescriptorIDRejection(const H501PDU & PTRACE_PARAM(pdu), const H501_DescriptorIDRejection & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveDescriptorIDRejection - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveDescriptorUpdate(const H501PDU & PTRACE_PARAM(pdu), const H501_DescriptorUpdate & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveDescriptorUpdate - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveDescriptorUpdateACK(const H501PDU & pdu, const H501_DescriptorUpdateAck & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveDescriptorUpdateACK - seq: " << pdu.m_common.m_sequenceNumber);
  return CheckForResponse(H501_MessageBody::e_descriptorUpdate, pdu.m_common.m_sequenceNumber);
}

BOOL H323_AnnexG::OnReceiveAccessRequest(const H501PDU & PTRACE_PARAM(pdu), const H501_AccessRequest & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveAccessRequest - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveAccessConfirmation(const H501PDU & pdu, const H501_AccessConfirmation & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveAccessConfirmation - seq: " << pdu.m_common.m_sequenceNumber);
  return CheckForResponse(H501_MessageBody::e_accessRequest, pdu.m_common.m_sequenceNumber);
}

BOOL H323_AnnexG::OnReceiveAccessRejection(const H501PDU & pdu, const H501_AccessRejection & pduBody)
{
  PTRACE(3, "AnnexG\tOnReceiveAccessRejection - seq: " << pdu.m_common.m_sequenceNumber);
  return CheckForResponse(H501_MessageBody::e_accessRequest, pdu.m_common.m_sequenceNumber, &pduBody.m_reason);
}

BOOL H323_AnnexG::OnReceiveRequestInProgress(const H501PDU & pdu, const H501_RequestInProgress & rip)
{
  return HandleRequestInProgress(pdu, rip.m_delay);
}

BOOL H323_AnnexG::OnReceiveNonStandardRequest(const H501PDU & PTRACE_PARAM(pdu), const H501_NonStandardRequest & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveNonStandardRequest - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveNonStandardConfirmation(const H501PDU & PTRACE_PARAM(pdu), const H501_NonStandardConfirmation & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveNonStandardConfirmation - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveNonStandardRejection(const H501PDU & PTRACE_PARAM(pdu), const H501_NonStandardRejection & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveNonStandardRejection - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveUnknownMessageResponse(const H501PDU & PTRACE_PARAM(pdu), const H501_UnknownMessageResponse & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveUnknownMessageResponse - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveUsageRequest(const H501PDU & PTRACE_PARAM(pdu), const H501_UsageRequest & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveUsageRequest - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveUsageConfirmation(const H501PDU & PTRACE_PARAM(pdu), const H501_UsageConfirmation & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveUsageConfirmation - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveUsageIndicationConfirmation(const H501PDU & PTRACE_PARAM(pdu), const H501_UsageIndicationConfirmation & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveUsageIndicationConfirmation - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveUsageIndicationRejection(const H501PDU & PTRACE_PARAM(pdu), const H501_UsageIndicationRejection & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveUsageIndicationRejection - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveUsageRejection(const H501PDU & PTRACE_PARAM(pdu), const H501_UsageRejection & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveUsageRejection - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveValidationRequest(const H501PDU & PTRACE_PARAM(pdu), const H501_ValidationRequest & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveValidationRequest - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveValidationConfirmation(const H501PDU & PTRACE_PARAM(pdu), const H501_ValidationConfirmation & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveValidationConfirmation - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveValidationRejection(const H501PDU & PTRACE_PARAM(pdu), const H501_ValidationRejection & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveValidationRejection - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveAuthenticationRequest(const H501PDU & PTRACE_PARAM(pdu), const H501_AuthenticationRequest & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveAuthenticationRequest - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveAuthenticationConfirmation(const H501PDU & PTRACE_PARAM(pdu), const H501_AuthenticationConfirmation & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveAuthenticationConfirmation - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}

BOOL H323_AnnexG::OnReceiveAuthenticationRejection(const H501PDU & PTRACE_PARAM(pdu), const H501_AuthenticationRejection & /*pdu*/)
{
  PTRACE(3, "AnnexG\tOnReceiveAuthenticationRejection - seq: " << pdu.m_common.m_sequenceNumber);
  return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
