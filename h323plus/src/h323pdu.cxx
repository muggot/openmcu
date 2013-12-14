/*
 * h323pdu.cxx
 *
 * H.323 PDU definitions
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
 * $Log: h323pdu.cxx,v $
 * Revision 1.4  2007/10/19 19:54:17  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.3  2007/10/16 17:05:38  shorne
 * Allow cryptoTokens to be insert in the setup after receiving ARQ
 *
 * Revision 1.2  2007/08/20 19:13:28  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.154.2.3  2007/09/05 04:14:40  rjongbloed
 * Back ported from OPAL media packetization in TCS
 *
 * Revision 1.154.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.154  2006/07/17 09:52:57  shorne
 * Added more support URL alias Addresses
 *
 * Revision 1.153  2006/06/23 20:19:39  shorne
 * More H460 support
 *
 * Revision 1.152  2006/05/30 11:14:56  hfriederich
 * Switch from DISABLE_H460 to H323_H460
 *
 * Revision 1.151  2006/05/17 03:36:52  shorne
 * DISABLE_H460 default under linux
 *
 * Revision 1.150  2006/05/16 18:46:50  shorne
 * Fixed H460 ReleaseComplete missing call
 *
 * Revision 1.149  2006/05/16 11:45:17  shorne
 * more H460 support and non-call services
 *
 * Revision 1.148  2006/01/26 03:28:10  shorne
 * Caller Authentication added
 *
 * Revision 1.147  2006/01/20 00:32:24  csoutheren
 * First check-in of signalling aggregation code - incomplete and disabled by default
 *
 * Revision 1.146  2005/12/09 05:20:07  csoutheren
 * Added ability to set explicit Q.931 cause code on call end
 *
 * Revision 1.145  2005/11/21 20:59:59  shorne
 * Adjusted Call End Reasons
 *
 * Revision 1.144  2005/09/16 08:12:50  csoutheren
 * Added ability to set timeout for connect
 *
 * Revision 1.143  2005/08/30 08:29:31  csoutheren
 * Added new error code
 *
 * Revision 1.142  2005/03/04 03:21:21  csoutheren
 * Added local and remote addresses to all PDU logs to assist in debugging
 *
 * Revision 1.141  2005/01/04 08:08:45  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.140  2005/01/03 06:26:09  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.139  2004/12/21 23:33:47  csoutheren
 * Fixed #defines for H.460, thanks to Simon Horne
 *
 * Revision 1.138  2004/12/14 06:22:22  csoutheren
 * More OSP implementation
 *
 * Revision 1.137  2004/12/08 01:59:23  csoutheren
 * initial support for Transnexus OSP toolkit
 *
 * Revision 1.136  2004/09/03 01:06:10  csoutheren
 * Added initial hooks for H.460 GEF
 * Thanks to Simon Horne and ISVO (Asia) Pte Ltd. for this contribution
 *
 * Revision 1.135  2004/05/07 06:44:16  csoutheren
 * Fixed problem with empty Q>931 DisplayName
 *
 * Revision 1.134  2004/04/20 05:24:54  csoutheren
 * Added ability to specify Q.931 DisplayName
 *
 * Revision 1.133  2004/04/03 08:28:06  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.132  2004/04/03 06:56:52  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.131  2004/02/26 08:32:47  csoutheren
 * Added release complete codes for MCU
 *
 * Revision 1.130  2003/12/11 05:41:00  csoutheren
 * Added storage of H.225 version in endpoint structure
 * Disabled sending RIPs to endpoints that cannot handle them
 *
 * Revision 1.129  2003/05/29 09:19:52  rjongbloed
 * Fixed minor problem with including DisplayName IE if localPartyName
 *   is blank, now does not include it, thanks Auri Vizgaitis
 *
 * Revision 1.128  2003/05/06 06:24:16  robertj
 * Added continuous DTMF tone support (H.245 UserInputIndication - signalUpdate)
 *   as per header documentation, thanks Auri Vizgaitis
 *
 * Revision 1.127  2003/04/10 09:37:20  robertj
 * Added some more functions for converting to alias addresses.
 *
 * Revision 1.126  2003/04/01 03:11:51  robertj
 * Added function to get array of AliasAddress into PStringArray.
 *
 * Revision 1.125  2003/03/25 04:56:21  robertj
 * Fixed issues to do with multiple inheritence in transaction reply cache.
 *
 * Revision 1.124  2003/03/20 01:51:12  robertj
 * More abstraction of H.225 RAS and H.501 protocols transaction handling.
 *
 * Revision 1.123  2003/02/25 06:48:19  robertj
 * More work on PDU transaction abstraction.
 *
 * Revision 1.122  2003/02/21 05:25:45  craigs
 * Abstracted out underlying transports for use with peerelements
 *
 * Revision 1.121  2003/02/03 04:31:05  robertj
 * Added special case for Cisco vendor field, they leave it rather incomplete,
 *
 * Revision 1.120  2003/01/26 02:57:58  craigs
 * Fixed oops in last checkin
 *
 * Revision 1.119  2003/01/26 02:50:38  craigs
 * Change so SETUP PDU uses conference and callIdentifier from H323Connection,
 * rather than both doing seperately and then overwriting
 *
 * Revision 1.118  2002/11/28 04:41:48  robertj
 * Added support for RAS ServiceControlIndication command.
 *
 * Revision 1.117  2002/11/27 06:54:57  robertj
 * Added Service Control Session management as per Annex K/H.323 via RAS
 *   only at this stage.
 * Added H.248 ASN and very primitive infrastructure for linking into the
 *   Service Control Session management system.
 * Added basic infrastructure for Annex K/H.323 HTTP transport system.
 * Added Call Credit Service Control to display account balances.
 *
 * Revision 1.116  2002/11/21 05:21:42  robertj
 * Fixed bug where get lots of zeros at the end of some PDU's
 *
 * Revision 1.115  2002/11/21 04:15:36  robertj
 * Added some changes to help interop with brain dead ASN decoders that do not
 *   know to ignore fields (eg greater than version 2) they do not understand.
 *
 * Revision 1.114  2002/11/13 04:38:17  robertj
 * Added ability to get (and set) Q.931 release complete cause codes.
 *
 * Revision 1.113  2002/11/07 03:50:28  robertj
 * Added extra "congested" Q.931 codes.
 *
 * Revision 1.112  2002/10/31 00:45:22  robertj
 * Enhanced jitter buffer system so operates dynamically between minimum and
 *   maximum values. Altered API to assure app writers note the change!
 *
 * Revision 1.111  2002/10/08 13:08:21  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.110  2002/08/14 01:07:22  robertj
 * Added translation of Q.931 unallocated number release complete code to
 *   OpenH323 EndedByNoUser which is the nearest match.
 *
 * Revision 1.109  2002/08/12 05:38:24  robertj
 * Changes to the RAS subsystem to support ability to make requests to client
 *   from gkserver without causing bottlenecks and race conditions.
 *
 * Revision 1.108  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.107  2002/08/05 05:17:41  robertj
 * Fairly major modifications to support different authentication credentials
 *   in ARQ to the logged in ones on RRQ. For both client and server.
 * Various other H.235 authentication bugs and anomalies fixed on the way.
 *
 * Revision 1.106  2002/07/31 02:25:04  robertj
 * Fixed translation of some call end reasons for to Q.931 codes.
 *
 * Revision 1.105  2002/07/25 10:55:44  robertj
 * Changes to allow more granularity in PDU dumps, hex output increasing
 *   with increasing trace level.
 *
 * Revision 1.104  2002/07/11 07:04:12  robertj
 * Added build InfoRequest pdu type to RAS.
 *
 * Revision 1.103  2002/06/13 03:59:56  craigs
 * Added codes to progress messages to allow inband audio before connect
 *
 * Revision 1.102  2002/05/29 03:55:21  robertj
 * Added protocol version number checking infrastructure, primarily to improve
 *   interoperability with stacks that are unforgiving of new features.
 *
 * Revision 1.101  2002/05/29 00:03:19  robertj
 * Fixed unsolicited IRR support in gk client and server,
 *   including support for IACK and INAK.
 *
 * Revision 1.100  2002/05/21 09:32:49  robertj
 * Added ability to set multiple alias names ona  connection by connection
 *   basis, defaults to endpoint list, thanks Artis Kugevics
 *
 * Revision 1.99  2002/05/07 03:18:15  robertj
 * Added application info (name/version etc) into registered endpoint data.
 *
 * Revision 1.98  2002/05/03 09:18:49  robertj
 * Added automatic retransmission of RAS responses to retried requests.
 *
 * Revision 1.97  2002/04/24 01:08:09  robertj
 * Added output of RAS pdu sequence number to level 3 trace output.
 *
 * Revision 1.96  2002/03/27 06:04:43  robertj
 * Added Temporary Failure end code for connection, an application may
 *   immediately retry the call if this occurs.
 *
 * Revision 1.95  2002/03/14 07:56:48  robertj
 * Added ability to specify alias type in H323SetAliasAddress, if not specified
 *   then defaults to previous behaviour, thanks Nils Bokerman.
 *
 * Revision 1.94  2002/02/13 07:52:30  robertj
 * Fixed missing parameters on Q.931 calling number, thanks Markus Rydh
 *
 * Revision 1.93  2002/02/01 01:48:45  robertj
 * Some more fixes for T.120 channel establishment, more to do!
 *
 * Revision 1.92  2002/01/18 06:01:23  robertj
 * Added some H323v4 functions (fastConnectRefused & TCS in SETUP)
 *
 * Revision 1.91  2001/12/15 07:10:59  robertj
 * Added functions to get E.164 address from alias address or addresses.
 *
 * Revision 1.90  2001/12/14 08:36:36  robertj
 * More implementation of T.38, thanks Adam Lazur
 *
 * Revision 1.89  2001/12/14 06:38:35  robertj
 * Broke out conversion of Q.850 and H.225 release complete codes to
 *   OpenH323 call end reasons enum.
 *
 * Revision 1.88  2001/12/13 10:56:28  robertj
 * Added build of request in progress pdu.
 *
 * Revision 1.87  2001/10/18 00:58:51  robertj
 * Fixed problem with GetDestinationAlias() adding source aliases instead
 *   of the destination from the setup PDU, thanks Mikael Stolt.
 *
 * Revision 1.86  2001/10/09 06:55:26  robertj
 * Fixed separating destCallSignalAddress fields with tabs in
 *    GetDestinationAlias() function, thanks Lee Kirchhoff
 *
 * Revision 1.85  2001/09/26 07:05:29  robertj
 * Fixed incorrect tags in building some PDU's, thanks Chris Purvis.
 *
 * Revision 1.84  2001/09/14 00:08:20  robertj
 * Optimised H323SetAliasAddress to use IsE164 function.
 *
 * Revision 1.83  2001/09/12 07:48:05  robertj
 * Fixed various problems with tracing.
 *
 * Revision 1.82  2001/08/16 07:49:19  robertj
 * Changed the H.450 support to be more extensible. Protocol handlers
 *   are now in separate classes instead of all in H323Connection.
 *
 * Revision 1.81  2001/08/10 11:03:52  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 * Revision 1.80  2001/08/06 03:08:56  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.79  2001/06/14 06:25:16  robertj
 * Added further H.225 PDU build functions.
 * Moved some functionality from connection to PDU class.
 *
 * Revision 1.78  2001/06/14 00:45:21  robertj
 * Added extra parameters for Q.931 fields, thanks Rani Assaf
 *
 * Revision 1.77  2001/06/05 03:14:41  robertj
 * Upgraded H.225 ASN to v4 and H.245 ASN to v7.
 *
 * Revision 1.76  2001/05/30 23:34:54  robertj
 * Added functions to send TCS=0 for transmitter side pause.
 *
 * Revision 1.75  2001/05/09 04:07:55  robertj
 * Added more call end codes for busy and congested.
 *
 * Revision 1.74  2001/05/03 06:45:21  robertj
 * Changed trace so dumps PDU if gets an error in decode.
 *
 * Revision 1.73  2001/04/11 03:01:29  robertj
 * Added H.450.2 (call transfer), thanks a LOT to Graeme Reid & Norwood Systems
 *
 * Revision 1.72  2001/03/24 00:58:03  robertj
 * Fixed MSVC warnings.
 *
 * Revision 1.71  2001/03/24 00:34:49  robertj
 * Added read/write hook functions so don't have to duplicate code in
 *    H323RasH235PDU descendant class of H323RasPDU.
 *
 * Revision 1.70  2001/03/23 05:38:30  robertj
 * Added PTRACE_IF to output trace if a conditional is TRUE.
 *
 * Revision 1.69  2001/03/02 06:59:59  robertj
 * Enhanced the globally unique identifier class.
 *
 * Revision 1.68  2001/02/09 05:13:56  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.67  2001/01/18 06:04:46  robertj
 * Bullet proofed code so local alias can not be empty string. This actually
 *   fixes an ASN PER encoding bug causing an assert.
 *
 * Revision 1.66  2000/10/12 05:11:54  robertj
 * Added trace log if get transport error on writing PDU.
 *
 * Revision 1.65  2000/09/25 06:48:11  robertj
 * Removed use of alias if there is no alias present, ie only have transport address.
 *
 * Revision 1.64  2000/09/22 01:35:51  robertj
 * Added support for handling LID's that only do symmetric codecs.
 *
 * Revision 1.63  2000/09/20 01:50:22  craigs
 * Added ability to set jitter buffer on a per-connection basis
 *
 * Revision 1.62  2000/09/05 01:16:20  robertj
 * Added "security" call end reason code.
 *
 * Revision 1.61  2000/07/15 09:51:41  robertj
 * Changed adding of Q.931 party numbers to only occur in SETUP.
 *
 * Revision 1.60  2000/07/13 12:29:49  robertj
 * Added some more cause codes on release complete,
 *
 * Revision 1.59  2000/07/12 10:20:43  robertj
 * Fixed incorrect tag code in H.245 ModeChange reject PDU.
 *
 * Revision 1.58  2000/07/09 15:21:11  robertj
 * Changed reference to the word "field" to be more correct IE or "Information Element"
 * Fixed return value of Q.931/H.225 PDU read so returns TRUE if no H.225 data in the
 *     User-User IE. Just flag it as empty and continue processing PDU's.
 *
 * Revision 1.57  2000/06/21 23:59:44  robertj
 * Fixed copy/paste error setting Q.931 display name to incorrect value.
 *
 * Revision 1.56  2000/06/21 08:07:47  robertj
 * Added cause/reason to release complete PDU, where relevent.
 *
 * Revision 1.55  2000/06/07 05:48:06  robertj
 * Added call forwarding.
 *
 * Revision 1.54  2000/05/25 01:59:05  robertj
 * Fixed bugs in calculation of GlLobally Uniqie ID according to DCE/H.225 rules.
 *
 * Revision 1.53  2000/05/23 11:32:37  robertj
 * Rewrite of capability table to combine 2 structures into one and move functionality into that class
 *    allowing some normalisation of usage across several applications.
 * Changed H323Connection so gets a copy of capabilities instead of using endponts, allows adjustments
 *    to be done depending on the remote client application.
 *
 * Revision 1.52  2000/05/15 08:38:59  robertj
 * Removed addition of calling/called party number field in Q.931 if there isn't one.
 *
 * Revision 1.51  2000/05/09 12:19:31  robertj
 * Added ability to get and set "distinctive ring" Q.931 functionality.
 *
 * Revision 1.50  2000/05/08 14:07:35  robertj
 * Improved the provision and detection of calling and caller numbers, aliases and hostnames.
 *
 * Revision 1.49  2000/05/08 05:06:27  robertj
 * Fixed bug in H.245 close logical channel timeout, thanks XuPeili.
 *
 * Revision 1.48  2000/05/02 04:32:27  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.47  2000/04/14 17:29:43  robertj
 * Fixed display of error message on timeout when timeouts are not errors.
 *
 * Revision 1.46  2000/04/10 20:39:18  robertj
 * Added support for more sophisticated DTMF and hook flash user indication.
 * Added function to extract E164 address from Q.931/H.225 PDU.
 *
 * Revision 1.45  2000/03/29 04:42:19  robertj
 * Improved some trace logging messages.
 *
 * Revision 1.44  2000/03/25 02:01:07  robertj
 * Added adjustable caller name on connection by connection basis.
 *
 * Revision 1.43  2000/03/21 01:08:10  robertj
 * Fixed incorrect call reference code being used in originated call.
 *
 * Revision 1.42  2000/02/17 12:07:43  robertj
 * Used ne wPWLib random number generator after finding major problem in MSVC rand().
 *
 * Revision 1.41  1999/12/23 22:47:09  robertj
 * Added calling party number field.
 *
 * Revision 1.40  1999/12/11 02:21:00  robertj
 * Added ability to have multiple aliases on local endpoint.
 *
 * Revision 1.39  1999/11/16 13:21:38  robertj
 * Removed extraneous error trace when doing asynchronous answer call.
 *
 * Revision 1.38  1999/11/15 14:11:29  robertj
 * Fixed trace output stream being put back after setting hex/fillchar modes.
 *
 * Revision 1.37  1999/11/10 23:30:20  robertj
 * Fixed unexpected closing of transport on PDU read error.
 *
 * Revision 1.36  1999/11/01 00:48:31  robertj
 * Added assert for illegal condition in capabilities, must have set if have table.
 *
 * Revision 1.35  1999/10/30 23:48:21  robertj
 * Fixed incorrect PDU type for H225 RAS location request.
 *
 * Revision 1.34  1999/10/29 03:35:06  robertj
 * Fixed setting of unique ID using fake MAC address from Win32 PPP device.
 *
 * Revision 1.33  1999/09/21 14:09:49  robertj
 * Removed warnings when no tracing enabled.
 *
 * Revision 1.32  1999/09/10 09:03:01  robertj
 * Used new GetInterfaceTable() function to get ethernet address for UniqueID
 *
 * Revision 1.31  1999/09/10 03:36:48  robertj
 * Added simple Q.931 Status response to Q.931 Status Enquiry
 *
 * Revision 1.30  1999/08/31 12:34:19  robertj
 * Added gatekeeper support.
 *
 * Revision 1.29  1999/08/31 11:37:30  robertj
 * Fixed problem with apparently randomly losing signalling channel.
 *
 * Revision 1.28  1999/08/25 05:08:14  robertj
 * File fission (critical mass reached).
 *
 * Revision 1.27  1999/08/13 06:34:38  robertj
 * Fixed problem in CallPartyNumber Q.931 encoding.
 * Added field name display to Q.931 protocol.
 *
 * Revision 1.26  1999/08/10 13:14:15  robertj
 * Added Q.931 Called Number field if have "phone number" style destination addres.
 *
 * Revision 1.25  1999/08/10 11:38:03  robertj
 * Changed population of setup UUIE destinationAddress if can be IA5 string.
 *
 * Revision 1.24  1999/07/26 05:10:30  robertj
 * Fixed yet another race condition on connection termination.
 *
 * Revision 1.23  1999/07/16 14:03:52  robertj
 * Fixed bug in Master/Slave negotiation that can cause looping.
 *
 * Revision 1.22  1999/07/16 06:15:59  robertj
 * Corrected semantics for tunnelled master/slave determination in fast start.
 *
 * Revision 1.21  1999/07/16 02:15:30  robertj
 * Fixed more tunneling problems.
 *
 * Revision 1.20  1999/07/15 14:45:36  robertj
 * Added propagation of codec open error to shut down logical channel.
 * Fixed control channel start up bug introduced with tunnelling.
 *
 * Revision 1.19  1999/07/15 09:08:04  robertj
 * Added extra debugging for if have PDU decoding error.
 *
 * Revision 1.18  1999/07/15 09:04:31  robertj
 * Fixed some fast start bugs
 *
 * Revision 1.17  1999/07/10 02:51:36  robertj
 * Added mutexing in H245 procedures. Also fixed MSD state bug.
 *
 * Revision 1.16  1999/07/09 06:09:50  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.15  1999/06/25 10:25:35  robertj
 * Added maintentance of callIdentifier variable in H.225 channel.
 *
 * Revision 1.14  1999/06/22 13:45:40  robertj
 * Fixed conferenceIdentifier generation algorithm to bas as in spec.
 *
 * Revision 1.13  1999/06/19 15:18:38  robertj
 * Fixed bug in MasterSlaveDeterminationAck pdu has incorrect master/slave state.
 *
 * Revision 1.12  1999/06/14 15:08:40  robertj
 * Added GSM codec class frame work (still no actual codec).
 *
 * Revision 1.11  1999/06/14 06:39:08  robertj
 * Fixed problem with getting transmit flag to channel from PDU negotiator
 *
 * Revision 1.10  1999/06/14 05:15:56  robertj
 * Changes for using RTP sessions correctly in H323 Logical Channel context
 *
 * Revision 1.9  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.8  1999/06/09 05:26:19  robertj
 * Major restructuring of classes.
 *
 * Revision 1.7  1999/06/06 06:06:36  robertj
 * Changes for new ASN compiler and v2 protocol ASN files.
 *
 * Revision 1.6  1999/04/26 06:20:22  robertj
 * Fixed bugs in protocol
 *
 * Revision 1.5  1999/04/26 06:14:47  craigs
 * Initial implementation for RTP decoding and lots of stuff
 * As a whole, these changes are called "First Noise"
 *
 * Revision 1.4  1999/02/23 11:04:28  robertj
 * Added capability to make outgoing call.
 *
 * Revision 1.3  1999/02/06 09:23:39  robertj
 * BeOS port
 *
 * Revision 1.2  1999/01/16 02:34:57  robertj
 * GNU compiler compatibility.
 *
 * Revision 1.1  1999/01/16 01:30:54  robertj
 * Initial revision
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h323pdu.h"
#endif

#include "h323pdu.h"

#include "h323ep.h"
#include "h225ras.h"

#ifdef H323_H460
#include "h460/h460.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif

#define new PNEW

const unsigned H225_ProtocolID[] = { 0,0,8,2250,0,H225_PROTOCOL_VERSION };
const unsigned H245_ProtocolID[] = { 0,0,8,245 ,0,H245_PROTOCOL_VERSION };

static const char E164NumberPrefix[] = "E164:";
static const char PrivatePartyPrefix[] = "Private:";
static const char DataPartyPrefix[] = "Data:";
static const char TelexPartyPrefix[] = "Telex:";
static const char NSPNumberPrefix[] = "NSP:";


///////////////////////////////////////////////////////////////////////////////

#if PTRACING
void H323TraceDumpPDU(const char * proto,
                      BOOL writing,
                      const PBYTEArray & rawData,
                      const PASN_Object & pdu,
                      const PASN_Choice & tags,
                      unsigned seqNum,
                      const H323TransportAddress & locAddr,
                      const H323TransportAddress & remAddr)
{
  if (!PTrace::CanTrace(3))
    return;

  ostream & trace = PTrace::Begin(3, __FILE__, __LINE__);
  trace << proto << '\t' << (writing ? "Send" : "Receiv") << "ing PDU [";

  if (locAddr.IsEmpty())
    trace << "(noaddr)";
  else
    trace << locAddr;
  trace << "/";
  if (remAddr.IsEmpty())
    trace << "(noaddr)";
  else
    trace << remAddr;

  trace << "] :";

  if (PTrace::CanTrace(4)) {
    trace << "\n  "
          << resetiosflags(ios::floatfield);

    if (!PTrace::CanTrace(5))
      trace << setiosflags(ios::fixed); // Will truncate hex dumps to 32 bytes

    trace << setprecision(2) << pdu
          << resetiosflags(ios::floatfield);

    if (PTrace::CanTrace(6))
      trace << "\nRaw PDU:\n"
            << hex << setfill('0')
            << setprecision(2) << rawData
            << dec << setfill(' ');
  }
  else {
    trace << ' ' << tags.GetTagName();
    PASN_Object & next = tags.GetObject();
    if (PIsDescendant(&next, PASN_Choice))
      trace << ' ' << ((PASN_Choice &)next).GetTagName();
    if (seqNum > 0)
      trace << ' ' << seqNum;
  }

  trace << PTrace::End;
}
#endif


///////////////////////////////////////////////////////////////////////////////

void H323SetAliasAddresses(const H323TransportAddressArray & addresses, H225_ArrayOf_AliasAddress & aliases)
{
  aliases.SetSize(addresses.GetSize());
  for (PINDEX i = 0; i < addresses.GetSize(); i++)
    H323SetAliasAddress(addresses[i], aliases[i]);
}


void H323SetAliasAddresses(const PStringArray & names,
                           H225_ArrayOf_AliasAddress & aliases,
                           int tag)
{
  aliases.SetSize(names.GetSize());
  for (PINDEX i = 0; i < names.GetSize(); i++)
    H323SetAliasAddress(names[i], aliases[i], tag);
}


void H323SetAliasAddresses(const PStringList & names,
                           H225_ArrayOf_AliasAddress & aliases,
                           int tag)
{
  aliases.SetSize(names.GetSize());
  for (PINDEX i = 0; i < names.GetSize(); i++)
    H323SetAliasAddress(names[i], aliases[i], tag);
}


static BOOL IsE164(const PString & str)
{
  return !str && strspn(str, "1234567890*#") == strlen(str);
}

static BOOL IsURL(const PString & str)
{
  return !str && str.Find("@") != P_MAX_INDEX;
}


void H323SetAliasAddress(const H323TransportAddress & address, H225_AliasAddress & alias)
{
  alias.SetTag(H225_AliasAddress::e_transportID);
  address.SetPDU(alias);
}

static struct {
  const char * name;
  int tag;
} aliasAddressTypes[5] = {
  { "e164",  H225_AliasAddress::e_dialedDigits },
  { "h323",  H225_AliasAddress::e_h323_ID },
  { "url",   H225_AliasAddress::e_url_ID },
  { "ip",    H225_AliasAddress::e_transportID },
  { "email", H225_AliasAddress::e_email_ID },
//  { "???",    H225_AliasAddresse_partyNumber },
//  { "???",    H225_AliasAddresse_mobileUIM }
};

void H323SetAliasAddress(const PString & _name, H225_AliasAddress & alias, int tag)
{
  PString name = _name;
  // See if alias type was explicitly specified
  if (tag < 0) {
    PINDEX colon = name.Find(':');
    if (colon != P_MAX_INDEX && colon > 0) {
      PString type = name.Left(colon);
      for (PINDEX i = 0; tag < 0 && i < 5; i++) {
        if (type == aliasAddressTypes[i].name) {
          tag = aliasAddressTypes[i].tag;
          name = name.Mid(colon+1);
        }
      }
    }
  }
  
  // otherwise guess it from the string: if all digits then assume an e164 address, @ URL else H323_ID.
  if (tag < 0) {
	if (IsE164(name)) 
		tag = H225_AliasAddress::e_dialedDigits;
	else if (IsURL(name)) 
		tag = H225_AliasAddress::e_url_ID;
	else 
		tag =H225_AliasAddress::e_h323_ID;
  }

  alias.SetTag(tag);
  switch (alias.GetTag()) {
    case H225_AliasAddress::e_dialedDigits :
    case H225_AliasAddress::e_url_ID :
    case H225_AliasAddress::e_email_ID :
      (PASN_IA5String &)alias = name;
      break;

    case H225_AliasAddress::e_h323_ID :
      (PASN_BMPString &)alias = name;
      break;

    case H225_AliasAddress::e_transportID :
    {
      H323TransportAddress addr = name;
      addr.SetPDU(alias);
      break;
    }
    case H225_AliasAddress::e_partyNumber :
    {
      H225_PartyNumber & party = alias;
      if (strncmp(name, E164NumberPrefix, sizeof(E164NumberPrefix)-1) == 0) {
        party.SetTag(H225_PartyNumber::e_e164Number);
        H225_PublicPartyNumber & number = party;
        number.m_publicNumberDigits = name.Mid(sizeof(E164NumberPrefix)-1);
      }
      else if (strncmp(name, PrivatePartyPrefix, sizeof(PrivatePartyPrefix)-1) == 0) {
        party.SetTag(H225_PartyNumber::e_privateNumber);
        H225_PrivatePartyNumber & number = party;
        number.m_privateNumberDigits = name.Mid(sizeof(PrivatePartyPrefix)-1);
      }
      else if (strncmp(name, DataPartyPrefix, sizeof(DataPartyPrefix)-1) == 0) {
        party.SetTag(H225_PartyNumber::e_dataPartyNumber);
        (H225_NumberDigits &)party = name.Mid(sizeof(DataPartyPrefix)-1);
      }
      else if (strncmp(name, TelexPartyPrefix, sizeof(TelexPartyPrefix)-1) == 0) {
        party.SetTag(H225_PartyNumber::e_telexPartyNumber);
        (H225_NumberDigits &)party = name.Mid(sizeof(TelexPartyPrefix)-1);
      }
      else if (strncmp(name, NSPNumberPrefix, sizeof(NSPNumberPrefix)-1) == 0) {
        party.SetTag(H225_PartyNumber::e_nationalStandardPartyNumber);
        (H225_NumberDigits &)party = name.Mid(sizeof(NSPNumberPrefix)-1);
      }
    }

    default :
      break;
  }
}


/////////////////////////////////////////////////////////////////////////////

PStringArray H323GetAliasAddressStrings(const H225_ArrayOf_AliasAddress & aliases)
{
  PStringArray strings(aliases.GetSize());

  for (PINDEX i = 0; i < aliases.GetSize(); i++)
    strings[i] = H323GetAliasAddressString(aliases[i]);

  return strings;
}


PString H323GetAliasAddressString(const H225_AliasAddress & alias)
{
  switch (alias.GetTag()) {
    case H225_AliasAddress::e_dialedDigits :
    case H225_AliasAddress::e_url_ID :
    case H225_AliasAddress::e_email_ID :
      return ((const PASN_IA5String &)alias).GetValue();

    case H225_AliasAddress::e_h323_ID :
      return ((const PASN_BMPString &)alias).GetValue();

    case H225_AliasAddress::e_transportID :
      return H323TransportAddress(alias);

    case H225_AliasAddress::e_partyNumber :
    {
      const H225_PartyNumber & party = alias;
      switch (party.GetTag()) {
        case H225_PartyNumber::e_e164Number :
        {
          const H225_PublicPartyNumber & number = party;
          return E164NumberPrefix + (PString)number.m_publicNumberDigits;
        }

        case H225_PartyNumber::e_privateNumber :
        {
          const H225_PrivatePartyNumber & number = party;
          return PrivatePartyPrefix + (PString)number.m_privateNumberDigits;
        }

        case H225_PartyNumber::e_dataPartyNumber :
          return DataPartyPrefix + (PString)(const H225_NumberDigits &)party;

        case H225_PartyNumber::e_telexPartyNumber :
          return TelexPartyPrefix + (PString)(const H225_NumberDigits &)party;

        case H225_PartyNumber::e_nationalStandardPartyNumber :
          return NSPNumberPrefix + (PString)(const H225_NumberDigits &)party;
      }
      break;
    }

    default :
      break;
  }

  return PString();
}


PString H323GetAliasAddressE164(const H225_AliasAddress & alias)
{
  PString str = H323GetAliasAddressString(alias);
  if (IsE164(str))
    return str;

  return PString();
}


PString H323GetAliasAddressE164(const H225_ArrayOf_AliasAddress & aliases)
{
  for (PINDEX i = 0; i < aliases.GetSize(); i++) {
    PString alias = H323GetAliasAddressE164(aliases[i]);
    if (!alias)
      return alias;
  }

  return PString();
}


///////////////////////////////////////////////////////////////////////////////

PString H323GetApplicationInfo(const H225_VendorIdentifier & vendor)
{
  PStringStream str;

  PString product = vendor.m_productId.AsString();
  PString version = vendor.m_versionId.AsString();

  // Special case, Cisco IOS does not put in the product and version fields
  if (vendor.m_vendor.m_t35CountryCode == 181 &&
      vendor.m_vendor.m_t35Extension == 0 &&
      vendor.m_vendor.m_manufacturerCode == 18) {
    if (product.IsEmpty())
      product = "Cisco IOS";
    if (version.IsEmpty())
      version = "12.2";
  }

  str << product << '\t' << version << '\t' << vendor.m_vendor.m_t35CountryCode;
  if (vendor.m_vendor.m_t35Extension != 0)
    str << '.' << vendor.m_vendor.m_t35Extension;
  str << '/' << vendor.m_vendor.m_manufacturerCode;

  str.MakeMinimumSize();
  return str;
}


///////////////////////////////////////////////////////////////////////////////

BOOL H323SetRTPPacketization(H245_RTPPayloadType & rtpPacketization,
                             const OpalMediaFormat & mediaFormat,
                             RTP_DataFrame::PayloadTypes payloadType)
{
  PString mediaPacketization = mediaFormat.GetOptionString("Media Packetization");
  if (mediaPacketization.IsEmpty())
    return FALSE;

  if (mediaPacketization.NumCompare("RFC") == PObject::EqualTo) {
    rtpPacketization.m_payloadDescriptor.SetTag(H245_RTPPayloadType_payloadDescriptor::e_rfc_number);
    ((PASN_Integer &)rtpPacketization.m_payloadDescriptor) = mediaPacketization.Mid(3).AsUnsigned();
  }
  else if (mediaPacketization.FindSpan("0123456789.") == P_MAX_INDEX) {
    rtpPacketization.m_payloadDescriptor.SetTag(H245_RTPPayloadType_payloadDescriptor::e_oid);
    ((PASN_ObjectId &)rtpPacketization.m_payloadDescriptor) = mediaPacketization;
  }
  else {
    rtpPacketization.m_payloadDescriptor.SetTag(H245_RTPPayloadType_payloadDescriptor::e_nonStandardIdentifier);
    H245_NonStandardParameter & nonstd = rtpPacketization.m_payloadDescriptor;
    nonstd.m_nonStandardIdentifier.SetTag(H245_NonStandardIdentifier::e_h221NonStandard);
    H245_NonStandardIdentifier_h221NonStandard & h221 = nonstd.m_nonStandardIdentifier;
    h221.m_t35CountryCode = 9;
    h221.m_t35Extension = 0;
    h221.m_manufacturerCode = 61;
    nonstd.m_data = mediaPacketization;
  }

  if (payloadType == RTP_DataFrame::MaxPayloadType)
    payloadType = mediaFormat.GetPayloadType();

  rtpPacketization.IncludeOptionalField(H245_RTPPayloadType::e_payloadType);
  rtpPacketization.m_payloadType = payloadType;

  return TRUE;
}


BOOL H323GetRTPPacketization(OpalMediaFormat & mediaFormat, const H245_RTPPayloadType & rtpPacketization)
{
  PString mediaPacketization;

  switch (rtpPacketization.m_payloadDescriptor.GetTag()) {
    case H245_RTPPayloadType_payloadDescriptor::e_rfc_number :
      mediaPacketization.sprintf("RFC%u", ((const PASN_Integer &)rtpPacketization.m_payloadDescriptor).GetValue());
      break;

    case H245_RTPPayloadType_payloadDescriptor::e_oid :
      mediaPacketization = ((const PASN_ObjectId &)rtpPacketization.m_payloadDescriptor).AsString();
      if (mediaPacketization.IsEmpty()) {
        PTRACE(1, "RTP_UDP\tInvalid OID in packetization type.");
        return FALSE;
      }
      break;
    case H245_RTPPayloadType_payloadDescriptor::e_nonStandardIdentifier :
      mediaPacketization = ((const H245_NonStandardParameter &)rtpPacketization.m_payloadDescriptor).m_data.AsString();
      if (mediaPacketization.IsEmpty()) {
        PTRACE(1, "RTP_UDP\tInvalid non-standard identifier in packetization type.");
        return FALSE;
      }
      break;

    default :
      PTRACE(1, "RTP_UDP\tUnknown packetization type.");
      return FALSE;
  }
  
  mediaFormat.SetOptionString("Media Packetization", mediaPacketization);
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
#ifdef H323_H460
static void SendSetupFeatureSet(const H323Connection * connection, H225_Setup_UUIE & pdu)
{
   
  H225_FeatureSet fs;

    if (!connection->OnSendFeatureSet(H460_MessageType::e_setup, fs)) 
	      return;

	if (fs.HasOptionalField(H225_FeatureSet::e_neededFeatures)) {
        pdu.IncludeOptionalField(H225_Setup_UUIE::e_neededFeatures);
	    H225_ArrayOf_FeatureDescriptor & fsn = pdu.m_neededFeatures;
	    fsn = fs.m_neededFeatures;
	}
     
	if (fs.HasOptionalField(H225_FeatureSet::e_desiredFeatures)) {
        pdu.IncludeOptionalField(H225_Setup_UUIE::e_desiredFeatures);
	    H225_ArrayOf_FeatureDescriptor & fsn = pdu.m_desiredFeatures;
	    fsn = fs.m_desiredFeatures;
	}

	if (fs.HasOptionalField(H225_FeatureSet::e_supportedFeatures)) {
        pdu.IncludeOptionalField(H225_Setup_UUIE::e_supportedFeatures);
	    H225_ArrayOf_FeatureDescriptor & fsn = pdu.m_supportedFeatures;
	    fsn = fs.m_supportedFeatures;
	}
}


template <typename PDUType>
static void SendFeatureSet(const H323Connection * connection, unsigned code, H225_H323_UU_PDU & msg, PDUType & pdu)
{
 H225_FeatureSet fs;
    if (!connection->OnSendFeatureSet(code,fs))
		return;

	if (code == H460_MessageType::e_callProceeding) {
        pdu.IncludeOptionalField(PDUType::e_featureSet);
	    pdu.m_featureSet = fs; 
    } else {
		if (fs.HasOptionalField(H225_FeatureSet::e_supportedFeatures)) {
			msg.IncludeOptionalField(H225_H323_UU_PDU::e_genericData);

			H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
		    H225_ArrayOf_GenericData & data = msg.m_genericData;

			for (PINDEX i=0; i < fsn.GetSize(); i++) {
				 PINDEX lastPos = data.GetSize();
				 data.SetSize(lastPos+1);
				 data[lastPos] = fsn[i];
			}
		} 
	}
}
#endif


#ifndef DISABLE_CALLAUTH
template <typename PDUType>
static void BuildAuthenticatorPDU(PDUType & pdu, unsigned code, const H323Connection * connection)
{

H235Authenticators authenticators = connection->GetEPAuthenticators();

  if (!authenticators.IsEmpty()) {
    connection->GetEPAuthenticators().PrepareSignalPDU(code,pdu.m_tokens,pdu.m_cryptoTokens);

	if (pdu.m_tokens.GetSize() > 0)
		pdu.IncludeOptionalField(PDUType::e_tokens);

	if (pdu.m_cryptoTokens.GetSize() > 0) 
		pdu.IncludeOptionalField(PDUType::e_cryptoTokens);
  }
}
#endif
///////////////////////////////////////////////////////////////////////////////

H323SignalPDU::H323SignalPDU()
{
}


static unsigned SetH225Version(const H323Connection & connection,
                               H225_ProtocolIdentifier & protocolIdentifier)
{
  unsigned version = connection.GetSignallingVersion();
  protocolIdentifier.SetValue(psprintf("0.0.8.2250.0.%u", version));
  return version;
}

H225_Setup_UUIE & H323SignalPDU::BuildSetup(const H323Connection & connection,
                                            const H323TransportAddress & destAddr)
{

  q931pdu.BuildSetup(connection.GetCallReference());
  SetQ931Fields(connection, TRUE);

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_setup);
  H225_Setup_UUIE & setup = m_h323_uu_pdu.m_h323_message_body;

  if (SetH225Version(connection, setup.m_protocolIdentifier) < 3) {
    setup.RemoveOptionalField(H225_Setup_UUIE::e_multipleCalls);
    setup.RemoveOptionalField(H225_Setup_UUIE::e_maintainConnection);
  }

  setup.IncludeOptionalField(H225_Setup_UUIE::e_sourceAddress);
  H323SetAliasAddresses(connection.GetLocalAliasNames(), setup.m_sourceAddress);

  setup.m_conferenceID = connection.GetConferenceIdentifier();

  if (connection.GetEndPoint().OnSendCallIndependentSupplementaryService(&connection,*this))
	  setup.m_conferenceGoal.SetTag(H225_Setup_UUIE_conferenceGoal::e_callIndependentSupplementaryService);
  else if (connection.GetEndPoint().OnConferenceInvite(TRUE,&connection,*this))
	  setup.m_conferenceGoal.SetTag(H225_Setup_UUIE_conferenceGoal::e_invite);
  else
      setup.m_conferenceGoal.SetTag(H225_Setup_UUIE_conferenceGoal::e_create);

  setup.m_callType.SetTag(H225_CallType::e_pointToPoint);

  setup.m_callIdentifier.m_guid = connection.GetCallIdentifier();
  setup.m_mediaWaitForConnect = FALSE;
  setup.m_canOverlapSend = FALSE;

  if (!destAddr) {
    setup.IncludeOptionalField(H225_Setup_UUIE::e_destCallSignalAddress);
    destAddr.SetPDU(setup.m_destCallSignalAddress);
  }

  PString destAlias = connection.GetRemotePartyName();
  if (!destAlias && destAlias != destAddr) {
    setup.IncludeOptionalField(H225_Setup_UUIE::e_destinationAddress);
    setup.m_destinationAddress.SetSize(1);

    // Try and encode it as a phone number
    H323SetAliasAddress(destAlias, setup.m_destinationAddress[0]);
    if (setup.m_destinationAddress[0].GetTag() == H225_AliasAddress::e_dialedDigits)
      q931pdu.SetCalledPartyNumber(destAlias);
  }

  connection.SetEndpointTypeInfo(setup.m_sourceInfo);

  return setup;
}

#ifndef DISABLE_CALLAUTH
void H323SignalPDU::InsertCryptoTokensSetup(const H323Connection & connection, H225_Setup_UUIE & setup)
{
  BuildAuthenticatorPDU<H225_Setup_UUIE>(setup,H225_H323_UU_PDU_h323_message_body::e_setup,
	   &connection);
}
#endif

#ifdef H323_H460
void H323SignalPDU::InsertH460Setup(const H323Connection & connection, H225_Setup_UUIE & setup)
{
   SendSetupFeatureSet(&connection,setup);
}
#endif


H225_CallProceeding_UUIE &
        H323SignalPDU::BuildCallProceeding(const H323Connection & connection)
{
  q931pdu.BuildCallProceeding(connection.GetCallReference());
  SetQ931Fields(connection);

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_callProceeding);
  H225_CallProceeding_UUIE & proceeding = m_h323_uu_pdu.m_h323_message_body;

  if (SetH225Version(connection, proceeding.m_protocolIdentifier) < 3) {
    proceeding.RemoveOptionalField(H225_CallProceeding_UUIE::e_multipleCalls);
    proceeding.RemoveOptionalField(H225_CallProceeding_UUIE::e_maintainConnection);
  }

  proceeding.m_callIdentifier.m_guid = connection.GetCallIdentifier();

  connection.SetEndpointTypeInfo(proceeding.m_destinationInfo);

#ifdef H323_H460
   SendFeatureSet<H225_CallProceeding_UUIE>(&connection,H460_MessageType::e_callProceeding, m_h323_uu_pdu, proceeding);
#endif

#ifndef DISABLE_CALLAUTH
   if (connection.HasAuthentication()) {
      BuildAuthenticatorPDU<H225_CallProceeding_UUIE>(proceeding,H225_H323_UU_PDU_h323_message_body::e_callProceeding,
	   &connection);
   }
#endif

  return proceeding;
}


H225_Connect_UUIE & H323SignalPDU::BuildConnect(const H323Connection & connection)
{
  q931pdu.BuildConnect(connection.GetCallReference());
  SetQ931Fields(connection);

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_connect);
  H225_Connect_UUIE & connect = m_h323_uu_pdu.m_h323_message_body;

  if (SetH225Version(connection, connect.m_protocolIdentifier) < 3) {
    connect.RemoveOptionalField(H225_Connect_UUIE::e_multipleCalls);
    connect.RemoveOptionalField(H225_Connect_UUIE::e_maintainConnection);
  }
  connect.m_callIdentifier.m_guid = connection.GetCallIdentifier();
  connect.m_conferenceID = connection.GetConferenceIdentifier();

  connection.SetEndpointTypeInfo(connect.m_destinationInfo);

#ifdef H323_H460
   SendFeatureSet<H225_Connect_UUIE>(&connection,H460_MessageType::e_connect, m_h323_uu_pdu,connect);
#endif

#ifndef DISABLE_CALLAUTH
   BuildAuthenticatorPDU<H225_Connect_UUIE>(connect,H225_H323_UU_PDU_h323_message_body::e_connect,
						&connection);
#endif
  return connect;
}


H225_Connect_UUIE & H323SignalPDU::BuildConnect(const H323Connection & connection,
                                                const PIPSocket::Address & h245Address,
                                                WORD port)
{
  H225_Connect_UUIE & connect = BuildConnect(connection);

  // indicate we are including the optional H245 address in the PDU
  connect.IncludeOptionalField(H225_Connect_UUIE::e_h245Address);

  // convert IP address into the correct H245 type
  H323TransportAddress transAddr(h245Address, port);
  transAddr.SetPDU(connect.m_h245Address);

  return connect;
}


H225_Alerting_UUIE & H323SignalPDU::BuildAlerting(const H323Connection & connection)
{
  q931pdu.BuildAlerting(connection.GetCallReference());
  SetQ931Fields(connection);

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_alerting);
  H225_Alerting_UUIE & alerting = m_h323_uu_pdu.m_h323_message_body;

  if (SetH225Version(connection, alerting.m_protocolIdentifier) < 3) {
    alerting.RemoveOptionalField(H225_Alerting_UUIE::e_multipleCalls);
    alerting.RemoveOptionalField(H225_Alerting_UUIE::e_maintainConnection);
  }

  alerting.m_callIdentifier.m_guid = connection.GetCallIdentifier();
  connection.SetEndpointTypeInfo(alerting.m_destinationInfo);

#ifdef H323_H460
	SendFeatureSet<H225_Alerting_UUIE>(&connection,H460_MessageType::e_alerting, m_h323_uu_pdu,alerting);
#endif

#ifdef H323_H248
     if(connection.OnSendServiceControlSessions(alerting.m_serviceControl,
		                               H225_ServiceControlSession_reason::e_open))
		 alerting.IncludeOptionalField(H225_Alerting_UUIE::e_serviceControl);
#endif

#ifndef DISABLE_CALLAUTH
   BuildAuthenticatorPDU<H225_Alerting_UUIE>(alerting,H225_H323_UU_PDU_h323_message_body::e_alerting,
						&connection);
#endif

  return alerting;
}


H225_Information_UUIE & H323SignalPDU::BuildInformation(const H323Connection & connection)
{
  q931pdu.BuildInformation(connection.GetCallReference(), connection.HadAnsweredCall());
  SetQ931Fields(connection);

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_information);
  H225_Information_UUIE & information = m_h323_uu_pdu.m_h323_message_body;

  SetH225Version(connection, information.m_protocolIdentifier);
  information.m_callIdentifier.m_guid = connection.GetCallIdentifier();

  return information;
}


H323Connection::CallEndReason H323TranslateToCallEndReason(Q931::CauseValues cause,
                                                           const H225_ReleaseCompleteReason & reason)
{
  PTRACE(4,"H225\tCall End Reason " << cause);

  switch (cause) {
    case Q931::ErrorInCauseIE :
      switch (reason.GetTag()) {
        case H225_ReleaseCompleteReason::e_noBandwidth :
          return H323Connection::EndedByNoBandwidth;

        case H225_ReleaseCompleteReason::e_gatekeeperResources :
        case H225_ReleaseCompleteReason::e_gatewayResources :
        case H225_ReleaseCompleteReason::e_adaptiveBusy :
          return H323Connection::EndedByRemoteCongestion;

        case H225_ReleaseCompleteReason::e_unreachableDestination :
          return H323Connection::EndedByUnreachable;

        case H225_ReleaseCompleteReason::e_calledPartyNotRegistered :
          return H323Connection::EndedByNoUser;

        case H225_ReleaseCompleteReason::e_callerNotRegistered:
          return H323Connection::EndedByGatekeeper;

        case H225_ReleaseCompleteReason::e_securityDenied:
          return H323Connection::EndedBySecurityDenial;

        case H225_ReleaseCompleteReason::e_newConnectionNeeded:
          return H323Connection::EndedByTemporaryFailure;
      }
      // Do next case
    case Q931::UnknownCauseIE :
    case Q931::CallRejected :
      return H323Connection::EndedByRefusal;

    case Q931::NormalCallClearing :
      return H323Connection::EndedByRemoteUser;

    case Q931::UserBusy :
      return H323Connection::EndedByRemoteBusy;

    case Q931::Congestion :
    case Q931::ResourceUnavailable :
      return H323Connection::EndedByRemoteCongestion;

    case Q931::NoResponse :
		return H323Connection::EndedByHostOffline;

    case Q931::NoAnswer :
      return H323Connection::EndedByNoAnswer;

    case Q931::NoRouteToNetwork :
    case Q931::RequestedCircuitNotAvailable :
    case Q931::ChannelUnacceptable :
      return H323Connection::EndedByUnreachable;

    case Q931::UnallocatedNumber :
    case Q931::NoRouteToDestination :
    case Q931::SubscriberAbsent :
      return H323Connection::EndedByNoUser;

    case Q931::Redirection :
      return H323Connection::EndedByCallForwarded;

    case Q931::DestinationOutOfOrder :
    case Q931::NoCircuitChannelAvailable :
      return H323Connection::EndedByConnectFail;

    case Q931::TemporaryFailure :
      return H323Connection::EndedByTemporaryFailure;

    case Q931::InvalidNumberFormat :
      return H323Connection::EndedByInvalidNumberFormat;

    default:
      return H323Connection::EndedByQ931Cause;
  }
}


Q931::CauseValues H323TranslateFromCallEndReason(H323Connection::CallEndReason callEndReason,
                                                 H225_ReleaseCompleteReason & reason)
{
  static int const ReasonCodes[H323Connection::NumCallEndReasons] = {
    Q931::NormalCallClearing,                               /// EndedByLocalUser,         Local endpoint application cleared call
    Q931::UserBusy,                                         /// EndedByNoAccept,          Local endpoint did not accept call
    Q931::CallRejected,                                     /// EndedByAnswerDenied,      Local endpoint declined to answer call
    Q931::NormalCallClearing,                               /// EndedByRemoteUser,        Remote endpoint application cleared call
    -H225_ReleaseCompleteReason::e_destinationRejection,    /// EndedByRefusal,           Remote endpoint refused call
    Q931::NoAnswer,                                         /// EndedByNoAnswer,          Remote endpoint did not answer in required time
    Q931::NormalCallClearing,                               /// EndedByCallerAbort,       Remote endpoint stopped calling
    -H225_ReleaseCompleteReason::e_undefinedReason,         /// EndedByTransportFail,     Transport error cleared call
    -H225_ReleaseCompleteReason::e_unreachableDestination,  /// EndedByConnectFail,       Transport connection failed to establish call
    -H225_ReleaseCompleteReason::e_gatekeeperResources,     /// EndedByGatekeeper,        Gatekeeper has cleared call
    -H225_ReleaseCompleteReason::e_calledPartyNotRegistered,/// EndedByNoUser,            Call failed as could not find user (in GK)
    -H225_ReleaseCompleteReason::e_noBandwidth,             /// EndedByNoBandwidth,       Call failed as could not get enough bandwidth
    -H225_ReleaseCompleteReason::e_undefinedReason,         /// EndedByCapabilityExchange,Could not find common capabilities
    -H225_ReleaseCompleteReason::e_facilityCallDeflection,  /// EndedByCallForwarded,     Call was forwarded using FACILITY message
    -H225_ReleaseCompleteReason::e_securityDenied,          /// EndedBySecurityDenial,    Call failed a security check and was ended
    Q931::UserBusy,                                         /// EndedByLocalBusy,         Local endpoint busy
    Q931::Congestion,                                       /// EndedByLocalCongestion,   Local endpoint congested
    Q931::UserBusy,                                         /// EndedByRemoteBusy,        Remote endpoint busy
    Q931::Congestion,                                       /// EndedByRemoteCongestion,  Remote endpoint congested
    Q931::NoRouteToDestination,                             /// EndedByUnreachable,       Could not reach the remote party
    Q931::InvalidCallReference,                             /// EndedByNoEndPoint,        The remote party is not running an endpoint
    Q931::NoResponse,										/// EndedByHostOffline,       The remote party host off line
    Q931::TemporaryFailure,                                 /// EndedByTemporaryFailure   The remote failed temporarily app may retry
    Q931::UnknownCauseIE,                                   /// EndedByQ931Cause,         The remote ended the call with unmapped Q.931 cause code
    Q931::NormalUnspecified,                                /// EndedByDurationLimit,     Call cleared due to an enforced duration limit
    -H225_ReleaseCompleteReason::e_invalidCID,              /// EndedByInvalidConferenceID Call cleared due to invalid conference ID
    -H225_ReleaseCompleteReason::e_calledPartyNotRegistered, /// EndedByOSPRefusal          Call cleared as OSP server unable or unwilling to route
    Q931::InvalidNumberFormat,                               /// EndedByInvalidNumberFormat Call cleared as number was invalid format
    Q931::ProtocolErrorUnspecified                           /// EndedByUnspecifiedProtocolError Call cleared due to unspecified protocol error
  };

  int code = ReasonCodes[callEndReason];
  //if (code == Q931::UnknownCauseIE)
  //  return (Q931::CauseValues)connection.GetCallEndReason();
  if (code >= 0)
    return (Q931::CauseValues)code;

  reason.SetTag(-code);
  return Q931::ErrorInCauseIE;
}

Q931::CauseValues H323TranslateFromCallEndReason(const H323Connection & connection,
                                                 H225_ReleaseCompleteReason & reason)
{
  return H323TranslateFromCallEndReason(connection.GetCallEndReason(), reason);
}

H225_ReleaseComplete_UUIE &
        H323SignalPDU::BuildReleaseComplete(const H323Connection & connection)
{
  q931pdu.BuildReleaseComplete(connection.GetCallReference(), connection.HadAnsweredCall());

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_releaseComplete);

  H225_ReleaseComplete_UUIE & release = m_h323_uu_pdu.m_h323_message_body;

  SetH225Version(connection, release.m_protocolIdentifier);
  release.m_callIdentifier.m_guid = connection.GetCallIdentifier();

  Q931::CauseValues cause = (Q931::CauseValues)connection.GetQ931Cause();
  if (cause == Q931::ErrorInCauseIE)
    cause = H323TranslateFromCallEndReason(connection, release.m_reason);
  if (cause != Q931::ErrorInCauseIE)
    q931pdu.SetCause(cause);
  else
    release.IncludeOptionalField(H225_ReleaseComplete_UUIE::e_reason);

#ifndef DISABLE_CALLAUTH
  BuildAuthenticatorPDU<H225_ReleaseComplete_UUIE>(release,H225_H323_UU_PDU_h323_message_body::e_releaseComplete,
	   &connection);
#endif

#ifdef H323_H460
	SendFeatureSet<H225_ReleaseComplete_UUIE>(&connection,H460_MessageType::e_releaseComplete, m_h323_uu_pdu,release);
#endif

  return release;
}


H225_Facility_UUIE * H323SignalPDU::BuildFacility(const H323Connection & connection,
                                                  BOOL empty, unsigned reason)
{
  q931pdu.BuildFacility(connection.GetCallReference(), connection.HadAnsweredCall());
  if (empty) {
    m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_empty);
    return NULL;
  }

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_facility);
  H225_Facility_UUIE & fac = m_h323_uu_pdu.m_h323_message_body;

  if (reason != H225_FacilityReason::e_undefinedReason)
	       fac.m_reason.SetTag(reason);

  SetH225Version(connection, fac.m_protocolIdentifier);
  fac.IncludeOptionalField(H225_Facility_UUIE::e_callIdentifier);
  fac.m_callIdentifier.m_guid = connection.GetCallIdentifier();

#ifdef H323_H460
   if (reason == H225_FacilityReason::e_featureSetUpdate)
       SendFeatureSet<H225_Facility_UUIE>(&connection,H460_MessageType::e_facility, m_h323_uu_pdu,fac);
#endif

#ifndef DISABLE_CALLAUTH
  BuildAuthenticatorPDU<H225_Facility_UUIE>(fac,H225_H323_UU_PDU_h323_message_body::e_facility,
	   &connection);
#endif

  return &fac;
}


H225_Progress_UUIE & H323SignalPDU::BuildProgress(const H323Connection & connection)
{
  q931pdu.BuildProgress(connection.GetCallReference(), connection.HadAnsweredCall(), Q931::ProgressInbandInformationAvailable);
  SetQ931Fields(connection);

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_progress);
  H225_Progress_UUIE & progress = m_h323_uu_pdu.m_h323_message_body;

  SetH225Version(connection, progress.m_protocolIdentifier);
  progress.m_callIdentifier.m_guid = connection.GetCallIdentifier();
  connection.SetEndpointTypeInfo(progress.m_destinationInfo);

  return progress;
}


H225_Status_UUIE & H323SignalPDU::BuildStatus(const H323Connection & connection)
{
  q931pdu.BuildStatus(connection.GetCallReference(), connection.HadAnsweredCall());

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_status);
  H225_Status_UUIE & status = m_h323_uu_pdu.m_h323_message_body;

  SetH225Version(connection, status.m_protocolIdentifier);
  status.m_callIdentifier.m_guid = connection.GetCallIdentifier();

  return status;
}


H225_StatusInquiry_UUIE & H323SignalPDU::BuildStatusInquiry(const H323Connection & connection)
{
  q931pdu.BuildStatusEnquiry(connection.GetCallReference(), connection.HadAnsweredCall());

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_statusInquiry);
  H225_StatusInquiry_UUIE & inquiry = m_h323_uu_pdu.m_h323_message_body;

  SetH225Version(connection, inquiry.m_protocolIdentifier);
  inquiry.m_callIdentifier.m_guid = connection.GetCallIdentifier();

  return inquiry;
}


H225_SetupAcknowledge_UUIE & H323SignalPDU::BuildSetupAcknowledge(const H323Connection & connection)
{
  q931pdu.BuildSetupAcknowledge(connection.GetCallReference());

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_setupAcknowledge);
  H225_SetupAcknowledge_UUIE & setupAck = m_h323_uu_pdu.m_h323_message_body;

  SetH225Version(connection, setupAck.m_protocolIdentifier);
  setupAck.m_callIdentifier.m_guid = connection.GetCallIdentifier();

  return setupAck;
}


H225_Notify_UUIE & H323SignalPDU::BuildNotify(const H323Connection & connection)
{
  q931pdu.BuildNotify(connection.GetCallReference(), connection.HadAnsweredCall());

  m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_notify);
  H225_Notify_UUIE & notify = m_h323_uu_pdu.m_h323_message_body;

  SetH225Version(connection, notify.m_protocolIdentifier);
  notify.m_callIdentifier.m_guid = connection.GetCallIdentifier();

  return notify;
}


void H323SignalPDU::BuildQ931()
{
  // Encode the H225 PDu into the Q931 PDU as User-User data
  PPER_Stream strm;
  Encode(strm);
  strm.CompleteEncoding();
  q931pdu.SetIE(Q931::UserUserIE, strm);
}


void H323SignalPDU::PrintOn(ostream & strm) const
{
  int indent = (int)strm.precision() + 2;
  strm << "{\n"
       << setw(indent+10) << "q931pdu = " << setprecision(indent) << q931pdu << '\n'
       << setw(indent+10) << "h225pdu = " << setprecision(indent);
  H225_H323_UserInformation::PrintOn(strm);
  strm << '\n'
       << setw(indent-1) << "}";
}


BOOL H323SignalPDU::Read(H323Transport & transport)
{
  PBYTEArray rawData;
  if (!transport.ReadPDU(rawData)) {
    PTRACE_IF(1, transport.GetErrorCode(PChannel::LastReadError) != PChannel::Timeout,
              "H225\tRead error (" << transport.GetErrorNumber(PChannel::LastReadError)
              << "): " << transport.GetErrorText(PChannel::LastReadError));
    return FALSE;
  }

  return ProcessReadData(transport, rawData);
}

BOOL H323SignalPDU::ProcessReadData(H323Transport & transport, const PBYTEArray & rawData)
{
  if (!q931pdu.Decode(rawData)) {
    PTRACE(1, "H225\tParse error of Q931 PDU:\n" << hex << setfill('0')
                                                 << setprecision(2) << rawData
                                                 << dec << setfill(' '));
    return FALSE;
  }

  if (!q931pdu.HasIE(Q931::UserUserIE)) {
    m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_empty);
    PTRACE(1, "H225\tNo Q931 User-User Information Element,"
              "\nRaw PDU:\n" << hex << setfill('0')
                             << setprecision(2) << rawData
                             << dec << setfill(' ') <<
              "\nQ.931 PDU:\n  " << setprecision(2) << q931pdu);
    return TRUE;
  }

  PPER_Stream strm = q931pdu.GetIE(Q931::UserUserIE);
  if (!Decode(strm)) {
    PTRACE(1, "H225\tRead error: PER decode failure in Q.931 User-User Information Element,"
              "\nRaw PDU:\n" << hex << setfill('0')
                             << setprecision(2) << rawData
                             << dec << setfill(' ') <<
              "\nQ.931 PDU:\n  " << setprecision(2) << q931pdu <<
              "\nPartial PDU:\n  " << setprecision(2) << *this);
    m_h323_uu_pdu.m_h323_message_body.SetTag(H225_H323_UU_PDU_h323_message_body::e_empty);
    return TRUE;
  }

  H323TraceDumpPDU("H225", FALSE, rawData, *this, m_h323_uu_pdu.m_h323_message_body, 0, 
                   transport.GetLocalAddress(), transport.GetRemoteAddress());
  return TRUE;
}


BOOL H323SignalPDU::Write(H323Transport & transport, H323Connection & connection)
{
  if (!q931pdu.HasIE(Q931::UserUserIE) && m_h323_uu_pdu.m_h323_message_body.IsValid())
    BuildQ931();

  PBYTEArray rawData;
  if (!q931pdu.Encode(rawData))
    return FALSE;

#ifndef DISABLE_CALLAUTH
  int tag = m_h323_uu_pdu.m_h323_message_body.GetTag();
  connection.OnAuthenticationFinalise(tag,rawData);
#endif

  H323TraceDumpPDU("H225", TRUE, rawData, *this, m_h323_uu_pdu.m_h323_message_body, 0, 
                   transport.GetLocalAddress(), transport.GetRemoteAddress());

  if (transport.WritePDU(rawData))
    return TRUE;

  PTRACE(1, "H225\tWrite PDU failed ("
         << transport.GetErrorNumber(PChannel::LastWriteError)
         << "): " << transport.GetErrorText(PChannel::LastWriteError));
  return FALSE;
}

PString H323SignalPDU::GetSourceURL() const
{

  PString url = PString();

  if (m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_setup) {
    const H225_Setup_UUIE & setup = m_h323_uu_pdu.m_h323_message_body;

	if (setup.HasOptionalField(H225_Setup_UUIE::e_sourceCallSignalAddress)) {
		const H225_ArrayOf_AliasAddress & aliases = setup.e_sourceCallSignalAddress;
		if (aliases.GetSize() > 0) {
			for (PINDEX i = 0; i < aliases.GetSize(); i++) {
			   switch (aliases[i].GetTag()) {
				  case H225_AliasAddress::e_url_ID :
				  case H225_AliasAddress::e_email_ID:
					 url =  H323GetAliasAddressString(aliases[i]); 
				  default:
					 break;
			   }
			}
		}
	}
  }

	return url;
}

PStringArray H323SignalPDU::GetSourceAliasNames() const 
{ 
	PStringArray srcAliases;

  if (m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_setup) {
    const H225_Setup_UUIE & setup = m_h323_uu_pdu.m_h323_message_body;

    if (setup.m_sourceAddress.GetSize() > 0) {
      for (PINDEX i = 0; i < setup.m_sourceAddress.GetSize(); i++) {
		  PString alias = H323GetAliasAddressString(setup.m_sourceAddress[i]);
		  srcAliases.AppendString(alias);
	  }
	}
  }
	return srcAliases; 
}

PString H323SignalPDU::GetSourceAliases(const H323Transport * transport) const
{
  PString remoteHostName;
  
  if (transport != NULL)
    remoteHostName = transport->GetRemoteAddress().GetHostName();

  PString displayName = GetQ931().GetDisplayName();

  PStringStream aliases;
  if (displayName != remoteHostName)
    aliases << displayName;

  if (m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_setup) {
    const H225_Setup_UUIE & setup = m_h323_uu_pdu.m_h323_message_body;

    if (remoteHostName.IsEmpty() &&
        setup.HasOptionalField(H225_Setup_UUIE::e_sourceCallSignalAddress)) {
      H323TransportAddress remoteAddress(setup.m_sourceCallSignalAddress);
      remoteHostName = remoteAddress.GetHostName();
    }

    if (setup.m_sourceAddress.GetSize() > 0) {
      BOOL needParen = !aliases.IsEmpty();
      BOOL needComma = FALSE;
      for (PINDEX i = 0; i < setup.m_sourceAddress.GetSize(); i++) {
        PString alias = H323GetAliasAddressString(setup.m_sourceAddress[i]);
        if (alias != displayName && alias != remoteHostName) {
          if (needComma)
            aliases << ", ";
          else if (needParen)
            aliases << " (";
          aliases << alias;
          needComma = TRUE;
        }
      }
      if (needParen && needComma)
        aliases << ')';
    }
  }

  if (aliases.IsEmpty())
    return remoteHostName;

  aliases << " [" << remoteHostName << ']';
  aliases.MakeMinimumSize();
  return aliases;
}


PString H323SignalPDU::GetDestinationAlias(BOOL firstAliasOnly) const
{
  PStringStream aliases;

  PString number;
  if (GetQ931().GetCalledPartyNumber(number)) {
    if (firstAliasOnly)
      return number;
    aliases << number;
  }

  if (m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_setup) {
    const H225_Setup_UUIE & setup = m_h323_uu_pdu.m_h323_message_body;
    if (setup.m_destinationAddress.GetSize() > 0) {
      if (firstAliasOnly)
        return H323GetAliasAddressString(setup.m_destinationAddress[0]);

      for (PINDEX i = 0; i < setup.m_destinationAddress.GetSize(); i++) {
        if (!aliases.IsEmpty())
          aliases << '\t';
        aliases << H323GetAliasAddressString(setup.m_destinationAddress[i]);
      }
    }

    if (setup.HasOptionalField(H225_Setup_UUIE::e_destCallSignalAddress)) {
      if (!aliases.IsEmpty())
        aliases << '\t';
      aliases << H323TransportAddress(setup.m_destCallSignalAddress);
    }
  }

  aliases.MakeMinimumSize();
  return aliases;
}


BOOL H323SignalPDU::GetSourceE164(PString & number) const
{
  if (GetQ931().GetCallingPartyNumber(number))
    return TRUE;

  if (m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_setup)
    return FALSE;

  const H225_Setup_UUIE & setup = m_h323_uu_pdu.m_h323_message_body;
  if (!setup.HasOptionalField(H225_Setup_UUIE::e_sourceAddress))
    return FALSE;

  PINDEX i;
  for (i = 0; i < setup.m_sourceAddress.GetSize(); i++) {
    if (setup.m_sourceAddress[i].GetTag() == H225_AliasAddress::e_dialedDigits) {
      number = (PASN_IA5String &)setup.m_sourceAddress[i];
      return TRUE;
    }
  }

  for (i = 0; i < setup.m_sourceAddress.GetSize(); i++) {
    PString str = H323GetAliasAddressString(setup.m_sourceAddress[i]);
    if (IsE164(str)) {
      number = str;
      return TRUE;
    }
  }

  return FALSE;
}


BOOL H323SignalPDU::GetDestinationE164(PString & number) const
{
  if (GetQ931().GetCalledPartyNumber(number))
    return TRUE;

  if (m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_setup)
    return FALSE;

  const H225_Setup_UUIE & setup = m_h323_uu_pdu.m_h323_message_body;
  if (!setup.HasOptionalField(H225_Setup_UUIE::e_destinationAddress))
    return FALSE;

  PINDEX i;
  for (i = 0; i < setup.m_destinationAddress.GetSize(); i++) {
    if (setup.m_destinationAddress[i].GetTag() == H225_AliasAddress::e_dialedDigits) {
      number = (PASN_IA5String &)setup.m_destinationAddress[i];
      return TRUE;
    }
  }

  for (i = 0; i < setup.m_destinationAddress.GetSize(); i++) {
    PString str = H323GetAliasAddressString(setup.m_destinationAddress[i]);
    if (IsE164(str)) {
      number = str;
      return TRUE;
    }
  }

  return FALSE;
}


unsigned H323SignalPDU::GetDistinctiveRing() const
{
  Q931::SignalInfo sig = GetQ931().GetSignalInfo();
  if (sig < Q931::SignalAlertingPattern0 || sig > Q931::SignalAlertingPattern7)
    return 0;

  return sig - Q931::SignalAlertingPattern0;
}


void H323SignalPDU::SetQ931Fields(const H323Connection & connection,
                                  BOOL insertPartyNumbers,
                                  unsigned plan,
                                  unsigned type,
                                  int presentation,
                                  int screening)
{
  PINDEX i;
  const PStringList & aliases = connection.GetLocalAliasNames();

  PString number;
  PString localName   = connection.GetLocalPartyName();
  PString displayName;

  if (IsE164(localName)) {
    number = localName;
    for (i = 0; i < aliases.GetSize(); i++) {
      if (!IsE164(aliases[i])) {
        displayName = aliases[i];
        break;
      }
    }
  }
  else {
    if (!localName)
      displayName = localName;
    for (i = 0; i < aliases.GetSize(); i++) {
      if (IsE164(aliases[i])) {
        number = aliases[i];
        break;
      }
    }
  }

  if (!connection.GetDisplayName().IsEmpty())
    displayName = connection.GetDisplayName();
  if (displayName.IsEmpty())
    displayName = number;
  q931pdu.SetDisplayName(displayName);

  if (insertPartyNumbers) {
    PString otherNumber = connection.GetRemotePartyNumber();
    if (otherNumber.IsEmpty()) {
      PString otherName = connection.GetRemotePartyName();
      if (IsE164(otherName))
        otherNumber = otherName;
    }

    if (connection.HadAnsweredCall()) {
      if (!number)
        q931pdu.SetCalledPartyNumber(number, plan, type);
      if (!otherNumber)
        q931pdu.SetCallingPartyNumber(otherNumber, plan, type, presentation, screening);
    }
    else {
      if (!number)
        q931pdu.SetCallingPartyNumber(number, plan, type, presentation, screening);
      if (!otherNumber)
        q931pdu.SetCalledPartyNumber(otherNumber, plan, type);
    }
  }

  unsigned ring = connection.GetDistinctiveRing();
  if (ring != 0)
    q931pdu.SetSignalInfo((Q931::SignalInfo)(ring + Q931::SignalAlertingPattern0));
}


/////////////////////////////////////////////////////////////////////////////

H245_RequestMessage & H323ControlPDU::Build(H245_RequestMessage::Choices request)
{
  SetTag(e_request);
  H245_RequestMessage & msg = *this;
  msg.SetTag(request);
  return msg;
}


H245_ResponseMessage & H323ControlPDU::Build(H245_ResponseMessage::Choices response)
{
  SetTag(e_response);
  H245_ResponseMessage & resp = *this;
  resp.SetTag(response);
  return resp;
}


H245_CommandMessage & H323ControlPDU::Build(H245_CommandMessage::Choices command)
{
  SetTag(e_command);
  H245_CommandMessage & cmd = *this;
  cmd.SetTag(command);
  return cmd;
}


H245_IndicationMessage & H323ControlPDU::Build(H245_IndicationMessage::Choices indication)
{
  SetTag(e_indication);
  H245_IndicationMessage & ind = *this;
  ind.SetTag(indication);
  return ind;
}


H245_MasterSlaveDetermination & 
      H323ControlPDU::BuildMasterSlaveDetermination(unsigned terminalType,
                                                    unsigned statusDeterminationNumber)
{
  H245_MasterSlaveDetermination & msd = Build(H245_RequestMessage::e_masterSlaveDetermination);
  msd.m_terminalType = terminalType;
  msd.m_statusDeterminationNumber = statusDeterminationNumber;
  return msd;
}


H245_MasterSlaveDeterminationAck &
      H323ControlPDU::BuildMasterSlaveDeterminationAck(BOOL isMaster)
{
  H245_MasterSlaveDeterminationAck & msda = Build(H245_ResponseMessage::e_masterSlaveDeterminationAck);
  msda.m_decision.SetTag(isMaster
                            ? H245_MasterSlaveDeterminationAck_decision::e_slave
                            : H245_MasterSlaveDeterminationAck_decision::e_master);
  return msda;
}


H245_MasterSlaveDeterminationReject &
      H323ControlPDU::BuildMasterSlaveDeterminationReject(unsigned cause)
{
  H245_MasterSlaveDeterminationReject & msdr = Build(H245_ResponseMessage::e_masterSlaveDeterminationReject);
  msdr.m_cause.SetTag(cause);
  return msdr;
}


H245_TerminalCapabilitySet &
      H323ControlPDU::BuildTerminalCapabilitySet(const H323Connection & connection,
                                                 unsigned sequenceNumber,
                                                 BOOL empty)
{
  H245_TerminalCapabilitySet & cap = Build(H245_RequestMessage::e_terminalCapabilitySet);

  cap.m_sequenceNumber = sequenceNumber;
  cap.m_protocolIdentifier.SetValue(H245_ProtocolID, PARRAYSIZE(H245_ProtocolID));

  if (empty)
    return cap;

  cap.IncludeOptionalField(H245_TerminalCapabilitySet::e_multiplexCapability);
  cap.m_multiplexCapability.SetTag(H245_MultiplexCapability::e_h2250Capability);
  H245_H2250Capability & h225_0 = cap.m_multiplexCapability;
  h225_0.m_maximumAudioDelayJitter = connection.GetMaxAudioJitterDelay();
  h225_0.m_receiveMultipointCapability.m_mediaDistributionCapability.SetSize(1);
  h225_0.m_transmitMultipointCapability.m_mediaDistributionCapability.SetSize(1);
  h225_0.m_receiveAndTransmitMultipointCapability.m_mediaDistributionCapability.SetSize(1);
  h225_0.m_t120DynamicPortCapability = TRUE;

  // Set the table of capabilities
  connection.GetLocalCapabilities().BuildPDU(connection, cap);

  return cap;
}


H245_TerminalCapabilitySetAck &
      H323ControlPDU::BuildTerminalCapabilitySetAck(unsigned sequenceNumber)
{
  H245_TerminalCapabilitySetAck & cap = Build(H245_ResponseMessage::e_terminalCapabilitySetAck);
  cap.m_sequenceNumber = sequenceNumber;
  return cap;
}


H245_TerminalCapabilitySetReject &
      H323ControlPDU::BuildTerminalCapabilitySetReject(unsigned sequenceNumber,
                                                       unsigned cause)
{
  H245_TerminalCapabilitySetReject & cap = Build(H245_ResponseMessage::e_terminalCapabilitySetReject);
  cap.m_sequenceNumber = sequenceNumber;
  cap.m_cause.SetTag(cause);

  return cap;
}


H245_OpenLogicalChannel &
      H323ControlPDU::BuildOpenLogicalChannel(unsigned forwardLogicalChannelNumber)
{
  H245_OpenLogicalChannel & open = Build(H245_RequestMessage::e_openLogicalChannel);
  open.m_forwardLogicalChannelNumber = forwardLogicalChannelNumber;
  return open;
}


H245_RequestChannelClose &
      H323ControlPDU::BuildRequestChannelClose(unsigned channelNumber,
                                               unsigned reason)
{
  H245_RequestChannelClose & rcc = Build(H245_RequestMessage::e_requestChannelClose);
  rcc.m_forwardLogicalChannelNumber = channelNumber;
  rcc.IncludeOptionalField(H245_RequestChannelClose::e_reason);
  rcc.m_reason.SetTag(reason);
  return rcc;
}


H245_CloseLogicalChannel &
      H323ControlPDU::BuildCloseLogicalChannel(unsigned channelNumber)
{
  H245_CloseLogicalChannel & clc = Build(H245_RequestMessage::e_closeLogicalChannel);
  clc.m_forwardLogicalChannelNumber = channelNumber;
  clc.m_source.SetTag(H245_CloseLogicalChannel_source::e_lcse);
  return clc;
}


H245_OpenLogicalChannelAck &
      H323ControlPDU::BuildOpenLogicalChannelAck(unsigned channelNumber)
{
  H245_OpenLogicalChannelAck & ack = Build(H245_ResponseMessage::e_openLogicalChannelAck);
  ack.m_forwardLogicalChannelNumber = channelNumber;
  return ack;
}


H245_OpenLogicalChannelReject &
      H323ControlPDU::BuildOpenLogicalChannelReject(unsigned channelNumber,
                                                    unsigned cause)
{
  H245_OpenLogicalChannelReject & reject = Build(H245_ResponseMessage::e_openLogicalChannelReject);
  reject.m_forwardLogicalChannelNumber = channelNumber;
  reject.m_cause.SetTag(cause);
  return reject;
}


H245_OpenLogicalChannelConfirm &
      H323ControlPDU::BuildOpenLogicalChannelConfirm(unsigned channelNumber)
{
  H245_OpenLogicalChannelConfirm & chan = Build(H245_IndicationMessage::e_openLogicalChannelConfirm);
  chan.m_forwardLogicalChannelNumber = channelNumber;
  return chan;
}


H245_CloseLogicalChannelAck &
      H323ControlPDU::BuildCloseLogicalChannelAck(unsigned channelNumber)
{
  H245_CloseLogicalChannelAck & chan = Build(H245_ResponseMessage::e_closeLogicalChannelAck);
  chan.m_forwardLogicalChannelNumber = channelNumber;
  return chan;
}


H245_RequestChannelCloseAck &
      H323ControlPDU::BuildRequestChannelCloseAck(unsigned channelNumber)
{
  H245_RequestChannelCloseAck & rcca = Build(H245_ResponseMessage::e_requestChannelCloseAck);
  rcca.m_forwardLogicalChannelNumber = channelNumber;
  return rcca;
}


H245_RequestChannelCloseReject &
      H323ControlPDU::BuildRequestChannelCloseReject(unsigned channelNumber)
{
  H245_RequestChannelCloseReject & rccr = Build(H245_ResponseMessage::e_requestChannelCloseReject);
  rccr.m_forwardLogicalChannelNumber = channelNumber;
  return rccr;
}


H245_RequestChannelCloseRelease &
      H323ControlPDU::BuildRequestChannelCloseRelease(unsigned channelNumber)
{
  H245_RequestChannelCloseRelease & rccr = Build(H245_IndicationMessage::e_requestChannelCloseRelease);
  rccr.m_forwardLogicalChannelNumber = channelNumber;
  return rccr;
}


H245_RequestMode & H323ControlPDU::BuildRequestMode(unsigned sequenceNumber)
{
  H245_RequestMode & rm = Build(H245_RequestMessage::e_requestMode);
  rm.m_sequenceNumber = sequenceNumber;

  return rm;
}


H245_RequestModeAck & H323ControlPDU::BuildRequestModeAck(unsigned sequenceNumber,
                                                          unsigned response)
{
  H245_RequestModeAck & ack = Build(H245_ResponseMessage::e_requestModeAck);
  ack.m_sequenceNumber = sequenceNumber;
  ack.m_response.SetTag(response);
  return ack;
}


H245_RequestModeReject & H323ControlPDU::BuildRequestModeReject(unsigned sequenceNumber,
                                                                unsigned cause)
{
  H245_RequestModeReject & reject = Build(H245_ResponseMessage::e_requestModeReject);
  reject.m_sequenceNumber = sequenceNumber;
  reject.m_cause.SetTag(cause);
  return reject;
}


H245_RoundTripDelayRequest &
      H323ControlPDU::BuildRoundTripDelayRequest(unsigned sequenceNumber)
{
  H245_RoundTripDelayRequest & req = Build(H245_RequestMessage::e_roundTripDelayRequest);
  req.m_sequenceNumber = sequenceNumber;
  return req;
}


H245_RoundTripDelayResponse &
      H323ControlPDU::BuildRoundTripDelayResponse(unsigned sequenceNumber)
{
  H245_RoundTripDelayResponse & resp = Build(H245_ResponseMessage::e_roundTripDelayResponse);
  resp.m_sequenceNumber = sequenceNumber;
  return resp;
}


H245_UserInputIndication &
      H323ControlPDU::BuildUserInputIndication(const PString & value)
{
  H245_UserInputIndication & ind = Build(H245_IndicationMessage::e_userInput);
  ind.SetTag(H245_UserInputIndication::e_alphanumeric);
  (PASN_GeneralString &)ind = value;
  return ind;
}


H245_UserInputIndication & H323ControlPDU::BuildUserInputIndication(char tone,
                                                                    unsigned duration,
                                                                    unsigned logicalChannel,
                                                                    unsigned rtpTimestamp)
{
  H245_UserInputIndication & ind = Build(H245_IndicationMessage::e_userInput);

  if (tone != ' ') {
    ind.SetTag(H245_UserInputIndication::e_signal);
    H245_UserInputIndication_signal & sig = ind;

    sig.m_signalType.SetValue(tone);

    if (duration > 0) {
      sig.IncludeOptionalField(H245_UserInputIndication_signal::e_duration);
      sig.m_duration = duration;
    }

    if (logicalChannel > 0) {
      sig.IncludeOptionalField(H245_UserInputIndication_signal::e_rtp);
      sig.m_rtp.m_logicalChannelNumber = logicalChannel;
      sig.m_rtp.m_timestamp = rtpTimestamp;
    }
  }
  else {
    ind.SetTag(H245_UserInputIndication::e_signalUpdate);
    H245_UserInputIndication_signalUpdate & sig = ind;

    sig.m_duration = duration;
    if (logicalChannel > 0) {
      sig.IncludeOptionalField(H245_UserInputIndication_signalUpdate::e_rtp);
      sig.m_rtp.m_logicalChannelNumber = logicalChannel;
    }
  }

  return ind;
}


H245_FunctionNotUnderstood &
      H323ControlPDU::BuildFunctionNotUnderstood(const H323ControlPDU & pdu)
{
  H245_FunctionNotUnderstood & fnu = Build(H245_IndicationMessage::e_functionNotUnderstood);

  switch (pdu.GetTag()) {
    case H245_MultimediaSystemControlMessage::e_request :
      fnu.SetTag(H245_FunctionNotUnderstood::e_request);
      (H245_RequestMessage &)fnu = (const H245_RequestMessage &)pdu;
      break;

    case H245_MultimediaSystemControlMessage::e_response :
      fnu.SetTag(H245_FunctionNotUnderstood::e_response);
      (H245_ResponseMessage &)fnu = (const H245_ResponseMessage &)pdu;
      break;

    case H245_MultimediaSystemControlMessage::e_command :
      fnu.SetTag(H245_FunctionNotUnderstood::e_command);
      (H245_CommandMessage &)fnu = (const H245_CommandMessage &)pdu;
      break;
  }

  return fnu;
}


H245_EndSessionCommand & H323ControlPDU::BuildEndSessionCommand(unsigned reason)
{
  H245_EndSessionCommand & end = Build(H245_CommandMessage::e_endSessionCommand);
  end.SetTag(reason);
  return end;
}


/////////////////////////////////////////////////////////////////////////////

H323RasPDU::H323RasPDU()
{
}


H323RasPDU::H323RasPDU(const H235Authenticators & auth)
  : H323TransactionPDU(auth)
{
}


PObject * H323RasPDU::Clone() const
{
  return new H323RasPDU(*this);
}


PASN_Object & H323RasPDU::GetPDU()
{
  return *this;
}


PASN_Choice & H323RasPDU::GetChoice()
{
  return *this;
}


const PASN_Object & H323RasPDU::GetPDU() const
{
  return *this;
}


const PASN_Choice & H323RasPDU::GetChoice() const
{
  return *this;
}


unsigned H323RasPDU::GetSequenceNumber() const
{
  switch (GetTag()) {
    case H225_RasMessage::e_gatekeeperRequest :
      return ((const H225_GatekeeperRequest &)*this).m_requestSeqNum;

    case H225_RasMessage::e_gatekeeperConfirm :
      return ((const H225_GatekeeperConfirm &)*this).m_requestSeqNum;

    case H225_RasMessage::e_gatekeeperReject :
      return ((const H225_GatekeeperReject &)*this).m_requestSeqNum;

    case H225_RasMessage::e_registrationRequest :
      return ((const H225_RegistrationRequest &)*this).m_requestSeqNum;

    case H225_RasMessage::e_registrationConfirm :
      return ((const H225_RegistrationConfirm &)*this).m_requestSeqNum;

    case H225_RasMessage::e_registrationReject :
      return ((const H225_RegistrationReject &)*this).m_requestSeqNum;

    case H225_RasMessage::e_unregistrationRequest :
      return ((const H225_UnregistrationRequest &)*this).m_requestSeqNum;

    case H225_RasMessage::e_unregistrationConfirm :
      return ((const H225_UnregistrationConfirm &)*this).m_requestSeqNum;

    case H225_RasMessage::e_unregistrationReject :
      return ((const H225_UnregistrationReject &)*this).m_requestSeqNum;

    case H225_RasMessage::e_admissionRequest :
      return ((const H225_AdmissionRequest &)*this).m_requestSeqNum;

    case H225_RasMessage::e_admissionConfirm :
      return ((const H225_AdmissionConfirm &)*this).m_requestSeqNum;

    case H225_RasMessage::e_admissionReject :
      return ((const H225_AdmissionReject &)*this).m_requestSeqNum;

    case H225_RasMessage::e_bandwidthRequest :
      return ((const H225_BandwidthRequest &)*this).m_requestSeqNum;

    case H225_RasMessage::e_bandwidthConfirm :
      return ((const H225_BandwidthConfirm &)*this).m_requestSeqNum;

    case H225_RasMessage::e_bandwidthReject :
      return ((const H225_BandwidthReject &)*this).m_requestSeqNum;

    case H225_RasMessage::e_disengageRequest :
      return ((const H225_DisengageRequest &)*this).m_requestSeqNum;

    case H225_RasMessage::e_disengageConfirm :
      return ((const H225_DisengageConfirm &)*this).m_requestSeqNum;

    case H225_RasMessage::e_disengageReject :
      return ((const H225_DisengageReject &)*this).m_requestSeqNum;

    case H225_RasMessage::e_locationRequest :
      return ((const H225_LocationRequest &)*this).m_requestSeqNum;

    case H225_RasMessage::e_locationConfirm :
      return ((const H225_LocationConfirm &)*this).m_requestSeqNum;

    case H225_RasMessage::e_locationReject :
      return ((const H225_LocationReject &)*this).m_requestSeqNum;

    case H225_RasMessage::e_infoRequest :
      return ((const H225_InfoRequest &)*this).m_requestSeqNum;

    case H225_RasMessage::e_infoRequestResponse :
      return ((const H225_InfoRequestResponse &)*this).m_requestSeqNum;

    case H225_RasMessage::e_nonStandardMessage :
      return ((const H225_NonStandardMessage &)*this).m_requestSeqNum;

    case H225_RasMessage::e_unknownMessageResponse :
      return ((const H225_UnknownMessageResponse &)*this).m_requestSeqNum;

    case H225_RasMessage::e_requestInProgress :
      return ((const H225_RequestInProgress &)*this).m_requestSeqNum;

    case H225_RasMessage::e_resourcesAvailableIndicate :
      return ((const H225_ResourcesAvailableIndicate &)*this).m_requestSeqNum;

    case H225_RasMessage::e_resourcesAvailableConfirm :
      return ((const H225_ResourcesAvailableConfirm &)*this).m_requestSeqNum;

    case H225_RasMessage::e_infoRequestAck :
      return ((const H225_InfoRequestAck &)*this).m_requestSeqNum;

    case H225_RasMessage::e_infoRequestNak :
      return ((const H225_InfoRequestNak &)*this).m_requestSeqNum;

    case H225_RasMessage::e_serviceControlIndication :
      return ((const H225_ServiceControlIndication &)*this).m_requestSeqNum;

    case H225_RasMessage::e_serviceControlResponse :
      return ((const H225_ServiceControlResponse &)*this).m_requestSeqNum;

    default :
      return 0;
  }
}


unsigned H323RasPDU::GetRequestInProgressDelay() const
{
  if (GetTag() != H225_RasMessage::e_requestInProgress)
    return 0;

  return ((const H225_RequestInProgress &)*this).m_delay;
}


#if PTRACING
const char * H323RasPDU::GetProtocolName() const
{
  return "H225RAS";
}
#endif


H323TransactionPDU * H323RasPDU::ClonePDU() const
{
  return new H323RasPDU(*this);
}


void H323RasPDU::DeletePDU()
{
  delete this;
}


H225_GatekeeperRequest & H323RasPDU::BuildGatekeeperRequest(unsigned seqNum)
{
  SetTag(e_gatekeeperRequest);
  H225_GatekeeperRequest & grq = *this;
  grq.m_requestSeqNum = seqNum;
  grq.m_protocolIdentifier.SetValue(H225_ProtocolID, PARRAYSIZE(H225_ProtocolID));
  return grq;
}


H225_GatekeeperConfirm & H323RasPDU::BuildGatekeeperConfirm(unsigned seqNum)
{
  SetTag(e_gatekeeperConfirm);
  H225_GatekeeperConfirm & gcf = *this;
  gcf.m_requestSeqNum = seqNum;
  gcf.m_protocolIdentifier.SetValue(H225_ProtocolID, PARRAYSIZE(H225_ProtocolID));
  return gcf;
}


H225_GatekeeperReject & H323RasPDU::BuildGatekeeperReject(unsigned seqNum, unsigned reason)
{
  SetTag(e_gatekeeperReject);
  H225_GatekeeperReject & grj = *this;
  grj.m_requestSeqNum = seqNum;
  grj.m_protocolIdentifier.SetValue(H225_ProtocolID, PARRAYSIZE(H225_ProtocolID));
  grj.m_rejectReason.SetTag(reason);
  return grj;
}


H225_RegistrationRequest & H323RasPDU::BuildRegistrationRequest(unsigned seqNum)
{
  SetTag(e_registrationRequest);
  H225_RegistrationRequest & rrq = *this;
  rrq.m_requestSeqNum = seqNum;
  rrq.m_protocolIdentifier.SetValue(H225_ProtocolID, PARRAYSIZE(H225_ProtocolID));
  return rrq;
}


H225_RegistrationConfirm & H323RasPDU::BuildRegistrationConfirm(unsigned seqNum)
{
  SetTag(e_registrationConfirm);
  H225_RegistrationConfirm & rcf = *this;
  rcf.m_requestSeqNum = seqNum;
  rcf.m_protocolIdentifier.SetValue(H225_ProtocolID, PARRAYSIZE(H225_ProtocolID));
  return rcf;
}


H225_RegistrationReject & H323RasPDU::BuildRegistrationReject(unsigned seqNum, unsigned reason)
{
  SetTag(e_registrationReject);
  H225_RegistrationReject & rrj = *this;
  rrj.m_requestSeqNum = seqNum;
  rrj.m_protocolIdentifier.SetValue(H225_ProtocolID, PARRAYSIZE(H225_ProtocolID));
  rrj.m_rejectReason.SetTag(reason);
  return rrj;
}


H225_UnregistrationRequest & H323RasPDU::BuildUnregistrationRequest(unsigned seqNum)
{
  SetTag(e_unregistrationRequest);
  H225_UnregistrationRequest & urq = *this;
  urq.m_requestSeqNum = seqNum;
  return urq;
}


H225_UnregistrationConfirm & H323RasPDU::BuildUnregistrationConfirm(unsigned seqNum)
{
  SetTag(e_unregistrationConfirm);
  H225_UnregistrationConfirm & ucf = *this;
  ucf.m_requestSeqNum = seqNum;
  return ucf;
}


H225_UnregistrationReject & H323RasPDU::BuildUnregistrationReject(unsigned seqNum, unsigned reason)
{
  SetTag(e_unregistrationReject);
  H225_UnregistrationReject & urj = *this;
  urj.m_requestSeqNum = seqNum;
  urj.m_rejectReason.SetTag(reason);
  return urj;
}


H225_LocationRequest & H323RasPDU::BuildLocationRequest(unsigned seqNum)
{
  SetTag(e_locationRequest);
  H225_LocationRequest & lrq = *this;
  lrq.m_requestSeqNum = seqNum;
  return lrq;
}


H225_LocationConfirm & H323RasPDU::BuildLocationConfirm(unsigned seqNum)
{
  SetTag(e_locationConfirm);
  H225_LocationConfirm & lcf = *this;
  lcf.m_requestSeqNum = seqNum;
  return lcf;
}


H225_LocationReject & H323RasPDU::BuildLocationReject(unsigned seqNum, unsigned reason)
{
  SetTag(e_locationReject);
  H225_LocationReject & lrj = *this;
  lrj.m_requestSeqNum = seqNum;
  lrj.m_rejectReason.SetTag(reason);
  return lrj;
}


H225_AdmissionRequest & H323RasPDU::BuildAdmissionRequest(unsigned seqNum)
{
  SetTag(e_admissionRequest);
  H225_AdmissionRequest & arq = *this;
  arq.m_requestSeqNum = seqNum;
  return arq;
}


H225_AdmissionConfirm & H323RasPDU::BuildAdmissionConfirm(unsigned seqNum)
{
  SetTag(e_admissionConfirm);
  H225_AdmissionConfirm & acf = *this;
  acf.m_requestSeqNum = seqNum;
  return acf;
}


H225_AdmissionReject & H323RasPDU::BuildAdmissionReject(unsigned seqNum, unsigned reason)
{
  SetTag(e_admissionReject);
  H225_AdmissionReject & arj = *this;
  arj.m_requestSeqNum = seqNum;
  arj.m_rejectReason.SetTag(reason);
  return arj;
}


H225_DisengageRequest & H323RasPDU::BuildDisengageRequest(unsigned seqNum)
{
  SetTag(e_disengageRequest);
  H225_DisengageRequest & drq = *this;
  drq.m_requestSeqNum = seqNum;
  return drq;
}


H225_DisengageConfirm & H323RasPDU::BuildDisengageConfirm(unsigned seqNum)
{
  SetTag(e_disengageConfirm);
  H225_DisengageConfirm & dcf = *this;
  dcf.m_requestSeqNum = seqNum;
  return dcf;
}


H225_DisengageReject & H323RasPDU::BuildDisengageReject(unsigned seqNum, unsigned reason)
{
  SetTag(e_disengageReject);
  H225_DisengageReject & drj = *this;
  drj.m_requestSeqNum = seqNum;
  drj.m_rejectReason.SetTag(reason);
  return drj;
}


H225_BandwidthRequest & H323RasPDU::BuildBandwidthRequest(unsigned seqNum)
{
  SetTag(e_bandwidthRequest);
  H225_BandwidthRequest & brq = *this;
  brq.m_requestSeqNum = seqNum;
  return brq;
}


H225_BandwidthConfirm & H323RasPDU::BuildBandwidthConfirm(unsigned seqNum, unsigned bandWidth)
{
  SetTag(e_bandwidthConfirm);
  H225_BandwidthConfirm & bcf = *this;
  bcf.m_requestSeqNum = seqNum;
  bcf.m_bandWidth = bandWidth;
  return bcf;
}


H225_BandwidthReject & H323RasPDU::BuildBandwidthReject(unsigned seqNum, unsigned reason)
{
  SetTag(e_bandwidthReject);
  H225_BandwidthReject & brj = *this;
  brj.m_requestSeqNum = seqNum;
  brj.m_rejectReason.SetTag(reason);
  return brj;
}


H225_InfoRequest & H323RasPDU::BuildInfoRequest(unsigned seqNum,
                                                unsigned callRef,
                                                const OpalGloballyUniqueID * id)
{
  SetTag(e_infoRequest);
  H225_InfoRequest & irq = *this;
  irq.m_requestSeqNum = seqNum;
  irq.m_callReferenceValue = callRef;
  if (callRef != 0 && id != NULL)
    irq.m_callIdentifier.m_guid = *id;
  return irq;
}


H225_InfoRequestResponse & H323RasPDU::BuildInfoRequestResponse(unsigned seqNum)
{
  SetTag(e_infoRequestResponse);
  H225_InfoRequestResponse & irr = *this;
  irr.m_requestSeqNum = seqNum;
  return irr;
}


H225_InfoRequestAck & H323RasPDU::BuildInfoRequestAck(unsigned seqNum)
{
  SetTag(e_infoRequestAck);
  H225_InfoRequestAck & iack = *this;
  iack.m_requestSeqNum = seqNum;
  return iack;
}


H225_InfoRequestNak & H323RasPDU::BuildInfoRequestNak(unsigned seqNum, unsigned reason)
{
  SetTag(e_infoRequestNak);
  H225_InfoRequestNak & inak = *this;
  inak.m_requestSeqNum = seqNum;
  inak.m_nakReason.SetTag(reason);
  return inak;
}


H225_UnknownMessageResponse & H323RasPDU::BuildUnknownMessageResponse(unsigned seqNum)
{
  SetTag(e_unknownMessageResponse);
  H225_UnknownMessageResponse & umr = *this;
  umr.m_requestSeqNum = seqNum;
  return umr;
}


H225_RequestInProgress & H323RasPDU::BuildRequestInProgress(unsigned seqNum, unsigned delay)
{
  SetTag(e_requestInProgress);
  H225_RequestInProgress & rip = *this;
  rip.m_requestSeqNum = seqNum;
  rip.m_delay = delay;
  return rip;
}


H225_ServiceControlIndication & H323RasPDU::BuildServiceControlIndication(unsigned seqNum, const OpalGloballyUniqueID * id)
{
  SetTag(e_serviceControlIndication);
  H225_ServiceControlIndication & sci = *this;
  sci.m_requestSeqNum = seqNum;

  if (id != NULL && !id->IsNULL()) {
    sci.IncludeOptionalField(H225_ServiceControlIndication::e_callSpecific);
    sci.m_callSpecific.m_callIdentifier.m_guid = *id;
  }

  return sci;
}


H225_ServiceControlResponse & H323RasPDU::BuildServiceControlResponse(unsigned seqNum)
{
  SetTag(e_serviceControlResponse);
  H225_ServiceControlResponse & scr = *this;
  scr.m_requestSeqNum = seqNum;
  return scr;
}

#ifdef _MSC_VER
#pragma warning(default : 4100)
#endif

/////////////////////////////////////////////////////////////////////////////
