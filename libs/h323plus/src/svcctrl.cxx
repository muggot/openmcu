/*
 * svcctrl.cxx
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
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: svcctrl.cxx,v $
 * Revision 1.2  2007/08/07 22:25:47  shorne
 * update for H323_H350
 *
 * Revision 1.1  2007/08/06 20:51:08  shorne
 * First commit of h323plus
 *
 * Revision 1.2.2.1  2007/07/20 22:03:27  shorne
 * Initial H.350 Support
 *
 * Revision 1.2  2006/05/16 11:37:11  shorne
 * Added ability to detect type of service control
 *
 * Revision 1.1  2003/04/01 01:06:28  robertj
 * Split service control handlers from H.225 RAS header.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "svcctrl.h"
#endif

#include "h323ep.h"
#include "h323pdu.h"
#include "h248.h"

#include "svcctrl.h"

#ifdef H323_H350
  #include "h350/h350_service.h"
#endif

#define new PNEW


/////////////////////////////////////////////////////////////////////////////

H323ServiceControlSession::H323ServiceControlSession()
{
}


PString H323ServiceControlSession::GetServiceControlType() const
{
  return GetClass();
}


/////////////////////////////////////////////////////////////////////////////

H323HTTPServiceControl::H323HTTPServiceControl(const PString & u)
  : url(u)
{
}


H323HTTPServiceControl::H323HTTPServiceControl(const H225_ServiceControlDescriptor & contents)
{
  OnReceivedPDU(contents);
}


BOOL H323HTTPServiceControl::IsValid() const
{
  return !url.IsEmpty();
}


PString H323HTTPServiceControl::GetServiceControlType() const
{
  return e_URL;
}


BOOL H323HTTPServiceControl::OnReceivedPDU(const H225_ServiceControlDescriptor & contents)
{
  if (contents.GetTag() != H225_ServiceControlDescriptor::e_url)
    return FALSE;

  const PASN_IA5String & pdu = contents;
  url = pdu;
  return TRUE;
}


BOOL H323HTTPServiceControl::OnSendingPDU(H225_ServiceControlDescriptor & contents) const
{
  contents.SetTag(H225_ServiceControlDescriptor::e_url);
  PASN_IA5String & pdu = contents;
  pdu = url;

  return TRUE;
}


void H323HTTPServiceControl::OnChange(unsigned type,
                                      unsigned sessionId,
                                      H323EndPoint & endpoint,
                                      H323Connection * /*connection*/) const
{
  PTRACE(2, "SvcCtrl\tOnChange HTTP service control " << url);

  endpoint.OnHTTPServiceControl(type, sessionId, url);
}


/////////////////////////////////////////////////////////////////////////////

H323H248ServiceControl::H323H248ServiceControl()
{
}


H323H248ServiceControl::H323H248ServiceControl(const H225_ServiceControlDescriptor & contents)
{
  OnReceivedPDU(contents);
}


BOOL H323H248ServiceControl::OnReceivedPDU(const H225_ServiceControlDescriptor & contents)
{
  if (contents.GetTag() != H225_ServiceControlDescriptor::e_signal)
    return FALSE;

  const H225_H248SignalsDescriptor & pdu = contents;

  H248_SignalsDescriptor signal;
  if (!pdu.DecodeSubType(signal))
    return FALSE;

  return OnReceivedPDU(signal);
}


BOOL H323H248ServiceControl::OnSendingPDU(H225_ServiceControlDescriptor & contents) const
{
  contents.SetTag(H225_ServiceControlDescriptor::e_signal);
  H225_H248SignalsDescriptor & pdu = contents;

  H248_SignalsDescriptor signal;

  pdu.EncodeSubType(signal);

  return OnSendingPDU(signal);
}


BOOL H323H248ServiceControl::OnReceivedPDU(const H248_SignalsDescriptor & descriptor)
{
  for (PINDEX i = 0; i < descriptor.GetSize(); i++) {
    if (!OnReceivedPDU(descriptor[i]))
      return FALSE;
  }

  return TRUE;
}


BOOL H323H248ServiceControl::OnSendingPDU(H248_SignalsDescriptor & descriptor) const
{
  PINDEX last = descriptor.GetSize();
  descriptor.SetSize(last+1);
  return OnSendingPDU(descriptor[last]);
}


/////////////////////////////////////////////////////////////////////////////

H323CallCreditServiceControl::H323CallCreditServiceControl(const PString & amt,
                                                           BOOL m,
                                                           unsigned dur)
  : amount(amt),
    mode(m),
    durationLimit(dur)
{
}


H323CallCreditServiceControl::H323CallCreditServiceControl(const H225_ServiceControlDescriptor & contents)
{
  OnReceivedPDU(contents);
}


BOOL H323CallCreditServiceControl::IsValid() const
{
  return !amount || durationLimit > 0;
}


BOOL H323CallCreditServiceControl::OnReceivedPDU(const H225_ServiceControlDescriptor & contents)
{
  if (contents.GetTag() != H225_ServiceControlDescriptor::e_callCreditServiceControl)
    return FALSE;

  const H225_CallCreditServiceControl & credit = contents;

  if (credit.HasOptionalField(H225_CallCreditServiceControl::e_amountString))
    amount = credit.m_amountString;

  if (credit.HasOptionalField(H225_CallCreditServiceControl::e_billingMode))
    mode = credit.m_billingMode.GetTag() == H225_CallCreditServiceControl_billingMode::e_debit;
  else
    mode = TRUE;

  if (credit.HasOptionalField(H225_CallCreditServiceControl::e_callDurationLimit))
    durationLimit = credit.m_callDurationLimit;
  else
    durationLimit = 0;

  return TRUE;
}


BOOL H323CallCreditServiceControl::OnSendingPDU(H225_ServiceControlDescriptor & contents) const
{
  contents.SetTag(H225_ServiceControlDescriptor::e_callCreditServiceControl);
  H225_CallCreditServiceControl & credit = contents;

  if (!amount) {
    credit.IncludeOptionalField(H225_CallCreditServiceControl::e_amountString);
    credit.m_amountString = amount;

    credit.IncludeOptionalField(H225_CallCreditServiceControl::e_billingMode);
    credit.m_billingMode.SetTag(mode ? H225_CallCreditServiceControl_billingMode::e_debit
                                     : H225_CallCreditServiceControl_billingMode::e_credit);
  }

  if (durationLimit > 0) {
    credit.IncludeOptionalField(H225_CallCreditServiceControl::e_callDurationLimit);
    credit.m_callDurationLimit = durationLimit;
    credit.IncludeOptionalField(H225_CallCreditServiceControl::e_enforceCallDurationLimit);
    credit.m_enforceCallDurationLimit = TRUE;
  }

  return !amount || durationLimit > 0;
}


void H323CallCreditServiceControl::OnChange(unsigned /*type*/,
                                             unsigned /*sessionId*/,
                                             H323EndPoint & endpoint,
                                             H323Connection * connection) const
{
  PTRACE(2, "SvcCtrl\tOnChange Call Credit service control "
         << amount << (mode ? " debit " : " credit ") << durationLimit);

  endpoint.OnCallCreditServiceControl(amount, mode, durationLimit);
  if (durationLimit > 0 && connection != NULL)
    connection->SetEnforcedDurationLimit(durationLimit);
}

/////////////////////////////////////////////////////////////////////////////

#ifdef H323_H350

static const char * LDAPServiceOID = "1.3.6.1.4.1.17090.2.1";

H323H350ServiceControl::H323H350ServiceControl(const PString & _ldapURL, const PString & _ldapDN)
: ldapURL(_ldapURL),ldapDN(_ldapDN)
{
}


H323H350ServiceControl::H323H350ServiceControl(const H225_ServiceControlDescriptor & contents)
{
  OnReceivedPDU(contents);
}


BOOL H323H350ServiceControl::IsValid() const
{
  return !ldapURL.IsEmpty();
}


PString H323H350ServiceControl::GetServiceControlType() const
{
  return e_NonStandard;
}


BOOL H323H350ServiceControl::OnReceivedPDU(const H225_ServiceControlDescriptor & contents)
{

  if (contents.GetTag() != H225_ServiceControlDescriptor::e_nonStandard)
        return FALSE;

  const H225_NonStandardParameter & pdu = contents;
  const H225_NonStandardIdentifier & id = pdu.m_nonStandardIdentifier;
  if (id.GetTag() != H225_NonStandardIdentifier::e_object)
	  return FALSE;

  const PASN_ObjectId & i = id;
  if (i.AsString() != LDAPServiceOID)
	  return FALSE;

  const PASN_OctetString & data = pdu.m_data;

  PPER_Stream argStream(data);
  H225_H350ServiceControl svc;
  if (!svc.Decode(argStream)) 
	  return FALSE;

  ldapURL = svc.m_ldapURL;
  ldapDN = svc.m_ldapDN;

  return TRUE;

}


BOOL H323H350ServiceControl::OnSendingPDU(H225_ServiceControlDescriptor & contents) const
{

  contents.SetTag(H225_ServiceControlDescriptor::e_nonStandard);

  H225_NonStandardParameter & pdu = contents;
  H225_NonStandardIdentifier & id = pdu.m_nonStandardIdentifier;
  id.SetTag(H225_NonStandardIdentifier::e_object);
  PASN_ObjectId i = id;
  i.SetValue(LDAPServiceOID);

  PASN_OctetString & data = pdu.m_data;

  H225_H350ServiceControl svc;
    svc.m_ldapURL = ldapURL;
    svc.m_ldapDN = ldapDN;

  data.EncodeSubType(svc);

  return TRUE;

}


void H323H350ServiceControl::OnChange(unsigned type,
                                      unsigned sessionId,
                                      H323EndPoint & endpoint,
                                      H323Connection * /*connection*/) const
{
  PTRACE(2, "SvcCtrl\tOnChange H350 service control ");

  endpoint.OnH350ServiceControl(ldapURL,ldapDN);
}

#endif

/////////////////////////////////////////////////////////////////////////////
