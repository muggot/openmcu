/*
 * h450pdu.h
 *
 * H.450 Helper functions
 *
 * Open H323 Library
 *
 * Copyright (c) 2001 Norwood Systems Pty. Ltd.
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
 * $Log: h450pdu.h,v $
 * Revision 1.1  2007/08/06 20:50:51  shorne
 * First commit of h323plus
 *
 * Revision 1.1.2.1  2006/12/23 19:02:56  shorne
 * File restructuring
 *
 * Revision 1.13  2006/06/27 12:35:03  csoutheren
 * Patch 1366328 - Support for H.450.3 divertingLegInformation2
 * Thanks to Norbert Bartalsky
 *
 * Revision 1.12  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.11  2005/03/10 07:01:29  csoutheren
 * Fixed problem with H.450 call identifiers not being unique across all calls on an
 *  endpoint. Thanks to Thien Nguyen
 *
 * Revision 1.10  2002/11/21 22:37:24  robertj
 * Fixed problems with unrecognized invoke APDU, thanks Andrea Bondavalli
 *
 * Revision 1.9  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.8  2002/09/03 06:19:37  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.7  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.6  2002/07/04 00:40:31  robertj
 * More H.450.11 call intrusion implementation, thanks Aleksandar Todorovic
 *
 * Revision 1.5  2002/06/22 05:48:38  robertj
 * Added partial implementation for H.450.11 Call Intrusion
 *
 * Revision 1.4  2002/02/04 07:17:52  robertj
 * Added H.450.2 Consultation Transfer, thanks Norwood Systems.
 *
 * Revision 1.3  2002/01/14 00:02:40  robertj
 * Added H.450.6
 * Added extra "failure mode" parts of H.250.2.
 * Various other bug fixes.
 *   Thanks Ben Madsen of Norwood Systems
 *
 * Revision 1.2  2001/08/16 07:49:16  robertj
 * Changed the H.450 support to be more extensible. Protocol handlers
 *   are now in separate classes instead of all in H323Connection.
 *
 * Revision 1.1  2001/04/11 03:01:27  robertj
 * Added H.450.2 (call transfer), thanks a LOT to Graeme Reid & Norwood Systems
 *
 */

#ifndef __OPAL_H450PDU_H
#define __OPAL_H450PDU_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include "x880.h"


class H323EndPoint;
class H323Connection;
class H323TransportAddress;
class H323SignalPDU;

class H4501_EndpointAddress;
class H4501_InterpretationApdu;


///////////////////////////////////////////////////////////////////////////////

/**PDU definition for H.450 services.
  */
class H450ServiceAPDU : public X880_ROS
{
  public:
    X880_Invoke& BuildInvoke(int invokeId, int operation);
    X880_ReturnResult& BuildReturnResult(int invokeId);
    X880_ReturnError& BuildReturnError(int invokeId, int error);
    X880_Reject& BuildReject(int invokeId);

    void BuildCallTransferInitiate(int invokeId,
                                   const PString & callIdentity,
                                   const PString & alias,
                                   const H323TransportAddress & address);

    void BuildCallTransferIdentify(int invokeId);
    void BuildCallTransferAbandon(int invokeId);
    void BuildCallTransferSetup(int invokeId,
                                const PString & callIdentity);

    void BuildCallWaiting(int invokeId, int numCallsWaiting);
    
    void BuildCallIntrusionForcedRelease(int invokeId, int CICL);
    X880_ReturnResult& BuildCallIntrusionForcedReleaseResult(int invokeId);
    void BuildCallIntrusionForcedReleaseError();
    void BuildCallIntrusionGetCIPL(int invokeId);
    void BuildCallIntrusionImpending(int invokeId);
    void BuildCallIntrusionForceRelesed(int invokeId);

    void AttachSupplementaryServiceAPDU(H323SignalPDU & pdu);
    BOOL WriteFacilityPDU(
      H323Connection & connection
    );

    static void ParseEndpointAddress(H4501_EndpointAddress & address,
                                     PString & party);
};


class H450xDispatcher;

class H450xHandler : public PObject
{
    PCLASSINFO(H450xHandler, PObject);
  public:
    H450xHandler(
      H323Connection & connection,
      H450xDispatcher & dispatcher
    );

    virtual void AttachToSetup(
      H323SignalPDU & pdu
    );

    virtual void AttachToAlerting(
      H323SignalPDU & pdu
    );

    virtual void AttachToConnect(
      H323SignalPDU & pdu
    );

    virtual void AttachToReleaseComplete(
      H323SignalPDU & pdu
    );

    virtual BOOL OnReceivedInvoke(
      int opcode,
      int invokeId,                           ///<  InvokeId of operation (used in response)
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the initiate operation
    ) = 0;

    virtual BOOL OnReceivedReturnResult(
      X880_ReturnResult & returnResult
    );

    virtual BOOL OnReceivedReturnError(
      int errorCode,
      X880_ReturnError & returnError
    );

    virtual BOOL OnReceivedReject(
      int problemType,
      int problemNumber
    );

    /**Send a return error in response to an invoke operation.
     */
    void SendReturnError(int returnError);

    void SendGeneralReject(int problem);

    void SendInvokeReject(int problem);

    void SendReturnResultReject(int problem);

    void SendReturnErrorReject(int problem);

    BOOL DecodeArguments(
      PASN_OctetString * argString,
      PASN_Object & argObject,
      int absentErrorCode
    );

    unsigned GetInvokeId() const { return currentInvokeId; }


  protected:
    H323EndPoint   & endpoint;
    H323Connection & connection;
    H450xDispatcher & dispatcher;
    unsigned          currentInvokeId;
};

PLIST(H450xHandlerList, H450xHandler);
PDICTIONARY(H450xHandlerDict, POrdinalKey, H450xHandler);


class H450xDispatcher : public PObject
{
    PCLASSINFO(H450xDispatcher, PObject);
  public:
    H450xDispatcher(
      H323Connection & connection
    );

    /**Add a handler for the op code.
      */
    void AddOpCode(
      unsigned opcode,
      H450xHandler * handler
    );

    virtual void AttachToSetup(
      H323SignalPDU & pdu
    );

    virtual void AttachToAlerting(
      H323SignalPDU & pdu
    );

    virtual void AttachToConnect(
      H323SignalPDU & pdu
    );

    virtual void AttachToReleaseComplete(
      H323SignalPDU & pdu
    );

    /** Handle the H.450.x Supplementary Service PDU if present in the H225_H323_UU_PDU
     */
    virtual BOOL HandlePDU(
      const H323SignalPDU & pdu
    );

    /**Handle an incoming X880 Invoke PDU.
       The default behaviour is to attempt to decode the invoke operation
       and call the corresponding OnReceived<Operation> method on the EndPoint.
     */
    virtual BOOL OnReceivedInvoke(X880_Invoke& invoke, H4501_InterpretationApdu& interpretation);

    /**Handle an incoming X880 Return Result PDU.
       The default behaviour is to attempt to match the return result
       to a previous invoke operation and call the corresponding
       OnReceived<Operation>Success method on the EndPoint.
     */
    virtual BOOL OnReceivedReturnResult(X880_ReturnResult& returnResult);

    /**Handle an incoming X880 Return Error PDU.
       The default behaviour is to attempt to match the return error
       to a previous invoke operation and call the corresponding
       OnReceived<Operation>Error method on the EndPoint.
     */
    virtual BOOL OnReceivedReturnError(X880_ReturnError& returnError);

    /**Handle an incoming X880 Reject PDU.
       The default behaviour is to attempt to match the reject
       to a previous invoke, return result or return error operation
       and call OnReceived<Operation>Reject method on the EndPoint.
     */
    virtual BOOL OnReceivedReject(X880_Reject& reject);

    /**Send a return error in response to an invoke operation.
     */
    void SendReturnError(int invokeId, int returnError);

    void SendGeneralReject(int invokeId, int problem);

    void SendInvokeReject(int invokeId, int problem);

    void SendReturnResultReject(int invokeId, int problem);

    void SendReturnErrorReject(int invokeId, int problem);

    /**Get the next available invoke Id for H450 operations
     */
    unsigned GetNextInvokeId() const { return ++nextInvokeId; }

  protected:
    H323Connection & connection;
    H450xHandlerList  handlers;
    H450xHandlerDict  opcodeHandler;
    mutable unsigned  nextInvokeId;             ///<  Next available invoke ID for H450 operations
};


class H4502Handler : public H450xHandler
{
    PCLASSINFO(H4502Handler, H450xHandler);
  public:
    H4502Handler(
      H323Connection & connection,
      H450xDispatcher & dispatcher
    );

    virtual void AttachToSetup(
      H323SignalPDU & pdu
    );

    virtual void AttachToAlerting(
      H323SignalPDU & pdu
    );

    virtual void AttachToConnect(
      H323SignalPDU & pdu
    );

    virtual void AttachToReleaseComplete(
      H323SignalPDU & pdu
    );

    virtual BOOL OnReceivedInvoke(
      int opcode,
      int invokeId,                           ///<  InvokeId of operation (used in response)
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the initiate operation
    );

    /**Handle an incoming Call Transfer Identify operation.
     */
    virtual void OnReceivedCallTransferIdentify(
      int linkedId                            ///<  InvokeId of associated operation (if any)
    );

    /**Handle an incoming Call Transfer Abandon operation.
     */
    virtual void OnReceivedCallTransferAbandon(
      int linkedId                            ///<  InvokeId of associated operation (if any)
    );

    /**Handle an incoming Call Transfer Initiate operation.
     */
    virtual void OnReceivedCallTransferInitiate(
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the initiate operation
    );

    /**Handle an incoming Call Transfer Setup operation.
     */
    virtual void OnReceivedCallTransferSetup(
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the setup operation
    );

    /**Handle an incoming Call Transfer Update operation.
     */
    virtual void OnReceivedCallTransferUpdate(
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the update operation
    );

    /**Handle an incoming Subaddress Transfer operation.
     */
    virtual void OnReceivedSubaddressTransfer(
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the subaddress transfer operation
    );

    /**Handle an incoming Call Transfer Complete operation.
     */
    virtual void OnReceivedCallTransferComplete(
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the complete operation
    );

    /**Handle an incoming Call Transfer Active operation.
     */
    virtual void OnReceivedCallTransferActive(
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the active operation
    );

    virtual BOOL OnReceivedReturnResult(
      X880_ReturnResult & returnResult
    );

    /**Handle the reception of a callTransferInitiate returnResult when we are in call transfer state 
       e_ctAwaitInitiateResponse.  Note this is an internal function and it is not expected an 
       application would use it. 
     */
    void OnReceivedInitiateReturnResult();
    
    /**Handle the reception of a callTransferSetup returnResult when we are in call transfer state 
       e_ctAwaitSetupResponse.  This funtion exists to handle the case when the transferred-to
       endpoint does not support H.450.2.  Note this is an internal function and it is not expected an 
       application would use it.
     */
    void OnReceivedSetupReturnResult();

    /**Handle the reception of a callTransferIdentify returnResult when we are in call transfer state 
       e_ctAwaitIdentifyResponse.  Note this is an internal function and it is not expected an 
       application would use it.
     */
    void OnReceivedIdentifyReturnResult(X880_ReturnResult &returnResult);

    virtual BOOL OnReceivedReturnError(
      int errorCode,
      X880_ReturnError & returnError
    );

    /**Handle the reception of a callTransferInitiate returnError or expiry of Call Transfer Timer CT-T3 
       when we are in call transfer state e_ctAwaitInitiateResponse.  Note this is an internal function 
       and it is not expected an application would use it. 
     */
    void OnReceivedInitiateReturnError(
      const bool timerExpiry = false ///<  Flag to indicate expiry
    );

    /**Handle the reception of a callTransferSetup returnError or expiry of Call Transfer Timer CT-T4 
       when we are in call transfer state e_ctAwaitSetupResponse.  This funtion also additionally handles 
       the case when the transferred-to endpoint does not support H.450.2 and has rejected the incoming 
       call request.  Note this is an internal function and it is not expected an application would use 
       it.
     */
    void OnReceivedSetupReturnError(
      int errorCode,
      const bool timerExpiry = false ///<  Flag to indicate expiry
    );

    /**Handle the reception of a callTransferIdentify returnError or expiry of Call Transfer Timer CT-T1
       when we are in call transfer state e_ctAwaitIdentifyResponse.  Note this is an internal function and it is not expected an 
       application would use it.
     */
    void OnReceivedIdentifyReturnError(
      const bool timerExpiry = false ///<  Flag to indicate expiry
    );

    /**Initiate the transfer of an existing call (connection) to a new remote party
       using H.450.2.  This sends a Call Transfer Initiate Invoke message from the
       A-Party (transferring endpoint) to the B-Party (transferred endpoint).
     */
    void TransferCall(
      const PString & remoteParty,   ///<  Remote party to transfer the existing call to
      const PString & callIdentity   ///<  Call Identity of secondary call if present  
    );

    /**Transfer the call through consultation so the remote party in the primary call is connected to
       the called party in the second call using H.450.2.  This sends a Call Transfer Identify Invoke 
       message from the A-Party (transferring endpoint) to the C-Party (transferred-to endpoint).
     */
    void ConsultationTransfer(
      const PString & primaryCallToken   ///<  Primary call
    );

    /**Handle the reception of a callTransferSetupInvoke APDU whilst a secondary call exists.
     */
    void HandleConsultationTransfer(
      const PString & callIdentity,  ///<  Call Identity of secondary call
      H323Connection& incoming       ///<  New incoming connection
    );

    void AwaitSetupResponse(
      const PString & token,
      const PString & identity
    );

    /**Sub-state for call transfer.
      */
    enum State {
      e_ctIdle,
      e_ctAwaitIdentifyResponse,
      e_ctAwaitInitiateResponse,
      e_ctAwaitSetupResponse,
      e_ctAwaitSetup,
      e_ctAwaitConnect
    };

    /**Get the current call transfer state.
     */
    State GetState() const { return ctState; }

    /**Handle the reception of an Admission Reject during a pending call
       transfer operation at the transferred endpoint. If the call transfer
       state of the current connection is e_ctAwaitSetupResponse, the stack
       attempts to find the existing connection between the transferred and
       transferring endpoints and inform this connection that a
       callTransferInitiateReturnError PDU needs to be sent.  No action is
       taken if the current connection is not in call transfer state
       e_ctAwaitSetupResponse.
     */
    virtual void onReceivedAdmissionReject(const int returnError);

    /**Handle the failure of a call transfer operation.
      */
    void HandleCallTransferFailure(
      const int returnError    ///<  failure reason
    );

    /** Start the Call Transfer Timer using the specified time interval.
     */
    void StartctTimer(const PTimeInterval value) { ctTimer = value; }

    /** Stop the Call Transfer Timer
     */
    void StopctTimer();

    /**Is the Call Transfer Timer running?
     */
    BOOL IsctTimerRunning() { return ctTimer.IsRunning(); }

    /**Callback mechanism for Call Transfer Timers CT-T1, CT-T2, CT-T3 & CT-T4
     */
    PDECLARE_NOTIFIER(PTimer, H4502Handler, OnCallTransferTimeOut);

    /**Get the connection assoicated with this H4502Handler.
     */
    const H323Connection& getAssociatedConnection() const { return connection; }

    /**Set the associated callToken.
     */
    void SetAssociatedCallToken(const PString& token) { CallToken = token; }

    /**Get the transferringCallToken member
     */
    const PString& getTransferringCallToken() const { return transferringCallToken; }

    /**Set the 'consultationTransfer' member to TRUE (indicating a successful transfer)
     */
    void SetConsultationTransferSuccess() { consultationTransfer = TRUE; }

    /**Was the transfer through consultation successful.
     */
    BOOL isConsultationTransferSuccess() { return consultationTransfer; }

  protected:
    PString transferringCallToken;    // Stores the call token for the transferring connection (if there is one)
    PString transferringCallIdentity; // Stores the call identity for the transferring call (if there is one)
    State   ctState;                  // Call Transfer state of the conneciton
    BOOL    ctResponseSent;           // Has a callTransferSetupReturnResult been sent?
    PTimer  ctTimer;                  // Call Transfer Timer - Handles all four timers CT-T1,
    PString CallToken;                // Call Token of the associated connection 
                                      // (used during a consultation transfer).
    BOOL consultationTransfer;        // Flag used to indicate whether an incoming call is involved in
                                      // a transfer through consultation.
};


class H4503Handler : public H450xHandler
{
    PCLASSINFO(H4503Handler, H450xHandler);
  public:
    H4503Handler(
      H323Connection & connection,
      H450xDispatcher & dispatcher
    );

    virtual BOOL OnReceivedInvoke(
      int opcode,
      int invokeId,                           /// InvokeId of operation (used in response)
      int linkedId,                           /// InvokeId of associated operation (if any)
      PASN_OctetString * argument             /// Parameters for the initiate operation
    );

    /**Handle an incoming Near-End Call Hold operation
    */
    void OnReceivedDivertingLegInfo2(
      int linkedId,                            /// InvokeId of associated operation (if any)
      PASN_OctetString * argument 
    );

    BOOL GetRedirectingNumber(
      PString &originalCalledNr,               /// InvokeId of associated operation (if any)
      PString &lastDivertingNr ,
      int &divCounter, 
      int &origdivReason,
	  int &divReason
    );

  protected:
    PString m_originalCalledNr;  // Originally called number (=> the 1st redirecting number) 
    PString m_lastDivertingNr;   // last diverting number (in the case of nultiple-call forward)
	int m_diversionCounter;
	int m_origdiversionReason;   // original diversion reason
	int m_diversionReason;       // diversion reason 
};


class H4504Handler : public H450xHandler
{
    PCLASSINFO(H4504Handler, H450xHandler);
  public:
    H4504Handler(
      H323Connection & connection,
      H450xDispatcher & dispatcher
    );

    virtual BOOL OnReceivedInvoke(
      int opcode,
      int invokeId,                           ///<  InvokeId of operation (used in response)
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the initiate operation
    );

    /**Handle an incoming Near-End Call Hold operation
    */
    virtual void OnReceivedLocalCallHold(
      int linkedId                            ///<  InvokeId of associated operation (if any)
    );

    /**Handle an incoming Near-End Call Retrieve operation
    */
    virtual void OnReceivedLocalCallRetrieve(
      int linkedId                            ///<  InvokeId of associated operation (if any)
    );

    /**Handle an incoming Remote Call Hold operation
    * TBD: Remote hold operations not yet implemented -- dcassel 4/01
    */
    virtual void OnReceivedRemoteCallHold(
      int linkedId                            ///<  InvokeId of associated operation (if any)
    );

    /**Handle an incoming Remote Call Retrieve operation
    * TBD: Remote hold operations not yet implemented -- dcassel 4/01
    */
    virtual void OnReceivedRemoteCallRetrieve(
      int linkedId                            ///<  InvokeId of associated operation (if any)
    );

    /**Place the call on hold, suspending all media channels (H.450.4)
    * NOTE: Only Local Hold is implemented so far. 
    */
    void HoldCall(
      BOOL localHold   ///<  true for Local Hold, false for Remote Hold
    );

    /**Retrieve the call from hold, activating all media channels (H.450.4)
    * NOTE: Only Local Hold is implemented so far. 
    */
    void RetrieveCall();

    /**Sub-state for call hold.
      */
    enum State {
      e_ch_Idle,
      e_ch_NE_Held,
      e_ch_RE_Requested,
      e_ch_RE_Held,
      e_ch_RE_Retrieve_Req
    };

    State GetState() const { return holdState; }


  protected:
    State holdState;  // Call Hold state of this connection
};


class H4506Handler : public H450xHandler
{
    PCLASSINFO(H4506Handler, H450xHandler);
  public:
    H4506Handler(
      H323Connection & connection,
      H450xDispatcher & dispatcher
    );

    virtual BOOL OnReceivedInvoke(
      int opcode,
      int invokeId,                           ///<  InvokeId of operation (used in response)
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the initiate operation
    );

    /**Handle an incoming Call Waiting Indication PDU
    */
    virtual void OnReceivedCallWaitingIndication(
      int linkedId,
      PASN_OctetString *argument
    );

    /**Attach a call waiting APDU to the passed in Alerting PDU.  The second paramter is used to
       indicate to the calling user how many additional users are "camped on" the called user.  A
       value of zero indicates to the calling user that he/she is the only user attempting to reach
       the busy called user.
    */
    virtual void AttachToAlerting(
      H323SignalPDU & pdu,
      unsigned numberOfCallsWaiting = 0
    );

    /**Sub-state for call waiting.
      */
    enum State {
      e_cw_Idle,
      e_cw_Invoked
    };

    State GetState() const { return cwState; }


  protected:
    State cwState;  // Call Waiting state of this connection
};


class H45011Handler : public H450xHandler
{
  PCLASSINFO(H45011Handler, H450xHandler);
  public:
    H45011Handler(
      H323Connection & connection,
      H450xDispatcher & dispatcher
    );

    virtual void AttachToSetup(
      H323SignalPDU & pdu
    );

    virtual void AttachToAlerting(
      H323SignalPDU & pdu
    );

    virtual void AttachToConnect(
      H323SignalPDU & pdu
    );

    virtual void AttachToReleaseComplete(
      H323SignalPDU & pdu
    );

    virtual BOOL OnReceivedInvoke(
      int opcode,
      int invokeId,                           ///<  InvokeId of operation (used in response)
      int linkedId,                           ///<  InvokeId of associated operation (if any)
      PASN_OctetString * argument             ///<  Parameters for the initiate operation
    );

    /**Handle an incoming Call Intrusion operation
    */
    virtual void OnReceivedCallIntrusionRequest(
      int linkedId,
      PASN_OctetString *argument
    );

    /**Handle an incoming Call Intrusion GetCIPL operation
    */
    virtual void OnReceivedCallIntrusionGetCIPL(
      int linkedId,
      PASN_OctetString *argument
    );

    /**Handle an incoming Call Intrusion Isolate operation
    */
    virtual void OnReceivedCallIntrusionIsolate(
      int linkedId,
      PASN_OctetString *argument
    );

    /**Handle an incoming Call Intrusion Forced Release operation
    */
    virtual BOOL OnReceivedCallIntrusionForcedRelease(
      int linkedId,
      PASN_OctetString *argument
    );

    /**Handle an incoming Call Intrusion WOB operation
    */
    virtual void OnReceivedCallIntrusionWOBRequest(
      int linkedId,
      PASN_OctetString *argument
    );

    /**Handle an incoming Call Intrusion Silent Monitor operation
    */
    virtual void OnReceivedCallIntrusionSilentMonitor(
      int linkedId,
      PASN_OctetString *argument
    );

    /**Handle an incoming Call Intrusion Notification operation
    */
    virtual void OnReceivedCallIntrusionNotification(
      int linkedId,
      PASN_OctetString *argument
    );

    /**Handle an incoming Call Intrusion cfb Override operation
    */
    virtual void OnReceivedCfbOverride(
      int linkedId,
      PASN_OctetString *argument
    );

    /**Handle an incoming Call Intrusion Remote User Alerting operation
    */
    virtual void OnReceivedRemoteUserAlerting(
      int linkedId,
      PASN_OctetString *argument
    );

    /**Handle an incoming Call Intrusion Call Waiting operation
    */
    virtual void OnReceivedCallWaiting(
      int linkedId,
      PASN_OctetString *argument
    );

    virtual BOOL OnReceivedReturnResult(
      X880_ReturnResult & returnResult
    );

    void OnReceivedCIRequestResult(/*X880_ReturnResult & returnResult*/);

    virtual BOOL OnReceivedReturnError(
      int errorCode,
      X880_ReturnError & returnError
    );

    BOOL OnReceivedInvokeReturnError (
      int errorCode,
      const bool timerExpiry = false ///<  Flag to indicate expiry
    );

    void OnReceivedCIGetCIPLResult(
      X880_ReturnResult & returnResult
    );

    BOOL OnReceivedGetCIPLReturnError(
      int errorCode,
      const bool timerExpiry = false ///<  Flag to indicate expiry
    );

    void IntrudeCall(int CICL );

    void AwaitSetupResponse(
      const PString & token,
      const PString & identity
    );

    BOOL GetRemoteCallIntrusionProtectionLevel(
      const PString & intrusionCallToken,
      unsigned intrusionCICL
    );

    void SetIntrusionImpending();

    void SetForcedReleaseAccepted();

    void SetIntrusionNotAuthorized();

    virtual BOOL OnReceivedReject(
      int problemType,
      int problemNumber
    );

    /**Sub-state for call intrusion.
      */
    enum State {
      e_ci_Idle,
      e_ci_WaitAck,
      e_ci_GetCIPL,
      e_ci_OrigInvoked,
      e_ci_OrigIsolated,
      e_ci_DestNotify,
      e_ci_DestInvoked,
      e_ci_DestIsolated,
      e_ci_DestWOB,
      e_ci_IsolationRequest,
      e_ci_ForcedReleaseRequest,
      e_ci_WOBRequest
    };

    /**What to generate */
    enum Generate{
      e_ci_gIdle,
      e_ci_gConferenceRequest,
      e_ci_gHeldRequest,
      e_ci_gSilentMonitorRequest,
      e_ci_gIsolationRequest,
      e_ci_gForcedReleaseRequest,
      e_ci_gWOBRequest
    };

    /** When to send SS message */
    enum SendState{
      e_ci_sIdle,
      e_ci_sAttachToSetup,
      e_ci_sAttachToAlerting,
      e_ci_sAttachToConnect,
      e_ci_sAttachToReleseComplete
    };

    /** What to return */
    enum ReturnState{
      e_ci_rIdle,
      e_ci_rCallIntrusionImpending,
      e_ci_rCallIntruded,
      e_ci_rCallIsolated,
      e_ci_rCallForceReleased,
      e_ci_rCallForceReleaseResult,
      e_ci_rCallIntrusionComplete,
      e_ci_rCallIntrusionEnd,
      e_ci_rNotBusy,
      e_ci_rTempUnavailable,
      e_ci_rNotAuthorized
    };
    
    /**Get the current call intrusion state.
     */
    State GetState() const { return ciState; }

    /** Start the Call Intrusion Timer using the specified time interval.
     */
    void StartciTimer(const PTimeInterval value) { ciTimer = value; }

    /** Stop the Call Intrusion Timer
     */
    void StopciTimer();

    /**Is the Call Intrusion Timer running?
     */
    BOOL IsctTimerRunning() { return ciTimer.IsRunning(); }

    /**Callback mechanism for Call Intrusion Timers CI-T1, CI-T2, CI-T3 & CI-T4 & CI-T5 & CI-T6
     */
    PDECLARE_NOTIFIER(PTimer, H45011Handler, OnCallIntrudeTimeOut);

  protected:
    State       ciState;               // Call state of this connection
    PTimer      ciTimer;               // Call Intrusion Timer - Handles all six timers CI-T1 to CI-T6,
    PString     intrudingCallToken;
    PString     intrudingCallIdentity;
    PString     activeCallToken;
    ReturnState ciReturnState;
    SendState   ciSendState;
    Generate    ciGenerateState;
    int         ciCICL;
    unsigned    intrudingCallCICL;
};


#endif // __OPAL_H450PDU_H


/////////////////////////////////////////////////////////////////////////////
