/*
 * rfc2833.cxx
 *
 * Open Phone Abstraction Library (OPAL)
 * Formally known as the Open H323 project.
 *
 * Copyright (c) 2001 Equivalence Pty. Ltd.
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
 * The Original Code is Open Phone Abstraction Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: rfc2833.cxx,v $
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.4  2003/06/03 05:02:18  rjongbloed
 * Added comment and test for NULL pointer parameter.
 *
 * Revision 1.3  2002/05/08 04:39:57  robertj
 * Fixed problem with receiving RFC2833 data, was still trying to process it as
 *   codec and upsettign the channel receiver code.
 *
 * Revision 1.2  2002/02/19 06:32:38  robertj
 * Allowed for RTP filter functions to force output of packet, or prevent it
 *   from being sent overriding the n frames per packet algorithm.
 * Added more tracing.
 *
 * Revision 1.1  2002/01/23 05:06:23  robertj
 * Added RFC2833 support as separate class
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "rfc2833.h"
#endif

#include "rfc2833.h"


static const char RFC2833Table1Events[] = "0123456789*#ABCD!";


#define new PNEW


///////////////////////////////////////////////////////////////////////////////

OpalRFC2833Info::OpalRFC2833Info(char t, unsigned d, unsigned ts)
{
  tone = t;
  duration = d;
  timestamp = ts;
}


///////////////////////////////////////////////////////////////////////////////

OpalRFC2833::OpalRFC2833(const PNotifier & rx)
  : receiveNotifier(rx),
#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif
    receiveHandler(PCREATE_NOTIFIER(ReceivedPacket)),
    transmitHandler(PCREATE_NOTIFIER(TransmitPacket))
#ifdef _MSC_VER
#pragma warning(default:4355)
#endif
{
  PTRACE(3, "RFC2833\tHandler created");

  payloadType = RTP_DataFrame::IllegalPayloadType;
  receiveComplete = TRUE;
  receiveTimestamp = 0;
  receiveTimer.SetNotifier(PCREATE_NOTIFIER(ReceiveTimeout));

  transmitState = TransmitIdle;
  transmitTimestamp = 0;
  transmitTimer.SetNotifier(PCREATE_NOTIFIER(TransmitEnded));
}


BOOL OpalRFC2833::SendTone(char tone, unsigned duration)
{
  if (!BeginTransmit(tone))
    return FALSE;

  transmitTimer = duration;
  return TRUE;
}


BOOL OpalRFC2833::BeginTransmit(char tone)
{
  PWaitAndSignal m(mutex);

  const char * theChar = strchr(RFC2833Table1Events, tone);
  if (theChar == NULL) {
    PTRACE(1, "RFC2833\tInvalid tone character.");
    return FALSE;
  }

  if (transmitState != TransmitIdle) {
    PTRACE(1, "RFC2833\tAttempt to send tone while currently sending.");
    return FALSE;
  }

  transmitCode = (BYTE)(theChar-RFC2833Table1Events);
  transmitState = TransmitActive;
  transmitTimestamp = 0;
  PTRACE(3, "RFC2833\tBegin transmit tone='" << tone << '\'');
  return TRUE;
}


BOOL OpalRFC2833::EndTransmit()
{
  PWaitAndSignal m(mutex);

  if (transmitState != TransmitActive) {
    PTRACE(1, "RFC2833\tAttempt to stop send tone while not sending.");
    return FALSE;
  }

  transmitState = TransmitEnding;
  PTRACE(3, "RFC2833\tEnd transmit tone='" << RFC2833Table1Events[transmitCode] << '\'');
  return TRUE;
}


void OpalRFC2833::OnStartReceive(char tone)
{
  OpalRFC2833Info info(tone);
  receiveNotifier(info, 0);
}


void OpalRFC2833::OnEndReceive(char tone, unsigned duration, unsigned timestamp)
{
  OpalRFC2833Info info(tone, duration, timestamp);
  receiveNotifier(info, 0);
}


void OpalRFC2833::ReceivedPacket(RTP_DataFrame & frame, INT)
{
  if (frame.GetPayloadType() != payloadType)
    return;

  PINDEX payloadSize = frame.GetPayloadSize();
  // Zero the payload size so the channel processing ignores this packet
  frame.SetPayloadSize(0);

  if (payloadSize < 4) {
    PTRACE_IF(1, payloadSize > 0,
              "RFC2833\tIgnoring packet, too small: " << frame.GetPayloadSize());
    return;
  }

  const BYTE * payload = frame.GetPayloadPtr();
  if (payload[0] >= sizeof(RFC2833Table1Events)-1) {
    PTRACE(2, "RFC2833\tIgnoring packet, unsupported event.");
    return;
  }

  PWaitAndSignal m(mutex);

  receivedTone = RFC2833Table1Events[payload[0]];
  receivedDuration = (payload[2]<<8) + payload[3];

  unsigned timestamp = frame.GetTimestamp();
  if (receiveTimestamp != timestamp) {
    PTRACE(3, "RFC2833\tReceived start tone=" << receivedTone);
    OnStartReceive(receivedTone);

    // Starting a new event.
    receiveTimestamp = timestamp;
    receiveComplete = FALSE;
    receiveTimer = 150;
  }
  else {
    receiveTimer = 150;
    if (receiveComplete) {
      PTRACE(3, "RFC2833\tIgnoring duplicate packet.");
      return;
    }
  }

  if ((payload[1]&0x80) == 0) {
    PTRACE(1, "RFC2833\tIgnoring packet, not end of event.");
    return;
  }

  receiveComplete = TRUE;
  receiveTimer.Stop();

  PTRACE(3, "RFC2833\tReceived end tone=" << receivedTone << " duration=" << receivedDuration);
  OnEndReceive(receivedTone, receivedDuration, receiveTimestamp);
}


void OpalRFC2833::ReceiveTimeout(PTimer &, INT)
{
  PWaitAndSignal m(mutex);

  if (receiveComplete)
    return;

  receiveComplete = TRUE;
  PTRACE(3, "RFC2833\tTimeout tone=" << receivedTone << " duration=" << receivedDuration);
  OnEndReceive(receivedTone, receivedDuration, receiveTimestamp);
}


void OpalRFC2833::TransmitPacket(RTP_DataFrame & frame, INT param)
{
  if (transmitState == TransmitIdle)
    return;

  // Set flag to force a packet to be sent.
  if (param != 0)
    *(BOOL *)param = TRUE;

  PWaitAndSignal m(mutex);

  //frame.SetMarker(transmitTimestamp == 0);

  unsigned actualTimestamp = frame.GetTimestamp();
  if (transmitTimestamp == 0)
    transmitTimestamp = actualTimestamp;
  frame.SetTimestamp(transmitTimestamp);

  frame.SetPayloadType(payloadType);
  frame.SetPayloadSize(4);

  BYTE * payload = frame.GetPayloadPtr();
  payload[0] = transmitCode;

  payload[1] = 7;  // Volume
  if (transmitState == TransmitEnding) {
    payload[1] |= 0x80;
    transmitState = TransmitIdle;
  }

  unsigned duration = actualTimestamp - transmitTimestamp;
  payload[2] = (BYTE)(duration>>8);
  payload[3] = (BYTE) duration    ;

  PTRACE(4, "RFC2833\tInserting packet: ts=" << transmitTimestamp
         << " code='" << RFC2833Table1Events[transmitCode] << "'"
            " duration=" << duration << ' '
         << (transmitState == TransmitIdle ? "ending" : "continuing"));
}


void OpalRFC2833::TransmitEnded(PTimer &, INT)
{
  EndTransmit();
}


/////////////////////////////////////////////////////////////////////////////
