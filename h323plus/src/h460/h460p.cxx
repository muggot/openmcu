/*
 * h460p.cxx
 *
 * H460 Presence class.
 *
 * h323plus library
 *
 * Copyright (c) 2008 ISVO (Asia) Pte. Ltd.
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
 * $Log: h460p.cxx,v $
 * Revision 1.2  2008/01/30 02:53:42  shorne
 * code format tidy up
 *
 * Revision 1.1  2008/01/29 04:38:13  shorne
 * completed Initial implementation
 *
 *
 *
 */

#include <ptlib.h>
#include <h323pdu.h>
#include "h460/h460p.h"

#ifdef H323_H460

static struct {
  unsigned id;
  int     notification;
  int     subscription;
  int     instruction;
  int     identifier;
  int     cryptoTokens;
} PresenceMessage_attributes[] = {
//messageId 									Notification	Subscription	Instruction	 Identifier	 Crypto/Tokens
{ H460P_PresenceMessage::e_presenceStatus			,1				,0				,2			,0          ,0 },	
{ H460P_PresenceMessage::e_presenceInstruct			,0				,0				,1			,0			,0 },		
{ H460P_PresenceMessage::e_presenceAuthorize			,0				,1				,0			,0			,0 },			
{ H460P_PresenceMessage::e_presenceNotify			,1				,0				,0			,0			,0 },				
{ H460P_PresenceMessage::e_presenceRequest			,0				,1				,0			,0			,3 },	
{ H460P_PresenceMessage::e_presenceResponse			,0				,1				,0			,0			,2 },
{ H460P_PresenceMessage::e_presenceAlive			,0				,0				,0			,1			,0 },	
{ H460P_PresenceMessage::e_presenceRemove			,0				,0				,0			,1			,0 },	
{ H460P_PresenceMessage::e_presenceAlert			,1				,0				,0			,0			,0 }				
};
//  0 - not used   1 - mandatory  2 - optional  3 - recommended


// Presence message abbreviations
const char *PresName[] = {
	"Status",
	"Instruct",
	"Authorize",
	"Notify",
	"Request",
	"Response",
	"Alive",
	"Remove",
	"Alert",
	"NotRecognized"		// for new messages 
};
const unsigned MaxPresTag =  H460P_PresenceMessage::e_presenceAlert;


///////////////////////////////////////////////////////////////////

struct H323PresenceMessage {
	PPER_Stream m_rawPDU;
	H460P_PresenceMessage m_recvPDU;
	H323PresenceHandler * m_handler;
	const H225_EndpointIdentifier * m_id;

	unsigned GetTag() const { return m_recvPDU.GetTag(); }
	const char *GetTagName() const;
};


const char *H323PresenceMessage::GetTagName() const
{
	return (GetTag() <= MaxPresTag) ? PresName[GetTag()] : PresName[MaxPresTag+1];
}

///////////////////////////////////////////////////////////////////


class H323PresenceBase  
{
 public: 
	H323PresenceBase(H323PresenceMessage *m);

	bool Process();

 protected:

    bool ReadNotification(const H460P_ArrayOf_PresenceNotification & notify);
	bool ReadSubscription(const H460P_ArrayOf_PresenceSubscription & subscription);
	bool ReadInstruction(const H460P_ArrayOf_PresenceInstruction & instruction);
	bool ReadIdentifier(const H460P_ArrayOf_PresenceIdentifier & identifier);

	virtual bool HandleNotification(bool /*opt*/) { return false; }
	virtual bool HandleSubscription(bool /*opt*/) { return false; }
	virtual bool HandleInstruction(bool /*opt*/) { return false; }
	virtual bool HandleIdentifier(bool /*opt*/) { return false; }
	virtual bool HandleCryptoTokens(bool /*opt*/) { return false; }

    unsigned tag;
    H323PresenceHandler * handler;
	const H225_EndpointIdentifier * m_id;
};

H323PresenceBase::H323PresenceBase(H323PresenceMessage *m) 
: tag(m->GetTag()), handler(m->m_handler), m_id(m->m_id)
{

}

bool H323PresenceBase::Process()
{
  if (tag > MaxPresTag) {
	 PTRACE(2,"PRESENCE\tReceived unrecognised Presence Message!");
	 return false;
  }

  if (PresenceMessage_attributes[tag].notification > 0)
	  HandleNotification((PresenceMessage_attributes[tag].notification >1));

  if (PresenceMessage_attributes[tag].subscription > 0)
	  HandleSubscription((PresenceMessage_attributes[tag].notification >1));

  if (PresenceMessage_attributes[tag].instruction > 0)
	  HandleInstruction((PresenceMessage_attributes[tag].notification >1));

  if (PresenceMessage_attributes[tag].identifier > 0)
	  HandleIdentifier((PresenceMessage_attributes[tag].notification >1));

  if (PresenceMessage_attributes[tag].cryptoTokens > 0)
	  HandleCryptoTokens((PresenceMessage_attributes[tag].notification >1));

  return true;

}

bool H323PresenceBase::ReadNotification(const H460P_ArrayOf_PresenceNotification & notify)
{
	for (PINDEX i = 0; i < notify.GetSize(); i++)
		handler->OnNotification((H323PresenceHandler::MsgType)tag, m_id, notify[i]);

	return true;
}

bool H323PresenceBase::ReadSubscription(const H460P_ArrayOf_PresenceSubscription & subscription)
{
	for (PINDEX i = 0; i < subscription.GetSize(); i++)
        	handler->OnSubscription((H323PresenceHandler::MsgType)tag, m_id, subscription[i]);

	return true;
}

bool H323PresenceBase::ReadInstruction(const H460P_ArrayOf_PresenceInstruction & instruction)
{
	handler->OnInstructions((H323PresenceHandler::MsgType)tag, m_id, instruction);
	return true;
}

bool H323PresenceBase::ReadIdentifier(const H460P_ArrayOf_PresenceIdentifier & identifier)
{
	handler->OnIdentifiers((H323PresenceHandler::MsgType)tag, identifier);
	return true;
}


////////////////////////////////////////////////////////////////////////


template<class Msg>
class H323PresencePDU : public H323PresenceBase {
  public:

	H323PresencePDU(H323PresenceMessage *m) : H323PresenceBase(m), request(m->m_recvPDU) {}
	H323PresencePDU(unsigned tag) : request(*new Msg) {}

	operator Msg & () { return request; }
	operator const Msg & () const { return request; }

  protected:
	Msg & request;
};

class H323PresenceStatus  : public H323PresencePDU<H460P_PresenceStatus>
{
  public:
	H323PresenceStatus(H323PresenceMessage *m) : H323PresencePDU<H460P_PresenceStatus>(m) {}

  protected:
	virtual bool HandleNotification(bool opt);
	virtual bool HandleInstruction(bool opt); 
};

class H323PresenceInstruct  : public H323PresencePDU<H460P_PresenceInstruct>
{
  public:
	H323PresenceInstruct(H323PresenceMessage *m) : H323PresencePDU<H460P_PresenceInstruct>(m) {}

  protected:
	virtual bool HandleInstruction(bool opt); 
};	

class H323PresenceAuthorize  : public H323PresencePDU<H460P_PresenceAuthorize>
{
  public:
	H323PresenceAuthorize(H323PresenceMessage *m) : H323PresencePDU<H460P_PresenceAuthorize>(m) {}

  protected:
	virtual bool HandleSubscription(bool opt); 
};
		
class H323PresenceNotify  : public H323PresencePDU<H460P_PresenceNotify>
{
  public:
	H323PresenceNotify(H323PresenceMessage *m) : H323PresencePDU<H460P_PresenceNotify>(m) {}

  protected:
	virtual bool HandleNotification(bool opt);
};
				
class H323PresenceRequest  : public H323PresencePDU<H460P_PresenceRequest>
{
  public:
	H323PresenceRequest(H323PresenceMessage *m) : H323PresencePDU<H460P_PresenceRequest>(m) {}

  protected:
	virtual bool HandleSubscription(bool opt); 
};

class H323PresenceResponse  : public H323PresencePDU<H460P_PresenceResponse>
{
  public:
	H323PresenceResponse(H323PresenceMessage *m) : H323PresencePDU<H460P_PresenceResponse>(m) {}

  protected:
	virtual bool HandleSubscription(bool opt);

};

class H323PresenceAlive  : public H323PresencePDU<H460P_PresenceAlive>
{
  public:
	H323PresenceAlive(H323PresenceMessage *m) : H323PresencePDU<H460P_PresenceAlive>(m) {}

  protected:
	virtual bool HandleIdentifier(bool opt); 
};


class H323PresenceRemove  : public H323PresencePDU<H460P_PresenceRemove>
{
  public:
	H323PresenceRemove(H323PresenceMessage *m) : H323PresencePDU<H460P_PresenceRemove>(m) {}

  protected:
	virtual bool HandleIdentifier(bool opt);
};

class H323PresenceAlert  : public H323PresencePDU<H460P_PresenceAlert>
{
  public:
	H323PresenceAlert(H323PresenceMessage *m) : H323PresencePDU<H460P_PresenceAlert>(m) {}

  protected:
	virtual bool HandleNotification(bool opt);

};


////////////////////////////////////////////////////////////////////////////



bool H323PresenceHandler::ReceivedPDU(const H225_EndpointIdentifier * id, const PASN_OctetString & pdu)
{
    H323PresenceMessage *m = new H323PresenceMessage;
	m->m_handler = this;
	m->m_id = id;
	PPER_Stream raw(pdu);
	m->m_rawPDU = raw;
	if (!m->m_recvPDU.Decode(raw)) {
        PTRACE(2,"PRES\tUnable to decode incoming message."); 
		return false;
	}

	H323PresenceBase * handler = NULL;
	switch (m->GetTag())
	{
		case H460P_PresenceMessage::e_presenceStatus:
			handler = new H323PresenceStatus(m);
			break;
		case H460P_PresenceMessage::e_presenceInstruct:
			handler = new H323PresenceInstruct(m);
			break;
		case H460P_PresenceMessage::e_presenceAuthorize:
			handler = new H323PresenceAuthorize(m);
			break;
		case H460P_PresenceMessage::e_presenceNotify:
			handler = new H323PresenceNotify(m);
			break;
		case H460P_PresenceMessage::e_presenceRequest:
			handler = new H323PresenceRequest(m);
			break;
		case H460P_PresenceMessage::e_presenceResponse:
			handler = new H323PresenceResponse(m);
			break;
		case H460P_PresenceMessage::e_presenceAlive:
			handler = new H323PresenceAlive(m);
			break;
		case H460P_PresenceMessage::e_presenceRemove:
			handler = new H323PresenceRemove(m);
			break;
		case H460P_PresenceMessage::e_presenceAlert:
			handler = new H323PresenceAlert(m);
			break;
		default:
			break;
	}

	if (handler != NULL && handler->Process())
			return true;

    	PTRACE(2,"PRES\tUnable to handle Message." << m->GetTagName()); 
	return false;
}

///////////////////////////////////////////////////////////////////////

template<class Msg>
class H323PresenceMsg  : public H460P_PresenceMessage
{
 public:
	Msg & Build(unsigned _tag)
	{
		SetTag(_tag);
		Msg & msg = *this;
		return msg;
	}
};


H460P_PresenceStatus &  H323PresenceHandler::BuildStatus(H460P_PresenceMessage & msg, 
						const H323PresenceNotifications & not,
						const H323PresenceInstructions & inst)
{
	H323PresenceMsg<H460P_PresenceStatus> m;
	H460P_PresenceStatus & pdu = m.Build(H460P_PresenceMessage::e_presenceStatus);
	pdu.m_notification = not;

	if (inst.GetSize() > 0) {
		pdu.IncludeOptionalField(H460P_PresenceStatus::e_instruction);
		pdu.m_instruction = inst;
	}

	msg = *(H460P_PresenceMessage *)m.Clone();
	return msg;
}

H460P_PresenceInstruct &  H323PresenceHandler::BuildInstruct(H460P_PresenceMessage & msg, const H323PresenceInstructions & inst)
{
	H323PresenceMsg<H460P_PresenceInstruct> m;
	H460P_PresenceInstruct & pdu = m.Build(H460P_PresenceMessage::e_presenceInstruct);
	pdu.m_instruction = inst;

	msg = *(H460P_PresenceMessage *)m.Clone();
	return msg;
}

H460P_PresenceAuthorize &  H323PresenceHandler::BuildAuthorize(H460P_PresenceMessage & msg, const H323PresenceSubscriptions & subs)
{
	H323PresenceMsg<H460P_PresenceAuthorize> m;
	H460P_PresenceAuthorize & pdu = m.Build(H460P_PresenceMessage::e_presenceAuthorize);
	pdu.m_subscription = subs;

	msg = *(H460P_PresenceMessage *)m.Clone();
	return msg;
}

H460P_PresenceNotify &  H323PresenceHandler::BuildNotify(H460P_PresenceMessage & msg, const H323PresenceNotifications & not)
{
	H323PresenceMsg<H460P_PresenceNotify> m;
	H460P_PresenceNotify & pdu = m.Build(H460P_PresenceMessage::e_presenceNotify);
	pdu.m_notification = not;

	msg = *(H460P_PresenceMessage *)m.Clone();
	return msg;
}
     
H460P_PresenceRequest &  H323PresenceHandler::BuildRequest(H460P_PresenceMessage & msg, const H323PresenceSubscriptions & subs)
{
	H323PresenceMsg<H460P_PresenceRequest> m;
	H460P_PresenceRequest & pdu = m.Build(H460P_PresenceMessage::e_presenceRequest);
	pdu.m_subscription = subs;

	msg = *(H460P_PresenceMessage *)m.Clone();
	return msg;
}
     
H460P_PresenceResponse &  H323PresenceHandler::BuildResponse(H460P_PresenceMessage & msg, const H323PresenceSubscriptions & subs)
{
	H323PresenceMsg<H460P_PresenceResponse> m;
	H460P_PresenceResponse & pdu = m.Build(H460P_PresenceMessage::e_presenceResponse);
	pdu.m_subscription = subs;

	msg = *(H460P_PresenceMessage *)m.Clone();
	return msg;;
}
     
H460P_PresenceAlive & H323PresenceHandler::BuildAlive(H460P_PresenceMessage & msg, const H323PresenceIdentifiers & id)
{
	H323PresenceMsg<H460P_PresenceAlive> m;
	H460P_PresenceAlive & pdu = m.Build(H460P_PresenceMessage::e_presenceAlive);
	pdu.m_identifier = id;

	msg = *(H460P_PresenceMessage *)m.Clone();
	return msg;
}
     
H460P_PresenceRemove & H323PresenceHandler::BuildRemove(H460P_PresenceMessage & msg, const H323PresenceIdentifiers & id)
{
	H323PresenceMsg<H460P_PresenceRemove> m;
	H460P_PresenceRemove & pdu = m.Build(H460P_PresenceMessage::e_presenceRemove);
	pdu.m_identifier = id;

	msg = *(H460P_PresenceMessage *)m.Clone();
	return msg;
}
     
H460P_PresenceAlert &  H323PresenceHandler::BuildAlert(H460P_PresenceMessage & msg, const H323PresenceNotifications & not)
{
	H323PresenceMsg<H460P_PresenceAlert> m;
	H460P_PresenceAlert & pdu = m.Build(H460P_PresenceMessage::e_presenceAlert);
	pdu.m_notification = not;

	msg = *(H460P_PresenceMessage *)m.Clone();
	return msg;
}

////////////////////////////////////////////////////////////////////////

bool H323PresenceStatus::HandleNotification(bool opt)
{
   if (!opt)
	   return ReadNotification(request.m_notification);

   return false;
}

bool H323PresenceNotify::HandleNotification(bool opt)
{
   if (!opt)
	   return ReadNotification(request.m_notification);

   return false;
}

bool H323PresenceAlert::HandleNotification(bool opt)
{
   if (!opt)
	   return ReadNotification(request.m_notification);

   return false;
}

bool H323PresenceAuthorize::HandleSubscription(bool opt) 
{ 
   if (!opt)
	   return ReadSubscription(request.m_subscription);

   return false; 
}

bool H323PresenceRequest::HandleSubscription(bool opt) 
{ 
   if (!opt)
	   return ReadSubscription(request.m_subscription);

   return false; 
}

bool H323PresenceResponse::HandleSubscription(bool opt) 
{ 
   if (!opt)
	   return ReadSubscription(request.m_subscription);

   return false; 
}

bool H323PresenceStatus::HandleInstruction(bool opt) 
{ 
   if (!opt || request.HasOptionalField(H460P_PresenceStatus::e_instruction))
	   return ReadInstruction(request.m_instruction);

   return false; 
}

bool H323PresenceInstruct::HandleInstruction(bool opt) 
{ 
   if (!opt)
	   return ReadInstruction(request.m_instruction);

   return false; 
}

bool H323PresenceAlive::HandleIdentifier(bool opt) 
{ 
   if (!opt)
	   return ReadIdentifier(request.m_identifier);

   return false; 
}

bool H323PresenceRemove::HandleIdentifier(bool opt) 
{ 
   if (!opt)
	   return ReadIdentifier(request.m_identifier);

   return false; 
}

///////////////////////////////////////////////////////////////////////

H323PresenceInstruction::H323PresenceInstruction(Instruction instruct, const PString & alias)
{
	SetTag((unsigned)instruct);
	H225_AliasAddress & addr = *this;
	H323SetAliasAddress(alias, addr);

}

H323PresenceInstruction::Instruction H323PresenceInstruction::GetInstruction()
{
	return (Instruction)GetTag();
}

PString H323PresenceInstruction::GetAlias()
{
	const H225_AliasAddress & addr = *this;
	return H323GetAliasAddressString(addr);
}

///////////////////////////////////////////////////////////////////////

void H323PresenceInstructions::Add(const H323PresenceInstruction & instruct)
{
    	int size = GetSize();
	SetSize(size+1);
	array.SetAt(size, instruct.Clone());
}
	
H323PresenceInstruction & H323PresenceInstructions::operator[](PINDEX i) const
{
   return (H323PresenceInstruction &)array[i];
}

static const char *InstructState[] = {
	"Subscribe",
	"Unsubscribe",
	"Block",
	"Unblock"
};

PString H323PresenceInstruction::GetInstructionString(unsigned instruct)
{
     return InstructState[instruct];
}

///////////////////////////////////////////////////////////////////////

void H323PresenceIdentifiers::Add(const OpalGloballyUniqueID & guid)
{
	H460P_PresenceIdentifier id;
	id.m_guid = guid;
	int size = GetSize();
	SetSize(size+1);
	array.SetAt(size, &id);
}

OpalGloballyUniqueID H323PresenceIdentifiers::GetIdentifier(PINDEX i)
{
	H460P_PresenceIdentifier & id = (H460P_PresenceIdentifier &)array[i];
	return OpalGloballyUniqueID(id.m_guid);
}

////////////////////////////////////////////////////////////////////////

void H323PresenceNotifications::Add(const H323PresenceNotification & not)
{
	int size = GetSize();
	SetSize(size+1);
	array.SetAt(size, not.Clone());
}


void H323PresenceNotification::SetPresenceState(H323PresenceNotification::States state, 
						const PString & display)
{
	H460P_Presentity & e = m_presentity;
	e.m_state.SetTag((unsigned)state);

	if (display.GetLength() > 0) {
		e.IncludeOptionalField(H460P_Presentity::e_display);
		e.m_display = display;
	}
}

void H323PresenceNotification::GetPresenceState(H323PresenceNotification::States & state, 
						PString & display)
{
	H460P_Presentity & e = m_presentity;
	state = (H323PresenceNotification::States)e.m_state.GetTag();

	if (state != e_generic) {
	   if (e.HasOptionalField(H460P_Presentity::e_display))
		   display = e.m_display;
	} else {
	   PASN_BMPString & m = e.m_state;
       display = m;
	}
}

void H323PresenceNotification::SetGenericState(const PString & state)
{
	H460P_Presentity & e = m_presentity;
	e.m_state.SetTag(H460P_PresenceState::e_generic);

	PASN_BMPString & display = e.m_state;
	display = state;
}

void H323PresenceNotification::AddSubscriber(const OpalGloballyUniqueID & guid)
{
	if (!HasOptionalField(H460P_PresenceNotification::e_subscribers))
		IncludeOptionalField(H460P_PresenceNotification::e_subscribers);

	H460P_PresenceIdentifier id;
	id.m_guid = guid;
	int size = m_subscribers.GetSize();
	m_subscribers.SetSize(size+1);
	m_subscribers[size] = id;
}

void H323PresenceNotification::RemoveSubscribers()
{
	if (HasOptionalField(H460P_PresenceNotification::e_subscribers)) {
		RemoveOptionalField(H460P_PresenceNotification::e_subscribers);
		m_subscribers.RemoveAll();
	}
}

OpalGloballyUniqueID H323PresenceNotification::GetSubscriber(PINDEX i)
{
	if (HasOptionalField(H460P_PresenceNotification::e_subscribers)) {
		H460P_PresenceIdentifier & id = m_subscribers[i];
		return OpalGloballyUniqueID(id.m_guid);
	}

	return OpalGloballyUniqueID();
}

void H323PresenceNotification::AddAlias(const PString & alias)
{
	if (!HasOptionalField(H460P_PresenceNotification::e_aliasAddress))
		IncludeOptionalField(H460P_PresenceNotification::e_aliasAddress);

	H323SetAliasAddress(alias, m_aliasAddress);
}

PString H323PresenceNotification::GetAlias()
{
	if (HasOptionalField(H460P_PresenceNotification::e_aliasAddress))
	     return H323GetAliasAddressString(m_aliasAddress);

 	return PString();
}

static const char *PresState[] = {
	"Hidden",
	"Available",
	"Online",
	"Offline",
	"OnCall",
	"VoiceMail",
	"NotAvailable",
	"Generic"
};

PString H323PresenceNotification::GetStateString(unsigned state)
{
	return PresState[state];
}

///////////////////////////////////////////////////////////////////////

void H323PresenceSubscriptions::Add(const H323PresenceSubscription & sub)
{
	int size = GetSize();
	SetSize(size+1);
	array.SetAt(size, sub.Clone());
}

H323PresenceSubscription::H323PresenceSubscription()
{
}

void H323PresenceSubscription::SetSubscriptionDetails(const PString & subscribe, const PStringList & aliases)
{
	H323SetAliasAddress(subscribe, m_subscribe);

	for (PINDEX i=0; i< aliases.GetSize(); i++) {
		H225_AliasAddress alias;
		H323SetAliasAddress(aliases[i],alias);
		int size = m_aliases.GetSize();
		m_aliases.SetSize(size+1);
		m_aliases[i] = alias;
	}
}

void H323PresenceSubscription::GetSubscriberDetails(PStringList & aliases)
{
	for (PINDEX i=0; i< m_aliases.GetSize(); i++) {
		PString a = H323GetAliasAddressString(m_aliases[i]);
		aliases.AppendString(a);
	}
}

PString H323PresenceSubscription::GetSubscribed()
{
	return H323GetAliasAddressString(m_subscribe);
}

void H323PresenceSubscription::SetGatekeeperRAS(const H323TransportAddress & address)
{
	IncludeOptionalField(H460P_PresenceSubscription::e_rasAddress);
	address.SetPDU(m_rasAddress);
}

H323TransportAddress H323PresenceSubscription::GetGatekeeperRAS()
{
	if (!HasOptionalField(H460P_PresenceSubscription::e_rasAddress))
		return H323TransportAddress();

	return H323TransportAddress(m_rasAddress);
}

void H323PresenceSubscription::SetApproved(bool success)
{
	if (HasOptionalField(H460P_PresenceSubscription::e_approved))
	        IncludeOptionalField(H460P_PresenceSubscription::e_approved);

	m_approved = success;
}

int H323PresenceSubscription::IsApproved()
{
	if (HasOptionalField(H460P_PresenceSubscription::e_approved))
	  return (int)m_approved;
	else
	  return -1;
}

void H323PresenceSubscription::SetTimeToLive(int t)
{
	IncludeOptionalField(H460P_PresenceSubscription::e_timeToLive);
	m_timeToLive = t;
}
	
void H323PresenceSubscription::SetSubscription(const OpalGloballyUniqueID & guid)
{
	IncludeOptionalField(H460P_PresenceSubscription::e_identifier);
	m_identifier.m_guid = guid;
}

OpalGloballyUniqueID H323PresenceSubscription::GetSubscription()
{
	if (!HasOptionalField(H460P_PresenceSubscription::e_identifier))
		return OpalGloballyUniqueID();

	return OpalGloballyUniqueID(m_identifier.m_guid);
}

void H323PresenceSubscription::MakeDecision(bool approve)
{
	SetApproved(approve);
}

bool H323PresenceSubscription::IsDecisionMade() 
{ 
	return HasOptionalField(H460P_PresenceSubscription::e_approved); 
}

#endif



