/*
 * t38proto.cxx
 *
 * T.38 protocol handler
 *
 * Open Phone Abstraction Library
 *
 * Copyright (c) 1998-2002 Equivalence Pty. Ltd.
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
 * Contributor(s): Vyacheslav Frolov.
 *
 * $Log: t38proto.cxx,v $
 * Revision 1.1  2007/08/06 20:51:08  shorne
 * First commit of h323plus
 *
 * Revision 1.19  2006/07/05 04:37:44  csoutheren
 * Applied 1488904 - SetPromiscuous(AcceptFromLastReceivedOnly) for T.38
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.18  2004/07/03 06:51:37  rjongbloed
 * Added PTRACE_PARAM() macro to fix warnings on parameters used in PTRACE
 *  macros only.
 *
 * Revision 1.17  2002/12/19 01:49:08  robertj
 * Fixed incorrect setting of optional fields in pre-corrigendum packet
 *   translation function, thanks Vyacheslav Frolov
 *
 * Revision 1.16  2002/12/06 04:18:02  robertj
 * Fixed GNU warning
 *
 * Revision 1.15  2002/12/02 04:08:02  robertj
 * Turned T.38 Originate inside out, so now has WriteXXX() functions that can
 *   be call ed in different thread contexts.
 *
 * Revision 1.14  2002/12/02 00:37:19  robertj
 * More implementation of T38 base library code, some taken from the t38modem
 *   application by Vyacheslav Frolov, eg redundent frames.
 *
 * Revision 1.13  2002/11/21 06:40:00  robertj
 * Changed promiscuous mode to be three way. Fixes race condition in gkserver
 *   which can cause crashes or more PDUs to be sent to the wrong place.
 *
 * Revision 1.12  2002/09/25 05:20:40  robertj
 * Fixed warning on no trace version.
 *
 * Revision 1.11  2002/08/05 10:03:48  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.10  2002/02/09 04:39:05  robertj
 * Changes to allow T.38 logical channels to use single transport which is
 *   now owned by the OpalT38Protocol object instead of H323Channel.
 *
 * Revision 1.9  2002/01/01 23:27:50  craigs
 * Added CleanupOnTermination functions
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.8  2001/12/22 22:18:07  craigs
 * Canged to ignore subsequent PDUs with identical sequence numbers
 *
 * Revision 1.7  2001/12/22 01:56:51  robertj
 * Cleaned up code and allowed for repeated sequence numbers.
 *
 * Revision 1.6  2001/12/19 09:15:43  craigs
 * Added changes from Vyacheslav Frolov
 *
 * Revision 1.5  2001/12/14 08:36:36  robertj
 * More implementation of T.38, thanks Adam Lazur
 *
 * Revision 1.4  2001/11/11 23:18:53  robertj
 * MSVC warnings removed.
 *
 * Revision 1.3  2001/11/11 23:07:52  robertj
 * Some clean ups after T.38 commit, thanks Adam Lazur
 *
 * Revision 1.2  2001/11/09 05:39:54  craigs
 * Added initial T.38 support thanks to Adam Lazur
 *
 * Revision 1.1  2001/07/17 04:44:32  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "t38proto.h"
#endif

#include "t38proto.h"

#include "t38.h"
#include "transports.h"


#define new PNEW


/////////////////////////////////////////////////////////////////////////////

OpalT38Protocol::OpalT38Protocol()
{
  transport = NULL;
  autoDeleteTransport = FALSE;
  corrigendumASN = TRUE;
  indicatorRedundancy = 0;
  lowSpeedRedundancy = 0;
  highSpeedRedundancy = 0;
  lastSentSequenceNumber = -1;
}


OpalT38Protocol::~OpalT38Protocol()
{
  if (autoDeleteTransport)
    delete transport;
}


void OpalT38Protocol::CleanUpOnTermination()
{
  transport->Close();
}


void OpalT38Protocol::SetTransport(H323Transport * t, BOOL autoDelete)
{
  if (transport != t) {
    if (autoDeleteTransport)
      delete transport;

    transport = t;
  }

  autoDeleteTransport = autoDelete;
}


BOOL OpalT38Protocol::Originate()
{
  PTRACE(3, "T38\tOriginate, transport=" << *transport);

  // Application would normally override this. The default just sends
  // a "heartbeat".
  while (WriteIndicator(T38_Type_of_msg_t30_indicator::e_no_signal))
    PThread::Sleep(500);

  return FALSE;
}


BOOL OpalT38Protocol::WritePacket(const T38_IFPPacket & ifp)
{
  T38_UDPTLPacket udptl;

  // If there are redundant frames saved from last time, put them in
  if (!redundantIFPs.IsEmpty()) {
    udptl.m_error_recovery.SetTag(T38_UDPTLPacket_error_recovery::e_secondary_ifp_packets);
    T38_UDPTLPacket_error_recovery_secondary_ifp_packets & secondary = udptl.m_error_recovery;
    secondary.SetSize(redundantIFPs.GetSize());
    for (PINDEX i = 0; i < redundantIFPs.GetSize(); i++)
      secondary[i].SetValue(redundantIFPs[i]);
  }

  // Encode the current ifp, but need to do stupid things as there are two
  // versions of the ASN out there, completely incompatible.
  if (corrigendumASN || !ifp.HasOptionalField(T38_IFPPacket::e_data_field))
    udptl.m_primary_ifp_packet.EncodeSubType(ifp);
  else {
    T38_PreCorrigendum_IFPPacket old_ifp;

    old_ifp.m_type_of_msg = ifp.m_type_of_msg;

    old_ifp.IncludeOptionalField(T38_IFPPacket::e_data_field);

    PINDEX count = ifp.m_data_field.GetSize();
    old_ifp.m_data_field.SetSize(count);

    for (PINDEX i = 0 ; i < count; i++) {
      old_ifp.m_data_field[i].m_field_type = ifp.m_data_field[i].m_field_type;
      if (ifp.m_data_field[i].HasOptionalField(T38_Data_Field_subtype::e_field_data)) {
        old_ifp.m_data_field[i].IncludeOptionalField(T38_Data_Field_subtype::e_field_data);
        old_ifp.m_data_field[i].m_field_data = ifp.m_data_field[i].m_field_data;
      }
    }

    udptl.m_primary_ifp_packet.PASN_OctetString::EncodeSubType(old_ifp);
  }

  lastSentSequenceNumber = (lastSentSequenceNumber + 1) & 0xffff;
  udptl.m_seq_number = lastSentSequenceNumber;

  PPER_Stream rawData;
  udptl.Encode(rawData);

#if PTRACING
  if (PTrace::CanTrace(4)) {
    PTRACE(4, "T38\tSending PDU:\n  "
           << setprecision(2) << ifp << "\n "
           << setprecision(2) << udptl << "\n "
           << setprecision(2) << rawData);
  }
  else {
    PTRACE(3, "T38\tSending PDU:"
              " seq=" << lastSentSequenceNumber <<
              " type=" << ifp.m_type_of_msg.GetTagName());
  }
#endif

  if (!transport->WritePDU(rawData)) {
    PTRACE(1, "T38\tWritePacket error: " << transport->GetErrorText());
    return FALSE;
  }

  // Calculate the level of redundency for this data phase
  PINDEX maxRedundancy;
  if (ifp.m_type_of_msg.GetTag() == T38_Type_of_msg::e_t30_indicator)
    maxRedundancy = indicatorRedundancy;
  else if ((T38_Type_of_msg_data)ifp.m_type_of_msg  == T38_Type_of_msg_data::e_v21)
    maxRedundancy = lowSpeedRedundancy;
  else
    maxRedundancy = highSpeedRedundancy;

  // Push down the current ifp into redundant data
  if (maxRedundancy > 0)
    redundantIFPs.InsertAt(0, new PBYTEArray(udptl.m_primary_ifp_packet.GetValue()));

  // Remove redundant data that are surplus to requirements
  while (redundantIFPs.GetSize() > maxRedundancy)
    redundantIFPs.RemoveAt(maxRedundancy);

  return TRUE;
}


BOOL OpalT38Protocol::WriteIndicator(unsigned indicator)
{
  T38_IFPPacket ifp;

  ifp.SetTag(T38_Type_of_msg::e_t30_indicator);
  T38_Type_of_msg_t30_indicator & ind = ifp.m_type_of_msg;
  ind.SetValue(indicator);

  return WritePacket(ifp);
}


BOOL OpalT38Protocol::WriteMultipleData(unsigned mode,
                                        PINDEX count,
                                        unsigned * type,
                                        const PBYTEArray * data)
{
  T38_IFPPacket ifp;

  ifp.SetTag(T38_Type_of_msg::e_data);
  T38_Type_of_msg_data & datamode = ifp.m_type_of_msg;
  datamode.SetValue(mode);

  ifp.IncludeOptionalField(T38_IFPPacket::e_data_field);
  ifp.m_data_field.SetSize(count);
  for (PINDEX i = 0; i < count; i++) {
    ifp.m_data_field[i].m_field_type.SetValue(type[i]);
    ifp.m_data_field[i].m_field_data.SetValue(data[i]);
  }

  return WritePacket(ifp);
}


BOOL OpalT38Protocol::WriteData(unsigned mode, unsigned type, const PBYTEArray & data)
{
  return WriteMultipleData(mode, 1, &type, &data);
}


BOOL OpalT38Protocol::Answer()
{
  PTRACE(3, "T38\tAnswer, transport=" << *transport);

  // We can't get negotiated sender's address and port,
  // so accept first packet from any address and port
  transport->SetPromiscuous(H323Transport::AcceptFromAny);

  int consecutiveBadPackets = 0;
  int expectedSequenceNumber = 0;	// 16 bit
  BOOL firstPacket = TRUE;

  for (;;) {
    PPER_Stream rawData;
    if (!transport->ReadPDU(rawData)) {
      PTRACE(1, "T38\tError reading PDU: " << transport->GetErrorText(PChannel::LastReadError));
      return FALSE;
    }

    // Decode the PDU
    T38_UDPTLPacket udptl;
    if (udptl.Decode(rawData)) {
      consecutiveBadPackets = 0;

      // When we get the first packet, we know sender's address and port,
      // so accept next packets from sender's address and port only
      if (firstPacket) {
        PTRACE(3, "T38\tReceived first packet, remote=" << transport->GetLastReceivedAddress());
        transport->SetPromiscuous(H323Transport::AcceptFromLastReceivedOnly);
        firstPacket = FALSE;
      }
    } else {
      consecutiveBadPackets++;
      PTRACE(2, "T38\tRaw data decode failure:\n  "
             << setprecision(2) << rawData << "\n  UDPTL = "
             << setprecision(2) << udptl);
      if (consecutiveBadPackets > 3) {
        PTRACE(1, "T38\tRaw data decode failed multiple times, aborting!");
        return FALSE;
      }
      continue;
    }

    unsigned receivedSequenceNumber = udptl.m_seq_number;

#if PTRACING
    if (PTrace::CanTrace(5)) {
      PTRACE(4, "T38\tReceived UDPTL packet:\n  "
             << setprecision(2) << rawData << "\n  "
             << setprecision(2) << udptl);
    }
    if (PTrace::CanTrace(4)) {
      PTRACE(4, "T38\tReceived UDPTL packet:\n  " << setprecision(2) << udptl);
    }
    else {
      PTRACE(3, "T38\tReceived UDPTL packet: seq=" << receivedSequenceNumber);
    }
#endif

    // Calculate the number of lost packets, if the number lost is really
    // really big then it means it is actually a packet arriving out of order
    int lostPackets = (receivedSequenceNumber - expectedSequenceNumber)&0xffff;
    if (lostPackets > 32767) {
      PTRACE(3, "T38\tIgnoring out of order packet");
      continue;
    }

    expectedSequenceNumber = (WORD)(receivedSequenceNumber+1);

    // See if this is the expected packet
    if (lostPackets > 0) {
      // Not what was expected, see if we have enough redundant data
      if (udptl.m_error_recovery.GetTag() == T38_UDPTLPacket_error_recovery::e_secondary_ifp_packets) {
        T38_UDPTLPacket_error_recovery_secondary_ifp_packets & secondary = udptl.m_error_recovery;
        int nRedundancy = secondary.GetSize();
        if (lostPackets >= nRedundancy) {
          if (!HandlePacketLost(lostPackets - nRedundancy)) {
            PTRACE(1, "T38\tHandle packet failed, aborting answer");
            return FALSE;
          }
          lostPackets = nRedundancy;
        }
        while (lostPackets > 0) {
          if (!HandleRawIFP(secondary[lostPackets++])) {
            PTRACE(1, "T38\tHandle packet failed, aborting answer");
            return FALSE;
          }
        }
      }
      else {
        if (!HandlePacketLost(lostPackets)) {
          PTRACE(1, "T38\tHandle lost packet, aborting answer");
          return FALSE;
        }
      }
    }

    if (!HandleRawIFP(udptl.m_primary_ifp_packet)) {
      PTRACE(1, "T38\tHandle packet failed, aborting answer");
      return FALSE;
    }
  }
}


BOOL OpalT38Protocol::HandleRawIFP(const PASN_OctetString & pdu)
{
  T38_IFPPacket ifp;

  if (corrigendumASN) {
    if (pdu.DecodeSubType(ifp))
      return HandlePacket(ifp);

    PTRACE(2, "T38\tIFP decode failure:\n  " << setprecision(2) << ifp);
    return TRUE;
  }

  T38_PreCorrigendum_IFPPacket old_ifp;
  if (!pdu.DecodeSubType(old_ifp)) {
    PTRACE(2, "T38\tPre-corrigendum IFP decode failure:\n  " << setprecision(2) << old_ifp);
    return TRUE;
  }

  ifp.m_type_of_msg = old_ifp.m_type_of_msg;

  if (old_ifp.HasOptionalField(T38_IFPPacket::e_data_field)) {
    ifp.IncludeOptionalField(T38_IFPPacket::e_data_field);
    PINDEX count = old_ifp.m_data_field.GetSize();
    ifp.m_data_field.SetSize(count);
    for (PINDEX i = 0 ; i < count; i++) {
      ifp.m_data_field[i].m_field_type = old_ifp.m_data_field[i].m_field_type;
      if (old_ifp.m_data_field[i].HasOptionalField(T38_Data_Field_subtype::e_field_data)) {
        ifp.m_data_field[i].IncludeOptionalField(T38_Data_Field_subtype::e_field_data);
        ifp.m_data_field[i].m_field_data = old_ifp.m_data_field[i].m_field_data;
      }
    }
  }

  return HandlePacket(ifp);
}


BOOL OpalT38Protocol::HandlePacket(const T38_IFPPacket & ifp)
{
  if (ifp.m_type_of_msg.GetTag() == T38_Type_of_msg::e_t30_indicator)
    return OnIndicator((T38_Type_of_msg_t30_indicator)ifp.m_type_of_msg);

  for (PINDEX i = 0; i < ifp.m_data_field.GetSize(); i++) {
    if (!OnData((T38_Type_of_msg_data)ifp.m_type_of_msg,
                ifp.m_data_field[i].m_field_type,
                ifp.m_data_field[i].m_field_data.GetValue()))
      return FALSE;
  }
  return TRUE;
}


BOOL OpalT38Protocol::OnIndicator(unsigned indicator)
{
  switch (indicator) {
    case T38_Type_of_msg_t30_indicator::e_no_signal :
      break;

    case T38_Type_of_msg_t30_indicator::e_cng :
      return OnCNG();

    case T38_Type_of_msg_t30_indicator::e_ced :
      return OnCED();

    case T38_Type_of_msg_t30_indicator::e_v21_preamble :
      return OnPreamble();

    case T38_Type_of_msg_t30_indicator::e_v27_2400_training :
    case T38_Type_of_msg_t30_indicator::e_v27_4800_training :
    case T38_Type_of_msg_t30_indicator::e_v29_7200_training :
    case T38_Type_of_msg_t30_indicator::e_v29_9600_training :
    case T38_Type_of_msg_t30_indicator::e_v17_7200_short_training :
    case T38_Type_of_msg_t30_indicator::e_v17_7200_long_training :
    case T38_Type_of_msg_t30_indicator::e_v17_9600_short_training :
    case T38_Type_of_msg_t30_indicator::e_v17_9600_long_training :
    case T38_Type_of_msg_t30_indicator::e_v17_12000_short_training :
    case T38_Type_of_msg_t30_indicator::e_v17_12000_long_training :
    case T38_Type_of_msg_t30_indicator::e_v17_14400_short_training :
    case T38_Type_of_msg_t30_indicator::e_v17_14400_long_training :
      return OnTraining(indicator);

    default:
      break;
  }

  return TRUE;
}


BOOL OpalT38Protocol::OnCNG()
{
  return TRUE;
}


BOOL OpalT38Protocol::OnCED()
{
  return TRUE;
}


BOOL OpalT38Protocol::OnPreamble()
{
  return TRUE;
}


BOOL OpalT38Protocol::OnTraining(unsigned /*indicator*/)
{
  return TRUE;
}


BOOL OpalT38Protocol::OnData(unsigned /*mode*/,
                             unsigned /*type*/,
                             const PBYTEArray & /*data*/)
{
  return TRUE;
}


BOOL OpalT38Protocol::HandlePacketLost(unsigned PTRACE_PARAM(nLost))
{
  PTRACE(2, "T38\tHandlePacketLost, n=" << nLost);
  /* don't handle lost packets yet */
  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
