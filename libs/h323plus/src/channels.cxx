/*
 * channels.cxx
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
 * $Log: channels.cxx,v $
 * Revision 1.2  2008/02/06 02:52:59  shorne
 * Added support for Standards based NAT Traversal
 *
 * Revision 1.1  2007/08/06 20:51:03  shorne
 * First commit of h323plus
 *
 * Revision 1.147.2.1  2007/05/23 06:58:02  shorne
 * Nat Support for EP's nested behind same NAT
 *
 * Revision 1.147  2006/03/21 10:42:25  csoutheren
 * Disable automatic payload type changes for video codecs
 *
 * Revision 1.146  2005/06/21 06:44:34  csoutheren
 * Added assert for capabilities that do not have codecs
 *
 * Revision 1.145  2005/01/03 06:25:54  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.144  2004/07/27 05:28:45  csoutheren
 * Added ability to set priority of channel threads
 *
 * Revision 1.143  2004/07/17 03:10:06  rjongbloed
 * Fixed possible endless loop in RTP receive thread, thanks Gustavo García Bernardo
 *
 * Revision 1.142  2004/07/14 01:03:20  csoutheren
 * Ensure payload type is set before codec Read function is called
 *
 * Revision 1.141  2004/07/07 03:53:45  csoutheren
 * Fixed non-faststart comptibility problem with Cisco gateways. Apparently, they do not return a session ID in the OLCack in non-fastStart mode but they do in fastStart mode (who'da - guessed that !)
 *
 * Revision 1.140  2004/07/03 06:51:37  rjongbloed
 * Added PTRACE_PARAM() macro to fix warnings on parameters used in PTRACE
 *  macros only.
 *
 * Revision 1.139  2004/07/03 05:47:54  rjongbloed
 * Added virtual function for determining RTP payload type used in an H.323 channel,
 *    also some added bullet proofing for exception conditions, thanks Guilhem Tardy
 *
 * Revision 1.138  2004/05/23 12:03:04  rjongbloed
 * Fix problem with crashing on debug if change log level in mid call, thanks Ben Lear
 *
 * Revision 1.137  2004/04/03 08:28:06  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.136  2003/02/10 05:36:13  robertj
 * Fixed returning mediaControlChannel address in preference to mediaChannel
 *   address as Cisco's just feed your own address back at you.
 *
 * Revision 1.135  2002/12/18 11:20:49  craigs
 * Fixed problem with T.38 channels SEGVing thanks to Vyacheslav Frolov
 *
 * Revision 1.134  2002/12/17 08:48:09  robertj
 * Set silence suppression mode earlier in codec life so gets correct
 *   value for silenceSuppression in fast start OLC's.
 *
 * Revision 1.133  2002/12/16 08:20:04  robertj
 * Fixed problem where a spurious RTP packet full of zeros could be sent
 *   at the beginning of the transmission, thanks Bruce Fitzsimons
 *
 * Revision 1.132  2002/11/26 02:59:25  robertj
 * Added logging to help find logical channel thread stop failures.
 *
 * Revision 1.131  2002/10/31 00:37:47  robertj
 * Enhanced jitter buffer system so operates dynamically between minimum and
 *   maximum values. Altered API to assure app writers note the change!
 *
 * Revision 1.130  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.129  2002/06/28 03:34:28  robertj
 * Fixed issues with address translation on gatekeeper RAS channel.
 *
 * Revision 1.128  2002/06/25 08:30:12  robertj
 * Changes to differentiate between stright G.723.1 and G.723.1 Annex A using
 *   the OLC dataType silenceSuppression field so does not send SID frames
 *   to receiver codecs that do not understand them.
 *
 * Revision 1.127  2002/06/24 00:07:31  robertj
 * Fixed bandwidth usage being exactly opposite (adding when it should
 *   be subtracting), thanks Saswat Praharaj.
 *
 * Revision 1.126  2002/05/23 04:53:57  robertj
 * Added function to remove a filter from logical channel.
 *
 * Revision 1.125  2002/05/10 05:47:15  robertj
 * Added session ID to the data logical channel class.
 *
 * Revision 1.124  2002/05/07 23:49:11  robertj
 * Fixed incorrect setting of session ID in data channel OLC, caused an
 *   incorrect optional field to be included, thanks Ulrich Findeisen.
 *
 * Revision 1.123  2002/05/03 00:07:24  robertj
 * Fixed missing setting of isRunning flag in external RTP channels.
 *
 * Revision 1.122  2002/05/02 07:56:27  robertj
 * Added automatic clearing of call if no media (RTP data) is transferred in a
 *   configurable (default 5 minutes) amount of time.
 *
 * Revision 1.121  2002/05/02 06:28:53  robertj
 * Fixed problem with external RTP channels not fast starting.
 *
 * Revision 1.120  2002/04/17 05:56:05  robertj
 * Added trace output of H323Channel::Direction enum.
 *
 * Revision 1.119  2002/02/25 08:42:26  robertj
 * Fixed comments on the real time requirements of the codec.
 *
 * Revision 1.118  2002/02/19 06:15:20  robertj
 * Allowed for RTP filter functions to force output of packet, or prevent it
 *   from being sent overriding the n frames per packet algorithm.
 *
 * Revision 1.117  2002/02/09 04:39:05  robertj
 * Changes to allow T.38 logical channels to use single transport which is
 *   now owned by the OpalT38Protocol object instead of H323Channel.
 *
 * Revision 1.116  2002/02/05 08:13:02  robertj
 * Added ability to not have addresses when external RTP channel created.
 *
 * Revision 1.115  2002/02/04 06:04:19  robertj
 * Fixed correct exit on terminating transmit channel, thanks Norwood Systems.
 *
 * Revision 1.114  2002/01/27 10:49:51  rogerh
 * Catch a division by zero case in a PTRACE()
 *
 * Revision 1.113  2002/01/24 03:33:07  robertj
 * Fixed payload type being incorrect for audio after sending RFC2833 packet.
 *
 * Revision 1.112  2002/01/22 22:48:25  robertj
 * Fixed RFC2833 support (transmitter) requiring large rewrite
 *
 * Revision 1.111  2002/01/22 07:08:26  robertj
 * Added IllegalPayloadType enum as need marker for none set
 *   and MaxPayloadType is a legal value.
 *
 * Revision 1.110  2002/01/22 06:05:03  robertj
 * Added ability for RTP payload type to be overridden at capability level.
 *
 * Revision 1.109  2002/01/17 07:05:03  robertj
 * Added support for RFC2833 embedded DTMF in the RTP stream.
 *
 * Revision 1.108  2002/01/17 00:10:37  robertj
 * Fixed double copy of rtpPayloadType in RTP channel, caused much confusion.
 *
 * Revision 1.107  2002/01/14 05:18:44  robertj
 * Fixed typo on external RTP channel constructor.
 *
 * Revision 1.106  2002/01/10 05:13:54  robertj
 * Added support for external RTP stacks, thanks NuMind Software Systems.
 *
 * Revision 1.105  2002/01/09 06:05:55  robertj
 * Rearranged transmitter timestamp calculation to allow for a codec that has
 *   variable number of timestamp units per call to Read().
 *
 * Revision 1.104  2001/12/22 01:50:47  robertj
 * Fixed bug in data channel (T.38) negotiations, using wrong PDU subclass.
 * Fixed using correct port number in data channel (T.38) negotiations.
 * Improved trace logging.
 *
 * Revision 1.103  2001/11/28 00:09:14  dereks
 * Additional information in PTRACE output.
 *
 * Revision 1.102  2001/11/09 05:39:54  craigs
 * Added initial T.38 support thanks to Adam Lazur
 *
 * Revision 1.101  2001/10/24 00:55:49  robertj
 * Made cosmetic changes to H.245 miscellaneous command function.
 *
 * Revision 1.100  2001/10/23 02:17:16  dereks
 * Initial release of cu30 video codec.
 *
 * Revision 1.99  2001/09/13 08:20:27  robertj
 * Fixed broken back out of rev 1.95, thanks Santiago Garcia Mantinan
 *
 * Revision 1.98  2001/09/11 00:21:23  robertj
 * Fixed missing stack sizes in endpoint for cleaner thread and jitter thread.
 *
 * Revision 1.97  2001/08/28 09:28:28  robertj
 * Backed out change in revision 1.95, not compatible with G.711
 *
 * Revision 1.96  2001/08/16 06:34:42  robertj
 * Plugged memory leak if using trace level 5.
 *
 * Revision 1.95  2001/08/10 01:34:41  robertj
 * Fixed problem with incorrect timestamp if codec returns more than one
 *    frame in read, thanks Lee Kirchhoff.
 *
 * Revision 1.94  2001/08/06 05:36:00  robertj
 * Fixed GNU warnings.
 *
 * Revision 1.93  2001/08/06 03:08:56  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.92  2001/07/24 02:26:44  robertj
 * Added start for handling reverse channels.
 *
 * Revision 1.91  2001/07/17 04:44:31  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 * Revision 1.90  2001/07/12 07:28:41  yurik
 * WinCE fix: Sleep(0) in Main to get system chance to digest
 *
 * Revision 1.89  2001/06/15 07:20:35  robertj
 * Moved OnClosedLogicalChannel() to be after channels threads halted.
 *
 * Revision 1.88  2001/06/02 01:35:32  robertj
 * Added thread names.
 *
 * Revision 1.87  2001/05/31 06:29:48  robertj
 * Changed trace of RTP mismatch so only displays for first n packets then
 *   does not dump messages any more. Was exactly the opposite.
 *
 * Revision 1.86  2001/04/20 02:32:07  robertj
 * Improved logging of bandwith, used more intuitive units.
 *
 * Revision 1.85  2001/04/02 04:12:53  robertj
 * Fixed trace output from packet transmit timing.
 *
 * Revision 1.84  2001/03/23 05:38:30  robertj
 * Added PTRACE_IF to output trace if a conditional is TRUE.
 *
 * Revision 1.83  2001/02/09 05:13:55  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.82  2001/02/07 05:04:45  robertj
 * Improved codec read analysis debug output.
 *
 * Revision 1.81  2001/02/06 07:40:46  robertj
 * Added debugging for timing of codec read.
 *
 * Revision 1.80  2001/01/25 07:27:16  robertj
 * Major changes to add more flexible OpalMediaFormat class to normalise
 *   all information about media types, especially codecs.
 *
 * Revision 1.79  2000/12/20 00:50:42  robertj
 * Fixed MSVC compatibility issues (No trace).
 *
 * Revision 1.78  2000/12/19 22:33:44  dereks
 * Adjust so that the video channel is used for reading/writing raw video
 * data, which better modularizes the video codec.
 *
 * Revision 1.77  2000/12/17 22:45:36  robertj
 * Set media stream threads to highest unprivileged priority.
 *
 * Revision 1.76  2000/11/24 10:52:50  robertj
 * Modified the ReadFrame/WriteFrame functions to allow for variable length codecs.
 * Added support for G.729 annex B packetisation scheme in RTP.
 * Fixed bug in fast started G.711 codec not working in one direction.
 *
 * Revision 1.75  2000/10/24 00:00:09  robertj
 * Improved memory hogging hash function for logical channels.
 *
 * Revision 1.74  2000/10/19 04:05:01  robertj
 * Added compare function for logical channel numbers, thanks Yuriy Ershov.
 *
 * Revision 1.73  2000/09/23 06:54:44  robertj
 * Prevented call of OnClose call back if channel was never opened.
 *
 * Revision 1.72  2000/09/22 01:35:49  robertj
 * Added support for handling LID's that only do symmetric codecs.
 *
 * Revision 1.71  2000/09/22 00:32:33  craigs
 * Added extra logging
 * Fixed problems with no fastConnect with tunelling
 *
 * Revision 1.70  2000/09/20 01:50:21  craigs
 * Added ability to set jitter buffer on a per-connection basis
 *
 * Revision 1.69  2000/09/14 23:03:45  robertj
 * Increased timeout on asserting because of driver lockup
 *
 * Revision 1.68  2000/08/31 08:15:40  robertj
 * Added support for dynamic RTP payload types in H.245 OpenLogicalChannel negotiations.
 *
 * Revision 1.67  2000/08/30 06:33:01  craigs
 * Add fix to ignore small runs of consectuive mismatched payload types
 *
 * Revision 1.66  2000/08/25 01:10:28  robertj
 * Added assert if various thrads ever fail to terminate.
 *
 * Revision 1.65  2000/08/21 02:50:28  robertj
 * Fixed race condition if close call just as slow start media channels are opening.
 *
 * Revision 1.64  2000/07/14 14:04:49  robertj
 * Clarified a debug message.
 *
 * Revision 1.63  2000/07/14 12:47:36  robertj
 * Added clarification to some logging messags.
 *
 * Revision 1.62  2000/07/13 16:05:47  robertj
 * Removed time critical priority as it can totally slag a Win98 system.
 * Fixed trace message displaying mismatched codecs in RTP packet around the wrong way.
 *
 * Revision 1.61  2000/07/11 11:15:52  robertj
 * Fixed bug when terminating RTP receiver and not also terminating transmitter.
 *
 * Revision 1.60  2000/06/23 02:04:01  robertj
 * Increased the priority of the media channels, only relevent for Win32 at this time.
 *
 * Revision 1.59  2000/06/15 01:46:15  robertj
 * Added channel pause (aka mute) functions.
 *
 * Revision 1.58  2000/05/18 12:10:50  robertj
 * Removed all Sleep() calls in codec as timing innacuracies make it unusable. All
 *    codec implementations must thus have timing built into them, usually using I/O.
 *
 * Revision 1.57  2000/05/11 23:54:25  craigs
 * Fixed the Windows fix with another Linux fix. But it worked OK on the Alpha!
 *
 * Revision 1.56  2000/05/11 09:56:46  robertj
 * Win32 compatibility and addition of some extra debugging on codec timing.
 *
 * Revision 1.55  2000/05/11 02:27:18  robertj
 * Added "fail safe" timer sleep on codec writes when on output of jitter buffer.
 *
 * Revision 1.54  2000/05/04 11:52:34  robertj
 * Added Packets Too Late statistics, requiring major rearrangement of jitter
 *    buffer code, not also changes semantics of codec Write() function slightly.
 *
 * Revision 1.53  2000/05/02 04:32:26  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.52  2000/05/01 01:01:49  robertj
 * Added flag for what to do with out of orer packets (use if jitter, don't if not).
 *
 * Revision 1.51  2000/04/28 13:01:44  robertj
 * Fixed problem with adjusting tx/rx frame counts in capabilities during fast start.
 *
 * Revision 1.50  2000/04/10 19:45:49  robertj
 * Changed RTP data receive tp be more forgiving, will process packet even if payload type is wrong.
 *
 * Revision 1.49  2000/03/31 20:04:28  robertj
 * Fixed log message for start/end of transmitted talk burst.
 *
 * Revision 1.48  2000/03/29 04:36:38  robertj
 * Improved some trace logging messages.
 *
 * Revision 1.47  2000/03/22 01:31:36  robertj
 * Fixed transmitter loop so codec can return multiple frames (crash in G.711 mode)
 *
 * Revision 1.46  2000/03/21 03:58:00  robertj
 * Fixed stuffed up RTP transmit loop after previous change.
 *
 * Revision 1.45  2000/03/21 03:06:49  robertj
 * Changes to make RTP TX of exact numbers of frames in some codecs.
 *
 * Revision 1.44  2000/03/20 20:59:28  robertj
 * Fixed possible buffer overrun problem in RTP_DataFrames
 *
 * Revision 1.43  2000/02/24 00:34:25  robertj
 * Fixed possible endless loop on channel abort, thanks Yura Ershov
 *
 * Revision 1.42  2000/02/04 05:11:19  craigs
 * Updated for new Makefiles and for new video transmission code
 *
 * Revision 1.41  2000/01/13 04:03:45  robertj
 * Added video transmission
 *
 * Revision 1.40  2000/01/08 06:52:10  robertj
 * Removed invalid assert
 *
 * Revision 1.39  1999/12/23 23:02:35  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 * Revision 1.38  1999/11/22 01:37:31  robertj
 * Fixed channel closure so OnClosedLogicalChannel() only called if channel was actually started.
 *
 * Revision 1.37  1999/11/20 05:35:26  robertj
 * Extra debugging
 *
 * Revision 1.36  1999/11/20 00:53:47  robertj
 * Fixed ability to have variable sized frames in single RTP packet under G.723.1
 *
 * Revision 1.35  1999/11/19 09:06:25  robertj
 * Changed to close down logical channel if get a transmit codec error.
 *
 * Revision 1.34  1999/11/11 23:28:46  robertj
 * Added first cut silence detection algorithm.
 *
 * Revision 1.33  1999/11/06 11:01:37  robertj
 * Extra debugging.
 *
 * Revision 1.32  1999/11/06 05:37:44  robertj
 * Complete rewrite of termination of connection to avoid numerous race conditions.
 *
 * Revision 1.31  1999/11/01 00:47:46  robertj
 * Added close of logical channel on write error
 *
 * Revision 1.30  1999/10/30 12:38:24  robertj
 * Added more tracing of channel threads.
 *
 * Revision 1.29  1999/10/08 09:59:03  robertj
 * Rewrite of capability for sending multiple audio frames
 *
 * Revision 1.28  1999/10/08 04:58:37  robertj
 * Added capability for sending multiple audio frames in single RTP packet
 *
 * Revision 1.27  1999/09/23 07:25:12  robertj
 * Added open audio and video function to connection and started multi-frame codec send functionality.
 *
 * Revision 1.26  1999/09/21 14:09:02  robertj
 * Removed warnings when no tracing enabled.
 *
 * Revision 1.25  1999/09/18 13:24:38  craigs
 * Added ability to disable jitter buffer
 * Added ability to access entire RTP packet in codec Write
 *
 * Revision 1.24  1999/09/08 04:05:48  robertj
 * Added support for video capabilities & codec, still needs the actual codec itself!
 *
 * Revision 1.23  1999/09/03 02:17:50  robertj
 * Added more debugging
 *
 * Revision 1.22  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 * Revision 1.21  1999/07/16 15:02:51  robertj
 * Changed jitter buffer to throw away old packets if jitter exceeded.
 *
 * Revision 1.20  1999/07/16 00:51:03  robertj
 * Some more debugging of fast start.
 *
 * Revision 1.19  1999/07/15 14:45:35  robertj
 * Added propagation of codec open error to shut down logical channel.
 * Fixed control channel start up bug introduced with tunnelling.
 *
 * Revision 1.18  1999/07/15 09:04:31  robertj
 * Fixed some fast start bugs
 *
 * Revision 1.17  1999/07/14 06:04:04  robertj
 * Fixed setting of channel number in fast start.
 *
 * Revision 1.16  1999/07/13 09:53:24  robertj
 * Fixed some problems with jitter buffer and added more debugging.
 *
 * Revision 1.15  1999/07/13 02:50:58  craigs
 * Changed semantics of SetPlayDevice/SetRecordDevice, only descendent
 *    endpoint assumes PSoundChannel devices for audio codec.
 *
 * Revision 1.14  1999/07/10 03:01:48  robertj
 * Removed debugging.
 *
 * Revision 1.13  1999/07/09 06:09:49  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.12  1999/06/25 14:19:40  robertj
 * Fixed termination race condition in logical channel tear down.
 *
 * Revision 1.11  1999/06/24 13:32:45  robertj
 * Fixed ability to change sound device on codec and fixed NM3 G.711 compatibility
 *
 * Revision 1.10  1999/06/22 13:49:40  robertj
 * Added GSM support and further RTP protocol enhancements.
 *
 * Revision 1.9  1999/06/14 05:15:55  robertj
 * Changes for using RTP sessions correctly in H323 Logical Channel context
 *
 * Revision 1.8  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.7  1999/06/09 06:18:00  robertj
 * GCC compatibiltiy.
 *
 * Revision 1.6  1999/06/09 05:26:19  robertj
 * Major restructuring of classes.
 *
 * Revision 1.5  1999/06/07 00:54:30  robertj
 * Displayed error on SetOption for buffer size
 *
 * Revision 1.4  1999/06/06 06:06:36  robertj
 * Changes for new ASN compiler and v2 protocol ASN files.
 *
 * Revision 1.3  1999/04/26 06:14:46  craigs
 * Initial implementation for RTP decoding and lots of stuff
 * As a whole, these changes are called "First Noise"
 *
 * Revision 1.2  1999/02/25 03:26:02  robertj
 * BeOS compatibility
 *
 * Revision 1.1  1999/01/16 01:31:09  robertj
 * Initial revision
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "channels.h"
#endif

#include "channels.h"

#include "h323pdu.h"
#include "h323ep.h"
#include "h323rtp.h"



#define	MAX_PAYLOAD_TYPE_MISMATCHES 8
#define RTP_TRACE_DISPLAY_RATE 16000 // 2 seconds


class H323LogicalChannelThread : public PThread
{
    PCLASSINFO(H323LogicalChannelThread, PThread)
  public:
    H323LogicalChannelThread(H323EndPoint & endpoint, H323Channel & channel, BOOL rx);
    void Main();
  private:
    H323Channel & channel;
    BOOL receiver;
};


#define new PNEW


#if PTRACING

ostream & operator<<(ostream & out, H323Channel::Directions dir)
{
  static const char * const DirNames[H323Channel::NumDirections] = {
    "IsBidirectional", "IsTransmitter", "IsReceiver"
  };

  if (dir < H323Channel::NumDirections && DirNames[dir] != NULL)
    out << DirNames[dir];
  else
    out << "Direction<" << (unsigned)dir << '>';

  return out;
}

#endif


/////////////////////////////////////////////////////////////////////////////

H323LogicalChannelThread::H323LogicalChannelThread(H323EndPoint & endpoint,
                                                   H323Channel & c,
                                                   BOOL rx)
  : PThread(endpoint.GetChannelThreadStackSize(),
            NoAutoDeleteThread,
            endpoint.GetChannelThreadPriority(),
            rx ? "LogChanRx:%0x" : "LogChanTx:%0x"),
    channel(c)
{
  PTRACE(4, "LogChan\tStarting logical channel thread " << this);
  receiver = rx;
  Resume();
}


void H323LogicalChannelThread::Main()
{
  PTRACE(4, "LogChan\tStarted logical channel thread " << this);
  if (receiver)
    channel.Receive();
  else
    channel.Transmit();

#ifdef _WIN32_WCE
	Sleep(0); // Relinquish control to other thread
#endif
}


/////////////////////////////////////////////////////////////////////////////

H323ChannelNumber::H323ChannelNumber(unsigned num, BOOL fromRem)
{
  PAssert(num < 0x10000, PInvalidParameter);
  number = num;
  fromRemote = fromRem;
}


PObject * H323ChannelNumber::Clone() const
{
  return new H323ChannelNumber(number, fromRemote);
}


PINDEX H323ChannelNumber::HashFunction() const
{
  PINDEX hash = (number%17) << 1;
  if (fromRemote)
    hash++;
  return hash;
}


void H323ChannelNumber::PrintOn(ostream & strm) const
{
  strm << (fromRemote ? 'R' : 'T') << '-' << number;
}


PObject::Comparison H323ChannelNumber::Compare(const PObject & obj) const
{
#ifndef PASN_LEANANDMEAN
  PAssert(PIsDescendant(this, H323ChannelNumber), PInvalidCast);
#endif
  const H323ChannelNumber & other = (const H323ChannelNumber &)obj;
  if (number < other.number)
    return LessThan;
  if (number > other.number)
    return GreaterThan;
  if (fromRemote && !other.fromRemote)
    return LessThan;
  if (!fromRemote && other.fromRemote)
    return GreaterThan;
  return EqualTo;
}


H323ChannelNumber & H323ChannelNumber::operator++(int)
{
  number++;
  return *this;
}


/////////////////////////////////////////////////////////////////////////////

H323Channel::H323Channel(H323Connection & conn, const H323Capability & cap)
  : endpoint(conn.GetEndPoint()),
    connection(conn)
{
  capability = (H323Capability *)cap.Clone();
  codec = NULL;
  bandwidthUsed = 0;
  receiveThread = NULL;
  transmitThread = NULL;
  terminating = FALSE;
  opened = FALSE;
  paused = FALSE;
}


H323Channel::~H323Channel()
{
  connection.UseBandwidth(bandwidthUsed, TRUE);
  PTRACE(3, "LogChan\tDeleting codec and capability");

  delete codec;
  delete capability;
}


void H323Channel::PrintOn(ostream & strm) const
{
  strm << number;
}


unsigned H323Channel::GetSessionID() const
{
  return 0;
}


void H323Channel::CleanUpOnTermination()
{
  if (!opened || terminating)
    return;

  PTRACE(3, "LogChan\tCleaning up " << number);

  terminating = TRUE;

  // If we have a codec, then close it, this allows the transmitThread to be
  // broken out of any I/O block on reading the codec.
  if (codec != NULL)
    codec->Close();

  // If we have a receiver thread, wait for it to die.
  if (receiveThread != NULL) {
    PTRACE(4, "LogChan\tAwaiting termination of " << receiveThread << ' ' << receiveThread->GetThreadName());
    PAssert(receiveThread->WaitForTermination(10000), "Receive media thread did not terminate");
    delete receiveThread;
    receiveThread = NULL;
  }

  // If we have a transmitter thread, wait for it to die.
  if (transmitThread != NULL) {
    PTRACE(4, "LogChan\tAwaiting termination of " << transmitThread << ' ' << transmitThread->GetThreadName());
    PAssert(transmitThread->WaitForTermination(10000), "Transmit media thread did not terminate");
    delete transmitThread;
    transmitThread = NULL;
  }

  // Signal to the connection that this channel is on the way out
  connection.OnClosedLogicalChannel(*this);

  PTRACE(3, "LogChan\tCleaned up " << number);
}


BOOL H323Channel::IsRunning() const
{
  if (receiveThread  != NULL && !receiveThread ->IsTerminated())
    return TRUE;

  if (transmitThread != NULL && !transmitThread->IsTerminated())
    return TRUE;

  return FALSE;
}


BOOL H323Channel::OnReceivedPDU(const H245_OpenLogicalChannel & /*pdu*/,
                                unsigned & /*errorCode*/)
{
  return TRUE;
}


BOOL H323Channel::OnReceivedAckPDU(const H245_OpenLogicalChannelAck & /*pdu*/)
{
  return TRUE;
}


void H323Channel::OnSendOpenAck(const H245_OpenLogicalChannel & /*pdu*/,
                                H245_OpenLogicalChannelAck & /* pdu*/) const
{
}


void H323Channel::OnFlowControl(long bitRateRestriction)
{
  if (GetCodec() != NULL)
    codec->OnFlowControl(bitRateRestriction);
  else
    PTRACE(3, "LogChan\tOnFlowControl: " << bitRateRestriction);
}


void H323Channel::OnMiscellaneousCommand(const H245_MiscellaneousCommand_type & type)
{
  if (GetCodec() != NULL)
    codec->OnMiscellaneousCommand(type);
  else
    PTRACE(3, "LogChan\tOnMiscellaneousCommand: chan=" << number
           << ", type=" << type.GetTagName());
}


void H323Channel::OnMiscellaneousIndication(const H245_MiscellaneousIndication_type & type)
{
  if (GetCodec() != NULL)
    codec->OnMiscellaneousIndication(type);
  else
    PTRACE(3, "LogChan\tOnMiscellaneousIndication: chan=" << number
           << ", type=" << type.GetTagName());
}


void H323Channel::OnJitterIndication(DWORD PTRACE_PARAM(jitter),
                                     int   PTRACE_PARAM(skippedFrameCount),
                                     int   PTRACE_PARAM(additionalBuffer))
{
  PTRACE(3, "LogChan\tOnJitterIndication:"
            " jitter=" << jitter <<
            " skippedFrameCount=" << skippedFrameCount <<
            " additionalBuffer=" << additionalBuffer);
}


BOOL H323Channel::SetInitialBandwidth()
{
  if (GetCodec() == NULL)
    return TRUE;

  return SetBandwidthUsed(codec->GetMediaFormat().GetBandwidth()/100);
}


BOOL H323Channel::SetBandwidthUsed(unsigned bandwidth)
{
  PTRACE(3, "LogChan\tBandwidth requested/used = "
         << bandwidth/10 << '.' << bandwidth%10 << '/'
         << bandwidthUsed/10 << '.' << bandwidthUsed%10
         << " kb/s");
  connection.UseBandwidth(bandwidthUsed, TRUE);
  bandwidthUsed = 0;

  if (!connection.UseBandwidth(bandwidth, FALSE))
    return FALSE;

  bandwidthUsed = bandwidth;
  return TRUE;
}


BOOL H323Channel::Open()
{
//  PTRACE(1, "LogChan\tH323Channel->OnStartLogicalChannel call");

  if (opened)
    return TRUE;

  // Give the connection (or endpoint) a chance to do something with
  // the opening of the codec. Default sets up various filters.

  if (!connection.OnStartLogicalChannel(*this)) {
    PTRACE(1, "LogChan\tOnStartLogicalChannel failed");
    return FALSE;
  }
  
  opened = TRUE;
  return TRUE;
}


H323Codec * H323Channel::GetCodec() const
{
  if (codec == NULL) {
    ((H323Channel*)this)->codec = capability->CreateCodec(
                  GetDirection() == IsReceiver ? H323Codec::Decoder : H323Codec::Encoder);
#ifdef H323_AUDIO_CODECS
    if (codec && PIsDescendant(codec, H323AudioCodec))
      ((H323AudioCodec*)codec)->SetSilenceDetectionMode(endpoint.GetSilenceDetectionMode()); 
#endif
  }

  return codec;
}


void H323Channel::SendMiscCommand(unsigned command)
{ 
  connection.SendLogicalChannelMiscCommand(*this, command); 
}


/////////////////////////////////////////////////////////////////////////////

H323UnidirectionalChannel::H323UnidirectionalChannel(H323Connection & conn,
                                                     const H323Capability & cap,
                                                     Directions direction)
  : H323Channel(conn, cap),
    receiver(direction == IsReceiver)
{
}


H323Channel::Directions H323UnidirectionalChannel::GetDirection() const
{
  return receiver ? IsReceiver : IsTransmitter;
}


BOOL H323UnidirectionalChannel::Start()
{
  if (!Open())
    return FALSE;

  PThread * thread = new H323LogicalChannelThread(endpoint, *this, receiver);

  if (receiver)
    receiveThread  = thread;
  else
    transmitThread = thread;
  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////

H323BidirectionalChannel::H323BidirectionalChannel(H323Connection & conn,
                                                   const H323Capability & cap)
  : H323Channel(conn, cap)
{
}


H323Channel::Directions H323BidirectionalChannel::GetDirection() const
{
  return IsBidirectional;
}


BOOL H323BidirectionalChannel::Start()
{
  receiveThread  = new H323LogicalChannelThread(endpoint, *this, TRUE);
  transmitThread = new H323LogicalChannelThread(endpoint, *this, FALSE);
  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////

H323_RealTimeChannel::H323_RealTimeChannel(H323Connection & connection,
                                           const H323Capability & capability,
                                           Directions direction)
  : H323UnidirectionalChannel(connection, capability, direction)
{
  rtpPayloadType = RTP_DataFrame::IllegalPayloadType;
}


BOOL H323_RealTimeChannel::OnSendingPDU(H245_OpenLogicalChannel & open) const
{
  PTRACE(3, "H323RTP\tOnSendingPDU");

  open.m_forwardLogicalChannelNumber = (unsigned)number;

  if (open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters)) {
    open.m_reverseLogicalChannelParameters.IncludeOptionalField(
            H245_OpenLogicalChannel_reverseLogicalChannelParameters::e_multiplexParameters);
    // Set the communications information for unicast IPv4
    open.m_reverseLogicalChannelParameters.m_multiplexParameters.SetTag(
                H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters
                    ::e_h2250LogicalChannelParameters);

	if (OnSendingAltPDU(open.m_genericInformation))
		open.IncludeOptionalField(H245_OpenLogicalChannel::e_genericInformation);

    return OnSendingPDU(open.m_reverseLogicalChannelParameters.m_multiplexParameters);
  }
  else {
    // Set the communications information for unicast IPv4
    open.m_forwardLogicalChannelParameters.m_multiplexParameters.SetTag(
                H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters
                    ::e_h2250LogicalChannelParameters);

	if (OnSendingAltPDU(open.m_genericInformation))
		open.IncludeOptionalField(H245_OpenLogicalChannel::e_genericInformation);

    return OnSendingPDU(open.m_forwardLogicalChannelParameters.m_multiplexParameters);
  }
}


void H323_RealTimeChannel::OnSendOpenAck(const H245_OpenLogicalChannel & open,
                                         H245_OpenLogicalChannelAck & ack) const
{
  PTRACE(3, "H323RTP\tOnSendOpenAck");

  // set forwardMultiplexAckParameters option
  ack.IncludeOptionalField(H245_OpenLogicalChannelAck::e_forwardMultiplexAckParameters);

  // select H225 choice
  ack.m_forwardMultiplexAckParameters.SetTag(
	  H245_OpenLogicalChannelAck_forwardMultiplexAckParameters::e_h2250LogicalChannelAckParameters);

  // get H225 parms
  H245_H2250LogicalChannelAckParameters & param = ack.m_forwardMultiplexAckParameters;

  // set session ID
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_sessionID);
  const H245_H2250LogicalChannelParameters & openparam =
                          open.m_forwardLogicalChannelParameters.m_multiplexParameters;
  unsigned sessionID = openparam.m_sessionID;
  param.m_sessionID = sessionID;

  if (connection.isSameNAT()) {
		ack.IncludeOptionalField(H245_OpenLogicalChannelAck::e_genericInformation);
		OnSendOpenAckAlt(ack.m_genericInformation);
  }

  OnSendOpenAck(param);

  PTRACE(2, "H323RTP\tSending open logical channel ACK: sessionID=" << sessionID);
}


BOOL H323_RealTimeChannel::OnReceivedPDU(const H245_OpenLogicalChannel & open,
                                         unsigned & errorCode)
{
  if (receiver)
    number = H323ChannelNumber(open.m_forwardLogicalChannelNumber, TRUE);

  PTRACE(3, "H323RTP\tOnReceivedPDU for channel: " << number);

  unsigned prevTxFrames = capability->GetTxFramesInPacket();
  unsigned prevRxFrames = capability->GetRxFramesInPacket();
  PString  prevFormat   = capability->GetFormatName();

  BOOL reverse = open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters);
  const H245_DataType & dataType = reverse ? open.m_reverseLogicalChannelParameters.m_dataType
                                           : open.m_forwardLogicalChannelParameters.m_dataType;

  if (!capability->OnReceivedPDU(dataType, receiver)) {
    PTRACE(1, "H323RTP\tData type not supported");
    errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotSupported;
    return FALSE;
  }

	if (open.HasOptionalField(H245_OpenLogicalChannel::e_genericInformation))
		            OnReceivedAltPDU(open.m_genericInformation);

  // If we have already created a codec, and the new parameters indicate that
  // the capability limits have changed, then kill off the old codec it will
  // be wrongly constructed.
  if (codec != NULL &&
      (prevTxFrames != capability->GetTxFramesInPacket() ||
       prevRxFrames != capability->GetRxFramesInPacket() ||
       prevFormat   != capability->GetFormatName())) {
    delete codec;
    codec = NULL;
  }

  if (reverse) {
    if (open.m_reverseLogicalChannelParameters.m_multiplexParameters.GetTag() ==
             H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters)
      return OnReceivedPDU(open.m_reverseLogicalChannelParameters.m_multiplexParameters, errorCode);
  }
  else {
    if (open.m_forwardLogicalChannelParameters.m_multiplexParameters.GetTag() ==
             H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters)
      return OnReceivedPDU(open.m_forwardLogicalChannelParameters.m_multiplexParameters, errorCode);
  }

  PTRACE(1, "H323RTP\tOnly H.225.0 multiplex supported");
  errorCode = H245_OpenLogicalChannelReject_cause::e_unsuitableReverseParameters;
  return FALSE;
}


BOOL H323_RealTimeChannel::OnReceivedAckPDU(const H245_OpenLogicalChannelAck & ack)
{
  PTRACE(3, "H323RTP\tOnReceiveOpenAck");

  if (!ack.HasOptionalField(H245_OpenLogicalChannelAck::e_forwardMultiplexAckParameters)) {
    PTRACE(1, "H323RTP\tNo forwardMultiplexAckParameters");
    return FALSE;
  }

  if (ack.m_forwardMultiplexAckParameters.GetTag() !=
            H245_OpenLogicalChannelAck_forwardMultiplexAckParameters::e_h2250LogicalChannelAckParameters) {
    PTRACE(1, "H323RTP\tOnly H.225.0 multiplex supported");
    return FALSE;
  }

  if (ack.HasOptionalField(H245_OpenLogicalChannel::e_genericInformation))
		               OnReceivedAckAltPDU(ack.m_genericInformation);

  return OnReceivedAckPDU(ack.m_forwardMultiplexAckParameters);
}


RTP_DataFrame::PayloadTypes H323_RealTimeChannel::GetRTPPayloadType() const
{
  RTP_DataFrame::PayloadTypes pt = rtpPayloadType;

  if (pt == RTP_DataFrame::IllegalPayloadType) {
    pt = capability->GetPayloadType();
    if (pt == RTP_DataFrame::IllegalPayloadType) {
      PAssertNULL(codec);
      pt = codec->GetMediaFormat().GetPayloadType();
    }
  }

  return pt;
}


BOOL H323_RealTimeChannel::SetDynamicRTPPayloadType(int newType)
{
  PTRACE(1, "H323RTP\tSetting dynamic RTP payload type: " << newType);

  // This is "no change"
  if (newType == -1)
    return TRUE;

  // Check for illegal type
  if (newType < RTP_DataFrame::DynamicBase || newType > RTP_DataFrame::MaxPayloadType)
    return FALSE;

  // Check for overwriting "known" type
  if (rtpPayloadType < RTP_DataFrame::DynamicBase)
    return FALSE;

  rtpPayloadType = (RTP_DataFrame::PayloadTypes)newType;
  PTRACE(3, "H323RTP\tSetting dynamic payload type to " << rtpPayloadType);
  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////

H323_RTPChannel::H323_RTPChannel(H323Connection & conn,
                                 const H323Capability & cap,
                                 Directions direction,
                                 RTP_Session & r)
  : H323_RealTimeChannel(conn, cap, direction),
    rtpSession(r),
    rtpCallbacks(*(H323_RTP_Session *)r.GetUserData())
{
  PTRACE(3, "H323RTP\t" << (receiver ? "Receiver" : "Transmitter")
         << " created using session " << GetSessionID());
}


H323_RTPChannel::~H323_RTPChannel()
{
  // Finished with the RTP session, this will delete the session if it is no
  // longer referenced by any logical channels.
  connection.ReleaseSession(GetSessionID());
}


void H323_RTPChannel::CleanUpOnTermination()
{
  if (terminating)
    return;

  PTRACE(3, "H323RTP\tCleaning up RTP " << number);

  // Break any I/O blocks and wait for the thread that uses this object to
  // terminate before we allow it to be deleted.
  if ((receiver ? receiveThread : transmitThread) != NULL)
    rtpSession.Close(receiver);

  H323Channel::CleanUpOnTermination();
}


unsigned H323_RTPChannel::GetSessionID() const
{
  return rtpSession.GetSessionID();
}


BOOL H323_RTPChannel::Open()
{
//  PTRACE(1, "LogChan\tH323_RTPChannel->OnStartLogicalChannel call");

  if (opened)
    return TRUE;

  if (GetCodec() == NULL) {
    PTRACE(1, "LogChan\t" << (GetDirection() == IsReceiver ? "Receive" : "Transmit")
           << " thread aborted (could not create codec)");
    return FALSE;
  }

  if (!codec->GetMediaFormat().IsValid()) {
    PTRACE(1, "LogChan\t" << (GetDirection() == IsReceiver ? "Receive" : "Transmit")
           << " thread aborted (invalid media format)");
    return FALSE;
  }

  codec->AttachLogicalChannel((H323Channel*)this);

  // Open the codec
  if (!codec->Open(connection)) {
    PTRACE(1, "LogChan\t" << (GetDirection() == IsReceiver ? "Receive" : "Transmit")
           << " thread aborted (open fail) for "<< *capability);
    return FALSE;
  }

  // Give the connection (or endpoint) a chance to do something with
  // the opening of the codec. Default sets up various filters.

  if (!connection.OnStartLogicalChannel(*this)) {
    PTRACE(1, "LogChan\t" << (GetDirection() == IsReceiver ? "Receive" : "Transmit")
           << " thread aborted (OnStartLogicalChannel fail)");
    return FALSE;
  }

  PTRACE(3, "LogChan\tOpened using capability " << *capability);

  opened = TRUE;

  return TRUE;
}


BOOL H323_RTPChannel::OnSendingPDU(H245_H2250LogicalChannelParameters & param) const
{
  return rtpCallbacks.OnSendingPDU(*this, param);
}

BOOL H323_RTPChannel::OnSendingAltPDU(H245_ArrayOf_GenericInformation & alternate) const
{
  return rtpCallbacks.OnSendingAltPDU(*this, alternate);
}

void H323_RTPChannel::OnSendOpenAck(H245_H2250LogicalChannelAckParameters & param) const
{
  rtpCallbacks.OnSendingAckPDU(*this, param);
}

void H323_RTPChannel::OnSendOpenAckAlt(H245_ArrayOf_GenericInformation & alternate) const
{
  rtpCallbacks.OnSendOpenAckAlt(*this, alternate);
}

BOOL H323_RTPChannel::OnReceivedPDU(const H245_H2250LogicalChannelParameters & param,
                                    unsigned & errorCode)
{
  return rtpCallbacks.OnReceivedPDU(*this, param, errorCode);
}

BOOL H323_RTPChannel::OnReceivedAltPDU(const H245_ArrayOf_GenericInformation & alternate)
{
  return rtpCallbacks.OnReceivedAltPDU(*this, alternate);
}


BOOL H323_RTPChannel::OnReceivedAckPDU(const H245_H2250LogicalChannelAckParameters & param)
{
  return rtpCallbacks.OnReceivedAckPDU(*this, param);
}

BOOL H323_RTPChannel::OnReceivedAckAltPDU(const H245_ArrayOf_GenericInformation & alternate)
{ 
  return rtpCallbacks.OnReceivedAckAltPDU(*this, alternate);
}

BOOL H323_RTPChannel::ReadFrame(DWORD & rtpTimestamp, RTP_DataFrame & frame)
{
  return rtpSession.ReadBufferedData(rtpTimestamp, frame);
}

BOOL H323_RTPChannel::WriteFrame(RTP_DataFrame & frame)
{
  return rtpSession.PreWriteData(frame) && rtpSession.WriteData(frame);
//  return rtpSession.WriteData(frame);
}


#if PTRACING
class CodecReadAnalyser
{
  enum { MaxSamples = 1000 };
  public:
    CodecReadAnalyser() { count = 0; }
    void AddSample(DWORD timestamp)
      {
        if (count < MaxSamples) {
          tick[count] = PTimer::Tick();
          rtp[count] = timestamp;
          count++;
        }
      }
    friend ostream & operator<<(ostream & strm, const CodecReadAnalyser & analysis)
      {
        PTimeInterval minimum = PMaxTimeInterval;
        PTimeInterval maximum;
        for (PINDEX i = 1; i < analysis.count; i++) {
          PTimeInterval delta = analysis.tick[i] - analysis.tick[i-1];
          strm << setw(6) << analysis.rtp[i] << ' '
               << setw(6) << (analysis.tick[i] - analysis.tick[0]) << ' '
               << setw(6) << delta
               << '\n';
          if (delta > maximum)
            maximum = delta;
          if (delta < minimum)
            minimum = delta;
        }
        strm << "Maximum delta time: " << maximum << "\n"
                "Minimum delta time: " << minimum << '\n';
        return strm;
      }
  private:
    PTimeInterval tick[MaxSamples];
    DWORD rtp[MaxSamples];
    PINDEX count;
};
#endif


void H323_RTPChannel::Transmit()
{
  if (terminating) {
    PTRACE(3, "H323RTP\tTransmit thread terminated on start up");
    return;
  }

  const OpalMediaFormat & mediaFormat = codec->GetMediaFormat();

  // Get parameters from the codec on time and data sizes
  BOOL isAudio = mediaFormat.NeedsJitterBuffer();
  unsigned framesInPacket = capability->GetTxFramesInPacket();

  rtpPayloadType = GetRTPPayloadType();
  if (rtpPayloadType == RTP_DataFrame::G722) framesInPacket/=10;
  
  unsigned maxFrameSize = mediaFormat.GetFrameSize();
  if (maxFrameSize == 0)
    maxFrameSize = isAudio ? 8 : 2000;
  RTP_DataFrame frame(framesInPacket*maxFrameSize);

  if (rtpPayloadType == RTP_DataFrame::IllegalPayloadType) {
     PTRACE(1, "H323RTP\tReceive " << mediaFormat << " thread ended (illegal payload type)");
     return;
  }
  frame.SetPayloadType(rtpPayloadType); 

  PTRACE(2, "H323RTP\tTransmit " << mediaFormat << " thread started:"
            " rate=" << codec->GetFrameRate() <<
            " time=" << (codec->GetFrameRate()/(mediaFormat.GetTimeUnits() > 0 ? mediaFormat.GetTimeUnits() : 1)) << "ms" <<
            " size=" << framesInPacket << '*' << maxFrameSize << '='
                    << (framesInPacket*maxFrameSize) );

  // This is real time so need to keep track of elapsed milliseconds
  BOOL silent = TRUE;
  unsigned length;
  unsigned frameOffset = 0;
  unsigned frameCount = 0;
  DWORD rtpFirstTimestamp = rand();
  DWORD rtpTimestamp = rtpFirstTimestamp;
  PTimeInterval firstFrameTick = PTimer::Tick();
  frame.SetPayloadSize(0);

#if PTRACING
  DWORD lastDisplayedTimestamp = 0;
  CodecReadAnalyser * codecReadAnalysis = NULL;
  if (PTrace::GetLevel() >= 5)
    codecReadAnalysis = new CodecReadAnalyser;
#endif

  /* Now keep getting encoded frames from the codec, it is expected that the
     Read() function will maintain the Real Time aspects of the transmission.
     That is for GSM codec say with a single frame, this function will take
     20 milliseconds to complete.
   */
  while (codec->Read(frame.GetPayloadPtr()+frameOffset, length, frame))
  {
    if (paused)
      length = 0; // Act as though silent/no video

    // Handle marker bit for audio codec
    if (isAudio) {
      // If switching from silence to signal
      if (silent && length > 0) {
        silent = FALSE;
        frame.SetMarker(TRUE);  // Set flag for start of sound
        PTRACE(3, "H323RTP\tTransmit start of talk burst: " << rtpTimestamp);
      }
      // If switching from signal to silence
      else if (!silent && length == 0) {
        silent = TRUE;
        // If had some data waiting to go out
        if (frameOffset > 0)
          frameCount = framesInPacket;  // Force the RTP write
        PTRACE(3, "H323RTP\tTransmit  end  of talk burst: " << rtpTimestamp);
      }
    }

    // See if is silence or have some audio data to stuff in the RTP packet
    if (length == 0)
      frame.SetTimestamp(rtpTimestamp);
    else {
      silenceStartTick = PTimer::Tick();

      // If first read frame in packet, set timestamp for it
      if (frameOffset == 0)
        frame.SetTimestamp(rtpTimestamp);
      frameOffset += length;

      // Look for special cases
      if (rtpPayloadType == RTP_DataFrame::G729 && length == 2) {
        /* If we have a G729 sid frame (ie 2 bytes instead of 10) then we must
           not send any more frames in the RTP packet.
         */
        frameCount = framesInPacket;
      }
      else {
        /* Increment by number of frames that were read in one hit Note a
           codec that does variable length frames should never return more
           than one frame per Read() call or confusion will result.
         */
        frameCount += (length + maxFrameSize - 1)/maxFrameSize;
      }
    }

    BOOL sendPacket = FALSE;

    // Have read number of frames for packet (or just went silent)
    if (frameCount >= framesInPacket) {
      // Set payload size to frame offset, now length of frame.
      frame.SetPayloadSize(frameOffset);
      frame.SetPayloadType(rtpPayloadType);

      frameOffset = 0;
      frameCount = 0;

      sendPacket = TRUE;
    }

    if(isAudio)
    {
     filterMutex.Wait();
     for (PINDEX i = 0; i < filters.GetSize(); i++)
     { filters[i](frame, (INT)&sendPacket); } 
     filterMutex.Signal();
    }

    if (sendPacket || (silent && frame.GetPayloadSize() > 0)) {
      // Send the frame of coded data we have so far to RTP transport
//      if (!rtpSession.WriteData(frame))
      if (!WriteFrame(frame))
         break;

      if (!isAudio)
//      { 
       if(!frame.GetMarker()) PThread::Sleep(1);
       /*else*/ //rtpTimestamp = nextTimestamp;
//      }

      // Reset flag for in talk burst
      if (isAudio)
        frame.SetMarker(FALSE); 

      frame.SetPayloadSize(0);
      frameOffset = 0;
      frameCount = 0;
    }
    else PTRACE(3, "H323READ\t Drop Packet");

    if (terminating)
      break;

    // Calculate the timestamp and real time to take in processing
    if(isAudio)
    {
      rtpTimestamp += codec->GetFrameRate();
    }
    else
    { // video:
      if(frame.GetMarker())
      {
        rtpTimestamp = rtpFirstTimestamp + ((PTimer::Tick() - firstFrameTick).GetInterval() * 90);
      }
    }

#if PTRACING
    if (rtpTimestamp - lastDisplayedTimestamp > RTP_TRACE_DISPLAY_RATE) {
      PTRACE(9, "H323RTP\tTransmitter sent timestamp " << rtpTimestamp);
      lastDisplayedTimestamp = rtpTimestamp;
    }

    if (codecReadAnalysis != NULL)
      codecReadAnalysis->AddSample(rtpTimestamp);
#endif

  }


#if PTRACING
  PTRACE_IF(5, codecReadAnalysis != NULL, "Codec read timing:\n" << *codecReadAnalysis);
  delete codecReadAnalysis;
#endif

  if (!terminating)
    connection.CloseLogicalChannelNumber(number);

  PTRACE(2, "H323RTP\tTransmit " << mediaFormat << " thread ended");
}


void H323_RTPChannel::Receive()
{
  if (terminating) {
    PTRACE(3, "H323RTP\tReceive thread terminated on start up");
    return;
  }

  const OpalMediaFormat & mediaFormat = codec->GetMediaFormat();

  PTRACE(2, "H323RTP\tReceive " << mediaFormat << " thread started.");

  // if jitter buffer required, start the thread that is on the other end of it
  if (mediaFormat.NeedsJitterBuffer())
    rtpSession.SetJitterBufferSize(connection.GetMinAudioJitterDelay()*mediaFormat.GetTimeUnits(),
                                   connection.GetMaxAudioJitterDelay()*mediaFormat.GetTimeUnits(),
                                   endpoint.GetJitterThreadStackSize());

  // Keep time using th RTP timestamps.
  DWORD codecFrameRate = codec->GetFrameRate();
  DWORD rtpTimestamp = 0;
#if PTRACING
  DWORD lastDisplayedTimestamp = 0;
#endif

  rtpPayloadType = GetRTPPayloadType();
  if (rtpPayloadType == RTP_DataFrame::IllegalPayloadType) {
     PTRACE(1, "H323RTP\tTransmit " << mediaFormat << " thread ended (illegal payload type)");
     return;
  }

  // keep track of consecutive payload type mismatches
  int consecutiveMismatches = 0;

  // do not change payload type for audio and video
  BOOL allowRtpPayloadChange = FALSE;
  //BOOL allowRtpPayloadChange = codec->GetMediaFormat().GetDefaultSessionID() == OpalMediaFormat::DefaultAudioSessionID;

  RTP_DataFrame frame;
  while (ReadFrame(rtpTimestamp, frame)) {

    filterMutex.Wait();
    for (PINDEX i = 0; i < filters.GetSize(); i++)
      filters[i](frame, 0);
    filterMutex.Signal();

    int size = frame.GetPayloadSize();
    rtpTimestamp = frame.GetTimestamp();

#if PTRACING
    if (rtpTimestamp - lastDisplayedTimestamp > RTP_TRACE_DISPLAY_RATE) {
      PTRACE(9, "H323RTP\tReceiver written timestamp " << rtpTimestamp);
      lastDisplayedTimestamp = rtpTimestamp;
    }
#endif

    unsigned written;
    BOOL ok = TRUE;
    if (size == 0) {
      ok = codec->Write(NULL, 0, frame, written);
      rtpTimestamp += codecFrameRate;
    } else {
      silenceStartTick = PTimer::Tick();

      BOOL isCodecPacket = TRUE;

      if (frame.GetPayloadType() == rtpPayloadType) {
        PTRACE_IF(2, consecutiveMismatches > 0,
                  "H323RTP\tPayload type matched again " << rtpPayloadType);
        consecutiveMismatches = 0;
      }
      else {
        consecutiveMismatches++;
        if (allowRtpPayloadChange && consecutiveMismatches >= MAX_PAYLOAD_TYPE_MISMATCHES) {
          rtpPayloadType = frame.GetPayloadType();
          consecutiveMismatches = 0;
          PTRACE(1, "H323RTP\tResetting expected payload type to " << rtpPayloadType);
        }
        PTRACE_IF(2, consecutiveMismatches < MAX_PAYLOAD_TYPE_MISMATCHES, "H323RTP\tPayload type mismatch: expected "
              << rtpPayloadType << ", got " << frame.GetPayloadType()
              << ". Ignoring packet.");
      }

      if (isCodecPacket && consecutiveMismatches == 0) {
        const BYTE * ptr = frame.GetPayloadPtr();
        while (ok && size > 0) {
          /* Now write data to the codec, it is expected that the Write()
             function will maintain the Real Time aspects of the system. That
             is for GSM codec, say with a single frame, this function will take
             20 milliseconds to complete. It is very important that this occurs
             for audio codecs or the jitter buffer will not operate correctly.
           */
          ok = codec->Write(ptr, paused ? 0 : size, frame, written);
          rtpTimestamp += codecFrameRate;
          size -= written != 0 ? written : size;
          ptr += written;
          PTRACE(9, "H323RTP\tWrite to decoder");          
        }
        PTRACE_IF(1, size < 0, "H323RTP\tPayload size too small, short " << -size << " bytes.");
      }
    }

    if (terminating)
      break;

    if (!ok) {
      connection.CloseLogicalChannelNumber(number);
      break;
    }
  }

  PTRACE(2, "H323RTP\tReceive " << mediaFormat << " thread ended");
}


void H323_RTPChannel::AddFilter(const PNotifier & filterFunction)
{
  filterMutex.Wait();
  filters.Append(new PNotifier(filterFunction));
  filterMutex.Signal();
}


void H323_RTPChannel::RemoveFilter(const PNotifier & filterFunction)
{
  filterMutex.Wait();
  PINDEX idx = filters.GetValuesIndex(filterFunction);
  if (idx != P_MAX_INDEX)
    filters.RemoveAt(idx);
  filterMutex.Signal();
}


PTimeInterval H323_RTPChannel::GetSilenceDuration() const
{
  if (silenceStartTick == 0)
    return silenceStartTick;

  return PTimer::Tick() - silenceStartTick;
}


/////////////////////////////////////////////////////////////////////////////

H323_ExternalRTPChannel::H323_ExternalRTPChannel(H323Connection & connection,
                                                 const H323Capability & capability,
                                                 Directions direction,
                                                 unsigned id)
  : H323_RealTimeChannel(connection, capability, direction)
{
  sessionID = id;
  isRunning = FALSE;
}


H323_ExternalRTPChannel::H323_ExternalRTPChannel(H323Connection & connection,
                                                 const H323Capability & capability,
                                                 Directions direction,
                                                 unsigned id,
                                                 const H323TransportAddress & data,
                                                 const H323TransportAddress & control)
  : H323_RealTimeChannel(connection, capability, direction),
    externalMediaAddress(data),
    externalMediaControlAddress(control)
{
  sessionID = id;
  isRunning = FALSE;
}


H323_ExternalRTPChannel::H323_ExternalRTPChannel(H323Connection & connection,
                                                 const H323Capability & capability,
                                                 Directions direction,
                                                 unsigned id,
                                                 const PIPSocket::Address & ip,
                                                 WORD dataPort)
  : H323_RealTimeChannel(connection, capability, direction),
    externalMediaAddress(ip, dataPort),
    externalMediaControlAddress(ip, (WORD)(dataPort+1))
{
  sessionID = id;
  isRunning = FALSE;
}


unsigned H323_ExternalRTPChannel::GetSessionID() const
{
  return sessionID;
}


BOOL H323_ExternalRTPChannel::Start()
{
  isRunning = TRUE;
  return Open();
}


BOOL H323_ExternalRTPChannel::IsRunning() const
{
  return opened && isRunning;
}


void H323_ExternalRTPChannel::Receive()
{
  // Do nothing
}


void H323_ExternalRTPChannel::Transmit()
{
  // Do nothing
}


BOOL H323_ExternalRTPChannel::OnSendingPDU(H245_H2250LogicalChannelParameters & param) const
{
  param.m_sessionID = sessionID;

  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaGuaranteedDelivery);
  param.m_mediaGuaranteedDelivery = FALSE;

  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_silenceSuppression);
  param.m_silenceSuppression = FALSE;

  // unicast must have mediaControlChannel
  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel);
  externalMediaControlAddress.SetPDU(param.m_mediaControlChannel);

  if (receiver) {
    // set mediaChannel
    param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
    externalMediaAddress.SetPDU(param.m_mediaChannel);
  }

  return TRUE;
}


void H323_ExternalRTPChannel::OnSendOpenAck(H245_H2250LogicalChannelAckParameters & param) const
{
  // set mediaControlChannel
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaControlChannel);
  externalMediaControlAddress.SetPDU(param.m_mediaControlChannel);

  // set mediaChannel
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
  externalMediaAddress.SetPDU(param.m_mediaChannel);
}


BOOL H323_ExternalRTPChannel::OnReceivedPDU(const H245_H2250LogicalChannelParameters & param,
                                          unsigned & errorCode)
{
  // Only support a single audio session
  if (param.m_sessionID != sessionID) {
    PTRACE(1, "LogChan\tOpen for invalid session: " << param.m_sessionID);
    errorCode = H245_OpenLogicalChannelReject_cause::e_invalidSessionID;
    return FALSE;
  }

  if (!param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel)) {
    PTRACE(1, "LogChan\tNo mediaControlChannel specified");
    errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
    return FALSE;
  }

  remoteMediaControlAddress = param.m_mediaControlChannel;
  if (remoteMediaControlAddress.IsEmpty())
    return FALSE;

  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaChannel)) {
    remoteMediaAddress = param.m_mediaChannel;
    if (remoteMediaAddress.IsEmpty())
      return FALSE;
  }

  return TRUE;
}


BOOL H323_ExternalRTPChannel::OnReceivedAckPDU(const H245_H2250LogicalChannelAckParameters & param)
{
  if (param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_sessionID) && (param.m_sessionID != sessionID)) {
    PTRACE(1, "LogChan\twarning: Ack for invalid session: " << param.m_sessionID);
  }

  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaControlChannel)) {
    PTRACE(1, "LogChan\tNo mediaControlChannel specified");
    return FALSE;
  }

  remoteMediaControlAddress = param.m_mediaControlChannel;
  if (remoteMediaControlAddress.IsEmpty())
    return FALSE;

  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel)) {
    PTRACE(1, "LogChan\tNo mediaChannel specified");
    return FALSE;
  }

  remoteMediaAddress = param.m_mediaChannel;
  if (remoteMediaAddress.IsEmpty())
    return FALSE;

  return TRUE;
}


void H323_ExternalRTPChannel::SetExternalAddress(const H323TransportAddress & data,
                                                 const H323TransportAddress & control)
{
  externalMediaAddress = data;
  externalMediaControlAddress = control;

  if (data.IsEmpty() || control.IsEmpty()) {
    PIPSocket::Address ip;
    WORD port;
    if (data.GetIpAndPort(ip, port))
      externalMediaControlAddress = H323TransportAddress(ip, (WORD)(port+1));
    else if (control.GetIpAndPort(ip, port))
      externalMediaAddress = H323TransportAddress(ip, (WORD)(port-1));
  }
}


BOOL H323_ExternalRTPChannel::GetRemoteAddress(PIPSocket::Address & ip,
                                               WORD & dataPort) const
{
  if (!remoteMediaControlAddress) {
    if (remoteMediaControlAddress.GetIpAndPort(ip, dataPort)) {
      dataPort--;
      return TRUE;
    }
  }

  if (!remoteMediaAddress)
    return remoteMediaAddress.GetIpAndPort(ip, dataPort);

  return FALSE;
}


/////////////////////////////////////////////////////////////////////////////

H323DataChannel::H323DataChannel(H323Connection & conn,
                                 const H323Capability & cap,
                                 Directions dir,
                                 unsigned id)
  : H323UnidirectionalChannel(conn, cap, dir)
{
  sessionID = id;
  listener = NULL;
  autoDeleteListener = TRUE;
  transport = NULL;
  autoDeleteTransport = TRUE;
  separateReverseChannel = FALSE;
}


H323DataChannel::~H323DataChannel()
{
  if (autoDeleteListener)
    delete listener;
  if (autoDeleteTransport)
    delete transport;
}


void H323DataChannel::CleanUpOnTermination()
{
  if (terminating)
    return;

  PTRACE(3, "LogChan\tCleaning up data channel " << number);

  // Break any I/O blocks and wait for the thread that uses this object to
  // terminate before we allow it to be deleted.
  if (listener != NULL)
    listener->Close();
  if (transport != NULL)
    transport->Close();

  H323UnidirectionalChannel::CleanUpOnTermination();
}


unsigned H323DataChannel::GetSessionID() const
{
  return sessionID;
}


BOOL H323DataChannel::OnSendingPDU(H245_OpenLogicalChannel & open) const
{
  PTRACE(3, "LogChan\tOnSendingPDU for channel: " << number);

  open.m_forwardLogicalChannelNumber = (unsigned)number;

  open.m_forwardLogicalChannelParameters.m_multiplexParameters.SetTag(
              H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters
                  ::e_h2250LogicalChannelParameters);
  H245_H2250LogicalChannelParameters & fparam = open.m_forwardLogicalChannelParameters.m_multiplexParameters;
  fparam.m_sessionID = GetSessionID();

  if (separateReverseChannel)
    return TRUE;

  open.IncludeOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters);
  open.m_reverseLogicalChannelParameters.IncludeOptionalField(
              H245_OpenLogicalChannel_reverseLogicalChannelParameters::e_multiplexParameters);
  open.m_reverseLogicalChannelParameters.m_multiplexParameters.SetTag(
              H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters
                  ::e_h2250LogicalChannelParameters);
  H245_H2250LogicalChannelParameters & rparam = open.m_reverseLogicalChannelParameters.m_multiplexParameters;
  rparam.m_sessionID = GetSessionID();

  return capability->OnSendingPDU(open.m_reverseLogicalChannelParameters.m_dataType);
}


void H323DataChannel::OnSendOpenAck(const H245_OpenLogicalChannel & /*open*/,
                                    H245_OpenLogicalChannelAck & ack) const
{
  if (listener == NULL && transport == NULL) {
    PTRACE(2, "LogChan\tOnSendOpenAck without a listener or transport");
    return;
  }

  PTRACE(3, "LogChan\tOnSendOpenAck for channel: " << number);

  H245_H2250LogicalChannelAckParameters * param;

  if (separateReverseChannel) {
    ack.IncludeOptionalField(H245_OpenLogicalChannelAck::e_forwardMultiplexAckParameters);
    ack.m_forwardMultiplexAckParameters.SetTag(
              H245_OpenLogicalChannelAck_forwardMultiplexAckParameters::e_h2250LogicalChannelAckParameters);
    param = (H245_H2250LogicalChannelAckParameters*)&ack.m_forwardMultiplexAckParameters.GetObject();
  }
  else {
    ack.IncludeOptionalField(H245_OpenLogicalChannelAck::e_reverseLogicalChannelParameters);
    ack.m_reverseLogicalChannelParameters.m_multiplexParameters.SetTag(
              H245_OpenLogicalChannelAck_reverseLogicalChannelParameters_multiplexParameters
                  ::e_h2250LogicalChannelParameters);
    param = (H245_H2250LogicalChannelAckParameters*)
                &ack.m_reverseLogicalChannelParameters.m_multiplexParameters.GetObject();
  }

  unsigned session = GetSessionID();
  if (session != 0) {
    param->IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_sessionID);
    param->m_sessionID = GetSessionID();
  }

  param->IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
  if (listener != NULL)
    listener->SetUpTransportPDU(param->m_mediaChannel, connection.GetControlChannel());
  else
    transport->SetUpTransportPDU(param->m_mediaChannel, H323Transport::UseLocalTSAP);
}


BOOL H323DataChannel::OnReceivedPDU(const H245_OpenLogicalChannel & open,
                                    unsigned & errorCode)
{
  number = H323ChannelNumber(open.m_forwardLogicalChannelNumber, TRUE);

  PTRACE(3, "LogChan\tOnReceivedPDU for data channel: " << number);

  if (!CreateListener()) {
    PTRACE(1, "LogChan\tCould not create listener");
    errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
    return FALSE;
  }

  if (separateReverseChannel &&
      open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters)) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_unsuitableReverseParameters;
    PTRACE(2, "LogChan\tOnReceivedPDU has unexpected reverse parameters");
    return FALSE;
  }

  if (!capability->OnReceivedPDU(open.m_forwardLogicalChannelParameters.m_dataType, receiver)) {
    PTRACE(1, "H323RTP\tData type not supported");
    errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotSupported;
    return FALSE;
  }

  return TRUE;
}


BOOL H323DataChannel::OnReceivedAckPDU(const H245_OpenLogicalChannelAck & ack)
{
  PTRACE(3, "LogChan\tOnReceivedAckPDU");

  const H245_TransportAddress * address;

  if (separateReverseChannel) {
      PTRACE(3, "LogChan\tseparateReverseChannels");
    if (!ack.HasOptionalField(H245_OpenLogicalChannelAck::e_forwardMultiplexAckParameters)) {
      PTRACE(1, "LogChan\tNo forwardMultiplexAckParameters");
      return FALSE;
    }

    if (ack.m_forwardMultiplexAckParameters.GetTag() !=
              H245_OpenLogicalChannelAck_forwardMultiplexAckParameters::e_h2250LogicalChannelAckParameters) {
      PTRACE(1, "LogChan\tOnly H.225.0 multiplex supported");
      return FALSE;
    }

    const H245_H2250LogicalChannelAckParameters & param = ack.m_forwardMultiplexAckParameters;

    if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel)) {
      PTRACE(1, "LogChan\tNo media channel address provided");
      return FALSE;
    }

    address = &param.m_mediaChannel;

    if (ack.HasOptionalField(H245_OpenLogicalChannelAck::e_reverseLogicalChannelParameters)) {
      PTRACE(3, "LogChan\treverseLogicalChannelParameters set");
      reverseChannel = H323ChannelNumber(ack.m_reverseLogicalChannelParameters.m_reverseLogicalChannelNumber, TRUE);
    }
  }
  else {
    if (!ack.HasOptionalField(H245_OpenLogicalChannelAck::e_reverseLogicalChannelParameters)) {
      PTRACE(1, "LogChan\tNo reverseLogicalChannelParameters");
      return FALSE;
    }

    if (ack.m_reverseLogicalChannelParameters.m_multiplexParameters.GetTag() !=
              H245_OpenLogicalChannelAck_reverseLogicalChannelParameters_multiplexParameters
                              ::e_h2250LogicalChannelParameters) {
      PTRACE(1, "LogChan\tOnly H.225.0 multiplex supported");
      return FALSE;
    }

    const H245_H2250LogicalChannelParameters & param = ack.m_reverseLogicalChannelParameters.m_multiplexParameters;

    if (!param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaChannel)) {
      PTRACE(1, "LogChan\tNo media channel address provided");
      return FALSE;
    }

    address = &param.m_mediaChannel;
  }

  if (!CreateTransport()) {
    PTRACE(1, "LogChan\tCould not create transport");
    return FALSE;
  }

  if (!transport->ConnectTo(*address)) {
    PTRACE(1, "LogChan\tCould not connect to remote transport address: " << *address);
    return FALSE;
  }

  return TRUE;
}


BOOL H323DataChannel::CreateListener()
{
  if (listener == NULL) {
    listener = connection.GetControlChannel().GetLocalAddress().CreateCompatibleListener(connection.GetEndPoint());
    if (listener == NULL)
      return FALSE;

    PTRACE(3, "LogChan\tCreated listener for data channel: " << *listener);
  }

  return listener->Open();
}


BOOL H323DataChannel::CreateTransport()
{
  if (transport == NULL) {
    transport = connection.GetControlChannel().GetLocalAddress().CreateTransport(connection.GetEndPoint());
    if (transport == NULL)
      return FALSE;

    PTRACE(3, "LogChan\tCreated transport for data channel: " << *transport);
  }

  return transport != NULL;
}


/////////////////////////////////////////////////////////////////////////////
