/*
 * h323.cxx
 *
 * H.323 protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
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
 * $Log: h323.cxx,v $
 * Revision 1.17  2008/02/13 09:26:48  shorne
 * Fix Bug for Fast Start from last commit
 *
 * Revision 1.16  2008/02/10 23:11:33  shorne
 * Fix to compile H323plus without Video
 *
 * Revision 1.15  2008/02/06 02:52:59  shorne
 * Added support for Standards based NAT Traversal
 *
 * Revision 1.14  2008/01/30 18:51:36  shorne
 * fix for duplicate h224handler definition
 *
 * Revision 1.13  2008/01/22 01:10:33  shorne
 * Fix H.224 opening unidirectional channel
 *
 * Revision 1.12  2008/01/04 06:23:09  shorne
 * Cleaner setup and teardown of h460 module
 *
 * Revision 1.11  2008/01/01 00:16:12  shorne
 * Added GnuGknat and FileTransfer support
 *
 * Revision 1.10  2007/11/29 14:19:42  willamowius
 * use seionID to test session type when doig capability merge
 *
 * Revision 1.9  2007/11/28 15:30:38  willamowius
 * fix capability type detection for merging
 *
 * Revision 1.8  2007/11/28 06:03:37  shorne
 * Video capability merge. Thx again Jan Willamowius
 *
 * Revision 1.7  2007/11/17 00:14:47  shorne
 * Fix to make disabling function calls consistent
 *
 * Revision 1.6  2007/11/16 22:09:43  shorne
 * Added ability to disable H.245 QoS for NetMeeting Interop
 *
 * Revision 1.5  2007/11/01 20:17:33  shorne
 * updates for H.239 support
 *
 * Revision 1.4  2007/10/19 19:54:17  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.3  2007/10/16 17:01:33  shorne
 * Various little fixes
 *
 * Revision 1.2  2007/08/20 19:13:28  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.1  2007/08/06 20:51:06  shorne
 * First commit of h323plus
 *
 *
 * Revision 1.385.2.14  2007/09/22 04:34:55  rjongbloed
 * Fixed in-band tone decoder, incorrect number of samples used.
 *
 * Revision 1.385.2.13  2007/08/20 09:47:13  shorne
 * Added OnEPAuthenticationFail callback
 *
 * Revision 1.385.2.12  2007/07/23 21:47:11  shorne
 * Added QoS GK Reporting
 *
 * Revision 1.385.2.11  2007/07/20 22:03:27  shorne
 * Initial H.350 Support
 *
 * Revision 1.385.2.10  2007/07/19 20:10:28  shorne
 * Changed HAS_AEC to H323_AEC
 *
 * Revision 1.385.2.9  2007/05/23 06:58:02  shorne
 * Nat Support for EP's nested behind same NAT
 *
 * Revision 1.385.2.8  2007/03/27 18:13:21  shorne
 * small bug fix from previous commit
 *
 * Revision 1.385.2.7  2007/03/24 23:39:43  shorne
 * More H.239 work
 *
 * Revision 1.385.2.6  2007/03/18 06:21:38  shorne
 * More tweaks with Multimedia OnHold
 *
 * Revision 1.385.2.5  2007/03/14 08:53:38  shorne
 * Added Video on Hold
 *
 * Revision 1.385.2.4  2007/03/06 00:18:38  shorne
 * Added Wideband AEC support
 *
 * Revision 1.385.2.3  2007/02/18 17:11:23  shorne
 * Added H.249 Extended UserInput Support
 *
 * Revision 1.385.2.2  2007/02/11 00:45:20  shorne
 * Added ability to disable NAT method on a call by call basis
 *
 * Revision 1.385.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.385  2006/10/30 20:45:02  shorne
 * Fix for changes in pwlib
 *
 * Revision 1.384  2006/07/21 10:31:03  shorne
 * Add ability for H.460 connection feature to access ARQ message
 *
 * Revision 1.383  2006/06/27 12:35:03  csoutheren
 * Patch 1366328 - Support for H.450.3 divertingLegInformation2
 * Thanks to Norbert Bartalsky
 *
 * Revision 1.382  2006/06/27 04:16:23  shorne
 * Updated H460 directive
 *
 * Revision 1.381  2006/06/23 20:01:29  shorne
 * More H460 support
 *
 * Revision 1.380  2006/06/23 07:06:04  csoutheren
 * Fixed linux compile
 *
 * Revision 1.379  2006/06/23 06:02:44  csoutheren
 * Added missing declarations for H.224 backport
 *
 * Revision 1.378  2006/06/21 04:53:32  csoutheren
 * Tweaked H.245 version 13 updates
 *
 * Revision 1.377  2006/06/20 05:24:40  csoutheren
 * Additional tweaks for H.225v6
 *
 * Revision 1.376  2006/06/09 06:30:12  csoutheren
 * Remove compile warning and errors with gcc
 *
 * Revision 1.375  2006/05/30 11:14:56  hfriederich
 * Switch from DISABLE_H460 to H323_H460
 *
 * Revision 1.374  2006/05/17 03:36:52  shorne
 * DISABLE_H460 default under linux
 *
 * Revision 1.373  2006/05/16 11:21:53  shorne
 * Call Credit support / H460 FeatureSet added
 *
 * Revision 1.372  2006/04/11 03:40:55  csoutheren
 * Fix for seperate H.245 channels timing out
 *
 * Revision 1.371  2006/04/06 08:22:17  csoutheren
 * Added support for conference commands
 *
 * Revision 1.370  2006/03/21 10:44:48  csoutheren
 * Extra bulletproofing for aggregation
 *
 * Revision 1.369  2006/03/02 07:52:51  csoutheren
 * Ensure prompt close of channels when using aggregation
 * Ensure MonitorCallStatus called when using aggregation
 *
 * Revision 1.368  2006/02/24 04:52:17  csoutheren
 * Fixd problem with H.245 aggregation
 *
 * Revision 1.367  2006/01/27 07:53:38  csoutheren
 * Fixed for signalling aggregation
 *
 * Revision 1.366  2006/01/26 07:25:40  shorne
 * oops try that again! Fix typo  :)
 *
 * Revision 1.365  2006/01/26 04:20:02  shorne
 * Fix typo from last commit
 *
 * Revision 1.364  2006/01/26 03:48:14  shorne
 * more PBX support, Caller Authentication
 *
 * Revision 1.363  2006/01/24 08:15:24  csoutheren
 * Implement outgoing H.225 aggregation, and H.245 aggregation (disabled by default)
 * More testing to do, but this looks good :)
 *
 * Revision 1.362  2006/01/23 05:58:26  csoutheren
 * Working outgoing H.225 aggregation (disabled by default)
 *
 * Revision 1.361  2006/01/20 00:32:24  csoutheren
 * First check-in of signalling aggregation code - incomplete and disabled by default
 *
 * Revision 1.360  2006/01/18 07:46:08  csoutheren
 * Initial version of RTP aggregation (disabled by default)
 *
 * Revision 1.359  2005/11/25 02:25:27  csoutheren
 * Applied patch #1351556 from Louis R. Marascio
 * Fix for endSession not being sent properly by remote EP
 *
 * Revision 1.358  2005/09/23 05:46:41  csoutheren
 * Fixed bug #1296199, where call limits were not enforced for incoming calls
 *
 * Revision 1.357  2005/09/16 08:14:34  csoutheren
 * Added override to control cleanup of OSP connections
 * Added new function to allow control of deferred fastStart connections
 *
 * Revision 1.356  2005/08/27 02:14:21  csoutheren
 * Capture time that reverse fast start acknowledge is received
 * Capture time that connect is sent/received
 *
 * Revision 1.355  2005/07/15 13:13:56  csoutheren
 * Fixed compile problem with gcc 2.95.3 compiler
 * Thanks to Roger Hardiman
 *
 * Revision 1.354  2005/03/08 03:45:00  csoutheren
 * Fixed debug problem in fastStart mode
 *
 * Revision 1.353  2005/03/04 03:21:20  csoutheren
 * Added local and remote addresses to all PDU logs to assist in debugging
 *
 * Revision 1.352  2005/01/03 14:03:21  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.351  2005/01/03 06:25:55  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.350  2004/12/21 23:33:47  csoutheren
 * Fixed #defines for H.460, thanks to Simon Horne
 *
 * Revision 1.349  2004/12/16 00:34:35  csoutheren
 * Fixed reporting of call end time and code
 * Added GetNextDestination
 *
 * Revision 1.348  2004/12/14 06:22:21  csoutheren
 * More OSP implementation
 *
 * Revision 1.347  2004/12/09 23:38:40  csoutheren
 * More OSP implementation
 *
 * Revision 1.346  2004/12/08 01:59:23  csoutheren
 * initial support for Transnexus OSP toolkit
 *
 * Revision 1.345  2004/11/22 11:31:02  rjongbloed
 * Added ability to restart H.245 negotiations, thanks Norbert Bartalsky
 *
 * Revision 1.344  2004/09/07 22:50:56  rjongbloed
 * Changed usage of template function as MSVC6 will not compile it.
 *
 * Revision 1.343  2004/09/03 01:06:10  csoutheren
 * Added initial hooks for H.460 GEF
 * Thanks to Simon Horne and ISVO (Asia) Pte Ltd. for this contribution
 *
 * Revision 1.342  2004/08/14 07:44:20  rjongbloed
 * Fixed compatibility with early start on Cisco CCM systems, thanks Portela Fernando
 *
 * Revision 1.341  2004/08/01 11:35:49  rjongbloed
 * Fixed possible issue with merging real TCS and the "fake" TCS we build from
 *   the fast start parameters. Should not merge but overwrite.
 *
 * Revision 1.340  2004/08/01 10:53:45  rjongbloed
 * Allowed greater flexibility with checking capability types in setting Q931 bearer caps
 *   on call setup, thanks Simon Horne
 *
 * Revision 1.339  2004/07/30 05:28:16  csoutheren
 * Fixed problem when inteoperating with some endpoints behind a firewall
 * Thanks to Derek Smithies for patiently hitting the author over the head until this problem was fixed
 *
 * Revision 1.338  2004/07/20 09:32:33  csoutheren
 * Ensured that TCS and MSD timers are stopped when a call is shut down
 * Thanks to Joegen Baclor
 *
 * Revision 1.337  2004/07/03 06:51:37  rjongbloed
 * Added PTRACE_PARAM() macro to fix warnings on parameters used in PTRACE
 *  macros only.
 *
 * Revision 1.336  2004/06/15 03:30:00  csoutheren
 * Added OnSendARQ to allow access to the ARQ message before sent by connection
 *
 * Revision 1.335  2004/06/09 23:48:41  csoutheren
 * Improved resolution of remote party name
 *
 * Revision 1.334  2004/06/09 23:31:08  csoutheren
 * Ensure correct call failure code is returned when call ends due to facility deflect
 *
 * Revision 1.333  2004/06/09 23:28:34  csoutheren
 * Ensured that Alerting that is not associated with media startup does
 * not prematurely send back a refusal of fastConnect
 *
 * Revision 1.332  2004/05/17 12:14:24  csoutheren
 * Added support for different SETUP PDU types
 *
 * Revision 1.331  2004/04/24 23:58:05  rjongbloed
 * Fixed GCC 3.4 warning about PAssertNULL
 *
 * Revision 1.330  2004/04/20 07:53:13  csoutheren
 * Fixed problems with NAT detection
 *
 * history trimmed CRS 23 Jan 2005
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h323con.h"
#endif

#include "h323con.h"

#include "h323ep.h"
#include "h323neg.h"
#include "h323rtp.h"

#ifdef H323_H450
#include "h450/h4501.h"
#include "h450/h4503.h"
#include "h450/h4504.h"
#include "h450/h45011.h"
#include "h450/h450pdu.h"
#endif

#ifdef H323_H460
#include "h460/h460.h"
#include "h460/h4601.h"
#endif

#include "gkclient.h"
#include "rfc2833.h"

#ifdef H323_T120
#include "t120proto.h"
#endif

#ifdef H323_T38
#include "t38proto.h"
#endif

#ifdef H323_H224
#include "h323h224.h"
#endif

#ifdef H323_FILE
#include "h323filetransfer.h"
#endif

#ifdef H323_AEC
#include <ptclib/paec.h>
#endif

#include "h235auth.h"

const PTimeInterval MonitorCallStatusTime(0, 10); // Seconds

#define new PNEW

#ifdef H323_SIGNAL_AGGREGATE

class AggregatedH225Handle : public H323AggregatedH2x5Handle
{
//  PCLASSINFO(AggregatedH225Handle, H323AggregatedH2x5Handle)
  public:
    AggregatedH225Handle(H323Transport & _transport, H323Connection & _connection)
      : H323AggregatedH2x5Handle(_transport, _connection)
    {
    }

    ~AggregatedH225Handle()
    {
    }

    BOOL OnRead()
    {
      BOOL ret = H323AggregatedH2x5Handle::OnRead();
      if (connection.controlChannel == NULL)
        connection.MonitorCallStatus();
      return ret;
    }

    BOOL HandlePDU(BOOL ok, PBYTEArray & dataPDU)
    {
      H323SignalPDU pdu;
      if (ok && dataPDU.GetSize() > 0)
        ok = pdu.ProcessReadData(transport, dataPDU);
      return connection.HandleReceivedSignalPDU(ok, pdu);
    }

    void OnClose()
    {  DeInit(); }

    void DeInit()
    {
      if (connection.controlChannel == NULL) {
        connection.endSessionReceived.Signal();
      }
    }
};

class AggregatedH245Handle : public H323AggregatedH2x5Handle
{
//  PCLASSINFO(AggregatedH245Handle, H323AggregatedH2x5Handle)
  public:
    AggregatedH245Handle(H323Transport & _transport, H323Connection & _connection)
      : H323AggregatedH2x5Handle(_transport, _connection)
    {
    }

    ~AggregatedH245Handle()
    {
    }

    BOOL OnRead()
    {
      BOOL ret = H323AggregatedH2x5Handle::OnRead();
      connection.MonitorCallStatus();
      return ret;
    }

    BOOL HandlePDU(BOOL ok, PBYTEArray & pdu)
    {
      PPER_Stream strm(pdu);
      return connection.HandleReceivedControlPDU(ok, strm);
    }

    void OnClose()
    {  
      PPER_Stream strm;
      connection.HandleReceivedControlPDU(FALSE, strm);
    }

    void DeInit()
    { 
      connection.EndHandleControlChannel(); 
    }
};

#endif

/////////////////////////////////////////////////////////////////////////////

#if PTRACING
ostream & operator<<(ostream & o, H323Connection::CallEndReason r)
{
  static const char * const CallEndReasonNames[H323Connection::NumCallEndReasons] = {
    "EndedByLocalUser",         /// Local endpoint application cleared call
    "EndedByNoAccept",          /// Local endpoint did not accept call OnIncomingCall()=FALSE
    "EndedByAnswerDenied",      /// Local endpoint declined to answer call
    "EndedByRemoteUser",        /// Remote endpoint application cleared call
    "EndedByRefusal",           /// Remote endpoint refused call
    "EndedByNoAnswer",          /// Remote endpoint did not answer in required time
    "EndedByCallerAbort",       /// Remote endpoint stopped calling
    "EndedByTransportFail",     /// Transport error cleared call
    "EndedByConnectFail",       /// Transport connection failed to establish call
    "EndedByGatekeeper",        /// Gatekeeper has cleared call
    "EndedByNoUser",            /// Call failed as could not find user (in GK)
    "EndedByNoBandwidth",       /// Call failed as could not get enough bandwidth
    "EndedByCapabilityExchange",/// Could not find common capabilities
    "EndedByCallForwarded",     /// Call was forwarded using FACILITY message
    "EndedBySecurityDenial",    /// Call failed a security check and was ended
    "EndedByLocalBusy",         /// Local endpoint busy
    "EndedByLocalCongestion",   /// Local endpoint congested
    "EndedByRemoteBusy",        /// Remote endpoint busy
    "EndedByRemoteCongestion",  /// Remote endpoint congested
    "EndedByUnreachable",       /// Could not reach the remote party
    "EndedByNoEndPoint",        /// The remote party is not running an endpoint
    "EndedByHostOffline",       /// The remote party host off line
    "EndedByTemporaryFailure",  /// The remote failed temporarily app may retry
    "EndedByQ931Cause",         /// The remote ended the call with unmapped Q.931 cause code
    "EndedByDurationLimit",     /// Call cleared due to an enforced duration limit
    "EndedByInvalidConferenceID", /// Call cleared due to invalid conference ID
    "EndedByOSPRefusal"         // Call cleared as OSP server unable or unwilling to route
  };

  if ((PINDEX)r >= PARRAYSIZE(CallEndReasonNames))
    o << "InvalidCallEndReason<" << (unsigned)r << '>';
  else if (CallEndReasonNames[r] == NULL)
    o << "CallEndReason<" << (unsigned)r << '>';
  else
    o << CallEndReasonNames[r];
  return o;
}


ostream & operator<<(ostream & o, H323Connection::AnswerCallResponse s)
{
  static const char * const AnswerCallResponseNames[H323Connection::NumAnswerCallResponses] = {
    "AnswerCallNow",
    "AnswerCallDenied",
    "AnswerCallPending",
    "AnswerCallDeferred",
    "AnswerCallAlertWithMedia",
    "AnswerCallDeferredWithMedia"
  };
  if ((PINDEX)s >= PARRAYSIZE(AnswerCallResponseNames))
    o << "InvalidAnswerCallResponse<" << (unsigned)s << '>';
  else if (AnswerCallResponseNames[s] == NULL)
    o << "AnswerCallResponse<" << (unsigned)s << '>';
  else
    o << AnswerCallResponseNames[s];
  return o;
}


ostream & operator<<(ostream & o, H323Connection::SendUserInputModes m)
{
  static const char * const SendUserInputModeNames[H323Connection::NumSendUserInputModes] = {
    "SendUserInputAsQ931",
    "SendUserInputAsString",
    "SendUserInputAsTone",
    "SendUserInputAsRFC2833",
    "SendUserInputAsSeparateRFC2833"
#ifdef H323_H249
	,"SendUserInputAsNavigation",
	"SendUserInputAsSoftkey",
	"SendUserInputAsPointDevice",
	"SendUserInputAsModal"
#endif
  };

  if ((PINDEX)m >= PARRAYSIZE(SendUserInputModeNames))
    o << "InvalidSendUserInputMode<" << (unsigned)m << '>';
  else if (SendUserInputModeNames[m] == NULL)
    o << "SendUserInputMode<" << (unsigned)m << '>';
  else
    o << SendUserInputModeNames[m];
  return o;
}


const char * const H323Connection::ConnectionStatesNames[NumConnectionStates] = {
  "NoConnectionActive",
  "AwaitingGatekeeperAdmission",
  "AwaitingTransportConnect",
  "AwaitingSignalConnect",
  "AwaitingLocalAnswer",
  "HasExecutedSignalConnect",
  "EstablishedConnection",
  "ShuttingDownConnection"
};

const char * const H323Connection::FastStartStateNames[NumFastStartStates] = {
  "FastStartDisabled",
  "FastStartInitiate",
  "FastStartResponse",
  "FastStartAcknowledged"
};
#endif
#ifdef H323_H460
static void ReceiveSetupFeatureSet(const H323Connection * connection, const H225_Setup_UUIE & pdu)
{
    H225_FeatureSet fs;
    BOOL hasFeaturePDU = FALSE;

	if (pdu.HasOptionalField(H225_Setup_UUIE::e_neededFeatures)) {
        fs.IncludeOptionalField(H225_FeatureSet::e_neededFeatures);
	    H225_ArrayOf_FeatureDescriptor & fsn = fs.m_neededFeatures;
	    fsn = pdu.m_neededFeatures;
		hasFeaturePDU = TRUE;
	}
     
	if (pdu.HasOptionalField(H225_Setup_UUIE::e_desiredFeatures)) {
        fs.IncludeOptionalField(H225_FeatureSet::e_desiredFeatures);
	    H225_ArrayOf_FeatureDescriptor & fsn = fs.m_desiredFeatures;
	    fsn = pdu.m_desiredFeatures;
		hasFeaturePDU = TRUE;
	}

	if (pdu.HasOptionalField(H225_Setup_UUIE::e_supportedFeatures)) {
        fs.IncludeOptionalField(H225_FeatureSet::e_supportedFeatures);
	    H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
	    fsn = pdu.m_supportedFeatures;
		hasFeaturePDU = TRUE;
	}

	if (hasFeaturePDU)
		connection->OnReceiveFeatureSet(H460_MessageType::e_setup, fs);

}

template <typename PDUType>
static void ReceiveFeatureData(const H323Connection * connection, unsigned code, const PDUType & pdu)
{
    if (pdu.m_h323_uu_pdu.HasOptionalField(H225_H323_UU_PDU::e_genericData)) {
        H225_FeatureSet fs;
	    fs.IncludeOptionalField(H225_FeatureSet::e_supportedFeatures);
	    H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
		const H225_ArrayOf_GenericData & data = pdu.m_h323_uu_pdu.m_genericData;
		for (PINDEX i=0; i < data.GetSize(); i++) {
			 PINDEX lastPos = fsn.GetSize();
			 fsn.SetSize(lastPos+1);
			 fsn[lastPos] = (H225_FeatureDescriptor &)data[i];
		}
        connection->OnReceiveFeatureSet(code, fs);
	}
}
#endif

template <typename PDUType>
static void ReceiveFeatureSet(const H323Connection * connection, unsigned code, const PDUType & pdu)
{
    if (pdu.HasOptionalField(PDUType::e_featureSet))
      connection->OnReceiveFeatureSet(code, pdu.m_featureSet);
}

#ifndef DISABLE_CALLAUTH
template <typename PDUType>
static BOOL ReceiveAuthenticatorPDU(const H323Connection * connection, 
								   const PDUType & pdu, unsigned code)
{

BOOL AuthResult = FALSE;
H235Authenticators authenticators = connection->GetEPAuthenticators();
PBYTEArray strm;

if (connection->GetEndPoint().GetEPSecurityPolicy() != H323EndPoint::SecNone) {

  if (!pdu.HasOptionalField(PDUType::e_cryptoTokens)) {
		PTRACE(2, "H235EP\tReceived unsecured EPAuthentication message (no crypto tokens),"
			" expected one of:\n" << setfill(',') << connection->GetEPAuthenticators() << setfill(' '));
       return connection->OnEPAuthenticationFailed(H235Authenticator::e_Absent);
  } else {

	H235Authenticator::ValidationResult result = authenticators.ValidateSignalPDU(code, 
													pdu.m_tokens, pdu.m_cryptoTokens,strm);
	  if (result == H235Authenticator::e_OK) {
		  PTRACE(4, "H235EP\tAuthentication succeeded");
		  AuthResult = TRUE;
	  }
	  return AuthResult ? TRUE : connection->OnEPAuthenticationFailed(result);
  }
}

   return AuthResult;
}
#endif

H323Connection::H323Connection(H323EndPoint & ep,
                               unsigned ref,
                               unsigned options)
  : endpoint(ep),
    localAliasNames(ep.GetAliasNames()),
    localPartyName(ep.GetLocalUserName()),
    localCapabilities(ep.GetCapabilities()),
    gkAccessTokenOID(ep.GetGkAccessTokenOID()),
    alertingTime(0),
    connectedTime(0),
    callEndTime(0),
    reverseMediaOpenTime(0),
    releaseSequence(ReleaseSequenceUnknown)
#ifndef DISABLE_CALLAUTH
    ,EPAuthenticators(ep.CreateEPAuthenticators())
#endif
#ifdef H323_H460
    ,features(ep.GetFeatureSet())
#endif
{
  localAliasNames.MakeUnique();

  callAnswered = FALSE;
  gatekeeperRouted = FALSE;
  distinctiveRing = 0;
  callReference = ref;
  remoteCallWaiting = -1;

  h225version = H225_PROTOCOL_VERSION;
  h245version = H245_PROTOCOL_VERSION;
  h245versionSet = FALSE;

  signallingChannel = NULL;
  controlChannel = NULL;

#ifdef H323_H450
  holdAudioMediaChannel = NULL;
  holdVideoMediaChannel = NULL;
  isConsultationTransfer = FALSE;
  isCallIntrusion = FALSE;
  callIntrusionProtectionLevel = endpoint.GetCallIntrusionProtectionLevel();
#endif

  switch (options&H245TunnelingOptionMask) {
    case H245TunnelingOptionDisable :
      h245Tunneling = FALSE;
      break;

    case H245TunnelingOptionEnable :
      h245Tunneling = TRUE;
      break;

    default :
      h245Tunneling = !ep.IsH245TunnelingDisabled();
      break;
  }

  h245TunnelTxPDU = NULL;
  h245TunnelRxPDU = NULL;
  alertingPDU = NULL;
  connectPDU = NULL;

  connectionState = NoConnectionActive;
  callEndReason = NumCallEndReasons;
  q931Cause = Q931::ErrorInCauseIE;

  bandwidthAvailable = endpoint.GetInitialBandwidth();

  uuiesRequested = 0; // Empty set
  addAccessTokenToSetup = TRUE; // Automatic inclusion of ACF access token in SETUP
  sendUserInputMode = endpoint.GetSendUserInputMode();

  mediaWaitForConnect = FALSE;
  transmitterSidePaused = FALSE;

  switch (options&FastStartOptionMask) {
    case FastStartOptionDisable :
      fastStartState = FastStartDisabled;
      break;

    case FastStartOptionEnable :
      fastStartState = FastStartInitiate;
      break;

    default :
      fastStartState = ep.IsFastStartDisabled() ? FastStartDisabled : FastStartInitiate;
      break;
  }

  mustSendDRQ = FALSE;
  earlyStart = FALSE;

#ifdef H323_T120
  startT120 = TRUE;
#endif

#ifdef H323_H224
  startH224 = TRUE;
#endif

  lastPDUWasH245inSETUP = FALSE;
  endSessionNeeded = FALSE;
  endSessionSent = FALSE;

  switch (options&H245inSetupOptionMask) {
    case H245inSetupOptionDisable :
      doH245inSETUP = FALSE;
      break;

    case H245inSetupOptionEnable :
      doH245inSETUP = TRUE;
      break;

    default :
      doH245inSETUP = !ep.IsH245inSetupDisabled();
      break;
  }

  doH245QoS = !ep.IsH245QoSDisabled();

#ifdef H323_AUDIO_CODECS
  remoteMaxAudioDelayJitter = 0;
  minAudioJitterDelay = endpoint.GetMinAudioJitterDelay();
  maxAudioJitterDelay = endpoint.GetMaxAudioJitterDelay();
#endif

  switch (options&DetectInBandDTMFOptionMask) {
    case DetectInBandDTMFOptionDisable :
      detectInBandDTMF = FALSE;
      break;

    case DetectInBandDTMFOptionEnable :
      detectInBandDTMF = TRUE;
      break;

    default :
      detectInBandDTMF = !ep.DetectInBandDTMFDisabled();
      break;
  }

  masterSlaveDeterminationProcedure = new H245NegMasterSlaveDetermination(endpoint, *this);
  capabilityExchangeProcedure = new H245NegTerminalCapabilitySet(endpoint, *this);
  logicalChannels = new H245NegLogicalChannels(endpoint, *this);
  requestModeProcedure = new H245NegRequestMode(endpoint, *this);
  roundTripDelayProcedure = new H245NegRoundTripDelay(endpoint, *this);

#ifdef H323_H450
  h450dispatcher = new H450xDispatcher(*this);
  h4502handler = new H4502Handler(*this, *h450dispatcher);
  h4503handler = new H4503Handler(*this, *h450dispatcher);
  h4504handler = new H4504Handler(*this, *h450dispatcher);
  h4506handler = new H4506Handler(*this, *h450dispatcher);
  h45011handler = new H45011Handler(*this, *h450dispatcher);
#endif

  rfc2833handler = new OpalRFC2833(PCREATE_NOTIFIER(OnUserInputInlineRFC2833));

#ifdef H323_T120
  t120handler = NULL;
#endif

#ifdef H323_T38
  t38handler = NULL;
#endif

#ifdef H323_H224
  h224handler = NULL;
  h281handler = NULL;
#endif

#ifdef H323_FILE
  filehandler = NULL;
#endif

  endSync = NULL;

  remoteIsNAT = FALSE;
  NATsupport =  TRUE;
  sameNAT = FALSE;

#ifndef DISABLE_CALLAUTH
  AuthenticationFailed = FALSE;
  hasAuthentication = FALSE;
#endif

  // set aggregation options
#ifdef H323_RTP_AGGREGATE
  useRTPAggregation        = (options & RTPAggregationMask)        != RTPAggregationDisable;
#endif
#ifdef H323_SIGNAL_AGGREGATE
  signalAggregator = NULL;
  controlAggregator = NULL;
  useSignallingAggregation = (options & SignallingAggregationMask) != SignallingAggregationDisable;
#endif

#ifdef H323_AEC
	aec = NULL;
#endif

#ifdef H323_H460
  disableH460 = ep.FeatureSetDisabled();
  features->LoadFeatureSet(H460_Feature::FeatureSignal,this);
#endif

  IsNonCallConnection = FALSE;
}


H323Connection::~H323Connection()
{

  delete masterSlaveDeterminationProcedure;
  delete capabilityExchangeProcedure;
  delete logicalChannels;
  delete requestModeProcedure;
  delete roundTripDelayProcedure;
#ifdef H323_H450
  delete h450dispatcher;
#endif
  delete rfc2833handler;
#ifdef H323_T120
  delete t120handler;
#endif
#ifdef H323_T38
  delete t38handler;
#endif
#ifdef H323_H224
  delete h224handler;
  delete h281handler;
#endif
  delete signallingChannel;
  delete controlChannel;
  delete alertingPDU;
  delete connectPDU;
#ifdef H323_H450
  delete holdAudioMediaChannel;
  delete holdVideoMediaChannel;
#endif
#ifdef H323_H460
  delete features;
#endif

  PTRACE(3, "H323\tConnection " << callToken << " deleted.");

  if (endSync != NULL)
    endSync->Signal();
}


BOOL H323Connection::Lock()
{
  outerMutex.Wait();

  // If shutting down don't try and lock, just return failed. If not then lock
  // it but do second test for shut down to avoid a possible race condition.

  if (connectionState == ShuttingDownConnection) {
    outerMutex.Signal();
    return FALSE;
  }

  innerMutex.Wait();
  return TRUE;
}


int H323Connection::TryLock()
{
  if (!outerMutex.Wait(0))
    return -1;

  if (connectionState == ShuttingDownConnection) {
    outerMutex.Signal();
    return 0;
  }

  innerMutex.Wait();
  return 1;
}


void H323Connection::Unlock()
{
  innerMutex.Signal();
  outerMutex.Signal();
}


void H323Connection::SetCallEndReason(CallEndReason reason, PSyncPoint * sync)
{
  // Only set reason if not already set to something
  if (callEndReason == NumCallEndReasons) {
    PTRACE(3, "H323\tCall end reason for " << callToken << " set to " << reason);
    callEndReason = reason;
  }

  // only set the sync point if it is NULL
  if (endSync == NULL)
    endSync = sync;
  else 
    PAssert(sync == NULL, "SendCallEndReason called to overwrite syncpoint");

  if (!callEndTime.IsValid())
    callEndTime = PTime();

  if (endSessionSent)
    return;

  endSessionSent = TRUE;

  PTRACE(2, "H225\tSending release complete PDU: callRef=" << callReference);
  H323SignalPDU rcPDU;
  rcPDU.BuildReleaseComplete(*this);

#ifdef H323_H450
  h450dispatcher->AttachToReleaseComplete(rcPDU);
#endif

  BOOL sendingReleaseComplete = OnSendReleaseComplete(rcPDU);

  if (endSessionNeeded) {
    if (sendingReleaseComplete)
      h245TunnelTxPDU = &rcPDU; // Piggy back H245 on this reply

    // Send an H.245 end session to the remote endpoint.
    H323ControlPDU pdu;
    pdu.BuildEndSessionCommand(H245_EndSessionCommand::e_disconnect);
    WriteControlPDU(pdu);
  }

  if (sendingReleaseComplete) {
    h245TunnelTxPDU = NULL;
    if (releaseSequence == ReleaseSequenceUnknown)
      releaseSequence = ReleaseSequence_Local;
    WriteSignalPDU(rcPDU);
  }
}


BOOL H323Connection::ClearCall(H323Connection::CallEndReason reason)
{
  return endpoint.ClearCall(callToken, reason);
}

BOOL H323Connection::ClearCallSynchronous(PSyncPoint * sync, H323Connection::CallEndReason reason)
{
  return endpoint.ClearCallSynchronous(callToken, reason, sync);
}


void H323Connection::CleanUpOnCallEnd()
{
  PTRACE(3, "H323\tConnection " << callToken << " closing: connectionState=" << connectionState);

  /* The following double mutex is designed to guarentee that there is no
     deadlock or access of deleted object with a random thread that may have
     just called Lock() at the instant we are trying to get rid of the
     connection.
   */

  outerMutex.Wait();
  connectionState = ShuttingDownConnection;
  outerMutex.Signal();
  innerMutex.Wait();

  // Unblock sync points
  digitsWaitFlag.Signal();

  // stop various timers
  masterSlaveDeterminationProcedure->Stop();
  capabilityExchangeProcedure->Stop();

  // Clean up any fast start "pending" channels we may have running.
  PINDEX i;
  for (i = 0; i < fastStartChannels.GetSize(); i++)
    fastStartChannels[i].CleanUpOnTermination();
  fastStartChannels.RemoveAll();

  // Dispose of all the logical channels
  logicalChannels->RemoveAll();

  if (endSessionNeeded) {
    // Calculate time since we sent the end session command so we do not actually
    // wait for returned endSession if it has already been that long
    PTimeInterval waitTime = endpoint.GetEndSessionTimeout();
    if (callEndTime.IsValid()) {
      PTime now;
      if (now > callEndTime) { // Allow for backward motion in time (DST change)
        waitTime -= now - callEndTime;
        if (waitTime < 0)
          waitTime = 0;
      }
    }

    // Wait a while for the remote to send an endSession
    PTRACE(4, "H323\tAwaiting end session from remote for " << waitTime << " seconds");
    if (!endSessionReceived.Wait(waitTime)) {
      PTRACE(3, "H323\tDid not receive an end session from remote.");
    }
  }

  // Wait for control channel to be cleaned up (thread ended).
  if (controlChannel != NULL)
    controlChannel->CleanUpOnTermination();

#ifdef H323_SIGNAL_AGGREGATE
  if (controlAggregator != NULL)
    endpoint.GetSignallingAggregator()->RemoveHandle(controlAggregator);
#endif

  // Wait for signalling channel to be cleaned up (thread ended).
  if (signallingChannel != NULL)
    signallingChannel->CleanUpOnTermination();

#ifdef H323_SIGNAL_AGGREGATE
  if (signalAggregator != NULL)
    endpoint.GetSignallingAggregator()->RemoveHandle(signalAggregator);
#endif

  // Check for gatekeeper and do disengage if have one
  if (mustSendDRQ) {
    H323Gatekeeper * gatekeeper = endpoint.GetGatekeeper();
    if (gatekeeper != NULL)
      gatekeeper->DisengageRequest(*this, H225_DisengageReason::e_normalDrop);
  }

  PTRACE(1, "H323\tConnection " << callToken << " terminated.");
}

void H323Connection::AttachSignalChannel(const PString & token,
                                         H323Transport * channel,
                                         BOOL answeringCall)
{
  callAnswered = answeringCall;

  if (signallingChannel != NULL && signallingChannel->IsOpen()) {
    PAssertAlways(PLogicError);
    return;
  }

  delete signallingChannel;
  signallingChannel = channel;

  // Set our call token for identification in endpoint dictionary
  callToken = token;

#ifndef DISABLE_CALLAUTH
  SetAuthenticationConnection();
#endif
}


BOOL H323Connection::WriteSignalPDU(H323SignalPDU & pdu)
{
  PAssert(signallingChannel != NULL, PLogicError);

  lastPDUWasH245inSETUP = FALSE;

  if (signallingChannel != NULL && signallingChannel->IsOpen()) {
    pdu.m_h323_uu_pdu.m_h245Tunneling = h245Tunneling;

    H323Gatekeeper * gk = endpoint.GetGatekeeper();
    if (gk != NULL)
      gk->InfoRequestResponse(*this, pdu.m_h323_uu_pdu, TRUE);

    if (pdu.Write(*signallingChannel,*this))
      return TRUE;
  }

  ClearCall(EndedByTransportFail);
  return FALSE;
}

void H323Connection::HandleSignallingChannel()
{
  PAssert(signallingChannel != NULL, PLogicError);

  PTRACE(2, "H225\tReading PDUs: callRef=" << callReference);

  while (signallingChannel->IsOpen()) {
    H323SignalPDU pdu;
    if (!HandleReceivedSignalPDU(pdu.Read(*signallingChannel), pdu))
      break;
  }

  // If we are the only link to the far end then indicate that we have
  // received endSession even if we hadn't, because we are now never going
  // to get one so there is no point in having CleanUpOnCallEnd wait.
  if (controlChannel == NULL)
    endSessionReceived.Signal();

  PTRACE(2, "H225\tSignal channel closed.");
}

BOOL H323Connection::HandleReceivedSignalPDU(BOOL readStatus, H323SignalPDU & pdu)
{
  if (readStatus) {
    if (!HandleSignalPDU(pdu)) {
#ifndef DISABLE_CALLAUTH
      if (AuthenticationFailed)
	      ClearCall(EndedBySecurityDenial);
      else 
#endif
        ClearCall(EndedByTransportFail);
      return FALSE;
    }
    switch (connectionState) {
      case EstablishedConnection :
        signallingChannel->SetReadTimeout(MonitorCallStatusTime);
        break;
      default:
        break;
    }
  }
  else if (signallingChannel->GetErrorCode() != PChannel::Timeout) {
    if (controlChannel == NULL || !controlChannel->IsOpen())
      ClearCall(EndedByTransportFail);
    signallingChannel->Close();
    return FALSE;
  }
  else {
    switch (connectionState) {
      case AwaitingSignalConnect :
        // Had time out waiting for remote to send a CONNECT
        ClearCall(EndedByNoAnswer);
        break;
      case HasExecutedSignalConnect :
        // Have had minimum MonitorCallStatusTime delay since CONNECT but
        // still no media to move it to EstablishedConnection state. Must
        // thus not have any common codecs to use!
        ClearCall(EndedByCapabilityExchange);
        break;
      default :
        break;
    }
  }

  if (controlChannel == NULL)
    MonitorCallStatus();

  return TRUE;
}


BOOL H323Connection::HandleSignalPDU(H323SignalPDU & pdu)
{
  // Process the PDU.
  const Q931 & q931 = pdu.GetQ931();

  PTRACE(3, "H225\tHandling PDU: " << q931.GetMessageTypeName()
                    << " callRef=" << q931.GetCallReference());

  if (!Lock()) {
    // Continue to look for endSession/releaseComplete pdus
    if (pdu.m_h323_uu_pdu.m_h245Tunneling) {
      for (PINDEX i = 0; i < pdu.m_h323_uu_pdu.m_h245Control.GetSize(); i++) {
        PPER_Stream strm = pdu.m_h323_uu_pdu.m_h245Control[i].GetValue();
        if (!InternalEndSessionCheck(strm))
          break;
      }
    }
    if (q931.GetMessageType() == Q931::ReleaseCompleteMsg)
      endSessionReceived.Signal();
    return FALSE;
  }

  // If remote does not do tunneling, so we don't either. Note that if it
  // gets turned off once, it stays off for good.  
  // GNUGK NAT may accidently send an information PDU
  if (q931.GetMessageType() != Q931::InformationMsg) {
	  if (h245Tunneling && !pdu.m_h323_uu_pdu.m_h245Tunneling) {
		masterSlaveDeterminationProcedure->Stop();
		capabilityExchangeProcedure->Stop();
		PTRACE(3, "H225\tFast Start DISABLED!");
		h245Tunneling = FALSE;
	  }
  }

  h245TunnelRxPDU = &pdu;

#ifdef H323_H450
  // Check for presence of supplementary services
  if (pdu.m_h323_uu_pdu.HasOptionalField(H225_H323_UU_PDU::e_h4501SupplementaryService)) {
    if (!h450dispatcher->HandlePDU(pdu)) // Process H4501SupplementaryService APDU
      return FALSE;
  }
#endif

#ifdef H323_H460
   ReceiveFeatureData<H323SignalPDU>(this,q931.GetMessageType(),pdu);
#endif

  // Add special code to detect if call is from a Cisco and remoteApplication needs setting
  if (remoteApplication.IsEmpty() && pdu.m_h323_uu_pdu.HasOptionalField(H225_H323_UU_PDU::e_nonStandardControl)) {
    for (PINDEX i = 0; i < pdu.m_h323_uu_pdu.m_nonStandardControl.GetSize(); i++) {
      const H225_NonStandardIdentifier & id = pdu.m_h323_uu_pdu.m_nonStandardControl[i].m_nonStandardIdentifier;
      if (id.GetTag() == H225_NonStandardIdentifier::e_h221NonStandard) {
        const H225_H221NonStandard & h221 = id;
        if (h221.m_t35CountryCode == 181 && h221.m_t35Extension == 0 && h221.m_manufacturerCode == 18) {
          remoteApplication = "Cisco IOS\t12.x\t181/18";
          PTRACE(2, "H225\tSet remote application name: \"" << remoteApplication << '"');
          break;
        }
      }
    }
  }

  BOOL ok;
  switch (q931.GetMessageType()) {
    case Q931::SetupMsg :
      setupTime = PTime();
      ok = OnReceivedSignalSetup(pdu);
      break;

    case Q931::CallProceedingMsg :
      ok = OnReceivedCallProceeding(pdu);
      break;

    case Q931::ProgressMsg :
      ok = OnReceivedProgress(pdu);
      break;

    case Q931::AlertingMsg :
      ok = OnReceivedAlerting(pdu);
      break;

    case Q931::ConnectMsg :
      connectedTime = PTime();
      ok = OnReceivedSignalConnect(pdu);
      break;

    case Q931::FacilityMsg :
      ok = OnReceivedFacility(pdu);
      break;

    case Q931::SetupAckMsg :
      ok = OnReceivedSignalSetupAck(pdu);
      break;

    case Q931::InformationMsg :
      ok = OnReceivedSignalInformation(pdu);
      break;

    case Q931::NotifyMsg :
      ok = OnReceivedSignalNotify(pdu);
      break;

    case Q931::StatusMsg :
      ok = OnReceivedSignalStatus(pdu);
      break;

    case Q931::StatusEnquiryMsg :
      ok = OnReceivedStatusEnquiry(pdu);
      break;

    case Q931::ReleaseCompleteMsg :
      if (releaseSequence == ReleaseSequenceUnknown)
        releaseSequence = ReleaseSequence_Remote;
      OnReceivedReleaseComplete(pdu);
      ok = FALSE;
      break;

    default :
      ok = OnUnknownSignalPDU(pdu);
  }

  if (ok) {
    // Process tunnelled H245 PDU, if present.
    HandleTunnelPDU(NULL);

    // Check for establishment criteria met
    InternalEstablishedConnectionCheck();
  }

  h245TunnelRxPDU = NULL;

  PString digits = pdu.GetQ931().GetKeypad();
  if (!digits)
    OnUserInputString(digits);

  H323Gatekeeper * gk = endpoint.GetGatekeeper();
  if (gk != NULL)
    gk->InfoRequestResponse(*this, pdu.m_h323_uu_pdu, FALSE);

  Unlock();

  return ok;
}


void H323Connection::HandleTunnelPDU(H323SignalPDU * txPDU)
{
  if (h245TunnelRxPDU == NULL || !h245TunnelRxPDU->m_h323_uu_pdu.m_h245Tunneling)
    return;

  if (!h245Tunneling && h245TunnelRxPDU->m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_setup)
    return;

  H323SignalPDU localTunnelPDU;
  if (txPDU != NULL)
    h245TunnelTxPDU = txPDU;
  else {
    /* Compensate for Cisco bug. IOS cannot seem to accept multiple tunnelled
       H.245 PDUs insode the same facility message */
    if (remoteApplication.Find("Cisco IOS") == P_MAX_INDEX) {
      // Not Cisco, so OK to tunnel multiple PDUs
      localTunnelPDU.BuildFacility(*this, TRUE);
      h245TunnelTxPDU = &localTunnelPDU;
    }
  }

  // if a response to a SETUP PDU containing TCS/MSD was ignored, then shutdown negotiations
  PINDEX i;
  if (lastPDUWasH245inSETUP && 
      (h245TunnelRxPDU->m_h323_uu_pdu.m_h245Control.GetSize() == 0) &&
      (h245TunnelRxPDU->GetQ931().GetMessageType() != Q931::CallProceedingMsg)) {
    PTRACE(4, "H225\tH.245 in SETUP ignored - resetting H.245 negotiations");
    masterSlaveDeterminationProcedure->Stop();
    lastPDUWasH245inSETUP = FALSE;
    capabilityExchangeProcedure->Stop();
  } else {
    for (i = 0; i < h245TunnelRxPDU->m_h323_uu_pdu.m_h245Control.GetSize(); i++) {
      PPER_Stream strm = h245TunnelRxPDU->m_h323_uu_pdu.m_h245Control[i].GetValue();
      HandleControlData(strm);
    }
  }

  // Make sure does not get repeated, clear tunnelled H.245 PDU's
  h245TunnelRxPDU->m_h323_uu_pdu.m_h245Control.SetSize(0);

  if (h245TunnelRxPDU->m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_setup) {
    H225_Setup_UUIE & setup = h245TunnelRxPDU->m_h323_uu_pdu.m_h323_message_body;

    if (setup.HasOptionalField(H225_Setup_UUIE::e_parallelH245Control)) {
      for (i = 0; i < setup.m_parallelH245Control.GetSize(); i++) {
        PPER_Stream strm = setup.m_parallelH245Control[i].GetValue();
        HandleControlData(strm);
      }

      // Make sure does not get repeated, clear tunnelled H.245 PDU's
      setup.m_parallelH245Control.SetSize(0);
    }
  }

  h245TunnelTxPDU = NULL;

  // If had replies, then send them off in their own packet
  if (txPDU == NULL && localTunnelPDU.m_h323_uu_pdu.m_h245Control.GetSize() > 0)
    WriteSignalPDU(localTunnelPDU);
}


static BOOL BuildFastStartList(const H323Channel & channel,
                               H225_ArrayOf_PASN_OctetString & array,
                               H323Channel::Directions reverseDirection)
{
  H245_OpenLogicalChannel open;
  const H323Capability & capability = channel.GetCapability();

  if (channel.GetDirection() != reverseDirection) {
    if (!capability.OnSendingPDU(open.m_forwardLogicalChannelParameters.m_dataType))
      return FALSE;
  }
  else {
    if (!capability.OnSendingPDU(open.m_reverseLogicalChannelParameters.m_dataType))
      return FALSE;

    open.m_forwardLogicalChannelParameters.m_multiplexParameters.SetTag(
                H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters::e_none);
    open.m_forwardLogicalChannelParameters.m_dataType.SetTag(H245_DataType::e_nullData);
    open.IncludeOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters);
  }

  if (!channel.OnSendingPDU(open))
    return FALSE;

  PTRACE(4, "H225\tBuild fastStart:\n  " << setprecision(2) << open);
  PINDEX last = array.GetSize();
  array.SetSize(last+1);
  array[last].EncodeSubType(open);

  PTRACE(3, "H225\tBuilt fastStart for " << capability);
  return TRUE;
}

void H323Connection::OnEstablished()
{
  endpoint.OnConnectionEstablished(*this, callToken);
}

void H323Connection::OnCleared()
{
  endpoint.OnConnectionCleared(*this, callToken);
}


void H323Connection::SetRemoteVersions(const H225_ProtocolIdentifier & protocolIdentifier)
{
  if (protocolIdentifier.GetSize() < 6)
    return;

  h225version = protocolIdentifier[5];

  if (h245versionSet) {
    PTRACE(3, "H225\tSet protocol version to " << h225version);
    return;
  }

  // If has not been told explicitly what the H.245 version use, make an
  // assumption based on the H.225 version
  switch (h225version) {
    case 1 :
      h245version = 2;  // H.323 version 1
      break;
    case 2 :
      h245version = 3;  // H.323 version 2
      break;
    case 3 :
      h245version = 5;  // H.323 version 3
      break;
    case 4 :
      h245version = 7;  // H.323 version 4
      break;
    case 5 :
      h245version = 9;  // H.323 version 5 
      break;
    default:
      h245version = 13; // H.323 version 6
      break;
  }
  PTRACE(3, "H225\tSet protocol version to " << h225version
         << " and implying H.245 version " << h245version);
}

BOOL H323Connection::DecodeFastStartCaps(const H225_ArrayOf_PASN_OctetString & fastStartCaps)
{
  if (!capabilityExchangeProcedure->HasReceivedCapabilities())
    remoteCapabilities.RemoveAll();

  PTRACE(3, "H225\tFast start detected");

  // Extract capabilities from the fast start OpenLogicalChannel structures
  PINDEX i;
  for (i = 0; i < fastStartCaps.GetSize(); i++) {
    H245_OpenLogicalChannel open;
    if (fastStartCaps[i].DecodeSubType(open)) {
      PTRACE(4, "H225\tFast start open:\n  " << setprecision(2) << open);
      unsigned error;
      H323Channel * channel = CreateLogicalChannel(open, TRUE, error);
      if (channel != NULL) {
        if (channel->GetDirection() == H323Channel::IsTransmitter)
          channel->SetNumber(logicalChannels->GetNextChannelNumber());
        fastStartChannels.Append(channel);
      }
    }
    else {
      PTRACE(1, "H225\tInvalid fast start PDU decode:\n  " << open);
    }
  }

  PTRACE(3, "H225\tOpened " << fastStartChannels.GetSize() << " fast start channels");

  // If we are incapable of ANY of the fast start channels, don't do fast start
  if (!fastStartChannels.IsEmpty())
    fastStartState = FastStartResponse;

  return !fastStartChannels.IsEmpty();
}



BOOL H323Connection::OnReceivedSignalSetup(const H323SignalPDU & setupPDU)
{
  if (setupPDU.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_setup)
    return FALSE;

  const H225_Setup_UUIE & setup = setupPDU.m_h323_uu_pdu.m_h323_message_body;

#ifndef DISABLE_CALLAUTH
  /// Do Authentication of Incoming Call before anything else
  if (!ReceiveAuthenticatorPDU<H225_Setup_UUIE>(this,setup, 
							H225_H323_UU_PDU_h323_message_body::e_setup)) {
	 if (GetEndPoint().GetEPSecurityPolicy() == H323EndPoint::SecRequired) {
		PTRACE(4, "H235EP\tAuthentication Failed. Ending Call");
		AuthenticationFailed = TRUE;
		return FALSE;
	 }
	 PTRACE(4, "H235EP\tAuthentication Failed but allowed by policy");
  } else {
	 hasAuthentication = TRUE;
  }
#endif

  switch (setup.m_conferenceGoal.GetTag()) {
    case H225_Setup_UUIE_conferenceGoal::e_create:
    case H225_Setup_UUIE_conferenceGoal::e_join:
      break;

    case H225_Setup_UUIE_conferenceGoal::e_invite:
      return endpoint.OnConferenceInvite(FALSE,this,setupPDU);

    case H225_Setup_UUIE_conferenceGoal::e_callIndependentSupplementaryService:
      IsNonCallConnection = endpoint.OnReceiveCallIndependentSupplementaryService(this,setupPDU);
	  if (!IsNonCallConnection) return FALSE;
	   break;

    case H225_Setup_UUIE_conferenceGoal::e_capability_negotiation:
      return endpoint.OnNegotiateConferenceCapabilities(setupPDU);
  }

  SetRemoteVersions(setup.m_protocolIdentifier);

  // Get the ring pattern
  distinctiveRing = setupPDU.GetDistinctiveRing();

  // Save the identifiers sent by caller
  if (setup.HasOptionalField(H225_Setup_UUIE::e_callIdentifier))
    callIdentifier = setup.m_callIdentifier.m_guid;
  conferenceIdentifier = setup.m_conferenceID;
// 2 lines moved here (from below) by kay27:
   remotePartyName = setupPDU.GetSourceAliases(signallingChannel);
   remoteAliasNames = setupPDU.GetSourceAliasNames();
  SetRemoteApplication(setup.m_sourceInfo);

  // Determine the remote parties name/number/address as best we can
  setupPDU.GetQ931().GetCallingPartyNumber(remotePartyNumber);
//  remotePartyName = setupPDU.GetSourceAliases(signallingChannel);
//  remoteAliasNames = setupPDU.GetSourceAliasNames();

  // get the peer address
  remotePartyAddress = signallingChannel->GetRemoteAddress();
  if (setup.m_sourceAddress.GetSize() > 0)
    remotePartyAddress = H323GetAliasAddressString(setup.m_sourceAddress[0]) + '@' + signallingChannel->GetRemoteAddress();

  // compare the source call signalling address
  if (setup.HasOptionalField(H225_Setup_UUIE::e_sourceCallSignalAddress)) {

    PIPSocket::Address srcAddr, sigAddr;
    H323TransportAddress sourceAddress(setup.m_sourceCallSignalAddress);
    sourceAddress.GetIpAddress(srcAddr);
    signallingChannel->GetRemoteAddress().GetIpAddress(sigAddr);

    // if the peer address is a public address, but the advertised source address is a private address
    // then there is a good chance the remote endpoint is behind a NAT but does not know it.
    // in this case, we active the NAT mode and wait for incoming RTP to provide the media address before 
    // sending anything to the remote endpoint
    if ((!sigAddr.IsRFC1918() && srcAddr.IsRFC1918()) ||    // Internet Address
		((sigAddr.IsRFC1918() && srcAddr.IsRFC1918()) && (sigAddr != srcAddr)))  // LAN on another LAN
    {
      PTRACE(3, "H225\tSource signal address " << srcAddr << " and TCP peer address " << sigAddr << " indicate remote endpoint is behind NAT");
      remoteIsNAT = TRUE;
    }
  }

  // Anything else we need from setup PDU
  mediaWaitForConnect = setup.m_mediaWaitForConnect;

  // Get the local capabilities before fast start or tunnelled TCS is handled
   if (!IsNonCallConnection)
      OnSetLocalCapabilities();

#ifdef H323_H460
     ReceiveSetupFeatureSet(this, setup);
#endif

  // Send back a H323 Call Proceeding PDU in case OnIncomingCall() takes a while
  PTRACE(3, "H225\tSending call proceeding PDU");
  H323SignalPDU callProceedingPDU;
  H225_CallProceeding_UUIE & callProceeding = callProceedingPDU.BuildCallProceeding(*this);

#ifdef H323_H450
  if (!isConsultationTransfer) {
#endif
    if (OnSendCallProceeding(callProceedingPDU)) {
      if (fastStartState == FastStartDisabled)
        callProceeding.IncludeOptionalField(H225_CallProceeding_UUIE::e_fastConnectRefused);

      if (!WriteSignalPDU(callProceedingPDU))
        return FALSE;
    }

    if(endpoint.IsSingleLine()) if(endpoint.GetAllConnections().GetSize() > 1){
      ClearCall(EndedByLocalBusy);
      PTRACE(1,"H225\tLocal endpoint is BUSY (uncheck \"Single Line\" to allow multiple conversations)");
      return false;
    }

if (!IsNonCallConnection) {

    /** Here is a spot where we should wait in case of Call Intrusion
	for CIPL from other endpoints 
	if (isCallIntrusion) return TRUE;
    */

    // if the application indicates not to contine, then send a Q931 Release Complete PDU
    alertingPDU = new H323SignalPDU;
    alertingPDU->BuildAlerting(*this);

    /** If we have a case of incoming call intrusion we should not Clear the Call*/
    {
      CallEndReason incomingCallEndReason = EndedByNoAccept;
      if (!OnIncomingCall(setupPDU, *alertingPDU, incomingCallEndReason)
#ifdef H323_H450
        && (!isCallIntrusion)
#endif
      ) {
        ClearCall(incomingCallEndReason);
        PTRACE(1, "H225\tApplication not accepting calls");
        return FALSE;
      }
    }

    // send Q931 Alerting PDU
    PTRACE(3, "H225\tIncoming call accepted");
}

    // Check for gatekeeper and do admission check if have one
    H323Gatekeeper * gatekeeper = endpoint.GetGatekeeper();
    if (gatekeeper != NULL) {
      H225_ArrayOf_AliasAddress destExtraCallInfoArray;
      H323Gatekeeper::AdmissionResponse response;
      response.destExtraCallInfo = &destExtraCallInfoArray;
      if (!gatekeeper->AdmissionRequest(*this, response)) {
        PTRACE(1, "H225\tGatekeeper refused admission: "
               << (response.rejectReason == UINT_MAX
                    ? PString("Transport error")
                    : H225_AdmissionRejectReason(response.rejectReason).GetTagName()));
        switch (response.rejectReason) {
          case H225_AdmissionRejectReason::e_calledPartyNotRegistered :
            ClearCall(EndedByNoUser);
            break;
          case H225_AdmissionRejectReason::e_requestDenied :
            ClearCall(EndedByNoBandwidth);
            break;
          case H225_AdmissionRejectReason::e_invalidPermission :
          case H225_AdmissionRejectReason::e_securityDenial :
            ClearCall(EndedBySecurityDenial);
            break;
          case H225_AdmissionRejectReason::e_resourceUnavailable :
            ClearCall(EndedByRemoteBusy);
            break;
          default :
            ClearCall(EndedByGatekeeper);
        }
        return FALSE;
      }

      if (destExtraCallInfoArray.GetSize() > 0)
        destExtraCallInfo = H323GetAliasAddressString(destExtraCallInfoArray[0]);
      mustSendDRQ = TRUE;
      gatekeeperRouted = response.gatekeeperRouted;
    }
#ifdef H323_H450
  }
#endif

  if (IsNonCallConnection) 
            return TRUE;

  // Check that it has the H.245 channel connection info
  if (setup.HasOptionalField(H225_Setup_UUIE::e_h245Address))
    if (!StartControlChannel(setup.m_h245Address))
      return FALSE;

  // See if remote endpoint wants to start fast
  if ((fastStartState != FastStartDisabled) && 
       setup.HasOptionalField(H225_Setup_UUIE::e_fastStart) &&
       localCapabilities.GetSize() > 0) {

    DecodeFastStartCaps(setup.m_fastStart);
  }

  // Build the reply with the channels we are actually using
  connectPDU = new H323SignalPDU;
  connectPDU->BuildConnect(*this);

#ifdef H323_H450
  /** If Call Intrusion is allowed we must answer the call*/
  if (IsCallIntrusion()) {
    AnsweringCall(AnswerCallDeferred);
  }
  else {
    if (!isConsultationTransfer) {
#endif
      // call the application callback to determine if to answer the call or not
      connectionState = AwaitingLocalAnswer;
      AnsweringCall(OnAnswerCall(remotePartyName, setupPDU, *connectPDU));
#ifdef H323_H450
    }
    else
      AnsweringCall(AnswerCallNow);
  }
#endif

  return connectionState != ShuttingDownConnection;
}

void H323Connection::SetLocalPartyName(const PString & name)
{
  localPartyName = name;

  if (!name.IsEmpty()) {
    localAliasNames.RemoveAll();
    localAliasNames.AppendString(name);
  }
}


void H323Connection::SetRemotePartyInfo(const H323SignalPDU & pdu)
{
  PString newNumber;
  if (pdu.GetQ931().GetCalledPartyNumber(newNumber))
    remotePartyNumber = newNumber;

  PString newRemotePartyName = pdu.GetQ931().GetDisplayName();
  if (!newRemotePartyName.IsEmpty())
    remotePartyName = newRemotePartyName;
  else if (!remotePartyNumber.IsEmpty())
    remotePartyName = remotePartyNumber;
  else
    remotePartyName = signallingChannel->GetRemoteAddress().GetHostName();

  PTRACE(2, "H225\tSet remote party name: \"" << remotePartyName << '"');
}


void H323Connection::SetRemoteApplication(const H225_EndpointType & pdu)
{
  if (pdu.HasOptionalField(H225_EndpointType::e_vendor)) {
    remoteApplication = H323GetApplicationInfo(pdu.m_vendor);
    PTRACE(2, "H225\tSet remote application name: \"" << remoteApplication << '"');
  }
}


BOOL H323Connection::OnReceivedSignalSetupAck(const H323SignalPDU & /*setupackPDU*/)
{
  OnInsufficientDigits();
  return TRUE;
}


BOOL H323Connection::OnReceivedSignalInformation(const H323SignalPDU & /*infoPDU*/)
{
  return TRUE;
}


BOOL H323Connection::OnReceivedCallProceeding(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_callProceeding)
    return FALSE;
  const H225_CallProceeding_UUIE & call = pdu.m_h323_uu_pdu.m_h323_message_body;

  SetRemoteVersions(call.m_protocolIdentifier);
  SetRemotePartyInfo(pdu);
  SetRemoteApplication(call.m_destinationInfo);

#ifndef DISABLE_CALLAUTH
  if (!ReceiveAuthenticatorPDU<H225_CallProceeding_UUIE>(this,call, 
                 H225_H323_UU_PDU_h323_message_body::e_callProceeding)) {
//          don't do anything
  }
#endif

#ifdef H323_H460
  ReceiveFeatureSet<H225_CallProceeding_UUIE>(this, H460_MessageType::e_callProceeding, call);
#endif

  // Check for fastStart data and start fast
  if (call.HasOptionalField(H225_CallProceeding_UUIE::e_fastStart))
    HandleFastStartAcknowledge(call.m_fastStart);

  // Check that it has the H.245 channel connection info
  if (call.HasOptionalField(H225_CallProceeding_UUIE::e_h245Address))
    return StartControlChannel(call.m_h245Address);

  return TRUE;
}


BOOL H323Connection::OnReceivedProgress(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_progress)
    return FALSE;
  const H225_Progress_UUIE & progress = pdu.m_h323_uu_pdu.m_h323_message_body;

  SetRemoteVersions(progress.m_protocolIdentifier);
  SetRemotePartyInfo(pdu);
  SetRemoteApplication(progress.m_destinationInfo);

  // Check for fastStart data and start fast
  if (progress.HasOptionalField(H225_Progress_UUIE::e_fastStart))
    HandleFastStartAcknowledge(progress.m_fastStart);

  // Check that it has the H.245 channel connection info
  if (progress.HasOptionalField(H225_Progress_UUIE::e_h245Address))
    return StartControlChannel(progress.m_h245Address);

  return TRUE;
}


BOOL H323Connection::OnReceivedAlerting(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_alerting)
    return FALSE;
  const H225_Alerting_UUIE & alert = pdu.m_h323_uu_pdu.m_h323_message_body;

  SetRemoteVersions(alert.m_protocolIdentifier);
  SetRemotePartyInfo(pdu);
  SetRemoteApplication(alert.m_destinationInfo);

#ifndef DISABLE_CALLAUTH
  if (!ReceiveAuthenticatorPDU<H225_Alerting_UUIE>(this,alert, 
                         H225_H323_UU_PDU_h323_message_body::e_alerting)){
//          don't do anything
  }
#endif

#ifdef H323_H248
   if (alert.HasOptionalField(H225_Alerting_UUIE::e_serviceControl))
          OnReceiveServiceControlSessions(alert.m_serviceControl);
#endif

#ifdef H323_H460
  ReceiveFeatureSet<H225_Alerting_UUIE>(this, H460_MessageType::e_alerting, alert);
#endif

  // Check for fastStart data and start fast
  if (alert.HasOptionalField(H225_Alerting_UUIE::e_fastStart))
    HandleFastStartAcknowledge(alert.m_fastStart);

  // Check that it has the H.245 channel connection info
  if (alert.HasOptionalField(H225_Alerting_UUIE::e_h245Address))
    if (!StartControlChannel(alert.m_h245Address))
      return FALSE;

  alertingTime = PTime();
  return OnAlerting(pdu, remotePartyName);
}


BOOL H323Connection::OnReceivedSignalConnect(const H323SignalPDU & pdu)
{

  if (IsNonCallConnection) {
    connectedTime = PTime();
    connectionState = EstablishedConnection;
    return TRUE;
  }

  if (connectionState == ShuttingDownConnection)
    return FALSE;
  connectionState = HasExecutedSignalConnect;

  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_connect)
    return FALSE;
  const H225_Connect_UUIE & connect = pdu.m_h323_uu_pdu.m_h323_message_body;

  SetRemoteVersions(connect.m_protocolIdentifier);
  SetRemotePartyInfo(pdu);
  SetRemoteApplication(connect.m_destinationInfo);

#ifndef DISABLE_CALLAUTH
   if (!ReceiveAuthenticatorPDU<H225_Connect_UUIE>(this,connect, 
                         H225_H323_UU_PDU_h323_message_body::e_connect)) {
//          don't do anything
   }
#endif

#ifdef H323_H460
  ReceiveFeatureSet<H225_Connect_UUIE>(this, H460_MessageType::e_connect, connect);
#endif

  if (!OnOutgoingCall(pdu)) {
    ClearCall(EndedByNoAccept);
    return FALSE;
  }

#ifdef H323_H450
  // Are we involved in a transfer with a non H.450.2 compatible transferred-to endpoint?
  if (h4502handler->GetState() == H4502Handler::e_ctAwaitSetupResponse &&
      h4502handler->IsctTimerRunning())
  {
    PTRACE(4, "H4502\tRemote Endpoint does not support H.450.2.");
    h4502handler->OnReceivedSetupReturnResult();
  }
#endif

  // have answer, so set timeout to interval for monitoring calls health
  signallingChannel->SetReadTimeout(MonitorCallStatusTime);

  // Check for fastStart data and start fast
  if (connect.HasOptionalField(H225_Connect_UUIE::e_fastStart))
    HandleFastStartAcknowledge(connect.m_fastStart);

  // Check that it has the H.245 channel connection info
  if (connect.HasOptionalField(H225_Connect_UUIE::e_h245Address)) {
    if (!StartControlChannel(connect.m_h245Address)) {
      if (fastStartState != FastStartAcknowledged)
        return FALSE;
    }
  }

  // If didn't get fast start channels accepted by remote then clear our
  // proposed channels
  if (fastStartState != FastStartAcknowledged) {
    fastStartState = FastStartDisabled;
    fastStartChannels.RemoveAll();
  }

  PTRACE(4, "H225\tFast Start " << (h245Tunneling ? "TRUE" : "FALSE")
									<< " fastStartState " << fastStartState);

  // If we have a H.245 channel available, bring it up. We either have media
  // and this is just so user indications work, or we don't have media and
  // desperately need it!
  if (h245Tunneling || controlChannel != NULL)
      return OnStartHandleControlChannel();

  // We have no tunnelling and not separate channel, but we really want one
  // so we will start one using a facility message
  PTRACE(2, "H225\tNo H245 address provided by remote, starting control channel");

  if (!StartControlChannel())
    return FALSE;

  H323SignalPDU want245PDU;
  H225_Facility_UUIE * fac = want245PDU.BuildFacility(*this, FALSE, H225_FacilityReason::e_startH245);

  fac->IncludeOptionalField(H225_Facility_UUIE::e_h245Address);
  controlChannel->SetUpTransportPDU(fac->m_h245Address, TRUE);

  return WriteSignalPDU(want245PDU);
}


BOOL H323Connection::OnReceivedFacility(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_empty)
    return TRUE;

  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_facility)
    return FALSE;
  const H225_Facility_UUIE & fac = pdu.m_h323_uu_pdu.m_h323_message_body;

#ifndef DISABLE_CALLAUTH
  if (!ReceiveAuthenticatorPDU<H225_Facility_UUIE>(this,fac, 
                            H225_H323_UU_PDU_h323_message_body::e_facility)) {
//          don't do anything
  }
#endif

#ifdef H323_H248
   if (fac.HasOptionalField(H225_Facility_UUIE::e_serviceControl))
          OnReceiveServiceControlSessions(fac.m_serviceControl);
#endif

#ifdef H323_H460
   // Do not process H.245 Control PDU's
   if (!pdu.m_h323_uu_pdu.HasOptionalField(H225_H323_UU_PDU::e_h245Control))
         ReceiveFeatureSet<H225_Facility_UUIE>(this, H460_MessageType::e_facility, fac);
#endif

  SetRemoteVersions(fac.m_protocolIdentifier);

  // Check for fastStart data and start fast
  if (fac.HasOptionalField(H225_Facility_UUIE::e_fastStart))
    HandleFastStartAcknowledge(fac.m_fastStart);

  // Check that it has the H.245 channel connection info
  if (fac.HasOptionalField(H225_Facility_UUIE::e_h245Address)) {
    if (controlChannel != NULL && !controlChannel->IsOpen()) {
      // Fix race condition where both side want to open H.245 channel. we have
      // channel bit it is not open (ie we are listening) and the remote has
      // sent us an address to connect to. To resolve we compare the addresses.

      H225_TransportAddress myAddress;
      controlChannel->GetLocalAddress().SetPDU(myAddress);
      PPER_Stream myBuffer;
      myAddress.Encode(myBuffer);

      PPER_Stream otherBuffer;
      fac.m_h245Address.Encode(otherBuffer);

      if (myBuffer < otherBuffer) {
        PTRACE(2, "H225\tSimultaneous start of H.245 channel, connecting to remote.");
        controlChannel->CleanUpOnTermination();
        delete controlChannel;
        controlChannel = NULL;
      }
      else {
        PTRACE(2, "H225\tSimultaneous start of H.245 channel, using local listener.");
      }
    }

    if (!StartControlChannel(fac.m_h245Address))
      return FALSE;
  }

  if (fac.m_reason.GetTag() != H225_FacilityReason::e_callForwarded)
    return TRUE;

  PString address;
  if (fac.HasOptionalField(H225_Facility_UUIE::e_alternativeAliasAddress) &&
      fac.m_alternativeAliasAddress.GetSize() > 0)
    address = H323GetAliasAddressString(fac.m_alternativeAliasAddress[0]);

  if (fac.HasOptionalField(H225_Facility_UUIE::e_alternativeAddress)) {
    if (!address)
      address += '@';
    address += H323TransportAddress(fac.m_alternativeAddress);
  }

  if (endpoint.OnConnectionForwarded(*this, address, pdu)) {
    ClearCall(EndedByCallForwarded);
    return FALSE;
  }

  if (!endpoint.CanAutoCallForward())
    return TRUE;

  if (!endpoint.ForwardConnection(*this, address, pdu))
    return TRUE;

  // This connection is on the way out and a new one has the same token now
  // so change our token to make sure no accidents can happen clearing the
  // wrong call
  callToken += "-forwarded";
  return FALSE;
}


BOOL H323Connection::OnReceivedSignalNotify(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_notify) {
    const H225_Notify_UUIE & notify = pdu.m_h323_uu_pdu.m_h323_message_body;
    SetRemoteVersions(notify.m_protocolIdentifier);
  }
  return TRUE;
}


BOOL H323Connection::OnReceivedSignalStatus(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_status) {
    const H225_Status_UUIE & status = pdu.m_h323_uu_pdu.m_h323_message_body;
    SetRemoteVersions(status.m_protocolIdentifier);
  }
  return TRUE;
}


BOOL H323Connection::OnReceivedStatusEnquiry(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_statusInquiry) {
    const H225_StatusInquiry_UUIE & status = pdu.m_h323_uu_pdu.m_h323_message_body;
    SetRemoteVersions(status.m_protocolIdentifier);
  }

  H323SignalPDU reply;
  reply.BuildStatus(*this);
  return reply.Write(*signallingChannel,*this);
}


void H323Connection::OnReceivedReleaseComplete(const H323SignalPDU & pdu)
{
  if (!callEndTime.IsValid())
    callEndTime = PTime();

  endSessionReceived.Signal();

  if (q931Cause == Q931::ErrorInCauseIE)
    q931Cause = pdu.GetQ931().GetCause();

  const H225_ReleaseComplete_UUIE & rc = pdu.m_h323_uu_pdu.m_h323_message_body;

  switch (connectionState) {
    case EstablishedConnection :
      if (rc.m_reason.GetTag() == H225_ReleaseCompleteReason::e_facilityCallDeflection)
        ClearCall(EndedByCallForwarded);
      else
        ClearCall(EndedByRemoteUser);
      break;

    case AwaitingLocalAnswer :
      if (rc.m_reason.GetTag() == H225_ReleaseCompleteReason::e_facilityCallDeflection)
        ClearCall(EndedByCallForwarded);
      else
        ClearCall(EndedByCallerAbort);
      break;

    default :
      if (callEndReason == EndedByRefusal)
        callEndReason = NumCallEndReasons;
      
#ifdef H323_H450
      // Are we involved in a transfer with a non H.450.2 compatible transferred-to endpoint?
      if (h4502handler->GetState() == H4502Handler::e_ctAwaitSetupResponse &&
          h4502handler->IsctTimerRunning())
      {
        PTRACE(4, "H4502\tThe Remote Endpoint has rejected our transfer request and does not support H.450.2.");
        h4502handler->OnReceivedSetupReturnError(H4501_GeneralErrorList::e_notAvailable);
      }
#endif

#ifdef H323_H460
    ReceiveFeatureSet<H225_ReleaseComplete_UUIE>(this, H460_MessageType::e_releaseComplete, rc);
#endif

      if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_releaseComplete)
        ClearCall(EndedByRefusal);
      else {
        SetRemoteVersions(rc.m_protocolIdentifier);
        ClearCall(H323TranslateToCallEndReason(pdu.GetQ931().GetCause(), rc.m_reason));
      }
  }
}

BOOL H323Connection::OnIncomingCall(const H323SignalPDU & setupPDU,   
                                          H323SignalPDU & alertingPDU,      
                                          CallEndReason & reason)
{
  return endpoint.OnIncomingCall(*this, setupPDU, alertingPDU, reason);
}

BOOL H323Connection::OnIncomingCall(const H323SignalPDU & setupPDU,
                                          H323SignalPDU & alertingPDU)
{
  return endpoint.OnIncomingCall(*this, setupPDU, alertingPDU);
}


BOOL H323Connection::ForwardCall(const PString & forwardParty)
{
  if (forwardParty.IsEmpty())
    return FALSE;

  PString alias;
  H323TransportAddress address;

  PStringList Addresses;
  if (!endpoint.ResolveCallParty(forwardParty, Addresses)) 
	  return FALSE;

  if (!endpoint.ParsePartyName(Addresses[0], alias, address)) {
	  PTRACE(2, "H323\tCould not parse forward party \"" << forwardParty << '"');
	  return FALSE;
  }

  H323SignalPDU redirectPDU;
  H225_Facility_UUIE * fac = redirectPDU.BuildFacility(*this, FALSE, H225_FacilityReason::e_callForwarded);

  if (!address) {
    fac->IncludeOptionalField(H225_Facility_UUIE::e_alternativeAddress);
    address.SetPDU(fac->m_alternativeAddress);
  }

  if (!alias) {
    fac->IncludeOptionalField(H225_Facility_UUIE::e_alternativeAliasAddress);
    fac->m_alternativeAliasAddress.SetSize(1);
    H323SetAliasAddress(alias, fac->m_alternativeAliasAddress[0]);
  }

  return WriteSignalPDU(redirectPDU);
}


H323Connection::AnswerCallResponse
     H323Connection::OnAnswerCall(const PString & caller,
                                  const H323SignalPDU & setupPDU,
                                  H323SignalPDU & connectPDU)
{
  return endpoint.OnAnswerCall(*this, caller, setupPDU, connectPDU);
}


void H323Connection::AnsweringCall(AnswerCallResponse response)
{
  PTRACE(2, "H323\tAnswering call: " << response);

  if (!Lock())
    return;

  switch (response) {
    default : // AnswerCallDeferred
      break;

    case AnswerCallDeferredWithMedia :
      if (!mediaWaitForConnect) {
        // create a new facility PDU if doing AnswerDeferredWithMedia
        H323SignalPDU want245PDU;
        H225_Progress_UUIE & prog = want245PDU.BuildProgress(*this);

        BOOL sendPDU = TRUE;

        if (SendFastStartAcknowledge(prog.m_fastStart))
          prog.IncludeOptionalField(H225_Progress_UUIE::e_fastStart);
        else {
          // See if aborted call
          if (connectionState == ShuttingDownConnection)
            break;

          // Do early H.245 start
          H225_Facility_UUIE & fac = *want245PDU.BuildFacility(*this, FALSE, H225_FacilityReason::e_startH245);
          earlyStart = TRUE;
          if (!h245Tunneling && (controlChannel == NULL)) {
            if (!StartControlChannel())
              break;

            fac.IncludeOptionalField(H225_Facility_UUIE::e_h245Address);
            controlChannel->SetUpTransportPDU(fac.m_h245Address, TRUE);
          } 
          else
            sendPDU = FALSE;
        }

        if (sendPDU) {
          HandleTunnelPDU(&want245PDU);
          WriteSignalPDU(want245PDU);
        }
      }
      break;

    case AnswerCallAlertWithMedia :
      if (alertingPDU != NULL && !mediaWaitForConnect) {
        H225_Alerting_UUIE & alerting = alertingPDU->m_h323_uu_pdu.m_h323_message_body;

        BOOL sendPDU = TRUE;
        if (SendFastStartAcknowledge(alerting.m_fastStart))
          alerting.IncludeOptionalField(H225_Alerting_UUIE::e_fastStart);
        else {
          alerting.IncludeOptionalField(H225_Alerting_UUIE::e_fastConnectRefused);

          // See if aborted call
          if (connectionState == ShuttingDownConnection)
            break;

          // Do early H.245 start
          earlyStart = TRUE;
          if (!h245Tunneling && (controlChannel == NULL)) {
            if (!StartControlChannel())
              break;
            alerting.IncludeOptionalField(H225_Alerting_UUIE::e_h245Address);
            controlChannel->SetUpTransportPDU(alerting.m_h245Address, TRUE);
          }
          else
            sendPDU = FALSE;
        }

        if (sendPDU) {
          HandleTunnelPDU(alertingPDU);

#ifdef H323_H450
          h450dispatcher->AttachToAlerting(*alertingPDU);
#endif

          WriteSignalPDU(*alertingPDU);
          alertingTime = PTime();
        }
        break;
      }
      // else clause falls into AnswerCallPending case

    case AnswerCallPending :
      if (alertingPDU != NULL) {
        // send Q931 Alerting PDU
        PTRACE(3, "H225\tSending Alerting PDU");

        HandleTunnelPDU(alertingPDU);

#ifdef H323_H450
        h450dispatcher->AttachToAlerting(*alertingPDU);
#endif

        // commented out by CRS: no need to check for lack of fastStart channels
        // as this Alerting is not associated with media channel. And doing so
        // screws up deferred fastStart setup
        //
        //if (fastStartChannels.IsEmpty()) {
        //  H225_Alerting_UUIE & alerting = alertingPDU->m_h323_uu_pdu.m_h323_message_body;
        //  alerting.IncludeOptionalField(H225_Alerting_UUIE::e_fastConnectRefused);
        //}

        WriteSignalPDU(*alertingPDU);
        alertingTime = PTime();
      }
      break;

    case AnswerCallDenied :
      // If response is denied, abort the call
      PTRACE(1, "H225\tApplication has declined to answer incoming call");
      ClearCall(EndedByAnswerDenied);
      break;

    case AnswerCallDeniedByInvalidCID :
      // If response is denied, abort the call
      PTRACE(1, "H225\tApplication has refused to answer incoming call due to invalid conference ID");
      ClearCall(EndedByInvalidConferenceID);
      break;

    case AnswerCallNow :
      if (connectPDU != NULL) {
        H225_Connect_UUIE & connect = connectPDU->m_h323_uu_pdu.m_h323_message_body;
        // Now ask the application to select which channels to start
        if (SendFastStartAcknowledge(connect.m_fastStart))
          connect.IncludeOptionalField(H225_Connect_UUIE::e_fastStart);
        else
          connect.IncludeOptionalField(H225_Connect_UUIE::e_fastConnectRefused);

        // See if aborted call
        if (connectionState == ShuttingDownConnection)
          break;

        // Set flag that we are up to CONNECT stage
        connectionState = HasExecutedSignalConnect;

#ifdef H323_H450
        h450dispatcher->AttachToConnect(*connectPDU);
#endif
 
        if (h245Tunneling) {
          // If no channels selected (or never provided) do traditional H245 start
          if (fastStartState == FastStartDisabled) {
            h245TunnelTxPDU = connectPDU; // Piggy back H245 on this reply
            BOOL ok = StartControlNegotiations();
            h245TunnelTxPDU = NULL;
            if (!ok)
              break;
          }

          HandleTunnelPDU(connectPDU);
        }
        else { // Start separate H.245 channel if not tunneling.
          if (!StartControlChannel())
            break;
          connect.IncludeOptionalField(H225_Connect_UUIE::e_h245Address);
          controlChannel->SetUpTransportPDU(connect.m_h245Address, TRUE);
        }

        connectedTime = PTime();
        WriteSignalPDU(*connectPDU); // Send H323 Connect PDU
        delete connectPDU;
        connectPDU = NULL;
        delete alertingPDU;
        alertingPDU = NULL;
      }
  }

  InternalEstablishedConnectionCheck();
  Unlock();
}

H323Connection::CallEndReason H323Connection::SendSignalSetup(const PString & alias,
                                                              const H323TransportAddress & address)
{
  // Start the call, first state is asking gatekeeper
  connectionState = AwaitingGatekeeperAdmission;

  // Indicate the direction of call.
  if (alias.IsEmpty())
    remotePartyName = remotePartyAddress = address;
  else {
    remotePartyName = alias;
	remoteAliasNames.AppendString(alias);
    remotePartyAddress = alias + '@' + address;
  }

  // Start building the setup PDU to get various ID's
  H323SignalPDU setupPDU;
  H225_Setup_UUIE & setup = setupPDU.BuildSetup(*this, address);

#ifdef H323_H450
  h450dispatcher->AttachToSetup(setupPDU);
#endif

  // Save the identifiers generated by BuildSetup
  setupPDU.GetQ931().GetCalledPartyNumber(remotePartyNumber);

  H323TransportAddress gatekeeperRoute = address;

  // Check for gatekeeper and do admission check if have one
  H323Gatekeeper * gatekeeper = endpoint.GetGatekeeper();
  H225_ArrayOf_AliasAddress newAliasAddresses;
  if (gatekeeper != NULL) {
    H323Gatekeeper::AdmissionResponse response;
    response.transportAddress = &gatekeeperRoute;
    response.aliasAddresses = &newAliasAddresses;
    if (!gkAccessTokenOID)
      response.accessTokenData = &gkAccessTokenData;
    while (!gatekeeper->AdmissionRequest(*this, response, alias.IsEmpty())) {
      PTRACE(1, "H225\tGatekeeper refused admission: "
             << (response.rejectReason == UINT_MAX
                  ? PString("Transport error")
                  : H225_AdmissionRejectReason(response.rejectReason).GetTagName()));
#ifdef H323_H450
      h4502handler->onReceivedAdmissionReject(H4501_GeneralErrorList::e_notAvailable);
#endif

      switch (response.rejectReason) {
        case H225_AdmissionRejectReason::e_calledPartyNotRegistered :
          return EndedByNoUser;
        case H225_AdmissionRejectReason::e_requestDenied :
          return EndedByNoBandwidth;
        case H225_AdmissionRejectReason::e_invalidPermission :
        case H225_AdmissionRejectReason::e_securityDenial :
          return EndedBySecurityDenial;
        case H225_AdmissionRejectReason::e_resourceUnavailable :
          return EndedByRemoteBusy;
        case H225_AdmissionRejectReason::e_incompleteAddress :
          if (OnInsufficientDigits())
            break;
          // Then default case
        default :
          return EndedByGatekeeper;
      }

      PString lastRemotePartyName = remotePartyName;
      while (lastRemotePartyName == remotePartyName) {
        Unlock(); // Release the mutex as can deadlock trying to clear call during connect.
        digitsWaitFlag.Wait();
        if (!Lock()) // Lock while checking for shutting down.
          return EndedByCallerAbort;
      }
    }
    mustSendDRQ = TRUE;
    if (response.gatekeeperRouted) {
      setup.IncludeOptionalField(H225_Setup_UUIE::e_endpointIdentifier);
      setup.m_endpointIdentifier = gatekeeper->GetEndpointIdentifier();
      gatekeeperRouted = TRUE;
    }
  }

  // Update the field e_destinationAddress in the SETUP PDU to reflect the new 
  // alias received in the ACF (m_destinationInfo).
  if (newAliasAddresses.GetSize() > 0) {
    setup.IncludeOptionalField(H225_Setup_UUIE::e_destinationAddress);
    setup.m_destinationAddress = newAliasAddresses;

    // Update the Q.931 Information Element (if is an E.164 address)
    PString e164 = H323GetAliasAddressE164(newAliasAddresses);
    if (!e164)
      remotePartyNumber = e164;
  }

  if (addAccessTokenToSetup && !gkAccessTokenOID && !gkAccessTokenData.IsEmpty()) {
    PString oid1, oid2;
    PINDEX comma = gkAccessTokenOID.Find(',');
    if (comma == P_MAX_INDEX)
      oid1 = oid2 = gkAccessTokenOID;
    else {
      oid1 = gkAccessTokenOID.Left(comma);
      oid2 = gkAccessTokenOID.Mid(comma+1);
    }
    setup.IncludeOptionalField(H225_Setup_UUIE::e_tokens);
    PINDEX last = setup.m_tokens.GetSize();
    setup.m_tokens.SetSize(last+1);
    setup.m_tokens[last].m_tokenOID = oid1;
    setup.m_tokens[last].IncludeOptionalField(H235_ClearToken::e_nonStandard);
    setup.m_tokens[last].m_nonStandard.m_nonStandardIdentifier = oid2;
    setup.m_tokens[last].m_nonStandard.m_data = gkAccessTokenData;
  }

  if (!signallingChannel->SetRemoteAddress(gatekeeperRoute)) {
    PTRACE(1, "H225\tInvalid "
           << (gatekeeperRoute != address ? "gatekeeper" : "user")
           << " supplied address: \"" << gatekeeperRoute << '"');
    connectionState = AwaitingTransportConnect;
    return EndedByConnectFail;
  }

  // Do the transport connect
  connectionState = AwaitingTransportConnect;

  // Release the mutex as can deadlock trying to clear call during connect.
  Unlock();

  signallingChannel->SetWriteTimeout(100);

  BOOL connectFailed = !signallingChannel->Connect();

  // Lock while checking for shutting down.
  if (!Lock())
    return EndedByCallerAbort;

  // See if transport connect failed, abort if so.
  if (connectFailed) {
    connectionState = NoConnectionActive;
    switch (signallingChannel->GetErrorNumber()) {
      case ENETUNREACH :
        return EndedByUnreachable;
      case ECONNREFUSED :
        return EndedByNoEndPoint;
      case ETIMEDOUT :
        return EndedByHostOffline;
    }
    return EndedByConnectFail;
  }

  PTRACE(3, "H225\tSending Setup PDU");
  connectionState = AwaitingSignalConnect;

 // Add CryptoTokens and H460 features if available (need to do this after the ARQ/ACF)
#ifndef DISABLE_CALLAUTH
   setupPDU.InsertCryptoTokensSetup(*this,setup);
#endif

#ifdef H323_H460
  setupPDU.InsertH460Setup(*this,setup);
#endif

  // Put in all the signalling addresses for link
  setup.IncludeOptionalField(H225_Setup_UUIE::e_sourceCallSignalAddress);
  signallingChannel->SetUpTransportPDU(setup.m_sourceCallSignalAddress, TRUE, this);
  if (!setup.HasOptionalField(H225_Setup_UUIE::e_destCallSignalAddress)) {
    setup.IncludeOptionalField(H225_Setup_UUIE::e_destCallSignalAddress);
    signallingChannel->SetUpTransportPDU(setup.m_destCallSignalAddress, FALSE, this);
  }

  // If a standard call do Fast Start (if required)
if (setup.m_conferenceGoal.GetTag() == H225_Setup_UUIE_conferenceGoal::e_create) {

  // Get the local capabilities before fast start is handled
  OnSetLocalCapabilities();

  // Ask the application what channels to open
  PTRACE(3, "H225\tCheck for Fast start by local endpoint");
  fastStartChannels.RemoveAll();
  OnSelectLogicalChannels();

  // If application called OpenLogicalChannel, put in the fastStart field
  if (!fastStartChannels.IsEmpty()) {
    PTRACE(3, "H225\tFast start begun by local endpoint");
    for (PINDEX i = 0; i < fastStartChannels.GetSize(); i++)
      BuildFastStartList(fastStartChannels[i], setup.m_fastStart, H323Channel::IsReceiver);
    if (setup.m_fastStart.GetSize() > 0)
      setup.IncludeOptionalField(H225_Setup_UUIE::e_fastStart);
  }

  // Search the capability set and see if we have video capability
  for (PINDEX i = 0; i < localCapabilities.GetSize(); i++) {
    switch (localCapabilities[i].GetMainType()) {
      case H323Capability::e_Audio :
      case H323Capability::e_UserInput :
        break;

      default : // Is video or other data (eg T.120)
        setupPDU.GetQ931().SetBearerCapabilities(Q931::TransferUnrestrictedDigital, 6);
        i = localCapabilities.GetSize(); // Break out of the for loop
        break;
    }
  }
}

  if (!OnSendSignalSetup(setupPDU))
    return EndedByNoAccept;

  // Do this again (was done when PDU was constructed) in case
  // OnSendSignalSetup() changed something.
  setupPDU.SetQ931Fields(*this, TRUE);
  setupPDU.GetQ931().GetCalledPartyNumber(remotePartyNumber);

  fastStartState = FastStartDisabled;
  BOOL set_lastPDUWasH245inSETUP = FALSE;

  if (h245Tunneling && doH245inSETUP) {
    h245TunnelTxPDU = &setupPDU;

    // Try and start the master/slave and capability exchange through the tunnel
    // Note: this used to be disallowed but is now allowed as of H323v4
    BOOL ok = StartControlNegotiations();

    h245TunnelTxPDU = NULL;

    if (!ok)
      return EndedByTransportFail;

    if (setup.m_fastStart.GetSize() > 0) {
      // Now if fast start as well need to put this in setup specific field
      // and not the generic H.245 tunneling field
      setup.IncludeOptionalField(H225_Setup_UUIE::e_parallelH245Control);
      setup.m_parallelH245Control = setupPDU.m_h323_uu_pdu.m_h245Control;
      setupPDU.m_h323_uu_pdu.RemoveOptionalField(H225_H323_UU_PDU::e_h245Control);
      set_lastPDUWasH245inSETUP = TRUE;
    }
  }

  // Send the initial PDU
  setupTime = PTime();
  if (!WriteSignalPDU(setupPDU))
    return EndedByTransportFail;

  // WriteSignalPDU always resets lastPDUWasH245inSETUP.
  // So set it here if required
  if (set_lastPDUWasH245inSETUP)
    lastPDUWasH245inSETUP = TRUE;

  // Set timeout for remote party to answer the call
  signallingChannel->SetReadTimeout(endpoint.GetSignallingChannelCallTimeout());

  return NumCallEndReasons;
}

#if P_STUN
void H323Connection::OnSetRTPNat(unsigned sessionid, PNatMethod & nat) const
{
}
#endif

void H323Connection::SetEndpointTypeInfo(H225_EndpointType & info) const
{
	return endpoint.SetEndpointTypeInfo(info);
}


BOOL H323Connection::OnSendSignalSetup(H323SignalPDU & /*setupPDU*/)
{
  return TRUE;
}


BOOL H323Connection::OnSendCallProceeding(H323SignalPDU & /*callProceedingPDU*/)
{
  return TRUE;
}


BOOL H323Connection::OnSendReleaseComplete(H323SignalPDU & /*releaseCompletePDU*/)
{
  return TRUE;
}


BOOL H323Connection::OnAlerting(const H323SignalPDU & alertingPDU,
                                const PString & username)
{
  return endpoint.OnAlerting(*this, alertingPDU, username);
}


BOOL H323Connection::OnInsufficientDigits()
{
  return FALSE;
}


void H323Connection::SendMoreDigits(const PString & digits)
{
  remotePartyNumber += digits;
  remotePartyName = remotePartyNumber;
  if (connectionState == AwaitingGatekeeperAdmission)
    digitsWaitFlag.Signal();
  else {
    H323SignalPDU infoPDU;
    infoPDU.BuildInformation(*this);
    infoPDU.GetQ931().SetCalledPartyNumber(digits);
    if (!WriteSignalPDU(infoPDU))
      ClearCall(EndedByTransportFail);
  }
}

BOOL H323Connection::OnOutgoingCall(const H323SignalPDU & connectPDU)
{
  return endpoint.OnOutgoingCall(*this, connectPDU);
}

BOOL H323Connection::SendFastStartAcknowledge(H225_ArrayOf_PASN_OctetString & array)
{
  PINDEX i;

  // See if we have already added the fast start OLC's
  if (array.GetSize() > 0)
    return TRUE;

  // See if we need to select our fast start channels
  if (fastStartState == FastStartResponse)
    OnSelectLogicalChannels();

  // Remove any channels that were not started by OnSelectLogicalChannels(),
  // those that were started are put into the logical channel dictionary
  for (i = 0; i < fastStartChannels.GetSize(); i++) {
    if (fastStartChannels[i].IsRunning())
      logicalChannels->Add(fastStartChannels[i]);
    else
      fastStartChannels.RemoveAt(i--);
  }

  // None left, so didn't open any channels fast
  if (fastStartChannels.IsEmpty()) {
    fastStartState = FastStartDisabled;
    return FALSE;
  }

  // The channels we just transferred to the logical channels dictionary
  // should not be deleted via this structure now.
  fastStartChannels.DisallowDeleteObjects();

  PTRACE(3, "H225\tAccepting fastStart for " << fastStartChannels.GetSize() << " channels");

  for (i = 0; i < fastStartChannels.GetSize(); i++)
    BuildFastStartList(fastStartChannels[i], array, H323Channel::IsTransmitter);

  // Have moved open channels to logicalChannels structure, remove all others.
  fastStartChannels.RemoveAll();

  // Set flag so internal establishment check does not require H.245
  fastStartState = FastStartAcknowledged;

  endSessionNeeded = FALSE;  

  return TRUE;
}


BOOL H323Connection::HandleFastStartAcknowledge(const H225_ArrayOf_PASN_OctetString & array)
{
  if (fastStartChannels.IsEmpty()) {
    PTRACE(3, "H225\tFast start response with no channels to open");
    return FALSE;
  }

  // record the time at which media was opened
  reverseMediaOpenTime = PTime();

  PTRACE(3, "H225\tFast start accepted by remote endpoint");

  PINDEX i;

  // Go through provided list of structures, if can decode it and match it up
  // with a channel we requested AND it has all the information needed in the
  // m_multiplexParameters, then we can start the channel.
  for (i = 0; i < array.GetSize(); i++) {
    H245_OpenLogicalChannel open;
    if (array[i].DecodeSubType(open)) {
      PTRACE(4, "H225\tFast start open:\n  " << setprecision(2) << open);
      BOOL reverse = open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters);
      const H245_DataType & dataType = reverse ? open.m_reverseLogicalChannelParameters.m_dataType
                                               : open.m_forwardLogicalChannelParameters.m_dataType;
      H323Capability * replyCapability = localCapabilities.FindCapability(dataType);
      if (replyCapability != NULL) {
        for (PINDEX ch = 0; ch < fastStartChannels.GetSize(); ch++) {
          H323Channel & channelToStart = fastStartChannels[ch];
          H323Channel::Directions dir = channelToStart.GetDirection();
          if ((dir == H323Channel::IsReceiver) == reverse &&
               channelToStart.GetCapability() == *replyCapability) {
            unsigned error = 1000;
            if (channelToStart.OnReceivedPDU(open, error)) {
              H323Capability * channelCapability;
              if (dir == H323Channel::IsReceiver)
                channelCapability = replyCapability;
              else {
                // For transmitter, need to fake a capability into the remote table
                channelCapability = remoteCapabilities.FindCapability(channelToStart.GetCapability());
                if (channelCapability == NULL) {
                  channelCapability = remoteCapabilities.Copy(channelToStart.GetCapability());
                  remoteCapabilities.SetCapability(0, channelCapability->GetDefaultSessionID()-1, channelCapability);
                }
              }
              // Must use the actual capability instance from the
              // localCapability or remoteCapability structures.
              if (OnCreateLogicalChannel(*channelCapability, dir, error)) {
                if (channelToStart.SetInitialBandwidth()) {
                  channelToStart.Start();
                  break;
                }
                else
                  PTRACE(2, "H225\tFast start channel open fail: insufficent bandwidth");
              }
              else
                PTRACE(2, "H225\tFast start channel open error: " << error);
            }
            else
              PTRACE(2, "H225\tFast start capability error: " << error);
          }
        }
      }
    }
    else {
      PTRACE(1, "H225\tInvalid fast start PDU decode:\n  " << setprecision(2) << open);
    }
  }

  // Remove any channels that were not started by above, those that were
  // started are put into the logical channel dictionary
  for (i = 0; i < fastStartChannels.GetSize(); i++) {
    if (fastStartChannels[i].IsRunning())
      logicalChannels->Add(fastStartChannels[i]);
    else
      fastStartChannels.RemoveAt(i--);
  }

  // The channels we just transferred to the logical channels dictionary
  // should not be deleted via this structure now.
  fastStartChannels.DisallowDeleteObjects();

  PTRACE(2, "H225\tFast starting " << fastStartChannels.GetSize() << " channels");
  if (fastStartChannels.IsEmpty())
    return FALSE;

  // Have moved open channels to logicalChannels structure, remove them now.
  fastStartChannels.RemoveAll();

  fastStartState = FastStartAcknowledged;

  endSessionNeeded = FALSE; 

  return TRUE;
}


BOOL H323Connection::StartControlChannel()
{
  // Already have the H245 channel up.
  if (controlChannel != NULL)
    return TRUE;

  controlChannel = signallingChannel->CreateControlChannel(*this);
  if (controlChannel == NULL) {
    ClearCall(EndedByTransportFail);
    return FALSE;
  }
  
  controlChannel->StartControlChannel(*this);
  return TRUE;
}


BOOL H323Connection::StartControlChannel(const H225_TransportAddress & h245Address)
{
  // Check that it is an IP address, all we support at the moment
  if (h245Address.GetTag() != H225_TransportAddress::e_ipAddress
#if P_HAS_IPV6
        && h245Address.GetTag() != H225_TransportAddress::e_ip6Address
#endif
      ) {
    PTRACE(1, "H225\tConnect of H245 failed: Unsupported transport");
    return FALSE;
  }

  // Already have the H245 channel up.
  if (controlChannel != NULL)
    return TRUE;

  controlChannel = new H323TransportTCP(endpoint);
  if (!controlChannel->SetRemoteAddress(h245Address)) {
    PTRACE(1, "H225\tCould not extract H245 address");
    delete controlChannel;
    controlChannel = NULL;
    return FALSE;
  }

  if (!controlChannel->Connect()) {
    PTRACE(1, "H225\tConnect of H245 failed: " << controlChannel->GetErrorText());
    delete controlChannel;
    controlChannel = NULL;
    return FALSE;
  }

  controlChannel->StartControlChannel(*this);
  return TRUE;
}


BOOL H323Connection::OnUnknownSignalPDU(const H323SignalPDU & PTRACE_PARAM(pdu))
{
  PTRACE(2, "H225\tUnknown signalling PDU: " << pdu);
  return TRUE;
}


BOOL H323Connection::WriteControlPDU(const H323ControlPDU & pdu)
{
  PPER_Stream strm;
  pdu.Encode(strm);
  strm.CompleteEncoding();

  H323TraceDumpPDU("H245", TRUE, strm, pdu, pdu, 0,
                   (controlChannel == NULL) ? H323TransportAddress("") : controlChannel->GetLocalAddress(),
                   (controlChannel == NULL) ? H323TransportAddress("") : controlChannel->GetRemoteAddress()
                  );

  if (!h245Tunneling) {
    if (controlChannel == NULL) {
      PTRACE(1, "H245\tWrite PDU fail: no control channel.");
      return FALSE;
    }

    if (controlChannel->IsOpen() && controlChannel->WritePDU(strm))
      return TRUE;

    PTRACE(1, "H245\tWrite PDU fail: " << controlChannel->GetErrorText(PChannel::LastWriteError));
    return FALSE;
  }

  // If have a pending signalling PDU, use it rather than separate write
  H323SignalPDU localTunnelPDU;
  H323SignalPDU * tunnelPDU;
  if (h245TunnelTxPDU != NULL)
    tunnelPDU = h245TunnelTxPDU;
  else {
    localTunnelPDU.BuildFacility(*this, TRUE);
    tunnelPDU = &localTunnelPDU;
  }

  tunnelPDU->m_h323_uu_pdu.IncludeOptionalField(H225_H323_UU_PDU::e_h245Control);
  PINDEX last = tunnelPDU->m_h323_uu_pdu.m_h245Control.GetSize();
  tunnelPDU->m_h323_uu_pdu.m_h245Control.SetSize(last+1);
  tunnelPDU->m_h323_uu_pdu.m_h245Control[last] = strm;

  if (h245TunnelTxPDU != NULL)
    return TRUE;

  return WriteSignalPDU(localTunnelPDU);
}


BOOL H323Connection::StartControlNegotiations(BOOL renegotiate)
{
  PTRACE(2, "H245\tStart control negotiations");

  if(renegotiate)  // makes reopening of media channels possible 
    connectionState = HasExecutedSignalConnect;

  // Begin the capability exchange procedure
  if (!capabilityExchangeProcedure->Start(renegotiate)) {
    PTRACE(1, "H245\tStart of Capability Exchange failed");
    return FALSE;
  }

  // Begin the Master/Slave determination procedure
  if (!masterSlaveDeterminationProcedure->Start(renegotiate)) {
    PTRACE(1, "H245\tStart of Master/Slave determination failed");
    return FALSE;
  }

  endSessionNeeded = TRUE;
  return TRUE;
}

BOOL H323Connection::OnStartHandleControlChannel()
{
  if (controlChannel != NULL) {
     PTRACE(2, "H245\tHandle control channel");
     return StartHandleControlChannel();
  } else 
     return StartControlNegotiations();
}

BOOL H323Connection::StartHandleControlChannel()
{
  // If have started separate H.245 channel then don't tunnel any more
  h245Tunneling = FALSE;

  // Start the TCS and MSD operations on new H.245 channel.
  if (!StartControlNegotiations())
    return FALSE;

  // Disable the signalling channels timeout for monitoring call status and
  // start up one in this thread instead. Then the Q.931 channel can be closed
  // without affecting the call.
  signallingChannel->SetReadTimeout(PMaxTimeInterval);
  controlChannel->SetReadTimeout(MonitorCallStatusTime);

  return TRUE;
}

void H323Connection::EndHandleControlChannel()
{
  // If we are the only link to the far end or if we have already sent our
  // endSession command then indicate that we have received endSession even 
  // if we hadn't, because we are now never going to get one so there is no 
  // point in having CleanUpOnCallEnd wait.
  if (signallingChannel == NULL || endSessionSent == TRUE)
    endSessionReceived.Signal();
}

void H323Connection::HandleControlChannel()
{
  if (!OnStartHandleControlChannel())
    return;

  BOOL ok = TRUE;
  while (ok) {
    MonitorCallStatus();
    PPER_Stream strm;
    BOOL readStatus = controlChannel->ReadPDU(strm);
    ok = HandleReceivedControlPDU(readStatus, strm);
  }

  EndHandleControlChannel();

  PTRACE(2, "H245\tControl channel closed.");
}


BOOL H323Connection::HandleReceivedControlPDU(BOOL readStatus, PPER_Stream & strm)
{
  BOOL ok = FALSE;

  if (readStatus) {
    // Lock while checking for shutting down.
    if (Lock()) {
      // Process the received PDU
      PTRACE(4, "H245\tReceived TPKT: " << strm);
      ok = HandleControlData(strm);
      Unlock(); // Unlock connection
    }
    else
      ok = InternalEndSessionCheck(strm);
  }
  else if (controlChannel->GetErrorCode() == PChannel::Timeout) {
    ok = TRUE;
  } 
  else {
      PTRACE(1, "H245\tRead error: " << controlChannel->GetErrorText(PChannel::LastReadError) 
          << " endSessionSent=" << endSessionSent);
    // If the connection is already shutting down then don't overwrite the
    // call end reason.  This could happen if the remote end point misbehaves
    // and simply closes the H.245 TCP connection rather than sending an 
    // endSession.
    if(endSessionSent == FALSE)
      ClearCall(EndedByTransportFail);
    else
      PTRACE(1, "H245\tendSession already sent assuming H245 connection closed by remote side");
    ok = FALSE;
  }

  return ok;
}


BOOL H323Connection::InternalEndSessionCheck(PPER_Stream & strm)
{
  H323ControlPDU pdu;

  if (!pdu.Decode(strm)) {
    PTRACE(1, "H245\tInvalid PDU decode:\n  " << setprecision(2) << pdu);
    return FALSE;
  }

  PTRACE(3, "H245\tChecking for end session on PDU: " << pdu.GetTagName()
         << ' ' << ((PASN_Choice &)pdu.GetObject()).GetTagName());

  if (pdu.GetTag() != H245_MultimediaSystemControlMessage::e_command)
    return TRUE;

  H245_CommandMessage & command = pdu;
  if (command.GetTag() == H245_CommandMessage::e_endSessionCommand)
    endSessionReceived.Signal();
  return FALSE;
}


BOOL H323Connection::HandleControlData(PPER_Stream & strm)
{
  while (!strm.IsAtEnd()) {
    H323ControlPDU pdu;
    if (!pdu.Decode(strm)) {
      PTRACE(1, "H245\tInvalid PDU decode!"
                "\nRaw PDU:\n" << hex << setfill('0')
                               << setprecision(2) << strm
                               << dec << setfill(' ') <<
                "\nPartial PDU:\n  " << setprecision(2) << pdu);
      return TRUE;
    }

    H323TraceDumpPDU("H245", FALSE, strm, pdu, pdu, 0,
                     (controlChannel == NULL) ? H323TransportAddress("") : controlChannel->GetLocalAddress(),
                     (controlChannel == NULL) ? H323TransportAddress("") : controlChannel->GetRemoteAddress()
                    );

    if (!HandleControlPDU(pdu))
      return FALSE;

    InternalEstablishedConnectionCheck();

    strm.ByteAlign();
  }

  return TRUE;
}


BOOL H323Connection::HandleControlPDU(const H323ControlPDU & pdu)
{
  switch (pdu.GetTag()) {
    case H245_MultimediaSystemControlMessage::e_request :
      return OnH245Request(pdu);

    case H245_MultimediaSystemControlMessage::e_response :
      return OnH245Response(pdu);

    case H245_MultimediaSystemControlMessage::e_command :
      return OnH245Command(pdu);

    case H245_MultimediaSystemControlMessage::e_indication :
      return OnH245Indication(pdu);
  }

  return OnUnknownControlPDU(pdu);
}


BOOL H323Connection::OnUnknownControlPDU(const H323ControlPDU & pdu)
{
  PTRACE(2, "H245\tUnknown Control PDU: " << pdu);

  H323ControlPDU reply;
  reply.BuildFunctionNotUnderstood(pdu);
  return WriteControlPDU(reply);
}


BOOL H323Connection::OnH245Request(const H323ControlPDU & pdu)
{
  const H245_RequestMessage & request = pdu;

  switch (request.GetTag()) {
    case H245_RequestMessage::e_masterSlaveDetermination :
      return masterSlaveDeterminationProcedure->HandleIncoming(request);

    case H245_RequestMessage::e_terminalCapabilitySet :
    {
      const H245_TerminalCapabilitySet & tcs = request;
      if (tcs.m_protocolIdentifier.GetSize() >= 6) {
        h245version = tcs.m_protocolIdentifier[5];
        h245versionSet = TRUE;
        PTRACE(3, "H245\tSet protocol version to " << h245version);
      }
      return capabilityExchangeProcedure->HandleIncoming(tcs);
    }

    case H245_RequestMessage::e_openLogicalChannel :
      return logicalChannels->HandleOpen(request);

    case H245_RequestMessage::e_closeLogicalChannel :
      return logicalChannels->HandleClose(request);

    case H245_RequestMessage::e_requestChannelClose :
      return logicalChannels->HandleRequestClose(request);

    case H245_RequestMessage::e_requestMode :
      return requestModeProcedure->HandleRequest(request);

    case H245_RequestMessage::e_roundTripDelayRequest :
      return roundTripDelayProcedure->HandleRequest(request);

    case H245_RequestMessage::e_conferenceRequest :
      if (OnHandleConferenceRequest(request))
        return TRUE;
      break;

	case H245_RequestMessage::e_genericRequest : 
      if (OnHandleH245GenericMessage(h245request,request))
	    return TRUE;
	  break;
  }

  return OnUnknownControlPDU(pdu);
}


BOOL H323Connection::OnH245Response(const H323ControlPDU & pdu)
{
  const H245_ResponseMessage & response = pdu;

  switch (response.GetTag()) {
    case H245_ResponseMessage::e_masterSlaveDeterminationAck :
      return masterSlaveDeterminationProcedure->HandleAck(response);

    case H245_ResponseMessage::e_masterSlaveDeterminationReject :
      return masterSlaveDeterminationProcedure->HandleReject(response);

    case H245_ResponseMessage::e_terminalCapabilitySetAck :
      return capabilityExchangeProcedure->HandleAck(response);

    case H245_ResponseMessage::e_terminalCapabilitySetReject :
      return capabilityExchangeProcedure->HandleReject(response);

    case H245_ResponseMessage::e_openLogicalChannelAck :
      return logicalChannels->HandleOpenAck(response);

    case H245_ResponseMessage::e_openLogicalChannelReject :
      return logicalChannels->HandleReject(response);

    case H245_ResponseMessage::e_closeLogicalChannelAck :
      return logicalChannels->HandleCloseAck(response);

    case H245_ResponseMessage::e_requestChannelCloseAck :
      return logicalChannels->HandleRequestCloseAck(response);

    case H245_ResponseMessage::e_requestChannelCloseReject :
      return logicalChannels->HandleRequestCloseReject(response);

    case H245_ResponseMessage::e_requestModeAck :
      return requestModeProcedure->HandleAck(response);

    case H245_ResponseMessage::e_requestModeReject :
      return requestModeProcedure->HandleReject(response);

    case H245_ResponseMessage::e_roundTripDelayResponse :
      return roundTripDelayProcedure->HandleResponse(response);

    case H245_ResponseMessage::e_conferenceResponse :
      if (OnHandleConferenceResponse(response))
        return TRUE;
      break;

	case H245_ResponseMessage::e_genericResponse :
      if (OnHandleH245GenericMessage(h245response,response))
	    return TRUE;
	  break;
  }

  return OnUnknownControlPDU(pdu);
}


BOOL H323Connection::OnH245Command(const H323ControlPDU & pdu)
{
  const H245_CommandMessage & command = pdu;

  switch (command.GetTag()) {
    case H245_CommandMessage::e_sendTerminalCapabilitySet :
      return OnH245_SendTerminalCapabilitySet(command);

    case H245_CommandMessage::e_flowControlCommand :
      return OnH245_FlowControlCommand(command);

    case H245_CommandMessage::e_miscellaneousCommand :
      return OnH245_MiscellaneousCommand(command);

    case H245_CommandMessage::e_endSessionCommand :
      endSessionNeeded = TRUE;
      endSessionReceived.Signal();
      switch (connectionState) {
        case EstablishedConnection :
          ClearCall(EndedByRemoteUser);
          break;
        case AwaitingLocalAnswer :
          ClearCall(EndedByCallerAbort);
          break;
        default :
          ClearCall(EndedByRefusal);
      }
      return FALSE;

    case H245_CommandMessage::e_conferenceCommand:
      if (OnHandleConferenceCommand(command))
        return TRUE;
      break;

	case H245_CommandMessage::e_genericCommand :
      if (OnHandleH245GenericMessage(h245command,command))
	    return TRUE;
	  break;
  }

  return OnUnknownControlPDU(pdu);
}


BOOL H323Connection::OnH245Indication(const H323ControlPDU & pdu)
{
  const H245_IndicationMessage & indication = pdu;

  switch (indication.GetTag()) {
    case H245_IndicationMessage::e_masterSlaveDeterminationRelease :
      return masterSlaveDeterminationProcedure->HandleRelease(indication);

    case H245_IndicationMessage::e_terminalCapabilitySetRelease :
      return capabilityExchangeProcedure->HandleRelease(indication);

    case H245_IndicationMessage::e_openLogicalChannelConfirm :
      return logicalChannels->HandleOpenConfirm(indication);

    case H245_IndicationMessage::e_requestChannelCloseRelease :
      return logicalChannels->HandleRequestCloseRelease(indication);

    case H245_IndicationMessage::e_requestModeRelease :
      return requestModeProcedure->HandleRelease(indication);

    case H245_IndicationMessage::e_miscellaneousIndication :
      return OnH245_MiscellaneousIndication(indication);

    case H245_IndicationMessage::e_jitterIndication :
      return OnH245_JitterIndication(indication);

    case H245_IndicationMessage::e_userInput :
      OnUserInputIndication(indication);
      break;

    case H245_IndicationMessage::e_conferenceIndication :
      if (OnHandleConferenceIndication(indication))
        return TRUE;
      break;

	case H245_IndicationMessage::e_genericIndication :
      if (OnHandleH245GenericMessage(h245indication,indication))
	    return TRUE;
	  break;
  }

  return TRUE; // Do NOT call OnUnknownControlPDU for indications
}


BOOL H323Connection::OnH245_SendTerminalCapabilitySet(
                 const H245_SendTerminalCapabilitySet & pdu)
{
  if (pdu.GetTag() == H245_SendTerminalCapabilitySet::e_genericRequest)
    return capabilityExchangeProcedure->Start(TRUE);

  PTRACE(2, "H245\tUnhandled SendTerminalCapabilitySet: " << pdu);
  return TRUE;
}


BOOL H323Connection::OnH245_FlowControlCommand(
                 const H245_FlowControlCommand & pdu)
{
  PTRACE(3, "H245\tFlowControlCommand: scope=" << pdu.m_scope.GetTagName());

  long restriction;
  if (pdu.m_restriction.GetTag() == H245_FlowControlCommand_restriction::e_maximumBitRate)
    restriction = (const PASN_Integer &)pdu.m_restriction;
  else
    restriction = -1; // H245_FlowControlCommand_restriction::e_noRestriction

  switch (pdu.m_scope.GetTag()) {
    case H245_FlowControlCommand_scope::e_wholeMultiplex :
      OnLogicalChannelFlowControl(NULL, restriction);
      break;

    case H245_FlowControlCommand_scope::e_logicalChannelNumber :
    {
      H323Channel * chan = logicalChannels->FindChannel((unsigned)(const H245_LogicalChannelNumber &)pdu.m_scope, FALSE);
      if (chan != NULL)
        OnLogicalChannelFlowControl(chan, restriction);
    }
  }

  return TRUE;
}


BOOL H323Connection::OnH245_MiscellaneousCommand(
                 const H245_MiscellaneousCommand & pdu)
{
  H323Channel * chan = logicalChannels->FindChannel((unsigned)pdu.m_logicalChannelNumber, FALSE);
  if (chan != NULL)
    chan->OnMiscellaneousCommand(pdu.m_type);
  else
    PTRACE(3, "H245\tMiscellaneousCommand: is ignored chan=" << pdu.m_logicalChannelNumber
           << ", type=" << pdu.m_type.GetTagName());

  return TRUE;
}


BOOL H323Connection::OnH245_MiscellaneousIndication(
                 const H245_MiscellaneousIndication & pdu)
{
  H323Channel * chan = logicalChannels->FindChannel((unsigned)pdu.m_logicalChannelNumber, TRUE);
  if (chan != NULL)
    chan->OnMiscellaneousIndication(pdu.m_type);
  else
    PTRACE(3, "H245\tMiscellaneousIndication is ignored. chan=" << pdu.m_logicalChannelNumber
           << ", type=" << pdu.m_type.GetTagName());

  return TRUE;
}


BOOL H323Connection::OnH245_JitterIndication(
                 const H245_JitterIndication & pdu)
{
  PTRACE(3, "H245\tJitterIndication: scope=" << pdu.m_scope.GetTagName());

  static const DWORD mantissas[8] = { 0, 1, 10, 100, 1000, 10000, 100000, 1000000 };
  static const DWORD exponents[8] = { 10, 25, 50, 75 };
  DWORD jitter = mantissas[pdu.m_estimatedReceivedJitterMantissa]*
                 exponents[pdu.m_estimatedReceivedJitterExponent]/10;

  int skippedFrameCount = -1;
  if (pdu.HasOptionalField(H245_JitterIndication::e_skippedFrameCount))
    skippedFrameCount = pdu.m_skippedFrameCount;

  int additionalBuffer = -1;
  if (pdu.HasOptionalField(H245_JitterIndication::e_additionalDecoderBuffer))
    additionalBuffer = pdu.m_additionalDecoderBuffer;

  switch (pdu.m_scope.GetTag()) {
    case H245_JitterIndication_scope::e_wholeMultiplex :
      OnLogicalChannelJitter(NULL, jitter, skippedFrameCount, additionalBuffer);
      break;

    case H245_JitterIndication_scope::e_logicalChannelNumber :
    {
      H323Channel * chan = logicalChannels->FindChannel((unsigned)(const H245_LogicalChannelNumber &)pdu.m_scope, FALSE);
      if (chan != NULL)
        OnLogicalChannelJitter(chan, jitter, skippedFrameCount, additionalBuffer);
    }
  }

  return TRUE;
}


H323Channel * H323Connection::GetLogicalChannel(unsigned number, BOOL fromRemote) const
{
  return logicalChannels->FindChannel(number, fromRemote);
}


H323Channel * H323Connection::FindChannel(unsigned rtpSessionId, BOOL fromRemote) const
{
  return logicalChannels->FindChannelBySession(rtpSessionId, fromRemote);
}

#ifdef H323_H450

void H323Connection::TransferCall(const PString & remoteParty,
                                  const PString & callIdentity)
{
  // According to H.450.4, if prior to consultation the primary call has been put on hold, the 
  // transferring endpoint shall first retrieve the call before Call Transfer is invoked.
  if (!callIdentity.IsEmpty() && IsLocalHold())
    RetrieveCall();
  h4502handler->TransferCall(remoteParty, callIdentity);
}

void H323Connection::OnReceivedInitiateReturnError()
{
	endpoint.OnReceivedInitiateReturnError();
}

void H323Connection::ConsultationTransfer(const PString & primaryCallToken)
{
  h4502handler->ConsultationTransfer(primaryCallToken);
}


void H323Connection::HandleConsultationTransfer(const PString & callIdentity,
                                                H323Connection& incoming)
{
  h4502handler->HandleConsultationTransfer(callIdentity, incoming);
}


BOOL H323Connection::IsTransferringCall() const
{
  switch (h4502handler->GetState()) {
    case H4502Handler::e_ctAwaitIdentifyResponse :
    case H4502Handler::e_ctAwaitInitiateResponse :
    case H4502Handler::e_ctAwaitSetupResponse :
      return TRUE;

    default :
      return FALSE;
  }
}


BOOL H323Connection::IsTransferredCall() const
{
   return (h4502handler->GetInvokeId() != 0 &&
           h4502handler->GetState() == H4502Handler::e_ctIdle) ||
           h4502handler->isConsultationTransferSuccess();
}


void H323Connection::HandleTransferCall(const PString & token,
                                        const PString & identity)
{
  if (!token.IsEmpty() || !identity)
    h4502handler->AwaitSetupResponse(token, identity);
}


int H323Connection::GetCallTransferInvokeId()
{
  return h4502handler->GetInvokeId();
}


void H323Connection::HandleCallTransferFailure(const int returnError)
{
  h4502handler->HandleCallTransferFailure(returnError);
}


void H323Connection::SetAssociatedCallToken(const PString& token)
{
  h4502handler->SetAssociatedCallToken(token);
}


void H323Connection::OnConsultationTransferSuccess(H323Connection& /*secondaryCall*/)
{
   h4502handler->SetConsultationTransferSuccess();
}

void H323Connection::SetCallLinkage(H225_AdmissionRequest& /*arq*/ )
{
}

void H323Connection::GetCallLinkage(const H225_AdmissionRequest& /*arq*/)
{
}

void H323Connection::HoldCall(BOOL localHold)
{
  h4504handler->HoldCall(localHold);
  holdAudioMediaChannel = SwapHoldMediaChannels(holdAudioMediaChannel,RTP_Session::DefaultAudioSessionID);
  holdVideoMediaChannel = SwapHoldMediaChannels(holdVideoMediaChannel,RTP_Session::DefaultVideoSessionID);
}

BOOL H323Connection::GetRedirectingNumber(
    PString &originalCalledNr,               
    PString &lastDivertingNr,
    int &divCounter, 
    int &originaldivReason,
	int &divReason)
{
  return h4503handler->GetRedirectingNumber(originalCalledNr,lastDivertingNr,
	                                     divCounter,originaldivReason,divReason);
}

void H323Connection::RetrieveCall()
{
  // Is the current call on hold?
  if (IsLocalHold()) {
    h4504handler->RetrieveCall();
    holdAudioMediaChannel = SwapHoldMediaChannels(holdAudioMediaChannel,RTP_Session::DefaultAudioSessionID);
    holdVideoMediaChannel = SwapHoldMediaChannels(holdVideoMediaChannel,RTP_Session::DefaultVideoSessionID);
  }
  else if (IsRemoteHold()) {
    PTRACE(4, "H4504\tRemote-end Call Hold not implemented.");
  }
  else {
    PTRACE(4, "H4504\tCall is not on Hold.");
  }
}


void H323Connection::SetHoldMedia(PChannel * audioChannel)
{
  holdAudioMediaChannel = PAssertNULL(audioChannel);
}

void H323Connection::SetVideoHoldMedia(PChannel * videoChannel)
{
  holdVideoMediaChannel = PAssertNULL(videoChannel);
}

BOOL H323Connection::IsMediaOnHold() const
{
  return holdAudioMediaChannel != NULL;
}


PChannel * H323Connection::SwapHoldMediaChannels(PChannel * newChannel,unsigned sessionId)
{
  if (IsMediaOnHold()) {
	  if (newChannel == NULL) {
	     PTRACE(4, "H4504\tCannot Retrieve session " << sessionId << " as hold media is NULL.");
         return NULL;
	  }
  }

  PChannel * existingTransmitChannel = NULL;

  PINDEX count = logicalChannels->GetSize();

  for (PINDEX i = 0; i < count; ++i) {
    H323Channel* channel = logicalChannels->GetChannelAt(i);

	if (!channel) {
		 PTRACE(4, "H4504\tLogical Channel " << i << " Empty or closed! Session ID: " << sessionId);
		// Fire off to ensure if channel is being Held that it is retrieved in derived application
	     OnCallRetrieve(TRUE,sessionId,0,newChannel);
         return NULL;
    }

	unsigned int session_id = channel->GetSessionID();
    if (session_id == sessionId) {
      const H323ChannelNumber & channelNumber = channel->GetNumber();

      H323_RTPChannel * chan2 = reinterpret_cast<H323_RTPChannel*>(channel);

	  H323Codec & codec = *channel->GetCodec();
	  PChannel * rawChannel = codec.GetRawDataChannel();
	  unsigned frameRate = codec.GetFrameRate()*2;

      if (!channelNumber.IsFromRemote()) { // Transmit channel
        if (IsMediaOnHold()) {
          if (IsCallOnHold()) {
             PTRACE(4, "H4504\tHold Media OnHold Transmit " << i);
          existingTransmitChannel = codec.SwapChannel(newChannel);
              existingTransmitChannel = OnCallHold(TRUE,session_id,frameRate,existingTransmitChannel);
          } else {
             PTRACE(4, "H4504\tRetrieve Media OnHold Transmit " << i);
	     existingTransmitChannel = codec.SwapChannel(OnCallRetrieve(TRUE,session_id,frameRate,existingTransmitChannel));
          }
        }
        else {
          // Enable/mute the transmit channel depending on whether the remote end is held
	   if (IsCallOnHold()) {
              PTRACE(4, "H4504\tHold Transmit " << i);
              chan2->SetPause(TRUE);
              if (codec.SetRawDataHeld(TRUE))
                codec.SwapChannel(OnCallHold(TRUE,session_id,frameRate,rawChannel));
           } else {
              PTRACE(4, "H4504\tRetreive Transmit " << i);
              codec.SwapChannel(OnCallRetrieve(TRUE,session_id,frameRate,rawChannel));
              if (codec.SetRawDataHeld(FALSE))
                chan2->SetPause(FALSE);
           }
        }
      }
      else {
        // Enable/mute the receive channel depending on whether the remote endis held
          if (IsCallOnHold()) {
            PTRACE(4, "H4504\tHold Receive " << i);
            chan2->SetPause(TRUE);
             if (codec.SetRawDataHeld(TRUE))
                 codec.SwapChannel(OnCallHold(FALSE,session_id,frameRate,rawChannel));
          } else {
             PTRACE(4, "H4504\tRetrieve Receive " << i);
             codec.SwapChannel(OnCallRetrieve(FALSE,session_id,frameRate,rawChannel));
             if (codec.SetRawDataHeld(FALSE))
                 chan2->SetPause(FALSE);
          }  
      }
    }
  }

  return existingTransmitChannel;
}

PChannel * H323Connection::OnCallHold(BOOL /*IsEncoder*/,				
                                  unsigned /*sessionId*/,		
                                  unsigned /*bufferSize*/,		
                                  PChannel * channel)
{
         return channel;
}

PChannel * H323Connection::OnCallRetrieve(BOOL /*IsEncoder*/, 
                                 unsigned /*sessionId*/, 
                                 unsigned bufferSize,   
                                 PChannel * channel)
{
	if (bufferSize == 0)
		return NULL;
	else
        return channel;
}

BOOL H323Connection::IsLocalHold() const
{
  return h4504handler->GetState() == H4504Handler::e_ch_NE_Held;
}


BOOL H323Connection::IsRemoteHold() const
{
  return h4504handler->GetState() == H4504Handler::e_ch_RE_Held;
}


BOOL H323Connection::IsCallOnHold() const
{
  return h4504handler->GetState() != H4504Handler::e_ch_Idle;
}


void H323Connection::IntrudeCall(unsigned capabilityLevel)
{
  h45011handler->IntrudeCall(capabilityLevel);
}


void H323Connection::HandleIntrudeCall(const PString & token,
                                       const PString & identity)
{
  if (!token.IsEmpty() || !identity)
    h45011handler->AwaitSetupResponse(token, identity);
}


BOOL H323Connection::GetRemoteCallIntrusionProtectionLevel(const PString & intrusionCallToken,
                                                           unsigned intrusionCICL)
{
  return h45011handler->GetRemoteCallIntrusionProtectionLevel(intrusionCallToken, intrusionCICL);
}


void H323Connection::SetIntrusionImpending()
{
  h45011handler->SetIntrusionImpending();
}


void H323Connection::SetForcedReleaseAccepted()
{
  h45011handler->SetForcedReleaseAccepted();
}


void H323Connection::SetIntrusionNotAuthorized()
{
  h45011handler->SetIntrusionNotAuthorized();
}


void H323Connection::SendCallWaitingIndication(const unsigned nbOfAddWaitingCalls)
{
  h4506handler->AttachToAlerting(*alertingPDU, nbOfAddWaitingCalls);
}

#endif // H323_H450


BOOL H323Connection::OnControlProtocolError(ControlProtocolErrors /*errorSource*/,
                                            const void * /*errorData*/)
{
  return TRUE;
}


static void SetRFC2833PayloadType(H323Capabilities & capabilities,
                                  OpalRFC2833 & rfc2833handler)
{
  H323Capability * capability = capabilities.FindCapability(H323_UserInputCapability::SubTypeNames[H323_UserInputCapability::SignalToneRFC2833]);
  if (capability != NULL) {
    RTP_DataFrame::PayloadTypes pt = ((H323_UserInputCapability*)capability)->GetPayloadType();
    if (rfc2833handler.GetPayloadType() != pt) {
      PTRACE(2, "H323\tUser Input RFC2833 payload type set to " << pt);
      rfc2833handler.SetPayloadType(pt);
    }
  }
}


void H323Connection::OnSendCapabilitySet(H245_TerminalCapabilitySet & /*pdu*/)
{
  // If we originated call, then check for RFC2833 capability and set payload type
  if (!callAnswered)
    SetRFC2833PayloadType(localCapabilities, *rfc2833handler);
}


BOOL H323Connection::OnReceivedCapabilitySet(const H323Capabilities & remoteCaps,
                                             const H245_MultiplexCapability * muxCap,
                                             H245_TerminalCapabilitySetReject & /*rejectPDU*/)
{
  if (muxCap != NULL) {
    if (muxCap->GetTag() != H245_MultiplexCapability::e_h2250Capability) {
      PTRACE(1, "H323\tCapabilitySet contains unsupported multiplex.");
      return FALSE;
    }

    const H245_H2250Capability & h225_0 = *muxCap;
    remoteMaxAudioDelayJitter = h225_0.m_maximumAudioDelayJitter;
  }

  // save this time as being when the reverse media channel was opened
  if (!reverseMediaOpenTime.IsValid())
    reverseMediaOpenTime = PTime();

  if (remoteCaps.GetSize() == 0) {
    // Received empty TCS, so close all transmit channels
    for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
      H245NegLogicalChannel & negChannel = logicalChannels->GetNegLogicalChannelAt(i);
      H323Channel * channel = negChannel.GetChannel();
      if (channel != NULL && !channel->GetNumber().IsFromRemote())
        negChannel.Close();
    }
    transmitterSidePaused = TRUE;
  }
  else { // Received non-empty TCS

    // If we had received a TCS=0 previously, or we have a remoteCapabilities which
    // was "faked" from the fast start data, overwrite it, don't merge it.
    if (transmitterSidePaused || !capabilityExchangeProcedure->HasReceivedCapabilities())
      remoteCapabilities.RemoveAll();

    if (!remoteCapabilities.Merge(remoteCaps))
      return FALSE;

    if (transmitterSidePaused) {
      transmitterSidePaused = FALSE;
      connectionState = HasExecutedSignalConnect;
      capabilityExchangeProcedure->Start(TRUE);
    }
    else {
      if (localCapabilities.GetSize() > 0)
        capabilityExchangeProcedure->Start(FALSE);

      // If we terminated call, then check for RFC2833 capability and set payload type
      if (callAnswered)
        SetRFC2833PayloadType(remoteCapabilities, *rfc2833handler);
    }
  }

  return TRUE;
}


void H323Connection::SendCapabilitySet(BOOL empty)
{
  capabilityExchangeProcedure->Start(TRUE, empty);
}


void H323Connection::OnSetLocalCapabilities()
{
}


BOOL H323Connection::IsH245Master() const
{
  return masterSlaveDeterminationProcedure->IsMaster();
}


void H323Connection::StartRoundTripDelay()
{
  if (Lock()) {
    if (masterSlaveDeterminationProcedure->IsDetermined() &&
        capabilityExchangeProcedure->HasSentCapabilities()) {
      if (roundTripDelayProcedure->IsRemoteOffline()) {
        PTRACE(2, "H245\tRemote failed to respond to PDU.");
        if (endpoint.ShouldClearCallOnRoundTripFail())
          ClearCall(EndedByTransportFail);
      }
      else
        roundTripDelayProcedure->StartRequest();
    }
    Unlock();
  }
}


PTimeInterval H323Connection::GetRoundTripDelay() const
{
  return roundTripDelayProcedure->GetRoundTripDelay();
}


void H323Connection::InternalEstablishedConnectionCheck()
{
  PTRACE(3, "H323\tInternalEstablishedConnectionCheck: "
            "connectionState=" << connectionState << " "
            "fastStartState=" << fastStartState);

  BOOL h245_available = masterSlaveDeterminationProcedure->IsDetermined() &&
                        capabilityExchangeProcedure->HasSentCapabilities() &&
                        capabilityExchangeProcedure->HasReceivedCapabilities();

  if (h245_available)
    endSessionNeeded = TRUE;

  // Check for if all the 245 conditions are met so can start up logical
  // channels and complete the connection establishment.
  if (fastStartState != FastStartAcknowledged) {
    if (!h245_available)
      return;

    // If we are early starting, start channels as soon as possible instead of
    // waiting for connect PDU
    if (earlyStart && FindChannel(RTP_Session::DefaultAudioSessionID, FALSE) == NULL)
      OnSelectLogicalChannels();
  }

#ifdef H323_T120
  if (h245_available && startT120) {
    if (remoteCapabilities.FindCapability("T.120") != NULL) {
      H323Capability * capability = localCapabilities.FindCapability("T.120");
      if (capability != NULL)
        OpenLogicalChannel(*capability, 3, H323Channel::IsBidirectional);
    }
    startT120 = FALSE;
  }
#endif

#ifdef H323_H224
  if (h245_available && startH224) {
    if(remoteCapabilities.FindCapability("H.224") != NULL) {
      H323Capability * capability = localCapabilities.FindCapability("H.224");
      if(capability != NULL) 
         OpenLogicalChannel(*capability,RTP_Session::DefaultH224SessionID, H323Channel::IsBidirectional);
    }	   
	startH224 = FALSE;
  }
#endif

  // Special case for Cisco CCM, when it does "early start" and opens its audio
  // channel to us, we better open one back or it hangs up!
  if ( h245_available &&
      !mediaWaitForConnect &&
       connectionState == AwaitingSignalConnect &&
       FindChannel(RTP_Session::DefaultAudioSessionID, TRUE) != NULL &&
       FindChannel(RTP_Session::DefaultAudioSessionID, FALSE) == NULL)
    OnSelectLogicalChannels();

  if (connectionState != HasExecutedSignalConnect)
    return;

  // Check if we have already got a transmitter running, select one if not
  if (FindChannel(RTP_Session::DefaultAudioSessionID, FALSE) == NULL)
    OnSelectLogicalChannels();

  connectionState = EstablishedConnection;
  OnEstablished();
}

#if defined(H323_AUDIO_CODECS) || defined(H323_VIDEO) || defined(H323_T38)

static void StartFastStartChannel(H323LogicalChannelList & fastStartChannels,
                                  unsigned sessionID, H323Channel::Directions direction)
{
  for (PINDEX i = 0; i < fastStartChannels.GetSize(); i++) {
    H323Channel & channel = fastStartChannels[i];
    if (channel.GetSessionID() == sessionID && channel.GetDirection() == direction) {
      fastStartChannels[i].Start();
      break;
    }
  }
}

#endif


void H323Connection::OnSelectLogicalChannels()
{
  PTRACE(2, "H245\tDefault OnSelectLogicalChannels, " << fastStartState);

  // Select the first codec that uses the "standard" audio session.
  switch (fastStartState) {
    default : //FastStartDisabled :
#ifdef H323_AUDIO_CODECS
      SelectDefaultLogicalChannel(RTP_Session::DefaultAudioSessionID);
#endif
#ifdef H323_VIDEO
      if (endpoint.CanAutoStartTransmitVideo())
        SelectDefaultLogicalChannel(RTP_Session::DefaultVideoSessionID);
#ifdef H323_H239
      if (endpoint.CanAutoStartTransmitExtVideo())
        SelectDefaultLogicalChannel(RTP_Session::DefaultExtVideoSessionID);
#endif
#endif // H323_VIDEO
#ifdef H323_T38
      if (endpoint.CanAutoStartTransmitFax())
        SelectDefaultLogicalChannel(RTP_Session::DefaultFaxSessionID);
#endif
      break;

    case FastStartInitiate :
#ifdef H323_AUDIO_CODECS
      SelectFastStartChannels(RTP_Session::DefaultAudioSessionID, TRUE, TRUE);
#endif
#ifdef H323_VIDEO
      SelectFastStartChannels(RTP_Session::DefaultVideoSessionID,
                              endpoint.CanAutoStartTransmitVideo(),
                              endpoint.CanAutoStartReceiveVideo());
#ifdef H323_H239
	  SelectFastStartChannels(RTP_Session::DefaultExtVideoSessionID,
                              endpoint.CanAutoStartTransmitExtVideo(),
                              endpoint.CanAutoStartReceiveExtVideo());
#endif
#endif // H323_VIDEO

#ifdef H323_T38
      SelectFastStartChannels(RTP_Session::DefaultFaxSessionID, 
		                      endpoint.CanAutoStartTransmitFax(),
                              endpoint.CanAutoStartReceiveFax());
#endif
      break;

    case FastStartResponse :
#ifdef H323_AUDIO_CODECS
      StartFastStartChannel(fastStartChannels, RTP_Session::DefaultAudioSessionID, H323Channel::IsTransmitter);
      StartFastStartChannel(fastStartChannels, RTP_Session::DefaultAudioSessionID, H323Channel::IsReceiver);
#endif
#ifdef H323_VIDEO
      if (endpoint.CanAutoStartTransmitVideo())
        StartFastStartChannel(fastStartChannels, RTP_Session::DefaultVideoSessionID, H323Channel::IsTransmitter);
      if (endpoint.CanAutoStartReceiveVideo())
        StartFastStartChannel(fastStartChannels, RTP_Session::DefaultVideoSessionID, H323Channel::IsReceiver);

#ifdef H323_H239
      if (endpoint.CanAutoStartTransmitExtVideo())
        StartFastStartChannel(fastStartChannels, RTP_Session::DefaultExtVideoSessionID, H323Channel::IsTransmitter);
      if (endpoint.CanAutoStartReceiveExtVideo())
        StartFastStartChannel(fastStartChannels, RTP_Session::DefaultExtVideoSessionID, H323Channel::IsReceiver);
#endif
#endif  // H323_VIDEO

#ifdef H323_T38
      if (endpoint.CanAutoStartTransmitFax())
        StartFastStartChannel(fastStartChannels, RTP_Session::DefaultFaxSessionID, H323Channel::IsTransmitter);
      if (endpoint.CanAutoStartReceiveFax())
        StartFastStartChannel(fastStartChannels, RTP_Session::DefaultFaxSessionID, H323Channel::IsReceiver);
#endif
      break;
  }
}


void H323Connection::SelectDefaultLogicalChannel(unsigned sessionID)
{
  if (FindChannel (sessionID, FALSE))
    return; 

  for (PINDEX i = 0; i < localCapabilities.GetSize(); i++) {
    H323Capability & localCapability = localCapabilities[i];
    if (localCapability.GetDefaultSessionID() == sessionID) {
      H323Capability * remoteCapability = remoteCapabilities.FindCapability(localCapability);
      if (remoteCapability != NULL) {
        PTRACE(3, "H323\tSelecting " << *remoteCapability);
        
		MergeCapabilities(sessionID, localCapability, remoteCapability);
        
        if (OpenLogicalChannel(*remoteCapability, sessionID, H323Channel::IsTransmitter))
          break;
        PTRACE(2, "H323\tOnSelectLogicalChannels, OpenLogicalChannel failed: "
               << *remoteCapability);
      }
    }
  }
}


BOOL H323Connection::MergeCapabilities(unsigned sessionID, const H323Capability & local, H323Capability * remote)
{

	// Only the Video and Extended Video Capabilities require merging
	if ((sessionID != RTP_Session::DefaultVideoSessionID) &&
		(sessionID != RTP_Session::DefaultExtVideoSessionID))
			return FALSE;
#if H323_VIDEO
   OpalVideoFormat & remoteFormat = (OpalVideoFormat &)(remote->GetWritableMediaFormat());
   const OpalMediaFormat & localFormat = local.GetMediaFormat();

   if (remoteFormat.Merge(localFormat)) {
#if PTRACING
	  PTRACE(6, "H323\t" << ((remote->GetMainType() != H323Capability::e_Video) ? "Ext " : "") << "Video Capability Merge: "); 
	  OpalMediaFormat::DebugOptionList(remoteFormat);
#endif
      return TRUE;
   }
#endif
   return FALSE;
}


void H323Connection::DisableFastStart()
{
	fastStartState = FastStartDisabled;
}


void H323Connection::SelectFastStartChannels(unsigned sessionID,
                                             BOOL transmitter,
                                             BOOL receiver)
{
  // Select all of the fast start channels to offer to the remote when initiating a call.
  for (PINDEX i = 0; i < localCapabilities.GetSize(); i++) {
    H323Capability & capability = localCapabilities[i];
    if (capability.GetDefaultSessionID() == sessionID) {
      if (receiver) {
        if (!OpenLogicalChannel(capability, sessionID, H323Channel::IsReceiver)) {
          PTRACE(2, "H323\tOnSelectLogicalChannels, OpenLogicalChannel rx failed: " << capability);
        }
      }
      if (transmitter) {
        if (!OpenLogicalChannel(capability, sessionID, H323Channel::IsTransmitter)) {
          PTRACE(2, "H323\tOnSelectLogicalChannels, OpenLogicalChannel tx failed: " << capability);
        }
      }
    }
  }
}


BOOL H323Connection::OpenLogicalChannel(const H323Capability & capability,
                                        unsigned sessionID,
                                        H323Channel::Directions dir)
{
  switch (fastStartState) {
    default : // FastStartDisabled
      if (dir == H323Channel::IsReceiver)
        return FALSE;

      // Traditional H245 handshake
      return logicalChannels->Open(capability, sessionID);

    case FastStartResponse :
      // Do not use OpenLogicalChannel for starting these.
      return FALSE;

    case FastStartInitiate :
      break;
  }

  /*If starting a receiver channel and are initiating the fast start call,
    indicated by the remoteCapabilities being empty, we do a "trial"
    listen on the channel. That is, for example, the UDP sockets are created
    to receive data in the RTP session, but no thread is started to read the
    packets and pass them to the codec. This is because at this point in time,
    we do not know which of the codecs is to be used, and more than one thread
    cannot read from the RTP ports at the same time.
  */
  H323Channel * channel = capability.CreateChannel(*this, dir, sessionID, NULL);
  if (channel == NULL)
    return FALSE;

  if (dir != H323Channel::IsReceiver)
    channel->SetNumber(logicalChannels->GetNextChannelNumber());

  fastStartChannels.Append(channel);
  return TRUE;
}


BOOL H323Connection::OnOpenLogicalChannel(const H245_OpenLogicalChannel & /*openPDU*/,
                                          H245_OpenLogicalChannelAck & /*ackPDU*/,
                                          unsigned & /*errorCode*/)
{
  // If get a OLC via H.245 stop trying to do fast start
  fastStartState = FastStartDisabled;
  if (!fastStartChannels.IsEmpty()) {
    fastStartChannels.RemoveAll();
    PTRACE(1, "H245\tReceived early start OLC, aborting fast start");
  }

  //errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
  return TRUE;
}


BOOL H323Connection::OnConflictingLogicalChannel(H323Channel & conflictingChannel)
{
  unsigned session = conflictingChannel.GetSessionID();
  PTRACE(2, "H323\tLogical channel " << conflictingChannel
         << " conflict on session " << session
         << ", codec: " << conflictingChannel.GetCapability());

  /* Matrix of conflicts:
       Local EP is master and conflicting channel from remote (OLC)
          Reject remote transmitter (function is not called)
       Local EP is master and conflicting channel to remote (OLCAck)
          Should not happen (function is not called)
       Local EP is slave and conflicting channel from remote (OLC)
          Close sessions reverse channel from remote
          Start new reverse channel using codec in conflicting channel
          Accept the OLC for masters transmitter
       Local EP is slave and conflicting channel to remote (OLCRej)
          Start transmitter channel using codec in sessions reverse channel

      Upshot is this is only called if a slave and require a restart of
      some channel. Possibly closing channels as master has precedence.
   */

  BOOL fromRemote = conflictingChannel.GetNumber().IsFromRemote();
  H323Channel * channel = FindChannel(session, !fromRemote);
  if (channel == NULL) {
    PTRACE(1, "H323\tCould not resolve conflict, no reverse channel.");
    return FALSE;
  }

  if (!fromRemote) {
    conflictingChannel.CleanUpOnTermination();
    H323Capability * capability = remoteCapabilities.FindCapability(channel->GetCapability());
    if (capability == NULL) {
      PTRACE(1, "H323\tCould not resolve conflict, capability not available on remote.");
      return FALSE;
    }
    OpenLogicalChannel(*capability, session, H323Channel::IsTransmitter);
    return TRUE;
  }

  // Shut down the conflicting channel that got in before our transmitter
  channel->CleanUpOnTermination();

  // Get the conflisting channel number to close
  H323ChannelNumber number = channel->GetNumber();

  // Must be slave and conflict from something we are sending, so try starting a
  // new channel using the master endpoints transmitter codec.
  logicalChannels->Open(conflictingChannel.GetCapability(), session, number);

  // Now close the conflicting channel
  CloseLogicalChannelNumber(number);
  return TRUE;
}


H323Channel * H323Connection::CreateLogicalChannel(const H245_OpenLogicalChannel & open,
                                                   BOOL startingFast,
                                                   unsigned & errorCode)
{
  const H245_H2250LogicalChannelParameters * param;
  const H245_DataType * dataType;
  H323Channel::Directions direction;

  if (startingFast && open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters)) {
    if (open.m_reverseLogicalChannelParameters.m_multiplexParameters.GetTag() !=
              H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters
                                                      ::e_h2250LogicalChannelParameters) {
      errorCode = H245_OpenLogicalChannelReject_cause::e_unsuitableReverseParameters;
      PTRACE(2, "H323\tCreateLogicalChannel - reverse channel, H225.0 only supported");
      return NULL;
    }

    PTRACE(3, "H323\tCreateLogicalChannel - reverse channel");
    dataType = &open.m_reverseLogicalChannelParameters.m_dataType;
    param = &(const H245_H2250LogicalChannelParameters &)
                      open.m_reverseLogicalChannelParameters.m_multiplexParameters;
    direction = H323Channel::IsTransmitter;
  }
  else {
    if (open.m_forwardLogicalChannelParameters.m_multiplexParameters.GetTag() !=
              H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters
                                                      ::e_h2250LogicalChannelParameters) {
      PTRACE(2, "H323\tCreateLogicalChannel - forward channel, H225.0 only supported");
      errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
      return NULL;
    }

    PTRACE(3, "H323\tCreateLogicalChannel - forward channel");
    dataType = &open.m_forwardLogicalChannelParameters.m_dataType;
    param = &(const H245_H2250LogicalChannelParameters &)
                      open.m_forwardLogicalChannelParameters.m_multiplexParameters;
    direction = H323Channel::IsReceiver;
  }

  unsigned sessionID = param->m_sessionID;

#ifdef H323_VIDEO
#ifdef H323_H239
  if (!startingFast &&
	  open.HasOptionalField(H245_OpenLogicalChannel::e_genericInformation)) {  // check for extended Video OLC

    unsigned roleLabel = 0;
	H323ChannelNumber channelnum = H323ChannelNumber(open.m_forwardLogicalChannelNumber, TRUE);

    const H245_ArrayOf_GenericInformation & cape = open.m_genericInformation;
	for (PINDEX i=0; i<cape.GetSize(); i++) {
       const H245_GenericMessage & gcap = cape[i];
       const PASN_ObjectId & object_id = gcap.m_messageIdentifier;
	   if (object_id.AsString() == OpalPluginCodec_Identifer_H239_Video) {
		   if (gcap.HasOptionalField(H245_GenericMessage::e_messageContent)) {
               const H245_ArrayOf_GenericParameter & params = gcap.m_messageContent;
               for (PINDEX j=0; j<params.GetSize(); j++) {
				   const H245_GenericParameter & content = params[j];
				   const H245_ParameterValue & paramval = content.m_parameterValue;
				   if (paramval.GetTag() == H245_ParameterValue::e_booleanArray) {
				       const PASN_Integer & val = paramval;
                       roleLabel = val;
				   }
			   }
		   }
	      OnReceivedExtendedVideoSession(roleLabel,channelnum);   
	   }
	}
  }
#endif
#endif // H323_VIDEO

  // See if datatype is supported

  H323Capability * capability = localCapabilities.FindCapability(*dataType);
  if (capability == NULL) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_unknownDataType;
    PTRACE(2, "H323\tCreateLogicalChannel - unknown data type");
    return NULL; // If codec not supported, return error
  }

  if (!capability->OnReceivedPDU(*dataType, direction == H323Channel::IsReceiver)) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotSupported;
    PTRACE(2, "H323\tCreateLogicalChannel - data type not supported");
    return NULL; // If codec not supported, return error
  }

  if (startingFast && (direction == H323Channel::IsTransmitter)) {
    H323Capability * remoteCapability = remoteCapabilities.FindCapability(*capability);
    if (remoteCapability != NULL)
      capability = remoteCapability;
    else {
      capability = remoteCapabilities.Copy(*capability);
      remoteCapabilities.SetCapability(0, 0, capability);
    }
  }

  if (!OnCreateLogicalChannel(*capability, direction, errorCode))
    return NULL; // If codec combination not supported, return error

  H323Channel * channel = capability->CreateChannel(*this, direction, sessionID, param);
  if (channel == NULL) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotAvailable;
    PTRACE(2, "H323\tCreateLogicalChannel - data type not available");
    return NULL;
  }

  if (!channel->SetInitialBandwidth())
    errorCode = H245_OpenLogicalChannelReject_cause::e_insufficientBandwidth;
  else if (channel->OnReceivedPDU(open, errorCode))
    return channel;

  PTRACE(2, "H323\tOnReceivedPDU gave error " << errorCode);
  delete channel;
  return NULL;
}


H323Channel * H323Connection::CreateRealTimeLogicalChannel(const H323Capability & capability,
                                                           H323Channel::Directions dir,
                                                           unsigned sessionID,
							   const H245_H2250LogicalChannelParameters * param,
                                                           RTP_QOS * rtpqos)
{
  RTP_Session * session = NULL;

  if (param != NULL)
    session = UseSession(param->m_sessionID, param->m_mediaControlChannel, dir, rtpqos);
  else {
    // Make a fake transmprt address from the connection so gets initialised with
    // the transport type (IP, IPX, multicast etc).
    H245_TransportAddress addr;
    GetControlChannel().SetUpTransportPDU(addr, H323Transport::UseLocalTSAP);
    session = UseSession(sessionID, addr, dir, rtpqos);
  }

  if (session == NULL)
    return NULL;

  return new H323_RTPChannel(*this, capability, dir, *session);
}


BOOL H323Connection::OnCreateLogicalChannel(const H323Capability & capability,
                                            H323Channel::Directions dir,
                                            unsigned & errorCode)
{
  if (connectionState == ShuttingDownConnection) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
    return FALSE;
  }

  // Default error if returns FALSE
  errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeALCombinationNotSupported;

  // Check if in set at all
  if (dir != H323Channel::IsReceiver) {
    if (!remoteCapabilities.IsAllowed(capability)) {
      PTRACE(2, "H323\tOnCreateLogicalChannel - transmit capability " << capability << " not allowed.");
      return FALSE;
    }
  }
  else {
    if (!localCapabilities.IsAllowed(capability)) {
      PTRACE(2, "H323\tOnCreateLogicalChannel - receive capability " << capability << " not allowed.");
      return FALSE;
    }
  }

  // Check all running channels, and if new one can't run with it return FALSE
  for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
    H323Channel * channel = logicalChannels->GetChannelAt(i);
    if (channel != NULL && channel->GetDirection() == dir) {
      if (dir != H323Channel::IsReceiver) {
        if (!remoteCapabilities.IsAllowed(capability, channel->GetCapability())) {
          PTRACE(2, "H323\tOnCreateLogicalChannel - transmit capability " << capability
                 << " and " << channel->GetCapability() << " incompatible.");
          return FALSE;
        }
      }
      else {
        if (!localCapabilities.IsAllowed(capability, channel->GetCapability())) {
          PTRACE(2, "H323\tOnCreateLogicalChannel - receive capability " << capability
                 << " and " << channel->GetCapability() << " incompatible.");
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}


BOOL H323Connection::OnStartLogicalChannel(H323Channel & channel)
{
//  PTRACE(1, "H323\tOnStartLogicalChannel call");

  if (channel.GetSessionID() == OpalMediaFormat::DefaultAudioSessionID &&
      PIsDescendant(&channel, H323_RTPChannel)) {
    H323_RTPChannel & rtp = (H323_RTPChannel &)channel;
    if (channel.GetNumber().IsFromRemote()) {
      rtp.AddFilter(rfc2833handler->GetReceiveHandler());

      if (detectInBandDTMF) {
        H323Codec * codec = channel.GetCodec();
        if (codec != NULL)
          codec->AddFilter(PCREATE_NOTIFIER(OnUserInputInBandDTMF));
      }
    }
    else
      rtp.AddFilter(rfc2833handler->GetTransmitHandler());
  }

  return endpoint.OnStartLogicalChannel(*this, channel);
}

#ifndef NO_H323_AUDIO_CODECS
BOOL H323Connection::OpenAudioChannel(BOOL isEncoding, unsigned bufferSize, H323AudioCodec & codec)
{
# ifdef H323_AEC
    if (endpoint.AECEnabled() && (aec == NULL))
    {
      PTRACE(2, "H323\tCreating AEC instance.");
      int rate = codec.GetMediaFormat().GetTimeUnits() * 1000;
      aec = new PAec(endpoint.AECAlgo());
//      aec = new PAec(128,0x3f);
//      aec = new PAec(rate,30);
    }
    codec.AttachAEC(aec);
# endif
  if(isEncoding) codec.EnableAGC(endpoint.agc);

  return endpoint.OpenAudioChannel(*this, isEncoding, bufferSize, codec);
}
#endif

#ifndef NO_H323_VIDEO
BOOL H323Connection::OpenVideoChannel(BOOL isEncoding, H323VideoCodec & codec)
{
  return endpoint.OpenVideoChannel(*this, isEncoding, codec);
}
#endif // NO_H323_VIDEO


void H323Connection::CloseLogicalChannel(unsigned number, BOOL fromRemote)
{
  if (connectionState != ShuttingDownConnection)
    logicalChannels->Close(number, fromRemote);
}


void H323Connection::CloseLogicalChannelNumber(const H323ChannelNumber & number)
{
  CloseLogicalChannel(number, number.IsFromRemote());
}


void H323Connection::CloseAllLogicalChannels(BOOL fromRemote)
{
  for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
    H245NegLogicalChannel & negChannel = logicalChannels->GetNegLogicalChannelAt(i);
    H323Channel * channel = negChannel.GetChannel();
    if (channel != NULL && channel->GetNumber().IsFromRemote() == fromRemote)
      negChannel.Close();
  }
}


BOOL H323Connection::OnClosingLogicalChannel(H323Channel & /*channel*/)
{
  return TRUE;
}


void H323Connection::OnClosedLogicalChannel(const H323Channel & channel)
{
  endpoint.OnClosedLogicalChannel(*this, channel);
}


void H323Connection::OnLogicalChannelFlowControl(H323Channel * channel,
                                                 long bitRateRestriction)
{
  if (channel != NULL)
    channel->OnFlowControl(bitRateRestriction);
}


void H323Connection::OnLogicalChannelJitter(H323Channel * channel,
                                            DWORD jitter,
                                            int skippedFrameCount,
                                            int additionalBuffer)
{
  if (channel != NULL)
    channel->OnJitterIndication(jitter, skippedFrameCount, additionalBuffer);
}


unsigned H323Connection::GetBandwidthUsed() const
{
  unsigned used = 0;

  for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
    H323Channel * channel = logicalChannels->GetChannelAt(i);
    if (channel != NULL)
      used += channel->GetBandwidthUsed();
  }

  PTRACE(3, "H323\tBandwidth used: " << used);

  return used;
}


BOOL H323Connection::UseBandwidth(unsigned bandwidth, BOOL removing)
{
  PTRACE(3, "H323\tBandwidth request: "
         << (removing ? '-' : '+')
         << bandwidth/10 << '.' << bandwidth%10
         << "kb/s, available: "
         << bandwidthAvailable/10 << '.' << bandwidthAvailable%10
         << "kb/s");

  if (removing)
    bandwidthAvailable += bandwidth;
  else {
    if (bandwidth > bandwidthAvailable) {
      PTRACE(2, "H323\tAvailable bandwidth exceeded");
      return FALSE;
    }

    bandwidthAvailable -= bandwidth;
  }

  return TRUE;
}


BOOL H323Connection::SetBandwidthAvailable(unsigned newBandwidth, BOOL force)
{
  unsigned used = GetBandwidthUsed();
  if (used > newBandwidth) {
    if (!force)
      return FALSE;

    // Go through logical channels and close down some.
    PINDEX chanIdx = logicalChannels->GetSize();
    while (used > newBandwidth && chanIdx-- > 0) {
      H323Channel * channel = logicalChannels->GetChannelAt(chanIdx);
      if (channel != NULL) {
        used -= channel->GetBandwidthUsed();
        CloseLogicalChannelNumber(channel->GetNumber());
      }
    }
  }

  bandwidthAvailable = newBandwidth - used;
  return TRUE;
}


void H323Connection::SetSendUserInputMode(SendUserInputModes mode)
{
  PAssert(mode != SendUserInputAsSeparateRFC2833, PUnimplementedFunction);

  PTRACE(2, "H323\tSetting default User Input send mode to " << mode);
  sendUserInputMode = mode;
}


static BOOL CheckSendUserInputMode(const H323Capabilities & caps,
                                   H323Connection::SendUserInputModes mode)
{
  // If have remote capabilities, then verify we can send selected mode,
  // otherwise just return and accept it for future validation
  static const H323_UserInputCapability::SubTypes types[H323Connection::NumSendUserInputModes] = {
    H323_UserInputCapability::NumSubTypes,
    H323_UserInputCapability::BasicString,
    H323_UserInputCapability::SignalToneH245,
    H323_UserInputCapability::SignalToneRFC2833
#ifdef H323_H249
//	H323_UserInputCapability::SignalToneSeperateRFC2833,  // Not implemented
   ,H323_UserInputCapability::H249A_Navigation,
	H323_UserInputCapability::H249B_Softkey,
	H323_UserInputCapability::H249C_PointDevice,
	H323_UserInputCapability::H249D_Modal,
    H323_UserInputCapability::NumSubTypes
#endif
  };

  if (types[mode] == H323_UserInputCapability::NumSubTypes)
    return mode == H323Connection::SendUserInputAsQ931;

  return caps.FindCapability(H323_UserInputCapability::SubTypeNames[types[mode]]) != NULL;
}


H323Connection::SendUserInputModes H323Connection::GetRealSendUserInputMode() const
{
  // If have not yet exchanged capabilities (ie not finished setting up the
  // H.245 channel) then the only thing we can do is Q.931
  if (!capabilityExchangeProcedure->HasReceivedCapabilities())
    return SendUserInputAsQ931;

  // First try recommended mode
  if (CheckSendUserInputMode(remoteCapabilities, sendUserInputMode))
    return sendUserInputMode;

  // Then try H.245 tones
  if (CheckSendUserInputMode(remoteCapabilities, SendUserInputAsTone))
    return SendUserInputAsTone;

  // Finally if is H.245 alphanumeric or does not indicate it could do other
  // modes we use H.245 alphanumeric as per spec.
  return SendUserInputAsString;
}


void H323Connection::SendUserInput(const PString & value)
{
  SendUserInputModes mode = GetRealSendUserInputMode();

  PTRACE(2, "H323\tSendUserInput(\"" << value << "\"), using mode " << mode);
  PINDEX i;

  switch (mode) {
    case SendUserInputAsQ931 :
      SendUserInputIndicationQ931(value);
      break;

    case SendUserInputAsString :
      SendUserInputIndicationString(value);
      break;

    case SendUserInputAsTone :
      for (i = 0; i < value.GetLength(); i++)
        SendUserInputIndicationTone(value[i]);
      break;

    case SendUserInputAsInlineRFC2833 :
      for (i = 0; i < value.GetLength(); i++)
        rfc2833handler->SendTone(value[i], 180);
      break;

    default :
      ;
  }
}


void H323Connection::OnUserInputString(const PString & value)
{
  endpoint.OnUserInputString(*this, value);
}


void H323Connection::SendUserInputTone(char tone,
                                       unsigned duration,
                                       unsigned logicalChannel,
                                       unsigned rtpTimestamp)
{
  SendUserInputModes mode = GetRealSendUserInputMode();

  PTRACE(2, "H323\tSendUserInputTone("
         << tone << ','
         << duration << ','
         << logicalChannel << ','
         << rtpTimestamp << "), using mode " << mode);

  switch (mode) {
    case SendUserInputAsQ931 :
      SendUserInputIndicationQ931(PString(tone));
      break;

    case SendUserInputAsString :
      SendUserInputIndicationString(PString(tone));
      break;

    case SendUserInputAsTone :
      SendUserInputIndicationTone(tone, duration, logicalChannel, rtpTimestamp);
      break;

    case SendUserInputAsInlineRFC2833 :
      rfc2833handler->SendTone(tone, duration);
      break;

    default :
      ;
  }
}


void H323Connection::OnUserInputTone(char tone,
                                     unsigned duration,
                                     unsigned logicalChannel,
                                     unsigned rtpTimestamp)
{
  endpoint.OnUserInputTone(*this, tone, duration, logicalChannel, rtpTimestamp);
}


void H323Connection::SendUserInputIndicationQ931(const PString & value)
{
  PTRACE(2, "H323\tSendUserInputIndicationQ931(\"" << value << "\")");

  H323SignalPDU pdu;
  pdu.BuildInformation(*this);
  pdu.GetQ931().SetKeypad(value);
  if (!WriteSignalPDU(pdu))
    ClearCall(EndedByTransportFail);
}


void H323Connection::SendUserInputIndicationString(const PString & value)
{
  PTRACE(2, "H323\tSendUserInputIndicationString(\"" << value << "\")");

  H323ControlPDU pdu;
  PASN_GeneralString & str = pdu.BuildUserInputIndication(value);
  if (!str.GetValue())
    WriteControlPDU(pdu);
  else {
    PTRACE(1, "H323\tInvalid characters for UserInputIndication");
  }
}


void H323Connection::SendUserInputIndicationTone(char tone,
                                                 unsigned duration,
                                                 unsigned logicalChannel,
                                                 unsigned rtpTimestamp)
{
  PTRACE(2, "H323\tSendUserInputIndicationTone("
         << tone << ','
         << duration << ','
         << logicalChannel << ','
         << rtpTimestamp << ')');

  H323ControlPDU pdu;
  pdu.BuildUserInputIndication(tone, duration, logicalChannel, rtpTimestamp);
  WriteControlPDU(pdu);
}

#ifdef H323_H249

void H323Connection::SendUserInputIndicationNavigate(H323_UserInputCapability::NavigateKeyID keyID)
{
 if (!CheckSendUserInputMode(remoteCapabilities,SendUserInputAsNavigation))
	 return;

  PTRACE(2, "H323\tSendUserInputIndicationNavigate(" << keyID << ')');

  H323ControlPDU pdu;
  H245_UserInputIndication & ind = pdu.Build(H245_IndicationMessage::e_userInput);
  ind.SetTag(H245_UserInputIndication::e_genericInformation);
  H245_ArrayOf_GenericInformation & infolist = ind;

  H245_GenericInformation * info =
             H323_UserInputCapability::BuildGenericIndication(H323_UserInputCapability::SubTypeOID[0]);

   info->IncludeOptionalField(H245_GenericMessage::e_messageContent);
   H245_ArrayOf_GenericParameter & contents = info->m_messageContent;

   H245_GenericParameter * content = 
		H323_UserInputCapability::BuildGenericParameter(1,H245_ParameterValue::e_unsignedMin,keyID);

   contents.Append(content);
   contents.SetSize(contents.GetSize()+1);

  infolist.Append(info);
  infolist.SetSize(infolist.GetSize()+1);
  WriteControlPDU(pdu);
}

void H323Connection::SendUserInputIndicationSoftkey(unsigned key, const PString & keyName)
{
 if (!CheckSendUserInputMode(remoteCapabilities,SendUserInputAsSoftkey))
	 return;

  PTRACE(2, "H323\tSendUserInputIndicationSoftkey(" << key << ')');

  H323ControlPDU pdu;
  H245_UserInputIndication & ind = pdu.Build(H245_IndicationMessage::e_userInput);
  ind.SetTag(H245_UserInputIndication::e_genericInformation);
  H245_ArrayOf_GenericInformation & infolist = ind;

  H245_GenericInformation * info =
             H323_UserInputCapability::BuildGenericIndication(H323_UserInputCapability::SubTypeOID[1]);

   info->IncludeOptionalField(H245_GenericMessage::e_messageContent);
   H245_ArrayOf_GenericParameter & contents = info->m_messageContent;

   H245_GenericParameter * content = 
		H323_UserInputCapability::BuildGenericParameter(2,H245_ParameterValue::e_unsignedMin,key);
    contents.Append(content);
    contents.SetSize(contents.GetSize()+1);

	if (keyName.GetLength() > 0) {
      H245_GenericParameter * contentstr = 
		 H323_UserInputCapability::BuildGenericParameter(1,H245_ParameterValue::e_octetString,keyName);
      contents.Append(contentstr);
      contents.SetSize(contents.GetSize()+1);
	}

  infolist.Append(info);
  infolist.SetSize(infolist.GetSize()+1);
  WriteControlPDU(pdu);
}

void H323Connection::SendUserInputIndicationPointDevice(unsigned x, unsigned y, unsigned button, 
														   unsigned buttonstate, unsigned clickcount)
{
 if (!CheckSendUserInputMode(remoteCapabilities,SendUserInputAsPointDevice))
	 return;

  PTRACE(6, "H323\tSendUserInputIndicationPointDevice");

  H323ControlPDU pdu;
  H245_UserInputIndication & ind = pdu.Build(H245_IndicationMessage::e_userInput);
  ind.SetTag(H245_UserInputIndication::e_genericInformation);
  H245_ArrayOf_GenericInformation & infolist = ind;

  H245_GenericInformation * info =
             H323_UserInputCapability::BuildGenericIndication(H323_UserInputCapability::SubTypeOID[2]);

   info->IncludeOptionalField(H245_GenericMessage::e_messageContent);
   H245_ArrayOf_GenericParameter & contents = info->m_messageContent;

/// Add X and Y co-ords
    H245_GenericParameter * X = 		 
		H323_UserInputCapability::BuildGenericParameter(1,H245_ParameterValue::e_unsignedMin,x);
    contents.Append(X);
    contents.SetSize(contents.GetSize()+1);

    H245_GenericParameter * Y = 		 
		H323_UserInputCapability::BuildGenericParameter(2,H245_ParameterValue::e_unsignedMin,y);
	contents.Append(Y);
    contents.SetSize(contents.GetSize()+1);

/// Optional values
	if (button > 0) {
      H245_GenericParameter * but = 		 
		H323_UserInputCapability::BuildGenericParameter(3,H245_ParameterValue::e_unsignedMin,button);
      contents.Append(but);
      contents.SetSize(contents.GetSize()+1);
	}

	if (buttonstate > 0) {
      H245_GenericParameter * butstate = 		 
		H323_UserInputCapability::BuildGenericParameter(4,H245_ParameterValue::e_unsignedMin,buttonstate);
      contents.Append(butstate);
      contents.SetSize(contents.GetSize()+1);
	}

	if (clickcount > 0) {
      H245_GenericParameter * cc = 		 
		H323_UserInputCapability::BuildGenericParameter(5,H245_ParameterValue::e_unsignedMin,clickcount);
      contents.Append(cc);
      contents.SetSize(contents.GetSize()+1);
	}

  infolist.Append(info);
  infolist.SetSize(infolist.GetSize()+1);
  WriteControlPDU(pdu);
}

void H323Connection::SendUserInputIndicationModal()
{
 if (!CheckSendUserInputMode(remoteCapabilities,SendUserInputAsModal))
	 return;

}
#endif

void H323Connection::SendUserInputIndication(const H245_UserInputIndication & indication)
{
  H323ControlPDU pdu;
  H245_UserInputIndication & ind = pdu.Build(H245_IndicationMessage::e_userInput);
  ind = indication;
  WriteControlPDU(pdu);
}


void H323Connection::OnUserInputIndication(const H245_UserInputIndication & ind)
{
  switch (ind.GetTag()) {
    case H245_UserInputIndication::e_alphanumeric :
      OnUserInputString((const PASN_GeneralString &)ind);
      break;

    case H245_UserInputIndication::e_signal :
    {
      const H245_UserInputIndication_signal & sig = ind;
      OnUserInputTone(sig.m_signalType[0],
                      sig.HasOptionalField(H245_UserInputIndication_signal::e_duration)
                                ? (unsigned)sig.m_duration : 0,
                      sig.m_rtp.m_logicalChannelNumber,
                      sig.m_rtp.m_timestamp);
      break;
    }
    case H245_UserInputIndication::e_signalUpdate :
    {
      const H245_UserInputIndication_signalUpdate & sig = ind;
      OnUserInputTone(' ', sig.m_duration, sig.m_rtp.m_logicalChannelNumber, 0);
      break;
    }
#ifdef H323_H249
	case H245_UserInputIndication::e_genericInformation :  
	{
	  const H245_ArrayOf_GenericInformation & sig = ind;
	  if ((sig.GetSize() > 0) &&
         sig[0].HasOptionalField(H245_GenericMessage::e_subMessageIdentifier)) {
		   const H245_CapabilityIdentifier & id = sig[0].m_messageIdentifier;
		   if (id.GetTag() == H245_CapabilityIdentifier::e_standard) {
               const PASN_ObjectId & gid = id;
			   PString sid = gid.AsString();
			   if (sid == H323_UserInputCapability::SubTypeOID[0]) {          // Navigation
				    OnUserInputIndicationNavigate(sig[0].m_messageContent);
			   } else if (sid == H323_UserInputCapability::SubTypeOID[1]) {   // Softkey
                    OnUserInputIndicationSoftkey(sig[0].m_messageContent);
			   } else if (sid == H323_UserInputCapability::SubTypeOID[2]) {   // PointingDevice
                    OnUserInputIndicationPointDevice(sig[0].m_messageContent);
			   } else if (sid == H323_UserInputCapability::SubTypeOID[3]) {   // Mode interface
				    OnUserInputIndicationModal(sig[0].m_messageContent);
			   }
		   }
	  }
	}
#endif
  }
}


void H323Connection::OnUserInputInlineRFC2833(OpalRFC2833Info & info, INT)
{
  if (!info.IsToneStart())
    OnUserInputTone(info.GetTone(), info.GetDuration(), 0, info.GetTimestamp());
}


void H323Connection::OnUserInputInBandDTMF(H323Codec::FilterInfo & info, INT)
{
  // This function is set up as an 'audio filter'.
  // This allows us to access the 16 bit PCM audio (at 8Khz sample rate)
  // before the audio is passed on to the sound card (or other output device)

#ifdef P_DTMF
  // Pass the 16 bit PCM audio through the DTMF decoder   
  PString tones = dtmfDecoder.Decode((short *)info.buffer, info.bufferLength/sizeof(short));
  if (!tones.IsEmpty()) {
    PTRACE(1, "DTMF detected. " << tones);
    PINDEX i;
    for (i = 0; i < tones.GetLength(); i++) {
      OnUserInputTone(tones[i], 0, 0, 0);
    }
  }
#endif
}

#ifdef H323_H249
void H323Connection::OnUserInputIndicationNavigate(const H245_ArrayOf_GenericParameter & contents)
{
}

void H323Connection::OnUserInputIndicationSoftkey(const H245_ArrayOf_GenericParameter & contents)
{
}

void H323Connection::OnUserInputIndicationPointDevice(const H245_ArrayOf_GenericParameter & contents)
{
}

void H323Connection::OnUserInputIndicationModal(const H245_ArrayOf_GenericParameter & contents)
{
}
#endif

RTP_Session * H323Connection::GetSession(unsigned sessionID) const
{
  return rtpSessions.GetSession(sessionID);
}


H323_RTP_Session * H323Connection::GetSessionCallbacks(unsigned sessionID) const
{
  RTP_Session * session = rtpSessions.GetSession(sessionID);
  if (session == NULL)
    return NULL;

  PTRACE(3, "RTP\tFound existing session " << sessionID);
  PObject * data = session->GetUserData();
  PAssert(PIsDescendant(data, H323_RTP_Session), PInvalidCast);
  return (H323_RTP_Session *)data;
}


RTP_Session * H323Connection::UseSession(unsigned sessionID,
                                         const H245_TransportAddress & taddr,
					                               H323Channel::Directions dir,
                                         RTP_QOS * rtpqos)
{
  // We only support unicast IP at this time.
  if (taddr.GetTag() != H245_TransportAddress::e_unicastAddress) {
    return NULL;
  }

  const H245_UnicastAddress & uaddr = taddr;
  if (uaddr.GetTag() != H245_UnicastAddress::e_iPAddress
#if P_HAS_IPV6
        && uaddr.GetTag() != H245_UnicastAddress::e_iP6Address
#endif
     ) {
    return NULL;
  }

  RTP_Session * session = rtpSessions.UseSession(sessionID);
  if (session != NULL) {
    ((RTP_UDP *) session)->Reopen(dir == H323Channel::IsReceiver);
    return session;
  }

  RTP_UDP * udp_session = new RTP_UDP(
#ifdef H323_RTP_AGGREGATE
                  useRTPAggregation ? endpoint.GetRTPAggregator() : NULL, 
#endif
                  sessionID, remoteIsNAT);

  udp_session->SetUserData(new H323_RTP_UDP(*this, *udp_session, rtpqos));
  rtpSessions.AddSession(udp_session);
  return udp_session;
}

BOOL H323Connection::OnReceiveRTPAltInformation(H323_RTP_UDP & rtp, 
	                    const H245_ArrayOf_GenericInformation & alternate) const
{
	return FALSE;
}

BOOL H323Connection::OnSendingRTPAltInformation(const H323_RTP_UDP & rtp,
				H245_ArrayOf_GenericInformation & alternate) const
{
  return FALSE; 
}

BOOL H323Connection::OnSendingOLCGenericInformation(const H323_RTP_UDP & rtp,
				H245_ArrayOf_GenericInformation & generic) const
{
  return FALSE; 
}

void H323Connection::ReleaseSession(unsigned sessionID)
{
  rtpSessions.ReleaseSession(sessionID);
}


void H323Connection::OnRTPStatistics(const RTP_Session & session) const
{
  endpoint.OnRTPStatistics(*this, session);
}

void H323Connection::OnRTPFinalStatistics(const RTP_Session & session) const
{
  endpoint.OnRTPFinalStatistics(*this, session);
}

static void AddSessionCodecName(PStringStream & name, H323Channel * channel)
{
  if (channel == NULL)
    return;

  H323Codec * codec = channel->GetCodec();
  if (codec == NULL)
    return;

  OpalMediaFormat mediaFormat = codec->GetMediaFormat();
  if (mediaFormat.IsEmpty())
    return;

  if (name.IsEmpty())
    name << mediaFormat;
  else if (name != mediaFormat)
    name << " / " << mediaFormat;
}


PString H323Connection::GetSessionCodecNames(unsigned sessionID) const
{
  PStringStream name;

  AddSessionCodecName(name, FindChannel(sessionID, FALSE));
  AddSessionCodecName(name, FindChannel(sessionID, TRUE));

  return name;
}


BOOL H323Connection::RequestModeChange(const PString & newModes)
{
  return requestModeProcedure->StartRequest(newModes);
}


BOOL H323Connection::RequestModeChange(const H245_ArrayOf_ModeDescription & newModes)
{
  return requestModeProcedure->StartRequest(newModes);
}


BOOL H323Connection::OnRequestModeChange(const H245_RequestMode & pdu,
                                         H245_RequestModeAck & /*ack*/,
                                         H245_RequestModeReject & /*reject*/,
                                         PINDEX & selectedMode)
{
  for (selectedMode = 0; selectedMode < pdu.m_requestedModes.GetSize(); selectedMode++) {
    BOOL ok = TRUE;
    for (PINDEX i = 0; i < pdu.m_requestedModes[selectedMode].GetSize(); i++) {
      if (localCapabilities.FindCapability(pdu.m_requestedModes[selectedMode][i]) == NULL) {
        ok = FALSE;
        break;
      }
    }
    if (ok)
      return TRUE;
  }

  PTRACE(1, "H245\tMode change rejected as does not have capabilities");
  return FALSE;
}


void H323Connection::OnModeChanged(const H245_ModeDescription & newMode)
{
  CloseAllLogicalChannels(FALSE);

  // Start up the new ones
  for (PINDEX i = 0; i < newMode.GetSize(); i++) {
    H323Capability * capability = localCapabilities.FindCapability(newMode[i]);
    if (PAssertNULL(capability) != NULL)  {// Should not occur as OnRequestModeChange checks them
      if (!OpenLogicalChannel(*capability,
                              capability->GetDefaultSessionID(),
                              H323Channel::IsTransmitter)) {
        PTRACE(1, "H245\tCould not open channel after mode change: " << *capability);
      }
    }
  }
}


void H323Connection::OnAcceptModeChange(const H245_RequestModeAck & pdu)
{
#if H323_T38
  if (t38ModeChangeCapabilities.IsEmpty())
    return;

  PTRACE(2, "H323\tT.38 mode change accepted.");

  // Now we have conviced the other side to send us T.38 data we should do the
  // same assuming the RequestModeChangeT38() function provided a list of \n
  // separaete capability names to start. Only one will be.

  CloseAllLogicalChannels(FALSE);

  PStringArray modes = t38ModeChangeCapabilities.Lines();

  PINDEX first, last;
  if (pdu.m_response.GetTag() == H245_RequestModeAck_response::e_willTransmitMostPreferredMode) {
    first = 0;
    last = 1;
  }
  else {
    first = 1;
    last = modes.GetSize();
  }

  for (PINDEX i = first; i < last; i++) {
    H323Capability * capability = localCapabilities.FindCapability(modes[i]);
    if (capability != NULL && OpenLogicalChannel(*capability,
                                                 capability->GetDefaultSessionID(),
                                                 H323Channel::IsTransmitter)) {
      PTRACE(1, "H245\tOpened " << *capability << " after T.38 mode change");
      break;
    }

    PTRACE(1, "H245\tCould not open channel after T.38 mode change");
  }

  t38ModeChangeCapabilities = PString::Empty();
#endif
}


void H323Connection::OnRefusedModeChange(const H245_RequestModeReject * /*pdu*/)
{
#ifdef H323_T38
  if (!t38ModeChangeCapabilities) {
    PTRACE(2, "H323\tT.38 mode change rejected.");
    t38ModeChangeCapabilities = PString::Empty();
  }
#endif
}

void H323Connection::OnSendARQ(H225_AdmissionRequest & arq)
{
#ifdef H323_H460
    H225_FeatureSet fs;
    if (OnSendFeatureSet(H460_MessageType::e_admissionRequest, fs)) {
		  if (fs.HasOptionalField(H225_FeatureSet::e_supportedFeatures)) {
			arq.IncludeOptionalField(H225_AdmissionRequest::e_genericData);

			H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
		    H225_ArrayOf_GenericData & data = arq.m_genericData;

			for (PINDEX i=0; i < fsn.GetSize(); i++) {
				 PINDEX lastPos = data.GetSize();
				 data.SetSize(lastPos+1);
				 data[lastPos] = fsn[i];
			}
		  }
	 }
#endif
  endpoint.OnSendARQ(*this, arq);
}

void H323Connection::OnReceivedACF(const H225_AdmissionConfirm & acf)
{
#ifdef H323_H460
	if (acf.HasOptionalField(H225_AdmissionConfirm::e_genericData)) {
		const H225_ArrayOf_GenericData & data = acf.m_genericData;

	  if (data.GetSize() > 0) {
         H225_FeatureSet fs;
		 fs.IncludeOptionalField(H225_FeatureSet::e_supportedFeatures);
		 H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
		 fsn.SetSize(data.GetSize());
			for (PINDEX i=0; i < data.GetSize(); i++) 
				 fsn[i] = (H225_FeatureDescriptor &)data[i];

		 OnReceiveFeatureSet(H460_MessageType::e_admissionConfirm, fs);
	  }
	}
#endif
  endpoint.OnReceivedACF(*this, acf);
}

void H323Connection::OnReceivedARJ(const H225_AdmissionReject & arj)
{
#ifdef H323_H460
	if (arj.HasOptionalField(H225_AdmissionReject::e_genericData)) {
		const H225_ArrayOf_GenericData & data = arj.m_genericData;

	  if (data.GetSize() > 0) {
         H225_FeatureSet fs;
		 fs.IncludeOptionalField(H225_FeatureSet::e_supportedFeatures);
		 H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
		 fsn.SetSize(data.GetSize());
			for (PINDEX i=0; i < data.GetSize(); i++) 
				 fsn[i] = (H225_FeatureDescriptor &)data[i];

		 OnReceiveFeatureSet(H460_MessageType::e_admissionReject, fs);
	  }
	}
#endif
  endpoint.OnReceivedARJ(*this, arj);
}

void H323Connection::OnSendIRR(H225_InfoRequestResponse & irr) const
{
#ifdef H323_H460
    H225_FeatureSet fs;
	if (OnSendFeatureSet(H460_MessageType::e_inforequestresponse, fs)) {
		if (fs.HasOptionalField(H225_FeatureSet::e_supportedFeatures)) {
			irr.IncludeOptionalField(H225_InfoRequestResponse::e_genericData);

			H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
		    H225_ArrayOf_GenericData & data = irr.m_genericData;

			for (PINDEX i=0; i < fsn.GetSize(); i++) {
				 PINDEX lastPos = data.GetSize();
				 data.SetSize(lastPos+1);
				 data[lastPos] = fsn[i];
			}
		}
	 }
#endif
}

void H323Connection::OnSendDRQ(H225_DisengageRequest & drq) const
{
#ifdef H323_H460
    H225_FeatureSet fs;
	if (OnSendFeatureSet(H460_MessageType::e_disengagerequest, fs)) {
		  if (fs.HasOptionalField(H225_FeatureSet::e_supportedFeatures)) {
			drq.IncludeOptionalField(H225_DisengageRequest::e_genericData);

			H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
		    H225_ArrayOf_GenericData & data = drq.m_genericData;

			for (PINDEX i=0; i < fsn.GetSize(); i++) {
				 PINDEX lastPos = data.GetSize();
				 data.SetSize(lastPos+1);
				 data[lastPos] = fsn[i];
			}
		  }
	 }
#endif
}


#ifdef H323_T120

OpalT120Protocol * H323Connection::CreateT120ProtocolHandler()
{
  if (t120handler == NULL)
    t120handler = endpoint.CreateT120ProtocolHandler(*this);
  return t120handler;
}

#endif

#ifdef H323_T38

OpalT38Protocol * H323Connection::CreateT38ProtocolHandler()
{
  if (t38handler == NULL)
    t38handler = endpoint.CreateT38ProtocolHandler(*this);
  return t38handler;
}


BOOL H323Connection::RequestModeChangeT38(const char * capabilityNames)
{
  t38ModeChangeCapabilities = capabilityNames;
  if (RequestModeChange(t38ModeChangeCapabilities))
    return TRUE;

  t38ModeChangeCapabilities = PString::Empty();
  return FALSE;
}

#endif // H323_T38

#ifdef H323_H224
OpalH224Handler * H323Connection::CreateH224ProtocolHandler(unsigned sessionID)
{
  if (h224handler == NULL)
    h224handler = endpoint.CreateH224ProtocolHandler(*this, sessionID);
	
  return h224handler;
}

OpalH281Handler * H323Connection::CreateH281ProtocolHandler(OpalH224Handler & h224Handler)
{
  return endpoint.CreateH281ProtocolHandler(h224Handler);
}
#endif

#ifdef H323_FILE
BOOL H323Connection::OpenFileTransferSession(H323ChannelNumber & num)
{
  BOOL filetransferOpen = FALSE;

  for (PINDEX i = 0; i < localCapabilities.GetSize(); i++) {
    H323Capability & localCapability = localCapabilities[i];
	if ((localCapability.GetMainType() == H323Capability::e_Data) && 
		(localCapability.GetSubType() == H245_DataApplicationCapability_application::e_genericDataCapability)) {
      H323FileTransferCapability * remoteCapability = (H323FileTransferCapability *)remoteCapabilities.FindCapability(localCapability);
      if (remoteCapability != NULL) {
        PTRACE(3, "H323\tFile Transfer Available " << *remoteCapability);      
		if (logicalChannels->Open(*remoteCapability, OpalMediaFormat::DefaultDataSessionID,num)) {
		   filetransferOpen = TRUE;
           break;
		}
        PTRACE(2, "H323\tFileTranfer OpenLogicalChannel failed: " << *remoteCapability);
      }
	  break;
    }
  }

  return filetransferOpen;
}

H323FileTransferHandler * H323Connection::CreateFileTransferHandler(unsigned sessionID,	
																	H323Channel::Directions dir,
						                                            H323FileTransferList & filelist)
{
  if ((filehandler == NULL) && (OpenFileTransferChannel(dir, filelist)))
     return new H323FileTransferHandler(*this, sessionID, dir, filelist);
  else
     return NULL;
}

BOOL H323Connection::OpenFileTransferChannel( H323Channel::Directions dir,
						                      H323FileTransferList & filelist
											 ) 
{
   return endpoint.OpenFileTransferChannel(*this,dir,filelist);
}


#endif


BOOL H323Connection::GetAdmissionRequestAuthentication(const H225_AdmissionRequest & /*arq*/,
                                                       H235Authenticators & /*authenticators*/)
{
  return FALSE;
}


const H323Transport & H323Connection::GetControlChannel() const
{
  return *(controlChannel != NULL ? controlChannel : signallingChannel);
}


void H323Connection::SetAudioJitterDelay(unsigned minDelay, unsigned maxDelay)
{
  PAssert(minDelay <= 1000 && maxDelay <= 1000, PInvalidParameter);

  if (minDelay < 10)
    minDelay = 10;
  minAudioJitterDelay = minDelay;

  if (maxDelay < minDelay)
    maxDelay = minDelay;
  maxAudioJitterDelay = maxDelay;
}


void H323Connection::SendLogicalChannelMiscCommand(H323Channel & channel,
                                                   unsigned commandIdentifier)
{
  if (channel.GetDirection() == H323Channel::IsReceiver) {
    H323ControlPDU pdu;
    H245_CommandMessage & command = pdu.Build(H245_CommandMessage::e_miscellaneousCommand);
    H245_MiscellaneousCommand & miscCommand = command;
    miscCommand.m_logicalChannelNumber = (unsigned)channel.GetNumber();
    miscCommand.m_type.SetTag(commandIdentifier);
    WriteControlPDU(pdu);
  }
}


void H323Connection::SetEnforcedDurationLimit(unsigned seconds)
{
  enforcedDurationLimit.SetInterval(0, seconds);
}


void H323Connection::MonitorCallStatus()
{
  if (!Lock())
    return;

  if (endpoint.GetRoundTripDelayRate() > 0 && !roundTripDelayTimer.IsRunning()) {
    roundTripDelayTimer = endpoint.GetRoundTripDelayRate();
    StartRoundTripDelay();
  }

  if (endpoint.GetNoMediaTimeout() > 0) {
    BOOL oneRunning = FALSE;
    BOOL allSilent = TRUE;
    for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
      H323Channel * channel = logicalChannels->GetChannelAt(i);
      if (channel != NULL && PIsDescendant(channel, H323_RTPChannel)) {
        if (channel->IsRunning()) {
          oneRunning = TRUE;
          if (((H323_RTPChannel *)channel)->GetSilenceDuration() < endpoint.GetNoMediaTimeout()) {
            allSilent = FALSE;
            break;
          }
        }
      }
    }
    if (oneRunning && allSilent)
      ClearCall(EndedByTransportFail);
  }

  if (enforcedDurationLimit.GetResetTime() > 0 && enforcedDurationLimit == 0)
    ClearCall(EndedByDurationLimit);

  Unlock();
}

#ifdef H323_H460
void H323Connection::DisableFeatures()
{
	 disableH460 = TRUE;
}
#endif

BOOL H323Connection::OnSendFeatureSet(unsigned code, H225_FeatureSet & feats) const
{
#ifdef H323_H460
   if (disableH460)
	   return FALSE;

   return features->SendFeature(code, feats);
#else
   return endpoint.OnSendFeatureSet(code, feats);
#endif
}

void H323Connection::OnReceiveFeatureSet(unsigned code, const H225_FeatureSet & feats) const
{
#ifdef H323_H460
   if (disableH460)
	   return;

   features->ReceiveFeature(code, feats);
#else
   endpoint.OnReceiveFeatureSet(code, feats);
#endif
}

#ifndef DISABLE_CALLAUTH
void H323Connection::SetAuthenticationConnection()
{
     for (PINDEX i = 0; i < EPAuthenticators.GetSize(); i++) {
        EPAuthenticators[i].SetConnection(this);
     }
}

const H235Authenticators & H323Connection::GetEPAuthenticators() const
{
      return EPAuthenticators;
}

BOOL H323Connection::OnCallAuthentication(const PString & username, 
                                         PString & password)
{
    return endpoint.OnCallAuthentication(username,password);
}

BOOL H323Connection::OnEPAuthenticationFailed(H235Authenticator::ValidationResult result) const
{
	return FALSE;
}

void H323Connection::OnAuthenticationFinalise(unsigned pdu,PBYTEArray & rawData)
{
     for (PINDEX i = 0; i < EPAuthenticators.GetSize(); i++) {
       if (EPAuthenticators[i].IsSecuredSignalPDU(pdu,FALSE))
           EPAuthenticators[i].Finalise(rawData);
       }
}
#endif

#ifdef H323_SIGNAL_AGGREGATE

void H323Connection::AggregateSignalChannel(H323Transport * transport)
{
  signalAggregator = new AggregatedH225Handle(*transport, *this);
  endpoint.GetSignallingAggregator()->AddHandle(signalAggregator);
}

void H323Connection::AggregateControlChannel(H323Transport * transport)
{
  if (!OnStartHandleControlChannel())
    return;

  controlAggregator = new AggregatedH245Handle(*transport, *this);
  endpoint.GetSignallingAggregator()->AddHandle(controlAggregator);
}

H323AggregatedH2x5Handle::H323AggregatedH2x5Handle(H323Transport & _transport, H323Connection & _connection)
  : PAggregatedHandle(TRUE), 
    fd(((PIPSocket *)_transport.GetBaseReadChannel())->GetHandle()),
    transport(_transport),
    connection(_connection)
{
  pduDataLen = 0;
}

H323AggregatedH2x5Handle::~H323AggregatedH2x5Handle()
{
}

PAggregatorFDList_t H323AggregatedH2x5Handle::GetFDs()
{
  PAggregatorFDList_t list;
  list.push_back(&fd);
  return list;
}

BOOL H323AggregatedH2x5Handle::OnRead()
{
  //
  // pduDataLen  : size of PDU data read so far, always less than pduBufferLen , always same as pduBuffer.GetSize()
  //

  // if the transport is not open, then there is no need process it further
  if (!transport.IsOpen())
    return FALSE;

  // make sure have a minimum headroom in the PDU buffer
  PINDEX pduSize = pduBuffer.GetSize();
  if ((pduSize - pduDataLen) < 100) {
    pduSize += 1000;
    pduBuffer.SetSize(pduSize);
  }

  // save the timeout
  PTimeInterval oldTimeout = transport.GetReadTimeout();
  transport.SetReadTimeout(0);

  // read PDU until no more data is available
  PINDEX numRead = 0;
  BOOL ok = TRUE;
  {
    char * ptr = (char *)(pduBuffer.GetPointer() + pduDataLen);
    int lenLeftInBuffer = pduSize - pduDataLen;
    while (ok && (lenLeftInBuffer > numRead) && transport.Read(ptr + numRead, lenLeftInBuffer - numRead)) {
      ok = transport.GetLastReadCount() > 0;
      if (ok)
        numRead += transport.GetLastReadCount();
    }
    if (pduBuffer[0] != 0x03) {
      PTRACE(1, "Error");
      ok = FALSE;
    }
  }

  // reset timeout
  transport.SetReadTimeout(oldTimeout);

  // if data was actually received, then process it
  if (ok) {

    // if PDU length was zero, must be timeout
    if (numRead == 0) {
      PBYTEArray dataPDU;
      ok = HandlePDU(FALSE, dataPDU);
    }

    else {

      // update pdu size for new data that was read
      pduDataLen += numRead;

      while (pduDataLen > 0) {

        // convert data to PDU. If PDU is invalid, return error
        PINDEX pduLen = pduDataLen;
        if (!transport.ExtractPDU(pduBuffer, pduLen)) {
          ok = FALSE;
          break;
        }

        // if PDU is not yet complete, then no error but stop looping
        else if (pduLen <= 0) {
          ok = TRUE;
          break;
        }

        // otherwise process the data
        else {
          transport.SetErrorValues(PChannel::NoError, 0, PChannel::LastReadError);
          {
            // create the new PDU
            PBYTEArray dataPDU((const BYTE *)pduBuffer+4, pduLen-4, FALSE);
            ok = HandlePDU(ok, dataPDU);
          }

          // remove processed data
          if (pduLen == pduDataLen)
            pduDataLen = 0;
          else {
            pduDataLen -= pduLen;
            memcpy(pduBuffer.GetPointer(), pduBuffer.GetPointer() + pduLen, pduDataLen);
          }
        }
      }
    }
  }


  return ok;
}

#endif

#ifdef H323_H248

BOOL H323Connection::OnSendServiceControlSessions(
				   H225_ArrayOf_ServiceControlSession & serviceControl, 
				   H225_ServiceControlSession_reason reason) const
{

	PString amount;
	BOOL credit=TRUE;
	unsigned time;
	PString url;

    if (!OnSendServiceControl(amount, credit,time, url) && 
						(serviceControlSessions.GetSize() == 0))
		return FALSE;

	PDictionary<POrdinalKey, H323ServiceControlSession> SCS = serviceControlSessions;
   
    if (!amount) {
        H323CallCreditServiceControl * csc = 
			     new H323CallCreditServiceControl(amount,credit,time);
        SCS.SetAt(H323ServiceControlSession::e_CallCredit, csc);
	}

	if (!url) {
		H323HTTPServiceControl * scs = new H323HTTPServiceControl(url);
        SCS.SetAt(H323ServiceControlSession::e_URL, scs);
	}

    for (PINDEX j = 0; j < SCS.GetSize(); j++) {

	  PINDEX last = serviceControl.GetSize();
	  serviceControl.SetSize(last+1);
	  H225_ServiceControlSession & pdu = serviceControl[last];

	  unsigned type = ((H323ServiceControlSession *)SCS.GetAt(j))->GetType();
	  pdu.m_sessionId = type;
	  pdu.m_reason = reason;

	  if (SCS[type].OnSendingPDU(pdu.m_contents))
		pdu.IncludeOptionalField(H225_ServiceControlSession::e_contents);

	}

   return TRUE;
}

void H323Connection::OnReceiveServiceControlSessions(const H225_ArrayOf_ServiceControlSession & serviceControl)
{

  BOOL isContent=FALSE;

  for (PINDEX i = 0; i < serviceControl.GetSize(); i++) {
    H225_ServiceControlSession & pdu = serviceControl[i];

    H323ServiceControlSession * session = NULL;
    unsigned sessionId = pdu.m_sessionId;

    if (serviceControlSessions.Contains(sessionId)) {
      session = &serviceControlSessions[sessionId];
      if (pdu.HasOptionalField(H225_ServiceControlSession::e_contents)) {
          if (session->OnReceivedPDU(pdu.m_contents))
              isContent = TRUE;
	  }
    }

    if (session == NULL && pdu.HasOptionalField(H225_ServiceControlSession::e_contents)) {
      session = endpoint.CreateServiceControlSession(pdu.m_contents);
      serviceControlSessions.SetAt(sessionId, session);
    }
  }

  if (isContent) {
	PString amount;
	BOOL credit=TRUE;
	unsigned time;
	PString url;
	PString ldapURL;
	PString baseDN;

    for (PINDEX j = 0; j < serviceControlSessions.GetSize(); j++) {
	  H323ServiceControlSession & sess = serviceControlSessions[j];
	  switch (sess.GetType()) {
	     case H323ServiceControlSession::e_CallCredit:
	        ((H323CallCreditServiceControl &)sess).GetValue(amount,credit,time);
                 break;
         case H323ServiceControlSession::e_URL:
            ((H323HTTPServiceControl &)sess).GetValue(url);
                break;
#ifdef H323_H350
		 case H323ServiceControlSession::e_NonStandard:
			((H323H350ServiceControl &)sess).GetValue(ldapURL,baseDN);
			    break;
#endif
		 default:
			    break;
	  } 
	}
    OnReceiveServiceControl(amount,credit,time,url,ldapURL,baseDN); 
  }								 
}

void H323Connection::OnReceiveServiceControl(const PString & amount, 
                                             BOOL credit, 
                                             const unsigned & timelimit,
                                             const PString & url,
											 const PString & ldapURL,
											 const PString & baseDN
                                            )
{
	if (!amount)
	    endpoint.OnCallCreditServiceControl(amount,credit,timelimit);

	if (!url)
		endpoint.OnHTTPServiceControl(0, 0, url);

#ifdef H323_H350
	if (!ldapURL)
	    endpoint.OnH350ServiceControl(ldapURL,baseDN);
#endif
}

BOOL H323Connection::OnSendServiceControl(PString & /*amount*/,
                                          BOOL /*credit*/,
                                          unsigned & /*timelimit*/,
                                          PString & /*url*/
                                         ) const
{
	return FALSE;
}

#endif

void H323Connection::DisableH245inSETUP()
{ 
	doH245inSETUP = FALSE; 
}

void H323Connection::DisableH245QoS()
{ 
    doH245QoS = FALSE;
}

BOOL H323Connection::H245QoSEnabled() const
{ 
    return doH245QoS;
}

void H323Connection::SetNonCallConnection()
{  
	IsNonCallConnection = TRUE; 
}

#ifdef H323_H460
H460_FeatureSet * H323Connection::GetFeatureSet()
{
	return features;
}
#endif

#ifndef NO_H323_VIDEO
#ifdef H323_H239
BOOL H323Connection::OpenExtendedVideoSession(H323ChannelNumber & num)
{
  BOOL applicationOpen = FALSE;

  for (PINDEX i = 0; i < localCapabilities.GetSize(); i++) {
    H323Capability & localCapability = localCapabilities[i];
	if ((localCapability.GetMainType() == H323Capability::e_Video) && 
		(localCapability.GetSubType() == H245_VideoCapability::e_extendedVideoCapability)) {
      H323ExtendedVideoCapability * remoteCapability = (H323ExtendedVideoCapability *)remoteCapabilities.FindCapability(localCapability);
      if (remoteCapability != NULL) {
        PTRACE(3, "H323\tApplication Available " << *remoteCapability);
         
		for (PINDEX j = 0; j < remoteCapability->GetSize(); j++) {
		  if (logicalChannels->Open(remoteCapability[j], OpalMediaFormat::DefaultExtVideoSessionID,num)) {
		     applicationOpen = TRUE;
             break;
		  }
		}
        PTRACE(2, "H323\tApplication OpenLogicalChannel failed: "
               << *remoteCapability);
      }
	  break;
    }
  }

  return applicationOpen;
}

BOOL H323Connection::CloseExtendedVideoSession(const H323ChannelNumber & num)
{
    CloseLogicalChannel(num,num.IsFromRemote());
	return TRUE;
}

BOOL H323Connection::OpenExtendedVideoChannel(BOOL isEncoding,H323VideoCodec & codec)
{
   return endpoint.OpenExtendedVideoChannel(*this, isEncoding, codec);
}
#endif  // H323_H239
#endif  // NO_H323_VIDEO

#ifdef H323_H230
BOOL H323Connection::OpenConferenceControlSession(BOOL & chairControl, BOOL & extControls)
{
  chairControl = FALSE;
  extControls = FALSE;
  for (PINDEX i = 0; i < localCapabilities.GetSize(); i++) {
    H323Capability & localCapability = localCapabilities[i];
	if (localCapability.GetMainType() == H323Capability::e_ConferenceControl) {
      H323_ConferenceControlCapability * remoteCapability = (H323_ConferenceControlCapability *)remoteCapabilities.FindCapability(localCapability);
      if (remoteCapability != NULL) {
          chairControl = remoteCapability->SupportChairControls();
          extControls = remoteCapability->SupportExtControls();
          PTRACE(3, "H323\tConference Controls Available for " << GetCallToken() << " Chair " << chairControl << " T124 " << extControls);
          return TRUE;
      }
    }
  }
  PTRACE(6, "H323\tConference Controls not available for " << GetCallToken());
  return FALSE;
}

#endif  // H323_H230

/////////////////////////////////////////////////////////////////////////////

