/*
 * transports.cxx
 *
 * H.323 transports handler
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
 * $Log: transports.cxx,v $
 * Revision 1.1  2007/08/06 20:51:08  shorne
 * First commit of h323plus
 *
 * Revision 1.161.2.2  2007/07/22 14:19:20  shorne
 * Removed initialisation warning under windows
 *
 * Revision 1.161.2.1  2007/02/11 00:45:20  shorne
 * Added ability to disable NAT method on a call by call basis
 *
 * Revision 1.161  2006/08/23 06:43:15  csoutheren
 * Improved locking for calls that end very quickly after starting
 *
 * Revision 1.160  2006/07/18 07:10:47  csoutheren
 * Set default listener port rather than using zero
 *
 * Revision 1.159  2006/07/05 04:37:44  csoutheren
 * Applied 1488904 - SetPromiscuous(AcceptFromLastReceivedOnly) for T.38
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.158  2006/06/23 06:02:44  csoutheren
 * Added missing declarations for H.224 backport
 *
 * Revision 1.157  2006/06/23 03:20:21  shorne
 * Added unsolicited Information support
 *
 * Revision 1.156  2006/03/02 07:52:51  csoutheren
 * Ensure prompt close of channels when using aggregation
 * Ensure MonitorCallStatus called when using aggregation
 *
 * Revision 1.155  2006/01/30 06:49:07  csoutheren
 * Removed warning on Linux
 *
 * Revision 1.154  2006/01/30 01:11:27  csoutheren
 * Fixed compile warning on Linux
 *
 * Revision 1.153  2006/01/26 03:38:54  shorne
 * Added transport capability exchange
 *
 * Revision 1.152  2006/01/24 08:15:24  csoutheren
 * Implement outgoing H.225 aggregation, and H.245 aggregation (disabled by default)
 * More testing to do, but this looks good :)
 *
 * Revision 1.151  2006/01/23 05:58:26  csoutheren
 * Working outgoing H.225 aggregation (disabled by default)
 *
 * Revision 1.150  2006/01/20 00:32:24  csoutheren
 * First check-in of signalling aggregation code - incomplete and disabled by default
 *
 * Revision 1.149  2005/11/25 00:37:45  csoutheren
 * Applied patch #1274826 from Paul Nader
 * HandleFirstSignallingChannelPDU incomplete ReleaseComplete
 *
 * Revision 1.148  2005/11/22 03:38:46  shorne
 * Added ToS support to TCP Transports. thx Norbert Bartalsky (TOPCALL)
 *
 * Revision 1.147  2005/09/16 08:11:06  csoutheren
 * Added new error code
 * Added override for OnIncomingCall that can set return code
 *
 * Revision 1.146  2005/03/07 02:10:14  csoutheren
 * Fixed more problems with locahost detection
 *
 * Revision 1.145  2005/03/04 03:22:12  csoutheren
 * Fixed problem with multi-homed hosts sending localhost in some gatekeeper PDUs
 *
 * Revision 1.144  2005/01/16 20:39:44  csoutheren
 * Fixed problem with IPv6 INADDR_ANY
 *
 * Revision 1.143  2005/01/03 14:03:43  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.142  2005/01/03 06:26:09  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.141  2004/12/02 00:24:26  csoutheren
 * Fixed problem with gatekeeper getting requests from localhost
 *
 * Revision 1.140  2004/11/25 07:38:59  csoutheren
 * Ensured that external TCP address translation is performed when using STUN to handle UDP
 *
 * Revision 1.139  2004/08/24 08:11:26  csoutheren
 * Added initial support for receiving broadcasts on Linux
 *
 * Revision 1.138  2004/07/11 08:09:14  csoutheren
 * Patch for VxWorks thanks to Eize Slange
 *
 * Revision 1.137  2004/04/25 22:23:22  ykiryanov
 * Removed couple of ifdef BEOS. More functionality
 *
 * Revision 1.136  2004/04/03 08:28:07  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.135  2003/12/03 06:57:11  csoutheren
 * Protected against dwarf Q.931 PDUs
 *
 * Revision 1.134  2003/04/10 09:45:34  robertj
 * Added associated transport to new GetInterfaceAddresses() function so
 *   interfaces can be ordered according to active transport links. Improves
 *   interoperability.
 * Replaced old listener GetTransportPDU() with GetInterfaceAddresses()
 *   and H323SetTransportAddresses() functions.
 *
 * Revision 1.133  2003/04/10 00:58:54  craigs
 * Added functions to access to lists of interfaces
 *
 * Revision 1.132  2003/03/26 06:14:31  robertj
 * More IPv6 support (INADDR_ANY handling), thanks Sébastien Josset
 *
 * Revision 1.131  2003/03/21 05:24:54  robertj
 * Added setting of remote port in UDP transport constructor.
 *
 * Revision 1.130  2003/03/20 01:51:12  robertj
 * More abstraction of H.225 RAS and H.501 protocols transaction handling.
 *
 * Revision 1.129  2003/03/11 23:15:23  robertj
 * Fixed possible double delete of socket (crash) on garbage input.
 *
 * Revision 1.128  2003/02/06 04:31:02  robertj
 * Added more support for adding things to H323TransportAddressArrays
 *
 * Revision 1.127  2003/02/05 01:57:18  robertj
 * Fixed STUN usage on gatekeeper discovery.
 *
 * Revision 1.126  2003/02/04 07:06:42  robertj
 * Added STUN support.
 *
 * Revision 1.125  2003/01/23 02:36:32  robertj
 * Increased (and made configurable) timeout for H.245 channel TCP connection.
 *
 * Revision 1.124  2002/12/23 22:46:06  robertj
 * Changed gatekeeper discovery so an GRJ does not indicate "discovered".
 *
 * Revision 1.123  2002/11/21 06:40:00  robertj
 * Changed promiscuous mode to be three way. Fixes race condition in gkserver
 *   which can cause crashes or more PDUs to be sent to the wrong place.
 *
 * Revision 1.122  2002/11/12 03:14:18  robertj
 * Fixed gatekeeper discovery so does IP address translation correctly for
 *   hosts inside the firewall.
 *
 * Revision 1.121  2002/11/10 08:10:44  robertj
 * Moved constants for "well known" ports to better place (OPAL change).
 *
 * Revision 1.120  2002/11/05 00:31:48  robertj
 * Prevented a failure to start separate H.245 channel stopping the call until
 *   after a CONNECT is received and have no audio. At that point no H.245
 *   is a useless call and we disconnect.
 *
 * Revision 1.119  2002/11/01 03:38:18  robertj
 * More IPv6 fixes, thanks Sébastien Josset.
 *
 * Revision 1.118  2002/10/29 08:30:32  robertj
 * Fixed problem with simultaneous startH245 condition possibly shutting down
 *   the call under some circumstances.
 *
 * Revision 1.117  2002/10/16 06:28:20  robertj
 * More IPv6 support changes, especially in unambiguising v6 addresses colons
 *   from the port fields colon, thanks Sebastien Josset
 *
 * Revision 1.116  2002/10/08 23:34:30  robertj
 * Fixed ip v6 usage on H.245 pdu setting.
 *
 * Revision 1.115  2002/10/08 13:08:21  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.114  2002/08/05 10:03:48  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.113  2002/08/05 05:17:41  robertj
 * Fairly major modifications to support different authentication credentials
 *   in ARQ to the logged in ones on RRQ. For both client and server.
 * Various other H.235 authentication bugs and anomalies fixed on the way.
 *
 * Revision 1.112  2002/07/22 09:40:19  robertj
 * Added ability to automatically convert string arrays, lists sorted lists
 *   directly to H323TransportAddressArray.
 *
 * Revision 1.111  2002/07/18 08:25:47  robertj
 * Fixed problem in decoding host when '+' was used without port in address.
 *
 * Revision 1.110  2002/07/10 01:23:33  robertj
 * Added extra debugging output
 *
 * Revision 1.109  2002/07/02 10:02:32  robertj
 * Added H323TransportAddress::GetIpAddress() so don't have to provide port
 *   when you don't need it as in GetIpAndPort(),.
 *
 * Revision 1.108  2002/06/28 03:34:29  robertj
 * Fixed issues with address translation on gatekeeper RAS channel.
 *
 * Revision 1.107  2002/06/24 07:35:23  robertj
 * Fixed ability to do gk discovery on localhost, thanks Artis Kugevics
 *
 * Revision 1.106  2002/06/12 03:52:27  robertj
 * Added function to compare two transport addresses in a more intelligent
 *   way that strict string comparison. Takes into account wildcarding.
 *
 * Revision 1.105  2002/05/28 06:38:08  robertj
 * Split UDP (for RAS) from RTP port bases.
 * Added current port variable so cycles around the port range specified which
 *   fixes some wierd problems on some platforms, thanks Federico Pinna
 *
 * Revision 1.104  2002/05/22 07:39:59  robertj
 * Fixed double increment of port number when making outgoing TCP connection.
 *
 * Revision 1.103  2002/04/18 00:18:58  robertj
 * Increased timeout for thread termination assert, on heavily loaded machines it can
 *   take more than one second to complete.
 *
 * Revision 1.102  2002/04/17 05:36:38  robertj
 * Fixed problems with using pre-bound inferface/port in gk discovery.
 *
 * Revision 1.101  2002/04/12 04:51:28  robertj
 * Fixed small possibility crashes if open and close transport at same time.
 *
 * Revision 1.100  2002/03/08 01:22:30  robertj
 * Fixed possible use of IsSuspended() on terminated thread causing assert.
 *
 * Revision 1.99  2002/03/05 04:49:41  robertj
 * Fixed leak of thread (and file handles) if get incoming connection aborted
 *   very early (before receiving a setup PDU), thanks Hans Bjurström
 *
 * Revision 1.98  2002/02/28 04:35:43  robertj
 * Added trace output of the socket handle number when have new connection.
 *
 * Revision 1.97  2002/02/28 00:57:03  craigs
 * Changed SetWriteTimeout to SetReadTimeout in connect, as Craig got it wrong!
 *
 * Revision 1.96  2002/02/25 10:55:33  robertj
 * Added ability to speficy dynamically allocated port in transport address.
 *
 * Revision 1.95  2002/02/14 03:36:14  craigs
 * Added default 10sec timeout on connect to IP addresses
 * This prevents indefinite hangs when connecting to IP addresses
 * that don't exist
 *
 * Revision 1.94  2002/02/05 23:29:09  robertj
 * Changed default for H.323 listener to reuse addresses.
 *
 * Revision 1.93  2002/02/01 01:48:18  robertj
 * Fixed ability to shut down a Listener, if it had never been started.
 *
 * Revision 1.92  2002/01/02 06:06:43  craigs
 * Made T.38 UDP socket obey rules
 *
 * Revision 1.91  2001/12/22 01:48:40  robertj
 * Added ability to use local and remote port from transport channel as well
 *   as explicit port in H.245 address PDU setting routine.
 * Added PrintOn() to listener and transport for tracing purposes.
 *
 * Revision 1.90  2001/12/15 07:12:22  robertj
 * Added optimisation so if discovering a static gk on same machine as ep is
 *   running on then uses that specific interface preventing multiple GRQs.
 *
 * Revision 1.89  2001/10/11 07:16:49  robertj
 * Removed port check for gk's that change sockets in mid-stream.
 *
 * Revision 1.88  2001/10/09 12:41:20  robertj
 * Set promiscuous flag back to FALSE after gatkeeper discovery.
 *
 * Revision 1.87  2001/09/10 03:06:29  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.86  2001/08/10 11:03:52  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 * Revision 1.85  2001/08/07 02:57:52  robertj
 * Improved tracing on closing transport.
 *
 * Revision 1.84  2001/08/06 03:08:57  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.83  2001/07/17 04:44:32  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 * Revision 1.82  2001/07/06 02:31:15  robertj
 * Made sure a release complete is sent if no connection is created.
 *
 * Revision 1.81  2001/07/04 09:02:07  robertj
 * Added more tracing
 *
 * Revision 1.80  2001/06/25 05:50:22  robertj
 * Improved error logging on TCP listener.
 *
 * Revision 1.79  2001/06/25 02:28:34  robertj
 * Allowed TCP listener socket to be opened in non-exclusive mode
 *   (ie SO_REUSEADDR) to avoid daemon restart problems.
 * Added trailing '+' on H323TransportAddress string to invoke above.
 *
 * Revision 1.78  2001/06/22 02:47:12  robertj
 * Took one too many lines out in previous fix!
 *
 * Revision 1.77  2001/06/22 02:40:27  robertj
 * Fixed discovery so uses new promiscuous mode.
 * Also used the RAS GRQ address of server isntead of UDP return address
 *   for address of gatekeeper for future packets.
 *
 * Revision 1.76  2001/06/22 01:54:47  robertj
 * Removed initialisation of localAddress to hosts IP address, does not
 *   work on multi-homed hosts.
 *
 * Revision 1.75  2001/06/22 00:14:46  robertj
 * Added ConnectTo() function to conencto specific address.
 * Added promiscuous mode for UDP channel.
 *
 * Revision 1.74  2001/06/14 23:18:06  robertj
 * Change to allow for CreateConnection() to return NULL to abort call.
 *
 * Revision 1.73  2001/06/14 04:23:32  robertj
 * Changed incoming call to pass setup pdu to endpoint so it can create
 *   different connection subclasses depending on the pdu eg its alias
 *
 * Revision 1.72  2001/06/06 00:29:54  robertj
 * Added trace for when doing TCP connect.
 *
 * Revision 1.71  2001/06/02 01:35:32  robertj
 * Added thread names.
 *
 * Revision 1.70  2001/05/31 07:16:52  craigs
 * Fixed remote address initialisation for incoming H245 channels
 *
 * Revision 1.69  2001/05/17 06:37:04  robertj
 * Added multicast gatekeeper discovery support.
 *
 * Revision 1.68  2001/04/13 07:44:51  robertj
 * Moved starting connection trace message to be on both Connect() and Accept()
 *
 * Revision 1.67  2001/04/10 01:21:02  robertj
 * Added some more error messages into trace log.
 *
 * Revision 1.66  2001/04/09 08:44:19  robertj
 * Added ability to get transport address for a listener.
 * Added '*' to indicate INADDR_ANY ip address.
 *
 * Revision 1.65  2001/03/06 05:03:00  robertj
 * Changed H.245 channel start failure so does not abort call if there were
 *   some fast started media streams opened. Just lose user indications.
 *
 * Revision 1.64  2001/03/05 04:28:50  robertj
 * Added net mask to interface info returned by GetInterfaceTable()
 *
 * Revision 1.63  2001/02/09 05:13:56  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.62  2001/02/08 22:29:39  robertj
 * Fixed failure to reset fill character in trace log when output interface list.
 *
 * Revision 1.61  2001/01/29 06:43:32  robertj
 * Added printing of entry of interface table.
 *
 * Revision 1.60  2001/01/23 05:06:52  robertj
 * Fixed bug when trying to clear call while waiting on H.245 channel connect, thanks Yura Aksyonov.
 * Fixed missing increment in H.245 listener when specified local port number range.
 *
 * Revision 1.59  2001/01/11 06:41:01  craigs
 * Fixed problem with outgoing sockets failing because of port in use
 *
 * Revision 1.58  2000/12/18 08:59:20  craigs
 * Added ability to set ports
 *
 * Revision 1.57  2000/11/06 06:19:19  robertj
 * Removed reverse DNS lookup for host name as this can cause HUGE delays in
 *    call setup making it look like a call failure.
 *
 * Revision 1.56  2000/10/20 06:18:49  robertj
 * Fixed very small race condition on creating new connectionon incoming call.
 * Fixed memory/socket leak if do TCP connect and don't send valid setup PDU.
 *
 * Revision 1.55  2000/10/04 12:21:07  robertj
 * Changed setting of callToken in H323Connection to be as early as possible.
 *
 * Revision 1.54  2000/10/04 05:59:26  robertj
 * Minor reorganisation of the H.245 secondary channel start up to make it simpler
 *    to override its behaviour.
 *
 * Revision 1.53  2000/09/25 12:59:34  robertj
 * Added StartListener() function that takes a H323TransportAddress to start
 *     listeners bound to specific interfaces.
 *
 * Revision 1.52  2000/08/31 13:14:10  robertj
 * Increased timeout on assert for stuck transport thread.
 *
 * Revision 1.51  2000/08/25 01:10:28  robertj
 * Added assert if various thrads ever fail to terminate.
 *
 * Revision 1.50  2000/08/22 09:01:06  robertj
 * Fixed small window in which control channel could do write through NULL pointer.
 *
 * Revision 1.49  2000/07/10 16:11:49  robertj
 * Fixed inclusion of interfaces with no IP address in GK registration.
 * Fixed possible crash when closing connection during H.245 TCP connect wait.
 *
 * Revision 1.48  2000/06/20 02:38:28  robertj
 * Changed H323TransportAddress to default to IP.
 *
 * Revision 1.47  2000/06/07 05:48:06  robertj
 * Added call forwarding.
 *
 * Revision 1.46  2000/05/23 02:38:15  robertj
 * Shortened the linger onTCP close to 3 seconds, should be adequate for lst end session transmit.
 *
 * Revision 1.45  2000/05/22 05:21:36  robertj
 * Fixed race condition where controlChannel variable could be used before set.
 *
 * Revision 1.44  2000/05/10 05:14:53  robertj
 * Fixed memory leak when doing GK discovery.
 *
 * Revision 1.43  2000/05/08 14:07:35  robertj
 * Improved the provision and detection of calling and caller numbers, aliases and hostnames.
 *
 * Revision 1.42  2000/05/05 04:37:50  robertj
 * Changed TCP transmit of PDU to not use Nagle algorithm delay, this gives
 *     a significant performance benefit for packet based stuff on stream based TCP.
 *
 * Revision 1.41  2000/05/02 04:32:28  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.40  2000/04/19 01:59:21  robertj
 * Improved some debugging messages.
 * Added code to restore transport state if gatekeeper discovery fails.
 *
 * Revision 1.39  2000/04/14 17:32:39  robertj
 * Changed transport to return some error number when getting malformed TPKT.
 *
 * Revision 1.38  2000/04/11 03:57:25  robertj
 * Fixed uninitialised variabel giving random port numbers for some PDU's (rare).
 *
 * Revision 1.37  2000/04/10 17:41:46  robertj
 * Changed transport address so does not do reverse DNS lookup, can really slow things down on some systems.
 *
 * Revision 1.36  2000/03/29 02:14:46  robertj
 * Changed TerminationReason to CallEndReason to use correct telephony nomenclature.
 * Added CallEndReason for capability exchange failure.
 *
 * Revision 1.35  2000/03/25 02:02:25  robertj
 * Added adjustable caller name on connection by connection basis.
 *
 * Revision 1.34  2000/02/09 00:00:53  robertj
 * Fixed TCP listener socket error message going to stderr, should go through PTRACE.
 *
 * Revision 1.33  2000/01/29 07:13:33  robertj
 * Fixed possible incorect port being used when GK RAS send address set to the
 *    IP address of the responding GK, thanks Stefan Ditscheid.
 *
 * Revision 1.32  2000/01/07 08:24:01  robertj
 * Added transport independent MakeCall requiring change to transport string format
 *
 * Revision 1.31  1999/12/09 21:46:42  robertj
 * Fixed detection of bound interface in gatekeeper discovery (can't use getsockname!)
 *
 * Revision 1.30  1999/12/09 20:25:41  robertj
 * Changed UDP transport to only do gatekeeper discovery on bound interface (if present)
 *
 * Revision 1.29  1999/11/22 00:56:35  robertj
 * Improved reason display for connection failure.
 *
 * Revision 1.28  1999/11/18 12:51:20  robertj
 * Fixed bug that termination connections incorrectly while awaiting answer.
 *
 * Revision 1.27  1999/11/06 05:37:45  robertj
 * Complete rewrite of termination of connection to avoid numerous race conditions.
 *
 * Revision 1.26  1999/10/28 11:17:38  robertj
 * Fixed bug causing delete of deleted object, thanks Benny Prijono
 *
 * Revision 1.25  1999/10/16 03:47:49  robertj
 * Fixed termination of gatekeeper RAS thread problem
 *
 * Revision 1.24  1999/10/14 12:04:11  robertj
 * Fixed ability to hang up call when still doing TCP connect.
 *
 * Revision 1.23  1999/10/13 04:32:41  robertj
 * Fixed missing port number from gatekeeper discovery.
 * Added removal of redundent transport listeners in H225 PDU.
 *
 * Revision 1.22  1999/10/10 14:12:06  robertj
 * Fixed failure to clean up connection if call is refused by user.
 *
 * Revision 1.21  1999/10/10 08:59:47  robertj
 * no message
 *
 * Revision 1.20  1999/09/27 01:37:23  robertj
 * BeOS port issues
 *
 * Revision 1.19  1999/09/17 07:26:17  robertj
 * Fixed attempt to broadcast to down interfaces or ones not bound to IP protocol.
 *
 * Revision 1.18  1999/09/14 08:19:37  robertj
 * Fixed timeout on retry of gatekeeper discover and added more tracing.
 *
 * Revision 1.17  1999/09/14 06:52:54  robertj
 * Added better support for multi-homed client hosts.
 *
 * Revision 1.16  1999/09/10 09:43:59  robertj
 * Removed attempt at determining local interface for gatekeeper, so still has problem on multi-homed hosts.
 *
 * Revision 1.15  1999/09/02 15:25:39  robertj
 * Old GNU C ompiler compatibility
 *
 * Revision 1.14  1999/08/31 12:34:19  robertj
 * Added gatekeeper support.
 *
 * Revision 1.13  1999/08/31 11:37:30  robertj
 * Fixed problem with apparently randomly losing signalling channel.
 *
 * Revision 1.12  1999/08/25 05:12:23  robertj
 * Changed MakeCall, so immediately spawns thread, no black on TCP connect.
 *
 * Revision 1.11  1999/07/23 02:37:53  robertj
 * Fixed problems with hang ups and crash closes of connections.
 *
 * Revision 1.10  1999/07/22 14:34:16  robertj
 * Fixed shut down problem, terminate packets being flushed on exit.
 *
 * Revision 1.9  1999/07/15 14:45:36  robertj
 * Added propagation of codec open error to shut down logical channel.
 * Fixed control channel start up bug introduced with tunnelling.
 *
 * Revision 1.8  1999/07/14 06:06:14  robertj
 * Fixed termination problems (race conditions) with deleting connection object.
 *
 * Revision 1.7  1999/07/09 06:09:52  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.6  1999/06/25 10:25:35  robertj
 * Added maintentance of callIdentifier variable in H.225 channel.
 *
 * Revision 1.5  1999/06/22 13:42:05  robertj
 * Added user question on listener version to accept incoming calls.
 *
 * Revision 1.4  1999/06/14 08:42:30  robertj
 * GNU C++ compatibility
 *
 * Revision 1.3  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.2  1999/06/09 06:18:01  robertj
 * GCC compatibiltiy.
 *
 * Revision 1.1  1999/06/09 05:26:20  robertj
 * Major restructuring of classes.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "transports.h"
#endif

#include "transports.h"

#include "h323pdu.h"
#include "h323ep.h"
#include "gkclient.h"

#ifdef P_STUN
#include <ptclib/pstun.h>
 #ifdef _MSC_VER
  #pragma warning(disable : 4701)  // initialisation warning
 #endif
#endif

class H225TransportThread : public PThread
{
  PCLASSINFO(H225TransportThread, PThread)

  public:
    H225TransportThread(H323EndPoint & endpoint, H323Transport * transport);

  protected:
    void Main();

    H323Transport * transport;
};


class H245TransportThread : public PThread
{
  PCLASSINFO(H245TransportThread, PThread)

  public:
    H245TransportThread(H323EndPoint & endpoint,
                        H323Connection & connection,
                        H323Transport & transport);

  protected:
    void Main();

    H323Connection & connection;
    H323Transport  & transport;
#ifdef H323_SIGNAL_AGGREGATE
    BOOL useAggregator;
#endif
};


#define new PNEW


/////////////////////////////////////////////////////////////////////////////

H225TransportThread::H225TransportThread(H323EndPoint & ep, H323Transport * t)
  : PThread(ep.GetSignallingThreadStackSize(),
            AutoDeleteThread,
            NormalPriority,
            "H225 Answer:%0x"),
    transport(t)
{
  Resume();
}


void H225TransportThread::Main()
{
  PTRACE(3, "H225\tStarted incoming call thread");

  if (!transport->HandleFirstSignallingChannelPDU())
    delete transport;
}

/////////////////////////////////////////////////////////////////////////////

H245TransportThread::H245TransportThread(H323EndPoint & endpoint,
                                         H323Connection & c,
                                         H323Transport & t)
  : PThread(endpoint.GetSignallingThreadStackSize(),
            NoAutoDeleteThread,
            NormalPriority,
            "H245:%0x"),
    connection(c),
    transport(t)
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


void H245TransportThread::Main()
{
  PTRACE(3, "H245\tStarted thread");

  if (transport.AcceptControlChannel(connection)) {
#ifdef H323_SIGNAL_AGGREGATE
    // if the endpoint is using signalling aggregation, we need to add this connection
    // to the signalling aggregator. 
    if (useAggregator) {
      connection.AggregateControlChannel(&transport);
      SetAutoDelete(AutoDeleteThread);
      return;
    }
#endif

    connection.HandleControlChannel();
  }
}


/////////////////////////////////////////////////////////////////////////////

static const char IpPrefix[] = "ip$";

H323TransportAddress::H323TransportAddress(const char * cstr)
  : PString(cstr)
{
  Validate();
}


H323TransportAddress::H323TransportAddress(const PString & str)
  : PString(str)
{
  Validate();
}


static PString BuildIP(const PIPSocket::Address & ip, unsigned port)
{
  PStringStream str;

  str << IpPrefix;

  if (!ip.IsValid())
    str << '*';
  else
#if P_HAS_IPV6
  if (ip.GetVersion() == 6)
    str << '[' << ip << ']';
  else
#endif
    str << ip;

  if (port != 0)
    str << ':' << port;

  return str;
}


H323TransportAddress::H323TransportAddress(const H225_TransportAddress & transport)
{
  switch (transport.GetTag()) {
    case H225_TransportAddress::e_ipAddress :
    {
      const H225_TransportAddress_ipAddress & ip = transport;
      *this = BuildIP(PIPSocket::Address(ip.m_ip.GetSize(), ip.m_ip.GetValue()), ip.m_port);
      break;
    }
#if P_HAS_IPV6
    case H225_TransportAddress::e_ip6Address :
    {
      const H225_TransportAddress_ip6Address & ip = transport;
      *this = BuildIP(PIPSocket::Address(ip.m_ip.GetSize(), ip.m_ip.GetValue()), ip.m_port);
      break;
    }
#endif
  }
}


H323TransportAddress::H323TransportAddress(const H245_TransportAddress & transport)
{
  switch (transport.GetTag()) {
    case H245_TransportAddress::e_unicastAddress :
    {
      const H245_UnicastAddress & unicast = transport;
      switch (unicast.GetTag()) {
        case H245_UnicastAddress::e_iPAddress :
        {
          const H245_UnicastAddress_iPAddress & ip = unicast;
          *this = BuildIP(PIPSocket::Address(ip.m_network.GetSize(), ip.m_network.GetValue()), ip.m_tsapIdentifier);
          break;
        }
#if P_HAS_IPV6
        case H245_UnicastAddress::e_iP6Address :
        {
          const H245_UnicastAddress_iP6Address & ip = unicast;
          *this = BuildIP(PIPSocket::Address(ip.m_network.GetSize(), ip.m_network.GetValue()), ip.m_tsapIdentifier);
          break;
        }
#endif
      }
      break;
    }
  }
}


H323TransportAddress::H323TransportAddress(const PIPSocket::Address & ip, WORD port)
{
  *this = BuildIP(ip, port);
}


void H323TransportAddress::Validate()
{
  if (IsEmpty())
    return;

  if (Find('$') == P_MAX_INDEX) {
    Splice(IpPrefix, 0, 0);
    return;
  }

  if (strncmp(theArray, IpPrefix, 3) == 0) {
    return;
  }

  *this = PString();
}


BOOL H323TransportAddress::SetPDU(H225_TransportAddress & pdu) const
{
  PIPSocket::Address ip;
  WORD port = H323EndPoint::DefaultTcpPort;
  if (GetIpAndPort(ip, port)) {
#if P_HAS_IPV6
    if (ip.GetVersion() == 6) {
      pdu.SetTag(H225_TransportAddress::e_ip6Address);
      H225_TransportAddress_ip6Address & addr = pdu;
      for (PINDEX i = 0; i < ip.GetSize(); i++)
        addr.m_ip[i] = ip[i];
      addr.m_port = port;
      return TRUE;
    }
#endif

    pdu.SetTag(H225_TransportAddress::e_ipAddress);
    H225_TransportAddress_ipAddress & addr = pdu;
    for (PINDEX i = 0; i < 4; i++)
      addr.m_ip[i] = ip[i];
    addr.m_port = port;
    return TRUE;
  }

  return FALSE;
}


BOOL H323TransportAddress::SetPDU(H245_TransportAddress & pdu) const
{
  PIPSocket::Address ip;
  WORD port = 0;
  if (GetIpAndPort(ip, port)) {
    pdu.SetTag(H245_TransportAddress::e_unicastAddress);

    H245_UnicastAddress & unicast = pdu;

#if P_HAS_IPV6
    if (ip.GetVersion() == 6) {
      unicast.SetTag(H245_UnicastAddress::e_iP6Address);
      H245_UnicastAddress_iP6Address & addr = unicast;
      for (PINDEX i = 0; i < ip.GetSize(); i++)
        addr.m_network[i] = ip[i];
      addr.m_tsapIdentifier = port;
      return TRUE;
    }
#endif

    unicast.SetTag(H245_UnicastAddress::e_iPAddress);
    H245_UnicastAddress_iPAddress & addr = unicast;
    for (PINDEX i = 0; i < 4; i++)
      addr.m_network[i] = ip[i];
    addr.m_tsapIdentifier = port;
    return TRUE;
  }

  return FALSE;
}


BOOL H323TransportAddress::IsEquivalent(const H323TransportAddress & address)
{
  if (*this == address)
    return TRUE;

  if (IsEmpty() || address.IsEmpty())
    return FALSE;

  PIPSocket::Address ip1, ip2;
  WORD port1 = 65535, port2 = 65535;
  return GetIpAndPort(ip1, port1) &&
         address.GetIpAndPort(ip2, port2) &&
         (ip1.IsAny() || ip2.IsAny() || ip1 == ip2) &&
         (port1 == 65535 || port2 == 65535 || port1 == port2);
}


BOOL H323TransportAddress::GetIpAddress(PIPSocket::Address & ip) const
{
  WORD dummy = 65535;
  return GetIpAndPort(ip, dummy);
}


static BOOL SplitAddress(const PString & addr, PString & host, PString & service)
{
  if (strncmp(addr, IpPrefix, 3) != 0) {
    PTRACE(2, "H323\tUse of non IP transport address: \"" << addr << '"');
    return FALSE;
  }

  PINDEX lastChar = addr.GetLength()-1;
  if (addr[lastChar] == '+')
    lastChar--;

  PINDEX bracket = addr.FindLast(']');
  if (bracket == P_MAX_INDEX)
    bracket = 0;

  PINDEX colon = addr.Find(':', bracket);
  if (colon == P_MAX_INDEX)
    host = addr(3, lastChar);
  else {
    host = addr.Mid(3, colon-3);
    service = addr.Mid(colon+1, lastChar);
  }

  return TRUE;
}


BOOL H323TransportAddress::GetIpAndPort(PIPSocket::Address & ip,
                                        WORD & port,
                                        const char * proto) const
{
  PString host, service;
  if (!SplitAddress(*this, host, service))
    return FALSE;

  if (host.IsEmpty()) {
    PTRACE(2, "H323\tIllegal IP transport address: \"" << *this << '"');
    return FALSE;
  }

  if (service == "*")
    port = 0;
  else {
    if (!service)
      port = PIPSocket::GetPortByService(proto, service);
    if (port == 0) {
      PTRACE(2, "H323\tIllegal IP transport port/service: \"" << *this << '"');
      return FALSE;
    }
  }

  if (host == "*") {
    ip = PIPSocket::GetDefaultIpAny();
    return TRUE;
  }

  if (PIPSocket::GetHostAddress(host, ip))
    return TRUE;

  PTRACE(1, "H323\tCould not find host : \"" << host << '"');
  return FALSE;
}


PString H323TransportAddress::GetHostName() const
{
  PString host, service;
  if (!SplitAddress(*this, host, service))
    return *this;

  PIPSocket::Address ip;
  if (PIPSocket::GetHostAddress(host, ip))
    return ip.AsString();

  return host;
}


H323Listener * H323TransportAddress::CreateListener(H323EndPoint & endpoint) const
{
  /*Have transport type name, create the transport object. Hard coded at the
    moment but would like to add some sort of "registration" of transport
    classes so new transports can be added without changing this source file
    every time. As we have one transport type at the moment and may never
    actually have another, we hard code it for now.
   */

  PIPSocket::Address ip;
  WORD port = H323EndPoint::DefaultTcpPort;
  if (GetIpAndPort(ip, port))
//    return new H323ListenerTCP(endpoint, ip, port, theArray[GetLength()-1] != '+');
    return new H323ListenerTCP(endpoint, ip, port, FALSE);

  return NULL;
}


H323Listener * H323TransportAddress::CreateCompatibleListener(H323EndPoint & endpoint) const
{
  /*Have transport type name, create the transport object. Hard coded at the
    moment but would like to add some sort of "registration" of transport
    classes so new transports can be added without changing this source file
    every time. As we have one transport type at the moment and may never
    actually have another, we hard code it for now.
   */

  PIPSocket::Address ip;
  if (GetIpAddress(ip))
    return new H323ListenerTCP(endpoint, ip, 0, FALSE);

  return NULL;
}


H323Transport * H323TransportAddress::CreateTransport(H323EndPoint & endpoint) const
{
  /*Have transport type name, create the transport object. Hard coded at the
    moment but would like to add some sort of "registration" of transport
    classes so new transports can be added without changing this source file
    every time. As we have one transport type at the moment and may never
    actually have another, we hard code it for now.
   */

  if (strncmp(theArray, IpPrefix, 3) == 0)
    return new H323TransportTCP(endpoint);

  return NULL;
}


H323TransportAddressArray H323GetInterfaceAddresses(const H323ListenerList & listeners,
                                                    BOOL excludeLocalHost,
                                                    H323Transport * associatedTransport)
{
  H323TransportAddressArray interfaceAddresses;

  PINDEX i;
  for (i = 0; i < listeners.GetSize(); i++) {
    H323TransportAddressArray newAddrs = H323GetInterfaceAddresses(listeners[i].GetTransportAddress(), excludeLocalHost, associatedTransport);
    PINDEX size  = interfaceAddresses.GetSize();
    PINDEX nsize = newAddrs.GetSize();
    interfaceAddresses.SetSize(size + nsize);
    PINDEX j;
    for (j = 0; j < nsize; j++)
      interfaceAddresses.SetAt(size + j, new H323TransportAddress(newAddrs[j]));
  }

  return interfaceAddresses;
}


H323TransportAddressArray H323GetInterfaceAddresses(const H323TransportAddress & addr,
                                                    BOOL excludeLocalHost,
                                                    H323Transport * associatedTransport)
{
  PIPSocket::Address ip;
  WORD port;
  if (!addr.GetIpAndPort(ip, port) || !ip.IsAny())
    return addr;

  PIPSocket::InterfaceTable interfaces;
  if (!PIPSocket::GetInterfaceTable(interfaces))
    return addr;

  if (interfaces.GetSize() == 1)
    return H323TransportAddress(interfaces[0].GetAddress(), port);

  PINDEX i;
  H323TransportAddressArray interfaceAddresses;
  PIPSocket::Address firstAddress(0);

  if (associatedTransport != NULL) {
    if (associatedTransport->GetLocalAddress().GetIpAddress(firstAddress)) {
      for (i = 0; i < interfaces.GetSize(); i++) {
        PIPSocket::Address ip = interfaces[i].GetAddress();
        if (ip == firstAddress)
          interfaceAddresses.Append(new H323TransportAddress(ip, port));
      }
    }
  }

  for (i = 0; i < interfaces.GetSize(); i++) {
    PIPSocket::Address ip = interfaces[i].GetAddress();
    if (ip != firstAddress && !(excludeLocalHost && ip.IsLoopback()))
      interfaceAddresses.Append(new H323TransportAddress(ip, port));
  }

  return interfaceAddresses;
}


void H323SetTransportAddresses(const H323Transport & associatedTransport,
                               const H323TransportAddressArray & addresses,
                               H225_ArrayOf_TransportAddress & pdu)
{
  for (PINDEX i = 0; i < addresses.GetSize(); i++) {
    H323TransportAddress addr = addresses[i];

    PIPSocket::Address ip;
    WORD port;
    if (addr.GetIpAndPort(ip, port)) {
      PIPSocket::Address remoteIP;
      if (associatedTransport.GetRemoteAddress().GetIpAddress(remoteIP)) {
        associatedTransport.GetEndPoint().InternalTranslateTCPAddress(ip, remoteIP);
	associatedTransport.GetEndPoint().TranslateTCPPort(port,remoteIP);
        addr = H323TransportAddress(ip, port);
      }
    }

    if (addresses.GetSize() > 1 && ip.IsLoopback())
      continue;

    PTRACE(4, "TCP\tAppending H.225 transport " << addr
           << " using associated transport " << associatedTransport);

    H225_TransportAddress pduAddr;
    addr.SetPDU(pduAddr);

    PINDEX lastPos = pdu.GetSize();

    // Check for already have had that address.
    PINDEX j;
    for (j = 0; j < lastPos; j++) {
      if (pdu[j] == pduAddr)
        break;
    }

    if (j >= lastPos) {
      // Put new listener into array
      pdu.SetSize(lastPos+1);
      pdu[lastPos] = pduAddr;
    }
  }
}


/////////////////////////////////////////////////////////////////////////////

H323TransportAddressArray::H323TransportAddressArray(const H225_ArrayOf_TransportAddress & addresses)
{
  for (PINDEX i = 0; i < addresses.GetSize(); i++)
    AppendAddress(H323TransportAddress(addresses[i]));
}


void H323TransportAddressArray::AppendString(const char * str)
{
  AppendAddress(H323TransportAddress(str));
}


void H323TransportAddressArray::AppendString(const PString & str)
{
  AppendAddress(H323TransportAddress(str));
}


void H323TransportAddressArray::AppendAddress(const H323TransportAddress & addr)
{
  if (!addr)
    Append(new H323TransportAddress(addr));
}


void H323TransportAddressArray::AppendStringCollection(const PCollection & coll)
{
  for (PINDEX i = 0; i < coll.GetSize(); i++) {
    PObject * obj = coll.GetAt(i);
    if (obj != NULL && PIsDescendant(obj, PString))
      AppendAddress(H323TransportAddress(*(PString *)obj));
  }
}


/////////////////////////////////////////////////////////////////////////////

H323Listener::H323Listener(H323EndPoint & end)
  : PThread(end.GetListenerThreadStackSize(),
            NoAutoDeleteThread,
            NormalPriority,
            "H323 Listener:%0x"),
    endpoint(end)
{
}


void H323Listener::PrintOn(ostream & strm) const
{
  strm << "Listener[" << GetTransportAddress() << ']';
}


/////////////////////////////////////////////////////////////////////////////

H323Transport::H323Transport(H323EndPoint & end)
  : endpoint(end)
{
  thread = NULL;
  canGetInterface = FALSE;
}


H323Transport::~H323Transport()
{
  PAssert(thread == NULL, PLogicError);
}


void H323Transport::PrintOn(ostream & strm) const
{
  strm << "Transport[";
  H323TransportAddress addr = GetRemoteAddress();
  if (!addr)
    strm << "remote=" << addr << ' ';
  strm << "if=" << GetLocalAddress() << ']';
}


BOOL H323Transport::Close()
{
  PTRACE(3, "H323\tH323Transport::Close");

  /* Do not use PIndirectChannel::Close() as this deletes the sub-channel
     member field crashing the background thread. Just close the base
     sub-channel so breaks the threads I/O block.
   */
  if (IsOpen()) {
    channelPointerMutex.StartRead();
    GetBaseReadChannel()->Close();
    channelPointerMutex.EndRead();
  }

  return TRUE;
}

BOOL H323Transport::HandleSignallingSocket(H323SignalPDU & pdu)
{

  for (;;) {
	  H323SignalPDU rpdu;
	  if (!rpdu.Read(*this)) { 
            return FALSE;
	  }
	  else if ((rpdu.GetQ931().GetMessageType() == Q931::InformationMsg) &&
              endpoint.OnUnsolicitedInformation(rpdu)) {
           // Handle unsolicited Information Message
                ;
	  } 
    else {
		  pdu = rpdu;
		  return TRUE;
	  }	
  }
	  
  return FALSE;
}

BOOL H323Transport::HandleFirstSignallingChannelPDU()
{
  PTRACE(3, "H225\tAwaiting first PDU");
  SetReadTimeout(15000); // Await 15 seconds after connect for first byte
  H323SignalPDU pdu;
//  if (!pdu.Read(*this)) {
  if (!HandleSignallingSocket(pdu)) {
    PTRACE(1, "H225\tFailed to get initial Q.931 PDU, connection not started.");
    return FALSE;
  }

  unsigned callReference = pdu.GetQ931().GetCallReference();
  PTRACE(3, "H225\tIncoming call, first PDU: callReference=" << callReference);

  // Get a new (or old) connection from the endpoint
  H323Connection * connection = endpoint.OnIncomingConnection(this, pdu);
  if (connection == NULL) {
    PTRACE(1, "H225\tEndpoint could not create connection, "
              "sending release complete PDU: callRef=" << callReference);
   
    H323SignalPDU releaseComplete;
    Q931 &q931PDU = releaseComplete.GetQ931();
    q931PDU.BuildReleaseComplete(callReference, TRUE);
    releaseComplete.m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_releaseComplete);

    H225_ReleaseComplete_UUIE &release = releaseComplete.m_h323_uu_pdu.m_h323_message_body;
    release.m_protocolIdentifier.SetValue(psprintf("0.0.8.2250.0.%u", H225_PROTOCOL_VERSION));

    H225_Setup_UUIE &setup = pdu.m_h323_uu_pdu.m_h323_message_body;
    if (setup.HasOptionalField(H225_Setup_UUIE::e_callIdentifier)) {
       release.IncludeOptionalField(H225_Setup_UUIE::e_callIdentifier);
       release.m_callIdentifier = setup.m_callIdentifier;
    }

    // Set the cause value
    q931PDU.SetCause(Q931::TemporaryFailure);

    // Send the PDU
    releaseComplete.Write(*this,*connection);
    return FALSE;
  }

  connection->Lock();

  // handle the first PDU
  if (connection->HandleSignalPDU(pdu)) {

#ifdef H323_SIGNAL_AGGREGATE
    // if the endpoint is using signalling aggregation, we need to add this connection
    // to the signalling aggregator. 
    if (connection != NULL && endpoint.GetSignallingAggregator() != NULL) {
      connection->AggregateSignalChannel(this);
      connection->Unlock();
      return TRUE;
    }
#endif

    // If aggregation is not being used, then this thread is attached to the transport, 
    // which is in turn attached to the connection so everything from gets cleaned up by the 
    // H323 cleaner thread from now on. So thread must not auto delete and the "transport" 
    // variable is not deleted either
    PThread * thread = PThread::Current();
    AttachThread(thread);
    thread->SetNoAutoDelete();

    connection->Unlock();

    // All subsequent PDU's should wait forever
    SetReadTimeout(PMaxTimeInterval);
    connection->HandleSignallingChannel();
  }
  else {
    connection->ClearCall(H323Connection::EndedByTransportFail);
    connection->Unlock();
    PTRACE(1, "H225\tSignal channel stopped on first PDU.");
  }


  return TRUE;
}


void H323Transport::StartControlChannel(H323Connection & connection)
{
  new H245TransportThread(endpoint, connection, *this);
}


void H323Transport::AttachThread(PThread * thrd)
{
  PAssert(thread == NULL, PLogicError);
  thread = thrd;
}


void H323Transport::CleanUpOnTermination()
{
  Close();

  if (thread != NULL) {
    PTRACE(3, "H323\tH323Transport::CleanUpOnTermination for " << thread->GetThreadName());
    PAssert(thread->WaitForTermination(10000), "Transport thread did not terminate");
    delete thread;
    thread = NULL;
  }
}


BOOL H323Transport::IsCompatibleTransport(const H225_TransportAddress & /*pdu*/) const
{
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
}


void H323Transport::SetUpTransportPDU(H225_TransportAddress & /*pdu*/,
                                      BOOL /*localTsap*/,
									  H323Connection * /*connection*/
									  ) const
{
  PAssertAlways(PUnimplementedFunction);
}


void H323Transport::SetUpTransportPDU(H245_TransportAddress & /*pdu*/,
                                      unsigned /*port*/) const
{
  PAssertAlways(PUnimplementedFunction);
}


void H323Transport::SetPromiscuous(PromisciousModes /*promiscuous*/)
{
}


H323TransportAddress H323Transport::GetLastReceivedAddress() const
{
  return GetRemoteAddress();
}


H323Transport * H323Transport::CreateControlChannel(H323Connection & /*connection*/)
{
  PAssertAlways(PUnimplementedFunction);
  return NULL;
}


BOOL H323Transport::AcceptControlChannel(H323Connection & /*connection*/)
{
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
}


BOOL H323Transport::DiscoverGatekeeper(H323Gatekeeper & /*gk*/,
                                       H323RasPDU & /*pdu*/,
                                       const H323TransportAddress & /*address*/)
{
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
}


/////////////////////////////////////////////////////////////////////////////

H323ListenerTCP::H323ListenerTCP(H323EndPoint & end,
                                 PIPSocket::Address binding,
                                 WORD port,
                                 BOOL exclusive)
  : H323Listener(end),
	  listener((port == 0) ? (WORD)H323EndPoint::DefaultTcpPort : port),
    localAddress(binding)
{
  exclusiveListener = exclusive;
}


H323ListenerTCP::~H323ListenerTCP()
{
  Close();
}


BOOL H323ListenerTCP::Open()
{
  if (listener.Listen(localAddress, 100, 0,
                      exclusiveListener ? PSocket::AddressIsExclusive
                                        : PSocket::CanReuseAddress))
    return TRUE;

  PTRACE(1, "TCP\tListen on " << localAddress << ':' << listener.GetPort()
         << " failed: " << listener.GetErrorText());
  return FALSE;
}


BOOL H323ListenerTCP::Close()
{
  BOOL ok = listener.Close();

  PAssert(PThread::Current() != this, PLogicError);

  if (!IsTerminated() && !IsSuspended())
    PAssert(WaitForTermination(10000), "Listener thread did not terminate");

  return ok;
}


H323Transport * H323ListenerTCP::Accept(const PTimeInterval & timeout)
{
  if (!listener.IsOpen())
    return NULL;

  listener.SetReadTimeout(timeout); // Wait for remote connect

  PTRACE(4, "TCP\tWaiting on socket accept on " << GetTransportAddress());
  PTCPSocket * socket = new PTCPSocket;
  if (socket->Accept(listener)) {
    H323TransportTCP * transport = new H323TransportTCP(endpoint);
    if (transport->Open(socket))
      return transport;

    PTRACE(1, "TCP\tFailed to open transport, connection not started.");
    delete transport;
    return NULL;
  }

  if (socket->GetErrorCode() != PChannel::Interrupted) {
    PTRACE(1, "TCP\tAccept error:" << socket->GetErrorText());
    listener.Close();
  }

  delete socket;
  return NULL;
}


H323TransportAddress H323ListenerTCP::GetTransportAddress() const
{
  return H323TransportAddress(localAddress, listener.GetPort());
}


BOOL H323ListenerTCP::SetUpTransportPDU(H245_TransportAddress & pdu,
                                        const H323Transport & associatedTransport)
{
  if (!localAddress.IsAny())
    return GetTransportAddress().SetPDU(pdu);

  PIPSocket::Address addressOfExistingInterface;
  if (!associatedTransport.GetLocalAddress().GetIpAddress(addressOfExistingInterface))
    return FALSE;

  H323TransportAddress transAddr(addressOfExistingInterface, listener.GetPort());
  transAddr.SetPDU(pdu);
  return TRUE;
}


void H323ListenerTCP::Main()
{
  PTRACE(2, "H323\tAwaiting TCP connections on port " << listener.GetPort());

  while (listener.IsOpen()) {
    H323Transport * transport = Accept(PMaxTimeInterval);
    if (transport != NULL)
      new H225TransportThread(endpoint, transport);
  }
}


/////////////////////////////////////////////////////////////////////////////

H323TransportIP::H323TransportIP(H323EndPoint & end, PIPSocket::Address binding, WORD remPort)
  : H323Transport(end),
    localAddress(binding),
    remoteAddress(0)
{
  localPort = 0;
  remotePort = remPort;
}


H323TransportAddress H323TransportIP::GetLocalAddress() const
{
  return H323TransportAddress(localAddress, localPort);
}


H323TransportAddress H323TransportIP::GetRemoteAddress() const
{
  return H323TransportAddress(remoteAddress, remotePort);
}


BOOL H323TransportIP::IsCompatibleTransport(const H225_TransportAddress & pdu) const
{
  return pdu.GetTag() == H225_TransportAddress::e_ipAddress
#if P_HAS_IPV6
            || pdu.GetTag() == H225_TransportAddress::e_ip6Address
#endif
         ;
}


void H323TransportIP::SetUpTransportPDU(H225_TransportAddress & pdu, BOOL localTsap,H323Connection * connection) const
{
  H323TransportAddress transAddr;
  if (!localTsap) 
    transAddr = H323TransportAddress(remoteAddress, remotePort);
  else {
    H323TransportAddress tAddr = GetLocalAddress();
    PIPSocket::Address ipAddr; 
    tAddr.GetIpAddress(ipAddr);
    endpoint.InternalTranslateTCPAddress(ipAddr, remoteAddress,connection);
    WORD tPort = localPort;
    endpoint.TranslateTCPPort(tPort,remoteAddress);
    transAddr = H323TransportAddress(ipAddr, tPort);
  }

  transAddr.SetPDU(pdu);
}


void H323TransportIP::SetUpTransportPDU(H245_TransportAddress & pdu, unsigned port) const
{
  PIPSocket::Address ipAddr = localAddress;
  endpoint.InternalTranslateTCPAddress(ipAddr, remoteAddress);

  switch (port) {
    case UseLocalTSAP :
      port = localPort;
      break;
    case UseRemoteTSAP :
      port = remotePort;
      break;
  }

  H323TransportAddress transAddr(ipAddr, (WORD)port);
  transAddr.SetPDU(pdu);
}


/////////////////////////////////////////////////////////////////////////////

H323TransportTCP::H323TransportTCP(H323EndPoint & end,
                                   PIPSocket::Address binding,
                                   BOOL listen)
  : H323TransportIP(end, binding, H323EndPoint::DefaultTcpPort)
{
  h245listener = NULL;

  // construct listener socket if required
  if (listen) {
    h245listener = new PTCPSocket;

    localPort = end.GetNextTCPPort();
    WORD firstPort = localPort;
    while (!h245listener->Listen(binding, 5, localPort, PSocket::CanReuseAddress)) {
      localPort = end.GetNextTCPPort();
      if (localPort == firstPort)
        break;
    }

    if (h245listener->IsOpen()) {
      localPort = h245listener->GetPort();
      PTRACE(3, "H225\tTCP Listen for H245 on " << binding << ':' << localPort);
    }
    else {
      PTRACE(1, "H225\tTCP Listen for H245 failed: " << h245listener->GetErrorText());
      delete h245listener;
      h245listener = NULL;
    }
  }
}


H323TransportTCP::~H323TransportTCP()
{
  delete h245listener;  // Delete any H245 listener that may be present
}


BOOL H323TransportTCP::OnOpen()
{
  PIPSocket * socket = (PIPSocket *)GetReadChannel();

  // Get name of the remote computer for information purposes
  if (!socket->GetPeerAddress(remoteAddress, remotePort)) {
    PTRACE(1, "H323TCP\tGetPeerAddress() failed: " << socket->GetErrorText());
    return FALSE;
  }

  // get local address of incoming socket to ensure that multi-homed machines
  // use a NIC address that is guaranteed to be addressable to destination
  if (!socket->GetLocalAddress(localAddress, localPort)) {
    PTRACE(1, "H323TCP\tGetLocalAddress() failed: " << socket->GetErrorText());
    return FALSE;
  }

  if (!socket->SetOption(TCP_NODELAY, 1, IPPROTO_TCP)) {
    PTRACE(1, "H323TCP\tSetOption(TCP_NODELAY) failed: " << socket->GetErrorText());
  }

  if (!socket->SetOption(IP_TOS, endpoint.GetTcpIpTypeofService(), IPPROTO_IP)) { 
    PTRACE(1, "H323TCP\tSetOption(IP_TOS) failed: " << socket->GetErrorText()); 
  }

#ifndef P_VXWORKS // VxWorks has alternative behaviour, so skip it
  // make sure do not lose outgoing packets on close
  const linger ling = { 1, 3 };
  if (!socket->SetOption(SO_LINGER, &ling, sizeof(ling))) {
    PTRACE(1, "H323TCP\tSetOption(SO_LINGER) failed: " << socket->GetErrorText());
    return FALSE;
  }
#endif //P_VXWORKS

  PTRACE(2, "H323TCP\tStarted connection: "
            " host=" << remoteAddress << ':' << remotePort << ","
            " if=" << localAddress << ':' << localPort << ","
            " handle=" << socket->GetHandle());

  return TRUE;
}


BOOL H323TransportTCP::Close()
{
  // Close listening socket to break waiting accept
  if (IsListening())
    h245listener->Close();

  return H323Transport::Close();
}


BOOL H323TransportTCP::SetRemoteAddress(const H323TransportAddress & address)
{
  return address.GetIpAndPort(remoteAddress, remotePort, "tcp");
}

BOOL H323TransportTCP::ExtractPDU(const PBYTEArray & pdu, PINDEX & pduLen)
{
  //
  // TPKT format is :
  //   byte 0   = type identifier - always 0x03
  //   byte 1   = ignored
  //   byte 2   = msb of data length
  //   byte 3   = lsb of data length
  //   byte xx  = data of length
  //
  // this gives minimum length of 4 bytes

  // ensure length is at least one byte
  if (pduLen < 1) {
    pduLen = 0;
    return TRUE;
  }

  // only accept TPKT of type 3
  if (pdu[0] != 3)
    return SetErrorValues(Miscellaneous, 0x41000000);

  // check for minimum header length
  if (pduLen < 4) {
    pduLen = 0;
    return TRUE;
  }

  // see if complete PDU received yet
  PINDEX dataLen = (pdu[2] << 8)|pdu[3];

  // dwarf PDUs are errors
  if (dataLen < 4) {
    PTRACE(1, "H323TCP\tDwarf PDU received (length " << dataLen << ")");
    return FALSE;
  }

  // wait for data to arrive
  if (pduLen < dataLen) {
    pduLen = 0;
    return TRUE;
  }

  // set the length of the complete PDU
  pduLen = dataLen;

  return TRUE;
}

BOOL H323TransportTCP::ReadPDU(PBYTEArray & pdu)
{
  // Make sure is a RFC1006 TPKT
  switch (ReadChar()) {
    case -1 :
      return FALSE;

    case 3 :  // Only support version 3
      break;

    default :  // Unknown version number
      return SetErrorValues(Miscellaneous, 0x41000000);
  }

  // Save timeout
  PTimeInterval oldTimeout = GetReadTimeout();

  // Should get all of PDU in 5 seconds or something is seriously wrong,
  SetReadTimeout(5000);

  // Get TPKT length
  BYTE header[3];
  BOOL ok = ReadBlock(header, sizeof(header));
  if (ok) {
    PINDEX packetLength = ((header[1] << 8)|header[2]);
    if (packetLength < 4) {
      PTRACE(1, "H323TCP\tDwarf PDU received (length " << packetLength << ")");
      ok = FALSE;
    } else {
      packetLength -= 4;
      ok = ReadBlock(pdu.GetPointer(packetLength), packetLength);
    }
  }

  SetReadTimeout(oldTimeout);

  return ok;
}


BOOL H323TransportTCP::WritePDU(const PBYTEArray & pdu)
{
  // We copy the data into a new buffer so we can do a single write call. This
  // is necessary as we have disabled the Nagle TCP delay algorithm to improve
  // network performance.

  int packetLength = pdu.GetSize() + 4;

  // Send RFC1006 TPKT length
  PBYTEArray tpkt(packetLength);
  tpkt[0] = 3;
  tpkt[1] = 0;
  tpkt[2] = (BYTE)(packetLength >> 8);
  tpkt[3] = (BYTE)packetLength;
  memcpy(tpkt.GetPointer()+4, (const BYTE *)pdu, pdu.GetSize());

  return Write((const BYTE *)tpkt, packetLength);
}


BOOL H323TransportTCP::Connect()
{
  if (IsListening())
    return TRUE;

  PTCPSocket * socket = new PTCPSocket(remotePort);
  Open(socket);

  channelPointerMutex.StartRead();

  socket->SetReadTimeout(endpoint.GetSignallingChannelConnectTimeout());

  localPort = endpoint.GetNextTCPPort();
  WORD firstPort = localPort;
  for (;;) {
    PTRACE(4, "H323TCP\tConnecting to "
           << remoteAddress << ':' << remotePort
           << " (local port=" << localPort << ')');
    if (socket->Connect(localAddress, localPort, remoteAddress))
      break;

    int errnum = socket->GetErrorNumber();
    if (localPort == 0 || (errnum != EADDRINUSE && errnum != EADDRNOTAVAIL)) {
      PTRACE(1, "H323TCP\tCould not connect to "
                << remoteAddress << ':' << remotePort
                << " (local port=" << localPort << ") - "
                << socket->GetErrorText() << '(' << errnum << ')');
      channelPointerMutex.EndRead();
      return SetErrorValues(socket->GetErrorCode(), errnum);
    }

    localPort = endpoint.GetNextTCPPort();
    if (localPort == firstPort) {
      PTRACE(1, "H323TCP\tCould not bind to any port in range " <<
                endpoint.GetTCPPortBase() << " to " << endpoint.GetTCPPortMax());
      channelPointerMutex.EndRead();
      return SetErrorValues(socket->GetErrorCode(), errnum);
    }
  }

  socket->SetReadTimeout(PMaxTimeInterval);

  channelPointerMutex.EndRead();

  return OnOpen();
}


H323Transport * H323TransportTCP::CreateControlChannel(H323Connection & connection)
{
  H323TransportTCP * tcpTransport = new H323TransportTCP(endpoint, localAddress, TRUE);
  tcpTransport->SetRemoteAddress(GetRemoteAddress());
  if (tcpTransport->IsListening()) // Listen() failed
    return tcpTransport;

  delete tcpTransport;
  connection.ClearCall(H323Connection::EndedByTransportFail);
  return FALSE;
}


BOOL H323TransportTCP::AcceptControlChannel(H323Connection & connection)
{
  if (IsOpen())
    return TRUE;

  if (h245listener == NULL) {
    PAssertAlways(PLogicError);
    return FALSE;
  }

  PTRACE(3, "H245\tTCP Accept wait");

  PTCPSocket * h245Socket = new PTCPSocket;

  h245listener->SetReadTimeout(endpoint.GetControlChannelStartTimeout());
  if (h245Socket->Accept(*h245listener))
    return Open(h245Socket);

  PTRACE(1, "H225\tAccept for H245 failed: " << h245Socket->GetErrorText());
  delete h245Socket;

  if (h245listener->IsOpen() &&
      connection.IsConnected() &&
      connection.FindChannel(RTP_Session::DefaultAudioSessionID, TRUE) == NULL &&
      connection.FindChannel(RTP_Session::DefaultAudioSessionID, FALSE) == NULL)
    connection.ClearCall(H323Connection::EndedByTransportFail);

  return FALSE;
}


BOOL H323TransportTCP::IsListening() const
{
  if (IsOpen())
    return FALSE;

  if (h245listener == NULL)
    return FALSE;

  return h245listener->IsOpen();
}


/////////////////////////////////////////////////////////////////////////////

static BOOL ListenUDP(PUDPSocket & socket,
                      H323EndPoint & endpoint,
                      PIPSocket::Address binding,
                      WORD localPort)
{
  if (localPort > 0) {
    if (socket.Listen(binding, 0, localPort, PSocket::CanReuseAddress))
      return TRUE;
  }
  else {
    localPort = endpoint.GetNextUDPPort();
    WORD firstPort = localPort;

    for (;;) {
      if (socket.Listen(binding, 0, localPort))
        return TRUE;

      int errnum = socket.GetErrorNumber();
      if (errnum != EADDRINUSE && errnum != EADDRNOTAVAIL)
        break;

      localPort = endpoint.GetNextUDPPort();
      if (localPort == firstPort) {
        PTRACE(1, "H323UDP\tCould not bind to any port in range " <<
                  endpoint.GetUDPPortBase() << " to " << endpoint.GetUDPPortMax());
        return FALSE;
      }
    }
  }

  PTRACE(1, "H323UDP\tCould not bind to "
            << binding << ':' << localPort << " - "
            << socket.GetErrorText() << '(' << socket.GetErrorNumber() << ')');
  return FALSE;
}


H323TransportUDP::H323TransportUDP(H323EndPoint & ep,
                                   PIPSocket::Address binding,
                                   WORD local_port,
                                   WORD remote_port)
  : H323TransportIP(ep, binding, remote_port)
{
  if (remotePort == 0)
    remotePort = H225_RAS::DefaultRasUdpPort; // For backward compatibility

  promiscuousReads = AcceptFromRemoteOnly;

  PUDPSocket * udp = new PUDPSocket;
  ListenUDP(*udp, ep, binding, local_port);

  interfacePort = localPort = udp->GetPort();

  Open(udp);

  PTRACE(3, "H323UDP\tBinding to interface: " << binding << ':' << localPort);

  canGetInterface = (binding.IsAny()) && udp->SetCaptureReceiveToAddress();

}


H323TransportUDP::~H323TransportUDP()
{
  Close();
}


BOOL H323TransportUDP::SetRemoteAddress(const H323TransportAddress & address)
{
  return address.GetIpAndPort(remoteAddress, remotePort, "udp");
}


BOOL H323TransportUDP::Connect()
{
  if (remoteAddress == 0 || remotePort == 0)
    return FALSE;

  PUDPSocket * socket;

#ifdef P_STUN
  PSTUNClient * stun = endpoint.GetSTUN(remoteAddress);
  if (stun != NULL) {
    if (stun->CreateSocket(socket)) {
      Open(socket);
      socket->GetLocalAddress(localAddress, localPort);
      PTRACE(4, "H323UDP\tSTUN created socket: " << localAddress << ':' << localPort);
    }
    else
      PTRACE(4, "H323UDP\tSTUN could not create socket!");
  }
#endif

  socket = (PUDPSocket *)GetReadChannel();
  socket->SetSendAddress(remoteAddress, remotePort);

  return TRUE;
}


void H323TransportUDP::SetPromiscuous(PromisciousModes promiscuous)
{
  promiscuousReads = promiscuous;
}


H323TransportAddress H323TransportUDP::GetLastReceivedAddress() const
{
  if (!lastReceivedAddress)
    return lastReceivedAddress;

  return H323Transport::GetLastReceivedAddress();
}

BOOL H323TransportUDP::ExtractPDU(const PBYTEArray & /*pdu*/, PINDEX & /*len*/)
{
  return TRUE;
}

BOOL H323TransportUDP::ReadPDU(PBYTEArray & pdu)
{
  for (;;) {
    if (!Read(pdu.GetPointer(10000), 10000)) {
      pdu.SetSize(0);
      return FALSE;
    }

    pdu.SetSize(GetLastReadCount());

    PUDPSocket * socket = (PUDPSocket *)GetReadChannel();

    if (canGetInterface)
      lastReceivedInterface = socket->GetLastReceiveToAddress();

    PIPSocket::Address address;
    WORD port;

    socket->GetLastReceiveAddress(address, port);

    switch (promiscuousReads) {
      case AcceptFromRemoteOnly :
        if (remoteAddress *= address)
          goto accept;
        break;

      case AcceptFromAnyAutoSet :
        remoteAddress = address;
        remotePort = port;
        socket->SetSendAddress(remoteAddress, remotePort);
        goto accept;

      case AcceptFromLastReceivedOnly :
        if (!lastReceivedAddress.IsEmpty()) {
          PIPSocket::Address lastAddr;
          WORD lastPort;

          if (lastReceivedAddress.GetIpAndPort(lastAddr, lastPort, "udp") &&
             (lastAddr *= address) && lastPort == port)
            goto accept;
        }
        break;

      default : //AcceptFromAny
      accept:
        lastReceivedAddress = H323TransportAddress(address, port);
        return TRUE;
    }

    PTRACE(1, "UDP\tReceived PDU from incorrect host: " << address << ':' << port);
  }
}


BOOL H323TransportUDP::WritePDU(const PBYTEArray & pdu)
{
  return Write((const BYTE *)pdu, pdu.GetSize());
}


BOOL H323TransportUDP::DiscoverGatekeeper(H323Gatekeeper & gk,
                                          H323RasPDU & request,
                                          const H323TransportAddress & address)
{
  PINDEX i;

  PTRACE(3, "H225\tStarted gatekeeper discovery of \"" << address << '"');

  PIPSocket::Address destAddr = INADDR_BROADCAST;
  WORD destPort = H225_RAS::DefaultRasUdpPort;
  if (!address) {
    if (!address.GetIpAndPort(destAddr, destPort, "udp")) {
      PTRACE(2, "RAS\tError decoding address");
      return FALSE;
    }
  }

  // Skip over the H323Transport::Close to make sure PUDPSocket is deleted.
  PIndirectChannel::Close();

  remoteAddress = 0;
  remotePort = 0;

  // Remember the original info for pre-bound socket
  PIPSocket::Address originalLocalAddress = localAddress;
  WORD originalLocalPort = 0;

  // Get the interfaces to try
  PIPSocket::InterfaceTable interfaces;

  // See if prebound to interface, only use that if so
  if (destAddr.IsLoopback()) {
    PTRACE(3, "RAS\tGatekeeper discovery on loopback interface");
    localAddress = destAddr;
  }
  else if (!localAddress.IsAny()) {
    PTRACE(3, "RAS\tGatekeeper discovery on pre-bound interface: "
              << localAddress << ':' << localPort);
    originalLocalPort = localPort;
  }
  else if (!PIPSocket::GetInterfaceTable(interfaces)) {
    PTRACE(1, "RAS\tNo interfaces on system!");
  }
  else {
    PTRACE(4, "RAS\tSearching interfaces:\n" << setfill('\n') << interfaces << setfill(' '));
    // Check for if destination machine is local machine, if so only use that interface
    for (i = 0; i < interfaces.GetSize(); i++) {
      if (interfaces[i].GetAddress() == destAddr) {
        PTRACE(3, "RAS\tGatekeeper discovery on local interface: " << destAddr);
        localAddress = destAddr;
        interfaces.RemoveAll();
      }
    }
  }

  if (interfaces.IsEmpty())
    interfaces.Append(new PIPSocket::InterfaceEntry("", localAddress, PIPSocket::Address(0xffffffff), ""));

#ifdef P_STUN
  PSTUNClient * stun = endpoint.GetSTUN(remoteAddress);
#endif

  PSocketList sockets;
  PSocket::SelectList selection;
  H225_GatekeeperRequest & grq = request;

  for (i = 0; i < interfaces.GetSize(); i++) {
    localAddress = interfaces[i].GetAddress();
    if (localAddress == 0 || (destAddr != localAddress && localAddress.IsLoopback()))
      continue;

    // Check for already have had that IP address.
    PINDEX j;
    for (j = 0; j < i; j++) {
      if (localAddress == interfaces[j].GetAddress())
        break;
    }
    if (j < i)
      continue;

    PUDPSocket * socket;

    static PIPSocket::Address MulticastRasAddress(224, 0, 1, 41);
    if (destAddr != MulticastRasAddress) {

#ifdef P_STUN
      // Not explicitly multicast
      if (stun != NULL && stun->CreateSocket(socket)) {
        socket->GetLocalAddress(localAddress, localPort);
        PTRACE(4, "H323UDP\tSTUN created socket: " << localAddress << ':' << localPort);
      }
      else
#endif
      {
        socket = new PUDPSocket;
        if (!ListenUDP(*socket, endpoint, localAddress, originalLocalPort)) {
          delete socket;
          return FALSE;
        }
        localPort = socket->GetPort();
      }

      sockets.Append(socket);

      if (destAddr == INADDR_BROADCAST) {
        if (!socket->SetOption(SO_BROADCAST, 1)) {
          PTRACE(2, "RAS\tError allowing broadcast: " << socket->GetErrorText());
          return FALSE;
        }
      }

      // Adjust the PDU to reflect the interface we are writing to.
      PIPSocket::Address ipAddr = localAddress;
      endpoint.InternalTranslateTCPAddress(ipAddr, destAddr);
      endpoint.TranslateTCPPort(localPort, destAddr);
      H323TransportAddress(ipAddr, localPort).SetPDU(grq.m_rasAddress);

      PTRACE(3, "RAS\tGatekeeper discovery on interface: " << localAddress << ':' << localPort);

      socket->SetSendAddress(destAddr, destPort);
      writeChannel = socket;
      if (request.Write(*this))
        selection.Append(socket);
      else
        PTRACE(2, "RAS\tError writing discovery PDU: " << socket->GetErrorText());

      if (destAddr == INADDR_BROADCAST)
        socket->SetOption(SO_BROADCAST, 0);
    }


#ifdef IP_ADD_MEMBERSHIP
    // Now do it again for Multicast
    if (destAddr == INADDR_BROADCAST || destAddr == MulticastRasAddress) {
      socket = new PUDPSocket;
      sockets.Append(socket);

      if (!ListenUDP(*socket, endpoint, localAddress, 0))
        return FALSE;

      localPort = socket->GetPort();

      struct ip_mreq mreq;
      mreq.imr_multiaddr = MulticastRasAddress;
      mreq.imr_interface = localAddress;    // ip address of host
      if (socket->SetOption(IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq), IPPROTO_IP)) {
        // Adjust the PDU to reflect the interface we are writing to.
        SetUpTransportPDU(grq.m_rasAddress, TRUE);

        socket->SetOption(SO_BROADCAST, 1);

        socket->SetSendAddress(INADDR_BROADCAST, H225_RAS::DefaultRasMulticastPort);
        writeChannel = socket;
        if (request.Write(*this))
          selection.Append(socket);
        else
          PTRACE(2, "RAS\tError writing discovery PDU: " << socket->GetErrorText());

        socket->SetOption(SO_BROADCAST, 0);
      }
      else
        PTRACE(2, "RAS\tError allowing multicast: " << socket->GetErrorText());
    }
#endif

    writeChannel = NULL;
  }

  if (sockets.IsEmpty()) {
    PTRACE(1, "RAS\tNo suitable interfaces for discovery!");
    return FALSE;
  }

  if (PSocket::Select(selection, endpoint.GetGatekeeperRequestTimeout()) != NoError) {
    PTRACE(3, "RAS\tError on discover request select");
    return FALSE;
  }

  SetReadTimeout(0);

  for (i = 0; i < selection.GetSize(); i++) {
    readChannel = &selection[i];
    promiscuousReads = AcceptFromAnyAutoSet;

    H323RasPDU response;
    if (!response.Read(*this)) {
      PTRACE(3, "RAS\tError on discover request read: " << readChannel->GetErrorText());
      break;
    }

    do {
      if (gk.HandleTransaction(response)) {
        if (!gk.IsDiscoveryComplete()) {
          localAddress = originalLocalAddress;
          localPort = originalLocalPort;
          promiscuousReads = AcceptFromRemoteOnly;
          readChannel = NULL;
          return TRUE;
        }

        PUDPSocket * socket = (PUDPSocket *)readChannel;
        socket->GetLocalAddress(localAddress, localPort);
        readChannel = NULL;
        if (Open(socket) && Connect()) {
          sockets.DisallowDeleteObjects();
          sockets.Remove(socket);
          sockets.AllowDeleteObjects();

          promiscuousReads = AcceptFromRemoteOnly;

          PTRACE(2, "RAS\tGatekeeper discovered at: "
                 << remoteAddress << ':' << remotePort
                 << " (if="
                 << localAddress << ':' << localPort << ')');
          return TRUE;
        }
      }
    } while (response.Read(*this));
  }

  PTRACE(2, "RAS\tGatekeeper discovery failed");
  localAddress = originalLocalAddress;
  localPort = originalLocalPort;
  promiscuousReads = AcceptFromRemoteOnly;
  readChannel = NULL;
  return FALSE;
}

H323TransportAddress H323TransportUDP::GetLocalAddress() const
{
  if (canGetInterface && !lastReceivedInterface.IsLoopback()) 
    return H323TransportAddress(lastReceivedInterface, interfacePort);

  // check for special case of local interface, which means the PDU came from the same machine
  H323TransportAddress taddr = H323TransportIP::GetLocalAddress();
  if (!lastReceivedAddress.IsEmpty()) {
    PIPSocket::Address tipAddr;
    WORD tipPort;
    taddr.GetIpAndPort(tipAddr, tipPort);
    if (tipAddr == PIPSocket::Address(0)) {
      PIPSocket::Address lastRxIPAddr;
      lastReceivedAddress.GetIpAddress(lastRxIPAddr);
      if (lastRxIPAddr != PIPSocket::Address())
        taddr = H323TransportAddress(lastRxIPAddr, tipPort);
    }
  }

  return taddr;
}

/////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
  #pragma warning(default : 4244)
#endif
