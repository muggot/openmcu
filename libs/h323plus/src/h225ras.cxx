/*
 * H225_RAS.cxx
 *
 * H.225 RAS protocol handler
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
 * Portions of this code were written with the assisance of funding from
 * iFace In, http://www.iface.com
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h225ras.cxx,v $
 * Revision 1.3  2008/02/04 00:15:54  shorne
 * BUGFIX: Features being sent on GRQ
 *
 * Revision 1.2  2007/11/10 23:07:50  willamowius
 * fix --disable-h460
 *
 * Revision 1.1  2007/08/06 20:51:05  shorne
 * First commit of h323plus
 *
 * Revision 1.60.2.3  2007/07/23 08:17:07  shorne
 * Expanded H.460 Support
 *
 * Revision 1.60.2.2  2007/01/30 17:28:12  shorne
 * Fix for compiling on linux
 *
 * Revision 1.60.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.60  2006/06/23 20:19:39  shorne
 * More H460 support
 *
 * Revision 1.59  2006/05/30 11:14:56  hfriederich
 * Switch from DISABLE_H460 to H323_H460
 *
 * Revision 1.58  2005/01/03 14:03:21  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.57  2005/01/03 06:25:55  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.56  2004/09/07 22:50:55  rjongbloed
 * Changed usage of template function as MSVC6 will not compile it.
 *
 * Revision 1.55  2004/09/03 01:06:10  csoutheren
 * Added initial hooks for H.460 GEF
 * Thanks to Simon Horne and ISVO (Asia) Pte Ltd. for this contribution
 *
 * Revision 1.54  2003/04/09 03:08:10  robertj
 * Fixed race condition in shutting down transactor (pure virtual call)
 *
 * Revision 1.53  2003/04/01 01:05:17  robertj
 * Split service control handlers from H.225 RAS header.
 *
 * Revision 1.52  2003/03/26 00:46:28  robertj
 * Had another go at making H323Transactor being able to be created
 *   without having a listener running.
 *
 * Revision 1.51  2003/03/21 05:25:47  robertj
 * Added setting of remote port in UDP transport constructor.
 *
 * Revision 1.50  2003/03/20 01:51:11  robertj
 * More abstraction of H.225 RAS and H.501 protocols transaction handling.
 *
 * Revision 1.49  2003/03/01 00:21:01  craigs
 * Fixed spelling mistake!
 *
 * Revision 1.48  2003/02/21 05:25:45  craigs
 * Abstracted out underlying transports for use with peerelements
 *
 * Revision 1.47  2003/02/10 04:18:55  robertj
 * Made RAS read loop more robust.
 *
 * Revision 1.46  2003/02/01 13:31:21  robertj
 * Changes to support CAT authentication in RAS.
 *
 * Revision 1.45  2002/12/18 06:24:52  robertj
 * Improved logging around the RAS poll function, on timeouts etc
 *
 * Revision 1.44  2002/12/13 02:52:19  robertj
 * Fixed failure to release mutex on call confirm/reject.
 *
 * Revision 1.43  2002/11/28 23:39:36  robertj
 * Fixed race condition for if RAS reply arrives very VERY quickly after
 *   sending packet. Introduced in previous change.
 *
 * Revision 1.42  2002/11/28 04:41:48  robertj
 * Added support for RAS ServiceControlIndication command.
 *
 * Revision 1.41  2002/11/28 02:13:28  robertj
 * Fixed copy and paste errors.
 *
 * Revision 1.40  2002/11/28 02:10:26  robertj
 * Changed order of function so OnSendXXX(pdu) can change the security
 *   credentials in descendant classes.
 *
 * Revision 1.39  2002/11/27 06:54:56  robertj
 * Added Service Control Session management as per Annex K/H.323 via RAS
 *   only at this stage.
 * Added H.248 ASN and very primitive infrastructure for linking into the
 *   Service Control Session management system.
 * Added basic infrastructure for Annex K/H.323 HTTP transport system.
 * Added Call Credit Service Control to display account balances.
 *
 * Revision 1.38  2002/11/21 22:26:20  robertj
 * Changed promiscuous mode to be three way. Fixes race condition in gkserver
 *   which can cause crashes or more PDUs to be sent to the wrong place.
 *
 * Revision 1.37  2002/11/21 07:21:49  robertj
 * Improvements to alternate gatekeeper client code, thanks Kevin Bouchard
 *
 * Revision 1.36  2002/11/11 08:13:40  robertj
 * Fixed GNU warning
 *
 * Revision 1.35  2002/11/11 07:20:12  robertj
 * Minor clean up of API for doing RAS requests suing authentication.
 *
 * Revision 1.34  2002/11/10 08:10:43  robertj
 * Moved constants for "well known" ports to better place (OPAL change).
 *
 * Revision 1.33  2002/10/17 02:10:55  robertj
 * Backed out previous change for including PDU tag, doesn't work!
 *
 * Revision 1.32  2002/10/16 03:46:05  robertj
 * Added PDU tag to cache look up key.
 *
 * Revision 1.31  2002/10/09 05:38:50  robertj
 * Fixed correct mutexing of response cache buffer.
 * Fixed correct setting of remote address when cached response transmitted.
 *
 * Revision 1.30  2002/09/19 09:16:01  robertj
 * Fixed problem with making (and assuring with multi-threading) IRQ and DRQ
 *   requests are sent to the correct endpoint address, thanks Martijn Roest.
 *
 * Revision 1.29  2002/08/29 06:58:37  robertj
 * Fixed (again) cached response age timeout adjusted to RIP time.
 *
 * Revision 1.28  2002/08/12 06:29:42  robertj
 * Fixed problem with cached responses being aged before the RIP time which
 *   made retries by client appear as "new" requests when they were not.
 *
 * Revision 1.27  2002/08/12 05:35:48  robertj
 * Changes to the RAS subsystem to support ability to make requests to client
 *   from gkserver without causing bottlenecks and race conditions.
 *
 * Revision 1.26  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.25  2002/08/05 05:17:41  robertj
 * Fairly major modifications to support different authentication credentials
 *   in ARQ to the logged in ones on RRQ. For both client and server.
 * Various other H.235 authentication bugs and anomalies fixed on the way.
 *
 * Revision 1.24  2002/07/29 11:36:08  robertj
 * Fixed race condition if RIP is followed very quickly by actual response.
 *
 * Revision 1.23  2002/07/16 04:18:38  robertj
 * Fixed incorrect check for GRQ in reject processing, thanks Thien Nguyen
 *
 * Revision 1.22  2002/06/28 03:34:28  robertj
 * Fixed issues with address translation on gatekeeper RAS channel.
 *
 * Revision 1.21  2002/06/24 00:11:21  robertj
 * Clarified error message during GRQ authentication.
 *
 * Revision 1.20  2002/06/12 03:50:25  robertj
 * Added PrintOn function for trace output of RAS channel.
 *
 * Revision 1.19  2002/05/29 00:03:19  robertj
 * Fixed unsolicited IRR support in gk client and server,
 *   including support for IACK and INAK.
 *
 * Revision 1.18  2002/05/17 03:41:00  robertj
 * Fixed problems with H.235 authentication on RAS for server and client.
 *
 * Revision 1.17  2002/05/03 09:18:49  robertj
 * Added automatic retransmission of RAS responses to retried requests.
 *
 * Revision 1.16  2002/03/10 19:34:13  robertj
 * Added random starting point for sequence numbers, thanks Chris Purvis
 *
 * Revision 1.15  2002/01/29 02:38:31  robertj
 * Fixed nasty race condition when getting RIP, end up with wrong timeout.
 * Improved tracing (included sequence numbers)
 *
 * Revision 1.14  2002/01/24 01:02:04  robertj
 * Removed trace when authenticator not used, implied error when wasn't one.
 *
 * Revision 1.13  2001/10/09 12:03:30  robertj
 * Fixed uninitialised variable for H.235 authentication checking.
 *
 * Revision 1.12  2001/10/09 08:04:59  robertj
 * Fixed unregistration so still unregisters if gk goes offline, thanks Chris Purvis
 *
 * Revision 1.11  2001/09/18 10:36:57  robertj
 * Allowed multiple overlapping requests in RAS channel.
 *
 * Revision 1.10  2001/09/12 07:48:05  robertj
 * Fixed various problems with tracing.
 *
 * Revision 1.9  2001/09/12 03:12:38  robertj
 * Added ability to disable the checking of RAS responses against
 *   security authenticators.
 * Fixed bug in having multiple authentications if have a retry.
 *
 * Revision 1.8  2001/08/10 11:03:52  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 * Revision 1.7  2001/08/06 07:44:55  robertj
 * Fixed problems with building without SSL
 *
 * Revision 1.6  2001/08/06 03:18:38  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 * Improved access to H.235 secure RAS functionality.
 * Changes to H.323 secure RAS contexts to help use with gk server.
 *
 * Revision 1.5  2001/08/03 05:56:04  robertj
 * Fixed RAS read of UDP when get ICMP error for host unreachabe.
 *
 * Revision 1.4  2001/06/25 01:06:40  robertj
 * Fixed resolution of RAS timeout so not rounded down to second.
 *
 * Revision 1.3  2001/06/22 00:21:10  robertj
 * Fixed bug in H.225 RAS protocol with 16 versus 32 bit sequence numbers.
 *
 * Revision 1.2  2001/06/18 07:44:21  craigs
 * Made to compile with h225ras.cxx under Linux
 *
 * Revision 1.1  2001/06/18 06:23:50  robertj
 * Split raw H.225 RAS protocol out of gatekeeper client class.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h225ras.h"
#endif

#include "h323.h"

#include "h225ras.h"

#include "h323ep.h"
#include "h323pdu.h"
#include "h235auth.h"

#ifdef H323_H460
#include "h460/h460.h"
#endif

#define new PNEW


/////////////////////////////////////////////////////////////////////////////

H225_RAS::H225_RAS(H323EndPoint & ep, H323Transport * trans)
  : H323Transactor(ep, trans, DefaultRasUdpPort, DefaultRasUdpPort)
{
}


H225_RAS::~H225_RAS()
{
  StopChannel();
}


void H225_RAS::PrintOn(ostream & strm) const
{
  if (gatekeeperIdentifier.IsEmpty())
    strm << "H225-RAS@";
  else
    strm << gatekeeperIdentifier << '@';
  H323Transactor::PrintOn(strm);
}


H323TransactionPDU * H225_RAS::CreateTransactionPDU() const
{
  return new H323RasPDU;
}


BOOL H225_RAS::HandleTransaction(const PASN_Object & rawPDU)
{
  const H323RasPDU & pdu = (const H323RasPDU &)rawPDU;

  switch (pdu.GetTag()) {
    case H225_RasMessage::e_gatekeeperRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveGatekeeperRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_gatekeeperConfirm :
      return OnReceiveGatekeeperConfirm(pdu, pdu);

    case H225_RasMessage::e_gatekeeperReject :
      return OnReceiveGatekeeperReject(pdu, pdu);

    case H225_RasMessage::e_registrationRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveRegistrationRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_registrationConfirm :
      return OnReceiveRegistrationConfirm(pdu, pdu);

    case H225_RasMessage::e_registrationReject :
      return OnReceiveRegistrationReject(pdu, pdu);

    case H225_RasMessage::e_unregistrationRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveUnregistrationRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_unregistrationConfirm :
      return OnReceiveUnregistrationConfirm(pdu, pdu);

    case H225_RasMessage::e_unregistrationReject :
      return OnReceiveUnregistrationReject(pdu, pdu);

    case H225_RasMessage::e_admissionRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveAdmissionRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_admissionConfirm :
      return OnReceiveAdmissionConfirm(pdu, pdu);

    case H225_RasMessage::e_admissionReject :
      return OnReceiveAdmissionReject(pdu, pdu);

    case H225_RasMessage::e_bandwidthRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveBandwidthRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_bandwidthConfirm :
      return OnReceiveBandwidthConfirm(pdu, pdu);

    case H225_RasMessage::e_bandwidthReject :
      return OnReceiveBandwidthReject(pdu, pdu);

    case H225_RasMessage::e_disengageRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveDisengageRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_disengageConfirm :
      return OnReceiveDisengageConfirm(pdu, pdu);

    case H225_RasMessage::e_disengageReject :
      return OnReceiveDisengageReject(pdu, pdu);

    case H225_RasMessage::e_locationRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveLocationRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_locationConfirm :
      return OnReceiveLocationConfirm(pdu, pdu);

    case H225_RasMessage::e_locationReject :
      return OnReceiveLocationReject(pdu, pdu);

    case H225_RasMessage::e_infoRequest :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveInfoRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_infoRequestResponse :
      return OnReceiveInfoRequestResponse(pdu, pdu);

    case H225_RasMessage::e_nonStandardMessage :
      OnReceiveNonStandardMessage(pdu, pdu);
      break;

    case H225_RasMessage::e_unknownMessageResponse :
      OnReceiveUnknownMessageResponse(pdu, pdu);
      break;

    case H225_RasMessage::e_requestInProgress :
      return OnReceiveRequestInProgress(pdu, pdu);

    case H225_RasMessage::e_resourcesAvailableIndicate :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveResourcesAvailableIndicate(pdu, pdu);
      break;

    case H225_RasMessage::e_resourcesAvailableConfirm :
      return OnReceiveResourcesAvailableConfirm(pdu, pdu);

    case H225_RasMessage::e_infoRequestAck :
      return OnReceiveInfoRequestAck(pdu, pdu);

    case H225_RasMessage::e_infoRequestNak :
      return OnReceiveInfoRequestNak(pdu, pdu);

#ifdef H323_H248
    case H225_RasMessage::e_serviceControlIndication :
      if (SendCachedResponse(pdu))
        return FALSE;
      OnReceiveServiceControlIndication(pdu, pdu);
      break;

    case H225_RasMessage::e_serviceControlResponse :
      return OnReceiveServiceControlResponse(pdu, pdu);
#endif

    default :
      OnReceiveUnknown(pdu);
  }

  return FALSE;
}


void H225_RAS::OnSendingPDU(PASN_Object & rawPDU)
{
  H323RasPDU & pdu = (H323RasPDU &)rawPDU;

  switch (pdu.GetTag()) {
    case H225_RasMessage::e_gatekeeperRequest :
      OnSendGatekeeperRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_gatekeeperConfirm :
      OnSendGatekeeperConfirm(pdu, pdu);
      break;

    case H225_RasMessage::e_gatekeeperReject :
      OnSendGatekeeperReject(pdu, pdu);
      break;

    case H225_RasMessage::e_registrationRequest :
      OnSendRegistrationRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_registrationConfirm :
      OnSendRegistrationConfirm(pdu, pdu);
      break;

    case H225_RasMessage::e_registrationReject :
      OnSendRegistrationReject(pdu, pdu);
      break;

    case H225_RasMessage::e_unregistrationRequest :
      OnSendUnregistrationRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_unregistrationConfirm :
      OnSendUnregistrationConfirm(pdu, pdu);
      break;

    case H225_RasMessage::e_unregistrationReject :
      OnSendUnregistrationReject(pdu, pdu);
      break;

    case H225_RasMessage::e_admissionRequest :
      OnSendAdmissionRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_admissionConfirm :
      OnSendAdmissionConfirm(pdu, pdu);
      break;

    case H225_RasMessage::e_admissionReject :
      OnSendAdmissionReject(pdu, pdu);
      break;

    case H225_RasMessage::e_bandwidthRequest :
      OnSendBandwidthRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_bandwidthConfirm :
      OnSendBandwidthConfirm(pdu, pdu);
      break;

    case H225_RasMessage::e_bandwidthReject :
      OnSendBandwidthReject(pdu, pdu);
      break;

    case H225_RasMessage::e_disengageRequest :
      OnSendDisengageRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_disengageConfirm :
      OnSendDisengageConfirm(pdu, pdu);
      break;

    case H225_RasMessage::e_disengageReject :
      OnSendDisengageReject(pdu, pdu);
      break;

    case H225_RasMessage::e_locationRequest :
      OnSendLocationRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_locationConfirm :
      OnSendLocationConfirm(pdu, pdu);
      break;

    case H225_RasMessage::e_locationReject :
      OnSendLocationReject(pdu, pdu);
      break;

    case H225_RasMessage::e_infoRequest :
      OnSendInfoRequest(pdu, pdu);
      break;

    case H225_RasMessage::e_infoRequestResponse :
      OnSendInfoRequestResponse(pdu, pdu);
      break;

    case H225_RasMessage::e_nonStandardMessage :
      OnSendNonStandardMessage(pdu, pdu);
      break;

    case H225_RasMessage::e_unknownMessageResponse :
      OnSendUnknownMessageResponse(pdu, pdu);
      break;

    case H225_RasMessage::e_requestInProgress :
      OnSendRequestInProgress(pdu, pdu);
      break;

    case H225_RasMessage::e_resourcesAvailableIndicate :
      OnSendResourcesAvailableIndicate(pdu, pdu);
      break;

    case H225_RasMessage::e_resourcesAvailableConfirm :
      OnSendResourcesAvailableConfirm(pdu, pdu);
      break;

    case H225_RasMessage::e_infoRequestAck :
      OnSendInfoRequestAck(pdu, pdu);
      break;

    case H225_RasMessage::e_infoRequestNak :
      OnSendInfoRequestNak(pdu, pdu);
      break;

#ifdef H323_H248
    case H225_RasMessage::e_serviceControlIndication :
      OnSendServiceControlIndication(pdu, pdu);
      break;

    case H225_RasMessage::e_serviceControlResponse :
      OnSendServiceControlResponse(pdu, pdu);
      break;
#endif

    default :
      break;
  }
}


BOOL H225_RAS::OnReceiveRequestInProgress(const H323RasPDU & pdu, const H225_RequestInProgress & rip)
{
  if (!HandleRequestInProgress(pdu, rip.m_delay))
    return FALSE;

  return OnReceiveRequestInProgress(rip);
}


BOOL H225_RAS::OnReceiveRequestInProgress(const H225_RequestInProgress & /*rip*/)
{
  return TRUE;
}


template <typename PDUType>
static void SendGenericData(const H225_RAS * ras, unsigned code, PDUType & pdu)
{

 H225_FeatureSet fs;
    if (!ras->OnSendFeatureSet(code,fs))
		return;

	if (fs.HasOptionalField(H225_FeatureSet::e_supportedFeatures)) {
	pdu.IncludeOptionalField(PDUType::e_genericData);

	H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
	H225_ArrayOf_GenericData & data = pdu.m_genericData;

		for (PINDEX i=0; i < fsn.GetSize(); i++) {
				PINDEX lastPos = data.GetSize();
				data.SetSize(lastPos+1);
				data[lastPos] = fsn[i];
		}
	}
}

template <typename PDUType>
static void SendFeatureSet(const H225_RAS * ras, unsigned code, PDUType & pdu)
{

 H225_FeatureSet fs;
    if (!ras->OnSendFeatureSet(code,fs))
		return;

     switch (code) {
#ifdef H323_H460
        case H460_MessageType::e_gatekeeperRequest:
        case H460_MessageType::e_gatekeeperConfirm:
        case H460_MessageType::e_gatekeeperReject:
        case H460_MessageType::e_registrationRequest:
        case H460_MessageType::e_registrationConfirm: 
        case H460_MessageType::e_registrationReject:
        case H460_MessageType::e_setup:					
        case H460_MessageType::e_callProceeding:
            pdu.IncludeOptionalField(PDUType::e_featureSet);
			pdu.m_featureSet = fs;
            break;
#endif
        default:
		  if (fs.HasOptionalField(H225_FeatureSet::e_supportedFeatures)) {
			pdu.IncludeOptionalField(PDUType::e_genericData);

			H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
		    H225_ArrayOf_GenericData & data = pdu.m_genericData;

			for (PINDEX i=0; i < fsn.GetSize(); i++) {
				 PINDEX lastPos = data.GetSize();
				 data.SetSize(lastPos+1);
				 data[lastPos] = fsn[i];
			}
		  }
		  break;
	 }
}


void H225_RAS::OnSendGatekeeperRequest(H323RasPDU &, H225_GatekeeperRequest & grq)
{
  // This function is never called during sending GRQ
  if (!gatekeeperIdentifier) {
    grq.IncludeOptionalField(H225_GatekeeperRequest::e_gatekeeperIdentifier);
    grq.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

  OnSendGatekeeperRequest(grq);
}


void H225_RAS::OnSendGatekeeperRequest(H225_GatekeeperRequest & grq)
{
#ifdef H323_H460
  SendFeatureSet<H225_GatekeeperRequest>(this, H460_MessageType::e_gatekeeperRequest, grq);
#endif
}


void H225_RAS::OnSendGatekeeperConfirm(H323RasPDU &, H225_GatekeeperConfirm & gcf)
{
  if (!gatekeeperIdentifier) {
    gcf.IncludeOptionalField(H225_GatekeeperConfirm::e_gatekeeperIdentifier);
    gcf.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

#ifdef H323_H460
  SendFeatureSet<H225_GatekeeperConfirm>(this, H460_MessageType::e_gatekeeperConfirm, gcf);
#endif

  OnSendGatekeeperConfirm(gcf);
}


void H225_RAS::OnSendGatekeeperConfirm(H225_GatekeeperConfirm & /*gcf*/)
{
}

void H225_RAS::OnSendGatekeeperReject(H323RasPDU &, H225_GatekeeperReject & grj)
{
  if (!gatekeeperIdentifier) {
    grj.IncludeOptionalField(H225_GatekeeperReject::e_gatekeeperIdentifier);
    grj.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

#ifdef H323_H460
  SendFeatureSet<H225_GatekeeperReject>(this, H460_MessageType::e_gatekeeperReject, grj);
#endif

  OnSendGatekeeperReject(grj);
}

void H225_RAS::OnSendGatekeeperReject(H225_GatekeeperReject & /*grj*/)
{
}

template <typename PDUType>
static void ProcessFeatureSet(const H225_RAS * ras, unsigned code, const PDUType & pdu)
{
    if (pdu.HasOptionalField(PDUType::e_genericData)) {
        H225_FeatureSet fs;
	    fs.IncludeOptionalField(H225_FeatureSet::e_supportedFeatures);
	    H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
		const H225_ArrayOf_GenericData & data = pdu.m_genericData;
		for (PINDEX i=0; i < data.GetSize(); i++) {
			 PINDEX lastPos = fsn.GetSize();
			 fsn.SetSize(lastPos+1);
			 fsn[lastPos] = (H225_FeatureDescriptor &)data[i];
		}
        ras->OnReceiveFeatureSet(code, fs);
	}
}

template <typename PDUType>
static void ReceiveGenericData(const H225_RAS * ras, unsigned code, const PDUType & pdu)
{
	ProcessFeatureSet<PDUType>(ras,code,pdu);
}

template <typename PDUType>
static void ReceiveFeatureSet(const H225_RAS * ras, unsigned code, const PDUType & pdu)
{
  if (pdu.HasOptionalField(PDUType::e_featureSet))
      ras->OnReceiveFeatureSet(code, pdu.m_featureSet);

	ProcessFeatureSet<PDUType>(ras,code,pdu);
}


BOOL H225_RAS::OnReceiveGatekeeperRequest(const H323RasPDU &, const H225_GatekeeperRequest & grq)
{
#ifdef H323_H460
  ReceiveFeatureSet<H225_GatekeeperRequest>(this, H460_MessageType::e_gatekeeperRequest, grq);
#endif

  return OnReceiveGatekeeperRequest(grq);
}


BOOL H225_RAS::OnReceiveGatekeeperRequest(const H225_GatekeeperRequest &)
{
  return TRUE;
}


BOOL H225_RAS::OnReceiveGatekeeperConfirm(const H323RasPDU &, const H225_GatekeeperConfirm & gcf)
{
  if (!CheckForResponse(H225_RasMessage::e_gatekeeperRequest, gcf.m_requestSeqNum))
    return FALSE;

  if (gatekeeperIdentifier.IsEmpty())
    gatekeeperIdentifier = gcf.m_gatekeeperIdentifier;
  else {
    PString gkid = gcf.m_gatekeeperIdentifier;
    if (gatekeeperIdentifier *= gkid)
      gatekeeperIdentifier = gkid;
    else {
      PTRACE(2, "RAS\tReceived a GCF from " << gkid
             << " but wanted it from " << gatekeeperIdentifier);
      return FALSE;
    }
  }

#ifdef H323_H460
  ReceiveFeatureSet<H225_GatekeeperConfirm>(this, H460_MessageType::e_gatekeeperConfirm, gcf);
#endif

  return OnReceiveGatekeeperConfirm(gcf);
}


BOOL H225_RAS::OnReceiveGatekeeperConfirm(const H225_GatekeeperConfirm & /*gcf*/)
{
  return TRUE;
}


BOOL H225_RAS::OnReceiveGatekeeperReject(const H323RasPDU &, const H225_GatekeeperReject & grj)
{
  if (!CheckForResponse(H225_RasMessage::e_gatekeeperRequest, grj.m_requestSeqNum, &grj.m_rejectReason))
    return FALSE;

#ifdef H323_H460
  ReceiveFeatureSet<H225_GatekeeperReject>(this, H460_MessageType::e_gatekeeperReject, grj);
#endif

  return OnReceiveGatekeeperReject(grj);
}


BOOL H225_RAS::OnReceiveGatekeeperReject(const H225_GatekeeperReject & /*grj*/)
{
  return TRUE;
}


void H225_RAS::OnSendRegistrationRequest(H323RasPDU & pdu, H225_RegistrationRequest & rrq)
{
  OnSendRegistrationRequest(rrq);

#ifdef H323_H460
  SendFeatureSet<H225_RegistrationRequest>(this, H460_MessageType::e_registrationRequest, rrq);
#endif

  pdu.Prepare(rrq.m_tokens, H225_RegistrationRequest::e_tokens,
              rrq.m_cryptoTokens, H225_RegistrationRequest::e_cryptoTokens);

}


void H225_RAS::OnSendRegistrationRequest(H225_RegistrationRequest & /*rrq*/)
{
}


void H225_RAS::OnSendRegistrationConfirm(H323RasPDU & pdu, H225_RegistrationConfirm & rcf)
{
  if (!gatekeeperIdentifier) {
    rcf.IncludeOptionalField(H225_RegistrationConfirm::e_gatekeeperIdentifier);
    rcf.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

  OnSendRegistrationConfirm(rcf);

#ifdef H323_H460
  SendFeatureSet<H225_RegistrationConfirm>(this, H460_MessageType::e_registrationConfirm, rcf);
#endif

  pdu.Prepare(rcf.m_tokens, H225_RegistrationConfirm::e_tokens,
              rcf.m_cryptoTokens, H225_RegistrationConfirm::e_cryptoTokens);
 
}


void H225_RAS::OnSendRegistrationConfirm(H225_RegistrationConfirm & /*rcf*/)
{
}


void H225_RAS::OnSendRegistrationReject(H323RasPDU & pdu, H225_RegistrationReject & rrj)
{
  if (!gatekeeperIdentifier) {
    rrj.IncludeOptionalField(H225_RegistrationReject::e_gatekeeperIdentifier);
    rrj.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

  OnSendRegistrationReject(rrj);

#ifdef H323_H460
  SendFeatureSet<H225_RegistrationReject>(this, H460_MessageType::e_registrationReject, rrj);
#endif

  pdu.Prepare(rrj.m_tokens, H225_RegistrationReject::e_tokens,
              rrj.m_cryptoTokens, H225_RegistrationReject::e_cryptoTokens);
}


void H225_RAS::OnSendRegistrationReject(H225_RegistrationReject & /*rrj*/)
{
}


BOOL H225_RAS::OnReceiveRegistrationRequest(const H323RasPDU &, const H225_RegistrationRequest & rrq)
{
#ifdef H323_H460
  ReceiveFeatureSet<H225_RegistrationRequest>(this, H460_MessageType::e_registrationRequest, rrq);
#endif

  return OnReceiveRegistrationRequest(rrq);
}


BOOL H225_RAS::OnReceiveRegistrationRequest(const H225_RegistrationRequest &)
{
  return TRUE;
}


BOOL H225_RAS::OnReceiveRegistrationConfirm(const H323RasPDU & pdu, const H225_RegistrationConfirm & rcf)
{
  if (!CheckForResponse(H225_RasMessage::e_registrationRequest, rcf.m_requestSeqNum))
    return FALSE;

  if (lastRequest != NULL) {
    PString endpointIdentifier = rcf.m_endpointIdentifier;
    const H235Authenticators & authenticators = lastRequest->requestPDU.GetAuthenticators();
    for (PINDEX i = 0; i < authenticators.GetSize(); i++) {
      H235Authenticator & authenticator = authenticators[i];
      if (authenticator.UseGkAndEpIdentifiers())
        authenticator.SetLocalId(endpointIdentifier);
    }
  }

  if (!CheckCryptoTokens(pdu,
                         rcf.m_tokens, H225_RegistrationConfirm::e_tokens,
                         rcf.m_cryptoTokens, H225_RegistrationConfirm::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveFeatureSet<H225_RegistrationConfirm>(this, H460_MessageType::e_registrationConfirm, rcf);
#endif

  return OnReceiveRegistrationConfirm(rcf);
}


BOOL H225_RAS::OnReceiveRegistrationConfirm(const H225_RegistrationConfirm & /*rcf*/)
{
  return TRUE;
}


BOOL H225_RAS::OnReceiveRegistrationReject(const H323RasPDU & pdu, const H225_RegistrationReject & rrj)
{
  if (!CheckForResponse(H225_RasMessage::e_registrationRequest, rrj.m_requestSeqNum, &rrj.m_rejectReason))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         rrj.m_tokens, H225_RegistrationReject::e_tokens,
                         rrj.m_cryptoTokens, H225_RegistrationReject::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveFeatureSet<H225_RegistrationReject>(this, H460_MessageType::e_registrationReject, rrj);
#endif

  return OnReceiveRegistrationReject(rrj);
}


BOOL H225_RAS::OnReceiveRegistrationReject(const H225_RegistrationReject & /*rrj*/)
{
  return TRUE;
}


void H225_RAS::OnSendUnregistrationRequest(H323RasPDU & pdu, H225_UnregistrationRequest & urq)
{
  OnSendUnregistrationRequest(urq);
  pdu.Prepare(urq.m_tokens, H225_UnregistrationRequest::e_tokens,
              urq.m_cryptoTokens, H225_UnregistrationRequest::e_cryptoTokens);
}


void H225_RAS::OnSendUnregistrationRequest(H225_UnregistrationRequest & /*urq*/)
{
}


void H225_RAS::OnSendUnregistrationConfirm(H323RasPDU & pdu, H225_UnregistrationConfirm & ucf)
{
  OnSendUnregistrationConfirm(ucf);
  pdu.Prepare(ucf.m_tokens, H225_UnregistrationConfirm::e_tokens,
              ucf.m_cryptoTokens, H225_UnregistrationConfirm::e_cryptoTokens);
}


void H225_RAS::OnSendUnregistrationConfirm(H225_UnregistrationConfirm & /*ucf*/)
{
}


void H225_RAS::OnSendUnregistrationReject(H323RasPDU & pdu, H225_UnregistrationReject & urj)
{
  OnSendUnregistrationReject(urj);
  pdu.Prepare(urj.m_tokens, H225_UnregistrationReject::e_tokens,
              urj.m_cryptoTokens, H225_UnregistrationReject::e_cryptoTokens);
}


void H225_RAS::OnSendUnregistrationReject(H225_UnregistrationReject & /*urj*/)
{
}


BOOL H225_RAS::OnReceiveUnregistrationRequest(const H323RasPDU & pdu, const H225_UnregistrationRequest & urq)
{
  if (!CheckCryptoTokens(pdu,
                         urq.m_tokens, H225_UnregistrationRequest::e_tokens,
                         urq.m_cryptoTokens, H225_UnregistrationRequest::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
   ProcessFeatureSet<H225_UnregistrationRequest>(this, H460_MessageType::e_unregistrationRequest, urq);
#endif

  return OnReceiveUnregistrationRequest(urq);
}


BOOL H225_RAS::OnReceiveUnregistrationRequest(const H225_UnregistrationRequest & /*urq*/)
{
  return TRUE;
}


BOOL H225_RAS::OnReceiveUnregistrationConfirm(const H323RasPDU & pdu, const H225_UnregistrationConfirm & ucf)
{
  if (!CheckForResponse(H225_RasMessage::e_unregistrationRequest, ucf.m_requestSeqNum))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         ucf.m_tokens, H225_UnregistrationConfirm::e_tokens,
                         ucf.m_cryptoTokens, H225_UnregistrationConfirm::e_cryptoTokens))
    return FALSE;

  return OnReceiveUnregistrationConfirm(ucf);
}


BOOL H225_RAS::OnReceiveUnregistrationConfirm(const H225_UnregistrationConfirm & /*ucf*/)
{
  return TRUE;
}


BOOL H225_RAS::OnReceiveUnregistrationReject(const H323RasPDU & pdu, const H225_UnregistrationReject & urj)
{
  if (!CheckForResponse(H225_RasMessage::e_unregistrationRequest, urj.m_requestSeqNum, &urj.m_rejectReason))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         urj.m_tokens, H225_UnregistrationReject::e_tokens,
                         urj.m_cryptoTokens, H225_UnregistrationReject::e_cryptoTokens))
    return FALSE;

  return OnReceiveUnregistrationReject(urj);
}


BOOL H225_RAS::OnReceiveUnregistrationReject(const H225_UnregistrationReject & /*urj*/)
{
  return TRUE;
}


void H225_RAS::OnSendAdmissionRequest(H323RasPDU & pdu, H225_AdmissionRequest & arq)
{
  OnSendAdmissionRequest(arq);

#ifdef H323_H460
  SendFeatureSet<H225_AdmissionRequest>(this, H460_MessageType::e_admissionRequest, arq);
#endif

  pdu.Prepare(arq.m_tokens, H225_AdmissionRequest::e_tokens,
              arq.m_cryptoTokens, H225_AdmissionRequest::e_cryptoTokens);
}


void H225_RAS::OnSendAdmissionRequest(H225_AdmissionRequest & /*arq*/)
{
}


void H225_RAS::OnSendAdmissionConfirm(H323RasPDU & pdu, H225_AdmissionConfirm & acf)
{
  OnSendAdmissionConfirm(acf);

  pdu.Prepare(acf.m_tokens, H225_AdmissionConfirm::e_tokens,
              acf.m_cryptoTokens, H225_AdmissionConfirm::e_cryptoTokens);
}


void H225_RAS::OnSendAdmissionConfirm(H225_AdmissionConfirm & /*acf*/)
{
}


void H225_RAS::OnSendAdmissionReject(H323RasPDU & pdu, H225_AdmissionReject & arj)
{
  OnSendAdmissionReject(arj);

#ifdef H323_H460
  SendFeatureSet<H225_AdmissionReject>(this, H460_MessageType::e_admissionReject, arj);
#endif

  pdu.Prepare(arj.m_tokens, H225_AdmissionReject::e_tokens,
              arj.m_cryptoTokens, H225_AdmissionReject::e_cryptoTokens);
}


void H225_RAS::OnSendAdmissionReject(H225_AdmissionReject & /*arj*/)
{
}


BOOL H225_RAS::OnReceiveAdmissionRequest(const H323RasPDU & pdu, const H225_AdmissionRequest & arq)
{
  if (!CheckCryptoTokens(pdu,
                         arq.m_tokens, H225_AdmissionRequest::e_tokens,
                         arq.m_cryptoTokens, H225_AdmissionRequest::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveFeatureSet<H225_AdmissionRequest>(this, H460_MessageType::e_admissionRequest, arq);
#endif

  return OnReceiveAdmissionRequest(arq);
}


BOOL H225_RAS::OnReceiveAdmissionRequest(const H225_AdmissionRequest & /*arq*/)
{
  return TRUE;
}


BOOL H225_RAS::OnReceiveAdmissionConfirm(const H323RasPDU & pdu, const H225_AdmissionConfirm & acf)
{
  if (!CheckForResponse(H225_RasMessage::e_admissionRequest, acf.m_requestSeqNum))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         acf.m_tokens, H225_AdmissionConfirm::e_tokens,
                         acf.m_cryptoTokens, H225_AdmissionConfirm::e_cryptoTokens))
    return FALSE;


  return OnReceiveAdmissionConfirm(acf);
}


BOOL H225_RAS::OnReceiveAdmissionConfirm(const H225_AdmissionConfirm & /*acf*/)
{
  return TRUE;
}


BOOL H225_RAS::OnReceiveAdmissionReject(const H323RasPDU & pdu, const H225_AdmissionReject & arj)
{
  if (!CheckForResponse(H225_RasMessage::e_admissionRequest, arj.m_requestSeqNum, &arj.m_rejectReason))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         arj.m_tokens, H225_AdmissionReject::e_tokens,
                         arj.m_cryptoTokens, H225_AdmissionReject::e_cryptoTokens))
    return FALSE;

  return OnReceiveAdmissionReject(arj);
}


BOOL H225_RAS::OnReceiveAdmissionReject(const H225_AdmissionReject & /*arj*/)
{
  return TRUE;
}


void H225_RAS::OnSendBandwidthRequest(H323RasPDU & pdu, H225_BandwidthRequest & brq)
{
  OnSendBandwidthRequest(brq);
  pdu.Prepare(brq.m_tokens, H225_BandwidthRequest::e_tokens,
              brq.m_cryptoTokens, H225_BandwidthRequest::e_cryptoTokens);
}


void H225_RAS::OnSendBandwidthRequest(H225_BandwidthRequest & /*brq*/)
{
}


BOOL H225_RAS::OnReceiveBandwidthRequest(const H323RasPDU & pdu, const H225_BandwidthRequest & brq)
{
  if (!CheckCryptoTokens(pdu,
                         brq.m_tokens, H225_BandwidthRequest::e_tokens,
                         brq.m_cryptoTokens, H225_BandwidthRequest::e_cryptoTokens))
    return FALSE;

  return OnReceiveBandwidthRequest(brq);
}


BOOL H225_RAS::OnReceiveBandwidthRequest(const H225_BandwidthRequest & /*brq*/)
{
  return TRUE;
}


void H225_RAS::OnSendBandwidthConfirm(H323RasPDU & pdu, H225_BandwidthConfirm & bcf)
{
  OnSendBandwidthConfirm(bcf);
  pdu.Prepare(bcf.m_tokens, H225_BandwidthConfirm::e_tokens,
              bcf.m_cryptoTokens, H225_BandwidthConfirm::e_cryptoTokens);
}


void H225_RAS::OnSendBandwidthConfirm(H225_BandwidthConfirm & /*bcf*/)
{
}


BOOL H225_RAS::OnReceiveBandwidthConfirm(const H323RasPDU & pdu, const H225_BandwidthConfirm & bcf)
{
  if (!CheckForResponse(H225_RasMessage::e_bandwidthRequest, bcf.m_requestSeqNum))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         bcf.m_tokens, H225_BandwidthConfirm::e_tokens,
                         bcf.m_cryptoTokens, H225_BandwidthConfirm::e_cryptoTokens))
    return FALSE;

  return OnReceiveBandwidthConfirm(bcf);
}


BOOL H225_RAS::OnReceiveBandwidthConfirm(const H225_BandwidthConfirm & /*bcf*/)
{
  return TRUE;
}


void H225_RAS::OnSendBandwidthReject(H323RasPDU & pdu, H225_BandwidthReject & brj)
{
  OnSendBandwidthReject(brj);
  pdu.Prepare(brj.m_tokens, H225_BandwidthReject::e_tokens,
              brj.m_cryptoTokens, H225_BandwidthReject::e_cryptoTokens);
}


void H225_RAS::OnSendBandwidthReject(H225_BandwidthReject & /*brj*/)
{
}


BOOL H225_RAS::OnReceiveBandwidthReject(const H323RasPDU & pdu, const H225_BandwidthReject & brj)
{
  if (!CheckForResponse(H225_RasMessage::e_bandwidthRequest, brj.m_requestSeqNum, &brj.m_rejectReason))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         brj.m_tokens, H225_BandwidthReject::e_tokens,
                         brj.m_cryptoTokens, H225_BandwidthReject::e_cryptoTokens))
    return FALSE;

  return OnReceiveBandwidthReject(brj);
}


BOOL H225_RAS::OnReceiveBandwidthReject(const H225_BandwidthReject & /*brj*/)
{
  return TRUE;
}


void H225_RAS::OnSendDisengageRequest(H323RasPDU & pdu, H225_DisengageRequest & drq)
{
  OnSendDisengageRequest(drq);
  pdu.Prepare(drq.m_tokens, H225_DisengageRequest::e_tokens,
              drq.m_cryptoTokens, H225_DisengageRequest::e_cryptoTokens);

#ifdef H323_H460
  SendGenericData<H225_DisengageRequest>(this, H460_MessageType::e_disengagerequest, drq);
#endif
}


void H225_RAS::OnSendDisengageRequest(H225_DisengageRequest & /*drq*/)
{
}


BOOL H225_RAS::OnReceiveDisengageRequest(const H323RasPDU & pdu, const H225_DisengageRequest & drq)
{
  if (!CheckCryptoTokens(pdu,
                         drq.m_tokens, H225_DisengageRequest::e_tokens,
                         drq.m_cryptoTokens, H225_DisengageRequest::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveGenericData<H225_DisengageRequest>(this, H460_MessageType::e_disengagerequest, drq);
#endif

  return OnReceiveDisengageRequest(drq);
}


BOOL H225_RAS::OnReceiveDisengageRequest(const H225_DisengageRequest & /*drq*/)
{
  return TRUE;
}


void H225_RAS::OnSendDisengageConfirm(H323RasPDU & pdu, H225_DisengageConfirm & dcf)
{
  OnSendDisengageConfirm(dcf);
  pdu.Prepare(dcf.m_tokens, H225_DisengageConfirm::e_tokens,
              dcf.m_cryptoTokens, H225_DisengageConfirm::e_cryptoTokens);

#ifdef H323_H460
  SendGenericData<H225_DisengageConfirm>(this, H460_MessageType::e_disengageconfirm, dcf);
#endif
}


void H225_RAS::OnSendDisengageConfirm(H225_DisengageConfirm & /*dcf*/)
{
}


BOOL H225_RAS::OnReceiveDisengageConfirm(const H323RasPDU & pdu, const H225_DisengageConfirm & dcf)
{
  if (!CheckForResponse(H225_RasMessage::e_disengageRequest, dcf.m_requestSeqNum))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         dcf.m_tokens, H225_DisengageConfirm::e_tokens,
                         dcf.m_cryptoTokens, H225_DisengageConfirm::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveGenericData<H225_DisengageConfirm>(this, H460_MessageType::e_disengageconfirm, dcf);
#endif

  return OnReceiveDisengageConfirm(dcf);
}


BOOL H225_RAS::OnReceiveDisengageConfirm(const H225_DisengageConfirm & /*dcf*/)
{
  return TRUE;
}


void H225_RAS::OnSendDisengageReject(H323RasPDU & pdu, H225_DisengageReject & drj)
{
  OnSendDisengageReject(drj);
  pdu.Prepare(drj.m_tokens, H225_DisengageReject::e_tokens,
              drj.m_cryptoTokens, H225_DisengageReject::e_cryptoTokens);
}


void H225_RAS::OnSendDisengageReject(H225_DisengageReject & /*drj*/)
{
}


BOOL H225_RAS::OnReceiveDisengageReject(const H323RasPDU & pdu, const H225_DisengageReject & drj)
{
  if (!CheckForResponse(H225_RasMessage::e_disengageRequest, drj.m_requestSeqNum, &drj.m_rejectReason))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         drj.m_tokens, H225_DisengageReject::e_tokens,
                         drj.m_cryptoTokens, H225_DisengageReject::e_cryptoTokens))
    return FALSE;

  return OnReceiveDisengageReject(drj);
}


BOOL H225_RAS::OnReceiveDisengageReject(const H225_DisengageReject & /*drj*/)
{
  return TRUE;
}


void H225_RAS::OnSendLocationRequest(H323RasPDU & pdu, H225_LocationRequest & lrq)
{
  OnSendLocationRequest(lrq);

#ifdef H323_H460
  SendFeatureSet<H225_LocationRequest>(this, H460_MessageType::e_locationRequest, lrq);
#endif

  pdu.Prepare(lrq.m_tokens, H225_LocationRequest::e_tokens,
              lrq.m_cryptoTokens, H225_LocationRequest::e_cryptoTokens);
}


void H225_RAS::OnSendLocationRequest(H225_LocationRequest & /*lrq*/)
{
}


BOOL H225_RAS::OnReceiveLocationRequest(const H323RasPDU & pdu, const H225_LocationRequest & lrq)
{
  if (!CheckCryptoTokens(pdu,
                         lrq.m_tokens, H225_LocationRequest::e_tokens,
                         lrq.m_cryptoTokens, H225_LocationRequest::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveFeatureSet<H225_LocationRequest>(this, H460_MessageType::e_locationRequest, lrq);
#endif

  return OnReceiveLocationRequest(lrq);
}


BOOL H225_RAS::OnReceiveLocationRequest(const H225_LocationRequest & /*lrq*/)
{
  return TRUE;
}


void H225_RAS::OnSendLocationConfirm(H323RasPDU & pdu, H225_LocationConfirm & lcf)
{
  OnSendLocationConfirm(lcf);

#ifdef H323_H460
  SendFeatureSet<H225_LocationConfirm>(this, H460_MessageType::e_locationConfirm, lcf);
#endif

  pdu.Prepare(lcf.m_tokens, H225_LocationRequest::e_tokens,
              lcf.m_cryptoTokens, H225_LocationRequest::e_cryptoTokens);
}


void H225_RAS::OnSendLocationConfirm(H225_LocationConfirm & /*lcf*/)
{
}


BOOL H225_RAS::OnReceiveLocationConfirm(const H323RasPDU &, const H225_LocationConfirm & lcf)
{
  if (!CheckForResponse(H225_RasMessage::e_locationRequest, lcf.m_requestSeqNum))
    return FALSE;

  if (lastRequest->responseInfo != NULL) {
    H323TransportAddress & locatedAddress = *(H323TransportAddress *)lastRequest->responseInfo;
    locatedAddress = lcf.m_callSignalAddress;
  }

#ifdef H323_H460
  ReceiveFeatureSet<H225_LocationConfirm>(this, H460_MessageType::e_locationConfirm, lcf);
#endif

  return OnReceiveLocationConfirm(lcf);
}


BOOL H225_RAS::OnReceiveLocationConfirm(const H225_LocationConfirm & /*lcf*/)
{
  return TRUE;
}


void H225_RAS::OnSendLocationReject(H323RasPDU & pdu, H225_LocationReject & lrj)
{
  OnSendLocationReject(lrj);

#ifdef H323_H460
  SendFeatureSet<H225_LocationReject>(this, H460_MessageType::e_locationReject, lrj);
#endif

  pdu.Prepare(lrj.m_tokens, H225_LocationReject::e_tokens,
              lrj.m_cryptoTokens, H225_LocationReject::e_cryptoTokens);
}


void H225_RAS::OnSendLocationReject(H225_LocationReject & /*lrj*/)
{
}


BOOL H225_RAS::OnReceiveLocationReject(const H323RasPDU & pdu, const H225_LocationReject & lrj)
{
  if (!CheckForResponse(H225_RasMessage::e_locationRequest, lrj.m_requestSeqNum, &lrj.m_rejectReason))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         lrj.m_tokens, H225_LocationReject::e_tokens,
                         lrj.m_cryptoTokens, H225_LocationReject::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveFeatureSet<H225_LocationReject>(this, H460_MessageType::e_locationReject, lrj);
#endif

  return OnReceiveLocationReject(lrj);
}


BOOL H225_RAS::OnReceiveLocationReject(const H225_LocationReject & /*lrj*/)
{
  return TRUE;
}


void H225_RAS::OnSendInfoRequest(H323RasPDU & pdu, H225_InfoRequest & irq)
{
  OnSendInfoRequest(irq);
  pdu.Prepare(irq.m_tokens, H225_InfoRequest::e_tokens,
              irq.m_cryptoTokens, H225_InfoRequest::e_cryptoTokens);

#ifdef H323_H460
  SendGenericData<H225_InfoRequest>(this, H460_MessageType::e_inforequest, irq);
#endif
}


void H225_RAS::OnSendInfoRequest(H225_InfoRequest & /*irq*/)
{
}


BOOL H225_RAS::OnReceiveInfoRequest(const H323RasPDU & pdu, const H225_InfoRequest & irq)
{
  if (!CheckCryptoTokens(pdu,
                         irq.m_tokens, H225_InfoRequest::e_tokens,
                         irq.m_cryptoTokens, H225_InfoRequest::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveGenericData<H225_InfoRequest>(this, H460_MessageType::e_inforequest, irq);
#endif

  return OnReceiveInfoRequest(irq);
}


BOOL H225_RAS::OnReceiveInfoRequest(const H225_InfoRequest & /*irq*/)
{
  return TRUE;
}


void H225_RAS::OnSendInfoRequestResponse(H323RasPDU & pdu, H225_InfoRequestResponse & irr)
{
  OnSendInfoRequestResponse(irr);
  pdu.Prepare(irr.m_tokens, H225_InfoRequestResponse::e_tokens,
              irr.m_cryptoTokens, H225_InfoRequestResponse::e_cryptoTokens);

#ifdef H323_H460
  SendGenericData<H225_InfoRequestResponse>(this, H460_MessageType::e_inforequestresponse, irr);
#endif
}


void H225_RAS::OnSendInfoRequestResponse(H225_InfoRequestResponse & /*irr*/)
{
}


BOOL H225_RAS::OnReceiveInfoRequestResponse(const H323RasPDU & pdu, const H225_InfoRequestResponse & irr)
{
  if (!CheckForResponse(H225_RasMessage::e_infoRequest, irr.m_requestSeqNum))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         irr.m_tokens, H225_InfoRequestResponse::e_tokens,
                         irr.m_cryptoTokens, H225_InfoRequestResponse::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveGenericData<H225_InfoRequestResponse>(this, H460_MessageType::e_inforequestresponse, irr);
#endif

  return OnReceiveInfoRequestResponse(irr);
}


BOOL H225_RAS::OnReceiveInfoRequestResponse(const H225_InfoRequestResponse & /*irr*/)
{
  return TRUE;
}


void H225_RAS::OnSendNonStandardMessage(H323RasPDU & pdu, H225_NonStandardMessage & nsm)
{
  OnSendNonStandardMessage(nsm);

#ifdef H323_H460
  SendFeatureSet<H225_NonStandardMessage>(this, H460_MessageType::e_nonStandardMessage, nsm);
#endif

  pdu.Prepare(nsm.m_tokens, H225_InfoRequestResponse::e_tokens,
              nsm.m_cryptoTokens, H225_InfoRequestResponse::e_cryptoTokens);
}


void H225_RAS::OnSendNonStandardMessage(H225_NonStandardMessage & /*nsm*/)
{
}


BOOL H225_RAS::OnReceiveNonStandardMessage(const H323RasPDU & pdu, const H225_NonStandardMessage & nsm)
{
  if (!CheckCryptoTokens(pdu,
                         nsm.m_tokens, H225_NonStandardMessage::e_tokens,
                         nsm.m_cryptoTokens, H225_NonStandardMessage::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveFeatureSet<H225_NonStandardMessage>(this, H460_MessageType::e_nonStandardMessage, nsm);
#endif

  return OnReceiveNonStandardMessage(nsm);
}


BOOL H225_RAS::OnReceiveNonStandardMessage(const H225_NonStandardMessage & /*nsm*/)
{
  return TRUE;
}


void H225_RAS::OnSendUnknownMessageResponse(H323RasPDU & pdu, H225_UnknownMessageResponse & umr)
{
  OnSendUnknownMessageResponse(umr);
  pdu.Prepare(umr.m_tokens, H225_UnknownMessageResponse::e_tokens,
              umr.m_cryptoTokens, H225_UnknownMessageResponse::e_cryptoTokens);
}


void H225_RAS::OnSendUnknownMessageResponse(H225_UnknownMessageResponse & /*umr*/)
{
}


BOOL H225_RAS::OnReceiveUnknownMessageResponse(const H323RasPDU & pdu, const H225_UnknownMessageResponse & umr)
{
  if (!CheckCryptoTokens(pdu,
                         umr.m_tokens, H225_UnknownMessageResponse::e_tokens,
                         umr.m_cryptoTokens, H225_UnknownMessageResponse::e_cryptoTokens))
    return FALSE;

  return OnReceiveUnknownMessageResponse(umr);
}


BOOL H225_RAS::OnReceiveUnknownMessageResponse(const H225_UnknownMessageResponse & /*umr*/)
{
  return TRUE;
}


void H225_RAS::OnSendRequestInProgress(H323RasPDU & pdu, H225_RequestInProgress & rip)
{
  OnSendRequestInProgress(rip);
  pdu.Prepare(rip.m_tokens, H225_RequestInProgress::e_tokens,
              rip.m_cryptoTokens, H225_RequestInProgress::e_cryptoTokens);
}


void H225_RAS::OnSendRequestInProgress(H225_RequestInProgress & /*rip*/)
{
}


void H225_RAS::OnSendResourcesAvailableIndicate(H323RasPDU & pdu, H225_ResourcesAvailableIndicate & rai)
{
  OnSendResourcesAvailableIndicate(rai);
  pdu.Prepare(rai.m_tokens, H225_ResourcesAvailableIndicate::e_tokens,
              rai.m_cryptoTokens, H225_ResourcesAvailableIndicate::e_cryptoTokens);
}


void H225_RAS::OnSendResourcesAvailableIndicate(H225_ResourcesAvailableIndicate & /*rai*/)
{
}


BOOL H225_RAS::OnReceiveResourcesAvailableIndicate(const H323RasPDU & pdu, const H225_ResourcesAvailableIndicate & rai)
{
  if (!CheckCryptoTokens(pdu,
                         rai.m_tokens, H225_ResourcesAvailableIndicate::e_tokens,
                         rai.m_cryptoTokens, H225_ResourcesAvailableIndicate::e_cryptoTokens))
    return FALSE;

  return OnReceiveResourcesAvailableIndicate(rai);
}


BOOL H225_RAS::OnReceiveResourcesAvailableIndicate(const H225_ResourcesAvailableIndicate & /*rai*/)
{
  return TRUE;
}


void H225_RAS::OnSendResourcesAvailableConfirm(H323RasPDU & pdu, H225_ResourcesAvailableConfirm & rac)
{
  OnSendResourcesAvailableConfirm(rac);
  pdu.Prepare(rac.m_tokens, H225_ResourcesAvailableConfirm::e_tokens,
              rac.m_cryptoTokens, H225_ResourcesAvailableConfirm::e_cryptoTokens);
}


void H225_RAS::OnSendResourcesAvailableConfirm(H225_ResourcesAvailableConfirm & /*rac*/)
{
}


BOOL H225_RAS::OnReceiveResourcesAvailableConfirm(const H323RasPDU & pdu, const H225_ResourcesAvailableConfirm & rac)
{
  if (!CheckForResponse(H225_RasMessage::e_resourcesAvailableIndicate, rac.m_requestSeqNum))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         rac.m_tokens, H225_ResourcesAvailableConfirm::e_tokens,
                         rac.m_cryptoTokens, H225_ResourcesAvailableConfirm::e_cryptoTokens))
    return FALSE;

  return OnReceiveResourcesAvailableConfirm(rac);
}


BOOL H225_RAS::OnReceiveResourcesAvailableConfirm(const H225_ResourcesAvailableConfirm & /*rac*/)
{
  return TRUE;
}

#ifdef H323_H248

void H225_RAS::OnSendServiceControlIndication(H323RasPDU & pdu, H225_ServiceControlIndication & sci)
{
  OnSendServiceControlIndication(sci);

#ifdef H323_H460
  SendFeatureSet<H225_ServiceControlIndication>(this, H460_MessageType::e_serviceControlIndication, sci);
#endif

  pdu.Prepare(sci.m_tokens, H225_ServiceControlIndication::e_tokens,
              sci.m_cryptoTokens, H225_ServiceControlIndication::e_cryptoTokens);
}


void H225_RAS::OnSendServiceControlIndication(H225_ServiceControlIndication & /*sci*/)
{
}


BOOL H225_RAS::OnReceiveServiceControlIndication(const H323RasPDU & pdu, const H225_ServiceControlIndication & sci)
{
  if (!CheckCryptoTokens(pdu,
                         sci.m_tokens, H225_ServiceControlIndication::e_tokens,
                         sci.m_cryptoTokens, H225_ServiceControlIndication::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveFeatureSet<H225_ServiceControlIndication>(this, H460_MessageType::e_serviceControlIndication, sci);
#endif

  return OnReceiveServiceControlIndication(sci);
}


BOOL H225_RAS::OnReceiveServiceControlIndication(const H225_ServiceControlIndication & /*sci*/)
{
  return TRUE;
}


void H225_RAS::OnSendServiceControlResponse(H323RasPDU & pdu, H225_ServiceControlResponse & scr)
{
  OnSendServiceControlResponse(scr);

#ifdef H323_H460
  SendFeatureSet<H225_ServiceControlResponse>(this, H460_MessageType::e_serviceControlResponse, scr);
#endif

  pdu.Prepare(scr.m_tokens, H225_ResourcesAvailableConfirm::e_tokens,
              scr.m_cryptoTokens, H225_ResourcesAvailableConfirm::e_cryptoTokens);
}


void H225_RAS::OnSendServiceControlResponse(H225_ServiceControlResponse & /*scr*/)
{
}


BOOL H225_RAS::OnReceiveServiceControlResponse(const H323RasPDU & pdu, const H225_ServiceControlResponse & scr)
{
  if (!CheckForResponse(H225_RasMessage::e_serviceControlIndication, scr.m_requestSeqNum))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         scr.m_tokens, H225_ServiceControlResponse::e_tokens,
                         scr.m_cryptoTokens, H225_ServiceControlResponse::e_cryptoTokens))
    return FALSE;

#ifdef H323_H460
  ReceiveFeatureSet<H225_ServiceControlResponse>(this, H460_MessageType::e_serviceControlResponse, scr);
#endif

  return OnReceiveServiceControlResponse(scr);
}


BOOL H225_RAS::OnReceiveServiceControlResponse(const H225_ServiceControlResponse & /*scr*/)
{
  return TRUE;
}

#endif // H323_H248


void H225_RAS::OnSendInfoRequestAck(H323RasPDU & pdu, H225_InfoRequestAck & iack)
{
  OnSendInfoRequestAck(iack);
  pdu.Prepare(iack.m_tokens, H225_InfoRequestAck::e_tokens,
              iack.m_cryptoTokens, H225_InfoRequestAck::e_cryptoTokens);
}


void H225_RAS::OnSendInfoRequestAck(H225_InfoRequestAck & /*iack*/)
{
}


BOOL H225_RAS::OnReceiveInfoRequestAck(const H323RasPDU & pdu, const H225_InfoRequestAck & iack)
{
  if (!CheckForResponse(H225_RasMessage::e_infoRequestResponse, iack.m_requestSeqNum))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         iack.m_tokens, H225_InfoRequestAck::e_tokens,
                         iack.m_cryptoTokens, H225_InfoRequestAck::e_cryptoTokens))
    return FALSE;

  return OnReceiveInfoRequestAck(iack);
}


BOOL H225_RAS::OnReceiveInfoRequestAck(const H225_InfoRequestAck & /*iack*/)
{
  return TRUE;
}


void H225_RAS::OnSendInfoRequestNak(H323RasPDU & pdu, H225_InfoRequestNak & inak)
{
  OnSendInfoRequestNak(inak);
  pdu.Prepare(inak.m_tokens, H225_InfoRequestAck::e_tokens,
              inak.m_cryptoTokens, H225_InfoRequestAck::e_cryptoTokens);
}


void H225_RAS::OnSendInfoRequestNak(H225_InfoRequestNak & /*inak*/)
{
}


BOOL H225_RAS::OnReceiveInfoRequestNak(const H323RasPDU & pdu, const H225_InfoRequestNak & inak)
{
  if (!CheckForResponse(H225_RasMessage::e_infoRequestResponse, inak.m_requestSeqNum, &inak.m_nakReason))
    return FALSE;

  if (!CheckCryptoTokens(pdu,
                         inak.m_tokens, H225_InfoRequestNak::e_tokens,
                         inak.m_cryptoTokens, H225_InfoRequestNak::e_cryptoTokens))
    return FALSE;

  return OnReceiveInfoRequestNak(inak);
}


BOOL H225_RAS::OnReceiveInfoRequestNak(const H225_InfoRequestNak & /*inak*/)
{
  return TRUE;
}


BOOL H225_RAS::OnReceiveUnknown(const H323RasPDU &)
{
  H323RasPDU response;
  response.BuildUnknownMessageResponse(0);
  return response.Write(*transport);
}


/////////////////////////////////////////////////////////////////////////////
