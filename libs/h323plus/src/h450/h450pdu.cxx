/*
 * h450pdu.cxx
 *
 * H.450 Helper functions
 *
 * Open H323 Library
 *
 * Copyright (c) 2001 Norwood Systems Pty. Ltd.
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
 * $Log: h450pdu.cxx,v $
 * Revision 1.1  2007/08/06 20:51:39  shorne
 * First commit of h323plus
 *
 * Revision 1.1.2.1  2006/12/24 05:32:55  shorne
 * File restructuring
 *
 * Revision 1.30  2006/06/27 12:35:03  csoutheren
 * Patch 1366328 - Support for H.450.3 divertingLegInformation2
 * Thanks to Norbert Bartalsky
 *
 * Revision 1.29  2006/05/16 11:38:27  shorne
 * Added DNS SRV support
 *
 * Revision 1.28  2006/01/26 03:43:06  shorne
 * Fix typo
 *
 * Revision 1.27  2005/07/12 12:28:57  csoutheren
 * Fixes for H.450 errors and return values
 * Thanks to Iker Perez San Roman
 *
 * Revision 1.26  2005/06/27 00:36:45  csoutheren
 * Added missing return after error
 * Thanks to Iker Pérez de San Román
 *
 * Revision 1.25  2005/03/10 07:01:30  csoutheren
 * Fixed problem with H.450 call identifiers not being unique across all calls on an
 *  endpoint. Thanks to Thien Nguyen
 *
 * Revision 1.24  2004/07/03 06:51:37  rjongbloed
 * Added PTRACE_PARAM() macro to fix warnings on parameters used in PTRACE
 *  macros only.
 *
 * Revision 1.23  2004/04/25 01:52:47  rjongbloed
 * Fixed GCC 3.4 warnings
 *
 * Revision 1.22  2004/02/17 09:53:54  csoutheren
 * Removed incorrect assignment of m_silentMonitoringPermitted to FALSE
 * See SourceForge bug 832371
 * Thanks to Vyacheslav E. Andrejev
 *
 * Revision 1.21  2004/02/07 03:35:56  rjongbloed
 * Fixed missing unlocks in some H.240 operations, in particular consultation transfer never being cleaned up. Thanks Sébastien Annedouche
 *
 * Revision 1.20  2003/07/15 11:22:07  csoutheren
 * Improved handling of alias addresses thanks to Federico Pinna
 *
 * Revision 1.19  2002/11/21 22:37:47  robertj
 * Fixed problems with unrecognized invoke APDU, thanks Andrea Bondavalli
 *
 * Revision 1.18  2002/10/01 06:47:39  robertj
 * Fixed GNU compiler warning
 *
 * Revision 1.17  2002/09/25 05:21:11  robertj
 * Fixed warning on no trace version.
 *
 * Revision 1.16  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.15  2002/07/04 00:40:34  robertj
 * More H.450.11 call intrusion implementation, thanks Aleksandar Todorovic
 *
 * Revision 1.14  2002/06/25 09:56:07  robertj
 * Fixed GNU warnings
 *
 * Revision 1.13  2002/06/22 05:48:42  robertj
 * Added partial implementation for H.450.11 Call Intrusion
 *
 * Revision 1.12  2002/06/13 06:13:28  robertj
 * Added trace dumps for outgoing H.450 supplementary service APDU's.
 *
 * Revision 1.11  2002/02/04 07:17:56  robertj
 * Added H.450.2 Consultation Transfer, thanks Norwood Systems.
 *
 * Revision 1.10  2002/01/14 00:03:01  robertj
 * Added H.450.6
 * Added extra "failure mode" parts of H.250.2.
 * Various other bug fixes.
 *   Thanks Ben Madsen of Norwood Systems
 *
 * Revision 1.9  2001/11/19 07:40:44  robertj
 * Fixed problem with error detection & state change in Call Transfer, thanks Graeme Reid
 *
 * Revision 1.8  2001/08/27 03:59:16  robertj
 * Fixed GNU warnings.
 *
 * Revision 1.7  2001/08/16 07:49:19  robertj
 * Changed the H.450 support to be more extensible. Protocol handlers
 *   are now in separate classes instead of all in H323Connection.
 *
 * Revision 1.6  2001/08/06 03:08:57  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.5  2001/06/14 06:25:16  robertj
 * Added further H.225 PDU build functions.
 * Moved some functionality from connection to PDU class.
 *
 * Revision 1.4  2001/06/05 03:14:41  robertj
 * Upgraded H.225 ASN to v4 and H.245 ASN to v7.
 *
 * Revision 1.3  2001/05/09 04:59:04  robertj
 * Bug fixes in H.450.2, thanks Klein Stefan.
 *
 * Revision 1.2  2001/04/20 02:16:53  robertj
 * Removed GNU C++ warnings.
 *
 * Revision 1.1  2001/04/11 03:01:29  robertj
 * Added H.450.2 (call transfer), thanks a LOT to Graeme Reid & Norwood Systems
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h450pdu.h"
#endif

#include "h450/h450pdu.h"

#include "h450/h4501.h"
#include "h450/h4502.h"
#include "h450/h4503.h"
#include "h450/h4504.h"
#include "h450/h4506.h"
#include "h450/h45010.h"
#include "h450/h45011.h"
#include "h323pdu.h"
#include "h323ep.h"
#include "h323con.h"


X880_Invoke& H450ServiceAPDU::BuildInvoke(int invokeId, int operation)
{
  SetTag(X880_ROS::e_invoke);
  X880_Invoke& invoke = (X880_Invoke&) *this;

  invoke.m_invokeId = invokeId;

  invoke.m_opcode.SetTag(X880_Code::e_local);
  PASN_Integer& opcode = (PASN_Integer&) invoke.m_opcode;
  opcode.SetValue(operation);

  return invoke;
}


X880_ReturnResult& H450ServiceAPDU::BuildReturnResult(int invokeId)
{
  SetTag(X880_ROS::e_returnResult);
  X880_ReturnResult& returnResult = (X880_ReturnResult&) *this;

  returnResult.m_invokeId = invokeId;

  return returnResult;
}


X880_ReturnError& H450ServiceAPDU::BuildReturnError(int invokeId, int error)
{
  SetTag(X880_ROS::e_returnError);
  X880_ReturnError& returnError = (X880_ReturnError&) *this;

  returnError.m_invokeId = invokeId;

  returnError.m_errorCode.SetTag(X880_Code::e_local);
  PASN_Integer& errorCode = (PASN_Integer&) returnError.m_errorCode;
  errorCode.SetValue(error);

  return returnError;
}


X880_Reject& H450ServiceAPDU::BuildReject(int invokeId)
{
  SetTag(X880_ROS::e_reject);
  X880_Reject& reject = (X880_Reject&) *this;

  reject.m_invokeId = invokeId;

  return reject;
}


void H450ServiceAPDU::BuildCallTransferInitiate(int invokeId,
                                                const PString & callIdentity,
                                                const PString & alias,
                                                const H323TransportAddress & address)
{
  X880_Invoke& invoke = BuildInvoke(invokeId, H4502_CallTransferOperation::e_callTransferInitiate);

  H4502_CTInitiateArg argument;

  argument.m_callIdentity = callIdentity;

  H4501_ArrayOf_AliasAddress& aliasAddress = argument.m_reroutingNumber.m_destinationAddress;

  // We have to have at least a destination transport address or alias.
  if (!alias.IsEmpty() && !address.IsEmpty()) {
    aliasAddress.SetSize(2);

    // Set the alias
    aliasAddress[1].SetTag(H225_AliasAddress::e_dialedDigits);
    H323SetAliasAddress(alias, aliasAddress[1]);

    // Set the transport
    aliasAddress[0].SetTag(H225_AliasAddress::e_transportID);
    H225_TransportAddress& cPartyTransport = (H225_TransportAddress&) aliasAddress[0];
    address.SetPDU(cPartyTransport);
  }
  else {
    aliasAddress.SetSize(1);
    if (alias.IsEmpty()) {
      // Set the transport, no alias present
      aliasAddress[0].SetTag(H225_AliasAddress::e_transportID);
      H225_TransportAddress& cPartyTransport = (H225_TransportAddress&) aliasAddress[0];
      address.SetPDU(cPartyTransport);
    }
    else {
      // Set the alias, no transport
      aliasAddress[0].SetTag(H225_AliasAddress::e_dialedDigits);
      H323SetAliasAddress(alias, aliasAddress[0]);
    }
  }

  PTRACE(4, "H4502\tSending supplementary service PDU argument:\n  "
         << setprecision(2) << argument);

  invoke.IncludeOptionalField(X880_Invoke::e_argument);
  invoke.m_argument.EncodeSubType(argument);
}


void H450ServiceAPDU::BuildCallTransferSetup(int invokeId,
                                             const PString & callIdentity)
{
  X880_Invoke& invoke = BuildInvoke(invokeId, H4502_CallTransferOperation::e_callTransferSetup);

  H4502_CTSetupArg argument;

  argument.m_callIdentity = callIdentity;

  PTRACE(4, "H4502\tSending supplementary service PDU argument:\n  "
         << setprecision(2) << argument);

  invoke.IncludeOptionalField(X880_Invoke::e_argument);
  invoke.m_argument.EncodeSubType(argument);
}


void H450ServiceAPDU::BuildCallTransferIdentify(int invokeId)
{
  X880_Invoke invoke = BuildInvoke(invokeId, H4502_CallTransferOperation::e_callTransferIdentify);
}


void H450ServiceAPDU::BuildCallTransferAbandon(int invokeId)
{
  X880_Invoke invoke = BuildInvoke(invokeId, H4502_CallTransferOperation::e_callTransferAbandon);
}


void H450ServiceAPDU::BuildCallWaiting(int invokeId, int numCallsWaiting)
{
  X880_Invoke& invoke = BuildInvoke(invokeId, H4506_CallWaitingOperations::e_callWaiting);

  H4506_CallWaitingArg argument;

  argument.IncludeOptionalField(H4506_CallWaitingArg::e_nbOfAddWaitingCalls);
  argument.m_nbOfAddWaitingCalls = numCallsWaiting;

  PTRACE(4, "H4502\tSending supplementary service PDU argument:\n  "
         << setprecision(2) << argument);
  
  invoke.IncludeOptionalField(X880_Invoke::e_argument);
  invoke.m_argument.EncodeSubType(argument);
}


void H450ServiceAPDU::BuildCallIntrusionForcedRelease(int invokeId,
                                                      int CICL)
{
  X880_Invoke& invoke = BuildInvoke(invokeId, H45011_H323CallIntrusionOperations::e_callIntrusionForcedRelease);

  H45011_CIFrcRelArg argument;

  argument.m_ciCapabilityLevel = CICL;

  invoke.IncludeOptionalField(X880_Invoke::e_argument);
  invoke.m_argument.EncodeSubType(argument);
}


X880_ReturnResult& H450ServiceAPDU::BuildCallIntrusionForcedReleaseResult(int invokeId)
{
  PTRACE(1 ,"H450.11\tH450ServiceAPDU::BuildCallIntrusionForcedReleaseResult BEGIN");
  
  X880_ReturnResult& result = BuildReturnResult(invokeId);
  result.IncludeOptionalField(X880_ReturnResult::e_result);
  result.m_result.m_opcode.SetTag(X880_Code::e_local);
  PASN_Integer& operation = (PASN_Integer&) result.m_result.m_opcode;
  operation.SetValue(H45011_H323CallIntrusionOperations::e_callIntrusionForcedRelease);

  H45011_CIFrcRelOptRes ciCIPLRes;

  PPER_Stream resultStream;
  ciCIPLRes.Encode(resultStream);
  resultStream.CompleteEncoding();
  result.m_result.m_result.SetValue(resultStream);
  PTRACE(4 ,"H450.11\tH450ServiceAPDU::BuildCallIntrusionForcedReleaseResult END");

  return result;
}


void H450ServiceAPDU::BuildCallIntrusionForcedReleaseError()
{
/**
  TBD
*/
}


void H450ServiceAPDU::BuildCallIntrusionGetCIPL(int invokeId)
{
  PTRACE(4, "H450.11\tBuildCallIntrusionGetCIPL invokeId=" << invokeId);
  X880_Invoke invoke = BuildInvoke(invokeId, H45011_H323CallIntrusionOperations::e_callIntrusionGetCIPL);
}


void H450ServiceAPDU::BuildCallIntrusionImpending(int invokeId)
{
  PTRACE(4, "H450.11\tBuildCallIntrusionImpending invokeId=" << invokeId);
  X880_Invoke& invoke = BuildInvoke(invokeId, H45011_H323CallIntrusionOperations::e_callIntrusionNotification);
 
  H45011_CINotificationArg argument;

  argument.m_ciStatusInformation = H45011_CIStatusInformation::e_callIntrusionImpending;

  invoke.IncludeOptionalField(X880_Invoke::e_argument);
  invoke.m_argument.EncodeSubType(argument);
}


void H450ServiceAPDU::BuildCallIntrusionForceRelesed(int invokeId)
{
  PTRACE(4, "H450.11\tBuildCallIntrusionForceRelesed invokeId=" << invokeId);
  X880_Invoke& invoke = BuildInvoke(invokeId, H45011_H323CallIntrusionOperations::e_callIntrusionNotification);
 
  H45011_CINotificationArg argument;

  argument.m_ciStatusInformation = H45011_CIStatusInformation::e_callForceReleased;

  invoke.IncludeOptionalField(X880_Invoke::e_argument);
  invoke.m_argument.EncodeSubType(argument);
}


void H450ServiceAPDU::AttachSupplementaryServiceAPDU(H323SignalPDU & pdu)
{
  H4501_SupplementaryService supplementaryService;

  // Create an H.450.1 supplementary service object
  // and store the H450ServiceAPDU in the ROS array.
  supplementaryService.m_serviceApdu.SetTag(H4501_ServiceApdus::e_rosApdus);
  H4501_ArrayOf_ROS & operations = (H4501_ArrayOf_ROS &)supplementaryService.m_serviceApdu;
  operations.SetSize(1);
  operations[0] = *this;

  PTRACE(4, "H4501\tSending supplementary service PDU:\n  "
         << setprecision(2) << supplementaryService);

  // Add the H.450 PDU to the H.323 User-to-User PDU as an OCTET STRING
  pdu.m_h323_uu_pdu.IncludeOptionalField(H225_H323_UU_PDU::e_h4501SupplementaryService);
  pdu.m_h323_uu_pdu.m_h4501SupplementaryService.SetSize(1);
  pdu.m_h323_uu_pdu.m_h4501SupplementaryService[0].EncodeSubType(supplementaryService);
}


BOOL H450ServiceAPDU::WriteFacilityPDU(H323Connection & connection)
{
  H323SignalPDU facilityPDU;
  facilityPDU.BuildFacility(connection, TRUE);

  AttachSupplementaryServiceAPDU(facilityPDU);

  return connection.WriteSignalPDU(facilityPDU);
}


void H450ServiceAPDU::ParseEndpointAddress(H4501_EndpointAddress& endpointAddress,
                                           PString& remoteParty)
{
  H4501_ArrayOf_AliasAddress& destinationAddress = endpointAddress.m_destinationAddress;

  PString alias;
  H323TransportAddress transportAddress;

  for (PINDEX i = 0; i < destinationAddress.GetSize(); i++) {
    H225_AliasAddress& aliasAddress = destinationAddress[i];

    if (aliasAddress.GetTag() == H225_AliasAddress::e_transportID)
      transportAddress = (H225_TransportAddress &)aliasAddress;
    else
      alias = ::H323GetAliasAddressString(aliasAddress);
  }

  if (alias.IsEmpty()) {
    remoteParty = transportAddress;
  }
  else if (transportAddress.IsEmpty()) {
    remoteParty = alias;
  }
  else {
    remoteParty = alias + '@' + transportAddress;
  }
}


/////////////////////////////////////////////////////////////////////////////

H450xDispatcher::H450xDispatcher(H323Connection & conn)
  : connection(conn)
{
  opcodeHandler.DisallowDeleteObjects();

  nextInvokeId = 0;
}


void H450xDispatcher::AddOpCode(unsigned opcode, H450xHandler * handler)
{
  if (PAssertNULL(handler) == NULL)
    return;

  if (handlers.GetObjectsIndex(handler) == P_MAX_INDEX)
    handlers.Append(handler);

  opcodeHandler.SetAt(opcode, handler);
}


void H450xDispatcher::AttachToSetup(H323SignalPDU & pdu)
{
  for (PINDEX i = 0; i < handlers.GetSize(); i++)
    handlers[i].AttachToSetup(pdu);
}


void H450xDispatcher::AttachToAlerting(H323SignalPDU & pdu)
{
  for (PINDEX i = 0; i < handlers.GetSize(); i++)
    handlers[i].AttachToAlerting(pdu);
}


void H450xDispatcher::AttachToConnect(H323SignalPDU & pdu)
{
  for (PINDEX i = 0; i < handlers.GetSize(); i++)
    handlers[i].AttachToConnect(pdu);
}


void H450xDispatcher::AttachToReleaseComplete(H323SignalPDU & pdu)
{
  for (PINDEX i = 0; i < handlers.GetSize(); i++)
    handlers[i].AttachToReleaseComplete(pdu);
}


BOOL H450xDispatcher::HandlePDU(const H323SignalPDU & pdu)
{
BOOL result = TRUE;
  for (PINDEX i = 0; i < pdu.m_h323_uu_pdu.m_h4501SupplementaryService.GetSize(); i++) {
    H4501_SupplementaryService supplementaryService;

    // Decode the supplementary service PDU from the PPER Stream
    if (pdu.m_h323_uu_pdu.m_h4501SupplementaryService[i].DecodeSubType(supplementaryService)) {
      PTRACE(4, "H4501\tReceived supplementary service PDU:\n  "
             << setprecision(2) << supplementaryService);
    }
    else {
      PTRACE(1, "H4501\tInvalid supplementary service PDU decode:\n  "
             << setprecision(2) << supplementaryService);
      continue;
    }

    H4501_InterpretationApdu & interpretation = supplementaryService.m_interpretationApdu;

    if (supplementaryService.m_serviceApdu.GetTag() == H4501_ServiceApdus::e_rosApdus) {
      H4501_ArrayOf_ROS& operations = (H4501_ArrayOf_ROS&) supplementaryService.m_serviceApdu;

      for (PINDEX j = 0; j < operations.GetSize(); j ++) {
        X880_ROS& operation = operations[j];

        PTRACE(3, "H4501\tX880 ROS " << operation.GetTagName());

        switch (operation.GetTag()) {
          case X880_ROS::e_invoke:
            result = OnReceivedInvoke((X880_Invoke &)operation, interpretation);
            break;

          case X880_ROS::e_returnResult:
            result = OnReceivedReturnResult((X880_ReturnResult &)operation);
            break;

          case X880_ROS::e_returnError:
            result = OnReceivedReturnError((X880_ReturnError &)operation);
            break;

          case X880_ROS::e_reject:
            result = OnReceivedReject((X880_Reject &)operation);
            break;

          default :
            break;
        }
      }
    }
  }
  return result;
}

BOOL H450xDispatcher::OnReceivedInvoke(X880_Invoke & invoke, H4501_InterpretationApdu & interpretation)
{
  BOOL result = TRUE;
  // Get the invokeId
  int invokeId = invoke.m_invokeId.GetValue();

  // Get the linkedId if present
  int linkedId = -1;
  if (invoke.HasOptionalField(X880_Invoke::e_linkedId)) {
    linkedId = invoke.m_linkedId.GetValue();
  }

  // Get the argument if present
  PASN_OctetString * argument = NULL;
  if (invoke.HasOptionalField(X880_Invoke::e_argument)) {
    argument = &invoke.m_argument;
  }

  // Get the opcode
  if (invoke.m_opcode.GetTag() == X880_Code::e_local) {
    int opcode = ((PASN_Integer&) invoke.m_opcode).GetValue();
    if (!opcodeHandler.Contains(opcode)) {
      PTRACE(2, "H4501\tInvoke of unsupported local opcode:\n  " << invoke);	  
      if (interpretation.GetTag() != H4501_InterpretationApdu::e_discardAnyUnrecognizedInvokePdu)
        SendInvokeReject(invokeId, 1 /*X880_InvokeProblem::e_unrecognisedOperation*/);
      if (interpretation.GetTag() == H4501_InterpretationApdu::e_clearCallIfAnyInvokePduNotRecognized)
        result = FALSE;
    }
    else
      result = opcodeHandler[opcode].OnReceivedInvoke(opcode, invokeId, linkedId, argument);
  }
  else {
    if (interpretation.GetTag() != H4501_InterpretationApdu::e_discardAnyUnrecognizedInvokePdu)
      SendInvokeReject(invokeId, 1 /*X880_InvokeProblem::e_unrecognisedOperation*/);
    PTRACE(2, "H4501\tInvoke of unsupported global opcode:\n  " << invoke);
    if (interpretation.GetTag() == H4501_InterpretationApdu::e_clearCallIfAnyInvokePduNotRecognized)
      result = FALSE;
  }
  return result;
}


BOOL H450xDispatcher::OnReceivedReturnResult(X880_ReturnResult & returnResult)
{
  unsigned invokeId = returnResult.m_invokeId.GetValue();

  for (PINDEX i = 0; i < handlers.GetSize(); i++) {
    if (handlers[i].GetInvokeId() == invokeId) {
      handlers[i].OnReceivedReturnResult(returnResult);
      break;
    }
  }
  return TRUE;
}


BOOL H450xDispatcher::OnReceivedReturnError(X880_ReturnError & returnError)
{
  BOOL result=TRUE;
  unsigned invokeId = returnError.m_invokeId.GetValue();
  int errorCode = 0;

  if (returnError.m_errorCode.GetTag() == X880_Code::e_local)
    errorCode = ((PASN_Integer&) returnError.m_errorCode).GetValue();

  for (PINDEX i = 0; i < handlers.GetSize(); i++) {
    if (handlers[i].GetInvokeId() == invokeId) {
      result = handlers[i].OnReceivedReturnError(errorCode, returnError);
      break;
    }
  }
  return result;
}


BOOL H450xDispatcher::OnReceivedReject(X880_Reject & reject)
{
  int problem = 0;

  switch (reject.m_problem.GetTag()) {
    case X880_Reject_problem::e_general:
    {
      X880_GeneralProblem & generalProblem = reject.m_problem;
      problem = generalProblem.GetValue();
    }
    break;

    case X880_Reject_problem::e_invoke:
    {
      X880_InvokeProblem & invokeProblem = reject.m_problem;
      problem = invokeProblem.GetValue();
    }
    break;

    case X880_Reject_problem::e_returnResult:
    {
      X880_ReturnResultProblem & returnResultProblem = reject.m_problem;
      problem = returnResultProblem.GetValue();
    }
    break;

    case X880_Reject_problem::e_returnError:
    {
      X880_ReturnErrorProblem & returnErrorProblem = reject.m_problem;
      problem = returnErrorProblem.GetValue();
    }
    break;

    default:
      break;
  }


  unsigned invokeId = reject.m_invokeId;
  for (PINDEX i = 0; i < handlers.GetSize(); i++) {
    if (handlers[i].GetInvokeId() == invokeId) {
      handlers[i].OnReceivedReject(reject.m_problem.GetTag(), problem);
      break;
    }
  }
  return TRUE;
}


void H450xDispatcher::SendReturnError(int invokeId, int returnError)
{
  H450ServiceAPDU serviceAPDU;

  serviceAPDU.BuildReturnError(invokeId, returnError);

  serviceAPDU.WriteFacilityPDU(connection);
}


void H450xDispatcher::SendGeneralReject(int invokeId, int problem)
{
  H450ServiceAPDU serviceAPDU;

  X880_Reject & reject = serviceAPDU.BuildReject(invokeId);
  reject.m_problem.SetTag(X880_Reject_problem::e_general);
  X880_GeneralProblem & generalProblem = (X880_GeneralProblem &) reject.m_problem;
  generalProblem = problem;

  serviceAPDU.WriteFacilityPDU(connection);
}


void H450xDispatcher::SendInvokeReject(int invokeId, int problem)
{
  H450ServiceAPDU serviceAPDU;

  X880_Reject & reject = serviceAPDU.BuildReject(invokeId);
  reject.m_problem.SetTag(X880_Reject_problem::e_invoke);
  X880_InvokeProblem & invokeProblem = (X880_InvokeProblem &) reject.m_problem;
  invokeProblem = problem;

  serviceAPDU.WriteFacilityPDU(connection);
}


void H450xDispatcher::SendReturnResultReject(int invokeId, int problem)
{
  H450ServiceAPDU serviceAPDU;

  X880_Reject & reject = serviceAPDU.BuildReject(invokeId);
  reject.m_problem.SetTag(X880_Reject_problem::e_returnResult);
  X880_ReturnResultProblem & returnResultProblem = reject.m_problem;
  returnResultProblem = problem;

  serviceAPDU.WriteFacilityPDU(connection);
}


void H450xDispatcher::SendReturnErrorReject(int invokeId, int problem)
{
  H450ServiceAPDU serviceAPDU;

  X880_Reject & reject = serviceAPDU.BuildReject(invokeId);
  reject.m_problem.SetTag(X880_Reject_problem::e_returnError);
  X880_ReturnErrorProblem & returnErrorProblem = reject.m_problem;
  returnErrorProblem = problem;

  serviceAPDU.WriteFacilityPDU(connection);
}


/////////////////////////////////////////////////////////////////////////////

H450xHandler::H450xHandler(H323Connection & conn, H450xDispatcher & disp)
  : endpoint(conn.GetEndPoint()),
    connection(conn),
    dispatcher(disp)
{
  currentInvokeId = 0;
}


void H450xHandler::AttachToSetup(H323SignalPDU &)
{
}


void H450xHandler::AttachToAlerting(H323SignalPDU &)
{
}


void H450xHandler::AttachToConnect(H323SignalPDU &)
{
}


void H450xHandler::AttachToReleaseComplete(H323SignalPDU &)
{
}


BOOL H450xHandler::OnReceivedReturnResult(X880_ReturnResult & /*returnResult*/)
{
  return TRUE;
}


BOOL H450xHandler::OnReceivedReturnError(int /*errorCode*/,
                                        X880_ReturnError & /*returnError*/)
{
  return TRUE;
}


BOOL H450xHandler::OnReceivedReject(int /*problemType*/,
                                   int /*problemNumber*/)
{
  return TRUE;
}


void H450xHandler::SendReturnError(int returnError)
{
  dispatcher.SendReturnError(currentInvokeId, returnError);
  currentInvokeId = 0;
}


void H450xHandler::SendGeneralReject(int problem)
{
  dispatcher.SendGeneralReject(currentInvokeId, problem);
  currentInvokeId = 0;
}


void H450xHandler::SendInvokeReject(int problem)
{
  dispatcher.SendInvokeReject(currentInvokeId, problem);
  currentInvokeId = 0;
}


void H450xHandler::SendReturnResultReject(int problem)
{
  dispatcher.SendReturnResultReject(currentInvokeId, problem);
  currentInvokeId = 0;
}


void H450xHandler::SendReturnErrorReject(int problem)
{
  dispatcher.SendReturnErrorReject(currentInvokeId, problem);
  currentInvokeId = 0;
}


BOOL H450xHandler::DecodeArguments(PASN_OctetString * argString,
                                  PASN_Object & argObject,
                                  int absentErrorCode)
{
  if (argString == NULL) {
    if (absentErrorCode >= 0)
      SendReturnError(absentErrorCode);
    return FALSE;
  }

  PPER_Stream argStream(*argString);
  if (argObject.Decode(argStream)) {
    PTRACE(4, "H4501\tSupplementary service argument:\n  "
           << setprecision(2) << argObject);
    return TRUE;
  }

  PTRACE(1, "H4501\tInvalid supplementary service argument:\n  "
         << setprecision(2) << argObject);
  return FALSE;
}


/////////////////////////////////////////////////////////////////////////////

H4502Handler::H4502Handler(H323Connection & conn, H450xDispatcher & disp)
  : H450xHandler(conn, disp)
{
  dispatcher.AddOpCode(H4502_CallTransferOperation::e_callTransferIdentify, this);
  dispatcher.AddOpCode(H4502_CallTransferOperation::e_callTransferAbandon, this);
  dispatcher.AddOpCode(H4502_CallTransferOperation::e_callTransferInitiate, this);
  dispatcher.AddOpCode(H4502_CallTransferOperation::e_callTransferSetup, this);
  dispatcher.AddOpCode(H4502_CallTransferOperation::e_callTransferUpdate, this);
  dispatcher.AddOpCode(H4502_CallTransferOperation::e_subaddressTransfer, this);
  dispatcher.AddOpCode(H4502_CallTransferOperation::e_callTransferComplete, this);
  dispatcher.AddOpCode(H4502_CallTransferOperation::e_callTransferActive, this);

  transferringCallToken = "";
  ctState = e_ctIdle;
  ctResponseSent = FALSE;
  CallToken = PString();
  consultationTransfer = FALSE;

  ctTimer.SetNotifier(PCREATE_NOTIFIER(OnCallTransferTimeOut));
}


void H4502Handler::AttachToSetup(H323SignalPDU & pdu)
{
  // Do we need to attach a call transfer setup invoke APDU?
  if (ctState != e_ctAwaitSetupResponse)
    return;

  H450ServiceAPDU serviceAPDU;

  // Store the outstanding invokeID associated with this connection
  currentInvokeId = dispatcher.GetNextInvokeId();

  // Use the call identity from the ctInitiateArg
  serviceAPDU.BuildCallTransferSetup(currentInvokeId, transferringCallIdentity);

  serviceAPDU.AttachSupplementaryServiceAPDU(pdu);
}


void H4502Handler::AttachToAlerting(H323SignalPDU & pdu)
{
  // Do we need to send a callTransferSetup return result APDU?
  if (currentInvokeId == 0 || ctResponseSent)
    return;

  H450ServiceAPDU serviceAPDU;
  serviceAPDU.BuildReturnResult(currentInvokeId);
  serviceAPDU.AttachSupplementaryServiceAPDU(pdu);
  ctResponseSent = TRUE;
  currentInvokeId = 0;
}


void H4502Handler::AttachToConnect(H323SignalPDU & pdu)
{
  // Do we need to include a ctInitiateReturnResult APDU in our Release Complete Message?
  if (currentInvokeId == 0 || ctResponseSent)
    return;

  H450ServiceAPDU serviceAPDU;
  serviceAPDU.BuildReturnResult(currentInvokeId);
  serviceAPDU.AttachSupplementaryServiceAPDU(pdu);
  ctResponseSent = TRUE;
  currentInvokeId = 0;
}


void H4502Handler::AttachToReleaseComplete(H323SignalPDU & pdu)
{
  // Do we need to include a ctInitiateReturnResult APDU in our Release Complete Message?
  if (currentInvokeId == 0)
    return;

  // If the SETUP message we received from the other end had a callTransferSetup APDU
  // in it, then we need to send back a RELEASE COMPLETE PDU with a callTransferSetup 
  // ReturnError.
  // Else normal call - clear it down
  H450ServiceAPDU serviceAPDU;

  if (ctResponseSent) {
    serviceAPDU.BuildReturnResult(currentInvokeId);
    ctResponseSent = FALSE;
    currentInvokeId = 0;
  }
  else {
    serviceAPDU.BuildReturnError(currentInvokeId, H4501_GeneralErrorList::e_notAvailable);
    ctResponseSent = TRUE;
    currentInvokeId = 0;
  }

  serviceAPDU.AttachSupplementaryServiceAPDU(pdu);
}


BOOL H4502Handler::OnReceivedInvoke(int opcode,
                                    int invokeId,
                                    int linkedId,
                                    PASN_OctetString * argument)
{
  currentInvokeId = invokeId;

  switch (opcode) {
    case H4502_CallTransferOperation::e_callTransferIdentify:
      OnReceivedCallTransferIdentify(linkedId);
      break;

    case H4502_CallTransferOperation::e_callTransferAbandon:
      OnReceivedCallTransferAbandon(linkedId);
      break;

    case H4502_CallTransferOperation::e_callTransferInitiate:
      OnReceivedCallTransferInitiate(linkedId, argument);
      break;

    case H4502_CallTransferOperation::e_callTransferSetup:
      OnReceivedCallTransferSetup(linkedId, argument);
      break;

    case H4502_CallTransferOperation::e_callTransferUpdate:
      OnReceivedCallTransferUpdate(linkedId, argument);
      break;

    case H4502_CallTransferOperation::e_subaddressTransfer:
      OnReceivedSubaddressTransfer(linkedId, argument);
      break;

    case H4502_CallTransferOperation::e_callTransferComplete:
      OnReceivedCallTransferComplete(linkedId, argument);
      break;

    case H4502_CallTransferOperation::e_callTransferActive:
      OnReceivedCallTransferActive(linkedId, argument);
      break;

    default:
      currentInvokeId = 0;
      return FALSE;
  }

  return TRUE;
}


void H4502Handler::OnReceivedCallTransferIdentify(int /*linkedId*/)
{
  if (!endpoint.OnCallTransferIdentify(connection)) {
    SendReturnError(H4501_GeneralErrorList::e_notAvailable);
    return;
  }
  
  // Send a FACILITY message with a callTransferIdentify return result
  // Supplementary Service PDU to the transferring endpoint.
  H450ServiceAPDU serviceAPDU;

  X880_ReturnResult& result = serviceAPDU.BuildReturnResult(currentInvokeId);
  result.IncludeOptionalField(X880_ReturnResult::e_result);
  result.m_result.m_opcode.SetTag(X880_Code::e_local);
  PASN_Integer& operation = (PASN_Integer&) result.m_result.m_opcode;
  operation.SetValue(H4502_CallTransferOperation::e_callTransferIdentify);

  H4502_CTIdentifyRes ctIdentifyResult;

  // Restrict the generated value to 4 digits (13 bits)
  unsigned int id = endpoint.GetNextH450CallIdentityValue() & 0x1FFF;
  PString pstrId(PString::Unsigned, id);
  ctIdentifyResult.m_callIdentity = pstrId;

  // Store the callIdentity of this connection in the dictionary
  endpoint.GetCallIdentityDictionary().SetAt(pstrId, &connection);

  H4501_ArrayOf_AliasAddress& aliasAddress = ctIdentifyResult.m_reroutingNumber.m_destinationAddress;

  PString localName = connection.GetLocalPartyName();
  if (localName.IsEmpty()) 
    aliasAddress.SetSize(1);
  else {
    aliasAddress.SetSize(2);
    aliasAddress[1].SetTag(H225_AliasAddress::e_dialedDigits);
    H323SetAliasAddress(localName, aliasAddress[1]);  // Will encode as h323-Id if not E.164
  }

  H323TransportAddress address;
  address = connection.GetSignallingChannel()->GetLocalAddress();

  aliasAddress[0].SetTag(H225_AliasAddress::e_transportID);
  H225_TransportAddress& cPartyTransport = (H225_TransportAddress&) aliasAddress[0];
  address.SetPDU(cPartyTransport);

  PPER_Stream resultStream;
  ctIdentifyResult.Encode(resultStream);
  resultStream.CompleteEncoding();
  result.m_result.m_result.SetValue(resultStream);

  serviceAPDU.WriteFacilityPDU(connection);

  ctState = e_ctAwaitSetup;

  // start timer CT-T2
  PTRACE(4, "H450.2\tStarting timer CT-T2");
  StartctTimer(endpoint.GetCallTransferT2());
}


void H4502Handler::OnReceivedCallTransferAbandon(int /*linkedId*/)
{
  switch (ctState) {
    case e_ctAwaitSetup:
      {
        // Stop Timer CT-T2 and enter state e_ctIdle
        StopctTimer();
        PTRACE(4, "H4502\tStopping timer CT-T2");

        currentInvokeId = 0;
        ctState = e_ctIdle;
      }
      break;

    default:
      break;
  }
}


void H4502Handler::OnReceivedCallTransferInitiate(int /*linkedId*/,
                                                  PASN_OctetString * argument)
{
  // TBD: Check Call Hold status. If call is held, it must first be 
  // retrieved before being transferred. -- dcassel 4/01

  H4502_CTInitiateArg ctInitiateArg;
  if (!DecodeArguments(argument, ctInitiateArg,
                       H4502_CallTransferErrors::e_invalidReroutingNumber))
    return;

  ctResponseSent = TRUE;

  PString remoteParty;
  H450ServiceAPDU::ParseEndpointAddress(ctInitiateArg.m_reroutingNumber, remoteParty);

  PString newToken;
  if (!endpoint.OnCallTransferInitiate(connection, remoteParty) ||
       endpoint.SetupTransfer(connection.GetCallToken(),
                              ctInitiateArg.m_callIdentity.GetValue(),
                              remoteParty, newToken) == NULL)
    SendReturnError(H4502_CallTransferErrors::e_establishmentFailure);
}


void H4502Handler::OnReceivedCallTransferSetup(int /*linkedId*/,
                                               PASN_OctetString * argument)
{
  H4502_CTSetupArg ctSetupArg;
  if (!DecodeArguments(argument, ctSetupArg,
                       H4502_CallTransferErrors::e_unrecognizedCallIdentity))
    return;

  // Get the Transferring User's details if present
  PString transferringParty;
  if (ctSetupArg.HasOptionalField(H4502_CTSetupArg::e_transferringNumber)) {
    H450ServiceAPDU::ParseEndpointAddress(ctSetupArg.m_transferringNumber, transferringParty);
  }

  PString callIdentity;
  callIdentity = ctSetupArg.m_callIdentity;

  if (callIdentity.IsEmpty()) { // Blind Transfer
    switch (ctState) {
      case e_ctIdle:
        ctState = e_ctAwaitSetupResponse;
        break;

      // Wrong State
      default :
        break;
    }
  }
  else { // Transfer through Consultation
    
    // We need to check that the call identity and destination address information match those in the 
    // second call.  For the time being we just check that the call identities match as there does not 
    // appear to be an elegant solution to compare the destination address information.

    // Get this callIdentity from our dictionary (if present)
    H323Connection *secondaryCall = endpoint.GetCallIdentityDictionary().GetAt(callIdentity);
  
    if (secondaryCall != NULL)
      secondaryCall->HandleConsultationTransfer(callIdentity, connection);
    else  // Mismatched callIdentity
      SendReturnError(H4502_CallTransferErrors::e_unrecognizedCallIdentity);
  }
}


void H4502Handler::OnReceivedCallTransferUpdate(int /*linkedId*/,
                                                PASN_OctetString * argument)
{
  H4502_CTUpdateArg ctUpdateArg;
  if (!DecodeArguments(argument, ctUpdateArg, -1))
    return;

}


void H4502Handler::OnReceivedSubaddressTransfer(int /*linkedId*/,
                                                PASN_OctetString * argument)
{
  H4502_SubaddressTransferArg subaddressTransferArg;
  if (!DecodeArguments(argument, subaddressTransferArg, -1))
    return;

}


void H4502Handler::OnReceivedCallTransferComplete(int /*linkedId*/,
                                                  PASN_OctetString * argument)
{
  H4502_CTCompleteArg ctCompleteArg;
  if (!DecodeArguments(argument, ctCompleteArg, -1))
    return;

}


void H4502Handler::OnReceivedCallTransferActive(int /*linkedId*/,
                                                PASN_OctetString * argument)
{
  H4502_CTActiveArg ctActiveArg;
  if (!DecodeArguments(argument, ctActiveArg, -1))
    return;

}


BOOL H4502Handler::OnReceivedReturnResult(X880_ReturnResult & returnResult)
{
  if (currentInvokeId == returnResult.m_invokeId.GetValue()) {
    switch (ctState) {
      case e_ctAwaitInitiateResponse:
        OnReceivedInitiateReturnResult(); 
        break;

      case e_ctAwaitSetupResponse:
        OnReceivedSetupReturnResult();
        break;

      case e_ctAwaitIdentifyResponse:
        OnReceivedIdentifyReturnResult(returnResult);
        break;

      default :
        break;
    }
  }
  return TRUE;
}


void H4502Handler::OnReceivedInitiateReturnResult()
{
  // stop timer CT-T3
  StopctTimer();
  PTRACE(4, "H4502\tStopping timer CT-T3");
  ctState = e_ctIdle;
  currentInvokeId = 0;

  // clear the primary and secondary call if not already cleared,
}


void H4502Handler::OnReceivedSetupReturnResult()
{
  // stop timer CT-T4
  StopctTimer();
  PTRACE(4, "H4502\tStopping timer CT-T4");
  ctState = e_ctIdle;
  currentInvokeId = 0;

  // Clear the primary call
  endpoint.ClearCall(transferringCallToken, H323Connection::EndedByCallForwarded);
}


void H4502Handler::OnReceivedIdentifyReturnResult(X880_ReturnResult &returnResult)
{
  // stop timer CT-T1
  StopctTimer();
  PTRACE(4, "H4502\tStopping timer CT-T1");

  // Have received response.
  ctState = e_ctIdle;

  // Get the return result if present
  PASN_OctetString * result = NULL;
  if (returnResult.HasOptionalField(X880_ReturnResult::e_result)) {
    result = &returnResult.m_result.m_result;

    // Extract the C Party Details
    H4502_CTIdentifyRes ctIdentifyResult;

    PPER_Stream resultStream(*result);
    ctIdentifyResult.Decode(resultStream);
    PString callIdentity = ctIdentifyResult.m_callIdentity.GetValue();

    PString remoteParty;
    H450ServiceAPDU::ParseEndpointAddress(ctIdentifyResult.m_reroutingNumber, remoteParty);

    // Store the secondary call token on the primary connection so we can send a 
    // callTransferAbandon invoke APDU on the secondary call at a later stage if we 
    // get back a callTransferInitiateReturnError
    H323Connection* primaryConnection = endpoint.FindConnectionWithLock(CallToken);
    if (primaryConnection != NULL) {
      primaryConnection->SetAssociatedCallToken(connection.GetCallToken());

      // Send a callTransferInitiate invoke APDU in a FACILITY message
      // to the transferred endpoint on the primary call
      endpoint.TransferCall(primaryConnection->GetCallToken(), remoteParty, callIdentity);

      primaryConnection->Unlock();
    }
  }
}


BOOL H4502Handler::OnReceivedReturnError(int errorCode, X880_ReturnError &returnError)
{
  if (currentInvokeId == returnError.m_invokeId.GetValue()) {
    switch (ctState) {
      case e_ctAwaitInitiateResponse:
        OnReceivedInitiateReturnError();
        break;

      case e_ctAwaitSetupResponse:
        OnReceivedSetupReturnError(errorCode);
        break;

      case e_ctAwaitIdentifyResponse:
        OnReceivedIdentifyReturnError();
        break;

      default :
        break;
    }
  }
  return TRUE;
}


void H4502Handler::OnReceivedInitiateReturnError(const bool timerExpiry)
{ 
  if (!timerExpiry) {
    // stop timer CT-T3
    StopctTimer();
    PTRACE(4, "H4502\tStopping timer CT-T3 on Error");
  }
  else
    PTRACE(4, "H4502\tTimer CT-T3 has expired on the Transferring Endpoint awaiting a response to a callTransferInitiate APDU.");

  currentInvokeId = 0;
  ctState = e_ctIdle;


  // Send a callTransferAbandon invoke APDU in a FACILITY message on the secondary call
  // (if it exists) and enter state CT-Idle.
  H323Connection* secondaryConnection = endpoint.FindConnectionWithLock(CallToken);

  if (secondaryConnection != NULL) {
    H450ServiceAPDU serviceAPDU;

    serviceAPDU.BuildCallTransferAbandon(dispatcher.GetNextInvokeId());
    serviceAPDU.WriteFacilityPDU(*secondaryConnection);
    secondaryConnection->Unlock();
  }

  if (!transferringCallToken) {
	  H323Connection* primaryConnection = endpoint.FindConnectionWithLock(transferringCallToken);
	  primaryConnection->OnReceivedInitiateReturnError();
	  primaryConnection->Unlock();
  } else {
      endpoint.OnReceivedInitiateReturnError();
  }
}


void H4502Handler::OnReceivedSetupReturnError(int errorCode,
                                              const bool timerExpiry)
{
  ctState = e_ctIdle;
  currentInvokeId = 0;
  
  if (!timerExpiry) {
    // stop timer CT-T4 if it is running
    StopctTimer();
    PTRACE(4, "H4502\tStopping timer CT-T4");  
  }
  else {
    PTRACE(3, "H4502\tTimer CT-T4 has expired on the Transferred Endpoint awaiting a response to a callTransferSetup APDU.");

    // Clear the transferred call.
    endpoint.ClearCall(connection.GetCallToken());
  }

  // Send a facility message to the transferring endpoint
  // containing a call transfer initiate return error
  H323Connection* primaryConnection = endpoint.FindConnectionWithLock(transferringCallToken);

  if (primaryConnection != NULL) {
    primaryConnection->HandleCallTransferFailure(errorCode);
    primaryConnection->Unlock();
  }
}


void H4502Handler::OnReceivedIdentifyReturnError(const bool timerExpiry)
{
  // The transferred-to user cannot participate in our transfer request
  ctState = e_ctIdle;
  currentInvokeId = 0;

  if (!timerExpiry) {
    // stop timer CT-T1
    StopctTimer();
    PTRACE(4, "H4502\tStopping timer CT-T1");
  }
  else {
    PTRACE(4, "H4502\tTimer CT-T1 has expired on the Transferring Endpoint awaiting a response to a callTransferIdentify APDU.");

    // send a callTransferAbandon invoke APDU in a FACILITY message on the secondary call
    // and enter state CT-Idle.
    connection.Lock();

    H450ServiceAPDU serviceAPDU;

    serviceAPDU.BuildCallTransferAbandon(dispatcher.GetNextInvokeId());
    serviceAPDU.WriteFacilityPDU(connection);

    connection.Unlock();
  }
}

void H4502Handler::TransferCall(const PString & remoteParty,
                                const PString & callIdentity)
{
  currentInvokeId = dispatcher.GetNextInvokeId();

  // Send a FACILITY message with a callTransferInitiate Invoke
  // Supplementary Service PDU to the transferred endpoint.
  H450ServiceAPDU serviceAPDU;

  PString alias;
  H323TransportAddress address;

  PStringList Addresses;
  endpoint.ResolveCallParty(remoteParty, Addresses);
  endpoint.ParsePartyName(Addresses[0], alias, address);

  serviceAPDU.BuildCallTransferInitiate(currentInvokeId, callIdentity, alias, address);
  serviceAPDU.WriteFacilityPDU(connection);

  ctState = e_ctAwaitInitiateResponse;

  // start timer CT-T3
  PTRACE(4, "H4502\tStarting timer CT-T3");
  StartctTimer(connection.GetEndPoint().GetCallTransferT3());
}


void H4502Handler::ConsultationTransfer(const PString & primaryCallToken)
{
  currentInvokeId = dispatcher.GetNextInvokeId();

  // Store the call token of the primary call on the secondary call.
  SetAssociatedCallToken(primaryCallToken);

  // Send a FACILITY message with a callTransferIdentify Invoke
  // Supplementary Service PDU to the transferred-to endpoint.
  H450ServiceAPDU serviceAPDU;

  serviceAPDU.BuildCallTransferIdentify(currentInvokeId);
  serviceAPDU.WriteFacilityPDU(connection);

  ctState = e_ctAwaitIdentifyResponse;

  // start timer CT-T1
  PTRACE(4, "H4502\tStarting timer CT-T1");
  StartctTimer(endpoint.GetCallTransferT1());
}


void H4502Handler::HandleConsultationTransfer(const PString & callIdentity,
                                              H323Connection& incoming)
{
  switch (ctState) {
    case e_ctAwaitSetup:
      {
        // Remove this callIdentity, connection pair from our dictionary as we no longer need it
        endpoint.GetCallIdentityDictionary().RemoveAt(callIdentity);

        // Stop timer CT-T2
        StopctTimer();
        PTRACE(4, "H4502\tStopping timer CT-T2");

        PTRACE(4, "H450.2\tConsultation Transfer successful, clearing secondary call");

        incoming.OnConsultationTransferSuccess(connection);

        currentInvokeId = 0;
        ctState = e_ctIdle;

        endpoint.ClearCall(connection.GetCallToken());
      }
      break;

    // Wrong Call Transfer State
    default :
      break;

  }
}


void H4502Handler::AwaitSetupResponse(const PString & token,
                                      const PString & identity)
{
  transferringCallToken = token;
  transferringCallIdentity = identity;
  ctState = e_ctAwaitSetupResponse;

  // start timer CT-T4
  PTRACE(4, "H450.2\tStarting timer CT-T4");
  StartctTimer(connection.GetEndPoint().GetCallTransferT4());
}


void H4502Handler::onReceivedAdmissionReject(const int returnError)
{
  if (ctState == e_ctAwaitSetupResponse) {
    ctState = e_ctIdle;

    // Stop timer CT-T4 if it is running
    StopctTimer();
    PTRACE(3, "H4502\tStopping timer CT-T4");

    // Send a FACILITY message back to the transferring party on the primary connection
    H323Connection * primaryConnection = endpoint.FindConnectionWithLock(transferringCallToken);

    if (primaryConnection != NULL) {
      PTRACE(3, "H4502\tReceived an Admission Reject at the Transferred Endpoint - aborting the transfer.");
      primaryConnection->HandleCallTransferFailure(returnError);
      primaryConnection->Unlock();
    }
  }
}


void H4502Handler::HandleCallTransferFailure(const int returnError)
{
  SendReturnError(returnError);
}


void H4502Handler::StopctTimer()
{
  if (ctTimer.IsRunning())
    ctTimer.Stop();
}


void H4502Handler::OnCallTransferTimeOut(PTimer &, INT)
{
  switch (ctState) {
    // CT-T3 Timeout
    case e_ctAwaitInitiateResponse:
      OnReceivedInitiateReturnError(true);
      break;

    // CT-T1 Timeout
    case e_ctAwaitIdentifyResponse:
      OnReceivedIdentifyReturnError(true);
      break;

    // CT-T2 Timeout
    case e_ctAwaitSetup:
      {
        // Abort the call transfer
        ctState = e_ctIdle;
        currentInvokeId = 0;
        PTRACE(4, "H450.2\tTimer CT-T2 has expired on the Transferred-to endpoint awaiting a callTransferSetup APDU.");  
      }
      break;

    // CT-T4 Timeout
    case e_ctAwaitSetupResponse:
      OnReceivedSetupReturnError(H4502_CallTransferErrors::e_establishmentFailure, true);
      break;

    default:
      break;
  }
}

/////////////////////////////////////////////////////////////////////////////

H4503Handler::H4503Handler(H323Connection & conn, H450xDispatcher & disp)
  : H450xHandler(conn, disp)
{
  dispatcher.AddOpCode(H4503_H323CallDiversionOperations::e_divertingLegInformation2, this);
   
}

BOOL H4503Handler::OnReceivedInvoke(int opcode,
                                    int invokeId,
                                    int linkedId,
                                    PASN_OctetString *argument)
{
  currentInvokeId = invokeId;
  switch (opcode) {
    case H4503_H323CallDiversionOperations::e_divertingLegInformation2:
      OnReceivedDivertingLegInfo2(linkedId, argument);
      break;
    
    default:
      currentInvokeId = 0;
      return FALSE;
  }

  return TRUE;
}

void H4503Handler::OnReceivedDivertingLegInfo2(int /* linkedId*/, PASN_OctetString * argument) 
{
  PTRACE(4, "H4503\tReceived a DivertingLegInfo2 Invoke APDU from the remote endpoint.");  
  H4503_DivertingLegInfo2Arg divertingLegInfo2Arg;
  DecodeArguments(argument, divertingLegInfo2Arg, -1); 

  if(divertingLegInfo2Arg.HasOptionalField(H4503_DivertingLegInfo2Arg::e_originalCalledNr)) {
    //m_originalCalledNr = divertingLegInfo2Arg.m_originalCalledNr.GetTypeAsString();
	H450ServiceAPDU::ParseEndpointAddress(divertingLegInfo2Arg.m_originalCalledNr, m_originalCalledNr);
  }

  if(divertingLegInfo2Arg.HasOptionalField(H4503_DivertingLegInfo2Arg::e_divertingNr))
    m_lastDivertingNr = divertingLegInfo2Arg.m_divertingNr.GetTypeAsString();

  m_diversionCounter = divertingLegInfo2Arg.m_diversionCounter.GetValue();
  m_diversionReason = divertingLegInfo2Arg.m_diversionReason.GetValue();

  PTRACE(4, "H450.3\tOnReceivedDivertingLegInfo2 redirNUm=" << m_originalCalledNr);

  
}

BOOL H4503Handler::GetRedirectingNumber(PString &originalCalledNr, PString &lastDivertingNr, int &divCounter, int &origdivReason, int &divReason)
{
 BOOL bRedirAvail=false; 

 if(!m_originalCalledNr.IsEmpty()) {
   originalCalledNr = m_originalCalledNr; 
   bRedirAvail = true; 
 }
 if(!m_lastDivertingNr.IsEmpty()) {
   lastDivertingNr  = m_lastDivertingNr;
   bRedirAvail = true; 
 }

 divCounter = m_diversionCounter;
 divReason  = m_diversionReason;
 origdivReason  = m_origdiversionReason;

 return bRedirAvail; 

}


/////////////////////////////////////////////////////////////////////////////

H4504Handler::H4504Handler(H323Connection & conn, H450xDispatcher & disp)
  : H450xHandler(conn, disp)
{
  dispatcher.AddOpCode(H4504_CallHoldOperation::e_holdNotific, this);
  dispatcher.AddOpCode(H4504_CallHoldOperation::e_retrieveNotific, this);
  dispatcher.AddOpCode(H4504_CallHoldOperation::e_remoteHold, this);
  dispatcher.AddOpCode(H4504_CallHoldOperation::e_remoteRetrieve, this);

  holdState = e_ch_Idle;
}


BOOL H4504Handler::OnReceivedInvoke(int opcode,
                                    int invokeId,
                                    int linkedId,
                                    PASN_OctetString *)
{
  currentInvokeId = invokeId;

  switch (opcode) {
    case H4504_CallHoldOperation::e_holdNotific:
      OnReceivedLocalCallHold(linkedId);
      break;

    case H4504_CallHoldOperation::e_retrieveNotific:
      OnReceivedLocalCallRetrieve(linkedId);
      break;

    case H4504_CallHoldOperation::e_remoteHold:
      OnReceivedRemoteCallHold(linkedId);
      break;

    case H4504_CallHoldOperation::e_remoteRetrieve:
      OnReceivedRemoteCallRetrieve(linkedId);
      break;

    default:
      currentInvokeId = 0;
      return FALSE;
  }

  return TRUE;
}


void H4504Handler::OnReceivedLocalCallHold(int /*linkedId*/)
{
  PTRACE(4, "H4504\tReceived a holdNotific Invoke APDU from the remote endpoint.");  
  // Optionally close our transmit channel.
}


void H4504Handler::OnReceivedLocalCallRetrieve(int /*linkedId*/)
{
  PTRACE(4, "H4504\tReceived a retrieveNotific Invoke APDU from the remote endpoint.");
  // Re-open our transmit channel if we previously closed it.
}


void H4504Handler::OnReceivedRemoteCallHold(int /*linkedId*/)
{
	// TBD
}


void H4504Handler::OnReceivedRemoteCallRetrieve(int /*linkedId*/)
{
	// TBD
}


void H4504Handler::HoldCall(BOOL localHold)
{
  // TBD: Implement Remote Hold. This implementation only does 
  // local hold. -- dcassel 4/01. 
  if (!localHold)
    return;
  
  // Send a FACILITY message with a callNotific Invoke
  // Supplementary Service PDU to the held endpoint.
  PTRACE(4, "H4504\tTransmitting a holdNotific Invoke APDU to the remote endpoint.");

  H450ServiceAPDU serviceAPDU;

  currentInvokeId = dispatcher.GetNextInvokeId();
  serviceAPDU.BuildInvoke(currentInvokeId, H4504_CallHoldOperation::e_holdNotific);
  serviceAPDU.WriteFacilityPDU(connection);
  
  // Update hold state
  holdState = e_ch_NE_Held;
}


void H4504Handler::RetrieveCall()
{
  // TBD: Implement Remote Hold. This implementation only does

  // Send a FACILITY message with a retrieveNotific Invoke
  // Supplementary Service PDU to the held endpoint.
  PTRACE(4, "H4504\tTransmitting a retrieveNotific Invoke APDU to the remote endpoint.");

  H450ServiceAPDU serviceAPDU;

  currentInvokeId = dispatcher.GetNextInvokeId();
  serviceAPDU.BuildInvoke(currentInvokeId, H4504_CallHoldOperation::e_retrieveNotific);
  serviceAPDU.WriteFacilityPDU(connection);
  
  // Update hold state
  holdState = e_ch_Idle;
}


/////////////////////////////////////////////////////////////////////////////

H4506Handler::H4506Handler(H323Connection & conn, H450xDispatcher & disp)
  : H450xHandler(conn, disp)
{
  dispatcher.AddOpCode(H4506_CallWaitingOperations::e_callWaiting, this);

  cwState = e_cw_Idle;
}


BOOL H4506Handler::OnReceivedInvoke(int opcode,
                                    int invokeId,
                                    int linkedId,
                                    PASN_OctetString *argument)
{
  currentInvokeId = invokeId;

  switch (opcode) {
    case H4506_CallWaitingOperations::e_callWaiting:
      OnReceivedCallWaitingIndication(linkedId, argument);
      break;

    default:
      currentInvokeId = 0;
      return FALSE;
  }

  return TRUE;
}


void H4506Handler::OnReceivedCallWaitingIndication(int /*linkedId*/,
                                                   PASN_OctetString *argument)
{
  H4506_CallWaitingArg cwArg;

  if(!DecodeArguments(argument, cwArg, -1))
    return;

  connection.SetRemoteCallWaiting(cwArg.m_nbOfAddWaitingCalls.GetValue());
  return;
}


void H4506Handler::AttachToAlerting(H323SignalPDU & pdu,
                                    unsigned numberOfCallsWaiting)
{
  PTRACE(4, "H450.6\tAttaching a Call Waiting Invoke PDU to this Alerting message.");
  
  H450ServiceAPDU serviceAPDU;

  // Store the call waiting invokeID associated with this connection
  currentInvokeId = dispatcher.GetNextInvokeId();

  serviceAPDU.BuildCallWaiting(currentInvokeId, numberOfCallsWaiting);
  serviceAPDU.AttachSupplementaryServiceAPDU(pdu);

  cwState = e_cw_Invoked;
}


/////////////////////////////////////////////////////////////////////////////

H45011Handler::H45011Handler(H323Connection & conn, H450xDispatcher & disp)
  : H450xHandler(conn, disp)
{
  dispatcher.AddOpCode(H45011_H323CallIntrusionOperations::e_callIntrusionRequest, this);
  dispatcher.AddOpCode(H45011_H323CallIntrusionOperations::e_callIntrusionGetCIPL, this);
  dispatcher.AddOpCode(H45011_H323CallIntrusionOperations::e_callIntrusionIsolate, this);
  dispatcher.AddOpCode(H45011_H323CallIntrusionOperations::e_callIntrusionForcedRelease, this);
  dispatcher.AddOpCode(H45011_H323CallIntrusionOperations::e_callIntrusionWOBRequest, this);
  dispatcher.AddOpCode(H45011_H323CallIntrusionOperations::e_callIntrusionSilentMonitor, this);
  dispatcher.AddOpCode(H45011_H323CallIntrusionOperations::e_callIntrusionNotification, this);

  dispatcher.AddOpCode(H45010_H323CallOfferOperations::e_cfbOverride, this);
  dispatcher.AddOpCode(H45010_H323CallOfferOperations::e_remoteUserAlerting, this);

  dispatcher.AddOpCode(H4506_CallWaitingOperations::e_callWaiting, this);

  ciState = e_ci_Idle;
  ciSendState = e_ci_sIdle;
  ciReturnState = e_ci_rIdle;
  ciTimer.SetNotifier(PCREATE_NOTIFIER(OnCallIntrudeTimeOut));
}


BOOL H45011Handler::OnReceivedInvoke(int opcode,
                                    int invokeId,
                                    int linkedId,
                                    PASN_OctetString * argument)
{
  BOOL result = TRUE;
  currentInvokeId = invokeId;

  switch (opcode) {
    case H45011_H323CallIntrusionOperations::e_callIntrusionRequest:
      OnReceivedCallIntrusionRequest(linkedId, argument);
      break;

    case H45011_H323CallIntrusionOperations::e_callIntrusionGetCIPL:
      OnReceivedCallIntrusionGetCIPL(linkedId, argument);
      break;

    case H45011_H323CallIntrusionOperations::e_callIntrusionIsolate:
      OnReceivedCallIntrusionIsolate(linkedId, argument);
      break;

    case H45011_H323CallIntrusionOperations::e_callIntrusionForcedRelease:
      result = OnReceivedCallIntrusionForcedRelease(linkedId, argument);
      break;

    case H45011_H323CallIntrusionOperations::e_callIntrusionWOBRequest:
      OnReceivedCallIntrusionWOBRequest(linkedId, argument);
      break;

    case H45011_H323CallIntrusionOperations::e_callIntrusionSilentMonitor:
      OnReceivedCallIntrusionSilentMonitor(linkedId, argument);
      break;

    case H45011_H323CallIntrusionOperations::e_callIntrusionNotification:
      OnReceivedCallIntrusionNotification(linkedId, argument);
      break;

    case H45010_H323CallOfferOperations::e_cfbOverride:
      OnReceivedCfbOverride(linkedId, argument);
      break;

    case H45010_H323CallOfferOperations::e_remoteUserAlerting:
      OnReceivedRemoteUserAlerting(linkedId, argument);
      break;

    case H4506_CallWaitingOperations::e_callWaiting:
      OnReceivedCallWaiting(linkedId, argument);
      break;
    
    default:
      currentInvokeId = 0;
      return FALSE;
  }

  return result;
}


void H45011Handler::AttachToSetup(H323SignalPDU & pdu)
{
  // Do we need to attach a call transfer setup invoke APDU?
  if (ciSendState != e_ci_sAttachToSetup)
    return;

  H450ServiceAPDU serviceAPDU;

  // Store the outstanding invokeID associated with this connection
  currentInvokeId = dispatcher.GetNextInvokeId();
  PTRACE(4, "H450.11\tAttachToSetup Invoke ID=" << currentInvokeId);

  switch (ciGenerateState){
    case e_ci_gConferenceRequest:
      break;
    case e_ci_gHeldRequest:
      break;
    case e_ci_gSilentMonitorRequest:
      break;
    case e_ci_gIsolationRequest:
      break;
    case e_ci_gForcedReleaseRequest:
      serviceAPDU.BuildCallIntrusionForcedRelease(currentInvokeId, ciCICL);
      break;
    case e_ci_gWOBRequest:
      break;
    default:
      break;
  }
  
  if(ciGenerateState != e_ci_gIdle){
    // Use the call identity from the ctInitiateArg
    serviceAPDU.AttachSupplementaryServiceAPDU(pdu);
    // start timer CT-T1
    PTRACE(4, "H450.11\tStarting timer CI-T1");
    StartciTimer(connection.GetEndPoint().GetCallIntrusionT1());
    ciState = e_ci_WaitAck;
  }
 
  ciSendState = e_ci_sIdle;
  ciGenerateState = e_ci_gIdle;
}


void H45011Handler::AttachToAlerting(H323SignalPDU & pdu)
{
  if (ciSendState != e_ci_sAttachToAlerting)
    return;

  PTRACE(4, "H450.11\tAttachToAlerting Invoke ID=" << currentInvokeId);

  // Store the outstanding invokeID associated with this connection
  currentInvokeId = dispatcher.GetNextInvokeId();
  PTRACE(4, "H450.11\tAttachToAlerting Invoke ID=" << currentInvokeId);
  if(ciReturnState!=e_ci_rIdle){
    H450ServiceAPDU serviceAPDU;
    switch (ciReturnState){
      case e_ci_rCallIntrusionImpending:
        serviceAPDU.BuildCallIntrusionImpending(currentInvokeId);
        PTRACE(4, "H450.11\tReturned e_ci_rCallIntrusionImpending");
        break;
      case e_ci_rCallIntruded:
        break;
      case e_ci_rCallIsolated:
        break;
      case e_ci_rCallForceReleased:
        break;
      case e_ci_rCallForceReleaseResult:
        serviceAPDU.BuildCallIntrusionForcedReleaseResult(currentInvokeId);
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionForced Release Result");
        break;
      case e_ci_rCallIntrusionComplete:
        break;
      case e_ci_rCallIntrusionEnd:
        break;
      case e_ci_rNotBusy:
        serviceAPDU.BuildReturnError(currentInvokeId, H45011_CallIntrusionErrors::e_notBusy);
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionErrors::e_notBusy");
        break;
      case e_ci_rTempUnavailable:
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionErrors::e_temporarilyUnavailable");
        serviceAPDU.BuildReturnError(currentInvokeId, H45011_CallIntrusionErrors::e_temporarilyUnavailable);
        break;
      case e_ci_rNotAuthorized:
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionErrors::e_notAuthorized");
        serviceAPDU.BuildReturnError(currentInvokeId, H45011_CallIntrusionErrors::e_notAuthorized);
        break;
      default :
        break;
    }
    serviceAPDU.AttachSupplementaryServiceAPDU(pdu);
  }

  ciState = e_ci_Idle;
  ciSendState = e_ci_sIdle;
  ciReturnState = e_ci_rIdle;
}


void H45011Handler::AttachToConnect(H323SignalPDU & pdu)
{
  if ((currentInvokeId == 0) || (ciSendState != e_ci_sAttachToConnect))
    return;

  currentInvokeId = dispatcher.GetNextInvokeId();
  PTRACE(4, "H450.11\tAttachToConnect Invoke ID=" << currentInvokeId);
  if(ciReturnState!=e_ci_rIdle){
    H450ServiceAPDU serviceAPDU;
    switch (ciReturnState){
      case e_ci_rCallIntrusionImpending:
        break;
      case e_ci_rCallIntruded:
        break;
      case e_ci_rCallIsolated:
        break;
      case e_ci_rCallForceReleased:
        break;
      case e_ci_rCallForceReleaseResult:
        serviceAPDU.BuildCallIntrusionForcedReleaseResult(currentInvokeId);
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionForced Release Result");
        break;
      case e_ci_rCallIntrusionComplete:
        break;
      case e_ci_rCallIntrusionEnd:
        break;
      case e_ci_rNotBusy:
        serviceAPDU.BuildReturnError(currentInvokeId, H45011_CallIntrusionErrors::e_notBusy);
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionErrors::e_notBusy");
        break;
      case e_ci_rTempUnavailable:
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionErrors::e_temporarilyUnavailable");
        serviceAPDU.BuildReturnError(currentInvokeId, H45011_CallIntrusionErrors::e_temporarilyUnavailable);
        break;
      case e_ci_rNotAuthorized:
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionErrors::e_notAuthorized");
        serviceAPDU.BuildReturnError(currentInvokeId, H45011_CallIntrusionErrors::e_notAuthorized);
        break;
      default :
        break;
    }

    serviceAPDU.AttachSupplementaryServiceAPDU(pdu);
  }


  ciState = e_ci_Idle;
  ciSendState = e_ci_sIdle;
  ciReturnState = e_ci_rIdle;
  currentInvokeId = 0;
}



void H45011Handler::AttachToReleaseComplete(H323SignalPDU & pdu)
{
  // Do we need to attach a call transfer setup invoke APDU?
  if (ciSendState != e_ci_sAttachToReleseComplete)
    return;

  PTRACE(4, "H450.11\tAttachToSetup Invoke ID=" << currentInvokeId);
  if(ciReturnState!=e_ci_rIdle){
    H450ServiceAPDU serviceAPDU;
    switch (ciReturnState){
      case e_ci_rNotBusy:
        serviceAPDU.BuildReturnError(currentInvokeId, H45011_CallIntrusionErrors::e_notBusy);
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionErrors::e_notBusy");
        break;
      case e_ci_rTempUnavailable:
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionErrors::e_temporarilyUnavailable");
        serviceAPDU.BuildReturnError(currentInvokeId, H45011_CallIntrusionErrors::e_temporarilyUnavailable);
        break;
      case e_ci_rNotAuthorized:
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionErrors::e_notAuthorized");
        serviceAPDU.BuildReturnError(currentInvokeId, H45011_CallIntrusionErrors::e_notAuthorized);
        break;
      case e_ci_rCallForceReleased:
        PTRACE(4, "H450.11\tReturned H45011_CallIntrusionForceRelease::e_ci_rCallForceReleased");
        serviceAPDU.BuildCallIntrusionForceRelesed(currentInvokeId);
        break;
      default :
        break;
    }
    serviceAPDU.AttachSupplementaryServiceAPDU(pdu);
  }

  ciState = e_ci_Idle;
  ciSendState = e_ci_sIdle;
  ciReturnState = e_ci_rIdle;
}


void H45011Handler::OnReceivedCallIntrusionRequest(int /*linkedId*/,
                                                   PASN_OctetString *argument)
{

  H45011_CIRequestArg ciArg;

  if(!DecodeArguments(argument, ciArg, -1))
    return;
/*
  TBD
*/
  return;
}


void H45011Handler::OnReceivedCallIntrusionGetCIPL(int /*linkedId*/,
                                                   PASN_OctetString *argument)
{
  PTRACE(4, "H450.11\tReceived GetCIPL Invoke");

  H45011_CIGetCIPLOptArg ciArg;

  // !!!!!!!!
  DecodeArguments(argument, ciArg, -1);
/*  if(!DecodeArguments(argument, ciArg, -1))
    return;
*/
  

  // Send a FACILITY message with a callTransferIdentify return result
  // Supplementary Service PDU to the transferring endpoint.
  H450ServiceAPDU serviceAPDU;

  X880_ReturnResult& result = serviceAPDU.BuildReturnResult(currentInvokeId);
  result.IncludeOptionalField(X880_ReturnResult::e_result);
  result.m_result.m_opcode.SetTag(X880_Code::e_local);
  PASN_Integer& operation = (PASN_Integer&) result.m_result.m_opcode;
  operation.SetValue(H45011_H323CallIntrusionOperations::e_callIntrusionGetCIPL);

  H45011_CIGetCIPLRes ciCIPLRes;

  ciCIPLRes.m_ciProtectionLevel = endpoint.GetCallIntrusionProtectionLevel();
  ciCIPLRes.IncludeOptionalField(H45011_CIGetCIPLRes::e_silentMonitoringPermitted);

  PPER_Stream resultStream;
  ciCIPLRes.Encode(resultStream);
  resultStream.CompleteEncoding();
  result.m_result.m_result.SetValue(resultStream);

  serviceAPDU.WriteFacilityPDU(connection);
  PTRACE(4, "H450.11\tSent GetCIPL Result CIPL=" << ciCIPLRes.m_ciProtectionLevel);
/*
  TBD
*/
  return;
}


void H45011Handler::OnReceivedCallIntrusionIsolate(int /*linkedId*/,
                                                   PASN_OctetString *argument)
{

  H45011_CIIsOptArg ciArg;

  if(!DecodeArguments(argument, ciArg, -1))
    return;
/*
  TBD
*/
  return;
}


BOOL H45011Handler::OnReceivedCallIntrusionForcedRelease(int /*linkedId*/,
                                                         PASN_OctetString *argument)
{
  BOOL result = TRUE;
  PTRACE(4, "H450.11\tReceived ForcedRelease Invoke");

  H45011_CIFrcRelArg ciArg;

  if(!DecodeArguments(argument, ciArg, -1))
    return FALSE;

  PStringList tokens = endpoint.GetAllConnections();

  if(tokens.GetSize() >1) {
    for (PINDEX i = 0; i < tokens.GetSize(); i++) {
      if(endpoint.HasConnection(tokens[i])){
        H323Connection* conn = endpoint.FindConnectionWithLock(tokens[i]);
        if (conn != NULL){
          if (conn->IsEstablished()){
            if((conn->GetLocalCallIntrusionProtectionLevel() < ciArg.m_ciCapabilityLevel)){
              activeCallToken = conn->GetCallToken();
              intrudingCallToken = connection.GetCallToken();
              conn->GetRemoteCallIntrusionProtectionLevel(connection.GetCallToken (), (unsigned)ciArg.m_ciCapabilityLevel);
              result = TRUE;
              conn->Unlock ();
              break;
            }
            else
              result = FALSE;
          }
          conn->Unlock ();
        }
      }
    }
    if(result){
      ciSendState = e_ci_sAttachToConnect;
      ciReturnState = e_ci_rCallForceReleaseResult;
      connection.SetCallIntrusion ();
    }
    else {
      ciSendState = e_ci_sAttachToReleseComplete;
      ciReturnState = e_ci_rNotAuthorized;
      connection.ClearCall(H323Connection::EndedByLocalBusy);
    }
  }
  else{
    ciSendState = e_ci_sAttachToAlerting;
    ciReturnState = e_ci_rNotBusy;
  }

  return result;
}


void H45011Handler::OnReceivedCallIntrusionWOBRequest(int /*linkedId*/,
                                                      PASN_OctetString *argument)
{

  H45011_CIWobOptArg ciArg;

  if(!DecodeArguments(argument, ciArg, -1))
    return;
/*
  TBD
*/
  return;
}


void H45011Handler::OnReceivedCallIntrusionSilentMonitor(int /*linkedId*/,
                                                         PASN_OctetString *argument)
{

  H45011_CISilentArg ciArg;

  if(!DecodeArguments(argument, ciArg, -1))
    return;
/*
  TBD
*/
  return;
}


void H45011Handler::OnReceivedCallIntrusionNotification(int /*linkedId*/,
                                                        PASN_OctetString *argument)
{

  H45011_CINotificationArg ciArg;

  if(!DecodeArguments(argument, ciArg, -1))
    return;
/*
  TBD
*/
  return;
}


void H45011Handler::OnReceivedCfbOverride(int /*linkedId*/,
                                          PASN_OctetString *argument)
{

  H45010_CfbOvrOptArg ciArg;

  if(!DecodeArguments(argument, ciArg, -1))
    return;
/*
  TBD
*/
  return;
}


void H45011Handler::OnReceivedRemoteUserAlerting(int /*linkedId*/,
                                                 PASN_OctetString *argument)
{

  H45010_RUAlertOptArg ciArg;

  if(!DecodeArguments(argument, ciArg, -1))
    return;
/*
  TBD
*/
  return;
}


void H45011Handler::OnReceivedCallWaiting(int /*linkedId*/,
                                          PASN_OctetString *argument)
{

  H4506_CallWaitingArg ciArg;

  if(!DecodeArguments(argument, ciArg, -1))
    return;
/*
  TBD
*/
  return;
}


BOOL H45011Handler::OnReceivedReturnResult(X880_ReturnResult & returnResult)
{
  PTRACE(4, "H450.11\tReceived Return Result");
  if (currentInvokeId == returnResult.m_invokeId.GetValue()) {
    PTRACE(4, "H450.11\tReceived Return Result Invoke ID=" << currentInvokeId);
    switch (ciState) {
      case e_ci_WaitAck:
        OnReceivedCIRequestResult(); 
        break;
      case e_ci_GetCIPL:
        OnReceivedCIGetCIPLResult(returnResult); 
        break;
      default :
        break;
    }
  }
  return TRUE;
}


void H45011Handler::OnReceivedCIRequestResult()
{
  PTRACE(4, "H450.11\tOnReceivedCIRequestResult");
  // stop timer CI-T1
  PTRACE(4, "H450.11\tTrying to stop timer CI-T1");
  StopciTimer();
}


void H45011Handler::OnReceivedCIGetCIPLResult(X880_ReturnResult & returnResult)
{
  PTRACE(4, "H450.11\tOnReceivedCIRequestResult");
  // Get the return result if present
  PASN_OctetString * result = NULL;
  if (returnResult.HasOptionalField(X880_ReturnResult::e_result)) {
    result = &returnResult.m_result.m_result;

    // Extract the C Party Details
    H45011_CIGetCIPLRes ciGetCIPLResult;

    PPER_Stream resultStream(*result);
    ciGetCIPLResult.Decode(resultStream);

    PTRACE(4 ,"H450.11\tReceived CIPL=" << ciGetCIPLResult.m_ciProtectionLevel );

    if (intrudingCallCICL > ciGetCIPLResult.m_ciProtectionLevel){

      // Send ciNotification.inv (ciImpending) To C
      connection.Lock();
      H450ServiceAPDU serviceAPDU;
      currentInvokeId = dispatcher.GetNextInvokeId();
      serviceAPDU.BuildCallIntrusionImpending(currentInvokeId);
      serviceAPDU.WriteFacilityPDU(connection);
      connection.Unlock();

      // Send ciNotification.inv (ciImpending) to  intruding (A)
      H323Connection* conn = endpoint.FindConnectionWithLock(intrudingCallToken);
      conn->SetIntrusionImpending ();

      //Send Ringing to intruding (A)
	  conn->AnsweringCall (conn->AnswerCallPending);

      // MUST RETURN ciNotification.inv (callForceRelesed) to active call (C) when releasing call !!!!!!
      ciSendState = e_ci_sAttachToReleseComplete;
      ciReturnState = e_ci_rCallForceReleased;
      
      //Send Forced Release Accepted when Answering call to intruding (A)
      conn->SetForcedReleaseAccepted();
      conn->Unlock ();
    }
    else {
      PTRACE(4 ,"H450.11\tCICL<CIPL -> Clear Call");
      // Clear Call with intruding (A)
      H323Connection* conn = endpoint.FindConnectionWithLock(intrudingCallToken);
      conn->SetIntrusionNotAuthorized();
      conn->Unlock();
      endpoint.ClearCall (intrudingCallToken);
    }
  }

  // stop timer CI-T5
  PTRACE(4, "H450.11\tTrying to stop timer CI-T5");
  StopciTimer();
}


BOOL H45011Handler::OnReceivedReturnError(int errorCode, X880_ReturnError &returnError)
{
  BOOL result = TRUE;
  PTRACE(4, "H450.11\tReceived Return Error CODE=" <<errorCode << ", InvokeId=" <<returnError.m_invokeId.GetValue());
  if (currentInvokeId == returnError.m_invokeId.GetValue()) {
    switch (ciState) {
      case e_ci_WaitAck:
        result = OnReceivedInvokeReturnError(errorCode);
        break;
      case e_ci_GetCIPL:
        result = OnReceivedGetCIPLReturnError(errorCode);
        break;
      default :
        break;
    }
  }
  return result;
}


BOOL H45011Handler::OnReceivedInvokeReturnError(int errorCode, const bool timerExpiry)
{
  BOOL result = FALSE;
  PTRACE(4, "H450.11\tOnReceivedInvokeReturnError CODE =" << errorCode);
  if (!timerExpiry) {
    // stop timer CI-T1
    StopciTimer();
    PTRACE(4, "H450.11\tStopping timer CI-T1");
  }
  else
    PTRACE(4, "H450.11\tTimer CI-T1 has expired awaiting a response to a callIntrusionInvoke return result.");

  currentInvokeId = 0;
  ciState = e_ci_Idle;
  ciSendState = e_ci_sIdle;

  switch(errorCode){
    case H45011_CallIntrusionErrors::e_notBusy :
      PTRACE(4, "H450.11\tH45011_CallIntrusionErrors::e_notBusy");
      result = TRUE;
      break;
    case H45011_CallIntrusionErrors::e_temporarilyUnavailable :
      PTRACE(4, "H450.11\tH45011_CallIntrusionErrors::e_temporarilyUnavailable");
      break;
    case H45011_CallIntrusionErrors::e_notAuthorized :
      PTRACE(4, "H450.11\tH45011_CallIntrusionErrors::e_notAuthorized");
      result = TRUE;
      break;
    default:
      PTRACE(4, "H450.11\tH45011_CallIntrusionErrors::DEFAULT");
      break;
  }
  return result;
}


BOOL H45011Handler::OnReceivedGetCIPLReturnError(int PTRACE_PARAM(errorCode),
                                                 const bool timerExpiry)
{
  PTRACE(4, "H450.11\tOnReceivedGetCIPLReturnError ErrorCode=" << errorCode);
  if(!timerExpiry){
    if (ciTimer.IsRunning()){
      ciTimer.Stop();
      PTRACE(4, "H450.11\tStopping timer CI-TX");
    }
  }

  // Send ciNotification.inv (ciImpending) to active call (C)
  connection.Lock();
  H450ServiceAPDU serviceAPDU;
  currentInvokeId = dispatcher.GetNextInvokeId();
  serviceAPDU.BuildCallIntrusionImpending(currentInvokeId);
  serviceAPDU.WriteFacilityPDU(connection);
  connection.Unlock();

  // Send ciNotification.inv (ciImpending) to intruding (A)
  H323Connection* conn = endpoint.FindConnectionWithLock(intrudingCallToken);
  conn->SetIntrusionImpending ();

  //Send Ringing to intruding (A)
  conn->AnsweringCall (conn->AnswerCallPending);

  ciSendState = e_ci_sAttachToReleseComplete;
  ciReturnState = e_ci_rCallForceReleased;
      
  //Forced Release Accepted to send when Answering call to intruding (A)
  conn->SetForcedReleaseAccepted();
  conn->Unlock ();

  return FALSE;
}


void H45011Handler::IntrudeCall(int CICL)
{
  ciSendState = e_ci_sAttachToSetup;
  ciGenerateState = e_ci_gForcedReleaseRequest;
  ciCICL = CICL;
}


void H45011Handler::AwaitSetupResponse(const PString & token,
                                      const PString & identity)
{
  intrudingCallToken = token;
  intrudingCallIdentity = identity;
  ciState = e_ci_WaitAck;

}


BOOL H45011Handler::GetRemoteCallIntrusionProtectionLevel(const PString & token,
                                                          unsigned intrusionCICL)
{
  if (!connection.Lock())
    return FALSE;

  intrudingCallToken = token;
  intrudingCallCICL = intrusionCICL;

  H450ServiceAPDU serviceAPDU;

  currentInvokeId = dispatcher.GetNextInvokeId();

  serviceAPDU.BuildCallIntrusionGetCIPL(currentInvokeId);

  connection.Unlock();

  if (!serviceAPDU.WriteFacilityPDU(connection))
    return FALSE;

  PTRACE(4, "H450.11\tStarting timer CI-T5");
  StartciTimer(connection.GetEndPoint().GetCallIntrusionT5());
  ciState = e_ci_GetCIPL;
  return TRUE;
}


void H45011Handler::SetIntrusionNotAuthorized()
{
  ciSendState = e_ci_sAttachToReleseComplete;
  ciReturnState = e_ci_rNotAuthorized;
}


void H45011Handler::SetIntrusionImpending()
{
  ciSendState = e_ci_sAttachToAlerting;
  ciReturnState = e_ci_rCallIntrusionImpending;
}


void H45011Handler::SetForcedReleaseAccepted()
{
  ciSendState = e_ci_sAttachToConnect;
  ciReturnState = e_ci_rCallForceReleaseResult;
  ciState = e_ci_DestNotify;

  StartciTimer(connection.GetEndPoint().GetCallIntrusionT6());
}


void H45011Handler::StopciTimer()
{
  if (ciTimer.IsRunning()){
    ciTimer.Stop();
    PTRACE(4, "H450.11\tStopping timer CI-TX");
  }
}


void H45011Handler::OnCallIntrudeTimeOut(PTimer &, INT)
{
  switch (ciState) {
    // CI-T1 Timeout
    case e_ci_WaitAck:
      PTRACE(4, "H450.11\tTimer CI-T1 has expired");
      OnReceivedInvokeReturnError(0,true);
      break;
    case e_ci_GetCIPL:
      PTRACE(4, "H450.11\tTimer CI-T5 has expired");
      OnReceivedGetCIPLReturnError(0,true);
      break;
    case e_ci_DestNotify:
      {
        PTRACE(4, "H450.11\tOnCallIntrudeTimeOut Timer CI-T6 has expired");
        // Clear the active call (call with C)
       PSyncPoint sync;
       endpoint.ClearCallSynchronous(activeCallToken, H323Connection::EndedByLocalUser, &sync);
        // Answer intruding call (call with A)
        PTRACE(4, "H450.11\tOnCallIntrudeTimeOut Trying to answer Call");
        if(endpoint.HasConnection(intrudingCallToken)){
          H323Connection* conn = endpoint.FindConnectionWithLock(intrudingCallToken);
          conn->AnsweringCall (conn->AnswerCallNow);
          conn->Unlock ();
        }
      }
      break;
    default:
      break;
  }
}


BOOL H45011Handler::OnReceivedReject(int PTRACE_PARAM(problemType), int PTRACE_PARAM(problemNumber))
{
  PTRACE(4, "H450.11\tH45011Handler::OnReceivedReject - problemType= "
         << problemType << ", problemNumber= " << problemNumber);

  if (ciTimer.IsRunning()){
    ciTimer.Stop();
    PTRACE(4, "H450.11\tStopping timer CI-TX");
  }

  switch (ciState) {
    case e_ci_GetCIPL:
    {
      H323Connection* conn = endpoint.FindConnectionWithLock(intrudingCallToken);
      conn->SetIntrusionImpending ();

      //Send Ringing to  intruding (A)
      conn->AnsweringCall (conn->AnswerCallPending);
      conn->SetForcedReleaseAccepted();  
      conn->Unlock ();
      break;
    }

    default:
      break;
  }
  ciState = e_ci_Idle;
  return TRUE;
};
