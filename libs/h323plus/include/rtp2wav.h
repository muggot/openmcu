/*
 * rfc28rtp2wav33.h
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
 * $Log: rtp2wav.h,v $
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.4  2003/01/07 07:53:00  craigs
 * Fixed problem with multi-frame G.723.1 packets
 *
 * Revision 1.3  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.2  2002/05/23 04:22:29  robertj
 * Fixed problem with detecting correct payload type. Must
 *   wait for first non-empty packet.
 * Added virtual function so can override record start point.
 *
 * Revision 1.1  2002/05/21 02:42:58  robertj
 * Added class to allow for saving of RTP data to a WAV file.
 *
 */

#ifndef __RTP_RTP2WAV_H
#define __RTP_RTP2WAV_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include <ptclib/pwavfile.h>
#include "rtp.h"


///////////////////////////////////////////////////////////////////////////////

/**This class encapsulates a WAV file that can be used to intercept RTP data
   in the standard H323RTPChannel class.
  */
class OpalRtpToWavFile : public PWAVFile
{
    PCLASSINFO(OpalRtpToWavFile, PWAVFile);
  public:
    OpalRtpToWavFile();
    OpalRtpToWavFile(
      const PString & filename
    );

    virtual BOOL OnFirstPacket(RTP_DataFrame & frame);

    const PNotifier & GetReceiveHandler() const { return receiveHandler; }

  protected:
    PDECLARE_NOTIFIER(RTP_DataFrame, OpalRtpToWavFile, ReceivedPacket);

    PNotifier                   receiveHandler;
    RTP_DataFrame::PayloadTypes payloadType;
    PBYTEArray                  lastFrame;
    PINDEX                      lastPayloadSize;
};


#endif // __RTP_RTP2WAV_H


/////////////////////////////////////////////////////////////////////////////
