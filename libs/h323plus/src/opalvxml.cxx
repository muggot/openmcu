/*
 * vxml.cxx
 *
 * VXML control for for Opal
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
 * $Log: opalvxml.cxx,v $
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.22  2004/07/15 11:20:38  rjongbloed
 * Migrated changes from crs_vxnml_devel branch into main trunk
 *
 * Revision 1.21  2004/07/15 03:18:00  csoutheren
 * Migrated changes from crs_vxnml_devel branch into main trunk
 *
 * Revision 1.20.2.1  2004/07/07 07:10:11  csoutheren
 * Changed to use new factory based PWAVFile
 * Removed redundant blocking/unblocking when using G.723.1
 *
 * Revision 1.20  2004/05/04 23:23:39  csoutheren
 * Removed usage of lpc10 and mscodecs
 *
 * Revision 1.19  2004/05/03 13:21:45  rjongbloed
 * Converted everything to be codec plug in freindly
 * Removed GSM and G.729 as now plug ins are "the way"!
 *
 * Revision 1.18  2002/12/10 23:50:25  robertj
 * Fixed some tracing issues
 *
 * Revision 1.17  2002/08/27 02:21:31  craigs
 * Added silence detection capability to fake G.723.1codec
 *
 * Revision 1.16  2002/08/15 04:55:26  robertj
 * Fixed shutdown problems with closing vxml session, leaks a thread.
 * Fixed potential problems with indirect channel Close() function.
 *
 * Revision 1.15  2002/08/15 02:19:42  robertj
 * Adjusted trace log levels for G.723.1 file codec read/write tracking.
 *
 * Revision 1.14  2002/08/07 13:53:05  craigs
 * Fixed problem with included opalvxml.h thanks to Vladmir Toncar
 *
 * Revision 1.13  2002/08/06 05:10:59  craigs
 * Moved most of stuff to ptclib
 *
 * Revision 1.12  2002/08/05 09:43:30  robertj
 * Added pragma interface/implementation
 * Moved virtual into .cxx file
 *
 * Revision 1.11  2002/07/29 15:10:36  craigs
 * Added autodelete option to PlayFile
 *
 * Revision 1.10  2002/07/29 12:54:42  craigs
 * Removed usages of cerr
 *
 * Revision 1.9  2002/07/18 04:17:12  robertj
 * Moved virtuals to source and changed name of G.723.1 file capability
 *
 * Revision 1.8  2002/07/10 13:16:58  craigs
 * Moved some VXML classes from Opal back into PTCLib
 * Added ability to repeat outputted data
 *
 * Revision 1.7  2002/07/09 08:48:41  craigs
 * Fixed trace messages
 *
 * Revision 1.6  2002/07/05 06:34:04  craigs
 * Changed comments and trace messages
 *
 * Revision 1.5  2002/07/03 04:58:13  robertj
 * Changed for compatibility with older GNU compilers
 *
 * Revision 1.4  2002/07/02 06:32:51  craigs
 * Added recording functions
 *
 * Revision 1.3  2002/06/28 02:42:11  craigs
 * Fixed problem with G.723.1 file naming conventions
 * Fixed problem with incorrect file open mode
 *
 * Revision 1.2  2002/06/28 01:24:03  robertj
 * Fixe dproblem with compiling without expat library.
 *
 * Revision 1.1  2002/06/27 05:44:11  craigs
 * Initial version
 *
 * Revision 1.2  2002/06/26 09:05:28  csoutheren
 * Added ability to utter various "sayas" types within prompts
 *
 * Revision 1.1  2002/06/26 01:13:53  csoutheren
 * Disassociated VXML and Opal/OpenH323 specific elements
 *
 * Revision 1.2  2002/06/21 08:18:22  csoutheren
 * Added start of grammar handling
 *
 * Revision 1.1  2002/06/20 06:35:44  csoutheren
 * Initial version
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "opalvxml.h"
#endif

#include "opalvxml.h"

#if P_EXPAT

#include <ptclib/delaychan.h>
#include <ptclib/pwavfile.h>
#include <ptclib/memfile.h>

#endif

#include "codecs.h"

#define	G7231_SAMPLES_PER_BLOCK	240
#define	G7231_BANDWIDTH		      (6300/100)

///////////////////////////////////////////////////////////////

G7231_File_Capability::G7231_File_Capability()
  : H323AudioCapability(8, 4)
{
}

unsigned G7231_File_Capability::GetSubType() const
{
  return H245_AudioCapability::e_g7231;
}

PString G7231_File_Capability::GetFormatName() const
{
  return "G.723.1{file}";
}

BOOL G7231_File_Capability::OnSendingPDU(H245_AudioCapability & cap, unsigned packetSize) const
{
  // set the choice to the correct type
  cap.SetTag(GetSubType());

  // get choice data
  H245_AudioCapability_g7231 & g7231 = cap;

  // max number of audio frames per PDU we want to send
  g7231.m_maxAl_sduAudioFrames = packetSize; 

  // enable silence suppression
  g7231.m_silenceSuppression = TRUE;

  return TRUE;
}

BOOL G7231_File_Capability::OnReceivedPDU(const H245_AudioCapability & cap, unsigned & packetSize)
{
  const H245_AudioCapability_g7231 & g7231 = cap;
  packetSize = g7231.m_maxAl_sduAudioFrames;
  return TRUE;
}

PObject * G7231_File_Capability::Clone() const
{
  return new G7231_File_Capability(*this);
}

H323Codec * G7231_File_Capability::CreateCodec(H323Codec::Direction direction) const
{
  return new G7231_File_Codec(direction);
}

///////////////////////////////////////////////////////////////

G7231_File_Codec::G7231_File_Codec(Direction dir)
  : H323AudioCodec(OPAL_G7231_6k3, dir)
{
  lastFrameLen = 4;
}


int G7231_File_Codec::GetFrameLen(int val)
{
  static const int frameLen[] = { 24, 20, 4, 1 };
  return frameLen[val & 3];
}

BOOL G7231_File_Codec::Read(BYTE * buffer, unsigned & length, RTP_DataFrame &)
{
  if (rawDataChannel == NULL)
    return FALSE;
    
  if (!rawDataChannel->Read(buffer, 24)) {
    PTRACE(1, "G7231WAV\tRead failed");
    return FALSE;
  }

  lastFrameLen = length = G7231_File_Codec::GetFrameLen(buffer[0]);

  return TRUE;
}


BOOL G7231_File_Codec::Write(const BYTE * buffer,
                             unsigned length,
                             const RTP_DataFrame & /* rtp */,
                             unsigned & writtenLength)
{
  if (rawDataChannel == NULL)
    return TRUE;

  static const BYTE silence[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0};

  // If the length is zero, output silence to the file..
  if (length == 0) {
    PTRACE(6,"G7231WAV\tZero length frame");
    writtenLength = 0;
    return rawDataChannel->Write(silence, 24);
  }

  int writeLen;
  switch (buffer[0]&3) {
    case 0:
      writeLen = 24;
      break;
    case 1:
      writeLen = 20;
      break;
    case 2:
      // Windows Media Player cannot play 4 byte SID (silence) frames.
      // So write out a 24 byte frame of silence instead.
      PTRACE(5, "G7231WAV\tReplacing SID with 24 byte frame");
      writtenLength = 4;
      return rawDataChannel->Write(silence, 24);
    default :
      writeLen = 1;
      break;
  }

  PTRACE(6, "G7231WAV\tFrame length = " <<writeLen);

  writtenLength = writeLen;

  return rawDataChannel->Write(buffer, writeLen);
}


unsigned G7231_File_Codec::GetBandwidth() const
{ 
  return G7231_BANDWIDTH; 
}


BOOL G7231_File_Codec::IsRawDataChannelNative() const
{
  return TRUE;
}

unsigned G7231_File_Codec::GetAverageSignalLevel()
{
  if (lastFrameLen == 4)
    return 0;
  else
    return UINT_MAX;
}

///////////////////////////////////////////////////////////////

#if P_EXPAT

OpalVXMLSession::OpalVXMLSession(H323Connection * _conn, PTextToSpeech * tts, BOOL autoDelete)
  : PVXMLSession(tts, autoDelete), conn(_conn)
{
}


BOOL OpalVXMLSession::Close()
{
  BOOL ok = PVXMLSession::Close();
  conn->ClearCall();
  return ok;
}




#endif


// End of File /////////////////////////////////////////////////////////////
