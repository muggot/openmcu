/*
 * rfc2833.h
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
 * $Log: rfc2833.h,v $
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.4  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.3  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.2  2002/09/03 06:19:37  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.1  2002/01/23 05:06:23  robertj
 * Added RFC2833 support as separate class
 *
 */

#ifndef __OPAL_RFC2833_H
#define __OPAL_RFC2833_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include "rtp.h"


///////////////////////////////////////////////////////////////////////////////

class OpalRFC2833Info : public PObject {
    PCLASSINFO(OpalRFC2833Info, PObject);
  public:
    OpalRFC2833Info(
      char tone,
      unsigned duration = 0,
      unsigned timestamp = 0
    );

    char GetTone() const { return tone; }
    unsigned GetDuration() const { return duration; }
    unsigned GetTimestamp() const { return timestamp; }
    BOOL IsToneStart() const { return duration == 0; }

  protected:
    char     tone;
    unsigned duration;
    unsigned timestamp;
};


class OpalRFC2833 : public PObject {
    PCLASSINFO(OpalRFC2833, PObject);
  public:
    OpalRFC2833(
      const PNotifier & receiveNotifier
    );

    virtual BOOL SendTone(
      char tone,              ///<  DTMF tone code
      unsigned duration       ///<  Duration of tone in milliseconds
    );

    virtual BOOL BeginTransmit(
      char tone  ///<  DTMF tone code
    );
    virtual BOOL EndTransmit();

    virtual void OnStartReceive(
      char tone
    );
    virtual void OnEndReceive(
      char tone,
      unsigned duration,
      unsigned timestamp
    );

    RTP_DataFrame::PayloadTypes GetPayloadType() const { return payloadType; }

    void SetPayloadType(
      RTP_DataFrame::PayloadTypes type ///<  new payload type
    ) { payloadType = type; }

    const PNotifier & GetReceiveHandler() const { return receiveHandler; }
    const PNotifier & GetTransmitHandler() const { return transmitHandler; }

  protected:
    PDECLARE_NOTIFIER(RTP_DataFrame, OpalRFC2833, ReceivedPacket);
    PDECLARE_NOTIFIER(RTP_DataFrame, OpalRFC2833, TransmitPacket);
    PDECLARE_NOTIFIER(PTimer, OpalRFC2833, ReceiveTimeout);
    PDECLARE_NOTIFIER(PTimer, OpalRFC2833, TransmitEnded);

    RTP_DataFrame::PayloadTypes payloadType;

    PMutex mutex;

    PNotifier receiveNotifier;
    BOOL      receiveComplete;
    BYTE      receivedTone;
    unsigned  receivedDuration;
    unsigned  receiveTimestamp;
    PTimer    receiveTimer;
    PNotifier receiveHandler;

    enum {
      TransmitIdle,
      TransmitActive,
      TransmitEnding
    }         transmitState;
    BYTE      transmitCode;
    unsigned  transmitTimestamp;
    PTimer    transmitTimer;
    PNotifier transmitHandler;
};


#endif // __OPAL_RFC2833_H


/////////////////////////////////////////////////////////////////////////////
