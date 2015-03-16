/*
 * h323caps.cxx
 *
 * H.323 protocol handler
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
 * $Log: h323caps.cxx,v $
 * Revision 1.15  2008/01/30 02:09:38  shorne
 * Fixes for removing and reloading extended Video Capability
 *
 * Revision 1.14  2007/12/07 07:33:22  shorne
 * Settled on codecs greater than CIF to be generic
 *
 * Revision 1.13  2007/11/20 11:40:47  willamowius
 * fix compilation without audio support Thanks Vladimir Voronin
 *
 * Revision 1.12  2007/11/14 08:55:22  shorne
 * Added ability to set 
 DSCP values for audio/video
 *
 * Revision 1.11  2007/11/10 13:37:32  shorne
 * Corrected Conference Control capability type and resolved error with PTRACE on release build
 *
 * Revision 1.10  2007/11/06 17:43:36  shorne
 * added i480 standard framesize
 *
 * Revision 1.9  2007/11/01 20:17:33  shorne
 * updates for H.239 support
 *
 * Revision 1.8  2007/10/30 04:23:45  shorne
 * Corrections and Improvements for H.239 support
 *
 * Revision 1.7  2007/10/25 21:08:04  shorne
 * Added support for HD Video devices
 *
 * Revision 1.6  2007/10/19 19:54:17  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.5  2007/10/16 17:00:28  shorne
 * Added H.230 Support
 *
 * Revision 1.4  2007/08/20 20:19:52  shorne
 * Moved opalplugin.h to codec directory to be plugin compile compatible with Opal
 *
 * Revision 1.3  2007/08/20 19:13:28  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.2  2007/08/08 20:13:29  shorne
 * Fix H.249 typo
 *
 * Revision 1.1  2007/08/06 20:51:06  shorne
 * First commit of h323plus
 *
 * Revision 1.105.2.10  2007/09/05 04:14:40  rjongbloed
 * Back ported from OPAL media packetization in TCS
 *
 * Revision 1.105.2.5  2007/07/19 20:05:05  shorne
 * Added QoS as default
 *
 * Revision 1.105.2.4  2007/03/24 23:39:43  shorne
 * More H.239 work
 *
 * Revision 1.105.2.3  2007/02/19 20:11:05  shorne
 * Added Baseline H.239 Support
 *
 * Revision 1.105.2.2  2007/02/18 17:11:23  shorne
 * Added H.249 Extended UserInput Support
 *
 * Revision 1.105.2.1  2007/02/02 22:12:43  shorne
 * Added ability to set FrameSize for video plugins
 *
 * Revision 1.105  2006/10/10 04:21:56  csoutheren
 * Fixed compile problem on Linux with latest PWLib
 *
 * Revision 1.104  2006/09/05 23:56:57  csoutheren
 * Convert media format and capability factories to use std::string
 *
 * Revision 1.103  2006/05/16 11:36:01  shorne
 * added AEC support and  H323VdieoCapability default constructor
 *
 * Revision 1.102  2005/06/07 07:09:13  csoutheren
 * Removed compiler warnings on Windows
 *
 * Revision 1.101  2005/06/07 03:22:24  csoutheren
 * Added patch 1198741 with support for plugin codecs with generic capabilities
 * Added patch 1198754 with support for setting quality level on audio codecs
 * Added patch 1198760 with GSM-AMR codec support
 * Many thanks to Richard van der Hoff for his work
 *
 * Revision 1.100  2005/01/03 06:25:55  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.99  2004/12/08 02:14:26  csoutheren
 * Guard against NULL capabilities
 *
 * Revision 1.98  2004/08/26 08:05:04  csoutheren
 * Codecs now appear in abstract factory system
 * Fixed Windows factory bootstrap system (again)
 *
 * Revision 1.97  2004/08/24 14:23:11  csoutheren
 * Fixed problem with plugin codecs using capability compare functions
 *
 * Revision 1.96  2004/07/07 03:52:12  csoutheren
 * Fixed incorrect strings returned by GetFormatName on G.711 codecs
 *
 * Revision 1.95  2004/07/03 07:05:42  csoutheren
 * Applied patch 979639 to prevent crash in FindCapability if capability does not exist
 * Thanks to Dave Parr
 *
 * Revision 1.94  2004/06/30 12:31:16  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.93  2004/06/17 22:40:56  csoutheren
 * Fixed typo in FindCapability, thanks to Vyacheslav Frolov
 *
 * Revision 1.92  2004/06/16 07:50:00  csoutheren
 * Fixed potential crash caused when user indication capabilities are not present. Thanks to Yuri Kiryanov
 *
 * Revision 1.91  2004/06/08 13:32:18  rjongbloed
 * Fixed (pre)condition for checking and matching capabilities, thanks Guilhem Tardy
 *
 * Revision 1.90  2004/06/08 01:20:28  csoutheren
 * Provide backwards compatibility for applications using old capability API
 *
 * Revision 1.89  2004/06/03 13:32:00  csoutheren
 * Renamed INSTANTIATE_FACTORY
 *
 * Revision 1.88  2004/06/03 12:48:35  csoutheren
 * Decomposed PFactory declarations to hopefully avoid problems with DLLs
 *
 * Revision 1.87  2004/06/01 05:48:03  csoutheren
 * Changed capability table to use abstract factory routines rather than internal linked list
 *
 * Revision 1.86  2004/05/31 01:32:24  csoutheren
 * Removed debugging left behind
 *
 * Revision 1.85  2004/05/26 04:13:21  csoutheren
 * Safeguard against accidentally removing all capabilities by passing empty string to RemoveCapability
 *
 * Revision 1.84  2004/05/26 04:00:49  csoutheren
 * Fixed calling of non-standard capability compare function used primarily for plugin codecs
 *
 * Revision 1.83  2004/05/20 02:07:29  csoutheren
 * Use macro to work around MSVC internal compiler errors
 *
 * Revision 1.82  2004/05/19 23:42:48  csoutheren
 * Included <algorithm>
 *
 * Revision 1.81  2004/05/19 13:28:30  csoutheren
 * Changed H323EndPoint::AddAllCapabilities to only add standard media formats
 *
 * Revision 1.80  2004/05/19 07:38:23  csoutheren
 * Changed OpalMediaFormat handling to use abstract factory method functions
 *
 * Revision 1.79  2004/05/13 12:49:33  rjongbloed
 * Fixed usage of the compare function in non-standard capabilities.
 *
 * Revision 1.78  2004/05/10 01:01:25  csoutheren
 * Guarded against NULL capabilities passed to AddCapability
 *
 * Revision 1.77  2004/05/06 12:53:09  rjongbloed
 * Fixed ability to encode rxtx and tx capabilities, thanks Vyacheslav Andrejev.
 *
 * Revision 1.76  2004/05/04 03:33:33  csoutheren
 * Added guards against comparing certain kinds of Capabilities
 *
 * Revision 1.75  2004/04/22 12:38:03  rjongbloed
 * Removed the RTP QoS class if there is no QoS support in build,
 *   thanks Nick Hoath, ISDN Communications Ltd.
 *
 * Revision 1.74  2004/04/21 04:58:27  csoutheren
 * Fixed problem with mode select, thanks to Vamsi K Pottangi
 *
 * Revision 1.73  2004/04/09 13:28:38  rjongbloed
 * Fixed conversion of plug ins from OpenH323 to OPAL naming convention.
 *
 * Revision 1.72  2004/04/03 10:38:25  csoutheren
 * Added in initial cut at codec plugin code. Branches are for wimps :)
 *
 * Revision 1.71  2004/04/03 08:28:06  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.70  2004/03/24 01:57:20  csoutheren
 * Updated for changes in H245v9
 *
 * Revision 1.69  2004/03/02 11:48:17  rjongbloed
 * Fixed correct capability table matching when FindCapability with a mode change,
 *   uses different enum so needs translation table.
 *
 * Revision 1.68  2003/11/08 03:11:29  rjongbloed
 * Fixed failure to call ancestor in copy ctor, thanks  Victor Ivashin.
 *
 * Revision 1.67  2003/10/27 06:03:39  csoutheren
 * Added support for QoS
 *   Thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.66  2003/06/06 02:13:48  rjongbloed
 * Changed non-standard capability semantics so can use C style strings as
 *   the embedded data block (ie automatically call strlen)
 *
 * Revision 1.65  2003/05/16 07:30:20  rjongbloed
 * Fixed correct matching of OLC data types to capabilities, for example CIF
 *   and QCIF video are different and should match exactly.
 *
 * Revision 1.64  2003/04/28 07:00:09  robertj
 * Fixed problem with compiler(s) not correctly initialising static globals
 *
 * Revision 1.63  2003/04/27 23:50:38  craigs
 * Made list of registered codecs available outside h323caps.cxx
 *
 * Revision 1.62  2003/03/18 05:11:22  robertj
 * Fixed OID based non-standard capabilities, thanks Philippe Massicotte
 *
 * Revision 1.61  2002/12/05 12:29:31  rogerh
 * Add non standard codec identifier for Xiph.org
 *
 * Revision 1.60  2002/11/27 11:47:09  robertj
 * Fixed GNU warning
 *
 * Revision 1.59  2002/11/26 22:48:18  craigs
 * Changed nonStd codec identification to use a table for MS codecs
 *
 * Revision 1.58  2002/11/26 13:52:59  craigs
 * Added PrintOn function for outputting names of nonStandard codecs
 *
 * Revision 1.57  2002/11/09 04:44:24  robertj
 * Fixed function to add capabilities to return correct table entry index.
 * Cosmetic changes.
 *
 * Revision 1.56  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.55  2002/07/18 08:28:52  robertj
 * Adjusted some trace log levels
 *
 * Revision 1.54  2002/06/04 07:16:14  robertj
 * Fixed user indications (DTMF) not working on some endpoints which indicated
 *   receiveAndTransmitUserInputCapability in TCS.
 *
 * Revision 1.53  2002/05/29 03:55:21  robertj
 * Added protocol version number checking infrastructure, primarily to improve
 *   interoperability with stacks that are unforgiving of new features.
 *
 * Revision 1.52  2002/05/14 23:20:03  robertj
 * Fixed incorrect comparison in non-standard capability, tnanks Vyacheslav Frolov
 *
 * Revision 1.51  2002/05/10 05:45:41  robertj
 * Added the max bit rate field to the data channel capability class.
 *
 * Revision 1.50  2002/03/26 05:51:12  robertj
 * Forced RFC2833 to payload type 101 as some IOS's go nuts otherwise.
 *
 * Revision 1.49  2002/03/05 06:18:46  robertj
 * Fixed problem with some custom local capabilities not being used in getting
 *   remote capability list, especially in things like OpenAM.
 *
 * Revision 1.48  2002/02/25 04:38:42  robertj
 * Fixed wildcard lookup with * at end of string.
 * Fixed being able to create remote capability table before have local table.
 * Fixed using add all with wildcards adding capability multiple times.
 *
 * Revision 1.47  2002/02/14 07:15:15  robertj
 * Fixed problem with creation of remoteCapabilities and the "set" part contianing
 *   pointers to objects that have been deleted. Does not seem to be a practical
 *   problem but certainly needs fixing!
 *
 * Revision 1.46  2002/01/22 07:08:26  robertj
 * Added IllegalPayloadType enum as need marker for none set
 *   and MaxPayloadType is a legal value.
 *
 * Revision 1.45  2002/01/22 06:07:35  robertj
 * Moved payload type to ancestor so any capability can adjust it on logical channel.
 *
 * Revision 1.44  2002/01/17 07:05:03  robertj
 * Added support for RFC2833 embedded DTMF in the RTP stream.
 *
 * Revision 1.43  2002/01/16 05:38:04  robertj
 * Added missing mode change functions on non standard capabilities.
 *
 * Revision 1.42  2002/01/10 05:13:54  robertj
 * Added support for external RTP stacks, thanks NuMind Software Systems.
 *
 * Revision 1.41  2002/01/09 00:21:39  robertj
 * Changes to support outgoing H.245 RequstModeChange.
 *
 * Revision 1.40  2001/12/22 01:44:30  robertj
 * Added more support for H.245 RequestMode operation.
 *
 * Revision 1.39  2001/09/21 02:52:56  robertj
 * Added default implementation for PDU encode/decode for codecs
 *   that have simple integer as frames per packet.
 *
 * Revision 1.38  2001/09/11 10:21:42  robertj
 * Added direction field to capabilities, thanks Nick Hoath.
 *
 * Revision 1.37  2001/08/06 03:08:56  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.36  2001/07/19 09:50:30  robertj
 * Added code for default session ID on data channel being three.
 *
 * Revision 1.35  2001/07/17 04:44:31  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 * Revision 1.34  2001/06/29 04:58:57  robertj
 * Added wildcard character '*' to capability name string searches.
 *
 * Revision 1.33  2001/06/15 16:10:19  rogerh
 * Fix the "capabilities are the same" assertion
 *
 * Revision 1.32  2001/05/31 06:28:37  robertj
 * Made sure capability descriptors alternate capability sets are in the same
 *   order as the capability table when doing reorder. This improves compatibility
 *   with endpoints that select the first capability in that list rather than the table.
 *
 * Revision 1.31  2001/05/14 05:56:28  robertj
 * Added H323 capability registration system so can add capabilities by
 *   string name instead of having to instantiate explicit classes.
 *
 * Revision 1.30  2001/05/02 16:22:21  rogerh
 * Add IsAllow() for a single capability to check if it is in the
 * capabilities set. This fixes the bug where OpenH323 would accept
 * incoming H261 video even when told not to accept it.
 *
 * Revision 1.29  2001/04/12 03:22:44  robertj
 * Fixed fast start checking of returned OLC frame count to use minimum
 *   of user setting and remotes maximum limitation, Was always just
 *   sending whatever the remote said it could do.
 *
 * Revision 1.28  2001/03/16 23:00:22  robertj
 * Improved validation of codec selection against capability set, thanks Chris Purvis.
 *
 * Revision 1.27  2001/03/06 04:44:47  robertj
 * Fixed problem where could send capability set twice. This should not be
 *   a problem except when talking to another broken stack, eg Cisco routers.
 *
 * Revision 1.26  2001/02/09 05:13:55  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.25  2001/01/25 07:27:16  robertj
 * Major changes to add more flexible OpalMediaFormat class to normalise
 *   all information about media types, especially codecs.
 *
 * Revision 1.24  2001/01/16 03:14:01  craigs
 * Changed nonstanard capability Compare functions to not assert
 * if compared to other capability types
 *
 * Revision 1.23  2001/01/09 23:05:24  robertj
 * Fixed inability to have 2 non standard codecs in capability table.
 *
 * Revision 1.22  2001/01/02 07:50:46  robertj
 * Fixed inclusion of arrays (with bad size) in TCS=0 pdu, thanks Yura Aksyonov.
 *
 * Revision 1.21  2000/12/19 22:32:26  dereks
 * Removed MSVC warning about unused parameter
 *
 * Revision 1.20  2000/11/08 04:50:22  craigs
 * Changed capability reorder function to reorder all capabilities matching
 * preferred order, rather than just the first
 *
 * Revision 1.19  2000/10/16 08:50:08  robertj
 * Added single function to add all UserInput capability types.
 *
 * Revision 1.18  2000/10/13 03:43:29  robertj
 * Added clamping to avoid ever setting incorrect tx frame count.
 *
 * Revision 1.17  2000/10/13 02:20:32  robertj
 * Fixed capability clone so gets all fields including those in ancestor.
 *
 * Revision 1.16  2000/08/23 14:27:04  craigs
 * Added prototype support for Microsoft GSM codec
 *
 * Revision 1.15  2000/07/13 12:30:46  robertj
 * Fixed problems with fast start frames per packet adjustment.
 *
 * Revision 1.14  2000/07/12 10:25:37  robertj
 * Renamed all codecs so obvious whether software or hardware.
 *
 * Revision 1.13  2000/07/10 16:03:02  robertj
 * Started fixing capability set merging, still more to do.
 *
 * Revision 1.12  2000/07/04 01:16:49  robertj
 * Added check for capability allowed in "combinations" set, still needs more done yet.
 *
 * Revision 1.11  2000/07/02 14:08:43  craigs
 * Fixed problem with removing capabilities based on wildcard
 *
 * Revision 1.10  2000/06/03 03:16:39  robertj
 * Fixed using the wrong capability table (should be connections) for some operations.
 *
 * Revision 1.9  2000/05/30 06:53:48  robertj
 * Fixed bug where capability numbers in duplicate table are not identical (should be!).
 *
 * Revision 1.8  2000/05/23 11:32:37  robertj
 * Rewrite of capability table to combine 2 structures into one and move functionality into that class
 *    allowing some normalisation of usage across several applications.
 * Changed H323Connection so gets a copy of capabilities instead of using endponts, allows adjustments
 *    to be done depending on the remote client application.
 *
 * Revision 1.7  2000/05/10 04:05:34  robertj
 * Changed capabilities so has a function to get name of codec, instead of relying on PrintOn.
 *
 * Revision 1.6  2000/05/02 04:32:26  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.5  2000/04/05 19:01:12  robertj
 * Added function so can change desired transmit packet size.
 *
 * Revision 1.4  2000/03/22 01:29:43  robertj
 * Fixed default "frame" size for audio codecs, caused crash using G.711
 *
 * Revision 1.3  2000/03/21 03:06:50  robertj
 * Changes to make RTP TX of exact numbers of frames in some codecs.
 *
 * Revision 1.2  2000/02/16 03:24:27  robertj
 * Fixed bug in clamping maximum transmit packet size in G.711 capabilities.
 *
 * Revision 1.1  1999/12/23 23:02:36  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 */

#ifdef __GNUC__
#pragma implementation "h323caps.h"
#endif

#include <ptlib.h>

#include "h323caps.h"
#include "h323ep.h"
#include "h323pdu.h"
#include "codec/opalplugin.h"
#include "mediafmt.h"

#include <algorithm>

#define DEFINE_G711_CAPABILITY(cls, code, capName) \
class cls : public H323_G711Capability { \
  public: \
    cls() : H323_G711Capability(code) { } \
}; \
H323_REGISTER_CAPABILITY(cls, capName) \

#ifndef NO_H323_AUDIO_CODECS

DEFINE_G711_CAPABILITY(H323_G711ALaw64Capability, H323_G711Capability::ALaw, "G.711-ALaw-64k{sw}")
DEFINE_G711_CAPABILITY(H323_G711uLaw64Capability, H323_G711Capability::muLaw, "G.711-uLaw-64k{sw}")

#endif

#define new PNEW
// #undef P_HAS_QOS



#if PTRACING
ostream & operator<<(ostream & o , H323Capability::MainTypes t)
{
  const char * const names[] = {
    "Audio", "Video", "Data", "UserInput"
  };
  return o << names[t];
}

ostream & operator<<(ostream & o , H323Capability::CapabilityDirection d)
{
  const char * const names[] = {
    "Unknown", "Receive", "Transmit", "ReceiveAndTransmit", "NoDirection"
  };
  return o << names[d];
}
#endif


/////////////////////////////////////////////////////////////////////////////

H323Capability::H323Capability()
{
  assignedCapabilityNumber = 0; // Unassigned
  capabilityDirection = e_Unknown;
  rtpPayloadType = RTP_DataFrame::IllegalPayloadType;
}


PObject::Comparison H323Capability::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, H323Capability), PInvalidCast);
  const H323Capability & other = (const H323Capability &)obj;

  int mt = GetMainType();
  int omt = other.GetMainType();
  if (mt < omt)
    return LessThan;
  if (mt > omt)
    return GreaterThan;

  int st = GetSubType();
  int ost = other.GetSubType();
  if (st < ost)
    return LessThan;
  if (st > ost)
    return GreaterThan;

  return EqualTo;
}


void H323Capability::PrintOn(ostream & strm) const
{
  strm << GetFormatName();
  if (assignedCapabilityNumber != 0)
    strm << " <" << assignedCapabilityNumber << '>';
}


H323Capability * H323Capability::Create(const PString & name)
{
  H323Capability * cap = H323CapabilityFactory::CreateInstance(name);
  if(cap == NULL && name.Right(4) != "{sw}")
    cap = H323CapabilityFactory::CreateInstance(name+"{sw}");
  if(cap == NULL && name.Right(4) == "{sw}" && name.GetLength() > 4)
    cap = H323CapabilityFactory::CreateInstance(name.Left(name.GetLength()-4));
  if(cap == NULL)
    return NULL;

  return (H323Capability *)cap->Clone();
}


unsigned H323Capability::GetDefaultSessionID() const
{
  return 0;
}


void H323Capability::SetTxFramesInPacket(unsigned /*frames*/)
{
}


unsigned H323Capability::GetTxFramesInPacket() const
{
  return 1;
}


unsigned H323Capability::GetRxFramesInPacket() const
{
  return 1;
}


BOOL H323Capability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return subTypePDU.GetTag() == GetSubType();
}


BOOL H323Capability::OnReceivedPDU(const H245_Capability & cap)
{
  switch (cap.GetTag()) {
    case H245_Capability::e_receiveVideoCapability:
    case H245_Capability::e_receiveAudioCapability:
    case H245_Capability::e_receiveDataApplicationCapability:
    case H245_Capability::e_h233EncryptionReceiveCapability:
    case H245_Capability::e_receiveUserInputCapability:
      capabilityDirection = e_Receive;
      break;

    case H245_Capability::e_transmitVideoCapability:
    case H245_Capability::e_transmitAudioCapability:
    case H245_Capability::e_transmitDataApplicationCapability:
    case H245_Capability::e_h233EncryptionTransmitCapability:
    case H245_Capability::e_transmitUserInputCapability:
      capabilityDirection = e_Transmit;
      break;

    case H245_Capability::e_receiveAndTransmitVideoCapability:
    case H245_Capability::e_receiveAndTransmitAudioCapability:
    case H245_Capability::e_receiveAndTransmitDataApplicationCapability:
    case H245_Capability::e_receiveAndTransmitUserInputCapability:
      capabilityDirection = e_ReceiveAndTransmit;
      break;

    case H245_Capability::e_conferenceCapability:
	case H245_Capability::e_genericControlCapability:
    case H245_Capability::e_h235SecurityCapability:
    case H245_Capability::e_maxPendingReplacementFor:
      capabilityDirection = e_NoDirection;
  }

  return TRUE;
}


BOOL H323Capability::IsUsable(const H323Connection &) const
{
  return TRUE;
}

void H323Capability::SetMediaFormatOptionInteger(const PString &name, int val)
{
 mediaFormat.SetOptionInteger(name, val);
}

const OpalMediaFormat & H323Capability::GetMediaFormat() const
{
  return PRemoveConst(H323Capability, this)->GetWritableMediaFormat();
}


OpalMediaFormat & H323Capability::GetWritableMediaFormat()
{
  if (mediaFormat.IsEmpty()) {
    PString name = GetFormatName();
    name.Delete(name.FindLast('{'), 4);
    mediaFormat = OpalMediaFormat(name);
  }
  return mediaFormat;
}


/////////////////////////////////////////////////////////////////////////////

H323RealTimeCapability::H323RealTimeCapability()
{
    rtpqos = NULL;
}

H323RealTimeCapability::H323RealTimeCapability(const H323RealTimeCapability & rtc)
  : H323Capability(rtc)
{
#if P_HAS_QOS
  if (rtc.rtpqos != NULL) 
    rtpqos  = new RTP_QOS(*rtc.rtpqos);
  else
#endif
    rtpqos = NULL;
}

H323RealTimeCapability::~H323RealTimeCapability()
{
#if P_HAS_QOS
  delete rtpqos;
#endif
}

void H323RealTimeCapability::AttachQoS(RTP_QOS * _rtpqos)
{
#if P_HAS_QOS
  delete rtpqos;
#endif
    
  rtpqos = _rtpqos;
}

H323Channel * H323RealTimeCapability::CreateChannel(H323Connection & connection,
                                                    H323Channel::Directions dir,
                                                    unsigned sessionID,
                                 const H245_H2250LogicalChannelParameters * param) const
{
  return connection.CreateRealTimeLogicalChannel(*this, dir, sessionID, param, rtpqos);
}


/////////////////////////////////////////////////////////////////////////////

H323NonStandardCapabilityInfo::H323NonStandardCapabilityInfo(CompareFuncType _compareFunc,
                                                             const BYTE * dataPtr,
                                                             PINDEX dataSize)
  :
    t35CountryCode(0),
    t35Extension(0),
    manufacturerCode(0),
    nonStandardData(dataPtr, dataSize == 0 && dataPtr != NULL
                                 ? strlen((const char *)dataPtr) : dataSize),
    comparisonOffset(0),
    comparisonLength(0),
    compareFunc(_compareFunc)
{
}

H323NonStandardCapabilityInfo::H323NonStandardCapabilityInfo(const BYTE * dataPtr,
                                                             PINDEX dataSize,
                                                             PINDEX _offset,
                                                             PINDEX _len)
  : t35CountryCode(H323EndPoint::defaultT35CountryCode),
    t35Extension(H323EndPoint::defaultT35Extension),
    manufacturerCode(H323EndPoint::defaultManufacturerCode),
    nonStandardData(dataPtr, dataSize == 0 && dataPtr != NULL
                                 ? strlen((const char *)dataPtr) : dataSize),
    comparisonOffset(_offset),
    comparisonLength(_len),
    compareFunc(NULL)
{
}


H323NonStandardCapabilityInfo::H323NonStandardCapabilityInfo(const PString & _oid,
                                                             const BYTE * dataPtr,
                                                             PINDEX dataSize,
                                                             PINDEX _offset,
                                                             PINDEX _len)
  : oid(_oid),
    nonStandardData(dataPtr, dataSize == 0 && dataPtr != NULL
                                 ? strlen((const char *)dataPtr) : dataSize),
    comparisonOffset(_offset),
    comparisonLength(_len),
    compareFunc(NULL)
{
}


H323NonStandardCapabilityInfo::H323NonStandardCapabilityInfo(BYTE country,
                                                             BYTE extension,
                                                             WORD manufacturer,
                                                             const BYTE * dataPtr,
                                                             PINDEX dataSize,
                                                             PINDEX _offset,
                                                             PINDEX _len)
  : t35CountryCode(country),
    t35Extension(extension),
    manufacturerCode(manufacturer),
    nonStandardData(dataPtr, dataSize == 0 && dataPtr != NULL
                                 ? strlen((const char *)dataPtr) : dataSize),
    comparisonOffset(_offset),
    comparisonLength(_len),
    compareFunc(NULL)
{
}


H323NonStandardCapabilityInfo::~H323NonStandardCapabilityInfo()
{
}


BOOL H323NonStandardCapabilityInfo::OnSendingPDU(PBYTEArray & data) const
{
  data = nonStandardData;
  return data.GetSize() > 0;
}


BOOL H323NonStandardCapabilityInfo::OnReceivedPDU(const PBYTEArray & data)
{
  if (CompareData(data) != PObject::EqualTo)
    return FALSE;

  nonStandardData = data;
  return TRUE;
}


BOOL H323NonStandardCapabilityInfo::OnSendingNonStandardPDU(PASN_Choice & pdu,
                                                            unsigned nonStandardTag) const
{
  PBYTEArray data;
  if (!OnSendingPDU(data)) 
    return FALSE;

  pdu.SetTag(nonStandardTag);
  H245_NonStandardParameter & param = (H245_NonStandardParameter &)pdu.GetObject();

  if (!oid) {
    param.m_nonStandardIdentifier.SetTag(H245_NonStandardIdentifier::e_object);
    PASN_ObjectId & nonStandardIdentifier = param.m_nonStandardIdentifier;
    nonStandardIdentifier = oid;
  }
  else {
    param.m_nonStandardIdentifier.SetTag(H245_NonStandardIdentifier::e_h221NonStandard);
    H245_NonStandardIdentifier_h221NonStandard & h221 = param.m_nonStandardIdentifier;
    h221.m_t35CountryCode = (unsigned)t35CountryCode;
    h221.m_t35Extension = (unsigned)t35Extension;
    h221.m_manufacturerCode = (unsigned)manufacturerCode;
  }

  param.m_data = data;
  return data.GetSize() > 0;
}


BOOL H323NonStandardCapabilityInfo::OnReceivedNonStandardPDU(const PASN_Choice & pdu,
                                                             unsigned nonStandardTag)
{
  if (pdu.GetTag() != nonStandardTag)
    return FALSE;

  const H245_NonStandardParameter & param = (const H245_NonStandardParameter &)pdu.GetObject();
  if (CompareParam(param) != PObject::EqualTo)
    return FALSE;

  return OnReceivedPDU(param.m_data);
}


BOOL H323NonStandardCapabilityInfo::IsMatch(const H245_NonStandardParameter & param) const
{
  return CompareParam(param) == PObject::EqualTo && CompareData(param.m_data) == PObject::EqualTo;
}


PObject::Comparison H323NonStandardCapabilityInfo::CompareParam(const H245_NonStandardParameter & param) const
{

  if (compareFunc != NULL) {

    PluginCodec_H323NonStandardCodecData compareData;

    PString objectId;
    if (param.m_nonStandardIdentifier.GetTag() == H245_NonStandardIdentifier::e_object) {
      const PASN_ObjectId & nonStandardIdentifier = param.m_nonStandardIdentifier;
      objectId = nonStandardIdentifier.AsString();
      compareData.objectId = objectId;
    } else {
      const H245_NonStandardIdentifier_h221NonStandard & h221 = param.m_nonStandardIdentifier;
      compareData.objectId         = NULL;
      compareData.t35CountryCode   = (unsigned char)h221.m_t35CountryCode;
      compareData.t35Extension     = (unsigned char)h221.m_t35Extension;
      compareData.manufacturerCode = (unsigned short)h221.m_manufacturerCode;
    }
    const PBYTEArray & data = param.m_data;
    compareData.data       = (const unsigned char *)data;
    compareData.dataLength = data.GetSize();
    return (PObject::Comparison)(*compareFunc)(&compareData);
  }

  if (!oid) {
    if (param.m_nonStandardIdentifier.GetTag() != H245_NonStandardIdentifier::e_object)
      return PObject::LessThan;

    const PASN_ObjectId & nonStandardIdentifier = param.m_nonStandardIdentifier;
    PObject::Comparison cmp = oid.Compare(nonStandardIdentifier.AsString());
    if (cmp != PObject::EqualTo)
      return cmp;
  }
  else {
    if (param.m_nonStandardIdentifier.GetTag() != H245_NonStandardIdentifier::e_h221NonStandard)
      return PObject::LessThan;

    const H245_NonStandardIdentifier_h221NonStandard & h221 = param.m_nonStandardIdentifier;

    if (h221.m_t35CountryCode < (unsigned)t35CountryCode)
      return PObject::LessThan;
    if (h221.m_t35CountryCode > (unsigned)t35CountryCode)
      return PObject::GreaterThan;

    if (h221.m_t35Extension < (unsigned)t35Extension)
      return PObject::LessThan;
    if (h221.m_t35Extension > (unsigned)t35Extension)
      return PObject::GreaterThan;

    if (h221.m_manufacturerCode < (unsigned)manufacturerCode)
      return PObject::LessThan;
    if (h221.m_manufacturerCode > (unsigned)manufacturerCode)
      return PObject::GreaterThan;

  }

  return CompareData(param.m_data);
}


PObject::Comparison H323NonStandardCapabilityInfo::CompareInfo(const H323NonStandardCapabilityInfo & other) const
{
  if (compareFunc != NULL) {

    PluginCodec_H323NonStandardCodecData compareData;

    PString objectId;
    if (!other.oid.IsEmpty())
      compareData.objectId = other.oid;
    else {
      compareData.objectId         = NULL;
      compareData.t35CountryCode   = other.t35CountryCode;
      compareData.t35Extension     = other.t35Extension;
      compareData.manufacturerCode = other.manufacturerCode;

    }
    compareData.data       = (const unsigned char *)other.nonStandardData;
    compareData.dataLength = other.nonStandardData.GetSize();

    return (*compareFunc)(&compareData);
  }

  if (!oid) {
    if (other.oid.IsEmpty())
      return PObject::LessThan;

    PObject::Comparison cmp = oid.Compare(other.oid);
    if (cmp != PObject::EqualTo)
      return cmp;
  }
  else {
    if (other.t35CountryCode < t35CountryCode)
      return PObject::LessThan;
    if (other.t35CountryCode > t35CountryCode)
      return PObject::GreaterThan;

    if (other.t35Extension < t35Extension)
      return PObject::LessThan;
    if (other.t35Extension > t35Extension)
      return PObject::GreaterThan;

    if (other.manufacturerCode < manufacturerCode)
      return PObject::LessThan;
    if (other.manufacturerCode > manufacturerCode)
      return PObject::GreaterThan;
  }

  return CompareData(other.nonStandardData);
}


PObject::Comparison H323NonStandardCapabilityInfo::CompareData(const PBYTEArray & data) const
{
 PINDEX cOffset = comparisonOffset;
 PINDEX len = comparisonLength;
 if (t35CountryCode == 181 && t35Extension == 0 && manufacturerCode == 21324)
    { cOffset=20; len-=20; }

  if (cOffset >= nonStandardData.GetSize())
    return PObject::LessThan;
  if (cOffset >= data.GetSize())
    return PObject::GreaterThan;

  if (cOffset+len > nonStandardData.GetSize())
    len = nonStandardData.GetSize() - cOffset;

  if (cOffset+len > data.GetSize())
    return PObject::GreaterThan;

  int cmp = memcmp((const BYTE *)nonStandardData + cOffset,
                   (const BYTE *)data + cOffset,
                   len);

/*
  if (comparisonOffset >= nonStandardData.GetSize())
    return PObject::LessThan;
  if (comparisonOffset >= data.GetSize())
    return PObject::GreaterThan;

  PINDEX len = comparisonLength;
  if (comparisonOffset+len > nonStandardData.GetSize())
    len = nonStandardData.GetSize() - comparisonOffset;

  if (comparisonOffset+len > data.GetSize())
    return PObject::GreaterThan;

  int cmp = memcmp((const BYTE *)nonStandardData + comparisonOffset,
                   (const BYTE *)data + comparisonOffset,
                   len);
*/                   
  if (cmp < 0)
    return PObject::LessThan;
  if (cmp > 0)
    return PObject::GreaterThan;
  return PObject::EqualTo;
}


/////////////////////////////////////////////////////////////////////////////

H323GenericCapabilityInfo::H323GenericCapabilityInfo(const PString & standardId, PINDEX bitRate)
	: maxBitRate(bitRate)
{
  identifier = new H245_CapabilityIdentifier(H245_CapabilityIdentifier::e_standard);
  PASN_ObjectId & object_id = *identifier;
    object_id = standardId;
}


H323GenericCapabilityInfo::H323GenericCapabilityInfo(const H323GenericCapabilityInfo & obj)
  : maxBitRate(obj.maxBitRate)
{
  identifier = new H245_CapabilityIdentifier(*obj.identifier);
}


H323GenericCapabilityInfo::~H323GenericCapabilityInfo()
{
  delete identifier;
}


BOOL H323GenericCapabilityInfo::OnSendingGenericPDU(H245_GenericCapability & pdu,
                                                    const OpalMediaFormat & mediaFormat,
                                                    H323Capability::CommandType type) const
{
  pdu.m_capabilityIdentifier = *identifier;

#ifndef NO_H323_VIDEO
  unsigned bitRate = maxBitRate != 0 ? maxBitRate : ((mediaFormat.GetOptionInteger(OpalVideoFormat::MaxBitRateOption)+99)/100);
#else
  unsigned bitRate = maxBitRate;
#endif
  if (bitRate != 0) {
    pdu.IncludeOptionalField(H245_GenericCapability::e_maxBitRate);
    pdu.m_maxBitRate = bitRate;
  }

  for (PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++) {
    const OpalMediaOption & option = mediaFormat.GetOption(i);
    OpalMediaOption::H245GenericInfo genericInfo = option.GetH245Generic();
    if (genericInfo.mode == OpalMediaOption::H245GenericInfo::None)
      continue;
    switch (type) {
      case H323Capability::e_TCS :
        if (genericInfo.excludeTCS)
          continue;
        break;
      case H323Capability::e_OLC :
        if (genericInfo.excludeOLC)
          continue;
        break;
      case H323Capability::e_ReqMode :
        if (genericInfo.excludeReqMode)
          continue;
        break;
    }
    
    H245_GenericParameter * param = new H245_GenericParameter;

    param->m_parameterIdentifier.SetTag(H245_ParameterIdentifier::e_standard);
    (PASN_Integer &)param->m_parameterIdentifier = genericInfo.ordinal;

    if (PIsDescendant(&option, OpalMediaOptionBoolean)) {
      if (!((const OpalMediaOptionBoolean &)option).GetValue()) {
        delete param;
        continue; // Do not include a logical at all if it is false
      }
      param->m_parameterValue.SetTag(H245_ParameterValue::e_logical);
    }
    else if (PIsDescendant(&option, OpalMediaOptionUnsigned)) {
      if (!((const OpalMediaOptionUnsigned &)option).GetValue()) {
        delete param;
        continue; // Do not include a unsigned at all if it is equal to zero
      }
      unsigned tag;
      switch (genericInfo.integerType) {
        default :
        case OpalMediaOption::H245GenericInfo::UnsignedInt :
          tag = option.GetMerge() == OpalMediaOption::MinMerge ? H245_ParameterValue::e_unsignedMin : H245_ParameterValue::e_unsignedMax;
          break;

        case OpalMediaOption::H245GenericInfo::Unsigned32 :
          tag = option.GetMerge() == OpalMediaOption::MinMerge ? H245_ParameterValue::e_unsigned32Min : H245_ParameterValue::e_unsigned32Max;
          break;

        case OpalMediaOption::H245GenericInfo::BooleanArray :
          tag = H245_ParameterValue::e_booleanArray;
          break;
      }
       param->m_parameterValue.SetTag(tag);
       (PASN_Integer &)param->m_parameterValue = ((const OpalMediaOptionUnsigned &)option).GetValue();
    }
    else {
      param->m_parameterValue.SetTag(H245_ParameterValue::e_octetString);
      PASN_OctetString & octetString = param->m_parameterValue;
      if (PIsDescendant(&option, OpalMediaOptionOctets))
        octetString = ((const OpalMediaOptionOctets &)option).GetValue();
      else
        octetString = option.AsString();
    }

    if (genericInfo.mode == OpalMediaOption::H245GenericInfo::Collapsing) {
      pdu.IncludeOptionalField(H245_GenericCapability::e_collapsing);
      pdu.m_collapsing.Append(param);
    }
    else {
      pdu.IncludeOptionalField(H245_GenericCapability::e_nonCollapsing);
      pdu.m_nonCollapsing.Append(param);
    }
  }

    return TRUE;
}

BOOL H323GenericCapabilityInfo::OnReceivedGenericPDU(OpalMediaFormat & mediaFormat,
                                                     const H245_GenericCapability & pdu,
                                                     H323Capability::CommandType type)
{

  if (pdu.m_capabilityIdentifier != *identifier)
    return FALSE;

  if (pdu.HasOptionalField(H245_GenericCapability::e_maxBitRate)) {
    maxBitRate = pdu.m_maxBitRate;
#ifndef NO_H323_VIDEO
    mediaFormat.SetOptionInteger(OpalVideoFormat::MaxBitRateOption, maxBitRate*100);
#else
    mediaFormat.SetOptionInteger(maxBitRate, maxBitRate*100);
#endif
  }

  for (PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++) {
    const OpalMediaOption & option = mediaFormat.GetOption(i);
    OpalMediaOption::H245GenericInfo genericInfo = option.GetH245Generic();
    if (genericInfo.mode == OpalMediaOption::H245GenericInfo::None)
      continue;
    switch (type) {
      case H323Capability::e_TCS :
        if (genericInfo.excludeTCS)
          continue;
        break;
      case H323Capability::e_OLC :
        if (genericInfo.excludeOLC)
          continue;
        break;
      case H323Capability::e_ReqMode :
        if (genericInfo.excludeReqMode)
          continue;
        break;
    }

    const H245_ArrayOf_GenericParameter * params;
    if (genericInfo.mode == OpalMediaOption::H245GenericInfo::Collapsing) {
      if (!pdu.HasOptionalField(H245_GenericCapability::e_collapsing))
        continue;
      params = &pdu.m_collapsing;
    }
    else {
      if (!pdu.HasOptionalField(H245_GenericCapability::e_nonCollapsing))
        continue;
      params = &pdu.m_nonCollapsing;
    }

    if (PIsDescendant(&option, OpalMediaOptionBoolean))
      ((OpalMediaOptionBoolean &)option).SetValue(false);

    for (PINDEX j = 0; j < params->GetSize(); j++) {
      const H245_GenericParameter & param = (*params)[j];
      if (param.m_parameterIdentifier.GetTag() == H245_ParameterIdentifier::e_standard &&
                         (const PASN_Integer &)param.m_parameterIdentifier == genericInfo.ordinal) {
        if (PIsDescendant(&option, OpalMediaOptionBoolean)) {
          if (param.m_parameterValue.GetTag() == H245_ParameterValue::e_logical) {
            ((OpalMediaOptionBoolean &)option).SetValue(true);
            break;
          }
        }
        else if (PIsDescendant(&option, OpalMediaOptionUnsigned)) {
          unsigned tag;
          switch (genericInfo.integerType) {
            default :
            case OpalMediaOption::H245GenericInfo::UnsignedInt :
              tag = option.GetMerge() == OpalMediaOption::MinMerge ? H245_ParameterValue::e_unsignedMin : H245_ParameterValue::e_unsignedMax;
              break;
 
            case OpalMediaOption::H245GenericInfo::Unsigned32 :
              tag = option.GetMerge() == OpalMediaOption::MinMerge ? H245_ParameterValue::e_unsigned32Min : H245_ParameterValue::e_unsigned32Max;
              break;

            case OpalMediaOption::H245GenericInfo::BooleanArray :
              tag = H245_ParameterValue::e_booleanArray;
              break;
          }

          if (param.m_parameterValue.GetTag() == tag) {
            ((OpalMediaOptionUnsigned &)option).SetValue((const PASN_Integer &)param.m_parameterValue);
            break;
          }
        }
        else {
          if (param.m_parameterValue.GetTag() == H245_ParameterValue::e_octetString) {
            const PASN_OctetString & octetString = param.m_parameterValue;
            if (PIsDescendant(&option, OpalMediaOptionOctets))
              ((OpalMediaOptionOctets &)option).SetValue(octetString);
            else
              ((OpalMediaOption &)option).FromString(octetString.AsString());
            break;
    }
    }

        PTRACE(2, "Invalid generic parameter type (" << param.m_parameterValue.GetTagName()
               << ") for option \"" << option.GetName() << "\" (" << option.GetClass() << ')');
      }
    }
    }

    return TRUE;
}

BOOL H323GenericCapabilityInfo::IsMatch(const H245_GenericCapability & param) const
{
  return param.m_capabilityIdentifier == *identifier;
}

PObject::Comparison H323GenericCapabilityInfo::CompareInfo(const H323GenericCapabilityInfo & obj) const
{
  return identifier->Compare(*obj.identifier);
}


/////////////////////////////////////////////////////////////////////////////

#ifndef NO_H323_AUDIO_CODECS
int H323AudioCapability::DSCPvalue = PQoS::guaranteedDSCP;
H323AudioCapability::H323AudioCapability(unsigned rx, unsigned tx)
{
  rxFramesInPacket = rx;
  txFramesInPacket = tx;

#if P_HAS_QOS
// Set to G.729 Settings Avg 56kb/s Peek 110 kb/s
	rtpqos = new RTP_QOS;
	rtpqos->dataQoS.SetWinServiceType(SERVICETYPE_GUARANTEED);
	rtpqos->dataQoS.SetAvgBytesPerSec(7000);
	rtpqos->dataQoS.SetMaxFrameBytes(680);
	rtpqos->dataQoS.SetPeakBytesPerSec(13750);
	rtpqos->dataQoS.SetDSCP(DSCPvalue);

	rtpqos->ctrlQoS.SetWinServiceType(SERVICETYPE_CONTROLLEDLOAD);
	rtpqos->ctrlQoS.SetDSCP(PQoS::controlledLoadDSCP); 
#endif
}


H323Capability::MainTypes H323AudioCapability::GetMainType() const
{
  return e_Audio;
}


unsigned H323AudioCapability::GetDefaultSessionID() const
{
  return RTP_Session::DefaultAudioSessionID;
}


void H323AudioCapability::SetTxFramesInPacket(unsigned frames)
{
  PAssert(frames > 0, PInvalidParameter);
  if (frames > 256)
    txFramesInPacket = 256;
  else
    txFramesInPacket = frames;
}


unsigned H323AudioCapability::GetTxFramesInPacket() const
{
  return txFramesInPacket;
}


unsigned H323AudioCapability::GetRxFramesInPacket() const
{
  return rxFramesInPacket;
}

void H323AudioCapability::SetDSCPvalue(int newValue) 
{ 
	if (newValue < 64)
		DSCPvalue = newValue;
}

int H323AudioCapability::GetDSCPvalue() 
{ 
	return DSCPvalue; 
} 

BOOL H323AudioCapability::OnSendingPDU(H245_Capability & cap) const
{
  switch (capabilityDirection) {
    case e_Transmit:
      cap.SetTag(H245_Capability::e_transmitAudioCapability);
      break;
    case e_ReceiveAndTransmit:
      cap.SetTag(H245_Capability::e_receiveAndTransmitAudioCapability);
      break;
    case e_Receive :
    default:
      cap.SetTag(H245_Capability::e_receiveAudioCapability);
  }
  return OnSendingPDU((H245_AudioCapability &)cap, rxFramesInPacket, e_TCS);
}


BOOL H323AudioCapability::OnSendingPDU(H245_DataType & dataType) const
{
  dataType.SetTag(H245_DataType::e_audioData);
  return OnSendingPDU((H245_AudioCapability &)dataType, txFramesInPacket, e_OLC);
}


BOOL H323AudioCapability::OnSendingPDU(H245_ModeElement & mode) const
{
  mode.m_type.SetTag(H245_ModeElementType::e_audioMode);
  return OnSendingPDU((H245_AudioMode &)mode.m_type);
}


BOOL H323AudioCapability::OnSendingPDU(H245_AudioCapability & pdu,
                                       unsigned packetSize) const
{
  pdu.SetTag(GetSubType());

  // Set the maximum number of frames
  PASN_Integer & value = pdu;
  value = packetSize;
  return TRUE;
}


BOOL H323AudioCapability::OnSendingPDU(H245_AudioCapability & pdu,
                                       unsigned packetSize,
                                       CommandType) const
{
  return OnSendingPDU(pdu, packetSize);
}


BOOL H323AudioCapability::OnSendingPDU(H245_AudioMode & pdu) const
{
  static const H245_AudioMode::Choices AudioTable[] = {
    H245_AudioMode::e_nonStandard,
    H245_AudioMode::e_g711Alaw64k,
    H245_AudioMode::e_g711Alaw56k,
    H245_AudioMode::e_g711Ulaw64k,
    H245_AudioMode::e_g711Ulaw56k,
    H245_AudioMode::e_g722_64k,
    H245_AudioMode::e_g722_56k,
    H245_AudioMode::e_g722_48k,
    H245_AudioMode::e_g7231,
    H245_AudioMode::e_g728,
    H245_AudioMode::e_g729,
    H245_AudioMode::e_g729AnnexA,
    H245_AudioMode::e_is11172AudioMode,
    H245_AudioMode::e_is13818AudioMode,
    H245_AudioMode::e_g729wAnnexB,
    H245_AudioMode::e_g729AnnexAwAnnexB,
    H245_AudioMode::e_g7231AnnexCMode,
    H245_AudioMode::e_gsmFullRate,
    H245_AudioMode::e_gsmHalfRate,
    H245_AudioMode::e_gsmEnhancedFullRate,
    H245_AudioMode::e_genericAudioMode,
    H245_AudioMode::e_g729Extensions
  };

  unsigned subType = GetSubType();
  if (subType >= PARRAYSIZE(AudioTable))
    return FALSE;

  pdu.SetTag(AudioTable[subType]);
  return TRUE;
}


BOOL H323AudioCapability::OnReceivedPDU(const H245_Capability & cap)
{
  H323Capability::OnReceivedPDU(cap);

  if (cap.GetTag() != H245_Capability::e_receiveAudioCapability &&
      cap.GetTag() != H245_Capability::e_receiveAndTransmitAudioCapability)
    return FALSE;

  unsigned packetSize = txFramesInPacket;
  if (!OnReceivedPDU((const H245_AudioCapability &)cap, packetSize, e_TCS))
    return FALSE;

  // Clamp our transmit size to maximum allowed
  if (txFramesInPacket > packetSize) {
    PTRACE(4, "H323\tCapability tx frames reduced from "
           << txFramesInPacket << " to " << packetSize);
    txFramesInPacket = packetSize;
  }
  else {
    PTRACE(4, "H323\tCapability tx frames left at "
           << txFramesInPacket << " as remote allows " << packetSize);
  }

  return TRUE;
}


BOOL H323AudioCapability::OnReceivedPDU(const H245_DataType & dataType, BOOL receiver)
{
  if (dataType.GetTag() != H245_DataType::e_audioData)
    return FALSE;

  unsigned & xFramesInPacket = receiver ? rxFramesInPacket : txFramesInPacket;
  unsigned packetSize = xFramesInPacket;
  if (!OnReceivedPDU((const H245_AudioCapability &)dataType, packetSize, e_OLC))
    return FALSE;

  // Clamp our transmit size to maximum allowed
  if (xFramesInPacket > packetSize) {
    PTRACE(4, "H323\tCapability " << (receiver ? 'r' : 't') << "x frames reduced from "
           << xFramesInPacket << " to " << packetSize);
    xFramesInPacket = packetSize;
  }
  else {
    PTRACE(4, "H323\tCapability " << (receiver ? 'r' : 't') << "x frames left at "
           << xFramesInPacket << " as remote allows " << packetSize);
  }

  return TRUE;
}


BOOL H323AudioCapability::OnReceivedPDU(const H245_AudioCapability & pdu,
                                        unsigned & packetSize)
{
  if (pdu.GetTag() != GetSubType())
    return FALSE;

  const PASN_Integer & value = pdu;

  // Get the maximum number of frames
  packetSize = value;
  return TRUE;
}


BOOL H323AudioCapability::OnReceivedPDU(const H245_AudioCapability & pdu,
                                        unsigned & packetSize,
                                        CommandType)
{
  return OnReceivedPDU(pdu, packetSize);
}


/////////////////////////////////////////////////////////////////////////////

H323GenericAudioCapability::H323GenericAudioCapability(
      unsigned max,
      unsigned desired,
      const PString &standardId,
      PINDEX maxBitRate)
  : H323AudioCapability(max, desired),
    H323GenericCapabilityInfo(standardId, maxBitRate)
{
}

PObject::Comparison H323GenericAudioCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323GenericAudioCapability))
    return LessThan;

  return CompareInfo((const H323GenericAudioCapability &)obj);
}


unsigned H323GenericAudioCapability::GetSubType() const
{
  return H245_AudioCapability::e_genericAudioCapability;
}


BOOL H323GenericAudioCapability::OnSendingPDU(H245_AudioCapability & pdu, unsigned, CommandType type) const
{
  pdu.SetTag(H245_AudioCapability::e_genericAudioCapability);
  return OnSendingGenericPDU(pdu, GetMediaFormat(), type);
}

BOOL H323GenericAudioCapability::OnSendingPDU(H245_AudioMode & pdu) const
{
  pdu.SetTag(H245_VideoMode::e_genericVideoMode);
  return OnSendingGenericPDU(pdu, GetMediaFormat(), e_ReqMode);
}

BOOL H323GenericAudioCapability::OnReceivedPDU(const H245_AudioCapability & pdu, unsigned &, CommandType type)
{
  if( pdu.GetTag() != H245_AudioCapability::e_genericAudioCapability)
    return FALSE;
  return OnReceivedGenericPDU(GetWritableMediaFormat(), pdu, type);
}

BOOL H323GenericAudioCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) &&
         H323GenericCapabilityInfo::IsMatch((const H245_GenericCapability &)subTypePDU.GetObject());
}



/////////////////////////////////////////////////////////////////////////////

H323NonStandardAudioCapability::H323NonStandardAudioCapability(
      unsigned max,
      unsigned desired,
      H323NonStandardCapabilityInfo::CompareFuncType compareFunc,
      const BYTE * fixedData,
      PINDEX dataSize
    )
  : H323AudioCapability(max, desired),
    H323NonStandardCapabilityInfo(compareFunc, fixedData, dataSize)
{
}

H323NonStandardAudioCapability::H323NonStandardAudioCapability(unsigned max,
                                                               unsigned desired,
                                                               const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323AudioCapability(max, desired),
    H323NonStandardCapabilityInfo(fixedData, dataSize, offset, length)
{
}

H323NonStandardAudioCapability::H323NonStandardAudioCapability(
      unsigned max,
      unsigned desired,
      H323EndPoint &,
      H323NonStandardCapabilityInfo::CompareFuncType compareFunc,
      const BYTE * fixedData,
      PINDEX dataSize)
  : H323AudioCapability(max, desired),
    H323NonStandardCapabilityInfo(compareFunc, fixedData, dataSize)
{
}

H323NonStandardAudioCapability::H323NonStandardAudioCapability(unsigned max,
                                                               unsigned desired,
                                                               H323EndPoint &,
                                                               const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323AudioCapability(max, desired),
    H323NonStandardCapabilityInfo(fixedData, dataSize, offset, length)
{
}

H323NonStandardAudioCapability::H323NonStandardAudioCapability(unsigned max,
                                                               unsigned desired,
                                                               const PString & oid,
                                                               const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323AudioCapability(max, desired),
    H323NonStandardCapabilityInfo(oid, fixedData, dataSize, offset, length)
{
}


H323NonStandardAudioCapability::H323NonStandardAudioCapability(unsigned max,
                                                               unsigned desired,
                                                               BYTE country,
                                                               BYTE extension,
                                                               WORD maufacturer,
                                                               const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323AudioCapability(max, desired),
    H323NonStandardCapabilityInfo(country, extension, maufacturer, fixedData, dataSize, offset, length)
{
}


PObject::Comparison H323NonStandardAudioCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323NonStandardAudioCapability))
    return LessThan;

  return CompareInfo((const H323NonStandardAudioCapability &)obj);
}


unsigned H323NonStandardAudioCapability::GetSubType() const
{
  return H245_AudioCapability::e_nonStandard;
}


BOOL H323NonStandardAudioCapability::OnSendingPDU(H245_AudioCapability & pdu,
                                                  unsigned) const
{
  return OnSendingNonStandardPDU(pdu, H245_AudioCapability::e_nonStandard);
}


BOOL H323NonStandardAudioCapability::OnSendingPDU(H245_AudioMode & pdu) const
{
  return OnSendingNonStandardPDU(pdu, H245_AudioMode::e_nonStandard);
}


BOOL H323NonStandardAudioCapability::OnReceivedPDU(const H245_AudioCapability & pdu,
                                                   unsigned &)
{
  return OnReceivedNonStandardPDU(pdu, H245_AudioCapability::e_nonStandard);
}


BOOL H323NonStandardAudioCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) &&
         H323NonStandardCapabilityInfo::IsMatch((const H245_NonStandardParameter &)subTypePDU.GetObject());
}

#endif // NO_H323_AUDIO_CODECS


/////////////////////////////////////////////////////////////////////////////

#ifndef NO_H323_VIDEO
/*
int H323VideoCapability::DSCPvalue = PQoS::controlledLoadDSCP;
H323VideoCapability::H323VideoCapability()
{
#if P_HAS_QOS
// Set to H263CIF Settings
	rtpqos = new RTP_QOS;
	rtpqos->dataQoS.SetWinServiceType(SERVICETYPE_CONTROLLEDLOAD);
	rtpqos->dataQoS.SetAvgBytesPerSec(16000);
	rtpqos->dataQoS.SetMaxFrameBytes(8192);
	rtpqos->dataQoS.SetDSCP(DSCPvalue);

	rtpqos->ctrlQoS.SetWinServiceType(SERVICETYPE_CONTROLLEDLOAD);
	rtpqos->ctrlQoS.SetDSCP(PQoS::controlledLoadDSCP); 
#endif
}
*/
/*
int H323VideoCapability::DSCPvalue = PQoS::guaranteedDSCP;
H323VideoCapability::H323VideoCapability()
{
#if P_HAS_QOS
// Set to G.729 Settings Avg 56kb/s Peek 110 kb/s
	rtpqos = new RTP_QOS;
	rtpqos->dataQoS.SetWinServiceType(SERVICETYPE_GUARANTEED);
	rtpqos->dataQoS.SetAvgBytesPerSec(32000);
	rtpqos->dataQoS.SetMaxFrameBytes(30000);
	rtpqos->dataQoS.SetPeakBytesPerSec(128000);
	rtpqos->dataQoS.SetDSCP(DSCPvalue);

	rtpqos->ctrlQoS.SetWinServiceType(SERVICETYPE_CONTROLLEDLOAD);
	rtpqos->ctrlQoS.SetDSCP(PQoS::controlledLoadDSCP); 
#endif
}
*/

int H323VideoCapability::DSCPvalue = PQoS::bestEffortDSCP;
H323VideoCapability::H323VideoCapability()
{
#if P_HAS_QOS
// Set to G.729 Settings Avg 56kb/s Peek 110 kb/s
	rtpqos = new RTP_QOS;
	rtpqos->dataQoS.SetWinServiceType(SERVICETYPE_BESTEFFORT);
//	rtpqos->dataQoS.SetAvgBytesPerSec(32000);
//	rtpqos->dataQoS.SetMaxFrameBytes(30000);
//	rtpqos->dataQoS.SetPeakBytesPerSec(128000);
	rtpqos->dataQoS.SetDSCP(DSCPvalue);

//	rtpqos->ctrlQoS.SetWinServiceType(SERVICETYPE_PNOTDEFINED);
//	rtpqos->ctrlQoS.SetDSCP(-1); 
#endif
}



H323Capability::MainTypes H323VideoCapability::GetMainType() const
{
  return e_Video;
}


BOOL H323VideoCapability::OnSendingPDU(H245_Capability & cap) const
{
  switch (capabilityDirection) {
    case e_Transmit:
      cap.SetTag(H245_Capability::e_transmitVideoCapability);
      break;
    case e_ReceiveAndTransmit:
      cap.SetTag(H245_Capability::e_receiveAndTransmitVideoCapability);
      break;
    case e_Receive :
    default:
      cap.SetTag(H245_Capability::e_receiveVideoCapability);
  }
  return OnSendingPDU((H245_VideoCapability &)cap, e_TCS);
}


BOOL H323VideoCapability::OnSendingPDU(H245_DataType & dataType) const
{
  dataType.SetTag(H245_DataType::e_videoData);
  return OnSendingPDU((H245_VideoCapability &)dataType, e_OLC);
}


BOOL H323VideoCapability::OnSendingPDU(H245_VideoCapability & pdu) const
{
  return FALSE;
}


BOOL H323VideoCapability::OnSendingPDU(H245_VideoCapability & pdu, CommandType) const
{
  return OnSendingPDU(pdu);
}


BOOL H323VideoCapability::OnSendingPDU(H245_ModeElement & mode) const
{
  mode.m_type.SetTag(H245_ModeElementType::e_videoMode);
  return OnSendingPDU((H245_VideoMode &)mode.m_type);
}


BOOL H323VideoCapability::OnReceivedPDU(const H245_Capability & cap)
{
  H323Capability::OnReceivedPDU(cap);

  if (cap.GetTag() != H245_Capability::e_receiveVideoCapability &&
      cap.GetTag() != H245_Capability::e_receiveAndTransmitVideoCapability)
    return FALSE;

  return OnReceivedPDU((const H245_VideoCapability &)cap, e_TCS);
}


BOOL H323VideoCapability::OnReceivedPDU(const H245_DataType & dataType, BOOL)
{
  if (dataType.GetTag() != H245_DataType::e_videoData)
    return FALSE;

  return OnReceivedPDU((const H245_VideoCapability &)dataType, e_OLC);
}


BOOL H323VideoCapability::OnReceivedPDU(const H245_VideoCapability &)
{
  return FALSE;
}


BOOL H323VideoCapability::OnReceivedPDU(const H245_VideoCapability & pdu, CommandType)
{
  return OnReceivedPDU(pdu);
}


unsigned H323VideoCapability::GetDefaultSessionID() const
{
  return RTP_Session::DefaultVideoSessionID;
}

void H323VideoCapability::SetDSCPvalue(int newValue) 
{ 
	if (newValue < 64)
		DSCPvalue = newValue;
}

int H323VideoCapability::GetDSCPvalue() 
{ 
	return DSCPvalue; 
}

/////////////////////////////////////////////////////////////////////////////

H323NonStandardVideoCapability::H323NonStandardVideoCapability(const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323NonStandardCapabilityInfo(fixedData, dataSize, offset, length)
{
}

H323NonStandardVideoCapability::H323NonStandardVideoCapability(H323EndPoint &,
                                                               const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323NonStandardCapabilityInfo(fixedData, dataSize, offset, length)
{
}

H323NonStandardVideoCapability::H323NonStandardVideoCapability(const PString & oid,
                                                               const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323NonStandardCapabilityInfo(oid, fixedData, dataSize, offset, length)
{
}


H323NonStandardVideoCapability::H323NonStandardVideoCapability(BYTE country,
                                                               BYTE extension,
                                                               WORD maufacturer,
                                                               const BYTE * fixedData,
                                                               PINDEX dataSize,
                                                               PINDEX offset,
                                                               PINDEX length)
  : H323NonStandardCapabilityInfo(country, extension, maufacturer, fixedData, dataSize, offset, length)
{
}


PObject::Comparison H323NonStandardVideoCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323NonStandardVideoCapability))
    return LessThan;

  return CompareInfo((const H323NonStandardVideoCapability &)obj);
}


unsigned H323NonStandardVideoCapability::GetSubType() const
{
  return H245_VideoCapability::e_nonStandard;
}


BOOL H323NonStandardVideoCapability::OnSendingPDU(H245_VideoCapability & pdu) const
{
  return OnSendingNonStandardPDU(pdu, H245_VideoCapability::e_nonStandard);
}


BOOL H323NonStandardVideoCapability::OnSendingPDU(H245_VideoMode & pdu) const
{
  return OnSendingNonStandardPDU(pdu, H245_VideoMode::e_nonStandard);
}


BOOL H323NonStandardVideoCapability::OnReceivedPDU(const H245_VideoCapability & pdu)
{
  return OnReceivedNonStandardPDU(pdu, H245_VideoCapability::e_nonStandard);
}


BOOL H323NonStandardVideoCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) &&
         H323NonStandardCapabilityInfo::IsMatch((const H245_NonStandardParameter &)subTypePDU.GetObject());
}

/////////////////////////////////////////////////////////////////////////////

H323GenericVideoCapability::H323GenericVideoCapability(
      const PString &capabilityId,
      PINDEX maxBitRate)
        : H323VideoCapability(),
          H323GenericCapabilityInfo(capabilityId, maxBitRate)
{
}

PObject::Comparison H323GenericVideoCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323GenericVideoCapability))
    return LessThan;

  return CompareInfo((const H323GenericVideoCapability &)obj);
}


unsigned H323GenericVideoCapability::GetSubType() const
{
  return H245_VideoCapability::e_genericVideoCapability;
}


BOOL H323GenericVideoCapability::OnSendingPDU(H245_VideoCapability & pdu, CommandType type) const
{
    pdu.SetTag(H245_VideoCapability::e_genericVideoCapability);
  return OnSendingGenericPDU(pdu, GetMediaFormat(), type);
}

BOOL H323GenericVideoCapability::OnSendingPDU(H245_VideoCapability & pdu) const
{
    pdu.SetTag(H245_VideoCapability::e_genericVideoCapability);
  return OnSendingGenericPDU(pdu, GetMediaFormat(), e_TCS);
}

BOOL H323GenericVideoCapability::OnSendingPDU(H245_VideoMode & pdu) const
{
  pdu.SetTag(H245_VideoMode::e_genericVideoMode);
  return OnSendingGenericPDU(pdu, GetMediaFormat(), e_ReqMode);
}

BOOL H323GenericVideoCapability::OnReceivedPDU(const H245_VideoCapability & pdu, CommandType type)
{
  if (pdu.GetTag() != H245_VideoCapability::e_genericVideoCapability)
	return FALSE;
  return OnReceivedGenericPDU(GetWritableMediaFormat(), pdu, type);
}

BOOL H323GenericVideoCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) && 
   H323GenericCapabilityInfo::IsMatch((const H245_GenericCapability &)subTypePDU.GetObject());
}

/////////////////////////////////////////////////////////////////////////////

#ifdef H323_H239

H323ExtendedVideoCapability::H323ExtendedVideoCapability(
      const PString &capabilityId )
        : H323Capability(),
          H323GenericCapabilityInfo(capabilityId, 0)
{
	SetCapabilityDirection(H323Capability::e_NoDirection);
}

BOOL H323ExtendedVideoCapability::OnSendingPDU(H245_Capability & cap) const
{
	  cap.SetTag(H245_Capability::e_genericControlCapability);
	  return OnSendingPDU((H245_GenericCapability &)cap, e_TCS);
}

BOOL H323ExtendedVideoCapability::OnReceivedPDU(const H245_Capability & cap)
{
  H323Capability::OnReceivedPDU(cap);

  if( cap.GetTag()!= H245_Capability::e_genericControlCapability)
    return FALSE;

  return OnReceivedPDU((const H245_GenericCapability &)cap, e_TCS);
} 

BOOL H323ExtendedVideoCapability::OnReceivedPDU(const H245_GenericCapability & pdu, CommandType type)
{
  OpalMediaFormat mediaFormat = GetMediaFormat();
  return OnReceivedGenericPDU(mediaFormat, pdu, type);
}

BOOL H323ExtendedVideoCapability::OnSendingPDU(H245_GenericCapability & pdu, CommandType type) const
{
  return OnSendingGenericPDU(pdu, GetMediaFormat(), type);
}

PObject::Comparison H323ExtendedVideoCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323ExtendedVideoCapability))
    return LessThan;

  return CompareInfo((const H323ExtendedVideoCapability &)obj);
}

H323Capability::MainTypes H323ExtendedVideoCapability::GetMainType() const
{
	return H323Capability::e_GenericControl;
}

unsigned H323ExtendedVideoCapability::GetSubType() const
{
	return 0; // Not used
}

unsigned H323ExtendedVideoCapability::GetDefaultSessionID() const
{
	return OpalMediaFormat::DefaultExtVideoSessionID;
}

void H323ExtendedVideoCapability::PrintOn(ostream & strm) const
{
  if (table.GetSize() > 0) {
	  strm << "H239<" << table[0] << ">";
	  return;
  }

  strm << GetFormatName();
  if (assignedCapabilityNumber != 0)
    strm << " <" << assignedCapabilityNumber << '>';

	if (extCapabilities.GetSize() > 0) {
	  int indent = (int)strm.precision() + 2;
      for (PINDEX i=0; i< extCapabilities.GetSize(); i++) {
         strm << '\n' << setw(indent+6) << extCapabilities[i];
	  }
	}
}

H323Channel * H323ExtendedVideoCapability::CreateChannel(H323Connection & connection,   
      H323Channel::Directions dir,unsigned sessionID,const H245_H2250LogicalChannelParameters * param
) const
{
   if (table.GetSize() > 0)
	 return table[0].CreateChannel(connection,dir,sessionID,param);
   else
	 return NULL;
}

BOOL H323ExtendedVideoCapability::OnSendingPDU(H245_DataType & pdu) const
{
   if (table.GetSize() > 0)
	 return table[0].OnSendingPDU(pdu);
   else
	 return FALSE;
}

BOOL H323ExtendedVideoCapability::OnSendingPDU(H245_ModeElement & pdu) const
{
   if (table.GetSize() > 0)
	 return table[0].OnSendingPDU(pdu);
   else
	 return FALSE;
}

BOOL H323ExtendedVideoCapability::OnReceivedPDU(const H245_DataType & pdu, BOOL receiver)
{
   if (table.GetSize() > 0)
	 return table[0].OnReceivedPDU(pdu,receiver);
   else
	 return FALSE;
}

BOOL H323ExtendedVideoCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) &&
         H323GenericCapabilityInfo::IsMatch((const H245_GenericCapability &)subTypePDU.GetObject());
}

void H323ExtendedVideoCapability::AddAllCapabilities(
      H323Capabilities & basecapabilities, PINDEX descriptorNum,PINDEX simultaneous)
{
  H323ExtendedVideoFactory::KeyList_T extCaps = H323ExtendedVideoFactory::GetKeyList();
  if (extCaps.size() > 0) {
    // Add all the extended Video Capabilities to the capability list
    H323ExtendedVideoFactory::KeyList_T::const_iterator r;
    for (r = extCaps.begin(); r != extCaps.end(); ++r) {
       H323CodecExtendedVideoCapability * extCapability = new H323CodecExtendedVideoCapability();
       PString capName(*r);
       H323Capability * capability = H323ExtendedVideoFactory::CreateInstance(capName);
	   extCapability->AddCapability(capability);
	   H323ExtendedVideoFactory::Unregister(*r);  // To avoid nasty segfault on shutdown.
      basecapabilities.SetCapability(descriptorNum, simultaneous,extCapability);
	}
 // control not yet available
 //     basecapabilities.SetCapability(descriptorNum, simultaneous, new H323ControlExtendedVideoCapability()); 
  } else {
	  PTRACE(4,"EXT\tNo Extended Capabilities found to load");
  }
}
/*
void H323ExtendedVideoCapability::AddAllCapabilities(
      H323Capabilities & basecapabilities, PINDEX descriptorNum,PINDEX simultaneous)
{
  H323ExtendedVideoFactory::KeyList_T extCaps = H323ExtendedVideoFactory::GetKeyList();
  if (extCaps.size() > 0) {
	  H323CodecExtendedVideoCapability * extCapability = new H323CodecExtendedVideoCapability();
    // Add all the extended Video Capabilities to the capability list
    H323ExtendedVideoFactory::KeyList_T::const_iterator r;
    for (r = extCaps.begin(); r != extCaps.end(); ++r) {
       PString capName(*r);
       H323Capability * capability = H323ExtendedVideoFactory::CreateInstance(capName);
	   extCapability->AddCapability(capability);
	   H323ExtendedVideoFactory::Unregister(*r);  // To avoid nasty segfault on shutdown.
	}
      basecapabilities.SetCapability(descriptorNum, simultaneous,extCapability);
 // control not yet available
 //     basecapabilities.SetCapability(descriptorNum, simultaneous, new H323ControlExtendedVideoCapability()); 
  } else {
	  PTRACE(4,"EXT\tNo Extended Capabilities found to load");
  }
}
*/
/*
void H323ExtendedVideoCapability::AddAllCapabilities(
      H323Capabilities & basecapabilities, PINDEX descriptorNum,PINDEX simultaneous)
{
  H323ExtendedVideoFactory::KeyList_T extCaps = H323ExtendedVideoFactory::GetKeyList();
  if (extCaps.size() > 0) {
	  H323CodecExtendedVideoCapability * extCapability = 
	  (H323CodecExtendedVideoCapability *)basecapabilities.FindCapability(H323Capability::e_Video,H245_VideoCapability::e_extendedVideoCapability);
   if(extCapability != NULL) {
    // Add all the extended Video Capabilities to the capability list
    H323ExtendedVideoFactory::KeyList_T::const_iterator r;
    for (r = extCaps.begin(); r != extCaps.end(); ++r) {
	   extCapability->AddCapability(*r);
	}
     }
 // control not yet available
 //     basecapabilities.SetCapability(descriptorNum, simultaneous, new H323ControlExtendedVideoCapability()); 
  } else {
	  PTRACE(4,"EXT\tNo Extended Capabilities found to load");
  }
}
*/
H323Capability & H323ExtendedVideoCapability::operator[](PINDEX i) 
{
  return table[i];
}

///////////////////////////////////////////////////////////////////////////////////////////////////


H323ControlExtendedVideoCapability::H323ControlExtendedVideoCapability()
  : H323ExtendedVideoCapability(OpalPluginCodec_Identifer_H239)
{ 
}

///////////////////////////////////////////////////////////////////////////////////////////////////

H323CodecExtendedVideoCapability::H323CodecExtendedVideoCapability()
   : H323ExtendedVideoCapability(OpalPluginCodec_Identifer_H239_Video)
{
	
   SetCapabilityDirection(H323Capability::e_Receive);
}

H323CodecExtendedVideoCapability::~H323CodecExtendedVideoCapability()
{
}

void H323CodecExtendedVideoCapability::AddCapability(H323Capability * capability)
{
	extCapabilities.Add(capability);
}
/*
void H323CodecExtendedVideoCapability::AddCapability(const PString & cap)
{
	extCapabilities.Add(H323ExtendedVideoFactory::CreateInstance(cap));
}
*/

H323Capability::MainTypes H323CodecExtendedVideoCapability::GetMainType() const
{  
	return H323Capability::e_Video; 
}

unsigned H323CodecExtendedVideoCapability::GetSubType() const
{  
	return H245_VideoCapability::e_extendedVideoCapability;  
}

BOOL H323CodecExtendedVideoCapability::OnSendingPDU(H245_Capability & cap) const
{
   cap.SetTag(H245_Capability::e_receiveVideoCapability);
   return OnSendingPDU((H245_VideoCapability &)cap);
}

BOOL H323CodecExtendedVideoCapability::OnReceivedPDU(const H245_Capability & cap)
{
  H323Capability::OnReceivedPDU(cap);

  if (cap.GetTag()!= H245_Capability::e_receiveVideoCapability)
    return FALSE;

  return OnReceivedPDU((const H245_VideoCapability &)cap);
} 

BOOL H323CodecExtendedVideoCapability::OnSendingPDU(H245_VideoCapability & pdu) const 
{ 
	if (extCapabilities.GetSize() == 0)
		return FALSE;

	pdu.SetTag(H245_VideoCapability::e_extendedVideoCapability);
    H245_ExtendedVideoCapability & extend = (H245_ExtendedVideoCapability &)pdu;

	extend.IncludeOptionalField(H245_ExtendedVideoCapability::e_videoCapabilityExtension);
    H245_ArrayOf_GenericCapability & cape = extend.m_videoCapabilityExtension;

	H245_GenericCapability gcap;
	 gcap.m_capabilityIdentifier = *(new H245_CapabilityIdentifier(H245_CapabilityIdentifier::e_standard));
	 PASN_ObjectId &object_id = gcap.m_capabilityIdentifier;
     object_id = OpalPluginCodec_Identifer_H239_Video;

	 // Add role
      H245_GenericParameter * param = new H245_GenericParameter;
      param->m_parameterIdentifier.SetTag(H245_ParameterIdentifier::e_standard);
      (PASN_Integer &)param->m_parameterIdentifier = 1;
	  param->m_parameterValue.SetTag(H245_ParameterValue::e_booleanArray);
      (PASN_Integer &)param->m_parameterValue = 3;  // Live video & presentation

      gcap.IncludeOptionalField(H245_GenericCapability::e_nonCollapsing);
      gcap.m_nonCollapsing.Append(param);
      cape.SetSize(1);
      cape[0] = gcap;


    H245_ArrayOf_VideoCapability & caps = extend.m_videoCapability;
    
	if (table.GetSize() > 0) {
	  caps.SetSize(table.GetSize());
      for (PINDEX i=0; i< table.GetSize(); i++) {
	     H245_VideoCapability vidcap;
        ((H323VideoCapability &)table[i]).OnSendingPDU(vidcap);
        caps[i] = vidcap;
	  }
	} else {
	  caps.SetSize(extCapabilities.GetSize());
      for (PINDEX i=0; i< extCapabilities.GetSize(); i++) {
	     H245_VideoCapability vidcap;
         ((H323VideoCapability &)extCapabilities[i]).OnSendingPDU(vidcap);
         caps[i] = vidcap;
      }
	}

  return TRUE;
}

BOOL H323CodecExtendedVideoCapability::OnReceivedPDU(const H245_VideoCapability & pdu )
{ 
   if (pdu.GetTag() != H245_VideoCapability::e_extendedVideoCapability)
		return FALSE;

   const H245_ExtendedVideoCapability & extend = (const H245_ExtendedVideoCapability &)pdu;

   if (!extend.HasOptionalField(H245_ExtendedVideoCapability::e_videoCapabilityExtension))
		return FALSE;

   // Role Information
   const H245_ArrayOf_GenericCapability & cape = extend.m_videoCapabilityExtension;
   if (cape.GetSize() == 0) {
	   PTRACE(2,"H239\tERROR: Missing Capability Extension!");
	   return FALSE;
   }

   for (PINDEX b =0; b < cape.GetSize(); b++) {
    const H245_GenericCapability & cap = cape[b];
	if (cap.m_capabilityIdentifier.GetTag() != H245_CapabilityIdentifier::e_standard) {
		PTRACE(4,"H239\tERROR: Wrong Capability type!");
		return FALSE;
	}
	
	const PASN_ObjectId & id = cap.m_capabilityIdentifier;
	if (id != OpalPluginCodec_Identifer_H239_Video) {
		PTRACE(4,"H239\tERROR: Wrong Capability Identifer " << id);
		return FALSE;
	}

	if (!cap.HasOptionalField(H245_GenericCapability::e_nonCollapsing)) {
		PTRACE(4,"H239\tERROR: No nonCollapsing field");
		return FALSE;
	}

	for (PINDEX c =0; c < cap.m_nonCollapsing.GetSize(); c++) {
		const H245_GenericParameter & param = cap.m_nonCollapsing[c];
		const PASN_Integer & id = param.m_parameterIdentifier;
		if (id.GetValue() != 1) {
	        PTRACE(4,"H239\tERROR: Unknown Role Identifer");
			return FALSE;
		}
		const PASN_Integer & role = param.m_parameterValue;
		if (role.GetValue() != 2) {
	        PTRACE(4,"H239\tERROR: Unsupported Role mode " << param.m_parameterValue );
			return FALSE;
		}
	}
  }

  // Get a Common Video Capability list
  const H245_ArrayOf_VideoCapability & caps = extend.m_videoCapability;
  H323Capabilities allCapabilities;
  for (PINDEX c = 0; c < extCapabilities.GetSize(); c++)
    allCapabilities.Add(allCapabilities.Copy(extCapabilities[c]));

  // Decode out of the PDU, the list of known codecs.
    for (PINDEX i = 0; i < caps.GetSize(); i++) {
        H323Capability * capability = allCapabilities.FindCapability(H323Capability::e_Video, caps[i], NULL);
        if (capability != NULL) {
          H323VideoCapability * copy = (H323VideoCapability *)capability->Clone();
          if (copy->OnReceivedPDU(caps[i]))
            table.Append(copy);
          else
            delete copy;
        }
    }
	return TRUE; 
}

BOOL H323CodecExtendedVideoCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
   if (table.GetSize() > 0)
	 return table[0].IsMatch(subTypePDU);
   else
	 return FALSE;
}

BOOL H323CodecExtendedVideoCapability::OnReceivedGenericPDU(const H245_GenericCapability &pdu)
{
	return TRUE;
}

#endif // H323_H239
#endif // NO_H323_VIDEO

/////////////////////////////////////////////////////////////////////////////

#ifdef H323_H230
H323_ConferenceControlCapability::H323_ConferenceControlCapability()
{
   chairControlCapability = FALSE;
   nonStandardExtension = FALSE;
}

H323_ConferenceControlCapability::H323_ConferenceControlCapability(BOOL chairControls, BOOL T124Extension)
{
   chairControlCapability = chairControls;
   nonStandardExtension = T124Extension;
}

PObject * H323_ConferenceControlCapability::Clone() const
{
  return new H323_ConferenceControlCapability(*this);
}

H323Capability::MainTypes H323_ConferenceControlCapability::GetMainType() const
{
  return e_ConferenceControl;
}

unsigned H323_ConferenceControlCapability::GetSubType()  const
{
    return 0;
}

PString H323_ConferenceControlCapability::GetFormatName() const
{
  return "H.230 Conference Controls";
}

H323Channel * H323_ConferenceControlCapability::CreateChannel(H323Connection &,
                                                      H323Channel::Directions,
                                                      unsigned,
                                                      const H245_H2250LogicalChannelParameters *) const
{
  PTRACE(1, "Codec\tCannot create ConferenceControlCapability channel");
  return NULL;
}

H323Codec * H323_ConferenceControlCapability::CreateCodec(H323Codec::Direction) const
{
  PTRACE(1, "Codec\tCannot create ConferenceControlCapability codec");
  return NULL;
}

static const char * ExtConferenceControlOID = "0.0.20.124.2";  // Tunnel T.124
BOOL H323_ConferenceControlCapability::OnSendingPDU(H245_Capability & pdu) const
{
   
  pdu.SetTag(H245_Capability::e_conferenceCapability);
  H245_ConferenceCapability & conf = pdu;
  // Supports Chair control
  conf.m_chairControlCapability = chairControlCapability;

  // Include Extended Custom Controls such as INVITE/EJECT etc.
  if (nonStandardExtension) {
	conf.IncludeOptionalField(H245_ConferenceCapability::e_nonStandardData);
	H245_ArrayOf_NonStandardParameter & nsParam = conf.m_nonStandardData;

	H245_NonStandardParameter param;
	H245_NonStandardIdentifier & id = param.m_nonStandardIdentifier;
	id.SetTag(H245_NonStandardIdentifier::e_object);
	PASN_ObjectId & oid = id;
	oid.SetValue(ExtConferenceControlOID);
	PASN_OctetString & data = param.m_data;
	data.SetValue("");

	nsParam.SetSize(1);
	nsParam[0] = param;
  }

  return TRUE;
}

BOOL H323_ConferenceControlCapability::OnSendingPDU(H245_DataType &) const
{
  PTRACE(1, "Codec\tCannot have ConferenceControlCapability in DataType");
  return FALSE;
}

BOOL H323_ConferenceControlCapability::OnSendingPDU(H245_ModeElement &) const
{
  PTRACE(1, "Codec\tCannot have ConferenceControlCapability in ModeElement");
  return FALSE;
}

BOOL H323_ConferenceControlCapability::OnReceivedPDU(const H245_Capability & pdu)
{

  H323Capability::OnReceivedPDU(pdu);

  if (pdu.GetTag() != H245_Capability::e_conferenceCapability)
    return FALSE;

  const H245_ConferenceCapability & conf = pdu;
  // Supports Chair control
  chairControlCapability = conf.m_chairControlCapability;

  // Include Extended Custom Control support.
  if (conf.HasOptionalField(H245_ConferenceCapability::e_nonStandardData)) {
	const H245_ArrayOf_NonStandardParameter & nsParam = conf.m_nonStandardData;

	for (PINDEX i=0; i < nsParam.GetSize(); i++) {
		const H245_NonStandardParameter & param = nsParam[i];
	    const H245_NonStandardIdentifier & id = param.m_nonStandardIdentifier;
		if (id.GetTag() == H245_NonStandardIdentifier::e_object) {
             const PASN_ObjectId & oid = id;
			 if (oid.AsString() == ExtConferenceControlOID)
				    nonStandardExtension = TRUE;
		}
	}
  }
  return TRUE;
}

BOOL H323_ConferenceControlCapability::OnReceivedPDU(const H245_DataType &, BOOL)
{
  PTRACE(1, "Codec\tCannot have ConferenceControlCapability in DataType");
  return FALSE;
}

#endif  // H323_H230

/////////////////////////////////////////////////////////////////////////////

H323DataCapability::H323DataCapability(unsigned rate)
  : maxBitRate(rate)
{
}


H323Capability::MainTypes H323DataCapability::GetMainType() const
{
  return e_Data;
}


unsigned H323DataCapability::GetDefaultSessionID() const
{
  return 3;
}


H323Codec * H323DataCapability::CreateCodec(H323Codec::Direction) const
{
  return NULL;
}


BOOL H323DataCapability::OnSendingPDU(H245_Capability & cap) const
{
  switch (capabilityDirection) {
    case e_Transmit:
      cap.SetTag(H245_Capability::e_transmitDataApplicationCapability);
      break;
    case e_Receive :
      cap.SetTag(H245_Capability::e_receiveDataApplicationCapability);
      break;
    case e_ReceiveAndTransmit:
    default:
      cap.SetTag(H245_Capability::e_receiveAndTransmitDataApplicationCapability);
  }
  H245_DataApplicationCapability & app = cap;
  app.m_maxBitRate = maxBitRate;
  return OnSendingPDU(app, e_TCS);
}


BOOL H323DataCapability::OnSendingPDU(H245_DataType & dataType) const
{
  dataType.SetTag(H245_DataType::e_data);
  H245_DataApplicationCapability & app = dataType;
  app.m_maxBitRate = maxBitRate;
  return OnSendingPDU(app, e_OLC);
}


BOOL H323DataCapability::OnSendingPDU(H245_ModeElement & mode) const
{
  mode.m_type.SetTag(H245_ModeElementType::e_dataMode);
  H245_DataMode & type = mode.m_type;
  type.m_bitRate = maxBitRate;
  return OnSendingPDU(type);
}


BOOL H323DataCapability::OnSendingPDU(H245_DataApplicationCapability &) const
{
  return FALSE;
}


BOOL H323DataCapability::OnSendingPDU(H245_DataApplicationCapability & pdu, CommandType) const
{
  return OnSendingPDU(pdu);
}


BOOL H323DataCapability::OnReceivedPDU(const H245_Capability & cap)
{
  H323Capability::OnReceivedPDU(cap);

  if (cap.GetTag() != H245_Capability::e_receiveDataApplicationCapability &&
      cap.GetTag() != H245_Capability::e_receiveAndTransmitDataApplicationCapability)
    return FALSE;

  const H245_DataApplicationCapability & app = cap;
  maxBitRate = app.m_maxBitRate;
  return OnReceivedPDU(app, e_TCS);
}


BOOL H323DataCapability::OnReceivedPDU(const H245_DataType & dataType, BOOL)
{
  if (dataType.GetTag() != H245_DataType::e_data)
    return FALSE;

  const H245_DataApplicationCapability & app = dataType;
  maxBitRate = app.m_maxBitRate;
  return OnReceivedPDU(app, e_OLC);
}


BOOL H323DataCapability::OnReceivedPDU(const H245_DataApplicationCapability &)
{
  return FALSE;
}


BOOL H323DataCapability::OnReceivedPDU(const H245_DataApplicationCapability & pdu, CommandType)
{
  return OnReceivedPDU(pdu);
}


/////////////////////////////////////////////////////////////////////////////

H323NonStandardDataCapability::H323NonStandardDataCapability(unsigned maxBitRate,
                                                             const BYTE * fixedData,
                                                             PINDEX dataSize,
                                                             PINDEX offset,
                                                             PINDEX length)
  : H323DataCapability(maxBitRate),
    H323NonStandardCapabilityInfo(fixedData, dataSize, offset, length)
{
}


H323NonStandardDataCapability::H323NonStandardDataCapability(unsigned maxBitRate,
                                                             const PString & oid,
                                                             const BYTE * fixedData,
                                                             PINDEX dataSize,
                                                             PINDEX offset,
                                                             PINDEX length)
  : H323DataCapability(maxBitRate),
    H323NonStandardCapabilityInfo(oid, fixedData, dataSize, offset, length)
{
}


H323NonStandardDataCapability::H323NonStandardDataCapability(unsigned maxBitRate,
                                                             BYTE country,
                                                             BYTE extension,
                                                             WORD maufacturer,
                                                             const BYTE * fixedData,
                                                             PINDEX dataSize,
                                                             PINDEX offset,
                                                             PINDEX length)
  : H323DataCapability(maxBitRate),
    H323NonStandardCapabilityInfo(country, extension, maufacturer, fixedData, dataSize, offset, length)
{
}


PObject::Comparison H323NonStandardDataCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323NonStandardDataCapability))
    return LessThan;

  return CompareInfo((const H323NonStandardDataCapability &)obj);
}


unsigned H323NonStandardDataCapability::GetSubType() const
{
  return H245_DataApplicationCapability_application::e_nonStandard;
}


BOOL H323NonStandardDataCapability::OnSendingPDU(H245_DataApplicationCapability & pdu) const
{
  return OnSendingNonStandardPDU(pdu.m_application, H245_DataApplicationCapability_application::e_nonStandard);
}


BOOL H323NonStandardDataCapability::OnSendingPDU(H245_DataMode & pdu) const
{
  return OnSendingNonStandardPDU(pdu.m_application, H245_DataMode_application::e_nonStandard);
}


BOOL H323NonStandardDataCapability::OnReceivedPDU(const H245_DataApplicationCapability & pdu)
{
  return OnReceivedNonStandardPDU(pdu.m_application, H245_DataApplicationCapability_application::e_nonStandard);
}


BOOL H323NonStandardDataCapability::IsMatch(const PASN_Choice & subTypePDU) const
{
  return H323Capability::IsMatch(subTypePDU) &&
         H323NonStandardCapabilityInfo::IsMatch((const H245_NonStandardParameter &)subTypePDU.GetObject());
}


/////////////////////////////////////////////////////////////////////////////

#ifndef NO_H323_AUDIO_CODECS

H323_G711Capability::H323_G711Capability(Mode m, Speed s)
//  : H323AudioCapability(240, 30) // 240ms max, 30ms desired
  : H323AudioCapability(120, 30) // 240ms max, 30ms desired
{
  mode = m;
  speed = s;
}


PObject * H323_G711Capability::Clone() const
{
  return new H323_G711Capability(*this);
}


unsigned H323_G711Capability::GetSubType() const
{
  static const unsigned G711SubType[2][2] = {
    { H245_AudioCapability::e_g711Alaw64k, H245_AudioCapability::e_g711Alaw56k },
    { H245_AudioCapability::e_g711Ulaw64k, H245_AudioCapability::e_g711Ulaw56k }
  };
  return G711SubType[mode][speed];
}


PString H323_G711Capability::GetFormatName() const
{
  static const char * const G711Name[2][2] = {
    { OPAL_G711_ALAW_64K, OPAL_G711_ALAW_56K },
    { OPAL_G711_ULAW_64K, OPAL_G711_ULAW_56K },
  };
  return G711Name[mode][speed];
}


H323Codec * H323_G711Capability::CreateCodec(H323Codec::Direction direction) const
{
  unsigned packetSize = 8*(direction == H323Codec::Encoder ? txFramesInPacket : rxFramesInPacket);

  if (mode == muLaw)
    return new H323_muLawCodec(direction, speed, packetSize);
  else
    return new H323_ALawCodec(direction, speed, packetSize);
}

#endif // H323AudioCodec


/////////////////////////////////////////////////////////////////////////////

char OpalUserInputRFC2833[] = "UserInput/RFC2833";

#ifdef H323_H249
// H.249 Identifiers
const char * const H323_UserInputCapability::SubTypeOID[4] = {
    "0.0.8.1",  // H.249 Annex A
    "0.0.8.2",  // H.249 Annex B
    "0.0.8.3",  // H.249 Annex C
    "0.0.8.4"   // H.249 Annex D
};
#endif

const char * const H323_UserInputCapability::SubTypeNames[NumSubTypes] = {
  "UserInput/basicString",
  "UserInput/iA5String",
  "UserInput/generalString",
  "UserInput/dtmf",
  "UserInput/hookflash",
  OpalUserInputRFC2833      // "UserInput/RFC2833"
#ifdef H323_H249
  ,"UserInput/Navigation",
  "UserInput/Softkey",
  "UserInput/PointDevice",
  "UserInput/Modal"
#endif
};

OPAL_MEDIA_FORMAT_DECLARE(OpalUserInputRFC2833Format,
        OpalUserInputRFC2833,
        OpalMediaFormat::DefaultAudioSessionID,
        (RTP_DataFrame::PayloadTypes)101, // Choose this for Cisco IOS compatibility
        TRUE,   // Needs jitter
        100,    // bits/sec
        4,      // bytes/frame
        8*150,  // 150 millisecond
        OpalMediaFormat::AudioTimeUnits,
        0)

H323_UserInputCapability::H323_UserInputCapability(SubTypes _subType)
{
  subType = _subType;
  
#ifdef H323_H249
  if (subType > 5)
       subTypeOID = SubTypeOID[subType-6];
  else {
#endif
  
    OpalMediaFormat * fmt = OpalMediaFormatFactory::CreateInstance(OpalUserInputRFC2833);
    if (fmt != NULL)
      rtpPayloadType = fmt->GetPayloadType();
      
#ifdef H323_H249
	subTypeOID = PString();
  }
#endif
}


PObject * H323_UserInputCapability::Clone() const
{
  return new H323_UserInputCapability(*this);
}


H323Capability::MainTypes H323_UserInputCapability::GetMainType() const
{
  return e_UserInput;
}


#define SignalToneRFC2833_SubType 10000

static unsigned UserInputCapabilitySubTypeCodes[] = {
  H245_UserInputCapability::e_basicString,
  H245_UserInputCapability::e_iA5String,
  H245_UserInputCapability::e_generalString,
  H245_UserInputCapability::e_dtmf,
  H245_UserInputCapability::e_hookflash,
  SignalToneRFC2833_SubType
#ifdef H323_H249
  ,H245_UserInputCapability::e_genericUserInputCapability,  // H.249 Annex A
  H245_UserInputCapability::e_genericUserInputCapability,  // H.249 Annex B
  H245_UserInputCapability::e_genericUserInputCapability,  // H.249 Annex C
  H245_UserInputCapability::e_genericUserInputCapability   // H.249 Annex D
#endif
};

unsigned  H323_UserInputCapability::GetSubType()  const
{
  return UserInputCapabilitySubTypeCodes[subType];
}


PString H323_UserInputCapability::GetFormatName() const
{
  return SubTypeNames[subType];
}


H323Channel * H323_UserInputCapability::CreateChannel(H323Connection &,
                                                      H323Channel::Directions,
                                                      unsigned,
                                                      const H245_H2250LogicalChannelParameters *) const
{
  PTRACE(1, "Codec\tCannot create UserInputCapability channel");
  return NULL;
}


H323Codec * H323_UserInputCapability::CreateCodec(H323Codec::Direction) const
{
  PTRACE(1, "Codec\tCannot create UserInputCapability codec");
  return NULL;
}

#ifdef H323_H249
H245_GenericInformation * H323_UserInputCapability::BuildGenericIndication(const char * oid)
{
  H245_GenericInformation * cap = new H245_GenericInformation();
  cap->IncludeOptionalField(H245_GenericMessage::e_subMessageIdentifier);

  H245_CapabilityIdentifier & id = cap->m_messageIdentifier;
  id.SetTag(H245_CapabilityIdentifier::e_standard);
  PASN_ObjectId & gid = id;
  gid.SetValue(oid);
  return cap;
}

H245_GenericParameter * H323_UserInputCapability::BuildGenericParameter(unsigned id,unsigned type, const PString & value)
{  

 H245_GenericParameter * content = new H245_GenericParameter();
      H245_ParameterIdentifier & paramid = content->m_parameterIdentifier;
        paramid.SetTag(H245_ParameterIdentifier::e_standard);
        PASN_Integer & pid = paramid;
        pid.SetValue(id);
	  H245_ParameterValue & paramval = content->m_parameterValue;
	    paramval.SetTag(type);
		 if ((type == H245_ParameterValue::e_unsignedMin) ||
			(type == H245_ParameterValue::e_unsignedMax) ||
			(type == H245_ParameterValue::e_unsigned32Min) ||
			(type == H245_ParameterValue::e_unsigned32Max)) {
				PASN_Integer & val = paramval;
				val.SetValue(value.AsUnsigned());
		 } else if (type == H245_ParameterValue::e_octetString) {
				PASN_OctetString & val = paramval;
				val.SetValue(value);
		 }		
//			H245_ParameterValue::e_logical,
//			H245_ParameterValue::e_booleanArray,
//			H245_ParameterValue::e_genericParameter

     return content;
}
#endif

BOOL H323_UserInputCapability::OnSendingPDU(H245_Capability & pdu) const
{
  if (subType == SignalToneRFC2833) {
    pdu.SetTag(H245_Capability::e_receiveRTPAudioTelephonyEventCapability);
    H245_AudioTelephonyEventCapability & atec = pdu;
    atec.m_dynamicRTPPayloadType = rtpPayloadType;
    atec.m_audioTelephoneEvent = "0-16"; // Support DTMF 0-9,*,#,A-D & hookflash
  }
  else {
    pdu.SetTag(H245_Capability::e_receiveUserInputCapability);
    H245_UserInputCapability & ui = pdu;
    ui.SetTag(UserInputCapabilitySubTypeCodes[subType]);

#ifdef H323_H249
	// H.249 Capabilities
	if (subType > 5) {
		H245_GenericCapability & generic = ui;
		H245_CapabilityIdentifier & id = generic.m_capabilityIdentifier;
        id.SetTag(H245_CapabilityIdentifier::e_standard);
		PASN_ObjectId & oid = id;
		oid.SetValue(subTypeOID);

		if (subType == H249B_Softkey) {
			H245_ArrayOf_GenericParameter & col = generic.m_collapsing;
			// Set this to 10 so to support either 2 or 5
		    H245_GenericParameter * param = 
			       BuildGenericParameter(1,H245_ParameterValue::e_unsignedMin,10); 
		  col.Append(param);  
		  col.SetSize(col.GetSize()+1);
	    }
	}
#endif
  }
  return TRUE;
}


BOOL H323_UserInputCapability::OnSendingPDU(H245_DataType &) const
{
  PTRACE(1, "Codec\tCannot have UserInputCapability in DataType");
  return FALSE;
}


BOOL H323_UserInputCapability::OnSendingPDU(H245_ModeElement &) const
{
  PTRACE(1, "Codec\tCannot have UserInputCapability in ModeElement");
  return FALSE;
}


BOOL H323_UserInputCapability::OnReceivedPDU(const H245_Capability & pdu)
{
  H323Capability::OnReceivedPDU(pdu);

  if (pdu.GetTag() == H245_Capability::e_receiveRTPAudioTelephonyEventCapability) {
    subType = SignalToneRFC2833;
    const H245_AudioTelephonyEventCapability & atec = pdu;
    rtpPayloadType = (RTP_DataFrame::PayloadTypes)(int)atec.m_dynamicRTPPayloadType;
    // Really should verify atec.m_audioTelephoneEvent here
    return TRUE;
  }

  const H245_UserInputCapability & ui = pdu;
  if (ui.GetTag() != UserInputCapabilitySubTypeCodes[subType])
	  return FALSE;

#ifdef H323_H249
  if (ui.GetTag() == H245_UserInputCapability::e_genericUserInputCapability) {
	  const H245_GenericCapability & generic = ui;
	  const H245_CapabilityIdentifier & id = generic.m_capabilityIdentifier;
      if (!id.GetTag() != H245_CapabilityIdentifier::e_standard)
		  return FALSE;
	  
	  const PASN_ObjectId & oid = id;
	  return (subTypeOID == oid.AsString());
  }
#endif

  return TRUE;

}


BOOL H323_UserInputCapability::OnReceivedPDU(const H245_DataType &, BOOL)
{
  PTRACE(1, "Codec\tCannot have UserInputCapability in DataType");
  return FALSE;
}


BOOL H323_UserInputCapability::IsUsable(const H323Connection & connection) const
{
  if (connection.GetControlVersion() >= 7)
    return TRUE;

  if (connection.GetRemoteApplication().Find("AltiServ-ITG") != P_MAX_INDEX)
    return FALSE;

  return subType != SignalToneRFC2833;
}


void H323_UserInputCapability::AddAllCapabilities(H323Capabilities & capabilities,
                                                  PINDEX descriptorNum,
                                                  PINDEX simultaneous)
{
  PINDEX num = capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(HookFlashH245));
  if (descriptorNum == P_MAX_INDEX) {
    descriptorNum = num;
    simultaneous = P_MAX_INDEX;
  }
  else if (simultaneous == P_MAX_INDEX)
    simultaneous = num+1;

  num = capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(BasicString));
  if (simultaneous == P_MAX_INDEX)
    simultaneous = num;

  capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(SignalToneH245));
  capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(SignalToneRFC2833));

#ifdef H323_H249
//// H.249 Capabilities
  capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(H249A_Navigation));
  capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(H249B_Softkey));
  capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(H249C_PointDevice));
  capabilities.SetCapability(descriptorNum, simultaneous, new H323_UserInputCapability(H249D_Modal));
#endif
}


/////////////////////////////////////////////////////////////////////////////

BOOL H323SimultaneousCapabilities::SetSize(PINDEX newSize)
{
  PINDEX oldSize = GetSize();

  if (!H323CapabilitiesListArray::SetSize(newSize))
    return FALSE;

  while (oldSize < newSize) {
    H323CapabilitiesList * list = new H323CapabilitiesList;
    // The lowest level list should not delete codecs on destruction
    list->DisallowDeleteObjects();
    SetAt(oldSize++, list);
  }

  return TRUE;
}


BOOL H323CapabilitiesSet::SetSize(PINDEX newSize)
{
  PINDEX oldSize = GetSize();

  if (!H323CapabilitiesSetArray::SetSize(newSize))
    return FALSE;

  while (oldSize < newSize)
    SetAt(oldSize++, new H323SimultaneousCapabilities);

  return TRUE;
}


H323Capabilities::H323Capabilities()
{
}


H323Capabilities::H323Capabilities(const H323Connection & connection,
                                   const H245_TerminalCapabilitySet & pdu_in)
{
  H323Capabilities allCapabilities;
  const H323Capabilities & localCapabilities = connection.GetLocalCapabilities();
  for (PINDEX c = 0; c < localCapabilities.GetSize(); c++)
    allCapabilities.Add(allCapabilities.Copy(localCapabilities[c]));
  allCapabilities.AddAllCapabilities(0, 0, "*");
  H323_UserInputCapability::AddAllCapabilities(allCapabilities, P_MAX_INDEX, P_MAX_INDEX);
  
  
  
  H245_TerminalCapabilitySet * pdu = (H245_TerminalCapabilitySet *)pdu_in.Clone();
  
  if(connection.GetRemoteApplication().Find("PCS-G") != P_MAX_INDEX)
  {
   if (pdu->HasOptionalField(H245_TerminalCapabilitySet::e_capabilityTable)) 
   {
    for (PINDEX i = 0; i < pdu->m_capabilityTable.GetSize() ; i++) 
    {
     if (pdu->m_capabilityTable[i].HasOptionalField(H245_CapabilityTableEntry::e_capability))
     {
      if(pdu->m_capabilityTable[i].m_capability.GetTag() == H245_Capability::e_receiveVideoCapability)
      {
       H245_VideoCapability & video = pdu->m_capabilityTable[i].m_capability;
       if(video.GetTag() == 3) //h263
       {
        H245_H263VideoCapability & h263 = video;
        if(!h263.HasOptionalField(H245_H263VideoCapability::e_h263Options))
          h263.IncludeOptionalField(H245_H263VideoCapability::e_h263Options);
       }
      }
     }
    }
   }
  }

  // Decode out of the PDU, the list of known codecs.
  if (pdu->HasOptionalField(H245_TerminalCapabilitySet::e_capabilityTable)) {
    for (PINDEX i = 0; i < pdu->m_capabilityTable.GetSize() ; i++) {
//      PTRACE(6,"PDU\tcap " << i << "/" << pdu->m_capabilityTable.GetSize() << ": " << pdu->m_capabilityTable[i]);
      if (pdu->m_capabilityTable[i].HasOptionalField(H245_CapabilityTableEntry::e_capability)) {
//        PTRACE(6,"PDU\tcap " << i << " has opt e_capability");
        H323Capability * capability = allCapabilities.FindCapability(pdu->m_capabilityTable[i].m_capability);
        if (capability != NULL) {
/*
          PTRACE(6,"PDU\tcap " << i << " has been found in local capability table: " << capability
            << ", GetMainType(): "            << capability->GetMainType()
            << ", GetSubType(): "             << capability->GetSubType()
            << ", GetFormatName(): "          << capability->GetFormatName()
            << ", GetDefaultSessionID(): "    << capability->GetDefaultSessionID()
            << ", GetCapabilityDirection(): " << capability->GetCapabilityDirection()
            << ", GetCapabilityNumber(): "    << capability->GetCapabilityNumber()
            << ", GetMediaFormat(): "         << capability->GetMediaFormat()
            << ", GetPayloadType(): "         << capability->GetPayloadType()
          );
*/
          H323Capability * copy = (H323Capability *)capability->Clone();
          copy->SetCapabilityNumber(pdu->m_capabilityTable[i].m_capabilityTableEntryNumber);
          if (copy->OnReceivedPDU(pdu->m_capabilityTable[i].m_capability))
           {
            table.Append(copy);
           }    
          else
            delete copy;
        }
      }
    }
  }

  PINDEX outerSize = pdu->m_capabilityDescriptors.GetSize();
  set.SetSize(outerSize);
  for (PINDEX outer = 0; outer < outerSize; outer++) {
    H245_CapabilityDescriptor & desc = pdu->m_capabilityDescriptors[outer];
    if (desc.HasOptionalField(H245_CapabilityDescriptor::e_simultaneousCapabilities)) {
      PINDEX middleSize = desc.m_simultaneousCapabilities.GetSize();
      set[outer].SetSize(middleSize);
      for (PINDEX middle = 0; middle < middleSize; middle++) {
        H245_AlternativeCapabilitySet & alt = desc.m_simultaneousCapabilities[middle];
        for (PINDEX inner = 0; inner < alt.GetSize(); inner++) {
          for (PINDEX cap = 0; cap < table.GetSize(); cap++) {
            if (table[cap].GetCapabilityNumber() == alt[inner]) {
              set[outer][middle].Append(&table[cap]);
              break;
            }
          }
        }
      }
    }
  }
 delete pdu; 
  
}


H323Capabilities::H323Capabilities(const H323Capabilities & original)
{
  operator=(original);
}


H323Capabilities & H323Capabilities::operator=(const H323Capabilities & original)
{
  RemoveAll();

  for (PINDEX i = 0; i < original.GetSize(); i++)
    Copy(original[i]);

  PINDEX outerSize = original.set.GetSize();
  set.SetSize(outerSize);
  for (PINDEX outer = 0; outer < outerSize; outer++) {
    PINDEX middleSize = original.set[outer].GetSize();
    set[outer].SetSize(middleSize);
    for (PINDEX middle = 0; middle < middleSize; middle++) {
      PINDEX innerSize = original.set[outer][middle].GetSize();
      for (PINDEX inner = 0; inner < innerSize; inner++)
        set[outer][middle].Append(FindCapability(original.set[outer][middle][inner].GetCapabilityNumber()));
    }
  }

  return *this;
}


void H323Capabilities::PrintOn(ostream & strm) const
{
  int indent = (int)strm.precision()-1;
  strm << setw(indent) << " " << "Table:\n";
  for (PINDEX i = 0; i < table.GetSize(); i++)
    strm << setw(indent+2) << " " << table[i] << '\n';

  strm << setw(indent) << " " << "Set:\n";
  for (PINDEX outer = 0; outer < set.GetSize(); outer++) {
    strm << setw(indent+2) << " " << outer << ":\n";
    for (PINDEX middle = 0; middle < set[outer].GetSize(); middle++) {
      strm << setw(indent+4) << " " << middle << ":\n";
      for (PINDEX inner = 0; inner < set[outer][middle].GetSize(); inner++)
        strm << setw(indent+6) << " " << set[outer][middle][inner] << '\n';
    }
  }
}


PINDEX H323Capabilities::SetCapability(PINDEX descriptorNum,
                                       PINDEX simultaneousNum,
                                       H323Capability * capability)
{
  if (capability == NULL)
    return P_MAX_INDEX;

  // Make sure capability has been added to table.
  Add(capability);

  BOOL newDescriptor = descriptorNum == P_MAX_INDEX;
  if (newDescriptor)
    descriptorNum = set.GetSize();

  // Make sure the outer array is big enough
  set.SetMinSize(descriptorNum+1);

  if (simultaneousNum == P_MAX_INDEX)
    simultaneousNum = set[descriptorNum].GetSize();

  // Make sure the middle array is big enough
  set[descriptorNum].SetMinSize(simultaneousNum+1);

  // Now we can put the new entry in.
  set[descriptorNum][simultaneousNum].Append(capability);
  return newDescriptor ? descriptorNum : simultaneousNum;
}


static BOOL MatchWildcard(const PCaselessString & str, const PStringArray & wildcard)
{
  PINDEX last = 0;
  for (PINDEX i = 0; i < wildcard.GetSize(); i++) {
    if (wildcard[i].IsEmpty())
      last = str.GetLength();
    else {
      PINDEX next = str.Find(wildcard[i], last);
      if (next == P_MAX_INDEX)
        return FALSE;
      last = next + wildcard[i].GetLength();
    }
  }

  return TRUE;
}


PINDEX H323Capabilities::AddAllCapabilities(PINDEX descriptorNum,
                                            PINDEX simultaneous,
                                            const PString & name)
{
  PINDEX reply = descriptorNum == P_MAX_INDEX ? P_MAX_INDEX : simultaneous;

  PStringArray wildcard = name.Tokenise('*', FALSE);

  H323CapabilityFactory::KeyList_T stdCaps = H323CapabilityFactory::GetKeyList();

  for (unsigned session = OpalMediaFormat::FirstSessionID; session <= OpalMediaFormat::LastSessionID; session++) {
    for (H323CapabilityFactory::KeyList_T::const_iterator r = stdCaps.begin(); r != stdCaps.end(); ++r) {
      PString capName(*r);
      if (MatchWildcard(capName, wildcard) && (FindCapability(capName) == NULL)) {
        OpalMediaFormat mediaFormat(capName);
        if (!mediaFormat.IsValid() && (capName.Right(4) == "{sw}") && capName.GetLength() > 4)
          mediaFormat = OpalMediaFormat(capName.Left(capName.GetLength()-4));
        if (mediaFormat.IsValid() && mediaFormat.GetDefaultSessionID() == session) {
          // add the capability
          H323Capability * capability = H323Capability::Create(capName);
          PINDEX num = SetCapability(descriptorNum, simultaneous, capability);
          if (descriptorNum == P_MAX_INDEX) {
            reply = num;
            descriptorNum = num;
            simultaneous = P_MAX_INDEX;
          }
          else if (simultaneous == P_MAX_INDEX) {
            if (reply == P_MAX_INDEX)
              reply = num;
            simultaneous = num;
          }
        }
      }
    }
    simultaneous = P_MAX_INDEX;
  }

  return reply;
}

static unsigned MergeCapabilityNumber(const H323CapabilitiesList & table,
                                      unsigned newCapabilityNumber)
{
  // Assign a unique number to the codec, check if the user wants a specific
  // value and start with that.
  if (newCapabilityNumber == 0)
    newCapabilityNumber = 1;

  PINDEX i = 0;
  while (i < table.GetSize()) {
    if (table[i].GetCapabilityNumber() != newCapabilityNumber)
      i++;
    else {
      // If it already in use, increment it
      newCapabilityNumber++;
      i = 0;
    }
  }

  return newCapabilityNumber;
}


void H323Capabilities::Add(H323Capability * capability)
{
  if (capability == NULL)
    return;

  // See if already added, confuses things if you add the same instance twice
  if (table.GetObjectsIndex(capability) != P_MAX_INDEX)
    return;

  capability->SetCapabilityNumber(MergeCapabilityNumber(table, 1));
  table.Append(capability);

  PTRACE(3, "H323\tAdded capability: " << *capability);
}


H323Capability * H323Capabilities::Copy(const H323Capability & capability)
{
  H323Capability * newCapability = (H323Capability *)capability.Clone();
  newCapability->SetCapabilityNumber(MergeCapabilityNumber(table, capability.GetCapabilityNumber()));
  table.Append(newCapability);

  PTRACE(3, "H323\tAdded capability: " << *newCapability);
  return newCapability;
}


void H323Capabilities::Remove(H323Capability * capability)
{
  if (capability == NULL)
    return;

  PTRACE(3, "H323\tRemoving capability: " << *capability);

  unsigned capabilityNumber = capability->GetCapabilityNumber();

  for (PINDEX outer = 0; outer < set.GetSize(); outer++) {
    for (PINDEX middle = 0; middle < set[outer].GetSize(); middle++) {
      for (PINDEX inner = 0; inner < set[outer][middle].GetSize(); inner++) {
        if (set[outer][middle][inner].GetCapabilityNumber() == capabilityNumber) {
          set[outer][middle].RemoveAt(inner);
          break;
        }
      }
      if (set[outer][middle].GetSize() == 0)
        set[outer].RemoveAt(middle);
    }
    if (set[outer].GetSize() == 0)
      set.RemoveAt(outer);
  }

  table.Remove(capability);
}


void H323Capabilities::Remove(const PString & codecName)
{
  if (codecName.IsEmpty())
    return;

  H323Capability * cap = FindCapability(codecName);
  while (cap != NULL) {
    Remove(cap);
    cap = FindCapability(codecName);
  }
}


void H323Capabilities::Remove(const PStringArray & codecNames)
{
  for (PINDEX i = 0; i < codecNames.GetSize(); i++)
    Remove(codecNames[i]);
}


void H323Capabilities::RemoveAll()
{
  table.RemoveAll();
  set.RemoveAll();
}


H323Capability * H323Capabilities::FindCapability(unsigned capabilityNumber) const
{
  PTRACE(4, "H323\tFindCapability1: " << capabilityNumber);

  for (PINDEX i = 0; i < table.GetSize(); i++) {
    if (table[i].GetCapabilityNumber() == capabilityNumber) {
      PTRACE(3, "H323\tFound capability: " << table[i]);
      return &table[i];
    }
  }

  return NULL;
}


H323Capability * H323Capabilities::FindCapability(const PString & formatName,
                              H323Capability::CapabilityDirection direction) const
{
  PTRACE(4, "H323\tFindCapability2: \"" << formatName << '"');

  PStringArray wildcard = formatName.Tokenise('*', FALSE);

  for (PINDEX i = 0; i < table.GetSize(); i++) {
    PCaselessString str = table[i].GetFormatName();
//    PTRACE(4, "H323\tCheckCapability2: \"" << str << '"');
    if (MatchWildcard(str, wildcard) &&
          (direction == H323Capability::e_Unknown ||
           table[i].GetCapabilityDirection() == direction)) {
      PTRACE(3, "H323\tFound capability: " << table[i]);
      return &table[i];
    }
  }

  return NULL;
}


H323Capability * H323Capabilities::FindCapability(
                              H323Capability::CapabilityDirection direction) const
{
  PTRACE(4, "H323\tFindCapability3: \"" << direction << '"');

  for (PINDEX i = 0; i < table.GetSize(); i++) {
    if (table[i].GetCapabilityDirection() == direction) {
      PTRACE(3, "H323\tFound capability: " << table[i]);
      return &table[i];
    }
  }

  return NULL;
}


H323Capability * H323Capabilities::FindCapability(const H323Capability & capability) const
{
  PTRACE(4, "H323\tFindCapability4: " << capability);

  if (capability.GetMainType() == H323Capability::e_Video)
  {
   PString name = capability.GetFormatName();
   for (PINDEX i = 0; i < table.GetSize(); i++) 
   {
    if (table[i].GetFormatName() == name) 
    {
     PTRACE(3, "H323\tFound capability: " << table[i]);
     return &table[i];
    }
   }
   return NULL;
   
   const OpalMediaFormat & format = capability.GetMediaFormat(); 
   if(format.GetOptionInteger("CIF16 MPI")>0) // looking for CIF16
   {
    PTRACE(3, "H323\tFind capability: CIF16");
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     if (table[i] == capability) {
        const OpalMediaFormat & tformat = table[i].GetMediaFormat();
        if(tformat.GetOptionInteger("CIF16 MPI")>0) {
           PTRACE(3, "H323\tFound capability CIF16: " << table[i]);
           return &table[i];
        }
     }
    }
    return NULL;
   }
   if(format.GetOptionInteger("CIF4 MPI")>0) // looking for CIF4
   {
    PTRACE(3, "H323\tFind capability: CIF4=" << format.GetOptionInteger("CIF4 MPI"));
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     if (table[i] == capability) {
        const OpalMediaFormat & tformat = table[i].GetMediaFormat();
        if(tformat.GetOptionInteger("CIF4 MPI")>0) {
           PTRACE(3, "H323\tFound capability CIF4: " << table[i]);
           return &table[i];
        }
     }
    }
    return NULL;
   }
   if(format.GetOptionInteger("CIF MPI")>0) // looking for CIF
   {
    PTRACE(3, "H323\tFind capability: CIF");
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     if (table[i] == capability) {
        const OpalMediaFormat & tformat = table[i].GetMediaFormat();
        if(tformat.GetOptionInteger("CIF MPI")>0) {
           PTRACE(3, "H323\tFound capability CIF: " << table[i]);
           return &table[i];
        }
     }
    }
    return NULL;
   }
   if(format.GetOptionInteger("QCIF MPI")>0) // looking for QCIF
   {
    PTRACE(3, "H323\tFind capability: QCIF");
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     if (table[i] == capability) {
        const OpalMediaFormat & tformat = table[i].GetMediaFormat();
        if(tformat.GetOptionInteger("QCIF MPI")>0) {
           PTRACE(3, "H323\tFound capability QCIF: " << table[i]);
           return &table[i];
        }
     }
    }
    return NULL;
   }
   if(format.GetOptionInteger("SQCIF MPI")>0) // looking for SQCIF
   {
    PTRACE(3, "H323\tFind capability: SQCIF");
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     if (table[i] == capability) {
        const OpalMediaFormat & tformat = table[i].GetMediaFormat();
        if(tformat.GetOptionInteger("SQCIF MPI")>0) {
           PTRACE(3, "H323\tFound capability SQCIF: " << table[i]);
           return &table[i];
        }
     }
    }
   }
    return NULL;
  }
  else
  for (PINDEX i = 0; i < table.GetSize(); i++) {
    if (table[i] == capability) {
      PTRACE(3, "H323\tFound capability: " << table[i]);
      return &table[i];
    }
  }

  return NULL;
}


H323Capability * H323Capabilities::FindCapability(const H245_Capability & cap) const
{
  PTRACE(4, "H323\tFindCapability5: " << cap.GetTagName());

  switch (cap.GetTag()) {
    case H245_Capability::e_receiveAudioCapability :
    case H245_Capability::e_transmitAudioCapability :
    case H245_Capability::e_receiveAndTransmitAudioCapability :
    {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      PStringStream c; c << cap;
      if(c.Find("capabilityIdentifier = standard ") != P_MAX_INDEX)
      { // check for something like: "capabilityIdentifier = standard 1.3.6.1.4.1.17091.1.3"
#       define _MY_CAP_SEARCHER(myid,mycap) \
        if (c.Find(myid) != P_MAX_INDEX) \
        { \
          for(PINDEX i=0; i<table.GetSize(); i++) \
          { \
            H323Capability & capability = table[i]; \
            PString tCap=capability.GetFormatName(); \
            tCap=tCap.RightTrim(); \
            if(tCap.Right(4)=="{sw}") tCap=tCap.Left(tCap.GetLength()-4); \
            if(tCap == mycap) \
            { \
              PTRACE(3, "H323\tFound capability*: " << table[i]); \
              return &capability; \
            } \
          } \
        }
        _MY_CAP_SEARCHER("1.3.6.1.4.1.17091.1.10.482", "OPUS_48K2");
        _MY_CAP_SEARCHER("1.3.6.1.4.1.17091.1.10.48" , "OPUS_48K");
        _MY_CAP_SEARCHER("1.3.6.1.4.1.17091.1.10.16" , "OPUS_16K");
        _MY_CAP_SEARCHER("1.3.6.1.4.1.17091.1.10.8"  , "OPUS_8K");
        _MY_CAP_SEARCHER("1.3.6.1.4.1.17091.1.3.24"  , "SILK_B40_24K");
        _MY_CAP_SEARCHER("1.3.6.1.4.1.17091.1.3"     , "SILK_B40");
#       undef _MY_CAP_SEARCHER
      }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // OpalPluginCodec_Identifer_G7221
      if(c.Find("standard 0.0.7.7221.1.0") != P_MAX_INDEX)
      {
        int bitrate = 0;
        if(c.Find("maxBitRate = 24000") != P_MAX_INDEX) bitrate = 24000;
        else if(c.Find("maxBitRate = 32000") != P_MAX_INDEX) bitrate = 32000;

        for(PINDEX i = 0; i < table.GetSize(); i++)
        {
          H323Capability & capability = table[i];
          if(capability.GetMainType() == H323Capability::e_Audio && capability.GetFormatName().Find("G.722.1-")==0)
          {
            int _bitrate = capability.GetMediaFormat().GetBandwidth();
            if(_bitrate == bitrate)
            {
              PTRACE(3, "H323\tFound capability*: " << capability);
              return &capability;
            }
          }
        }
        return NULL;
      }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      const H245_AudioCapability & audio = cap;
      return FindCapability(H323Capability::e_Audio, audio, NULL);
    }

    case H245_Capability::e_receiveVideoCapability :
    case H245_Capability::e_transmitVideoCapability :
    case H245_Capability::e_receiveAndTransmitVideoCapability :
    {
      const H245_VideoCapability & video = cap;
      return FindCapability(video);
    }

    case H245_Capability::e_receiveDataApplicationCapability :
    case H245_Capability::e_transmitDataApplicationCapability :
    case H245_Capability::e_receiveAndTransmitDataApplicationCapability :
    {
      const H245_DataApplicationCapability & data = cap;
      return FindCapability(H323Capability::e_Data, data.m_application, NULL);
    }

    case H245_Capability::e_receiveUserInputCapability :
    case H245_Capability::e_transmitUserInputCapability :
    case H245_Capability::e_receiveAndTransmitUserInputCapability :
    {
      const H245_UserInputCapability & ui = cap;
      return FindCapability(H323Capability::e_UserInput, ui, NULL);
    }

    case H245_Capability::e_receiveRTPAudioTelephonyEventCapability :
      return FindCapability(H323Capability::e_UserInput, SignalToneRFC2833_SubType);

	case H245_Capability::e_genericControlCapability :
	  return FindCapability(H323Capability::e_GenericControl);

	case H245_Capability::e_conferenceCapability :
	  return FindCapability(H323Capability::e_ConferenceControl);

    default :
      break;
  }

  return NULL;
}


H323Capability * H323Capabilities::FindCapability(const H245_DataType & dataType) const
{
  PTRACE(4, "H323\tFindCapability6: " << dataType.GetTagName());

  for (PINDEX i = 0; i < table.GetSize(); i++) {
    H323Capability & capability = table[i];
    BOOL checkExact;
    switch (dataType.GetTag()) {
      case H245_DataType::e_audioData :
      {
        const H245_AudioCapability & audio = dataType;
        checkExact = capability.IsMatch(audio);
        break;
      }

      case H245_DataType::e_videoData :
      {
        const H245_VideoCapability & video = dataType;
        checkExact = capability.IsMatch(video);
        break;
      }

      case H245_DataType::e_data :
      {
        const H245_DataApplicationCapability & data = dataType;
        checkExact = capability.IsMatch(data.m_application);
        break;
      }

      default :
        checkExact = FALSE;
    }

    if (checkExact) {
      H323Capability * compare = (H323Capability *)capability.Clone();
      if (compare->OnReceivedPDU(dataType, FALSE) && *compare == capability) {
        delete compare;
        PTRACE(3, "H323\tFound capability: " << capability);
        return &capability;
      }
      delete compare;
    }
  }

  return NULL;
}


H323Capability * H323Capabilities::FindCapability(const H245_ModeElement & modeElement) const
{
  PTRACE(4, "H323\tFindCapability7: " << modeElement.m_type.GetTagName());

  switch (modeElement.m_type.GetTag()) {
    case H245_ModeElementType::e_audioMode :
      {
        const H245_AudioMode & audio = modeElement.m_type;
        static unsigned const AudioSubTypes[] = {
	  H245_AudioCapability::e_nonStandard,
	  H245_AudioCapability::e_g711Alaw64k,
	  H245_AudioCapability::e_g711Alaw56k,
	  H245_AudioCapability::e_g711Ulaw64k,
	  H245_AudioCapability::e_g711Ulaw56k,
	  H245_AudioCapability::e_g722_64k,
	  H245_AudioCapability::e_g722_56k,
	  H245_AudioCapability::e_g722_48k,
	  H245_AudioCapability::e_g728,
	  H245_AudioCapability::e_g729,
	  H245_AudioCapability::e_g729AnnexA,
	  H245_AudioCapability::e_g7231,
	  H245_AudioCapability::e_is11172AudioCapability,
	  H245_AudioCapability::e_is13818AudioCapability,
	  H245_AudioCapability::e_g729wAnnexB,
	  H245_AudioCapability::e_g729AnnexAwAnnexB,
	  H245_AudioCapability::e_g7231AnnexCCapability,
	  H245_AudioCapability::e_gsmFullRate,
	  H245_AudioCapability::e_gsmHalfRate,
	  H245_AudioCapability::e_gsmEnhancedFullRate,
	  H245_AudioCapability::e_genericAudioCapability,
	  H245_AudioCapability::e_g729Extensions
        };
        return FindCapability(H323Capability::e_Audio, audio, AudioSubTypes);
      }

    case H245_ModeElementType::e_videoMode :
      {
        const H245_VideoMode & video = modeElement.m_type;
        static unsigned const VideoSubTypes[] = {
	  H245_VideoCapability::e_nonStandard,
	  H245_VideoCapability::e_h261VideoCapability,
	  H245_VideoCapability::e_h262VideoCapability,
	  H245_VideoCapability::e_h263VideoCapability,
	  H245_VideoCapability::e_is11172VideoCapability,
	  H245_VideoCapability::e_genericVideoCapability
        };
        return FindCapability(H323Capability::e_Video, video, VideoSubTypes);
      }

    case H245_ModeElementType::e_dataMode :
      {
        const H245_DataMode & data = modeElement.m_type;
        static unsigned const DataSubTypes[] = {
	  H245_DataApplicationCapability_application::e_nonStandard,
	  H245_DataApplicationCapability_application::e_t120,
	  H245_DataApplicationCapability_application::e_dsm_cc,
	  H245_DataApplicationCapability_application::e_userData,
	  H245_DataApplicationCapability_application::e_t84,
	  H245_DataApplicationCapability_application::e_t434,
	  H245_DataApplicationCapability_application::e_h224,
	  H245_DataApplicationCapability_application::e_nlpid,
	  H245_DataApplicationCapability_application::e_dsvdControl,
	  H245_DataApplicationCapability_application::e_h222DataPartitioning,
	  H245_DataApplicationCapability_application::e_t30fax,
	  H245_DataApplicationCapability_application::e_t140,
	  H245_DataApplicationCapability_application::e_t38fax,
	  H245_DataApplicationCapability_application::e_genericDataCapability
        };
        return FindCapability(H323Capability::e_Data, data.m_application, DataSubTypes);
      }

    default :
      break;
  }

  return NULL;
}


H323Capability * H323Capabilities::FindCapability(H323Capability::MainTypes mainType,
                                                  const PASN_Choice & subTypePDU,
                                                  const unsigned * translationTable) const
{
    unsigned int subTypeID = subTypePDU.GetTag();
  if (subTypePDU.GetTag() != 0) {
    if (translationTable != NULL)
      subTypeID = translationTable[subTypeID];
    return FindCapability(mainType, subTypeID);
  }

  PTRACE(4, "H323\tFindCapability8: " << mainType << " nonStandard");

  for (PINDEX i = 0; i < table.GetSize(); i++) {
    H323Capability & capability = table[i];
    if (capability.IsMatch(subTypePDU)) {
      PTRACE(3, "H323\tFound capability: " << capability);
      return &capability;
    }
  }

  return NULL;
}

H323Capability * H323Capabilities::FindCapability(H323Capability::MainTypes mainType,
                                                  unsigned subType) const
{
  if (subType != UINT_MAX) {
     PTRACE(4, "H323\tFindCapability9: " << mainType << " subtype=" << subType);
  }

  for (PINDEX i = 0; i < table.GetSize(); i++) {
    H323Capability & capability = table[i];
    if (capability.GetMainType() == mainType &&
                        (subType == UINT_MAX || capability.GetSubType() == subType)) {
      PTRACE(3, "H323\tFound capability: " << capability);
      return &capability;
    }
  }

  return NULL;
}

H323Capability * H323Capabilities::FindCapability(const H245_VideoCapability & video) const
{
//  PTRACE(3, "H323\tFindCapability12 " << video << "subtype " << video.GetTag());
  int frs[5]={0};
  int plus=0;
  unsigned int subType = video.GetTag();
  if(subType==1)
  {
   const H245_H261VideoCapability & h261 = video;
   if(h261.HasOptionalField(H245_H261VideoCapability::e_cifMPI)) frs[2]=1;
   if(h261.HasOptionalField(H245_H261VideoCapability::e_qcifMPI)) frs[3]=1;
  }
  if(subType==3)
  {
   const H245_H263VideoCapability & h263 = video;
   if(h263.HasOptionalField(H245_H263VideoCapability::e_cif16MPI)) frs[0]=1;
   if(h263.HasOptionalField(H245_H263VideoCapability::e_cif4MPI)) frs[1]=1;
   if(h263.HasOptionalField(H245_H263VideoCapability::e_cifMPI)) frs[2]=1;
   if(h263.HasOptionalField(H245_H263VideoCapability::e_qcifMPI)) frs[3]=1;
   if(h263.HasOptionalField(H245_H263VideoCapability::e_sqcifMPI)) frs[4]=1;
// h.263+ capability, RFC 2429 needed
   if(h263.HasOptionalField(H245_H263VideoCapability::e_h263Options)) plus=2;
   else plus=1;
  }
  if (subType != 0) {
   if(frs[0]) {
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     H323Capability & capability = table[i];
     if (capability.GetMainType() == H323Capability::e_Video &&
         (subType == UINT_MAX || capability.GetSubType() == subType)) {
       const OpalMediaFormat & format = capability.GetMediaFormat();
       PString mediaPacketization = format.GetOptionString("Media Packetization");
       if ( plus==0
          ||(mediaPacketization.IsEmpty() && plus==1)
          ||(mediaPacketization.NumCompare("RFC2429") == PObject::EqualTo && plus==2))
       if(format.GetOptionInteger("CIF16 MPI")>0) {
         PTRACE(3, "H323\tFound capability: " << capability);
         return &capability;
        }
     }
    }
   }
   if(frs[1]) {
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     H323Capability & capability = table[i];
     if (capability.GetMainType() == H323Capability::e_Video &&
         (subType == UINT_MAX || capability.GetSubType() == subType)) {
       const OpalMediaFormat & format = capability.GetMediaFormat(); 
       PString mediaPacketization = format.GetOptionString("Media Packetization");
       if ( plus==0
          ||(mediaPacketization.IsEmpty() && plus==1)
          ||(mediaPacketization.NumCompare("RFC2429") == PObject::EqualTo && plus==2))
       if(format.GetOptionInteger("CIF4 MPI")>0) {
         PTRACE(3, "H323\tFound capability: " << capability);
         return &capability;
        }
     }
    }
   }
   if(frs[2]) {
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     H323Capability & capability = table[i];
     if (capability.GetMainType() == H323Capability::e_Video &&
         (subType == UINT_MAX || capability.GetSubType() == subType)) {
       const OpalMediaFormat & format = capability.GetMediaFormat(); 
       PString mediaPacketization = format.GetOptionString("Media Packetization");
       if ( plus==0
          ||(mediaPacketization.IsEmpty() && plus==1)
          ||(mediaPacketization.NumCompare("RFC2190") == PObject::EqualTo && plus==1)
          ||(mediaPacketization.NumCompare("RFC2429") == PObject::EqualTo && plus==2))
       if(format.GetOptionInteger("CIF MPI")>0) {
         PTRACE(3, "H323\tFound capability: " << capability);
         return &capability;
        }
     }
    }
   }
   if(frs[3]) {
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     H323Capability & capability = table[i];
     if (capability.GetMainType() == H323Capability::e_Video &&
         (subType == UINT_MAX || capability.GetSubType() == subType)) {
       const OpalMediaFormat & format = capability.GetMediaFormat(); 
       PString mediaPacketization = format.GetOptionString("Media Packetization");
       if ( plus==0
          ||(mediaPacketization.IsEmpty() && plus==1)
          ||(mediaPacketization.NumCompare("RFC2429") == PObject::EqualTo && plus==2))
       if(format.GetOptionInteger("QCIF MPI")>0) {
         PTRACE(3, "H323\tFound capability: " << capability);
         return &capability;
        }
     }
    }
   }
   if(frs[4]) {
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     H323Capability & capability = table[i];
     if (capability.GetMainType() == H323Capability::e_Video &&
         (subType == UINT_MAX || capability.GetSubType() == subType)) {
       const OpalMediaFormat & format = capability.GetMediaFormat(); 
       PString mediaPacketization = format.GetOptionString("Media Packetization");
       if ( plus==0
          ||(mediaPacketization.IsEmpty() && plus==1)
          ||(mediaPacketization.NumCompare("RFC2429") == PObject::EqualTo && plus==2))
       if(format.GetOptionInteger("SQCIF MPI")>0) {
         PTRACE(3, "H323\tFound capability: " << capability);
         return &capability;
        }
     }
    }
   }

   if(subType==5) //h.264 //vp8 only for openmcu.ru
   {
     {
       PStringStream cap_str; cap_str << video; //vp8
       PINDEX fake_vp8 = cap_str.Find("capabilityIdentifier = standard 1.3.6.1.4.1.17091.1.9.");
       if(fake_vp8 != P_MAX_INDEX)
       {
         int width = 0, height = 0;
         for(PINDEX i = 0; i < table.GetSize(); i++)
         {
           H323Capability & capability = table[i];
           if(capability.GetMainType() == H323Capability::e_Video && capability.GetFormatName().Find("VP8")==0)
           {
             // new format
             if(width == 0 || height == 0)
             {
               H323GenericVideoCapability * cap = (H323GenericVideoCapability *)capability.Clone();
               cap->OnReceivedPDU(video, (H323Capability::CommandType)0);
               const OpalMediaFormat & mf = cap->GetMediaFormat();
               width = mf.GetOptionInteger("Generic Parameter 1");
               height = mf.GetOptionInteger("Generic Parameter 2");
               delete cap;
             }
             // old format
             if(width == 0 || height == 0)
             {
               PINDEX resolutionId = cap_str.Mid(fake_vp8+54,2).AsInteger();
               if(     resolutionId ==  0) { width = 176;  height = 144; }  // "VP8-QCIF"
               else if(resolutionId ==  1) { width = 352;  height = 288; }  // "VP8-CIF"
               else if(resolutionId ==  2) { width = 704;  height = 576; }  // "VP8-4CIF"
               else if(resolutionId == 10) { width = 852;  height = 480; }  // "VP8-480P"
               else if(resolutionId == 11) { width = 1280; height = 720; }  // "VP8-720P"
               else if(resolutionId == 12) { width = 1920; height = 1080; } // "VP8-1080P"
               else if(resolutionId == 21) { width = 424;  height = 240; }  // "VP8-240P"
               else if(resolutionId == 22) { width = 640;  height = 360; }  // "VP8-360P"
               else if(resolutionId == 23) { width = 1364; height = 768; }  // "VP8-768P"
             }
             if(width == 0 || height == 0)
               return NULL;
             const OpalMediaFormat & mf = capability.GetMediaFormat();
             int cap_width = mf.GetOptionInteger("Generic Parameter 1");
             int cap_height = mf.GetOptionInteger("Generic Parameter 2");
             if(width == cap_width && height == cap_height)
             {
               PTRACE(3, "H323\tFound capability*: " << capability);
               return &capability;
             }
           }
         }
         return NULL;
       }
     }
    int profile=-1, level=0; //h.264
    for (PINDEX i = 0; i < table.GetSize(); i++) {
     H323Capability & capability = table[i];
     if (capability.GetMainType() == H323Capability::e_Video &&
         (capability.GetFormatName().Find(".264") != P_MAX_INDEX) && //count on current H.264_123 plugin capability table //kay27
         (subType == UINT_MAX || capability.GetSubType() == subType)) {
         if(profile<0)
         {
          H323GenericVideoCapability * cap = (H323GenericVideoCapability *) capability.Clone();
          cap->OnReceivedPDU(video,(H323Capability::CommandType)0);
          const OpalMediaFormat & format = cap->GetMediaFormat();
          profile=format.GetOptionInteger("Generic Parameter 41");
          level=format.GetOptionInteger("Generic Parameter 42");
          delete cap;
         }
         const OpalMediaFormat & format = capability.GetMediaFormat();
	 int cap_profile=format.GetOptionInteger("Generic Parameter 41");
	 int cap_level=format.GetOptionInteger("Generic Parameter 42");
	 if(cap_profile!=profile || cap_level!=level) continue;
         PTRACE(3, "H323\tFound capability: " << capability);
         return &capability;
        }
    }
   }

   return NULL;
  }

  PTRACE(4, "H323\tFindCapability10: Video nonStandard");
  for (PINDEX i = 0; i < table.GetSize(); i++) {
    H323Capability & capability = table[i];
     if (capability.GetMainType() == H323Capability::e_Video && capability.IsMatch(video)) {
      PTRACE(3, "H323\tFound capability: " << capability);
      return &capability;
    }
  }

  return NULL;
}


BOOL H323Capabilities::FindCapability(const H323Capability & capability, unsigned ID) const
{
   PTRACE(4, "H323\tFindCapability11: " << capability);

 if(capability.GetSubType()==5) return TRUE; // test

   const OpalMediaFormat & format = capability.GetMediaFormat(); 
   for (PINDEX i = 0; i < table.GetSize(); i++) {
    if (table[i].GetDefaultSessionID() == ID) {
      int subt1=capability.GetSubType();
      int subt2=table[i].GetSubType();
      if(subt1<subt2) return FALSE;
      const OpalMediaFormat & tformat = table[i].GetMediaFormat();
      if(subt1==3 || subt2==3)  // compare rtp packetization RFC for h.263
      {
       PString mp1=format.GetOptionString("Media Packetization");
       PString mp2=tformat.GetOptionString("Media Packetization");
       if(mp1.IsEmpty() && !mp2.IsEmpty()) return FALSE;
      }
      if(format.GetOptionInteger("CIF16 MPI") > 0) continue;
      if(tformat.GetOptionInteger("CIF16 MPI") > 0) return FALSE;
      if(format.GetOptionInteger("CIF4 MPI") > 0) continue;
      if(tformat.GetOptionInteger("CIF4 MPI") > 0) return FALSE;
      if(format.GetOptionInteger("CIF MPI") > 0) continue;
      if(tformat.GetOptionInteger("CIF MPI") > 0) return FALSE;
      if(format.GetOptionInteger("QCIF MPI") > 0) continue;
      if(tformat.GetOptionInteger("QCIF MPI") > 0) return FALSE;
      if(format.GetOptionInteger("SCIF MPI") > 0) continue;
      if(tformat.GetOptionInteger("SCIF MPI") > 0) return FALSE;
    }
   }

   H323Capability *wcapability = FindCapability(capability);
   OpalMediaFormat & wf = wcapability->GetWritableMediaFormat(); 
   int fl=0;
   if(fl==0 && wf.GetOptionInteger("CIF16 MPI") > 0) fl++;
   if(fl==0 && wf.GetOptionInteger("CIF4 MPI") > 0) fl++; else wf.SetOptionInteger("CIF4 MPI",0);
   if(fl==0 && wf.GetOptionInteger("CIF MPI") > 0) fl++; else wf.SetOptionInteger("CIF MPI",0);
   if(fl==0 && wf.GetOptionInteger("QCIF MPI") > 0) fl++; else wf.SetOptionInteger("QCIF MPI",0);
   if(fl==0 && wf.GetOptionInteger("SQCIF MPI") > 0) fl++; else wf.SetOptionInteger("SQCIF MPI",0);
   return TRUE;
}


BOOL H323Capabilities::RemoveCapability(H323Capability::MainTypes capabilityType)
{
	// List of codecs
	PStringList codecsToRemove;
	for (PINDEX i = 0; i < table.GetSize(); i++) {
	    H323Capability & capability = table[i];
		if (capabilityType == H323Capability::e_Video) {
		    if ((capability.GetMainType() == H323Capability::e_Video) &&
			    (capability.GetSubType() != H245_VideoCapability::e_extendedVideoCapability))
			      codecsToRemove.AppendString(capability.GetFormatName()); 
		} else if ((capabilityType == H323Capability::e_ExtendVideo) &&
             (capability.GetMainType() == H323Capability::e_Video) &&
			 (capability.GetSubType() == H245_VideoCapability::e_extendedVideoCapability)) {
                  codecsToRemove.AppendString(capability.GetFormatName());
		} else 
			 if (capability.GetMainType() == capabilityType)
			   codecsToRemove.AppendString(capability.GetFormatName());  
	}

	for (PINDEX i=0; i < codecsToRemove.GetSize(); i++) 
		      Remove(codecsToRemove[i]);

	return TRUE;
}

#ifdef H323_VIDEO
BOOL H323Capabilities::SetVideoFrameSize(H323Capability::CapabilityFrameSize frameSize, int frameUnits) 
{ 
    // Remove the unmatching capabilities
	if (frameSize != H323Capability::cifMPI) Remove("*-CIF*");
    if (frameSize != H323Capability::qcifMPI) Remove("*-QCIF*");
	if (frameSize != H323Capability::sqcifMPI) Remove("*-SQCIF*");

	// Remove Generic size Capabilities
	PStringList genericCaps;
	if ((frameSize != H323Capability::i1080MPI) &&
	    (frameSize != H323Capability::p720MPI) &&
	    (frameSize != H323Capability::i480MPI) &&
		(frameSize != H323Capability::cif16MPI) &&
        (frameSize != H323Capability::cif4MPI)) {
       	for (PINDEX i = 0; i < table.GetSize(); i++) {
	     H323Capability & capability = table[i];
		  if ((capability.GetMainType() == H323Capability::e_Video) &&
		   (capability.GetSubType() != H245_VideoCapability::e_extendedVideoCapability)) {
			 PCaselessString str = table[i].GetFormatName();
		     PString formatName = "*-*";
		     PStringArray wildcard = formatName.Tokenise('*', FALSE);
             if (!MatchWildcard(str, wildcard))
			    genericCaps.AppendString(str);
	      }
	    }
		Remove(genericCaps);
	}

	// Instruct remaining Video Capabilities to set Frame Size to new Value
	for (PINDEX i = 0; i < table.GetSize(); i++) {
	    H323Capability & capability = table[i];
	    if (capability.GetMainType() == H323Capability::e_Video)
			     capability.SetMaxFrameSize(frameSize,frameUnits);
	}
	return TRUE; 
}
#endif

void H323Capabilities::BuildPDU(const H323Connection & connection,
                                H245_TerminalCapabilitySet & pdu) const
{
  PINDEX tableSize = table.GetSize();
  PINDEX setSize = set.GetSize();
  //PAssert((tableSize > 0) == (setSize > 0), PLogicError);
  if (tableSize == 0 || setSize == 0)
    return;

  // Set the table of capabilities
  pdu.IncludeOptionalField(H245_TerminalCapabilitySet::e_capabilityTable);

  H245_H2250Capability & h225_0 = pdu.m_multiplexCapability;
  PINDEX rtpPacketizationCount = 0;

  PINDEX count = 0;
  for (PINDEX i = 0; i < tableSize; i++) {
    H323Capability & capability = table[i];
    if (capability.IsUsable(connection)) {
      pdu.m_capabilityTable.SetSize(count+1);
      H245_CapabilityTableEntry & entry = pdu.m_capabilityTable[count++];
      entry.m_capabilityTableEntryNumber = capability.GetCapabilityNumber();
      entry.IncludeOptionalField(H245_CapabilityTableEntry::e_capability);
      capability.OnSendingPDU(entry.m_capability);

      h225_0.m_mediaPacketizationCapability.m_rtpPayloadType.SetSize(rtpPacketizationCount+1);
      if (H323SetRTPPacketization(h225_0.m_mediaPacketizationCapability.m_rtpPayloadType[rtpPacketizationCount],
                                                    capability.GetMediaFormat(), RTP_DataFrame::MaxPayloadType)) {
        // Check if already in list
        PINDEX test;
        for (test = 0; test < rtpPacketizationCount; test++) {
          if (h225_0.m_mediaPacketizationCapability.m_rtpPayloadType[test] == h225_0.m_mediaPacketizationCapability.m_rtpPayloadType[rtpPacketizationCount])
            break;
        }
        if (test == rtpPacketizationCount)
          rtpPacketizationCount++;
      }
    }
  }

  // Have some mediaPacketizations to include.
  if (rtpPacketizationCount > 0) {
    h225_0.m_mediaPacketizationCapability.m_rtpPayloadType.SetSize(rtpPacketizationCount);
    h225_0.m_mediaPacketizationCapability.IncludeOptionalField(H245_MediaPacketizationCapability::e_rtpPayloadType);
  }

  // Set the sets of compatible capabilities
  pdu.IncludeOptionalField(H245_TerminalCapabilitySet::e_capabilityDescriptors);

  pdu.m_capabilityDescriptors.SetSize(setSize);
  for (PINDEX outer = 0; outer < setSize; outer++) {
    H245_CapabilityDescriptor & desc = pdu.m_capabilityDescriptors[outer];
    desc.m_capabilityDescriptorNumber = (unsigned)(outer + 1);
    desc.IncludeOptionalField(H245_CapabilityDescriptor::e_simultaneousCapabilities);
    PINDEX middleSize = set[outer].GetSize();
    desc.m_simultaneousCapabilities.SetSize(middleSize);
    for (PINDEX middle = 0; middle < middleSize; middle++) {
      H245_AlternativeCapabilitySet & alt = desc.m_simultaneousCapabilities[middle];
      PINDEX innerSize = set[outer][middle].GetSize();
      alt.SetSize(innerSize);
      count = 0;
      for (PINDEX inner = 0; inner < innerSize; inner++) {
        H323Capability & capability = set[outer][middle][inner];
        if (capability.IsUsable(connection)) {
          alt.SetSize(count+1);
          alt[count++] = capability.GetCapabilityNumber();
        }
      }
    }
  }
}


BOOL H323Capabilities::Merge(const H323Capabilities & newCaps)
{
  PTRACE_IF(4, !table.IsEmpty(), "H245\tCapability merge of:\n" << newCaps
            << "\nInto:\n" << *this);

  PTRACE(3, "H245\tCapability add start\n");

  // Add any new capabilities not already in set.
  PINDEX i;
//  for (i = newCaps.GetSize()-1; i >= 0; i--) {
  for (i = 0; i < newCaps.GetSize(); i++) {

  PTRACE(3, "H323\tGetFormatName= " << newCaps[i].GetFormatName());

    if (FindCapability(newCaps[i]) == NULL)
      Copy(newCaps[i]);
  }

  PTRACE(3, "H245\tCapability merge start\n");

  // This should merge instead of just adding to it.
  PINDEX outerSize = newCaps.set.GetSize();
  PINDEX outerBase = set.GetSize();
  set.SetSize(outerBase+outerSize);
  for (PINDEX outer = 0; outer < outerSize; outer++) {
    PINDEX middleSize = newCaps.set[outer].GetSize();
    set[outerBase+outer].SetSize(middleSize);
    for (PINDEX middle = 0; middle < middleSize; middle++) {
      PINDEX innerSize = newCaps.set[outer][middle].GetSize();
      for (PINDEX inner = 0; inner < innerSize; inner++) {
        H323Capability * cap = FindCapability(newCaps.set[outer][middle][inner].GetCapabilityNumber());
        if (cap != NULL)
          set[outerBase+outer][middle].Append(cap);
      }
    }
  }

  PTRACE_IF(4, !table.IsEmpty(), "H245\tCapability merge result:\n" << *this);
  PTRACE(3, "H245\tReceived capability set, is "
                 << (table.IsEmpty() ? "rejected" : "accepted"));
  return !table.IsEmpty();
}

void H323Capabilities::Reorder(const PStringArray & preferenceOrder)
{
  if (preferenceOrder.IsEmpty())
    return;

  table.DisallowDeleteObjects();

  PINDEX preference = 0;
  PINDEX base = 0;

  for (preference = 0; preference < preferenceOrder.GetSize(); preference++) {
    PStringArray wildcard = preferenceOrder[preference].Tokenise('*', FALSE);
    for (PINDEX idx = base; idx < table.GetSize(); idx++) {
      PCaselessString str = table[idx].GetFormatName();
      if (MatchWildcard(str, wildcard)) {
        if (idx != base)
          table.InsertAt(base, table.RemoveAt(idx));
        base++;
      }
    }
  }

  for (PINDEX outer = 0; outer < set.GetSize(); outer++) {
    for (PINDEX middle = 0; middle < set[outer].GetSize(); middle++) {
      H323CapabilitiesList & list = set[outer][middle];
      for (PINDEX idx = 0; idx < table.GetSize(); idx++) {
        for (PINDEX inner = 0; inner < list.GetSize(); inner++) {
          if (&table[idx] == &list[inner]) {
            list.Append(list.RemoveAt(inner));
            break;
          }
        }
      }
    }
  }

  table.AllowDeleteObjects();
}


BOOL H323Capabilities::IsAllowed(const H323Capability & capability)
{
  return IsAllowed(capability.GetCapabilityNumber());
}


BOOL H323Capabilities::IsAllowed(const unsigned a_capno)
{
  // Check that capno is actually in the set
  PINDEX outerSize = set.GetSize();
  for (PINDEX outer = 0; outer < outerSize; outer++) {
    PINDEX middleSize = set[outer].GetSize();
    for (PINDEX middle = 0; middle < middleSize; middle++) {
      PINDEX innerSize = set[outer][middle].GetSize();
      for (PINDEX inner = 0; inner < innerSize; inner++) {
        if (a_capno == set[outer][middle][inner].GetCapabilityNumber()) {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}


BOOL H323Capabilities::IsAllowed(const H323Capability & capability1,
                                 const H323Capability & capability2)
{
  return IsAllowed(capability1.GetCapabilityNumber(),
                   capability2.GetCapabilityNumber());
}


BOOL H323Capabilities::IsAllowed(const unsigned a_capno1, const unsigned a_capno2)
{
  if (a_capno1 == a_capno2) {
    PTRACE(1, "H323\tH323Capabilities::IsAllowed() capabilities are the same.");
    return TRUE;
  }

  PINDEX outerSize = set.GetSize();
  for (PINDEX outer = 0; outer < outerSize; outer++) {
    PINDEX middleSize = set[outer].GetSize();
    for (PINDEX middle = 0; middle < middleSize; middle++) {
      PINDEX innerSize = set[outer][middle].GetSize();
      for (PINDEX inner = 0; inner < innerSize; inner++) {
        if (a_capno1 == set[outer][middle][inner].GetCapabilityNumber()) {
          /* Now go searching for the other half... */
          for (PINDEX middle2 = 0; middle2 < middleSize; ++middle2) {
            if (middle != middle2) {
              PINDEX innerSize2 = set[outer][middle2].GetSize();
              for (PINDEX inner2 = 0; inner2 < innerSize2; ++inner2) {
                if (a_capno2 == set[outer][middle2][inner2].GetCapabilityNumber()) {
                  return TRUE;
                }
              }
            }
          }
        }
      }
    }
  }
  return FALSE;
}


/////////////////////////////////////////////////////////////////////////////

#ifndef PASN_NOPRINTON


struct msNonStandardCodecDef {
  const char * name;
  BYTE sig[2];
};


static msNonStandardCodecDef msNonStandardCodec[] = {
  { "L&H CELP 4.8k", { 0x01, 0x11 } },
  { "ADPCM",         { 0x02, 0x00 } },
  { "L&H CELP 8k",   { 0x02, 0x11 } },
  { "L&H CELP 12k",  { 0x03, 0x11 } },
  { "L&H CELP 16k",  { 0x04, 0x11 } },
  { "IMA-ADPCM",     { 0x11, 0x00 } },
  { "GSM",           { 0x31, 0x00 } },
  { NULL,            { 0,    0    } }
};

void H245_AudioCapability::PrintOn(ostream & strm) const
{
  strm << GetTagName();

  // tag 0 is nonstandard
  if (tag == 0) {

    H245_NonStandardParameter & param = (H245_NonStandardParameter &)GetObject();
    const PBYTEArray & data = param.m_data;

    switch (param.m_nonStandardIdentifier.GetTag()) {
      case H245_NonStandardIdentifier::e_h221NonStandard:
        {
          H245_NonStandardIdentifier_h221NonStandard & h221 = param.m_nonStandardIdentifier;

          // Microsoft is 181/0/21324
          if ((h221.m_t35CountryCode   == 181) &&
              (h221.m_t35Extension     == 0) &&
              (h221.m_manufacturerCode == 21324)
            ) {
            PString name = "Unknown";
            PINDEX i;
            if (data.GetSize() >= 21) {
              for (i = 0; msNonStandardCodec[i].name != NULL; i++) {
                if ((data[20] == msNonStandardCodec[i].sig[0]) && 
                    (data[21] == msNonStandardCodec[i].sig[1])) {
                  name = msNonStandardCodec[i].name;
                  break;
                }
              }
            }
            strm << (PString(" [Microsoft") & name) << "]";
          }

          // Equivalence is 9/0/61
          else if ((h221.m_t35CountryCode   == 9) &&
                   (h221.m_t35Extension     == 0) &&
                   (h221.m_manufacturerCode == 61)
                  ) {
            PString name;
            if (data.GetSize() > 0)
              name = PString((const char *)(const BYTE *)data, data.GetSize());
            strm << " [Equivalence " << name << "]";
          }

          // Xiph is 181/0/38
          else if ((h221.m_t35CountryCode   == 181) &&
                   (h221.m_t35Extension     == 0) &&
                   (h221.m_manufacturerCode == 38)
                  ) {
            PString name;
            if (data.GetSize() > 0)
              name = PString((const char *)(const BYTE *)data, data.GetSize());
            strm << " [Xiph " << name << "]";
          }

          // Cisco is 181/0/18
          else if ((h221.m_t35CountryCode   == 181) &&
                   (h221.m_t35Extension     == 0) &&
                   (h221.m_manufacturerCode == 18)
                  ) {
            PString name;
            if (data.GetSize() > 0)
              name = PString((const char *)(const BYTE *)data, data.GetSize());
            strm << " [Cisco " << name << "]";
          }

        }
        break;
      default:
        break;
    }
  }

  if (choice == NULL)
    strm << " (NULL)";
  else {
    strm << ' ' << *choice;
  }

  //PASN_Choice::PrintOn(strm);
}
#endif
