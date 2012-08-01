/*
 * h323pdu.h
 *
 * H.323 protocol handler
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
 * Vovida Networks, Inc. http://www.vovida.com.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h323pdu.h,v $
 * Revision 1.3  2007/10/19 19:53:44  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.2  2007/10/16 17:08:01  shorne
 * Allow cryptoTokens to be insert in the setup after receiving ARQ
 *
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.75.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.75  2006/07/17 09:52:57  shorne
 * Added more support URL alias Addresses
 *
 * Revision 1.74  2006/06/21 04:53:32  csoutheren
 * Tweaked H.245 version 13 updates
 *
 * Revision 1.73  2006/06/20 05:24:40  csoutheren
 * Additional tweaks for H.225v6
 *
 * Revision 1.72  2006/01/26 03:28:10  shorne
 * Caller Authentication added
 *
 * Revision 1.71  2006/01/20 00:32:24  csoutheren
 * First check-in of signalling aggregation code - incomplete and disabled by default
 *
 * Revision 1.70  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.69  2005/06/30 01:57:39  csoutheren
 * Updated to H.245v11
 *
 * Revision 1.68  2005/03/04 03:21:20  csoutheren
 * Added local and remote addresses to all PDU logs to assist in debugging
 *
 * Revision 1.67  2005/01/24 00:10:48  csoutheren
 * Added function to set Q.931 info in PDU, thanks to Paul Nader
 *
 * Revision 1.66  2004/12/14 06:22:21  csoutheren
 * More OSP implementation
 *
 * Revision 1.65  2003/04/10 09:36:52  robertj
 * Added some more functions for converting to alias addresses.
 *
 * Revision 1.64  2003/04/01 03:11:01  robertj
 * Added function to get array of AliasAddress into PStringArray.
 *
 * Revision 1.63  2003/03/25 04:56:17  robertj
 * Fixed issues to do with multiple inheritence in transaction reply cache.
 *
 * Revision 1.62  2003/03/20 01:51:07  robertj
 * More abstraction of H.225 RAS and H.501 protocols transaction handling.
 *
 * Revision 1.61  2003/03/01 00:23:42  craigs
 * New PeerElement implementation
 *
 * Revision 1.60  2003/02/25 06:48:15  robertj
 * More work on PDU transaction abstraction.
 *
 * Revision 1.59  2003/02/21 05:28:39  craigs
 * Factored out code for user with peer elements
 *
 * Revision 1.58  2003/02/01 13:31:14  robertj
 * Changes to support CAT authentication in RAS.
 *
 * Revision 1.57  2002/11/28 04:41:44  robertj
 * Added support for RAS ServiceControlIndication command.
 *
 * Revision 1.56  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.55  2002/09/03 06:19:37  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.54  2002/08/12 05:38:21  robertj
 * Changes to the RAS subsystem to support ability to make requests to client
 *   from gkserver without causing bottlenecks and race conditions.
 *
 * Revision 1.53  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.52  2002/08/05 05:17:37  robertj
 * Fairly major modifications to support different authentication credentials
 *   in ARQ to the logged in ones on RRQ. For both client and server.
 * Various other H.235 authentication bugs and anomalies fixed on the way.
 *
 * Revision 1.51  2002/07/25 10:55:40  robertj
 * Changes to allow more granularity in PDU dumps, hex output increasing
 *   with increasing trace level.
 *
 * Revision 1.50  2002/07/11 07:04:12  robertj
 * Added build InfoRequest pdu type to RAS.
 *
 * Revision 1.49  2002/05/29 03:55:17  robertj
 * Added protocol version number checking infrastructure, primarily to improve
 *   interoperability with stacks that are unforgiving of new features.
 *
 * Revision 1.48  2002/05/29 00:03:15  robertj
 * Fixed unsolicited IRR support in gk client and server,
 *   including support for IACK and INAK.
 *
 * Revision 1.47  2002/05/07 03:18:12  robertj
 * Added application info (name/version etc) into registered endpoint data.
 *
 * Revision 1.46  2002/05/03 09:18:45  robertj
 * Added automatic retransmission of RAS responses to retried requests.
 *
 * Revision 1.45  2002/03/14 07:57:02  robertj
 * Added ability to specify alias type in H323SetAliasAddress, if not specified
 *   then defaults to previous behaviour, thanks Nils Bokerman.
 *
 * Revision 1.44  2001/12/15 07:09:56  robertj
 * Added functions to get E.164 address from alias address or addresses.
 *
 * Revision 1.43  2001/12/14 06:38:47  robertj
 * Broke out conversion of Q.850 and H.225 release complete codes to
 *   OpenH323 call end reasons enum.
 *
 * Revision 1.42  2001/12/13 10:56:04  robertj
 * Added build of request in progress pdu.
 *
 * Revision 1.41  2001/08/16 07:49:16  robertj
 * Changed the H.450 support to be more extensible. Protocol handlers
 *   are now in separate classes instead of all in H323Connection.
 *
 * Revision 1.40  2001/08/10 11:03:49  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 * Revision 1.39  2001/08/06 07:44:52  robertj
 * Fixed problems with building without SSL
 *
 * Revision 1.38  2001/08/06 03:08:11  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.37  2001/06/14 06:25:13  robertj
 * Added further H.225 PDU build functions.
 * Moved some functionality from connection to PDU class.
 *
 * Revision 1.36  2001/06/14 00:45:19  robertj
 * Added extra parameters for Q.931 fields, thanks Rani Assaf
 *
 * Revision 1.35  2001/05/30 23:34:54  robertj
 * Added functions to send TCS=0 for transmitter side pause.
 *
 * Revision 1.34  2001/04/11 03:01:27  robertj
 * Added H.450.2 (call transfer), thanks a LOT to Graeme Reid & Norwood Systems
 *
 * Revision 1.33  2001/03/24 00:34:35  robertj
 * Added read/write hook functions so don't have to duplicate code in
 *    H323RasH235PDU descendant class of H323RasPDU.
 *
 * Revision 1.32  2001/03/21 04:52:40  robertj
 * Added H.235 security to gatekeepers, thanks Fürbass Franz!
 *
 * Revision 1.31  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.30  2001/01/19 01:20:38  robertj
 * Added non-const function to get access to Q.931 PDU in H323SignalPDU.
 *
 * Revision 1.29  2000/10/04 05:59:09  robertj
 * Minor reorganisation of the H.245 secondary channel start up to make it simpler
 *    to override its behaviour.
 *
 * Revision 1.28  2000/09/25 06:47:54  robertj
 * Removed use of alias if there is no alias present, ie only have transport address.
 *
 * Revision 1.27  2000/09/22 01:35:02  robertj
 * Added support for handling LID's that only do symmetric codecs.
 *
 * Revision 1.26  2000/07/15 09:50:49  robertj
 * Changed adding of Q.931 party numbers to only occur in SETUP.
 *
 * Revision 1.25  2000/06/21 08:07:39  robertj
 * Added cause/reason to release complete PDU, where relevent.
 *
 * Revision 1.24  2000/05/23 11:32:27  robertj
 * Rewrite of capability table to combine 2 structures into one and move functionality into that class
 *    allowing some normalisation of usage across several applications.
 * Changed H323Connection so gets a copy of capabilities instead of using endponts, allows adjustments
 *    to be done depending on the remote client application.
 *
 * Revision 1.23  2000/05/08 14:07:26  robertj
 * Improved the provision and detection of calling and caller numbers, aliases and hostnames.
 *
 * Revision 1.22  2000/05/08 05:05:43  robertj
 * Fixed bug in H.245 close logical channel timeout, thanks XuPeili.
 *
 * Revision 1.21  2000/05/02 04:32:24  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.20  2000/04/10 20:39:30  robertj
 * Added support for more sophisticated DTMF and hook flash user indication.
 * Added function to extract E164 address from Q.931/H.225 PDU.
 *
 * Revision 1.19  2000/03/25 02:00:39  robertj
 * Added adjustable caller name on connection by connection basis.
 *
 * Revision 1.18  2000/03/21 01:22:01  robertj
 * Fixed incorrect call reference code being used in originated call.
 *
 * Revision 1.17  1999/12/11 02:20:58  robertj
 * Added ability to have multiple aliases on local endpoint.
 *
 * Revision 1.16  1999/09/10 03:36:48  robertj
 * Added simple Q.931 Status response to Q.931 Status Enquiry
 *
 * Revision 1.15  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 * Revision 1.14  1999/08/25 05:07:49  robertj
 * File fission (critical mass reached).
 *
 * Revision 1.13  1999/07/16 06:15:59  robertj
 * Corrected semantics for tunnelled master/slave determination in fast start.
 *
 * Revision 1.12  1999/07/16 02:15:30  robertj
 * Fixed more tunneling problems.
 *
 * Revision 1.11  1999/07/15 14:45:35  robertj
 * Added propagation of codec open error to shut down logical channel.
 * Fixed control channel start up bug introduced with tunnelling.
 *
 * Revision 1.10  1999/07/10 02:51:53  robertj
 * Added mutexing in H245 procedures.
 *
 * Revision 1.9  1999/07/09 06:09:49  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.8  1999/06/25 10:25:35  robertj
 * Added maintentance of callIdentifier variable in H.225 channel.
 *
 * Revision 1.7  1999/06/14 05:15:56  robertj
 * Changes for using RTP sessions correctly in H323 Logical Channel context
 *
 * Revision 1.6  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.5  1999/06/09 05:26:20  robertj
 * Major restructuring of classes.
 *
 * Revision 1.4  1999/06/06 06:06:36  robertj
 * Changes for new ASN compiler and v2 protocol ASN files.
 *
 * Revision 1.3  1999/04/26 06:14:47  craigs
 * Initial implementation for RTP decoding and lots of stuff
 * As a whole, these changes are called "First Noise"
 *
 * Revision 1.2  1999/01/16 02:35:04  robertj
 * GNi compiler compatibility.
 *
 * Revision 1.1  1999/01/16 01:30:58  robertj
 * Initial revision
 *
 */

#ifndef __OPAL_H323PDU_H
#define __OPAL_H323PDU_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include <ptlib/sockets.h>
#include "h323con.h"
#include "transports.h"
#include "q931.h"
#include "h225.h"
#include "h245.h"
#include "h323trans.h"


class H225_RAS;


#define H225_PROTOCOL_VERSION 6
#define H245_PROTOCOL_VERSION 13


///////////////////////////////////////////////////////////////////////////////

/**Wrapper class for the H323 signalling channel.
 */
class H323SignalPDU : public H225_H323_UserInformation
{
  PCLASSINFO(H323SignalPDU, H225_H323_UserInformation);

  public:
  /**@name Construction */
  //@{
    /**Create a new H.323 signalling channel (H.225/Q.931) PDU.
     */
    H323SignalPDU();

    /**Build a SETUP message.
      */
    H225_Setup_UUIE & BuildSetup(
      const H323Connection & connection,    ///< Connection PDU is generated for
      const H323TransportAddress & destAddr ///< Destination address for packet
    );

    /**Build a CALL-PROCEEDING message.
      */
    H225_CallProceeding_UUIE & BuildCallProceeding(
      const H323Connection & connection    ///< Connection PDU is generated for
    );

    /**Build a CONNECT message.
      */
    H225_Connect_UUIE & BuildConnect(
      const H323Connection & connection    ///< Connection PDU is generated for
    );

    /**Build a CONNECT message with H.245 address.
      */
    H225_Connect_UUIE & BuildConnect(
      const H323Connection & connection,    ///< Connection PDU is generated for
      const PIPSocket::Address & h245Address, ///< H.245 IP address
      WORD port                               ///< H.245 TCP port
    );

    /**Build an ALERTING message.
      */
    H225_Alerting_UUIE & BuildAlerting(
      const H323Connection & connection    ///< Connection PDU is generated for
    );

    /**Build a INFORMATION message.
      */
    H225_Information_UUIE & BuildInformation(
      const H323Connection & connection    ///< Connection PDU is generated for
    );

    /**Build a RELEASE-COMPLETE message.
      */
    H225_ReleaseComplete_UUIE & BuildReleaseComplete(
      const H323Connection & connection    ///< Connection PDU is generated for
    );

    /**Build a FACILITY message.
      */
    H225_Facility_UUIE * BuildFacility(
      const H323Connection & connection,  ///< Connection PDU is generated for
      BOOL empty,                         ///< Flag for empty facility message
	  unsigned reason = H225_FacilityReason::e_undefinedReason ///< Reason for Facility
    );

    /**Build a PROGRESS message.
      */
    H225_Progress_UUIE & BuildProgress(
      const H323Connection & connection    ///< Connection PDU is generated for
    );

    /**Build a STATUS message.
      */
    H225_Status_UUIE & BuildStatus(
      const H323Connection & connection    ///< Connection PDU is generated for
    );

    /**Build a STATUS-INQUIRY message.
      */
    H225_StatusInquiry_UUIE & BuildStatusInquiry(
      const H323Connection & connection    ///< Connection PDU is generated for
    );

    /**Build a SETUP-ACKNOWLEDGE message.
      */
    H225_SetupAcknowledge_UUIE & BuildSetupAcknowledge(
      const H323Connection & connection    ///< Connection PDU is generated for
    );

    /**Build a NOTIFY message.
      */
    H225_Notify_UUIE & BuildNotify(
      const H323Connection & connection    ///< Connection PDU is generated for
    );
  //@}


  /**@name Operations */
  //@{
    /**Print PDU to stream.
      */
    void PrintOn(
      ostream & strm
    ) const;

    /**Read PDU from the specified transport.
      */
    BOOL Read(
      H323Transport & transport   ///< Transport to read from
    );

    /** Process a block of raw read as a PDU
      */
    BOOL ProcessReadData(
      H323Transport & transport,   ///< Transport to read from
      const PBYTEArray & rawData
    );

    /**Write the PDU to the transport.
      */
    BOOL Write(
      H323Transport & transport,   ///* Transport to write to
      H323Connection & connection  ///* Connection for CallBack
    );

    /**Get the Q.931 wrapper PDU for H.225 signalling PDU.
      */
    const Q931 & GetQ931() const { return q931pdu; }

    /**Get the Q.931 wrapper PDU for H.225 signalling PDU.
      */
    Q931 & GetQ931() { return q931pdu; }

    /**Set the Q.931 wrapper PDU for H.225 signalling PDU
     */
    void SetQ931(const Q931 & _q931pdu) { q931pdu = _q931pdu; }

    /**Build the Q.931 wrapper PDU for H.225 signalling PDU.
       This must be called after altering fields in the H.225 part of the PDU.
       If it has never been done, then the Write() functions will do so.
      */
    void BuildQ931();

    /**Get the source alias names for the remote endpoint.
       This returns a human readable set of names that was provided by the
       remote endpoint to identify it, eg phone number, display name etc etc
      */
    PString GetSourceAliases(
      const H323Transport * transport = NULL  ///< Transport PDU was read from.
    ) const;


    /**Get the source alias names for the remote endpoint.
       This returns an array of names of the remote endpoint to identify it.
      */
    PStringArray GetSourceAliasNames() const;

	/**Get the Source H323 URL
	   This return the URL of the caller if Present. This can be used for Inter-Domain
	   Name caller resolution 
	  */
    PString GetSourceURL() const;

    /**Get the destination alias name(s) for the local endpoint.
       The alias returned here can be used to determine the routing of an
       incoming connection.
      */
    PString GetDestinationAlias(
      BOOL firstAliasOnly = FALSE   ///< Only return the first possible alias
    ) const;

    /**Get the source endpoints identification as a phone number.
       This returns FALSE if the remote never provided any alias or Q.931
       field that indicated a valid e.164 telephone number.
      */
    BOOL GetSourceE164(
      PString & number    ///< String to receive number
    ) const;

    /**Get the destiation  phone number.
       This returns FALSE if the remote never provided any alias or Q.931
       field that indicated a valid e.164 telephone number.
      */
    BOOL GetDestinationE164(
      PString & number    ///< String to receive number
    ) const;

    /**Get the distinctive ring code if present.
       This returns zero if no distinctive ring information is provided.
      */
    unsigned GetDistinctiveRing() const;

    /**Set the Q.931 fields in the PDU.
       This sets the default values for various fields, eg caller party number
       into the Q.931 from the supplied connection.
      */
    void SetQ931Fields(
      const H323Connection & connection,
      BOOL insertPartyNumbers = FALSE,
      unsigned plan = 1,
      unsigned type = 0,
      int presentation = -1,
      int screening = -1
    );

#ifdef H323_H460
	/** When sending the H460 message in the Setup PDU you have to ensure
	    the ARQ is received first then add the Fields to the Setup PDU
		so we require a call back
	  */
	void InsertH460Setup(const H323Connection & connection, H225_Setup_UUIE & setup);
#endif

#ifndef DISABLE_CALLAUTH
	/** When sending the Setup PDU you have to ensure
	    the ARQ is received first then add the cryptoFields to the Setup PDU
		so we require a call back
	  */
    void InsertCryptoTokensSetup(const H323Connection & connection, H225_Setup_UUIE & setup);
#endif


  protected:
    // Even though we generally deal with the H323 protocol (H225) it is
    // actually contained within a field of the Q931 protocol.
    Q931 q931pdu;
};


/////////////////////////////////////////////////////////////////////////////

/**Wrapper class for the H323 control channel.
 */
class H323ControlPDU : public H245_MultimediaSystemControlMessage
{
  PCLASSINFO(H323ControlPDU, H245_MultimediaSystemControlMessage);

  public:
    H245_RequestMessage    & Build(H245_RequestMessage   ::Choices request);
    H245_ResponseMessage   & Build(H245_ResponseMessage  ::Choices response);
    H245_CommandMessage    & Build(H245_CommandMessage   ::Choices command);
    H245_IndicationMessage & Build(H245_IndicationMessage::Choices indication);

    H245_MasterSlaveDetermination & BuildMasterSlaveDetermination(
      unsigned terminalType,
      unsigned statusDeterminationNumber
    );
    H245_MasterSlaveDeterminationAck & BuildMasterSlaveDeterminationAck(
      BOOL isMaster
    );
    H245_MasterSlaveDeterminationReject & BuildMasterSlaveDeterminationReject(
      unsigned cause
    );

    H245_TerminalCapabilitySet & BuildTerminalCapabilitySet(
      const H323Connection & connection,
      unsigned sequenceNumber,
      BOOL empty
    );
    H245_TerminalCapabilitySetAck & BuildTerminalCapabilitySetAck(
      unsigned sequenceNumber
    );
    H245_TerminalCapabilitySetReject & BuildTerminalCapabilitySetReject(
      unsigned sequenceNumber,
      unsigned cause
    );

    H245_OpenLogicalChannel & BuildOpenLogicalChannel(
      unsigned forwardLogicalChannelNumber
    );
    H245_RequestChannelClose & BuildRequestChannelClose(
      unsigned channelNumber,
      unsigned reason
    );
    H245_CloseLogicalChannel & BuildCloseLogicalChannel(
      unsigned channelNumber
    );
    H245_OpenLogicalChannelAck & BuildOpenLogicalChannelAck(
      unsigned channelNumber
    );
    H245_OpenLogicalChannelReject & BuildOpenLogicalChannelReject(
      unsigned channelNumber,
      unsigned cause
    );
    H245_OpenLogicalChannelConfirm & BuildOpenLogicalChannelConfirm(
      unsigned channelNumber
    );
    H245_CloseLogicalChannelAck & BuildCloseLogicalChannelAck(
      unsigned channelNumber
    );
    H245_RequestChannelCloseAck & BuildRequestChannelCloseAck(
      unsigned channelNumber
    );
    H245_RequestChannelCloseReject & BuildRequestChannelCloseReject(
      unsigned channelNumber
    );
    H245_RequestChannelCloseRelease & BuildRequestChannelCloseRelease(
      unsigned channelNumber
    );

    H245_RequestMode & BuildRequestMode(
      unsigned sequenceNumber
    );
    H245_RequestModeAck & BuildRequestModeAck(
      unsigned sequenceNumber,
      unsigned response
    );
    H245_RequestModeReject & BuildRequestModeReject(
      unsigned sequenceNumber,
      unsigned cause
    );

    H245_RoundTripDelayRequest & BuildRoundTripDelayRequest(
      unsigned sequenceNumber
    );
    H245_RoundTripDelayResponse & BuildRoundTripDelayResponse(
      unsigned sequenceNumber
    );

    H245_UserInputIndication & BuildUserInputIndication(
      const PString & value
    );
    H245_UserInputIndication & BuildUserInputIndication(
      char tone,               ///< DTMF tone code
      unsigned duration,       ///< Duration of tone in milliseconds
      unsigned logicalChannel, ///< Logical channel number for RTP sync.
      unsigned rtpTimestamp    ///< RTP timestamp in logical channel sync.
    );

    H245_FunctionNotUnderstood & BuildFunctionNotUnderstood(
      const H323ControlPDU & pdu
    );

    H245_EndSessionCommand & BuildEndSessionCommand(
      unsigned reason
    );
};


/////////////////////////////////////////////////////////////////////////////

/**Wrapper class for the H323 gatekeeper RAS channel.
 */
class H323RasPDU : public H225_RasMessage, public H323TransactionPDU
{
  PCLASSINFO(H323RasPDU, H225_RasMessage);

  public:
    H323RasPDU();
    H323RasPDU(
      const H235Authenticators & authenticators
    );

    // overrides from PObject
    virtual PObject * Clone() const;

    // overrides from H323TransactionPDU
    virtual PASN_Object & GetPDU();
    virtual PASN_Choice & GetChoice();
    virtual const PASN_Object & GetPDU() const;
    virtual const PASN_Choice & GetChoice() const;
    virtual unsigned GetSequenceNumber() const;
    virtual unsigned GetRequestInProgressDelay() const;
#if PTRACING
    virtual const char * GetProtocolName() const;
#endif
    virtual H323TransactionPDU * ClonePDU() const;
    virtual void DeletePDU();

    // new functions
    H225_GatekeeperRequest       & BuildGatekeeperRequest(unsigned seqNum);
    H225_GatekeeperConfirm       & BuildGatekeeperConfirm(unsigned seqNum);
    H225_GatekeeperReject        & BuildGatekeeperReject(unsigned seqNum, unsigned reason = H225_GatekeeperRejectReason::e_undefinedReason);
    H225_RegistrationRequest     & BuildRegistrationRequest(unsigned seqNum);
    H225_RegistrationConfirm     & BuildRegistrationConfirm(unsigned seqNum);
    H225_RegistrationReject      & BuildRegistrationReject(unsigned seqNum, unsigned reason = H225_RegistrationRejectReason::e_undefinedReason);
    H225_UnregistrationRequest   & BuildUnregistrationRequest(unsigned seqNum);
    H225_UnregistrationConfirm   & BuildUnregistrationConfirm(unsigned seqNum);
    H225_UnregistrationReject    & BuildUnregistrationReject(unsigned seqNum, unsigned reason = H225_UnregRejectReason::e_undefinedReason);
    H225_LocationRequest         & BuildLocationRequest(unsigned seqNum);
    H225_LocationConfirm         & BuildLocationConfirm(unsigned seqNum);
    H225_LocationReject          & BuildLocationReject(unsigned seqNum, unsigned reason = H225_LocationRejectReason::e_undefinedReason);
    H225_AdmissionRequest        & BuildAdmissionRequest(unsigned seqNum);
    H225_AdmissionConfirm        & BuildAdmissionConfirm(unsigned seqNum);
    H225_AdmissionReject         & BuildAdmissionReject(unsigned seqNum, unsigned reason = H225_AdmissionRejectReason::e_undefinedReason);
    H225_DisengageRequest        & BuildDisengageRequest(unsigned seqNum);
    H225_DisengageConfirm        & BuildDisengageConfirm(unsigned seqNum);
    H225_DisengageReject         & BuildDisengageReject(unsigned seqNum, unsigned reason = H225_DisengageRejectReason::e_securityDenial);
    H225_BandwidthRequest        & BuildBandwidthRequest(unsigned seqNum);
    H225_BandwidthConfirm        & BuildBandwidthConfirm(unsigned seqNum, unsigned bandwidth = 0);
    H225_BandwidthReject         & BuildBandwidthReject(unsigned seqNum, unsigned reason = H225_BandRejectReason::e_undefinedReason);
    H225_InfoRequest             & BuildInfoRequest(unsigned seqNum, unsigned callRef = 0, const OpalGloballyUniqueID * id = NULL);
    H225_InfoRequestResponse     & BuildInfoRequestResponse(unsigned seqNum);
    H225_InfoRequestAck          & BuildInfoRequestAck(unsigned seqNum);
    H225_InfoRequestNak          & BuildInfoRequestNak(unsigned seqNum, unsigned reason = H225_InfoRequestNakReason::e_undefinedReason);
    H225_ServiceControlIndication& BuildServiceControlIndication(unsigned seqNum, const OpalGloballyUniqueID * id = NULL);
    H225_ServiceControlResponse  & BuildServiceControlResponse(unsigned seqNum);
    H225_UnknownMessageResponse  & BuildUnknownMessageResponse(unsigned seqNum);
    H225_RequestInProgress       & BuildRequestInProgress(unsigned seqNum, unsigned delay);
};


/////////////////////////////////////////////////////////////////////////////

void H323SetAliasAddresses(const H323TransportAddressArray & addresses, H225_ArrayOf_AliasAddress & aliases);
void H323SetAliasAddresses(const PStringArray & names, H225_ArrayOf_AliasAddress & aliases, int tag = -1);
void H323SetAliasAddresses(const PStringList & names, H225_ArrayOf_AliasAddress & aliases, int tag = -1);
void H323SetAliasAddress(const H323TransportAddress & address, H225_AliasAddress & alias);
void H323SetAliasAddress(const PString & name, H225_AliasAddress & alias, int tag = -1);
PStringArray H323GetAliasAddressStrings(const H225_ArrayOf_AliasAddress & aliases);
PString H323GetAliasAddressString(const H225_AliasAddress & alias);
PString H323GetAliasAddressE164(const H225_AliasAddress & alias);
PString H323GetAliasAddressE164(const H225_ArrayOf_AliasAddress & aliases);

H323Connection::CallEndReason H323TranslateToCallEndReason(
  Q931::CauseValues cause,
  const H225_ReleaseCompleteReason & reason
);
Q931::CauseValues H323TranslateFromCallEndReason(
  H323Connection::CallEndReason endReason,
  H225_ReleaseCompleteReason & reason
);
Q931::CauseValues H323TranslateFromCallEndReason(
  const H323Connection & connection,
  H225_ReleaseCompleteReason & rcReason
);

PString H323GetApplicationInfo(const H225_VendorIdentifier & vendor);

BOOL H323SetRTPPacketization(
  H245_RTPPayloadType & rtpPacketization,
  const OpalMediaFormat & mediaFormat,
  RTP_DataFrame::PayloadTypes payloadType
);
BOOL H323GetRTPPacketization(
  OpalMediaFormat & mediaFormat,
  const H245_RTPPayloadType & rtpPacketization
);

#if PTRACING
void H323TraceDumpPDU(
  const char * proto,
  BOOL writing,
  const PBYTEArray & rawData,
  const PASN_Object & pdu,
  const PASN_Choice & tag1,
  unsigned seqNum,
  const H323TransportAddress & locAddr,
  const H323TransportAddress & remAddr
);
#else
#define H323TraceDumpPDU(proto, writing, rawData, pdu, tag1, seqNum, locAddr, remAddr)
#endif


#endif // __OPAL_H323PDU_H


/////////////////////////////////////////////////////////////////////////////
