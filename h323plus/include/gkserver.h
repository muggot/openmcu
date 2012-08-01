/*
 * gkserver.h
 *
 * H225 Registration Admission and Security protocol handler
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
 * This code was based on original code from OpenGate of Egoboo Ltd. thanks
 * to Ashley Unitt for his efforts.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: gkserver.h,v $
 * Revision 1.1  2007/08/06 20:50:48  shorne
 * First commit of h323plus
 *
 * Revision 1.87  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.86  2005/01/03 14:03:19  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.85  2005/01/03 06:25:52  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.84  2004/12/14 06:22:20  csoutheren
 * More OSP implementation
 *
 * Revision 1.83  2004/09/03 01:06:08  csoutheren
 * Added initial hooks for H.460 GEF
 * Thanks to Simon Horne and ISVO (Asia) Pte Ltd. for this contribution
 *
 * Revision 1.82  2004/08/03 07:00:13  csoutheren
 * Added isGKRouted hint to TranslateAliasAddress as this is needed for the reply ACF
 *
 * Revision 1.81  2004/06/15 03:30:45  csoutheren
 * Adde guard to function to prevent crash if no aliases on an endpoint
 *
 * Revision 1.80  2004/06/04 07:05:07  csoutheren
 * Fixed obvious typos
 *
 * Revision 1.79  2004/04/25 01:52:46  rjongbloed
 * Fixed GCC 3.4 warnings
 *
 * Revision 1.78  2004/04/21 01:39:05  csoutheren
 * Added new overrides to GatekeeperServer to provide virtuals when calls and endpoints are created and destroyed
 *
 * Revision 1.77  2004/04/15 07:43:36  csoutheren
 * Allow gatekeeper to make a decision on multiple alias registrations for each registration
 *
 * Revision 1.76  2004/04/14 01:41:59  csoutheren
 * Added access to endpoint information for GetUserPassword
 *
 * Revision 1.75  2004/03/31 07:16:22  csoutheren
 * Added virtual to provide access to H.501 descriptor sent by GK
 *
 * Revision 1.74  2004/02/15 03:36:30  rjongbloed
 * Fixed bug in removing prefixes, plus added ability to have multiple endpoints
 *   with the same profix, thanks Rossano Ravelli
 *
 * Revision 1.73  2004/02/07 11:43:59  rjongbloed
 * Fixed deadlocks caused under high load by failure to check locking
 *   success. Also added functions to find destination endpoint.
 *   Thanks Federico Pinna and the gang at Reitek S.p.A.
 *
 * Revision 1.72  2003/12/24 07:08:47  csoutheren
 * Added TranslateAliasAddress on H323GatekeeperCall
 *
 * Revision 1.71  2003/12/11 05:39:04  csoutheren
 * Added storage of H.225 version in endpoint structure
 * Disabled sending RIPs to endpoints that cannot handle them
 *
 * Revision 1.70  2003/12/11 02:18:17  csoutheren
 * More fixes for NM deregistration problems. Seems to be fixed now :)
 *
 * Revision 1.69  2003/12/09 00:58:46  csoutheren
 * Fixed problem with endpoints deregistering
 *
 * Revision 1.68  2003/12/09 00:33:17  csoutheren
 * Added patch to prevent crash when NM unregisters from GK
 * Abject apologies to Rossano Ravelli for taking so long to add this
 *
 * Revision 1.67  2003/04/30 00:28:50  robertj
 * Redesigned the alternate credentials in ARQ system as old implementation
 *   was fraught with concurrency issues, most importantly it can cause false
 *   detection of replay attacks taking out an endpoint completely.
 *
 * Revision 1.66  2003/04/02 06:58:10  robertj
 * Added support for (optional) duplicate aliases in gatekeeper server, necessitating
 *   the removal of AddAlias and RemoveAlias functions.
 *
 * Revision 1.65  2003/04/02 06:07:12  robertj
 * Added H.323 Annex G Peer Element support to gatekeeper server.
 *
 * Revision 1.64  2003/04/01 07:30:52  robertj
 * Added function to translate alias into alias list and transport to make
 *   use of full information returned by peer element.
 *
 * Revision 1.63  2003/04/01 04:47:48  robertj
 * Abstracted H.225 RAS transaction processing (RIP and secondary thread) in
 *   server environment for use by H.501 peer elements.
 *
 * Revision 1.62  2003/03/26 07:10:03  robertj
 * Added more access functions.
 *
 * Revision 1.61  2003/03/17 22:52:06  craigs
 * Improvided useability of AddAlias and RemoveAlias
 *
 * Revision 1.60  2003/03/04 05:34:13  robertj
 * Fixed GNU warnings
 *
 * Revision 1.59  2003/03/04 05:27:51  robertj
 * Added statistics for rejected registrations and call admissions.
 *
 * Revision 1.58  2003/03/03 06:59:26  robertj
 * Fixed call SetBandwidth function so adjusts global bandwidth as well.
 *
 * Revision 1.57  2003/02/21 07:21:40  robertj
 * Fixed incorrect ancestor in PCLASSINFO()
 *
 * Revision 1.56  2003/02/21 05:28:39  craigs
 * Factored out code for user with peer elements
 *
 * Revision 1.55  2003/02/06 04:43:14  robertj
 * Changes to help with endpoints behind NAT.
 *
 * Revision 1.54  2003/02/03 03:58:06  robertj
 * Fixed use of local password database with ep's that have multiple aliases.
 *
 * Revision 1.53  2003/02/01 13:31:14  robertj
 * Changes to support CAT authentication in RAS.
 *
 * Revision 1.52  2002/12/17 01:25:23  robertj
 * Added call backs on gk server when ep indicates alerting or connect.
 *
 * Revision 1.51  2002/11/28 05:45:46  robertj
 * Fixed bug so can set total bandwidth while calls are in progress.
 *
 * Revision 1.50  2002/11/28 04:41:44  robertj
 * Added support for RAS ServiceControlIndication command.
 *
 * Revision 1.49  2002/11/27 06:54:52  robertj
 * Added Service Control Session management as per Annex K/H.323 via RAS
 *   only at this stage.
 * Added H.248 ASN and very primitive infrastructure for linking into the
 *   Service Control Session management system.
 * Added basic infrastructure for Annex K/H.323 HTTP transport system.
 * Added Call Credit Service Control to display account balances.
 *
 * Revision 1.48  2002/11/22 00:11:29  robertj
 * Added call start time.
 *
 * Revision 1.47  2002/11/18 23:40:45  robertj
 * Changed to use the H323EndPoint::CreateAuthenticators() function, we
 *   should use only one.
 *
 * Revision 1.46  2002/11/12 11:36:03  robertj
 * Added function to find endpoint by partial alias.
 * Optimised endpoint "indexes" to use sorted lists instead of dictionaries.
 *
 * Revision 1.45  2002/11/06 23:23:48  robertj
 * Fixed minor error in parameter, should be reference not value
 *
 * Revision 1.44  2002/10/29 00:12:02  robertj
 * Changed template classes so things like PSafeList actually creates the
 *   base collection class as well.
 * Improved setting of usage info, included getting it from BRQ etc.
 *
 * Revision 1.43  2002/10/21 00:54:47  robertj
 * Added function to unregister an endpoint via user interface.
 * Added reason code to call disengage.
 * Added ability to find call by a description string.
 *
 * Revision 1.42  2002/10/17 05:26:33  robertj
 * Added function to get at registered endpoints protocol version.
 *
 * Revision 1.41  2002/10/16 07:22:50  robertj
 * Added support for endpoints that do not support RRQ timeToLive parameter,
 *   will actively go see if ep is there using IRQ before expiring the ep.
 *
 * Revision 1.40  2002/10/01 08:17:34  robertj
 * Fixed (benign) race condition between client and server both wanting to do
 *   a DRQ of a call at the same time. Caused an Assert, but no crash.
 *
 * Revision 1.39  2002/09/26 01:27:31  robertj
 * Changed appliation info name in registered endpoint to be caseless string.
 *
 * Revision 1.38  2002/09/24 08:03:51  robertj
 * Added H.225 RAS protocol version number to registered endpoint structure.
 *
 * Revision 1.37  2002/09/19 09:15:56  robertj
 * Fixed problem with making (and assuring with multi-threading) IRQ and DRQ
 *   requests are sent to the correct endpoint address, thanks Martijn Roest.
 *
 * Revision 1.36  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.35  2002/09/03 06:19:36  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.34  2002/08/29 07:57:08  robertj
 * Added some statistics to gatekeeper server.
 *
 * Revision 1.33  2002/08/29 06:54:52  robertj
 * Removed redundent thread member variable from request info.
 *
 * Revision 1.32  2002/08/12 08:12:45  robertj
 * Added extra hint to help with ARQ using separate credentials from RRQ.
 *
 * Revision 1.31  2002/08/12 05:38:20  robertj
 * Changes to the RAS subsystem to support ability to make requests to client
 *   from gkserver without causing bottlenecks and race conditions.
 *
 * Revision 1.30  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.29  2002/08/05 05:17:37  robertj
 * Fairly major modifications to support different authentication credentials
 *   in ARQ to the logged in ones on RRQ. For both client and server.
 * Various other H.235 authentication bugs and anomalies fixed on the way.
 *
 * Revision 1.28  2002/07/16 13:49:22  robertj
 * Added missing lock when removing call from endpoint.
 *
 * Revision 1.27  2002/07/11 09:33:56  robertj
 * Added access functions to various call statistics member variables.
 *
 * Revision 1.26  2002/07/11 07:01:37  robertj
 * Added Disengage() function to force call drop from gk server.
 * Added InfoRequest() function to force client to send an IRR.
 * Added ability to automatically clear calls if do not get IRR for it.
 *
 * Revision 1.25  2002/06/21 02:52:44  robertj
 * Fixed problem with double checking H.235 hashing, this causes failure as
 *   the authenticator thinks it is a replay attack.
 *
 * Revision 1.24  2002/06/19 05:03:08  robertj
 * Changed gk code to allow for H.235 security on an endpoint by endpoint basis.
 *
 * Revision 1.23  2002/06/12 03:55:21  robertj
 * Added function to add/remove multiple listeners in one go comparing against
 *   what is already running so does not interrupt unchanged listeners.
 *
 * Revision 1.22  2002/05/29 00:03:15  robertj
 * Fixed unsolicited IRR support in gk client and server,
 *   including support for IACK and INAK.
 *
 * Revision 1.21  2002/05/21 06:30:31  robertj
 * Changed GRQ to the same as all the other xRQ request handlers.
 *
 * Revision 1.20  2002/05/17 03:42:08  robertj
 * Fixed problems with H.235 authentication on RAS for server and client.
 *
 * Revision 1.19  2002/05/07 03:18:12  robertj
 * Added application info (name/version etc) into registered endpoint data.
 *
 * Revision 1.18  2002/05/06 00:56:37  robertj
 * Sizeable rewrite of gatekeeper server code to make more bulletproof against
 *   multithreaded operation. Especially when using slow response/RIP feature.
 * Also changed the call indexing to use call id and direction as key.
 *
 * Revision 1.17  2002/04/30 23:19:00  dereks
 * Fix documentation typos.
 *
 * Revision 1.16  2002/03/06 02:01:31  robertj
 * Fixed race condition when starting slow server response thread.
 *
 * Revision 1.15  2002/03/05 00:36:01  craigs
 * Added GetReplyAddress for H323GatekeeperRequest
 *
 * Revision 1.14  2002/03/03 21:34:50  robertj
 * Added gatekeeper monitor thread.
 *
 * Revision 1.13  2002/03/02 05:58:57  robertj
 * Fixed possible bandwidth leak (thanks Francisco Olarte Sanz) and in
 *   the process added OnBandwidth function to H323GatekeeperCall class.
 *
 * Revision 1.12  2002/03/01 04:09:09  robertj
 * Fixed problems with keeping track of calls. Calls are now indexed by call-id
 *   alone and maintain both endpoints of call in its structure. Fixes problem
 *   with calls form an endpoint to itself, and having two objects being tracked
 *   where there is really only one call.
 *
 * Revision 1.11  2002/02/04 05:21:13  robertj
 * Lots of changes to fix multithreaded slow response code (RIP).
 * Fixed problem with having two entries for same call in call list.
 *
 * Revision 1.10  2002/01/31 06:45:44  robertj
 * Added more checking for invalid list processing in calls database.
 *
 * Revision 1.9  2002/01/31 00:16:15  robertj
 * Removed const to allow things to compile!
 *
 * Revision 1.8  2001/12/15 08:08:52  robertj
 * Added alerting, connect and end of call times to be sent to RAS server.
 *
 * Revision 1.7  2001/12/14 06:40:47  robertj
 * Added call end reason codes in DisengageRequest for GK server use.
 *
 * Revision 1.6  2001/12/13 11:08:45  robertj
 * Significant changes to support slow request handling, automatically sending
 *   RIP and spawning thread to handle time consuming operation.
 *
 * Revision 1.5  2001/11/19 06:56:44  robertj
 * Added prefix strings for gateways registered with this gk, thanks Mikael Stolt
 *
 * Revision 1.4  2001/08/10 11:03:49  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 * Revision 1.3  2001/08/06 07:44:52  robertj
 * Fixed problems with building without SSL
 *
 * Revision 1.2  2001/08/06 03:18:35  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 * Improved access to H.235 secure RAS functionality.
 * Changes to H.323 secure RAS contexts to help use with gk server.
 *
 * Revision 1.1  2001/07/24 02:30:55  robertj
 * Added gatekeeper RAS protocol server classes.
 *
 */

#ifndef __OPAL_GKSERVER_H
#define __OPAL_GKSERVER_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include "h323.h"
#include "h323ep.h"
#include "h225ras.h"
#include "guid.h"
#include "h235auth.h"
#include "h323pdu.h"
#include "h323trans.h"

#include <ptlib/safecoll.h>

class PASN_Sequence;
class PASN_Choice;

class H225_AliasAddress;
class H225_ArrayOf_TransportAddress;
class H225_GatekeeperIdentifier;
class H225_EndpointIdentifier;
class H225_InfoRequestResponse_perCallInfo_subtype;
class H225_RasUsageInformation;

class H323RegisteredEndPoint;
class H323GatekeeperListener;
class H323GatekeeperServer;
class H323RasPDU;
class H323PeerElement;


class H323GatekeeperRequest : public H323Transaction
{
    PCLASSINFO(H323GatekeeperRequest, H323Transaction);
  public:
  /**@name Construction */
  //@{
    /**Create a new gatekeeper server request.
     */
    H323GatekeeperRequest(
      H323GatekeeperListener & rasChannel,
      const H323RasPDU & pdu
    );
  //@}

    virtual H323TransactionPDU * CreateRIP(
      unsigned sequenceNumber,
      unsigned delay
    ) const;

    virtual BOOL WritePDU(
      H323TransactionPDU & pdu
    );
    BOOL CheckCryptoTokens();
    BOOL CheckGatekeeperIdentifier();
    BOOL GetRegisteredEndPoint();

    virtual PString GetGatekeeperIdentifier() const = 0;
    virtual unsigned GetGatekeeperRejectTag() const = 0;
    virtual PString GetEndpointIdentifier() const = 0;
    virtual unsigned GetRegisteredEndPointRejectTag() const = 0;
    virtual unsigned GetSecurityRejectTag() const = 0;

    H323GatekeeperListener & GetRasChannel() const { return rasChannel; }

    PSafePtr<H323RegisteredEndPoint> endpoint;

  protected:
    H323GatekeeperListener & rasChannel;
};


class H323GatekeeperGRQ : public H323GatekeeperRequest
{
    PCLASSINFO(H323GatekeeperGRQ, H323GatekeeperRequest);
  public:
    H323GatekeeperGRQ(
      H323GatekeeperListener & listener,
      const H323RasPDU & pdu
    );

#if PTRACING
    virtual const char * GetName() const;
#endif
    virtual PString GetGatekeeperIdentifier() const;
    virtual unsigned GetGatekeeperRejectTag() const;
    virtual PString GetEndpointIdentifier() const;
    virtual unsigned GetRegisteredEndPointRejectTag() const;
    virtual H235Authenticator::ValidationResult ValidatePDU() const;
    virtual unsigned GetSecurityRejectTag() const;
    virtual void SetRejectReason(
      unsigned reasonCode
    );

    H225_GatekeeperRequest & grq;
    H225_GatekeeperConfirm & gcf;
    H225_GatekeeperReject  & grj;

  protected:
    virtual Response OnHandlePDU();
};


class H323GatekeeperRRQ : public H323GatekeeperRequest
{
    PCLASSINFO(H323GatekeeperRRQ, H323GatekeeperRequest);
  public:
    H323GatekeeperRRQ(
      H323GatekeeperListener & listener,
      const H323RasPDU & pdu
    );

#if PTRACING
    virtual const char * GetName() const;
#endif
    virtual PString GetGatekeeperIdentifier() const;
    virtual unsigned GetGatekeeperRejectTag() const;
    virtual PString GetEndpointIdentifier() const;
    virtual unsigned GetRegisteredEndPointRejectTag() const;
    virtual H235Authenticator::ValidationResult ValidatePDU() const;
    virtual unsigned GetSecurityRejectTag() const;
    virtual void SetRejectReason(
      unsigned reasonCode
    );

    H225_RegistrationRequest & rrq;
    H225_RegistrationConfirm & rcf;
    H225_RegistrationReject  & rrj;

  protected:
    virtual Response OnHandlePDU();
};


class H323GatekeeperURQ : public H323GatekeeperRequest
{
    PCLASSINFO(H323GatekeeperURQ, H323GatekeeperRequest);
  public:
    H323GatekeeperURQ(
      H323GatekeeperListener & listener,
      const H323RasPDU & pdu
    );

#if PTRACING
    virtual const char * GetName() const;
#endif
    virtual PString GetGatekeeperIdentifier() const;
    virtual unsigned GetGatekeeperRejectTag() const;
    virtual PString GetEndpointIdentifier() const;
    virtual unsigned GetRegisteredEndPointRejectTag() const;
    virtual H235Authenticator::ValidationResult ValidatePDU() const;
    virtual unsigned GetSecurityRejectTag() const;
    virtual void SetRejectReason(
      unsigned reasonCode
    );

    H225_UnregistrationRequest & urq;
    H225_UnregistrationConfirm & ucf;
    H225_UnregistrationReject  & urj;

  protected:
    virtual Response OnHandlePDU();
};


class H323GatekeeperARQ : public H323GatekeeperRequest
{
    PCLASSINFO(H323GatekeeperARQ, H323GatekeeperRequest);
  public:
    H323GatekeeperARQ(
      H323GatekeeperListener & listener,
      const H323RasPDU & pdu
    );

#if PTRACING
    virtual const char * GetName() const;
#endif
    virtual PString GetGatekeeperIdentifier() const;
    virtual unsigned GetGatekeeperRejectTag() const;
    virtual PString GetEndpointIdentifier() const;
    virtual unsigned GetRegisteredEndPointRejectTag() const;
    virtual H235Authenticator::ValidationResult ValidatePDU() const;
    virtual unsigned GetSecurityRejectTag() const;
    virtual void SetRejectReason(
      unsigned reasonCode
    );

    H225_AdmissionRequest & arq;
    H225_AdmissionConfirm & acf;
    H225_AdmissionReject  & arj;

    PString alternateSecurityID;

  protected:
    virtual Response OnHandlePDU();
};


class H323GatekeeperDRQ : public H323GatekeeperRequest
{
    PCLASSINFO(H323GatekeeperDRQ, H323GatekeeperRequest);
  public:
    H323GatekeeperDRQ(
      H323GatekeeperListener & listener,
      const H323RasPDU & pdu
    );

#if PTRACING
    virtual const char * GetName() const;
#endif
    virtual PString GetGatekeeperIdentifier() const;
    virtual unsigned GetGatekeeperRejectTag() const;
    virtual PString GetEndpointIdentifier() const;
    virtual unsigned GetRegisteredEndPointRejectTag() const;
    virtual H235Authenticator::ValidationResult ValidatePDU() const;
    virtual unsigned GetSecurityRejectTag() const;
    virtual void SetRejectReason(
      unsigned reasonCode
    );

    H225_DisengageRequest & drq;
    H225_DisengageConfirm & dcf;
    H225_DisengageReject  & drj;

  protected:
    virtual Response OnHandlePDU();
};


class H323GatekeeperBRQ : public H323GatekeeperRequest
{
    PCLASSINFO(H323GatekeeperBRQ, H323GatekeeperRequest);
  public:
    H323GatekeeperBRQ(
      H323GatekeeperListener & listener,
      const H323RasPDU & pdu
    );

#if PTRACING
    virtual const char * GetName() const;
#endif
    virtual PString GetGatekeeperIdentifier() const;
    virtual unsigned GetGatekeeperRejectTag() const;
    virtual PString GetEndpointIdentifier() const;
    virtual unsigned GetRegisteredEndPointRejectTag() const;
    virtual H235Authenticator::ValidationResult ValidatePDU() const;
    virtual unsigned GetSecurityRejectTag() const;
    virtual void SetRejectReason(
      unsigned reasonCode
    );

    H225_BandwidthRequest & brq;
    H225_BandwidthConfirm & bcf;
    H225_BandwidthReject  & brj;

  protected:
    virtual Response OnHandlePDU();
};


class H323GatekeeperLRQ : public H323GatekeeperRequest
{
    PCLASSINFO(H323GatekeeperLRQ, H323GatekeeperRequest);
  public:
    H323GatekeeperLRQ(
      H323GatekeeperListener & listener,
      const H323RasPDU & pdu
    );

#if PTRACING
    virtual const char * GetName() const;
#endif
    virtual PString GetGatekeeperIdentifier() const;
    virtual unsigned GetGatekeeperRejectTag() const;
    virtual PString GetEndpointIdentifier() const;
    virtual unsigned GetRegisteredEndPointRejectTag() const;
    virtual H235Authenticator::ValidationResult ValidatePDU() const;
    virtual unsigned GetSecurityRejectTag() const;
    virtual void SetRejectReason(
      unsigned reasonCode
    );

    H225_LocationRequest & lrq;
    H225_LocationConfirm & lcf;
    H225_LocationReject  & lrj;

  protected:
    virtual Response OnHandlePDU();
};


class H323GatekeeperIRR : public H323GatekeeperRequest
{
    PCLASSINFO(H323GatekeeperIRR, H323GatekeeperRequest);
  public:
    H323GatekeeperIRR(
      H323GatekeeperListener & listener,
      const H323RasPDU & pdu
    );

#if PTRACING
    virtual const char * GetName() const;
#endif
    virtual PString GetGatekeeperIdentifier() const;
    virtual unsigned GetGatekeeperRejectTag() const;
    virtual PString GetEndpointIdentifier() const;
    virtual unsigned GetRegisteredEndPointRejectTag() const;
    virtual H235Authenticator::ValidationResult ValidatePDU() const;
    virtual unsigned GetSecurityRejectTag() const;
    virtual void SetRejectReason(
      unsigned reasonCode
    );

    H225_InfoRequestResponse & irr;
    H225_InfoRequestAck      & iack;
    H225_InfoRequestNak      & inak;

  protected:
    virtual Response OnHandlePDU();
};


/**This class describes an active call on a gatekeeper.
  */
class H323GatekeeperCall : public PSafeObject
{
    PCLASSINFO(H323GatekeeperCall, PSafeObject);
  public:
  /**@name Construction */
  //@{
    enum Direction {
      AnsweringCall,
      OriginatingCall,
      UnknownDirection
    };

    /**Create a new gatekeeper call tracking record.
     */
    H323GatekeeperCall(
      H323GatekeeperServer & server,               ///< Owner gatekeeper server
      const OpalGloballyUniqueID & callIdentifier, ///< Unique call identifier
      Direction direction                          ///< Direction of call
    );

    /**Destroy the call, removing itself from the endpoint.
      */
    ~H323GatekeeperCall();
  //@}

  /**@name Overrides from PObject */
  //@{
    /**Compare two objects.
      */
    Comparison Compare(
      const PObject & obj  ///< Other object
    ) const;

    /**Print the name of the gatekeeper.
      */
    void PrintOn(
      ostream & strm    ///< Stream to print to.
    ) const;
  //@}

  /**@name Operations */
  //@{
    /**Handle an admission ARQ PDU.
       The default behaviour sets some internal variables from the ARQ data
       and then calls OnResolveAdmission, if necessary, and OnReplyAdmission
       every time.
      */
    virtual H323GatekeeperRequest::Response OnAdmission(
      H323GatekeeperARQ & request
    );

    /**Shut down a call.
       This sendsa DRQ to the endpoint(s) to close the call down.
      */
    virtual BOOL Disengage(
      int reason = -1   ///< Reason for disengage
    );

    /**Handle a disengage DRQ PDU.
       The default behaviour simply returns TRUE.
      */
    virtual H323GatekeeperRequest::Response OnDisengage(
      H323GatekeeperDRQ & request
    );

    /**Handle a bandwidth BRQ PDU.
       The default behaviour adjusts the bandwidth used by the gatekeeper and
       adjusts the remote endpoint according to those limits.
      */
    virtual H323GatekeeperRequest::Response OnBandwidth(
      H323GatekeeperBRQ & request
    );

    /**Handle an info request response IRR PDU.
       The default behaviour resets the heartbeat time monitoring the call.
      */
    virtual H323GatekeeperRequest::Response OnInfoResponse(
      H323GatekeeperIRR & request,
      H225_InfoRequestResponse_perCallInfo_subtype & call
    );

    /**Call back when an info response detects an Alerting.
      */
    virtual void OnAlerting();

    /**Call back when an info response detects an Connected.
      */
    virtual void OnConnected();

    /**Function called to do heartbeat check of the call.
       Monitor the state of the call and make sure everything is OK.

       A return value of FALSE indicates the call is to be closed for some
       reason.

       Default behaviour checks the time since the last received IRR and if
       it has been too long does an IRQ to see if the call (and endpoint!) is
       still there and running. If the IRQ fails, FALSE is returned.
      */
    virtual BOOL OnHeartbeat();

#ifdef H323_H248

    /**Get the current credit for this call.
       This function is only called if the client indicates that it can use
       the information provided.

       The default behaviour calls the same function on the endpoint.
      */
    virtual PString GetCallCreditAmount() const;

    /**Get the call credit billing mode for this endpoint.
       This function is only called if the client indicates that it can use
       the information provided.

       The default behaviour calls the same function on the endpoint.
      */
    virtual BOOL GetCallCreditMode() const;

    /**Get the duration limit for this call.
       This function is only called if the client indicates that it can use
       the information provided.

       The default behaviour returns zero which indicates there is no
       duration limit applicable.
      */
    virtual unsigned GetDurationLimit() const;

    /**Send the call credit service control PDU.
       This will send an SCI pdu to the endpoint with the control service
       session information for the current call credit, if enabled.
      */
    virtual BOOL SendCallCreditServiceControl();

    /**Add call credit and duration information to PDU.
      */
    BOOL AddCallCreditServiceControl(
      H225_ArrayOf_ServiceControlSession & serviceControl
    ) const;

    /**Send the service control session for the PDU.
       This will send an SCI pdu to the endpoint with the control service
       session information provided.
      */
    virtual BOOL SendServiceControlSession(
      const H323ServiceControlSession & session
    );

#endif // H323_H248

    /**Translate a given alias to an alias set and signal address.
       This is called by the OnAdmission() handler to fill in the ACF

       The default behaviour calls H323GatekeeperServer::TranslateAliasAddress.
      */
    virtual BOOL TranslateAliasAddress(
      const H225_AliasAddress & alias,
      H225_ArrayOf_AliasAddress & aliases,
      H323TransportAddress & address,
      BOOL & isGkRouted
    );

  //@}

  /**@name Access functions */
  //@{
    H323GatekeeperServer & GetGatekeeper() const { return gatekeeper; }
    H323RegisteredEndPoint & GetEndPoint() const { return *PAssertNULL(endpoint); }
    BOOL IsAnsweringCall() const { return direction == AnsweringCall; }
    unsigned GetCallReference() const { return callReference; }
    const OpalGloballyUniqueID & GetCallIdentifier() const { return callIdentifier; }
    const OpalGloballyUniqueID & GetConferenceIdentifier() const { return conferenceIdentifier; }
    const PString & GetSourceNumber() const { return srcNumber; }
    const PStringArray & GetSourceAliases() const { return srcAliases; }
    const H323TransportAddress & GetSourceHost() const { return srcHost; }
    PString GetSourceAddress() const;
    const PString & GetDestinationNumber() const { return dstNumber; }
    const PStringArray & GetDestinationAliases() const { return dstAliases; }
    const H323TransportAddress & GetDestinationHost() const { return dstHost; }
    PString GetDestinationAddress() const;
    unsigned GetBandwidthUsed() const { return bandwidthUsed; }
    BOOL SetBandwidthUsed(unsigned bandwidth);
    const PTime & GetLastInfoResponseTime() const { return lastInfoResponse; }
    const PTime & GetCallStartTime() const { return callStartTime; }
    const PTime & GetAlertingTime() const { return alertingTime; }
    const PTime & GetConnectedTime() const { return connectedTime; }
    const PTime & GetCallEndTime() const { return callEndTime; }
    H323Connection::CallEndReason GetCallEndReason() const { return callEndReason; }
  //@}

  protected:
    void SetUsageInfo(const H225_RasUsageInformation & usage);

    H323GatekeeperServer   & gatekeeper;
    H323RegisteredEndPoint * endpoint;
    H323GatekeeperListener * rasChannel;

    Direction            direction;
    unsigned             callReference;
    OpalGloballyUniqueID callIdentifier;
    OpalGloballyUniqueID conferenceIdentifier;
    PString              srcNumber;
    PStringArray         srcAliases;
    H323TransportAddress srcHost;
    PString              dstNumber;
    PStringArray         dstAliases;
    H323TransportAddress dstHost;
    unsigned             bandwidthUsed;
    unsigned             infoResponseRate;
    PTime                lastInfoResponse;

    BOOL                          drqReceived;
    PTime                         callStartTime;
    PTime                         alertingTime;
    PTime                         connectedTime;
    PTime                         callEndTime;
    H323Connection::CallEndReason callEndReason;
};


/**This class describes endpoints that are registered with a gatekeeper server.
   Note that a registered endpoint has no realationship in this software to a
   H323EndPoint class. This is purely a description of endpoints that are
   registered with the gatekeeper.
  */
class H323RegisteredEndPoint : public PSafeObject
{
    PCLASSINFO(H323RegisteredEndPoint, PSafeObject);
  public:
  /**@name Construction */
  //@{
    /**Create a new endpoint registration record.
     */
    H323RegisteredEndPoint(
      H323GatekeeperServer & server, ///< Gatekeeper server data
      const PString & id             ///< Identifier
    );
  //@}

  /**@name Overrides from PObject */
  //@{
    /**Compare two objects.
      */
    Comparison Compare(
      const PObject & obj  ///< Other object
    ) const;

    /**Print the name of the gatekeeper.
      */
    void PrintOn(
      ostream & strm    ///< Stream to print to.
    ) const;
  //@}

  /**@name Call Operations */
  //@{
    /**Add a call to the endpoints list of active calls.
       This is largely an internal routine, it is not expected the user would
       need to deal with this function.
      */
    virtual void AddCall(
      H323GatekeeperCall * call
    );

    /**Remove a call from the endpoints list of active calls.
       This is largely an internal routine, it is not expected the user would
       need to deal with this function.
      */
    virtual BOOL RemoveCall(
      H323GatekeeperCall * call
    );

    /**Get the count of active calls on this endpoint.
      */
    PINDEX GetCallCount() const { return activeCalls.GetSize(); }

    /**Get the details of teh active call on this endpoint.
      */
    H323GatekeeperCall & GetCall(
      PINDEX idx
    ) { return activeCalls[idx]; }
  //@}

  /**@name Protocol Operations */
  //@{
    /**Call back on receiving a RAS registration for this endpoint.
       The default behaviour extract information from the RRQ and sets
       internal variables to that data.

       If returns TRUE then a RCF is sent otherwise an RRJ is sent.
      */
    virtual H323GatekeeperRequest::Response OnRegistration(
      H323GatekeeperRRQ & request
    );

    /**Call back on receiving a RAS full registration for this endpoint.
       This is not called if the keepAlive flag is set indicating a
       lightweight RRQ has been received.

       The default behaviour extract information from the RRQ and sets
       internal variables to that data.

       If returns TRUE then a RCF is sent otherwise an RRJ is sent.
      */
    virtual H323GatekeeperRequest::Response OnFullRegistration(
      H323GatekeeperRRQ & request
    );

    /**Call back to set security on RAS full registration for this endpoint.
       This is called from OnFullRegistration().

       The default behaviour extract information from the RRQ and sets
       internal variables to that data.

       If returns TRUE then a RCF is sent otherwise an RRJ is sent.
      */
    virtual H323GatekeeperRequest::Response OnSecureRegistration(
      H323GatekeeperRRQ & request
    );

    /**Call back on receiving a RAS unregistration for this endpoint.
       The default behaviour clears all calls owned by this endpoint.
      */
    virtual H323GatekeeperRequest::Response OnUnregistration(
      H323GatekeeperURQ & request
    );

    /**Force unregistration of the endpoint.
       This sendsa URQ to the endpoint(s) to close the call down.
      */
    virtual BOOL Unregister(
      int reason = -1   ///< Reason for unregistration
    );

    /**Handle an info request response IRR PDU.
       The default behaviour finds each call current for endpoint and calls 
       the function of the same name in the H323GatekeeperCall instance.
      */
    virtual H323GatekeeperRequest::Response OnInfoResponse(
      H323GatekeeperIRR & request
    );

    /**Function called to do time to live check of the call.
       Monitor the state of the endpoint and make sure everything is OK.

       A return value of FALSE indicates the endpoint has expired and is to be
       unregistered and removed.

       Default behaviour checks the time since the last received RRQ and if
       it has been too long does an IRQ to see if the endpoint is
       still there and running. If the IRQ fails, FALSE is returned.
      */
    virtual BOOL OnTimeToLive();

#ifdef H323_H248

    /**Get the current call credit for this endpoint.
       This function is only called if the client indicates that it can use
       the information provided. If a server wishes to enable this feature by
       returning a non-empty string, it must be consistent in that usage. That
       is the H323GatekeeperCall::GetCallCreditAmount() for this endpoint
       must also return non-empty value.

       The return value is a UTF-8 string for amount, including currency.

       The default behaviour returns an empty string disabling the function.
      */
    virtual PString GetCallCreditAmount() const;

    /**Get the call credit billing mode for this endpoint.
       This function is only called if the client indicates that it can use
       the information provided.

       The default behaviour return TRUE indicating that calls will debit the
       account.
      */
    virtual BOOL GetCallCreditMode() const;

    /**Send the service control session for the PDU.
       This will send an SCI pdu to the endpoint with the control service
       session information provided.
      */
    virtual BOOL SendServiceControlSession(
      const H323ServiceControlSession & session
    );

    /**Set the service control session for the PDU.
       This is an internal function.
      */
    virtual BOOL AddServiceControlSession(
      const H323ServiceControlSession & session,
      H225_ArrayOf_ServiceControlSession & serviceControl
    );
#endif // H323_H248
  //@}

  /**@name Access functions */
  //@{
    /**Set password for user activating H.235 security.
      */
    virtual BOOL SetPassword(
      const PString & password,
      const PString & username = PString::Empty()
    );

    /**Get the endpoint identifier assigned to the endpoint.
      */
    const PString & GetIdentifier() const { return identifier; }

    /**Get the Peer Element descriptor ID assigned to the endpoint.
      */
    const OpalGloballyUniqueID & GetDescriptorID() const { return descriptorID; }

    /**Get the gatekeeper server data object that owns this endpoint.
      */
    H323GatekeeperServer & GetGatekeeper() const { return gatekeeper; }

    /**Get the addresses that can be used to contact this endpoint via the
       RAS protocol.
      */
    const H323TransportAddressArray & GetRASAddresses() const { return rasAddresses; }

    /**Get the number of addresses that can be used to contact this
       endpoint via the RAS protocol.
      */
    PINDEX GetRASAddressCount() const { return rasAddresses.GetSize(); }

    /**Get an address that can be used to contact this endpoint via the RAS
       protocol.
      */
    H323TransportAddress GetRASAddress(
      PINDEX idx
    ) const { return rasAddresses[idx]; }

    /**Get the addresses that can be used to contact this
       endpoint via the H.225/Q.931 protocol, ie normal calls.
      */
    const H323TransportAddressArray & GetSignalAddresses() const { return signalAddresses; }

    /**Get the number of addresses that can be used to contact this
       endpoint via the H.225/Q.931 protocol, ie normal calls.
      */
    PINDEX GetSignalAddressCount() const { return signalAddresses.GetSize(); }

    /**Get an address that can be used to contact this endpoint via the
       H.225/Q.931 protocol, ie normal calls.
      */
    H323TransportAddress GetSignalAddress(
      PINDEX idx
    ) const { return signalAddresses[idx]; }

    /**Get the aliases this endpoint may be identified by.
      */
    const PStringArray & GetAliases() const { return aliases; }

    /**Determine if alias is an alias that this endpoint may be identified by.
      */
    BOOL ContainsAlias(
      const PString & alias
      ) { return aliases.GetStringsIndex(alias) != P_MAX_INDEX; }

    /**Get the number of aliases this endpoint may be identified by.
      */
    PINDEX GetAliasCount() const { return aliases.GetSize(); }

    /**Get an alias that this endpoint may be identified by.
      */
    PString GetAlias(
      PINDEX idx
    ) const { if (idx < aliases.GetSize()) return aliases[idx]; return PString::Empty(); }

    /** Remove an alias that this endpoint may be identified by.
      * If this was the last alias, then endpoint will be deleted soon
      * after by a cleanup thread. As this function is in the endpoint itself
      * it is not possible for this function to delete the endpoint immediately
      */
    void RemoveAlias(
      const PString & alias
    );

    /**Get the security context for this RAS connection.
      */
    virtual const H235Authenticators & GetAuthenticators() const { return authenticators; }

    /**Get the number of prefixes this endpoint can accept.
      */
    PINDEX GetPrefixCount() const { return voicePrefixes.GetSize(); }

    /**Get a prefix that this endpoint can accept.
      */
    PString GetPrefix(
      PINDEX idx
    ) const { return voicePrefixes[idx]; }

    /**Get application info (name/version etc) for endpoint.
      */
    const PCaselessString & GetApplicationInfo() const { return applicationInfo; }

    /**Get the protocol version the endpoint registered with.
      */
    unsigned GetProtocolVersion() const { return protocolVersion; }

    /**Return if gatekeeper thinks the endpoint is behind a firewall.
      */
    BOOL IsBehindNAT() const { return isBehindNAT; }

    /**Get the flag indicating the endpoint can display credit amounts.
      */
    BOOL CanDisplayAmountString() const { return canDisplayAmountString; }

    /**Get the flag indicating the endpoint can enforce a duration limit.
      */
    BOOL CanEnforceDurationLimit() const { return canEnforceDurationLimit; }

    /**Get the flag indicating the endpoint can handle RIPs (H225v1 endpoints cannot)
      */
    BOOL CanReceiveRIP() const;

    /**Get the H225 version reported in the RRQ
      */
    BOOL GetH225Version() const { return h225Version; }
  //@}

#ifdef H323_H501
  /**@name H.501 access functions */
  //@{
    /**
      * Function called when gatekeeper sends a descriptor for this endpoint.
      * This allows the gatekeeper to alter the descriptor information before
      * it is sent.
      *
      * If returns FALSE then the desriptor is not sent
      */
      virtual BOOL OnSendDescriptorForEndpoint(
        H225_ArrayOf_AliasAddress & aliases,          ///< aliases for the enndpoint
        H225_EndpointType & terminalType,             ///< terminal type
        H225_ArrayOf_AliasAddress & transportAddresses  ///< transport addresses
      );
  //@}
#endif


  protected:
    H323GatekeeperServer    & gatekeeper;
    H323GatekeeperListener  * rasChannel;

    PString                   identifier;
    OpalGloballyUniqueID      descriptorID;
    H323TransportAddressArray rasAddresses;
    H323TransportAddressArray signalAddresses;
    PStringArray              aliases;
    PStringArray              voicePrefixes;
    PCaselessString           applicationInfo;
    unsigned                  protocolVersion;
    BOOL                      isBehindNAT;
    BOOL                      canDisplayAmountString;
    BOOL                      canEnforceDurationLimit;
    unsigned                  h225Version;
    unsigned                  timeToLive;
    H235Authenticators        authenticators;

    PTime lastRegistration;
    PTime lastInfoResponse;

    PSortedList<H323GatekeeperCall> activeCalls;
#ifdef H323_H248
    POrdinalDictionary<PString>     serviceControlSessions;
#endif
};


/**This class embodies the low level H.225.0 RAS protocol on gatekeepers.
   One or more instances of this class may be used to access a single
   H323GatekeeperServer instance. Thus specific interfaces could be set up
   to receive UDP packets, all operating as the same gatekeeper.
  */
class H323GatekeeperListener : public H225_RAS
{
    PCLASSINFO(H323GatekeeperListener, H225_RAS);
  public:
  /**@name Construction */
  //@{
    /**Create a new gatekeeper listener.
     */
    H323GatekeeperListener(
      H323EndPoint & endpoint,               ///< Local endpoint
      H323GatekeeperServer & server,         ///< Database for gatekeeper
      const PString & gatekeeperIdentifier,  ///< Name of this gatekeeper
      H323Transport * transport = NULL       ///< Transport over which gatekeepers communicates.
    );

    /**Destroy gatekeeper listener.
     */
    ~H323GatekeeperListener();
  //@}

  /**@name Operations */
  //@{
    /**Send a UnregistrationRequest (URQ) to endpoint.
      */
    BOOL UnregistrationRequest(
      const H323RegisteredEndPoint & ep,
      unsigned reason
    );

    /**Send a DisengageRequest (DRQ) to endpoint.
      */
    BOOL DisengageRequest(
      const H323GatekeeperCall & call,
      unsigned reason
    );

    /**Send an InfoRequest (IRQ) to endpoint.
      */
    virtual BOOL InfoRequest(
      H323RegisteredEndPoint & ep,
      H323GatekeeperCall * call = NULL
    );

#ifdef H323_H248
    /**Send an ServiceControlIndication (SCI) to endpoint.
      */
    virtual BOOL ServiceControlIndication(
      H323RegisteredEndPoint & ep,
      const H323ServiceControlSession & session,
      H323GatekeeperCall * call = NULL
    );
#endif
  //@}

  /**@name Operation callbacks */
  //@{
    /**Handle a discovery GRQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual H323GatekeeperRequest::Response OnDiscovery(
      H323GatekeeperGRQ & request
    );

    /**Handle a registration RRQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual H323GatekeeperRequest::Response OnRegistration(
      H323GatekeeperRRQ & request
    );

    /**Handle an unregistration URQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual H323GatekeeperRequest::Response OnUnregistration(
      H323GatekeeperURQ & request
    );

    /**Handle an admission ARQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual H323GatekeeperRequest::Response OnAdmission(
      H323GatekeeperARQ & request
    );

    /**Handle a disengage DRQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual H323GatekeeperRequest::Response OnDisengage(
      H323GatekeeperDRQ & request
    );

    /**Handle a bandwidth BRQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual H323GatekeeperRequest::Response OnBandwidth(
      H323GatekeeperBRQ & request
    );

    /**Handle a location LRQ PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual H323GatekeeperRequest::Response OnLocation(
      H323GatekeeperLRQ & request
    );

    /**Handle an info request response IRR PDU.
       The default behaviour does some checks and calls the gatekeeper server
       instances function of the same name.
      */
    virtual H323GatekeeperRequest::Response OnInfoResponse(
      H323GatekeeperIRR & request
    );
  //@}

  /**@name Low level protocol callbacks */
  //@{
    BOOL OnReceiveGatekeeperRequest(const H323RasPDU &, const H225_GatekeeperRequest &);
    BOOL OnReceiveRegistrationRequest(const H323RasPDU &, const H225_RegistrationRequest &);
    BOOL OnReceiveUnregistrationRequest(const H323RasPDU &, const H225_UnregistrationRequest &);
    BOOL OnReceiveUnregistrationConfirm(const H225_UnregistrationConfirm &);
    BOOL OnReceiveUnregistrationReject(const H225_UnregistrationReject &);
    BOOL OnReceiveAdmissionRequest(const H323RasPDU &, const H225_AdmissionRequest &);
    BOOL OnReceiveBandwidthRequest(const H323RasPDU &, const H225_BandwidthRequest &);
    BOOL OnReceiveBandwidthConfirm(const H225_BandwidthConfirm &);
    BOOL OnReceiveBandwidthReject(const H225_BandwidthReject &);
    BOOL OnReceiveDisengageRequest(const H323RasPDU &, const H225_DisengageRequest &);
    BOOL OnReceiveDisengageConfirm(const H225_DisengageConfirm &);
    BOOL OnReceiveDisengageReject(const H225_DisengageReject &);
    BOOL OnReceiveLocationRequest(const H323RasPDU &, const H225_LocationRequest &);
    BOOL OnReceiveInfoRequestResponse(const H323RasPDU &, const H225_InfoRequestResponse &);
    BOOL OnReceiveResourcesAvailableConfirm(const H225_ResourcesAvailableConfirm &);
    BOOL OnSendFeatureSet(unsigned, H225_FeatureSet & features) const;
    void OnReceiveFeatureSet(unsigned, const H225_FeatureSet & features) const;
  //@}

  /**@name Member access */
  //@{
    H323GatekeeperServer & GetGatekeeper() const { return gatekeeper; }
  //@}


  protected:
    H323GatekeeperServer & gatekeeper;
};


/**This class implements a basic gatekeeper server functionality.
   An instance of this class contains all of the state information and
   operations for a gatekeeper. Multiple gatekeeper listeners may be using
   this class to link individual UDP (or other protocol) packets from various
   sources (interfaces etc) into a single instance.

   There is typically only one instance of this class, though it is not
   limited to that. An application would also quite likely descend from this
   class and override call back functions to implement more complex policy.
  */
class H323GatekeeperServer : public H323TransactionServer
{
    PCLASSINFO(H323GatekeeperServer, H323TransactionServer);
  public:
  /**@name Construction */
  //@{
    /**Create a new gatekeeper.
     */
    H323GatekeeperServer(
      H323EndPoint & endpoint
    );

    /**Destroy gatekeeper.
     */
    ~H323GatekeeperServer();
  //@}

    WORD GetDefaultUdpPort() { return H225_RAS::DefaultRasUdpPort; }

  /**@name Protocol Handler Operations */
  //@{
    /**Create a new H323GatkeeperListener.
       The user woiuld not usually use this function as it is used internally
       by the server when new listeners are added by H323TransportAddress.

       However, a user may override this function to create objects that are
       user defined descendants of H323GatekeeperListener so the user can
       maintain extra information on a interface by interface basis.
      */
    virtual H323Transactor * CreateListener(
      H323Transport * transport  ///< Transport for listener
    );

  //@}

  /**@name EndPoint Operations */
  //@{
    /**Handle a discovery GRQ PDU.
       The default behaviour deals with the authentication scheme nogotiation.
      */
    virtual H323GatekeeperRequest::Response OnDiscovery(
      H323GatekeeperGRQ & request
    );

    /**Call back on receiving a RAS registration for this endpoint.
       The default behaviour checks if the registered endpoint already exists
       and if not creates a new endpoint. It then calls the OnRegistration()
       on that new endpoint instance.

       If returns TRUE then a RCF is sent otherwise an RRJ is sent.
      */
    virtual H323GatekeeperRequest::Response OnRegistration(
      H323GatekeeperRRQ & request
    );

    /**Handle an unregistration URQ PDU.
       The default behaviour removes the aliases defined in the URQ and if all
       aliases for the registered endpoint are removed then the endpoint itself
       is removed.
      */
    virtual H323GatekeeperRequest::Response OnUnregistration(
      H323GatekeeperURQ & request
    );

    /**Handle an info request response IRR PDU.
       The default behaviour calls the function of the same name in the
       endpoint instance.
      */
    virtual H323GatekeeperRequest::Response OnInfoResponse(
      H323GatekeeperIRR & request
    );

    /**Add a new registered endpoint to the server database.
       Once the endpoint has been added it is then owned by the server and
       will be deleted when it is removed.

       The user woiuld not usually use this function as it is used internally
       by the server when new registration requests (RRQ) are received.

       Note that a registered endpoint has no realationship in this software
       to a H323EndPoint class.
      */
    virtual void AddEndPoint(
      H323RegisteredEndPoint * ep
    );

    /**Remove a registered endpoint from the server database.
      */
    virtual BOOL RemoveEndPoint(
      H323RegisteredEndPoint * ep
    );

    /**Create a new registered endpoint object.
       The user woiuld not usually use this function as it is used internally
       by the server when new registration requests (RRQ) are received.

       However, a user may override this function to create objects that are
       user defined descendants of H323RegisteredEndPoint so the user can
       maintain extra information on a endpoint by endpoint basis.
      */
    virtual H323RegisteredEndPoint * CreateRegisteredEndPoint(
      H323GatekeeperRRQ & request
    );

    /**Create a new unique identifier for the registered endpoint.
       The returned identifier must be unique over the lifetime of this
       gatekeeper server.

       The default behaviour simply returns the string representation of the
       member variable nextIdentifier. There could be a problem in this
       implementation after 4,294,967,296 have been registered.
      */
    virtual PString CreateEndPointIdentifier();

    /**Find a registered endpoint given its endpoint identifier.
      */
    virtual PSafePtr<H323RegisteredEndPoint> FindEndPointByIdentifier(
      const PString & identifier,
      PSafetyMode mode = PSafeReference
    );

    /**Find a registered endpoint given a list of signal addresses.
      */
    virtual PSafePtr<H323RegisteredEndPoint> FindEndPointBySignalAddresses(
      const H225_ArrayOf_TransportAddress & addresses,
      PSafetyMode mode = PSafeReference
    );

    /**Find a registered endpoint given its signal address.
      */
    virtual PSafePtr<H323RegisteredEndPoint> FindEndPointBySignalAddress(
      const H323TransportAddress & address,
      PSafetyMode mode = PSafeReference
    );

    /**Find a registered endpoint given its raw alias address.
      */
    virtual PSafePtr<H323RegisteredEndPoint> FindEndPointByAliasAddress(
      const H225_AliasAddress & alias,
      PSafetyMode mode = PSafeReadWrite
    );

    /**Find a registered endpoint given its simple alias string.
      */
    virtual PSafePtr<H323RegisteredEndPoint> FindEndPointByAliasString(
      const PString & alias,
      PSafetyMode mode = PSafeReference
    );

    /**Find the first registered endpoint given a partial alias string.
      */
    virtual PSafePtr<H323RegisteredEndPoint> FindEndPointByPartialAlias(
      const PString & alias,
      PSafetyMode mode = PSafeReference
    );

    /**Find the first registered endpoint given a number prefix string.
      */
    virtual PSafePtr<H323RegisteredEndPoint> FindEndPointByPrefixString(
      const PString & prefix,
      PSafetyMode mode = PSafeReference
    );

    /**Get first endpoint for enumeration.
      */
    PSafePtr<H323RegisteredEndPoint> GetFirstEndPoint(
      PSafetyMode mode = PSafeReference
    ) { return PSafePtr<H323RegisteredEndPoint>(byIdentifier, mode); }
  //@}

    PSafePtr<H323RegisteredEndPoint> FindDestinationEndPoint(
      const OpalGloballyUniqueID & id, 
      H323GatekeeperCall::Direction direction
    );
    
  /**@name Call Operations */
  //@{
    /**Handle an admission ARQ PDU.
       The default behaviour verifies that the call is allowed by the policies
       the gatekeeper server requires, then attempts to look up the required
       signal address for the call. It also manages bandwidth allocations.
      */
    virtual H323GatekeeperRequest::Response OnAdmission(
      H323GatekeeperARQ & request
    );

    /**Handle a disengage DRQ PDU.
       The default behaviour finds the call by its id provided in the DRQ and
       removes it from the gatekeeper server database.
      */
    virtual H323GatekeeperRequest::Response OnDisengage(
      H323GatekeeperDRQ & request
    );

    /**Handle a bandwidth BRQ PDU.
       The default behaviour finds the call and does some checks then calls
       the H323GatekeeperCall function of the same name.
      */
    virtual H323GatekeeperRequest::Response OnBandwidth(
      H323GatekeeperBRQ & request
    );

    /**Create a new call object.
       The user woiuld not usually use this function as it is used internally
       by the server when new calls (ARQ) are made.

       However, a user may override this function to create objects that are
       user defined descendants of H323GatekeeperCall so the user can
       maintain extra information on a call by call basis.
      */
    virtual H323GatekeeperCall * CreateCall(
      const OpalGloballyUniqueID & callIdentifier,
      H323GatekeeperCall::Direction direction
    );

    /** Called whenever a new call is started
      */
    virtual void AddCall(H323GatekeeperCall *) 
    { }

    /** Called whenever call is ended. By default, removes the call from the call list
      */
    virtual void RemoveCall(
      H323GatekeeperCall * call
    );

    /**Find the call given the identifier.
      */
    virtual PSafePtr<H323GatekeeperCall> FindCall(
      const PString & description,
      PSafetyMode mode = PSafeReference
    );

    /**Find the call given the identifier.
      */
    virtual PSafePtr<H323GatekeeperCall> FindCall(
      const OpalGloballyUniqueID & callIdentifier,
      BOOL answeringCall,
      PSafetyMode mode = PSafeReference
    );

    /**Find the call given the identifier.
      */
    virtual PSafePtr<H323GatekeeperCall> FindCall(
      const OpalGloballyUniqueID & callIdentifier,
      H323GatekeeperCall::Direction direction,
      PSafetyMode mode = PSafeReference
    );

    /**Get first endpoint for enumeration.
      */
    PSafePtr<H323GatekeeperCall> GetFirstCall(
      PSafetyMode mode = PSafeReference
    ) { return PSafePtr<H323GatekeeperCall>(activeCalls, mode); }


  //@}

  /**@name Routing operations */
  //@{
    /**Handle a location LRQ PDU.
       The default behaviour just uses TranslateAliasAddressToSignalAddress to
       determine the endpoints location.

       It is expected that a user would override this function to implement
       application specified look up algorithms.
      */
    virtual H323GatekeeperRequest::Response OnLocation(
      H323GatekeeperLRQ & request
    );

    /**Translate a given alias to an alias set and signal address.
       This is called by the OnAdmission() handler to fill in the ACF
       or OnLocation() to fill the LCF informing the calling endpoint where
       to actually connect to.

       It is expected that a user would override this function to implement
       application specified look up algorithms.

       The default behaviour calls TranslateAliasAddressToSignalAddress() which
       is provided only for backwards compatibility.
      */
    virtual BOOL TranslateAliasAddress(
      const H225_AliasAddress & alias,
      H225_ArrayOf_AliasAddress & aliases,
      H323TransportAddress & address,
      BOOL & isGkRouted,
      H323GatekeeperCall * call
    );
    virtual BOOL TranslateAliasAddressToSignalAddress(
      const H225_AliasAddress & alias,
      H323TransportAddress & address
    );

  //@}

  /**@name Policy operations */
  //@{
    /**Check the signal address against the security policy.
       This validates that the specified endpoint is allowed to make a
       connection to or from the specified signal address.

       It is expected that a user would override this function to implement
       application specified security policy algorithms.

       The default behaviour simply returns TRUE.
      */
    virtual BOOL CheckSignalAddressPolicy(
      const H323RegisteredEndPoint & ep,
      const H225_AdmissionRequest & arq,
      const H323TransportAddress & address
    );

    /**Check the alias address against the security policy.
       This validates that the specified endpoint is allowed to make a
       connection to or from the specified alias address.

       It is expected that a user would override this function to implement
       application specified security policy algorithms.

       The default behaviour checks the canOnlyAnswerRegisteredEP or
       canOnlyCallRegisteredEP meber variables depending on if it is an
       incoming call and if that is TRUE only allows the call to proceed
       if the alias is also registered with the gatekeeper.
      */
    virtual BOOL CheckAliasAddressPolicy(
      const H323RegisteredEndPoint & ep,
      const H225_AdmissionRequest & arq,
      const H225_AliasAddress & alias
    );

    /**Check the alias address against the security policy.
       This validates that the specified endpoint is allowed to make a
       connection to or from the specified simple alias string.

       It is expected that a user would override this function to implement
       application specified security policy algorithms.

       The default behaviour checks the canOnlyAnswerRegisteredEP or
       canOnlyCallRegisteredEP meber variables depending on if it is an
       incoming call and if that is TRUE only allows the call to proceed
       if the alias is also registered with the gatekeeper.
      */
    virtual BOOL CheckAliasStringPolicy(
      const H323RegisteredEndPoint & ep,
      const H225_AdmissionRequest & arq,
      const PString & alias
    );

    /**Allocate or change the bandwidth being used.
       This function modifies the total bandwidth used by the all endpoints
       registered with this gatekeeper. It is called when ARQ or BRQ PDU's are
       received.
      */
    virtual unsigned AllocateBandwidth(
      unsigned newBandwidth,
      unsigned oldBandwidth = 0
    );
  //@}

  /**@name Security and authentication functions */
  //@{
    /**Get separate H.235 authentication for the connection.
       This allows an individual ARQ to override the authentical credentials
       used in H.235 based RAS for this particular connection.

       A return value of FALSE indicates to use the default credentials of the
       endpoint, while TRUE indicates that new credentials are to be used.

       The default behavour does nothing and returns FALSE.
      */
    virtual BOOL GetAdmissionRequestAuthentication(
      H323GatekeeperARQ & info,           ///< ARQ being constructed
      H235Authenticators & authenticators ///< New authenticators for ARQ
    );

    /**Get password for user if H.235 security active.
       Returns TRUE if user is found and password returned. Note the password
       may be empty in which case the user was found but explicitly ddoes not
       require security, possibly overriding the requireH235 flag.
      */
    virtual BOOL GetUsersPassword(
      const PString & alias,
      PString & password,
      H323RegisteredEndPoint & registeredEndpoint
    ) const;
    virtual BOOL GetUsersPassword(
      const PString & alias,
      PString & password
    ) const;
  //@}

#ifdef H323_H501

  /**@name H.323 Annex G Peer Element support */
  //@{
    /**Get the associated peer element for the gatekeeper.
      */
    H323PeerElement * GetPeerElement() const { return peerElement; }

    /**Set the associated peer element for the gatekeeper.
       The existing peer element is automatically deleted.
      */
    void SetPeerElement(
      H323PeerElement * newPeerElement
    );

    /**Create an associated peer element for the gatekeeper.
       The existing peer element is checked to be listening on the same
       interface as specified and if it is not it is changed to the specified
       address.
      */
    void CreatePeerElement(
      const H323TransportAddress & h501Interface
    );

    /**Create a new Peer Element and establish a service relationship.
       If append is FALSE, the existing peer element is automatically deleted
       and recreated. If append is TRUE then a new service relationship is
       added to the existing peer element.
      */
    BOOL OpenPeerElement(
      const H323TransportAddress & remotePeer,
      BOOL append = FALSE,
      BOOL keepTrying = TRUE
    );
  //@}

#endif // H323_H501

  /**@name Access functions */
  //@{
    /**Get the identifier name for this gatekeeper.
      */
    const PString & GetGatekeeperIdentifier() const { return gatekeeperIdentifier; }

    /**Set the identifier name for this gatekeeper.
       If adjustListeners is TRUE then all gatekeeper listeners that are
       attached to this gatekeeper server have their identifier names changed
       as well.
      */
    void SetGatekeeperIdentifier(
      const PString & id,
      BOOL adjustListeners = TRUE
    );

    /**Get the total bandwidth available in 100's of bits per second.
      */
    unsigned GetAvailableBandwidth() const { return totalBandwidth; }

    /**Set the total bandwidth available in 100's of bits per second.
      */
    void SetAvailableBandwidth(unsigned bps100) { totalBandwidth = bps100; }

    /**Get the total bandwidth used in 100's of bits per second.
      */
    unsigned GetUsedBandwidth() const { return usedBandwidth; }

    /**Get the default bandwidth for calls.
      */
    unsigned GetDefaultBandwidth() const { return defaultBandwidth; }

    /**Get the default time to live for new registered endpoints.
      */
    unsigned GetTimeToLive() const { return defaultTimeToLive; }

    /**Set the default time to live for new registered endpoints.
      */
    void SetTimeToLive(unsigned seconds) { defaultTimeToLive = seconds; }

    /**Get the default time for monitoring calls via IRR.
      */
    unsigned GetInfoResponseRate() const { return defaultInfoResponseRate; }

    /**Set the default time for monitoring calls via IRR.
      */
    void SetInfoResponseRate(unsigned seconds) { defaultInfoResponseRate = seconds; }

    /**Get flag for is gatekeeper routed.
      */
    BOOL IsGatekeeperRouted() const { return isGatekeeperRouted; }

    /**Get flag for if H.235 authentication is required.
      */
    BOOL IsRequiredH235() const { return requireH235; }

    /**Get the currently active registration count.
      */
    unsigned GetActiveRegistrations() const { return byIdentifier.GetSize(); }

    /**Get the peak registration count.
      */
    unsigned GetPeakRegistrations() const { return peakRegistrations; }

    /**Get the total registrations since start up.
      */
    unsigned GetTotalRegistrations() const { return totalRegistrations; }

    /**Get the total registrations rejected since start up.
      */
    unsigned GetRejectedRegistrations() const { return rejectedRegistrations; }

    /**Get the currently active call count.
      */
    unsigned GetActiveCalls() const { return activeCalls.GetSize(); }

    /**Get the peak calls count.
      */
    unsigned GetPeakCalls() const { return peakCalls; }

    /**Get the total calls since start up.
      */
    unsigned GetTotalCalls() const { return totalCalls; }

    /**Get the total calls rejected since start up.
      */
    unsigned GetRejectedCalls() const { return rejectedCalls; }
  //@}

    // Remove an alias from the server database.
    void RemoveAlias(
      H323RegisteredEndPoint & ep,
      const PString & alias
    );

#ifdef H323_H501
    // called when an endpoint needs to send a descriptor to the H.501 peer element
    virtual BOOL OnSendDescriptorForEndpoint(
      H323RegisteredEndPoint & /*ep*/,                    ///< endpoint
      H225_ArrayOf_AliasAddress & /*aliases*/,            ///< aliases for the enndpoint
      H225_EndpointType & /*terminalType*/,               ///< terminal type
      H225_ArrayOf_AliasAddress & /*transportAddresses*/  ///< transport addresses
    )
    { return TRUE; } 
#endif

    virtual BOOL AllowDuplicateAlias(const H225_ArrayOf_AliasAddress & /*aliases*/)
    { return canHaveDuplicateAlias; }

    virtual BOOL OnSendFeatureSet(unsigned, H225_FeatureSet & features) const;
    virtual void OnReceiveFeatureSet(unsigned, const H225_FeatureSet & features) const;

  protected:

    PDECLARE_NOTIFIER(PThread, H323GatekeeperServer, MonitorMain);

    // Configuration & policy variables
    PString  gatekeeperIdentifier;
    unsigned totalBandwidth;
    unsigned usedBandwidth;
    unsigned defaultBandwidth;
    unsigned maximumBandwidth;
    unsigned defaultTimeToLive;
    unsigned defaultInfoResponseRate;
    BOOL     overwriteOnSameSignalAddress;
    BOOL     canHaveDuplicateAlias;
    BOOL     canHaveDuplicatePrefix;
    BOOL     canOnlyCallRegisteredEP;
    BOOL     canOnlyAnswerRegisteredEP;
    BOOL     answerCallPreGrantedARQ;
    BOOL     makeCallPreGrantedARQ;
    BOOL     isGatekeeperRouted;
    BOOL     aliasCanBeHostName;
    BOOL     requireH235;
    BOOL     disengageOnHearbeatFail;

    PStringToString passwords;

    // Dynamic variables
    PMutex         mutex;
    time_t         identifierBase;
    unsigned       nextIdentifier;
    PThread      * monitorThread;
    PSyncPoint     monitorExit;

    PLIST(ListenerList, H323GatekeeperListener);
    ListenerList listeners;

    H323PeerElement * peerElement;

    PSafeDictionary<PString, H323RegisteredEndPoint> byIdentifier;

    class StringMap : public PString {
        PCLASSINFO(StringMap, PString);
      public:
        StringMap(const PString & from, const PString & id)
          : PString(from), identifier(id) { }
        PString identifier;
    };
    PSortedStringList byAddress;
    PSortedStringList byAlias;
    PSortedStringList byVoicePrefix;

    PSafeSortedList<H323GatekeeperCall> activeCalls;

    PINDEX peakRegistrations;
    PINDEX totalRegistrations;
    PINDEX rejectedRegistrations;
    PINDEX peakCalls;
    PINDEX totalCalls;
    PINDEX rejectedCalls;

  friend class H323GatekeeperRRQ;
  friend class H323GatekeeperARQ;
};


#endif // __OPAL_GKSERVER_H


/////////////////////////////////////////////////////////////////////////////

