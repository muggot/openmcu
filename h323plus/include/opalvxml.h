/*
 * opalvxml.h
 *
 * Header file for IVR code
 *
 * A H.323 IVR application.
 *
 * Copyright (C) 2002 Equivalence Pty. Ltd.
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
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: opalvxml.h,v $
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.16  2004/07/15 11:20:37  rjongbloed
 * Migrated changes from crs_vxnml_devel branch into main trunk
 *
 * Revision 1.15.6.1  2004/07/07 07:10:11  csoutheren
 * Changed to use new factory based PWAVFile
 * Removed redundant blocking/unblocking when using G.723.1
 *
 * Revision 1.15  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.14  2002/08/27 02:21:07  craigs
 * Added silence detection capability to fake G.723.1codec
 *
 * Revision 1.13  2002/08/15 08:22:25  craigs
 * Added P_EXPAT test back in
 *
 * Revision 1.12  2002/08/15 04:55:26  robertj
 * Fixed shutdown problems with closing vxml session, leaks a thread.
 * Fixed potential problems with indirect channel Close() function.
 *
 * Revision 1.11  2002/08/06 06:00:38  craigs
 * Removed ifdef test for EXPLATFLAG
 *
 * Revision 1.10  2002/08/06 05:11:24  craigs
 * Moved most of stuff to ptclib
 *
 * Revision 1.9  2002/08/05 09:43:30  robertj
 * Added pragma interface/implementation
 * Moved virtual into .cxx file
 *
 * Revision 1.8  2002/07/29 15:13:51  craigs
 * Added autodelete option to PlayFile
 *
 * Revision 1.7  2002/07/18 04:17:12  robertj
 * Moved virtuals to source and changed name of G.723.1 file capability
 *
 * Revision 1.6  2002/07/10 13:16:19  craigs
 * Moved some VXML classes from Opal back into PTCLib
 * Added ability to repeat outputted data
 *
 * Revision 1.5  2002/07/09 08:27:23  craigs
 * Added GetMediaFormat to channels
 *
 * Revision 1.4  2002/07/03 01:53:58  craigs
 * Added newline to the end of the file for Linux
 *
 * Revision 1.3  2002/07/02 06:32:04  craigs
 * Added recording functions
 *
 * Revision 1.2  2002/06/28 02:42:54  craigs
 * Fixed problem with G.723.1 file codec not identified as native format
 *
 * Revision 1.1  2002/06/27 05:44:39  craigs
 * Initial version
 *
 * Revision 1.2  2002/06/26 09:05:28  csoutheren
 * Added ability to utter various "sayas" types within prompts
 *
 * Revision 1.1  2002/06/26 01:13:53  csoutheren
 * Disassociated VXML and Opal/OpenH323 specific elements
 *
 *
 */

#ifndef _OpenIVR_OPALVXML_H
#define _OpenIVR_OPALVXML_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptclib/vxml.h>
#include <opalwavfile.h>
#include <ptclib/delaychan.h>
#include <h323caps.h>
#include <h245.h>
#include <h323con.h>

//////////////////////////////////////////////////////////////////

class G7231_File_Codec : public H323AudioCodec
{
  PCLASSINFO(G7231_File_Codec, H323AudioCodec);

  public:
    G7231_File_Codec(Direction dir);

    unsigned GetBandwidth() const;
    static int GetFrameLen(int val);
      
    BOOL Read(BYTE * buffer, unsigned & length, RTP_DataFrame &);
    BOOL Write(const BYTE * buffer, unsigned length, const RTP_DataFrame & rtp, unsigned & frames);

    BOOL IsRawDataChannelNative() const;

    unsigned GetAverageSignalLevel();

  protected:
    int lastFrameLen;
};  


class G7231_File_Capability : public H323AudioCapability
{
  PCLASSINFO(G7231_File_Capability, H323AudioCapability)

  public:
    G7231_File_Capability();

    unsigned GetSubType() const;
    PString GetFormatName() const;

    H323Codec * CreateCodec(H323Codec::Direction direction) const;

    BOOL OnSendingPDU(H245_AudioCapability & cap, unsigned packetSize) const;
    BOOL OnReceivedPDU(const H245_AudioCapability & pdu, unsigned & packetSize);
    PObject * Clone() const;
};


//////////////////////////////////////////////////////////////////


#if P_EXPAT

class PTextToSpeech;

class OpalVXMLSession : public PVXMLSession 
{
  PCLASSINFO(OpalVXMLSession, PVXMLSession);
  public:
    OpalVXMLSession(H323Connection * _conn, PTextToSpeech * tts = NULL, BOOL autoDelete = FALSE);
    BOOL Close();

  protected:
    H323Connection * conn;
};

#endif

#endif

