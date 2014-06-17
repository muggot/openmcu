/*
 * gkclient.cxx
 *
 * Gatekeeper client protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1999-2000 Equivalence Pty. Ltd.
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
 * iFace In, http://www.iface.com
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: gkclient.cxx,v $
 * Revision 1.6  2008/01/29 04:33:24  shorne
 * Added SendServiceControlIndication - fixed Endpoint initiation with H.460
 *
 * Revision 1.5  2008/01/04 06:23:09  shorne
 * Cleaner setup and teardown of h460 module
 *
 * Revision 1.4  2008/01/01 00:16:12  shorne
 * Added GnuGknat and FileTransfer support
 *
 * Revision 1.3  2007/11/10 23:07:35  willamowius
 * fix --disable-h450
 *
 * Revision 1.2  2007/10/16 17:02:47  shorne
 * Fix for H.235.1 on full reregistration
 *
 * Revision 1.1  2007/08/06 20:51:04  shorne
 * First commit of h323plus
 *
 * Revision 1.165.2.5  2007/07/23 21:47:11  shorne
 * Added QoS GK Reporting
 *
 * Revision 1.165.2.4  2007/07/19 20:05:56  shorne
 * Changed Terminal Aliases to be only sent on full registration
 *
 * Revision 1.165.2.3  2007/05/23 06:59:36  shorne
 * Added Assigned Alias/Gatekeeper
 *
 * Revision 1.165.2.2  2007/04/19 12:16:16  shorne
 * added ability to detect if no nat
 *
 * Revision 1.165.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.165  2006/07/23 23:27:55  shorne
 * supportsAssignedGK made optional field in RRQ & GRQ for backwards interoperability
 *
 * Revision 1.164  2006/07/21 16:29:13  csoutheren
 * Initialise mandatory extension elements in GRQ and RRQ for H.225 V6
 *
 * Revision 1.163  2006/06/30 05:26:43  csoutheren
 * Applied 1509255 - Checking whether SetSize succeeds in GkClient
 * Thanks to Borko Jandras
 *
 * Revision 1.162  2006/06/09 06:30:12  csoutheren
 * Remove compile warning and errors with gcc
 *
 * Revision 1.161  2006/05/30 11:14:56  hfriederich
 * Switch from DISABLE_H460 to H323_H460
 *
 * Revision 1.160  2006/05/29 02:31:45  shorne
 * H460 functions now get called
 *
 * Revision 1.159  2006/05/18 17:15:54  shorne
 * Added H460 Support
 *
 * Revision 1.158  2006/05/16 11:39:39  shorne
 * call linkage support
 *
 * Revision 1.157  2006/03/07 10:37:46  csoutheren
 * Add ability to disable GRQ on GK registration
 *
 * Revision 1.156  2005/11/21 20:52:35  shorne
 * Added GnuGK Nat detection support
 *
 * Revision 1.155  2005/01/16 20:39:44  csoutheren
 * Fixed problem with IPv6 INADDR_ANY
 *
 * Revision 1.154  2005/01/03 06:25:54  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.153  2004/11/20 22:00:10  csoutheren
 * Check address from RequestLocation due to stupid gatekeepers
 *
 * Revision 1.152  2004/09/07 23:51:46  rjongbloed
 * Fixed MSVC6 warning
 *
 * Revision 1.151  2004/09/03 01:06:09  csoutheren
 * Added initial hooks for H.460 GEF
 * Thanks to Simon Horne and ISVO (Asia) Pte Ltd. for this contribution
 *
 * Revision 1.150  2004/06/15 03:30:00  csoutheren
 * Added OnSendARQ to allow access to the ARQ message before sent by connection
 *
 * Revision 1.149  2004/04/03 08:28:06  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.148  2004/01/17 18:20:15  csoutheren
 * No longer force re-register on completion of LRQ
 *
 * Revision 1.147  2003/12/29 04:59:25  csoutheren
 * Added callbacks on H323EndPoint when gatekeeper discovery succeeds or fails
 *
 * Revision 1.146  2003/12/28 00:06:34  csoutheren
 * Added callbacks on H323EndPoint when gatekeeper registration succeeds or fails
 *
 * Revision 1.145  2003/05/01 05:04:00  robertj
 * Fixed inclusion of 127.0.0.1 into listener lists when no needed.
 *
 * Revision 1.144  2003/04/30 07:25:32  robertj
 * Fixed setting of remote ID in alternate credentials.
 *
 * Revision 1.143  2003/04/30 00:28:54  robertj
 * Redesigned the alternate credentials in ARQ system as old implementation
 *   was fraught with concurrency issues, most importantly it can cause false
 *   detection of replay attacks taking out an endpoint completely.
 *
 * Revision 1.142  2003/04/10 09:44:31  robertj
 * Added associated transport to new GetInterfaceAddresses() function so
 *   interfaces can be ordered according to active transport links. Improves
 *   interoperability.
 * Replaced old listener GetTransportPDU() with GetInterfaceAddresses()
 *   and H323SetTransportAddresses() functions.
 *
 * Revision 1.141  2003/04/09 03:08:10  robertj
 * Fixed race condition in shutting down transactor (pure virtual call)
 *
 * Revision 1.140  2003/03/26 00:46:28  robertj
 * Had another go at making H323Transactor being able to be created
 *   without having a listener running.
 *
 * Revision 1.139  2003/03/20 01:51:11  robertj
 * More abstraction of H.225 RAS and H.501 protocols transaction handling.
 *
 * Revision 1.138  2003/02/21 05:25:45  craigs
 * Abstracted out underlying transports for use with peerelements
 *
 * Revision 1.137  2003/02/12 23:59:25  robertj
 * Fixed adding missing endpoint identifer in SETUP packet when gatekeeper
 * routed, pointed out by Stefan Klein
 * Also fixed correct rutrn of gk routing in IRR packet.
 *
 * Revision 1.136  2003/02/11 04:46:37  robertj
 * Fixed keep alive RRQ being rejected with full registration required
 *   reason actually doing a full registration!
 *
 * Revision 1.135  2003/02/10 01:51:50  robertj
 * Fixed bad tokens causing an apparent "transport error", now correctly
 *   indicates a security error.
 *
 * Revision 1.134  2003/02/07 06:38:47  robertj
 * Changed registration state to an enum so can determine why the RRQ failed.
 *
 * Revision 1.133  2003/02/04 07:04:45  robertj
 * Prevent multiple calls to Connect() if did not change the gk.
 *
 * Revision 1.132  2003/02/01 13:31:21  robertj
 * Changes to support CAT authentication in RAS.
 *
 * Revision 1.131  2003/01/11 05:04:03  robertj
 * Added checks for valid URQ packet, thanks Chih-Wei Huang
 *
 * Revision 1.130  2003/01/09 04:45:04  robertj
 * Fixed problem where if gets GRJ which does not have an alternate gatekeeper
 *   the system gets into an infinite loop, pointed out by Vladimir Toncar
 *
 * Revision 1.129  2003/01/06 07:09:43  robertj
 * Further fixes for alternate gatekeeper, thanks Kevin Bouchard
 *
 * Revision 1.128  2002/12/23 22:47:53  robertj
 * Changed gatekeeper discovery so an GRJ does not indicate "discovered".
 * Added trace output of alternate gatekeepers list.
 * Fixed receiving GRJ with alternate gatekeepers to immediately  do discover
 *   and registration on the alternate.
 *
 * Revision 1.127  2002/12/19 23:52:53  robertj
 * Fixed probelm with registering with alternate gk, thanks Kevin Bouchard
 *
 * Revision 1.126  2002/12/10 23:39:03  robertj
 * Added some extra tracing.
 *
 * Revision 1.125  2002/11/28 04:41:48  robertj
 * Added support for RAS ServiceControlIndication command.
 *
 * Revision 1.124  2002/11/27 06:54:56  robertj
 * Added Service Control Session management as per Annex K/H.323 via RAS
 *   only at this stage.
 * Added H.248 ASN and very primitive infrastructure for linking into the
 *   Service Control Session management system.
 * Added basic infrastructure for Annex K/H.323 HTTP transport system.
 * Added Call Credit Service Control to display account balances.
 *
 * Revision 1.123  2002/11/22 07:16:14  robertj
 * Changed ARQ to include all local aliases for connection.
 *
 * Revision 1.122  2002/11/21 07:29:15  robertj
 * Fixed GNU warning
 *
 * Revision 1.121  2002/11/21 07:21:49  robertj
 * Improvements to alternate gatekeeper client code, thanks Kevin Bouchard
 *
 * Revision 1.120  2002/11/12 03:13:24  robertj
 * Removed redundent code.
 *
 * Revision 1.119  2002/11/10 08:10:43  robertj
 * Moved constants for "well known" ports to better place (OPAL change).
 *
 * Revision 1.118  2002/11/01 03:48:18  robertj
 * Fixed previous two hacks!! Neither of which would have worked.
 *
 * Revision 1.117  2002/10/31 23:31:41  dereks
 * Fix for previous quick hack. Thanks Damien Sandras.
 *
 * Revision 1.116  2002/10/31 21:40:28  dereks
 * Quick (and temporary) hack to enable compilation on redhat 8.0 boxes.
 *
 * Revision 1.115  2002/09/19 23:19:25  robertj
 * Fixed setting of info request rate, broken in a previous patch
 *
 * Revision 1.114  2002/09/18 06:58:32  robertj
 * Fixed setting of IRR frequency, an RCF could reset timer so it did not time
 *   out correctly and send IRR in time causing problems with gatekeeper.
 *
 * Revision 1.113  2002/09/09 23:59:59  robertj
 * Fixed incorrect inserting of UUIE pdu's into IRR, thanks Ravelli Rossano
 *
 * Revision 1.112  2002/08/29 07:02:19  robertj
 * Allowed network latency deadband in unsolicited IRR response time.
 *
 * Revision 1.111  2002/08/28 00:07:02  robertj
 * Added supportsAltGK capability flag in GRQ & RRQ.
 *
 * Revision 1.110  2002/08/15 09:38:55  robertj
 * Added more logging for when endpoint becomes unregistered.
 *
 * Revision 1.109  2002/08/15 04:12:54  robertj
 * Fixed correct status of isRegistered flag on various reject/errors.
 *
 * Revision 1.108  2002/08/12 05:38:24  robertj
 * Changes to the RAS subsystem to support ability to make requests to client
 *   from gkserver without causing bottlenecks and race conditions.
 *
 * Revision 1.107  2002/08/11 23:30:36  robertj
 * Fixed typo in previous patch.
 *
 * Revision 1.106  2002/08/11 23:24:24  robertj
 * Fixed problem with retrying ARQ after getting error saying are not
 *   registered and reregistering, needed new sequence number.
 * Fixed return of correct error in ARQ response when is a transport error
 *   rather than a reject reason from ARJ.
 *
 * Revision 1.105  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.104  2002/08/05 05:17:41  robertj
 * Fairly major modifications to support different authentication credentials
 *   in ARQ to the logged in ones on RRQ. For both client and server.
 * Various other H.235 authentication bugs and anomalies fixed on the way.
 *
 * Revision 1.103  2002/07/19 10:20:03  robertj
 * Fixed bug of missing test for IRR frequency in RCF, thanks Thien Nguyen
 *
 * Revision 1.102  2002/07/18 03:03:19  robertj
 * Fixed bug with continually doing lightweight RRQ if no timeToLive present
 *   and it should not be doing it at all, ditto for unsolicited IRR.
 *
 * Revision 1.101  2002/07/17 00:04:10  robertj
 * Fixed missing initialisation of alternat gk pointer to NULL, thanks Kevin Bouchard
 *
 * Revision 1.100  2002/07/16 13:19:13  robertj
 * Minor optimisation of unsolicited IRR when no calls active.
 *
 * Revision 1.99  2002/07/16 11:06:27  robertj
 * Added more alternate gatekeeper implementation, thanks Kevin Bouchard
 *
 * Revision 1.98  2002/07/11 09:34:32  robertj
 * Fixed minor compliance to letter of specification.
 *
 * Revision 1.97  2002/07/11 01:34:37  robertj
 * Temporary fix for IRR frequency provided in ACF
 *
 * Revision 1.96  2002/07/07 02:08:53  robertj
 * Fixed missing originator field in IRR perCallInfo, thanks Ravelli Rossano
 *
 * Revision 1.95  2002/06/28 03:34:28  robertj
 * Fixed issues with address translation on gatekeeper RAS channel.
 *
 * Revision 1.94  2002/06/26 03:47:49  robertj
 * Added support for alternate gatekeepers.
 *
 * Revision 1.93  2002/06/26 00:50:12  robertj
 * Added other error code in ARJ that indicates we should reregister.
 *
 * Revision 1.92  2002/06/05 09:20:07  robertj
 * Added IRQ redirect of IRR to different address, thanks "thsuk".
 *
 * Revision 1.91  2002/05/29 00:03:19  robertj
 * Fixed unsolicited IRR support in gk client and server,
 *   including support for IACK and INAK.
 *
 * Revision 1.90  2002/05/17 04:01:53  robertj
 * Fixed problems with H.235 authentication on RAS for server and client.
 * Added support for unsolicited IRR transmission in background (heartbeat).
 *
 * Revision 1.89  2002/05/09 05:43:44  robertj
 * Added reattempt of full RRQ if get fullRegistrationRequired RRJ.
 *
 * Revision 1.88  2002/05/01 06:39:41  robertj
 * Fixed incorrect setting of srcCallSignalAddress in ARQ for outgoing call as
 *   putting in incorrect data is worse than not putting anything in at all! So
 *   unless the correct data is available it is now left out.
 *
 * Revision 1.87  2002/03/20 02:12:49  robertj
 * Added missing return value for number of endpoints returned in ACF
 *
 * Revision 1.86  2002/03/19 05:17:25  robertj
 * Normalised ACF destExtraCallIInfo to be same as other parameters.
 * Added ability to get multiple endpoint addresses and tokens from ACF.
 *
 * Revision 1.85  2002/03/01 04:06:44  robertj
 * Fixed autoreregister on ARQ failing due to unregistered endpoint.
 *
 * Revision 1.84  2002/02/11 04:25:57  robertj
 * Added ability to automatically reregister if do an ARQ and are told are not
 *   registered. Can occur if gk is restarted and is faster than waiting for TTL..
 *
 * Revision 1.83  2002/01/13 23:58:48  robertj
 * Added ability to set destination extra call info in ARQ
 * Filled in destinationInfo in ARQ when answering call.
 * Allowed application to override srcInfo in ARQ on outgoing call by
 *   changing localPartyName.
 * Added better end call codes for some ARJ reasons.
 * Thanks Ben Madsen of Norwood Systems.
 *
 * Revision 1.82  2001/12/15 10:10:48  robertj
 * GCC compatibility
 *
 * Revision 1.81  2001/12/15 08:36:49  robertj
 * Added previous call times to all the other PDU's it is supposed to be in!
 *
 * Revision 1.80  2001/12/15 08:09:21  robertj
 * Added alerting, connect and end of call times to be sent to RAS server.
 *
 * Revision 1.79  2001/12/14 06:41:36  robertj
 * Added call end reason codes in DisengageRequest for GK server use.
 *
 * Revision 1.78  2001/12/13 11:00:13  robertj
 * Changed search for access token in ACF to be able to look for two OID's.
 *
 * Revision 1.77  2001/12/06 06:44:42  robertj
 * Removed "Win32 SSL xxx" build configurations in favour of system
 *   environment variables to select optional libraries.
 *
 * Revision 1.76  2001/10/12 04:14:31  robertj
 * Changed gk unregister so only way it doe not actually unregister is if
 *   get URJ with reason code callInProgress, thanks Chris Purvis.
 *
 * Revision 1.75  2001/10/09 08:04:59  robertj
 * Fixed unregistration so still unregisters if gk goes offline, thanks Chris Purvis
 *
 * Revision 1.74  2001/10/08 01:37:42  robertj
 * Fixed uninitialised variable for ARQ authentication override.
 *
 * Revision 1.73  2001/09/26 07:03:08  robertj
 * Added needed mutex for SeparateAuthenticationInARQ mode, thanks Nick Hoath
 *
 * Revision 1.72  2001/09/18 10:36:57  robertj
 * Allowed multiple overlapping requests in RAS channel.
 *
 * Revision 1.71  2001/09/13 03:21:16  robertj
 * Added ability to override authentication credentials for ARQ, thanks Nick Hoath
 *
 * Revision 1.70  2001/09/13 01:15:20  robertj
 * Added flag to H235Authenticator to determine if gkid and epid is to be
 *   automatically set as the crypto token remote id and local id.
 *
 * Revision 1.69  2001/09/13 00:32:24  robertj
 * Added missing gkid in ARQ, thanks Nick Hoath
 *
 * Revision 1.68  2001/09/12 07:48:05  robertj
 * Fixed various problems with tracing.
 *
 * Revision 1.67  2001/09/12 06:58:00  robertj
 * Added support for iNow Access Token from gk, thanks Nick Hoath
 *
 * Revision 1.66  2001/09/12 06:04:38  robertj
 * Added support for sending UUIE's to gk on request, thanks Nick Hoath
 *
 * Revision 1.65  2001/09/05 01:16:32  robertj
 * Added overloaded AdmissionRequest for backward compatibility.
 *
 * Revision 1.64  2001/08/14 04:26:46  robertj
 * Completed the Cisco compatible MD5 authentications, thanks Wolfgang Platzer.
 *
 * Revision 1.63  2001/08/13 01:27:03  robertj
 * Changed GK admission so can return multiple aliases to be used in
 *   setup packet, thanks Nick Hoath.
 *
 * Revision 1.62  2001/08/13 00:22:14  robertj
 * Allowed for received DRQ not having call ID (eg v1 gk), uses conference ID
 *
 * Revision 1.61  2001/08/10 11:03:52  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 * Revision 1.60  2001/08/06 07:44:55  robertj
 * Fixed problems with building without SSL
 *
 * Revision 1.59  2001/08/06 03:18:38  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 * Improved access to H.235 secure RAS functionality.
 * Changes to H.323 secure RAS contexts to help use with gk server.
 *
 * Revision 1.58  2001/08/02 04:30:43  robertj
 * Added ability for AdmissionRequest to alter destination alias used in
 *   the outgoing call. Thanks Ben Madsen & Graeme Reid.
 *
 * Revision 1.57  2001/06/22 00:21:10  robertj
 * Fixed bug in H.225 RAS protocol with 16 versus 32 bit sequence numbers.
 *
 * Revision 1.56  2001/06/18 23:35:01  robertj
 * Removed condition that prevented aliases on non-terminal endpoints.
 *
 * Revision 1.55  2001/06/18 06:23:50  robertj
 * Split raw H.225 RAS protocol out of gatekeeper client class.
 *
 * Revision 1.54  2001/05/17 03:29:13  robertj
 * Fixed missing replyAddress in LRQ, thanks Alexander Smirnov.
 * Added some extra optional fields to LRQ.
 *
 * Revision 1.53  2001/04/19 08:03:21  robertj
 * Fixed scale on RIp delay, is milliseconds!
 *
 * Revision 1.52  2001/04/13 07:44:20  robertj
 * Fixed setting isRegistered flag to false when get RRJ
 *
 * Revision 1.51  2001/04/05 03:39:43  robertj
 * Fixed deadlock if tried to do discovery in time to live timeout.
 *
 * Revision 1.50  2001/03/28 07:13:06  robertj
 * Changed RAS thread interlock to allow for what should not happen, the
 *   syncpoint being signalled before receiving any packets.
 *
 * Revision 1.49  2001/03/27 02:19:22  robertj
 * Changed to send gk a GRQ if it gives a discoveryRequired error on RRQ.
 * Fixed BIG  condition in use of sequence numbers.
 *
 * Revision 1.48  2001/03/26 05:06:03  robertj
 * Added code to do full registration if RRJ indicates discovery to be redone.
 *
 * Revision 1.47  2001/03/24 00:51:41  robertj
 * Added retry every minute of time to live registration if fails.
 *
 * Revision 1.46  2001/03/23 01:47:49  robertj
 * Improved debug trace message on RAS packet retry.
 *
 * Revision 1.45  2001/03/23 01:19:25  robertj
 * Fixed usage of secure RAS in GRQ, should not do for that one PDU.
 *
 * Revision 1.44  2001/03/21 04:52:42  robertj
 * Added H.235 security to gatekeepers, thanks Fürbass Franz!
 *
 * Revision 1.43  2001/03/19 23:32:30  robertj
 * Fixed problem with auto-reregister doing so in the RAS receive thread.
 *
 * Revision 1.42  2001/03/19 05:50:52  robertj
 * Fixed trace display of timeout value.
 *
 * Revision 1.41  2001/03/18 22:21:29  robertj
 * Fixed GNU C++ problem.
 *
 * Revision 1.40  2001/03/17 00:05:52  robertj
 * Fixed problems with Gatekeeper RIP handling.
 *
 * Revision 1.39  2001/03/16 06:46:21  robertj
 * Added ability to set endpoints desired time to live on a gatekeeper.
 *
 * Revision 1.38  2001/03/15 00:25:58  robertj
 * Fixed bug in receiving RIP packet, did not restart timeout.
 *
 * Revision 1.37  2001/03/09 02:55:53  robertj
 * Fixed bug in RAS IRR, optional field not being included, thanks Erik Larsson.
 *
 * Revision 1.36  2001/03/02 06:59:59  robertj
 * Enhanced the globally unique identifier class.
 *
 * Revision 1.35  2001/02/28 00:20:16  robertj
 * Added DiscoverByNameAndAddress() function, thanks Chris Purvis.
 *
 * Revision 1.34  2001/02/18 22:33:47  robertj
 * Added better handling of URJ, thanks Chris Purvis.
 *
 * Revision 1.33  2001/02/09 05:13:55  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.32  2001/01/25 01:44:26  robertj
 * Reversed order of changing alias list to avoid assert if delete all aliases.
 *
 * Revision 1.31  2000/11/01 03:30:27  robertj
 * Changed gatekeeper registration time to live to update in slightly less than the
 *    time to live time. Allows for system/network latency. Thanks Laurent PELLETIER.
 *
 * Revision 1.30  2000/09/25 06:48:11  robertj
 * Removed use of alias if there is no alias present, ie only have transport address.
 *
 * Revision 1.29  2000/09/01 02:12:37  robertj
 * Fixed problem when multiple GK's on LAN, only discovered first one.
 * Added ability to select a gatekeeper on LAN via it's identifier name.
 *
 * Revision 1.28  2000/07/15 09:54:21  robertj
 * Fixed problem with having empty or unusable assigned aliases.
 *
 * Revision 1.27  2000/07/11 19:26:39  robertj
 * Fixed problem with endpoint identifiers from some gatekeepers not being a string, just binary info.
 *
 * Revision 1.26  2000/06/20 03:18:04  robertj
 * Added function to get name of gatekeeper, subtle difference from getting identifier.
 *
 * Revision 1.25  2000/05/09 12:14:32  robertj
 * Added adjustment of endpoints alias list as approved by gatekeeper.
 *
 * Revision 1.24  2000/05/09 08:52:50  robertj
 * Added support for preGrantedARQ fields on registration.
 *
 * Revision 1.23  2000/05/04 10:43:54  robertj
 * Fixed problem with still trying to RRQ if got a GRJ.
 *
 * Revision 1.22  2000/05/02 04:32:26  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.21  2000/04/27 02:52:58  robertj
 * Added keepAlive field to RRQ if already registered,
 *
 * Revision 1.20  2000/04/12 21:22:16  robertj
 * Fixed warning in No Trace mode.
 *
 * Revision 1.19  2000/04/11 04:00:55  robertj
 * Filled in destCallSignallingAddress if specified by caller, used for gateway permissions.
 *
 * Revision 1.18  2000/04/11 03:11:12  robertj
 * Added ability to reject reason on gatekeeper requests.
 *
 * Revision 1.17  2000/03/29 02:14:43  robertj
 * Changed TerminationReason to CallEndReason to use correct telephony nomenclature.
 * Added CallEndReason for capability exchange failure.
 *
 * Revision 1.16  2000/03/23 02:45:28  robertj
 * Changed ClearAllCalls() so will wait for calls to be closed (usefull in endpoint dtors).
 *
 * Revision 1.15  2000/03/21 23:17:55  robertj
 * Changed GK client so does not fill in destCallSignalAddress on outgoing call.
 *
 * Revision 1.14  2000/01/28 00:56:48  robertj
 * Changed ACF to return destination address irrespective of callModel, thanks Chris Gindel.
 *
 * Revision 1.13  1999/12/23 23:02:35  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 * Revision 1.12  1999/12/11 02:20:58  robertj
 * Added ability to have multiple aliases on local endpoint.
 *
 * Revision 1.11  1999/12/10 01:43:25  robertj
 * Fixed outgoing call Admissionrequestion addresses.
 *
 * Revision 1.10  1999/12/09 21:49:18  robertj
 * Added reregister on unregister and time to live reregistration
 *
 * Revision 1.9  1999/11/06 05:37:45  robertj
 * Complete rewrite of termination of connection to avoid numerous race conditions.
 *
 * Revision 1.8  1999/10/16 03:47:48  robertj
 * Fixed termination of gatekeeper RAS thread problem
 *
 * Revision 1.7  1999/10/15 05:55:50  robertj
 * Fixed crash in responding to InfoRequest
 *
 * Revision 1.6  1999/09/23 08:48:45  robertj
 * Changed register request so cannot do it of have no listeners.
 *
 * Revision 1.5  1999/09/21 14:09:19  robertj
 * Removed warnings when no tracing enabled.
 *
 * Revision 1.4  1999/09/14 08:19:37  robertj
 * Fixed timeout on retry of gatekeeper discover and added more tracing.
 *
 * Revision 1.3  1999/09/14 06:52:54  robertj
 * Added better support for multi-homed client hosts.
 *
 * Revision 1.2  1999/09/10 02:45:31  robertj
 * Added missing binding of address to transport when a specific gatway is used.
 *
 * Revision 1.1  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "gkclient.h"
#endif

#if defined(_WIN32) && (_MSC_VER > 1300)
  #pragma warning(disable:4244) // warning about possible loss of data
#endif

#include "gkclient.h"

#include "h323ep.h"
#include "h323rtp.h"
#include "h323pdu.h"

#ifdef H323_H460
#include "h460/h4601.h"
#endif

#define new PNEW


static PTimeInterval AdjustTimeout(unsigned seconds)
{
  // Allow for an incredible amount of system/network latency
  static unsigned TimeoutDeadband = 5; // seconds

  return PTimeInterval(0, seconds > TimeoutDeadband
                              ? (seconds - TimeoutDeadband)
                              : TimeoutDeadband);
}


/////////////////////////////////////////////////////////////////////////////

H323Gatekeeper::H323Gatekeeper(H323EndPoint & ep, H323Transport * trans)
  : H225_RAS(ep, trans),
    requestMutex(1, 1),
    authenticators(ep.CreateAuthenticators())
#ifdef H323_H460
    ,features(ep.GetFeatureSet())
#endif
{
  alternatePermanent = FALSE;
  discoveryComplete = FALSE;
  registrationFailReason = UnregisteredLocally;

  pregrantMakeCall = pregrantAnswerCall = RequireARQ;

  autoReregister = TRUE;
  reregisterNow = FALSE;
  requiresDiscovery = FALSE;

  timeToLive.SetNotifier(PCREATE_NOTIFIER(TickleMonitor));
  infoRequestRate.SetNotifier(PCREATE_NOTIFIER(TickleMonitor));

  willRespondToIRR = FALSE;
  monitorStop = FALSE;

  monitor = PThread::Create(PCREATE_NOTIFIER(MonitorMain), 0,
                            PThread::NoAutoDeleteThread,
                            PThread::NormalPriority,
                            "GkMonitor:%x");
#ifdef H323_H460
  features->AttachEndPoint(&ep);
  features->LoadFeatureSet(H460_Feature::FeatureRas);
#endif

  localId = PString();
  assignedGK = NULL;
}


H323Gatekeeper::~H323Gatekeeper()
{
  if (monitor != NULL) {
    monitorStop = TRUE;
    monitorTickle.Signal();
    monitor->WaitForTermination();
    delete monitor;
  }
#ifdef H323_H460
  delete features;
#endif

  StopChannel();
}


PString H323Gatekeeper::GetName() const
{
  PStringStream s;
  s << *this;
  return s;
}


BOOL H323Gatekeeper::DiscoverAny()
{
  gatekeeperIdentifier = PString();
  return StartDiscovery(H323TransportAddress());
}


BOOL H323Gatekeeper::DiscoverByName(const PString & identifier)
{
  gatekeeperIdentifier = identifier;
  return StartDiscovery(H323TransportAddress());
}


BOOL H323Gatekeeper::DiscoverByAddress(const H323TransportAddress & address)
{
  gatekeeperIdentifier = PString();
  return StartDiscovery(address);
}


BOOL H323Gatekeeper::DiscoverByNameAndAddress(const PString & identifier,
                                              const H323TransportAddress & address)
{
  gatekeeperIdentifier = identifier;
  return StartDiscovery(address);
}

BOOL H323Gatekeeper::StartDiscovery(const H323TransportAddress & initialAddress)
{
  if (PAssertNULL(transport) == NULL)
    return FALSE;

  /// don't send GRQ if not requested
  if (!endpoint.GetSendGRQ() && !initialAddress.IsEmpty()) {
    transport->SetRemoteAddress(initialAddress);
    if (!transport->Connect()) {
      PTRACE(2, "RAS\tUnable to connect to gatekeeper at " << initialAddress);
      return FALSE;
    }
    transport->SetPromiscuous(H323Transport::AcceptFromRemoteOnly);
    StartChannel();
    PTRACE(2, "RAS\tSkipping gatekeeper discovery for " << initialAddress);
    return TRUE;
  }

  H323RasPDU pdu;
  Request request(SetupGatekeeperRequest(pdu), pdu);

  H323TransportAddress address = initialAddress;
  request.responseInfo = &address;

  requestsMutex.Wait();
  requests.SetAt(request.sequenceNumber, &request);
  requestsMutex.Signal();

  discoveryComplete = FALSE;
  unsigned retries = endpoint.GetGatekeeperRequestRetries();
  while (!discoveryComplete) {
    if (transport->DiscoverGatekeeper(*this, pdu, address)) {
      if (address == initialAddress)
        break;
      /// If get here must have been GRJ with an alternate gk, start again
    }
    else {
      // Transport failure, retry
      if (--retries == 0)
        break;
    }
  }

  requestsMutex.Wait();
  requests.SetAt(request.sequenceNumber, NULL);
  requestsMutex.Signal();

  if (discoveryComplete) {
    if (transport->Connect())
      StartChannel();
  }

  return discoveryComplete;
}


unsigned H323Gatekeeper::SetupGatekeeperRequest(H323RasPDU & request)
{
  if (PAssertNULL(transport) == NULL)
    return 0;

  H225_GatekeeperRequest & grq = request.BuildGatekeeperRequest(GetNextSequenceNumber());

  endpoint.SetEndpointTypeInfo(grq.m_endpointType);
  transport->SetUpTransportPDU(grq.m_rasAddress, TRUE);

  grq.IncludeOptionalField(H225_GatekeeperRequest::e_endpointAlias);
  H323SetAliasAddresses(endpoint.GetAliasNames(), grq.m_endpointAlias);

  if (!gatekeeperIdentifier) {
    grq.IncludeOptionalField(H225_GatekeeperRequest::e_gatekeeperIdentifier);
    grq.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

  grq.IncludeOptionalField(H225_GatekeeperRequest::e_supportsAltGK);

  grq.IncludeOptionalField(H225_GatekeeperRequest::e_supportsAssignedGK);
  grq.m_supportsAssignedGK = TRUE;

  OnSendGatekeeperRequest(grq);

  discoveryComplete = FALSE;

  return grq.m_requestSeqNum;
}


void H323Gatekeeper::OnSendGatekeeperRequest(H225_GatekeeperRequest & grq)
{
  H225_RAS::OnSendGatekeeperRequest(grq);

  for (PINDEX i = 0; i < authenticators.GetSize(); i++) {
    if (authenticators[i].SetCapability(grq.m_authenticationCapability, grq.m_algorithmOIDs)) {
      grq.IncludeOptionalField(H225_GatekeeperRequest::e_authenticationCapability);
      grq.IncludeOptionalField(H225_GatekeeperRequest::e_algorithmOIDs);
    }
  }
}


BOOL H323Gatekeeper::OnReceiveGatekeeperConfirm(const H225_GatekeeperConfirm & gcf)
{
  if (!H225_RAS::OnReceiveGatekeeperConfirm(gcf))
    return FALSE;

  PINDEX i;

  for (i = 0; i < authenticators.GetSize(); i++) {
    H235Authenticator & authenticator = authenticators[i];
    if (authenticator.UseGkAndEpIdentifiers())
      authenticator.SetRemoteId(gatekeeperIdentifier);
  }

  if (gcf.HasOptionalField(H225_GatekeeperConfirm::e_authenticationMode) &&
      gcf.HasOptionalField(H225_GatekeeperConfirm::e_algorithmOID)) {
    for (i = 0; i < authenticators.GetSize(); i++) {
      H235Authenticator & authenticator = authenticators[i];
      authenticator.Enable(authenticator.IsCapability(gcf.m_authenticationMode,
                                                      gcf.m_algorithmOID));
	  PTRACE(4,"RAS\tAuthenticator " << authenticator.GetName() 
		              << (authenticator.IsActive() ? " ACTIVATED" : " disabled"));
    }
  }

  H323TransportAddress locatedAddress = gcf.m_rasAddress;
  PTRACE(2, "RAS\tGatekeeper discovery found " << locatedAddress);

  if (!transport->SetRemoteAddress(locatedAddress)) {
    PTRACE(2, "RAS\tInvalid gatekeeper discovery address: \"" << locatedAddress << '"');
    return FALSE;
  }

  if (gcf.HasOptionalField(H225_GatekeeperConfirm::e_alternateGatekeeper))
    SetAlternates(gcf.m_alternateGatekeeper, FALSE);

  if (gcf.HasOptionalField(H225_GatekeeperConfirm::e_assignedGatekeeper)) {
    SetAssignedGatekeeper(gcf.m_assignedGatekeeper);
    PTRACE(2, "RAS\tAssigned Gatekeeper redirected " << assignedGK);
	// This will force the gatekeeper to register to the assigned Gatekeeper.
	if (lastRequest->responseInfo != NULL) {
      H323TransportAddress & gkAddress = *(H323TransportAddress *)lastRequest->responseInfo;
      gkAddress = assignedGK->rasAddress;
	  gatekeeperIdentifier = PString();
	}
  } else {
    endpoint.OnGatekeeperConfirm();
    discoveryComplete = TRUE;
  }
  return TRUE;
}


BOOL H323Gatekeeper::OnReceiveGatekeeperReject(const H225_GatekeeperReject & grj)
{
  if (!H225_RAS::OnReceiveGatekeeperReject(grj))
    return FALSE;

  if (grj.HasOptionalField(H225_GatekeeperReject::e_altGKInfo)) {
    SetAlternates(grj.m_altGKInfo.m_alternateGatekeeper,
                  grj.m_altGKInfo.m_altGKisPermanent);
  }

  if ((alternates.GetSize() > 0) && (lastRequest->responseInfo != NULL)) {
      H323TransportAddress & gkAddress = *(H323TransportAddress *)lastRequest->responseInfo;
      gkAddress = alternates[0].rasAddress;
  }

  endpoint.OnGatekeeperReject();

  return TRUE;
}


BOOL H323Gatekeeper::RegistrationRequest(BOOL autoReg)
{
  if (PAssertNULL(transport) == NULL)
    return FALSE;

  autoReregister = autoReg;

  H323RasPDU pdu;
  H225_RegistrationRequest & rrq = pdu.BuildRegistrationRequest(GetNextSequenceNumber());

  // If discoveryComplete flag is FALSE then do lightweight reregister
  rrq.m_discoveryComplete = discoveryComplete;

  // Check if the IP address might of changed since last registration (for DDNS Type registrations)
  H323TransportAddress newaddress;
  if ((!discoveryComplete) && (endpoint.GatekeeperCheckIP(transport->GetRemoteAddress(),newaddress)))
	  transport->SetRemoteAddress(newaddress);

  rrq.m_rasAddress.SetSize(1);
  transport->SetUpTransportPDU(rrq.m_rasAddress[0], TRUE);

  H323TransportAddressArray listeners = endpoint.GetInterfaceAddresses(TRUE, transport);
  if (listeners.IsEmpty()) {
    PTRACE(1, "RAS\tCannot register with Gatekeeper without a H323Listener!");
    return FALSE;
  }

  H323SetTransportAddresses(*transport, listeners, rrq.m_callSignalAddress);

  endpoint.SetEndpointTypeInfo(rrq.m_terminalType);
  endpoint.SetVendorIdentifierInfo(rrq.m_endpointVendor);

  if (!IsRegistered()) {  // only send terminal aliases on full registration reset localId
    rrq.IncludeOptionalField(H225_RegistrationRequest::e_terminalAlias);
    H323SetAliasAddresses(endpoint.GetAliasNames(), rrq.m_terminalAlias);
		for (PINDEX i = 0; i < authenticators.GetSize(); i++) { 
			H235Authenticator & authenticator = authenticators[i];
			if (authenticator.UseGkAndEpIdentifiers())
			    authenticator.SetLocalId(localId);
		}
  }

  rrq.m_willSupplyUUIEs = TRUE;
  rrq.IncludeOptionalField(H225_RegistrationRequest::e_usageReportingCapability);
  rrq.m_usageReportingCapability.IncludeOptionalField(H225_RasUsageInfoTypes::e_startTime);
  rrq.m_usageReportingCapability.IncludeOptionalField(H225_RasUsageInfoTypes::e_endTime);
  rrq.m_usageReportingCapability.IncludeOptionalField(H225_RasUsageInfoTypes::e_terminationCause);
  rrq.IncludeOptionalField(H225_RegistrationRequest::e_supportsAltGK);

  if (!gatekeeperIdentifier) {
    rrq.IncludeOptionalField(H225_RegistrationRequest::e_gatekeeperIdentifier);
    rrq.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

  if (!endpointIdentifier.IsEmpty()) {
    rrq.IncludeOptionalField(H225_RegistrationRequest::e_endpointIdentifier);
    rrq.m_endpointIdentifier = endpointIdentifier;
  }

  PTimeInterval ttl = endpoint.GetGatekeeperTimeToLive();
  if (ttl > 0) {
    rrq.IncludeOptionalField(H225_RegistrationRequest::e_timeToLive);
    rrq.m_timeToLive = (int)ttl.GetSeconds();
  }

  if (endpoint.CanDisplayAmountString()) {
    rrq.IncludeOptionalField(H225_RegistrationRequest::e_callCreditCapability);
    rrq.m_callCreditCapability.IncludeOptionalField(H225_CallCreditCapability::e_canDisplayAmountString);
    rrq.m_callCreditCapability.m_canDisplayAmountString = TRUE;
  }

  if (endpoint.CanEnforceDurationLimit()) {
    rrq.IncludeOptionalField(H225_RegistrationRequest::e_callCreditCapability);
    rrq.m_callCreditCapability.IncludeOptionalField(H225_CallCreditCapability::e_canEnforceDurationLimit);
    rrq.m_callCreditCapability.m_canEnforceDurationLimit = TRUE;
  }

  if (assignedGK != NULL) {
	  rrq.IncludeOptionalField(H225_RegistrationRequest::e_assignedGatekeeper);
	  rrq.m_assignedGatekeeper = assignedGK->GetAlternate();
  }

  if (IsRegistered()) {
    rrq.IncludeOptionalField(H225_RegistrationRequest::e_keepAlive);
    rrq.m_keepAlive = TRUE;
  }

  // After doing full register, do lightweight reregisters from now on
  discoveryComplete = FALSE;

  endpoint.OnRegistrationRequest();

  Request request(rrq.m_requestSeqNum, pdu);
  if (MakeRequest(request))
    return TRUE;

  PTRACE(3, "RAS\tFailed registration of " << endpointIdentifier << " with " << gatekeeperIdentifier);
  switch (request.responseResult) {
    case Request::RejectReceived :
      switch (request.rejectReason) {
        case H225_RegistrationRejectReason::e_discoveryRequired :
          // If have been told by GK that we need to discover it again, set flag
          // for next register done by timeToLive handler to do discovery
          requiresDiscovery = TRUE;
          // Do next case

        case H225_RegistrationRejectReason::e_fullRegistrationRequired :
          registrationFailReason = GatekeeperLostRegistration;
          // Set timer to retry registration
          reregisterNow = TRUE;
          monitorTickle.Signal();
          break;

        // Onse below here are permananent errors, so don't try again
        case H225_RegistrationRejectReason::e_invalidCallSignalAddress :
          registrationFailReason = InvalidListener;
          break;

        case H225_RegistrationRejectReason::e_duplicateAlias :
          registrationFailReason = DuplicateAlias;
          break;

        case H225_RegistrationRejectReason::e_securityDenial :
          registrationFailReason = SecurityDenied;
          break;

        default :
          registrationFailReason = (RegistrationFailReasons)(request.rejectReason|RegistrationRejectReasonMask);
          break;
      }
      break;

    case Request::BadCryptoTokens :
      registrationFailReason = SecurityDenied;
      break;

    default :
      registrationFailReason = TransportError;
      break;
  }

  return FALSE;
}


BOOL H323Gatekeeper::OnReceiveRegistrationConfirm(const H225_RegistrationConfirm & rcf)
{
  if (!H225_RAS::OnReceiveRegistrationConfirm(rcf))
    return FALSE;

  registrationFailReason = RegistrationSuccessful;

  endpointIdentifier = rcf.m_endpointIdentifier;
  PTRACE(3, "RAS\tRegistered " << endpointIdentifier << " with " << gatekeeperIdentifier);

  if (rcf.HasOptionalField(H225_RegistrationConfirm::e_assignedGatekeeper))
    SetAssignedGatekeeper(rcf.m_assignedGatekeeper);

  if (rcf.HasOptionalField(H225_RegistrationConfirm::e_alternateGatekeeper))
    SetAlternates(rcf.m_alternateGatekeeper, FALSE);

  if (rcf.HasOptionalField(H225_RegistrationConfirm::e_timeToLive))
    timeToLive = AdjustTimeout(rcf.m_timeToLive);
  else
    timeToLive = 0; // zero disables lightweight RRQ

  // At present only support first call signal address to GK
  if (rcf.m_callSignalAddress.GetSize() > 0)
    gkRouteAddress = rcf.m_callSignalAddress[0];

  willRespondToIRR = rcf.m_willRespondToIRR;

  pregrantMakeCall = pregrantAnswerCall = RequireARQ;
  if (rcf.HasOptionalField(H225_RegistrationConfirm::e_preGrantedARQ)) {
    if (rcf.m_preGrantedARQ.m_makeCall)
      pregrantMakeCall = rcf.m_preGrantedARQ.m_useGKCallSignalAddressToMakeCall
                                                      ? PreGkRoutedARQ : PregrantARQ;
    if (rcf.m_preGrantedARQ.m_answerCall)
      pregrantAnswerCall = rcf.m_preGrantedARQ.m_useGKCallSignalAddressToAnswer
                                                      ? PreGkRoutedARQ : PregrantARQ;
    if (rcf.m_preGrantedARQ.HasOptionalField(H225_RegistrationConfirm_preGrantedARQ::e_irrFrequencyInCall))
      SetInfoRequestRate(AdjustTimeout(rcf.m_preGrantedARQ.m_irrFrequencyInCall));
    else
      ClearInfoRequestRate();
  }
  else
    ClearInfoRequestRate();

  // Remove the endpoint aliases that the gatekeeper did not like and add the
  // ones that it really wants us to be.
  if (rcf.HasOptionalField(H225_RegistrationConfirm::e_terminalAlias) &&
	                      !endpoint.OnGatekeeperAliases(rcf.m_terminalAlias)) {
    const PStringList & currentAliases = endpoint.GetAliasNames();
    PStringList aliasesToChange;
    PINDEX i, j;

    for (i = 0; i < rcf.m_terminalAlias.GetSize(); i++) {
      PString alias = H323GetAliasAddressString(rcf.m_terminalAlias[i]);
      if (!alias) {
        for (j = 0; j < currentAliases.GetSize(); j++) {
          if (alias *= currentAliases[j])
            break;
        }
        if (j >= currentAliases.GetSize())
          aliasesToChange.AppendString(alias);
      }
    }
    for (i = 0; i < aliasesToChange.GetSize(); i++) {
      PTRACE(2, "RAS\tGatekeeper add of alias \"" << aliasesToChange[i] << '"');
      endpoint.AddAliasName(aliasesToChange[i]);
    }

    aliasesToChange.RemoveAll();

    for (i = 0; i < currentAliases.GetSize(); i++) {
      for (j = 0; j < rcf.m_terminalAlias.GetSize(); j++) {
        if (currentAliases[i] *= H323GetAliasAddressString(rcf.m_terminalAlias[j]))
          break;
      }
      if (j >= rcf.m_terminalAlias.GetSize())
        aliasesToChange.AppendString(currentAliases[i]);
    }
    for (i = 0; i < aliasesToChange.GetSize(); i++) {
      PTRACE(2, "RAS\tGatekeeper removal of alias \"" << aliasesToChange[i] << '"');
      endpoint.RemoveAliasName(aliasesToChange[i]);
    }
  }

#ifdef H323_H248
  if (rcf.HasOptionalField(H225_RegistrationConfirm::e_serviceControl))
    OnServiceControlSessions(rcf.m_serviceControl, NULL);
#endif

// NAT Detection with GNUGK
#ifdef H323_GNUGK
  if (rcf.HasOptionalField(H225_RegistrationConfirm::e_nonStandardData))
  {
    PString NATaddr = rcf.m_nonStandardData.m_data.AsString();
    if (!NATaddr.IsEmpty())
    {
      if (NATaddr.Left(4) == "NAT=")
        endpoint.OnGatekeeperNATDetect(NATaddr.Right(NATaddr.GetLength()-4),endpointIdentifier,gkRouteAddress);
      else
        endpoint.OnGatekeeperOpenNATDetect(endpointIdentifier,gkRouteAddress);
    }
  }
#endif

  endpoint.OnRegistrationConfirm(gkRouteAddress);

  return TRUE;
}


BOOL H323Gatekeeper::OnReceiveRegistrationReject(const H225_RegistrationReject & rrj)
{
  if (!H225_RAS::OnReceiveRegistrationReject(rrj))
    return FALSE;

  if (rrj.HasOptionalField(H225_RegistrationReject::e_assignedGatekeeper))
     SetAssignedGatekeeper(rrj.m_assignedGatekeeper);
  else if (rrj.HasOptionalField(H225_RegistrationReject::e_altGKInfo))
    SetAlternates(rrj.m_altGKInfo.m_alternateGatekeeper,
                  rrj.m_altGKInfo.m_altGKisPermanent);
  else 
      endpoint.OnRegistrationReject();

  return TRUE;
}

void H323Gatekeeper::ReRegisterNow()
{
  PTRACE(3, "RAS\tforcing reregistration");
  RegistrationTimeToLive();

}

void H323Gatekeeper::RegistrationTimeToLive()
{
  PTRACE(3, "RAS\tTime To Live reregistration");

  if (requiresDiscovery) {
    PTRACE(2, "RAS\tRepeating discovery on gatekeepers request.");

    H323RasPDU pdu;
    Request request(SetupGatekeeperRequest(pdu), pdu);
    if (!MakeRequest(request) || !discoveryComplete) {
      PTRACE(2, "RAS\tRediscovery failed, retrying in 1 minute.");
      timeToLive = PTimeInterval(0, 0, 1);
      return;
    }

    requiresDiscovery = FALSE;
  }

  if (!RegistrationRequest(autoReregister)) {
    PTRACE_IF(2, !reregisterNow, "RAS\tTime To Live reregistration failed, retrying in 1 minute");
    timeToLive = PTimeInterval(0, 0, 1);
  }
}


BOOL H323Gatekeeper::UnregistrationRequest(int reason)
{
  if (PAssertNULL(transport) == NULL)
    return FALSE;

  PINDEX i;
  H323RasPDU pdu;
  H225_UnregistrationRequest & urq = pdu.BuildUnregistrationRequest(GetNextSequenceNumber());

  H225_TransportAddress rasAddress;
  transport->SetUpTransportPDU(rasAddress, TRUE);

  H323SetTransportAddresses(*transport,
                            endpoint.GetInterfaceAddresses(TRUE, transport),
                            urq.m_callSignalAddress);

  urq.IncludeOptionalField(H225_UnregistrationRequest::e_endpointAlias);
  H323SetAliasAddresses(endpoint.GetAliasNames(), urq.m_endpointAlias);

  if (!gatekeeperIdentifier) {
    urq.IncludeOptionalField(H225_UnregistrationRequest::e_gatekeeperIdentifier);
    urq.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

  if (!endpointIdentifier.IsEmpty()) {
    urq.IncludeOptionalField(H225_UnregistrationRequest::e_endpointIdentifier);
    urq.m_endpointIdentifier = endpointIdentifier;
  }

  if (reason >= 0) {
    urq.IncludeOptionalField(H225_UnregistrationRequest::e_reason);
    urq.m_reason = reason;
  }

  Request request(urq.m_requestSeqNum, pdu);

  BOOL requestResult = MakeRequest(request);

  for (i = 0; i < alternates.GetSize(); i++) {
    AlternateInfo & altgk = alternates[i];
    if (altgk.registrationState == AlternateInfo::IsRegistered) {
      Connect(altgk.rasAddress,altgk.gatekeeperIdentifier);
      UnregistrationRequest(reason);
    }
  }

  if (requestResult)
    return TRUE;

  switch (request.responseResult) {
    case Request::NoResponseReceived :
      registrationFailReason = TransportError;
      timeToLive = 0; // zero disables lightweight RRQ
      break;

    case Request::BadCryptoTokens :
      registrationFailReason = SecurityDenied;
      timeToLive = 0; // zero disables lightweight RRQ
      break;

    default :
      break;
  }

  return !IsRegistered();
}


BOOL H323Gatekeeper::OnReceiveUnregistrationConfirm(const H225_UnregistrationConfirm & ucf)
{
  if (!H225_RAS::OnReceiveUnregistrationConfirm(ucf))
    return FALSE;

  registrationFailReason = UnregisteredLocally;
  timeToLive = 0; // zero disables lightweight RRQ

  return TRUE;
}


BOOL H323Gatekeeper::OnReceiveUnregistrationRequest(const H225_UnregistrationRequest & urq)
{
  if (!H225_RAS::OnReceiveUnregistrationRequest(urq))
    return FALSE;

  PTRACE(2, "RAS\tUnregistration received");
  if (!urq.HasOptionalField(H225_UnregistrationRequest::e_gatekeeperIdentifier) ||
       urq.m_gatekeeperIdentifier.GetValue() != gatekeeperIdentifier) {
    PTRACE(1, "RAS\tInconsistent gatekeeperIdentifier!");
    return FALSE;
  }

  if (!urq.HasOptionalField(H225_UnregistrationRequest::e_endpointIdentifier) ||
       urq.m_endpointIdentifier.GetValue() != endpointIdentifier) {
    PTRACE(1, "RAS\tInconsistent endpointIdentifier!");
    return FALSE;
  }

  endpoint.ClearAllCalls(H323Connection::EndedByGatekeeper, FALSE);

  PTRACE(3, "RAS\tUnregistered, calls cleared");
  registrationFailReason = UnregisteredByGatekeeper;
  timeToLive = 0; // zero disables lightweight RRQ

  if (urq.HasOptionalField(H225_UnregistrationRequest::e_alternateGatekeeper))
    SetAlternates(urq.m_alternateGatekeeper, FALSE);

  H323RasPDU response(authenticators);
  response.BuildUnregistrationConfirm(urq.m_requestSeqNum);
  BOOL ok = WritePDU(response);

  if (autoReregister) {
    PTRACE(3, "RAS\tReregistering by setting timeToLive");
    reregisterNow = TRUE;
    monitorTickle.Signal();
  }

  endpoint.OnUnRegisterRequest();

  return ok;
}


BOOL H323Gatekeeper::OnReceiveUnregistrationReject(const H225_UnregistrationReject & urj)
{
  if (!H225_RAS::OnReceiveUnregistrationReject(urj))
    return FALSE;

  if (lastRequest->rejectReason != H225_UnregRejectReason::e_callInProgress) {
    registrationFailReason = UnregisteredLocally;
    timeToLive = 0; // zero disables lightweight RRQ
  }

  return TRUE;
}


BOOL H323Gatekeeper::LocationRequest(const PString & alias,
                                     H323TransportAddress & address)
{
  PStringList aliases;
  aliases.AppendString(alias);
  return LocationRequest(aliases, address);
}


BOOL H323Gatekeeper::LocationRequest(const PStringList & aliases,
                                     H323TransportAddress & address)
{
  if (PAssertNULL(transport) == NULL)
    return FALSE;

  H323RasPDU pdu;
  H225_LocationRequest & lrq = pdu.BuildLocationRequest(GetNextSequenceNumber());

  H323SetAliasAddresses(aliases, lrq.m_destinationInfo);

  if (!endpointIdentifier.IsEmpty()) {
    lrq.IncludeOptionalField(H225_LocationRequest::e_endpointIdentifier);
    lrq.m_endpointIdentifier = endpointIdentifier;
  }

  transport->SetUpTransportPDU(lrq.m_replyAddress, TRUE);

  lrq.IncludeOptionalField(H225_LocationRequest::e_sourceInfo);
  H323SetAliasAddresses(endpoint.GetAliasNames(), lrq.m_sourceInfo);

  if (!gatekeeperIdentifier) {
    lrq.IncludeOptionalField(H225_LocationRequest::e_gatekeeperIdentifier);
    lrq.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

  Request request(lrq.m_requestSeqNum, pdu);
  request.responseInfo = &address;
  if (!MakeRequest(request))
    return FALSE;

  // sanity check the address - some Gks return address 0.0.0.0 and port 0
  PIPSocket::Address ipAddr;
  WORD port;
  return address.GetIpAndPort(ipAddr, port) && (port != 0);
}


H323Gatekeeper::AdmissionResponse::AdmissionResponse()
{
  rejectReason = UINT_MAX;

  gatekeeperRouted = FALSE;
  endpointCount = 1;
  transportAddress = NULL;
  accessTokenData = NULL;

  aliasAddresses = NULL;
  destExtraCallInfo = NULL;
}


struct AdmissionRequestResponseInfo {
  AdmissionRequestResponseInfo(
    H323Gatekeeper::AdmissionResponse & r,
    H323Connection & c
  ) : param(r), connection(c) { }

  H323Gatekeeper::AdmissionResponse & param;
  H323Connection & connection;
  unsigned allocatedBandwidth;
  unsigned uuiesRequested;
  PString      accessTokenOID1;
  PString      accessTokenOID2;
};


BOOL H323Gatekeeper::AdmissionRequest(H323Connection & connection,
                                      AdmissionResponse & response,
                                      BOOL ignorePreGrantedARQ)
{
  BOOL answeringCall = connection.HadAnsweredCall();

  if (!ignorePreGrantedARQ) {
    switch (answeringCall ? pregrantAnswerCall : pregrantMakeCall) {
      case RequireARQ :
        break;
      case PregrantARQ :
        return TRUE;
      case PreGkRoutedARQ :
        if (gkRouteAddress.IsEmpty()) {
          response.rejectReason = UINT_MAX;
          return FALSE;
        }
        if (response.transportAddress != NULL)
          *response.transportAddress = gkRouteAddress;
        response.gatekeeperRouted = TRUE;
        return TRUE;
    }
  }

  H323RasPDU pdu;
  H225_AdmissionRequest & arq = pdu.BuildAdmissionRequest(GetNextSequenceNumber());

  arq.m_callType.SetTag(H225_CallType::e_pointToPoint);
  arq.m_endpointIdentifier = endpointIdentifier;
  arq.m_answerCall = answeringCall;
  arq.m_canMapAlias = TRUE; // Stack supports receiving a different number in the ACF 
                            // to the one sent in the ARQ
  arq.m_willSupplyUUIEs = TRUE;

  if (!gatekeeperIdentifier) {
    arq.IncludeOptionalField(H225_AdmissionRequest::e_gatekeeperIdentifier);
    arq.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

  PString destInfo = connection.GetRemotePartyName();
  arq.m_srcInfo.SetSize(1);
  if (answeringCall) {
    H323SetAliasAddress(destInfo, arq.m_srcInfo[0]);
    if (!connection.GetLocalPartyName()) {
      arq.IncludeOptionalField(H225_AdmissionRequest::e_destinationInfo);
      H323SetAliasAddresses(connection.GetLocalAliasNames(), arq.m_destinationInfo);
    }
  }
  else {
    H323SetAliasAddresses(connection.GetLocalAliasNames(), arq.m_srcInfo);
    if (response.transportAddress == NULL || destInfo != *response.transportAddress) {
      arq.IncludeOptionalField(H225_AdmissionRequest::e_destinationInfo);
      arq.m_destinationInfo.SetSize(1);
      H323SetAliasAddress(destInfo, arq.m_destinationInfo[0]);
    }
  }

  const H323Transport * signallingChannel = connection.GetSignallingChannel();
  if (answeringCall) {
    arq.IncludeOptionalField(H225_AdmissionRequest::e_srcCallSignalAddress);
    signallingChannel->SetUpTransportPDU(arq.m_srcCallSignalAddress, FALSE);
    arq.IncludeOptionalField(H225_AdmissionRequest::e_destCallSignalAddress);
    signallingChannel->SetUpTransportPDU(arq.m_destCallSignalAddress, TRUE);
  }
  else {
    if (signallingChannel != NULL && signallingChannel->IsOpen()) {
      arq.IncludeOptionalField(H225_AdmissionRequest::e_srcCallSignalAddress);
      signallingChannel->SetUpTransportPDU(arq.m_srcCallSignalAddress, TRUE);
    }
    if (response.transportAddress != NULL && !response.transportAddress->IsEmpty()) {
      arq.IncludeOptionalField(H225_AdmissionRequest::e_destCallSignalAddress);
      response.transportAddress->SetPDU(arq.m_destCallSignalAddress);
    }
  }

  arq.m_bandWidth = connection.GetBandwidthAvailable();
  arq.m_callReferenceValue = connection.GetCallReference();
  arq.m_conferenceID = connection.GetConferenceIdentifier();
  arq.m_callIdentifier.m_guid = connection.GetCallIdentifier();

#ifdef H323_H450
  connection.SetCallLinkage(pdu);
#endif

  AdmissionRequestResponseInfo info(response, connection);
  info.accessTokenOID1 = connection.GetGkAccessTokenOID();
  PINDEX comma = info.accessTokenOID1.Find(',');
  if (comma == P_MAX_INDEX)
    info.accessTokenOID2 = info.accessTokenOID1;
  else {
    info.accessTokenOID2 = info.accessTokenOID1.Mid(comma+1);
    info.accessTokenOID1.Delete(comma, P_MAX_INDEX);
  }

  connection.OnSendARQ(arq);

  Request request(arq.m_requestSeqNum, pdu);
  request.responseInfo = &info;

  if (!authenticators.IsEmpty()) {
    pdu.Prepare(arq.m_tokens, H225_AdmissionRequest::e_tokens,
                arq.m_cryptoTokens, H225_AdmissionRequest::e_cryptoTokens);

    H235Authenticators adjustedAuthenticators;
    if (connection.GetAdmissionRequestAuthentication(arq, adjustedAuthenticators)) {
      PTRACE(3, "RAS\tAuthenticators credentials replaced with \""
             << setfill(',') << adjustedAuthenticators << setfill(' ') << "\" during ARQ");

      for (PINDEX i = 0; i < adjustedAuthenticators.GetSize(); i++) {
        H235Authenticator & authenticator = adjustedAuthenticators[i];
        if (authenticator.UseGkAndEpIdentifiers())
          authenticator.SetRemoteId(gatekeeperIdentifier);
      }

      adjustedAuthenticators.PreparePDU(pdu,
                                        arq.m_tokens, H225_AdmissionRequest::e_tokens,
                                        arq.m_cryptoTokens, H225_AdmissionRequest::e_cryptoTokens);
      pdu.SetAuthenticators(adjustedAuthenticators);
    }
  }

  if (!MakeRequest(request)) {
    response.rejectReason = request.rejectReason;

    // See if we are registered.
    if (request.responseResult == Request::RejectReceived &&
        response.rejectReason != H225_AdmissionRejectReason::e_callerNotRegistered &&
        response.rejectReason != H225_AdmissionRejectReason::e_invalidEndpointIdentifier)
      return FALSE;

    PTRACE(2, "RAS\tEndpoint has become unregistered during ARQ from gatekeeper " << gatekeeperIdentifier);

    // Have been told we are not registered (or gk offline)
    switch (request.responseResult) {
      case Request::NoResponseReceived :
        registrationFailReason = TransportError;
        response.rejectReason = UINT_MAX;
        break;

      case Request::BadCryptoTokens :
        registrationFailReason = SecurityDenied;
        response.rejectReason = H225_AdmissionRejectReason::e_securityDenial;
        break;

      default :
        registrationFailReason = GatekeeperLostRegistration;
    }

    // If we are not registered and auto register is set ...
    if (!autoReregister)
      return FALSE;

    // Then immediately reregister.
    if (!RegistrationRequest(autoReregister))
      return FALSE;

    // Reset the gk info in ARQ
    arq.m_endpointIdentifier = endpointIdentifier;
    if (!gatekeeperIdentifier) {
      arq.IncludeOptionalField(H225_AdmissionRequest::e_gatekeeperIdentifier);
      arq.m_gatekeeperIdentifier = gatekeeperIdentifier;
    }
    else
      arq.RemoveOptionalField(H225_AdmissionRequest::e_gatekeeperIdentifier);

    // Is new request so need new sequence number as well.
    arq.m_requestSeqNum = GetNextSequenceNumber();
    request.sequenceNumber = arq.m_requestSeqNum;

    if (!MakeRequest(request)) {
      response.rejectReason = request.responseResult == Request::RejectReceived
                                                ? request.rejectReason : UINT_MAX;
	 
      return FALSE;
    }
  }

  connection.SetBandwidthAvailable(info.allocatedBandwidth);
  connection.SetUUIEsRequested(info.uuiesRequested);

  return TRUE;
}


void H323Gatekeeper::OnSendAdmissionRequest(H225_AdmissionRequest & /*arq*/)
{
  // Override default function as it sets crypto tokens and this is really
  // done by the AdmissionRequest() function.
}


static unsigned GetUUIEsRequested(const H225_UUIEsRequested & pdu)
{
  unsigned uuiesRequested = 0;

  if ((BOOL)pdu.m_setup)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_setup);
  if ((BOOL)pdu.m_callProceeding)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_callProceeding);
  if ((BOOL)pdu.m_connect)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_connect);
  if ((BOOL)pdu.m_alerting)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_alerting);
  if ((BOOL)pdu.m_information)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_information);
  if ((BOOL)pdu.m_releaseComplete)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_releaseComplete);
  if ((BOOL)pdu.m_facility)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_facility);
  if ((BOOL)pdu.m_progress)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_progress);
  if ((BOOL)pdu.m_empty)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_empty);

  if (pdu.HasOptionalField(H225_UUIEsRequested::e_status) && (BOOL)pdu.m_status)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_status);
  if (pdu.HasOptionalField(H225_UUIEsRequested::e_statusInquiry) && (BOOL)pdu.m_statusInquiry)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_statusInquiry);
  if (pdu.HasOptionalField(H225_UUIEsRequested::e_setupAcknowledge) && (BOOL)pdu.m_setupAcknowledge)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_setupAcknowledge);
  if (pdu.HasOptionalField(H225_UUIEsRequested::e_notify) && (BOOL)pdu.m_notify)
    uuiesRequested |= (1<<H225_H323_UU_PDU_h323_message_body::e_notify);

  return uuiesRequested;
}


static void ExtractToken(const AdmissionRequestResponseInfo & info,
                         const H225_ArrayOf_ClearToken & tokens,
                         PBYTEArray & accessTokenData)
{
  if (!info.accessTokenOID1 && tokens.GetSize() > 0) {
    PTRACE(4, "Looking for OID " << info.accessTokenOID1 << " in ACF to copy.");
    for (PINDEX i = 0; i < tokens.GetSize(); i++) {
      if (tokens[i].m_tokenOID == info.accessTokenOID1) {
        PTRACE(4, "Looking for OID " << info.accessTokenOID2 << " in token to copy.");
        if (tokens[i].HasOptionalField(H235_ClearToken::e_nonStandard) &&
            tokens[i].m_nonStandard.m_nonStandardIdentifier == info.accessTokenOID2) {
          PTRACE(4, "Copying ACF nonStandard OctetString.");
          accessTokenData = tokens[i].m_nonStandard.m_data;
          break;
        }
      }
    }
  }
}


BOOL H323Gatekeeper::OnReceiveAdmissionConfirm(const H225_AdmissionConfirm & acf)
{
  if (!H225_RAS::OnReceiveAdmissionConfirm(acf))
    return FALSE;

  AdmissionRequestResponseInfo & info = *(AdmissionRequestResponseInfo *)lastRequest->responseInfo;
  info.allocatedBandwidth = acf.m_bandWidth;
  if (info.param.transportAddress != NULL)
    *info.param.transportAddress = acf.m_destCallSignalAddress;

  info.param.gatekeeperRouted = acf.m_callModel.GetTag() == H225_CallModel::e_gatekeeperRouted;

  // Remove the endpoint aliases that the gatekeeper did not like and add the
  // ones that it really wants us to be.
  if (info.param.aliasAddresses != NULL &&
      acf.HasOptionalField(H225_AdmissionConfirm::e_destinationInfo)) {
    PTRACE(3, "RAS\tGatekeeper specified " << acf.m_destinationInfo.GetSize() << " aliases in ACF");
    *info.param.aliasAddresses = acf.m_destinationInfo;
  }

  if (acf.HasOptionalField(H225_AdmissionConfirm::e_uuiesRequested))
    info.uuiesRequested = GetUUIEsRequested(acf.m_uuiesRequested);

  if (info.param.destExtraCallInfo != NULL &&
      acf.HasOptionalField(H225_AdmissionConfirm::e_destExtraCallInfo))
    *info.param.destExtraCallInfo = acf.m_destExtraCallInfo;

  if (info.param.accessTokenData != NULL && acf.HasOptionalField(H225_AdmissionConfirm::e_tokens))
    ExtractToken(info, acf.m_tokens, *info.param.accessTokenData);

  if (info.param.transportAddress != NULL) {
    PINDEX count = 1;
    for (PINDEX i = 0; i < acf.m_alternateEndpoints.GetSize() && count < info.param.endpointCount; i++) {
      if (acf.m_alternateEndpoints[i].HasOptionalField(H225_Endpoint::e_callSignalAddress) &&
          acf.m_alternateEndpoints[i].m_callSignalAddress.GetSize() > 0) {
        info.param.transportAddress[count] = acf.m_alternateEndpoints[i].m_callSignalAddress[0];
        if (info.param.accessTokenData != NULL)
          ExtractToken(info, acf.m_alternateEndpoints[i].m_tokens, info.param.accessTokenData[count]);
        count++;
      }
    }
    info.param.endpointCount = count;
  }

  if (acf.HasOptionalField(H225_AdmissionConfirm::e_irrFrequency))
    SetInfoRequestRate(AdjustTimeout(acf.m_irrFrequency));
  willRespondToIRR = acf.m_willRespondToIRR;

  info.connection.OnReceivedACF(acf);

#ifdef H323_H248
  if (acf.HasOptionalField(H225_AdmissionConfirm::e_serviceControl))
    OnServiceControlSessions(acf.m_serviceControl, &info.connection);
#endif

  return TRUE;
}


BOOL H323Gatekeeper::OnReceiveAdmissionReject(const H225_AdmissionReject & arj)
{
  if (!H225_RAS::OnReceiveAdmissionReject(arj))
    return FALSE;

  AdmissionRequestResponseInfo & info = *(AdmissionRequestResponseInfo *)lastRequest->responseInfo;
  info.connection.OnReceivedARJ(arj);

#ifdef H323_H248
  if (arj.HasOptionalField(H225_AdmissionConfirm::e_serviceControl))
    OnServiceControlSessions(arj.m_serviceControl,&info.connection);
#endif

  return TRUE;
}


static void SetRasUsageInformation(const H323Connection & connection, 
                                   H225_RasUsageInformation & usage)
{
  unsigned time = connection.GetAlertingTime().GetTimeInSeconds();
  if (time != 0) {
    usage.IncludeOptionalField(H225_RasUsageInformation::e_alertingTime);
    usage.m_alertingTime = time;
  }

  time = connection.GetConnectionStartTime().GetTimeInSeconds();
  if (time != 0) {
    usage.IncludeOptionalField(H225_RasUsageInformation::e_connectTime);
    usage.m_connectTime = time;
  }

  time = connection.GetConnectionEndTime().GetTimeInSeconds();
  if (time != 0) {
    usage.IncludeOptionalField(H225_RasUsageInformation::e_endTime);
    usage.m_endTime = time;
  }
}


BOOL H323Gatekeeper::DisengageRequest(const H323Connection & connection, unsigned reason)
{
  H323RasPDU pdu;
  H225_DisengageRequest & drq = pdu.BuildDisengageRequest(GetNextSequenceNumber());

  drq.m_endpointIdentifier = endpointIdentifier;
  drq.m_conferenceID = connection.GetConferenceIdentifier();
  drq.m_callReferenceValue = connection.GetCallReference();
  drq.m_callIdentifier.m_guid = connection.GetCallIdentifier();
  drq.m_disengageReason.SetTag(reason);
  drq.m_answeredCall = connection.HadAnsweredCall();

  drq.IncludeOptionalField(H225_DisengageRequest::e_usageInformation);
  SetRasUsageInformation(connection, drq.m_usageInformation);

  drq.IncludeOptionalField(H225_DisengageRequest::e_terminationCause);
  drq.m_terminationCause.SetTag(H225_CallTerminationCause::e_releaseCompleteReason);
  Q931::CauseValues cause = H323TranslateFromCallEndReason(connection, drq.m_terminationCause);
  if (cause != Q931::ErrorInCauseIE) {
    drq.m_terminationCause.SetTag(H225_CallTerminationCause::e_releaseCompleteCauseIE);
    PASN_OctetString & rcReason = drq.m_terminationCause;
    rcReason.SetSize(2);
    rcReason[0] = 0x80;
    rcReason[1] = (BYTE)(0x80|cause);
  }

  if (!gatekeeperIdentifier) {
    drq.IncludeOptionalField(H225_DisengageRequest::e_gatekeeperIdentifier);
    drq.m_gatekeeperIdentifier = gatekeeperIdentifier;
  }

  connection.OnSendDRQ(drq);

  Request request(drq.m_requestSeqNum, pdu);
  return MakeRequestWithReregister(request, H225_DisengageRejectReason::e_notRegistered);
}


BOOL H323Gatekeeper::OnReceiveDisengageRequest(const H225_DisengageRequest & drq)
{
  if (!H225_RAS::OnReceiveDisengageRequest(drq))
    return FALSE;

  OpalGloballyUniqueID id = NULL;
  if (drq.HasOptionalField(H225_DisengageRequest::e_callIdentifier))
    id = drq.m_callIdentifier.m_guid;
  if (id == NULL)
    id = drq.m_conferenceID;

  H323RasPDU response(authenticators);
  PTRACE(2, "gkclient.cxx\tFCWL from OnReceiveDisengageRequest: " << id.AsString());
  H323Connection * connection = endpoint.FindConnectionWithLock(id.AsString());
  if (connection == NULL)
    response.BuildDisengageReject(drq.m_requestSeqNum,
                                  H225_DisengageRejectReason::e_requestToDropOther);
  else {
    H225_DisengageConfirm & dcf = response.BuildDisengageConfirm(drq.m_requestSeqNum);

    dcf.IncludeOptionalField(H225_DisengageConfirm::e_usageInformation);
    SetRasUsageInformation(*connection, dcf.m_usageInformation);

    connection->ClearCall(H323Connection::EndedByGatekeeper);
    connection->Unlock();
  }

#ifdef H323_H248
  if (drq.HasOptionalField(H225_DisengageRequest::e_serviceControl))
    OnServiceControlSessions(drq.m_serviceControl, connection);
#endif

  return WritePDU(response);
}


BOOL H323Gatekeeper::BandwidthRequest(H323Connection & connection,
                                      unsigned requestedBandwidth)
{
  H323RasPDU pdu;
  H225_BandwidthRequest & brq = pdu.BuildBandwidthRequest(GetNextSequenceNumber());

  brq.m_endpointIdentifier = endpointIdentifier;
  brq.m_conferenceID = connection.GetConferenceIdentifier();
  brq.m_callReferenceValue = connection.GetCallReference();
  brq.m_callIdentifier.m_guid = connection.GetCallIdentifier();
  brq.m_bandWidth = requestedBandwidth;
  brq.IncludeOptionalField(H225_BandwidthRequest::e_usageInformation);
  SetRasUsageInformation(connection, brq.m_usageInformation);

  Request request(brq.m_requestSeqNum, pdu);
  
  unsigned allocatedBandwidth;
  request.responseInfo = &allocatedBandwidth;

  if (!MakeRequestWithReregister(request, H225_BandRejectReason::e_notBound))
    return FALSE;

  connection.SetBandwidthAvailable(allocatedBandwidth);
  return TRUE;
}


BOOL H323Gatekeeper::OnReceiveBandwidthConfirm(const H225_BandwidthConfirm & bcf)
{
  if (!H225_RAS::OnReceiveBandwidthConfirm(bcf))
    return FALSE;

  if (lastRequest->responseInfo != NULL)
    *(unsigned *)lastRequest->responseInfo = bcf.m_bandWidth;

  return TRUE;
}


BOOL H323Gatekeeper::OnReceiveBandwidthRequest(const H225_BandwidthRequest & brq)
{
  if (!H225_RAS::OnReceiveBandwidthRequest(brq))
    return FALSE;

  OpalGloballyUniqueID id = brq.m_callIdentifier.m_guid;
  H323Connection * connection = endpoint.FindConnectionWithLock(id.AsString());

  H323RasPDU response(authenticators);
  if (connection == NULL)
    response.BuildBandwidthReject(brq.m_requestSeqNum,
                                  H225_BandRejectReason::e_invalidConferenceID);
  else {
    if (connection->SetBandwidthAvailable(brq.m_bandWidth))
      response.BuildBandwidthConfirm(brq.m_requestSeqNum, brq.m_bandWidth);
    else
      response.BuildBandwidthReject(brq.m_requestSeqNum,
                                    H225_BandRejectReason::e_insufficientResources);
    connection->Unlock();
  }

  return WritePDU(response);
}


void H323Gatekeeper::SetInfoRequestRate(const PTimeInterval & rate)
{
  if (rate < infoRequestRate.GetResetTime() || infoRequestRate.GetResetTime() == 0) {
    // Have to be sneaky here becuase we do not want to actually change the
    // amount of time to run on the timer.
    PTimeInterval timeToGo = infoRequestRate;
    infoRequestRate = rate;
    if (rate > timeToGo)
      infoRequestRate.PTimeInterval::operator=(timeToGo);
  }
}


void H323Gatekeeper::ClearInfoRequestRate()
{
  // Only reset rate to zero (disabled) if no calls present
  if (endpoint.GetAllConnections().IsEmpty())
    infoRequestRate = 0;
}


H225_InfoRequestResponse & H323Gatekeeper::BuildInfoRequestResponse(H323RasPDU & response,
                                                                    unsigned seqNum)
{
  H225_InfoRequestResponse & irr = response.BuildInfoRequestResponse(seqNum);

  endpoint.SetEndpointTypeInfo(irr.m_endpointType);
  irr.m_endpointIdentifier = endpointIdentifier;
  transport->SetUpTransportPDU(irr.m_rasAddress, TRUE);
  H323SetTransportAddresses(*transport,
                            endpoint.GetInterfaceAddresses(TRUE, transport),
                            irr.m_callSignalAddress);

  irr.IncludeOptionalField(H225_InfoRequestResponse::e_endpointAlias);
  H323SetAliasAddresses(endpoint.GetAliasNames(), irr.m_endpointAlias);

  return irr;
}


BOOL H323Gatekeeper::SendUnsolicitedIRR(H225_InfoRequestResponse & irr,
                                        H323RasPDU & response)
{
  irr.m_unsolicited = TRUE;

  if (willRespondToIRR) {
    PTRACE(4, "RAS\tSending unsolicited IRR and awaiting acknowledgement");
    Request request(irr.m_requestSeqNum, response);
    return MakeRequest(request);
  }

  PTRACE(4, "RAS\tSending unsolicited IRR and without acknowledgement");
  response.SetAuthenticators(authenticators);
  return WritePDU(response);
}


static void AddInfoRequestResponseCall(H225_InfoRequestResponse & irr,
                                       const H323Connection & connection)
{
  irr.IncludeOptionalField(H225_InfoRequestResponse::e_perCallInfo);

  PINDEX sz = irr.m_perCallInfo.GetSize();
  if (!irr.m_perCallInfo.SetSize(sz+1))
    return;

  H225_InfoRequestResponse_perCallInfo_subtype & info = irr.m_perCallInfo[sz];

  info.m_callReferenceValue = connection.GetCallReference();
  info.m_callIdentifier.m_guid = connection.GetCallIdentifier();
  info.m_conferenceID = connection.GetConferenceIdentifier();
  info.IncludeOptionalField(H225_InfoRequestResponse_perCallInfo_subtype::e_originator);
  info.m_originator = !connection.HadAnsweredCall();

  H323_RTP_Session * session = connection.GetSessionCallbacks(RTP_Session::DefaultAudioSessionID);
  if (session != NULL) {
    info.IncludeOptionalField(H225_InfoRequestResponse_perCallInfo_subtype::e_audio);
    info.m_audio.SetSize(1);
    session->OnSendRasInfo(info.m_audio[0]);
  }

  session = connection.GetSessionCallbacks(RTP_Session::DefaultVideoSessionID);
  if (session != NULL) {
    info.IncludeOptionalField(H225_InfoRequestResponse_perCallInfo_subtype::e_video);
    info.m_video.SetSize(1);
    session->OnSendRasInfo(info.m_video[0]);
  }

  const H323Transport & controlChannel = connection.GetControlChannel();
  controlChannel.SetUpTransportPDU(info.m_h245.m_recvAddress, TRUE);
  controlChannel.SetUpTransportPDU(info.m_h245.m_sendAddress, FALSE);

  info.m_callType.SetTag(H225_CallType::e_pointToPoint);
  info.m_bandWidth = connection.GetBandwidthUsed();
  info.m_callModel.SetTag(connection.IsGatekeeperRouted() ? H225_CallModel::e_gatekeeperRouted
                                                          : H225_CallModel::e_direct);

  info.IncludeOptionalField(H225_InfoRequestResponse_perCallInfo_subtype::e_usageInformation);
  SetRasUsageInformation(connection, info.m_usageInformation);
}


static BOOL AddAllInfoRequestResponseCall(H225_InfoRequestResponse & irr,
                                          H323EndPoint & endpoint,
                                          const PStringList & tokens)
{
  BOOL addedOne = FALSE;

  for (PINDEX i = 0; i < tokens.GetSize(); i++) {
    H323Connection * connection = endpoint.FindConnectionWithLock(tokens[i]);
    if (connection != NULL) {
      AddInfoRequestResponseCall(irr, *connection);
	  connection->OnSendIRR(irr);
      connection->Unlock();
      addedOne = TRUE;
    }
  }

  return addedOne;
}


void H323Gatekeeper::InfoRequestResponse()
{
  PStringList tokens = endpoint.GetAllConnections();
  if (tokens.IsEmpty())
    return;

  H323RasPDU response;
  H225_InfoRequestResponse & irr = BuildInfoRequestResponse(response, GetNextSequenceNumber());

  if (AddAllInfoRequestResponseCall(irr, endpoint, tokens))
    SendUnsolicitedIRR(irr, response);
}


void H323Gatekeeper::InfoRequestResponse(const H323Connection & connection)
{
  H323RasPDU response;
  H225_InfoRequestResponse & irr = BuildInfoRequestResponse(response, GetNextSequenceNumber());

  AddInfoRequestResponseCall(irr, connection);

  connection.OnSendIRR(irr);

  SendUnsolicitedIRR(irr, response);
}


void H323Gatekeeper::InfoRequestResponse(const H323Connection & connection,
                                         const H225_H323_UU_PDU & pdu,
                                         BOOL sent)
{
  // Are unknown Q.931 PDU
  if (pdu.m_h323_message_body.GetTag() == P_MAX_INDEX)
    return;

  // Check mask of things to report on
  if ((connection.GetUUIEsRequested() & (1<<pdu.m_h323_message_body.GetTag())) == 0)
    return;

  PTRACE(3, "RAS\tSending unsolicited IRR for requested UUIE");

  // Report the PDU
  H323RasPDU response;
  H225_InfoRequestResponse & irr = BuildInfoRequestResponse(response, GetNextSequenceNumber());

  AddInfoRequestResponseCall(irr, connection);

  irr.m_perCallInfo[0].IncludeOptionalField(H225_InfoRequestResponse_perCallInfo_subtype::e_pdu);
  irr.m_perCallInfo[0].m_pdu.SetSize(1);
  irr.m_perCallInfo[0].m_pdu[0].m_sent = sent;
  irr.m_perCallInfo[0].m_pdu[0].m_h323pdu = pdu;

  connection.OnSendIRR(irr);
  SendUnsolicitedIRR(irr, response);
}


BOOL H323Gatekeeper::OnReceiveInfoRequest(const H225_InfoRequest & irq)
{
  if (!H225_RAS::OnReceiveInfoRequest(irq))
    return FALSE;

  H323RasPDU response(authenticators);
  H225_InfoRequestResponse & irr = BuildInfoRequestResponse(response, irq.m_requestSeqNum);

  if (irq.m_callReferenceValue == 0) {
    if (!AddAllInfoRequestResponseCall(irr, endpoint, endpoint.GetAllConnections())) {
      irr.IncludeOptionalField(H225_InfoRequestResponse::e_irrStatus);
      irr.m_irrStatus.SetTag(H225_InfoRequestResponseStatus::e_invalidCall);
    }
  }
  else {
    OpalGloballyUniqueID id = irq.m_callIdentifier.m_guid;
    H323Connection * connection = endpoint.FindConnectionWithLock(id.AsString());
    if (connection == NULL) {
      irr.IncludeOptionalField(H225_InfoRequestResponse::e_irrStatus);
      irr.m_irrStatus.SetTag(H225_InfoRequestResponseStatus::e_invalidCall);
    }
    else {
      if (irq.HasOptionalField(H225_InfoRequest::e_uuiesRequested))
        connection->SetUUIEsRequested(::GetUUIEsRequested(irq.m_uuiesRequested));

      AddInfoRequestResponseCall(irr, *connection);

      connection->Unlock();
    }
  }

  if (!irq.HasOptionalField(H225_InfoRequest::e_replyAddress))
    return WritePDU(response);

  H323TransportAddress replyAddress = irq.m_replyAddress;
  if (replyAddress.IsEmpty())
    return FALSE;

  H323TransportAddress oldAddress = transport->GetRemoteAddress();

  BOOL ok = transport->ConnectTo(replyAddress) && WritePDU(response);

  transport->ConnectTo(oldAddress);

  return ok;
}

#ifdef H323_H248

BOOL H323Gatekeeper::SendServiceControlIndication()
{
  PTRACE(3, "RAS\tSending Empty ServiceControlIndication");

  H323RasPDU pdu;
  H225_ServiceControlIndication & sci = pdu.BuildServiceControlIndication(GetNextSequenceNumber());

  sci.m_serviceControl.SetSize(0);

  Request request(sci.m_requestSeqNum, pdu);
  return MakeRequest(request);
}

BOOL H323Gatekeeper::OnReceiveServiceControlIndication(const H225_ServiceControlIndication & sci)
{
  if (!H225_RAS::OnReceiveServiceControlIndication(sci))
    return FALSE;

  H323Connection * connection = NULL;

  if (sci.HasOptionalField(H225_ServiceControlIndication::e_callSpecific)) {
    OpalGloballyUniqueID id = sci.m_callSpecific.m_callIdentifier.m_guid;
    if (id.IsNULL())
      id = sci.m_callSpecific.m_conferenceID;
    connection = endpoint.FindConnectionWithLock(id.AsString());
  }

  OnServiceControlSessions(sci.m_serviceControl, connection);


  H323RasPDU response(authenticators);
  response.BuildServiceControlResponse(sci.m_requestSeqNum);
  return WritePDU(response);
}


void H323Gatekeeper::OnServiceControlSessions(const H225_ArrayOf_ServiceControlSession & serviceControl,
                                              H323Connection * connection)
{
  for (PINDEX i = 0; i < serviceControl.GetSize(); i++) {
    H225_ServiceControlSession & pdu = serviceControl[i];

    H323ServiceControlSession * session = NULL;
    unsigned sessionId = pdu.m_sessionId;

    if (serviceControlSessions.Contains(sessionId)) {
      session = &serviceControlSessions[sessionId];
      if (pdu.HasOptionalField(H225_ServiceControlSession::e_contents)) {
        if (!session->OnReceivedPDU(pdu.m_contents)) {
          PTRACE(2, "SvcCtrl\tService control for session has changed!");
          session = NULL;
        }
      }
    }

    if (session == NULL && pdu.HasOptionalField(H225_ServiceControlSession::e_contents)) {
      session = endpoint.CreateServiceControlSession(pdu.m_contents);
      serviceControlSessions.SetAt(sessionId, session);
    }

    if (session != NULL)
      endpoint.OnServiceControlSession(pdu.m_reason.GetTag(),sessionId, *session, connection);
  }
}

#endif // H323_H248


void H323Gatekeeper::SetPassword(const PString & password, 
                                 const PString & username)
{
  localId = username;
  if (localId.IsEmpty())
    localId = endpoint.GetLocalUserName();

  for (PINDEX i = 0; i < authenticators.GetSize(); i++) {
    authenticators[i].SetLocalId(localId);
    authenticators[i].SetPassword(password);
  }
}


void H323Gatekeeper::MonitorMain(PThread &, INT)
{
  PTRACE(3, "RAS\tBackground thread started");

  for (;;) {
    monitorTickle.Wait();
    if (monitorStop)
      break;

    if (reregisterNow || 
                (!timeToLive.IsRunning() && timeToLive.GetResetTime() > 0)) {
      RegistrationTimeToLive();
      timeToLive.Reset();
    }

    if (!infoRequestRate.IsRunning() && infoRequestRate.GetResetTime() > 0) {
      InfoRequestResponse();
      infoRequestRate.Reset();
    }
  }

  PTRACE(3, "RAS\tBackground thread ended");
}


void H323Gatekeeper::TickleMonitor(PTimer &, INT)
{
  monitorTickle.Signal();
}


void H323Gatekeeper::SetAlternates(const H225_ArrayOf_AlternateGK & alts, BOOL permanent)
{
  PINDEX i;

  if (!alternatePermanent)  {
    // don't want to replace alternates gatekeepers if this is an alternate and it's not permanent
    for (i = 0; i < alternates.GetSize(); i++) {
      if (transport->GetRemoteAddress().IsEquivalent(alternates[i].rasAddress) &&
          gatekeeperIdentifier == alternates[i].gatekeeperIdentifier)
        return;
    }
  }

  alternates.RemoveAll();

  if (assignedGK != NULL)
	  alternates.Append(assignedGK);

  for (i = 0; i < alts.GetSize(); i++) {
    AlternateInfo * alt = new AlternateInfo(alts[i]);
    if (alt->rasAddress.IsEmpty())
      delete alt;
    else
      alternates.Append(alt);
  }
  
  alternatePermanent = permanent;

  PTRACE(3, "RAS\tSet alternate gatekeepers:\n"
         << setfill('\n') << alternates << setfill(' '));
}

void H323Gatekeeper::SetAssignedGatekeeper(const H225_AlternateGK & gk)
{
   assignedGK = new AlternateInfo(gk);
}

BOOL H323Gatekeeper::GetAssignedGatekeeper(H225_AlternateGK & gk)
{
	if (assignedGK == NULL)
		return FALSE;

	gk = assignedGK->GetAlternate();
	return TRUE;
}


BOOL H323Gatekeeper::MakeRequestWithReregister(Request & request, unsigned unregisteredTag)
{
  if (MakeRequest(request))
    return TRUE;

  if (request.responseResult == Request::RejectReceived &&
      request.rejectReason != unregisteredTag)
    return FALSE;

  PTRACE(2, "RAS\tEndpoint has become unregistered from gatekeeper " << gatekeeperIdentifier);

  // Have been told we are not registered (or gk offline)
  switch (request.responseResult) {
    case Request::NoResponseReceived :
      registrationFailReason = TransportError;
      break;

    case Request::BadCryptoTokens :
      registrationFailReason = SecurityDenied;
      break;

    default :
      registrationFailReason = GatekeeperLostRegistration;
  }

  // If we are not registered and auto register is set ...
  if (!autoReregister)
    return FALSE;

  reregisterNow = TRUE;
  monitorTickle.Signal();
  return FALSE;
}


void H323Gatekeeper::Connect(const H323TransportAddress & address,
                             const PString & gkid)
{
  if (transport == NULL)
    transport = new H323TransportUDP(endpoint, PIPSocket::GetDefaultIpAny());

  transport->SetRemoteAddress(address);
  transport->Connect();
  gatekeeperIdentifier = gkid;
}


BOOL H323Gatekeeper::MakeRequest(Request & request)
{
  if (PAssertNULL(transport) == NULL)
    return FALSE;

  // Set authenticators if not already set by caller
  requestMutex.Wait();

  if (request.requestPDU.GetAuthenticators().IsEmpty())
    request.requestPDU.SetAuthenticators(authenticators);

  /* To be sure that the H323 Cleaner, H225 Caller or Monitor don't set the
     transport address of the alternate while the other is in timeout. We
     have to block the function */

  H323TransportAddress tempAddr = transport->GetRemoteAddress();
  PString tempIdentifier = gatekeeperIdentifier;

  PINDEX alt = 0;
  for (;;) {
    if (H225_RAS::MakeRequest(request)) {
      if (!alternatePermanent &&
            (transport->GetRemoteAddress() != tempAddr ||
             gatekeeperIdentifier != tempIdentifier))
        Connect(tempAddr, tempIdentifier);
      requestMutex.Signal();
      return TRUE;
    }
    
    if (request.responseResult != Request::NoResponseReceived &&
        request.responseResult != Request::TryAlternate) {
      // try alternate in those cases and see if it's successful
      requestMutex.Signal();
      return FALSE;
    }
    
    AlternateInfo * altInfo;
    PIPSocket::Address localAddress;
    WORD localPort;
    do {
      if (alt >= alternates.GetSize()) {
        if (!alternatePermanent) 
          Connect(tempAddr,tempIdentifier);
        requestMutex.Signal();
        return FALSE;
      }
      
      altInfo = &alternates[alt++];
      transport->GetLocalAddress().GetIpAndPort(localAddress,localPort);
      transport->CleanUpOnTermination();
      delete transport;

      transport = new H323TransportUDP(endpoint,localAddress,localPort);
      transport->SetRemoteAddress (altInfo->rasAddress);
      transport->Connect();
      gatekeeperIdentifier = altInfo->gatekeeperIdentifier;
      StartChannel();
    } while (altInfo->registrationState == AlternateInfo::RegistrationFailed);
    
    if (altInfo->registrationState == AlternateInfo::NeedToRegister) {
      altInfo->registrationState = AlternateInfo::RegistrationFailed;
      registrationFailReason = TransportError;
      discoveryComplete = FALSE;
      H323RasPDU pdu;
      Request req(SetupGatekeeperRequest(pdu), pdu);
      
      if (H225_RAS::MakeRequest(req)) {
        requestMutex.Signal(); // avoid deadlock...
        if (RegistrationRequest(autoReregister)) {
          altInfo->registrationState = AlternateInfo::IsRegistered;
          // The wanted registration is done, we can return
          if (request.requestPDU.GetChoice().GetTag() == H225_RasMessage::e_registrationRequest) {
	    if (!alternatePermanent)
	      Connect(tempAddr,tempIdentifier);
	    return TRUE;
          }
        }
        requestMutex.Wait();
      }
    }
  }
}
	

H323Gatekeeper::AlternateInfo::AlternateInfo(const H225_AlternateGK & alt)
  : rasAddress(alt.m_rasAddress),
    gatekeeperIdentifier(alt.m_gatekeeperIdentifier.GetValue()),
    priority(alt.m_priority)
{
  registrationState = alt.m_needToRegister ? NeedToRegister : NoRegistrationNeeded;
}


H323Gatekeeper::AlternateInfo::~AlternateInfo ()
{

}


PObject::Comparison H323Gatekeeper::AlternateInfo::Compare(const PObject & obj)
{
  PAssert(PIsDescendant(&obj, H323Gatekeeper), PInvalidCast);
  unsigned otherPriority = ((const AlternateInfo & )obj).priority;
  if (priority < otherPriority)
    return LessThan;
  if (priority > otherPriority)
    return GreaterThan;
  return EqualTo;
}

H225_AlternateGK H323Gatekeeper::AlternateInfo::GetAlternate()
{
	H225_AlternateGK gk;
    rasAddress.SetPDU(gk.m_rasAddress);
    gk.m_gatekeeperIdentifier = gatekeeperIdentifier;
    gk.m_priority = priority;
	gk.m_needToRegister = registrationState;

	return gk;
}


void H323Gatekeeper::AlternateInfo::PrintOn(ostream & strm) const
{
  if (!gatekeeperIdentifier)
    strm << gatekeeperIdentifier << '@';

  strm << rasAddress;

  if (priority > 0)
    strm << ";priority=" << priority;
}

BOOL H323Gatekeeper::OnSendFeatureSet(unsigned pduType, H225_FeatureSet & feats) const
{
#ifdef H323_H460
    return features->SendFeature(pduType, feats);
#else
    return endpoint.OnSendFeatureSet(pduType, feats);
#endif
}

void H323Gatekeeper::OnReceiveFeatureSet(unsigned pduType, const H225_FeatureSet & feats) const
{
#ifdef H323_H460
    features->ReceiveFeature(pduType, feats);
#else
    endpoint.OnReceiveFeatureSet(pduType, feats);
#endif
}

/////////////////////////////////////////////////////////////////////////////
