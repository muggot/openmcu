// H4601.h:
/*
 * Virteos H.460 Implementation for the h323plus Library.
 *
 * Virteos is a Trade Mark of ISVO (Asia) Pte Ltd.
 *
 * Copyright (c) 2004 ISVO (Asia) Pte Ltd. All Rights Reserved.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
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
 * The Original Code is derived from and used in conjunction with the 
 * OpenH323 Project (www.openh323.org/)
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte Ltd.
 *
 *
 * Contributor(s): ______________________________________.
 *
* $Log: h4601.h,v $
* Revision 1.3  2008/01/02 17:50:59  shorne
* Fix for memory leak in H.460 module
*
* Revision 1.2  2007/10/30 09:38:48  shorne
* Better Linux interoperability and fix for small memory leak
*
* Revision 1.1  2007/08/06 20:50:51  shorne
* First commit of h323plus
*
*
*/

#if !defined(_H460_H)
#define _H460_H


#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include "h225.h"
#include "transports.h"
#include "guid.h"
#include <ptlib/pluginmgr.h>
#include <ptclib/url.h>


#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

///////////////////////////////////////////////////////////////////////////////

template<class H225>
class H460 : public H225 {
public:
	
 /**@name PDU Handling */
    /**Handle a received PDU.
       Update in the internal state from the received PDU.
       Returns FALSE is PDU is not sutiable for the class type.
      */
    virtual void OnReceivedPDU(
      const H225 & /*id*/
	  ) {};

    /**Handle a sent PDU.
       Set the PDU fields from in the internal state.
       Returns FALSE is PDU cannot be created.
      */
    virtual BOOL OnSendingPDU(
      H225 & /*id*/
	  ) const { return FALSE;};
	
protected:

};

class OpalOID : public PASN_ObjectId
{
public:

	OpalOID();

	OpalOID(
      const char * str  ///< New string to assign.
    );

	OpalOID & operator+(const char *);

	unsigned GetLastIdentifier() {
         return (*this)[GetSize()-1];
	}
};



//////////////////////////////////////////////////////////////////////////////
/**This is a base class for H.460 FeatureID.
	Feature ID's can be in 3 formats:
		Standard     : h460 series feature unsigned values;
		OID		     : Array of unsigned values;
		NonStandard  : PString Value

   The Derived Feature ID used as an index of the H460_FeatureContent
   is used to describe the parameters of a Feature.
  */

class H460_FeatureID : public H460<H225_GenericIdentifier>
{

  public:

  /**@name Construction */
  //@{
	/** Blank Feature
	*/
	H460_FeatureID();

	/** Standard Feature ID 
	*/
	H460_FeatureID(unsigned ID);

	/** OID Feature ID 
	*/
    H460_FeatureID(OpalOID ID);

	/** NonStandard Feature ID 
	*/
	H460_FeatureID(PString ID);

	H460_FeatureID(H225_GenericIdentifier ID);
  //@}

  /**@name Operators */
  //@{
	/** Standard Feature ID 
	*/
       operator unsigned () const 
		{ return ((PASN_Integer *)choice)->GetValue(); };

	/** OID Feature ID 
	*/
       operator OpalOID & () 
	   { return (OpalOID &)*choice; };

	/** NonStandard Feature ID 
	*/
       operator PString () const 
	   { return ((H225_GloballyUniqueID *)choice)->AsString(); };

    
	   PINLINE H460_FeatureID & operator=(unsigned ID);

	   PINLINE H460_FeatureID & operator=(OpalOID ID);

	   PINLINE H460_FeatureID & operator=(PString ID);
	 
	/** Get the FeatureType
	*/
	   unsigned GetFeatureType() const { return GetTag(); };

	   PString IDString() const;
  //@}

  /**@name PDictionary */
  //@{
    PObject * Clone() const;

	PObject::Comparison Compare(const PObject & obj) const;
  //@}

};


///////////////////////////////////////////////////////////////////////////////

/**This is a base class for H.323 Feature handling.
   This implements the service class session management as per H460 Series.
  */

class H460_Feature;
class H460_FeatureTable;
class H460_FeatureContent : public H460<H225_Content>
{
 
   public:

  /**@name Construction */
  //@{
	/** Blank Feature
	*/
	   H460_FeatureContent();

	/** PASN_OctetString Raw information which can 
		be deseminated by Derived Class
	*/
	   H460_FeatureContent(PASN_OctetString & param);

	/** String Value
	*/
	   H460_FeatureContent(const PString & param);

	/** Blank Feature
	*/
	   H460_FeatureContent(PASN_BMPString & param);

	/** Boolean Value
	*/
	   H460_FeatureContent(BOOL param);

	/** Integer Value
	*/
	   H460_FeatureContent(unsigned param, unsigned len);

	/** Feature Identifier
	*/
	   H460_FeatureContent(const H460_FeatureID & id);

	/** Alias Address Raw
	*/
	   H460_FeatureContent(const H225_AliasAddress & add);

	/** Alias Address Encoded
	*/
       H460_FeatureContent(const PURL & add);

	/** Transport Address
	*/
	   H460_FeatureContent(const H323TransportAddress & add);

	/** Feature Table
	*/
	   H460_FeatureContent(const H460_FeatureTable & table);

    /** GUID
	  */
	   H460_FeatureContent(const OpalGloballyUniqueID & guid);

	/** Features
	*/
	   H460_FeatureContent(H460_Feature * data);

	/** Creation from PDU
	*/
	   H460_FeatureContent(const H225_Content & param);

  //@}

  /**@name Content Operators */
  //@{
	operator PASN_OctetString () const { return *((PASN_OctetString *)choice); };
	operator PString () const
	{ 
		switch (GetTag()) {
			case e_text:  
				return ((PASN_IA5String &)*choice).GetValue();
			case e_transport: 
				return H323TransportAddress(*(H225_TransportAddress *)choice);
		}
		
		return PString();
	}; 

	operator PASN_BMPString () const { return *(PASN_BMPString *)choice; };
	operator BOOL () const { return *(PASN_Boolean *)choice; };

	operator unsigned () const 
	{ 
		switch (GetTag()) {
			case e_number8:  
			case e_number16:
			case e_number32: 
				return *(PASN_Integer*)choice;
			default: 
				return 0;
		 }
	}
		
	operator H460_FeatureID () const { return *(H225_GenericIdentifier *)choice; };
	operator H225_AliasAddress () const { return *(H225_AliasAddress *)choice; };
	operator H323TransportAddress () const { return H323TransportAddress(*(H225_TransportAddress *)choice); };
	operator H460_FeatureTable *() { return (H460_FeatureTable *)choice; };
	operator H460_Feature *() { return (H460_Feature *)choice; };

 //@} 
};

///////////////////////////////////////////////////////////////////////////////

/**This is a base class for H.323 Feature handling.
   This implements the service class session management as per H460 Series.
  */

class H460_Feature;
class H460_FeatureParameter : public H460<H225_EnumeratedParameter>
{
 
  public:

  /**@name Construction */
  //@{
    /**Create a Blank Feature.
     */
    H460_FeatureParameter();

    /**Create a new handler for a Standard H460 Feature.
     */
    H460_FeatureParameter(unsigned Identifier);

    /**Create a new handler for a NonStandard H460 Feature.
     */
	H460_FeatureParameter(const PString & Identifier);

    /**Create a new handler for a OID H460 Feature.
     */
	H460_FeatureParameter(const OpalOID & Identifier);

    /**Create a new handler from PDU
     */
	H460_FeatureParameter(const H225_EnumeratedParameter & param);

	/**Create a new Handler from FeatureID
	*/
	H460_FeatureParameter(const H460_FeatureID & ID);
  //@}

  /**@name ID Operators */
  //@{
	/** Feature Parameter ID 
	*/
	const H460_FeatureID ID() { return m_id; };

	/** Feature Parameter Contents
	*/
	 void addContent(const H460_FeatureContent & con ) 
	 { IncludeOptionalField(e_content);  m_content = con; };

	/** Replace Parameter Contents
	*/
	 void replaceContent(const H460_FeatureContent & con )
			{ if (hasContent())  
					delete &m_content; 
				m_content = con; };

	/** Feature parameter has Value
	*/
	 BOOL hasContent() 
			{ return (GetTag() == e_content); };

  //@}


  /**@name Parameter Value Operators */
  //@{
	operator PASN_OctetString &();
	operator PString &();
	operator PASN_BMPString &();
	operator BOOL ();
	operator unsigned ();
	operator H460_FeatureID &();
	operator H225_AliasAddress &();
	operator H323TransportAddress ();
	operator H225_ArrayOf_EnumeratedParameter &();
	operator PURL &();
	operator OpalGloballyUniqueID ();


	H460_FeatureContent operator=(
	const PASN_OctetString & value
	);

	H460_FeatureContent operator=(
	const PString & value
	);

	H460_FeatureContent operator=(
	const PASN_BMPString & value
	);

	H460_FeatureContent operator=(
	const BOOL & value
	);

	H460_FeatureContent operator=(
	const unsigned & value
	);

	H460_FeatureContent operator=(
	const H460_FeatureID & value
	);

	H460_FeatureContent operator=(
	const H225_AliasAddress & value
	);

	H460_FeatureContent operator=(
	const H323TransportAddress & value
	);

	H460_FeatureContent operator=(
	const H460_FeatureTable & value
	);

	H460_FeatureContent operator=(
	H460_Feature * value
	);

	H460_FeatureContent operator=(
	const OpalGloballyUniqueID & value
	);


};

///////////////////////////////////////////////////////////////////////////////

/**This is a base class for H.323 Feature handling.
   This implements the service class session management as per H460 Series.
  */

class H460_FeatureTable : public H460<H225_ArrayOf_EnumeratedParameter>

{
  public:

  /**@name Construction */
  //@{
	/** Blank Feature
	*/
	H460_FeatureTable();

	/** Contruction received from PDU 
	*/
	H460_FeatureTable(const H225_ArrayOf_EnumeratedParameter & Xparams);
  //@}

  /**@name Construction */
  //@{

	/** AddParameter
		Add a parameter to the parameter list from the
		two components which make up the parameter.
	*/
	H460_FeatureParameter & AddParameter(const H460_FeatureID & id, const H460_FeatureContent & con);

	/** AddParameter
		Add parameter without any content
	*/
	H460_FeatureParameter & AddParameter(const H460_FeatureID & id);

	/** AddParameter
		Add parameter from information received in a PDU
	*/
	void AddParameter(H225_EnumeratedParameter & Xparam);

	/** GetParameter
		Get the parameter at defined index location in 
		the parameter list. Used for iterations of the
		parameter list.
	*/
	H460_FeatureParameter & GetParameter(PINDEX id);

	/** GetParameter
		Get the parameter with the matching Feature ID. The
		paramter list is searcheed to find the parameter with the
		matching feature id. If not found returns a blank 
		feature parameter
	*/
    H460_FeatureParameter & GetParameter(const H460_FeatureID & id);

	/** GetParameterIndex
		Get the paramter list index for the feature parameter
		with the matching feature id.
	*/
	PINDEX GetParameterIndex(const H460_FeatureID & id);

	/** HasParameter
		Return True if the parameter list contains a matching
		feature parameter with the supplied feature id.
	*/
	BOOL HasParameter(const H460_FeatureID & id);

	/** RemoveParameter
		Removes Feature Parameter from the Parameter list at the 
		specified index.
	*/
	void RemoveParameter(PINDEX id);

	/** RemoveParameter
		Remove Feature Parameter from the Parameter list with the
		matching Feature ID.
	*/
	void RemoveParameter(const H460_FeatureID & id);

	/** ReplaceParameter
		Replace the Feature contents of the unique Feature parameter 
		with matching Feature ID in the parameter list.
	*/
    void ReplaceParameter(const H460_FeatureID & id, const H460_FeatureContent & con);

	/** ParameterCount
		Number of Feature Parameters in the Parameter List.
	*/
	int ParameterCount() { return GetSize(); };

	/** ParameterIsUnique
		return TRUE if there is only 1 instance of
		feature parameter with matching feature ID
		exists in the feature list. You cannot replace
		the contents of the parameter if the parameter ID
		is not unique.
	*/
	BOOL ParameterIsUnique(const H460_FeatureID & id);

	/** Operator
	*/
    inline H460_FeatureParameter & operator[](
      PINDEX id  ///* Index position in the collection of the object.
    ) const { return operator[](id); };

	/** Operator
	*/
	H460_FeatureParameter & operator[](
      PINDEX id  ///* Index position in the collection of the object.
    );
  //@}

};

///////////////////////////////////////////////////////////////////////////////

/**This is a base class for H.323 Feature handling.
   This implements the service class session management as per H460 Series.
  */
class H323EndPoint;
class H323Connection;
class H460_Feature : public H460<H225_FeatureDescriptor>
{
  public:
  /**@name Construction */
  //@{

    /**Default Constructor
     */
	H460_Feature();

    /**Create a new handler for a Standard H460 Feature.
     */
    H460_Feature(unsigned Identifier);

    /**Create a new handler for a NonStandard H460 Feature.
     */
	H460_Feature(PString Identifier);

    /**Create a new handler for a OID H460 Feature.
     */
	H460_Feature(OpalOID Indentifier);

    /**Create a new handler for a PDU Received Feature.
     */
	H460_Feature(const H225_FeatureDescriptor & descriptor);

  //@}

  /**@name Enumerators */
  //@{

	enum {
      FeatureNeeded = 1,      ///< The Feature is Needed 
      FeatureDesired,         ///< Desired Feature
      FeatureSupported        ///< Supported Feature
	} FeatureCategory;


	enum {
	  FeatureBase      =4,        ///< Create Startup use only Startup
	  FeatureBaseAll   =5,        ///< Create Startup use RAS and Signal
	  FeatureBaseRas   =6,        ///< Create Startup use RAS
	  FeatureBaseSignal=7,	      ///< Create Base use Signal
      FeatureRas       =8,        ///< Create Registering GK
	  FeatureSignal    =16        ///< Create Call Setup
	} FeatureInstance;

  //@}

  /**@name Operators */
  //@{
	/** Standard Feature ID 
	*/
       operator unsigned () const { return (H460_FeatureID)m_id; };

	/** OID Feature ID 
	*/
 //      operator OpalOID () const { return (H460_FeatureID)m_id; };

	/** NonStandard Feature ID 
	*/
       operator PString () const { return (H460_FeatureID)m_id; };

	/** Get the FeatureID 
	*/
	   H460_FeatureID GetFeatureID() { return m_id; };

	/** set the FeatureID
	*/
       void SetFeatureID(const H460_FeatureID & id) { m_id = id; };

	/** Get FeatureID as String
	*/
	   PString GetFeatureIDAsString();

	/** Get the FeatureType
	*/
	   unsigned GetFeatureType() { return ((H460_FeatureID)m_id).GetFeatureType(); };
  //@}

  /**@name Parameter Control */
  //@{
	/** Add Parameter 
	*/
	virtual H460_FeatureParameter & AddParameter(H460_FeatureID * id, const H460_FeatureContent & con);

	/** Add Parameter without contents 
	*/
	virtual H460_FeatureParameter & AddParameter(H460_FeatureID * id);

	/** Add Parameter from H460_FeatureParameter
	*/
	virtual void AddParameter(H460_FeatureParameter * param);

	/** Delete Parameter 
	*/
	virtual void RemoveParameter(PINDEX id);

	/** Replace Parameter
	*/
	virtual void ReplaceParameter(const H460_FeatureID id, const H460_FeatureContent & con);

	/** Get Parameter at index id
	*/
    H460_FeatureParameter & GetFeatureParameter(PINDEX id);

	/** Get Parameter with FeatureID
	*/
    H460_FeatureParameter & GetFeatureParameter(const H460_FeatureID & id);

	/** Has Feature with FeatureID
	*/
	BOOL HasFeatureParameter(const H460_FeatureID & id);
 
	/** Contains Parameter (Use this when casting from H225_FeatureDescriptor)
	  */
	BOOL Contains(const H460_FeatureID & id);

	/** Retrieve Parameter (Use this when casting from H225_FeatureDescriptor)
	  */
    H460_FeatureParameter & Value(const H460_FeatureID & id);

	/** Accessing the Parameters
	*/
    inline H460_FeatureParameter & operator()(
      PINDEX id  //* Index position in the collection of the object.
    ) const { return operator()(id); };

	H460_FeatureParameter & operator()(
      PINDEX id  ///* Index position in the collection of the object.
    );

	/** Accessing the Parameters
	*/
    inline H460_FeatureParameter & operator[](
      const H460_FeatureID & id  //< FeatureID of the object.
    ) const { return operator()(id); };

	H460_FeatureParameter & operator()(
      const H460_FeatureID & id  ///< FeatureID of the object.
    );

	/** Get the Number of Parameters
	*/
	int GetParameterCount() 
		{ return CurrentTable->ParameterCount(); };

	/** Get the Current Feature Table
	  */
	H460_FeatureTable & GetCurrentTable();

	/** Set the current feature table
	  */
	void SetCurrentTable(H460_FeatureTable & table);

	/** Set the curernt Feature Table from Parameter
	  */
	void SetCurrentTable(H460_FeatureParameter & param);

    /** Set to default Feature table
	  */
	void SetDefaultTable();

  //@}

  /**@name Plugin Management */
  //@{
	/** Get Feature Names 
	*/
	static PStringList GetFeatureNames(PPluginManager * pluginMgr = NULL);

	/** Get Feature Friendly Names
	*/
	static PStringList GetFeatureFriendlyNames(const PString & feature, PPluginManager * pluginMgr = NULL);

	/** Create instance of a feature 
	*/
    static H460_Feature * CreateFeature(const PString & featurename,        ///< Feature Name Expression
										int FeatureType = FeatureBase,		///< Feature Type
										PPluginManager * pluginMgr = NULL   ///< Plugin Manager
                                        );
	/** Get Feature Name. This is usually the Derived Class Name
	  */
    static PStringList GetFeatureName() { return PStringList("empty"); };

	/** Get Feature Friendly Name This usually the user friendly description
	  */
    static PStringList GetFeatureFriendlyName() { return PStringList("empty"); };

    /** Get the purpose of the the Feature whether for Signalling,RAS or both. This determines
	    when the class is instantized
	  */
	static int GetPurpose()	{ return FeatureBase; };

	/** Attach the endpoint. Override this to link your own Endpoint Instance.
	  */
	virtual void AttachEndPoint(H323EndPoint * _ep);

	/** Attach the connection object, Override this to link to your own Connection Instance
	  */
	virtual void AttachConnection(H323Connection * _con);

  //@}

  /**@name H323 RAS Interface */
  //@{
	/* These are the main calls which can be overridden to
		allow the various derived features access to the GEF
		interface.
	*/
  // PDU calls (Used in the H225_RAS Class)
    virtual BOOL OnSendGatekeeperRequest(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual BOOL OnSendGatekeeperConfirm(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual BOOL OnSendGatekeeperReject(H225_FeatureDescriptor & pdu) { return FALSE; };

    virtual void OnReceiveGatekeeperRequest(const H225_FeatureDescriptor & pdu) {};
    virtual void OnReceiveGatekeeperConfirm(const H225_FeatureDescriptor & pdu) {};
    virtual void OnReceiveGatekeeperReject(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendRegistrationRequest(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual BOOL OnSendRegistrationConfirm(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual BOOL OnSendRegistrationReject(H225_FeatureDescriptor & pdu) { return FALSE; };

    virtual void OnReceiveRegistrationRequest(const H225_FeatureDescriptor & pdu) {};
    virtual void OnReceiveRegistrationConfirm(const H225_FeatureDescriptor & pdu) {};
    virtual void OnReceiveRegistrationReject(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendAdmissionRequest(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual BOOL OnSendAdmissionConfirm(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual BOOL OnSendAdmissionReject(H225_FeatureDescriptor & pdu) { return FALSE; };

    virtual void OnReceiveAdmissionRequest(const H225_FeatureDescriptor & pdu) {};
    virtual void OnReceiveAdmissionConfirm(const H225_FeatureDescriptor & pdu) {};
    virtual void OnReceiveAdmissionReject(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendLocationRequest(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual BOOL OnSendLocationConfirm(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual BOOL OnSendLocationReject(H225_FeatureDescriptor & pdu) { return FALSE; };

    virtual void OnReceiveLocationRequest(const H225_FeatureDescriptor & pdu) {};
    virtual void OnReceiveLocationConfirm(const H225_FeatureDescriptor & pdu) {};
    virtual void OnReceiveLocationReject(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendServiceControlIndication(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual BOOL OnSendServiceControlResponse(H225_FeatureDescriptor & pdu) { return FALSE; };

    virtual void OnReceiveServiceControlIndication(const H225_FeatureDescriptor & pdu) {};
    virtual void OnReceiveServiceControlResponse(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendNonStandardMessage(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual void OnReceiveNonStandardMessage(const H225_FeatureDescriptor & pdu) {};

	virtual BOOL OnSendUnregistrationRequest(H225_FeatureDescriptor & pdu) { return FALSE; };
	virtual void OnReceiveUnregistrationRequest(const H225_FeatureDescriptor & pdu) {};

	virtual BOOL OnSendEndpoint(H225_FeatureDescriptor & pdu) { return FALSE; };
	virtual void OnReceiveEndpoint(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendInfoRequestMessage(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual void OnReceiveInfoRequestMessage(const H225_FeatureDescriptor & pdu) {};

	virtual BOOL OnSendInfoRequestResponseMessage(H225_FeatureDescriptor & pdu) { return FALSE; };
	virtual void OnReceiveInfoRequestResponseMessage(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendDisengagementRequestMessage(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual void OnReceiveDisengagementRequestMessage(const H225_FeatureDescriptor & pdu) {};

	virtual BOOL OnSendDisengagementConfirmMessage(H225_FeatureDescriptor & pdu) { return FALSE; };
	virtual void OnReceiveDisengagementConfirmMessage(const H225_FeatureDescriptor & pdu) {};
  //@}

  /**@name Signal PDU Interface */
  //@{
  // UUIE Calls (Used in the H323SignalPDU Class)
    virtual BOOL OnSendSetup_UUIE(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual void OnReceiveSetup_UUIE(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendAlerting_UUIE(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual void OnReceiveAlerting_UUIE(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendCallProceeding_UUIE(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual void OnReceiveCallProceeding_UUIE(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendCallConnect_UUIE(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual void OnReceiveCallConnect_UUIE(const H225_FeatureDescriptor & pdu) {};

    virtual BOOL OnSendFacility_UUIE(H225_FeatureDescriptor & pdu) { return FALSE; };
    virtual void OnReceiveFacility_UUIE(const H225_FeatureDescriptor & pdu) {};

	virtual BOOL OnSendReleaseComplete_UUIE(H225_FeatureDescriptor & pdu) { return FALSE; };
	virtual void OnReceiveReleaseComplete_UUIE(const H225_FeatureDescriptor & pdu) {};

	virtual BOOL OnSendUnAllocatedPDU(H225_FeatureDescriptor & pdu) { return FALSE; };
	virtual void OnReceivedUnAllocatedPDU(const H225_FeatureDescriptor & pdu) {};

  //@}

  /**@name H501 Support */
  //@{
  // H501 Calls (To Be Implemented
		// H501_MessageCommonInfo
		// H501_AddressTemplate
		// H501_ContactInformation
		// H501_RouteInformation
  //@}

  protected:
	  H460_FeatureTable * CurrentTable;
	  H323EndPoint * ep;
	  H323Connection * con;
};

class H460_FeatureStd : public H460_Feature
{
    PCLASSINFO(H460_FeatureStd, H460_Feature);
  public:

  /**@name Construction */
  //@{
	  H460_FeatureStd() {};

    /**Create a new handler for a Standard H460 Feature.
     */
	H460_FeatureStd(unsigned Identifier);
  //@}

  /**@name Parameter Control */
  //@{
	/** Add item 
	*/
	H460_FeatureParameter & Add(unsigned id, const H460_FeatureContent & con);

	/** Delete item 
	*/
	void Remove(unsigned id);

	/** Replace item 
	*/
	void Replace(unsigned id,const H460_FeatureContent & con);

	/** Has Parameter
	  */
	BOOL HasParameter(unsigned id);

	/** Get Parameter
	  */
	H460_FeatureParameter & GetParameter(unsigned id);
  //@}

	/** Operator
	*/
    inline H460_FeatureParameter & operator[] (
      unsigned id  ///* Index value of the object.
    ) { return GetParameter(id); };

};

class H460_FeatureNonStd : public H460_Feature
{
    PCLASSINFO(H460_FeatureNonStd, H460_Feature);
  public:

  /**@name Construction */
  //@{
    /**Create a new handler for a Standard H460 Feature.
     */
	H460_FeatureNonStd(PString Identifier);
  //@}

  /**@name Parameter Control */
  //@{
	/** Add item 
	*/
	H460_FeatureParameter & Add(const PString id, const H460_FeatureContent & con);

	/** Delete item 
	*/
	void Remove(const PString & id);

	/** Replace item 
	*/
	void Replace( const PString & id, const H460_FeatureContent & con);

	/** Has Parameter
	  */
	BOOL HasParameter(PString id);

  //@}

	/** Operator
	*/
    inline H460_FeatureParameter & operator[](
      PString id  ///* Index position in the collection of the object.
    ) const { return operator[](id); };

	/** Operator
	*/
	H460_FeatureParameter & operator[](
      PString id  ///* Index position in the collection of the object.
    );

};

class H460_FeatureOID : public H460_Feature
{
    PCLASSINFO(H460_FeatureOID, H460_Feature);
  public:

  /**@name Construction */
  //@{
    /**Create a new handler for a Standard H460 Feature.
     */
	H460_FeatureOID(OpalOID Identifier);
  //@}

  /**@name Parameter Control */
  //@{
	/** Add item 
	*/
	H460_FeatureParameter & Add(const PString & id, const H460_FeatureContent & con);

	/** Delete item 
	*/
	void Remove(const PString & id);

	/** Replace item 
	*/
	void Replace(const PString & id, const H460_FeatureContent & con);

	/** Has Parameter
	  */
	BOOL HasParameter(OpalOID id);

	/** Contains a Parameter
	  */
	BOOL Contains(const PString & id);

    /** Value of a parameter
	  */
	H460_FeatureParameter & Value(const PString & id);

  //@}

	/** Operator
	*/
    inline H460_FeatureParameter & operator[](
      OpalOID id  ///* Index position in the collection of the object.
    ) const { return operator[](id); };

	/** Operator
	*/
	H460_FeatureParameter & operator[](
      OpalOID id  ///* Index position in the collection of the object.
    );

  protected:
    PString GetBase();

};
///////////////////////////////////////////////////////////////////////////////
// Dictionary/List of Features

PDICTIONARY(H460_Features, H460_FeatureID , H460_Feature);

//////////////////////////////////////////////////////////////////////////////
// FeatureSet Main Calling Class
class H323EndPoint;
class H460_FeatureSet : public PObject
{
    PCLASSINFO(H460_FeatureSet, PObject);
  public:

	/** Blank Constructor
	 */
	H460_FeatureSet();

	/** Build a new featureSet from a base featureset
	 */
    H460_FeatureSet(H460_FeatureSet * _base);

	/** Build a Feature Set from the contents of a Feature Set PDU
	 */
	H460_FeatureSet(const H225_FeatureSet & fs);

    /** Build a FeatureSet from the contents of a generic data field.
	 */
	H460_FeatureSet(const H225_ArrayOf_GenericData & generic);

    /** Derive new Feature Set based on this Feature Set ie Clone this FeatureSet
	  */
	H460_FeatureSet * DeriveNewFeatureSet();

	/** Load Entire Feature Sets from PFactory loader
	   */
	virtual BOOL LoadFeatureSet(int inst = H460_Feature::FeatureBase,
		                                  H323Connection * con = NULL);

	/** Process the first PDU, This will combine the features supported by
	    the remote and local party to derive a common feature set. This will 
		remove features that are not supported be the remote.
	  */
	BOOL ProcessFirstPDU(const H225_FeatureSet & fs);

	/**  Create FeatureSet from a FeatureSet PDU
	  */
	virtual BOOL CreateFeatureSet(const H225_FeatureSet & fs);

	/** Load Feature from id.
	  */
	virtual BOOL LoadFeature(const PString & featid);
	
	/** Add a Feature to the Feature Set
	*/
	BOOL AddFeature(H460_Feature * Nfeat);

	/** Remove a Feature from the Feature Set
	*/
	void RemoveFeature(H460_FeatureID id);

	/** Get Feature with id
	*/
	H460_Feature * GetFeature(const H460_FeatureID & id);

	/** Determine if the FeatureSet has a particular FeatureID.
	  */
	BOOL HasFeature(const H460_FeatureID & feat);

	/** New Processing Paradigm
		 Main PDU & RAS link to OpenH323
	*/
	void ReceiveFeature(unsigned id, const H225_FeatureSet & Message);

	/** New Processing Paradigm
		 Main PDU & RAS link to OpenH323
	*/
	BOOL SendFeature(unsigned id, H225_FeatureSet & Message);	

	/** Attach Endpoint
	*/
	virtual void AttachEndPoint(H323EndPoint * _ep);

	/** Attach Base FeatureSet
	 */
	virtual void AttachBaseFeatureSet(H460_FeatureSet * _baseSet);

	/** Attach Endpoint to collect Events from
	  */
	H323EndPoint * GetEndPoint() { return ep; };

  protected:

   BOOL CreateFeatureSetPDU(H225_FeatureSet & fs, unsigned MessageID);

   void ReadFeatureSetPDU(const H225_FeatureSet & fs, unsigned MessageID);

   H460_FeatureID GetFeatureIDPDU(H225_FeatureDescriptor & pdu);

   BOOL CreateFeaturePDU(H460_Feature & Feat, H225_FeatureDescriptor & pdu, unsigned MessageID);
   void ReadFeaturePDU(H460_Feature & Feat, const H225_FeatureDescriptor & pdu, unsigned MessageID);

   PString PTracePDU(PINDEX id) const;

   H460_Features  Features;
   H323EndPoint * ep;
   H460_FeatureSet * baseSet;

};

/////////////////////////////////////////////////////////////////////

template <class className> class H460PluginServiceDescriptor : public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject *   CreateInstance(int /*userData*/) const { return new className; }
    virtual PStringList GetDeviceNames(int /*userData*/) const { return className::GetFeatureFriendlyName(); }
    virtual bool  ValidateDeviceName(const PString & deviceName, int userData) const 
	{ 
	     PStringList devices = className::GetFeatureName(); 
		 if ((deviceName == devices[0]) &&
			 (className::GetPurpose() >= userData) && 
			 (className::GetPurpose() < userData*2)) {

		        return TRUE; 
		 } else
			    return FALSE;
	} 
};

#define H460_FEATURE(name)    \
static H460PluginServiceDescriptor<H460_Feature##name> H460_Feature##name##_descriptor; \
PCREATE_PLUGIN(H460_Feature##name##, H460_Feature, &H460_Feature##name##_descriptor); \


#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

#endif // !defined(_H460_H)

