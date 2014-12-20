/*
 * h323ep.cxx
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
 * $Log: h323ep.cxx,v $
 * Revision 1.15  2008/02/10 23:11:33  shorne
 * Fix to compile H323plus without Video
 *
 * Revision 1.14  2008/02/01 09:34:20  shorne
 * Cleaner shutdown of GnuGk NAT support
 *
 * Revision 1.13  2008/01/30 02:10:19  shorne
 * Changed ASN.1 vendor information from OpenH323 to H323plus
 *
 * Revision 1.12  2008/01/29 04:28:55  shorne
 * Initialise gnugk as NULL
 *
 * Revision 1.11  2008/01/04 22:53:40  shorne
 * Increased default soundBuffers to 10 for Vista
 *
 * Revision 1.10  2008/01/02 19:37:45  willamowius
 * add newline at eof
 *
 * Revision 1.9  2008/01/01 00:16:12  shorne
 * Added GnuGknat and FileTransfer support
 *
 * Revision 1.8  2007/11/20 11:40:47  willamowius
 * fix compilation without audio support Thanks Vladimir Voronin
 *
 * Revision 1.7  2007/11/16 22:09:43  shorne
 * Added ability to disable H.245 QoS for NetMeeting Interop
 *
 * Revision 1.6  2007/11/01 20:17:33  shorne
 * updates for H.239 support
 *
 * Revision 1.5  2007/10/25 21:08:04  shorne
 * Added support for HD Video devices
 *
 * Revision 1.4  2007/10/22 08:24:08  shorne
 * Fixed small bug
 *
 * Revision 1.3  2007/08/20 19:13:28  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.2  2007/08/08 20:13:10  shorne
 * Increased default Buffers to 5
 *
 * Revision 1.1  2007/08/06 20:51:06  shorne
 * First commit of h323plus
 *
 * Revision 1.228.2.9  2007/07/20 22:03:27  shorne
 * Initial H.350 Support
 *
 * Revision 1.228.2.8  2007/05/23 07:04:56  shorne
 * Fix for ENUM/SRV support
 *
 * Revision 1.228.2.7  2007/05/01 01:17:52  shorne
 * Fix for NAT support disablement for same NAT
 *
 * Revision 1.228.2.6  2007/04/19 12:16:16  shorne
 * added ability to detect if no nat
 *
 * Revision 1.228.2.5  2007/02/19 20:11:06  shorne
 * Added Baseline H.239 Support
 *
 * Revision 1.228.2.4  2007/02/11 00:45:20  shorne
 * Added ability to disable NAT method on a call by call basis
 *
 * Revision 1.228.2.3  2007/02/02 22:12:44  shorne
 * Added ability to set FrameSize for video plugins
 *
 * Revision 1.228.2.2  2007/01/30 01:07:41  shorne
 * Added ability to disable H460
 *
 * Revision 1.228.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.228  2006/08/10 04:05:03  csoutheren
 * Apply 1537305 - Fix compile problems on gcc 4.1
 * Thanks to Stanislav Brabec
 *
 * Revision 1.227  2006/06/26 02:52:51  shorne
 * Moved H460 feature loader from H323EndPoint Constructor
 *
 * Revision 1.226  2006/06/23 20:01:29  shorne
 * More H460 support
 *
 * Revision 1.225  2006/06/23 07:22:40  csoutheren
 * Fixed compile when H.224 disabled
 *
 * Revision 1.224  2006/06/23 06:02:44  csoutheren
 * Added missing declarations for H.224 backport
 *
 * Revision 1.223  2006/06/21 05:11:48  csoutheren
 * Fixed build with latest PWLib
 *
 * Revision 1.222  2006/06/09 06:30:12  csoutheren
 * Remove compile warning and errors with gcc
 *
 * Revision 1.221  2006/05/30 11:14:56  hfriederich
 * Switch from DISABLE_H460 to H323_H460
 *
 * Revision 1.220  2006/05/16 11:44:09  shorne
 * extended DNS SRV, H460 Feature , Call Credit, extend conference goals
 *
 * Revision 1.219  2006/03/07 10:37:46  csoutheren
 * Add ability to disable GRQ on GK registration
 *
 * Revision 1.218  2006/03/02 10:08:35  shorne
 * Corrected Caller Authentcation & added more NAT traversal debug code
 *
 * Revision 1.217  2006/02/26 14:07:53  shorne
 * Updated for function name change in PDNS
 *
 * Revision 1.216  2006/02/26 11:00:57  shorne
 * Added DNS SRV record lookups to ParseParty
 *
 * Revision 1.215  2006/01/30 00:51:06  csoutheren
 * Fixed compile problem on Linux
 *
 * Revision 1.214  2006/01/27 07:53:40  csoutheren
 * Fixed for signalling aggregation
 *
 * Revision 1.213  2006/01/26 03:47:18  shorne
 * Caller Authentication, more Nat Traversal support, more PBX support
 *
 * Revision 1.212  2006/01/24 08:15:24  csoutheren
 * Implement outgoing H.225 aggregation, and H.245 aggregation (disabled by default)
 * More testing to do, but this looks good :)
 *
 * Revision 1.211  2006/01/20 00:32:24  csoutheren
 * First check-in of signalling aggregation code - incomplete and disabled by default
 *
 * Revision 1.210  2006/01/18 07:46:08  csoutheren
 * Initial version of RTP aggregation (disabled by default)
 *
 * Revision 1.209  2005/11/25 03:42:04  csoutheren
 * Added fix for bug #1326612
 * H323EndPoint::OpenAudioChannel bug
 *
 * Revision 1.208  2005/11/22 03:38:45  shorne
 * Added ToS support to TCP Transports. thx Norbert Bartalsky (TOPCALL)
 *
 * Revision 1.207  2005/11/21 20:54:43  shorne
 * Added GnuGK Nat detection support / ParseParrty returns FALSE if no ENUM servers found.
 *  .
 *
 * Revision 1.206  2005/09/16 08:12:50  csoutheren
 * Added ability to set timeout for connect
 *
 * Revision 1.205  2005/08/04 19:43:20  csoutheren
 * Applied patch #1240787
 * Fixed problem when disabling H.450
 * Thanks to Boris Pavacic
 *
 * Revision 1.204  2005/07/12 12:28:55  csoutheren
 * Fixes for H.450 errors and return values
 * Thanks to Iker Perez San Roman
 *
 * Revision 1.203  2005/06/07 00:22:24  csoutheren
 * Added patch 1198111 to allow disabling the audio jitter buffer by calling
 * H323EndPoint::SetAudioJitterDelay(0, 0). Thanks to Michael Manousos
 *
 * Revision 1.202  2005/05/03 12:22:55  csoutheren
 * Unlock connection list when creating connection
 * Remove chance of race condition with H.245 negotiation timer
 * Unregister OpalPluginMediaFormat when instance is destroyed
 * Thank to Paul Cadach
 *
 * Revision 1.201  2005/03/10 07:01:29  csoutheren
 * Fixed problem with H.450 call identifiers not being unique across all calls on an
 *  endpoint. Thanks to Thien Nguyen
 *
 * Revision 1.200  2005/03/07 23:46:25  csoutheren
 * Fixed problem with namespace of ETSI OSP token
 *
 * Revision 1.199  2005/01/21 21:26:50  csoutheren
 * Fixed problem compiling with audio and without sound driver support
 *
 * Revision 1.198  2005/01/16 20:39:44  csoutheren
 * Fixed problem with IPv6 INADDR_ANY
 *
 * Revision 1.197  2005/01/04 08:08:45  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.196  2005/01/03 14:03:42  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.195  2005/01/03 06:25:55  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.194  2004/12/23 22:27:58  csoutheren
 * Fixed problem with not using specified port when attempting to resolve URLs
 * hostnames using DNS
 *
 * Revision 1.193  2004/12/20 02:32:35  csoutheren
 * Cleeaned up OSP functions
 *
 * Revision 1.192  2004/12/09 23:38:40  csoutheren
 * More OSP implementation
 *
 * Revision 1.191  2004/12/08 05:16:14  csoutheren
 * Fixed OSP compilation on Linux
 *
 * Revision 1.190  2004/12/08 01:59:23  csoutheren
 * initial support for Transnexus OSP toolkit
 *
 * Revision 1.189  2004/11/30 00:16:54  csoutheren
 * Don' t convert userIndication::signalUpdate messages into UserInputString messages
 *
 * Revision 1.188  2004/11/29 23:44:21  csoutheren
 * Fixed type on TranslateTCPAddress
 *
 * Revision 1.187  2004/11/29 06:30:54  csoutheren
 * Added support for wideband codecs
 *
 * Revision 1.186  2004/11/25 07:38:58  csoutheren
 * Ensured that external TCP address translation is performed when using STUN to handle UDP
 *
 * Revision 1.185  2004/11/20 22:00:49  csoutheren
 * Added hacks for linker problem
 *
 * Revision 1.184  2004/11/12 06:04:44  csoutheren
 * Changed H235Authentiators to use PFactory
 *
 * Revision 1.183  2004/10/16 03:07:52  rjongbloed
 * Fixed correct detection of when to use STUN, this does not include the local host!
 *
 * Revision 1.182  2004/09/03 01:06:10  csoutheren
 * Added initial hooks for H.460 GEF
 * Thanks to Simon Horne and ISVO (Asia) Pte Ltd. for this contribution
 *
 * Revision 1.181  2004/08/26 08:05:04  csoutheren
 * Codecs now appear in abstract factory system
 * Fixed Windows factory bootstrap system (again)
 *
 * Revision 1.180  2004/08/04 10:44:10  csoutheren
 * More guards when testing for resolution via ENUM
 *
 * Revision 1.179  2004/08/03 23:06:27  csoutheren
 * Disabled ENUM when calling an IP address
 *
 * Revision 1.178  2004/08/03 13:38:56  csoutheren
 * Added support for ENUM when no GK is configured
 *
 * Revision 1.177  2004/07/27 05:28:45  csoutheren
 * Added ability to set priority of channel threads
 *
 * Revision 1.176  2004/07/03 06:51:37  rjongbloed
 * Added PTRACE_PARAM() macro to fix warnings on parameters used in PTRACE
 *  macros only.
 *
 * Revision 1.175  2004/06/15 03:30:01  csoutheren
 * Added OnSendARQ to allow access to the ARQ message before sent by connection
 *
 * Revision 1.174  2004/06/01 05:48:03  csoutheren
 * Changed capability table to use abstract factory routines rather than internal linked list
 *
 * Revision 1.173  2004/05/18 06:03:45  csoutheren
 * Removed warnings under Windows
 *
 * Revision 1.172  2004/05/17 12:14:25  csoutheren
 * Added support for different SETUP PDU types
 *
 * Revision 1.171  2004/04/07 05:31:43  csoutheren
 * Added ability to receive calls from endpoints behind NAT firewalls
 *
 * Revision 1.170  2004/01/26 11:42:36  rjongbloed
 * Added pass through of port numbers to STUN client
 *
 * Revision 1.169  2003/12/29 04:59:25  csoutheren
 * Added callbacks on H323EndPoint when gatekeeper discovery succeeds or fails
 *
 * Revision 1.168  2003/12/28 02:37:49  csoutheren
 * Added H323EndPoint::OnOutgoingCall
 *
 * Revision 1.167  2003/12/28 00:06:51  csoutheren
 * Added callbacks on H323EndPoint when gatekeeper registration succeeds or fails
 *
 * Revision 1.166  2003/04/28 09:01:15  robertj
 * Fixed problem with backward compatibility with non-url based remote
 *   addresses passed to MakeCall()
 *
 * Revision 1.165  2003/04/24 01:49:33  dereks
 * Add ability to set no media timeout interval
 *
 * Revision 1.164  2003/04/10 09:41:26  robertj
 * Added associated transport to new GetInterfaceAddresses() function so
 *   interfaces can be ordered according to active transport links. Improves
 *   interoperability.
 *
 * Revision 1.163  2003/04/10 01:01:56  craigs
 * Added functions to access to lists of interfaces
 *
 * Revision 1.162  2003/04/07 13:09:30  robertj
 * Added ILS support to callto URL parsing in MakeCall(), ie can now call hosts
 *   registered with an ILS directory.
 *
 * Revision 1.161  2003/04/07 11:11:45  craigs
 * Fixed compile problem on Linux
 *
 * Revision 1.160  2003/04/04 08:04:35  robertj
 * Added support for URL's in MakeCall, especially h323 and callto schemes.
 *
 * Revision 1.159  2003/03/04 03:58:32  robertj
 * Fixed missing local interface usage if specified in UseGatekeeper()
 *
 * Revision 1.158  2003/02/28 09:00:37  rogerh
 * remove redundant code
 *
 * Revision 1.157  2003/02/25 23:51:49  robertj
 * Fxied bug where not getting last port in range, thanks Sonya Cooper-Hull
 *
 * Revision 1.156  2003/02/09 00:48:09  robertj
 * Added function to return if registered with gatekeeper.
 *
 * Revision 1.155  2003/02/05 06:32:10  robertj
 * Fixed non-stun symmetric NAT support recently broken.
 *
 * Revision 1.154  2003/02/05 04:56:35  robertj
 * Fixed setting STUN server to enpty string clearing stun variable.
 *
 * Revision 1.153  2003/02/04 07:06:41  robertj
 * Added STUN support.
 *
 * Revision 1.152  2003/02/01 13:31:22  robertj
 * Changes to support CAT authentication in RAS.
 *
 * Revision 1.151  2003/01/26 05:57:29  robertj
 * Changed ParsePartyName so will accept addresses of the form
 *   alias@gk:address which will do an LRQ call to "address" using "alias"
 *   to determine the IP address to connect to.
 *
 * Revision 1.150  2003/01/23 02:36:32  robertj
 * Increased (and made configurable) timeout for H.245 channel TCP connection.
 *
 * Revision 1.149  2003/01/06 06:13:37  robertj
 * Increased maximum possible jitter configuration to 10 seconds.
 *
 * Revision 1.148  2002/11/27 06:54:57  robertj
 * Added Service Control Session management as per Annex K/H.323 via RAS
 *   only at this stage.
 * Added H.248 ASN and very primitive infrastructure for linking into the
 *   Service Control Session management system.
 * Added basic infrastructure for Annex K/H.323 HTTP transport system.
 * Added Call Credit Service Control to display account balances.
 *
 * Revision 1.147  2002/11/19 07:07:44  robertj
 * Changed priority so H.235 standard authentication used by preference.
 *
 * Revision 1.146  2002/11/15 06:53:24  robertj
 * Fixed non facility redirect calls being able to be cleared!
 *
 * Revision 1.145  2002/11/15 05:17:26  robertj
 * Added facility redirect support without changing the call token for access
 *   to the call. If it gets redirected a new H323Connection object is
 *   created but it looks like the same thing to an application.
 *
 * Revision 1.144  2002/11/14 22:06:08  robertj
 * Increased default maximum number of ports.
 *
 * Revision 1.143  2002/11/10 08:10:43  robertj
 * Moved constants for "well known" ports to better place (OPAL change).
 *
 * Revision 1.142  2002/10/31 00:42:41  robertj
 * Enhanced jitter buffer system so operates dynamically between minimum and
 *   maximum values. Altered API to assure app writers note the change!
 *
 * Revision 1.141  2002/10/24 07:18:24  robertj
 * Changed gatekeeper call so if can do local DNS lookup or using IP address
 *   then uses destCallSignalAddress for ARQ instead of destinationInfo.
 *
 * Revision 1.140  2002/10/23 06:06:13  robertj
 * Added function to be smarter in using a gatekeeper for use by endpoint.
 *
 * Revision 1.139  2002/10/01 06:38:36  robertj
 * Removed GNU compiler warning
 *
 * Revision 1.138  2002/10/01 03:07:15  robertj
 * Added version number functions for OpenH323 library itself, plus included
 *   library version in the default vendor information.
 *
 * Revision 1.137  2002/08/15 04:56:56  robertj
 * Fixed operation of ports system assuring RTP is even numbers.
 *
 * Revision 1.136  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.135  2002/07/19 11:25:10  robertj
 * Added extra trace on attempt to clear non existent call.
 *
 * Revision 1.134  2002/07/19 03:39:22  robertj
 * Bullet proofed setting of RTP IP port base, can't be zero!
 *
 * Revision 1.133  2002/07/18 01:50:14  robertj
 * Changed port secltion code to force apps to use function interface.
 *
 * Revision 1.132  2002/07/04 00:11:25  robertj
 * Fixed setting of gk password when password changed in endpoint.
 *
 * Revision 1.131  2002/06/22 05:48:42  robertj
 * Added partial implementation for H.450.11 Call Intrusion
 *
 * Revision 1.130  2002/06/15 03:06:46  robertj
 * Fixed locking of connection on H.250.2 transfer, thanks Gilles Delcayre
 *
 * Revision 1.129  2002/06/13 06:15:20  robertj
 * Allowed TransferCall() to be used on H323Connection as well as H323EndPoint.
 *
 * Revision 1.128  2002/06/12 03:55:21  robertj
 * Added function to add/remove multiple listeners in one go comparing against
 *   what is already running so does not interrupt unchanged listeners.
 *
 * Revision 1.127  2002/05/29 06:40:33  robertj
 * Changed sending of endSession/ReleaseComplete PDU's to occur immediately
 *   on call clearance and not wait for background thread to do it.
 * Stricter compliance by waiting for reply endSession before closing down.
 *
 * Revision 1.126  2002/05/28 06:16:12  robertj
 * Split UDP (for RAS) from RTP port bases.
 * Added current port variable so cycles around the port range specified which
 *   fixes some wierd problems on some platforms, thanks Federico Pinna
 *
 * Revision 1.125  2002/05/17 03:39:01  robertj
 * Fixed problems with H.235 authentication on RAS for server and client.
 *
 * Revision 1.124  2002/05/15 23:57:49  robertj
 * Added function to get the tokens for all active calls.
 * Fixed setting of password info in gatekeeper so is before discovery.
 *
 * Revision 1.123  2002/05/02 07:56:28  robertj
 * Added automatic clearing of call if no media (RTP data) is transferred in a
 *   configurable (default 5 minutes) amount of time.
 *
 * Revision 1.122  2002/04/18 01:40:56  robertj
 * Fixed bad variable name for disabling DTMF detection, very confusing.
 *
 * Revision 1.121  2002/04/17 00:50:57  robertj
 * Added ability to disable the in band DTMF detection.
 *
 * Revision 1.120  2002/04/10 08:09:03  robertj
 * Allowed zero TCP ports
 *
 * Revision 1.119  2002/04/10 06:50:08  robertj
 * Added functions to set port member variables.
 *
 * Revision 1.118  2002/04/01 21:32:24  robertj
 * Fixed problem with busy loop on Solaris, thanks chad@broadmind.com
 *
 * Revision 1.117  2002/02/04 07:17:56  robertj
 * Added H.450.2 Consultation Transfer, thanks Norwood Systems.
 *
 * Revision 1.116  2002/01/24 06:29:06  robertj
 * Added option to disable H.245 negotiation in SETUP pdu, this required
 *   API change so have a bit mask instead of a series of booleans.
 *
 * Revision 1.115  2002/01/17 07:05:04  robertj
 * Added support for RFC2833 embedded DTMF in the RTP stream.
 *
 * Revision 1.114  2002/01/14 00:00:04  robertj
 * Added CallTransfer timeouts to endpoint, hanks Ben Madsen of Norwood Systems.
 *
 * Revision 1.113  2002/01/08 04:45:04  robertj
 * Added MakeCallLocked() so can start a call with the H323Connection instance
 *   initally locked so can do things to it before the call really starts.
 *
 * Revision 1.112  2001/12/22 03:20:05  robertj
 * Added create protocol function to H323Connection.
 *
 * Revision 1.111  2001/12/14 08:36:36  robertj
 * More implementation of T.38, thanks Adam Lazur
 *
 * Revision 1.110  2001/12/13 11:01:37  robertj
 * Fixed missing initialisation of auto fax start variables.
 *
 * Revision 1.109  2001/11/01 06:11:57  robertj
 * Plugged very small mutex hole that could cause crashes.
 *
 * Revision 1.108  2001/11/01 00:59:07  robertj
 * Added auto setting of silence detect mode when using PSoundChannel codec.
 *
 * Revision 1.107  2001/11/01 00:27:35  robertj
 * Added default Fast Start disabled and H.245 tunneling disable flags
 *   to the endpoint instance.
 *
 * Revision 1.106  2001/10/30 07:34:33  robertj
 * Added trace for when cleaner thread start, stops and runs
 *
 * Revision 1.105  2001/10/24 07:25:59  robertj
 * Fixed possible deadlocks during destruction of H323Connection object.
 *
 * Revision 1.104  2001/09/26 06:20:59  robertj
 * Fixed properly nesting connection locking and unlocking requiring a quite
 *   large change to teh implementation of how calls are answered.
 *
 * Revision 1.103  2001/09/13 02:41:21  robertj
 * Fixed call reference generation to use full range and common code.
 *
 * Revision 1.102  2001/09/11 01:24:36  robertj
 * Added conditional compilation to remove video and/or audio codecs.
 *
 * Revision 1.101  2001/09/11 00:21:23  robertj
 * Fixed missing stack sizes in endpoint for cleaner thread and jitter thread.
 *
 * Revision 1.100  2001/08/24 13:38:25  rogerh
 * Free the locally declared listener.
 *
 * Revision 1.99  2001/08/24 13:23:59  rogerh
 * Undo a recent memory leak fix. The listener object has to be deleted in the
 * user application as they use the listener when StartListener() fails.
 * Add a Resume() if StartListener() fails so user applications can delete
 * the suspended thread.
 *
 * Revision 1.98  2001/08/22 06:54:51  robertj
 * Changed connection locking to use double mutex to guarantee that
 *   no threads can ever deadlock or access deleted connection.
 *
 * Revision 1.97  2001/08/16 07:49:19  robertj
 * Changed the H.450 support to be more extensible. Protocol handlers
 *   are now in separate classes instead of all in H323Connection.
 *
 * Revision 1.96  2001/08/10 11:03:52  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 * Revision 1.95  2001/08/08 23:55:27  robertj
 * Fixed problem with setting gk password before have a gk variable.
 * Fixed memory leak if listener fails to open.
 * Fixed problem with being able to specify an alias with an @ in it.
 *
 * Revision 1.94  2001/08/06 07:44:55  robertj
 * Fixed problems with building without SSL
 *
 * Revision 1.93  2001/08/06 03:08:56  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.92  2001/08/02 04:31:48  robertj
 * Changed to still maintain gatekeeper link if GRQ succeeded but RRQ
 *   failed. Thanks Ben Madsen & Graeme Reid.
 *
 * Revision 1.91  2001/07/17 04:44:32  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 * Revision 1.90  2001/07/06 07:28:55  robertj
 * Fixed rearranged connection creation to avoid possible nested mutex.
 *
 * Revision 1.89  2001/07/06 04:46:19  robertj
 * Rearranged connection creation to avoid possible nested mutex.
 *
 * Revision 1.88  2001/06/19 08:43:38  robertj
 * Fixed deadlock if ClearCallSynchronous ever called from cleaner thread.
 *
 * Revision 1.87  2001/06/19 03:55:30  robertj
 * Added transport to CreateConnection() function so can use that as part of
 *   the connection creation decision making process.
 *
 * Revision 1.86  2001/06/15 00:55:53  robertj
 * Added thread name for cleaner thread
 *
 * Revision 1.85  2001/06/14 23:18:06  robertj
 * Change to allow for CreateConnection() to return NULL to abort call.
 *
 * Revision 1.84  2001/06/14 04:23:32  robertj
 * Changed incoming call to pass setup pdu to endpoint so it can create
 *   different connection subclasses depending on the pdu eg its alias
 *
 * Revision 1.83  2001/06/02 01:35:32  robertj
 * Added thread names.
 *
 * Revision 1.82  2001/05/30 11:13:54  robertj
 * Fixed possible deadlock when getting connection from endpoint.
 *
 * Revision 1.81  2001/05/14 05:56:28  robertj
 * Added H323 capability registration system so can add capabilities by
 *   string name instead of having to instantiate explicit classes.
 *
 * Revision 1.80  2001/05/01 04:34:11  robertj
 * Changed call transfer API slightly to be consistent with new hold function.
 *
 * Revision 1.79  2001/04/23 01:31:15  robertj
 * Improved the locking of connections especially at shutdown.
 *
 * Revision 1.78  2001/04/11 03:01:29  robertj
 * Added H.450.2 (call transfer), thanks a LOT to Graeme Reid & Norwood Systems
 *
 * Revision 1.77  2001/03/21 04:52:42  robertj
 * Added H.235 security to gatekeepers, thanks Fürbass Franz!
 *
 * Revision 1.76  2001/03/17 00:05:52  robertj
 * Fixed problems with Gatekeeper RIP handling.
 *
 * Revision 1.75  2001/03/15 00:24:47  robertj
 * Added function for setting gatekeeper with address and identifier values.
 *
 * Revision 1.74  2001/02/27 00:03:59  robertj
 * Fixed possible deadlock in FindConnectionsWithLock(), thanks Hans Andersen
 *
 * Revision 1.73  2001/02/16 04:11:35  robertj
 * Added ability for RemoveListener() to remove all listeners.
 *
 * Revision 1.72  2001/01/18 06:04:18  robertj
 * Bullet proofed code so local alias can not be empty string. This actually
 *   fixes an ASN PER encoding bug causing an assert.
 *
 * Revision 1.71  2001/01/11 02:19:15  craigs
 * Fixed problem with possible window of opportunity for ClearCallSynchronous
 * to return even though call has not been cleared
 *
 * Revision 1.70  2000/12/22 03:54:33  craigs
 * Fixed problem with listening fix
 *
 * Revision 1.69  2000/12/21 12:38:24  craigs
 * Fixed problem with "Listener thread did not terminate" assert
 * when listener port is in use
 *
 * Revision 1.68  2000/12/20 00:51:03  robertj
 * Fixed MSVC compatibility issues (No trace).
 *
 * Revision 1.67  2000/12/19 22:33:44  dereks
 * Adjust so that the video channel is used for reading/writing raw video
 * data, which better modularizes the video codec.
 *
 * Revision 1.66  2000/12/18 08:59:20  craigs
 * Added ability to set ports
 *
 * Revision 1.65  2000/12/18 01:22:28  robertj
 * Changed semantics or HasConnection() so returns TRUE until the connection
 *   has been deleted and not just until ClearCall() was executure on it.
 *
 * Revision 1.64  2000/11/27 02:44:06  craigs
 * Added ClearCall Synchronous to H323Connection and H323Endpoint to
 * avoid race conditions with destroying descendant classes
 *
 * Revision 1.63  2000/11/26 23:13:23  craigs
 * Added ability to pass user data to H323Connection constructor
 *
 * Revision 1.62  2000/11/12 23:49:16  craigs
 * Added per connection versions of OnEstablished and OnCleared
 *
 * Revision 1.61  2000/10/25 00:53:52  robertj
 * Used official manafacturer code for the OpenH323 project.
 *
 * Revision 1.60  2000/10/20 06:10:51  robertj
 * Fixed very small race condition on creating new connectionon incoming call.
 *
 * Revision 1.59  2000/10/19 04:07:50  robertj
 * Added function to be able to remove a listener.
 *
 * Revision 1.58  2000/10/04 12:21:07  robertj
 * Changed setting of callToken in H323Connection to be as early as possible.
 *
 * Revision 1.57  2000/09/25 12:59:34  robertj
 * Added StartListener() function that takes a H323TransportAddress to start
 *     listeners bound to specific interfaces.
 *
 * Revision 1.56  2000/09/14 23:03:45  robertj
 * Increased timeout on asserting because of driver lockup
 *
 * Revision 1.55  2000/09/01 02:13:05  robertj
 * Added ability to select a gatekeeper on LAN via it's identifier name.
 *
 * Revision 1.54  2000/08/30 05:37:44  robertj
 * Fixed bogus destCallSignalAddress in ARQ messages.
 *
 * Revision 1.53  2000/08/29 02:59:50  robertj
 * Added some debug output for channel open/close.
 *
 * Revision 1.52  2000/08/25 01:10:28  robertj
 * Added assert if various thrads ever fail to terminate.
 *
 * Revision 1.51  2000/07/13 12:34:41  robertj
 * Split autoStartVideo so can select receive and transmit independently
 *
 * Revision 1.50  2000/07/11 19:30:15  robertj
 * Fixed problem where failure to unregister from gatekeeper prevented new registration.
 *
 * Revision 1.49  2000/07/09 14:59:28  robertj
 * Fixed bug if using default transport (no '@') for address when gatekeeper present, used port 1719.
 *
 * Revision 1.48  2000/07/04 04:15:40  robertj
 * Fixed capability check of "combinations" for fast start cases.
 *
 * Revision 1.47  2000/07/04 01:16:50  robertj
 * Added check for capability allowed in "combinations" set, still needs more done yet.
 *
 * Revision 1.46  2000/06/29 11:00:04  robertj
 * Added user interface for sound buffer depth adjustment.
 *
 * Revision 1.45  2000/06/29 07:10:32  robertj
 * Fixed incorrect setting of default number of audio buffers on Win32 systems.
 *
 * Revision 1.44  2000/06/23 02:48:24  robertj
 * Added ability to adjust sound channel buffer depth, needed increasing under Win32.
 *
 * Revision 1.43  2000/06/20 02:38:28  robertj
 * Changed H323TransportAddress to default to IP.
 *
 * Revision 1.42  2000/06/17 09:13:06  robertj
 * Removed redundent line of code, thanks David Iodice.
 *
 * Revision 1.41  2000/06/07 05:48:06  robertj
 * Added call forwarding.
 *
 * Revision 1.40  2000/06/03 03:16:39  robertj
 * Fixed using the wrong capability table (should be connections) for some operations.
 *
 * Revision 1.39  2000/05/25 00:34:59  robertj
 * Changed default tos on Unix platforms to avoid needing to be root.
 *
 * Revision 1.38  2000/05/23 12:57:37  robertj
 * Added ability to change IP Type Of Service code from applications.
 *
 * Revision 1.37  2000/05/23 11:32:37  robertj
 * Rewrite of capability table to combine 2 structures into one and move functionality into that class
 *    allowing some normalisation of usage across several applications.
 * Changed H323Connection so gets a copy of capabilities instead of using endponts, allows adjustments
 *    to be done depending on the remote client application.
 *
 * Revision 1.36  2000/05/16 07:38:50  robertj
 * Added extra debug info indicating sound channel buffer size.
 *
 * Revision 1.35  2000/05/11 03:48:11  craigs
 * Moved SetBuffer command to fix audio delay
 *
 * Revision 1.34  2000/05/02 04:32:26  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.33  2000/05/01 13:00:28  robertj
 * Changed SetCapability() to append capabilities to TCS, helps with assuring no gaps in set.
 *
 * Revision 1.32  2000/04/30 03:58:37  robertj
 * Changed PSoundChannel to be only double bufferred, this is all that is needed with jitter bufferring.
 *
 * Revision 1.31  2000/04/11 04:02:48  robertj
 * Improved call initiation with gatekeeper, no longer require @address as
 *    will default to gk alias if no @ and registered with gk.
 * Added new call end reasons for gatekeeper denied calls.
 *
 * Revision 1.30  2000/04/10 20:37:33  robertj
 * Added support for more sophisticated DTMF and hook flash user indication.
 *
 * Revision 1.29  2000/04/06 17:50:17  robertj
 * Added auto-start (including fast start) of video channels, selectable via boolean on the endpoint.
 *
 * Revision 1.28  2000/04/05 03:17:31  robertj
 * Added more RTP statistics gathering and H.245 round trip delay calculation.
 *
 * Revision 1.27  2000/03/29 02:14:45  robertj
 * Changed TerminationReason to CallEndReason to use correct telephony nomenclature.
 * Added CallEndReason for capability exchange failure.
 *
 * Revision 1.26  2000/03/25 02:03:36  robertj
 * Added default transport for gatekeeper to be UDP.
 *
 * Revision 1.25  2000/03/23 02:45:29  robertj
 * Changed ClearAllCalls() so will wait for calls to be closed (usefull in endpoint dtors).
 *
 * Revision 1.24  2000/02/17 12:07:43  robertj
 * Used ne wPWLib random number generator after finding major problem in MSVC rand().
 *
 * Revision 1.23  2000/01/07 08:22:49  robertj
 * Added status functions for connection and added transport independent MakeCall
 *
 * Revision 1.22  1999/12/23 23:02:36  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 * Revision 1.21  1999/12/11 02:21:00  robertj
 * Added ability to have multiple aliases on local endpoint.
 *
 * Revision 1.20  1999/12/09 23:14:20  robertj
 * Fixed deadock in termination with gatekeeper removal.
 *
 * Revision 1.19  1999/11/19 08:07:27  robertj
 * Changed default jitter time to 50 milliseconds.
 *
 * Revision 1.18  1999/11/06 05:37:45  robertj
 * Complete rewrite of termination of connection to avoid numerous race conditions.
 *
 * Revision 1.17  1999/10/30 12:34:47  robertj
 * Added information callback for closed logical channel on H323EndPoint.
 *
 * Revision 1.16  1999/10/19 00:04:57  robertj
 * Changed OpenAudioChannel and OpenVideoChannel to allow a codec AttachChannel with no autodelete.
 *
 * Revision 1.15  1999/10/14 12:05:03  robertj
 * Fixed deadlock possibilities in clearing calls.
 *
 * Revision 1.14  1999/10/09 01:18:23  craigs
 * Added codecs to OpenAudioChannel and OpenVideoDevice functions
 *
 * Revision 1.13  1999/10/08 08:31:45  robertj
 * Fixed failure to adjust capability when startign channel
 *
 * Revision 1.12  1999/09/23 07:25:12  robertj
 * Added open audio and video function to connection and started multi-frame codec send functionality.
 *
 * Revision 1.11  1999/09/21 14:24:48  robertj
 * Changed SetCapability() so automatically calls AddCapability().
 *
 * Revision 1.10  1999/09/21 14:12:40  robertj
 * Removed warnings when no tracing enabled.
 *
 * Revision 1.9  1999/09/21 08:10:03  craigs
 * Added support for video devices and H261 codec
 *
 * Revision 1.8  1999/09/14 06:52:54  robertj
 * Added better support for multi-homed client hosts.
 *
 * Revision 1.7  1999/09/13 14:23:11  robertj
 * Changed MakeCall() function return value to be something useful.
 *
 * Revision 1.6  1999/09/10 02:55:36  robertj
 * Changed t35 country code to Australia (finally found magic number).
 *
 * Revision 1.5  1999/09/08 04:05:49  robertj
 * Added support for video capabilities & codec, still needs the actual codec itself!
 *
 * Revision 1.4  1999/08/31 12:34:19  robertj
 * Added gatekeeper support.
 *
 * Revision 1.3  1999/08/27 15:42:44  craigs
 * Fixed problem with local call tokens using ambiguous interface names, and connect timeouts not changing connection state
 *
 * Revision 1.2  1999/08/27 09:46:05  robertj
 * Added sepearte function to initialise vendor information from endpoint.
 *
 * Revision 1.1  1999/08/25 05:10:36  robertj
 * File fission (critical mass reached).
 * Improved way in which remote capabilities are created, removed case statement!
 * Changed MakeCall, so immediately spawns thread, no black on TCP connect.
 *
 */

#include <ptlib.h>
#include <ptlib/sound.h>

#ifdef __GNUC__
#pragma implementation "h323ep.h"
#endif

#include "openh323buildopts.h"

#include "h323ep.h"
#include "h323pdu.h"

#ifdef H323_H450
#include "h450/h450pdu.h"
#endif

#include "gkclient.h"

#ifdef H323_T38
#include "t38proto.h"
#endif

#include "../version.h"
#include "h323pluginmgr.h"

#include <ptlib/sound.h>
#include <ptclib/random.h>
#include <ptclib/pstun.h>
#include <ptclib/url.h>
#include <ptclib/pils.h>
#include <ptclib/enum.h>

#ifdef H323_H224
#include <h224handler.h>
#endif

#ifdef H323_FILE
#include "h323filetransfer.h"
#endif

#if defined(H323_RTP_AGGREGATE) || defined(H323_SIGNAL_AGGREGATE)
#include <ptclib/sockagg.h>
#endif

#ifndef IPTOS_PREC_CRITIC_ECP
#define IPTOS_PREC_CRITIC_ECP (5 << 5)
#endif

#ifndef IPTOS_LOWDELAY
#define IPTOS_LOWDELAY 0x10
#endif

#include "opalglobalstatics.cxx"

//////////////////////////////////////////////////////////////////////////////////////

BYTE H323EndPoint::defaultT35CountryCode    = 9; // Country code for Australia
BYTE H323EndPoint::defaultT35Extension      = 0;
WORD H323EndPoint::defaultManufacturerCode  = 61; // Allocated by Australian Communications Authority, Oct 2000;

//////////////////////////////////////////////////////////////////////////////////////

class H225CallThread : public PThread
{
  PCLASSINFO(H225CallThread, PThread)

  public:
    H225CallThread(H323EndPoint & endpoint,
                   H323Connection & connection,
                   H323Transport & transport,
                   const PString & alias,
                   const H323TransportAddress & address);

  protected:
    void Main();

    H323Connection     & connection;
    H323Transport      & transport;
    PString              alias;
    H323TransportAddress address;
#ifdef H323_SIGNAL_AGGREGATE
    BOOL                 useAggregator;
#endif
};


class H323ConnectionsCleaner : public PThread
{
  PCLASSINFO(H323ConnectionsCleaner, PThread)

  public:
    H323ConnectionsCleaner(H323EndPoint & endpoint);
    ~H323ConnectionsCleaner();

    void Signal() { wakeupFlag.Signal(); }

  protected:
    void Main();

    H323EndPoint & endpoint;
    BOOL           stopFlag;
    PSyncPoint     wakeupFlag;
};


#define new PNEW


/////////////////////////////////////////////////////////////////////////////

PString OpalGetVersion()
{
#define AlphaCode   "alpha"
#define BetaCode    "beta"
#define ReleaseCode "."

  return psprintf("%u.%u%s%u", MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER);
}


unsigned OpalGetMajorVersion()
{
  return MAJOR_VERSION;
}

unsigned OpalGetMinorVersion()
{
  return MINOR_VERSION;
}

unsigned OpalGetBuildNumber()
{
  return BUILD_NUMBER;
}



/////////////////////////////////////////////////////////////////////////////

H225CallThread::H225CallThread(H323EndPoint & endpoint,
                               H323Connection & c,
                               H323Transport & t,
                               const PString & a,
                               const H323TransportAddress & addr)
  : PThread(endpoint.GetSignallingThreadStackSize(),
            NoAutoDeleteThread,
            NormalPriority,
            "H225 Caller:%0x"),
    connection(c),
    transport(t),
    alias(a),
    address(addr)
{
#ifdef H323_SIGNAL_AGGREGATE
  useAggregator = endpoint.GetSignallingAggregator() != NULL;
  if (!useAggregator)
#endif
  {
    transport.AttachThread(this);
  }

  Resume();
}


void H225CallThread::Main()
{
  PTRACE(3, "H225\tStarted call thread");

  if (connection.Lock()) {
    H323Connection::CallEndReason reason = connection.SendSignalSetup(alias, address);

    // Special case, if we aborted the call then already will be unlocked
    if (reason != H323Connection::EndedByCallerAbort)
      connection.Unlock();

    // Check if had an error, clear call if so
    if (reason != H323Connection::NumCallEndReasons)
      connection.ClearCall(reason);
    else {
#ifdef H323_SIGNAL_AGGREGATE
      if (useAggregator) {
        connection.AggregateSignalChannel(&transport);
        SetAutoDelete(AutoDeleteThread);
        return;
      }
#endif
      connection.HandleSignallingChannel();
    }
  }
}


/////////////////////////////////////////////////////////////////////////////

H323ConnectionsCleaner::H323ConnectionsCleaner(H323EndPoint & ep)
  : PThread(ep.GetCleanerThreadStackSize(),
            NoAutoDeleteThread,
            NormalPriority,
            "H323 Cleaner"),
    endpoint(ep)
{
  Resume();
  stopFlag = FALSE;
}


H323ConnectionsCleaner::~H323ConnectionsCleaner()
{
  stopFlag = TRUE;
  wakeupFlag.Signal();
  PAssert(WaitForTermination(10000), "Cleaner thread did not terminate");
}


void H323ConnectionsCleaner::Main()
{
  PTRACE(3, "H323\tStarted cleaner thread");

  for (;;) {
    wakeupFlag.Wait();
    if (stopFlag)
      break;

    endpoint.CleanUpConnections();
  }

  PTRACE(3, "H323\tStopped cleaner thread");
}


/////////////////////////////////////////////////////////////////////////////

H323EndPoint::H323EndPoint()
  :
#ifdef P_AUDIO
    soundChannelPlayDevice(PSoundChannel::GetDefaultDevice(PSoundChannel::Player)),
    soundChannelRecordDevice(PSoundChannel::GetDefaultDevice(PSoundChannel::Recorder)),
#endif
    signallingChannelConnectTimeout(0, 10, 0), // seconds
    signallingChannelCallTimeout(0, 0, 1),  // Minutes
    controlChannelStartTimeout(0, 0, 2),    // Minutes
    endSessionTimeout(0, 10),               // Seconds
    masterSlaveDeterminationTimeout(0, 30), // Seconds
    capabilityExchangeTimeout(0, 30),       // Seconds
    logicalChannelTimeout(0, 30),           // Seconds
    requestModeTimeout(0, 30),              // Seconds
    roundTripDelayTimeout(0, 10),           // Seconds
    roundTripDelayRate(0, 0, 1),            // Minutes
    noMediaTimeout(0, 0, 5),                // Minutes
    gatekeeperRequestTimeout(0, 5),         // Seconds
    rasRequestTimeout(0, 3)                // Seconds

#ifdef H323_H450
    ,
    callTransferT1(0,10),                   // Seconds
    callTransferT2(0,10),                   // Seconds
    callTransferT3(0,10),                   // Seconds
    callTransferT4(0,10),                   // Seconds
    callIntrusionT1(0,30),                  // Seconds
    callIntrusionT2(0,30),                  // Seconds
    callIntrusionT3(0,30),                  // Seconds
    callIntrusionT4(0,30),                  // Seconds
    callIntrusionT5(0,10),                  // Seconds
    callIntrusionT6(0,10),                   // Seconds
    nextH450CallIdentity(0)
#endif

{
  PString username = PProcess::Current().GetUserName();
  if (username.IsEmpty())
    username = PProcess::Current().GetName() & "User";
  localAliasNames.AppendString(username);

#if defined(P_AUDIO) && defined(_WIN32)
  PString DefaultAudioDriver = "WindowsMultimedia";
  SetSoundChannelPlayDriver(DefaultAudioDriver);
  SetSoundChannelRecordDriver(DefaultAudioDriver);
#endif

#ifdef H323_VIDEO
  autoStartReceiveVideo = autoStartTransmitVideo = TRUE;

#ifdef H323_H239
  autoStartReceiveExtVideo = autoStartTransmitExtVideo = FALSE;
#endif
#endif

#ifdef H323_T38
  autoStartReceiveFax = autoStartTransmitFax = FALSE;
#endif

#ifdef H323_AUDIO_CODECS
  minAudioJitterDelay = 50;  // milliseconds
  maxAudioJitterDelay = 250; // milliseconds
#endif

  autoCallForward = TRUE;
  disableFastStart = TRUE;
  disableH245Tunneling = FALSE;
  disableH245inSetup = FALSE;
  disableH245QoS = FALSE;
  disableDetectInBandDTMF = FALSE;
  canDisplayAmountString = FALSE;
  canEnforceDurationLimit = TRUE;

#ifdef H323_H450
  callIntrusionProtectionLevel = 3; //H45011_CIProtectionLevel::e_fullProtection;
#endif

#ifdef H323_AUDIO_CODECS
  defaultSilenceDetection = H323AudioCodec::AdaptiveSilenceDetection;
#endif

  defaultSendUserInputMode = H323Connection::SendUserInputAsString;

  terminalType = e_TerminalOnly;
  initialBandwidth = 100000; // Standard 10base LAN in 100's of bits/sec
  clearCallOnRoundTripFail = FALSE;

  t35CountryCode   = defaultT35CountryCode;   // Country code for Australia
  t35Extension     = defaultT35Extension;
  manufacturerCode = defaultManufacturerCode; // Allocated by Australian Communications Authority, Oct 2000

  rtpIpPorts.current = rtpIpPorts.base = 5000;
  rtpIpPorts.max = 5999;

  // use dynamic port allocation by default
  tcpPorts.current = tcpPorts.base = tcpPorts.max = 0;
  udpPorts.current = udpPorts.base = udpPorts.max = 0;

#ifdef P_STUN
  stun = NULL;
  disableSTUNTranslate = FALSE;
#endif

#ifdef _WIN32

#  if defined(H323_AUDIO_CODECS) && defined(P_AUDIO)
     // Windows MultiMedia stuff seems to need greater depth due to enormous
     // latencies in its operation, need to use DirectSound maybe?
     // for Win2000 and XP you need 5, for Vista you need 10! so set to 10! -SH
     soundChannelBuffers = 10;
#  endif

  rtpIpTypeofService = IPTOS_PREC_CRITIC_ECP|IPTOS_LOWDELAY;

#else

#  ifdef H323_AUDIO_CODECS
#ifdef P_AUDIO
     // Should only need double buffering for Unix platforms
     soundChannelBuffers = 2;
#endif
#  endif

  // Don't use IPTOS_PREC_CRITIC_ECP on Unix platforms as then need to be root
  rtpIpTypeofService = IPTOS_LOWDELAY;

#endif
  tcpIpTypeofService = IPTOS_LOWDELAY;

  masterSlaveDeterminationRetries = 10;
  gatekeeperRequestRetries = 2;
  rasRequestRetries = 2;
  sendGRQ = TRUE;

  cleanerThreadStackSize    = 30000;
  listenerThreadStackSize   = 30000;
  signallingThreadStackSize = 30000;
  controlThreadStackSize    = 30000;
  logicalThreadStackSize    = 30000;
  rasThreadStackSize        = 30000;
  jitterThreadStackSize     = 30000;

#ifdef H323_SIGNAL_AGGREGATE
  signallingAggregationSize = 25;
  signallingAggregator = NULL;
#endif

#ifdef H323_RTP_AGGREGATE
  rtpAggregationSize        = 10;
  rtpAggregator = NULL;
#endif

  channelThreadPriority     = PThread::HighestPriority;

  gatekeeper = NULL;

  connectionsActive.DisallowDeleteObjects();

#ifdef H323_H450
  secondaryConnectionsActive.DisallowDeleteObjects();
#endif

  connectionsCleaner = new H323ConnectionsCleaner(*this);

  srand((unsigned)time(NULL)+clock());

#ifndef DISABLE_CALLAUTH
  SetEPSecurityPolicy(SecNone);
  SetEPCredentials(PString(),PString());
  isSecureCall = FALSE;
#endif

#ifdef H323_H460
  disableH460 = FALSE;
#endif

#ifdef H323_AEC 
  algoAEC = 0;
#endif

#ifdef H323_GNUGK
  gnugk = NULL;
#endif

  agc = 0;

  singleLine = FALSE;

  PTRACE(3, "H323\tCreated endpoint.");
}


H323EndPoint::~H323EndPoint()
{

#if defined(H323_RTP_AGGREGATE) || defined (H323_SIGNAL_AGGREGATE)
  // delete aggregators
  {
    PWaitAndSignal m(connectionsMutex);
#ifdef H323_RTP_AGGREGATE
    if (rtpAggregator != NULL) {
      delete rtpAggregator;
      rtpAggregator = NULL;
    }
#endif
#ifdef H323_SIGNAL_AGGREGATE
    if (signallingAggregator != NULL) {
      delete signallingAggregator;
      signallingAggregator = NULL;
    }
#endif
  }
#endif

  // And shut down the gatekeeper (if there was one)
  RemoveGatekeeper();

#ifdef H323_GNUGK
#if H323_FILE
  delete gnugk;
#endif
#endif

  // Shut down the listeners as soon as possible to avoid race conditions
  listeners.RemoveAll();

  // Clear any pending calls on this endpoint
  ClearAllCalls();

  // Shut down the cleaner thread
  delete connectionsCleaner;

  // Clean up any connections that the cleaner thread missed
  CleanUpConnections();

#ifdef P_STUN
 // delete stun;
#endif

  PTRACE(3, "H323\tDeleted endpoint.");
}


void H323EndPoint::SetEndpointTypeInfo(H225_EndpointType & info) const
{
  info.IncludeOptionalField(H225_EndpointType::e_vendor);
  SetVendorIdentifierInfo(info.m_vendor);

  switch (terminalType) {
    case e_TerminalOnly :
    case e_TerminalAndMC :
      info.IncludeOptionalField(H225_EndpointType::e_terminal);
      break;
    case e_GatewayOnly :
    case e_GatewayAndMC :
    case e_GatewayAndMCWithDataMP :
    case e_GatewayAndMCWithAudioMP :
    case e_GatewayAndMCWithAVMP :
      info.IncludeOptionalField(H225_EndpointType::e_gateway);
	  if (SetGatewaySupportedProtocol(info.m_gateway.m_protocol))
		  info.m_gateway.IncludeOptionalField(H225_GatewayInfo::e_protocol);
      break;
    case e_GatekeeperOnly :
    case e_GatekeeperWithDataMP :
    case e_GatekeeperWithAudioMP :
    case e_GatekeeperWithAVMP :
      info.IncludeOptionalField(H225_EndpointType::e_gatekeeper);
      break;
    case e_MCUOnly :
    case e_MCUWithDataMP :
    case e_MCUWithAudioMP :
    case e_MCUWithAVMP :
      info.IncludeOptionalField(H225_EndpointType::e_mcu);
      info.m_mc = TRUE;
	  if (SetGatewaySupportedProtocol(info.m_mcu.m_protocol))
		  info.m_mcu.IncludeOptionalField(H225_McuInfo::e_protocol);
  }
}

BOOL H323EndPoint::SetGatewaySupportedProtocol(H225_ArrayOf_SupportedProtocols & protocols) const
{
	PStringList prefixes;

	if (OnSetGatewayPrefixes(prefixes)) {
		H225_SupportedProtocols proto;
		proto.SetTag(H225_SupportedProtocols::e_h323);
		H225_H323Caps & caps = proto;
		caps.IncludeOptionalField(H225_H323Caps::e_supportedPrefixes);
         H225_ArrayOf_SupportedPrefix & pre = caps.m_supportedPrefixes;	
		 pre.SetSize(prefixes.GetSize());
		  for (PINDEX i=0; i < prefixes.GetSize(); i++) {
		     H225_SupportedPrefix p;
			   H225_AliasAddress & alias = p.m_prefix;
			   H323SetAliasAddress(prefixes[i],alias);
		     pre[i] = p;
		  }
		protocols.SetSize(1);
		protocols[0] = proto;
		return TRUE;
	}

	return FALSE;
}

BOOL H323EndPoint::OnSetGatewayPrefixes(PStringList & prefixes) const
{
	return FALSE;
}

void H323EndPoint::SetVendorIdentifierInfo(H225_VendorIdentifier & info) const
{
  SetH221NonStandardInfo(info.m_vendor);

  info.IncludeOptionalField(H225_VendorIdentifier::e_productId);
  info.m_productId = PProcess::Current().GetManufacturer() & PProcess::Current().GetName();
  info.m_productId.SetSize(info.m_productId.GetSize()+2);

  info.IncludeOptionalField(H225_VendorIdentifier::e_versionId);
  info.m_versionId = PProcess::Current().GetVersion(TRUE) + " (H323plus v" + OpalGetVersion() + ')';
  info.m_versionId.SetSize(info.m_versionId.GetSize()+2);
}


void H323EndPoint::SetH221NonStandardInfo(H225_H221NonStandard & info) const
{
  info.m_t35CountryCode = t35CountryCode;
  info.m_t35Extension = t35Extension;
  info.m_manufacturerCode = manufacturerCode;
}


H323Capability * H323EndPoint::FindCapability(const H245_Capability & cap) const
{
  return capabilities.FindCapability(cap);
}


H323Capability * H323EndPoint::FindCapability(const H245_DataType & dataType) const
{
  return capabilities.FindCapability(dataType);
}


H323Capability * H323EndPoint::FindCapability(H323Capability::MainTypes mainType,
                                              unsigned subType) const
{
  return capabilities.FindCapability(mainType, subType);
}


void H323EndPoint::AddCapability(H323Capability * capability)
{
  capabilities.Add(capability);
}


PINDEX H323EndPoint::SetCapability(PINDEX descriptorNum,
                                   PINDEX simultaneousNum,
                                   H323Capability * capability)
{
  return capabilities.SetCapability(descriptorNum, simultaneousNum, capability);
}

BOOL H323EndPoint::RemoveCapability(H323Capability::MainTypes capabilityType)
{
	return capabilities.RemoveCapability(capabilityType);
}

#ifdef H323_VIDEO
BOOL H323EndPoint::SetVideoFrameSize(H323Capability::CapabilityFrameSize frameSize, 
		                  int frameUnits)
{
    return capabilities.SetVideoFrameSize(frameSize,frameUnits);
}
#endif

PINDEX H323EndPoint::AddAllCapabilities(PINDEX descriptorNum,
                                        PINDEX simultaneous,
                                        const PString & name)
{
    PINDEX reply = simultaneous;
	reply = capabilities.AddAllCapabilities(descriptorNum, simultaneous, name);
//#ifdef H323_H239
//	AddAllExtendedVideoCapabilities(descriptorNum, simultaneous);
//#endif
  return reply;
}


void H323EndPoint::AddAllUserInputCapabilities(PINDEX descriptorNum,
                                               PINDEX simultaneous)
{
  H323_UserInputCapability::AddAllCapabilities(capabilities, descriptorNum, simultaneous);
}

#ifdef H323_VIDEO
#ifdef H323_H239
BOOL H323EndPoint::OpenExtendedVideoSession(const PString & token,H323ChannelNumber & num)
{
  H323Connection * connection = FindConnectionWithLock(token);
 
  BOOL success = FALSE;
  if (connection != NULL) {
    success = connection->OpenExtendedVideoSession(num);
    connection->Unlock();
  }

  return success;
}

BOOL H323EndPoint::CloseExtendedVideoSession(
		                       const PString & token,         
		                       const H323ChannelNumber & num  
	                           )
{
  H323Connection * connection = FindConnectionWithLock(token);
 
  BOOL success = FALSE;
  if (connection != NULL) {
    success = connection->CloseExtendedVideoSession(num);
    connection->Unlock();
  }

  return success;
}

void H323EndPoint::AddAllExtendedVideoCapabilities(PINDEX descriptorNum,
                                                   PINDEX simultaneous)
{
  H323ExtendedVideoCapability::AddAllCapabilities(capabilities, descriptorNum, simultaneous);
}
#endif  // H323_H239
#endif  // H323_VIDEO

void H323EndPoint::RemoveCapabilities(const PStringArray & codecNames)
{
  capabilities.Remove(codecNames);
}


void H323EndPoint::ReorderCapabilities(const PStringArray & preferenceOrder)
{
  capabilities.Reorder(preferenceOrder);
}


BOOL H323EndPoint::UseGatekeeper(const PString & address,
                                 const PString & identifier,
                                 const PString & localAddress)
{
  if (gatekeeper != NULL) {
    BOOL same = TRUE;

    if (!address)
      same = gatekeeper->GetTransport().GetRemoteAddress().IsEquivalent(address);

    if (!same && !identifier)
      same = gatekeeper->GetIdentifier() == identifier;

    if (!same && !localAddress)
      same = gatekeeper->GetTransport().GetLocalAddress().IsEquivalent(localAddress);

    if (same) {
      PTRACE(2, "H323\tUsing existing gatekeeper " << *gatekeeper);
      return TRUE;
    }
  }

  H323Transport * transport = NULL;
  if (!localAddress.IsEmpty()) {
    H323TransportAddress iface(localAddress);
    PIPSocket::Address ip;
    WORD port = H225_RAS::DefaultRasUdpPort;
    if (iface.GetIpAndPort(ip, port))
      transport = new H323TransportUDP(*this, ip, port);
  }

  if (address.IsEmpty()) {
    if (identifier.IsEmpty())
      return DiscoverGatekeeper(transport);
    else
      return LocateGatekeeper(identifier, transport);
  }
  else {
    if (identifier.IsEmpty())
      return SetGatekeeper(address, transport);
    else
      return SetGatekeeperZone(address, identifier, transport);
  }
}


BOOL H323EndPoint::SetGatekeeper(const PString & address, H323Transport * transport)
{
  H323Gatekeeper * gk = InternalCreateGatekeeper(transport);
  return InternalRegisterGatekeeper(gk, gk->DiscoverByAddress(address));
}


BOOL H323EndPoint::SetGatekeeperZone(const PString & address,
                                     const PString & identifier,
                                     H323Transport * transport)
{
  H323Gatekeeper * gk = InternalCreateGatekeeper(transport);
  return InternalRegisterGatekeeper(gk, gk->DiscoverByNameAndAddress(identifier, address));
}


BOOL H323EndPoint::LocateGatekeeper(const PString & identifier, H323Transport * transport)
{
  H323Gatekeeper * gk = InternalCreateGatekeeper(transport);
  return InternalRegisterGatekeeper(gk, gk->DiscoverByName(identifier));
}


BOOL H323EndPoint::DiscoverGatekeeper(H323Transport * transport)
{
  H323Gatekeeper * gk = InternalCreateGatekeeper(transport);
  return InternalRegisterGatekeeper(gk, gk->DiscoverAny());
}


H323Gatekeeper * H323EndPoint::InternalCreateGatekeeper(H323Transport * transport)
{
  RemoveGatekeeper(H225_UnregRequestReason::e_reregistrationRequired);

  if (transport == NULL)
    transport = new H323TransportUDP(*this);

  H323Gatekeeper * gk = CreateGatekeeper(transport);

  gk->SetPassword(gatekeeperPassword);

  return gk;
}


BOOL H323EndPoint::InternalRegisterGatekeeper(H323Gatekeeper * gk, BOOL discovered)
{
  if (discovered) {
    if (gk->RegistrationRequest()) {
      gatekeeper = gk;
      return TRUE;
    }

    // RRQ was rejected continue trying
    gatekeeper = gk;
  }
  else // Only stop listening if the GRQ was rejected
    delete gk;

  return FALSE;
}


H323Gatekeeper * H323EndPoint::CreateGatekeeper(H323Transport * transport)
{
  return new H323Gatekeeper(*this, transport);
}


BOOL H323EndPoint::IsRegisteredWithGatekeeper() const
{
  if (gatekeeper == NULL)
    return FALSE;

  return gatekeeper->IsRegistered();
}


BOOL H323EndPoint::RemoveGatekeeper(int reason)
{
  BOOL ok = TRUE;

  if (gatekeeper == NULL)
    return ok;

  ClearAllCalls();

  if (gatekeeper->IsRegistered()) // If we are registered send a URQ
    ok = gatekeeper->UnregistrationRequest(reason);

  delete gatekeeper;
  gatekeeper = NULL;

  return ok;
}


void H323EndPoint::SetGatekeeperPassword(const PString & password)
{
  gatekeeperPassword = password;

  if (gatekeeper != NULL) {
    gatekeeper->SetPassword(gatekeeperPassword);
    if (gatekeeper->IsRegistered()) // If we are registered send a URQ
      gatekeeper->UnregistrationRequest(H225_UnregRequestReason::e_reregistrationRequired);
    InternalRegisterGatekeeper(gatekeeper, TRUE);
  }
}

BOOL H323EndPoint::GatekeeperCheckIP(const H323TransportAddress & /*oldAddr*/,
									 H323TransportAddress & /*newaddress*/)
{
	return FALSE;
}

void H323EndPoint::OnAdmissionRequest(H323Connection & connection)
{
}

void H323EndPoint::OnGatekeeperConfirm()
{
}

void H323EndPoint::OnGatekeeperReject()
{
}

void H323EndPoint::OnRegistrationConfirm(const H323TransportAddress & /*rasAddress*/)
{
}

void H323EndPoint::OnRegistrationReject()
{
}

void H323EndPoint::OnUnRegisterRequest()
{
}

H235Authenticators H323EndPoint::CreateAuthenticators()
{
  H235Authenticators authenticators;

  PFactory<H235Authenticator>::KeyList_T keyList = PFactory<H235Authenticator>::GetKeyList();
  PFactory<H235Authenticator>::KeyList_T::const_iterator r;
  for (r = keyList.begin(); r != keyList.end(); ++r) {
    H235Authenticator * Auth = PFactory<H235Authenticator>::CreateInstance(*r);
    if ((Auth->GetApplication() == H235Authenticator::GKAdmission) ||
                  (Auth->GetApplication() == H235Authenticator::AnyApplication)) 
                               authenticators.Append(Auth);
  }

  return authenticators;
}

#ifndef DISABLE_CALLAUTH
H235Authenticators H323EndPoint::CreateEPAuthenticators() 
{
  H235Authenticators authenticators;

  PString username;
  PString password;

  if ((GetEPSecurityPolicy() != SecNone) || (isSecureCall)) {
	  if (GetEPCredentials(password, username)) {
             PFactory<H235Authenticator>::KeyList_T keyList = PFactory<H235Authenticator>::GetKeyList();
             PFactory<H235Authenticator>::KeyList_T::const_iterator r;
              for (r = keyList.begin(); r != keyList.end(); ++r) {
                H235Authenticator * Auth = PFactory<H235Authenticator>::CreateInstance(*r);
                if ((Auth->GetApplication() == H235Authenticator::EPAuthentication) ||
                     (Auth->GetApplication() == H235Authenticator::AnyApplication)) {
                        Auth->SetLocalId(username);
                        Auth->SetPassword(password);
                        authenticators.Append(Auth);
                }
              }
	    SetEPCredentials(PString(),PString());
	  }
	  isSecureCall = FALSE;
  }

  return authenticators;
}

BOOL H323EndPoint::GetEPCredentials(PString & password, 
									PString & username)
{
  if (EPSecurityPassword.IsEmpty())
     return FALSE;
  else
     password = EPSecurityPassword;

  if (EPSecurityUserName.IsEmpty())
      username = GetLocalUserName();
  else
      username = EPSecurityUserName;  

  return TRUE;
}

void H323EndPoint::SetEPCredentials(PString password, PString username)
{
	EPSecurityPassword = password;			
	EPSecurityUserName = username;			
}

void H323EndPoint::SetEPSecurityPolicy(EPSecurityPolicy policy)
{
	CallAuthPolicy = policy;
}

H323EndPoint::EPSecurityPolicy H323EndPoint::GetEPSecurityPolicy()
{
	return CallAuthPolicy;
}

H235AuthenticatorList H323EndPoint::GetAuthenticatorList()
{
	return EPAuthList;
}

BOOL H323EndPoint::OnCallAuthentication(const PString & username, 
										PString & password)
{
	if (EPAuthList.HasUserName(username)) {
		EPAuthList.LoadPassword(username, password);
		return TRUE;
	}

	return FALSE;
}

H323Connection * H323EndPoint::MakeAuthenticatedCall(const PString & remoteParty, 
		const PString & UserName, const PString & Password, PString & token, void * userData)
{
  isSecureCall = TRUE;
  SetEPCredentials(Password,UserName);
  return MakeCall(remoteParty, token, userData);
}
#endif

BOOL H323EndPoint::StartListeners(const H323TransportAddressArray & ifaces)
{
  if (ifaces.IsEmpty())
    return StartListener("*");

  PINDEX i;

  for (i = 0; i < listeners.GetSize(); i++) {
    BOOL remove = TRUE;
    for (PINDEX j = 0; j < ifaces.GetSize(); j++) {
      if (listeners[i].GetTransportAddress().IsEquivalent(ifaces[j])) {
        remove = FALSE;
        break;
      }
    }
    if (remove) {
      PTRACE(3, "H323\tRemoving listener " << listeners[i]);
      listeners.RemoveAt(i--);
    }
  }

  for (i = 0; i < ifaces.GetSize(); i++) {
    if (!ifaces[i])
      StartListener(ifaces[i]);
  }

  return listeners.GetSize() > 0;
}


BOOL H323EndPoint::StartListener(const H323TransportAddress & iface)
{
  H323Listener * listener;

  if (iface.IsEmpty())
    listener = new H323ListenerTCP(*this, PIPSocket::GetDefaultIpAny(), DefaultTcpPort);
  else
    listener = iface.CreateListener(*this);

  if (H323EndPoint::StartListener(listener))
    return TRUE;

  PTRACE(1, "H323\tCould not start listener: " << iface);
  delete listener;
  return FALSE;
}


BOOL H323EndPoint::StartListener(H323Listener * listener)
{
  if (listener == NULL)
    return FALSE;

  for (PINDEX i = 0; i < listeners.GetSize(); i++) {
    if (listeners[i].GetTransportAddress() == listener->GetTransportAddress()) {
      PTRACE(2, "H323\tAlready have listener for " << *listener);
      delete listener;
      return TRUE;
    }
  }

  // as the listener is not open, this will have the effect of immediately
  // stopping the listener thread. This is good - it means that the 
  // listener Close function will appear to have stopped the thread
  if (!listener->Open()) {
    listener->Resume(); // set the thread running so we can delete it later
    return FALSE;
  }

  PTRACE(3, "H323\tStarted listener " << *listener);
  listeners.Append(listener);
  listener->Resume();
  return TRUE;
}


BOOL H323EndPoint::RemoveListener(H323Listener * listener)
{
  if (listener != NULL) {
    PTRACE(3, "H323\tRemoving listener " << *listener);
    return listeners.Remove(listener);
  }

  PTRACE(3, "H323\tRemoving all listeners");
  listeners.RemoveAll();
  return TRUE;
}


H323TransportAddressArray H323EndPoint::GetInterfaceAddresses(BOOL excludeLocalHost,
                                                              H323Transport * associatedTransport)
{
  return H323GetInterfaceAddresses(listeners, excludeLocalHost, associatedTransport);
}

H323Connection * H323EndPoint::MakeCall(const PString & remoteParty,
                                        PString & token,
                                        void * userData)
{
  return MakeCall(remoteParty, NULL, token, userData);
}


H323Connection * H323EndPoint::MakeCall(const PString & remoteParty,
                                        H323Transport * transport,
                                        PString & token,
                                        void * userData)
{
  token = PString::Empty();

  PStringList Addresses;
  if (!ResolveCallParty(remoteParty, Addresses))
	return NULL;

  H323Connection * connection = NULL;
  for (PINDEX i = 0; i < Addresses.GetSize(); i++) {
       connection = InternalMakeCall(PString::Empty(),
                                     PString::Empty(),
                                     UINT_MAX,
                                     Addresses[i],
                                     transport,
                                     token,
                                     userData);
    if (connection != NULL) {
        connection->Unlock();
	    break;
	}
  }

  return connection;
}


H323Connection * H323EndPoint::MakeCallLocked(const PString & remoteParty,
                                              PString & token,
                                              void * userData,
                                              H323Transport * transport)
{
  token = PString::Empty();

  PStringList Addresses;
  if (!ResolveCallParty(remoteParty, Addresses))
	return NULL;

  H323Connection * connection = NULL;
  for (PINDEX i = 0; i < Addresses.GetSize(); i++) {
      connection = InternalMakeCall(PString::Empty(),
                             PString::Empty(),
                             UINT_MAX,
                             Addresses[i],
                             transport,
                             token,
                             userData);
     if (connection != NULL) 
		    break;		 
  }

  return connection;

}

H323Connection * H323EndPoint::InternalMakeCall(const PString & trasferFromToken,
                                                const PString & callIdentity,
                                                unsigned capabilityLevel,
                                                const PString & remoteParty,
                                                H323Transport * transport,
                                                PString & newToken,
                                                void * userData)
{
  PTRACE(2, "H323\tMaking call to: " << remoteParty);

  PString alias;
  H323TransportAddress address;
  if (!ParsePartyName(remoteParty, alias, address)) {
    PTRACE(2, "H323\tCould not parse \"" << remoteParty << '"');
    return NULL;
  }

  if (transport == NULL) {
    // Restriction: the call must be made on the same transport as the one
    // that the gatekeeper is using.
    if (gatekeeper != NULL)
      transport = gatekeeper->GetTransport().GetRemoteAddress().CreateTransport(*this);

    // assume address is an IP address/hostname
    else
      transport = address.CreateTransport(*this);

    if (transport == NULL) {
      PTRACE(1, "H323\tInvalid transport in \"" << remoteParty << '"');
      return NULL;
    }
  }

  H323Connection * connection;

  connectionsMutex.Wait();

  unsigned lastReference;
  if (newToken.IsEmpty()) {
    do {
      lastReference = Q931::GenerateCallReference();
      newToken = BuildConnectionToken(*transport, lastReference, FALSE);
    } while (connectionsActive.Contains(newToken));
  }
  else {
    lastReference = newToken.Mid(newToken.Find('/')+1).AsUnsigned();

    // Move old connection on token to new value and flag for removal
    PString adjustedToken;
    unsigned tieBreaker = 0;
    do {
      adjustedToken = newToken + "-replaced";
      adjustedToken.sprintf("-%u", ++tieBreaker);
    } while (connectionsActive.Contains(adjustedToken));
    connectionsActive.SetAt(adjustedToken, connectionsActive.RemoveAt(newToken));
    connectionsToBeCleaned += adjustedToken;
    PTRACE(3, "H323\tOverwriting call " << newToken << ", renamed to " << adjustedToken);
  }
//  connectionsMutex.Signal(); // [1]

  connection = CreateConnection(lastReference, userData, transport, NULL);
  if (connection == NULL) {
    PTRACE(1, "H323\tCreateConnection returned NULL");
    connectionsMutex.Signal();
    return NULL;
  }

  connection->Lock();

//  connectionsMutex.Wait();   // [2]

// 1 & 2 commented 14.10.13 by kay27 due to having assertion fails time to time:
// "Invalid array index, file ../common/collect.cxx, line 1572, class PDictionary"
// when one connects & other disconnects at the same time.

  connectionsActive.SetAt(newToken, connection);

  connectionsMutex.Signal();

  connection->AttachSignalChannel(newToken, transport, FALSE);

#ifdef H323_H450
  if (capabilityLevel == UINT_MAX)
    connection->HandleTransferCall(trasferFromToken, callIdentity);
  else {
    connection->HandleIntrudeCall(trasferFromToken, callIdentity);
    connection->IntrudeCall(capabilityLevel);
  }
#endif

  PTRACE(3, "H323\tCreated new connection: " << newToken);

  new H225CallThread(*this, *connection, *transport, alias, address);
  return connection;
}

#if P_DNS

struct LookupRecord {
  enum {
    CallDirect,
    LRQ
  };
  int type;
  PIPSocket::Address addr;
  WORD port;
};
/*
static BOOL FindSRVRecords(std::vector<LookupRecord> & recs,
                    const PString & domain,
                    int type,
                    const PString & srv)
{
  PDNS::SRVRecordList srvRecords;
  PString srvLookupStr = srv + domain;
  BOOL found = PDNS::GetRecords(srvLookupStr, srvRecords);
  if (found) {
    PDNS::SRVRecord * recPtr = srvRecords.GetFirst();
    while (recPtr != NULL) {
      LookupRecord rec;
      rec.addr = recPtr->hostAddress;
      rec.port = recPtr->port;
      rec.type = type;
      recs.push_back(rec);
      recPtr = srvRecords.GetNext();
      PTRACE(4, "H323\tFound " << rec.addr << ":" << rec.port << " with SRV " << srv << " using domain " << domain);
    }
  } 
  return found;
}

static BOOL FindRoutes(const PString & domain, WORD port, std::vector<LookupRecord> & routes)
{
  BOOL hasGK = FindSRVRecords(    routes, domain, LookupRecord::LRQ,        "_h323ls._udp.");
  hasGK = hasGK || FindSRVRecords(routes, domain, LookupRecord::LRQ,        "_h323rs._udp.");
  FindSRVRecords(                 routes, domain, LookupRecord::CallDirect, "_h323cs._tcp.");

  // see if the domain is actually a host
  PIPSocket::Address addr;
  if (PIPSocket::GetHostAddress(domain, addr)) {
    LookupRecord rec;
    rec.addr = addr;
    rec.port = port;
    rec.type = LookupRecord::CallDirect;
    PTRACE(4, "H323\tDomain " << domain << " is a host - using as call signal address");
    routes.push_back(rec);
  }

  if (routes.size() != 0) {
    PDNS::MXRecordList mxRecords;
    if (PDNS::GetRecords(domain, mxRecords)) {
      PDNS::MXRecord * recPtr = mxRecords.GetFirst();
      while (recPtr != NULL) {
        LookupRecord rec;
        rec.addr = recPtr->hostAddress;
        rec.port = 1719;
        rec.type = LookupRecord::LRQ;
        routes.push_back(rec);
        recPtr = mxRecords.GetNext();
        PTRACE(4, "H323\tFound " << rec.addr << ":" << rec.port << " with MX for domain " << domain);
      }
    } 
  }

  return routes.size() != 0;
}
*/
#endif

BOOL H323EndPoint::ResolveCallParty(const PString & _remoteParty, PStringList & addresses)
{
  PString remoteParty = _remoteParty;

#if P_DNS
  // if there is no gatekeeper, 
  if (gatekeeper == NULL)
  {
    //if there is no '@', and there is no URL scheme, then attempt to use ENUM
    if ((_remoteParty.Find(':') == P_MAX_INDEX) && (remoteParty.Find('@') == P_MAX_INDEX))
    {
      PString number = _remoteParty;
      if (number.Left(5) *= "h323:") number = number.Mid(5);

      PINDEX i;
      for (i = 0; i < number.GetLength(); ++i) if (!isdigit(number[i])) break;
      if (i >= number.GetLength())
      {
        PString str;
        if (PDNS::ENUMLookup(number, "E2U+h323", str))
        {
          if ((str.Find("//1") != P_MAX_INDEX) &&
              (str.Find('@'  ) != P_MAX_INDEX))
          {
            remoteParty = "h323:" + number + str.Mid(str.Find('@')-1);
          }
          else
          {
            remoteParty = str;
          }
          PTRACE(4, "H323\tENUM converted remote party " << _remoteParty << " to " << remoteParty);
        }
        else
        {
          PTRACE(4, "H323\tENUM Cannot resolve remote party " << _remoteParty);
        }
      }
    }

     // attempt a DNS SRV lookup to detect a call signalling entry
    PINDEX atPos=remoteParty.Find('@');
    if (atPos != P_MAX_INDEX)
    {

      {
        PString host = remoteParty.Mid(atPos+1);
        PINDEX lcpos = host.Find(':');
        if(lcpos != P_MAX_INDEX) host = host.Left(lcpos);
        PTRACE(4,"H323\tDNS SRV Extracted host descriptor: " << host);
        PIPSocket::Address addr;
        if(addr.FromString(host))
        {
          PTRACE(4,"H323\tDNS SRV Lookup prevented: domain part is already IP address " << addr.AsString());
          addresses = PStringList(remoteParty);
          return TRUE;
        }
      }

      PString number = remoteParty;
      if (!(number.Left(5) *= "h323:")) number = "h323:" + number;

      PStringList str;
      if (PDNS::LookupSRV(number,"_h323cs._tcp.",str))
      {
        for (PINDEX i=0; i<str.GetSize(); i++)
        {
          PTRACE(4, "H323\tDNS SRV CS converted remote party " << _remoteParty << " to " << str[i]);
          addresses.AppendString(str[i]);
        }
      } 
      else
      {
        PTRACE(4, "H323\tDNS SRV Cannot resolve remote party " << remoteParty);
        addresses = PStringList(remoteParty);
      }
    }
    else
    {
      addresses = PStringList(remoteParty);
    }
    return TRUE;
  }  
#endif
  addresses = PStringList(remoteParty);
  return TRUE;
}

BOOL H323EndPoint::ParsePartyName(const PString & _remoteParty,
                                  PString & alias,
                                  H323TransportAddress & address)
{
  PString remoteParty = _remoteParty;

  // Support [address]##[Alias] dialing
  PINDEX hash = _remoteParty.Find("##");
  if (hash != P_MAX_INDEX) {
    remoteParty = "h323:" + _remoteParty.Mid(hash+2) + "@" + _remoteParty.Left(hash);
    PTRACE(4, "H323\tConverted " << _remoteParty << " to " << remoteParty);
  }

  // convert the remote party string to a URL, with a default URL of "h323:"
  PURL url(remoteParty, "h323");

  // if the scheme does not match the prefix of the remote party, then
  // the URL parser got confused, so force the URL to be of type "h323"
  if ((remoteParty.Find('@') == P_MAX_INDEX) && (remoteParty.NumCompare(url.GetScheme()) != EqualTo)) {
    if (gatekeeper == NULL)
      url.Parse("h323:@" + remoteParty);
    else
      url.Parse("h323:" + remoteParty);
  }

  // get the various parts of the name
  PString hostOnly = PString();
  if(remoteParty.Find('@') != P_MAX_INDEX)
  {
    alias = url.GetUserName();
    hostOnly = remoteParty.Mid(remoteParty.Find('@')+1);
  } else {
    alias = url.GetUserName();
    hostOnly = url.GetHostName();
  }
  address = hostOnly;
/*
  // get the various parts of the name
  PString hostOnly = PString();
  if (remoteParty.Find('@') != P_MAX_INDEX) {
    if (gatekeeper != NULL)
      alias = url.AsString();
    else {
      alias = remoteParty.Left(remoteParty.Find('@'));
      hostOnly = remoteParty.Mid(remoteParty.Find('@')+1);
    }
  } else {
    alias = url.GetUserName();
    hostOnly = url.GetHostName();
  }
  address = hostOnly;
*/

  // make sure the address contains the port, if not default
  if (!address && (url.GetPort() != 0))
    address.sprintf(":%u", url.GetPort());

  if (alias.IsEmpty() && address.IsEmpty()) {
    PTRACE(1, "H323\tAttempt to use invalid URL \"" << remoteParty << '"');
    return FALSE;
  }

  BOOL gatekeeperSpecified = FALSE;
  BOOL gatewaySpecified = FALSE;

  PCaselessString type = url.GetParamVars()("type");

  if (url.GetScheme() == "callto") {
    // Do not yet support ILS
    if (type == "directory") {
#if P_LDAP
      PString server = url.GetHostName();
      if (server.IsEmpty())
        server = ilsServer;
      if (server.IsEmpty())
        return FALSE;

      PILSSession ils;
      if (!ils.Open(server, url.GetPort())) {
        PTRACE(1, "H323\tCould not open ILS server at \"" << server
               << "\" - " << ils.GetErrorText());
        return FALSE;
      }

      PILSSession::RTPerson person;
      if (!ils.SearchPerson(alias, person)) {
        PTRACE(1, "H323\tCould not find "
               << server << '/' << alias << ": " << ils.GetErrorText());
        return FALSE;
      }

      if (!person.sipAddress.IsValid()) {
        PTRACE(1, "H323\tILS user " << server << '/' << alias
               << " does not have a valid IP address");
        return FALSE;
      }

      // Get the IP address
      address = H323TransportAddress(person.sipAddress);

      // Get the port if provided
      for (PINDEX i = 0; i < person.sport.GetSize(); i++) {
        if (person.sport[i] != 1503) { // Dont use the T.120 port
          address = H323TransportAddress(person.sipAddress, person.sport[i]);
          break;
        }
      }

      alias = PString::Empty(); // No alias for ILS lookup, only host
      return TRUE;
#else
      return FALSE;
#endif
    }

    if (url.GetParamVars().Contains("gateway"))
      gatewaySpecified = TRUE;
  }

  else if (url.GetScheme() == "h323") {
    if (type == "gw")
      gatewaySpecified = TRUE;
    else if (type == "gk")
      gatekeeperSpecified = TRUE;
    else if (!type) {
      PTRACE(1, "H323\tUnsupported host type \"" << type << "\" in h323 URL");
      return FALSE;
    }
  }

  // User explicitly asked to use a GK for lookup
  if (gatekeeperSpecified) {
    if (alias.IsEmpty()) {
      PTRACE(1, "H323\tAttempt to use explict gatekeeper without alias!");
      return FALSE;
    }

    if (address.IsEmpty()) {
      PTRACE(1, "H323\tAttempt to use explict gatekeeper without address!");
      return FALSE;
    }

    H323TransportAddress gkAddr = address;
    PTRACE(3, "H323\tLooking for \"" << alias << "\" on gatekeeper at " << gkAddr);

    H323Gatekeeper * gk = CreateGatekeeper(new H323TransportUDP(*this));

    BOOL ok = gk->DiscoverByAddress(gkAddr);
    if (ok) {
      ok = gk->LocationRequest(alias, address);
      if (ok) {
        PTRACE(3, "H323\tLocation Request of \"" << alias << "\" on gk " << gkAddr << " found " << address);
      }
      else {
        PTRACE(1, "H323\tLocation Request failed for \"" << alias << "\" on gk " << gkAddr);
      }
    }
    else {
      PTRACE(1, "H323\tLocation Request discovery failed for gk " << gkAddr);
    }

    delete gk;

    return ok;
  }

  // User explicitly said to use a gw, or we do not have a gk to do look up
  if (gatekeeper == NULL
      || gatewaySpecified) {
    // If URL did not have a host, but user said to use gw, or we do not have
    // a gk to do a lookup so we MUST have a host, use alias must be host
    if (address.IsEmpty()) {
      address = alias;
      alias = PString::Empty();
      return TRUE;
    }
/*
#if P_DNS
    // if we have an address and the correct scheme, then check DNS
    if (!address && (url.GetScheme() *= "h323")) {
      std::vector<LookupRecord> routes;
      if (FindRoutes(hostOnly, url.GetPort(), routes)) {
        std::vector<LookupRecord>::const_iterator r;
        for (r = routes.begin(); r != routes.end(); ++r) {
          const LookupRecord & rec = *r;
          switch (rec.type) {
            case LookupRecord::CallDirect:
              address = H323TransportAddress(rec.addr, rec.port);
              PTRACE(3, "H323\tParty name \"" << url << "\" mapped to \"" << alias << "@" << address);
              return TRUE;
              break; 

            case LookupRecord::LRQ:
              {
                H323TransportAddress newAddr, gkAddr(rec.addr, rec.port);
                H323Gatekeeper * gk = CreateGatekeeper(new H323TransportUDP(*this));
                BOOL ok = gk->DiscoverByAddress(gkAddr);
                if (ok) 
                  ok = gk->LocationRequest(alias, newAddr);
                delete gk;
                if (ok) {
                  address = newAddr;
                  PTRACE(3, "H323\tLocation Request of \"" << alias << "\" on gk " << gkAddr << " found " << address);
                  return TRUE;
                }
              } 
              break;
  
            default:
              break;
          }
        }
      }
    }
#endif   // P_DNS
*/
  } 

  if (!address)
    return TRUE;

  // We have a gk and user did not explicitly supply a host, so lets
  // do a check to see if it is an IP address or hostname
  if (alias.FindOneOf("$.:[") != P_MAX_INDEX) {
    H323TransportAddress test = alias;
    PIPSocket::Address ip;
    if (test.GetIpAddress(ip) && ip.IsValid()) {
      // The alias was a valid internet address, use it as such
      alias = PString::Empty();
      address = test;
    }
  }

  return TRUE;
}

#ifdef H323_H450

H323Connection * H323EndPoint::SetupTransfer(const PString & oldToken,
                                             const PString & callIdentity,
                                             const PString & remoteParty,
                                             PString & newToken,
                                             void * userData)
{
  newToken = PString::Empty();

  PStringList Addresses;
  if (!ResolveCallParty(remoteParty, Addresses))
	return NULL;

  H323Connection * connection = NULL;
     for (PINDEX i = 0; i < Addresses.GetSize(); i++) {
         connection = InternalMakeCall(oldToken,
                                       callIdentity,
                                       UINT_MAX,
                                       Addresses[i],
                                       NULL,
                                       newToken,
                                       userData);
        if (connection != NULL) {
            connection->Unlock();
			break;
		}
	 }

   return connection;
}


void H323EndPoint::TransferCall(const PString & token, 
                                const PString & remoteParty,
                                const PString & callIdentity)
{
  H323Connection * connection = FindConnectionWithLock(token);
  if (connection != NULL) {
    connection->TransferCall(remoteParty, callIdentity);
    connection->Unlock();
  }
}


void H323EndPoint::ConsultationTransfer(const PString & primaryCallToken,   
                                        const PString & secondaryCallToken)
{
  H323Connection * secondaryCall = FindConnectionWithLock(secondaryCallToken);
  if (secondaryCall != NULL) {
    secondaryCall->ConsultationTransfer(primaryCallToken);
    secondaryCall->Unlock();
  }
}


void H323EndPoint::HoldCall(const PString & token, BOOL localHold)
{
  H323Connection * connection = FindConnectionWithLock(token);
  if (connection != NULL) {
    connection->HoldCall(localHold);
    connection->Unlock();
  }
}


H323Connection * H323EndPoint::IntrudeCall(const PString & remoteParty,
                                        PString & token,
                                        unsigned capabilityLevel,
                                        void * userData)
{
  return IntrudeCall(remoteParty, NULL, token, capabilityLevel, userData);
}


H323Connection * H323EndPoint::IntrudeCall(const PString & remoteParty,
                                        H323Transport * transport,
                                        PString & token,
                                        unsigned capabilityLevel,
                                        void * userData)
{
  token = PString::Empty();

 PStringList Addresses;
  if (!ResolveCallParty(remoteParty, Addresses))
	return NULL;

  H323Connection * connection = NULL;
     for (PINDEX i = 0; i < Addresses.GetSize(); i++) {
             connection = InternalMakeCall(PString::Empty(),
                                           PString::Empty(),
                                           capabilityLevel,
                                           Addresses[i],
                                           transport,
                                           token,
                                           userData);
		 if (connection != NULL) {
            connection->Unlock();
			break;
		 }
	 }
  return connection;
}

void H323EndPoint::OnReceivedInitiateReturnError()
{
}

#endif  // H323_H450


BOOL H323EndPoint::ClearCall(const PString & token,
                             H323Connection::CallEndReason reason)
{
  return ClearCallSynchronous(token, reason, NULL);
}

void H323EndPoint::OnCallClearing(H323Connection * /*connection*/,
								  H323Connection::CallEndReason /*reason*/)
{
}

BOOL H323EndPoint::ClearCallSynchronous(const PString & token,
                             H323Connection::CallEndReason reason)
{
  PSyncPoint sync;
  return ClearCallSynchronous(token, reason, &sync);
}

BOOL H323EndPoint::ClearCallSynchronous(const PString & token,
                                        H323Connection::CallEndReason reason,
                                        PSyncPoint * sync)
{
  if (PThread::Current() == connectionsCleaner)
    sync = NULL;

  /*The hugely multi-threaded nature of the H323Connection objects means that
    to avoid many forms of race condition, a call is cleared by moving it from
    the "active" call dictionary to a list of calls to be cleared that will be
    processed by a background thread specifically for the purpose of cleaning
    up cleared connections. So that is all that this function actually does.
    The real work is done in the H323ConnectionsCleaner thread.
   */

  {
    PWaitAndSignal wait(connectionsMutex);

    // Find the connection by token, callid or conferenceid
    H323Connection * connection = FindConnectionWithoutLocks(token);
    if (connection == NULL) {
      PTRACE(3, "H323\tAttempt to clear unknown call " << token);
      return FALSE;
    }

    PTRACE(3, "H323\tClearing connection " << connection->GetCallToken()
                                           << " reason=" << reason);

    OnCallClearing(connection,reason);

    // Add this to the set of connections being cleaned, if not in already
    if (!connectionsToBeCleaned.Contains(connection->GetCallToken())) 
      connectionsToBeCleaned += connection->GetCallToken();

    // Now set reason for the connection close
    connection->SetCallEndReason(reason, sync);

    // Signal the background threads that there is some stuff to process.
    connectionsCleaner->Signal();
  }

  if (sync != NULL)
    sync->Wait();

  return TRUE;
}


void H323EndPoint::ClearAllCalls(H323Connection::CallEndReason reason,
                                 BOOL wait)
{
  /*The hugely multi-threaded nature of the H323Connection objects means that
    to avoid many forms of race condition, a call is cleared by moving it from
    the "active" call dictionary to a list of calls to be cleared that will be
    processed by a background thread specifically for the purpose of cleaning
    up cleared connections. So that is all that this function actually does.
    The real work is done in the H323ConnectionsCleaner thread.
   */

  connectionsMutex.Wait();

  // Add all connections to the to be deleted set
  PINDEX i;
  for (i = 0; i < connectionsActive.GetSize(); i++) {
    H323Connection & connection = connectionsActive.GetDataAt(i);
    connectionsToBeCleaned += connection.GetCallToken();
    // Now set reason for the connection close
    connection.SetCallEndReason(reason, NULL);
  }

  // Signal the background threads that there is some stuff to process.
  connectionsCleaner->Signal();

  // Make sure any previous signals are removed before waiting later
  while (connectionsAreCleaned.Wait(0))
    ;

  connectionsMutex.Signal();

  if (wait)
    connectionsAreCleaned.Wait();
}

void H323EndPoint::CleanUpConnections()
{
  PTRACE(3, "H323\tCleaning up connections");

  // Lock the connections database.
  connectionsMutex.Wait();

  // Continue cleaning up until no more connections to clean
  while (connectionsToBeCleaned.GetSize() > 0) {
    // Just get the first entry in the set of tokens to clean up.
    PString token = connectionsToBeCleaned.GetKeyAt(0);
    H323Connection & connection = connectionsActive[token];

    // Unlock the structures here so does not block other uses of ClearCall()
    // for the possibly long time it takes to CleanUpOnCallEnd().
    connectionsMutex.Signal();

    // Clean up the connection, waiting for all threads to terminate
    connection.CleanUpOnCallEnd();
    connection.OnCleared();

    // Get the lock again as we remove the connection from our database
    connectionsMutex.Wait();

    // Remove the token from the set of connections to be cleaned up
    connectionsToBeCleaned -= token;

    // And remove the connection instance itself from the dictionary which will
    // cause its destructor to be called.
    H323Connection * connectionToDelete = connectionsActive.RemoveAt(token);

    // Unlock the structures yet again to avoid possible race conditions when
    // deleting the connection as well as the delte of a conncetion descendent
    // is application writer dependent and may cause deadlocks or just consume
    // lots of time.
    connectionsMutex.Signal();

    // Finally we get to delete it!
    delete connectionToDelete;

    // Get the lock again as we continue around the loop
    connectionsMutex.Wait();
  }

  // Finished with loop, unlock the connections database.
  connectionsMutex.Signal();

  // Signal thread that may be waiting on ClearAllCalls()
  connectionsAreCleaned.Signal();
}


BOOL H323EndPoint::HasConnection(const PString & token)
{
  PWaitAndSignal wait(connectionsMutex);

  return FindConnectionWithoutLocks(token) != NULL;
}


H323Connection * H323EndPoint::FindConnectionWithLock(const PString & token)
{
  PWaitAndSignal mutex(connectionsMutex);

  /*We have a very yucky polling loop here as a semi permanant measure.
    Why? We cannot call Lock() inside the connectionsMutex critical section as
    it will cause a deadlock with something like a RELEASE-COMPLETE coming in
    on separate thread. But if we put it outside there is a small window where
    the connection could get deleted before the Lock() test is done.
    The solution is to attempt to get the mutex while inside the
    connectionsMutex but not block. That means a polling loop. There is
    probably a way to do this properly with mutexes but I don't have time to
    figure it out.
   */
  H323Connection * connection;
  int count=0;
  while ((connection = FindConnectionWithoutLocks(token)) != NULL) {
    switch (connection->TryLock()) {
      case 0 :
        return NULL;
      case 1 :
        return connection;
    }
    if(count==100) 
    {
     cout << "Warning! Can not lock connection\n";
     return NULL; //BAD idea but avoid deadlock
    }
    count++;
    // Could not get connection lock, unlock the endpoint lists so a thread
    // that has the connection lock gets a chance at the endpoint lists.
    connectionsMutex.Signal();
    PThread::Sleep(20);
    connectionsMutex.Wait();
  }

  return NULL;
}


H323Connection * H323EndPoint::FindConnectionWithoutLocks(const PString & token)
{
  if (token.IsEmpty())
    return NULL;

  H323Connection * conn_ptr = connectionsActive.GetAt(token);
  if (conn_ptr != NULL)
    return conn_ptr;

  PINDEX i;
  for (i = 0; i < connectionsActive.GetSize(); i++) {
    H323Connection & conn = connectionsActive.GetDataAt(i);
    if (conn.GetCallIdentifier().AsString() == token)
      return &conn;
  }

  for (i = 0; i < connectionsActive.GetSize(); i++) {
    H323Connection & conn = connectionsActive.GetDataAt(i);
    if (conn.GetConferenceIdentifier().AsString() == token)
      return &conn;
  }

  return NULL;
}


PStringList H323EndPoint::GetAllConnections()
{
  PStringList tokens;

  connectionsMutex.Wait();

  for (PINDEX i = 0; i < connectionsActive.GetSize(); i++)
    tokens.AppendString(connectionsActive.GetKeyAt(i));

  connectionsMutex.Signal();

  return tokens;
}


BOOL H323EndPoint::OnIncomingCall(H323Connection & /*connection*/,
                                  const H323SignalPDU & /*setupPDU*/,
                                  H323SignalPDU & /*alertingPDU*/)
{
  return TRUE;
}

BOOL H323EndPoint::OnIncomingCall(H323Connection & connection,
                             const H323SignalPDU & setupPDU,
                                   H323SignalPDU & alertingPDU,
                   H323Connection::CallEndReason & reason)
{
  reason = H323Connection::EndedByNoAccept;
  return connection.OnIncomingCall(setupPDU, alertingPDU);
}

BOOL H323EndPoint::OnCallTransferInitiate(H323Connection & /*connection*/,
                                          const PString & /*remoteParty*/)
{
  return TRUE;
}


BOOL H323EndPoint::OnCallTransferIdentify(H323Connection & /*connection*/)
{
  return TRUE;
}

void H323EndPoint::OnSendARQ(H323Connection & /*conn*/, H225_AdmissionRequest & /*arq*/)
{
}

void H323EndPoint::OnReceivedACF(H323Connection & /*conn*/, const H225_AdmissionConfirm & /*arq*/)
{
}

void H323EndPoint::OnReceivedARJ(H323Connection & /*conn*/, const H225_AdmissionReject & /*arq*/)
{
}

H323Connection::AnswerCallResponse
       H323EndPoint::OnAnswerCall(H323Connection & /*connection*/,
                                  const PString & PTRACE_PARAM(caller),
                                  const H323SignalPDU & /*setupPDU*/,
                                  H323SignalPDU & /*connectPDU*/)
{
  PTRACE(2, "H225\tOnAnswerCall from \"" << caller << '"');
  return H323Connection::AnswerCallNow;
}


BOOL H323EndPoint::OnAlerting(H323Connection & /*connection*/,
                              const H323SignalPDU & /*alertingPDU*/,
                              const PString & /*username*/)
{
  PTRACE(1, "H225\tReceived alerting PDU.");
  return TRUE;
}


BOOL H323EndPoint::OnConnectionForwarded(H323Connection & /*connection*/,
                                         const PString & /*forwardParty*/,
                                         const H323SignalPDU & /*pdu*/)
{
  return FALSE;
}


BOOL H323EndPoint::ForwardConnection(H323Connection & connection,
                                     const PString & forwardParty,
                                     const H323SignalPDU & /*pdu*/)
{
  PString token = connection.GetCallToken();

 PStringList Addresses;
  if (!ResolveCallParty(forwardParty, Addresses))
	return FALSE;

  H323Connection * newConnection = NULL;
     for (PINDEX i = 0; i < Addresses.GetSize(); i++) {
          newConnection = InternalMakeCall(PString::Empty(),
                                           PString::Empty(),
                                           UINT_MAX,
                                           Addresses[i],
                                           NULL,
                                           token,
                                           NULL);
        if (newConnection != NULL)
             break;
	 }


	 if (newConnection == NULL) 
		 return FALSE;

     connection.SetCallEndReason(H323Connection::EndedByCallForwarded);
     newConnection->Unlock();
     return TRUE;
}


void H323EndPoint::OnConnectionEstablished(H323Connection & /*connection*/,
                                           const PString & /*token*/)
{
}


BOOL H323EndPoint::IsConnectionEstablished(const PString & token)
{
  H323Connection * connection = FindConnectionWithLock(token);
  if (connection == NULL)
    return FALSE;

  BOOL established = connection->IsEstablished();
  connection->Unlock();
  return established;
}


BOOL H323EndPoint::OnOutgoingCall(H323Connection & /*connection*/,
                             const H323SignalPDU & /*connectPDU*/)
{
  PTRACE(1, "H225\tReceived connect PDU.");
  return TRUE;
}


void H323EndPoint::OnConnectionCleared(H323Connection & /*connection*/,
                                       const PString & /*token*/)
{
}


PString H323EndPoint::BuildConnectionToken(const H323Transport & transport,
                                           unsigned callReference,
                                           BOOL fromRemote)
{
  PString token;

  if (fromRemote)
    token = transport.GetRemoteAddress();
  else
    token = "ip$localhost";

  token.sprintf("/%u", callReference);

  return token;
}


H323Connection * H323EndPoint::OnIncomingConnection(H323Transport * transport,
                                                    H323SignalPDU & setupPDU)
{
  unsigned callReference = setupPDU.GetQ931().GetCallReference();
  PString token = BuildConnectionToken(*transport, callReference, TRUE);

  connectionsMutex.Wait();
  H323Connection * connection = connectionsActive.GetAt(token);
  connectionsMutex.Signal();

  if (connection == NULL) {
    connection = CreateConnection(callReference, NULL, transport, &setupPDU);
    if (connection == NULL) {
      PTRACE(1, "H323\tCreateConnection returned NULL");
      return NULL;
    }

    PTRACE(3, "H323\tCreated new connection: " << token);

    connectionsMutex.Wait();
    connectionsActive.SetAt(token, connection);
    connectionsMutex.Signal();
  }

  connection->AttachSignalChannel(token, transport, TRUE);

  return connection;
}


H323Connection * H323EndPoint::CreateConnection(unsigned callReference,
                                                void * userData,
                                                H323Transport * /*transport*/,
                                                H323SignalPDU * /*setupPDU*/)
{
  return CreateConnection(callReference, userData);
}


H323Connection * H323EndPoint::CreateConnection(unsigned callReference, void * /*userData*/)
{
  return CreateConnection(callReference);
}

H323Connection * H323EndPoint::CreateConnection(unsigned callReference)
{
  return new H323Connection(*this, callReference);
}


#if PTRACING
static void OnStartStopChannel(const char * startstop, const H323Channel & channel)
{
  const char * dir;
  switch (channel.GetDirection()) {
    case H323Channel::IsTransmitter :
      dir = "send";
      break;

    case H323Channel::IsReceiver :
      dir = "receiv";
      break;

    default :
      dir = "us";
      break;
  }

  PTRACE(2, "H323\t" << startstop << "ed "
                     << dir << "ing logical channel: "
                     << channel.GetCapability());
}
#endif


BOOL H323EndPoint::OnStartLogicalChannel(H323Connection & /*connection*/,
                                         H323Channel & PTRACE_PARAM(channel))
{
#if PTRACING
  OnStartStopChannel("Start", channel);
#endif
  return TRUE;
}


void H323EndPoint::OnClosedLogicalChannel(H323Connection & /*connection*/,
                                          const H323Channel & PTRACE_PARAM(channel))
{
#if PTRACING
  OnStartStopChannel("Stopp", channel);
#endif
}


#ifdef H323_AUDIO_CODECS

BOOL H323EndPoint::OpenAudioChannel(H323Connection & /*connection*/,
                                    BOOL isEncoding,
                                    unsigned bufferSize,
                                    H323AudioCodec & codec)
{
  codec.SetSilenceDetectionMode(GetSilenceDetectionMode());

#ifdef P_AUDIO

  int rate = codec.GetMediaFormat().GetTimeUnits() * 1000;

  unsigned codecChannels = 1;
  PString deviceName;
  PString deviceDriver;
  if (isEncoding) {
    deviceName   = GetSoundChannelRecordDevice();
    deviceDriver = GetSoundChannelRecordDriver();
   { PString OptionValue; if(codec.GetMediaFormat().GetOptionValue((const PString)"Encoder Channels", OptionValue))
      codecChannels = atoi(OptionValue);
    }
  } else {
    deviceName = GetSoundChannelPlayDevice();
    deviceDriver = GetSoundChannelPlayDriver();
    { PString OptionValue; if(codec.GetMediaFormat().GetOptionValue((const PString)"Decoder Channels", OptionValue))
        codecChannels = atoi(OptionValue);
    }
  }

  PTRACE(1, "OpenAudioChannel\tTring to use driver=" << deviceDriver << " device=" << deviceName);

  PSoundChannel * soundChannel;
  if (!deviceDriver.IsEmpty()) 
    soundChannel = PSoundChannel::CreateChannel(deviceDriver);
  else {
    soundChannel = new PSoundChannel;
    deviceDriver = "default";
    PTRACE(1, "OpenAudioChannel\tFailed to set driver, using default");
  }

  if (soundChannel == NULL) {
    PTRACE(1, "Codec\tCould not open a sound channel for " << deviceDriver);
    return FALSE;
  }

  if (soundChannel->Open(deviceName, isEncoding ? PSoundChannel::Recorder
                                                : PSoundChannel::Player,
                         codecChannels, rate, 16)) {
    PTRACE(3, "Codec\tOpened sound channel \"" << deviceName
           << "\" for " << (isEncoding ? "record" : "play")
           << "ing at " << codecChannels << "x" << rate << " samples/second using " << soundChannelBuffers
           << 'x' << bufferSize << " byte buffers.");
    soundChannel->SetBuffers(bufferSize, soundChannelBuffers);
    return codec.AttachChannel(soundChannel);
  }

  PTRACE(1, "Codec\tCould not open " << deviceDriver << " sound channel \"" << deviceName
         << "\" for " << (isEncoding ? "record" : "play")
         << "ing: " << soundChannel->GetErrorText());

  delete soundChannel;

#endif

  return FALSE;
}

#endif // H323_AUDIO_CODECS


#ifndef NO_H323_VIDEO
BOOL H323EndPoint::OpenVideoChannel(H323Connection & /*connection*/,
                                    BOOL PTRACE_PARAM(isEncoding),
                                    H323VideoCodec & /*codec*/)
{
  PTRACE(1, "Codec\tCould not open video channel for "
         << (isEncoding ? "captur" : "display")
         << "ing: not yet implemented");
  return FALSE;
}

#ifdef H323_H239
BOOL H323EndPoint::OpenExtendedVideoChannel(H323Connection & /*connection*/,
											BOOL PTRACE_PARAM(isEncoding),
											H323VideoCodec & /*codec*/)
{
  PTRACE(1, "Codec\tCould not open extended video channel for "
         << (isEncoding ? "captur" : "display")
         << "ing: not yet implemented");
  return FALSE;
}
#endif // H323_H239
#endif // NO_H323_VIDEO


void H323EndPoint::OnRTPStatistics(const H323Connection & /*connection*/,
                                   const RTP_Session & /*session*/) const
{
}

void H323EndPoint::OnRTPFinalStatistics(const H323Connection & /*connection*/,
                                   const RTP_Session & /*session*/) const
{
}


void H323EndPoint::OnUserInputString(H323Connection & /*connection*/,
                                     const PString & /*value*/)
{
}


void H323EndPoint::OnUserInputTone(H323Connection & connection,
                                   char tone,
                                   unsigned /*duration*/,
                                   unsigned /*logicalChannel*/,
                                   unsigned /*rtpTimestamp*/)
{
  // don't pass through signalUpdate messages
  if (tone != ' ')
    connection.OnUserInputString(PString(tone));
}

#ifdef H323_GNUGK
void H323EndPoint::OnGatekeeperNATDetect(
                                   PIPSocket::Address publicAddr,   
                                   PString & gkIdentifier,
				           H323TransportAddress & gkRouteAddress
                                   )
{
	if (gnugk != NULL) {
		if (gnugk->ReRegister(gkIdentifier))
			return;
		else {
		   	PTRACE(4, "GNUGK\tReRegistration Failure. Attempting new connection");
			if (!gnugk->CreateNewTransport()) {
			  PTRACE(4, "GNUGK\tNAT Support Failure: Retry from scratch");	
			   delete gnugk;
			   gnugk = NULL;
			}
		}
	}

	gnugk = new GNUGK_Feature(*this,gkRouteAddress,gkIdentifier);

	 if (gnugk->IsOpen()) {
 	     PTRACE(4, "GNUGK\tNat Address " << gkRouteAddress);

		 PNatMethod_GnuGk * natMethod = new PNatMethod_GnuGk();
		 natMethod->AttachEndPoint(this);
	     natMethods.AddMethod(natMethod);
		 return; 
	 } 

	  PTRACE(4, "GNUGK\tConnection failed. Disabling support.");
	  delete gnugk;
      gnugk = NULL;
}

void H323EndPoint::OnGatekeeperOpenNATDetect(
                                   PString & /*gkIdentifier*/,
				   H323TransportAddress & /*gkRouteAddress*/
                                   )
{
}
#endif

BOOL H323EndPoint::OnGatekeeperAliases(
		const H225_ArrayOf_AliasAddress & /*aliases*/  
		                              )
{
	return FALSE;
}

#ifdef H323_H248

void H323EndPoint::OnHTTPServiceControl(unsigned /*opeartion*/,
                                        unsigned /*sessionId*/,
                                        const PString & /*url*/)
{
}

void H323EndPoint::OnCallCreditServiceControl(const PString & amount, BOOL mode, const unsigned & /*durationLimit*/)
{
    OnCallCreditServiceControl(amount, mode);
}

void H323EndPoint::OnCallCreditServiceControl(const PString & /*amount*/, BOOL /*mode*/)
{
}

#ifdef H323_H350
void H323EndPoint::OnH350ServiceControl(const PString & /*url*/,
										const PString & /*BaseDN*/)
{
}
#endif

void H323EndPoint::OnServiceControlSession(unsigned type,
                                           unsigned sessionId,
                                           const H323ServiceControlSession & session,
                                           H323Connection * connection)
{
  session.OnChange(type, sessionId, *this, connection);
}

H323ServiceControlSession * H323EndPoint::CreateServiceControlSession(const H225_ServiceControlDescriptor & contents)
{
  switch (contents.GetTag()) {
    case H225_ServiceControlDescriptor::e_url :
      return new H323HTTPServiceControl(contents);

    case H225_ServiceControlDescriptor::e_callCreditServiceControl :
      return new H323CallCreditServiceControl(contents);
#ifdef H323_H350
	case H225_ServiceControlDescriptor::e_nonStandard  :
      return new H323H350ServiceControl(contents);
#endif
  }

  return NULL;
}

#endif // H323_H248

BOOL H323EndPoint::OnConferenceInvite(BOOL /*sending*/,                  
      const H323Connection * /*connection*/,  
      const H323SignalPDU & /*setupPDU */)
{
  return FALSE;
}

BOOL H323EndPoint::OnSendCallIndependentSupplementaryService(const H323Connection * /*connection*/, 
														     H323SignalPDU & /* pdu */)
{
  return FALSE;
}

BOOL H323EndPoint::OnReceiveCallIndependentSupplementaryService(const H323Connection * /*connection*/, 
														        const H323SignalPDU & /* pdu */)
{
  return FALSE;
}

BOOL H323EndPoint::OnNegotiateConferenceCapabilities(const H323SignalPDU & /* setupPDU */)
{
  return FALSE;
}

#ifdef H323_T120
OpalT120Protocol * H323EndPoint::CreateT120ProtocolHandler(const H323Connection &) const
{
  return NULL;
}
#endif


#ifdef H323_T38
OpalT38Protocol * H323EndPoint::CreateT38ProtocolHandler(const H323Connection &) const
{
  return NULL;
}
#endif

#ifdef H323_H224

OpalH224Handler * H323EndPoint::CreateH224ProtocolHandler(H323Connection & connection, 
														  unsigned sessionID) const
{
  return new OpalH224Handler(connection, sessionID);
}


OpalH281Handler * H323EndPoint::CreateH281ProtocolHandler(OpalH224Handler & h224Handler) const
{
  return new OpalH281Handler(h224Handler);
}

#endif

#ifdef H323_FILE
BOOL H323EndPoint::OpenFileTransferSession( const PString & token, H323ChannelNumber & num)
{
  H323Connection * connection = FindConnectionWithLock(token);
 
  BOOL success = FALSE;
  if (connection != NULL) {
    success = connection->OpenFileTransferSession(num);
    connection->Unlock();
  }

  return success;
}

BOOL H323EndPoint::OpenFileTransferChannel(H323Connection & connection,
											 H323Channel::Directions dir,
						                     H323FileTransferList & filelist
											) 
{
   PTRACE(2,"FT\tAttempt to open File Transfer session! Not implemented Yet!");
   return FALSE;
}
#endif

void H323EndPoint::SetLocalUserName(const PString & name)
{
  PAssert(!name, "Must have non-empty string in AliasAddress!");
  if (name.IsEmpty())
    return;

  localAliasNames.RemoveAll();
  localAliasNames.AppendString(name);
}


BOOL H323EndPoint::AddAliasName(const PString & name)
{
  PAssert(!name, "Must have non-empty string in AliasAddress!");

  if (localAliasNames.GetValuesIndex(name) != P_MAX_INDEX)
    return FALSE;

  localAliasNames.AppendString(name);
  return TRUE;
}


BOOL H323EndPoint::RemoveAliasName(const PString & name)
{
  PINDEX pos = localAliasNames.GetValuesIndex(name);
  if (pos == P_MAX_INDEX)
    return FALSE;

  PAssert(localAliasNames.GetSize() > 1, "Must have at least one AliasAddress!");
  if (localAliasNames.GetSize() < 2)
    return FALSE;

  localAliasNames.RemoveAt(pos);
  return TRUE;
}

#ifdef H323_AUDIO_CODECS

#ifdef P_AUDIO

BOOL H323EndPoint::SetSoundChannelPlayDevice(const PString & name)
{
  if (PSoundChannel::GetDeviceNames(soundChannelPlayDriver, PSoundChannel::Player).GetValuesIndex(name) == P_MAX_INDEX)
    return FALSE;

  soundChannelPlayDevice = name;
  return TRUE;
}


BOOL H323EndPoint::SetSoundChannelRecordDevice(const PString & name)
{
  if (PSoundChannel::GetDeviceNames(soundChannelRecordDriver, PSoundChannel::Recorder).GetValuesIndex(name) == P_MAX_INDEX)
    return FALSE;

  soundChannelRecordDevice = name;
  return TRUE;
}


BOOL H323EndPoint::SetSoundChannelPlayDriver(const PString & name)
{
  PPluginManager & pluginMgr = PPluginManager::GetPluginManager(); 
  PStringList list = pluginMgr.GetPluginsProviding("PSoundChannel");
  if (list.GetValuesIndex(name) == P_MAX_INDEX)
    return FALSE;

  soundChannelPlayDriver = name;
  soundChannelPlayDevice.MakeEmpty();
  list = PSoundChannel::GetDeviceNames(name, PSoundChannel::Player);
  if (list.GetSize() == 0)
    return FALSE;

  soundChannelPlayDevice = list[0];
  return TRUE;
}


BOOL H323EndPoint::SetSoundChannelRecordDriver(const PString & name)
{
  PPluginManager & pluginMgr = PPluginManager::GetPluginManager(); 
  PStringList list = pluginMgr.GetPluginsProviding("PSoundChannel");
  if (list.GetValuesIndex(name) == P_MAX_INDEX)
    return FALSE;

  soundChannelRecordDriver = name;
  list = PSoundChannel::GetDeviceNames(name, PSoundChannel::Recorder);
  if (list.GetSize() == 0)
    return FALSE;

  PTRACE(1,"SetSoundChannelRecordDriver\tAvailable devices: " << list);

  soundChannelRecordDevice = list[0];
  return TRUE;
}


void H323EndPoint::SetSoundChannelBufferDepth(unsigned depth)
{
  PAssert(depth > 1, PInvalidParameter);
  soundChannelBuffers = depth;
}

#endif  // P_AUDIO

#endif  // H323_AUDIO_CODECS


BOOL H323EndPoint::IsTerminal() const
{
  switch (terminalType) {
    case e_TerminalOnly :
    case e_TerminalAndMC :
      return TRUE;

    default :
      return FALSE;
  }
}


BOOL H323EndPoint::IsGateway() const
{
  switch (terminalType) {
    case e_GatewayOnly :
    case e_GatewayAndMC :
    case e_GatewayAndMCWithDataMP :
    case e_GatewayAndMCWithAudioMP :
    case e_GatewayAndMCWithAVMP :
      return TRUE;

    default :
      return FALSE;
  }
}


BOOL H323EndPoint::IsGatekeeper() const
{
  switch (terminalType) {
    case e_GatekeeperOnly :
    case e_GatekeeperWithDataMP :
    case e_GatekeeperWithAudioMP :
    case e_GatekeeperWithAVMP :
      return TRUE;

    default :
      return FALSE;
  }
}


BOOL H323EndPoint::IsMCU() const
{
  switch (terminalType) {
    case e_MCUOnly :
    case e_MCUWithDataMP :
    case e_MCUWithAudioMP :
    case e_MCUWithAVMP :
      return TRUE;

    default :
      return FALSE;
  }
}

#ifdef H323_AUDIO_CODECS

void H323EndPoint::SetAudioJitterDelay(unsigned minDelay, unsigned maxDelay)
{
  if (minDelay == 0 && maxDelay == 0) {
    // Disable jitter buffer
    minAudioJitterDelay = 0;
    maxAudioJitterDelay = 0;
    return;
  }

  PAssert(minDelay <= 10000 && maxDelay <= 10000, PInvalidParameter);

  if (minDelay < 10)
    minDelay = 10;
  minAudioJitterDelay = minDelay;

  if (maxDelay < minDelay)
    maxDelay = minDelay;
  maxAudioJitterDelay = maxDelay;
}

#endif

#ifdef P_STUN

PSTUNClient * H323EndPoint::GetSTUN(const PIPSocket::Address & ip) const
{
  if (ip.IsValid() && IsLocalAddress(ip))
    return NULL;

  return stun;
}

PNatMethod * H323EndPoint::GetPreferedNatMethod(const PIPSocket::Address & ip)
{

  if (ip.IsValid() && IsLocalAddress(ip))
    return NULL;

#if PTRACING
    PNatMethod * meth = NULL;
    PNatList list = natMethods.GetNATList();

    if (list.GetSize() > 0) {
      for (PINDEX i=0; i < list.GetSize(); i++) {
        PTRACE(6, "H323\tNAT Method " << i << " " << list[i].GetName()[0] << " Ready: " << (list[i].IsAvailable() ? "Yes" : "No"));   
        if (list[i].IsAvailable()) {
             meth = &list[i];
             break;
        }
      }
    } else {
       PTRACE(6, "H323\tNo NAT Methods!");
    }
    return meth;
#else 
  return natMethods.GetMethod();
#endif

}

PNatStrategy H323EndPoint::GetNatMethods() 
{ 
	return natMethods; 
}

void H323EndPoint::SetSTUNServer(const PString & server)
{
  natMethods.RemoveMethod("STUN");
  delete stun;

  if (server.IsEmpty())
    stun = NULL;
  else {
    stun = new PSTUNClient(server,
                           GetUDPPortBase(), GetUDPPortMax(),
                           GetRtpIpPortBase(), GetRtpIpPortMax());

    natMethods.AddMethod(stun);

    PTRACE(2, "H323\tSTUN server \"" << server << "\" replies " << stun->GetNatTypeName());
	
	STUNNatType((int)stun->GetNatType());
  }
}

#endif // P_STUN

void H323EndPoint::InternalTranslateTCPAddress(PIPSocket::Address & localAddr, const PIPSocket::Address & remoteAddr, 
											   const H323Connection * connection)
{
  if (remoteAddr.GetVersion() != 4)
      return;

#ifdef P_STUN
#if PTLIB_VER > PTLIB_VERSION_INT(2,0,1)
  // if using NAT Method, then translate internal local address to external if required
  if (connection && !connection->HasNATSupport())
      return;

  if (localAddr.IsRFC1918() && !remoteAddr.IsRFC1918()) {
      if (!connection) {
#if PTLIB_VER > PTLIB_VERSION_INT(2,8,0)
        PNatMethod * stun = GetNatMethods().GetMethodByName("STUN");
#else
        PNatMethod * stun = GetNatMethods().GetMethod();
#endif
        if (stun && stun->IsAvailable(remoteAddr) && stun->GetExternalAddress(localAddr)) {
           PTRACE(2,"EP\tSTUN set localIP as " << localAddr);
        } else {
            //const PNatList & list = natMethods->GetNATList();
            const PNatList & list = natMethods.GetNATList();
              for (PINDEX i=0; i < list.GetSize(); i++) {
                  if (list[i].IsAvailable(remoteAddr) && list[i].GetExternalAddress(localAddr)) {
                     PTRACE(2,"EP\tNATMethod " << list[i].GetName() << " rewrite localIP as " << localAddr);
                     break;
                  }
              }
        }
      }
   }
  else
#else
  // if using STUN server, then translate internal local address to external if required
  BOOL disableSTUN;
  if (connection != NULL)
    disableSTUN = !connection->HasNATSupport();
  else
    disableSTUN = disableSTUNTranslate;

  PIPSocket::Address addr;
  if (
       stun != NULL && !disableSTUN && (
        (stun->IsSupportingRTP() == PSTUNClient::RTPOK) ||
        (stun->IsSupportingRTP() == PSTUNClient::RTPIfSendMedia) 
       ) && 
       localAddr.IsRFC1918() && 
       !remoteAddr.IsRFC1918() && 
       stun->GetExternalAddress(addr)
     )
  {
    localAddr = addr;
  }
  else
#endif // PTLIB_VER
#endif // P_STUN
     TranslateTCPAddress(localAddr, remoteAddr);
}

BOOL H323EndPoint::IsLocalAddress(const PIPSocket::Address & ip) const
{
  /* Check if the remote address is a private IP, broadcast, or us */
  return ip.IsRFC1918() || ip.IsBroadcast() || PIPSocket::IsLocalHost(ip);
}


void H323EndPoint::PortInfo::Set(unsigned newBase,
                                 unsigned newMax,
                                 unsigned range,
                                 unsigned dflt)
{
  if (newBase == 0) {
    newBase = dflt;
    newMax = dflt;
    if (dflt > 0)
      newMax += range;
  }
  else {
    if (newBase < 1024)
      newBase = 1024;
    else if (newBase > 65500)
      newBase = 65500;

    if (newMax <= newBase)
      newMax = newBase + range;
    if (newMax > 65535)
      newMax = 65535;
  }

  mutex.Wait();

  current = base = (WORD)newBase;
  max = (WORD)newMax;

  mutex.Signal();
}


WORD H323EndPoint::PortInfo::GetNext(unsigned increment)
{
  PWaitAndSignal m(mutex);

  if (current < base || current > (max-increment))
    current = base;

  if (current == 0)
    return 0;

  WORD p = current;
  current = (WORD)(current + increment);
  return p;
}


void H323EndPoint::SetTCPPorts(unsigned tcpBase, unsigned tcpMax)
{
  tcpPorts.Set(tcpBase, tcpMax, 99, 0);
}


WORD H323EndPoint::GetNextTCPPort()
{
  return tcpPorts.GetNext(1);
}


void H323EndPoint::SetUDPPorts(unsigned udpBase, unsigned udpMax)
{
  udpPorts.Set(udpBase, udpMax, 199, 0);

#ifdef P_STUN
    natMethods.SetPortRanges(GetUDPPortBase(), GetUDPPortMax(), GetRtpIpPortBase(), GetRtpIpPortMax());
#endif
}


WORD H323EndPoint::GetNextUDPPort()
{
  return udpPorts.GetNext(1);
}


void H323EndPoint::SetRtpIpPorts(unsigned rtpIpBase, unsigned rtpIpMax)
{
  rtpIpPorts.Set((rtpIpBase+1)&0xfffe, rtpIpMax&0xfffe, 999, 5000);

#ifdef P_STUN
     natMethods.SetPortRanges(GetUDPPortBase(), GetUDPPortMax(), GetRtpIpPortBase(), GetRtpIpPortMax());
#endif
}


WORD H323EndPoint::GetRtpIpPortPair()
{
  return rtpIpPorts.GetNext(2);
}

const PTimeInterval & H323EndPoint::GetNoMediaTimeout() const
{
  PWaitAndSignal m(noMediaMutex);
  
  return noMediaTimeout; 
}

BOOL H323EndPoint::SetNoMediaTimeout(PTimeInterval newInterval) 
{
  PWaitAndSignal m(noMediaMutex);

  if (newInterval < 0)
    return FALSE;

  noMediaTimeout = newInterval; 
  return TRUE; 
}

BOOL H323EndPoint::OnSendFeatureSet(unsigned pdu, H225_FeatureSet & feats)
{
#ifdef H323_H460
    return features.SendFeature(pdu,feats);
#else
    return FALSE;
#endif
}

void H323EndPoint::OnReceiveFeatureSet(unsigned pdu, const H225_FeatureSet & feats)
{
#ifdef H323_H460
	features.ReceiveFeature(pdu,feats);
#endif
}

void H323EndPoint::LoadBaseFeatureSet()
{

#ifdef H323_H460
  features.AttachEndPoint(this);
  features.LoadFeatureSet(H460_Feature::FeatureBase);
#endif

}

BOOL H323EndPoint::HandleUnsolicitedInformation(const H323SignalPDU & )
{
  return FALSE;
}

#ifdef H323_RTP_AGGREGATE
PHandleAggregator * H323EndPoint::GetRTPAggregator()
{
  PWaitAndSignal m(connectionsMutex);
  if (rtpAggregationSize == 0)
    return NULL;

  if (rtpAggregator == NULL)
    rtpAggregator = new PHandleAggregator(rtpAggregationSize);

  return rtpAggregator;
}
#endif

#ifdef H323_SIGNAL_AGGREGATE
PHandleAggregator * H323EndPoint::GetSignallingAggregator()
{
  PWaitAndSignal m(connectionsMutex);
  if (signallingAggregationSize == 0)
    return NULL;

  if (signallingAggregator == NULL)
    signallingAggregator = new PHandleAggregator(signallingAggregationSize);

  return signallingAggregator;
}
#endif

#ifdef H323_GNUGK
void H323EndPoint::NATLostConnection(BOOL lost)
{
	PTRACE(4,"GNUGK\tNAT Connection" << (lost ? "Lost" : " Re-established"));
	if (!lost)
		RegInvokeReRegistration();  
}

void H323EndPoint::RegInvokeReRegistration()
{
	 RegThread = PThread::Create(PCREATE_NOTIFIER(RegMethod), 0,
					PThread::AutoDeleteThread,
					PThread::NormalPriority,
					"regmeth:%x");
}

void H323EndPoint::RegMethod(PThread &, INT)
{
	PWaitAndSignal m(reregmutex);

	PTRACE(4,"GNUGK\tForcing ReRegistration");
	gatekeeper->ReRegisterNow();
}
#endif

