/*
 * h235auth.h
 *
 * H.235 authorisation PDU's
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2001 Equivalence Pty. Ltd.
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
 * Contributor(s): Fürbass Franz <franz.fuerbass@infonova.at>
 *
 * $Log: h235auth.h,v $
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.22.2.3  2007/07/19 19:57:36  shorne
 * added missiing secure signal PDU check
 *
 * Revision 1.22.2.2  2007/04/19 15:07:13  shorne
 * Added missing IsSecurePDU to H235.1 authenticator
 *
 * Revision 1.22.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.22  2006/06/23 03:15:58  shorne
 * Updated H.235 class name
 *
 * Revision 1.21  2006/01/26 03:25:55  shorne
 * Caller Authentication added
 *
 * Revision 1.20  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.19  2005/02/13 23:54:48  csoutheren
 * Allow access to H.235 timestamp grace period
 * Thanks to Jan Willamowius
 *
 * Revision 1.18  2004/11/20 22:00:48  csoutheren
 * Added hacks for linker problem
 *
 * Revision 1.17  2004/11/12 06:04:42  csoutheren
 * Changed H235Authentiators to use PFactory
 *
 * Revision 1.16  2004/05/13 02:26:13  dereksmithies
 * Fixes so make docs does not generate warning messages about brackets.
 *
 * Revision 1.15  2003/04/30 00:28:50  robertj
 * Redesigned the alternate credentials in ARQ system as old implementation
 *   was fraught with concurrency issues, most importantly it can cause false
 *   detection of replay attacks taking out an endpoint completely.
 *
 * Revision 1.14  2003/04/01 04:47:48  robertj
 * Abstracted H.225 RAS transaction processing (RIP and secondary thread) in
 *   server environment for use by H.501 peer elements.
 *
 * Revision 1.13  2003/02/25 06:48:14  robertj
 * More work on PDU transaction abstraction.
 *
 * Revision 1.12  2003/02/11 04:43:22  robertj
 * Fixed use of asymmetrical authentication schemes such as MD5.
 *
 * Revision 1.11  2003/02/01 13:31:14  robertj
 * Changes to support CAT authentication in RAS.
 *
 * Revision 1.10  2003/01/08 04:40:31  robertj
 * Added more debug tracing for H.235 authenticators.
 *
 * Revision 1.9  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.8  2002/09/03 06:19:36  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.7  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.6  2002/08/05 05:17:37  robertj
 * Fairly major modifications to support different authentication credentials
 *   in ARQ to the logged in ones on RRQ. For both client and server.
 * Various other H.235 authentication bugs and anomalies fixed on the way.
 *
 * Revision 1.5  2002/05/17 03:39:28  robertj
 * Fixed problems with H.235 authentication on RAS for server and client.
 *
 * Revision 1.4  2001/12/06 06:44:42  robertj
 * Removed "Win32 SSL xxx" build configurations in favour of system
 *   environment variables to select optional libraries.
 *
 * Revision 1.3  2001/09/14 00:13:37  robertj
 * Fixed problem with some athenticators needing extra conditions to be
 *   "active", so make IsActive() virtual and add localId to H235AuthSimpleMD5
 *
 * Revision 1.2  2001/09/13 01:15:18  robertj
 * Added flag to H235Authenticator to determine if gkid and epid is to be
 *   automatically set as the crypto token remote id and local id.
 *
 * Revision 1.1  2001/08/10 11:03:49  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 */

#ifndef __OPAL_H235AUTH_H
#define __OPAL_H235AUTH_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

class H323TransactionPDU;
class H225_CryptoH323Token;
class H225_ArrayOf_AuthenticationMechanism;
class H225_ArrayOf_PASN_ObjectId;
class H235_ClearToken;
class H235_AuthenticationMechanism;
class PASN_ObjectId;
class PASN_Sequence;
class PASN_Array;

class H323SignalPDU;
class H323Connection;
class PSSLCertificate;

namespace PWLibStupidLinkerHacks {
extern int h235AuthLoader;
};

/** This abtract class embodies an H.235 authentication mechanism.
    NOTE: descendants must have a Clone() function for correct operation.
*/
class H235Authenticator : public PObject
{
    PCLASSINFO(H235Authenticator, PObject);
  public:
    H235Authenticator();

    virtual void PrintOn(
      ostream & strm
    ) const;

    virtual const char * GetName() const = 0;

    virtual BOOL PrepareTokens(
      PASN_Array & clearTokens,
      PASN_Array & cryptoTokens
    );

    virtual H235_ClearToken * CreateClearToken();
    virtual H225_CryptoH323Token * CreateCryptoToken();

    virtual BOOL Finalise(
      PBYTEArray & rawPDU
    );

    enum ValidationResult {
      e_OK = 0,     ///< Security parameters and Msg are ok, no security attacks
      e_Absent,     ///< Security parameters are expected but absent
      e_Error,      ///< Security parameters are present but incorrect
      e_InvalidTime,///< Security parameters indicate peer has bad real time clock
      e_BadPassword,///< Security parameters indicate bad password in token
      e_ReplyAttack,///< Security parameters indicate an attack was made
      e_Disabled    ///< Security is disabled by local system
    };

    virtual ValidationResult ValidateTokens(
      const PASN_Array & clearTokens,
      const PASN_Array & cryptoTokens,
      const PBYTEArray & rawPDU
    );

    virtual ValidationResult ValidateClearToken(
      const H235_ClearToken & clearToken
    );

    virtual ValidationResult ValidateCryptoToken(
      const H225_CryptoH323Token & cryptoToken,
      const PBYTEArray & rawPDU
    );

    virtual BOOL IsCapability(
      const H235_AuthenticationMechanism & mechansim,
      const PASN_ObjectId & algorithmOID
    ) = 0;

    virtual BOOL SetCapability(
      H225_ArrayOf_AuthenticationMechanism & mechansims,
      H225_ArrayOf_PASN_ObjectId & algorithmOIDs
    ) = 0;

    virtual BOOL UseGkAndEpIdentifiers() const;

    virtual BOOL IsSecuredPDU(
      unsigned rasPDU,
      BOOL received
    ) const;

    virtual BOOL IsSecuredSignalPDU(
      unsigned signalPDU,
      BOOL received
    ) const;

    virtual BOOL IsActive() const;

    virtual void Enable(
      BOOL enab = TRUE
    ) { enabled = enab; }
    virtual void Disable() { enabled = FALSE; }

    virtual const PString & GetRemoteId() const { return remoteId; }
    virtual void SetRemoteId(const PString & id) { remoteId = id; }

    virtual const PString & GetLocalId() const { return localId; }
    virtual void SetLocalId(const PString & id) { localId = id; }

    virtual const PString & GetPassword() const { return password; }
    virtual void SetPassword(const PString & pw) { password = pw; }

    virtual int GetTimestampGracePeriod() const { return timestampGracePeriod; }
    virtual void SetTimestampGracePeriod(int grace) { timestampGracePeriod = grace; }

    enum Application {
        GKAdmission,		///< To Be Used for GK Admission
        EPAuthentication,	///< To Be Used for EP Authentication
        LRQOnly,            ///< To Be Used for Location Request Authentication
        AnyApplication,		///< To Be Used for Any Application
    };

    Application GetApplication() { return usage; }  // Get Authentication Application

    virtual void SetConnection(H323Connection * con);	// Set the connection for EPAuthentication

  protected:
    BOOL AddCapability(
      unsigned mechanism,
      const PString & oid,
      H225_ArrayOf_AuthenticationMechanism & mechansims,
      H225_ArrayOf_PASN_ObjectId & algorithmOIDs
    );

    BOOL     enabled;

    PString  remoteId;      // ID of remote entity
    PString  localId;       // ID of local entity
    PString  password;      // shared secret

    unsigned sentRandomSequenceNumber;
    unsigned lastRandomSequenceNumber;
    unsigned lastTimestamp;
    int      timestampGracePeriod;

    Application usage;	       ///* Authenticator's Application 
    H323Connection * connection;   ///* CallToken of the Connection for EP Authentication
    PMutex mutex;
};


PDECLARE_LIST(H235Authenticators, H235Authenticator)
#ifdef DOC_PLUS_PLUS
{
#endif
  public:
// GKAdmission
    void PreparePDU(
      H323TransactionPDU & pdu,
      PASN_Array & clearTokens,
      unsigned clearOptionalField,
      PASN_Array & cryptoTokens,
      unsigned cryptoOptionalField
    ) const;

    H235Authenticator::ValidationResult ValidatePDU(
      const H323TransactionPDU & pdu,
      const PASN_Array & clearTokens,
      unsigned clearOptionalField,
      const PASN_Array & cryptoTokens,
      unsigned cryptoOptionalField,
      const PBYTEArray & rawPDU
    ) const;

// EPAuthentication
    void PrepareSignalPDU(
      unsigned code,
      PASN_Array & clearTokens,
      PASN_Array & cryptoTokens
    ) const;

    H235Authenticator::ValidationResult ValidateSignalPDU(
      unsigned code,
      const PASN_Array & clearTokens,
      const PASN_Array & cryptoTokens,
      const PBYTEArray & rawPDU
    ) const;

};

class H235AuthenticatorInfo : public PObject
{
    PCLASSINFO(H235AuthenticatorInfo, PObject);
public:
	H235AuthenticatorInfo(PString username,PString password,BOOL ishashed);
	H235AuthenticatorInfo(PSSLCertificate * cert);
	PString UserName;
	PString Password;
	BOOL isHashed;
	PSSLCertificate * Certificate;
};

PDECLARE_LIST(H235AuthenticatorList, H235AuthenticatorInfo)
#ifdef DOC_PLUS_PLUS
{
#endif
	BOOL HasUserName(PString UserName) const;
	void LoadPassword(PString UserName, PString & pass) const;
	void Add(PString username, PString password, BOOL isHashed = FALSE);
	PString PasswordEncrypt(const PString &clear) const;
	PString PasswordDecrypt(const PString &encrypt) const;
};

/** Dictionary of Addresses and Associated Security Info  */
PDICTIONARY(H235AuthenticatorDict,PString,H235AuthenticatorInfo); 

/** This class embodies a simple MD5 based authentication.
    The users password is concatenated with the 4 byte timestamp and 4 byte
    random fields and an MD5 generated and sent/verified
*/
class H235AuthSimpleMD5 : public H235Authenticator
{
    PCLASSINFO(H235AuthSimpleMD5, H235Authenticator);
  public:
    H235AuthSimpleMD5();

    PObject * Clone() const;

    virtual const char * GetName() const;

    virtual H225_CryptoH323Token * CreateCryptoToken();

    virtual ValidationResult ValidateCryptoToken(
      const H225_CryptoH323Token & cryptoToken,
      const PBYTEArray & rawPDU
    );

    virtual BOOL IsCapability(
      const H235_AuthenticationMechanism & mechansim,
      const PASN_ObjectId & algorithmOID
    );

    virtual BOOL SetCapability(
      H225_ArrayOf_AuthenticationMechanism & mechansim,
      H225_ArrayOf_PASN_ObjectId & algorithmOIDs
    );

    virtual BOOL IsSecuredPDU(
      unsigned rasPDU,
      BOOL received
    ) const;

    virtual BOOL IsSecuredSignalPDU(
      unsigned rasPDU,
      BOOL received
    ) const;
};


/** This class embodies a RADIUS compatible based authentication (aka Cisco
    Access Token or CAT).
    The users password is concatenated with the 4 byte timestamp and 1 byte
    random fields and an MD5 generated and sent/verified via the challenge
    field.
*/
class H235AuthCAT : public H235Authenticator
{
    PCLASSINFO(H235AuthCAT, H235Authenticator);
  public:
    H235AuthCAT();

    PObject * Clone() const;

    virtual const char * GetName() const;

    virtual H235_ClearToken * CreateClearToken();

    virtual ValidationResult ValidateClearToken(
      const H235_ClearToken & clearToken
    );

    virtual BOOL IsCapability(
      const H235_AuthenticationMechanism & mechansim,
      const PASN_ObjectId & algorithmOID
    );

    virtual BOOL SetCapability(
      H225_ArrayOf_AuthenticationMechanism & mechansim,
      H225_ArrayOf_PASN_ObjectId & algorithmOIDs
    );

    virtual BOOL IsSecuredPDU(
      unsigned rasPDU,
      BOOL received
    ) const;
};


#if P_SSL

namespace PWLibStupidLinkerHacks {
extern int h235AuthProcedure1Loader;
};

/** This class embodies the H.235 "base line" from H235.1.
*/

class H2351_Authenticator : public H235Authenticator
{
    PCLASSINFO(H2351_Authenticator, H235Authenticator);
  public:
    H2351_Authenticator();

    PObject * Clone() const;

    virtual const char * GetName() const;

    virtual H225_CryptoH323Token * CreateCryptoToken();

    virtual BOOL Finalise(
      PBYTEArray & rawPDU
    );

    virtual ValidationResult ValidateCryptoToken(
      const H225_CryptoH323Token & cryptoToken,
      const PBYTEArray & rawPDU
    );

    virtual BOOL IsCapability(
      const H235_AuthenticationMechanism & mechansim,
      const PASN_ObjectId & algorithmOID
    );

    virtual BOOL SetCapability(
      H225_ArrayOf_AuthenticationMechanism & mechansim,
      H225_ArrayOf_PASN_ObjectId & algorithmOIDs
    );

    virtual BOOL IsSecuredPDU(
      unsigned rasPDU,
      BOOL received
    ) const;

    virtual BOOL IsSecuredSignalPDU(
      unsigned rasPDU,
      BOOL received
    ) const;

    virtual BOOL UseGkAndEpIdentifiers() const;
};

typedef H2351_Authenticator H235AuthProcedure1;  // Backwards interoperability

#endif


#endif //__OPAL_H235AUTH_H


/////////////////////////////////////////////////////////////////////////////
