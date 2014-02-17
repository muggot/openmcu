/*
 * rtp2wav.cxx
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
 * $Log: rtp2wav.cxx,v $
 * Revision 1.1  2007/08/06 20:51:08  shorne
 * First commit of h323plus
 *
 * Revision 1.3  2003/01/07 07:52:50  craigs
 * Fixed problem with multi-frame G.723.1 packets
 *
 * Revision 1.2  2002/05/23 04:22:32  robertj
 * Fixed problem with detecting correct payload type. Must
 *   wait for first non-empty packet.
 * Added virtual function so can override record start point.
 *
 * Revision 1.1  2002/05/21 02:42:58  robertj
 * Added class to allow for saving of RTP data to a WAV file.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "rtp2wav.h"
#endif

#include "rtp2wav.h"


#define new PNEW


///////////////////////////////////////////////////////////////////////////////

OpalRtpToWavFile::OpalRtpToWavFile()
#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif
  : receiveHandler(PCREATE_NOTIFIER(ReceivedPacket))
#ifdef _MSC_VER
#pragma warning(default:4355)
#endif
{
  payloadType = RTP_DataFrame::IllegalPayloadType;
}


OpalRtpToWavFile::OpalRtpToWavFile(const PString & filename)
#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif
  :  receiveHandler(PCREATE_NOTIFIER(ReceivedPacket))
#ifdef _MSC_VER
#pragma warning(default:4355)
#endif
{
  SetFilePath(filename);
  payloadType = RTP_DataFrame::IllegalPayloadType;
  lastPayloadSize = 0;
}


BOOL OpalRtpToWavFile::OnFirstPacket(RTP_DataFrame & frame)
{
  static int SupportedTypes[] = {
    PWAVFile::fmt_uLaw,
    0, 0,
    PWAVFile::fmt_GSM,
    PWAVFile::fmt_VivoG7231,
    0, 0, 0,
    PWAVFile::fmt_ALaw,
    0, 0,
    PWAVFile::fmt_PCM
  };

  payloadType = frame.GetPayloadType();

  if (payloadType >= PARRAYSIZE(SupportedTypes) || SupportedTypes[payloadType] == 0) {
    PTRACE(1, "rtp2wav\tUnsupported payload type: " << payloadType);
    return FALSE;
  }

  if (!SetFormat(SupportedTypes[payloadType])) {
    PTRACE(1, "rtp2wav\tCould not set WAV file format: " << SupportedTypes[payloadType]);
    return FALSE;
  }

  if (!Open(PFile::WriteOnly)) {
    PTRACE(1, "rtp2wav\tCould not open WAV file: " << GetErrorText());
    return FALSE;
  }

  PTRACE(3, "rtp2wav\tStarted recording payload type " << payloadType
         << " to " << GetFilePath());
  return TRUE;
}


void OpalRtpToWavFile::ReceivedPacket(RTP_DataFrame & frame, INT)
{
  PINDEX payloadSize = frame.GetPayloadSize();

  if (payloadType == RTP_DataFrame::IllegalPayloadType) {
    // Ignore packets until actually get something of jitter buffer
    if (payloadSize == 0)
      return;
    if (!OnFirstPacket(frame))
      return;
  }

  if (payloadType != frame.GetPayloadType())
    return;

  if (!IsOpen())
    return;

  if (payloadSize > 0) {
    if (Write(frame.GetPayloadPtr(), payloadSize)) {
      lastPayloadSize = payloadSize;
      memcpy(lastFrame.GetPointer(lastPayloadSize), frame.GetPayloadPtr(), payloadSize);
      return;
    }
  }

  else if (lastPayloadSize == 0)
    return;

  else if (Write(lastFrame.GetPointer(), lastPayloadSize))
    return;

  PTRACE(1, "rtp2wav\tError writing to WAV file: " << GetErrorText(PChannel::LastWriteError));
  Close();
}


/////////////////////////////////////////////////////////////////////////////
