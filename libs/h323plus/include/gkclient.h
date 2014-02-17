/*
 * gkclient.h
 *
 * Gatekeeper client protocol handler
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
 * Portions of this code were written with the assisance of funding from
 * iFace, Inc. http://www.iface.com
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: gkclient.h,v $
 * Revision 1.4  2008/01/29 04:33:24  shorne
 * Added SendServiceControlIndication - fixed Endpoint initiation with H.460
 *
 * Revision 1.3  2008/01/04 06:23:07  shorne
 * Cleaner setup and teardown of h460 module
 *
 * Revision 1.2  2007/10/16 16:54:32  shorne
 * Fix for H.235.1 on full reregistration
 *
 * Revision 1.1  2007/08/06 20:50:48  shorne
 * First commit of h323plus
 *
 * Revision 1.58.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.58  2006/05/30 11:14:55  hfriederich
 * Switch from DISABLE_H460 to H323_H460
 *
 * Revision 1.57  2006/05/18 17:16:34  shorne
 * Added H460 Support
 *
 * Revision 1.56  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.55  2005/01/03 14:03:19  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.54  2005/01/03 06:25:52  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.53  2004/09/03 01:06:08  csoutheren
 * Added initial hooks for H.460 GEF
 * Thanks to Simon Horne and ISVO (Asia) Pte Ltd. for this contribution
 *
 * Revision 1.52  2004/03/04 04:49:34  csoutheren
 * Added access function for gatekeeper routed address
 *
 * Revision 1.51  2003/03/26 00:46:25  robertj
 * Had another go at making H323Transactor being able to be created
 *   without having a listener running.
 *
 * Revision 1.50  2003/02/12 23:59:22  robertj
 * Fixed adding missing endpoint identifer in SETUP packet when gatekeeper
 * routed, pointed out by Stefan Klein
 * Also fixed correct rutrn of gk routing in IRR packet.
 *
 * Revision 1.49  2003/02/07 06:37:42  robertj
 * Changed registration state to an enum so can determine why the RRQ failed.
 *
 * Revision 1.48  2003/01/06 07:09:28  robertj
 * Further fixes for alternate gatekeeper, thanks Kevin Bouchard
 *
 * Revision 1.47  2002/12/23 22:46:38  robertj
 * Changed gatekeeper discovery so an GRJ does not indicate "discovered".
 * Added trace output of alternate gatekeepers list.
 *
 * Revision 1.46  2002/11/28 04:41:44  robertj
 * Added support for RAS ServiceControlIndication command.
 *
 * Revision 1.45  2002/11/27 06:54:52  robertj
 * Added Service Control Session management as per Annex K/H.323 via RAS
 *   only at this stage.
 * Added H.248 ASN and very primitive infrastructure for linking into the
 *   Service Control Session management system.
 * Added basic infrastructure for Annex K/H.323 HTTP transport system.
 * Added Call Credit Service Control to display account balances.
 *
 * Revision 1.44  2002/11/21 07:21:46  robertj
 * Improvements to alternate gatekeeper client code, thanks Kevin Bouchard
 *
 * Revision 1.43  2002/09/18 06:58:29  robertj
 * Fixed setting of IRR frequency, an RCF could reset timer so it did not time
 *   out correctly and send IRR in time causing problems with gatekeeper.
 *
 * Revision 1.42  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.41  2002/09/03 06:19:36  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.40  2002/08/15 04:13:21  robertj
 * Fixed correct status of isRegistered flag on various reject/errors.
 *
 * Revision 1.39  2002/08/12 05:38:20  robertj
 * Changes to the RAS subsystem to support ability to make requests to client
 *   from gkserver without causing bottlenecks and race conditions.
 *
 * Revision 1.38  2002/08/05 05:17:36  robertj
 * Fairly major modifications to support different authentication credentials
 *   in ARQ to the logged in ones on RRQ. For both client and server.
 * Various other H.235 authentication bugs and anomalies fixed on the way.
 *
 * Revision 1.37  2002/07/18 03:03:38  robertj
 * Fixed bug with continually doing lightweight RRQ if no timeToLive present
 *   and it should not be doing it at all, ditto for unsolicited IRR.
 *
 * Revision 1.36  2002/07/16 11:06:21  robertj
 * Added more alternate gatekeeper implementation, thanks Kevin Bouchard
 *
 * Revision 1.35  2002/06/26 03:47:45  robertj
 * Added support for alternate gatekeepers.
 *
 * Revision 1.34  2002/05/29 00:03:15  robertj
 * Fixed unsolicited IRR support in gk client and server,
 *   including support for IACK and INAK.
 *
 * Revision 1.33  2002/05/17 04:12:38  robertj
 * Added support for unsolicited IRR transmission in background (heartbeat).
 *
 * Revision 1.32  2002/03/19 05:17:11  robertj
 * Normalised ACF destExtraCallIInfo to be same as other parameters.
 * Added ability to get multiple endpoint addresses and tokens from ACF.
 *
 * Revision 1.31  2001/09/26 07:02:49  robertj
 * Added needed mutex for SeparateAuthenticationInARQ mode, thanks Nick Hoath
 *
 * Revision 1.30  2001/09/18 10:36:54  robertj
 * Allowed multiple overlapping requests in RAS channel.
 *
 * Revision 1.29  2001/09/13 03:21:16  robertj
 * Added ability to override authentication credentials for ARQ, thanks Nick Hoath
 *
 * Revision 1.28  2001/09/12 06:57:58  robertj
 * Added support for iNow Access Token from gk, thanks Nick Hoath
 *
 * Revision 1.27  2001/09/12 06:04:36  robertj
 * Added support for sending UUIE's to gk on request, thanks Nick Hoath
 *
 * Revision 1.26  2001/09/06 02:32:26  robertj
 * Added overloaded AdmissionRequest for backward compatibility.
 *
 * Revision 1.25  2001/08/14 04:26:46  robertj
 * Completed the Cisco compatible MD5 authentications, thanks Wolfgang Platzer.
 *
 * Revision 1.24  2001/08/13 01:27:00  robertj
 * Changed GK admission so can return multiple aliases to be used in
 *   setup packet, thanks Nick Hoath.
 *
 * Revision 1.23  2001/08/10 11:03:49  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 * Revision 1.22  2001/08/06 07:44:52  robertj
 * Fixed problems with building without SSL
 *
 * Revision 1.21  2001/08/06 03:18:35  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 * Improved access to H.235 secure RAS functionality.
 * Changes to H.323 secure RAS contexts to help use with gk server.
 *
 * Revision 1.20  2001/08/02 04:30:09  robertj
 * Added ability for AdmissionRequest to alter destination alias used in
 *   the outgoing call.
 *
 * Revision 1.19  2001/06/18 06:23:47  robertj
 * Split raw H.225 RAS protocol out of gatekeeper client class.
 *
 * Revision 1.18  2001/04/05 03:39:42  robertj
 * Fixed deadlock if tried to do discovery in time to live timeout.
 *
 * Revision 1.17  2001/03/28 07:12:56  robertj
 * Changed RAS thread interlock to allow for what should not happen, the
 *   syncpoint being signalled before receiving any packets.
 *
 * Revision 1.16  2001/03/27 02:18:41  robertj
 * Changed to send gk a GRQ if it gives a discoveryRequired error on RRQ.
 *
 * Revision 1.15  2001/03/17 00:05:52  robertj
 * Fixed problems with Gatekeeper RIP handling.
 *
 * Revision 1.14  2001/02/28 00:20:15  robertj
 * Added DiscoverByNameAndAddress() function, thanks Chris Purvis.
 *
 * Revision 1.13  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.12  2000/09/25 06:47:54  robertj
 * Removed use of alias if there is no alias present, ie only have transport address.
 *
 * Revision 1.11  2000/09/01 02:12:54  robertj
 * Added ability to select a gatekeeper on LAN via it's identifier name.
 *
 * Revision 1.10  2000/07/11 19:20:02  robertj
 * Fixed problem with endpoint identifiers from some gatekeepers not being a string, just binary info.
 *
 * Revision 1.9  2000/06/20 03:17:56  robertj
 * Added function to get name of gatekeeper, subtle difference from getting identifier.
 *
 * Revision 1.8  2000/05/18 11:53:33  robertj
 * Changes to support doc++ documentation generation.
 *
 * Revision 1.7  2000/05/09 08:52:36  robertj
 * Added support for preGrantedARQ fields on registration.
 *
 * Revision 1.6  2000/05/02 04:32:24  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.5  2000/04/11 03:10:40  robertj
 * Added ability to reject reason on gatekeeper requests.
 * Added ability to get the transport being used to talk to the gatekeeper.
 *
 * Revision 1.4  2000/04/10 17:37:13  robertj
 * Added access function to get the gatekeeper identification string.
 *
 * Revision 1.3  1999/12/09 21:49:17  robertj
 * Added reregister on unregister and time to live reregistration
 *
 * Revision 1.2  1999/09/14 06:52:54  robertj
 * Added better support for multi-homed client hosts.
 *
 * Revision 1.1  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 */

#ifndef __OPAL_GKCLIENT_H
#define __OPAL_GKCLIENT_H

#include "h323.h"
#include "h225ras.h"
#include "h235auth.h"

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifdef H323_H460
class H460_FeatureSet;
#endif

class H225_ArrayOf_AliasAddress;
class H225_H323_UU_PDU;
class H225_AlternateGK;
class H225_ArrayOf_AlternateGK;
class H225_ArrayOf_ServiceControlSession;


///////////////////////////////////////////////////////////////////////////////

/**This class embodies the H.225.0 RAS protocol to gatekeepers.
  */
class H323Gatekeeper : public H225_RAS
{
    PCLASSINFO(H323Gatekeeper, H225_RAS);
  public:
  /**@name Construction */
  //@{
    /**Create a new gatekeeper.
     */
    H323Gatekeeper(
      H323EndPoint & endpoint,  ///< Endpoint gatekeeper is associated with.
      H323Transport * transport       ///< Transport over which gatekeepers communicates.
    );

    /**Destroy gatekeeper.
     */
    ~H323Gatekeeper();
  //@}

  /**@name Overrides from H225_RAS */
  //@{
    BOOL OnReceiveGatekeeperConfirm(const H225_GatekeeperConfirm & gcf);
    BOOL OnReceiveGatekeeperReject(const H225_GatekeeperReject & grj);
    BOOL OnReceiveRegistrationConfirm(const H225_RegistrationConfirm & rcf);
    BOOL OnReceiveRegistrationReject(const H225_RegistrationReject & rrj);
    BOOL OnReceiveUnregistrationRequest(const H225_UnregistrationRequest & urq);
    BOOL OnReceiveUnregistrationConfirm(const H225_UnregistrationConfirm & ucf);
    BOOL OnReceiveUnregistrationReject(const H225_UnregistrationReject & urj);
    BOOL OnReceiveAdmissionConfirm(const H225_AdmissionConfirm & acf);
    BOOL OnReceiveAdmissionReject(const H225_AdmissionReject & arj);
    BOOL OnReceiveDisengageRequest(const H225_DisengageRequest & drq);
    BOOL OnReceiveBandwidthConfirm(const H225_BandwidthConfirm & bcf);
    BOOL OnReceiveBandwidthRequest(const H225_BandwidthRequest & brq);
    BOOL OnReceiveInfoRequest(const H225_InfoRequest & irq);

#ifdef H323_H248
    BOOL OnReceiveServiceControlIndication(const H225_ServiceControlIndication &);
#endif

    void OnSendGatekeeperRequest(H225_GatekeeperRequest & grq);
    void OnSendAdmissionRequest(H225_AdmissionRequest & arq);
#ifdef H323_H248
    BOOL SendServiceControlIndication();
#endif
    BOOL OnSendFeatureSet(unsigned, H225_FeatureSet & features) const;
    void OnReceiveFeatureSet(unsigned, const H225_FeatureSet & features) const;
  //@}

  /**@name Protocol operations */
  //@{
    /**Discover a gatekeeper on the local network.
     */
    BOOL DiscoverAny();

    /**Discover a gatekeeper on the local network.
       If the identifier string is empty then the first gatekeeper to respond
       to a broadcast is used.
     */
    BOOL DiscoverByName(
      const PString & identifier  ///< Gatekeeper identifier to find
    );

    /**Discover a gatekeeper on the local network.
       If the address string is empty then the first gatekeeper to respond
       to a broadcast is used.
     */
    BOOL DiscoverByAddress(
      const H323TransportAddress & address ///< Address of gatekeeper.
    );

    /**Discover a gatekeeper on the local network.
       Combination of DiscoverByName() and DiscoverByAddress().
     */
    BOOL DiscoverByNameAndAddress(
      const PString & identifier,
      const H323TransportAddress & address
    );

    /**Register with gatekeeper.
     */
    BOOL RegistrationRequest(
      BOOL autoReregister = TRUE  ///< Automatic register on unregister
    );

    /**Unregister with gatekeeper.
     */
    BOOL UnregistrationRequest(
      int reason      ///< Reason for unregistration
    );

    /**Location request to gatekeeper.
     */
    BOOL LocationRequest(
      const PString & alias,          ///< Alias name we wish to find.
      H323TransportAddress & address  ///< Resultant transport address.
    );

    /**Location request to gatekeeper.
     */
    BOOL LocationRequest(
      const PStringList & aliases,    ///< Alias names we wish to find.
      H323TransportAddress & address  ///< Resultant transport address.
    );

    struct AdmissionResponse {
      AdmissionResponse();

      unsigned rejectReason;                      ///< Reject reason if returns FALSE

      BOOL gatekeeperRouted;                      ///< Flag for call is through gk
      PINDEX endpointCount;                       ///< Number of endpoints that can be returned
      H323TransportAddress * transportAddress;    ///< Transport address or remote endpoint.
      PBYTEArray * accessTokenData;               ///< iNow Gatekeeper Access Token data

      H225_ArrayOf_AliasAddress * aliasAddresses; ///< DestinationInfo to use in SETUP if not empty
      H225_ArrayOf_AliasAddress * destExtraCallInfo; ///< DestinationInfo to use in SETUP if not empty
    };

    /**Admission request to gatekeeper.
     */
    BOOL AdmissionRequest(
      H323Connection & connection,      ///< Connection we wish to change.
      AdmissionResponse & response,     ///< Response parameters to ARQ
      BOOL ignorePreGrantedARQ = FALSE  ///< Flag to force ARQ to be sent
    );

    /**Disengage request to gatekeeper.
     */
    BOOL DisengageRequest(
      const H323Connection & connection,  ///< Connection we wish admitted.
      unsigned reason                     ///< Reason code for disengage
    );

    /**Bandwidth request to gatekeeper.
     */
    BOOL BandwidthRequest(
      H323Connection & connection,    ///< Connection we wish to change.
      unsigned requestedBandwidth     ///< New bandwidth wanted in 0.1kbps
    );

    /**Send an unsolicited info response to the gatekeeper.
     */
    void InfoRequestResponse();

    /**Send an unsolicited info response to the gatekeeper.
     */
    void InfoRequestResponse(
      const H323Connection & connection  ///< Connection to send info about
    );

    /**Send an unsolicited info response to the gatekeeper.
     */
    void InfoRequestResponse(
      const H323Connection & connection,  ///< Connection to send info about
      const H225_H323_UU_PDU & pdu,       ///< PDU that was sent or received
      BOOL sent                           ///< Flag for PDU was sent or received
    );

#ifdef H323_H248
    /**Handle incoming service control session information.
     */
    virtual void OnServiceControlSessions(
      const H225_ArrayOf_ServiceControlSession & serviceControl,
      H323Connection * connection
    );
#endif
  //@}

  /**@name Member variable access */
  //@{
    /**Determine if the endpoint has discovered the gatekeeper.
      */
    BOOL IsDiscoveryComplete() const { return discoveryComplete; }

    /**Determine if the endpoint is registered with the gatekeeper.
      */
    BOOL IsRegistered() const { return registrationFailReason == RegistrationSuccessful; }

	/** Force the client to reregister with gatekeeper (used with H.460 features)
	  */
    void ReRegisterNow();

    enum RegistrationFailReasons {
      RegistrationSuccessful,
      UnregisteredLocally,
      UnregisteredByGatekeeper,
      GatekeeperLostRegistration,
      InvalidListener,
      DuplicateAlias,
      SecurityDenied,
      TransportError,
      NumRegistrationFailReasons,
      RegistrationRejectReasonMask = 0x8000
    };
    /**Get the registration fail reason.
     */
    RegistrationFailReasons GetRegistrationFailReason() const { return registrationFailReason; }

    /**Get the gatekeeper name.
       The gets the name of the gatekeeper. It will be of the form id@address
       where id is the gatekeeperIdentifier and address is the transport
       address used. If the gatekeeperIdentifier is empty the '@' is not
       included and only the transport is shown. The transport is minimised
       also, with the type removed if IP is used and the :port removed if the
       default port is used.
      */
    PString GetName() const;

    /** Get the endpoint identifier
      */
    const PString & GetEndpointIdentifier() const { return endpointIdentifier; }

    /**Set the H.235 password in the gatekeeper.
       If no username is present then it will default to the endpoint local
       user name (ie first alias).
      */
    void SetPassword(
      const PString & password,            ///< New password
      const PString & username = PString() ///< Username for password
    );

    /*
     * Return the call signalling address for the gatekeeper (if present)
     */
    H323TransportAddress GetGatekeeperRouteAddress() const
    { return gkRouteAddress; }
  //@}
	

  protected:
    BOOL StartDiscovery(const H323TransportAddress & address);
    unsigned SetupGatekeeperRequest(H323RasPDU & request);
	
    void Connect(const H323TransportAddress & address, const PString & gatekeeperIdentifier);
    PDECLARE_NOTIFIER(PThread, H323Gatekeeper, MonitorMain);
    PDECLARE_NOTIFIER(PTimer, H323Gatekeeper, TickleMonitor);
    void RegistrationTimeToLive();

    void SetInfoRequestRate(
      const PTimeInterval & rate
    );
    void ClearInfoRequestRate();
    H225_InfoRequestResponse & BuildInfoRequestResponse(
      H323RasPDU & response,
      unsigned seqNum
    );
    BOOL SendUnsolicitedIRR(
      H225_InfoRequestResponse & irr,
      H323RasPDU & response
    );

    void SetAlternates(
      const H225_ArrayOf_AlternateGK & alts,
      BOOL permanent
    );

	void SetAssignedGatekeeper(
	    const H225_AlternateGK & gk
	);

	BOOL GetAssignedGatekeeper(
	    H225_AlternateGK & gk
	);

    virtual BOOL MakeRequest(
      Request & request
    );
    BOOL MakeRequestWithReregister(
      Request & request,
      unsigned unregisteredTag
    );


    // Gatekeeper registration state variables
    BOOL     discoveryComplete;
    PString  endpointIdentifier;
	PString  localId;
    RegistrationFailReasons registrationFailReason;

    class AlternateInfo : public PObject {
        PCLASSINFO(AlternateInfo, PObject);
      public:
        AlternateInfo(const H225_AlternateGK & alt);
	    ~AlternateInfo();
		H225_AlternateGK GetAlternate();
        Comparison Compare(const PObject & obj);
        void PrintOn(ostream & strm) const;

        H323TransportAddress rasAddress;
        PString              gatekeeperIdentifier;
        unsigned             priority;
        enum {
          NoRegistrationNeeded,
          NeedToRegister,
          Register,
          IsRegistered,
          RegistrationFailed
        } registrationState;

      private:
        // Disable copy constructor and assignment
        AlternateInfo(const AlternateInfo &) { }
        AlternateInfo & operator=(const AlternateInfo &) { return *this; }
    };
    PSortedList<AlternateInfo> alternates;
    BOOL               alternatePermanent;
    PSemaphore         requestMutex;
    H235Authenticators authenticators;
	AlternateInfo *    assignedGK;

    enum {
      RequireARQ,
      PregrantARQ,
      PreGkRoutedARQ
    } pregrantMakeCall, pregrantAnswerCall;
    H323TransportAddress gkRouteAddress;

    // Gatekeeper operation variables
    BOOL       autoReregister;
    BOOL       reregisterNow;
    PTimer     timeToLive;
    BOOL       requiresDiscovery;
    PTimer     infoRequestRate;
    BOOL       willRespondToIRR;
    PThread  * monitor;
    BOOL       monitorStop;
    PSyncPoint monitorTickle;

#ifdef H323_H248
    PDictionary<POrdinalKey, H323ServiceControlSession> serviceControlSessions;
#endif

#ifdef H323_H460
    H460_FeatureSet     * features;
#endif

};


PLIST(H323GatekeeperList, H323Gatekeeper);



#endif // __OPAL_GKCLIENT_H


/////////////////////////////////////////////////////////////////////////////
