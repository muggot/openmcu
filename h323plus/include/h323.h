/*
 * h323.h
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
 * $Log: h323.h,v $
 * Revision 1.2  2007/08/07 18:37:26  shorne
 * removed ptlib loading
 *
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.140.6.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.140  2005/01/03 06:25:52  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.139  2004/05/10 13:37:49  rjongbloed
 * Changed G.726 and MS-ADPCM to plug in codecs.
 *
 * Revision 1.138  2004/05/04 23:54:07  csoutheren
 * Removed usage of lpc10codec.h
 *
 * Revision 1.137  2004/05/03 13:21:40  rjongbloed
 * Converted everything to be codec plug in freindly
 * Removed GSM and G.729 as now plug ins are "the way"!
 *
 * Revision 1.136  2004/05/02 13:47:56  csoutheren
 * Removed gsmcodec.h
 *
 * Revision 1.135  2004/04/05 04:22:55  rjongbloed
 * Removed speex and iLBC direct build as now use plug in
 *
 * Revision 1.134  2004/04/03 12:17:07  csoutheren
 * Updated plugin changes for RTTI changes and added missing include
 *
 * Revision 1.133  2004/04/03 10:38:23  csoutheren
 * Added in initial cut at codec plugin code. Branches are for wimps :)
 *
 * Revision 1.132.2.1  2004/03/31 11:50:24  csoutheren
 * Removed references to builtin iLBC as this has been moved to plugins
 *
 * Revision 1.132  2003/06/06 02:19:03  rjongbloed
 * Added iLBC codec
 *
 * Revision 1.131  2003/05/14 03:29:56  rjongbloed
 * Added H.263 codec and autoconf options header file.
 *
 * Revision 1.130  2002/11/05 02:18:03  robertj
 * Changed default for speex codec to be included.
 *
 * Revision 1.129  2002/11/04 02:57:24  dereks
 * Put test around include of speexcodec.h    SPEEX is now an optional codec.
 *
 * Revision 1.128  2002/10/24 05:34:03  robertj
 * Added speex codec
 *
 * Revision 1.127  2002/10/01 03:07:12  robertj
 * Added version number functions for OpenH323 library itself, plus included
 *   library version in the default vendor information.
 *
 * Revision 1.126  2002/06/27 03:08:57  robertj
 * Added code to include G.729 & G.726 codecs on static linking.
 *
 * Revision 1.125  2001/10/23 02:18:06  dereks
 * Initial release of CU30 video codec.
 *
 * Revision 1.124  2001/09/11 01:24:36  robertj
 * Added conditional compilation to remove video and/or audio codecs.
 *
 * Revision 1.123  2001/08/06 03:08:11  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.122  2001/08/01 00:46:16  craigs
 * Added ability to early start without Alerting
 *
 * Revision 1.121  2001/07/17 04:44:29  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 * Revision 1.120  2001/07/06 02:29:12  robertj
 * Moved initialisation of local capabilities back to constructor for
 *   backward compatibility reasons.
 *
 * Revision 1.119  2001/07/05 04:18:23  robertj
 * Added call back for setting local capabilities.
 *
 * Revision 1.118  2001/06/19 03:55:28  robertj
 * Added transport to CreateConnection() function so can use that as part of
 *   the connection creation decision making process.
 *
 * Revision 1.117  2001/06/14 23:18:04  robertj
 * Change to allow for CreateConnection() to return NULL to abort call.
 *
 * Revision 1.116  2001/06/14 04:24:16  robertj
 * Changed incoming call to pass setup pdu to endpoint so it can create
 *   different connection subclasses depending on the pdu eg its alias
 *
 * Revision 1.115  2001/06/13 06:38:23  robertj
 * Added early start (media before connect) functionality.
 *
 * Revision 1.114  2001/05/31 01:28:47  robertj
 * Added functions to determine if call currently being held.
 *
 * Revision 1.113  2001/05/30 23:34:54  robertj
 * Added functions to send TCS=0 for transmitter side pause.
 *
 * Revision 1.112  2001/05/17 07:11:29  robertj
 * Added more call end types for common transport failure modes.
 *
 * Revision 1.111  2001/05/17 03:31:07  robertj
 * Fixed support for transmiter side paused (TCS=0), thanks Paul van de Wijngaard
 *
 * Revision 1.110  2001/05/14 05:56:25  robertj
 * Added H323 capability registration system so can add capabilities by
 *   string name instead of having to instantiate explicit classes.
 *
 * Revision 1.109  2001/05/09 04:59:02  robertj
 * Bug fixes in H.450.2, thanks Klein Stefan.
 *
 * Revision 1.108  2001/05/09 04:07:53  robertj
 * Added more call end codes for busy and congested.
 *
 * Revision 1.107  2001/05/01 04:34:10  robertj
 * Changed call transfer API slightly to be consistent with new hold function.
 *
 * Revision 1.106  2001/05/01 02:12:48  robertj
 * Added H.450.4 call hold (Near End only), thanks David M. Cassel.
 *
 * Revision 1.105  2001/04/23 01:31:13  robertj
 * Improved the locking of connections especially at shutdown.
 *
 * Revision 1.104  2001/04/11 03:01:27  robertj
 * Added H.450.2 (call transfer), thanks a LOT to Graeme Reid & Norwood Systems
 *
 * Revision 1.103  2001/03/21 04:52:40  robertj
 * Added H.235 security to gatekeepers, thanks Fürbass Franz!
 *
 * Revision 1.102  2001/03/16 06:46:19  robertj
 * Added ability to set endpoints desired time to live on a gatekeeper.
 *
 * Revision 1.101  2001/03/15 00:24:01  robertj
 * Added function for setting gatekeeper with address and identifier values.
 *
 * Revision 1.100  2001/03/08 07:45:04  robertj
 * Fixed issues with getting media channels started in some early start
 *   regimes, in particular better Cisco compatibility.
 *
 * Revision 1.99  2001/03/02 06:59:57  robertj
 * Enhanced the globally unique identifier class.
 *
 * Revision 1.98  2001/02/16 04:11:46  robertj
 * Added ability for RemoveListener() to remove all listeners.
 *
 * Revision 1.97  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.96  2000/12/19 22:33:44  dereks
 * Adjust so that the video channel is used for reading/writing raw video
 * data, which better modularizes the video codec.
 *
 * Revision 1.95  2000/12/18 08:58:30  craigs
 * Added ability set ports
 *
 * Revision 1.94  2000/12/18 01:22:28  robertj
 * Changed semantics or HasConnection() so returns TRUE until the connection
 *   has been deleted and not just until ClearCall() was executure on it.
 *
 * Revision 1.93  2000/12/05 01:52:00  craigs
 * Made ClearCall functions virtual to allow overiding
 *
 * Revision 1.92  2000/11/27 02:44:06  craigs
 * Added ClearCall Synchronous to H323Connection and H323Endpoint to
 * avoid race conditions with destroying descendant classes
 *
 * Revision 1.91  2000/11/26 23:13:23  craigs
 * Added ability to pass user data to H323Connection constructor
 *
 * Revision 1.90  2000/11/12 23:49:16  craigs
 * Added per connection versions of OnEstablished and OnCleared
 *
 * Revision 1.89  2000/11/08 04:30:00  robertj
 * Added function to be able to alter/remove the call proceeding PDU.
 *
 * Revision 1.88  2000/10/20 06:10:20  robertj
 * Fixed very small race condition on creating new connectionon incoming call.
 *
 * Revision 1.87  2000/10/19 04:06:54  robertj
 * Added function to be able to remove a listener.
 *
 * Revision 1.86  2000/10/16 09:51:16  robertj
 * Fixed problem with not opening fast start video receive if do not have transmit enabled.
 *
 * Revision 1.85  2000/10/13 02:15:23  robertj
 * Added support for Progress Indicator Q.931/H.225 message.
 *
 * Revision 1.84  2000/10/04 12:20:50  robertj
 * Changed setting of callToken in H323Connection to be as early as possible.
 *
 * Revision 1.83  2000/09/25 12:59:16  robertj
 * Added StartListener() function that takes a H323TransportAddress to start
 *     listeners bound to specific interfaces.
 *
 * Revision 1.82  2000/09/22 01:35:02  robertj
 * Added support for handling LID's that only do symmetric codecs.
 *
 * Revision 1.81  2000/09/20 01:50:22  craigs
 * Added ability to set jitter buffer on a per-connection basis
 *
 * Revision 1.80  2000/09/05 01:16:19  robertj
 * Added "security" call end reason code.
 *
 * Revision 1.79  2000/09/01 02:12:54  robertj
 * Added ability to select a gatekeeper on LAN via it's identifier name.
 *
 * Revision 1.78  2000/07/31 14:08:09  robertj
 * Added fast start and H.245 tunneling flags to the H323Connection constructor so can
 *    disabled these features in easier manner to overriding virtuals.
 *
 * Revision 1.77  2000/07/13 12:33:38  robertj
 * Split autoStartVideo so can select receive and transmit independently
 *
 * Revision 1.76  2000/07/04 04:14:06  robertj
 * Fixed capability check of "combinations" for fast start cases.
 *
 * Revision 1.75  2000/07/04 01:16:49  robertj
 * Added check for capability allowed in "combinations" set, still needs more done yet.
 *
 * Revision 1.74  2000/06/29 10:59:53  robertj
 * Added user interface for sound buffer depth adjustment.
 *
 * Revision 1.73  2000/06/23 02:48:23  robertj
 * Added ability to adjust sound channel buffer depth, needed increasing under Win32.
 *
 * Revision 1.72  2000/06/21 08:07:38  robertj
 * Added cause/reason to release complete PDU, where relevent.
 *
 * Revision 1.71  2000/06/10 02:03:36  robertj
 * Fixed typo in comment
 *
 * Revision 1.70  2000/06/07 05:47:55  robertj
 * Added call forwarding.
 *
 * Revision 1.69  2000/06/05 06:33:08  robertj
 * Fixed problem with roud trip time statistic not being calculated if constant traffic.
 *
 * Revision 1.68  2000/05/23 12:57:28  robertj
 * Added ability to change IP Type Of Service code from applications.
 *
 * Revision 1.67  2000/05/23 11:32:26  robertj
 * Rewrite of capability table to combine 2 structures into one and move functionality into that class
 *    allowing some normalisation of usage across several applications.
 * Changed H323Connection so gets a copy of capabilities instead of using endponts, allows adjustments
 *    to be done depending on the remote client application.
 *
 * Revision 1.66  2000/05/18 11:53:33  robertj
 * Changes to support doc++ documentation generation.
 *
 * Revision 1.65  2000/05/16 08:12:37  robertj
 * Added documentation for FindChannel() function.
 * Added function to get a logical channel by channel number.
 *
 * Revision 1.64  2000/05/16 02:06:00  craigs
 * Added access functions for particular sessions
 *
 * Revision 1.63  2000/05/09 12:19:22  robertj
 * Added ability to get and set "distinctive ring" Q.931 functionality.
 *
 * Revision 1.62  2000/05/08 14:07:26  robertj
 * Improved the provision and detection of calling and caller numbers, aliases and hostnames.
 *
 * Revision 1.61  2000/05/02 04:32:24  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.60  2000/05/01 13:00:09  robertj
 * Changed SetCapability() to append capabilities to TCS, helps with assuring no gaps in set.
 *
 * Revision 1.59  2000/04/14 21:08:53  robertj
 * Work around for compatibility problem wth broken Altigen AltaServ-IG PBX.
 *
 * Revision 1.58  2000/04/14 20:01:38  robertj
 * Added function to get remote endpoints application name.
 *
 * Revision 1.57  2000/04/11 04:02:47  robertj
 * Improved call initiation with gatekeeper, no longer require @address as
 *    will default to gk alias if no @ and registered with gk.
 * Added new call end reasons for gatekeeper denied calls.
 *
 * Revision 1.56  2000/04/10 20:02:49  robertj
 * Added support for more sophisticated DTMF and hook flash user indication.
 *
 * Revision 1.55  2000/04/06 17:50:15  robertj
 * Added auto-start (including fast start) of video channels, selectable via boolean on the endpoint.
 *
 * Revision 1.54  2000/04/05 03:17:30  robertj
 * Added more RTP statistics gathering and H.245 round trip delay calculation.
 *
 * Revision 1.53  2000/03/29 04:32:55  robertj
 * Improved some trace logging messages.
 *
 * Revision 1.52  2000/03/29 02:12:38  robertj
 * Changed TerminationReason to CallEndReason to use correct telephony nomenclature.
 * Added CallEndReason for capability exchange failure.
 *
 * Revision 1.51  2000/03/25 02:03:18  robertj
 * Added default transport for gatekeeper to be UDP.
 *
 * Revision 1.50  2000/03/23 02:44:49  robertj
 * Changed ClearAllCalls() so will wait for calls to be closed (usefull in endpoint dtors).
 *
 * Revision 1.49  2000/03/02 02:18:13  robertj
 * Further fixes for early H245 establishment confusing the fast start code.
 *
 * Revision 1.48  2000/01/07 08:19:14  robertj
 * Added status functions for connection and tidied up the answer call function
 *
 * Revision 1.47  2000/01/04 00:14:26  craigs
 * Added additional states to AnswerCall callback
 *
 * Revision 1.46  1999/12/23 23:02:34  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 * Revision 1.45  1999/12/11 02:20:53  robertj
 * Added ability to have multiple aliases on local endpoint.
 *
 * Revision 1.44  1999/11/22 10:07:23  robertj
 * Fixed some errors in correct termination states.
 *
 * Revision 1.43  1999/11/19 08:15:41  craigs
 * Added connectionStartTime
 *
 * Revision 1.42  1999/11/17 00:01:11  robertj
 * Improved determination of caller name, thanks Ian MacDonald
 *
 * Revision 1.41  1999/11/10 23:29:37  robertj
 * Changed OnAnswerCall() call back function  to allow for asyncronous response.
 *
 * Revision 1.40  1999/11/06 05:37:44  robertj
 * Complete rewrite of termination of connection to avoid numerous race conditions.
 *
 * Revision 1.39  1999/10/30 12:34:13  robertj
 * Added information callback for closed logical channel on H323EndPoint.
 *
 * Revision 1.38  1999/10/29 02:26:13  robertj
 * Added reason for termination code to H323Connection.
 *
 * Revision 1.37  1999/10/19 00:03:20  robertj
 * Changed OpenAudioChannel and OpenVideoChannel to allow a codec AttachChannel with no autodelete.
 * Added function to set initial bandwidth limit on a new connection.
 *
 * Revision 1.36  1999/10/14 12:05:03  robertj
 * Fixed deadlock possibilities in clearing calls.
 *
 * Revision 1.35  1999/10/09 01:19:07  craigs
 * Added codecs to OpenAudioChannel and OpenVideoDevice functions
 *
 * Revision 1.34  1999/09/23 07:25:12  robertj
 * Added open audio and video function to connection and started multi-frame codec send functionality.
 *
 * Revision 1.33  1999/09/21 14:24:34  robertj
 * Changed SetCapability() so automatically calls AddCapability().
 *
 * Revision 1.32  1999/09/21 14:05:21  robertj
 * Fixed incorrect PTRACING test and removed uneeded include of videoio.h
 *
 * Revision 1.31  1999/09/21 08:29:13  craigs
 * Added support for video codecs and H261
 *
 * Revision 1.30  1999/09/15 01:26:27  robertj
 * Changed capability set call backs to have more specific class as parameter.
 *
 * Revision 1.29  1999/09/14 06:52:54  robertj
 * Added better support for multi-homed client hosts.
 *
 * Revision 1.28  1999/09/13 14:23:11  robertj
 * Changed MakeCall() function return value to be something useful.
 *
 * Revision 1.27  1999/09/10 03:36:47  robertj
 * Added simple Q.931 Status response to Q.931 Status Enquiry
 *
 * Revision 1.26  1999/09/08 04:05:48  robertj
 * Added support for video capabilities & codec, still needs the actual codec itself!
 *
 * Revision 1.25  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 * Revision 1.24  1999/08/27 09:46:05  robertj
 * Added sepearte function to initialise vendor information from endpoint.
 *
 * Revision 1.23  1999/08/25 05:14:49  robertj
 * File fission (critical mass reached).
 * Improved way in which remote capabilities are created, removed case statement!
 * Changed MakeCall, so immediately spawns thread, no black on TCP connect.
 *
 * Revision 1.22  1999/08/08 10:02:49  robertj
 * Added access functions to remote capability table.
 *
 * Revision 1.21  1999/07/23 02:37:53  robertj
 * Fixed problems with hang ups and crash closes of connections.
 *
 * Revision 1.20  1999/07/18 14:29:31  robertj
 * Fixed bugs in slow start with H245 tunnelling, part 3.
 *
 * Revision 1.19  1999/07/16 14:05:10  robertj
 * Added application level jitter buffer adjustment.
 *
 * Revision 1.18  1999/07/16 06:15:59  robertj
 * Corrected semantics for tunnelled master/slave determination in fast start.
 *
 * Revision 1.17  1999/07/15 14:45:35  robertj
 * Added propagation of codec open error to shut down logical channel.
 * Fixed control channel start up bug introduced with tunnelling.
 *
 * Revision 1.16  1999/07/14 06:06:13  robertj
 * Fixed termination problems (race conditions) with deleting connection object.
 *
 * Revision 1.15  1999/07/13 02:50:58  craigs
 * Changed semantics of SetPlayDevice/SetRecordDevice, only descendent
 *    endpoint assumes PSoundChannel devices for audio codec.
 *
 * Revision 1.14  1999/07/10 02:59:26  robertj
 * Fixed ability to hang up incoming connection.
 *
 * Revision 1.13  1999/07/09 06:09:49  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.12  1999/06/25 10:25:35  robertj
 * Added maintentance of callIdentifier variable in H.225 channel.
 *
 * Revision 1.11  1999/06/22 13:45:05  robertj
 * Added user question on listener version to accept incoming calls.
 *
 * Revision 1.10  1999/06/14 05:15:56  robertj
 * Changes for using RTP sessions correctly in H323 Logical Channel context
 *
 * Revision 1.9  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.8  1999/06/09 06:18:00  robertj
 * GCC compatibiltiy.
 *
 * Revision 1.7  1999/06/09 05:26:19  robertj
 * Major restructuring of classes.
 *
 * Revision 1.6  1999/06/06 06:06:36  robertj
 * Changes for new ASN compiler and v2 protocol ASN files.
 *
 * Revision 1.5  1999/04/26 06:14:46  craigs
 * Initial implementation for RTP decoding and lots of stuff
 * As a whole, these changes are called "First Noise"
 *
 * Revision 1.4  1999/02/23 11:04:28  robertj
 * Added capability to make outgoing call.
 *
 * Revision 1.3  1999/01/16 01:31:34  robertj
 * Major implementation.
 *
 * Revision 1.2  1999/01/02 04:00:59  robertj
 * Added higher level protocol negotiations.
 *
 * Revision 1.1  1998/12/14 09:13:12  robertj
 * Initial revision
 *
 */

#include "openh323buildopts.h"

#include "h323con.h"
#include "h323ep.h"
#include "gkclient.h"

#include <h323pluginmgr.h>

PString  OpalGetVersion();
unsigned OpalGetMajorVersion();
unsigned OpalGetMinorVersion();
unsigned OpalGetBuildNumber();


/////////////////////////////////////////////////////////////////////////////
