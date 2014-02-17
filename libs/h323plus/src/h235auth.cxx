/*
 * h235auth.cxx
 *
 * H.235 security PDU's
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
 * Contributor(s): __________________________________
 *
 * $Log: h235auth.cxx,v $
 * Revision 1.1  2007/08/06 20:51:05  shorne
 * First commit of h323plus
 *
 * Revision 1.31.2.3  2007/07/19 19:56:10  shorne
 * added missiing secure signal PDU check
 *
 * Revision 1.31.2.2  2007/05/23 07:02:38  shorne
 * Removed VS 2005 compile warning
 *
 * Revision 1.31.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.31  2006/02/13 11:24:23  csoutheren
 * Fixed problem with H235 authenticator factory not being populated
 *
 * Revision 1.30  2006/01/26 03:25:55  shorne
 * Caller Authentication added
 *
 * Revision 1.29  2005/02/03 02:46:07  csoutheren
 * Altered authentication functions to only update state information if authentication
 *  is correct. Thanks to Michal Zygmuntowicz
 *
 * Revision 1.28  2005/01/04 08:08:45  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.27  2004/11/21 12:59:07  csoutheren
 * Fixed cut and paste error affecting H235 authentication
 *
 * Revision 1.26  2004/11/20 22:00:49  csoutheren
 * Added hacks for linker problem
 *
 * Revision 1.25  2004/11/12 06:04:44  csoutheren
 * Changed H235Authentiators to use PFactory
 *
 * Revision 1.24  2003/04/30 00:28:54  robertj
 * Redesigned the alternate credentials in ARQ system as old implementation
 *   was fraught with concurrency issues, most importantly it can cause false
 *   detection of replay attacks taking out an endpoint completely.
 *
 * Revision 1.23  2003/04/01 04:47:55  robertj
 * Abstracted H.225 RAS transaction processing (RIP and secondary thread) in
 *   server environment for use by H.501 peer elements.
 *
 * Revision 1.22  2003/02/25 06:48:19  robertj
 * More work on PDU transaction abstraction.
 *
 * Revision 1.21  2003/02/12 07:40:36  robertj
 * Added more logging for authentication errors in validation.
 * Fixed problem with possible negative numbers in CAT random field.
 *
 * Revision 1.20  2003/02/11 04:44:13  robertj
 * Fixed use of asymmetrical authentication schemes such as MD5.
 *
 * Revision 1.19  2003/02/03 02:55:08  robertj
 * Fixed problem with pasing through other tokens transparently.
 *
 * Revision 1.18  2003/02/02 11:46:32  robertj
 * Fixed CAT selection in GRQ authentication mechanisms.
 *
 * Revision 1.17  2003/02/01 13:31:22  robertj
 * Changes to support CAT authentication in RAS.
 *
 * Revision 1.16  2003/01/08 04:40:34  robertj
 * Added more debug tracing for H.235 authenticators.
 *
 * Revision 1.15  2002/11/11 07:04:22  robertj
 * Fixed typo in trace.
 *
 * Revision 1.14  2002/11/05 00:04:21  robertj
 * Returned code back to including trailing NULL in BMPString after
 *   PString::AsUCS2() implementation changes.
 *
 * Revision 1.13  2002/10/31 07:11:16  robertj
 * Added UTF-8/UCS-2 conversion functions to PString.
 *
 * Revision 1.12  2002/08/13 05:11:03  robertj
 * Removed redundent code.
 *
 * Revision 1.11  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.10  2002/08/05 05:17:41  robertj
 * Fairly major modifications to support different authentication credentials
 *   in ARQ to the logged in ones on RRQ. For both client and server.
 * Various other H.235 authentication bugs and anomalies fixed on the way.
 *
 * Revision 1.9  2002/06/24 00:11:21  robertj
 * Clarified error message during GRQ authentication.
 *
 * Revision 1.8  2002/05/17 03:40:09  robertj
 * Fixed problems with H.235 authentication on RAS for server and client.
 *
 * Revision 1.7  2001/09/21 04:55:27  robertj
 * Removed redundant code, thanks Chih-Wei Huang
 *
 * Revision 1.6  2001/09/14 00:13:39  robertj
 * Fixed problem with some athenticators needing extra conditions to be
 *   "active", so make IsActive() virtual and add localId to H235AuthSimpleMD5
 *
 * Revision 1.5  2001/09/13 01:15:20  robertj
 * Added flag to H235Authenticator to determine if gkid and epid is to be
 *   automatically set as the crypto token remote id and local id.
 *
 * Revision 1.4  2001/08/14 04:26:46  robertj
 * Completed the Cisco compatible MD5 authentications, thanks Wolfgang Platzer.
 *
 * Revision 1.3  2001/08/13 10:03:54  robertj
 * Fixed problem if do not have a localId when doing MD5 authentication.
 *
 * Revision 1.2  2001/08/10 13:49:35  robertj
 * Fixed alpha Linux warning.
 *
 * Revision 1.1  2001/08/10 11:03:52  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h235auth.h"
#endif

#if defined(_WIN32) && (_MSC_VER > 1300)
  #pragma warning(disable:4244) // warning about possible loss of data
#endif

#include "h235auth.h"

#include "h323pdu.h"
#include <ptclib/random.h>
#include <ptclib/cypher.h>


#define new PNEW

namespace PWLibStupidLinkerHacks {
  int h235AuthLoader;
};

/////////////////////////////////////////////////////////////////////////////

H235Authenticator::H235Authenticator()
{
  enabled = TRUE;
  sentRandomSequenceNumber = PRandom::Number()&INT_MAX;
  lastRandomSequenceNumber = 0;
  lastTimestamp = 0;
  timestampGracePeriod = 2*60*60+10; // 2 hours 10 seconds to allow for DST adjustments
  usage = GKAdmission;               // Set Default Application to GKAdmission
  connection = NULL;
}


void H235Authenticator::PrintOn(ostream & strm) const
{
  PWaitAndSignal m(mutex);

  strm << GetName() << '<';
  if (IsActive())
    strm << "active";
  else if (!enabled)
    strm << "disabled";
  else if (password.IsEmpty())
    strm << "no-pwd";
  else
    strm << "inactive";
  strm << '>';
}


BOOL H235Authenticator::PrepareTokens(PASN_Array & clearTokens,
                                      PASN_Array & cryptoTokens)
{
  PWaitAndSignal m(mutex);

  if (!IsActive())
    return FALSE;

  H235_ClearToken * clearToken = CreateClearToken();
  if (clearToken != NULL) {
    // Check if already have a token of thsi type and overwrite it
    for (PINDEX i = 0; i < clearTokens.GetSize(); i++) {
      H235_ClearToken & oldToken = (H235_ClearToken &)clearTokens[i];
      if (clearToken->m_tokenOID == oldToken.m_tokenOID) {
        oldToken = *clearToken;
        delete clearToken;
        clearToken = NULL;
        break;
      }
    }

    if (clearToken != NULL)
      clearTokens.Append(clearToken);
  }

  H225_CryptoH323Token * cryptoToken = CreateCryptoToken();
  if (cryptoToken != NULL)
    cryptoTokens.Append(cryptoToken);

  return TRUE;
}


H235_ClearToken * H235Authenticator::CreateClearToken()
{
  return NULL;
}


H225_CryptoH323Token * H235Authenticator::CreateCryptoToken()
{
  return NULL;
}


BOOL H235Authenticator::Finalise(PBYTEArray & /*rawPDU*/)
{
  return TRUE;
}


H235Authenticator::ValidationResult H235Authenticator::ValidateTokens(
                                        const PASN_Array & clearTokens,
                                        const PASN_Array & cryptoTokens,
                                        const PBYTEArray & rawPDU)
{
  PWaitAndSignal m(mutex);

  if (!IsActive())
    return e_Disabled;

  PINDEX i;
  for (i = 0; i < clearTokens.GetSize(); i++) {
    ValidationResult s = ValidateClearToken((H235_ClearToken &)clearTokens[i]);
    if (s != e_Absent)
      return s;
  }

  for (i = 0; i < cryptoTokens.GetSize(); i++) {
    ValidationResult s = ValidateCryptoToken((H225_CryptoH323Token &)cryptoTokens[i], rawPDU);
    if (s != e_Absent)
      return s;
  }

  return e_Absent;
}


H235Authenticator::ValidationResult H235Authenticator::ValidateClearToken(
                                                 const H235_ClearToken & /*clearToken*/)
{
  return e_Absent;
}


H235Authenticator::ValidationResult H235Authenticator::ValidateCryptoToken(
                                            const H225_CryptoH323Token & /*cryptoToken*/,
                                            const PBYTEArray & /*rawPDU*/)
{
  return e_Absent;
}


BOOL H235Authenticator::UseGkAndEpIdentifiers() const
{
  return FALSE;
}


BOOL H235Authenticator::IsSecuredPDU(unsigned, BOOL) const
{
  return TRUE;
}

BOOL H235Authenticator::IsSecuredSignalPDU(unsigned, BOOL) const
{
  return FALSE;
}

BOOL H235Authenticator::IsActive() const
{
  return enabled && !password;
}


BOOL H235Authenticator::AddCapability(unsigned mechanism,
                                      const PString & oid,
                                      H225_ArrayOf_AuthenticationMechanism & mechanisms,
                                      H225_ArrayOf_PASN_ObjectId & algorithmOIDs)
{
  PWaitAndSignal m(mutex);

  if (!IsActive()) {
    PTRACE(2, "RAS\tAuthenticator " << *this
            << " not active during SetCapability negotiation");
    return FALSE;
  }

  PINDEX i;
  PINDEX size = mechanisms.GetSize();
  for (i = 0; i < size; i++) {
    if (mechanisms[i].GetTag() == mechanism)
      break;
  }
  if (i >= size) {
    mechanisms.SetSize(size+1);
    mechanisms[size].SetTag(mechanism);
  }

  size = algorithmOIDs.GetSize();
  for (i = 0; i < size; i++) {
    if (algorithmOIDs[i] == oid)
      break;
  }
  if (i >= size) {
    algorithmOIDs.SetSize(size+1);
    algorithmOIDs[size] = oid;
  }

  return TRUE;
}

void H235Authenticator::SetConnection(H323Connection * con)
{
	connection = con;
}

///////////////////////////////////////////////////////////////////////////////

void H235Authenticators::PreparePDU(H323TransactionPDU & pdu,
                                    PASN_Array & clearTokens,
                                    unsigned clearOptionalField,
                                    PASN_Array & cryptoTokens,
                                    unsigned cryptoOptionalField) const
{
  // Clean out any crypto tokens in case this is a retry message
  // and we are regenerating the tokens due to possible timestamp
  // issues. We don't do this for clear tokens which may be used by
  // other endpoints and should be passed through unchanged.
  cryptoTokens.RemoveAll();

  for (PINDEX i = 0; i < GetSize(); i++) {
    H235Authenticator & authenticator = (*this)[i];
    if (authenticator.IsSecuredPDU(pdu.GetChoice().GetTag(), FALSE) &&
        authenticator.PrepareTokens(clearTokens, cryptoTokens)) {
      PTRACE(4, "H235RAS\tPrepared PDU with authenticator " << authenticator);
    }
  }

  PASN_Sequence & subPDU = (PASN_Sequence &)pdu.GetChoice().GetObject();
  if (clearTokens.GetSize() > 0)
    subPDU.IncludeOptionalField(clearOptionalField);

  if (cryptoTokens.GetSize() > 0)
    subPDU.IncludeOptionalField(cryptoOptionalField);
}


H235Authenticator::ValidationResult
       H235Authenticators::ValidatePDU(const H323TransactionPDU & pdu,
                                       const PASN_Array & clearTokens,
                                       unsigned clearOptionalField,
                                       const PASN_Array & cryptoTokens,
                                       unsigned cryptoOptionalField,
                                       const PBYTEArray & rawPDU) const
{
  BOOL noneActive = TRUE;
  PINDEX i;
  for (i = 0; i < GetSize(); i++) {
    H235Authenticator & authenticator = (*this)[i];
    if (authenticator.IsActive() && authenticator.IsSecuredPDU(pdu.GetChoice().GetTag(), TRUE)) {
      noneActive = FALSE;
      break;
    }
  }

  if (noneActive)
    return H235Authenticator::e_OK;

  //do not accept non secure RAS Messages
  const PASN_Sequence & subPDU = (const PASN_Sequence &)pdu.GetChoice().GetObject();
  if (!subPDU.HasOptionalField(clearOptionalField) &&
      !subPDU.HasOptionalField(cryptoOptionalField)) {
    PTRACE(2, "H235RAS\tReceived unsecured RAS message (no crypto tokens),"
              " need one of:\n" << setfill(',') << *this << setfill(' '));
    return H235Authenticator::e_Absent;
  }

  for (i = 0; i < GetSize(); i++) {
    H235Authenticator & authenticator = (*this)[i];
    if (authenticator.IsSecuredPDU(pdu.GetChoice().GetTag(), TRUE)) {
      H235Authenticator::ValidationResult result = authenticator.ValidateTokens(clearTokens, cryptoTokens, rawPDU);
      switch (result) {
        case H235Authenticator::e_OK :
          PTRACE(4, "H235RAS\tAuthenticator " << authenticator << " succeeded");
          return H235Authenticator::e_OK;

        case H235Authenticator::e_Absent :
          PTRACE(4, "H235RAS\tAuthenticator " << authenticator << " absent from PDU");
          authenticator.Disable();
          break;

        case H235Authenticator::e_Disabled :
          PTRACE(4, "H235RAS\tAuthenticator " << authenticator << " disabled");
          break;

        default : // Various other failure modes
          PTRACE(4, "H235RAS\tAuthenticator " << authenticator << " failed: " << (int)result);
          return result;
      }
    }
  }

  return H235Authenticator::e_Absent;
}


void H235Authenticators::PrepareSignalPDU(unsigned code,
                                    PASN_Array & clearTokens,
                                    PASN_Array & cryptoTokens) const
{
  // Clean out any crypto tokens in case this is a retry message
  // and we are regenerating the tokens due to possible timestamp
  // issues. We don't do this for clear tokens which may be used by
  // other endpoints and should be passed through unchanged.
  cryptoTokens.RemoveAll();

  for (PINDEX i = 0; i < GetSize(); i++) {
    H235Authenticator & authenticator = (*this)[i];
     if (authenticator.IsSecuredSignalPDU(code, FALSE) &&
			authenticator.PrepareTokens(clearTokens, cryptoTokens)) {
      PTRACE(4, "H235EP\tPrepared SignalPDU with authenticator " << authenticator);
	   }
  }
}


H235Authenticator::ValidationResult
       H235Authenticators::ValidateSignalPDU(unsigned code,
											const PASN_Array & clearTokens,
											const PASN_Array & cryptoTokens,
											const PBYTEArray & rawPDU) const
{

  H235Authenticator::ValidationResult finalresult = H235Authenticator::e_Absent;
 
  for (PINDEX i = 0; i < GetSize(); i++) {
    H235Authenticator & authenticator = (*this)[i];
    if (authenticator.IsSecuredSignalPDU(code, TRUE)) {

		H235Authenticator::ValidationResult result = authenticator.ValidateTokens(clearTokens, cryptoTokens, rawPDU);
		  switch (result) {
			case H235Authenticator::e_OK :
			  PTRACE(4, "H235EP\tAuthenticator " << authenticator << " succeeded");
			  finalresult = result;
			  break;

			case H235Authenticator::e_Absent :
			  PTRACE(4, "H235EP\tAuthenticator " << authenticator << " absent from PDU");
			  authenticator.Disable();
			  break;

			case H235Authenticator::e_Disabled :
			  PTRACE(4, "H235EP\tAuthenticator " << authenticator << " disabled");
			  break;

			default : // Various other failure modes
			  PTRACE(4, "H235EP\tAuthenticator " << authenticator << " failed: " << (int)result);
			    if (finalresult != H235Authenticator::e_OK) 
									finalresult = result;
			  break;
		  }	

    } else {
		authenticator.Disable();
	}

  }

  return finalresult;
}

///////////////////////////////////////////////////////////////////////////////

BOOL H235AuthenticatorList::HasUserName(PString UserName) const
{
	for (PINDEX i = 0; i < GetSize(); i++) {
        H235AuthenticatorInfo & info = (*this)[i];
			if (UserName == info.UserName)
				return TRUE;
	}
    return FALSE;
}

void H235AuthenticatorList::LoadPassword(PString UserName, PString & pass) const
{
	for (PINDEX i = 0; i < GetSize(); i++) {
        H235AuthenticatorInfo & info = (*this)[i];
		if (UserName == info.UserName) {
			if (info.isHashed) {
				pass = PasswordDecrypt(info.Password);
			} else 
				pass = info.Password;
		}
	}
}

void H235AuthenticatorList::Add(PString username, PString password, BOOL isHashed)
{
  H235AuthenticatorInfo * info = new H235AuthenticatorInfo(username,password,isHashed);

  (*this).Append(info);

}

#define AuthenticatorListHashKey  "H235Authenticator"

PString H235AuthenticatorList::PasswordEncrypt(const PString &clear) const
{

int keyFilled = 0;

const PString key = AuthenticatorListHashKey;
	
	PTEACypher::Key thekey;
	memset(&thekey, keyFilled, sizeof(PTEACypher::Key));
	memcpy(&thekey, const_cast<PString &>(key).GetPointer(), min(sizeof(PTEACypher::Key), size_t(key.GetLength())));
	PTEACypher cypher(thekey);
	return cypher.Encode(clear);	
}

PString H235AuthenticatorList::PasswordDecrypt(const PString &encrypt) const
{

int keyFilled = 0;

const PString key = AuthenticatorListHashKey;

	PTEACypher::Key thekey;
	memset(&thekey, keyFilled, sizeof(PTEACypher::Key));
	memcpy(&thekey, const_cast<PString &>(key).GetPointer(), min(sizeof(PTEACypher::Key), size_t(key.GetLength())));
	PTEACypher cypher(thekey);
	return cypher.Decode(encrypt);	
}

///////////////////////////////////////////////////////////////////////////////
H235AuthenticatorInfo::H235AuthenticatorInfo(PString username,PString password,BOOL ishashed)
	: UserName(username), Password(password), isHashed(ishashed)
{
}

H235AuthenticatorInfo::H235AuthenticatorInfo(PSSLCertificate * cert)
	: Certificate(cert)
{
}
///////////////////////////////////////////////////////////////////////////////

static PFactory<H235Authenticator>::Worker<H235AuthSimpleMD5> factoryH235AuthSimpleMD5("SimpleMD5");

static const char OID_MD5[] = "1.2.840.113549.2.5";

H235AuthSimpleMD5::H235AuthSimpleMD5()
{
	usage = AnyApplication; // Can be used either for GKAdmission or EPAuthenticstion
}


PObject * H235AuthSimpleMD5::Clone() const
{
  return new H235AuthSimpleMD5(*this);
}


const char * H235AuthSimpleMD5::GetName() const
{
  return "MD5";
}


#if PTLIB_MAJOR == 2 && PTLIB_MINOR > 0
static PWCharArray GetUCS2plusNULL(const PString & str)
#else
static PWORDArray GetUCS2plusNULL(const PString & str)
#endif
{
#if PTLIB_MAJOR == 2 && PTLIB_MINOR > 0
  PWCharArray ucs2 = str.AsUCS2();
#else
  PWORDArray ucs2 = str.AsUCS2();
#endif
  PINDEX len = ucs2.GetSize();
  if (len > 0 && ucs2[len-1] != 0)
    ucs2.SetSize(len+1);
  return ucs2;
}


H225_CryptoH323Token * H235AuthSimpleMD5::CreateCryptoToken()
{
  if (!IsActive())
    return NULL;

  if (localId.IsEmpty()) {
    PTRACE(2, "H235RAS\tH235AuthSimpleMD5 requires local ID for encoding.");
    return NULL;
  }

  // Cisco compatible hash calculation
  H235_ClearToken clearToken;

  // fill the PwdCertToken to calculate the hash
  clearToken.m_tokenOID = "0.0";

  clearToken.IncludeOptionalField(H235_ClearToken::e_generalID);
  clearToken.m_generalID = GetUCS2plusNULL(localId);

  clearToken.IncludeOptionalField(H235_ClearToken::e_password);
  clearToken.m_password = GetUCS2plusNULL(password);

  clearToken.IncludeOptionalField(H235_ClearToken::e_timeStamp);
  clearToken.m_timeStamp = (int)time(NULL);

  // Encode it into PER
  PPER_Stream strm;
  clearToken.Encode(strm);
  strm.CompleteEncoding();

  // Generate an MD5 of the clear tokens PER encoding.
  PMessageDigest5 stomach;
  stomach.Process(strm.GetPointer(), strm.GetSize());
  PMessageDigest5::Code digest;
  stomach.Complete(digest);

  // Create the H.225 crypto token
  H225_CryptoH323Token * cryptoToken = new H225_CryptoH323Token;
  cryptoToken->SetTag(H225_CryptoH323Token::e_cryptoEPPwdHash);
  H225_CryptoH323Token_cryptoEPPwdHash & cryptoEPPwdHash = *cryptoToken;

  // Set the token data that actually goes over the wire
  H323SetAliasAddress(localId, cryptoEPPwdHash.m_alias);

  cryptoEPPwdHash.m_timeStamp = clearToken.m_timeStamp;
  cryptoEPPwdHash.m_token.m_algorithmOID = OID_MD5;
  cryptoEPPwdHash.m_token.m_hash.SetData(sizeof(digest)*8, (const BYTE *)&digest);

  return cryptoToken;
}


H235Authenticator::ValidationResult H235AuthSimpleMD5::ValidateCryptoToken(
                                             const H225_CryptoH323Token & cryptoToken,
                                             const PBYTEArray &)
{
  if (!IsActive())
    return e_Disabled;

  // verify the token is of correct type
  if (cryptoToken.GetTag() != H225_CryptoH323Token::e_cryptoEPPwdHash)
    return e_Absent;

  const H225_CryptoH323Token_cryptoEPPwdHash & cryptoEPPwdHash = cryptoToken;

  PString alias = H323GetAliasAddressString(cryptoEPPwdHash.m_alias);

#ifndef DISABLE_CALLAUTH
  // If has connection then EP Authenticator so CallBack to Check SenderID and Set Password
  if (connection != NULL) { 
	 if (!connection->OnCallAuthentication(alias,password)) {
		PTRACE(1, "H235EP\tH235AuthSimpleMD5 Authentication Fail UserName \"" << alias 
			   << "\", not Authorised. \"");
		return e_BadPassword;
	 }
  } else {
#endif
     if (!remoteId && alias != remoteId) {
        PTRACE(1, "H235RAS\tH235AuthSimpleMD5 alias is \"" << alias
           << "\", should be \"" << remoteId << '"');
         return e_Error;
     }
#ifndef DISABLE_CALLAUTH
  }
#endif

  // Build the clear token
  H235_ClearToken clearToken;
  clearToken.m_tokenOID = "0.0";

  clearToken.IncludeOptionalField(H235_ClearToken::e_generalID);
  clearToken.m_generalID = GetUCS2plusNULL(alias);

  clearToken.IncludeOptionalField(H235_ClearToken::e_password);
  clearToken.m_password = GetUCS2plusNULL(password);

  clearToken.IncludeOptionalField(H235_ClearToken::e_timeStamp);
  clearToken.m_timeStamp = cryptoEPPwdHash.m_timeStamp;

  // Encode it into PER
  PPER_Stream strm;
  clearToken.Encode(strm);
  strm.CompleteEncoding();

  // Generate an MD5 of the clear tokens PER encoding.
  PMessageDigest5 stomach;
  stomach.Process(strm.GetPointer(), strm.GetSize());
  PMessageDigest5::Code digest;
  stomach.Complete(digest);

  if (cryptoEPPwdHash.m_token.m_hash.GetSize() == sizeof(digest)*8 &&
      memcmp(cryptoEPPwdHash.m_token.m_hash.GetDataPointer(), &digest, sizeof(digest)) == 0)
    return e_OK;

  PTRACE(1, "H235RAS\tH235AuthSimpleMD5 digest does not match.");
  return e_BadPassword;
}


BOOL H235AuthSimpleMD5::IsCapability(const H235_AuthenticationMechanism & mechanism,
                                     const PASN_ObjectId & algorithmOID)
{
  return mechanism.GetTag() == H235_AuthenticationMechanism::e_pwdHash &&
         algorithmOID.AsString() == OID_MD5;
}


BOOL H235AuthSimpleMD5::SetCapability(H225_ArrayOf_AuthenticationMechanism & mechanisms,
                                      H225_ArrayOf_PASN_ObjectId & algorithmOIDs)
{
  return AddCapability(H235_AuthenticationMechanism::e_pwdHash, OID_MD5, mechanisms, algorithmOIDs);
}


BOOL H235AuthSimpleMD5::IsSecuredPDU(unsigned rasPDU, BOOL received) const
{
  switch (rasPDU) {
    case H225_RasMessage::e_registrationRequest :
    case H225_RasMessage::e_unregistrationRequest :
    case H225_RasMessage::e_admissionRequest :
    case H225_RasMessage::e_disengageRequest :
    case H225_RasMessage::e_bandwidthRequest :
    case H225_RasMessage::e_infoRequestResponse :
      return received ? !remoteId.IsEmpty() : !localId.IsEmpty();

    default :
      return FALSE;
  }
}

BOOL H235AuthSimpleMD5::IsSecuredSignalPDU(unsigned signalPDU, BOOL received) const
{
  switch (signalPDU) {
    case H225_H323_UU_PDU_h323_message_body::e_setup:       
      return received ? !remoteId.IsEmpty() : !localId.IsEmpty();

    default :
      return FALSE;
  }
}


///////////////////////////////////////////////////////////////////////////////

static PFactory<H235Authenticator>::Worker<H235AuthCAT> factoryH235AuthCAT("SimpleCAT");

static const char OID_CAT[] = "1.2.840.113548.10.1.2.1";

H235AuthCAT::H235AuthCAT()
{
	usage = GKAdmission;		 /// To be used for GKAdmission
}


PObject * H235AuthCAT::Clone() const
{
  return new H235AuthCAT(*this);
}


const char * H235AuthCAT::GetName() const
{
  return "CAT";
}


H235_ClearToken * H235AuthCAT::CreateClearToken()
{
  if (!IsActive())
    return NULL;

  if (localId.IsEmpty()) {
    PTRACE(2, "H235RAS\tH235AuthCAT requires local ID for encoding.");
    return NULL;
  }

  H235_ClearToken * clearToken = new H235_ClearToken;

  // Cisco compatible hash calculation
  clearToken->m_tokenOID = OID_CAT;

  clearToken->IncludeOptionalField(H235_ClearToken::e_generalID);
  clearToken->m_generalID = GetUCS2plusNULL(localId);

  clearToken->IncludeOptionalField(H235_ClearToken::e_timeStamp);
  clearToken->m_timeStamp = (int)time(NULL);
  PUInt32b timeStamp = (DWORD)clearToken->m_timeStamp;

  clearToken->IncludeOptionalField(H235_ClearToken::e_random);
  BYTE random = (BYTE)++sentRandomSequenceNumber;
  clearToken->m_random = (unsigned)random;

  // Generate an MD5 of the clear tokens PER encoding.
  PMessageDigest5 stomach;
  stomach.Process(&random, 1);
  stomach.Process(password);
  stomach.Process(&timeStamp, 4);
  PMessageDigest5::Code digest;
  stomach.Complete(digest);

  clearToken->IncludeOptionalField(H235_ClearToken::e_challenge);
  clearToken->m_challenge.SetValue((const BYTE *)&digest, sizeof(digest));

  return clearToken;
}


H235Authenticator::ValidationResult
        H235AuthCAT::ValidateClearToken(const H235_ClearToken & clearToken)
{
  if (!IsActive())
    return e_Disabled;

  if (clearToken.m_tokenOID != OID_CAT)
    return e_Absent;

  if (!clearToken.HasOptionalField(H235_ClearToken::e_generalID) ||
      !clearToken.HasOptionalField(H235_ClearToken::e_timeStamp) ||
      !clearToken.HasOptionalField(H235_ClearToken::e_random) ||
      !clearToken.HasOptionalField(H235_ClearToken::e_challenge)) {
    PTRACE(2, "H235RAS\tCAT requires generalID, timeStamp, random and challenge fields");
    return e_Error;
  }

  //first verify the timestamp
  PTime now;
  int deltaTime = now.GetTimeInSeconds() - clearToken.m_timeStamp;
  if (PABS(deltaTime) > timestampGracePeriod) {
    PTRACE(1, "H235RAS\tInvalid timestamp ABS(" << now.GetTimeInSeconds() << '-' 
           << (int)clearToken.m_timeStamp << ") > " << timestampGracePeriod);
    //the time has elapsed
    return e_InvalidTime;
  }

  //verify the randomnumber
  if (lastTimestamp == clearToken.m_timeStamp &&
      lastRandomSequenceNumber == clearToken.m_random) {
    //a message with this timespamp and the same random number was already verified
    PTRACE(1, "H235RAS\tConsecutive messages with the same random and timestamp");
    return e_ReplyAttack;
  }

  if (!remoteId && clearToken.m_generalID.GetValue() != remoteId) {
    PTRACE(1, "H235RAS\tGeneral ID is \"" << clearToken.m_generalID.GetValue()
           << "\", should be \"" << remoteId << '"');
    return e_Error;
  }

  int randomInt = clearToken.m_random;
  if (randomInt < -127 || randomInt > 255) {
    PTRACE(2, "H235RAS\tCAT requires single byte random field, got " << randomInt);
    return e_Error;
  }

  PUInt32b timeStamp = (DWORD)clearToken.m_timeStamp;
  BYTE randomByte = (BYTE)randomInt;

  // Generate an MD5 of the clear tokens PER encoding.
  PMessageDigest5 stomach;
  stomach.Process(&randomByte, 1);
  stomach.Process(password);
  stomach.Process(&timeStamp, 4);
  PMessageDigest5::Code digest;
  stomach.Complete(digest);

  if (clearToken.m_challenge.GetValue().GetSize() != sizeof(digest)) {
    PTRACE(2, "H235RAS\tCAT requires 16 byte challenge field");
    return e_Error;
  }

  if (memcmp((const BYTE *)clearToken.m_challenge.GetValue(), &digest, sizeof(digest)) == 0) {

    // save the values for the next call
    lastRandomSequenceNumber = clearToken.m_random;
    lastTimestamp = clearToken.m_timeStamp;
  
    return e_OK;
  }

  PTRACE(2, "H235RAS\tCAT hash does not match");
  return e_BadPassword;
}


BOOL H235AuthCAT::IsCapability(const H235_AuthenticationMechanism & mechanism,
                                     const PASN_ObjectId & algorithmOID)
{
  if (mechanism.GetTag() != H235_AuthenticationMechanism::e_authenticationBES ||
         algorithmOID.AsString() != OID_CAT)
    return FALSE;

  const H235_AuthenticationBES & bes = mechanism;
  return bes.GetTag() == H235_AuthenticationBES::e_radius;
}


BOOL H235AuthCAT::SetCapability(H225_ArrayOf_AuthenticationMechanism & mechanisms,
                                H225_ArrayOf_PASN_ObjectId & algorithmOIDs)
{
  if (!AddCapability(H235_AuthenticationMechanism::e_authenticationBES, OID_CAT,
                     mechanisms, algorithmOIDs))
    return FALSE;

  H235_AuthenticationBES & bes = mechanisms[mechanisms.GetSize()-1];
  bes.SetTag(H235_AuthenticationBES::e_radius);
  return TRUE;
}


BOOL H235AuthCAT::IsSecuredPDU(unsigned rasPDU, BOOL received) const
{
  switch (rasPDU) {
    case H225_RasMessage::e_registrationRequest :
    case H225_RasMessage::e_admissionRequest :
      return received ? !remoteId.IsEmpty() : !localId.IsEmpty();

    default :
      return FALSE;
  }
}


/////////////////////////////////////////////////////////////////////////////
