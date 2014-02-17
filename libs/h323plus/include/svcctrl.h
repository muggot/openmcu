/*
 * svcctrl.h
 *
 * H.225 Service Control protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 2003 Equivalence Pty. Ltd.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: svcctrl.h,v $
 * Revision 1.2  2007/08/07 22:25:46  shorne
 * update for H323_H350
 *
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.2.2.1  2007/07/20 22:03:26  shorne
 * Initial H.350 Support
 *
 * Revision 1.2  2006/05/16 11:37:11  shorne
 * Added ability to detect type of service control
 *
 * Revision 1.1  2003/04/01 01:07:22  robertj
 * Split service control handlers from H.225 RAS header.
 *
 */

#ifndef __OPAL_SVCCTRL_H
#define __OPAL_SVCCTRL_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


class H225_ServiceControlDescriptor;
class H225_ServiceControlIndication;
class H225_ServiceControlResponse;

class H248_SignalsDescriptor;
class H248_SignalRequest;

class H323EndPoint;
class H323Connection;


///////////////////////////////////////////////////////////////////////////////

/**This is a base class for H.323 Service Control Session handling.
   This implements the service class session management as per Annex K/H.323.
  */
class H323ServiceControlSession : public PObject
{
    PCLASSINFO(H323ServiceControlSession, PObject);
  public:
  /**@name Construction */
  //@{
    /**Create a new handler for a Service Control.
     */
    H323ServiceControlSession();
  //@}

  /**@name Operations */
  //@{
    /**Determine of the session is valid.
       That is has all of the data it needs to correctly encode a PDU.

       Default behaviour is pure.
      */
    virtual BOOL IsValid() const = 0;

    /**Get identification name for the Control Service.
       This function separates the dynamic data from the fundamental type of
       the control service which will cause a new session ID to be generated
       by the gatekeeper server.

       Default behaviour returns the class name.
      */
    virtual PString GetServiceControlType() const;

    /**Handle a received PDU.
       Update in the internal state from the received PDU.

       Returns FALSE is PDU is not sutiable for the class type.

       Default behaviour is pure.
      */
    virtual BOOL OnReceivedPDU(
      const H225_ServiceControlDescriptor & descriptor
    ) = 0;

    /**Handle a sent PDU.
       Set the PDU fields from in the internal state.

       Returns FALSE is PDU cannot be created.

       Default behaviour is pure.
      */
    virtual BOOL OnSendingPDU(
      H225_ServiceControlDescriptor & descriptor
    ) const = 0;

    enum ChangeType {
      OpenSession,    // H225_ServiceControlSession_reason::e_open
      RefreshSession, // H225_ServiceControlSession_reason::e_refresh
      CloseSession    // H225_ServiceControlSession_reason::e_close
    };

    /**Handle a change of the state of the Service Control Session.

       Default behaviour is pure.
      */
    virtual void OnChange(
      unsigned type,
      unsigned sessionId,
      H323EndPoint & endpoint,
      H323Connection * connection
    ) const = 0;

    enum serviceType {
       e_URL,
       e_Signal,
       e_NonStandard,
       e_CallCredit
     };

     virtual serviceType GetType() = 0;
  //@}
};


/**This class is for H.323 Service Control Session handling for HTTP.
   This implements the HTTP channel management as per Annex K/H.323.
  */
class H323HTTPServiceControl : public H323ServiceControlSession
{
    PCLASSINFO(H323HTTPServiceControl, H323ServiceControlSession);
  public:
  /**@name Construction */
  //@{
    /**Create a new handler for a Service Control.
     */
    H323HTTPServiceControl(
      const PString & url
    );

    /**Create a new handler for a Service Control, initialise to PDU.
     */
    H323HTTPServiceControl(
      const H225_ServiceControlDescriptor & contents
    );
  //@}

  /**@name Operations */
  //@{
    /**Determine of the session is valid.
       That is has all of the data it needs to correctly encode a PDU.

       Default behaviour returns TRUE if url is not an empty string.
      */
    virtual BOOL IsValid() const;

    /**Get identification name for the Control Service.
       This function separates the dynamic data from the fundamental type of
       the control service which will cause a new session ID to be generated
       by the gatekeeper server.

       Default behaviour returns the class name.
      */
    virtual PString GetServiceControlType() const;

    /**Handle a received PDU.
       Update in the internal state from the received PDU.

       Default behaviour gets the contents for an e_url.
      */
    virtual BOOL OnReceivedPDU(
      const H225_ServiceControlDescriptor & contents
    );

    /**Handle a sent PDU.
       Set the PDU fields from in the internal state.

       Default behaviour sets the contents to an e_url.
      */
    virtual BOOL OnSendingPDU(
      H225_ServiceControlDescriptor & contents
    ) const;

    /**Handle a change of the state of the Service Control Session.

       Default behaviour calls endpoint.OnHTTPServiceControl().
      */
    virtual void OnChange(
      unsigned type,
      unsigned sessionId,
      H323EndPoint & endpoint,
      H323Connection * connection
    ) const;

    serviceType GetType() { return e_URL; };

    void GetValue(PString & _url) { _url = url; }
  //@}

  protected:
    PString url;
};


/**This is a base class for H.323 Service Control Session handling for H.248.
  */
class H323H248ServiceControl : public H323ServiceControlSession
{
    PCLASSINFO(H323H248ServiceControl, H323ServiceControlSession);
  public:
  /**@name Construction */
  //@{
    /**Create a new handler for a Service Control.
     */
    H323H248ServiceControl();

    /**Create a new handler for a Service Control, initialise to PDU.
     */
    H323H248ServiceControl(
      const H225_ServiceControlDescriptor & contents
    );
  //@}

  /**@name Operations */
  //@{
    /**Handle a received PDU.
       Update in the internal state from the received PDU.

       Default behaviour converts to pdu to H248_SignalsDescriptor and calls
       that version of OnReceivedPDU().
      */
    virtual BOOL OnReceivedPDU(
      const H225_ServiceControlDescriptor & contents
    );

    /**Handle a sent PDU.
       Set the PDU fields from in the internal state.

       Default behaviour calls the H248_SignalsDescriptor version of
       OnSendingPDU() and converts that to the contents pdu.
      */
    virtual BOOL OnSendingPDU(
      H225_ServiceControlDescriptor & contents
    ) const;

    /**Handle a received PDU.
       Update in the internal state from the received PDU.

       Default behaviour calls the H248_SignalRequest version of
       OnReceivedPDU() for every element in H248_SignalsDescriptor.
      */
    virtual BOOL OnReceivedPDU(
      const H248_SignalsDescriptor & descriptor
    );

    /**Handle a sent PDU.
       Set the PDU fields from in the internal state.

       Default behaviour calls the H248_SignalRequest version of
       OnSendingPDU() and appends it to the H248_SignalsDescriptor.
      */
    virtual BOOL OnSendingPDU(
      H248_SignalsDescriptor & descriptor
    ) const;

    /**Handle a received PDU.
       Update in the internal state from the received PDU.

       Default behaviour is pure.
      */
    virtual BOOL OnReceivedPDU(
      const H248_SignalRequest & request
    ) = 0;

    /**Handle a sent PDU.
       Set the PDU fields from in the internal state.

       Default behaviour is pure.
      */
    virtual BOOL OnSendingPDU(
      H248_SignalRequest & request
    ) const = 0;

    serviceType GetType() { return e_Signal; };
  //@}
};


/**This class is for H.323 Service Control Session handling for call credit.
  */
class H323CallCreditServiceControl : public H323ServiceControlSession
{
    PCLASSINFO(H323CallCreditServiceControl, H323ServiceControlSession);
  public:
  /**@name Construction */
  //@{
    /**Create a new handler for a Service Control.
     */
    H323CallCreditServiceControl(
      const PString & amount,
      BOOL mode,
      unsigned duration = 0
    );

    /**Create a new handler for a Service Control, initialise to PDU.
     */
    H323CallCreditServiceControl(
      const H225_ServiceControlDescriptor & contents
    );
  //@}

  /**@name Operations */
  //@{
    /**Determine of the session is valid.
       That is has all of the data it needs to correctly encode a PDU.

       Default behaviour returns TRUE if amount or duration is set.
      */
    virtual BOOL IsValid() const;

    /**Handle a received PDU.
       Update in the internal state from the received PDU.

       Default behaviour gets the contents for an e_callCreditServiceControl.
      */
    virtual BOOL OnReceivedPDU(
      const H225_ServiceControlDescriptor & contents
    );

    /**Handle a sent PDU.
       Set the PDU fields from in the internal state.

       Default behaviour sets the contents to an e_callCreditServiceControl.
      */
    virtual BOOL OnSendingPDU(
      H225_ServiceControlDescriptor & contents
    ) const;

    /**Handle a change of the state of the Service Control Session.

       Default behaviour calls endpoint.OnCallCreditServiceControl() and
       optionally connection->SetEnforceDurationLimit().
      */
    virtual void OnChange(
      unsigned type,
      unsigned sessionId,
      H323EndPoint & endpoint,
      H323Connection * connection
    ) const;

    serviceType GetType() { return e_CallCredit; };

    void GetValue(PString & _amount,BOOL & _credit, unsigned & _time)
	{ _amount = amount; _credit = mode; _time = durationLimit;}
  //@}

  protected:
    PString  amount;
    BOOL     mode;
    unsigned durationLimit;
};


#ifdef H323_H350
/**This nonstandard class is for H.323 Service Control Session 
   handling for LDAP directory lookups.
  */
class H323H350ServiceControl : public H323ServiceControlSession
{
    PCLASSINFO(H323H350ServiceControl, H323ServiceControlSession);
  public:
  /**@name Construction */
  //@{
    /**Create a new handler for a Service Control.
     */
    H323H350ServiceControl(
      const PString & _ldapURL, 
	  const PString & _ldapDN
    );

    /**Create a new handler for a Service Control, initialise to PDU.
     */
    H323H350ServiceControl(
      const H225_ServiceControlDescriptor & contents
    );
  //@}

  /**@name Operations */
  //@{
    /**Determine of the session is valid.
       That is has all of the data it needs to correctly encode a PDU.

       Default behaviour returns TRUE if url is not an empty string.
      */
    virtual BOOL IsValid() const;

    /**Get identification name for the Control Service.
       This function separates the dynamic data from the fundamental type of
       the control service which will cause a new session ID to be generated
       by the gatekeeper server.

       Default behaviour returns the class name.
      */
    virtual PString GetServiceControlType() const;

    /**Handle a received PDU.
       Update in the internal state from the received PDU.

       Default behaviour gets the contents for an e_url.
      */
    virtual BOOL OnReceivedPDU(
      const H225_ServiceControlDescriptor & contents
    );

    /**Handle a sent PDU.
       Set the PDU fields from in the internal state.

       Default behaviour sets the contents to an e_url.
      */
    virtual BOOL OnSendingPDU(
      H225_ServiceControlDescriptor & contents
    ) const;

    /**Handle a change of the state of the Service Control Session.

       Default behaviour calls endpoint.OnHTTPServiceControl().
      */
    virtual void OnChange(
      unsigned type,
      unsigned sessionId,
      H323EndPoint & endpoint,
      H323Connection * connection
    ) const;

    serviceType GetType() { return e_NonStandard; };

    void GetValue(PString & _ldapURL,PString & _ldapDN) 
	      { _ldapURL = ldapURL; _ldapDN = ldapDN; }
  //@}

  protected:
    PString ldapURL;
	PString ldapDN;
};

#endif


#endif // __OPAL_SVCCTRL_H


/////////////////////////////////////////////////////////////////////////////
