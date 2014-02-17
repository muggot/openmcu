/*
 * h230.cxx
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
 * $Log: h230.cxx,v $
 * Revision 1.5  2008/01/22 01:17:11  shorne
 * Fixes to the H.230 system
 *
 * Revision 1.4  2007/11/19 18:06:32  shorne
 * changed lists from PList to std::list
 *
 * Revision 1.3  2007/11/07 15:45:43  willamowius
 * linux compile fix
 *
 * Revision 1.2  2007/10/18 17:44:21  shorne
 * Small fixes during test compile
 *
 * Revision 1.1  2007/10/16 16:35:44  shorne
 * Added H.230 Support
 *
 *
 *
 *
 */

#include <ptlib.h>
#include "h230/h230.h"
#include "h230/OID2.h"

#ifdef H323_H230 

static const char * h230OID  = "0.0.8.230.2";      // Advertised Feature
static const char * T124OID  = "0.0.20.124.2";                // Advertised Feature
static const char * PACKOID  = "1.3.6.1.4.1.17090.0.2.0";      // Advertised Feature

H230Control::H230Control(const PString & _h323token)
:  m_h323token(_h323token)
{
	m_userID = -1;
	m_mcuID = 0;
	m_ConferenceChair = FALSE;
	m_ConferenceFloor = FALSE;
}

BOOL H230Control::OnHandleConferenceRequest(const H245_ConferenceRequest & req)
{
  switch (req.GetTag()) {
	case H245_ConferenceRequest::e_terminalListRequest :
    case H245_ConferenceRequest::e_makeMeChair :
    case H245_ConferenceRequest::e_cancelMakeMeChair :
		 return OnGeneralRequest(req.GetTag());

    case H245_ConferenceRequest::e_requestChairTokenOwner :
         return OnReceiveChairTokenRequest();

    case H245_ConferenceRequest::e_requestTerminalID :  // highjacked for chair assign
		 return OnReceiveChairAssignRequest(req);
    
	case H245_ConferenceRequest::e_sendThisSource :  // highjacked for floor assign
		 return OnReceiveFloorAssignRequest(req);
       
 //   Not Supported
    case H245_ConferenceRequest::e_dropTerminal :
    case H245_ConferenceRequest::e_requestAllTerminalIDs :
    case H245_ConferenceRequest::e_enterH243Password :
    case H245_ConferenceRequest::e_enterH243TerminalID :
    case H245_ConferenceRequest::e_enterH243ConferenceID :
    case H245_ConferenceRequest::e_enterExtensionAddress :
    case H245_ConferenceRequest::e_makeTerminalBroadcaster :

    case H245_ConferenceRequest::e_requestTerminalCertificate :
    case H245_ConferenceRequest::e_broadcastMyLogicalChannel :
    case H245_ConferenceRequest::e_remoteMCRequest :
	default:
		return FALSE;
  }
}

BOOL H230Control::TerminalListRequest()
{
	if (m_userID < 0) {
		PTRACE(4,"H230\tRequest denied: No conference token");
		return FALSE;
	}

	H323ControlPDU pdu;
	H245_ConferenceRequest & req = pdu.Build(H245_RequestMessage::e_conferenceRequest);
	req.SetTag(H245_ConferenceRequest::e_terminalListRequest);
    return WriteControlPDU(pdu);
}

BOOL H230Control::ChairRequest(BOOL revoke)
{
	if (m_userID < 0) {
		PTRACE(4,"H230\tRequest denied: No conference token");
		return FALSE;
	}

	H323ControlPDU pdu;
	H245_ConferenceRequest & req = pdu.Build(H245_RequestMessage::e_conferenceRequest);
	if (revoke) 
	   req.SetTag(H245_ConferenceRequest::e_cancelMakeMeChair);
	else
	   req.SetTag(H245_ConferenceRequest::e_makeMeChair);

    return WriteControlPDU(pdu);
}

BOOL H230Control::ChairAssign(int node)
{
	if (!m_ConferenceChair) {  // only a conference chair can assign another
		PTRACE(4,"H230\tRequest denied: Not conference chair");
		return FALSE; 
	}

	H323ControlPDU pdu;
	H245_ConferenceRequest & req = pdu.Build(H245_RequestMessage::e_conferenceRequest);
	req.SetTag(H245_ConferenceRequest::e_requestTerminalID);

	H245_TerminalLabel & label = req;
	label.m_mcuNumber = m_mcuID; 
	label.m_terminalNumber = node;
	
    return WriteControlPDU(pdu);
}

BOOL H230Control::FloorAssign(int node)
{
	if (!m_ConferenceChair) {  // only a conference chair can assign another
		PTRACE(4,"H230\tRequest denied: Not conference chair");
		return FALSE; 
	}

	H323ControlPDU pdu;
	H245_ConferenceRequest & req = pdu.Build(H245_RequestMessage::e_conferenceRequest);
	req.SetTag(H245_ConferenceRequest::e_requestTerminalID);

	H245_TerminalLabel & label = req;
	label.m_mcuNumber = m_mcuID; 
	label.m_terminalNumber = node;
	
    return WriteControlPDU(pdu);
}

BOOL H230Control::WhoIsChair()
{
	if (m_userID < 0) {
		PTRACE(4,"H230\tRequest denied: No conference token");
		return FALSE;
	}

	H323ControlPDU pdu;
	H245_ConferenceRequest & req = pdu.Build(H245_RequestMessage::e_conferenceRequest);
	req.SetTag(H245_ConferenceRequest::e_requestChairTokenOwner);

    return WriteControlPDU(pdu);
}

BOOL H230Control::OnReceiveChairAssignRequest(const H245_TerminalLabel & req)
{
	const H245_TerminalNumber & num = req.m_terminalNumber;
    OnChairAssign(num);
	return TRUE;
}

BOOL H230Control::OnReceiveFloorAssignRequest(const H245_TerminalLabel & req)
{
	const H245_TerminalNumber & num = req.m_terminalNumber;
    OnFloorAssign(num);
	return TRUE;
}


BOOL H230Control::OnReceiveChairTokenRequest()
{
	OnChairTokenRequest();
	return TRUE;
}

BOOL H230Control::OnGeneralRequest(int request)
{
	switch (request) {
	   case H245_ConferenceRequest::e_terminalListRequest :
		   OnTerminalListRequest();
       case H245_ConferenceRequest::e_makeMeChair :
		   ChairRequested(m_userID,FALSE);
		   break;
       case H245_ConferenceRequest::e_cancelMakeMeChair :
		   ChairRequested(m_userID,TRUE);
		   break;
	   default:
		   break;
	}
	return TRUE;
}

BOOL H230Control::OnHandleConferenceResponse(const H245_ConferenceResponse & resp)
{
  switch (resp.GetTag()) {
    case H245_ConferenceResponse::e_terminalListResponse :
		return OnReceiveTerminalListResponse(resp);
    case H245_ConferenceResponse::e_makeMeChairResponse :
		return OnReceiveChairResponse(resp);
    case H245_ConferenceResponse::e_chairTokenOwnerResponse :
		return OnReceiveChairTokenResponse(resp);
    case H245_ConferenceResponse::e_terminalIDResponse :  // Hijack for Chair Assign
		return OnReceiveChairAssignResponse(resp);
	case H245_ConferenceResponse::e_conferenceIDResponse : // Hijack for Floor Assign
		return OnReceiveFloorAssignResponse(resp);

	case H245_ConferenceResponse::e_mCTerminalIDResponse :
    case H245_ConferenceResponse::e_passwordResponse :
    case H245_ConferenceResponse::e_videoCommandReject :
    case H245_ConferenceResponse::e_terminalDropReject :

  // Not Implemented
    case H245_ConferenceResponse::e_extensionAddressResponse :
    case H245_ConferenceResponse::e_terminalCertificateResponse :
    case H245_ConferenceResponse::e_broadcastMyLogicalChannelResponse :
    case H245_ConferenceResponse::e_makeTerminalBroadcasterResponse :
    case H245_ConferenceResponse::e_sendThisSourceResponse :
    case H245_ConferenceResponse::e_requestAllTerminalIDsResponse :
    case H245_ConferenceResponse::e_remoteMCResponse :
	default:
		return FALSE;
  }
}

BOOL H230Control::OnReceiveChairTokenResponse(const H245_ConferenceResponse_chairTokenOwnerResponse & resp)
{
	const H245_TerminalLabel & label = resp.m_terminalLabel;
	const H245_TerminalNumber & number = label.m_terminalNumber;
	int num = number;

    const H245_TerminalID & id = resp.m_terminalID;
	PString name = id.AsString();

	OnChairTokenResponse(num, name);

	return TRUE;
}

BOOL H230Control::OnReceiveChairAssignResponse(const H245_ConferenceResponse_terminalIDResponse & resp)
{
	const H245_TerminalLabel & label = resp.m_terminalLabel;
	const H245_TerminalNumber & number = label.m_terminalNumber;
	int num = number;

	if (num < m_userID) {
		m_ConferenceChair = TRUE;
		OnConferenceChair(TRUE);
	} else if (m_ConferenceChair) {
        m_ConferenceChair = FALSE;
        OnConferenceChair(FALSE);
	} 
	    
	ChairAssigned(num);
	return TRUE;
}

BOOL H230Control::OnReceiveFloorAssignResponse(const H245_ConferenceResponse_conferenceIDResponse & resp)
{
	const H245_TerminalLabel & label = resp.m_terminalLabel;
	const H245_TerminalNumber & number = label.m_terminalNumber;
	int num = number;

	if (num < m_userID) {
		m_ConferenceFloor = TRUE;
		OnConferenceFloor(TRUE);
	} else if (m_ConferenceChair) {
        m_ConferenceFloor = FALSE;
        OnConferenceFloor(FALSE);
	}

	FloorAssigned(num);
	return TRUE;
}

BOOL H230Control::ChairTokenResponse(int termid , const PString & name) 
{
	H323ControlPDU pdu;
	H245_ConferenceResponse & resp = pdu.Build(H245_ResponseMessage::e_conferenceResponse);
	resp.SetTag(H245_ConferenceResponse::e_chairTokenOwnerResponse);

	H245_ConferenceResponse_chairTokenOwnerResponse & chair = resp;
	H245_TerminalLabel & label = chair.m_terminalLabel;
	H245_TerminalNumber & number = label.m_terminalNumber;
	number = termid;
	H245_McuNumber & mcu = label.m_mcuNumber;
	mcu = m_mcuID;  

    H245_TerminalID & id = chair.m_terminalID;
	id = name;

	return WriteControlPDU(pdu);
}

BOOL H230Control::ChairAssignResponse(int termid , const PString & termname) 
{
	H323ControlPDU pdu;
	H245_ConferenceResponse & resp = pdu.Build(H245_ResponseMessage::e_conferenceResponse);
	resp.SetTag(H245_ConferenceResponse::e_terminalIDResponse);

	H245_ConferenceResponse_terminalIDResponse & rep = resp; 

	H245_TerminalLabel & label = rep.m_terminalLabel;
	H245_TerminalNumber & number = label.m_terminalNumber;
	number = termid;
	H245_McuNumber & mcu = label.m_mcuNumber;
	mcu = m_mcuID;  

    H245_TerminalID & id = rep.m_terminalID;
	id.SetValue(termname);

	return WriteControlPDU(pdu);
}

BOOL H230Control::FloorAssignResponse(int termid,const PString & termname)
{
	H323ControlPDU pdu;
	H245_ConferenceResponse & resp = pdu.Build(H245_ResponseMessage::e_conferenceResponse);
	resp.SetTag(H245_ConferenceResponse::e_conferenceIDResponse);

	H245_ConferenceResponse_conferenceIDResponse & rep = resp; 

	H245_TerminalLabel & label = rep.m_terminalLabel;
	H245_TerminalNumber & number = label.m_terminalNumber;
	number = termid;
	H245_McuNumber & mcu = label.m_mcuNumber;
	mcu = m_mcuID;  

    H245_ConferenceID & id = rep.m_conferenceID;
	id.SetValue(termname);

	return WriteControlPDU(pdu);
}

BOOL H230Control::OnReceiveChairResponse(const H245_ConferenceResponse_makeMeChairResponse & resp)
{
	switch (resp.GetTag()) {
	  case H245_ConferenceResponse_makeMeChairResponse::e_grantedChairToken:
		  MakeChairResponse(TRUE);
		  break;
	  case H245_ConferenceResponse_makeMeChairResponse::e_deniedChairToken:
		  MakeChairResponse(FALSE);
		  break;
	}
	return TRUE;
}

BOOL H230Control::OnReceiveTerminalListResponse(const H245_ArrayOf_TerminalLabel & termlist)
{
    list<int> node;
	for (PINDEX i = 0; i < termlist.GetSize(); i++)
        node.push_back(termlist[i].m_terminalNumber);

	OnTerminalListResponse(node);
	return TRUE;
}


BOOL H230Control::TerminalListResponse(list<int> node)
{
	H323ControlPDU pdu;
	H245_ConferenceResponse & resp = pdu.Build(H245_ResponseMessage::e_conferenceResponse);
	resp.SetTag(H245_ConferenceResponse::e_terminalListResponse);
	H245_ArrayOf_TerminalLabel & termlist = resp;

	termlist.SetSize(node.size());
	int i = 0;
	for (list<int>::iterator r = node.begin(); r != node.end(); ++r) {
		termlist[i].m_mcuNumber = m_mcuID;  
        termlist[i].m_terminalNumber = *r;
		i++;
	}

    return WriteControlPDU(pdu);
}



BOOL H230Control::OnHandleConferenceCommand(const H245_ConferenceCommand & command)
{

  switch (command.GetTag()) {
    case H245_ConferenceCommand::e_broadcastMyLogicalChannel :
    case H245_ConferenceCommand::e_cancelBroadcastMyLogicalChannel :
    case H245_ConferenceCommand::e_makeTerminalBroadcaster :
    case H245_ConferenceCommand::e_sendThisSource :
    case H245_ConferenceCommand::e_cancelMakeTerminalBroadcaster :
    case H245_ConferenceCommand::e_cancelSendThisSource :
    case H245_ConferenceCommand::e_dropConference :
    case H245_ConferenceCommand::e_substituteConferenceIDCommand :
	default:
		return FALSE;
  }
}

BOOL H230Control::OnHandleConferenceIndication(const H245_ConferenceIndication & ind)
{
  switch (ind.GetTag()) {
	 case H245_ConferenceIndication::e_terminalNumberAssign :
	 case H245_ConferenceIndication::e_terminalJoinedConference :
	 case H245_ConferenceIndication::e_terminalLeftConference :
 	 case H245_ConferenceIndication::e_floorRequested :
     case H245_ConferenceIndication::e_requestForFloor :
		 return OnGeneralIndication(ind.GetTag(),ind);

  // Not Implemented
 	 case H245_ConferenceIndication::e_sbeNumber :
	 case H245_ConferenceIndication::e_seenByAtLeastOneOther :
	 case H245_ConferenceIndication::e_cancelSeenByAtLeastOneOther :
	 case H245_ConferenceIndication::e_seenByAll :
	 case H245_ConferenceIndication::e_cancelSeenByAll :
	 case H245_ConferenceIndication::e_terminalYouAreSeeing :
	 case H245_ConferenceIndication::e_withdrawChairToken :
	 case H245_ConferenceIndication::e_terminalYouAreSeeingInSubPictureNumber :
	 case H245_ConferenceIndication::e_videoIndicateCompose :
	 default:
		 return FALSE;
  }
}


BOOL H230Control::ConferenceJoinedInd(int termId)
{
    H323ControlPDU pdu;
    H245_ConferenceIndication & ind = pdu.Build(H245_IndicationMessage::e_conferenceIndication);
    ind.SetTag(H245_ConferenceIndication::e_terminalJoinedConference);
    H245_TerminalLabel & terminalId = ind;
    terminalId.m_mcuNumber      = m_mcuID;  
    terminalId.m_terminalNumber = termId;

	return WriteControlPDU(pdu);
}


BOOL H230Control::ConferenceLeftInd(int termId)
{
    H323ControlPDU pdu;
    H245_ConferenceIndication & ind = pdu.Build(H245_IndicationMessage::e_conferenceIndication);
    ind.SetTag(H245_ConferenceIndication::e_terminalLeftConference);
    H245_TerminalLabel & terminalId = ind;
    terminalId.m_mcuNumber      = m_mcuID;  
    terminalId.m_terminalNumber = termId;

	return WriteControlPDU(pdu);
}


BOOL H230Control::ConferenceTokenAssign(int mcuId, int termId)
{
	m_userID = termId;
	m_mcuID = mcuId;
	
    H323ControlPDU pdu;
	H245_ConferenceIndication & ind = pdu.Build(H245_IndicationMessage::e_conferenceIndication);
	ind.SetTag(H245_ConferenceIndication::e_terminalNumberAssign);
    H245_TerminalLabel & terminalId = ind;
    terminalId.m_mcuNumber      = m_mcuID;  
    terminalId.m_terminalNumber = termId;

	return WriteControlPDU(pdu);
}


BOOL H230Control::FloorRequest()
{
	if (m_userID < 0) {
		PTRACE(4,"H230\tRequest denied: No conference token");
		return FALSE;
	}

	H323ControlPDU pdu;
	H245_ConferenceIndication & req = pdu.Build(H245_IndicationMessage::e_conferenceIndication);
	   req.SetTag(H245_ConferenceIndication::e_requestForFloor);

    return WriteControlPDU(pdu);
}

BOOL H230Control::OnGeneralIndication(int req, const H245_TerminalLabel & label)
{
	const H245_McuNumber & mcu = label.m_mcuNumber; 
	const H245_TerminalNumber & number = label.m_terminalNumber;
    int num = number;

	switch (req) {
	   case H245_ConferenceIndication::e_terminalNumberAssign :
		    PTRACE(4,"H230\tUser assigned confID: " << num);
            SetLocalID(mcu,num);
			break;
       case H245_ConferenceIndication::e_terminalJoinedConference :
		   	ConferenceJoined(num);
			break;
       case H245_ConferenceIndication::e_terminalLeftConference :
		    ConferenceLeft(num);
			break;
       case H245_ConferenceIndication::e_floorRequested :
		    OnFloorRequested(num,FALSE);
			break;
       case H245_ConferenceIndication::e_requestForFloor :
		    OnFloorRequest();
			break;
	}

	return TRUE;
}

void H230Control::SetLocalID(int mcu, int num) 
{ 
	m_mcuID = mcu;
	m_userID = num; 
	OnControlsEnabled(TRUE);
}

void H230Control::RemoveLocalID() 
{ 
	m_userID = 0; 
	OnControlsEnabled(FALSE);
}

int H230Control::GetLocalID()
{
	return m_userID;
}


///////////////////////////////////////////////////////////////////////////////////

BOOL H230Control::OnHandleGenericPDU(const H245_GenericMessage & msg)
{

    const H245_CapabilityIdentifier & id = msg.m_messageIdentifier;
     if (id.GetTag() != H245_CapabilityIdentifier::e_standard)
		 return FALSE;

    const PASN_ObjectId & val = id; 
	PString oid = val.AsString();
	 if ((oid != h230OID) && 
		 (oid != T124OID) && 
		 (oid != PACKOID)) {
	   PTRACE(5,"H230\tRecieved unknown Identifier " << oid);
	   return FALSE;
	}

	 if (!msg.HasOptionalField(H245_GenericMessage::e_messageContent)) {
		 PTRACE(5,"H230\tReceived No Message contents!");
		 return FALSE;
	 }

 	 PTRACE(5,"H230\tHandling Incoming PDU");

    unsigned msgid;
	const PASN_Integer & num = msg.m_subMessageIdentifier;
	  msgid = num;

    const H245_ArrayOf_GenericParameter & content = msg.m_messageContent;

	  if (content.GetSize() > 0) {
		for (PINDEX i=0; i < content.GetSize(); i++) {
			const H245_GenericParameter & param = content[i];
			const H245_ParameterIdentifier & id = param.m_parameterIdentifier;
			const PASN_Integer & idx = id;
			const H245_ParameterValue & genvalue = param.m_parameterValue;
			 if (oid == h230OID)
		       return ReceivedH230PDU(msgid, idx, genvalue);
			 else if (oid == T124OID)
 		       return ReceivedT124PDU(msgid, idx, genvalue);
			 else if (oid == PACKOID)
			   return ReceivedPACKPDU(msgid, idx, genvalue);
		}    
	  } else 
		  return FALSE;

	return TRUE;
}

BOOL H230Control::ReceivedH230PDU(unsigned /*msgId*/, unsigned /*paramId*/, const H245_ParameterValue & /*value*/)
{
	return FALSE;
}

BOOL H230Control::WriteControlPDU(const H323ControlPDU & /*pdu*/)
{
	return FALSE;
}



//////////////////////////////////////////////////////////////////////////////////////////////
// T.124 Tunnelling


BOOL H230Control::ReceivedT124PDU(unsigned msgId, unsigned paramId, const H245_ParameterValue & value)
{
	if ((msgId != 1) || (paramId != 1) || (value.GetTag() != H245_ParameterValue::e_octetString)) {
		PTRACE(4, "H230T124\tError: Message Incorrect Format");
		return FALSE;
	}

	PASN_OctetString val = value;
	PPER_Stream argStream(val);

	GCC_GCCPDU pdu;

	if (!pdu.Decode(argStream)) {
		PTRACE(4, "H230T124\tError decoding Message");
		return FALSE;
	}
   
	switch (pdu.GetTag()) {
		case GCC_GCCPDU::e_request:
			return OnReceivedT124Request(pdu);
		case GCC_GCCPDU::e_response:
			return OnReceivedT124Response(pdu);
		case GCC_GCCPDU::e_indication:
            return OnReceivedT124Indication(pdu);
	}
	return FALSE;
}


BOOL H230Control::OnReceivedT124Request(const GCC_RequestPDU & pdu)
{
	switch (pdu.GetTag()) {
	  case GCC_RequestPDU::e_conferenceJoinRequest:
           return OnConferenceJoinRequest(pdu);
	  case GCC_RequestPDU::e_conferenceAddRequest:
		   return OnConferenceAddRequest(pdu);
	  case GCC_RequestPDU::e_conferenceLockRequest:
		   return OnConferenceAddRequest(pdu);
	  case GCC_RequestPDU::e_conferenceUnlockRequest:
		   return OnConferenceLockRequest(pdu);
	  case GCC_RequestPDU::e_conferenceTerminateRequest:
		   return OnConferenceUnlockRequest(pdu);
	  case GCC_RequestPDU::e_conferenceEjectUserRequest:
		   return OnConferenceTerminateRequest(pdu);
	  case GCC_RequestPDU::e_conferenceTransferRequest:
		   return OnConferenceTransferRequest(pdu);

  // not supported
	  case GCC_RequestPDU::e_registryRegisterChannelRequest:
	  case GCC_RequestPDU::e_registryAssignTokenRequest:
	  case GCC_RequestPDU::e_registrySetParameterRequest:
	  case GCC_RequestPDU::e_registryRetrieveEntryRequest:
	  case GCC_RequestPDU::e_registryDeleteEntryRequest:
	  case GCC_RequestPDU::e_registryMonitorEntryRequest:
	  case GCC_RequestPDU::e_registryAllocateHandleRequest:
	  case GCC_RequestPDU::e_nonStandardRequest: 
	  default:
         return FALSE;
	}
}

BOOL H230Control::OnReceivedT124Response(const GCC_ResponsePDU & pdu)
{
	switch (pdu.GetTag()) {
	  case GCC_ResponsePDU::e_conferenceJoinResponse:
		  return OnConferenceJoinResponse(pdu);
	  case GCC_ResponsePDU::e_conferenceAddResponse:
          return OnConferenceAddResponse(pdu);
	  case GCC_ResponsePDU::e_conferenceLockResponse:
          return OnConferenceLockResponse(pdu);
	  case GCC_ResponsePDU::e_conferenceUnlockResponse:
		  return OnConferenceUnlockResponse(pdu);
	  case GCC_ResponsePDU::e_conferenceEjectUserResponse:
          return OnConferenceEjectUserResponse(pdu);
	  case GCC_ResponsePDU::e_conferenceTransferResponse:
		  return OnConferenceTransferResponse(pdu);
	  case GCC_ResponsePDU::e_functionNotSupportedResponse:
		  return OnFunctionNotSupportedResponse(pdu);

  // Not Suppported
	  case GCC_ResponsePDU::e_registryResponse:
	  case GCC_ResponsePDU::e_conferenceTerminateResponse:
	  case GCC_ResponsePDU::e_registryAllocateHandleResponse:
	  case GCC_ResponsePDU::e_nonStandardResponse:
	  default:
		  return FALSE;
	}
}

BOOL H230Control::OnReceivedT124Indication(const GCC_IndicationPDU & pdu)
{
	switch (pdu.GetTag()) {
  // Not yet Implemented
	  case GCC_IndicationPDU::e_userIDIndication:
	  case GCC_IndicationPDU::e_conferenceLockIndication:
	  case GCC_IndicationPDU::e_conferenceUnlockIndication:
	  case GCC_IndicationPDU::e_conferenceTerminateIndication:
	  case GCC_IndicationPDU::e_conferenceEjectUserIndication:
	  case GCC_IndicationPDU::e_conferenceTransferIndication:
	  case GCC_IndicationPDU::e_rosterUpdateIndication:
	  case GCC_IndicationPDU::e_applicationInvokeIndication:


 // Not Implemented
	  case GCC_IndicationPDU::e_registryMonitorEntryIndication:
	  case GCC_IndicationPDU::e_conductorAssignIndication:
	  case GCC_IndicationPDU::e_conductorReleaseIndication:
	  case GCC_IndicationPDU::e_conductorPermissionAskIndication:
	  case GCC_IndicationPDU::e_conductorPermissionGrantIndication:
	  case GCC_IndicationPDU::e_conferenceTimeRemainingIndication:
	  case GCC_IndicationPDU::e_conferenceTimeInquireIndication:
	  case GCC_IndicationPDU::e_conferenceTimeExtendIndication:
	  case GCC_IndicationPDU::e_conferenceAssistanceIndication:
	  case GCC_IndicationPDU::e_textMessageIndication:
	  case GCC_IndicationPDU::e_nonStandardIndication:
	  default:
		  return FALSE;
	}
	return FALSE;
}

BOOL H230Control::OnConferenceJoinRequest(const GCC_ConferenceJoinRequest & /*pdu*/)
{
	if (!m_ConferenceChair) {  // only a conference chair can request.
		PTRACE(4,"H230T124\tRequest denied: Not conference chair");
		return FALSE; 
	}

	// Unimplemented instead use the add request.
	return FALSE;	
}

BOOL H230Control::Invite(const PStringList & aliases)
{
	if (!m_ConferenceChair) {  // only a conference chair can add user
		PTRACE(4,"H230T124\tInvite Fail: Not conference chair");
		return FALSE; 
	}

	GCC_RequestPDU req;

	req.SetTag(GCC_RequestPDU::e_conferenceAddRequest);
    GCC_ConferenceAddRequest & pdu = req;

	GCC_UserID & id = pdu.m_requestingNode;
	id = m_userID;
    PASN_Integer & tag = pdu.m_tag;
	tag = 1;

	GCC_NetworkAddress & addr = pdu.m_networkAddress;

	addr.SetSize(aliases.GetSize());
	for (PINDEX i=0; i < aliases.GetSize(); i++) {
	  GCC_NetworkAddress_subtype & ad = addr[i];
	  ad.SetTag(GCC_NetworkAddress_subtype::e_nonStandard);
	  GCC_NonStandardParameter & num = ad;
	  num.m_key.SetTag(GCC_Key::e_object);
	  PASN_ObjectId & idp = num.m_key;
	  idp = PString(i);
	  num.m_data.SetValue(aliases[i]);
	}

	H230T124PDU gpdu;
    gpdu.BuildRequest(req);
	return WriteControlPDU(gpdu);

}

BOOL H230Control::OnConferenceAddRequest(const GCC_ConferenceAddRequest & pdu)
{
	if (!m_ConferenceChair) {  // only a conference chair can add user
		PTRACE(4,"H230T124\tRequest denied: Not conference chair");
		return FALSE; 
	}

    const GCC_NetworkAddress & addr = pdu.m_networkAddress;
	if (addr.GetSize()==0) {
		PTRACE(4,"H230T124\tRequest denied: No numbers to Add");
		return FALSE;
	}

	PStringList addresses;
	for (PINDEX i=0; i < addr.GetSize(); i++) {
	  const GCC_NetworkAddress_subtype & ad = (GCC_NetworkAddress_subtype &)addr[i];
	  if (ad.GetTag() == GCC_NetworkAddress_subtype::e_nonStandard) { 
		  const GCC_NonStandardParameter & num = ad;
		  addresses.AppendString(num.m_data.AsString());
	  }
	}

	if (addresses.GetSize() > 0)
		 OnInvite(addresses);

	return TRUE;
}

BOOL H230Control::LockConference()
{
	if (!m_ConferenceChair) {  // only a conference chair can add user
		PTRACE(4,"H230T124\tInvite Fail: Not conference chair");
		return FALSE; 
	}

	GCC_RequestPDU req;

	req.SetTag(GCC_RequestPDU::e_conferenceLockRequest);

	H230T124PDU gpdu;
    gpdu.BuildRequest(req);
	return WriteControlPDU(gpdu);
}

BOOL H230Control::OnConferenceLockRequest(const GCC_ConferenceLockRequest & /*pdu*/)
{
	if (!m_ConferenceChair) {  // only a conference chair can add user
		PTRACE(4,"H230T124\tRequest denied: Not conference chair");
		return FALSE; 
	}

	OnLockConference(TRUE);
	return TRUE;
}

BOOL H230Control::UnLockConference()
{
	if (!m_ConferenceChair) {  // only a conference chair can add user
	   PTRACE(4,"H230T124\tInvite Fail: Not conference chair");
	   return FALSE; 
	}

	GCC_RequestPDU req;

	req.SetTag(GCC_RequestPDU::e_conferenceUnlockRequest);

	H230T124PDU gpdu;
    gpdu.BuildRequest(req);
	return WriteControlPDU(gpdu);
}

BOOL H230Control::OnConferenceUnlockRequest(const GCC_ConferenceUnlockRequest & /*pdu*/)
{
	if (!m_ConferenceChair) {  // only a conference chair can add user
		PTRACE(4,"H230T124\tRequest denied: Not conference chair");
		return FALSE; 
	}

	OnLockConference(FALSE);
	return TRUE;
}

BOOL H230Control::OnConferenceTerminateRequest(const GCC_ConferenceTerminateRequest & /*pdu*/)
{
	return FALSE;
}

BOOL H230Control::EjectUser(int node)
{
	if (!m_ConferenceChair) {  // only a conference chair can add user
	   PTRACE(4,"H230T124\tInvite Fail: Not conference chair");
	   return FALSE; 
	}

	GCC_RequestPDU req;

	req.SetTag(GCC_RequestPDU::e_conferenceEjectUserRequest);
	GCC_ConferenceEjectUserRequest & pdu = req;

    GCC_UserID & id = pdu.m_nodeToEject;
    id = node;
    GCC_ConferenceEjectUserRequest_reason & reason = pdu.m_reason;
	reason = GCC_ConferenceEjectUserRequest_reason::e_userInitiated;

	H230T124PDU gpdu;
    gpdu.BuildRequest(req);
	return WriteControlPDU(gpdu);
}


BOOL H230Control::OnConferenceEjectUserRequest(const GCC_ConferenceEjectUserRequest & pdu)
{
	if (!m_ConferenceChair) {  // only a conference chair can add user
		PTRACE(4,"H230T124\tRequest denied: Not conference chair");
		return FALSE; 
	}

	OnEjectUser(pdu.m_nodeToEject);
	return TRUE;

}

BOOL H230Control::TransferUser(list<int> node,const PString & number)
{
	if (!m_ConferenceChair) {  // only a conference chair can add user
	   PTRACE(4,"H230T124\tInvite Fail: Not conference chair");
	   return FALSE; 
	}

	GCC_RequestPDU req;

	req.SetTag(GCC_RequestPDU::e_conferenceTransferRequest);
	GCC_ConferenceTransferRequest & pdu = req;

    GCC_ConferenceNameSelector & name = pdu.m_conferenceName;
	name.SetTag(GCC_ConferenceNameSelector::e_text);
	GCC_SimpleTextString & str = name;
	str.SetValue(number);

	pdu.IncludeOptionalField(GCC_ConferenceTransferRequest::e_transferringNodes);
    GCC_ArrayOf_UserID & nodes = pdu.m_transferringNodes;

	nodes.SetSize(node.size());
	int i = 0;
	for (list<int>::iterator r = node.begin(); r != node.end(); ++r) {
		nodes[i] = *r;
		i++;
	}

	H230T124PDU gpdu;
    gpdu.BuildRequest(req);
	return WriteControlPDU(gpdu);
}

BOOL H230Control::OnConferenceTransferRequest(const GCC_ConferenceTransferRequest & pdu)
{
	if (!m_ConferenceChair) {  // only a conference chair can add user
	   PTRACE(4,"H230T124\tInvite Fail: Not conference chair");
	   return FALSE; 
	}

    const GCC_ConferenceNameSelector & name = pdu.m_conferenceName;
	PString destination = PString();
	if (name.GetTag() == GCC_ConferenceNameSelector::e_text) {
	          const GCC_SimpleTextString & str = name;
			  destination = str;    
	}

    list<int> node;
	if (pdu.HasOptionalField(GCC_ConferenceTransferRequest::e_transferringNodes)) {
        const GCC_ArrayOf_UserID & nodes = pdu.m_transferringNodes;
		  for (PINDEX i=0; i < nodes.GetSize(); i++) 
			 node.push_back(nodes[i]);
	}

	OnTransferUser(node,destination);

	return TRUE;
}

BOOL H230Control::OnConferenceJoinResponse(const GCC_ConferenceJoinResponse & /*pdu*/)
{
	return FALSE;
}

BOOL H230Control::InviteResponse(int id, const PString & calledNo, AddResponse response, int errCode)
{
	GCC_ResponsePDU resp;

	resp.SetTag(GCC_ResponsePDU::e_conferenceAddResponse);
    GCC_ConferenceAddResponse & pdu = resp;

	PASN_Integer & tag = pdu.m_tag;
	tag = id;
    GCC_ConferenceAddResponse_result & result = pdu.m_result;
	result = response;

	pdu.IncludeOptionalField(GCC_ConferenceAddResponse::e_userData);
	  GCC_UserData & data = pdu.m_userData;

	  data.SetSize(2);
	  for (PINDEX i=0; i < 2; i++) {
	    GCC_UserData_subtype entry;
        entry.IncludeOptionalField(GCC_UserData_subtype::e_value);
		GCC_Key & key = entry.m_key;
		key.SetTag(GCC_Key::e_object);
		PASN_OctetString & id =key;
		id.SetValue(PString(i));
	    switch (i) {
		  case 0:
			entry.m_value.SetValue(calledNo);
		  case 1:
			entry.m_value.SetValue(PString(errCode)); 
		}
		data[i] = entry;
	  }

	H230T124PDU gpdu;
    gpdu.BuildResponse(resp);
	return WriteControlPDU(gpdu);
}

BOOL H230Control::OnConferenceAddResponse(const GCC_ConferenceAddResponse & pdu)
{
	const PASN_Integer & tag = pdu.m_tag;
	int id = tag;
    const GCC_ConferenceAddResponse_result & result = pdu.m_result;
	int response = result;
    PString calledNo = PString();
	PString errCode = "0";
	if (pdu.HasOptionalField(GCC_ConferenceAddResponse::e_userData)) {
		const GCC_UserData & data = pdu.m_userData;
		for (PINDEX i = 0; i < data.GetSize(); i++) {
		    const GCC_UserData_subtype & entry = pdu.m_userData[i];
			if (entry.HasOptionalField(GCC_UserData_subtype::e_value)) {
              const GCC_Key & key = entry.m_key;
			  if (key.GetTag() == GCC_Key::e_object) {
			   const PASN_OctetString & id = key;
		       PString k = id.AsString();
			   switch (k.AsInteger()) {
				case 0:
					calledNo = entry.m_value.AsString();
				case 1:
					errCode = entry.m_value.AsString();
			   }
			  }
			}
		}  
	}

	OnInviteResponse(id,calledNo,(AddResponse)response, errCode.AsInteger());
	return TRUE;
}

BOOL H230Control::LockConferenceResponse(LockResponse lock)
{
	GCC_ResponsePDU resp;

	resp.SetTag(GCC_ResponsePDU::e_conferenceLockResponse);
    GCC_ConferenceLockResponse & pdu = resp;
	pdu.m_result = lock;

	H230T124PDU gpdu;
    gpdu.BuildResponse(resp);
	return WriteControlPDU(gpdu);
}


BOOL H230Control::OnConferenceLockResponse(const GCC_ConferenceLockResponse & pdu)
{
	int resp = pdu.m_result;
	OnLockConferenceResponse((LockResponse)resp);
	return TRUE;
}

BOOL H230Control::UnLockConferenceResponse(LockResponse lock)
{
	GCC_ResponsePDU resp;

	resp.SetTag(GCC_ResponsePDU::e_conferenceUnlockResponse);
    GCC_ConferenceUnlockResponse & pdu = resp;
	pdu.m_result = lock;

	H230T124PDU gpdu;
    gpdu.BuildResponse(resp);
	return WriteControlPDU(gpdu);
}

BOOL H230Control::OnConferenceUnlockResponse(const GCC_ConferenceUnlockResponse & pdu)
{
	int resp = pdu.m_result;
	OnUnLockConferenceResponse((LockResponse)resp);
	return TRUE;
}

BOOL H230Control::EjectUserResponse(int node, EjectResponse lock)
{
	GCC_ResponsePDU resp;

	resp.SetTag(GCC_ResponsePDU::e_conferenceEjectUserResponse);
    GCC_ConferenceEjectUserResponse & pdu = resp;
    pdu.m_nodeToEject = node;
	pdu.m_result = lock;

	H230T124PDU gpdu;
    gpdu.BuildResponse(resp);
	return WriteControlPDU(gpdu);
}


BOOL H230Control::OnConferenceEjectUserResponse(const GCC_ConferenceEjectUserResponse & pdu)
{
	int result = pdu.m_result;
	OnEjectUserResponse(pdu.m_nodeToEject, (EjectResponse)result);
	return TRUE;
}

BOOL H230Control::TransferUserResponse(list<int> node,const PString & number,TransferResponse result)
{
	GCC_ResponsePDU resp;

	resp.SetTag(GCC_ResponsePDU::e_conferenceTransferResponse);
    GCC_ConferenceTransferResponse & pdu = resp;

    GCC_ConferenceNameSelector & name = pdu.m_conferenceName;
	name.SetTag(GCC_ConferenceNameSelector::e_text);
	GCC_SimpleTextString & str = name;
	str.SetValue(number);

	pdu.IncludeOptionalField(GCC_ConferenceTransferRequest::e_transferringNodes);
    GCC_ArrayOf_UserID & nodes = pdu.m_transferringNodes;

	nodes.SetSize(node.size());
	int i = 0;
	for (list<int>::iterator r = node.begin(); r != node.end(); ++r) {
		nodes[i] = *r;
		i++;
	}

    GCC_ConferenceTransferResponse_result & rst = pdu.m_result;
	rst = result;

	H230T124PDU gpdu;
    gpdu.BuildResponse(resp);
	return WriteControlPDU(gpdu);
}

BOOL H230Control::OnConferenceTransferResponse(const GCC_ConferenceTransferResponse & pdu)
{

    const GCC_ConferenceNameSelector & name = pdu.m_conferenceName;
	PString destination = PString();
	if (name.GetTag() == GCC_ConferenceNameSelector::e_text) {
	          const GCC_SimpleTextString & str = name;
			  destination = str;    
	}

    list<int> node;
	if (pdu.HasOptionalField(GCC_ConferenceTransferRequest::e_transferringNodes)) {
        const GCC_ArrayOf_UserID & nodes = pdu.m_transferringNodes;
		  for (PINDEX i=0; i < nodes.GetSize(); i++) 
			 node.push_back(nodes[i]);
	}

    const GCC_ConferenceTransferResponse_result & rst = pdu.m_result;
	int response = rst;

	OnTransferUserResponse(node,destination,(TransferResponse)response);

	return TRUE;
}

BOOL H230Control::OnFunctionNotSupportedResponse(const GCC_FunctionNotSupportedResponse & /*pdu*/)
{
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////

BOOL H230Control::UserEnquiry(list<int> node)
{
	if (m_userID < 0) {
		PTRACE(4,"H230\tRequest denied: No conference token");
		return FALSE;
	}

    PASN_OctetString rawpdu;

	H245_ArrayOf_TerminalLabel labels;
    labels.SetSize(node.size());
	int i = 0;
	for (list<int>::iterator r = node.begin(); r != node.end(); ++r) 
	{
        H245_TerminalLabel id;
		H245_McuNumber & mcu = id.m_mcuNumber;
		mcu = m_mcuID; 
        H245_TerminalNumber lab = id.m_terminalNumber;
		lab = (int)*r;
		labels[i] = id;
		i++;
	}
    rawpdu.EncodeSubType(labels);

	PTRACE(6,"CONF\t" << labels);

  return SendPACKGenericRequest(1,rawpdu);
}

BOOL H230Control::UserEnquiryResponse(const list<userInfo> & userlist)
{
    PASN_OctetString rawpdu;
    
	H230OID2_ParticipantList ulist;
	H230OID2_ArrayOf_Participant & users = ulist.m_list;

	users.SetSize(userlist.size());
	int i = 0;
	for (std::list<userInfo>::const_iterator r = userlist.begin(); r != userlist.end(); ++r) 
	{
        userInfo u = *r;
		H230OID2_Participant & user = users[i];
		user.m_token = u.m_Token;
		user.m_number = u.m_Number;
		if (u.m_Name.GetLength() > 0) {
		   user.IncludeOptionalField(H230OID2_Participant::e_name);
		   user.m_name = u.m_Name;
		}
		if (u.m_vCard.GetLength() > 0) {
			user.IncludeOptionalField(H230OID2_Participant::e_vCard);
		    user.m_vCard = u.m_vCard;
		}
        i++;
	}
	PTRACE(4,"H230PACK\tSending UserList " << ulist);
    rawpdu.EncodeSubType(ulist);
    return SendPACKGenericResponse(2,rawpdu);
}

BOOL H230Control::SendPACKGenericRequest(int paramid,const PASN_OctetString & rawpdu)
{
   H323ControlPDU pdu;

   H245_GenericMessage & msg = pdu.Build(H245_RequestMessage::e_genericRequest);

	msg.IncludeOptionalField(H245_GenericMessage::e_subMessageIdentifier);
    msg.IncludeOptionalField(H245_GenericMessage::e_messageContent);

    H245_CapabilityIdentifier & id = msg.m_messageIdentifier;
     id.SetTag(H245_CapabilityIdentifier::e_standard);
        PASN_ObjectId & val = id; 
        val.SetValue(PACKOID);

	PASN_Integer & num = msg.m_subMessageIdentifier;
	  num = 1;

    msg.SetTag(H245_GenericMessage::e_messageContent);
    H245_ArrayOf_GenericParameter & content = msg.m_messageContent;

	 content.SetSize(1);
	   H245_GenericParameter & param = content[0];
	   H245_ParameterIdentifier & idm = param.m_parameterIdentifier; 
	     idm.SetTag(H245_ParameterIdentifier::e_standard);
		 PASN_Integer & idx = idm;
		 idx = paramid;
		 H245_ParameterValue & genvalue = param.m_parameterValue;
		 genvalue.SetTag(H245_ParameterValue::e_octetString);
		 PASN_OctetString & valg = param.m_parameterValue;
		 valg = rawpdu;

    return WriteControlPDU(pdu);
}

BOOL H230Control::SendPACKGenericResponse(int paramid, const PASN_OctetString & rawpdu)
{
    H323ControlPDU pdu;
    H245_GenericMessage & msg = pdu.Build(H245_ResponseMessage::e_genericResponse);
      
	msg.IncludeOptionalField(H245_GenericMessage::e_subMessageIdentifier);
    msg.IncludeOptionalField(H245_GenericMessage::e_messageContent);

    H245_CapabilityIdentifier & id = msg.m_messageIdentifier;
     id.SetTag(H245_CapabilityIdentifier::e_standard);
        PASN_ObjectId & val = id; 
        val.SetValue(PACKOID);

	PASN_Integer & num = msg.m_subMessageIdentifier;
	  num = 2;

    msg.SetTag(H245_GenericMessage::e_messageContent);
    H245_ArrayOf_GenericParameter & content = msg.m_messageContent;

	 content.SetSize(1);
	   H245_GenericParameter & param = content[0];
	   H245_ParameterIdentifier & idm = param.m_parameterIdentifier; 
	     idm.SetTag(H245_ParameterIdentifier::e_standard);
		 PASN_Integer & idx = idm;
		 idx = paramid;
		 H245_ParameterValue & genvalue = param.m_parameterValue;
		 genvalue.SetTag(H245_ParameterValue::e_octetString);
		 PASN_OctetString & valg = param.m_parameterValue;
		 valg = rawpdu;

    return WriteControlPDU(pdu);
}

BOOL H230Control::ReceivedPACKPDU(unsigned msgId, unsigned paramId, const H245_ParameterValue & value)
{
	if (value.GetTag() != H245_ParameterValue::e_octetString) {
		  PTRACE(4, "H230PACK\tError: Message Incorrect Format");
		  return FALSE;
	}
    PTRACE(4, "H230PACK\tProcessing message " << paramId);
	switch (msgId) {
		case 1:
            return OnReceivePACKRequest(value);
		case 2:
            return OnReceivePACKResponse(value);
		default:
			return FALSE;
	}
}

BOOL H230Control::OnReceivePACKRequest(const PASN_OctetString & rawpdu)
{
	PPER_Stream argStream(rawpdu);
	H245_ArrayOf_TerminalLabel pdu;

	if (!pdu.Decode(argStream)) {
		PTRACE(4, "H230PACK\tError decoding Message");
		return FALSE;
	}
	PTRACE(5, "H230PACK\tDecoded Message " << pdu);

	list<int> req;
	for (PINDEX i =0; i < pdu.GetSize(); i++) 
	{
        H245_TerminalNumber lab = pdu[i].m_terminalNumber;
		req.push_back(lab);
	}

	OnUserEnquiry(req);
    return TRUE;
}

BOOL H230Control::OnReceivePACKResponse(const PASN_OctetString & rawpdu) 
{
	PPER_Stream argStream(rawpdu);
	H230OID2_ParticipantList list;

	if (!list.Decode(argStream)) {
		PTRACE(4, "H230PACK\tError decoding Message");
		return FALSE;
	}
	PTRACE(5, "H230PACK\tDecoded Message " << list);

	H230OID2_ArrayOf_Participant & users = list.m_list;

	std::list<userInfo> userlist;
	for (PINDEX i=0; i < users.GetSize(); ++i) 
	{
		userInfo * u = new userInfo();
		H230OID2_Participant & user = users[i];
		u->m_Token = user.m_token;
		u->m_Number = user.m_number;
		if (user.HasOptionalField(H230OID2_Participant::e_name))
		   u->m_Name = user.m_name;
		else
		   u->m_Name = PString();

		if (user.HasOptionalField(H230OID2_Participant::e_vCard))
		   u->m_vCard = user.m_vCard.AsString();
		else
           u->m_vCard = PString();

        PTRACE(4,"H230PACK\tReading " << u->m_Number);
		userlist.push_back(*u);
	}

	if (userlist.size() > 0)
       OnUserEnquiryResponse(userlist);

    return TRUE;
}

void H230Control::SetChair(BOOL success)
{
	m_ConferenceChair = success;
}

void H230Control::SetFloor(BOOL success)
{
	m_ConferenceFloor = success;
}

//////////////////////////////////////////////////////////////////////

void H230T124PDU::BuildRequest(GCC_RequestPDU & pdu)
{
	Build(H245_RequestMessage::e_genericRequest);
	PASN_OctetString raw;
	raw.EncodeSubType(pdu);
	BuildGeneric(raw);
}
	
void H230T124PDU::BuildResponse(GCC_ResponsePDU & pdu)
{
	Build(H245_ResponseMessage::e_genericResponse);
	PASN_OctetString raw;
	raw.EncodeSubType(pdu);
	BuildGeneric(raw);
}
	
void H230T124PDU::BuildIndication(GCC_IndicationPDU & pdu)
{
	Build(H245_IndicationMessage::e_genericIndication);
	PASN_OctetString raw;
	raw.EncodeSubType(pdu);
	BuildGeneric(raw);
}

void H230T124PDU::BuildGeneric(PASN_OctetString & pdu)
{
	H245_GenericMessage & msg = (H245_GenericMessage &)(*this);

	msg.IncludeOptionalField(H245_GenericMessage::e_subMessageIdentifier);
    msg.IncludeOptionalField(H245_GenericMessage::e_messageContent);

    H245_CapabilityIdentifier & id = msg.m_messageIdentifier;
     id.SetTag(H245_CapabilityIdentifier::e_standard);
        PASN_ObjectId & val = id; 
        val.SetValue(T124OID);

	PASN_Integer & num = msg.m_subMessageIdentifier;
	  num = 1;

    msg.SetTag(H245_GenericMessage::e_messageContent);
    H245_ArrayOf_GenericParameter & content = msg.m_messageContent;

	 content.SetSize(1);
	 H245_GenericParameter & param = content[0];
	 H245_ParameterIdentifier & idm = param.m_parameterIdentifier; 
	     idm.SetTag(H245_ParameterIdentifier::e_standard);
		 PASN_Integer & idx = idm;
		 idx = 1;
		 H245_ParameterValue & genvalue = param.m_parameterValue;
		 genvalue.SetTag(H245_ParameterValue::e_octetString);
		 PASN_OctetString & valg = genvalue;
		 valg = pdu;
}

///////////////////////////////////////////////////////////////////////////

const int requestTimeout = 15;
H230Control_EndPoint::result::result()
{
    errCode = -1;
	node = -1;
	cancel = FALSE;
	name = PString();
}

BOOL H230Control_EndPoint::ReqInvite(const PStringList & aliases)
{
	PWaitAndSignal m(requestMutex);

	return Invite(aliases);
}

BOOL H230Control_EndPoint::ReqLockConference()
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

	if (LockConference()) {
		responseMutex.Wait(requestTimeout);
		return (res->errCode == 0);
	}
	return FALSE;
}

BOOL H230Control_EndPoint::ReqUnLockConference()
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

	if (UnLockConference()) {
		responseMutex.Wait(requestTimeout);
		return (res->errCode == 0);
	}

	return FALSE;
}

BOOL H230Control_EndPoint::ReqEjectUser(int node)
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

    if (EjectUser(node)) {
		responseMutex.Wait(requestTimeout);
		return (res->errCode == 0);
	}

	return FALSE;
}
BOOL H230Control_EndPoint::ReqTransferUser(list<int> node,const PString & number)
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

	if (TransferUser(node,number)) {
		responseMutex.Wait(requestTimeout);
		return (res->errCode == 0);
	}

	return FALSE;
}

BOOL H230Control_EndPoint::ReqTerminalList(list<int> & node)
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

	if (TerminalListRequest()) {
		responseMutex.Wait(requestTimeout);
		node = res->ids;
		return (res->errCode == 0);
	}

	return FALSE;
}

BOOL H230Control_EndPoint::ReqChair(BOOL revoke)
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

	if (ChairRequest(revoke)) {
		responseMutex.Wait(requestTimeout);
		return (res->cancel);
	}

	return FALSE;
}

BOOL H230Control_EndPoint::ReqChairAssign(int node)
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

	if (ChairAssign(node)) {
		responseMutex.Wait(requestTimeout);
		return (res->node == node);
	}

	return FALSE;
}

BOOL H230Control_EndPoint::ReqFloor()
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

	if (FloorRequest()) {
		responseMutex.Wait(requestTimeout);
		return (res->errCode == 0);
	}

	return FALSE;
}

BOOL H230Control_EndPoint::ReqFloorAssign(int node)
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

	if (FloorAssign(node)) {
		responseMutex.Wait(requestTimeout); 
		return (res->node == node);
	}

	return FALSE;
}

BOOL H230Control_EndPoint::ReqWhoIsChair(int & node)
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

	if (WhoIsChair()) {
		responseMutex.Wait(requestTimeout);
		node = res->node;
		return (res->errCode == 0);
	}

	return FALSE;
}

BOOL H230Control_EndPoint::ReqUserEnquiry(list<int> node, list<userInfo> & info)
{
	PWaitAndSignal m(requestMutex);

	delete res;
	res = new result();

	if (UserEnquiry(node)) {
		responseMutex.Wait(requestTimeout);
		info = res->info;
        return (res->errCode == 0);
	}

	return FALSE;
}


H230Control_EndPoint::H230Control_EndPoint(const PString & _h323token)
: H230Control(_h323token)
{
}

H230Control_EndPoint::~H230Control_EndPoint()
{
	if (GetLocalID() > 0)
        OnControlsEnabled(FALSE);
}

void H230Control_EndPoint::OnLockConferenceResponse(LockResponse lock) 
{
	res->errCode = lock;
    responseMutex.Signal();
}

void H230Control_EndPoint::OnUnLockConferenceResponse(LockResponse lock) 
{
	res->errCode = lock;
    responseMutex.Signal();
}
void H230Control_EndPoint::OnEjectUserResponse(int node, EjectResponse lock)
{
	res->node = node;
	res->errCode = lock;
    responseMutex.Signal();
}

void H230Control_EndPoint::OnTransferUserResponse(list<int> node,const PString & number, TransferResponse result)
{
	res->ids = node;
	res->name = number;
	res->errCode = result;
    responseMutex.Signal();
}

void H230Control_EndPoint::OnTerminalListResponse(list<int> node)
{
//	res->ids = node;
//  res->errCode = 0;
	for (list<int>::iterator r = node.begin(); r != node.end(); ++r) 
	      ConferenceJoined(*r);

    responseMutex.Signal();
}

void H230Control_EndPoint::MakeChairResponse(BOOL success)
{
	res->cancel = success;
    res->errCode = 0;
    responseMutex.Signal();
}

void H230Control_EndPoint::ChairAssigned(int node)
{
//	res->node = node;
//  res->errCode = 0;
	OnChairAssigned(node);
    responseMutex.Signal();
}

void H230Control_EndPoint::FloorAssigned(int node)
{
//	res->node = node;
//  res->errCode = 0;
	OnFloorAssigned(node);
    responseMutex.Signal();
}

void H230Control_EndPoint::OnChairTokenResponse(int id, const PString & name)
{
	res->node = id;
	res->name = name;
	res->errCode = 0;
    responseMutex.Signal();
}

void H230Control_EndPoint::OnUserEnquiryResponse(const list<userInfo> & info) 
{
//	res->info = info;
//	res->errCode = 0;
	std::list<userInfo>::const_iterator r = info.begin();
	while (r != info.end()) {
	    ConferenceJoinInfo(r->m_Token,r->m_Number,r->m_Name,r->m_vCard);
		r++;
	}

    responseMutex.Signal();
}

#endif

