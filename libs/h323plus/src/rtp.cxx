/*
 * rtp.cxx
 *
 * RTP protocol handler
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
 * Portions of this code were written with the assisance of funding from
 * Vovida Networks, Inc. http://www.vovida.com.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: rtp.cxx,v $
 * Revision 1.3  2008/02/06 02:52:59  shorne
 * Added support for Standards based NAT Traversal
 *
 * Revision 1.2  2007/10/16 17:03:54  shorne
 * Qos capability negotiation
 *
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.106.2.5  2007/08/02 20:30:44  shorne
 * Added trace to determine which NAT method opened what ports
 *
 * Revision 1.106.2.4  2007/07/23 21:47:11  shorne
 * Added QoS GK Reporting
 *
 * Revision 1.106.2.3  2007/05/23 07:02:38  shorne
 * Removed VS 2005 compile warning
 *
 * Revision 1.106.2.2  2007/02/17 15:26:20  shorne
 * Support static memory allocation of RTP_DataFrame for Plugin Video Support
 *
 * Revision 1.106.2.1  2006/12/23 19:08:03  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.106  2006/01/26 03:35:04  shorne
 * Added more NAT traversal support
 *
 * Revision 1.105  2006/01/18 07:46:08  csoutheren
 * Initial version of RTP aggregation (disabled by default)
 *
 * Revision 1.104  2005/11/21 21:06:23  shorne
 * Added GQoS switches
 *
 * Revision 1.103  2005/08/27 02:06:01  csoutheren
 * Keep track of time first RTP packet is received
 *
 * Revision 1.102  2005/01/16 20:39:44  csoutheren
 * Fixed problem with IPv6 INADDR_ANY
 *
 * Revision 1.101  2005/01/04 08:08:46  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.100  2005/01/03 14:03:42  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.99  2005/01/03 06:26:09  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.98  2004/07/03 06:51:37  rjongbloed
 * Added PTRACE_PARAM() macro to fix warnings on parameters used in PTRACE
 *  macros only.
 *
 * Revision 1.97  2004/05/02 18:49:08  ykiryanov
 * Removed ifdef BeOS to add functionality
 *
 * Revision 1.96  2004/04/24 23:58:05  rjongbloed
 * Fixed GCC 3.4 warning about PAssertNULL
 *
 * Revision 1.95  2004/04/22 12:38:04  rjongbloed
 * Removed the RTP QoS class if there is no QoS support in build,
 *   thanks Nick Hoath, ISDN Communications Ltd.
 *
 * Revision 1.94  2004/04/07 05:31:43  csoutheren
 * Added ability to receive calls from endpoints behind NAT firewalls
 *
 * Revision 1.93  2004/03/02 10:02:13  rjongbloed
 * Added check for unusual error in reading UDP socket, thanks Michael Smith
 *
 * Revision 1.92  2004/02/09 11:17:50  rjongbloed
 * Improved check for compound RTCP packets so does not possibly acess
 *   memory beyond that allocated in packet. Pointed out by Paul Slootman
 *
 * Revision 1.91  2003/10/28 22:35:21  dereksmithies
 * Fix warning about possible use of unitialized variable.
 *
 * Revision 1.90  2003/10/27 06:03:39  csoutheren
 * Added support for QoS
 *   Thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.89  2003/10/09 09:47:45  csoutheren
 * Fixed problem with re-opening RTP half-channels under unusual
 * circumstances. Thanks to Damien Sandras
 *
 * Revision 1.88  2003/05/02 04:57:47  robertj
 * Added header extension support to RTP data frame class.
 *
 * Revision 1.87  2003/02/07 00:30:21  robertj
 * Changes for bizarre usage of RTP code outside of scope of H.323 specs.
 *
 * Revision 1.86  2003/02/04 23:50:06  robertj
 * Changed trace log for RTP session creation to show local address.
 *
 * Revision 1.85  2003/02/04 07:06:42  robertj
 * Added STUN support.
 *
 * Revision 1.84  2003/01/07 06:32:22  robertj
 * Fixed faint possibility of getting an error on UDP write caused by ICMP
 *   unreachable being received when no UDP read is active on the socket. Then
 *   the UDP write gets the error stopping transmission.
 *
 * Revision 1.83  2002/11/19 01:48:00  robertj
 * Allowed get/set of canonical anme and tool name.
 *
 * Revision 1.82  2002/11/05 03:32:04  robertj
 * Added session ID to trace logs.
 * Fixed possible extra wait exiting RTP read loop on close.
 *
 * Revision 1.81  2002/11/05 01:55:50  robertj
 * Added comments about strange mutex usage.
 *
 * Revision 1.80  2002/10/31 00:47:07  robertj
 * Enhanced jitter buffer system so operates dynamically between minimum and
 *   maximum values. Altered API to assure app writers note the change!
 *
 * Revision 1.79  2002/09/26 04:01:49  robertj
 * Fixed calculation of fraction of packets lost in RR, thanks Awais Ali
 *
 * Revision 1.78  2002/09/03 06:15:32  robertj
 * Added copy constructor/operator for session manager.
 *
 * Revision 1.77  2002/08/05 10:03:48  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.76  2002/07/23 06:28:16  robertj
 * Added statistics call back on first sent or received RTP packet, helps with,
 *   for example, detecting if remote endpoint has started to send audio.
 *
 * Revision 1.75  2002/05/28 02:37:55  robertj
 * Fixed reading data out of RTCP compound statements.
 *
 * Revision 1.74  2002/05/21 07:12:39  robertj
 * Fixed 100% CPU usage loop for checkin on sending RTCP data if
 *   have no RTP data transferred at all.
 *
 * Revision 1.73  2002/05/02 05:58:28  robertj
 * Changed the mechanism for sending RTCP reports so that they will continue
 *   to be sent regardless of if there is any actual data traffic.
 * Added support for compound RTCP statements for sender and receiver reports.
 *
 * Revision 1.72  2002/04/18 05:48:58  robertj
 * Fixed problem with new SSRC value being ignored after RTP session has
 *    been restarted, thanks "Jacky".
 *
 * Revision 1.71  2002/02/09 02:33:49  robertj
 * Improved payload type docuemntation and added Cisco CN.
 *
 * Revision 1.70  2001/12/20 04:34:56  robertj
 * Fixed display of some of the unknown RTP types.
 *
 * Revision 1.69  2001/09/12 07:48:05  robertj
 * Fixed various problems with tracing.
 *
 * Revision 1.68  2001/09/11 00:21:24  robertj
 * Fixed missing stack sizes in endpoint for cleaner thread and jitter thread.
 *
 * Revision 1.67  2001/09/10 08:24:04  robertj
 * Fixed setting of destination RTP address so works with endpoints that
 *   do not get the OLC packets quite right.
 *
 * Revision 1.66  2001/07/11 03:23:54  robertj
 * Bug fixed where every 65536 PDUs the session thinks it is the first PDU again.
 *
 * Revision 1.65  2001/07/06 06:32:24  robertj
 * Added flag and checks for RTP data having specific SSRC.
 * Changed transmitter IP address check so is based on first received
 *    PDU instead of expecting it to come from the host we are sending to.
 *
 * Revision 1.64  2001/06/04 13:43:44  robertj
 * Fixed I/O block breaker code if RTP session is bound to INADDR_ANY interface.
 *
 * Revision 1.63  2001/06/04 11:37:50  robertj
 * Added thread safe enumeration functions of RTP sessions.
 * Added member access functions to UDP based RTP sessions.
 *
 * Revision 1.62  2001/05/24 01:12:23  robertj
 * Fixed sender report timestamp field, thanks Johan Gnosspelius
 *
 * Revision 1.61  2001/05/08 05:28:02  yurik
 * No ifdef _WIN32_WCE anymore - 3+ version of  SDK allows it
 *
 * Revision 1.60  2001/04/24 06:15:50  robertj
 * Added work around for strange Cisco bug which suddenly starts sending
 *   RTP packets beginning at a difference sequence number base.
 *
 * Revision 1.59  2001/04/02 23:58:24  robertj
 * Added jitter calculation to RTP session.
 * Added trace of statistics.
 *
 * Revision 1.58  2001/03/20 07:24:05  robertj
 * Changed RTP SDES to have simple host name instead of attempting a
 *   reverse DNS lookup on IP address as badly configured DNS can cause
 *   a 30 second delay before audio is sent.
 *
 * Revision 1.57  2001/03/15 05:45:45  robertj
 * Changed, yet again, the setting of RTP info to allow for Cisco idosyncracies.
 *
 * Revision 1.56  2001/02/21 08:08:26  robertj
 * Added more debugging.
 *
 * Revision 1.55  2001/02/09 05:13:56  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.54  2001/01/28 07:06:59  yurik
 * WinCE-port - avoid using of IP_TOS
 *
 * Revision 1.53  2000/12/18 08:59:20  craigs
 * Added ability to set ports
 *
 * Revision 1.52  2000/09/25 22:27:40  robertj
 * Fixed uninitialised variable for lastRRSequenceNumber
 *
 * Revision 1.51  2000/09/25 01:53:20  robertj
 * Fixed MSVC warnings.
 *
 * Revision 1.50  2000/09/25 01:44:13  robertj
 * Fixed possible race condition on shutdown of RTP session with jitter buffer.
 *
 * Revision 1.49  2000/09/22 00:32:34  craigs
 * Added extra logging
 * Fixed problems with no fastConnect with tunelling
 *
 * Revision 1.48  2000/09/21 02:06:07  craigs
 * Added handling for endpoints that return conformant, but useless, RTP address
 * and port numbers
 *
 * Revision 1.47  2000/08/17 00:42:39  robertj
 * Fixed RTP Goodbye message reason string parsing, thanks Thien Nguyen.
 *
 * Revision 1.46  2000/05/30 10:35:41  robertj
 * Fixed GNU compiler warning.
 *
 * Revision 1.45  2000/05/30 06:52:26  robertj
 * Fixed problem with Cisco restarting sequence numbers when changing H.323 logical channels.
 *
 * Revision 1.44  2000/05/23 12:57:37  robertj
 * Added ability to change IP Type Of Service code from applications.
 *
 * Revision 1.43  2000/05/12 00:27:35  robertj
 * Fixed bug in UseSession() that caused asserts on BSD and possible race condition everywhere.
 *
 * Revision 1.42  2000/05/04 11:52:35  robertj
 * Added Packets Too Late statistics, requiring major rearrangement of jitter
 *    buffer code, not also changes semantics of codec Write() function slightly.
 *
 * Revision 1.41  2000/05/02 04:32:27  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.40  2000/05/01 01:01:49  robertj
 * Added flag for what to do with out of orer packets (use if jitter, don't if not).
 *
 * Revision 1.39  2000/04/30 03:55:09  robertj
 * Improved the RTCP messages, epecially reports
 *
 * Revision 1.38  2000/04/28 12:56:39  robertj
 * Fixed transmission of SDES record in RTCP channel.
 *
 * Revision 1.37  2000/04/19 01:50:05  robertj
 * Improved debugging messages.
 *
 * Revision 1.36  2000/04/13 18:07:39  robertj
 * Fixed missing mutex release causing possible deadlocks.
 *
 * Revision 1.35  2000/04/10 17:40:05  robertj
 * Fixed debug output of RTP payload types to allow for unknown numbers.
 *
 * Revision 1.34  2000/04/05 04:09:24  robertj
 * Fixed portability problem with max() macro.
 *
 * Revision 1.33  2000/04/05 03:17:32  robertj
 * Added more RTP statistics gathering and H.245 round trip delay calculation.
 *
 * Revision 1.32  2000/04/03 18:15:44  robertj
 * Added "fractional" part of RTCP status NTP timestamp field.
 *
 * Revision 1.31  2000/03/23 02:54:57  robertj
 * Added sending of SDES control packets.
 *
 * Revision 1.30  2000/03/20 20:53:42  robertj
 * Fixed problem with being able to reopen for reading an RTP_Session (Cisco compatibilty)
 *
 * Revision 1.29  2000/03/04 12:32:23  robertj
 * Added setting of TOS field in IP header to get poor mans QoS on some routers.
 *
 * Revision 1.28  2000/02/29 13:00:13  robertj
 * Added extra statistic display for RTP packets out of order.
 *
 * Revision 1.27  2000/02/29 02:13:56  robertj
 * Fixed RTP receive of both control and data, ignores ECONNRESET/ECONNREFUSED errors.
 *
 * Revision 1.26  2000/02/27 10:56:24  robertj
 * Fixed error in code allowing non-consecutive RTP port numbers coming from broken stacks, thanks Vassili Leonov.
 *
 * Revision 1.25  2000/02/17 12:07:44  robertj
 * Used ne wPWLib random number generator after finding major problem in MSVC rand().
 *
 * Revision 1.24  2000/01/29 07:10:20  robertj
 * Fixed problem with RTP transmit to host that is not yet ready causing the
 *   receive side to die with ECONNRESET, thanks Ian MacDonald
 *
 * Revision 1.23  2000/01/20 05:57:46  robertj
 * Added extra flexibility in receiving incorrectly formed OpenLogicalChannel PDU's
 *
 * Revision 1.22  2000/01/14 00:31:40  robertj
 * Bug fix for RTP port allocation (MSVC optimised version), Thanks to Ian MacDonald.
 *
 * Revision 1.21  1999/12/30 09:14:49  robertj
 * Changed payload type functions to use enum.
 *
 * Revision 1.20  1999/12/23 23:02:36  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 * Revision 1.19  1999/11/29 04:50:11  robertj
 * Added adaptive threshold calculation to silence detection.
 *
 * Revision 1.18  1999/11/22 00:09:59  robertj
 * Fixed error in RTP transmit rewrite for ternary state, didn't transmit at all!
 *
 * Revision 1.17  1999/11/20 05:35:48  robertj
 * Fixed possibly I/O block in RTP read loops.
 *
 * Revision 1.16  1999/11/19 13:13:31  robertj
 * Fixed Windows 95 compatibility issue in shutdown of RTP reading.
 *
 * Revision 1.15  1999/11/19 10:23:16  robertj
 * Fixed local binding of socket to correct address on multi-homes systems.
 *
 * Revision 1.14  1999/11/19 09:17:15  robertj
 * Fixed problems with aycnhronous shut down of logical channels.
 *
 * Revision 1.13  1999/11/17 03:49:51  robertj
 * Added RTP statistics display.
 *
 * Revision 1.12  1999/11/14 11:41:07  robertj
 * Added access functions to RTP statistics.
 *
 * Revision 1.11  1999/10/18 23:55:11  robertj
 * Fixed bug in setting contributing sources, length put into wrong spot in PDU header
 *
 * Revision 1.10  1999/09/21 14:10:04  robertj
 * Removed warnings when no tracing enabled.
 *
 * Revision 1.9  1999/09/05 00:58:37  robertj
 * Removed requirement that OpenLogicalChannalAck sessionId match original OLC.
 *
 * Revision 1.8  1999/09/03 02:17:50  robertj
 * Added more debugging
 *
 * Revision 1.7  1999/08/31 12:34:19  robertj
 * Added gatekeeper support.
 *
 * Revision 1.6  1999/07/16 02:13:54  robertj
 * Slowed down the control channel statistics packets.
 *
 * Revision 1.5  1999/07/13 09:53:24  robertj
 * Fixed some problems with jitter buffer and added more debugging.
 *
 * Revision 1.4  1999/07/09 06:09:52  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.3  1999/06/22 13:49:40  robertj
 * Added GSM support and further RTP protocol enhancements.
 *
 * Revision 1.2  1999/06/14 08:42:52  robertj
 * Fixed bug in dropped packets display was negative.
 *
 * Revision 1.1  1999/06/14 06:12:25  robertj
 * Changes for using RTP sessions correctly in H323 Logical Channel context
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "rtp.h"
#endif

#if defined(_WIN32) && (_MSC_VER > 1300)
  #pragma warning(disable:4244) // warning about possible loss of data
#endif

#include "openh323buildopts.h"

#include "rtp.h"
#include "h323con.h"

#ifndef NO_H323_AUDIO_CODECS
#include "jitter.h"
#endif

#include <ptclib/random.h>

#ifdef P_STUN
#include <ptclib/pnat.h>
#endif

#if defined(H323_RTP_AGGREGATE) || defined(H323_SIGNAL_AGGREGATE)
#include <ptclib/sockagg.h>
#endif

#define new PNEW

// #undef P_HAS_QOS

const unsigned SecondsFrom1900to1970 = (70*365+17)*24*60*60U;

#define UDP_BUFFER_SIZE 32768

#define MIN_HEADER_SIZE 12


/////////////////////////////////////////////////////////////////////////////

RTP_DataFrame::RTP_DataFrame(PINDEX sz, BOOL dynamicAllocation)
  : PBYTEArray(MIN_HEADER_SIZE+sz)
{
  payloadSize = sz;
  allocatedDynamically = dynamicAllocation;
  theArray[0] = '\x80';
}


void RTP_DataFrame::SetExtension(BOOL ext)
{
  if (ext)
    theArray[0] |= 0x10;
  else
    theArray[0] &= 0xef;
}


void RTP_DataFrame::SetMarker(BOOL m)
{
  if (m)
    theArray[1] |= 0x80;
  else
    theArray[1] &= 0x7f;
}


void RTP_DataFrame::SetPayloadType(PayloadTypes t)
{
  PAssert(t <= 0x7f, PInvalidParameter);

  theArray[1] &= 0x80;
  theArray[1] |= t;
}


DWORD RTP_DataFrame::GetContribSource(PINDEX idx) const
{
  PAssert(idx < GetContribSrcCount(), PInvalidParameter);
  return ((PUInt32b *)&theArray[MIN_HEADER_SIZE])[idx];
}


void RTP_DataFrame::SetContribSource(PINDEX idx, DWORD src)
{
  PAssert(idx <= 15, PInvalidParameter);

  if (idx >= GetContribSrcCount()) {
    BYTE * oldPayload = GetPayloadPtr();
    theArray[0] &= 0xf0;
    theArray[0] |= idx+1;
    SetSize(GetHeaderSize()+payloadSize);
    memmove(GetPayloadPtr(), oldPayload, payloadSize);
  }

  ((PUInt32b *)&theArray[MIN_HEADER_SIZE])[idx] = src;
}


PINDEX RTP_DataFrame::GetHeaderSize() const
{
  PINDEX sz = MIN_HEADER_SIZE + 4*GetContribSrcCount();

  if (GetExtension())
    sz += 4 + GetExtensionSize();

  return sz;
}


int RTP_DataFrame::GetExtensionType() const
{
  if (GetExtension())
    return *(PUInt16b *)&theArray[MIN_HEADER_SIZE + 4*GetContribSrcCount()];

  return -1;
}


void RTP_DataFrame::SetExtensionType(int type)
{
  if (type < 0)
    SetExtension(FALSE);
  else {
    if (!GetExtension())
      SetExtensionSize(0);
    *(PUInt16b *)&theArray[MIN_HEADER_SIZE + 4*GetContribSrcCount()] = (WORD)type;
  }
}


PINDEX RTP_DataFrame::GetExtensionSize() const
{
  if (GetExtension())
    return *(PUInt16b *)&theArray[MIN_HEADER_SIZE + 4*GetContribSrcCount() + 2];

  return 0;
}


BOOL RTP_DataFrame::SetExtensionSize(PINDEX sz)
{
  if (!SetMinSize(MIN_HEADER_SIZE + 4*GetContribSrcCount() + 4+4*sz + payloadSize))
    return FALSE;

  SetExtension(TRUE);
  *(PUInt16b *)&theArray[MIN_HEADER_SIZE + 4*GetContribSrcCount() + 2] = (WORD)sz;
  return TRUE;
}


BYTE * RTP_DataFrame::GetExtensionPtr() const
{
  if (GetExtension())
    return (BYTE *)&theArray[MIN_HEADER_SIZE + 4*GetContribSrcCount() + 4];

  return NULL;
}


BOOL RTP_DataFrame::SetPayloadSize(PINDEX sz)
{
  payloadSize = sz;
  return SetMinSize(GetHeaderSize()+payloadSize);
}

void RTP_DataFrame::SetPadding(BOOL padding)
{
  if (padding)
    theArray[0] |= 0x20;
  else
    theArray[0] &= 0xdf;
}

BYTE * RTP_DataFrame::GetSequenceNumberPtr() const
{
    return (BYTE *)&theArray[2];
}


#if PTRACING
static const char * const PayloadTypesNames[RTP_DataFrame::LastKnownPayloadType] = {
  "PCMU",
  "FS1016",
  "G721",
  "GSM",
  "G7231",
  "DVI4_8k",
  "DVI4_16k",
  "LPC",
  "PCMA",
  "G722",
  "L16_Stereo",
  "L16_Mono",
  "G723",
  "CN",
  "MPA",
  "G728",
  "DVI4_11k",
  "DVI4_22k",
  "G729",
  "CiscoCN",
  NULL, NULL, NULL, NULL, NULL,
  "CelB",
  "JPEG",
  NULL, NULL, NULL, NULL,
  "H261",
  "MPV",
  "MP2T",
  "H263"
};

ostream & operator<<(ostream & o, RTP_DataFrame::PayloadTypes t)
{
  if ((PINDEX)t < PARRAYSIZE(PayloadTypesNames) && PayloadTypesNames[t] != NULL)
    o << PayloadTypesNames[t];
  else
    o << "[pt=" << (int)t << ']';
  return o;
}

#endif


/////////////////////////////////////////////////////////////////////////////

RTP_ControlFrame::RTP_ControlFrame(PINDEX sz)
  : PBYTEArray(sz)
{
  compoundOffset = 0;
  compoundSize = 0;
  theArray[0] = '\x80'; // Set version 2
}


void RTP_ControlFrame::SetCount(unsigned count)
{
  PAssert(count < 32, PInvalidParameter);
  theArray[compoundOffset] &= 0xe0;
  theArray[compoundOffset] |= count;
}


void RTP_ControlFrame::SetPayloadType(unsigned t)
{
  PAssert(t < 256, PInvalidParameter);
  theArray[compoundOffset+1] = (BYTE)t;
}


void RTP_ControlFrame::SetPayloadSize(PINDEX sz)
{
  sz = (sz+3)/4;
  PAssert(sz <= 0xffff, PInvalidParameter);

  compoundSize = compoundOffset+4*(sz+1);
  SetMinSize(compoundSize+1);
  *(PUInt16b *)&theArray[compoundOffset+2] = (WORD)sz;
}


BOOL RTP_ControlFrame::ReadNextCompound()
{
  compoundOffset += GetPayloadSize()+4;
  if (compoundOffset+4 > GetSize())
    return FALSE;
  return compoundOffset+GetPayloadSize()+4 <= GetSize();
}


BOOL RTP_ControlFrame::WriteNextCompound()
{
  compoundOffset += GetPayloadSize()+4;
  if (!SetMinSize(compoundOffset+4))
    return FALSE;

  theArray[compoundOffset] = '\x80'; // Set version 2
  theArray[compoundOffset+1] = 0;    // Set payload type to illegal
  theArray[compoundOffset+2] = 0;    // Set payload size to zero
  theArray[compoundOffset+3] = 0;
  return TRUE;
}


RTP_ControlFrame::SourceDescription & RTP_ControlFrame::AddSourceDescription(DWORD src)
{
  SetPayloadType(RTP_ControlFrame::e_SourceDescription);

  PINDEX index = GetCount();
  SetCount(index+1);

  PINDEX originalPayloadSize = index != 0 ? GetPayloadSize() : 0;
  SetPayloadSize(originalPayloadSize+sizeof(SourceDescription));
  SourceDescription & sdes = *(SourceDescription *)(GetPayloadPtr()+originalPayloadSize);
  sdes.src = src;
  sdes.item[0].type = e_END;
  return sdes;
}


RTP_ControlFrame::SourceDescription::Item &
        RTP_ControlFrame::AddSourceDescriptionItem(SourceDescription & sdes,
                                                   unsigned type,
                                                   const PString & data)
{
  PINDEX dataLength = data.GetLength();
  SetPayloadSize(GetPayloadSize()+sizeof(SourceDescription::Item)+dataLength-1);

  SourceDescription::Item * item = sdes.item;
  while (item->type != e_END)
    item = item->GetNextItem();

  item->type = (BYTE)type;
  item->length = (BYTE)dataLength;
  memcpy(item->data, (const char *)data, item->length);

  item->GetNextItem()->type = e_END;
  return *item;
}


void RTP_ControlFrame::ReceiverReport::SetLostPackets(unsigned packets)
{
  lost[0] = (BYTE)(packets >> 16);
  lost[1] = (BYTE)(packets >> 8);
  lost[2] = (BYTE)packets;
}


/////////////////////////////////////////////////////////////////////////////

void RTP_UserData::OnTxStatistics(const RTP_Session & /*session*/) const
{
}


void RTP_UserData::OnRxStatistics(const RTP_Session & /*session*/) const
{
}

void RTP_UserData::OnFinalStatistics(const RTP_Session & /*session*/ ) const
{
}

/////////////////////////////////////////////////////////////////////////////

RTP_Session::RTP_Session(
#ifdef H323_RTP_AGGREGATE

                         PHandleAggregator * _aggregator, 
#endif
                         unsigned id, RTP_UserData * data)
  : canonicalName(PProcess::Current().GetUserName()),
    toolName(PProcess::Current().GetName()),
    reportTimeInterval(0, 12),  // Seconds
    firstDataReceivedTime(0),
    reportTimer(reportTimeInterval)
#ifdef H323_RTP_AGGREGATE
    ,aggregator(_aggregator)
#endif
{
  PAssert(id > 0 && id < 256, PInvalidParameter);
  sessionID = (BYTE)id;

  referenceCount = 1;
  userData = data;

#ifndef NO_H323_AUDIO_CODECS
  jitter = NULL;
#endif

  ignoreOtherSources = TRUE;
  ignoreOutOfOrderPackets = TRUE;
  syncSourceOut = PRandom::Number();
  syncSourceIn = 0;
  txStatisticsInterval = 100;  // Number of data packets between tx reports
  rxStatisticsInterval = 100;  // Number of data packets between rx reports
  lastSentSequenceNumber = (WORD)PRandom::Number();
  expectedSequenceNumber = 0;
  lastRRSequenceNumber = 0;
  consecutiveOutOfOrderPackets = 0;

  packetsSent = 0;
  octetsSent = 0;
  packetsReceived = 0;
  octetsReceived = 0;
  packetsLost = 0;
  packetsOutOfOrder = 0;
  averageSendTime = 0;
  maximumSendTime = 0;
  minimumSendTime = 0;
  averageReceiveTime = 0;
  maximumReceiveTime = 0;
  minimumReceiveTime = 0;
  jitterLevel = 0;
  maximumJitterLevel = 0;

  txStatisticsCount = 0;
  rxStatisticsCount = 0;
  averageSendTimeAccum = 0;
  maximumSendTimeAccum = 0;
  minimumSendTimeAccum = 0xffffffff;
  averageReceiveTimeAccum = 0;
  maximumReceiveTimeAccum = 0;
  minimumReceiveTimeAccum = 0xffffffff;
  packetsLostSinceLastRR = 0;
  lastTransitTime = 0;

  localAddress = PString();
  remoteAddress = PString(); 

}


RTP_Session::~RTP_Session()
{
  if(userData) userData->OnFinalStatistics(*this);

  PTRACE_IF(2, packetsSent != 0 || packetsReceived != 0,
            "RTP\tFinal statistics:\n"
            "    packetsSent       = " << packetsSent << "\n"
            "    octetsSent        = " << octetsSent << "\n"
            "    averageSendTime   = " << averageSendTime << "\n"
            "    maximumSendTime   = " << maximumSendTime << "\n"
            "    minimumSendTime   = " << minimumSendTime << "\n"
            "    packetsReceived   = " << packetsReceived << "\n"
            "    octetsReceived    = " << octetsReceived << "\n"
            "    packetsLost       = " << packetsLost << "\n"
            "    packetsTooLate    = " << GetPacketsTooLate() << "\n"
            "    packetsOutOfOrder = " << packetsOutOfOrder << "\n"
            "    averageReceiveTime= " << averageReceiveTime << "\n"
            "    maximumReceiveTime= " << maximumReceiveTime << "\n"
            "    minimumReceiveTime= " << minimumReceiveTime << "\n"
            "    averageJitter     = " << (jitterLevel >> 7) << "\n"
            "    maximumJitter     = " << (maximumJitterLevel >> 7)
            );
  if(userData) delete userData;

#ifndef NO_H323_AUDIO_CODECS
  delete jitter;
#endif

}


PString RTP_Session::GetCanonicalName() const
{
  PWaitAndSignal mutex(reportMutex);
  PString s = canonicalName;
  s.MakeUnique();
  return s;
}


void RTP_Session::SetCanonicalName(const PString & name)
{
  PWaitAndSignal mutex(reportMutex);
  canonicalName = name;
}


PString RTP_Session::GetToolName() const
{
  PWaitAndSignal mutex(reportMutex);
  PString s = toolName;
  s.MakeUnique();
  return s;
}


void RTP_Session::SetToolName(const PString & name)
{
  PWaitAndSignal mutex(reportMutex);
  toolName = name;
}


void RTP_Session::SetUserData(RTP_UserData * data)
{
  if(userData) delete userData;
  userData = data;
}


void RTP_Session::SetJitterBufferSize(unsigned minJitterDelay,
                                      unsigned maxJitterDelay,
                                      PINDEX stackSize)
{
  if (minJitterDelay == 0 && maxJitterDelay == 0) {
#ifndef NO_H323_AUDIO_CODECS
    delete jitter;
    jitter = NULL;
#endif
  }
  else if (jitter != NULL) {
#ifndef NO_H323_AUDIO_CODECS
    jitter->SetDelay(minJitterDelay, maxJitterDelay);
#endif
  }
  else {
    SetIgnoreOutOfOrderPackets(FALSE);
#ifndef NO_H323_AUDIO_CODECS
    jitter = new RTP_JitterBuffer(*this, minJitterDelay, maxJitterDelay, stackSize);
    jitter->Resume(
#ifdef H323_RTP_AGGREGATE
      aggregator
#endif
      );
#endif
  }
}


unsigned RTP_Session::GetJitterBufferSize() const
{
  return
#ifndef NO_H323_AUDIO_CODECS
  jitter != NULL ? jitter->GetJitterTime() :
#endif
  0;
}


BOOL RTP_Session::ReadBufferedData(DWORD timestamp, RTP_DataFrame & frame)
{
#ifndef NO_H323_AUDIO_CODECS
  if (jitter != NULL)
    return jitter->ReadData(timestamp, frame);
  else
#endif
    return ReadData(frame, TRUE);
}


void RTP_Session::SetTxStatisticsInterval(unsigned packets)
{
  txStatisticsInterval = PMAX(packets, 2);
  txStatisticsCount = 0;
  averageSendTimeAccum = 0;
  maximumSendTimeAccum = 0;
  minimumSendTimeAccum = 0xffffffff;
}


void RTP_Session::SetRxStatisticsInterval(unsigned packets)
{
  rxStatisticsInterval = PMAX(packets, 2);
  rxStatisticsCount = 0;
  averageReceiveTimeAccum = 0;
  maximumReceiveTimeAccum = 0;
  minimumReceiveTimeAccum = 0xffffffff;
}


void RTP_Session::AddReceiverReport(RTP_ControlFrame::ReceiverReport & receiver)
{
  receiver.ssrc = syncSourceIn;
  receiver.SetLostPackets(packetsLost);

  if (expectedSequenceNumber > lastRRSequenceNumber)
    receiver.fraction = (BYTE)((packetsLostSinceLastRR<<8)/(expectedSequenceNumber - lastRRSequenceNumber));
  else
    receiver.fraction = 0;
  packetsLostSinceLastRR = 0;

  receiver.last_seq = lastRRSequenceNumber;
  lastRRSequenceNumber = expectedSequenceNumber;

  receiver.jitter = jitterLevel >> 4; // Allow for rounding protection bits

  // The following have not been calculated yet.
  receiver.lsr = 0;
  receiver.dlsr = 0;

  PTRACE(3, "RTP\tSentReceiverReport:"
            " ssrc=" << receiver.ssrc
         << " fraction=" << (unsigned)receiver.fraction
         << " lost=" << receiver.GetLostPackets()
         << " last_seq=" << receiver.last_seq
         << " jitter=" << receiver.jitter
         << " lsr=" << receiver.lsr
         << " dlsr=" << receiver.dlsr);
}


RTP_Session::SendReceiveStatus RTP_Session::OnSendData(RTP_DataFrame & frame)
{
  PTimeInterval tick = PTimer::Tick();  // Timestamp set now

  frame.SetSequenceNumber(++lastSentSequenceNumber);
  frame.SetSyncSource(syncSourceOut);

  if (packetsSent != 0 && !frame.GetMarker()) {
    // Only do statistics on subsequent packets
    DWORD diff = (tick - lastSentPacketTime).GetInterval();

    averageSendTimeAccum += diff;
    if (diff > maximumSendTimeAccum)
      maximumSendTimeAccum = diff;
    if (diff < minimumSendTimeAccum)
      minimumSendTimeAccum = diff;
    txStatisticsCount++;
  }

  lastSentTimestamp = frame.GetTimestamp();
  lastSentPacketTime = tick;

  octetsSent += frame.GetPayloadSize();
  packetsSent++;

  // Call the statistics call-back on the first PDU with total count == 1
  if (packetsSent == 1 && userData != NULL)
    userData->OnTxStatistics(*this);

  if (!SendReport())
    return e_AbortTransport;

  if (txStatisticsCount < txStatisticsInterval)
    return e_ProcessPacket;

  txStatisticsCount = 0;

  averageSendTime = averageSendTimeAccum/txStatisticsInterval;
  maximumSendTime = maximumSendTimeAccum;
  minimumSendTime = minimumSendTimeAccum;

  averageSendTimeAccum = 0;
  maximumSendTimeAccum = 0;
  minimumSendTimeAccum = 0xffffffff;

  PTRACE(2, "RTP\tTransmit statistics: "
            " packets=" << packetsSent <<
            " octets=" << octetsSent <<
            " avgTime=" << averageSendTime <<
            " maxTime=" << maximumSendTime <<
            " minTime=" << minimumSendTime
            );

  if (userData != NULL)
    userData->OnTxStatistics(*this);

  return e_ProcessPacket;
}


RTP_Session::SendReceiveStatus RTP_Session::OnReceiveData(const RTP_DataFrame & frame, const RTP_UDP & rtp)
{
  // Check that the PDU is the right version
  if (frame.GetVersion() != RTP_DataFrame::ProtocolVersion)
    return e_IgnorePacket; // Non fatal error, just ignore

  // Check for if a control packet rather than data packet.
  if (frame.GetPayloadType() > RTP_DataFrame::MaxPayloadType)
    return e_IgnorePacket; // Non fatal error, just ignore

  PTimeInterval tick = PTimer::Tick();  // Get timestamp now

  // Have not got SSRC yet, so grab it now
  if (syncSourceIn == 0)
    syncSourceIn = frame.GetSyncSource();

  // Check packet sequence numbers
  if (packetsReceived == 0) {
    expectedSequenceNumber = (WORD)(frame.GetSequenceNumber() + 1);
    firstDataReceivedTime = PTime();
    PTRACE(2, "RTP\tFirst data:"
              " ver=" << frame.GetVersion()
           << " pt=" << frame.GetPayloadType()
           << " psz=" << frame.GetPayloadSize()
           << " m=" << frame.GetMarker()
           << " x=" << frame.GetExtension()
           << " seq=" << frame.GetSequenceNumber()
           << " ts=" << frame.GetTimestamp()
           << " src=" << frame.GetSyncSource()
           << " ccnt=" << frame.GetContribSrcCount());
  }
  else {
    if (ignoreOtherSources && frame.GetSyncSource() != syncSourceIn) {
      PTRACE(2, "RTP\tPacket from SSRC=" << frame.GetSyncSource()
             << " ignored, expecting SSRC=" << syncSourceIn);
      return e_IgnorePacket; // Non fatal error, just ignore
    }

    WORD sequenceNumber = frame.GetSequenceNumber();
    if (sequenceNumber == expectedSequenceNumber) 
    {
      expectedSequenceNumber++;
      consecutiveOutOfOrderPackets = 0;
      // Only do statistics on packets after first received in talk burst
      if (!frame.GetMarker()) {
        DWORD diff = (tick - lastReceivedPacketTime).GetInterval();

        averageReceiveTimeAccum += diff;
        if (diff > maximumReceiveTimeAccum)
          maximumReceiveTimeAccum = diff;
        if (diff < minimumReceiveTimeAccum)
          minimumReceiveTimeAccum = diff;
        rxStatisticsCount++;

        // The following has the implicit assumption that something that has jitter
        // is an audio codec and thus is in 8kHz timestamp units.
        diff *= 8;
        long variance = diff - lastTransitTime;
        lastTransitTime = diff;
        if (variance < 0)
          variance = -variance;
        jitterLevel += variance - ((jitterLevel+8) >> 4);
        if (jitterLevel > maximumJitterLevel)
          maximumJitterLevel = jitterLevel;
      }
    }
    else if (sequenceNumber < expectedSequenceNumber) {
      PTRACE(3, "RTP\tOut of order packet, received "
             << sequenceNumber << " expected " << expectedSequenceNumber
             << " ssrc=" << syncSourceIn);
      packetsOutOfOrder++;

      // Check for Cisco bug where sequence numbers suddenly start incrementing
      // from a different base.
      if (++consecutiveOutOfOrderPackets > 10) {
        expectedSequenceNumber = (WORD)(sequenceNumber + 1);
        PTRACE(1, "RTP\tAbnormal change of sequence numbers, adjusting to expect "
               << expectedSequenceNumber << " ssrc=" << syncSourceIn);
      }

      if (ignoreOutOfOrderPackets)
        return e_IgnorePacket; // Non fatal error, just ignore
    }
    else {
      unsigned dropped = sequenceNumber - expectedSequenceNumber;
      packetsLost += dropped;
      packetsLostSinceLastRR += dropped;
      PTRACE(3, "RTP\tDropped " << dropped << " packet(s) at " << sequenceNumber
             << ", ssrc=" << syncSourceIn);
      expectedSequenceNumber = (WORD)(sequenceNumber + 1);
      consecutiveOutOfOrderPackets = 0;
    }
  }

  lastReceivedPacketTime = tick;

  octetsReceived += frame.GetPayloadSize();
  packetsReceived++;

  if (rtp.GetRemoteDataPort() > 0 && localAddress.IsEmpty()) {
	  localAddress = rtp.GetLocalAddress().AsString() + ":" + PString(rtp.GetLocalDataPort());
	  remoteAddress = rtp.GetRemoteAddress().AsString() + ":" + PString(rtp.GetRemoteDataPort());
  }

  // Call the statistics call-back on the first PDU with total count == 1
  if (packetsReceived == 1 && userData != NULL)
    userData->OnRxStatistics(*this);

  if (!SendReport())
    return e_AbortTransport;

  if (rxStatisticsCount < rxStatisticsInterval)
    return e_ProcessPacket;

  rxStatisticsCount = 0;

  averageReceiveTime = averageReceiveTimeAccum/rxStatisticsInterval;
  maximumReceiveTime = maximumReceiveTimeAccum;
  minimumReceiveTime = minimumReceiveTimeAccum;

  averageReceiveTimeAccum = 0;
  maximumReceiveTimeAccum = 0;
  minimumReceiveTimeAccum = 0xffffffff;
  
  PTRACE(2, "RTP\tReceive statistics: "
            " packets=" << packetsReceived <<
            " octets=" << octetsReceived <<
            " lost=" << packetsLost <<
            " tooLate=" << GetPacketsTooLate() <<
            " order=" << packetsOutOfOrder <<
            " avgTime=" << averageReceiveTime <<
            " maxTime=" << maximumReceiveTime <<
            " minTime=" << minimumReceiveTime <<
            " jitter=" << (jitterLevel >> 7) <<
            " maxJitter=" << (maximumJitterLevel >> 7)
            );

  if (userData != NULL)
    userData->OnRxStatistics(*this);

  return e_ProcessPacket;
}


BOOL RTP_Session::SendReport()
{
  PWaitAndSignal mutex(reportMutex);

  if (reportTimer.IsRunning())
    return TRUE;

  // Have not got anything yet, do nothing
  if (packetsSent == 0 && packetsReceived == 0) {
    reportTimer = reportTimeInterval;
    return TRUE;
  }

  RTP_ControlFrame report;

  // No packets sent yet, so only send RR
  if (packetsSent == 0) {
    // Send RR as we are not transmitting
    report.SetPayloadType(RTP_ControlFrame::e_ReceiverReport);
    report.SetPayloadSize(4+sizeof(RTP_ControlFrame::ReceiverReport));
    report.SetCount(1);

    PUInt32b * payload = (PUInt32b *)report.GetPayloadPtr();
    *payload = syncSourceOut;
    AddReceiverReport(*(RTP_ControlFrame::ReceiverReport *)&payload[1]);
  }
  else {
    report.SetPayloadType(RTP_ControlFrame::e_SenderReport);
    report.SetPayloadSize(sizeof(RTP_ControlFrame::SenderReport));

    RTP_ControlFrame::SenderReport * sender =
                              (RTP_ControlFrame::SenderReport *)report.GetPayloadPtr();
    sender->ssrc = syncSourceOut;
    PTime now;
    sender->ntp_sec = now.GetTimeInSeconds()+SecondsFrom1900to1970; // Convert from 1970 to 1900
    sender->ntp_frac = now.GetMicrosecond()*4294; // Scale microseconds to "fraction" from 0 to 2^32
    sender->rtp_ts = lastSentTimestamp;
    sender->psent = packetsSent;
    sender->osent = octetsSent;

    PTRACE(3, "RTP\tSentSenderReport: "
                " ssrc=" << sender->ssrc
             << " ntp=" << sender->ntp_sec << '.' << sender->ntp_frac
             << " rtp=" << sender->rtp_ts
             << " psent=" << sender->psent
             << " osent=" << sender->osent);

    if (syncSourceIn != 0) {
      report.SetPayloadSize(sizeof(RTP_ControlFrame::SenderReport) +
                            sizeof(RTP_ControlFrame::ReceiverReport));
      report.SetCount(1);
      AddReceiverReport(*(RTP_ControlFrame::ReceiverReport *)&sender[1]);
    }
  }

  // Add the SDES part to compound RTCP packet
  PTRACE(2, "RTP\tSending SDES: " << canonicalName);
  report.WriteNextCompound();

  RTP_ControlFrame::SourceDescription & sdes = report.AddSourceDescription(syncSourceOut);
  report.AddSourceDescriptionItem(sdes, RTP_ControlFrame::e_CNAME, canonicalName);
  report.AddSourceDescriptionItem(sdes, RTP_ControlFrame::e_TOOL, toolName);

  // Wait a fuzzy amount of time so things don't get into lock step
  int interval = (int)reportTimeInterval.GetMilliSeconds();
  int third = interval/3;
  interval += PRandom::Number()%(2*third);
  interval -= third;
  reportTimer = interval;

  return WriteControl(report);
}


static RTP_Session::ReceiverReportArray
                BuildReceiverReportArray(const RTP_ControlFrame & frame, PINDEX offset)
{
  RTP_Session::ReceiverReportArray reports;

  const RTP_ControlFrame::ReceiverReport * rr = (const RTP_ControlFrame::ReceiverReport *)(frame.GetPayloadPtr()+offset);
  for (PINDEX repIdx = 0; repIdx < (PINDEX)frame.GetCount(); repIdx++) {
    RTP_Session::ReceiverReport * report = new RTP_Session::ReceiverReport;
    report->sourceIdentifier = rr->ssrc;
    report->fractionLost = rr->fraction;
    report->totalLost = rr->GetLostPackets();
    report->lastSequenceNumber = rr->last_seq;
    report->jitter = rr->jitter;
    report->lastTimestamp = (PInt64)(DWORD)rr->lsr;
    report->delay = ((PInt64)rr->dlsr << 16)/1000;
    reports.SetAt(repIdx, report);
    rr++;
  }

  return reports;
}


RTP_Session::SendReceiveStatus RTP_Session::OnReceiveControl(RTP_ControlFrame & frame)
{
  do {
    BYTE * payload = frame.GetPayloadPtr();
    unsigned size = frame.GetPayloadSize();

    switch (frame.GetPayloadType()) {
      case RTP_ControlFrame::e_SenderReport :
        if (size >= sizeof(RTP_ControlFrame::SenderReport)) {
          SenderReport sender;
          const RTP_ControlFrame::SenderReport & sr = *(const RTP_ControlFrame::SenderReport *)payload;
          sender.sourceIdentifier = sr.ssrc;
          sender.realTimestamp = PTime(sr.ntp_sec-SecondsFrom1900to1970, sr.ntp_frac/4294);
          sender.rtpTimestamp = sr.rtp_ts;
          sender.packetsSent = sr.psent;
          sender.octetsSent = sr.osent;
          RTP_Session::ReceiverReportArray ra = BuildReceiverReportArray(frame, sizeof(RTP_ControlFrame::SenderReport));
          OnRxSenderReport(sender, ra);
        }
        else {
          PTRACE(2, "RTP\tSenderReport packet truncated");
        }
        break;

      case RTP_ControlFrame::e_ReceiverReport :
        if (size >= 4)
        {
          RTP_Session::ReceiverReportArray ra = BuildReceiverReportArray(frame, sizeof(PUInt32b));
          OnRxReceiverReport(*(const PUInt32b *)payload, ra);
        }
        else {
          PTRACE(2, "RTP\tReceiverReport packet truncated");
        }
        break;

      case RTP_ControlFrame::e_SourceDescription :
        if (size >= frame.GetCount()*sizeof(RTP_ControlFrame::SourceDescription)) {
          SourceDescriptionArray descriptions;
          const RTP_ControlFrame::SourceDescription * sdes = (const RTP_ControlFrame::SourceDescription *)payload;
          for (PINDEX srcIdx = 0; srcIdx < (PINDEX)frame.GetCount(); srcIdx++) {
            descriptions.SetAt(srcIdx, new SourceDescription(sdes->src));
            const RTP_ControlFrame::SourceDescription::Item * item = sdes->item;
            while (item->type != RTP_ControlFrame::e_END) {
              descriptions[srcIdx].items.SetAt(item->type, PString(item->data, item->length));
              item = item->GetNextItem();
            }
            sdes = (const RTP_ControlFrame::SourceDescription *)item->GetNextItem();
          }
          OnRxSourceDescription(descriptions);
        }
        else {
          PTRACE(2, "RTP\tSourceDescription packet truncated");
        }
        break;

      case RTP_ControlFrame::e_Goodbye :
        if (size >= 4) {
          PString str;
          unsigned count = frame.GetCount()*4;
          if (size > count)
            str = PString((const char *)(payload+count+1), payload[count]);
          PDWORDArray sources(count);
          for (PINDEX i = 0; i < (PINDEX)count; i++)
            sources[i] = ((const PUInt32b *)payload)[i];
          OnRxGoodbye(sources, str);
        }
        else {
          PTRACE(2, "RTP\tGoodbye packet truncated");
        }
        break;

      case RTP_ControlFrame::e_ApplDefined :
        if (size >= 4) {
          PString str((const char *)(payload+4), 4);
          OnRxApplDefined(str, frame.GetCount(), *(const PUInt32b *)payload,
                          payload+8, frame.GetPayloadSize()-8);
        }
        else {
          PTRACE(2, "RTP\tApplDefined packet truncated");
        }
        break;

      default :
        PTRACE(2, "RTP\tUnknown control payload type: " << frame.GetPayloadType());
    }
  } while (frame.ReadNextCompound());

  return e_ProcessPacket;
}


void RTP_Session::OnRxSenderReport(const SenderReport & PTRACE_PARAM(sender),
                                   const ReceiverReportArray & PTRACE_PARAM(reports))
{
#if PTRACING
  PTRACE(3, "RTP\tOnRxSenderReport: " << sender);
  for (PINDEX i = 0; i < reports.GetSize(); i++)
    PTRACE(3, "RTP\tOnRxSenderReport RR: " << reports[i]);
#endif
}


void RTP_Session::OnRxReceiverReport(DWORD PTRACE_PARAM(src),
                                     const ReceiverReportArray & PTRACE_PARAM(reports))
{
#if PTRACING
  PTRACE(3, "RTP\tOnReceiverReport: ssrc=" << src);
  for (PINDEX i = 0; i < reports.GetSize(); i++)
    PTRACE(3, "RTP\tOnReceiverReport RR: " << reports[i]);
#endif
}


void RTP_Session::OnRxSourceDescription(const SourceDescriptionArray & PTRACE_PARAM(description))
{
#if PTRACING
  for (PINDEX i = 0; i < description.GetSize(); i++)
    PTRACE(3, "RTP\tOnSourceDescription: " << description[i]);
#endif
}


void RTP_Session::OnRxGoodbye(const PDWORDArray & PTRACE_PARAM(src),
                              const PString & PTRACE_PARAM(reason))
{
  PTRACE(3, "RTP\tOnGoodbye: \"" << reason << "\" srcs=" << src);
}


void RTP_Session::OnRxApplDefined(const PString & PTRACE_PARAM(type),
                                  unsigned PTRACE_PARAM(subtype),
                                  DWORD PTRACE_PARAM(src),
                                  const BYTE * /*data*/, PINDEX PTRACE_PARAM(size))
{
  PTRACE(3, "RTP\tOnApplDefined: \"" << type << "\"-" << subtype
                          << " " << src << " [" << size << ']');
}


void RTP_Session::ReceiverReport::PrintOn(ostream & strm) const
{
  strm << "ssrc=" << sourceIdentifier
       << " fraction=" << fractionLost
       << " lost=" << totalLost
       << " last_seq=" << lastSequenceNumber
       << " jitter=" << jitter
       << " lsr=" << lastTimestamp
       << " dlsr=" << delay;
}


void RTP_Session::SenderReport::PrintOn(ostream & strm) const
{
  strm << "ssrc=" << sourceIdentifier
       << " ntp=" << realTimestamp.AsString("yyyy/M/d-h:m:s.uuuu")
       << " rtp=" << rtpTimestamp
       << " psent=" << packetsSent
       << " osent=" << octetsSent;
}


void RTP_Session::SourceDescription::PrintOn(ostream & strm) const
{
  static const char * const DescriptionNames[RTP_ControlFrame::NumDescriptionTypes] = {
    "END", "CNAME", "NAME", "EMAIL", "PHONE", "LOC", "TOOL", "NOTE", "PRIV"
  };

  strm << "ssrc=" << sourceIdentifier;
  for (PINDEX i = 0; i < items.GetSize(); i++) {
    strm << "\n  item[" << i << "]: type=";
    unsigned typeNum = items.GetKeyAt(i);
    if (typeNum < PARRAYSIZE(DescriptionNames))
      strm << DescriptionNames[typeNum];
    else
      strm << typeNum;
    strm << " data=\""
         << items.GetDataAt(i)
         << '"';
  }
}


DWORD RTP_Session::GetPacketsTooLate() const
{
  return
#ifndef NO_H323_AUDIO_CODECS
    jitter != NULL ? jitter->GetPacketsTooLate() :
#endif
  0;
}


/////////////////////////////////////////////////////////////////////////////

RTP_SessionManager::RTP_SessionManager()
{
  enumerationIndex = P_MAX_INDEX;
}


RTP_SessionManager::RTP_SessionManager(const RTP_SessionManager & sm)
  : sessions(sm.sessions)
{
  enumerationIndex = P_MAX_INDEX;
}


RTP_SessionManager & RTP_SessionManager::operator=(const RTP_SessionManager & sm)
{
  PWaitAndSignal m1(mutex);
  PWaitAndSignal m2(sm.mutex);
  sessions   = sm.sessions;
  return *this;
}


RTP_Session * RTP_SessionManager::UseSession(unsigned sessionID)
{
  mutex.Wait();

  RTP_Session * session = sessions.GetAt(sessionID);
  if (session == NULL)
    return NULL;  // Deliberately have not release mutex here! See AddSession.

  PTRACE(3, "RTP\tFound existing session " << sessionID);
  session->IncrementReference();

  mutex.Signal();
  return session;
}


void RTP_SessionManager::AddSession(RTP_Session * session)
{
  if (PAssertNULL(session) != NULL) {
    PTRACE(2, "RTP\tAdding session " << *session);
    sessions.SetAt(session->GetSessionID(), session);
  }

  // The following is the mutex.Signal() that was not done in the UseSession()
  mutex.Signal();
}


void RTP_SessionManager::ReleaseSession(unsigned sessionID)
{
  PTRACE(2, "RTP\tReleasing session " << sessionID);

  mutex.Wait();

  if (sessions.Contains(sessionID)) {
    if (sessions[sessionID].DecrementReference()) {
      PTRACE(3, "RTP\tDeleting session " << sessionID);
      sessions[sessionID].SetJitterBufferSize(0, 0);
      sessions.SetAt(sessionID, NULL);
    }
  }

  mutex.Signal();
}


RTP_Session * RTP_SessionManager::GetSession(unsigned sessionID) const
{
  PWaitAndSignal wait(mutex);
  if (!sessions.Contains(sessionID))
  {
    PTRACE(3, "RTP\tNot found existing session " << sessionID);
    return NULL;
  }

  //PTRACE(3, "RTP\tFound existing session " << sessionID);
  return &sessions[sessionID];
}


RTP_Session * RTP_SessionManager::First()
{
  mutex.Wait();

  enumerationIndex = 0;
  return Next();
}


RTP_Session * RTP_SessionManager::Next()
{
  if (enumerationIndex < sessions.GetSize())
    return &sessions.GetDataAt(enumerationIndex++);

  Exit();
  return NULL;
}


void RTP_SessionManager::Exit()
{
  enumerationIndex = P_MAX_INDEX;
  mutex.Signal();
}


/////////////////////////////////////////////////////////////////////////////

static void SetMinBufferSize(PUDPSocket & sock, int buftype)
{
  int sz = 0;
  if (sock.GetOption(buftype, sz)) {
    if (sz >= UDP_BUFFER_SIZE)
      return;
  }

  if (!sock.SetOption(buftype, UDP_BUFFER_SIZE)) {
    PTRACE(1, "RTP_UDP\tSetOption(" << buftype << ") failed: " << sock.GetErrorText());
  }
}


RTP_UDP::RTP_UDP(
#ifdef H323_RTP_AGGREGATE
                 PHandleAggregator * _aggregator, 
#endif
                 unsigned id, BOOL _remoteIsNAT)
  : RTP_Session(
#ifdef H323_RTP_AGGREGATE
  _aggregator, 
#endif
  id),
    remoteAddress(0),
    remoteTransmitAddress(0),
    remoteIsNAT(_remoteIsNAT)
{
  remoteDataPort = 0;
  remoteControlPort = 0;
  shutdownRead = FALSE;
  shutdownWrite = FALSE;
  dataSocket = NULL;
  controlSocket = NULL;
  appliedQOS = FALSE;
  enableGQOS = FALSE;
}


RTP_UDP::~RTP_UDP()
{
  Close(TRUE);
  Close(FALSE);

  delete dataSocket;
  delete controlSocket;
}


void RTP_UDP::ApplyQOS(const PIPSocket::Address & addr)
{
  if (dataSocket != NULL)
    dataSocket->SetSendAddress(addr,GetRemoteDataPort());
  else if (controlSocket != NULL)
    controlSocket->SetSendAddress(addr,GetRemoteControlPort());

  appliedQOS = TRUE;
}


BOOL RTP_UDP::ModifyQOS(RTP_QOS * rtpqos)
{
  BOOL retval = FALSE;

  if (rtpqos == NULL)
    return retval;

#if P_HAS_QOS
  if (dataSocket != NULL)
    retval &= dataSocket->ModifyQoSSpec(&(rtpqos->dataQoS));
  else if (controlSocket != NULL)
    retval = controlSocket->ModifyQoSSpec(&(rtpqos->ctrlQoS));
#endif

  appliedQOS = FALSE;
  return retval;
}

void RTP_UDP::EnableGQoS(BOOL success)
{
	enableGQOS = success;
}

#if P_HAS_QOS
PQoS & RTP_UDP::GetQOS()
{
    if (dataSocket != NULL) 
       return dataSocket->GetQoSSpec();
    else if (controlSocket != NULL) 
       return controlSocket->GetQoSSpec();
	else
       return *new PQoS(); 
}
#endif

BOOL RTP_UDP::Open(PIPSocket::Address _localAddress,
                   WORD portBase, WORD portMax,
                   BYTE tos,
				   const H323Connection & connection,
#ifdef P_STUN
                   PNatMethod * meth,
#else
                   void *,
#endif
                   RTP_QOS * rtpQos)
{
  // save local address 
  localAddress = _localAddress;

  localDataPort    = (WORD)(portBase&0xfffe);
  localControlPort = (WORD)(localDataPort + 1);

  delete dataSocket;
  delete controlSocket;
  dataSocket = NULL;
  controlSocket = NULL;

  PQoS * dataQos = NULL;
  PQoS * ctrlQos = NULL;
  if (rtpQos != NULL) {
#if P_HAS_QOS
    dataQos = &(rtpQos->dataQoS);
    ctrlQos = &(rtpQos->ctrlQoS);
#endif
  }

#ifdef P_STUN
  if (meth != NULL) {
	connection.OnSetRTPNat(GetSessionID(),*meth);

    if (meth->CreateSocketPair(dataSocket, controlSocket)) {
      dataSocket->GetLocalAddress(localAddress, localDataPort);
      controlSocket->GetLocalAddress(localAddress, localControlPort);
	  PTRACE(4, "RTP\tNAT Method " << meth->GetName() << " created NAT ports " << localDataPort << " " << localControlPort);
    }
    else
      PTRACE(1, "RTP\tNAT could not create socket pair!");
  }
#endif

  if (dataSocket == NULL || controlSocket == NULL) {
    dataSocket = new PUDPSocket(dataQos);
    controlSocket = new PUDPSocket(ctrlQos);
    while (!dataSocket->Listen(localAddress,    1, localDataPort) ||
           !controlSocket->Listen(localAddress, 1, localControlPort)) {
      dataSocket->Close();
      controlSocket->Close();
      if ((localDataPort > portMax) || (localDataPort > 0xfffd))
        return FALSE; // If it ever gets to here the OS has some SERIOUS problems!
      localDataPort    += 2;
      localControlPort += 2;
    }
  }

  // Set the IP Type Of Service field for prioritisation of media UDP packets
  // through some Cisco routers and Linux boxes
  if (!dataSocket->SetOption(IP_TOS, tos, IPPROTO_IP)) {
    PTRACE(1, "RTP_UDP\tCould not set TOS field in IP header: " << dataSocket->GetErrorText());
  }

  // Increase internal buffer size on media UDP sockets
  SetMinBufferSize(*dataSocket,    SO_RCVBUF);
  SetMinBufferSize(*dataSocket,    SO_SNDBUF);
  SetMinBufferSize(*controlSocket, SO_RCVBUF);
  SetMinBufferSize(*controlSocket, SO_SNDBUF);

  shutdownRead = FALSE;
  shutdownWrite = FALSE;

  if (canonicalName.Find('@') == P_MAX_INDEX)
    canonicalName += '@' + GetLocalHostName();

  PTRACE(2, "RTP_UDP\tSession " << sessionID << " created: "
         << localAddress << ':' << localDataPort << '-' << localControlPort
         << " ssrc=" << syncSourceOut);
  
  return TRUE;
}


void RTP_UDP::Reopen(BOOL reading)
{
  if (reading)
    shutdownRead = FALSE;
  else
    shutdownWrite = FALSE;
}


void RTP_UDP::Close(BOOL reading)
{
  if (reading) {
    if (!shutdownRead) {
      PTRACE(3, "RTP_UDP\tSession " << sessionID << ", Shutting down read.");
      syncSourceIn = 0;
      shutdownRead = TRUE;
      if (dataSocket != NULL && controlSocket != NULL) {
        PIPSocket::Address addr;
        controlSocket->GetLocalAddress(addr);
        if (addr.IsAny())
          PIPSocket::GetHostAddress(addr);
        dataSocket->WriteTo("", 1, addr, controlSocket->GetPort());
      }
    }
  }
  else {
    PTRACE(3, "RTP_UDP\tSession " << sessionID << ", Shutting down write.");
    shutdownWrite = TRUE;
  }
}


PString RTP_UDP::GetLocalHostName()
{
  return PIPSocket::GetHostName();
}


BOOL RTP_UDP::SetRemoteSocketInfo(PIPSocket::Address address, WORD port, BOOL isDataPort)
{
  if (remoteIsNAT) {
    PTRACE(3, "RTP_UDP\tIgnoring remote socket info as remote is behind NAT");
    return TRUE;
  }

  PTRACE(3, "RTP_UDP\tSetRemoteSocketInfo: session=" << sessionID << ' '
         << (isDataPort ? "data" : "control") << " channel, "
            "new=" << address << ':' << port << ", "
            "local=" << localAddress << ':' << localDataPort << '-' << localControlPort << ", "
            "remote=" << remoteAddress << ':' << remoteDataPort << '-' << remoteControlPort);

  if (localAddress == address && (isDataPort ? localDataPort : localControlPort) == port)
    return TRUE;

  remoteAddress = address;

  if (isDataPort) {
    remoteDataPort = port;
    if (remoteControlPort == 0)
      remoteControlPort = (WORD)(port + 1);
  }
  else {
    remoteControlPort = port;
    if (remoteDataPort == 0)
      remoteDataPort = (WORD)(port - 1);
  }

  if (!appliedQOS)
      ApplyQOS(remoteAddress);

  return remoteAddress != 0 && port != 0;
}


BOOL RTP_UDP::ReadData(RTP_DataFrame & frame, BOOL loop)
{
  do 
  {

#ifdef H323_RTP_AGGREGATE
    PTime start;
#endif
    int selectStatus = PSocket::Select(*dataSocket, *controlSocket, reportTimer);
#ifdef H323_RTP_AGGREGATE
    unsigned duration = (unsigned)(PTime() - start).GetMilliSeconds();
    if (duration > 50) {
      PTRACE(4, "Warning: aggregator read routine was of extended duration = " << duration << " msecs");
    }
#endif

    if (shutdownRead) {
      PTRACE(3, "RTP_UDP\tSession " << sessionID << ", Read shutdown.");
      shutdownRead = FALSE;
      return FALSE;
    }

    switch (selectStatus) {
      case -2 :
        if (ReadControlPDU() == e_AbortTransport)
          return FALSE;
        break;

      case -3 :
        if (ReadControlPDU() == e_AbortTransport)
          return FALSE;
        // Then do -1 case

      case -1 :
        switch (ReadDataPDU(frame)) {
          case e_ProcessPacket :
            if (!shutdownRead)
              return TRUE;
          case e_IgnorePacket :
            break;
          case e_AbortTransport :
            return FALSE;
        }
        break;

      case 0 :
        PTRACE(5, "RTP_UDP\tSession " << sessionID << ", check for sending report.");
        if (!SendReport())
          return FALSE;
        break;

      case PSocket::Interrupted:
        PTRACE(3, "RTP_UDP\tSession " << sessionID << ", Interrupted.");
        return FALSE;

      default :
        PTRACE(1, "RTP_UDP\tSession " << sessionID << ", Select error: "
                << PChannel::GetErrorText((PChannel::Errors)selectStatus));
        return FALSE;
    }
  } while (loop);

  return TRUE;
}


RTP_Session::SendReceiveStatus RTP_UDP::ReadDataOrControlPDU(PUDPSocket & socket,
                                                             PBYTEArray & frame,
                                                             BOOL fromDataChannel)
{
#if PTRACING
  const char * channelName = fromDataChannel ? "Data" : "Control";
#endif
  PIPSocket::Address addr;
  WORD port;

  if (socket.ReadFrom(frame.GetPointer(), frame.GetSize(), addr, port)) {
    if (ignoreOtherSources) {

      // If remote address never set from higher levels, then try and figure
      // it out from the first packet received.
      if (!remoteAddress.IsValid()) {
        remoteAddress = addr;
        PTRACE(4, "RTP\tSet remote address from first " << channelName
               << " PDU from " << addr << ':' << port);
      }
      if (fromDataChannel) {
        if (remoteDataPort == 0)
          remoteDataPort = port;
      }
      else {
        if (remoteControlPort == 0)
          remoteControlPort = port;
      }

      if (!remoteTransmitAddress.IsValid())
        remoteTransmitAddress = addr;
      else if (remoteTransmitAddress != addr) {
        PTRACE(1, "RTP_UDP\tSession " << sessionID << ", "
               << channelName << " PDU from incorrect host, "
                  " is " << addr << " should be " << remoteTransmitAddress);
        return RTP_Session::e_IgnorePacket;
      }
    }

    if (remoteAddress.IsValid() && !appliedQOS) 
      ApplyQOS(remoteAddress);

    return RTP_Session::e_ProcessPacket;
  }

  switch (socket.GetErrorNumber()) {
    case ECONNRESET :
    case ECONNREFUSED :
      PTRACE(2, "RTP_UDP\tSession " << sessionID << ", "
             << channelName << " port on remote not ready.");
      return RTP_Session::e_IgnorePacket;

    case EAGAIN :
      // Shouldn't happen, but it does.
      return RTP_Session::e_IgnorePacket;

    default:
      PTRACE(1, "RTP_UDP\t" << channelName << " read error ("
             << socket.GetErrorNumber(PChannel::LastReadError) << "): "
             << socket.GetErrorText(PChannel::LastReadError));
      return RTP_Session::e_AbortTransport;
  }
}


RTP_Session::SendReceiveStatus RTP_UDP::ReadDataPDU(RTP_DataFrame & frame)
{
  SendReceiveStatus status = ReadDataOrControlPDU(*dataSocket, frame, TRUE);
  if (status != e_ProcessPacket)
    return status;

  // Check received PDU is big enough
  PINDEX pduSize = dataSocket->GetLastReadCount();
  if (pduSize < RTP_DataFrame::MinHeaderSize || pduSize < frame.GetHeaderSize()) {
    PTRACE(2, "RTP_UDP\tSession " << sessionID
           << ", Received data packet too small: " << pduSize << " bytes");
    return e_IgnorePacket;
  }

  frame.SetPayloadSize(pduSize - frame.GetHeaderSize());

  PTRACE(9, "RTP_UDP\tSession " << sessionID << ", Received frame " << pduSize << " " << frame.GetHeaderSize() << "+" << frame.GetPayloadSize());
  return OnReceiveData(frame,*this);
}


RTP_Session::SendReceiveStatus RTP_UDP::ReadControlPDU()
{
  RTP_ControlFrame frame(2048);

  SendReceiveStatus status = ReadDataOrControlPDU(*controlSocket, frame, FALSE);
  if (status != e_ProcessPacket)
    return status;

  PINDEX pduSize = controlSocket->GetLastReadCount();
  if (pduSize < 4 || pduSize < 4+frame.GetPayloadSize()) {
    PTRACE(2, "RTP_UDP\tSession " << sessionID
           << ", Received control packet too small: " << pduSize << " bytes");
    return e_IgnorePacket;
  }

  frame.SetSize(pduSize);
  return OnReceiveControl(frame);
}


BOOL RTP_UDP::PreWriteData(RTP_DataFrame & frame)
{
  if (shutdownWrite) {
    PTRACE(3, "RTP_UDP\tSession " << sessionID << ", Write shutdown.");
    shutdownWrite = FALSE;
    return FALSE;
  }

  // Trying to send a PDU before we are set up!
  if (remoteAddress.IsAny() || !remoteAddress.IsValid() || remoteDataPort == 0)
    return TRUE;

  switch (OnSendData(frame)) {
    case e_ProcessPacket :
      break;
    case e_IgnorePacket :
      return TRUE;
    case e_AbortTransport :
      return FALSE;
  }

  return TRUE;
}

BOOL RTP_UDP::WriteData(RTP_DataFrame & frame)
{

  while (!dataSocket->WriteTo(frame.GetPointer(),
                             frame.GetHeaderSize()+frame.GetPayloadSize(),
                             remoteAddress, remoteDataPort)) {
    switch (dataSocket->GetErrorNumber()) {
      case ECONNRESET :
      case ECONNREFUSED :
        PTRACE(2, "RTP_UDP\tSession " << sessionID << ", data port on remote not ready.");
        break;

      default:
        PTRACE(1, "RTP_UDP\tSession " << sessionID
               << ", Write error on data port ("
               << dataSocket->GetErrorNumber(PChannel::LastWriteError) << "): "
               << dataSocket->GetErrorText(PChannel::LastWriteError));
        return FALSE;
    }
  }

  return TRUE;
}


BOOL RTP_UDP::WriteControl(RTP_ControlFrame & frame)
{
  // Trying to send a PDU before we are set up!
  if (!remoteAddress.IsValid() || remoteControlPort == 0)
    return TRUE;

  while (!controlSocket->WriteTo(frame.GetPointer(), frame.GetCompoundSize(),
                                remoteAddress, remoteControlPort)) {
    switch (controlSocket->GetErrorNumber()) {
      case ECONNRESET :
      case ECONNREFUSED :
        PTRACE(2, "RTP_UDP\tSession " << sessionID << ", control port on remote not ready.");
        break;

      default:
        PTRACE(1, "RTP_UDP\tSession " << sessionID
               << ", Write error on control port ("
               << controlSocket->GetErrorNumber(PChannel::LastWriteError) << "): "
               << controlSocket->GetErrorText(PChannel::LastWriteError));
    }
  }

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
