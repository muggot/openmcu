/*
 * q931.h
 *
 * Q.931 protocol handler
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
 * $Log: q931.h,v $
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.56  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.55  2004/12/10 00:20:22  csoutheren
 * Fixed typo
 *
 * Revision 1.54  2004/12/09 23:37:14  csoutheren
 * Added new Q.931 termination codes
 *
 * Revision 1.53  2004/07/11 11:37:28  rjongbloed
 * Added ConnectAck support and fixed bugs in ChannelIdentificationIE, thanks Eize Slange
 *
 * Revision 1.52  2003/03/18 05:54:14  robertj
 * Added ChannelIdentifier IE support, thanks Eize Slange
 *
 * Revision 1.51  2003/02/12 00:02:06  robertj
 * Added more Q.931 cause codes.
 * Added ability to trace text version of cause codes and IE codes.
 *
 * Revision 1.50  2002/11/25 22:40:00  robertj
 * Added another Q.850 code
 *
 * Revision 1.49  2002/11/07 03:49:49  robertj
 * Added extra "congested" Q.931 codes.
 *
 * Revision 1.48  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.47  2002/09/03 05:42:27  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 * Removed redundent includes.
 *
 * Revision 1.46  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.45  2002/07/05 03:13:35  robertj
 * Added copy constructor for Q.931 so makes duplicate instead of reference to IE's.
 *
 * Revision 1.44  2002/05/22 23:12:01  robertj
 * Enhanced the display of Release-Complete cause codes.
 *
 * Revision 1.43  2002/05/03 05:38:16  robertj
 * Added Q.931 Keypad IE mechanism for user indications (DTMF).
 *
 * Revision 1.42  2002/04/22 08:03:41  craigs
 * Added Q931 progress codes
 *
 * Revision 1.41  2002/04/22 07:33:02  craigs
 * Changed GetProgressIndicator to be const
 * Fixed spelling mistake in SignalType enums list
 *
 * Revision 1.40  2002/04/18 09:35:00  robertj
 * Added CallState IE processing.
 *
 * Revision 1.39  2002/03/27 06:04:42  robertj
 * Added Temporary Failure end code for connection, an application may
 *   immediately retry the call if this occurs.
 *
 * Revision 1.38  2002/02/08 00:05:14  robertj
 * Changed release complete causes so can dsitinguish between unknown
 *   cause and no cause at all.
 *
 * Revision 1.37  2002/01/07 04:25:41  robertj
 * Added support for Connected-Number Information Element, thanks Hans Verbeek
 *
 * Revision 1.36  2001/09/17 02:06:38  robertj
 * Added Redirecting Number IE to Q.931, thanks Frank Derks
 *
 * Revision 1.35  2001/09/13 02:41:32  robertj
 * Fixed call reference generation to use full range and common code, thanks Carlo Kielstra
 *
 * Revision 1.34  2001/09/12 02:01:33  robertj
 * Added "No Circuit" release complete code.
 *
 * Revision 1.33  2001/08/20 06:48:26  robertj
 * Added Q.931 function for setting bearer capabilities, allowing
 *    applications to set the data rate as they require.
 *
 * Revision 1.32  2001/08/03 14:12:07  robertj
 * Fixed value for Call State Information Element
 *
 * Revision 1.31  2001/07/24 23:40:15  craigs
 * Added ability to remove Q931 IE
 *
 * Revision 1.30  2001/06/14 06:25:13  robertj
 * Added further H.225 PDU build functions.
 * Moved some functionality from connection to PDU class.
 *
 * Revision 1.29  2001/05/30 04:38:38  robertj
 * Added BuildStatusEnquiry() Q.931 function, thanks Markus Storm
 *
 * Revision 1.28  2001/04/11 00:12:38  robertj
 * Added some enums for numbering plans and call types, thanks Markus Storm.
 *
 * Revision 1.27  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.26  2001/01/19 07:01:42  robertj
 * Added all of the Q.931 message type codes.
 *
 * Revision 1.25  2000/10/13 02:15:23  robertj
 * Added support for Progress Indicator Q.931/H.225 message.
 *
 * Revision 1.24  2000/07/09 14:53:17  robertj
 * Added facility IE to facility message.
 * Changed reference to the word "field" to be more correct IE or "Information Element"
 *
 * Revision 1.23  2000/06/21 08:07:39  robertj
 * Added cause/reason to release complete PDU, where relevent.
 *
 * Revision 1.22  2000/05/18 11:53:34  robertj
 * Changes to support doc++ documentation generation.
 *
 * Revision 1.21  2000/05/09 12:19:23  robertj
 * Added ability to get and set "distinctive ring" Q.931 functionality.
 *
 * Revision 1.20  2000/05/08 14:07:26  robertj
 * Improved the provision and detection of calling and caller numbers, aliases and hostnames.
 *
 * Revision 1.19  2000/05/06 02:17:49  robertj
 * Changed the new CallingPartyNumber code so defaults for octet3a are application dependent.
 *
 * Revision 1.18  2000/05/05 00:44:05  robertj
 * Added presentation and screening fields to Calling Party Number field, thanks Dean Anderson.
 *
 * Revision 1.17  2000/05/02 04:32:25  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.16  2000/03/21 01:07:21  robertj
 * Fixed incorrect call reference code being used in originated call.
 *
 * Revision 1.15  1999/12/23 22:43:36  robertj
 * Added calling party number field.
 *
 * Revision 1.14  1999/09/10 03:36:48  robertj
 * Added simple Q.931 Status response to Q.931 Status Enquiry
 *
 * Revision 1.13  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 * Revision 1.12  1999/08/13 06:34:38  robertj
 * Fixed problem in CallPartyNumber Q.931 encoding.
 * Added field name display to Q.931 protocol.
 *
 * Revision 1.11  1999/08/10 13:14:15  robertj
 * Added Q.931 Called Number field if have "phone number" style destination addres.
 *
 * Revision 1.10  1999/07/23 02:36:56  robertj
 * Finally found correct value for FACILITY message.
 *
 * Revision 1.9  1999/07/16 02:15:30  robertj
 * Fixed more tunneling problems.
 *
 * Revision 1.8  1999/07/09 06:09:49  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.7  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.6  1999/06/09 05:26:20  robertj
 * Major restructuring of classes.
 *
 * Revision 1.5  1999/02/23 11:04:29  robertj
 * Added capability to make outgoing call.
 *
 * Revision 1.4  1999/01/16 11:31:46  robertj
 * Fixed name in header comment.
 *
 * Revision 1.3  1999/01/16 01:31:39  robertj
 * Major implementation.
 *
 * Revision 1.2  1999/01/02 04:00:55  robertj
 * Added higher level protocol negotiations.
 *
 * Revision 1.1  1998/12/14 09:13:41  robertj
 * Initial revision
 *
 */

#ifndef __OPAL_Q931_H
#define __OPAL_Q931_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


///////////////////////////////////////////////////////////////////////////////

/**This class embodies a Q.931 Protocol Data Unit.
  */
class Q931 : public PObject
{
  PCLASSINFO(Q931, PObject)
  public:
    enum MsgTypes {
      NationalEscapeMsg  = 0x00,
      AlertingMsg        = 0x01,
      CallProceedingMsg  = 0x02,
      ConnectMsg         = 0x07,
      ConnectAckMsg      = 0x0f,
      ProgressMsg        = 0x03,
      SetupMsg           = 0x05,
      SetupAckMsg        = 0x0d,
      ResumeMsg          = 0x26,
      ResumeAckMsg       = 0x2e,
      ResumeRejectMsg    = 0x22,
      SuspendMsg         = 0x25,
      SuspendAckMsg      = 0x2d,
      SuspendRejectMsg   = 0x21,
      UserInformationMsg = 0x20,
      DisconnectMsg      = 0x45,
      ReleaseMsg         = 0x4d,
      ReleaseCompleteMsg = 0x5a,
      RestartMsg         = 0x46,
      RestartAckMsg      = 0x4e,
      SegmentMsg         = 0x60,
      CongestionCtrlMsg  = 0x79,
      InformationMsg     = 0x7b,
      NotifyMsg          = 0x6e,
      StatusMsg          = 0x7d,
      StatusEnquiryMsg   = 0x75,
      FacilityMsg        = 0x62
    };

    Q931();
    Q931(const Q931 & other);
    Q931 & operator=(const Q931 & other);

    void BuildFacility(int callRef, BOOL fromDest);
    void BuildInformation(int callRef, BOOL fromDest);
    void BuildProgress(
      int callRef,
      BOOL fromDest,
      unsigned description,
      unsigned codingStandard = 0,
      unsigned location = 0
    );
    void BuildNotify(int callRef, BOOL fromDest);
    void BuildCallProceeding(int callRef);
    void BuildSetupAcknowledge(int callRef);
    void BuildAlerting(int callRef);
    void BuildSetup(int callRef = -1);
    void BuildConnect(int callRef);
    void BuildConnectAck(int callRef, BOOL fromDest);
    void BuildStatus(int callRef, BOOL fromDest);
    void BuildStatusEnquiry(int callRef, BOOL fromDest);
    void BuildReleaseComplete(int callRef, BOOL fromDest);

    BOOL Decode(const PBYTEArray & data);
    BOOL Encode(PBYTEArray & data) const;

    void PrintOn(ostream & strm) const;
    PString GetMessageTypeName() const;

    static unsigned GenerateCallReference();
    unsigned GetCallReference() const { return callReference; }
    BOOL IsFromDestination() const { return fromDestination; }
    MsgTypes GetMessageType() const { return messageType; }

    enum InformationElementCodes {
      BearerCapabilityIE      = 0x04,
      CauseIE                 = 0x08,
      ChannelIdentificationIE = 0x18,
      FacilityIE              = 0x1c,
      ProgressIndicatorIE     = 0x1e,
      CallStateIE             = 0x14,
      DisplayIE               = 0x28,
      KeypadIE                = 0x2c,
      SignalIE                = 0x34,
      ConnectedNumberIE       = 0x4c,
      CallingPartyNumberIE    = 0x6c,
      CalledPartyNumberIE     = 0x70,
      RedirectingNumberIE     = 0x74,
      UserUserIE              = 0x7e
    };
    friend ostream & operator<<(ostream & strm, InformationElementCodes ie);

    BOOL HasIE(InformationElementCodes ie) const;
    PBYTEArray GetIE(InformationElementCodes ie) const;
    void SetIE(InformationElementCodes ie, const PBYTEArray & userData);
    void RemoveIE(InformationElementCodes ie);

    enum InformationTransferCapability {
      TransferSpeech,
      TransferUnrestrictedDigital = 8,
      TransferRestrictedDigital = 9,
      Transfer3_1kHzAudio = 16,
      TransferUnrestrictedDigitalWithTones = 17,
      TransferVideo = 24
    };

    void SetBearerCapabilities(
      InformationTransferCapability capability,
      unsigned transferRate,        ///<  Number of 64k B channels
      unsigned codingStandard = 0,  ///<  0 = ITU-T standardized coding
      unsigned userInfoLayer1 = 5   ///<  5 = Recommendations H.221 and H.242
    );

    BOOL GetBearerCapabilities(
      InformationTransferCapability & capability,
      unsigned & transferRate,        ///<  Number of 64k B channels
      unsigned * codingStandard = NULL,
      unsigned * userInfoLayer1 = NULL
    );

    enum CauseValues {
      UnknownCauseIE               =  0,
      UnallocatedNumber            =  1,
      NoRouteToNetwork             =  2,
      NoRouteToDestination         =  3,
      SendSpecialTone              =  4,
      MisdialledTrunkPrefix        =  5,
      ChannelUnacceptable          =  6,
      CallAwarded                  =  7,
      Preemption                   =  8,
      PreemptionCircuitReserved    =  9,
      NormalCallClearing           = 16,
      UserBusy                     = 17,
      NoResponse                   = 18,
      NoAnswer                     = 19,
      SubscriberAbsent             = 20,
      CallRejected                 = 21,
      NumberChanged                = 22,
      Redirection                  = 23,
      ExchangeRoutingError         = 25,
      NonSelectedUserClearing      = 26,
      DestinationOutOfOrder        = 27,
      InvalidNumberFormat          = 28,
      FacilityRejected             = 29,
      StatusEnquiryResponse        = 30,
      NormalUnspecified            = 31,
      NoCircuitChannelAvailable    = 34,
      CallQueued                   = 35,
      NetworkOutOfOrder            = 38,
      FrameModeOOS                 = 39,
      FrameModeOperational         = 40,
      TemporaryFailure             = 41,
      Congestion                   = 42,
      AccessInformationDiscarded   = 43,
      RequestedCircuitNotAvailable = 44,
      PrecedenceCallBlocked        = 46,
      ResourceUnavailable          = 47,
      QoSNotAvailable              = 49,
      RequestedFacilityNotSubscribed = 50,
      OutgoingCallsBarred          = 52,
      OutgoingCallsBarredInCUG     = 53,
      IncomingCallsBarred          = 54,
      IncomingCallsBarredInCUG     = 55,
      BearerCapNotAuthorised       = 57,
      BearerCapNotPresentlyAvailable = 58,
      InconsistentOutgoingIE       = 62,
      ServiceOptionNotAvailable    = 63,
      BearerCapNotImplemented      = 65,
      ChannelTypeNotImplemented    = 66,
      RequestedFacilityNotImplemented         = 69,
      OnlyRestrictedDigitalBearerCapAvailable = 70,
      ServiceOrOptionNotImplemented           = 79,
      InvalidCallReference         = 81,
      IdentifiedChannelNonExistent = 82,
      CallIdentifyNotSuspendedCall = 83,
      CallIdentifyInUse            = 84,
      NoCallSuspended              = 85,
      ClearedRequestedCallIdentity = 86,
      UserNotInCUG                 = 87,
      IncompatibleDestination      = 88,
      NonexistentCUG               = 90,
      InvalidTransitNetwork        = 91,
      InvalidMessageUnspecified    = 95,
      MandatoryIEMissing           = 96,
      MessageTypeNonexistent       = 97,
      MessageNotCompatible         = 98,
      IENonExistantOrNotImplemented     = 99,
      InvalidIEContents                 = 100,
      MessageNotCompatibleWithCallState = 101,
      TimerExpiry                       = 102,
      ParameterNonexistent              = 103,
      UnrecognisedParamaterDiscarded    = 110,
      ProtocolErrorUnspecified     = 111,
      InterworkingUnspecified      = 127,
      ErrorInCauseIE               = 0x100
    };
    friend ostream & operator<<(ostream & strm, CauseValues cause);

    void SetCause(
      CauseValues value,
      unsigned standard = 0,  ///<  0 = ITU-T standardized coding
      unsigned location = 0   ///<  0 = User
    );
    CauseValues GetCause(
      unsigned * standard = NULL,  ///<  0 = ITU-T standardized coding
      unsigned * location = NULL   ///<  0 = User
    ) const;

    enum CallStates {
      CallState_Null                  = 0,
      CallState_CallInitiated         = 1,
      CallState_OverlapSending        = 2,
      CallState_OutgoingCallProceeding= 3,
      CallState_CallDelivered         = 4,
      CallState_CallPresent           = 6,
      CallState_CallReceived          = 7,
      CallState_ConnectRequest        = 8,
      CallState_IncomingCallProceeding= 9,
      CallState_Active                = 10,
      CallState_DisconnectRequest     = 11,
      CallState_DisconnectIndication  = 12,
      CallState_SuspendRequest        = 15,
      CallState_ResumeRequest         = 17,
      CallState_ReleaseRequest        = 19,
      CallState_OverlapReceiving      = 25,
      CallState_ErrorInIE             = 0x100
    };
    void SetCallState(
      CallStates value,
      unsigned standard = 0  ///<  0 = ITU-T standardized coding
    );
    CallStates GetCallState(
      unsigned * standard = NULL  ///<  0 = ITU-T standardized coding
    ) const;

    enum SignalInfo {
      SignalDialToneOn,
      SignalRingBackToneOn,
      SignalInterceptToneOn,
      SignalNetworkCongestionToneOn,
      SignalBusyToneOn,
      SignalConfirmToneOn,
      SignalAnswerToneOn,
      SignalCallWaitingTone,
      SignalOffhookWarningTone,
      SignalPreemptionToneOn,
      SignalTonesOff = 0x3f,
      SignalAlertingPattern0 = 0x40,
      SignalAlertingPattern1,
      SignalAlertingPattern2,
      SignalAlertingPattern3,
      SignalAlertingPattern4,
      SignalAlertingPattern5,
      SignalAlertingPattern6,
      SignalAlertingPattern7,
      SignalAlertingOff = 0x4f,
      SignalErrorInIE = 0x100
    };
    void SetSignalInfo(SignalInfo value);
    SignalInfo GetSignalInfo() const;

    void SetKeypad(const PString & digits);
    PString GetKeypad() const;

    enum ProgressIndication {
       ProgressNotEndToEndISDN      = 1,      // Call is not end-to-end ISDN; 
                                              // further call progress information may be available in-band  
       ProgressDestinationNonISDN   = 2,      // Destination address is non ISDN  
       ProgressOriginNotISDN        = 3,      // Origination address is non ISDN  
       ProgressReturnedToISDN       = 4,      // Call has returned to the ISDN 
       ProgressServiceChange        = 5,      // Interworking has occurred and has 
                                              // resulted in a telecommunication service change
       ProgressInbandInformationAvailable = 8 // In-band information or an appropriate pattern is now available.   
    };

    void SetProgressIndicator(
      unsigned description,
      unsigned codingStandard = 0,
      unsigned location = 0
    );
    BOOL GetProgressIndicator(
      unsigned & description,
      unsigned * codingStandard = NULL,
      unsigned * location = NULL
    ) const;

    void SetDisplayName(const PString & name);
    PString GetDisplayName() const;

    enum NumberingPlanCodes {
      UnknownPlan          = 0x00,
      ISDNPlan             = 0x01,
      DataPlan             = 0x03,
      TelexPlan            = 0x04,
      NationalStandardPlan = 0x08,
      PrivatePlan          = 0x09,
      ReservedPlan         = 0x0f
    };

    enum TypeOfNumberCodes {
      UnknownType          = 0x00,
      InternationalType    = 0x01,
      NationalType         = 0x02,
      NetworkSpecificType  = 0x03,
      SubscriberType       = 0x04,
      AbbreviatedType      = 0x06,
      ReservedType         = 0x07
    };

    void SetCallingPartyNumber(
      const PString & number, ///<  Number string
      unsigned plan = 1,      ///<  1 = ISDN/Telephony numbering system
      unsigned type = 0,      ///<  0 = Unknown number type
      int presentation = -1,  ///<  0 = presentation allowed, -1 = no octet3a
      int screening = -1      ///<   0 = user provided, not screened
    );
    BOOL GetCallingPartyNumber(
      PString & number,               ///<  Number string
      unsigned * plan = NULL,         ///<  ISDN/Telephony numbering system
      unsigned * type = NULL,         ///<  Number type
      unsigned * presentation = NULL, ///<  Presentation indicator
      unsigned * screening = NULL,    ///<  Screening indicator
      unsigned defPresentation = 0,   ///<  Default value if octet3a not present
      unsigned defScreening = 0       ///<  Default value if octet3a not present
    ) const;

    void SetCalledPartyNumber(
      const PString & number, ///<  Number string
      unsigned plan = 1,      ///<  1 = ISDN/Telephony numbering system
      unsigned type = 0       ///<  0 = Unknown number type
    );
    BOOL GetCalledPartyNumber(
      PString & number,       ///<  Number string
      unsigned * plan = NULL, ///<  ISDN/Telephony numbering system
      unsigned * type = NULL  ///<  Number type
    ) const;

    void SetRedirectingNumber(
      const PString & number, ///<  Number string
      unsigned plan = 1,      ///<  1 = ISDN/Telephony numbering system
      unsigned type = 0,      ///<  0 = Unknown number type
      int presentation = -1,  ///<  0 = presentation allowed, -1 = no octet3a
      int screening = -1,     ///<  0 = user provided, not screened
      int reason = -1         ///<  0 = Unknown reason , -1 = no octet 3b
    );
    BOOL GetRedirectingNumber(
      PString & number,               ///<  Number string
      unsigned * plan = NULL,         ///<  ISDN/Telephony numbering system
      unsigned * type = NULL,         ///<  Number type
      unsigned * presentation = NULL, ///<  Presentation indicator
      unsigned * screening = NULL,    ///<  Screening indicator
      unsigned * reason = NULL,       ///<  Reason for redirection
      unsigned defPresentation = 0,   ///<  Default value if octet3a not present
      unsigned defScreening = 0,      ///<  Default value if octet3a not present
      unsigned defReason =0           ///<  Default value if octet 3b not present
    ) const;

    void SetConnectedNumber(
      const PString & number, ///<  Number string
      unsigned plan = 1,      ///<  1 = ISDN/Telephony numbering system
      unsigned type = 0,      ///<  0 = Unknown number type
      int presentation = -1,  ///<  0 = presentation allowed, -1 = no octet3a
      int screening = -1,     ///<  0 = user provided, not screened
      int reason = -1         ///<  0 = Unknown reason , -1 = no octet 3b
    );
    BOOL GetConnectedNumber(
      PString & number,               ///<  Number string
      unsigned * plan = NULL,         ///<  ISDN/Telephony numbering system
      unsigned * type = NULL,         ///<  Number type
      unsigned * presentation = NULL, ///<  Presentation indicator
      unsigned * screening = NULL,    ///<  Screening indicator
      unsigned * reason = NULL,       ///<  Reason for redirection
      unsigned defPresentation = 0,   ///<  Default value if octet3a not present
      unsigned defScreening = 0,      ///<  Default value if octet3a not present
      unsigned defReason =0           ///<  Default value if octet 3b not present
    ) const;

    /**Set the limitations to ChannelIdentification.
        - the interface identifier cannot be specified
        - channel in PRI can only be indicated by number and cannot be indicated by map
        - one and only one channel can be indicated
        - the coding standard is always ITU Q.931
      */
    void SetChannelIdentification(
      unsigned interfaceType = 0,        ///<   0 = basic,     1 = other (e.g. primary)
      unsigned preferredOrExclusive = 0, ///<   0 = preferred, 1 = exclusive
      int      channelNumber = 1         ///<  -1 = any,       0 = none/D, 1 = channel 1/B1, etc. 1-15,17-31
    );

    /**Get the limitations to ChannelIdentification.
      */
    BOOL GetChannelIdentification(
      unsigned * interfaceType = NULL,        ///<  Interface type
      unsigned * preferredOrExclusive = NULL, ///<  Channel negotiation preference
      int      * channelNumber = NULL         ///<  Channel number
    ) const;

  protected:
    unsigned callReference;
    BOOL fromDestination;
    unsigned protocolDiscriminator;
    MsgTypes messageType;

    PDICTIONARY(InternalInformationElements, POrdinalKey, PBYTEArray);
    InternalInformationElements informationElements;
};


#endif // __OPAL_Q931_H


/////////////////////////////////////////////////////////////////////////////
