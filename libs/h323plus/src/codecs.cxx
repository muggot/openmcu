/*
 * codecs.cxx
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
 * $Log: codecs.cxx,v $
 * Revision 1.4  2007/11/14 18:48:44  willamowius
 * avoid comparing a uninitialized variable
 *
 * Revision 1.3  2007/11/10 13:33:51  shorne
 * Fix reference to the correct AEC library based on the type of build
 *
 * Revision 1.2  2007/10/19 19:54:17  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.1  2007/08/06 20:51:03  shorne
 * First commit of h323plus
 *
 * Revision 1.92.2.8  2007/09/03 09:45:47  rjongbloed
 * Fixed failure to propagate meda format options to codec.
 *
 * Revision 1.92.2.7  2007/07/19 20:10:28  shorne
 * Changed HAS_AEC to H323_AEC
 *
 * Revision 1.92.2.6  2007/03/24 23:39:43  shorne
 * More H.239 work
 *
 * Revision 1.92.2.5  2007/03/06 00:18:13  shorne
 * Added Debug AEC support
 *
 * Revision 1.92.2.4  2007/03/05 11:57:12  shorne
 * Fixed compile issue with AEC
 *
 * Revision 1.92.2.3  2007/02/18 18:59:26  shorne
 * AEC tweaks
 *
 * Revision 1.92.2.2  2007/02/06 11:45:59  shorne
 * Added ability to send general codec options to Video Plugins
 *
 * Revision 1.92.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.92  2006/05/16 11:28:58  shorne
 * added AEC support and  more call hold support.
 *
 * Revision 1.91  2006/01/26 03:31:09  shorne
 * Add the ability to remove a local input device when placing a call on hold
 *
 * Revision 1.90  2005/01/03 06:25:54  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.89  2004/11/29 06:30:53  csoutheren
 * Added support for wideband codecs
 *
 * Revision 1.88  2004/07/03 06:49:28  rjongbloed
 * Split video temporal/spatial trade off H.245 packets to separate command and
 *   indication functions and added quality parameter, thanks Guilhem Tardy.
 * Added PTRACE_PARAM() macro to fix warnings on parameters used in PTRACE
 *  macros only.
 *
 * Revision 1.87  2004/05/09 10:22:26  csoutheren
 * Changed new DecodeFrame to handle bytes per frame
 *
 * Revision 1.86  2004/05/09 10:08:36  csoutheren
 * Changed new DecodeFrame to return bytes decoded rather than samples decoded
 * Added support for new DecodeFrame to plugin manager
 *
 * Revision 1.85  2004/05/02 04:52:24  rjongbloed
 * Fixed problems with G.711 caused by fixing problem with G.723.1-5k3 mode.
 *
 * Revision 1.84  2004/04/16 04:04:28  csoutheren
 * Prevent codecs with variable length frrames from doing strange things
 *
 * Revision 1.83  2004/02/04 10:29:27  rjongbloed
 * Fixed G.726 by allowing for more bits per pixels sizes in streamed codec, thanks Kevin Bouchard
 *
 * Revision 1.82  2003/11/12 11:14:51  csoutheren
 * Added H323FramedAudioCodec::DecodeSilenceFrame thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.81  2003/07/16 10:43:13  csoutheren
 * Added SwapChannel function to H323Codec to allow media hold channels
 * to work better. Thanks to Federico Pinna
 *
 * Revision 1.80  2002/12/16 09:11:19  robertj
 * Added new video bit rate control, thanks Walter H. Whitlock
 *
 * Revision 1.79  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.78  2002/05/29 04:48:48  robertj
 * Changed framed codec so if cannot decode frame just plays silence instead
 *   of returning error and thus shutting down channel, thanks Federico Pinna
 *
 * Revision 1.77  2002/04/16 03:27:54  dereks
 * Correct logical flaw in CloseRawDataChannel method.
 *
 * Revision 1.76  2002/04/05 00:52:17  dereks
 * Minor tweaks to cope with received h261 messages.
 *
 * Revision 1.75  2002/02/26 18:00:18  rogerh
 * Improve the information given in the trace for codec truncation
 *
 * Revision 1.74  2002/01/23 06:13:56  robertj
 * Added filter function hooks to codec raw data channel.
 *
 * Revision 1.73  2002/01/23 01:58:28  robertj
 * Added function to determine if codecs raw data channel is native format.
 *
 * Revision 1.72  2002/01/22 16:09:38  rogerh
 * Back out the DTMF detection from H323FramedAudioCodec::Write().
 * There will shortly be a better place for it.
 *
 * Revision 1.71  2002/01/22 15:21:47  rogerh
 * Add DTMF decoding to PCM audio streams. This has been tested with
 * NetMeeting sending Dial Pad codes, using the G.711 codec.
 * At this time, DTMF codes (fron NetMeeting) are just displayed on the
 * screen and are not passed up to the users application.
 *
 * Revision 1.70  2002/01/13 23:55:21  robertj
 * Added mutex so can change raw data channel while reading/writing from codec.
 *
 * Revision 1.69  2002/01/06 05:34:05  robertj
 * Fixed encoding error for 4 bit streamed codecs.
 *
 * Revision 1.68  2001/12/04 05:13:12  robertj
 * Added videa bandwidth limiting code for H.261, thanks Jose Luis Urien.
 *
 * Revision 1.67  2001/11/28 00:09:14  dereks
 * Additional information in PTRACE output.
 *
 * Revision 1.66  2001/11/16 01:05:35  craigs
 * Changed to allow access to uLaw/ALaw to/from linear functions
 *
 * Revision 1.65  2001/10/23 02:17:16  dereks
 * Initial release of cu30 video codec.
 *
 * Revision 1.64  2001/09/25 03:14:47  dereks
 * Add constant bitrate control for the h261 video codec.
 * Thanks Tiziano Morganti for the code to set bit rate. Good work!
 *
 * Revision 1.63  2001/09/21 02:50:06  robertj
 * Implemented static object for all "known" media formats.
 *
 * Revision 1.62  2001/09/12 07:48:05  robertj
 * Fixed various problems with tracing.
 *
 * Revision 1.61  2001/09/11 01:24:36  robertj
 * Added conditional compilation to remove video and/or audio codecs.
 *
 * Revision 1.60  2001/08/06 03:08:56  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.59  2001/04/03 09:34:13  robertj
 * Fixed output of partial frames when short changed by transmitter with G.711
 *
 * Revision 1.58  2001/03/29 23:45:08  robertj
 * Added ability to get current silence detect state and threshold.
 * Changed default signal on deadband time to be much shorter.
 *
 * Revision 1.57  2001/02/09 05:13:55  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.56  2001/01/25 07:27:16  robertj
 * Major changes to add more flexible OpalMediaFormat class to normalise
 *   all information about media types, especially codecs.
 *
 * Revision 1.55  2000/12/19 22:33:44  dereks
 * Adjust so that the video channel is used for reading/writing raw video
 * data, which better modularizes the video codec.
 *
 * Revision 1.54  2000/09/22 01:35:49  robertj
 * Added support for handling LID's that only do symmetric codecs.
 *
 * Revision 1.53  2000/08/31 08:15:41  robertj
 * Added support for dynamic RTP payload types in H.245 OpenLogicalChannel negotiations.
 *
 * Revision 1.52  2000/07/14 14:08:10  robertj
 * Fixed stream based codec so can support stream "frames" less than maximum specified.
 *
 * Revision 1.51  2000/05/16 02:04:17  craigs
 * Added access functions for silence compression mode
 *
 * Revision 1.50  2000/05/04 11:52:35  robertj
 * Added Packets Too Late statistics, requiring major rearrangement of jitter
 *    buffer code, not also changes semantics of codec Write() function slightly.
 *
 * Revision 1.49  2000/05/02 04:32:26  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.48  2000/04/28 12:58:37  robertj
 * Changed silence detection code so does not PTRACE unless threshold actually changes.
 *
 * Revision 1.47  2000/04/10 18:52:45  robertj
 * Improved "bootstrap" of silence detection algorithm.
 *
 * Revision 1.46  2000/03/23 03:00:06  robertj
 * Changed framed codec so only writes max of bytesPerFrame regardless of length.
 *
 * Revision 1.45  2000/02/04 05:11:19  craigs
 * Updated for new Makefiles and for new video transmission code
 *
 * Revision 1.44  2000/01/13 04:03:45  robertj
 * Added video transmission
 *
 * Revision 1.43  1999/12/31 00:05:36  robertj
 * Added Microsoft ACM G.723.1 codec capability.
 *
 * Revision 1.42  1999/12/23 23:02:35  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 * Revision 1.41  1999/12/21 07:36:43  craigs
 * Fixed problem in H323VideoCodec destructor that caused hang or segv on exit
 *
 * Revision 1.40  1999/11/29 08:59:09  craigs
 * Added new code for new video code interface
 *
 * Revision 1.39  1999/11/29 04:50:11  robertj
 * Added adaptive threshold calculation to silence detection.
 *
 * Revision 1.38  1999/11/20 00:53:47  robertj
 * Fixed ability to have variable sized frames in single RTP packet under G.723.1
 *
 * Revision 1.37  1999/11/13 14:10:59  robertj
 * Changes to make silence detection selectable.
 *
 * Revision 1.36  1999/11/11 23:28:46  robertj
 * Added first cut silence detection algorithm.
 *
 * Revision 1.35  1999/11/04 00:45:07  robertj
 * Added extra constructors for nonStandard codecs and fixed receiveAndTransmitAudioCapability problem.
 *
 * Revision 1.34  1999/11/01 00:51:13  robertj
 * Fixed problem where codec close does not dispose of attached channel.
 *
 * Revision 1.33  1999/10/19 00:04:57  robertj
 * Changed OpenAudioChannel and OpenVideoChannel to allow a codec AttachChannel with no autodelete.
 *
 * Revision 1.32  1999/10/14 12:02:40  robertj
 * Fixed assignment of t35 info in nonstandard capabilities (wrong way around).
 *
 * Revision 1.31  1999/10/10 23:00:15  craigs
 * Fixed problem with raw channel ptrs not being NULLed out after deletion
 *
 * Revision 1.30  1999/10/09 02:15:08  craigs
 * Added codec to OpenVideoDevice and OpenAudioChannel
 *
 * Revision 1.29  1999/10/09 01:20:48  robertj
 * Fixed error in G711 packet size and trace message
 *
 * Revision 1.28  1999/10/08 09:59:03  robertj
 * Rewrite of capability for sending multiple audio frames
 *
 * Revision 1.27  1999/10/08 08:32:22  robertj
 * Fixed misleading trace text.
 *
 * Revision 1.26  1999/10/08 04:58:38  robertj
 * Added capability for sending multiple audio frames in single RTP packet
 *
 * Revision 1.25  1999/09/23 07:25:12  robertj
 * Added open audio and video function to connection and started multi-frame codec send functionality.
 *
 * Revision 1.24  1999/09/21 14:51:34  robertj
 * Fixed NonStandardCapabilityInfo class virtual destructor (and name).
 *
 * Revision 1.23  1999/09/21 14:14:36  robertj
 * Added non-standard codec capability classes
 *
 * Revision 1.22  1999/09/21 08:10:03  craigs
 * Added support for video devices and H261 codec
 *
 * Revision 1.21  1999/09/18 13:24:38  craigs
 * Added ability to disable jitter buffer
 * Added ability to access entire RTP packet in codec Write
 *
 * Revision 1.20  1999/09/13 13:59:14  robertj
 * Removed incorrect comment.
 *
 * Revision 1.19  1999/09/08 04:05:49  robertj
 * Added support for video capabilities & codec, still needs the actual codec itself!
 *
 * Revision 1.18  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 * Revision 1.17  1999/08/25 05:05:36  robertj
 * Added UserInput capability.
 * Allowed the attachment of a channel on a codec to optionally delete the channel object,
 * Improved opening of audio codecs, PSoundChannel creation now in endpoint.
 *
 * Revision 1.16  1999/07/16 16:05:48  robertj
 * Added "human readable" codec type name display.
 *
 * Revision 1.15  1999/07/16 15:01:30  robertj
 * Added message print when starting GSM codec.
 *
 * Revision 1.14  1999/07/15 14:45:36  robertj
 * Added propagation of codec open error to shut down logical channel.
 * Fixed control channel start up bug introduced with tunnelling.
 *
 * Revision 1.13  1999/07/13 09:53:24  robertj
 * Fixed some problems with jitter buffer and added more debugging.
 *
 * Revision 1.12  1999/07/10 02:42:53  robertj
 * Fixed interopability problem with NetMetting 2.1 G711 capability.
 *
 * Revision 1.11  1999/07/09 06:09:49  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.10  1999/06/24 13:32:45  robertj
 * Fixed ability to change sound device on codec and fixed NM3 G.711 compatibility
 *
 * Revision 1.9  1999/06/22 13:49:40  robertj
 * Added GSM support and further RTP protocol enhancements.
 *
 * Revision 1.8  1999/06/14 15:08:40  robertj
 * Added GSM codec class frame work (still no actual codec).
 *
 * Revision 1.7  1999/06/14 08:44:58  robertj
 * Fixed sound buffers to be correct size for stream based audio.
 * GNU C++ compatibility
 *
 * Revision 1.6  1999/06/14 06:39:08  robertj
 * Fixed problem with getting transmit flag to channel from PDU negotiator
 *
 * Revision 1.5  1999/06/14 05:15:55  robertj
 * Changes for using RTP sessions correctly in H323 Logical Channel context
 *
 * Revision 1.4  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.3  1999/06/09 05:26:19  robertj
 * Major restructuring of classes.
 *
 * Revision 1.2  1999/06/06 06:06:36  robertj
 * Changes for new ASN compiler and v2 protocol ASN files.
 *
 * Revision 1.1  1999/01/16 01:31:04  robertj
 * Initial revision
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "codecs.h"
#endif

#include "codecs.h"

#include "channels.h"
#include "h323pdu.h"
#include "h323con.h"

#ifdef H323_AEC
#include <ptclib/paec.h>

#if _DEBUG
  #pragma comment(lib,"paecd.lib")
#elif PTRACING
  #pragma comment(lib,"paec.lib")
#else
  #pragma comment(lib,"paecn.lib")
#endif
#endif // H323_AEC


#define new PNEW


extern "C" {
  unsigned char linear2ulaw(int pcm_val);
  int ulaw2linear(unsigned char u_val);
  unsigned char linear2alaw(int pcm_val);
  int alaw2linear(unsigned char u_val);
};


/////////////////////////////////////////////////////////////////////////////

H323Codec::H323Codec(const OpalMediaFormat & fmt, Direction dir)
  : mediaFormat(fmt)
{
  logicalChannel = NULL;
  direction = dir;

  lastSequenceNumber = 1;
  rawDataChannel = NULL;
  deleteChannel  = FALSE;
}


BOOL H323Codec::Open(H323Connection & /*connection*/)
{
  return TRUE;
}


unsigned H323Codec::GetFrameRate() const
{
  return mediaFormat.GetFrameTime();
}


void H323Codec::OnFlowControl(long PTRACE_PARAM(bitRateRestriction))
{
  PTRACE(3, "Codec\tOnFlowControl: " << bitRateRestriction);
}


void H323Codec::OnMiscellaneousCommand(const H245_MiscellaneousCommand_type & PTRACE_PARAM(type))
{
  PTRACE(3, "Codec\tOnMiscellaneousCommand: " << type.GetTagName());
}


void H323Codec::OnMiscellaneousIndication(const H245_MiscellaneousIndication_type & PTRACE_PARAM(type))
{
  PTRACE(3, "Codec\tOnMiscellaneousIndication: " << type.GetTagName());
}


BOOL H323Codec::AttachChannel(PChannel * channel, BOOL autoDelete)
{
  PWaitAndSignal mutex(rawChannelMutex);

  CloseRawDataChannel();

  rawDataChannel = channel;
  deleteChannel = autoDelete;

  if (channel == NULL){
	PTRACE(3, "Codec\tError attaching channel. channel is NULL");
    return FALSE;
  }

  return channel->IsOpen();
}

PChannel * H323Codec::SwapChannel(PChannel * newChannel, BOOL autoDelete)
{
  PWaitAndSignal mutex(rawChannelMutex);

  PChannel * oldChannel = rawDataChannel;

  rawDataChannel = newChannel;
  deleteChannel = autoDelete;

  return oldChannel;
}


BOOL H323Codec::CloseRawDataChannel()
{
  if (rawDataChannel == NULL)
    return FALSE;
  
  BOOL closeOK = rawDataChannel->Close();
  
  if (deleteChannel) {
     delete rawDataChannel;
     rawDataChannel = NULL;
  }
  
  return closeOK;
}  


BOOL H323Codec::IsRawDataChannelNative() const
{
  return FALSE;
}


BOOL H323Codec::ReadRaw(void * data, PINDEX size, PINDEX & length)
{
  if (rawDataChannel == NULL) {
    PTRACE(1, "Codec\tNo audio channel for read");
    return FALSE;
  }

  if (!rawDataChannel->Read(data, size)) {
    PTRACE(1, "Codec\tAudio read failed: " << rawDataChannel->GetErrorText(PChannel::LastReadError));
    return FALSE;
  }

  length = rawDataChannel->GetLastReadCount();

  for (PINDEX i = 0; i < filters.GetSize(); i++) {
    FilterInfo info(*this, data, size, length);
    filters[i](info, 0);
    length = info.bufferLength;
  }

  return TRUE;
}


BOOL H323Codec::WriteRaw(void * data, PINDEX length)
{
  if (rawDataChannel == NULL) {
    PTRACE(1, "Codec\tNo audio channel for write");
    return FALSE;
  }

  for (PINDEX i = 0; i < filters.GetSize(); i++) {
    FilterInfo info(*this, data, length, length);
    filters[i](info, 0);
    length = info.bufferLength;
  }

  if (rawDataChannel->Write(data, length))
    return TRUE;

  PTRACE(1, "Codec\tWrite failed: " << rawDataChannel->GetErrorText(PChannel::LastWriteError));
  return FALSE;
}


BOOL H323Codec::AttachLogicalChannel(H323Channel *channel)
{
  logicalChannel = channel;

  return TRUE;
}


void H323Codec::AddFilter(const PNotifier & notifier)
{
  rawChannelMutex.Wait();
  filters.Append(new PNotifier(notifier));
  rawChannelMutex.Signal();
}

BOOL H323Codec::SetRawDataHeld(BOOL /*hold*/) 
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////

#ifdef H323_VIDEO

H323VideoCodec::H323VideoCodec(const OpalMediaFormat & fmt, Direction dir)
  : H323Codec(fmt, dir)
{
  frameWidth = frameHeight = 0;
  targetFrameTimeMs = 0;
  videoBitRateControlModes = None;

  oldLength = 0;
  oldTime = 0;
  newTime = 0;
}


H323VideoCodec::~H323VideoCodec()
{
  Close();    //The close operation may delete the rawDataChannel.
}


BOOL H323VideoCodec::Open(H323Connection & connection)
{
#ifdef H323_H239
  if (logicalChannel->GetSessionID() == OpalMediaFormat::DefaultExtVideoSessionID)
    return connection.OpenExtendedVideoChannel(direction == Encoder, *this);
  else 
#endif
    return connection.OpenVideoChannel(direction == Encoder, *this);
}


void H323VideoCodec::OnMiscellaneousCommand(const H245_MiscellaneousCommand_type & type)
{
  switch (type.GetTag()) {
    case H245_MiscellaneousCommand_type::e_videoFreezePicture :
      OnFreezePicture();
      break;

    case H245_MiscellaneousCommand_type::e_videoFastUpdatePicture :
      OnFastUpdatePicture();
      break;

    case H245_MiscellaneousCommand_type::e_videoFastUpdateGOB :
    {
      const H245_MiscellaneousCommand_type_videoFastUpdateGOB & fuGOB = type;
      OnFastUpdateGOB(fuGOB.m_firstGOB, fuGOB.m_numberOfGOBs);
      break;
    }

    case H245_MiscellaneousCommand_type::e_videoFastUpdateMB :
    {
      const H245_MiscellaneousCommand_type_videoFastUpdateMB & fuMB = type;
      OnFastUpdateMB(fuMB.HasOptionalField(H245_MiscellaneousCommand_type_videoFastUpdateMB::e_firstGOB) ? (int)fuMB.m_firstGOB : -1,
                     fuMB.HasOptionalField(H245_MiscellaneousCommand_type_videoFastUpdateMB::e_firstMB)  ? (int)fuMB.m_firstMB  : -1,
                     fuMB.m_numberOfMBs);
      break;
    }

    case H245_MiscellaneousCommand_type::e_lostPartialPicture :
      OnLostPartialPicture();
      break;

    case H245_MiscellaneousCommand_type::e_lostPicture :
      OnLostPicture();
      break;

    case H245_MiscellaneousCommand_type::e_videoTemporalSpatialTradeOff :
    {
      const PASN_Integer & newQuality = type;
      OnVideoTemporalSpatialTradeOffCommand(newQuality);
      break;
    }
  }

  H323Codec::OnMiscellaneousCommand(type);
}


void H323VideoCodec::OnFreezePicture()
{
  PTRACE(3, "Codec\tOnFreezePicture()");
}

void H323VideoCodec::OnFreezeVideo(int disable)
{
  PTRACE(3, "Codec\tOnFreezeVideo(" << disable << ")");
}


void H323VideoCodec::OnFastUpdatePicture()
{
  PTRACE(3, "Codec\tOnFastUpdatePicture()");
}


void H323VideoCodec::OnFastUpdateGOB(unsigned PTRACE_PARAM(firstGOB),
                                     unsigned PTRACE_PARAM(numberOfGOBs))
{
  PTRACE(3, "Codecs\tOnFastUpdateGOB(" << firstGOB << ',' << numberOfGOBs << ')');
}


void H323VideoCodec::OnFastUpdateMB(int PTRACE_PARAM(firstGOB),
                                    int PTRACE_PARAM(firstMB),
                                    unsigned PTRACE_PARAM(numberOfMBs))
{
  PTRACE(3, "Codecs\tOnFastUpdateMB(" << firstGOB << ',' << firstMB << ',' << numberOfMBs << ')');
}


void H323VideoCodec::OnLostPartialPicture()
{
  PTRACE(3, "Codec\tOnLostPartialPicture()");
}


void H323VideoCodec::OnLostPicture()
{
  PTRACE(3, "Codec\tOnLostPicture()");
}


void H323VideoCodec::OnMiscellaneousIndication(const H245_MiscellaneousIndication_type & type)
{
  switch (type.GetTag()) {
    case H245_MiscellaneousIndication_type::e_videoIndicateReadyToActivate :
      OnVideoIndicateReadyToActivate();
      break;

    case H245_MiscellaneousIndication_type::e_videoTemporalSpatialTradeOff :
    {
      const PASN_Integer & newQuality = type;
      OnVideoTemporalSpatialTradeOffIndication(newQuality);
      break;
    }

    case H245_MiscellaneousIndication_type::e_videoNotDecodedMBs :
    {
      const H245_MiscellaneousIndication_type_videoNotDecodedMBs & vndMB = type;
      OnVideoNotDecodedMBs(vndMB.m_firstMB, vndMB.m_numberOfMBs, vndMB.m_temporalReference);
      break;
    }
  }

  H323Codec::OnMiscellaneousIndication(type);
}


void H323VideoCodec::OnVideoIndicateReadyToActivate()
{
  PTRACE(3, "Codec\tOnVideoIndicateReadyToActivate()");
}


void H323VideoCodec::OnVideoTemporalSpatialTradeOffCommand(int PTRACE_PARAM(newQuality))
{
  PTRACE(3, "Codecs\tOnVideoTemporalSpatialTradeOffCommand(" << newQuality << ')');
}


void H323VideoCodec::OnVideoTemporalSpatialTradeOffIndication(int PTRACE_PARAM(newQuality))
{
  PTRACE(3, "Codecs\tOnVideoTemporalSpatialTradeOffIndication(" << newQuality << ')');
}


void H323VideoCodec::OnVideoNotDecodedMBs(unsigned PTRACE_PARAM(firstMB),
                                          unsigned PTRACE_PARAM(numberOfMBs),
                                          unsigned PTRACE_PARAM(temporalReference))
{
  PTRACE(3, "Codecs\tOnVideoNotDecodedMBs(" << firstMB << ',' << numberOfMBs << ',' << temporalReference << ')');
}


void H323VideoCodec::Close()
{
  PWaitAndSignal mutex1(videoHandlerActive);  

  CloseRawDataChannel();
}


BOOL H323VideoCodec::SetMaxBitRate(unsigned bitRate)
{
  PTRACE(1,"Set bitRateHighLimit for video to " << bitRate << " bps");
        
  bitRateHighLimit = bitRate;

  if (0 == bitRateHighLimit) // disable bitrate control
    videoBitRateControlModes &= ~AdaptivePacketDelay;
  return TRUE;
}

BOOL H323VideoCodec::SetTargetFrameTimeMs(unsigned ms)
{
  PTRACE(1,"Set targetFrameTimeMs for video to " << ms << " milliseconds");

  targetFrameTimeMs = ms;

  if (0 == targetFrameTimeMs)
    videoBitRateControlModes &= ~DynamicVideoQuality;
  return TRUE;
}

void H323VideoCodec::SetGeneralCodecOption(const char * /*opt*/,  int /*val*/)
{
}


void H323VideoCodec::SendMiscCommand(unsigned command)
{
  if (logicalChannel != NULL)
    logicalChannel->SendMiscCommand(command);
}


#endif // NO_H323_VIDEO


/////////////////////////////////////////////////////////////////////////////

#ifndef NO_H323_AUDIO_CODECS

H323AudioCodec::H323AudioCodec(const OpalMediaFormat & fmt, Direction dir)
  : H323Codec(fmt, dir)
{
  framesReceived = 0;
  samplesPerFrame = (mediaFormat.GetFrameTime() * mediaFormat.GetTimeUnits()) / 8;
  if (samplesPerFrame == 0)
    samplesPerFrame = 8; // Default for non-frame based codecs.

  if (direction == Encoder)
    codecChannels = mediaFormat.GetEncoderChannels();
  else
    codecChannels = mediaFormat.GetDecoderChannels();

  // Start off in silent mode
  inTalkBurst = FALSE;

  IsRawDataHeld = FALSE;

  // Initialise the adaptive threshold variables.
  SetSilenceDetectionMode(AdaptiveSilenceDetection);
}


H323AudioCodec::~H323AudioCodec()
{
  Close();

  CloseRawDataChannel();
}


BOOL H323AudioCodec::Open(H323Connection & connection)
{
  return connection.OpenAudioChannel(direction == Encoder, samplesPerFrame*2*codecChannels, *this);
}


void H323AudioCodec::Close()
{
  PWaitAndSignal mutex(rawChannelMutex);

  if (rawDataChannel != NULL)
    rawDataChannel->Close();
}


unsigned H323AudioCodec::GetFrameRate() const
{
  return samplesPerFrame;
}


H323AudioCodec::SilenceDetectionMode H323AudioCodec::GetSilenceDetectionMode(
                                BOOL * isInTalkBurst, unsigned * currentThreshold) const
{
  if (isInTalkBurst != NULL)
    *isInTalkBurst = inTalkBurst;

  if (currentThreshold != NULL)
    *currentThreshold = ulaw2linear((BYTE)(levelThreshold ^ 0xff));

  return silenceDetectMode;
}


void H323AudioCodec::SetSilenceDetectionMode(SilenceDetectionMode mode,
                                             unsigned threshold,
                                             unsigned signalDeadband,
                                             unsigned silenceDeadband,
                                             unsigned adaptivePeriod)
{
  silenceDetectMode = mode;

  // The silence deadband is the number of frames of low energy that have to
  // occur before the system stops sending data over the RTP link.
  signalDeadbandFrames = (signalDeadband+samplesPerFrame-1)/samplesPerFrame;
  silenceDeadbandFrames = (silenceDeadband+samplesPerFrame-1)/samplesPerFrame;

  // This is the period over which the adaptive algorithm operates
  adaptiveThresholdFrames = (adaptivePeriod+samplesPerFrame-1)/samplesPerFrame;

  if (mode != AdaptiveSilenceDetection) {
    levelThreshold = threshold;
    return;
  }

  // Initials threshold levels
  levelThreshold = 0;

  // Initialise the adaptive threshold variables.
  signalMinimum = UINT_MAX;
  silenceMaximum = 0;
  signalFramesReceived = 0;
  silenceFramesReceived = 0;

  // Restart in silent mode
  inTalkBurst = FALSE;
}


BOOL H323AudioCodec::DetectSilence(unsigned channels, unsigned sampleRate)
{
  // Can never have silence if NoSilenceDetection
  if (silenceDetectMode == NoSilenceDetection)
	  return FALSE;

  PTRACE(6, "Codec\tSilence detection enabled");

  // Can never have average signal level that high, this indicates that the
  // hardware cannot do silence detection.
  unsigned level = GetAverageSignalLevel();
  if (level == UINT_MAX)
    return FALSE;

  // Convert to a logarithmic scale - use uLaw which is complemented
  level = linear2ulaw(level) ^ 0xff;

  // Now if signal level above threshold we are "talking"
  BOOL haveSignal = level > levelThreshold;

  // If no change ie still talking or still silent, resent frame counter
  if (inTalkBurst == haveSignal)
    framesReceived = 0;
  else {
    framesReceived++;
    // If have had enough consecutive frames talking/silent, swap modes.
    if (framesReceived >= (inTalkBurst ? silenceDeadbandFrames : signalDeadbandFrames)) {
      inTalkBurst = !inTalkBurst;
      PTRACE(4, "Codec\tSilence detection transition: "
             << (inTalkBurst ? "Talk" : "Silent")
             << " level=" << level << " threshold=" << levelThreshold);

      // If we had talk/silence transition restart adaptive threshold measurements
      signalMinimum = UINT_MAX;
      silenceMaximum = 0;
      signalFramesReceived = 0;
      silenceFramesReceived = 0;
    }
  }

  if (silenceDetectMode == FixedSilenceDetection)
    return !inTalkBurst;

  if (levelThreshold == 0) {
    if (level > 1) {
      // Bootstrap condition, use first frame level as silence level
      levelThreshold = level/2;
      if(levelThreshold>64) levelThreshold=64;
      PTRACE(4, "Codec\tSilence detection threshold initialised to: " << levelThreshold);
    }
    return TRUE; // inTalkBurst always FALSE here, so return silent
  }

  // Count the number of silent and signal frames and calculate min/max
  if (haveSignal) {
    if (level < signalMinimum)
      signalMinimum = level;
    signalFramesReceived++;
  }
  else {
    if (level > silenceMaximum)
      silenceMaximum = level;
    silenceFramesReceived++;
  }

  // See if we have had enough frames to look at proportions of silence/signal
  
  unsigned atff = (unsigned)(((unsigned long)adaptiveThresholdFrames) * sampleRate * channels / 8000);
  if ((signalFramesReceived + silenceFramesReceived) > atff) {

    /* Now we have had a period of time to look at some average values we can
       make some adjustments to the threshold. There are four cases:
     */
    if (signalFramesReceived >= atff) {
      /* If every frame was noisy, move threshold up. Don't want to move too
         fast so only go a quarter of the way to minimum signal value over the
         period. This avoids oscillations, and time will continue to make the
         level go up if there really is a lot of background noise.
       */
      int delta = (signalMinimum - levelThreshold)/4;
      if (delta != 0) {
        levelThreshold += delta;
         if(levelThreshold>64) levelThreshold=64;
       PTRACE(4, "Codec\tSilence detection threshold increased to: " << levelThreshold);
      }
    }
    else if (silenceFramesReceived >= atff) {
      /* If every frame was silent, move threshold down. Again do not want to
         move too quickly, but we do want it to move faster down than up, so
         move to halfway to maximum value of the quiet period. As a rule the
         lower the threshold the better as it would improve response time to
         the start of a talk burst.
       */
      unsigned newThreshold = (levelThreshold + silenceMaximum)/2 + 1;
      if (levelThreshold != newThreshold) {
        levelThreshold = newThreshold;
        PTRACE(4, "Codec\tSilence detection threshold decreased to: " << levelThreshold);
      }
    }
    else if (signalFramesReceived > silenceFramesReceived) {
      /* We haven't got a definitive silent or signal period, but if we are
         constantly hovering at the threshold and have more signal than
         silence we should creep up a bit.
       */
      levelThreshold++;
      PTRACE(4, "Codec\tSilence detection threshold incremented to: " << levelThreshold
             << " signal=" << signalFramesReceived << ' ' << signalMinimum
             << " silence=" << silenceFramesReceived << ' ' << silenceMaximum);
    }

    signalMinimum = UINT_MAX;
    silenceMaximum = 0;
    signalFramesReceived = 0;
    silenceFramesReceived = 0;
  }

  return !inTalkBurst;
}


unsigned H323AudioCodec::GetAverageSignalLevel()
{
  return UINT_MAX;
}

BOOL H323AudioCodec::SetRawDataHeld(BOOL hold) { 
	
  PTimedMutex m;
	m.Wait(50);    // wait for 50ms to avoid current locks
	IsRawDataHeld = hold; 
	m.Wait(50); 	// wait for 50ms to avoid any further locks
	return TRUE;
} 

/////////////////////////////////////////////////////////////////////////////

H323FramedAudioCodec::H323FramedAudioCodec(const OpalMediaFormat & fmt, Direction dir)
  : H323AudioCodec(fmt, dir),
    sampleBuffer(samplesPerFrame*codecChannels)
{
  bytesPerFrame = mediaFormat.GetFrameSize();
  sampleRate = 8000;
  currVolCoef = 1.0;
  lastReadTime = time(0);
  readPerSecond = 0;
  agc = 0;
}


void H323FramedAudioCodec::AutoGainControl(const short * pcm, unsigned samplesPerFrame, unsigned codecChannels, unsigned sampleRate, unsigned level, float* currVolCoef)
{
  if(!level) return;

  float max_vol = 17000.0;
  float inc_vol = 0.05*8000.0/sampleRate;

  unsigned samplesCount = samplesPerFrame*codecChannels;

  const short * end = pcm + samplesCount;
  short *buf = (short*)pcm;
  int c_max_vol = 0, c_avg_vol = 0;
  while (pcm != end) 
  {
    if (*pcm < 0) 
    { if(-*pcm > c_max_vol) c_max_vol = -*pcm; c_avg_vol -= *pcm++; }
    else 
    { if( *pcm > c_max_vol) c_max_vol =  *pcm; c_avg_vol += *pcm++; }
  }
  c_avg_vol /= samplesPerFrame;

  float & cvc = *currVolCoef;
  float vc0= cvc;
  
  if(c_avg_vol > level)
  {
    if(c_max_vol*cvc >= 32768) // есть перегрузка
      cvc = 32767.0 / c_max_vol;
    else
    if(c_max_vol*cvc < max_vol) // нужно увеличить усиление
      cvc += inc_vol;
  }
  else // не должен срабатывать, но временно грубая защита от перегрузки:
  {
    if(c_max_vol*cvc >= 32768) // есть перегрузка
      cvc = 32767.0 / c_max_vol;
  }
  PTRACE(9,"AGC\tavg_vol=" << c_avg_vol << " max_vol=" << c_max_vol << " vol_coef=" << vc0 << "->" << cvc << " inc_vol=" << inc_vol);

  float delta0=(cvc-vc0)/samplesCount;

  for(int i=0; i<samplesCount; i++) 
  {
    int v = buf[i];
    v*=vc0;
    if(v > 32767) buf[i]=32767;
    else if(v < -32768) buf[i]=-32768;
    else buf[i] = (short)v;
    vc0+=delta0;
  }
}


BOOL H323FramedAudioCodec::Read(BYTE * buffer, unsigned & length, RTP_DataFrame &)
{
  PWaitAndSignal mutex(rawChannelMutex);

  if (direction != Encoder) {
    PTRACE(1, "Codec\tAttempt to decode from encoder");
    return FALSE;
  }

  if (IsRawDataHeld) {	 // If connection is onHold
    PProcess::Sleep(5);  // Sleep to avoid CPU overload. <--Should be a better method but it works :)
    length = 0;
    return TRUE;
  }

  PINDEX numBytes = samplesPerFrame*2*codecChannels;
  PINDEX count;
  if (!ReadRaw(sampleBuffer.GetPointer(samplesPerFrame*codecChannels), numBytes, count))
    return FALSE;

//  PTRACE(6,"Audio\tReadRaw " << numBytes);

#ifdef H323_AEC
    if (aec != NULL) {
       PTRACE(6,"AEC\tSend " << numBytes);
       aec->Send((BYTE*)sampleBuffer.GetPointer(samplesPerFrame*codecChannels),(unsigned &)numBytes);
    }
#endif

  if (IsRawDataHeld) {
    length = 0;
    return TRUE;
  }

  if (count != numBytes) {
    PTRACE(1, "Codec\tRead truncated frame of raw data. Wanted " << numBytes << " and got "<<count);
    return FALSE;
  }

  sampleRate = GetMediaFormat().GetTimeUnits() * 1000;
  PTRACE(9,"SR=" << sampleRate);

  if (DetectSilence(sampleRate, codecChannels)) {
    length = 0;
    return TRUE;
  }

  // Default length is the frame size
  length = bytesPerFrame;
//  PTRACE(6,"Read\tlength " << length);

  if(agc)
  AutoGainControl((const short*)sampleBuffer.GetPointer(), samplesPerFrame, codecChannels, sampleRate, agc, &currVolCoef);

  return EncodeFrame(buffer, length);
}


BOOL H323FramedAudioCodec::Write(const BYTE * buffer,
                                 unsigned length,
                                 const RTP_DataFrame & /*rtpFrame*/,
                                 unsigned & written)
{
  PWaitAndSignal mutex(rawChannelMutex);

  if (direction != Decoder) {
    PTRACE(1, "Codec\tAttempt to encode from decoder");
    return FALSE;
  }

  // If length is zero then it indicates silence, do nothing.
  written = 0;

  unsigned bytesDecoded = samplesPerFrame*2*codecChannels;
  PTRACE(9,"H323FramedAudioCodec\tWrite: length " << length << ", codecChannels " << codecChannels << ", samplesPerFrame " << samplesPerFrame << ", bytesDecoded " << bytesDecoded);

  if (length != 0) {
    if (length > bytesPerFrame)
      length = bytesPerFrame;
    written = bytesPerFrame;

    // Decode the data
    if (!DecodeFrame(buffer, length, written, bytesDecoded)) {
      written = length;
      length = 0;
    }
  }

  // was memset(sampleBuffer.GetPointer(samplesPerFrame), 0, bytesDecoded);
  if (length == 0)
    DecodeSilenceFrame(sampleBuffer.GetPointer(bytesDecoded), bytesDecoded);

  // Write as 16bit PCM to sound channel
  if (IsRawDataHeld) {		// If Connection om Hold 
	PProcess::Sleep(5);	// Sleep to avoid CPU Overload <--- Must be a better way but need it to work.
	return TRUE;
  } else {
#ifdef H323_AEC
	  if (aec != NULL) {
		  PTRACE(6,"AEC\tReceive " << bytesDecoded);
		  aec->Receive((BYTE *)sampleBuffer.GetPointer(), bytesDecoded);
	  }
#endif
      if (!WriteRaw(sampleBuffer.GetPointer(), bytesDecoded)) 
		  return FALSE;
  }
	  return TRUE;


}


unsigned H323FramedAudioCodec::GetAverageSignalLevel()
{
  // Calculate the average signal level of this frame
  int sum = 0;

  const short * pcm = sampleBuffer;
  const short * end = pcm + samplesPerFrame;
  while (pcm != end) {
    if (*pcm < 0)
      sum -= *pcm++;
    else
      sum += *pcm++;
  }

  return sum/samplesPerFrame;
}


BOOL H323FramedAudioCodec::DecodeFrame(const BYTE * buffer,
                                       unsigned length,
                                       unsigned & written,
                                       unsigned & /*decodedBytes*/)
{
  return DecodeFrame(buffer, length, written);
}


BOOL H323FramedAudioCodec::DecodeFrame(const BYTE * /*buffer*/,
                                       unsigned /*length*/,
                                       unsigned & /*written*/)
{
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
}

#ifdef H323_AEC 
void H323FramedAudioCodec::AttachAEC(PAec * _aec)
{
  aec = _aec;
}
#endif

void H323FramedAudioCodec::EnableAGC(int _agc)
{
  agc = _agc;
}

/////////////////////////////////////////////////////////////////////////////

H323StreamedAudioCodec::H323StreamedAudioCodec(const OpalMediaFormat & fmt,
                                               Direction dir,
                                               unsigned samples,
                                               unsigned bits)
  : H323FramedAudioCodec(fmt, dir)
{
  samplesPerFrame = samples;
  bytesPerFrame = (samples*bits+7)/8;
  bitsPerSample = bits;
}


BOOL H323StreamedAudioCodec::EncodeFrame(BYTE * buffer, unsigned &)
{
  PINDEX i;
  unsigned short position = 0;
  BYTE encoded;
  switch (bitsPerSample) {
    case 8 :
      for (i = 0; i < (PINDEX)samplesPerFrame; i++)
        *buffer++ = (BYTE)Encode(sampleBuffer[i]);
      break;
    case 5 : // g.726-40 payload encoding....
      for (i = 0; i < (PINDEX)samplesPerFrame;i++)
      {
        // based on a 40 bits encoding, we have 8 words of 5 bits each
        encoded = (BYTE)Encode(sampleBuffer[i]);
        switch(position)
        {
          case 0: // 0 bits overflow
            *buffer = encoded;
            position++;
            break;
          case 1: // 2 bits overflow
            *buffer++ |= (encoded << 5);
            *buffer = (BYTE)(encoded >> 3);
            position++;
            break;
          case 2: 
            *buffer |= (encoded << 2);
            position++;
            break;
          case 3: // one bit left for word 4
            *buffer++ |= (encoded << 7);
            *buffer = (BYTE)(encoded >> 1);
            position++;
            break;
          case 4:
            *buffer++ |= (encoded << 4);
            *buffer = (BYTE)(encoded >> 4);
            position++;
            break;
          case 5:
            *buffer |= (encoded << 1);
            position++;
            break;
          case 6: //two bits left for the new encoded word
            *buffer++ |= (encoded << 6);
            *buffer =  (BYTE)(encoded >> 2);
            position++;
            break;
          case 7: // now five bits left for the last word
            *buffer++ |= (encoded << 3);
            position = 0;
            break;
        }
      }
      break;

    case 4 :
      for (i = 0; i < (PINDEX)samplesPerFrame; i++) {
        if ((i&1) == 0)
          *buffer = (BYTE)Encode(sampleBuffer[i]);
        else
          *buffer++ |= (BYTE)(Encode(sampleBuffer[i]) << 4);
      }
      break;

    case 3 :
      for (i = 0;i < (PINDEX)samplesPerFrame;i++)
      {
        encoded = (BYTE)Encode(sampleBuffer[i]);
        switch(position)
        {
          case 0: // 0 bits overflow
            *buffer = encoded;
            position++;
            break;
          case 1: // 2 bits overflow
            *buffer |= (encoded << 3);
            position++;
            break;
          case 2: 
            *buffer++ |= (encoded << 6);
            *buffer = (BYTE)(encoded >> 2);
            position++;
            break;
          case 3: // one bit left for word 4
            *buffer |= (encoded << 1);
            position++;
            break;
          case 4:
            *buffer |= (encoded << 4);
            position++;
            break;
          case 5:
            *buffer++ |= (encoded << 7);
            *buffer = (BYTE)(encoded >> 1);
            position++;
            break;
          case 6: //two bits left for the new encoded word
            *buffer |= (encoded << 2);
            position++;
            break;
          case 7: // now five bits left for the last word
            *buffer++ |= (encoded << 5);
            position = 0;
            break;
        }
      }
      break;

    case 2:
      for (i = 0; i < (PINDEX)samplesPerFrame; i++) 
      {
        switch(position)
        {
          case 0:
            *buffer = (BYTE)Encode(sampleBuffer[i]);
            position++;
            break;
          case 1:
            *buffer |= (BYTE)(Encode(sampleBuffer[i]) << 2);
            position++;
            break;
          case 2:
            *buffer |= (BYTE)(Encode(sampleBuffer[i]) << 4);
            position++;
            break;
          case 3:
            *buffer++ |= (BYTE)(Encode(sampleBuffer[i]) << 6);
            position = 0;
            break;
        }
      }
      break;

    default :
      PAssertAlways("Unsupported bit size");
      return FALSE;
  }
  
  return TRUE;
}


BOOL H323StreamedAudioCodec::DecodeFrame(const BYTE * buffer,
                                         unsigned length,
                                         unsigned & written,
                                         unsigned & decodedBytes)
{
  unsigned i;
  
  short * sampleBufferPtr = sampleBuffer.GetPointer(samplesPerFrame);
  short * out = sampleBufferPtr;
  unsigned short position = 0;
  unsigned remaining = 0;
  
  switch (bitsPerSample) {
    case 8 :
      for (i = 0; i < length; i++)
        *out++ = Decode(*buffer++);
      break;

    // those case are for ADPCM G.726
    case 5 :
      for (i = 0; i < length; i++) {
        switch(position)
        {
          case 0:
            *out++ = Decode(*buffer & 31);
            remaining = *buffer >> 5; // get the three remaining bytes for the next word
            buffer++;
            position++;
            break;
          case 1: // we can decode more than one word in second buffer
            *out++ = Decode (((*buffer&3) << 3) | remaining);
            *out++ = Decode( (*buffer >> 2) & 31);
            remaining = *buffer >> 7;
            buffer++;
            position++;
            break;
          case 2:
            *out++ = Decode( remaining | ((*buffer&15) << 1));
            remaining = *buffer >> 4;
            buffer++;
            position++;
            break;
          case 3:
            *out++ = Decode( remaining | ((*buffer&1) << 4));
            *out++ = Decode( (*buffer >> 1) & 31);
            remaining = *buffer >> 6;
            buffer++;
            position++;
            break;
          case 4 :
            *out++ = Decode( remaining | ((*buffer&7) << 2));
            *out++ = Decode(*buffer >> 3);
            buffer++;
            position = 0;
            break;
        }
      }
      break;

    case 4 :
      for (i = 0; i < length; i++) {
        *out++ = Decode(*buffer & 15);
        *out++ = Decode(*buffer >> 4);
        buffer++;
      }
      break;

    case 3:
      for (i = 0; i < length; i++) {
        switch(position)
        {
        case 0:
          *out++ = Decode(*buffer & 7);
          *out++ = Decode((*buffer>>3)&7);
          remaining = *buffer >> 6;
          buffer++;
          position++;
          break;
        case 1:
          *out++ = Decode(remaining | ((*buffer&1) << 2));
          *out++ = Decode((*buffer >> 1) & 7);
          *out++ = Decode((*buffer >> 4)&7);
          remaining = *buffer >> 7;
          buffer++;
          position++;
          break;
        case 2:
          *out++ = Decode(remaining | ((*buffer&3) << 1));
          *out++ = Decode((*buffer >> 2) & 7);
          *out++ = Decode((*buffer >> 5) & 7);
          buffer++;
          position = 0;
          break;
        }
      }
      break;

    case 2:
      for (i = 0; i < length; i++) 
      {
        *out++ = Decode(*buffer & 3);
        *out++ = Decode((*buffer >> 2) & 3);
        *out++ = Decode((*buffer >> 4) & 3);
        *out++ = Decode((*buffer >> 6) & 3);
        buffer++;
      
      }
      break;

    default :
      PAssertAlways("Unsupported bit size");
      return FALSE;
  }

  written = length;
  decodedBytes = (out - sampleBufferPtr)*2;
  
  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////

H323_ALawCodec::H323_ALawCodec(Direction dir,
                               BOOL at56kbps,
                               unsigned frameSize)
  : H323StreamedAudioCodec(OpalG711ALaw, dir, frameSize, 8)
{
  sevenBit = at56kbps;

  PTRACE(3, "Codec\tG711 ALaw " << (dir == Encoder ? "en" : "de")
         << "coder created for at "
         << (sevenBit ? "56k" : "64k") << ", " << frameSize << " samples");
}



int H323_ALawCodec::EncodeSample(short sample)
{
  return linear2alaw(sample);
}


short H323_ALawCodec::DecodeSample(int sample)
{
  return (short)alaw2linear((unsigned char)sample);
}


/////////////////////////////////////////////////////////////////////////////

H323_muLawCodec::H323_muLawCodec(Direction dir,
                                 BOOL at56kbps,
                                 unsigned frameSize)
  : H323StreamedAudioCodec(OpalG711uLaw, dir, frameSize, 8)
{
  sevenBit = at56kbps;

  PTRACE(3, "Codec\tG711 uLaw " << (dir == Encoder ? "en" : "de")
         << "coder created for at "
         << (sevenBit ? "56k" : "64k") << ", frame of " << frameSize << " samples");
}


int H323_muLawCodec::EncodeSample(short sample)
{
  return linear2ulaw(sample);
}


short H323_muLawCodec::DecodeSample(int sample)
{
  return (short)ulaw2linear((unsigned char)sample);
}


/////////////////////////////////////////////////////////////////////////////

#endif // NO_H323_AUDIO_CODECS
