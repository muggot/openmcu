/*
 * h323con.h
 *
 * H.323 protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2001 Equivalence Pty. Ltd.
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
 * $Log: h323con.h,v $
 * Revision 1.13  2008/02/06 02:52:58  shorne
 * Added support for Standards based NAT Traversal
 *
 * Revision 1.12  2008/01/30 18:51:35  shorne
 * fix for duplicate h224handler definition
 *
 * Revision 1.11  2008/01/04 06:23:07  shorne
 * Cleaner setup and teardown of h460 module
 *
 * Revision 1.10  2008/01/01 00:16:12  shorne
 * Added GnuGknat and FileTransfer support
 *
 * Revision 1.9  2007/11/29 14:19:42  willamowius
 * use seionID to test session type when doig capability merge
 *
 * Revision 1.8  2007/11/28 15:30:38  willamowius
 * fix capability type detection for merging
 *
 * Revision 1.7  2007/11/28 06:03:36  shorne
 * Video capability merge. Thx again Jan Willamowius
 *
 * Revision 1.6  2007/11/16 22:09:42  shorne
 * Added ability to disable H.245 QoS for NetMeeting Interop
 *
 * Revision 1.5  2007/11/01 20:17:30  shorne
 * updates for H.239 support
 *
 * Revision 1.4  2007/10/16 17:01:33  shorne
 * Various little fixes
 *
 * Revision 1.3  2007/08/20 19:13:27  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.2  2007/08/08 17:24:58  willamowius
 * fix Linux compile error (gcc 4.1.2)
 *
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.101.2.12  2007/08/02 20:08:10  shorne
 * added virtual to GetPartyName
 *
 * Revision 1.101.2.11  2007/07/23 21:47:10  shorne
 * Added QoS GK Reporting
 *
 * Revision 1.101.2.10  2007/07/20 22:03:20  shorne
 * Initial H.350 Support
 *
 * Revision 1.101.2.9  2007/07/19 20:10:27  shorne
 * Changed HAS_AEC to H323_AEC
 *
 * Revision 1.101.2.8  2007/05/23 06:58:01  shorne
 * Nat Support for EP's nested behind same NAT
 *
 * Revision 1.101.2.7  2007/05/01 01:17:52  shorne
 * Fix for NAT support disablement for same NAT
 *
 * Revision 1.101.2.6  2007/03/24 23:39:42  shorne
 * More H.239 work
 *
 * Revision 1.101.2.5  2007/03/18 06:21:38  shorne
 * More tweaks with Multimedia OnHold
 *
 * Revision 1.101.2.4  2007/03/14 08:53:38  shorne
 * Added Video on Hold
 *
 * Revision 1.101.2.3  2007/02/18 17:11:23  shorne
 * Added H.249 Extended UserInput Support
 *
 * Revision 1.101.2.2  2007/02/11 00:45:20  shorne
 * Added ability to disable NAT method on a call by call basis
 *
 * Revision 1.101.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.101  2006/07/06 05:20:18  shorne
 * Added Feature CallEndReason
 *
 * Revision 1.100  2006/06/27 12:35:03  csoutheren
 * Patch 1366328 - Support for H.450.3 divertingLegInformation2
 * Thanks to Norbert Bartalsky
 *
 * Revision 1.99  2006/06/23 20:01:28  shorne
 * More H460 support
 *
 * Revision 1.98  2006/06/23 07:06:04  csoutheren
 * Fixed linux compile
 *
 * Revision 1.97  2006/06/23 06:02:44  csoutheren
 * Added missing declarations for H.224 backport
 *
 * Revision 1.96  2006/05/30 11:14:55  hfriederich
 * Switch from DISABLE_H460 to H323_H460
 *
 * Revision 1.95  2006/05/16 11:41:15  shorne
 * call credit, AEC and H460 support
 *
 * Revision 1.94  2006/04/07 10:24:20  rjongbloed
 * Fixed DevStudio compile errors
 *
 * Revision 1.93  2006/04/06 08:22:17  csoutheren
 * Added support for conference commands
 *
 * Revision 1.92  2006/03/02 07:52:49  csoutheren
 * Ensure prompt close of channels when using aggregation
 * Ensure MonitorCallStatus called when using aggregation
 *
 * Revision 1.91  2006/02/24 04:52:17  csoutheren
 * Fixd problem with H.245 aggregation
 *
 * Revision 1.90  2006/01/26 03:36:51  shorne
 * Add the ability to remove a local input device when placing a call on hold
 *
 * Revision 1.89  2006/01/24 08:15:24  csoutheren
 * Implement outgoing H.225 aggregation, and H.245 aggregation (disabled by default)
 * More testing to do, but this looks good :)
 *
 * Revision 1.88  2006/01/23 05:58:25  csoutheren
 * Working outgoing H.225 aggregation (disabled by default)
 *
 * Revision 1.87  2006/01/20 00:32:24  csoutheren
 * First check-in of signalling aggregation code - incomplete and disabled by default
 *
 * Revision 1.86  2006/01/18 07:46:08  csoutheren
 * Initial version of RTP aggregation (disabled by default)
 *
 * Revision 1.85  2005/12/09 05:20:07  csoutheren
 * Added ability to set explicit Q.931 cause code on call end
 *
 * Revision 1.84  2005/12/06 05:28:55  csoutheren
 * Changed AttachSignalChannel to be virtual
 *
 * Revision 1.83  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.82  2005/11/04 06:41:28  csoutheren
 * Changed PMutex to PTimedMutex
 *
 * Revision 1.81  2005/09/16 08:11:06  csoutheren
 * Added new error code
 * Added override for OnIncomingCall that can set return code
 *
 * Revision 1.80  2005/08/30 08:29:30  csoutheren
 * Added new error code
 *
 * Revision 1.79  2005/08/27 02:14:21  csoutheren
 * Capture time that reverse fast start acknowledge is received
 * Capture time that connect is sent/received
 *
 * Revision 1.78  2005/01/03 14:03:20  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.77  2005/01/03 06:25:52  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.76  2004/12/08 01:59:22  csoutheren
 * initial support for Transnexus OSP toolkit
 *
 * Revision 1.75  2004/11/22 11:30:42  rjongbloed
 * Added ability to restart H.245 negotiations, thanks Norbert Bartalsky
 *
 * Revision 1.74  2004/09/27 08:25:14  csoutheren
 * Changed in-header documentation for external headers to reflect changes
 * in external RTP interface
 *
 * Revision 1.73  2004/09/07 22:50:55  rjongbloed
 * Changed usage of template function as MSVC6 will not compile it.
 *
 * Revision 1.72  2004/09/03 01:06:09  csoutheren
 * Added initial hooks for H.460 GEF
 * Thanks to Simon Horne and ISVO (Asia) Pte Ltd. for this contribution
 *
 * Revision 1.71  2004/07/13 10:19:40  csoutheren
 * Fixed cut & paste errors in comments
 *
 * Revision 1.70  2004/06/15 03:30:00  csoutheren
 * Added OnSendARQ to allow access to the ARQ message before sent by connection
 *
 * Revision 1.69  2004/04/20 05:24:54  csoutheren
 * Added ability to specify Q.931 DisplayName
 *
 * Revision 1.68  2004/04/13 05:50:50  csoutheren
 * Added function to get access to the connection NAT flag
 *
 * Revision 1.67  2004/04/07 05:31:42  csoutheren
 * Added ability to receive calls from endpoints behind NAT firewalls
 *
 * Revision 1.66  2004/03/04 04:48:29  csoutheren
 * Added virtual keyword to allow overring of capability management functions
 * Added additional response code needed for MCU
 *
 * Revision 1.65  2004/02/26 08:32:47  csoutheren
 * Added release complete codes for MCU
 *
 * Revision 1.64  2003/12/28 02:38:14  csoutheren
 * Added H323EndPoint::OnOutgoingCall
 *
 * Revision 1.63  2003/12/14 10:42:29  rjongbloed
 * Changes for compilability without video support.
 *
 * Revision 1.62  2003/10/27 06:03:38  csoutheren
 * Added support for QoS
 *   Thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.61  2003/10/09 09:47:45  csoutheren
 * Fixed problem with re-opening RTP half-channels under unusual
 * circumstances. Thanks to Damien Sandras
 *
 * Revision 1.60  2003/04/30 00:28:50  robertj
 * Redesigned the alternate credentials in ARQ system as old implementation
 *   was fraught with concurrency issues, most importantly it can cause false
 *   detection of replay attacks taking out an endpoint completely.
 *
 * Revision 1.59  2003/02/12 23:59:22  robertj
 * Fixed adding missing endpoint identifer in SETUP packet when gatekeeper
 * routed, pointed out by Stefan Klein
 * Also fixed correct rutrn of gk routing in IRR packet.
 *
 * Revision 1.58  2002/11/27 06:54:52  robertj
 * Added Service Control Session management as per Annex K/H.323 via RAS
 *   only at this stage.
 * Added H.248 ASN and very primitive infrastructure for linking into the
 *   Service Control Session management system.
 * Added basic infrastructure for Annex K/H.323 HTTP transport system.
 * Added Call Credit Service Control to display account balances.
 *
 * Revision 1.57  2002/11/15 05:17:22  robertj
 * Added facility redirect support without changing the call token for access
 *   to the call. If it gets redirected a new H323Connection object is
 *   created but it looks like the same thing to an application.
 *
 * Revision 1.56  2002/11/13 04:37:23  robertj
 * Added ability to get (and set) Q.931 release complete cause codes.
 *
 * Revision 1.55  2002/11/10 06:17:26  robertj
 * Fixed minor documentation errors.
 *
 * Revision 1.54  2002/11/05 00:24:09  robertj
 * Added function to determine if Q.931 CONNECT sent/received.
 *
 * Revision 1.53  2002/10/31 00:31:47  robertj
 * Enhanced jitter buffer system so operates dynamically between minimum and
 *   maximum values. Altered API to assure app writers note the change!
 *
 * Revision 1.52  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.51  2002/09/03 06:19:36  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.50  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.49  2002/08/05 05:17:37  robertj
 * Fairly major modifications to support different authentication credentials
 *   in ARQ to the logged in ones on RRQ. For both client and server.
 * Various other H.235 authentication bugs and anomalies fixed on the way.
 *
 * Revision 1.48  2002/07/05 02:22:56  robertj
 * Added support for standard and non-standard T.38 mode change.
 *
 * Revision 1.47  2002/07/04 00:40:31  robertj
 * More H.450.11 call intrusion implementation, thanks Aleksandar Todorovic
 *
 * Revision 1.46  2002/06/22 06:11:30  robertj
 * Fixed bug on sometimes missing received endSession causing 10 second
 *   timeout in connection clean up.
 *
 * Revision 1.45  2002/06/22 05:48:38  robertj
 * Added partial implementation for H.450.11 Call Intrusion
 *
 * Revision 1.44  2002/06/13 06:15:19  robertj
 * Allowed TransferCall() to be used on H323Connection as well as H323EndPoint.
 *
 * Revision 1.43  2002/06/05 08:58:58  robertj
 * Fixed documentation of remote application name string.
 * Added missing virtual keywards on some protocol handler functions.
 *
 * Revision 1.42  2002/05/29 06:40:29  robertj
 * Changed sending of endSession/ReleaseComplete PDU's to occur immediately
 *   on call clearance and not wait for background thread to do it.
 * Stricter compliance by waiting for reply endSession before closing down.
 *
 * Revision 1.41  2002/05/29 03:55:17  robertj
 * Added protocol version number checking infrastructure, primarily to improve
 *   interoperability with stacks that are unforgiving of new features.
 *
 * Revision 1.40  2002/05/21 09:32:49  robertj
 * Added ability to set multiple alias names ona  connection by connection
 *   basis, defaults to endpoint list, thanks Artis Kugevics
 *
 * Revision 1.39  2002/05/15 23:59:33  robertj
 * Added memory management of created T.38 and T.120 handlers.
 * Improved documentation for use of T.38 and T.120 functions.
 * Added ability to initiate a mode change for non-standard T.38
 *
 * Revision 1.38  2002/05/07 01:31:51  dereks
 * Fix typo in documentation.
 *
 * Revision 1.37  2002/05/03 05:38:15  robertj
 * Added Q.931 Keypad IE mechanism for user indications (DTMF).
 *
 * Revision 1.36  2002/05/02 07:56:24  robertj
 * Added automatic clearing of call if no media (RTP data) is transferred in a
 *   configurable (default 5 minutes) amount of time.
 *
 * Revision 1.35  2002/04/25 20:55:25  dereks
 * Fix documentation. Thanks Olaf Schulz.
 *
 * Revision 1.34  2002/04/17 00:50:34  robertj
 * Added ability to disable the in band DTMF detection.
 *
 * Revision 1.33  2002/03/27 06:04:42  robertj
 * Added Temporary Failure end code for connection, an application may
 *   immediately retry the call if this occurs.
 *
 * Revision 1.32  2002/02/11 04:20:48  robertj
 * Fixed documentation errors, thanks Horacio J. Peña
 *
 * Revision 1.31  2002/02/11 04:16:37  robertj
 * Fixed bug where could send DRQ if never received an ACF.
 *
 * Revision 1.30  2002/02/06 06:30:47  craigs
 * Fixed problem whereby MSD/TCS was stalled if H245 was included in
 * SETUP, but other end did not respond
 *
 * Revision 1.29  2002/02/04 07:17:52  robertj
 * Added H.450.2 Consultation Transfer, thanks Norwood Systems.
 *
 * Revision 1.28  2002/01/25 05:20:05  robertj
 * Moved static strings for enum printing to inside of function, could crash with DLL's
 *
 * Revision 1.27  2002/01/24 06:29:02  robertj
 * Added option to disable H.245 negotiation in SETUP pdu, this required
 *   API change so have a bit mask instead of a series of booleans.
 *
 * Revision 1.26  2002/01/23 12:45:37  rogerh
 * Add the DTMF decoder. This identifies DTMF tones in an audio stream.
 *
 * Revision 1.25  2002/01/23 07:12:48  robertj
 * Added hooks for in band DTMF detection. Now need the detector!
 *
 * Revision 1.24  2002/01/22 22:48:21  robertj
 * Fixed RFC2833 support (transmitter) requiring large rewrite
 *
 * Revision 1.23  2002/01/18 06:02:08  robertj
 * Added some H323v4 functions (fastConnectRefused & TCS in SETUP)
 *
 * Revision 1.22  2002/01/17 07:04:58  robertj
 * Added support for RFC2833 embedded DTMF in the RTP stream.
 *
 * Revision 1.21  2002/01/14 00:05:24  robertj
 * Added H.450.6, better H.450.2 error handling and  and Music On Hold.
 * Added destExtraCallInfo field for ARQ.
 *   Thanks Ben Madsen of Norwood Systems
 *
 * Revision 1.20  2002/01/10 05:13:50  robertj
 * Added support for external RTP stacks, thanks NuMind Software Systems.
 *
 * Revision 1.19  2002/01/09 00:21:36  robertj
 * Changes to support outgoing H.245 RequstModeChange.
 *
 * Revision 1.18  2001/12/22 03:20:31  robertj
 * Added create protocol function to H323Connection.
 *
 * Revision 1.17  2001/12/22 03:09:36  robertj
 * Changed OnRequstModeChange to return ack, then actually do the change.
 *
 * Revision 1.16  2001/12/22 01:52:54  robertj
 * Added more support for H.245 RequestMode operation.
 *
 * Revision 1.15  2001/12/15 08:09:54  robertj
 * Added alerting, connect and end of call times to be sent to RAS server.
 *
 * Revision 1.14  2001/12/13 10:54:23  robertj
 * Added ability to automatically add ACF access token to SETUP pdu.
 *
 * Revision 1.13  2001/11/01 06:11:54  robertj
 * Plugged very small mutex hole that could cause crashes.
 *
 * Revision 1.12  2001/11/01 00:27:33  robertj
 * Added default Fast Start disabled and H.245 tunneling disable flags
 *   to the endpoint instance.
 *
 * Revision 1.11  2001/10/24 00:54:13  robertj
 * Made cosmetic changes to H.245 miscellaneous command function.
 *
 * Revision 1.10  2001/10/23 02:18:06  dereks
 * Initial release of CU30 video codec.
 *
 * Revision 1.9  2001/09/26 06:20:56  robertj
 * Fixed properly nesting connection locking and unlocking requiring a quite
 *   large change to teh implementation of how calls are answered.
 *
 * Revision 1.8  2001/09/19 03:30:53  robertj
 * Added some support for overlapped dialing, thanks Chris Purvis & Nick Hoath.
 *
 * Revision 1.7  2001/09/13 06:48:13  robertj
 * Added call back functions for remaining Q.931/H.225 messages.
 * Added call back to allow modification of Release Complete,thanks Nick Hoath
 *
 * Revision 1.6  2001/09/12 06:57:58  robertj
 * Added support for iNow Access Token from gk, thanks Nick Hoath
 *
 * Revision 1.5  2001/09/12 06:04:36  robertj
 * Added support for sending UUIE's to gk on request, thanks Nick Hoath
 *
 * Revision 1.4  2001/09/11 01:24:36  robertj
 * Added conditional compilation to remove video and/or audio codecs.
 *
 * Revision 1.3  2001/08/22 06:54:50  robertj
 * Changed connection locking to use double mutex to guarantee that
 *   no threads can ever deadlock or access deleted connection.
 *
 * Revision 1.2  2001/08/16 07:49:16  robertj
 * Changed the H.450 support to be more extensible. Protocol handlers
 *   are now in separate classes instead of all in H323Connection.
 *
 * Revision 1.1  2001/08/06 03:08:11  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 */

#ifndef __OPAL_H323CON_H
#define __OPAL_H323CON_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifdef P_DTMF
#include <ptclib/dtmf.h>
#endif

#include "h323caps.h"
#include "transports.h"
#include "channels.h"
#include "guid.h"
#include "openh323buildopts.h"
#include "h235auth.h"

#ifdef H323_SIGNAL_AGGREGATE
#include <ptclib/sockagg.h>

/** This class defines a handle suitable for use with the aggregation system
  */
class H323AggregatedH2x5Handle : public PAggregatedHandle
{
  PCLASSINFO(H323AggregatedH2x5Handle, PAggregatedHandle)
  public:
    H323AggregatedH2x5Handle(H323Transport & _transport, H323Connection & _connection);
    ~H323AggregatedH2x5Handle();

    PAggregatorFDList_t GetFDs();

    BOOL OnRead();
    virtual BOOL HandlePDU(BOOL ok, PBYTEArray & pdu) = 0;
    PTimeInterval GetTimeout()
    { return transport.GetReadTimeout(); }

  protected:
    PAggregatorFD fd;
    H323Transport & transport;
    H323Connection & connection;
    PBYTEArray pduBuffer;
    PINDEX pduDataLen;
};

#endif

#ifdef H323_H248
 #include "svcctrl.h"
#endif


/* The following classes have forward references to avoid including the VERY
   large header files for H225 and H245. If an application requires access
   to the protocol classes they can include them, but for simple usage their
   inclusion can be avoided.
 */
class PPER_Stream;

class H225_EndpointType;
class H225_TransportAddress;
class H225_ArrayOf_PASN_OctetString;
class H225_ProtocolIdentifier;
class H225_AdmissionRequest;
class H225_AdmissionReject;
class H225_AdmissionConfirm;
class H225_InfoRequestResponse;
class H225_DisengageRequest;
class H225_FeatureSet;
class H225_Setup_UUIE;
class H225_ArrayOf_ServiceControlSession;
class H225_ServiceControlSession_reason;

class H245_TerminalCapabilitySet;
class H245_TerminalCapabilitySetReject;
class H245_OpenLogicalChannel;
class H245_OpenLogicalChannelAck;
class H245_TransportAddress;
class H245_UserInputIndication;
class H245_RequestMode;
class H245_RequestModeAck;
class H245_RequestModeReject;
class H245_ModeDescription;
class H245_ArrayOf_ModeDescription;
class H245_SendTerminalCapabilitySet;
class H245_MultiplexCapability;
class H245_FlowControlCommand;
class H245_MiscellaneousCommand;
class H245_MiscellaneousIndication;
class H245_JitterIndication;
class H245_ConferenceRequest;
class H245_ConferenceResponse;
class H245_ConferenceCommand;
class H245_ConferenceIndication;
class H245_GenericMessage;
class H245_ArrayOf_GenericParameter;

class H323SignalPDU;
class H323ControlPDU;
class H323_RTP_UDP;

class H235Authenticators;

class H245NegMasterSlaveDetermination;
class H245NegTerminalCapabilitySet;
class H245NegLogicalChannels;
class H245NegRequestMode;
class H245NegRoundTripDelay;

#ifdef H323_H450

class H450xDispatcher;
class H4502Handler;
class H4503Handler;
class H4504Handler;
class H4506Handler;
class H45011Handler;

#endif

#ifdef H323_T120
class OpalT120Protocol;
#endif

#ifdef H323_T38
class OpalT38Protocol;
#endif

#ifdef H323_H224
class OpalH224Handler;
class OpalH281Handler;
#endif

class OpalRFC2833;
class OpalRFC2833Info;

#ifdef H323_H460
class H460_FeatureSet;
#endif

#if H323_FILE
class H323FileTransferHandler;
class H323FileTransferList;
#endif

///////////////////////////////////////////////////////////////////////////////

/**This class represents a particular H323 connection between two endpoints.
   There are at least two threads in use, this one to look after the
   signalling channel, an another to look after the control channel. There
   would then be additional threads created for each data channel created by
   the control channel protocol thread.
 */

class H323Connection : public PObject
{
  PCLASSINFO(H323Connection, PObject);

  friend class AggregatedH225Handle;
  friend class AggregatedH245Handle;
  public:
  /**@name Construction */
  //@{
    enum Options {
      FastStartOptionDisable       = 0x0001,
      FastStartOptionEnable        = 0x0002,
      FastStartOptionMask          = 0x0003,

      H245TunnelingOptionDisable   = 0x0004,
      H245TunnelingOptionEnable    = 0x0008,
      H245TunnelingOptionMask      = 0x000c,

      H245inSetupOptionDisable     = 0x0010,
      H245inSetupOptionEnable      = 0x0020,
      H245inSetupOptionMask        = 0x0030,

      DetectInBandDTMFOptionDisable= 0x0040,
      DetectInBandDTMFOptionEnable = 0x0080,
      DetectInBandDTMFOptionMask   = 0x00c0,

#ifdef H323_RTP_AGGREGATE
      RTPAggregationDisable        = 0x0100,
      RTPAggregationEnable         = 0x0200,
      RTPAggregationMask           = 0x0300,
#endif

#ifdef H323_SIGNAL_AGGREGATE
      SignallingAggregationDisable = 0x0400,
      SignallingAggregationEnable  = 0x0800,
      SignallingAggregationMask    = 0x0c00
#endif
    };

    /**Create a new connection.
     */
    H323Connection(
      H323EndPoint & endpoint,  ///< H323 End Point object
      unsigned callReference,   ///< Call reference
      unsigned options = 0      ///< Connection option bits
    );

    /**Destroy the connection
     */
    ~H323Connection();

    /**Lock connection.
       When any thread wants exclusive use of the connection, it must use this
       function to gain the mutex. Note this is not a simple mutex to allow
       for the rather complicated mechanism for preventing deadlocks in
       associated threads to the connection (eg Q.931 reader thread).

       Returns FALSE if the lock was not obtainable due to the connection being
       shut down.
     */
    BOOL Lock();

    /**Try to lock connection.
       When the H323EndPoint::FindConnectionWithLock() function is used to gain
       access to a connection object, this is called to prevent it from being
       closed and deleted by the background threads.

       Note this is an internal function and it is not expected an application
       would use it.

       Returns 0 if the lock was not obtainable due to the connection being
       shut down, -1 if it was not available, and +1 if lock is obtained.
     */
    int TryLock();

    /**Unlock connection.
       If the H323EndPoint::FindConnectionWithLock() function is used to gain
       access to a connection object, this MUST be called to allow it to
       subsequently be closed and disposed of.
     */
    void Unlock();

    /**
      * called when an ARQ needs to be sent to a gatekeeper. This allows the connection
      * to change or check fields in the ARQ before it is sent.
      *
      * By default, this calls the matching function on the endpoint
      */
    virtual void OnSendARQ(
      H225_AdmissionRequest & arq
    );

   /**
      * called when an ACF is received from a gatekeeper. 
      *
      * By default, this calls the matching function on the endpoint
      */
    virtual void OnReceivedACF(
      const H225_AdmissionConfirm & acf
    );

   /**
      * called when an ARJ is received from a gatekeeper. 
      *
      * By default, this calls the matching function on the endpoint
      */
    virtual void OnReceivedARJ(
      const H225_AdmissionReject & arj
    );

    /**
      * called when an IRR needs to be sent to a gatekeeper. This allows the connection
      * to change or check fields in the IRR before it is sent.
      *
      * By default, this does nothing
      */
    virtual void OnSendIRR(
		H225_InfoRequestResponse & irr
	) const;

    /**
      * called when an DRQ needs to be sent to a gatekeeper. This allows the connection
      * to change or check fields in the DRQ before it is sent.
      *
      * By default, this does nothing
      */
    virtual void OnSendDRQ(
		H225_DisengageRequest & drq
	) const;

    /** Called when a connection is established.
        Default behaviour is to call H323EndPoint::OnConnectionEstablished
      */
    virtual void OnEstablished();

    /** Called when a connection is cleared, just after CleanUpOnCallEnd()
        Default behaviour is to call H323EndPoint::OnConnectionCleared
      */
    virtual void OnCleared();

    /**Determine if the call has been connected.
       This indicates that Q.931 CONNECT has occurred. This usually means in
       PSTN gateway environments that a charge will be made for the call. This
       is not quite the same as IsEstablished() as that indicates the call is
       connected AND there is media open.
      */
    BOOL IsConnected() const { return connectionState == HasExecutedSignalConnect || connectionState == EstablishedConnection; }

    /**Determine if the call has been established.
       This can be used in combination with the GetCallEndReason() function
       to determine the three main phases of a call, call setup, call
       established and call cleared.
      */
    BOOL IsEstablished() const { return connectionState == EstablishedConnection; }

    /**Call clearance reasons.
       NOTE: if anything is added to this, you also need to add the field to
       the tables in h323.cxx and h323pdu.cxx.
      */
    enum CallEndReason {
      EndedByLocalUser,         ///< Local endpoint application cleared call
      EndedByNoAccept,          ///< Local endpoint did not accept call OnIncomingCall()=FALSE
      EndedByAnswerDenied,      ///< Local endpoint declined to answer call
      EndedByRemoteUser,        ///< Remote endpoint application cleared call
      EndedByRefusal,           ///< Remote endpoint refused call
      EndedByNoAnswer,          ///< Remote endpoint did not answer in required time
      EndedByCallerAbort,       ///< Remote endpoint stopped calling
      EndedByTransportFail,     ///< Transport error cleared call
      EndedByConnectFail,       ///< Transport connection failed to establish call
      EndedByGatekeeper,        ///< Gatekeeper has cleared call
      EndedByNoUser,            ///< Call failed as could not find user (in GK)
      EndedByNoBandwidth,       ///< Call failed as could not get enough bandwidth
      EndedByCapabilityExchange,///< Could not find common capabilities
      EndedByCallForwarded,     ///< Call was forwarded using FACILITY message
      EndedBySecurityDenial,    ///< Call failed a security check and was ended
      EndedByLocalBusy,         ///< Local endpoint busy
      EndedByLocalCongestion,   ///< Local endpoint congested
      EndedByRemoteBusy,        ///< Remote endpoint busy
      EndedByRemoteCongestion,  ///< Remote endpoint congested
      EndedByUnreachable,       ///< Could not reach the remote party
      EndedByNoEndPoint,        ///< The remote party is not running an endpoint
      EndedByHostOffline,       ///< The remote party host off line
      EndedByTemporaryFailure,  ///< The remote failed temporarily app may retry
      EndedByQ931Cause,         ///< The remote ended the call with unmapped Q.931 cause code
      EndedByDurationLimit,     ///< Call cleared due to an enforced duration limit
      EndedByInvalidConferenceID, ///< Call cleared due to invalid conference ID
      EndedByOSPRefusal,          ///< Call cleared as OSP server unable or unwilling to route
      EndedByInvalidNumberFormat, ///< Call cleared as number was invalid format
      EndedByUnspecifiedProtocolError, ///< Call cleared due to unspecified protocol error
      EndedByNoFeatureSupport,         ///< Call ended due to Feature not being present.
      NumCallEndReasons
    };
#if PTRACING
    friend ostream & operator<<(ostream & o, CallEndReason r);
#endif

    /**Get the call clearand reason for this connection shutting down.
       Note that this function is only generally useful in the
       H323EndPoint::OnConnectionCleared() function. This is due to the
       connection not being cleared before that, and the object not even
       exiting after that.

       If the call is still active then this will return NumCallEndReasons.
      */
    CallEndReason GetCallEndReason() const { return callEndReason; }

    /**Set the call clearance reason.
       An application should have no cause to use this function. It is present
       for the H323EndPoint::ClearCall() function to set the clearance reason.
      */
    virtual void SetCallEndReason(
      CallEndReason reason,     ///< Reason for clearance of connection.
      PSyncPoint * sync = NULL  ///< syncpoint to use for synchronous destruction
    );

    /**Clear a current connection.
       This hangs up the connection to a remote endpoint. It actually just
       calls the endpoint version of the ClearCall() function to avoid
       possible multithreading race conditions.
      */
    virtual BOOL ClearCall(
      CallEndReason reason = EndedByLocalUser  ///< Reason for call clearing
    );

    /**Clear a current connection, synchronously
      */
    virtual BOOL ClearCallSynchronous(
      PSyncPoint * sync,
      CallEndReason reason = EndedByLocalUser  ///< Reason for call clearing
    );

    /**Clean up the call clearance of the connection.
       This function will do any internal cleaning up and waiting on background
       threads that may be using the connection object. After this returns it
       is then safe to delete the object.

       An application will not typically use this function as it is used by the
       H323EndPoint during a clear call.
      */
    virtual void CleanUpOnCallEnd();
  //@}


  /**@name Signalling Channel */
  //@{
    /**Attach a transport to this connection as the signalling channel.
      */
    virtual void AttachSignalChannel(
      const PString & token,    ///< New token to use to identify connection
      H323Transport * channel,  ///< Transport for the PDU's
      BOOL answeringCall        ///< Flag for if incoming/outgoing call.
    );

    /**Write a PDU to the signalling channel.
      */
    virtual BOOL WriteSignalPDU(
      H323SignalPDU & pdu       ///< PDU to write.
    );

    /**Handle reading PDU's from the signalling channel.
       This is an internal function and is unlikely to be used by applications.
     */
    virtual void HandleSignallingChannel();

    /**Handle a single received PDU from the signalling channel.
       This is an internal function and is unlikely to be used by applications.
    */
    virtual BOOL HandleReceivedSignalPDU(BOOL readStatus, H323SignalPDU & pdu);

    /**Handle a single received PDU from the control channel.
       This is an internal function and is unlikely to be used by applications.
    */
    virtual BOOL HandleReceivedControlPDU(BOOL readStatus, PPER_Stream & strm);

    /**Handle PDU from the signalling channel.
       This is an internal function and is unlikely to be used by applications.
     */
    virtual BOOL HandleSignalPDU(
      H323SignalPDU & pdu       ///< PDU to handle.
    );

    /**Handle Control PDU tunnelled in the signalling channel.
       This is an internal function and is unlikely to be used by applications.
     */
    virtual void HandleTunnelPDU(
      H323SignalPDU * txPDU       ///< PDU tunnel response into.
    );

    /**Handle an incoming Q931 setup PDU.
       The default behaviour is to do the handshaking operation calling a few
       virtuals at certain moments in the sequence.

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.
     */
    virtual BOOL OnReceivedSignalSetup(
      const H323SignalPDU & pdu   ///< Received setup PDU
    );

    /**Handle an incoming Q931 setup acknowledge PDU.
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour does nothing.
     */
    virtual BOOL OnReceivedSignalSetupAck(
      const H323SignalPDU & pdu   ///< Received setup PDU
    );

    /**Handle an incoming Q931 information PDU.
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour does nothing.
     */
    virtual BOOL OnReceivedSignalInformation(
      const H323SignalPDU & pdu   ///< Received setup PDU
    );

    /**Handle an incoming Q931 call proceeding PDU.
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour checks for hH245Address field and if present
       starts the separate H245 channel, if successful or not present it
       returns TRUE.
     */
    virtual BOOL OnReceivedCallProceeding(
      const H323SignalPDU & pdu   ///< Received call proceeding PDU
    );

    /**Handle an incoming Q931 progress PDU.
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour checks for hH245Address field and if present
       starts the separate H245 channel, if successful or not present it
       returns TRUE.
     */
    virtual BOOL OnReceivedProgress(
      const H323SignalPDU & pdu   ///< Received call proceeding PDU
    );

    /**Handle an incoming Q931 alerting PDU.
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour obtains the display name and calls OnAlerting().
     */
    virtual BOOL OnReceivedAlerting(
      const H323SignalPDU & pdu   ///< Received Alerting PDU
    );

    /**Handle an incoming Q931 connect PDU.
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour checks for hH245Address field and if present
       starts the separate H245 channel, if successful it returns TRUE.
       If not present and there is no H245Tunneling then it returns FALSE.
     */
    virtual BOOL OnReceivedSignalConnect(
      const H323SignalPDU & pdu   ///< Received connect PDU
    );

    /**Handle an incoming Q931 facility PDU.
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour checks for hH245Address field and if present
       starts the separate H245 channel, if successful or not present it
       returns TRUE.
     */
    virtual BOOL OnReceivedFacility(
      const H323SignalPDU & pdu   ///< Received Facility PDU
    );

    /**Handle an incoming Q931 Notify PDU.
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour simply returns TRUE.
     */
    virtual BOOL OnReceivedSignalNotify(
      const H323SignalPDU & pdu   ///< Received Notify PDU
    );

    /**Handle an incoming Q931 Status PDU.
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour simply returns TRUE.
     */
    virtual BOOL OnReceivedSignalStatus(
      const H323SignalPDU & pdu   ///< Received Status PDU
    );

    /**Handle an incoming Q931 Status Enquiry PDU.
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour sends a Q931 Status PDU back.
     */
    virtual BOOL OnReceivedStatusEnquiry(
      const H323SignalPDU & pdu   ///< Received Status Enquiry PDU
    );

    /**Handle an incoming Q931 Release Complete PDU.
       The default behaviour calls Clear() using reason code based on the
       Release Complete Cause field and the current connection state.
     */
    virtual void OnReceivedReleaseComplete(
      const H323SignalPDU & pdu   ///< Received Release Complete PDU
    );

    /**This function is called from the HandleSignallingChannel() function
       for unhandled PDU types.

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent. The default behaviour returns TRUE.
     */
    virtual BOOL OnUnknownSignalPDU(
      const H323SignalPDU & pdu  ///< Received PDU
    );

    /**Call back for incoming call.
       This function is called from the OnReceivedSignalSetup() function
       before it sends the Alerting PDU. It gives an opportunity for an
       application to alter the reply before transmission to the other
       endpoint.

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour calls the endpoint function of the same name.
     */
    virtual BOOL OnIncomingCall(
      const H323SignalPDU & setupPDU,   ///< Received setup PDU
      H323SignalPDU & alertingPDU       ///< Alerting PDU to send
    );
    virtual BOOL OnIncomingCall(
      const H323SignalPDU & setupPDU,   ///< Received setup PDU
      H323SignalPDU & alertingPDU,      ///< Alerting PDU to send
      CallEndReason & reason            ///< reason for call refusal, if returned false
    );

    /**Forward incoming call to specified address.
       This would typically be called from within the OnIncomingCall()
       function when an application wishes to redirct an unwanted incoming
       call.

       The return value is TRUE if the call is to be forwarded, FALSE
       otherwise. Note that if the call is forwarded the current connection is
       cleared with the ended call code of EndedByCallForwarded.
      */
    virtual BOOL ForwardCall(
      const PString & forwardParty   ///< Party to forward call to.
    );

#ifdef H323_H450

    /**Initiate the transfer of an existing call (connection) to a new remote party
       using H.450.2.  This sends a Call Transfer Initiate Invoke message from the
       A-Party (transferring endpoint) to the B-Party (transferred endpoint).
     */
    void TransferCall(
      const PString & remoteParty,   ///< Remote party to transfer the existing call to
      const PString & callIdentity = PString::Empty()
                                    ///< Call Identity of secondary call if present
    );

    /**Transfer the call through consultation so the remote party in the primary call is connected to
       the called party in the second call using H.450.2.  This sends a Call Transfer Identify Invoke 
       message from the A-Party (transferring endpoint) to the C-Party (transferred-to endpoint).
     */
    void ConsultationTransfer(
      const PString & primaryCallToken  ///< Primary call
    );

    /**Called from H.450 OnReceivedInitiateReturnError Error in Transfer
     */
    virtual void OnReceivedInitiateReturnError();

    /**Handle the reception of a callTransferSetupInvoke APDU whilst a secondary call exists.  This 
       method checks whether the secondary call is still waiting for a callTransferSetupInvoke APDU and 
       proceeds to clear the call if the call identies match.
       This is an internal function and it is not expected the user will call
       it directly.
     */
    virtual void HandleConsultationTransfer(
      const PString & callIdentity, /**Call Identity of secondary call 
                                       received in SETUP Message. */
      H323Connection & incoming     ///< Connection upon which SETUP PDU was received.
    );

    /**Determine whether this connection is being transferred.
     */
    BOOL IsTransferringCall() const;

    /**Determine whether this connection is the result of a transferred call.
     */
    BOOL IsTransferredCall() const;

    /**Handle the transfer of an existing connection to a new remote.
       This is an internal function and it is not expected the user will call
       it directly.
     */
    virtual void HandleTransferCall(
      const PString & token,
      const PString & identity
    );

    /**Get transfer invoke ID dureing trasfer.
       This is an internal function and it is not expected the user will call
       it directly.
      */
    int GetCallTransferInvokeId();

    /**Handle the failure of a call transfer operation at the Transferred Endpoint.  This method is
       used to handle the following transfer failure cases that can occur at the Transferred Endpoint. 
       The cases are:
       Reception of an Admission Reject
       Reception of a callTransferSetup return error APDU.
       Expiry of Call Transfer timer CT-T4.
     */
    virtual void HandleCallTransferFailure(
      const int returnError    ///< Failure reason code
    );

    /**Store the passed token on the current connection's H4502Handler.
       This is an internal function and it is not expected the user will call
       it directly.
     */
    void SetAssociatedCallToken(
      const PString & token  ///< Associated token
    );

    /**Callback to indicate a successful transfer through consultation.  The paramter passed is a
       reference to the existing connection between the Transferring endpoint and Transferred-to 
       endpoint.
     */
    virtual void OnConsultationTransferSuccess(
      H323Connection & secondaryCall  ///< Secondary call for consultation
    );

     /**Set the call linkage associated with the current call. This is used to include the callToken which is 
      requesting this connection. ie. Call Transfer. This information can be used for billing systems
      to correctly charge the correct party for Transferred or forwarded calls.
       */
     virtual void SetCallLinkage(
		H225_AdmissionRequest & arq   ///< Admission Request PDU
      );

     /**Set the call linkage associated with the current call. This is used to detect the callToken which has 
      requesting this connection. ie. Call Transfer. This information can be used for billing systems
      to correctly charge the correct party for Transferred or forwarded calls.
     */
     virtual void GetCallLinkage(
		const H225_AdmissionRequest & arq  ///< Admission Request PDU
	);

    /**Retrieves the redirecting number(s) and additional call diversion information (div. counter
	   and div. reason) as of an incoming redirected call, currently only according to H.450.3 call diversion
	   supplementary service 
	*/

    BOOL GetRedirectingNumber(
      PString &originalCalledNr,               
      PString &lastDivertingNr,
      int &divCounter, 
      int &originaldivReason,
      int &divReason);

    /**Place the call on hold, suspending all media channels (H.450.4).  Note it is the responsibility 
       of the application layer to delete the MOH Channel if music on hold is provided to the remote
       endpoint.  So far only Local Hold has been implemented. 
     */
    void HoldCall(
      BOOL localHold   ///< true for Local Hold, false for Remote Hold
    );

    /**Retrieve the call from hold, activating all media channels (H.450.4).
       This method examines the call hold state and performs the necessary
       actions required to retrieve a Near-end or Remote-end call on hold.
       NOTE: Only Local Hold is implemented so far. 
    */
    void RetrieveCall();

    /**Set the alternative Audio media channel.  This channel can be used to provide
       Media On Hold (MOH) for a near end call hold operation or to provide
       Recorded Voice Anouncements (RVAs).  If this method is not called before
       a call hold operation is attempted, no Audio media on hold will be provided
       for the held endpoint.
      */
    void SetHoldMedia(
      PChannel * audioChannel
    );

    /**Set the alternative Video media channel.  This channel can be used to provide
       Video On Hold (VOH) for a near end call hold operation or to provide
       a fake or onhold video.  If this method is not called before
       a call hold operation is attempted, no Video media on hold will be provided
       for the held endpoint.
      */
    void SetVideoHoldMedia(
	  PChannel * videoChannel
	);

    /**CallBack when Call is put on hold. This allows the device to release the 
       Local Input device to be used for another active connection
      */
    virtual PChannel *  OnCallHold(BOOL IsEncoder,     ///* Direction
                               unsigned sessionId,     ///* Session Id 
                               unsigned bufferSize,    ///* Size of each sound buffer (Audio)
                               PChannel * channel);    ///* Channel being Held

     /**CallBack when call is about to be retrieved. This allows the Local Input device
       to be reattached to the Held Channel.
       */
    virtual PChannel *  OnCallRetrieve(BOOL IsEncoder,  ///* Direction
                               unsigned sessionId,      ///* Session Id    
                               unsigned bufferSize,     ///* Size of each sound buffer (Audio)
                               PChannel * channel);	///* Channel being Held

    /**Determine if Meadia On Hold is enabled.
      */
    BOOL IsMediaOnHold() const;

    /**Determine if held.
      */
    BOOL IsLocalHold() const;

    /**Determine if held.
      */
    BOOL IsRemoteHold() const;

    /**Determine if the current call is held or in the process of being held.
      */
    BOOL IsCallOnHold() const;

    /**Begin a call intrusion request.
       Calls h45011handler->IntrudeCall where SS pdu is added to Call Setup
       message.
      */
    virtual void IntrudeCall(
      unsigned capabilityLevel
    );

    /**Handle an incoming call instrusion request.
       Calls h45011handler->AwaitSetupResponse where we set Handler state to
       CI-Wait-Ack
      */
    virtual void HandleIntrudeCall(
      const PString & token,
      const PString & identity
    );

    /**Set flag indicating call intrusion.
       Used to set a flag when intrusion occurs and to determine if
       connection is created for Call Intrusion. This flag is used when we
       should decide whether to Answer the call or to Close it.
      */
    void SetCallIntrusion() { isCallIntrusion = TRUE; }

    BOOL IsCallIntrusion() { return isCallIntrusion; }

    /**Get Call Intrusion Protection Level of the local endpoint.
      */
    unsigned GetLocalCallIntrusionProtectionLevel() { return callIntrusionProtectionLevel; }

    /**Get Call Intrusion Protection Level of other endpoints that we are in
       connection with.
      */
    virtual BOOL GetRemoteCallIntrusionProtectionLevel(
      const PString & callToken,
      unsigned callIntrusionProtectionLevel
    );

    virtual void SetIntrusionImpending();

    virtual void SetForcedReleaseAccepted();

    virtual void SetIntrusionNotAuthorized();

    /**Send a Call Waiting indication message to the remote endpoint using
       H.450.6.  The second paramter is used to indicate to the calling user
       how many additional users are "camped on" the called user. A value of
       zero indicates to the calling user that he/she is the only user
       attempting to reach the busy called user.
     */
    void SendCallWaitingIndication(
      const unsigned nbOfAddWaitingCalls = 0   ///< number of additional waiting calls at the served user
    );

#endif // H323_H450

    enum AnswerCallResponse {
      AnswerCallNow,               ///< Answer the call continuing with the connection.
      AnswerCallDenied,            ///< Refuse the call sending a release complete.
      AnswerCallPending,           ///< Send an Alerting PDU and wait for AnsweringCall()
      AnswerCallDeferred,          ///< As for AnswerCallPending but does not send Alerting PDU
      AnswerCallAlertWithMedia,    ///< As for AnswerCallPending but starts media channels
      AnswerCallDeferredWithMedia, ///< As for AnswerCallDeferred but starts media channels
      AnswerCallDeniedByInvalidCID, ///< As for AnswerCallDenied but returns e_invalidCID
      NumAnswerCallResponses
    };
#if PTRACING
    friend ostream & operator<<(ostream & o, AnswerCallResponse s);
#endif

    /**Call back for answering an incoming call.
       This function is used for an application to control the answering of
       incoming calls. It is usually used to indicate the immediate action to
       be taken in answering the call.

       It is called from the OnReceivedSignalSetup() function before it sends
       the Alerting or Connect PDUs. It also gives an opportunity for an
       application to alter the Connect PDU reply before transmission to the
       remote endpoint.

       If AnswerCallNow is returned then the H.323 protocol proceeds with the
       connection. If AnswerCallDenied is returned the connection is aborted
       and a Release Complete PDU is sent. If AnswerCallPending is returned
       then the Alerting PDU is sent and the protocol negotiations are paused
       until the AnsweringCall() function is called. Finally, if
       AnswerCallDeferred is returned then no Alerting PDU is sent, but the
       system still waits as in the AnswerCallPending response.

       Note this function should not block for any length of time. If the
       decision to answer the call may take some time eg waiting for a user to
       pick up the phone, then AnswerCallPending or AnswerCallDeferred should
       be returned.

       The default behaviour calls the endpoint function of the same name
       which in turn will return AnswerCallNow.
     */
    virtual AnswerCallResponse OnAnswerCall(
      const PString & callerName,       ///< Name of caller
      const H323SignalPDU & setupPDU,   ///< Received setup PDU
      H323SignalPDU & connectPDU        ///< Connect PDU to send. 
    );

    /**Indicate the result of answering an incoming call.
       This should only be called if the OnAnswerCall() callback function has
       returned a AnswerCallPending or AnswerCallDeferred response.

       Note sending further AnswerCallPending responses via this function will
       have the result of an Alerting PDU being sent to the remote endpoint.
       In this way multiple Alerting PDUs may be sent.

       Sending a AnswerCallDeferred response would have no effect.
      */
    void AnsweringCall(
      AnswerCallResponse response ///< Answer response to incoming call
    );

    /**Send first PDU in signalling channel.
       This function does the signalling handshaking for establishing a
       connection to a remote endpoint. The transport (TCP/IP) for the
       signalling channel is assumed to be already created. This function
       will then do the SetRemoteAddress() and Connect() calls o establish
       the transport.

       Returns the error code for the call failure reason or NumCallEndReasons
       if the call was successful to that point in the protocol.
     */
    virtual CallEndReason SendSignalSetup(
      const PString & alias,                ///< Name of remote party
      const H323TransportAddress & address  ///< Address of destination
    );

    /**Adjust setup PDU being sent on initialisation of signal channel.
       This function is called from the SendSignalSetup() function before it
       sends the Setup PDU. It gives an opportunity for an application to
       alter the request before transmission to the other endpoint.

       The default behaviour simply returns TRUE. Note that this is usually
       overridden by the transport dependent descendent class, eg the
       H323ConnectionTCP descendent fills in the destCallSignalAddress field
       with the TCP/IP data. Therefore if you override this in your
       application make sure you call the ancestor function.
     */
    virtual BOOL OnSendSignalSetup(
      H323SignalPDU & setupPDU   ///< Setup PDU to send
    );

    /**Adjust call proceeding PDU being sent. This function is called from
       the OnReceivedSignalSetup() function before it sends the Call
       Proceeding PDU. It gives an opportunity for an application to alter
       the request before transmission to the other endpoint. If this function
       returns FALSE then the Call Proceeding PDU is not sent at all.

       The default behaviour simply returns TRUE.
     */
    virtual BOOL OnSendCallProceeding(
      H323SignalPDU & callProceedingPDU   ///< Call Proceeding PDU to send
    );

    /**Call back for Release Complete being sent.
       This allows an application to add things to the release complete before
       it is sent to the remote endpoint.

       Returning FALSE will prevent the release complete from being sent. Note
       that this would be very unusual as this is called when the connection
       is being cleaned up. There will be no second chance to send the PDU and
       it must be sent.

       The default behaviour simply returns TRUE.
      */
    virtual BOOL OnSendReleaseComplete(
      H323SignalPDU & releaseCompletePDU ///< Release Complete PDU to send
    );

    /**Call back for remote party being alerted.
       This function is called from the SendSignalSetup() function after it
       receives the optional Alerting PDU from the remote endpoint. That is
       when the remote "phone" is "ringing".

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour calls the endpoint function of the same name.
     */
    virtual BOOL OnAlerting(
      const H323SignalPDU & alertingPDU,  ///< Received Alerting PDU
      const PString & user                ///< Username of remote endpoint
    );

    /**This function is called when insufficient digits have been entered.
       This supports overlapped dialling so that a call can begin when it is
       not known how many more digits are to be entered in a phone number.

       It is expected that the application will override this function. It
       should be noted that the application should not block in the function
       but only indicate to whatever other thread is gathering digits that
       more are required and that thread should call SendMoreDigits().

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour simply returns FALSE.
     */
    virtual BOOL OnInsufficientDigits();

    /**This function is called when sufficient digits have been entered.
       This supports overlapped dialling so that a call can begin when it is
       not known how many more digits are to be entered in a phone number.

       The digits parameter is appended to the existing remoteNumber member
       variable and the call is retried.

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour simply returns TRUE.
     */
    virtual void SendMoreDigits(
      const PString & digits    ///< Extra digits
    );

    /**This function is called from the SendSignalSetup() function after it
       receives the Connect PDU from the remote endpoint, but before it
       attempts to open the control channel.

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour calls H323EndPoint::OnOutgoingCall
     */
    virtual BOOL OnOutgoingCall(
      const H323SignalPDU & connectPDU   ///< Received Connect PDU
    );

    /**Send an the acknowldege of a fast start.
       This function is called when the fast start channels provided to this
       connection by the original SETUP PDU have been selected and opened and
       need to be sent back to the remote endpoint.

       If FALSE is returned then no fast start has been acknowledged, possibly
       due to no common codec in fast start request.

       The default behaviour uses OnSelectLogicalChannels() to find a pair of
       channels and adds then to the provided PDU.
     */
    virtual BOOL SendFastStartAcknowledge(
      H225_ArrayOf_PASN_OctetString & array   ///< Array of H245_OpenLogicalChannel
    );

    /**Handle the acknowldege of a fast start.
       This function is called from one of a number of functions after it
       receives a PDU from the remote endpoint that has a fastStart field. It
       is in response to a request for a fast strart from the local endpoint.

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour parses the provided array and starts the channels
       acknowledged in it.
     */
    virtual BOOL HandleFastStartAcknowledge(
      const H225_ArrayOf_PASN_OctetString & array   ///< Array of H245_OpenLogicalChannel
    );

    /**Start a separate H245 channel.
       This function is called from one of a number of functions when it needs
       to create the h245 channel for the remote endpoint to connect back to.

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.
     */
    virtual BOOL StartControlChannel();

    /**Start a separate H245 channel.
       This function is called from one of a number of functions after it
       receives a PDU from the remote endpoint that has a h245Address field.

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour checks to see if it is a known transport and
       creates a corresponding H323Transport decendent for the control
       channel.
     */
    virtual BOOL StartControlChannel(
      const H225_TransportAddress & h245Address   ///< H245 address
    );
  //@}

  /**@name Control Channel */
  //@{
    /**Write a PDU to the control channel.
       If there is no control channel open then this will tunnel the PDU
       into the signalling channel.
      */
    BOOL WriteControlPDU(
      const H323ControlPDU & pdu
    );

    /**Start control channel negotiations.
      */
    virtual BOOL StartControlNegotiations(
      BOOL renegotiate = FALSE  ///< Force renogotiation of TCS/MSD
    );

    /**Handle reading data on the control channel.
     */
    virtual void HandleControlChannel();

    /**Handle incoming data on the control channel.
       This decodes the data stream into a PDU and calls HandleControlPDU().

       If FALSE is returned the connection is aborted. The default behaviour
       returns TRUE.
     */
    virtual BOOL HandleControlData(
      PPER_Stream & strm
    );

    /**Handle incoming PDU's on the control channel. Dispatches them to the
       various virtuals off this class.

       If FALSE is returned the connection is aborted. The default behaviour
       returns TRUE.
     */
    virtual BOOL HandleControlPDU(
      const H323ControlPDU & pdu
    );

    /**This function is called from the HandleControlPDU() function
       for unhandled PDU types.

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent. The default behaviour returns TRUE.

       The default behaviour send a FunctioNotUnderstood indication back to
       the sender, and returns TRUE to continue operation.
     */
    virtual BOOL OnUnknownControlPDU(
      const H323ControlPDU & pdu  ///< Received PDU
    );

    /**Handle incoming request PDU's on the control channel.
       Dispatches them to the various virtuals off this class.
     */
    virtual BOOL OnH245Request(
      const H323ControlPDU & pdu  ///< Received PDU
    );

    /**Handle incoming response PDU's on the control channel.
       Dispatches them to the various virtuals off this class.
     */
    virtual BOOL OnH245Response(
      const H323ControlPDU & pdu  ///< Received PDU
    );

    /**Handle incoming command PDU's on the control channel.
       Dispatches them to the various virtuals off this class.
     */
    virtual BOOL OnH245Command(
      const H323ControlPDU & pdu  ///< Received PDU
    );

    /**Handle incoming indication PDU's on the control channel.
       Dispatches them to the various virtuals off this class.
     */
    virtual BOOL OnH245Indication(
      const H323ControlPDU & pdu  ///< Received PDU
    );

    /**Handle H245 command to send terminal capability set.
     */
    virtual BOOL OnH245_SendTerminalCapabilitySet(
      const H245_SendTerminalCapabilitySet & pdu  ///< Received PDU
    );

    /**Handle H245 command to control flow control.
       This function calls OnLogicalChannelFlowControl() with the channel and
       bit rate restriction.
     */
    virtual BOOL OnH245_FlowControlCommand(
      const H245_FlowControlCommand & pdu  ///< Received PDU
    );

    /**Handle H245 miscellaneous command.
       This function passes the miscellaneous command on to the channel
       defined by the pdu.
     */
    virtual BOOL OnH245_MiscellaneousCommand(
      const H245_MiscellaneousCommand & pdu  ///< Received PDU
    );

    /**Handle H245 miscellaneous indication.
       This function passes the miscellaneous indication on to the channel
       defined by the pdu.
     */
    virtual BOOL OnH245_MiscellaneousIndication(
      const H245_MiscellaneousIndication & pdu  ///< Received PDU
    );

    /**Handle H245 indication of received jitter.
       This function calls OnLogicalChannelJitter() with the channel and
       estimated jitter.
     */
    virtual BOOL OnH245_JitterIndication(
      const H245_JitterIndication & pdu  ///< Received PDU
    );

    /**Error discriminator for the OnControlProtocolError() function.
      */
    enum ControlProtocolErrors {
      e_MasterSlaveDetermination,
      e_CapabilityExchange,
      e_LogicalChannel,
      e_ModeRequest,
      e_RoundTripDelay
    };

    /**This function is called from the HandleControlPDU() function or
       any of its sub-functions for protocol errors, eg unhandled PDU types.

       The errorData field may be a string or PDU or some other data depending
       on the value of the errorSource parameter. These are:
          e_UnhandledPDU                    &H323ControlPDU
          e_MasterSlaveDetermination        const char *

       If FALSE is returned the connection is aborted. The default behaviour
       returns TRUE.
     */
    virtual BOOL OnControlProtocolError(
      ControlProtocolErrors errorSource,  ///< Source of the proptoerror
      const void * errorData = NULL       ///< Data associated with error
    );

    /**This function is called from the HandleControlPDU() function when
       it is about to send the Capabilities Set to the remote endpoint. This
       gives the application an oppurtunity to alter the PDU to be sent.

       The default behaviour will make "adjustments" for compatibility with
       some broken remote endpoints.
     */
    virtual void OnSendCapabilitySet(
      H245_TerminalCapabilitySet & pdu  ///< PDU to send
    );

    /**This function is called when the remote endpoint sends its capability
       set. This gives the application an opportunity to determine what codecs
       are available and if it supports any of the combinations of codecs.

       Note any codec types that are the remote system supports that are not in
       the codecs list member variable for the endpoint are ignored and not
       included in the remoteCodecs list.

       The default behaviour assigns the table and set to member variables and
       returns TRUE if the remoteCodecs list is not empty.
     */
    virtual BOOL OnReceivedCapabilitySet(
      const H323Capabilities & remoteCaps,      ///< Capability combinations remote supports
      const H245_MultiplexCapability * muxCap,  ///< Transport capability, if present
      H245_TerminalCapabilitySetReject & reject ///< Rejection PDU (if return FALSE)
    );

    /**Send a new capability set.
      */
    virtual void SendCapabilitySet(
      BOOL empty  ///< Send an empty set.
    );

    /**Call back to set the local capabilities.
       This is called just before the capabilties are required when a call
       is begun. It is called when a SETUP PDU is received or when one is
       about to be sent, so that the capabilities may be adjusted for correct
       fast start operation.

       The default behaviour does nothing.
      */
    virtual void OnSetLocalCapabilities();

    /**Return if this H245 connection is a master or slave
     */
    BOOL IsH245Master() const;

    /**Start the round trip delay calculation over the control channel.
     */
    void StartRoundTripDelay();

    /**Get the round trip delay over the control channel.
     */
    PTimeInterval GetRoundTripDelay() const;
  //@}

  /**@name Logical Channel Management */
  //@{
    /**Call back to select logical channels to start.

       This function must be defined by the descendent class. It is used
       to select the logical channels to be opened between the two endpoints.
       There are three ways in which this may be called: when a "fast start"
       has been initiated by the local endpoint (via SendSignalSetup()
       function), when a "fast start" has been requested from the remote
       endpoint (via the OnReceivedSignalSetup() function) or when the H245
       capability set (and master/slave) negotiations have completed (via the
       OnControlChannelOpen() function.

       The function would typically examine several member variable to decide
       which mode it is being called in and what to do. If fastStartState is
       FastStartDisabled then non-fast start semantics should be used. The
       H245 capabilities in the remoteCapabilities members should be
       examined, and appropriate transmit channels started using
       OpenLogicalChannel().

       If fastStartState is FastStartInitiate, then the local endpoint has
       initiated a call and is asking the application if fast start semantics
       are to be used. If so it is expected that the function call 
       OpenLogicalChannel() for all the channels that it wishes to be able to
       be use. A subset (possibly none!) of these would actually be started
       when the remote endpoint replies.

       If fastStartState is FastStartResponse, then this indicates the remote
       endpoint is attempting a fast start. The fastStartChannels member
       contains a list of possible channels from the remote that the local
       endpoint is to select which to accept. For each accepted channel it
       simply necessary to call the Start() function on that channel eg
       fastStartChannels[0].Start();

       The default behaviour selects the first codec of each session number
       that is available. This is according to the order of the capabilities
       in the remoteCapabilities, the local capability table or of the
       fastStartChannels list respectively for each of the above scenarios.
      */
    virtual void OnSelectLogicalChannels();

    /**Select default logical channel for normal start.
      */
    virtual void SelectDefaultLogicalChannel(
      unsigned sessionID    ///< Session ID to find default logical channel.
    );

	/** MinMerge the local and remote Video and Extended Video Capabilities to ensure
	    correct maximum framesize is negotiated between the parties.
	  */
	virtual BOOL MergeCapabilities(
		unsigned sessionID,                ///< Session ID to find default logical channel.
		const H323Capability & local,     ///< Local Capability
		H323Capability * remote           ///< remote Capability
	);

    /**Select default logical channel for fast start.
       Internal function, not for normal use.
      */
    virtual void SelectFastStartChannels(
      unsigned sessionID,   ///< Session ID to find default logical channel.
      BOOL transmitter,     ///< Whether to open transmitters
      BOOL receiver         ///< Whether to open receivers
    );

	/** Disable FastStart on a call by call basis
	  */
    void DisableFastStart();

    /**Open a new logical channel.
       This function will open a channel between the endpoints for the
       specified capability.

       If this function is called while there is not yet a conenction
       established, eg from the OnFastStartLogicalChannels() function, then
       a "trial" receiver/transmitter channel is created. This channel is not
       started until the remote enpoint has confirmed that they are to start.
       Any channels not confirmed are deleted.

       If this function is called later in the call sequence, eg from
       OnSelectLogicalChannels(), then it may only establish a transmit
       channel, ie fromRemote must be FALSE.
      */
    virtual BOOL OpenLogicalChannel(
      const H323Capability & capability,  ///< Capability to open channel with
      unsigned sessionID,                 ///< Session for the channel
      H323Channel::Directions dir         ///< Direction of channel
    );

    /**This function is called when the remote endpoint want's to open
       a new channel.

       If the return value is FALSE then the open is rejected using the
       errorCode as the cause, this would be a value from the enum
       H245_OpenLogicalChannelReject_cause::Choices.

       The default behaviour simply returns TRUE.
     */
    virtual BOOL OnOpenLogicalChannel(
      const H245_OpenLogicalChannel & openPDU,  ///< Received PDU for the channel open
      H245_OpenLogicalChannelAck & ackPDU,      ///< PDU to send for acknowledgement
      unsigned & errorCode                      ///< Error to return if refused
    );

    /**Callback for when a logical channel conflict has occurred.
       This is called when the remote endpoint, which is a master, rejects
       our transmitter channel due to a resource conflict. Typically an
       inability to do asymmetric codecs. The local (slave) endpoint must then
       try and open a new transmitter channel using the same codec as the
       receiver that is being opened.
      */
    virtual BOOL OnConflictingLogicalChannel(
      H323Channel & channel    ///< Channel that conflicted
    );

    /**Create a new logical channel object.
       This is in response to a request from the remote endpoint to open a
       logical channel.
      */
    virtual H323Channel * CreateLogicalChannel(
      const H245_OpenLogicalChannel & open, ///< Parameters for opening channel
      BOOL startingFast,                    ///< Flag for fast/slow starting.
      unsigned & errorCode                  ///< Reason for create failure
    );

    /**Create a new real time logical channel object.
       This creates a logical channel for handling RTP data. It is primarily
       used to allow an application to redirect the RTP media streams to other
       hosts to the local one. In that case it would create an instance of
       the H323_ExternalRTPChannel class with the appropriate address. eg:

         H323Channel * MyConnection::CreateRealTimeLogicalChannel(
                                        const H323Capability & capability,
                                        H323Channel::Directions dir,
                                        unsigned sessionID,
                                        const H245_H2250LogicalChannelParameters * param,
                                        RTP_QOS * rtpqos)
         {
           return new H323_ExternalRTPChannel(*this, capability, dir, sessionID,
                                              externalIpAddress, externalPort);
         }

       An application would typically also override the OnStartLogicalChannel()
       function to obtain from the H323_ExternalRTPChannel instance the address
       of the remote endpoints media server RTP addresses to complete the
       setting up of the external RTP stack. eg:

         BOOL OnStartLogicalChannel(H323Channel & channel)
         {
           H323_ExternalRTPChannel & external = (H323_ExternalRTPChannel &)channel;
           external.GetRemoteAddress(remoteIpAddress, remotePort);
         }

       Note that the port in the above example is always the data port, the
       control port is assumed to be data+1.

       The default behaviour assures there is an RTP session for the session ID,
       and if not creates one, then creates a H323_RTPChannel which will do RTP
       media to the local host.
      */
    virtual H323Channel * CreateRealTimeLogicalChannel(
      const H323Capability & capability, ///< Capability creating channel
      H323Channel::Directions dir,       ///< Direction of channel
      unsigned sessionID,                ///< Session ID for RTP channel
      const H245_H2250LogicalChannelParameters * param,
                                         ///< Parameters for channel
      RTP_QOS * rtpqos = NULL            ///< QoS for RTP
    );

    /**This function is called when the remote endpoint want's to create
       a new channel.

       If the return value is FALSE then the open is rejected using the
       errorCode as the cause, this would be a value from the enum
       H245_OpenLogicalChannelReject_cause::Choices.

       The default behaviour checks the capability set for if this capability
       is allowed to be opened with other channels that may already be open.
     */
    virtual BOOL OnCreateLogicalChannel(
      const H323Capability & capability,  ///< Capability for the channel open
      H323Channel::Directions dir,        ///< Direction of channel
      unsigned & errorCode                ///< Error to return if refused
    );

    /**Call back function when a logical channel thread begins.

       The default behaviour does nothing and returns TRUE.
      */
    virtual BOOL OnStartLogicalChannel(
      H323Channel & channel    ///< Channel that has been started.
    );

#ifndef NO_H323_AUDIO_CODECS
    /**Open a channel for use by an audio codec.
       The H323AudioCodec class will use this function to open the channel to
       read/write PCM data.

       The default behaviour calls the equivalent function on the endpoint.
      */
    virtual BOOL OpenAudioChannel(
      BOOL isEncoding,       ///< Direction of data flow
      unsigned bufferSize,   ///< Size of each sound buffer
      H323AudioCodec & codec ///< codec that is doing the opening
    );
#endif

#ifndef NO_H323_VIDEO
    /**Open a channel for use by an video codec.
       The H323VideoCodec class will use this function to open the channel to
       read/write image data.

       The default behaviour calls the equivalent function on the endpoint.
      */
    virtual BOOL OpenVideoChannel(
      BOOL isEncoding,       ///< Direction of data flow
      H323VideoCodec & codec ///< codec doing the opening
    );

#ifdef H323_H239
	/** Open an Extended Video Session
	    This will open an Extended Video session.
	*/
    BOOL OpenExtendedVideoSession(H323ChannelNumber & num);

	/** On Received an Extended Video OLC
	    This indicates the receipt of Extended Video OLC
	*/
	virtual void OnReceivedExtendedVideoSession(
		   unsigned /*role*/,                           ///< role 1-Presentation 2-Live
		   const H323ChannelNumber & /*channelnum*/     ///< Channel number of just opened channel
	) const {};
    
	/** Close an Extended Video Session matching the channel number
	    This will close the Extended Video matching the channel number (if open)
	 */
    virtual BOOL CloseExtendedVideoSession(
       const H323ChannelNumber & num     ///< Channel number to close.
	);

    /**Open a channel for use by an extended video codec.
       The H323VideoCodec class will use this function to open the channel to
       read/write image data.

       The default behaviour returns FALSE.
      */
    virtual BOOL OpenExtendedVideoChannel(
      BOOL isEncoding,       ///< Direction of data flow
      H323VideoCodec & codec ///< codec doing the opening
    );

#endif // H323_H239

#endif // NO_H323_VIDEO

    /**Close a logical channel.
      */
    virtual void CloseLogicalChannel(
      unsigned number,    ///< Channel number to close.
      BOOL fromRemote     ///< Indicates close request of remote channel
    );

    /**Close a logical channel by number.
      */
    virtual void CloseLogicalChannelNumber(
      const H323ChannelNumber & number    ///< Channel number to close.
    );

    /**Close a logical channel.
      */
    virtual void CloseAllLogicalChannels(
      BOOL fromRemote     ///< Indicates close request of remote channel
    );

    /**This function is called when the remote endpoint has closed down
       a logical channel.

       The default behaviour does nothing.
     */
    virtual void OnClosedLogicalChannel(
      const H323Channel & channel   ///< Channel that was closed
    );

    /**This function is called when the remote endpoint request the close of
       a logical channel.

       The application may get an opportunity to refuse to close the channel by
       returning FALSE from this function.

       The default behaviour returns TRUE.
     */
    virtual BOOL OnClosingLogicalChannel(
      H323Channel & channel   ///< Channel that is to be closed
    );

    /**This function is called when the remote endpoint wishes to limit the
       bit rate being sent on a channel.

       If channel is NULL, then the bit rate limit applies to all channels.

       The default behaviour does nothing if channel is NULL, otherwise calls
       H323Channel::OnFlowControl() on the specific channel.
     */
    virtual void OnLogicalChannelFlowControl(
      H323Channel * channel,   ///< Channel that is to be limited
      long bitRateRestriction  ///< Limit for channel
    );

    /**This function is called when the remote endpoint indicates the level
       of jitter estimated by the receiver.

       If channel is NULL, then the jitter applies to all channels.

       The default behaviour does nothing if channel is NULL, otherwise calls
       H323Channel::OnJitter() on the specific channel.
     */
    virtual void OnLogicalChannelJitter(
      H323Channel * channel,   ///< Channel that is to be limited
      DWORD jitter,            ///< Estimated received jitter in microseconds
      int skippedFrameCount,   ///< Frames skipped by decodec
      int additionalBuffer     ///< Additional size of video decoder buffer
    );

    /**Send a miscellaneous command on the associated H245 channel.
    */
    virtual void SendLogicalChannelMiscCommand(
      H323Channel & channel,  ///< Channel to send command for
      unsigned command        ///< Command code to send
    );

    /**Get a logical channel.
       Locates the specified channel number and returns a pointer to it.
      */
    H323Channel * GetLogicalChannel(
      unsigned number,    ///< Channel number to get.
      BOOL fromRemote     ///< Indicates get a remote channel
    ) const;

    /**Find a logical channel.
       Locates a channel give a RTP session ID. Each session would usually
       have two logical channels associated with it, so the fromRemote flag
       bay be used to distinguish which channel to return.
      */
    H323Channel * FindChannel(
      unsigned sessionId,   ///< Session ID to search for.
      BOOL fromRemote       ///< Indicates the direction of RTP data.
    ) const;
  //@}

  /**@name Bandwidth Management */
  //@{
    /**Get the bandwidth currently used.
       This totals the open channels and returns the total bandwidth used in
       100's of bits/sec
      */
    unsigned GetBandwidthUsed() const;

    /**Request use the available bandwidth in 100's of bits/sec.
       If there is insufficient bandwidth available, FALSE is returned. If
       sufficient bandwidth is available, then TRUE is returned and the amount
       of available bandwidth is reduced by the specified amount.
      */
    BOOL UseBandwidth(
      unsigned bandwidth,     ///< Bandwidth required
      BOOL removing           ///< Flag for adding/removing bandwidth usage
    );

    /**Get the available bandwidth in 100's of bits/sec.
      */
    unsigned GetBandwidthAvailable() const { return bandwidthAvailable; }

    /**Set the available bandwidth in 100's of bits/sec.
       Note if the force parameter is TRUE this function will close down
       active logical channels to meet the new bandwidth requirement.
      */
    BOOL SetBandwidthAvailable(
      unsigned newBandwidth,    ///< New bandwidth limit
      BOOL force = FALSE        ///< Force bandwidth limit
    );
  //@}

  /**@name Indications */
  //@{
    enum SendUserInputModes {
      SendUserInputAsQ931,
      SendUserInputAsString,
      SendUserInputAsTone,
      SendUserInputAsInlineRFC2833,
      SendUserInputAsSeparateRFC2833,  // Not implemented
#ifdef H323_H249
      SendUserInputAsNavigation,
      SendUserInputAsSoftkey,
      SendUserInputAsPointDevice,
      SendUserInputAsModal,
#endif
      NumSendUserInputModes
    };
#if PTRACING
    friend ostream & operator<<(ostream & o, SendUserInputModes m);
#endif

    /**Set the user input indication transmission mode.
      */
    void SetSendUserInputMode(SendUserInputModes mode);

    /**Get the user input indication transmission mode.
      */
    SendUserInputModes GetSendUserInputMode() const { return sendUserInputMode; }

    /**Get the real user input indication transmission mode.
       This will return the user input mode that will actually be used for
       transmissions. It will be the value of GetSendUserInputMode() provided
       the remote endpoint is capable of that mode.
      */
    SendUserInputModes GetRealSendUserInputMode() const;

    /**Send a user input indication to the remote endpoint.
       This is for sending arbitrary strings as user indications.

       The user indication is sent according to the sendUserInputMode member
       variable. If SendUserInputAsString then this uses an H.245 "string"
       UserInputIndication pdu sending the entire string in one go. If
       SendUserInputAsTone then a separate H.245 "signal" UserInputIndication
       pdu is sent for each character. If SendUserInputAsInlineRFC2833 then
       the indication is inserted into the outgoing audio stream as an RFC2833
       RTP data pdu.

       SendUserInputAsSeparateRFC2833 is not yet supported.
      */
    virtual void SendUserInput(
      const PString & value                   ///< String value of indication
    );

    /**Call back for remote endpoint has sent user input.
       This will be called irrespective of the source (H.245 string, H.245
       signal or RFC2833).

       The default behaviour calls the endpoint function of the same name.
      */
    virtual void OnUserInputString(
      const PString & value   ///< String value of indication
    );

    /**Send a user input indication to the remote endpoint.
       This sends DTMF emulation user input. If something more sophisticated
       than the simple tones that can be sent using the SendUserInput()
       function.

       A duration of zero indicates that no duration is to be indicated.
       A non-zero logical channel indicates that the tone is to be syncronised
       with the logical channel at the rtpTimestamp value specified.

       The tone parameter must be one of "0123456789#*ABCD!" where '!'
       indicates a hook flash. If tone is a ' ' character then a
       signalUpdate PDU is sent that updates the last tone indication
       sent. See the H.245 specifcation for more details on this.

       The user indication is sent according to the sendUserInputMode member
       variable. If SendUserInputAsString then this uses an H.245 "string"
       UserInputIndication pdu sending the entire string in one go. If
       SendUserInputAsTone then a separate H.245 "signal" UserInputIndication
       pdu is sent for each character. If SendUserInputAsInlineRFC2833 then
       the indication is inserted into the outgoing audio stream as an RFC2833
       RTP data pdu.

       SendUserInputAsSeparateRFC2833 is not yet supported.
      */
    virtual void SendUserInputTone(
      char tone,                   ///< DTMF tone code
      unsigned duration = 0,       ///< Duration of tone in milliseconds
      unsigned logicalChannel = 0, ///< Logical channel number for RTP sync.
      unsigned rtpTimestamp = 0    ///< RTP timestamp in logical channel sync.
    );

    /**Call back for remote enpoint has sent user input.

       The default behaviour calls the endpoint function of the same name.
      */
    virtual void OnUserInputTone(
      char tone,               ///< DTMF tone code
      unsigned duration,       ///< Duration of tone in milliseconds
      unsigned logicalChannel, ///< Logical channel number for RTP sync.
      unsigned rtpTimestamp    ///< RTP timestamp in logical channel sync.
    );

    /**Send a user input indication to the remote endpoint.
       This sends a Hook Flash emulation user input.
      */
    void SendUserInputHookFlash(
      int duration = 500  ///< Duration of tone in milliseconds
    ) { SendUserInputTone('!', duration); }

    /**Send a user input indication to the remote endpoint.
       This is for sending arbitrary strings as user indications.

       This always uses a Q.931 Keypad Information Element in a Information
       pdu sending the entire string in one go.
      */
    virtual void SendUserInputIndicationQ931(
      const PString & value                   ///< String value of indication
    );

    /**Send a user input indication to the remote endpoint.
       This is for sending arbitrary strings as user indications.

       This always uses an H.245 "string" UserInputIndication pdu sending the
       entire string in one go.
      */
    virtual void SendUserInputIndicationString(
      const PString & value                   ///< String value of indication
    );

    /**Send a user input indication to the remote endpoint.
       This sends DTMF emulation user input.This uses an H.245 "signal"
       UserInputIndication pdu.
      */
    virtual void SendUserInputIndicationTone(
      char tone,                   ///< DTMF tone code
      unsigned duration = 0,       ///< Duration of tone in milliseconds
      unsigned logicalChannel = 0, ///< Logical channel number for RTP sync.
      unsigned rtpTimestamp = 0    ///< RTP timestamp in logical channel sync.
    );

#ifdef H323_H249

    /**Send a user input indication to the remote endpoint.
       This sends a H.249 Annex A Navigation user input. 
      */
	virtual void SendUserInputIndicationNavigate(
		H323_UserInputCapability::NavigateKeyID keyID
	);

    /**Send a user input indication to the remote endpoint.
       This Receives/Sends a H.249 Annex A Navigation user input. 
      */
	virtual void OnUserInputIndicationNavigate(
		const H245_ArrayOf_GenericParameter & contents
	);
	
    /**Send a user input indication to the remote endpoint.
       This Receives/Sends a H.249 Annex B Softkey user input. 
      */
    virtual void SendUserInputIndicationSoftkey(
		unsigned key, 
		const PString & keyName = PString()
	);

    virtual void OnUserInputIndicationSoftkey(
		const H245_ArrayOf_GenericParameter & contents
	);

    /**Send a user input indication to the remote endpoint.
       This Receives/Sends a H.249 Annex C Point Device user input. 
      */
    virtual void SendUserInputIndicationPointDevice(
	                    unsigned x,              ///< X coord 
						unsigned y,              ///< Y coord 
						unsigned button=0,       ///< Mouse Button 1 = left 2 = right
						unsigned buttonstate=0,  ///< Button state 1 = button down 2 = button up
						unsigned clickcount=0    ///< ClickCount 1 = sigle click 2= doubleclick
    );

    virtual void OnUserInputIndicationPointDevice(
		const H245_ArrayOf_GenericParameter & contents
	);
	
    /**Send a user input indication to the remote endpoint.
       This Receives/Sends a H.249 Annex D Softkey user input. 
      */
	virtual void SendUserInputIndicationModal();

    virtual void OnUserInputIndicationModal(
		const H245_ArrayOf_GenericParameter & contents
	);
	
#endif


    /**Send a user input indication to the remote endpoint.
       The two forms are for basic user input of a simple string using the
       SendUserInput() function or a full DTMF emulation user input using the
       SendUserInputTone() function.

       An application could do more sophisticated usage by filling in the 
       H245_UserInputIndication structure directly ans using this function.
      */
    virtual void SendUserInputIndication(
      const H245_UserInputIndication & pdu    ///< Full user indication PDU
    );

    /**Call back for remote enpoint has sent user input.
       The default behaviour calls OnUserInputString() if the PDU is of the
       alphanumeric type, or OnUserInputTone() if of a tone type.
      */
    virtual void OnUserInputIndication(
      const H245_UserInputIndication & pdu  ///< Full user indication PDU
    );
  //@}

  /**@name RTP Session Management */
  //@{
    /**Get an RTP session for the specified ID.
       If there is no session of the specified ID, NULL is returned.
      */
    virtual RTP_Session * GetSession(
      unsigned sessionID
    ) const;

    /**Get an H323 RTP session for the specified ID.
       If there is no session of the specified ID, NULL is returned.
      */
    virtual H323_RTP_Session * GetSessionCallbacks(
      unsigned sessionID
    ) const;

    /**Use an RTP session for the specified ID and for the given direction.
       If there is no session of the specified ID, a new one is created using
       the information provided in the H245_TransportAddress PDU. If the system
       does not support the specified transport, NULL is returned.

       If this function is used, then the ReleaseSession() function MUST be
       called or the session is never deleted for the lifetime of the H323
       connection.
      */
    virtual RTP_Session * UseSession(
      unsigned sessionID,
      const H245_TransportAddress & pdu,
      H323Channel::Directions dir,
      RTP_QOS * rtpqos = NULL
    );

    /**Release the session. If the session ID is not being used any more any
       clients via the UseSession() function, then the session is deleted.
     */
    virtual void ReleaseSession(
      unsigned sessionID
    );

	/**Received RTP alternate Information. This is used to supply alternate RTP 
	   destination information in the generic information field in the OLC for the
	   purpose of probing for an alternate route to the remote party.
	  */
	virtual BOOL OnReceiveRTPAltInformation(H323_RTP_UDP & rtp, 
	                    const H245_ArrayOf_GenericInformation & alternate) const;

	/**Send RTP alternate Information. This is used to supply alternate RTP 
	   destination information in the generic information field in the OLC for the
	   purpose of probing for an alternate route to the remote party.
	  */
    virtual BOOL OnSendingRTPAltInformation(const H323_RTP_UDP & rtp,
				        H245_ArrayOf_GenericInformation & alternate) const;


	/**Send Generic Information with the OLC. This is used to include generic
	   information in the openlogicalchannel
	  */
    virtual BOOL OnSendingOLCGenericInformation(const H323_RTP_UDP & rtp,
				        H245_ArrayOf_GenericInformation & generic) const;

    /**Callback from the RTP session for statistics monitoring.
       This is called every so many packets on the transmitter and receiver
       threads of the RTP session indicating that the statistics have been
       updated.

       The default behaviour calls H323EndPoint::OnRTPStatistics().
      */
    virtual void OnRTPStatistics(
      const RTP_Session & session   ///< Session with statistics
    ) const;

    /**Callback from the RTP session for statistics monitoring.
       This is called at the end of the RTP session indicating that the statistics 
       of the call

       The default behaviour calls H323EndPoint::OnFinalRTPStatistics().
      */
    virtual void OnRTPFinalStatistics(
      const RTP_Session & session   ///< Session with statistics
    ) const;

    /**Get the names of the codecs in use for the RTP session.
       If there is no session of the specified ID, an empty string is returned.
      */
    virtual PString GetSessionCodecNames(
      unsigned sessionID
    ) const;

    /** Return TRUE if the remote appears to be behind a NAT firewall
    */
    BOOL IsBehindNAT() const
    { return remoteIsNAT; }

    /** Set Remote is behind NAT
    */
	void SetRemoteNAT()
	{ remoteIsNAT = TRUE; }

	/** Is NAT Support Available
	  */
	BOOL HasNATSupport() const
	{ return NATsupport; }

	/** Disable NAT Support for allocation of RTP sockets
	  */
	void DisableNATSupport()
	{ NATsupport = FALSE; remoteIsNAT = FALSE; }
	
	/** Set the information that the call parties are 
	    behind the same NAT device
	  */
	void SetSameNAT() { sameNAT = TRUE; };

	/** Determine if the two parties are behind the same NAT
	  */
	BOOL isSameNAT() const { return sameNAT; };

#if P_STUN
	/** On Set RTP information from H.225 & H.245 signalling
	 */
	virtual void OnSetRTPNat(unsigned sessionid, PNatMethod & nat) const;

	/** Set RTP NAT information callback
	  */
	virtual void SetRTPNAT(unsigned /*sessionid*/, PUDPSocket * /*socket*/) {};

#endif
	/** Set Endpoint Type Information
	  Override this to advertise the Endpoint type on a Call by Call basis

      The default behaviour calls H323EndPoint::SetEndpointTypeInfo().
	  */
    virtual void SetEndpointTypeInfo(H225_EndpointType & info) const;

  //@}

  /**@name Request Mode Changes */
  //@{
    /**Make a request to mode change to remote.
       This asks the remote system to stop it transmitters and start sending
       one of the combinations specifed.

       The modes are separated in the string by \n characters, and all of the
       channels (capabilities) are strings separated by \t characters. Thus a
       very simple mode change would be "T.38" which requests that the remote
       start sending T.38 data and nothing else. A more complicated example
       would be "G.723\tH.261\nG.729\tH.261\nG.728" which indicates that the
       remote should either start sending G.723 and H.261, G.729 and H.261 or
       just G.728 on its own.

       Returns FALSE if a mode change is currently in progress, only one mode
       change may be done at a time.
      */
    virtual BOOL RequestModeChange(
      const PString & newModes  ///< New modes to select
    );

    /**Make a request to mode change to remote.
       This asks the remote system to stop it transmitters and start sending
       one of the combinations specifed.

       Returns FALSE if a mode change is currently in progress, only one mode
       change may be done at a time.
      */
    virtual BOOL RequestModeChange(
      const H245_ArrayOf_ModeDescription & newModes  ///< New modes to select
    );

    /**Received request for mode change from remote.
      */
    virtual BOOL OnRequestModeChange(
      const H245_RequestMode & pdu,     ///< Received PDU
      H245_RequestModeAck & ack,        ///< Ack PDU to send
      H245_RequestModeReject & reject,  ///< Reject PDU to send
      PINDEX & selectedMode           ///< Which mode was selected
    );

    /**Completed request for mode change from remote.
       This is a call back that accurs after the ack has been sent to the
       remote as indicated by the OnRequestModeChange() return result. This
       function is intended to actually implement the mode change after it
       had been accepted.
      */
    virtual void OnModeChanged(
      const H245_ModeDescription & newMode
    );

    /**Received acceptance of last mode change request.
       This callback indicates that the RequestModeChange() was accepted by
       the remote endpoint.
      */
    virtual void OnAcceptModeChange(
      const H245_RequestModeAck & pdu  ///< Received PDU
    );

    /**Received reject of last mode change request.
       This callback indicates that the RequestModeChange() was accepted by
       the remote endpoint.
      */
    virtual void OnRefusedModeChange(
      const H245_RequestModeReject * pdu  ///< Received PDU, if NULL is a timeout
    );
  //@}

#ifdef H323_T120
  /**@name Other services */
  //@{
    /**Create an instance of the T.120 protocol handler.
       This is called when the OpenLogicalChannel subsystem requires that
       a T.120 channel be established.

       Note that if the application overrides this and returns a pointer to a
       heap variable (using new) then it is the responsibility of the creator
       to subsequently delete the object. The user of this function (the 
       H323_T120Channel class) will not do so.

       The default behavour returns H323Endpoint::CreateT120ProtocolHandler()
       while keeping track of that variable for autmatic deletion.
      */
    virtual OpalT120Protocol * CreateT120ProtocolHandler();
#endif

#ifdef H323_T38
    /**Create an instance of the T.38 protocol handler.
       This is called when the OpenLogicalChannel subsystem requires that
       a T.38 fax channel be established.

       Note that if the application overrides this and returns a pointer to a
       heap variable (using new) then it is the responsibility of the creator
       to subsequently delete the object. The user of this function (the 
       H323_T38Channel class) will not do so.

       The default behavour returns H323Endpoint::CreateT38ProtocolHandler()
       while keeping track of that variable for autmatic deletion.
      */
    virtual OpalT38Protocol * CreateT38ProtocolHandler();

    /**Request a mode change to T.38 data.
      */
    virtual BOOL RequestModeChangeT38(
      const char * capabilityNames = "T.38\nT38FaxUDP"
    );
#endif

#ifdef H323_H224

    /** Create an instance of the H.224 protocol handler.
        This is called when the subsystem requires that a H.224 channel be established.
  		
        Note that if the application overrides this it should return a pointer
        to a heap variable (using new) as it will be automatically deleted when
        the H323Connection is deleted.
 	
        The default behaviour calls the OpalEndpoint function of the same name if
        there is not already a H.224 handler associated with this connection. If there
        is already such a H.224 handler associated, this instance is returned instead.
    */
    virtual OpalH224Handler *CreateH224ProtocolHandler(unsigned sessionID);
  	
    /** Create an instance of the H.281 protocol handler.
        This is called when the subsystem requires that a H.224 channel be established.
  		
        Note that if the application overrides this it should return a pointer
        to a heap variable (using new) as it will be automatically deleted when
        the associated H.224 handler is deleted.
  		
     The default behavour returns H323Endpoint::CreateH224ProtocolHandler()
     */
     virtual OpalH281Handler *CreateH281ProtocolHandler(OpalH224Handler & h224Handler);
#endif

#ifdef H323_FILE
	/** Open an File Transfer Session
	    Use this to open a file transfer session for the tranferring of files
		between H323 clients.
	*/
     BOOL OpenFileTransferSession(H323ChannelNumber & num   ///< Created Channel number
		                          );

    /** Create an instance of the File Transfer handler.
        This is called when the subsystem requires that a a file transfer channel be established.
  		
        Note that if the application overrides this it should return a pointer
        to a heap variable (using new) as it will be automatically deleted when
        the H323Connection is deleted.
 	
        The default behaviour calls the OpalEndpoint function of the same name if
        there is not already a H.224 handler associated with this connection. If there
        is already such a H.224 handler associated, this instance is returned instead.
    */
     virtual H323FileTransferHandler *CreateFileTransferHandler(unsigned sessionID,           ///< Session Identifier
															H323Channel::Directions dir,      ///< direction of channel
						                                    H323FileTransferList & filelist   ///< Transfer File List
															);

	/** Open a File Transfer Channel.
        This is called when the subsystem requires that a File Transfer channel be established.

		An implementer should override this function to facilitate file transfer. 
		If transmitting, list of files should be populated to notify the channel which files to read.
		If receiving, the list of files should be altered to include path information for the storage
		of received files.
		
        The default behaviour returns FALSE to indicate File Transfer is not implemented. 
      */
      virtual BOOL OpenFileTransferChannel( H323Channel::Directions dir,           ///< direction of channel
						                  H323FileTransferList & filelist          ///< Transfer File List
										 ); 
#endif

    /**Get separate H.235 authentication for the connection.
       This allows an individual ARQ to override the authentical credentials
       used in H.235 based RAS for this particular connection.

       A return value of FALSE indicates to use the default credentials of the
       endpoint, while TRUE indicates that new credentials are to be used.

       The default behavour does nothing and returns FALSE.
      */
    virtual BOOL GetAdmissionRequestAuthentication(
      const H225_AdmissionRequest & arq,  ///< ARQ being constructed
      H235Authenticators & authenticators ///< New authenticators for ARQ
    );
  //@}

  /**@name Member variable access */
  //@{
    /**Get the owner endpoint for this connection.
     */
    H323EndPoint & GetEndPoint() const { return endpoint; }

    /**Get the call direction for this connection.
     */
    BOOL HadAnsweredCall() const { return callAnswered; }

    /**Determined if connection is gatekeeper routed.
     */
    BOOL IsGatekeeperRouted() const { return gatekeeperRouted; }

    /**Get the Q.931 cause code (Q.850) that terminated this call.
       See Q931::CauseValues for common values.
     */
    unsigned GetQ931Cause() const { return q931Cause; }

    /**Set the outgoing Q.931 cause code (Q.850) that is sent for this call
       See Q931::CauseValues for common values.
     */
    void SetQ931Cause(unsigned v) { q931Cause = v; }

    /**Get the distinctive ring code for incoming call.
       This returns an integer from 0 to 7 that may indicate to an application
       that different ring cadences are to be used.
      */
    unsigned GetDistinctiveRing() const { return distinctiveRing; }

    /**Set the distinctive ring code for outgoing call.
       This sets the integer from 0 to 7 that will be used in the outgoing
       Setup PDU. Note this must be called either immediately after
       construction or during the OnSendSignalSetup() callback function so the
       member variable is set befor ethe PDU is sent.
      */
    void SetDistinctiveRing(unsigned pattern) { distinctiveRing = pattern&7; }

    /**Get the internal OpenH323 call token for this connection.
     */
    const PString & GetCallToken() const { return callToken; }

    /**Get the call reference for this connection.
     */
    unsigned GetCallReference() const { return callReference; }

    /**Get the call identifier for this connection.
     */
    const OpalGloballyUniqueID & GetCallIdentifier() const { return callIdentifier; }

    /**Get the conference identifier for this connection.
     */
    const OpalGloballyUniqueID & GetConferenceIdentifier() const { return conferenceIdentifier; }

    /**Get the local name/alias.
      */
    const PString & GetLocalPartyName() const { return localPartyName; }

    /**Set the local name/alias from information in the PDU.
      */
    void SetLocalPartyName(const PString & name);

    /**Set the local Q.931 display name
      */
    void SetDisplayName(const PString & name) { localDisplayName = name; }

    /**Get the local Q.931 display name
      */
    const PString GetDisplayName() const { return localDisplayName; }

    const PStringList & GetLocalAliasNames() const { return localAliasNames; }

    /**Get the remote party name.
       This returns a string indicating the remote parties names and aliases.
       This can be a complicated string containing all the aliases and the
       remote host name. For example:
              "Fred Nurk (fred, 5551234) [fred.nurk.com]"
      */
    virtual const PString & GetRemotePartyName() const { return remotePartyName; }

    /**Get the remote party number, if there was one one.
       If the remote party has indicated an e164 number as one of its aliases
       or as a field in the Q.931 PDU, then this function will return it.
      */
    const PString & GetRemotePartyNumber() const { return remotePartyNumber; }

    /**Get the remote party address.
       This will return the "best guess" at an address to use in a
       H323EndPoint::MakeCall() function to call the remote party back again.
       Note that due to the presence of gatekeepers/proxies etc this may not
       always be accurate.
      */
    const PString & GetRemotePartyAddress() const { return remotePartyAddress; }

    /**Get the remote party Alias List, if there was one.
      */
	const PStringArray & GetRemotePartyAliases() const { return remoteAliasNames; }

    /**Set the name/alias of remote end from information in the PDU.
      */
    void SetRemotePartyInfo(
      const H323SignalPDU & pdu ///< PDU from which to extract party info.
    );

    /**Get the remote application name and version.
       This information is obtained from the sourceInfo field of the H.225
       Setup PDU or the destinationInfo of the call proceeding or alerting
       PDU's. The general format of the string will be information extracted
       from the VendorIdentifier field of the EndpointType. In particular:

          productId <tab> versionId <tab> t35CountryCode/manufacturerCode

       for example
          "Equivalence OpenPhone\t1.4.2\t9/61"
      */
    const PString & GetRemoteApplication() const { return remoteApplication; }

    /**Set the name/alias of remote end from information in the PDU.
      */
    void SetRemoteApplication(
      const H225_EndpointType & pdu ///< PDU from which to extract application info.
    );
    
    /**Get the remotes capability table for this connection.
     */
    const H323Capabilities & GetLocalCapabilities() const { return localCapabilities; }

    /**Get the remotes capability table for this connection.
     */
    const H323Capabilities & GetRemoteCapabilities() const { return remoteCapabilities; }

    /**Get the maximum audio jitter delay.
     */
    unsigned GetRemoteMaxAudioDelayJitter() const { return remoteMaxAudioDelayJitter; }

    /**Get the signalling channel being used.
      */
    const H323Transport * GetSignallingChannel() const { return signallingChannel; }

    /**Get the signalling channel protocol version number.
      */
    unsigned GetSignallingVersion() const { return h225version; }

    /**Get the control channel being used (may return signalling channel).
      */
    const H323Transport & GetControlChannel() const;

    /**Get the control channel protocol version number.
      */
    unsigned GetControlVersion() const { return h245version; }

    /**Get the time at which the connection was begun
      */
    PTime GetSetupUpTime() const { return setupTime; }

    /**Get the time at which the ALERTING was received
      */
    PTime GetAlertingTime() const { return alertingTime; }

    /**Get the time at which the connection was connected. That is the point
       at which charging is likely to have begun.
      */
    PTime GetConnectionStartTime() const { return connectedTime; }

    /**Get the time at which the connection was cleared
      */
    PTime GetConnectionEndTime() const { return callEndTime; }

    /**Get the time at which the remote opened a media channel
      */
    PTime GetReverseMediaOpenTime() const { return reverseMediaOpenTime; }

    /**Get the default maximum audio jitter delay parameter.
       Defaults to 50ms
     */
    unsigned GetMinAudioJitterDelay() const { return minAudioJitterDelay; }

    /**Get the default maximum audio delay jitter parameter.
       Defaults to 250ms.
     */
    unsigned GetMaxAudioJitterDelay() const { return maxAudioJitterDelay; }

    /**Set the maximum audio delay jitter parameter.
     */
    void SetAudioJitterDelay(
      unsigned minDelay,   ///< New minimum jitter buffer delay in milliseconds
      unsigned maxDelay    ///< New maximum jitter buffer delay in milliseconds
    );

    /**Get the UUIE PDU monitor bit mask.
     */
    unsigned GetUUIEsRequested() const { return uuiesRequested; }

    /**Set the UUIE PDU monitor bit mask.
     */
    void SetUUIEsRequested(unsigned mask) { uuiesRequested = mask; }

    /**Get the iNow Gatekeeper Access Token OID.
     */
    const PString GetGkAccessTokenOID() const { return gkAccessTokenOID; }

    /**Set the iNow Gatekeeper Access Token OID.
     */
    void SetGkAccessTokenOID(const PString & oid) { gkAccessTokenOID = oid; }

    /**Get the iNow Gatekeeper Access Token data.
     */
    const PBYTEArray & GetGkAccessTokenData() const { return gkAccessTokenData; }

    /**Set the Destionation Extra Call Info memeber.
     */
    void SetDestExtraCallInfo(
      const PString & info
    ) { destExtraCallInfo = info; }

    /** Set the remote call waiting flag
     */
    void SetRemoteCallWaiting(const unsigned value) { remoteCallWaiting = value; }

    /**How many caller's are waiting on the remote endpoint?
      -1 - None
       0 - Just this connection
       n - n plus this connection
     */
    const int GetRemoteCallWaiting() const { return remoteCallWaiting; }

    /**Set the enforced duration limit for the call.
       This starts a timer that will automatically shut down the call when it
       expires.
      */
    void SetEnforcedDurationLimit(
      unsigned seconds  ///< max duration of call in seconds
    );

#ifdef H323_H460
	/** Disable Feautures on a call by call basis
	  */
	void DisableFeatures();
#endif

    virtual BOOL OnSendFeatureSet(unsigned, H225_FeatureSet &) const;

    virtual void OnReceiveFeatureSet(unsigned, const H225_FeatureSet &) const;

  //@}

#ifndef DISABLE_CALLAUTH
  /**@name Endpoint Authentication */
  //@{
     /** Get Endpoint Authenticator mothods
       */
    const H235Authenticators & GetEPAuthenticators() const;

    /** Set Authentication to support Validation CallBack
	OnCallAuthentication is called when a EPAuthentication CryptoToken 
	is Validated.
      */
    virtual void SetAuthenticationConnection();

    /** EP Authentication CallBack to check username and get Password
	for Call Authentication. By Default it calls the corresponding
	Endpoint Function. 
      */
    virtual BOOL OnCallAuthentication(const PString & username, 
                                      PString & password);

    /** EP Authentication CallBack to allow the connection to approve the authentication
	   even if it has failed. Use this to override authentication on a call by call basis
	   Return TRUE to Authenticate.
      */
	virtual BOOL OnEPAuthenticationFailed(H235Authenticator::ValidationResult result) const;

    /** EP Authentication Finalise Callback. After the PDU has been built this
        Callback allows the Authentication mechanisms to finalise the PDU.
      */
    virtual void OnAuthenticationFinalise(unsigned pdu,PBYTEArray & rawData);

    /** Whether the Call has Authentication
	  */
    BOOL HasAuthentication() const
       { return hasAuthentication; }

    /** Whether the Authentication has Failed
	  */
	BOOL HasAuthenticationFailed() 
	   { return AuthenticationFailed; };
  //@}
#endif

#ifdef H323_H248
  /**@name Call Service Control Session */
  //@{
    /** On Send Service Control Session
      */
    BOOL OnSendServiceControlSessions(
	         H225_ArrayOf_ServiceControlSession & serviceControl, ///< Service control PDU
                 H225_ServiceControlSession_reason reason           ///< Reason for Service Control
		) const;

    /** On Receive Service Control Session
      */
    void OnReceiveServiceControlSessions(
		const H225_ArrayOf_ServiceControlSession & serviceControl  ///< Service control PDU
		);

    /** On Send Call Credit 
      */
    virtual void OnReceiveServiceControl(const PString & amount,    ///< Current Balance 
                                         BOOL credit,               ///< Debit or Credit
                                         const unsigned & timelimit,///< Time Remaining
                                         const PString & url,	    ///< url for TopUp
	 									 const PString & ldapURL,   ///< LDAP URL
									     const PString & baseDN     ///< LDAP base DN
									    );

    /** On Receive Call Credit
     */
    virtual BOOL OnSendServiceControl(PString & amount,          ///< Current Balance 
                                      BOOL credit,               ///< Debit or Credit
                                      unsigned & timelimit,      ///< Time Remaining
                                      PString & url		 ///< url for TopUp
								     ) const;
  //@}
#endif

  /**@name Feature Related Function */
  //@{
    /** Disable H245 in Setup
      */
    virtual void DisableH245inSETUP();

    /** Disable QoS in H.245
      */
	virtual void DisableH245QoS();

    /** Has QoS in H.245
      */
    virtual BOOL H245QoSEnabled() const;


    /** Set the connection as a non Standard Call
      */
    virtual void SetNonCallConnection();

#ifdef H323_H460
    /** Get the connection FeatureSet
      */
    virtual H460_FeatureSet * GetFeatureSet();
#endif
  //@}

    enum ReleaseSequence {
      ReleaseSequenceUnknown,
      ReleaseSequence_Local,
      ReleaseSequence_Remote
    };

    ReleaseSequence GetReleaseSequence() const
    { return releaseSequence; }

    virtual BOOL OnHandleConferenceRequest(const H245_ConferenceRequest &)
    { return FALSE; }

    virtual BOOL OnHandleConferenceResponse(const H245_ConferenceResponse &)
    { return FALSE; }

    virtual BOOL OnHandleConferenceCommand(const H245_ConferenceCommand &)
    { return FALSE; }

    virtual BOOL OnHandleConferenceIndication(const H245_ConferenceIndication &)
    { return FALSE; }

    enum h245MessageType {
      h245request,
      h245response,
      h245command,
	  h245indication
    };

	virtual BOOL OnHandleH245GenericMessage(h245MessageType, const H245_GenericMessage &)
	{ return FALSE; }

#ifdef H323_H230
    /** Open Conference Controls
      */
	BOOL OpenConferenceControlSession(
				        BOOL & chairControl,
		                BOOL & extControls
						);


#endif

  protected:
    /**Internal function to check if call established.
       This checks all the criteria for establishing a call an initiating the
       starting of media channels, if they have not already been started via
       the fast start algorithm.
    */
    virtual void InternalEstablishedConnectionCheck();
    BOOL DecodeFastStartCaps(const H225_ArrayOf_PASN_OctetString & fastStartCaps);
    BOOL InternalEndSessionCheck(PPER_Stream & strm);
    void SetRemoteVersions(const H225_ProtocolIdentifier & id);
    void MonitorCallStatus();
    PDECLARE_NOTIFIER(OpalRFC2833Info, H323Connection, OnUserInputInlineRFC2833);
    PDECLARE_NOTIFIER(H323Codec::FilterInfo, H323Connection, OnUserInputInBandDTMF);

    H323EndPoint & endpoint;
    PSyncPoint     * endSync;

    int                  remoteCallWaiting; // Number of call's waiting at the remote endpoint
    BOOL                 callAnswered;
    BOOL                 gatekeeperRouted;
    unsigned             distinctiveRing;
    PString              callToken;
    unsigned             callReference;
    OpalGloballyUniqueID callIdentifier;
    OpalGloballyUniqueID conferenceIdentifier;

    PStringList        localAliasNames;
    PString            localPartyName;
    PString            localDisplayName;
    H323Capabilities   localCapabilities; // Capabilities local system supports
    PString            remotePartyName;
    PString            remotePartyNumber;
    PString            remotePartyAddress;
	PStringArray       remoteAliasNames;
    PString            destExtraCallInfo;
    PString            remoteApplication;
    H323Capabilities   remoteCapabilities; // Capabilities remote system supports
    unsigned           remoteMaxAudioDelayJitter;
    PTimer             roundTripDelayTimer;
    unsigned           minAudioJitterDelay;
    unsigned           maxAudioJitterDelay;
    unsigned           bandwidthAvailable;
    unsigned           uuiesRequested;
    PString            gkAccessTokenOID;
    PBYTEArray         gkAccessTokenData;
    BOOL               addAccessTokenToSetup;
    SendUserInputModes sendUserInputMode;

    H323Transport * signallingChannel;
    H323Transport * controlChannel;
    BOOL            h245Tunneling;
    H323SignalPDU * h245TunnelRxPDU;
    H323SignalPDU * h245TunnelTxPDU;
    H323SignalPDU * alertingPDU;
    H323SignalPDU * connectPDU;

    enum ConnectionStates {
      NoConnectionActive,
      AwaitingGatekeeperAdmission,
      AwaitingTransportConnect,
      AwaitingSignalConnect,
      AwaitingLocalAnswer,
      HasExecutedSignalConnect,
      EstablishedConnection,
      ShuttingDownConnection,
      NumConnectionStates
    } connectionState;

    PTime         setupTime;
    PTime         alertingTime;
    PTime         connectedTime;
    PTime         callEndTime;
    PTime         reverseMediaOpenTime;
    CallEndReason callEndReason;
    unsigned      q931Cause;
    ReleaseSequence releaseSequence;

    unsigned   h225version;
    unsigned   h245version;
    BOOL       h245versionSet;
    BOOL doH245inSETUP;
    BOOL lastPDUWasH245inSETUP;
    BOOL detectInBandDTMF;
    BOOL mustSendDRQ;
    BOOL mediaWaitForConnect;
    BOOL transmitterSidePaused;
    BOOL earlyStart;
    BOOL doH245QoS;

#ifdef H323_T120
    BOOL startT120;
#endif

#ifdef H323_T38
    PString    t38ModeChangeCapabilities;
#endif

#ifdef H323_H224
	BOOL startH224;
#endif

    PSyncPoint digitsWaitFlag;
    BOOL       endSessionNeeded;
    BOOL       endSessionSent;
    PSyncPoint endSessionReceived;
    PTimer     enforcedDurationLimit;

#ifdef H323_H450
    // Used as part of a local call hold operation involving MOH
    PChannel * holdAudioMediaChannel;
    PChannel * holdVideoMediaChannel;
    BOOL       isConsultationTransfer;

    /** Call Intrusion flag and parameters */
    BOOL     isCallIntrusion;
    unsigned callIntrusionProtectionLevel;
#endif

    RTP_SessionManager rtpSessions;

    enum FastStartStates {
      FastStartDisabled,
      FastStartInitiate,
      FastStartResponse,
      FastStartAcknowledged,
      NumFastStartStates
    };
    FastStartStates        fastStartState;
    H323LogicalChannelList fastStartChannels;

#if PTRACING
    static const char * const ConnectionStatesNames[NumConnectionStates];
    friend ostream & operator<<(ostream & o, ConnectionStates s) { return o << ConnectionStatesNames[s]; }
    static const char * const FastStartStateNames[NumFastStartStates];
    friend ostream & operator<<(ostream & o, FastStartStates s) { return o << FastStartStateNames[s]; }
#endif


    // The following pointers are to protocol procedures, they are pointers to
    // hide their complexity from the H323Connection classes users.
    H245NegMasterSlaveDetermination  * masterSlaveDeterminationProcedure;
    H245NegTerminalCapabilitySet     * capabilityExchangeProcedure;
    H245NegLogicalChannels           * logicalChannels;
    H245NegRequestMode               * requestModeProcedure;
    H245NegRoundTripDelay            * roundTripDelayProcedure;

#ifdef H323_H450
    H450xDispatcher                  * h450dispatcher;
    H4502Handler                     * h4502handler;
    H4503Handler                     * h4503handler;
    H4504Handler                     * h4504handler;
    H4506Handler                     * h4506handler;
    H45011Handler                    * h45011handler;
#endif

    OpalRFC2833                      * rfc2833handler;

#ifdef H323_T120
    OpalT120Protocol                 * t120handler;
#endif

#ifdef H323_T38
    OpalT38Protocol                  * t38handler;
#endif

#ifdef H323_H224
    OpalH224Handler                  * h224handler;
    OpalH281Handler                  * h281handler;
#endif

#ifdef H323_FILE
	H323FileTransferHandler          * filehandler;
#endif

#ifdef P_DTMF
    // The In-Band DTMF detector. This is used inside an audio filter which is
    // added to the audio channel.
    PDTMFDecoder                     dtmfDecoder;
#endif

    // used to detect remote NAT endpoints
    BOOL remoteIsNAT;    ///< Remote Caller is NAT
	BOOL NATsupport;     ///< Disable support for NATed callers
	BOOL sameNAT;        ///< Call parties are behind the same NAT

#ifndef DISABLE_CALLAUTH
    BOOL AuthenticationFailed;
    BOOL hasAuthentication;
    const H235Authenticators  EPAuthenticators;
#endif

#ifdef H323_AEC
	PAec * aec;
#endif

    BOOL IsNonCallConnection; 

  private:
    PChannel * SwapHoldMediaChannels(PChannel * newChannel,unsigned sessionId);

    PTimedMutex outerMutex;
    PMutex innerMutex;

  public:
    BOOL StartHandleControlChannel();
	virtual BOOL OnStartHandleControlChannel();
    void EndHandleControlChannel();

#ifdef H323_RTP_AGGREGATE
  private:
    BOOL useRTPAggregation;
#endif

#ifdef H323_SIGNAL_AGGREGATE
  public:
    void AggregateSignalChannel(H323Transport * transport);
    void AggregateControlChannel(H323Transport * transport);
  protected:
    BOOL useSignallingAggregation;
    H323AggregatedH2x5Handle * signalAggregator;
    H323AggregatedH2x5Handle * controlAggregator;
#endif

#ifdef H323_H248
    PDictionary<POrdinalKey, H323ServiceControlSession> serviceControlSessions;
#endif

#ifdef H323_H460
	BOOL disableH460;
	H460_FeatureSet       * features;
#endif

#ifdef H323_FILE
  H323FileTransferHandler * fileHandler;
#endif
};


PLIST(H323ConnectionList, H323Connection);
PDICTIONARY(H323ConnectionDict, PString, H323Connection);
PDICTIONARY(H323CallIdentityDict, PString, H323Connection);


#endif // __OPAL_H323CON_H


/////////////////////////////////////////////////////////////////////////////
