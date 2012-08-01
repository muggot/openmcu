/*
 * h230.h
 *
 * H.230 Conference control class.
 *
 * h323plus library
 *
 * Copyright (c) 2007 ISVO (Asia) Pte. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the General Public License (the  "GNU License"), in which case the
 * provisions of GNU License are applicable instead of those
 * above. If you wish to allow use of your version of this file only
 * under the terms of the GNU License and not to allow others to use
 * your version of this file under the MPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and
 * other provisions required by the GNU License. If you do not delete
 * the provisions above, a recipient may use your version of this file
 * under either the MPL or the GNU License."
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h230.h,v $
 * Revision 1.5  2008/01/22 01:17:11  shorne
 * Fixes to the H.230 system
 *
 * Revision 1.4  2007/11/19 18:06:31  shorne
 * changed lists from PList to std::list
 *
 * Revision 1.3  2007/11/07 15:45:42  willamowius
 * linux compile fix
 *
 * Revision 1.2  2007/10/18 17:44:12  shorne
 * Small fixes during test compile
 *
 * Revision 1.1  2007/10/16 16:35:44  shorne
 * Added H.230 Support
 *
 *
 *
 *
 */


#pragma once

#include <ptlib.h>
#include <h323pdu.h>
#include <gccpdu.h>
#include <list>

#ifdef H323_H230 

class H230Control  : public PObject
{
   PCLASSINFO(H230Control, PObject);

public:

	enum AddResponse
	{
      e_Addsuccess,
      e_AddinvalidRequester,
      e_AddinvalidNetworkType,
      e_AddinvalidNetworkAddress,
      e_AddaddedNodeBusy,
      e_AddnetworkBusy,
      e_AddnoPortsAvailable,
      e_AddconnectionUnsuccessful
	};

	enum LockResponse
	{
      e_Locksuccess,
      e_LockinvalidRequester,
      e_LockalreadyLocked
	};

	enum EjectResponse
	{
      e_Ejectsuccess,
      e_EjectinvalidRequester,
      e_EjectinvalidNode
	};

	enum  TransferResponse
	{
	  e_Transfersuccess,
      e_TransferinvalidRequester
	};

	class userInfo
	{
	 public:
		int       m_Token;
		PString   m_Number;
		PString   m_Name;
		PString   m_vCard;
	};

	H230Control(const PString & _h323token);


///////////////////////////////////////////
// Endpoint Functions
    BOOL Invite(const PStringList & aliases);
	BOOL LockConference();
    BOOL UnLockConference();
    BOOL EjectUser(int node);
	BOOL TransferUser(list<int> node,const PString & number);
	BOOL TerminalListRequest();
	BOOL ChairRequest(BOOL revoke);
	BOOL ChairAssign(int node);
	BOOL FloorRequest();
	BOOL FloorAssign(int node);
	BOOL WhoIsChair();
	BOOL UserEnquiry(list<int> node);

// Endpoint Events
	virtual void OnControlsEnabled(BOOL /*success*/) {};
	virtual void OnConferenceChair(BOOL /*success*/) {};
	virtual void OnConferenceFloor(BOOL /*success*/) {};
	virtual void OnInviteResponse(int /*id*/, const PString & /*calledNo*/, AddResponse /*response*/, int /*errCode*/){};
	virtual void OnLockConferenceResponse(LockResponse /*lock*/)  {};
	virtual void OnUnLockConferenceResponse(LockResponse /*lock*/)  {};
	virtual void OnEjectUserResponse(int /*node*/, EjectResponse /*lock*/) {};
	virtual void OnTransferUserResponse(list<int> /*node*/,const PString & /*number*/, TransferResponse /*result*/) {};
	virtual void OnTerminalListResponse(list<int> node) {};
	virtual void ConferenceJoined(int /*terminalId*/){};
	virtual void ConferenceLeft(int /*terminalId*/) {};
	virtual void MakeChairResponse(BOOL /*success*/) {};
	virtual void ChairAssigned(int /*node*/) {};
	virtual void FloorAssigned(int /*node*/) {};
	virtual void OnChairTokenResponse(int /*id*/, const PString & /*name*/) {};
	virtual void OnFloorRequested(int /*terminalId*/,BOOL /*cancel*/) {};
	virtual void OnUserEnquiryResponse(const list<userInfo> &) {};


///////////////////////////////////////////
// Server Events
	virtual void OnInvite(const PStringList & /*alias*/) const {};
	virtual void OnLockConference(BOOL /*state*/) const {};
    virtual void OnEjectUser(int /*node*/) const {};
	virtual void OnTransferUser(list<int> /*node*/,const PString & /*number*/) const {};
	virtual void OnTerminalListRequest() const {};
	virtual void ChairRequested(const int & /*terminalId*/,BOOL /*cancel*/) {};
	virtual void OnFloorRequest() {};
	virtual void OnChairTokenRequest() const {};
	virtual void OnChairAssign(int /*node*/) const {};
	virtual void OnFloorAssign(int /*node*/) const {};
	virtual void OnUserEnquiry(list<int>) const {};


//  Server Commands
	BOOL InviteResponse(int /*id*/, const PString & /*calledNo*/, AddResponse /*response*/, int /*errCode*/);
	BOOL LockConferenceResponse(LockResponse lock);
	BOOL UnLockConferenceResponse(LockResponse lock);
	BOOL EjectUserResponse(int node, EjectResponse lock);
	BOOL TransferUserResponse(list<int> node,const PString & number, TransferResponse result);
	BOOL TerminalListResponse(list<int> node);
	BOOL ChairTokenResponse(int termid,const PString & termname);
	BOOL ChairAssignResponse(int termid,const PString & termname);
	BOOL FloorAssignResponse(int termid,const PString & termname);
	BOOL UserEnquiryResponse(const list<userInfo> &);

// Server Indications
	BOOL ConferenceJoinedInd(int termId);
	BOOL ConferenceLeftInd(int termId);
	BOOL ConferenceTokenAssign(int mcuId,int termId);

	void SetChair(BOOL success);
	void SetFloor(BOOL success);


////////////////////////////////////////////////
// Common
 // standard incoming requests
    BOOL OnHandleConferenceRequest(const H245_ConferenceRequest &);
    BOOL OnHandleConferenceResponse(const H245_ConferenceResponse &);
    BOOL OnHandleConferenceCommand(const H245_ConferenceCommand &);
    BOOL OnHandleConferenceIndication(const H245_ConferenceIndication &);

  // Generic incoming requests
	BOOL OnHandleGenericPDU(const H245_GenericMessage & msg);


 protected:
  // H.245
	BOOL OnGeneralRequest(int request);
	BOOL OnGeneralIndication(int req, const H245_TerminalLabel & label);

	BOOL OnReceiveTerminalListResponse(const H245_ArrayOf_TerminalLabel & list);
	BOOL OnReceiveChairResponse(const H245_ConferenceResponse_makeMeChairResponse & resp);
	BOOL OnReceiveChairTokenResponse(const H245_ConferenceResponse_chairTokenOwnerResponse & resp);
	BOOL OnReceiveChairTokenRequest();
	BOOL OnReceiveChairAssignRequest(const H245_TerminalLabel & req);
	BOOL OnReceiveChairAssignResponse(const H245_ConferenceResponse_terminalIDResponse & req);
    BOOL OnReceiveFloorAssignRequest(const H245_TerminalLabel & req);
    BOOL OnReceiveFloorAssignResponse(const H245_ConferenceResponse_conferenceIDResponse & resp);
	
  // H.230
	BOOL ReceivedH230PDU(unsigned msgId, unsigned paramId, const H245_ParameterValue & value);

  // T.124
	BOOL ReceivedT124PDU(unsigned msgId, unsigned paramId, const H245_ParameterValue & value);
	BOOL OnReceivedT124Request(const GCC_RequestPDU &);
    BOOL OnReceivedT124Response(const GCC_ResponsePDU &);
    BOOL OnReceivedT124Indication(const GCC_IndicationPDU &);

    BOOL OnConferenceJoinRequest(const GCC_ConferenceJoinRequest &);
    BOOL OnConferenceAddRequest(const GCC_ConferenceAddRequest &);
    BOOL OnConferenceLockRequest(const GCC_ConferenceLockRequest &);
    BOOL OnConferenceUnlockRequest(const GCC_ConferenceUnlockRequest &);
    BOOL OnConferenceTerminateRequest(const GCC_ConferenceTerminateRequest &);
    BOOL OnConferenceEjectUserRequest(const GCC_ConferenceEjectUserRequest &);
    BOOL OnConferenceTransferRequest(const GCC_ConferenceTransferRequest &);

    BOOL OnConferenceJoinResponse(const GCC_ConferenceJoinResponse & pdu);
    BOOL OnConferenceAddResponse(const GCC_ConferenceAddResponse & pdu);
    BOOL OnConferenceLockResponse(const GCC_ConferenceLockResponse & pdu);
    BOOL OnConferenceUnlockResponse(const GCC_ConferenceUnlockResponse & pdu);
    BOOL OnConferenceEjectUserResponse(const GCC_ConferenceEjectUserResponse & pdu);
    BOOL OnConferenceTransferResponse(const GCC_ConferenceTransferResponse & pdu);
    BOOL OnFunctionNotSupportedResponse(const GCC_FunctionNotSupportedResponse & pdu);


    BOOL ReceivedPACKPDU(unsigned msgId, unsigned paramId, const H245_ParameterValue & value);

	BOOL SendPACKGenericRequest(int paramid, const PASN_OctetString & rawpdu);
	BOOL SendPACKGenericResponse(int paramid, const PASN_OctetString & rawpdu);
	BOOL OnReceivePACKRequest(const PASN_OctetString & rawpdu);
	BOOL OnReceivePACKResponse(const PASN_OctetString & rawpdu);

	virtual BOOL WriteControlPDU(const H323ControlPDU & pdu);

	void SetLocalID(int mcu, int num);
	int GetLocalID();
	void RemoveLocalID();

 	PString m_h323token;
	int m_mcuID;
    int m_userID;

    BOOL m_ConferenceChair;
	BOOL m_ConferenceFloor;
};


class H230T124PDU :  public H323ControlPDU
{
    PCLASSINFO(H230T124PDU, H323ControlPDU);

public:
	void BuildRequest(GCC_RequestPDU & pdu);
	void BuildResponse(GCC_ResponsePDU & pdu);
	void BuildIndication(GCC_IndicationPDU & pdu);

protected:
	void BuildGeneric(PASN_OctetString & pdu);

};


class H230Control_EndPoint   : public H230Control 
{
  public:
   PCLASSINFO(H230Control_EndPoint, H230Control);

	  class result {
	   public:
	     result();

         int errCode;
		 int node;
		 BOOL cancel;
		 PString name;
		 list<int> ids;
		 list<userInfo> info;
	  };

  	H230Control_EndPoint(const PString & _h323token);
	~H230Control_EndPoint();

// Chair Instructions
    BOOL ReqInvite(const PStringList & aliases);
	BOOL ReqLockConference();
    BOOL ReqUnLockConference();
    BOOL ReqEjectUser(int node);
	BOOL ReqTransferUser(list<int> node,const PString & number);
	BOOL ReqChairAssign(int node);
	BOOL ReqFloorAssign(int node);

// General Requests
	BOOL ReqTerminalList(list<int> & node);
	BOOL ReqChair(BOOL revoke);
	BOOL ReqFloor();
	BOOL ReqWhoIsChair(int & node);
	BOOL ReqUserEnquiry(list<int> node, list<userInfo> & info);

// conference Indications
	virtual void OnControlsEnabled(BOOL /*success*/) {};
	virtual void OnConferenceChair(BOOL /*success*/) {};
	virtual void OnConferenceFloor(BOOL /*success*/) {};


// Inherited
	virtual void ConferenceJoined(int /*terminalId*/){};
	virtual void ConferenceJoinInfo(int /*termId*/, PString /*number*/, PString /*name*/, PString /*vcard*/) {}; 
	virtual void ConferenceLeft(int /*terminalId*/) {};
	virtual void OnFloorRequested(int /*terminalId*/,BOOL /*cancel*/) {};
	virtual void OnChairAssigned(int /*node*/) {};
	virtual void OnFloorAssigned(int /*node*/) {};
	virtual void OnInviteResponse(int /*id*/, const PString & /*calledNo*/, AddResponse /*response*/, int /*errCode*/) {};

//////////////////////////////////////////////////////////
// Endpoint Responses
	void OnLockConferenceResponse(LockResponse /*lock*/);
	void OnUnLockConferenceResponse(LockResponse /*lock*/);
	void OnEjectUserResponse(int /*node*/, EjectResponse /*lock*/);
	void OnTransferUserResponse(list<int> /*node*/,const PString & /*number*/, TransferResponse /*result*/);
	void OnTerminalListResponse(list<int> node);
	void MakeChairResponse(BOOL /*success*/);
	void OnChairTokenResponse(int /*id*/, const PString & /*name*/);
	void OnUserEnquiryResponse(const list<userInfo> &);
	void ChairAssigned(int /*node*/);
	void FloorAssigned(int /*node*/);

 protected:
	PMutex requestMutex;
	PTimedMutex responseMutex;
	result * res;

};

#endif
