/*
 * h323trans.h
 *
 * H.323 Transactor handler
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
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h323trans.h,v $
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.17  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.16  2004/08/24 08:11:24  csoutheren
 * Added initial support for receiving broadcasts on Linux
 *
 * Revision 1.15  2003/12/11 05:39:04  csoutheren
 * Added storage of H.225 version in endpoint structure
 * Disabled sending RIPs to endpoints that cannot handle them
 *
 * Revision 1.14  2003/04/30 07:50:58  robertj
 * Redesigned the alternate credentials in ARQ system as old implementation
 *   was fraught with concurrency issues, most importantly it can cause false
 *   detection of replay attacks taking out an endpoint completely.
 *
 * Revision 1.13  2003/04/10 09:40:05  robertj
 * Added associated transport to new GetInterfaceAddresses() function so
 *   interfaces can be ordered according to active transport links. Improves
 *   interoperability.
 *
 * Revision 1.12  2003/04/10 01:03:58  craigs
 * Added functions to access to lists of interfaces
 *
 * Revision 1.11  2003/04/09 03:08:06  robertj
 * Fixed race condition in shutting down transactor (pure virtual call)
 *
 * Revision 1.10  2003/04/01 05:59:30  robertj
 * Fixed H.501 transaction code setting members for m_common PDU part.
 *
 * Revision 1.9  2003/04/01 04:47:48  robertj
 * Abstracted H.225 RAS transaction processing (RIP and secondary thread) in
 *   server environment for use by H.501 peer elements.
 *
 * Revision 1.8  2003/03/26 00:46:25  robertj
 * Had another go at making H323Transactor being able to be created
 *   without having a listener running.
 *
 * Revision 1.7  2003/03/25 04:56:17  robertj
 * Fixed issues to do with multiple inheritence in transaction reply cache.
 *
 * Revision 1.6  2003/03/21 05:26:45  robertj
 * Added setting of remote port in UDP transport constructor.
 *
 * Revision 1.5  2003/03/20 01:51:07  robertj
 * More abstraction of H.225 RAS and H.501 protocols transaction handling.
 *
 * Revision 1.4  2003/03/01 00:23:42  craigs
 * New PeerElement implementation
 *
 * Revision 1.3  2003/02/25 06:48:15  robertj
 * More work on PDU transaction abstraction.
 *
 * Revision 1.2  2003/02/25 03:14:58  robertj
 * Added missing virtual destructor.
 *
 * Revision 1.1  2003/02/21 05:28:39  craigs
 * Factored out code for user with peer elements
 *
 */

#ifndef __OPAL_H323TRANS_H
#define __OPAL_H323TRANS_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include "transports.h"
#include "h235auth.h"

#include <ptclib/asner.h>


class H323TransactionPDU {
  public:
    H323TransactionPDU();
    H323TransactionPDU(const H235Authenticators & auth);

    virtual ~H323TransactionPDU() { }

    virtual BOOL Read(H323Transport & transport);
    virtual BOOL Write(H323Transport & transport);

    virtual PASN_Object & GetPDU() = 0;
    virtual PASN_Choice & GetChoice() = 0;
    virtual const PASN_Object & GetPDU() const = 0;
    virtual const PASN_Choice & GetChoice() const = 0;
    virtual unsigned GetSequenceNumber() const = 0;
    virtual unsigned GetRequestInProgressDelay() const = 0;
#if PTRACING
    virtual const char * GetProtocolName() const = 0;
#endif
    virtual H323TransactionPDU * ClonePDU() const = 0;
    virtual void DeletePDU() = 0;

    const H235Authenticators & GetAuthenticators() const { return authenticators; }
    void SetAuthenticators(
      const H235Authenticators & auth
    ) { authenticators = auth; }

    H235Authenticator::ValidationResult Validate(
      const PASN_Array & clearTokens,
      unsigned clearOptionalField,
      const PASN_Array & cryptoTokens,
      unsigned cryptoOptionalField
    ) const { return authenticators.ValidatePDU(*this, clearTokens, clearOptionalField, cryptoTokens, cryptoOptionalField, rawPDU); }

    void Prepare(
      PASN_Array & clearTokens,
      unsigned clearOptionalField,
      PASN_Array & cryptoTokens,
      unsigned cryptoOptionalField
    ) { authenticators.PreparePDU(*this, clearTokens, clearOptionalField, cryptoTokens, cryptoOptionalField); }

  protected:
    H235Authenticators authenticators;
    PPER_Stream        rawPDU;
};


///////////////////////////////////////////////////////////

class H323Transactor : public PObject
{
  PCLASSINFO(H323Transactor, PObject);
  public:
  /**@name Construction */
  //@{

    /**Create a new protocol handler.
     */
    H323Transactor(
      H323EndPoint & endpoint,   ///<  Endpoint gatekeeper is associated with.
      H323Transport * transport, ///<  Transport over which to communicate.
      WORD localPort,                     ///<  Local port to listen on
      WORD remotePort                     ///<  Remote port to connect on
    );
    H323Transactor(
      H323EndPoint & endpoint,   ///<  Endpoint gatekeeper is associated with.
      const H323TransportAddress & iface, ///<  Local interface over which to communicate.
      WORD localPort,                     ///<  Local port to listen on
      WORD remotePort                     ///<  Remote port to connect on
    );

    /**Destroy protocol handler.
     */
    ~H323Transactor();
  //@}

  /**@name Overrides from PObject */
  //@{
    /**Print the name of the gatekeeper.
      */
    void PrintOn(
      ostream & strm    ///<  Stream to print to.
    ) const;
  //@}

  /**@name new operations */
  //@{
    /**Set a new transport for use by the transactor.
      */
    BOOL SetTransport(
      const H323TransportAddress & iface ///<  Local interface for transport
    );

    /**Return the list of addresses used for this peer element
      */
    H323TransportAddressArray GetInterfaceAddresses(
      BOOL excludeLocalHost = TRUE,       ///<  Flag to exclude 127.0.0.1
      H323Transport * associatedTransport = NULL
                          ///<  Associated transport for precedence and translation
    );

    /**Start the channel processing transactions
      */
    virtual BOOL StartChannel();

    /**Stop the channel processing transactions.
       Must be called in each descendants destructor.
      */
    virtual void StopChannel();

    /**Create the transaction PDU for reading.
      */
    virtual H323TransactionPDU * CreateTransactionPDU() const = 0;

    /**Handle and dispatch a transaction PDU
      */
    virtual BOOL HandleTransaction(
      const PASN_Object & rawPDU
    ) = 0;

    /**Allow for modifications to PDU on send.
      */
    virtual void OnSendingPDU(
      PASN_Object & rawPDU
    ) = 0;

    /**Write PDU to transport after executing callback.
      */
    virtual BOOL WritePDU(
      H323TransactionPDU & pdu
    );

    /**Write PDU to transport after executing callback.
      */
    virtual BOOL WriteTo(
      H323TransactionPDU & pdu,
      const H323TransportAddressArray & addresses,
      BOOL callback = TRUE
    );
  //@}

  /**@name Member variable access */
  //@{
    /**Get the gatekeepers associated endpoint.
      */
    H323EndPoint & GetEndPoint() const { return endpoint; }

    /**Get the gatekeepers transport channel.
      */
    H323Transport & GetTransport() const { return *transport; }

    /**Set flag to check all crypto tokens on responses.
      */
    void SetCheckResponseCryptoTokens(
      BOOL value    ///<  New value for checking crypto tokens.
    ) { checkResponseCryptoTokens = value; }

    /**Get flag to check all crypto tokens on responses.
      */
    BOOL GetCheckResponseCryptoTokens() { return checkResponseCryptoTokens; }
  //@}

  protected:
    void Construct();

    unsigned GetNextSequenceNumber();
    BOOL SetUpCallSignalAddresses(
      H225_ArrayOf_TransportAddress & addresses
    );

    //Background thread handler.
    PDECLARE_NOTIFIER(PThread, H323Transactor, HandleTransactions);
	
    class Request : public PObject
    {
        PCLASSINFO(Request, PObject);
      public:
        Request(
          unsigned seqNum,
          H323TransactionPDU & pdu
        );
        Request(
          unsigned seqNum,
          H323TransactionPDU & pdu,
          const H323TransportAddressArray & addresses
        );

        BOOL Poll(H323Transactor &);
        void CheckResponse(unsigned, const PASN_Choice *);
        void OnReceiveRIP(unsigned milliseconds);

        // Inter-thread transfer variables
        unsigned rejectReason;
        void   * responseInfo;

        H323TransportAddressArray requestAddresses;

        unsigned             sequenceNumber;
        H323TransactionPDU & requestPDU;
        PTimeInterval        whenResponseExpected;
        PSyncPoint           responseHandled;
        PMutex               responseMutex;

        enum {
          AwaitingResponse,
          ConfirmReceived,
          RejectReceived,
          TryAlternate,
          BadCryptoTokens,
          RequestInProgress,
          NoResponseReceived
        } responseResult;
    };

    virtual BOOL MakeRequest(
      Request & request
    );
    BOOL CheckForResponse(
      unsigned,
      unsigned,
      const PASN_Choice * = NULL
    );
    BOOL HandleRequestInProgress(
      const H323TransactionPDU & pdu,
      unsigned delay
    );
    BOOL CheckCryptoTokens(
      const H323TransactionPDU & pdu,
      const PASN_Array & clearTokens,
      unsigned clearOptionalField,
      const PASN_Array & cryptoTokens,
      unsigned cryptoOptionalField
    );

    void AgeResponses();
    BOOL SendCachedResponse(
      const H323TransactionPDU & pdu
    );

    class Response : public PString
    {
        PCLASSINFO(Response, PString);
      public:
        Response(const H323TransportAddress & addr, unsigned seqNum);
        ~Response();

        void SetPDU(const H323TransactionPDU & pdu);
        BOOL SendCachedResponse(H323Transport & transport);

        PTime                lastUsedTime;
        PTimeInterval        retirementAge;
        H323TransactionPDU * replyPDU;
    };

    // Configuration variables
    H323EndPoint  & endpoint;
    WORD            defaultLocalPort;
    WORD            defaultRemotePort;
    H323Transport * transport;
    BOOL            checkResponseCryptoTokens;

    unsigned  nextSequenceNumber;
    PMutex    nextSequenceNumberMutex;

    PDictionary<POrdinalKey, Request> requests;
    PMutex                            requestsMutex;
    Request                         * lastRequest;

    PMutex                pduWriteMutex;
    PSortedList<Response> responses;
};


////////////////////////////////////////////////////////////////////////////////////

class H323Transaction : public PObject
{
    PCLASSINFO(H323Transaction, PObject);
  public:
  /**@name Construction */
  //@{
    /**Create a new transaction handler.
     */
    H323Transaction(
      H323Transactor & transactor,
      const H323TransactionPDU & requestToCopy,
      H323TransactionPDU * confirm,
      H323TransactionPDU * reject
    );
    ~H323Transaction();
  //@}

    enum Response {
      Ignore = -2,
      Reject = -1,
      Confirm = 0
    };
    inline static Response InProgress(unsigned time) { return (Response)(time&0xffff); }

    virtual H323TransactionPDU * CreateRIP(
      unsigned sequenceNumber,
      unsigned delay
    ) const = 0;

    BOOL HandlePDU();

    virtual BOOL WritePDU(
      H323TransactionPDU & pdu
    );

    BOOL CheckCryptoTokens(
      const H235Authenticators & authenticators
    );

#if PTRACING
    virtual const char * GetName() const = 0;
#endif
    virtual H235Authenticator::ValidationResult ValidatePDU() const = 0;
    virtual void SetRejectReason(
      unsigned reasonCode
    ) = 0;

    BOOL IsFastResponseRequired() const { return fastResponseRequired && canSendRIP; }
    BOOL CanSendRIP() const { return canSendRIP; }
    H323TransportAddress GetReplyAddress() const { return replyAddresses[0]; }
    const H323TransportAddressArray & GetReplyAddresses() const { return replyAddresses; }
    BOOL IsBehindNAT() const { return isBehindNAT; }
    H323Transactor & GetTransactor() const { return transactor; }
    H235Authenticator::ValidationResult GetAuthenticatorResult() const { return authenticatorResult; }

  protected:
    virtual Response OnHandlePDU() = 0;
    PDECLARE_NOTIFIER(PThread, H323Transaction, SlowHandler);

    H323Transactor         & transactor;
    unsigned                 requestSequenceNumber;
    H323TransportAddressArray replyAddresses;
    BOOL                     fastResponseRequired;
    H323TransactionPDU     * request;
    H323TransactionPDU     * confirm;
    H323TransactionPDU     * reject;

    H235Authenticators                  authenticators;
    H235Authenticator::ValidationResult authenticatorResult;
    BOOL                                isBehindNAT;
    BOOL                                canSendRIP;
};


///////////////////////////////////////////////////////////

class H323TransactionServer : public PObject
{
  PCLASSINFO(H323TransactionServer, PObject);
  public:
  /**@name Construction */
  //@{
    /**Create a new gatekeeper.
     */
    H323TransactionServer(
      H323EndPoint & endpoint
    );

    /**Destroy gatekeeper.
     */
    ~H323TransactionServer();
  //@}

    virtual WORD GetDefaultUdpPort() = 0;

  /**@name Access functions */
  //@{
    /**Get the owner endpoint.
     */
    H323EndPoint & GetOwnerEndPoint() const { return ownerEndPoint; }

  /**@name Protocol Handler Operations */
  //@{
    /**Add listeners to the transaction server.
       If a listener already exists on the interface specified in the list
       then it is ignored. If a listener does not yet exist a new one is
       created and if a listener is running that is not in the list then it
       is stopped and removed.

       If the array is empty then the string "*" is assumed which will listen
       on the standard UDP port on INADDR_ANY.

       Returns TRUE if at least one interface was successfully started.
      */
    BOOL AddListeners(
      const H323TransportAddressArray & ifaces ///<  Interfaces to listen on.
    );

    /**Add a gatekeeper listener to this gatekeeper server given the
       transport address for the local interface.
      */
    BOOL AddListener(
      const H323TransportAddress & interfaceName
    );

    /**Add a gatekeeper listener to this gatekeeper server given the transport.
       Note that the transport is then owned by the listener and will be
       deleted automatically when the listener is destroyed. Note also the
       transport is deleted if this function returns FALSE and no listener was
       created.
      */
    BOOL AddListener(
      H323Transport * transport
    );

    /**Add a gatekeeper listener to this gatekeeper server.
       Note that the gatekeeper listener is then owned by the gatekeeper
       server and will be deleted automatically when the listener is removed.
       Note also the listener is deleted if this function returns FALSE and
       the listener was not used.
      */
    BOOL AddListener(
      H323Transactor * listener
    );

    /**Create a new H323GatkeeperListener.
       The user woiuld not usually use this function as it is used internally
       by the server when new listeners are added by H323TransportAddress.

       However, a user may override this function to create objects that are
       user defined descendants of H323GatekeeperListener so the user can
       maintain extra information on a interface by interface basis.
      */
    virtual H323Transactor * CreateListener(
      H323Transport * transport  ///<  Transport for listener
    ) = 0;

    /**Remove a gatekeeper listener from this gatekeeper server.
       The gatekeeper listener is automatically deleted.
      */
    BOOL RemoveListener(
      H323Transactor * listener
    );

    BOOL SetUpCallSignalAddresses(H225_ArrayOf_TransportAddress & addresses);
  //@}

  protected:
    H323EndPoint & ownerEndPoint;

    PThread      * monitorThread;
    PSyncPoint     monitorExit;

    PMutex         mutex;
    PLIST(ListenerList, H323Transactor);
    ListenerList listeners;
    BOOL usingAllInterfaces;
};


#endif // __OPAL_H323TRANS_H


/////////////////////////////////////////////////////////////////////////////
