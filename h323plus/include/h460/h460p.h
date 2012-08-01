/*
 * h460p.h
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
 * $Log: h460p.h,v $
 * Revision 1.1  2008/01/29 04:38:12  shorne
 * completed Initial implementation
 *
 *
 *
 */


#pragma once

#include "openh323buildopts.h"

#ifdef H323_H460

#include <h460/h460pres.h>

// Derive you implementation from H323PresenceHandler.

class H323PresenceSubscriptions;
class H323PresenceNotifications;
class H323PresenceIdentifiers;
class H323PresenceInstructions;
class H323PresenceHandler  : public PObject
{
    PCLASSINFO(H323PresenceHandler, PObject);

public:
	bool ReceivedPDU(const H225_EndpointIdentifier * id,const PASN_OctetString & pdu);

    enum MsgType {
      e_Status,
      e_Instruct,
      e_Authorize,
      e_Notify,
      e_Request,
      e_Response,
      e_Alive,
      e_Remove,
      e_Alert
    };

// Callbacks
	virtual void OnNotification(MsgType /*tag*/,
								const H225_EndpointIdentifier * /*id*/, 
								const H460P_PresenceNotification & /*notify*/) {}
	virtual void OnSubscription(MsgType /*tag*/,
								const H225_EndpointIdentifier * /*id*/, 
								const H460P_PresenceSubscription & /*subscription*/) {}
	virtual void OnInstructions(MsgType /*tag*/,
								const H225_EndpointIdentifier * /*id*/, 
								const H460P_ArrayOf_PresenceInstruction & /*instruction*/) {}
	virtual void OnIdentifiers(MsgType /*tag*/,
								const H460P_ArrayOf_PresenceIdentifier & /*identifier*/) {}

// Build Messages
     H460P_PresenceStatus & BuildStatus(H460P_PresenceMessage & msg, 
									const H323PresenceNotifications & not,
									const H323PresenceInstructions & inst);
     H460P_PresenceInstruct & BuildInstruct(H460P_PresenceMessage & msg, 
									const H323PresenceInstructions & inst);
     H460P_PresenceAuthorize & BuildAuthorize(H460P_PresenceMessage & msg, 
									const H323PresenceSubscriptions & subs);
     H460P_PresenceNotify & BuildNotify(H460P_PresenceMessage & msg, 
									const H323PresenceNotifications & not);
     H460P_PresenceRequest & BuildRequest(H460P_PresenceMessage & msg, 
									const H323PresenceSubscriptions & subs);
     H460P_PresenceResponse & BuildResponse(H460P_PresenceMessage & msg, 
									const H323PresenceSubscriptions & subs);
     H460P_PresenceAlive & BuildAlive(H460P_PresenceMessage & msg, 
									const H323PresenceIdentifiers & id);
     H460P_PresenceRemove & BuildRemove(H460P_PresenceMessage & msg, 
									const H323PresenceIdentifiers & id);
     H460P_PresenceAlert & BuildAlert(H460P_PresenceMessage & msg, 
									const H323PresenceNotifications & not);
};


class H323PresenceNotification : public H460P_PresenceNotification
{

public:

	enum States {
      e_hidden,
      e_available,
      e_online,
      e_offline,
      e_onCall,
      e_voiceMail,
      e_notAvailable,
	  e_generic
	};

	static PString GetStateString(unsigned state);

    void SetPresenceState(States state, const PString & display = PString());
    void SetGenericState(const PString & state);
	void GetPresenceState(States & state, PString & display);

	void AddSubscriber(const OpalGloballyUniqueID & guid);
	OpalGloballyUniqueID GetSubscriber(PINDEX i);
	void RemoveSubscribers();
    void AddAlias(const PString & alias);
	PString GetAlias();


};

class H323PresenceNotifications : public H460P_ArrayOf_PresenceNotification  
{

  public:
     void Add(const H323PresenceNotification & not);
};

class H323PresenceSubscription : public H460P_PresenceSubscription
{

public:
	H323PresenceSubscription();

 // Sending Gatekeeper
	void SetSubscriptionDetails(const PString & subscribe, const PStringList & aliases);
	void GetSubscriberDetails(PStringList & aliases);
	PString GetSubscribed();

	void SetGatekeeperRAS(const H323TransportAddress & address);
	H323TransportAddress GetGatekeeperRAS();

 // Receiving Gatekeeper/Endpoint
	void MakeDecision(bool approve);
	bool IsDecisionMade();
	int IsApproved(); // -1 not decided; 0 - not approved; 1 - approved;
	void SetTimeToLive(int t);
	int GetTimeToLive();

	void SetSubscription(const OpalGloballyUniqueID & guid);
    OpalGloballyUniqueID GetSubscription();

protected:
	void SetApproved(bool success);
    
};

class H323PresenceSubscriptions : public H460P_ArrayOf_PresenceSubscription 
{
   public:
	   void Add(const H323PresenceSubscription & sub);
};

class H323PresenceInstruction  :  public H460P_PresenceInstruction
{

 public:
	enum Instruction {
	  e_subscribe,
      e_unsubscribe,
      e_block,
      e_unblock
	};

	static PString GetInstructionString(unsigned instruct);
 
    H323PresenceInstruction(Instruction instruct, const PString & alias);
	Instruction GetInstruction();
	PString GetAlias();
};

class H323PresenceInstructions  : public H460P_ArrayOf_PresenceInstruction
{
  public:
	void Add(const H323PresenceInstruction & instruct);
	H323PresenceInstruction & operator[](PINDEX i) const;
};

class H323PresenceIdentifiers   : public H460P_ArrayOf_PresenceIdentifier
{

  public:
	void Add(const OpalGloballyUniqueID & guid);
	OpalGloballyUniqueID GetIdentifier(PINDEX i);
};

#endif


