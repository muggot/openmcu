/*
 * h323ep.h
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
 * $Log: h323ep.h,v $
 * Revision 1.10  2008/02/10 23:11:33  shorne
 * Fix to compile H323plus without Video
 *
 * Revision 1.9  2008/01/04 06:23:07  shorne
 * Cleaner setup and teardown of h460 module
 *
 * Revision 1.8  2008/01/02 17:50:57  shorne
 * Fix for memory leak in H.460 module
 *
 * Revision 1.7  2008/01/01 00:16:12  shorne
 * Added GnuGknat and FileTransfer support
 *
 * Revision 1.6  2007/11/17 00:14:47  shorne
 * Fix to make disabling function calls consistent
 *
 * Revision 1.5  2007/11/16 22:09:42  shorne
 * Added ability to disable H.245 QoS for NetMeeting Interop
 *
 * Revision 1.4  2007/11/01 20:17:31  shorne
 * updates for H.239 support
 *
 * Revision 1.3  2007/10/25 21:08:03  shorne
 * Added support for HD Video devices
 *
 * Revision 1.2  2007/08/20 19:13:27  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.87.2.8  2007/07/20 22:03:21  shorne
 * Initial H.350 Support
 *
 * Revision 1.87.2.7  2007/05/23 06:59:36  shorne
 * Added Assigned Alias/Gatekeeper
 *
 * Revision 1.87.2.6  2007/04/19 12:16:15  shorne
 * added ability to detect if no nat
 *
 * Revision 1.87.2.5  2007/02/19 20:11:05  shorne
 * Added Baseline H.239 Support
 *
 * Revision 1.87.2.4  2007/02/11 00:45:20  shorne
 * Added ability to disable NAT method on a call by call basis
 *
 * Revision 1.87.2.3  2007/02/02 22:12:43  shorne
 * Added ability to set FrameSize for video plugins
 *
 * Revision 1.87.2.2  2007/01/30 01:07:40  shorne
 * Added ability to disable H460
 *
 * Revision 1.87.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.87  2006/06/26 02:52:51  shorne
 * Moved H460 feature loader from H323EndPoint Constructor
 *
 * Revision 1.86  2006/06/23 20:01:28  shorne
 * More H460 support
 *
 * Revision 1.85  2006/06/23 07:06:04  csoutheren
 * Fixed linux compile
 *
 * Revision 1.84  2006/06/23 06:02:44  csoutheren
 * Added missing declarations for H.224 backport
 *
 * Revision 1.83  2006/06/09 06:30:12  csoutheren
 * Remove compile warning and errors with gcc
 *
 * Revision 1.82  2006/05/30 11:14:55  hfriederich
 * Switch from DISABLE_H460 to H323_H460
 *
 * Revision 1.81  2006/05/16 11:44:09  shorne
 * extended DNS SRV, H460 Feature , Call Credit, extend conference goals
 *
 * Revision 1.80  2006/03/07 10:37:46  csoutheren
 * Add ability to disable GRQ on GK registration
 *
 * Revision 1.79  2006/01/26 03:47:17  shorne
 * Caller Authentication, more Nat Traversal support, more PBX support
 *
 * Revision 1.78  2006/01/20 00:32:24  csoutheren
 * First check-in of signalling aggregation code - incomplete and disabled by default
 *
 * Revision 1.77  2006/01/18 07:46:08  csoutheren
 * Initial version of RTP aggregation (disabled by default)
 *
 * Revision 1.76  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.75  2005/11/22 03:38:45  shorne
 * Added ToS support to TCP Transports. thx Norbert Bartalsky (TOPCALL)
 *
 * Revision 1.74  2005/11/21 21:02:19  shorne
 * Added GnuGK Nat detection support
 *
 * Revision 1.73  2005/09/16 08:12:49  csoutheren
 * Added ability to set timeout for connect
 *
 * Revision 1.72  2005/07/12 12:28:52  csoutheren
 * Fixes for H.450 errors and return values
 * Thanks to Iker Perez San Roman
 *
 * Revision 1.71  2005/03/10 07:01:29  csoutheren
 * Fixed problem with H.450 call identifiers not being unique across all calls on an
 *  endpoint. Thanks to Thien Nguyen
 *
 * Revision 1.70  2005/01/03 14:03:20  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.69  2005/01/03 06:25:52  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.68  2004/12/20 02:32:34  csoutheren
 * Cleeaned up OSP functions
 *
 * Revision 1.67  2004/12/08 01:59:22  csoutheren
 * initial support for Transnexus OSP toolkit
 *
 * Revision 1.66  2004/11/25 07:38:58  csoutheren
 * Ensured that external TCP address translation is performed when using STUN to handle UDP
 *
 * Revision 1.65  2004/09/03 01:06:09  csoutheren
 * Added initial hooks for H.460 GEF
 * Thanks to Simon Horne and ISVO (Asia) Pte Ltd. for this contribution
 *
 * Revision 1.64  2004/07/27 05:28:45  csoutheren
 * Added ability to set priority of channel threads
 *
 * Revision 1.63  2004/07/27 01:15:16  csoutheren
 * Added virtual to InternalMakeCall
 *
 * Revision 1.62  2004/07/19 13:19:55  csoutheren
 * Fixed typo in secondaryConnectionsActive
 *
 * Revision 1.61  2004/07/11 11:36:25  rjongbloed
 * Added virtual to CleanUpConnections, thanks Eize Slange
 *
 * Revision 1.60  2004/06/15 03:30:00  csoutheren
 * Added OnSendARQ to allow access to the ARQ message before sent by connection
 *
 * Revision 1.59  2004/06/04 07:05:10  csoutheren
 * Fixed obvious typos
 *
 * Revision 1.58  2004/06/01 05:48:02  csoutheren
 * Changed capability table to use abstract factory routines rather than internal linked list
 *
 * Revision 1.57  2004/05/17 12:14:24  csoutheren
 * Added support for different SETUP PDU types
 *
 * Revision 1.56  2003/12/29 04:58:55  csoutheren
 * Added callbacks on H323EndPoint when gatekeeper discovery succeeds or fails
 *
 * Revision 1.55  2003/12/28 02:52:15  csoutheren
 * Added virtual to a few functions
 *
 * Revision 1.54  2003/12/28 02:38:14  csoutheren
 * Added H323EndPoint::OnOutgoingCall
 *
 * Revision 1.53  2003/12/28 00:07:10  csoutheren
 * Added callbacks on H323EndPoint when gatekeeper registration succeeds or fails
 *
 * Revision 1.52  2003/04/24 01:49:33  dereks
 * Add ability to set no media timeout interval
 *
 * Revision 1.51  2003/04/10 09:39:48  robertj
 * Added associated transport to new GetInterfaceAddresses() function so
 *   interfaces can be ordered according to active transport links. Improves
 *   interoperability.
 *
 * Revision 1.50  2003/04/10 01:05:11  craigs
 * Added functions to access to lists of interfaces
 *
 * Revision 1.49  2003/04/07 13:09:25  robertj
 * Added ILS support to callto URL parsing in MakeCall(), ie can now call hosts
 *   registered with an ILS directory.
 *
 * Revision 1.48  2003/02/13 00:11:31  robertj
 * Added missing virtual for controlling call transfer, thanks Andrey Pinaev
 *
 * Revision 1.47  2003/02/09 00:48:06  robertj
 * Added function to return if registered with gatekeeper.
 *
 * Revision 1.46  2003/02/04 07:06:41  robertj
 * Added STUN support.
 *
 * Revision 1.45  2003/01/26 05:57:58  robertj
 * Changed ParsePartyName so will accept addresses of the form
 *   alias@gk:address which will do an LRQ call to "address" using "alias"
 *   to determine the IP address to connect to.
 *
 * Revision 1.44  2003/01/23 02:36:30  robertj
 * Increased (and made configurable) timeout for H.245 channel TCP connection.
 *
 * Revision 1.43  2002/11/28 01:19:55  craigs
 * Added virtual to several functions
 *
 * Revision 1.42  2002/11/27 06:54:52  robertj
 * Added Service Control Session management as per Annex K/H.323 via RAS
 *   only at this stage.
 * Added H.248 ASN and very primitive infrastructure for linking into the
 *   Service Control Session management system.
 * Added basic infrastructure for Annex K/H.323 HTTP transport system.
 * Added Call Credit Service Control to display account balances.
 *
 * Revision 1.41  2002/11/15 05:17:22  robertj
 * Added facility redirect support without changing the call token for access
 *   to the call. If it gets redirected a new H323Connection object is
 *   created but it looks like the same thing to an application.
 *
 * Revision 1.40  2002/11/10 08:10:43  robertj
 * Moved constants for "well known" ports to better place (OPAL change).
 *
 * Revision 1.39  2002/10/31 00:32:15  robertj
 * Enhanced jitter buffer system so operates dynamically between minimum and
 *   maximum values. Altered API to assure app writers note the change!
 *
 * Revision 1.38  2002/10/23 06:06:10  robertj
 * Added function to be smarter in using a gatekeeper for use by endpoint.
 *
 * Revision 1.37  2002/10/21 06:07:44  robertj
 * Added function to set gatekeeper access token OID.
 *
 * Revision 1.36  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.35  2002/09/10 06:32:25  robertj
 * Added function to get gatekeeper password.
 *
 * Revision 1.34  2002/09/03 06:19:36  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.33  2002/07/19 03:39:19  robertj
 * Bullet proofed setting of RTP IP port base, can't be zero!
 *
 * Revision 1.32  2002/07/18 01:50:10  robertj
 * Changed port secltion code to force apps to use function interface.
 *
 * Revision 1.31  2002/06/22 05:48:38  robertj
 * Added partial implementation for H.450.11 Call Intrusion
 *
 * Revision 1.30  2002/06/13 06:15:19  robertj
 * Allowed TransferCall() to be used on H323Connection as well as H323EndPoint.
 *
 * Revision 1.29  2002/06/12 03:55:21  robertj
 * Added function to add/remove multiple listeners in one go comparing against
 *   what is already running so does not interrupt unchanged listeners.
 *
 * Revision 1.28  2002/05/29 06:40:29  robertj
 * Changed sending of endSession/ReleaseComplete PDU's to occur immediately
 *   on call clearance and not wait for background thread to do it.
 * Stricter compliance by waiting for reply endSession before closing down.
 *
 * Revision 1.27  2002/05/28 06:15:09  robertj
 * Split UDP (for RAS) from RTP port bases.
 * Added current port variable so cycles around the port range specified which
 *   fixes some wierd problems on some platforms, thanks Federico Pinna
 *
 * Revision 1.26  2002/05/17 03:38:05  robertj
 * Fixed problems with H.235 authentication on RAS for server and client.
 *
 * Revision 1.25  2002/05/16 00:03:05  robertj
 * Added function to get the tokens for all active calls.
 * Improved documentation for use of T.38 and T.120 functions.
 *
 * Revision 1.24  2002/05/15 08:59:18  rogerh
 * Update comments
 *
 * Revision 1.23  2002/05/03 05:38:15  robertj
 * Added Q.931 Keypad IE mechanism for user indications (DTMF).
 *
 * Revision 1.22  2002/05/02 07:56:24  robertj
 * Added automatic clearing of call if no media (RTP data) is transferred in a
 *   configurable (default 5 minutes) amount of time.
 *
 * Revision 1.21  2002/04/18 01:41:07  robertj
 * Fixed bad variable name for disabling DTMF detection, very confusing.
 *
 * Revision 1.20  2002/04/17 00:49:56  robertj
 * Added ability to disable the in band DTMF detection.
 *
 * Revision 1.19  2002/04/10 06:48:47  robertj
 * Added functions to set port member variables.
 *
 * Revision 1.18  2002/03/14 03:49:38  dereks
 * Fix minor documentation error.
 *
 * Revision 1.17  2002/02/04 07:17:52  robertj
 * Added H.450.2 Consultation Transfer, thanks Norwood Systems.
 *
 * Revision 1.16  2002/01/24 06:29:02  robertj
 * Added option to disable H.245 negotiation in SETUP pdu, this required
 *   API change so have a bit mask instead of a series of booleans.
 *
 * Revision 1.15  2002/01/17 07:04:58  robertj
 * Added support for RFC2833 embedded DTMF in the RTP stream.
 *
 * Revision 1.14  2002/01/13 23:59:43  robertj
 * Added CallTransfer timeouts to endpoint, hanks Ben Madsen of Norwood Systems.
 *
 * Revision 1.13  2002/01/08 04:45:35  robertj
 * Added MakeCallLocked() so can start a call with the H323Connection instance
 *   initally locked so can do things to it before the call really starts.
 *
 * Revision 1.12  2001/12/22 03:20:44  robertj
 * Added create protocol function to H323Connection.
 *
 * Revision 1.11  2001/12/13 10:55:30  robertj
 * Added gatekeeper access token OID specification for auto inclusion of
 *   access tokens frm ACF to SETUP pdu.
 *
 * Revision 1.10  2001/11/09 05:39:54  craigs
 * Added initial T.38 support thanks to Adam Lazur
 *
 * Revision 1.9  2001/11/01 00:27:33  robertj
 * Added default Fast Start disabled and H.245 tunneling disable flags
 *   to the endpoint instance.
 *
 * Revision 1.8  2001/09/11 01:24:36  robertj
 * Added conditional compilation to remove video and/or audio codecs.
 *
 * Revision 1.7  2001/09/11 00:21:21  robertj
 * Fixed missing stack sizes in endpoint for cleaner thread and jitter thread.
 *
 * Revision 1.6  2001/08/24 14:03:26  rogerh
 * Fix some spelling mistakes
 *
 * Revision 1.5  2001/08/16 07:49:16  robertj
 * Changed the H.450 support to be more extensible. Protocol handlers
 *   are now in separate classes instead of all in H323Connection.
 *
 * Revision 1.4  2001/08/10 11:03:49  robertj
 * Major changes to H.235 support in RAS to support server.
 *
 * Revision 1.3  2001/08/08 23:54:11  robertj
 * Fixed problem with setting gk password before have a gk variable.
 *
 * Revision 1.2  2001/08/06 03:15:17  robertj
 * Improved access to H.235 secure RAS functionality.
 *
 * Revision 1.1  2001/08/06 03:08:11  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 */

#ifndef __OPAL_H323EP_H
#define __OPAL_H323EP_H

#include "h323.h"
#include "h323con.h"

class PHandleAggregator;

/* The following classes have forward references to avoid including the VERY
   large header files for H225 and H245. If an application requires access
   to the protocol classes they can include them, but for simple usage their
   inclusion can be avoided.
 */
class H225_EndpointType;
class H225_ArrayOf_SupportedProtocols;
class H225_VendorIdentifier;
class H225_H221NonStandard;
class H225_ServiceControlDescriptor;
class H225_ArrayOf_AliasAddress;

class H323SignalPDU;
class H323ConnectionsCleaner;
class H323ServiceControlSession;

#if H323_H224
class OpalH224Handler;
class OpalH281Handler;
#endif

#ifndef DISABLE_CALLAUTH
#include "h235auth.h"
#endif

#ifdef H323_T120
class OpalT120Protocol;
#endif

#ifdef H323_T38
class OpalT38Protocol;
#endif

#ifdef H323_FILE
class H323FileTransferHandler;
class H323FileTransferList;
#endif

#ifdef P_STUN
#include <ptclib/pnat.h>
class PSTUNClient;
#endif

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifdef H323_H460
#include "h460/h4601.h"
#endif

#ifdef H323_GNUGK
#include "gnugknat.h"
#endif

///////////////////////////////////////////////////////////////////////////////

/**This class manages the H323 endpoint.
   An endpoint may have zero or more listeners to create incoming connections
   or zero or more outgoing conenctions initiated via the MakeCall() function.
   Once a conection exists it is managed by this class instance.

   The main thing this class embodies is the capabilities of the application,
   that is the codecs and protocols it is capable of.

   An application may create a descendent off this class and overide the
   CreateConnection() function, if they require a descendent of H323Connection
   to be created. This would be quite likely in most applications.
 */
class H323EndPoint : public PObject
{
  PCLASSINFO(H323EndPoint, PObject);

  public:
    enum {
      DefaultTcpPort = 1720
    };

  /**@name Construction */
  //@{
    /**Create a new endpoint.
     */
    H323EndPoint();

    /**Destroy endpoint.
     */
    ~H323EndPoint();

    /**Set the endpoint information in H225 PDU's.
      */
    virtual void SetEndpointTypeInfo(
      H225_EndpointType & info
    ) const;

    /**Set the vendor information in H225 PDU's.
      */
    virtual void SetVendorIdentifierInfo(
      H225_VendorIdentifier & info
    ) const;

	/**Set the Gateway supported protocol default always H.323
	  */
    BOOL SetGatewaySupportedProtocol(
		H225_ArrayOf_SupportedProtocols & protocols
	) const;

   /**Set the gateway prefixes 
      Override this to set the acceptable prefixes to the gatekeeper
      */
    virtual BOOL OnSetGatewayPrefixes(
		PStringList & prefixes
	) const;

    /**Set the H221NonStandard information in H225 PDU's.
      */
    virtual void SetH221NonStandardInfo(
      H225_H221NonStandard & info
    ) const;
  //@}


  /**@name Capabilities */
  //@{
    /**Add a codec to the capabilities table. This will assure that the
       assignedCapabilityNumber field in the codec is unique for all codecs
       installed on this endpoint.

       If the specific instnace of the capability is already in the table, it
       is not added again. Ther can be multiple instances of the same
       capability class however.
     */
    void AddCapability(
      H323Capability * capability   ///< New codec specification
    );

    /**Set the capability descriptor lists. This is three tier set of
       codecs. The top most level is a list of particular capabilities. Each
       of these consists of a list of alternatives that can operate
       simultaneously. The lowest level is a list of codecs that cannot
       operate together. See H323 section 6.2.8.1 and H245 section 7.2 for
       details.

       If descriptorNum is P_MAX_INDEX, the the next available index in the
       array of descriptors is used. Similarly if simultaneous is P_MAX_INDEX
       the the next available SimultaneousCapabilitySet is used. The return
       value is the index used for the new entry. Note if both are P_MAX_INDEX
       then the return value is the descriptor index as the simultaneous index
       must be zero.

       Note that the capability specified here is automatically added to the
       capability table using the AddCapability() function. A specific
       instance of a capability is only ever added once, so multiple
       SetCapability() calls with the same H323Capability pointer will only
       add that capability once.
     */
    PINDEX SetCapability(
      PINDEX descriptorNum, ///< The member of the capabilityDescriptor to add
      PINDEX simultaneous,  ///< The member of the SimultaneousCapabilitySet to add
      H323Capability * cap  ///< New capability specification
    );

	/**Manually remove capability type. This removes the specified Capability type out of the 
	   default capability list.
	  */
	BOOL RemoveCapability(
		H323Capability::MainTypes capabilityType  ///< capability type
	);

#ifdef H323_VIDEO
	/**Set the Video Frame Size. This is used for capabilities
	   that use 1 definition for all Video Frame Sizes. This will remove all capabilities
	   not matching the specified Frame Size and send a message to the remaining video capabilities 
	   to set the maximum framesize allowed to the specified value
	  */
	BOOL SetVideoFrameSize(H323Capability::CapabilityFrameSize frameSize, 
		                  int frameUnits = 1
	);
#endif

    /**Add all matching capabilities in list.
       All capabilities that match the specified name are added. See the
       capabilities code for details on the matching algorithm.
      */
    PINDEX AddAllCapabilities(
      PINDEX descriptorNum, ///< The member of the capabilityDescriptor to add
      PINDEX simultaneous,  ///< The member of the SimultaneousCapabilitySet to add
      const PString & name  ///< New capabilities name, if using "known" one.
    );

    /**Add all user input capabilities to this endpoints capability table.
      */
    void AddAllUserInputCapabilities(
      PINDEX descriptorNum, ///< The member of the capabilityDescriptor to add
      PINDEX simultaneous   ///< The member of the SimultaneousCapabilitySet to add
    );

#ifdef H323_H239
    /** Open Extended Video Session
	  */
	BOOL OpenExtendedVideoSession(
		const PString & token,   ///< Connection Token
		H323ChannelNumber & num  ///< Opened Channel number
	);

    BOOL CloseExtendedVideoSession(
		const PString & token,         ///< Connection Token
		const H323ChannelNumber & num  ///< channel number
	);

    /**Add all Extended Video capabilities to this endpoints capability table.
      */
    void AddAllExtendedVideoCapabilities(
      PINDEX descriptorNum, ///< The member of the capabilityDescriptor to add
      PINDEX simultaneous   ///< The member of the SimultaneousCapabilitySet to add
    );
#endif

    /**Remove capabilites in table.
      */
    void RemoveCapabilities(
      const PStringArray & codecNames
    );

    /**Reorder capabilites in table.
      */
    void ReorderCapabilities(
      const PStringArray & preferenceOrder
    );

    /**Find a capability that has been registered.
     */
    H323Capability * FindCapability(
      const H245_Capability & cap  ///< H245 capability table entry
    ) const;

    /**Find a capability that has been registered.
     */
    H323Capability * FindCapability(
      const H245_DataType & dataType  ///< H245 data type of codec
    ) const;

    /**Find a capability that has been registered.
     */
    H323Capability * FindCapability(
      H323Capability::MainTypes mainType,   ///< Main type of codec
      unsigned subType                      ///< Subtype of codec
    ) const;
  //@}

  /**@name Gatekeeper management */
  //@{
    /**Use and register with an explicit gatekeeper.
       This will call other functions according to the following table:

           address    identifier   function
           empty      empty        DiscoverGatekeeper()
           non-empty  empty        SetGatekeeper()
           empty      non-empty    LocateGatekeeper()
           non-empty  non-empty    SetGatekeeperZone()

       The localAddress field, if non-empty, indicates the interface on which
       to look for the gatekeeper. An empty string is equivalent to "ip$*:*"
       which is any interface or port.

       If the endpoint is already registered with a gatekeeper that meets
       the same criteria then the gatekeeper is not changed, otherwise it is
       deleted (with unregistration) and new one created and registered to.
     */
    BOOL UseGatekeeper(
      const PString & address = PString::Empty(),     ///< Address of gatekeeper to use.
      const PString & identifier = PString::Empty(),  ///< Identifier of gatekeeper to use.
      const PString & localAddress = PString::Empty() ///< Local interface to use.
    );

    /**Select and register with an explicit gatekeeper.
       This will use the specified transport and a string giving a transport
       dependent address to locate a specific gatekeeper. The endpoint will
       register with that gatekeeper and, if successful, set it as the current
       gatekeeper used by this endpoint.

       Note the transport being passed in will be deleted by this function or
       the H323Gatekeeper object it becomes associated with. Also if transport
       is NULL then a H323TransportUDP is created.
     */
    BOOL SetGatekeeper(
      const PString & address,          ///< Address of gatekeeper to use.
      H323Transport * transport = NULL  ///< Transport over which to talk to gatekeeper.
    );

    /**Select and register with an explicit gatekeeper and zone.
       This will use the specified transport and a string giving a transport
       dependent address to locate a specific gatekeeper. The endpoint will
       register with that gatekeeper and, if successful, set it as the current
       gatekeeper used by this endpoint.

       The gatekeeper identifier is set to the spplied parameter to allow the
       gatekeeper to either allocate a zone or sub-zone, or refuse to register
       if the zones do not match.

       Note the transport being passed in will be deleted by this function or
       the H323Gatekeeper object it becomes associated with. Also if transport
       is NULL then a H323TransportUDP is created.
     */
    BOOL SetGatekeeperZone(
      const PString & address,          ///< Address of gatekeeper to use.
      const PString & identifier,       ///< Identifier of gatekeeper to use.
      H323Transport * transport = NULL  ///< Transport over which to talk to gatekeeper.
    );

    /**Locate and select gatekeeper.
       This function will use the automatic gatekeeper discovery methods to
       locate the gatekeeper on the particular transport that has the specified
       gatekeeper identifier name. This is often the "Zone" for the gatekeeper.

       Note the transport being passed in will be deleted becomes owned by the
       H323Gatekeeper created by this function and will be deleted by it. Also
       if transport is NULL then a H323TransportUDP is created.
     */
    BOOL LocateGatekeeper(
      const PString & identifier,       ///< Identifier of gatekeeper to locate.
      H323Transport * transport = NULL  ///< Transport over which to talk to gatekeeper.
    );

    /**Discover and select gatekeeper.
       This function will use the automatic gatekeeper discovery methods to
       locate the first gatekeeper on a particular transport.

       Note the transport being passed in will be deleted becomes owned by the
       H323Gatekeeper created by this function and will be deleted by it. Also
       if transport is NULL then a H323TransportUDP is created.
     */
    BOOL DiscoverGatekeeper(
      H323Transport * transport = NULL  ///< Transport over which to talk to gatekeeper.
    );

    /**Create a gatekeeper.
       This allows the application writer to have the gatekeeper as a
       descendent of the H323Gatekeeper in order to add functionality to the
       base capabilities in the library.

       The default creates an instance of the H323Gatekeeper class.
     */
    virtual H323Gatekeeper * CreateGatekeeper(
      H323Transport * transport  ///< Transport over which gatekeepers communicates.
    );

    /**Get the gatekeeper we are registered with.
     */
    H323Gatekeeper * GetGatekeeper() const { return gatekeeper; }

    /**Return if endpoint is registered with gatekeeper.
      */
    BOOL IsRegisteredWithGatekeeper() const;

    /**Unregister and delete the gatekeeper we are registered with.
       The return value indicates FALSE if there was an error during the
       unregistration. However the gatekeeper is still removed and its
       instance deleted regardless of this error.
     */
    BOOL RemoveGatekeeper(
      int reason = -1    ///< Reason for gatekeeper removal
    );

    /**Set the H.235 password for the gatekeeper.
      */
    virtual void SetGatekeeperPassword(
      const PString & password
    );

    /**Get the H.235 password for the gatekeeper.
      */
    virtual const PString & GetGatekeeperPassword() const { return gatekeeperPassword; }

    /** Check the IP of the Gatekeeper on reregistration
	    Use this to check if Gatekeeper IP had changed (used for DDNS type registrations)
		Default returns FALSE
	  */
	virtual BOOL GatekeeperCheckIP(const H323TransportAddress & oldAddr,H323TransportAddress & newaddress);

    /**Create a list of authenticators for gatekeeper.
      */
    virtual H235Authenticators CreateAuthenticators();

	/**Called when sending a Admission Request to the gatekeeper
	 */
	virtual void OnAdmissionRequest(H323Connection & connection);

    /**Called when the gatekeeper sends a GatekeeperConfirm
      */
    virtual void  OnGatekeeperConfirm();

    /**Called when the gatekeeper sends a GatekeeperReject
      */
    virtual void  OnGatekeeperReject();

    /**Called when the gatekeeper sends a RegistrationConfirm
      */
    virtual void OnRegistrationConfirm(const H323TransportAddress & rasAddress);

    /**Called when the gatekeeper sends a RegistrationReject
      */
    virtual void  OnRegistrationReject();

    /**Called when send registration request
      */
    virtual void OnRegistrationRequest() { };

    /**Called when Unregistered by Gatekeeper
      */
    virtual void OnUnRegisterRequest();

  //@}

  /**@name Connection management */
  //@{
    /**Add a listener to the endpoint.
       This allows for the automatic creating of incoming call connections. An
       application should use OnConnectionEstablished() to monitor when calls
       have arrived and been successfully negotiated.

       Note if this returns TRUE, then the endpoint is responsible for
       deleting the H323Listener listener object. If FALSE is returned then
       the object is not deleted and it is up to the caller to release the
       memory allocated for the object.

       If a listener already exists on the same transport address then it is
       ignored, but TRUE is still returned. The caller does not need to delete
       the object.
      */
    BOOL StartListener(
      H323Listener * listener ///< Transport dependent listener.
    );

    /**Add a listener to the endpoint.
       This allows for the automatic creating of incoming call connections. An
       application should use OnConnectionEstablished() to monitor when calls
       have arrived and been successfully negotiated.

       If a listener already exists on the same address then it is ignored,
       but TRUE is still returned.

       If the iface string is empty then "*" is assumed which will listen on
       the standard TCP port on INADDR_ANY.
      */
    BOOL StartListener(
      const H323TransportAddress & iface ///< Address of interface to listen on.
    );

    /**Add listeners to the endpoint.
       Set the collection of listeners which will allow the automatic
       creating of incoming call connections. An application should use
       OnConnectionEstablished() to monitor when calls have arrived and been
       successfully negotiated.

       If a listener already exists on the interface specified in the list
       then it is ignored. If a listener does not yet exist a new one is
       created and if a listener is running that is not in the list then it
       is stopped and removed.

       If the array is empty then the string "*" is assumed which will listen
       on the standard TCP port on INADDR_ANY.

       Returns TRUE if at least one interface was successfully started.
      */
    BOOL StartListeners(
      const H323TransportAddressArray & ifaces ///< Interfaces to listen on.
    );

    /**Remove a listener from the endpoint.
       If the listener parameter is NULL then all listeners are removed.
      */
    BOOL RemoveListener(
      H323Listener * listener ///< Transport dependent listener.
    );

    /**Return a list of the transport addresses for all listeners on this endpoint
      */
    H323TransportAddressArray GetInterfaceAddresses(
      BOOL excludeLocalHost = TRUE,       ///< Flag to exclude 127.0.0.1
      H323Transport * associatedTransport = NULL
                          ///< Associated transport for precedence and translation
    );

#ifndef DISABLE_CALLAUTH
     /**Make a Authenticated call to a remote party. 
	This Function sets Security Information to be included when calling
	a EP which requires Authentication
       */
      H323Connection * MakeAuthenticatedCall (
                        const PString & remoteParty,  ///* Remote party to call
                        const PString & UserName,     ///* UserName to Use (Default is LocalPartyName)
                        const PString & Password,     ///* Password to Use (MUST NOT BE EMPTY)
                        PString & token,              ///* String to receive token for connection
                        void * userData = NULL        ///* user data to pass to CreateConnection
     );											
#endif								

    /**Make a call to a remote party. An appropriate transport is determined
       from the remoteParty parameter. The general form for this parameter is
       [alias@][transport$]host[:port] where the default alias is the same as
       the host, the default transport is "ip" and the default port is 1720.

       This function returns almost immediately with the call occurring in a
       new background thread. Note that the call could be created and cleared
       ie OnConnectionCleared is called BEFORE this function returns. It is
       guaranteed that the token variable is set before OnConnectionCleared
       called.

       Note, the returned pointer to the connection is not locked and may be
       deleted at any time. This is extremely unlikely immediately after the
       function is called, but you should not keep this pointer beyond that
       brief time. The the FindConnectionWithLock() function for future
       references to the connection object. It is recommended that
       MakeCallLocked() be usedin instead.
     */
    H323Connection * MakeCall(
      const PString & remoteParty,  ///< Remote party to call
      PString & token,              ///< String to receive token for connection
      void * userData = NULL        ///< user data to pass to CreateConnection
    );

    /**Make a call to a remote party using the specified transport.
       The remoteParty may be a hostname, alias or other user name that is to
       be passed to the transport, if present.

       If the transport parameter is NULL the transport is determined from the
       remoteParty description.

       This function returns almost immediately with the call occurring in a
       new background thread. Note that the call could be created and cleared
       ie OnConnectionCleared is called BEFORE this function returns. It is
       guaranteed that the token variable is set before OnConnectionCleared
       called.

       Note, the returned pointer to the connection is not locked and may be
       deleted at any time. This is extremely unlikely immediately after the
       function is called, but you should not keep this pointer beyond that
       brief time. The the FindConnectionWithLock() function for future
       references to the connection object. It is recommended that
       MakeCallLocked() be usedin instead.
     */
    H323Connection * MakeCall(
      const PString & remoteParty,  ///< Remote party to call
      H323Transport * transport,    ///< Transport to use for call.
      PString & token,              ///< String to receive token for connection
      void * userData = NULL        ///< user data to pass to CreateConnection
    );

    /**Make a call to a remote party using the specified transport.
       The remoteParty may be a hostname, alias or other user name that is to
       be passed to the transport, if present.

       If the transport parameter is NULL the transport is determined from the
       remoteParty description.

       This function returns almost immediately with the call occurring in a
       new background thread. However the call will not progress very far 
     */
    H323Connection * MakeCallLocked(
      const PString & remoteParty,     ///< Remote party to call
      PString & token,                 ///< String to receive token for connection
      void * userData = NULL,          ///< user data to pass to CreateConnection
      H323Transport * transport = NULL ///< Transport to use for call.
    );

#ifdef H323_H450
    /**Setup the transfer of an existing call (connection) to a new remote party
       using H.450.2.  This sends a Call Transfer Setup Invoke message from the
       B-Party (transferred endpoint) to the C-Party (transferred-to endpoint).

       If the transport parameter is NULL the transport is determined from the
       remoteParty description. The general form for this parameter is
       [alias@][transport$]host[:port] where the default alias is the same as
       the host, the default transport is "ip" and the default port is 1720.

       This function returns almost immediately with the transfer occurring in a
       new background thread.

       Note, the returned pointer to the connection is not locked and may be
       deleted at any time. This is extremely unlikely immediately after the
       function is called, but you should not keep this pointer beyond that
       brief time. The the FindConnectionWithLock() function for future
       references to the connection object.

       This function is declared virtual to allow an application to override
       the function and get the new call token of the forwarded call.
     */
    virtual H323Connection * SetupTransfer(
      const PString & token,        ///< Existing connection to be transferred
      const PString & callIdentity, ///< Call identity of the secondary call (if it exists)
      const PString & remoteParty,  ///< Remote party to transfer the existing call to
      PString & newToken,           ///< String to receive token for the new connection
      void * userData = NULL        ///< user data to pass to CreateConnection
    );

    /**Initiate the transfer of an existing call (connection) to a new remote
       party using H.450.2.  This sends a Call Transfer Initiate Invoke
       message from the A-Party (transferring endpoint) to the B-Party
       (transferred endpoint).
     */
    void TransferCall(
      const PString & token,        ///< Existing connection to be transferred
      const PString & remoteParty,  ///< Remote party to transfer the existing call to
      const PString & callIdentity = PString::Empty()
                                    ///< Call Identity of secondary call if present
    );

    /**Transfer the call through consultation so the remote party in the
       primary call is connected to the called party in the second call
       using H.450.2.  This sends a Call Transfer Identify Invoke message
       from the A-Party (transferring endpoint) to the C-Party
       (transferred-to endpoint).
     */
    void ConsultationTransfer(
      const PString & primaryCallToken,   ///< Token of primary call
      const PString & secondaryCallToken  ///< Token of secondary call
    );

    /**Place the call on hold, suspending all media channels (H.450.4)
    * NOTE: Only Local Hold is implemented so far. 
    */
    void HoldCall(
      const PString & token,        ///< Existing connection to be transferred
      BOOL localHold   ///< true for Local Hold, false for Remote Hold
    );

    /** Initiate Call intrusion
        Designed similar to MakeCall function
      */
    H323Connection * IntrudeCall(
      const PString & remoteParty,  ///< Remote party to intrude call
      PString & token,              ///< String to receive token for connection
      unsigned capabilityLevel,     ///< Capability level
      void * userData = NULL        ///< user data to pass to CreateConnection
    );

    H323Connection * IntrudeCall(
      const PString & remoteParty,  ///< Remote party to intrude call
      H323Transport * transport,    ///< Transport to use for call.
      PString & token,              ///< String to receive token for connection
      unsigned capabilityLevel,     ///< Capability level
      void * userData = NULL        ///< user data to pass to CreateConnection
    );

#endif // H323_H450

	/** Use DNS SRV and ENUM to resolve all the possible addresses a call party 
       can be found. Only effective if not registered with Gatekeeper
	  */
    BOOL ResolveCallParty(
      const PString & _remoteParty, 
      PStringList & addresses
    );

    /**Parse a party address into alias and transport components.
       An appropriate transport is determined from the remoteParty parameter.
       The general form for this parameter is [alias@][transport$]host[:port]
       where the default alias is the same as the host, the default transport
       is "ip" and the default port is 1720.
      */
    BOOL ParsePartyName(
      const PString & party,          ///< Party name string.
      PString & alias,                ///< Parsed alias name
      H323TransportAddress & address  ///< Parsed transport address
    );

    /**Clear a current connection.
       This hangs up the connection to a remote endpoint. Note that this function
       is asynchronous
      */
    virtual BOOL ClearCall(
      const PString & token,  ///< Token for identifying connection
      H323Connection::CallEndReason reason =
                  H323Connection::EndedByLocalUser ///< Reason for call clearing
    );

     /**Clearing a current connection.
        A connection is being cleared callback. This can be used for PBX applications 
        to reallocate the line early without waiting for the cleaner thread to clean-up
        the connection.
       */
     virtual void OnCallClearing(H323Connection * connection,       ///* Connection being Cleared
                              H323Connection::CallEndReason reason  ///* Reason for call being cleared
     );

    /**Clear a current connection.
       This hangs up the connection to a remote endpoint. Note that these functions
       are synchronous
      */
    virtual BOOL ClearCallSynchronous(
      const PString & token,            ///< Token for identifying connection
      H323Connection::CallEndReason reason =
                        H323Connection::EndedByLocalUser ///< Reason for call clearing
    );
    virtual BOOL ClearCallSynchronous(
      const PString & token,                ///< Token for identifying connection
      H323Connection::CallEndReason reason, ///< Reason for call clearing
      PSyncPoint * sync
    );

    /**Clear all current connections.
       This hangs up all the connections to remote endpoints. The wait
       parameter is used to wait for all the calls to be cleared and their
       memory usage cleaned up before returning. This is typically used in
       the destructor for your descendant of H323EndPoint.
      */
    virtual void ClearAllCalls(
      H323Connection::CallEndReason reason =
                  H323Connection::EndedByLocalUser, ///< Reason for call clearing
      BOOL wait = TRUE   ///< Flag for wait for calls to e cleared.
    );

    /**Determine if a connection is active.
      */
    virtual BOOL HasConnection(
      const PString & token   ///< Token for identifying connection
    );

    /**Find a connection that uses the specified token.
       This searches the endpoint for the connection that contains the token
       as provided by functions such as MakeCall() (as built by the
       BuildConnectionToken() function). if not found it will then search for
       the string representation of the CallIdentifier for the connection, and
       finally try for the string representation of the ConferenceIdentifier.

       Note the caller of this function MUSt call the H323Connection::Unlock()
       function if this function returns a non-NULL pointer. If it does not
       then a deadlock can occur.
      */
    H323Connection * FindConnectionWithLock(
      const PString & token     ///< Token to identify connection
    );

    /**Get all calls current on the endpoint.
      */
    PStringList GetAllConnections();

    /**Call back for incoming call.
       This function is called from the OnReceivedSignalSetup() function
       before it sends the Alerting PDU. It gives an opportunity for an
       application to alter the reply before transmission to the other
       endpoint.

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour simply returns TRUE.
     */
    virtual BOOL OnIncomingCall(
      H323Connection & connection,    ///< Connection that was established
      const H323SignalPDU & setupPDU,   ///< Received setup PDU
      H323SignalPDU & alertingPDU       ///< Alerting PDU to send
    );
    virtual BOOL OnIncomingCall(
      H323Connection & connection,           ///< Connection that was established
      const H323SignalPDU & setupPDU,        ///< Received setup PDU
      H323SignalPDU & alertingPDU,           ///< Alerting PDU to send
      H323Connection::CallEndReason & reason ///< reason for call refusal, if returned false
    );

    /**Handle a connection transfer.
       This gives the application an opportunity to abort the transfer.
       The default behaviour just returns TRUE.
      */
    virtual BOOL OnCallTransferInitiate(
      H323Connection & connection,    ///< Connection to transfer
      const PString & remoteParty     ///< Party transferring to.
    );

    /**Handle a transfer via consultation.
       This gives the transferred-to user an opportunity to abort the transfer.
       The default behaviour just returns TRUE.
      */
    virtual BOOL OnCallTransferIdentify(
      H323Connection & connection    ///< Connection to transfer
    );

    /**
      * Callback for ARQ send to a gatekeeper. This allows the endpoint
      * to change or check fields in the ARQ before it is sent.
      */
    virtual void OnSendARQ(
      H323Connection & conn,
      H225_AdmissionRequest & arq
    );

   /**
      * Callback for ACF sent back from gatekeeper. 
      */
    virtual void OnReceivedACF(
      H323Connection & conn,
      const H225_AdmissionConfirm & acf
    );

   /**
      * Callback for ARJ sent back from  gatekeeper. 
      */
    virtual void OnReceivedARJ(
      H323Connection & conn,
      const H225_AdmissionReject & arj
    );

    /**Call back for answering an incoming call.
       This function is called from the OnReceivedSignalSetup() function
       before it sends the Connect PDU. It gives an opportunity for an
       application to alter the reply before transmission to the other
       endpoint.

       It also gives an application time to wait for some event before
       signalling to the endpoint that the connection is to proceed. For
       example the user pressing an "Answer call" button.

       If AnswerCallDenied is returned the connection is aborted and a Release
       Complete PDU is sent. If AnswerCallNow is returned then the H.323
       protocol proceeds. Finally if AnswerCallPending is returned then the
       protocol negotiations are paused until the AnsweringCall() function is
       called.

       The default behaviour simply returns AnswerNow.
     */
    virtual H323Connection::AnswerCallResponse OnAnswerCall(
      H323Connection & connection,    ///< Connection that was established
      const PString & callerName,       ///< Name of caller
      const H323SignalPDU & setupPDU,   ///< Received setup PDU
      H323SignalPDU & connectPDU        ///< Connect PDU to send. 
    );

    /**Call back for remote party being alerted.
       This function is called from the SendSignalSetup() function after it
       receives the optional Alerting PDU from the remote endpoint. That is
       when the remote "phone" is "ringing".

       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour simply returns TRUE.
     */
    virtual BOOL OnAlerting(
      H323Connection & connection,    ///< Connection that was established
      const H323SignalPDU & alertingPDU,  ///< Received Alerting PDU
      const PString & user                ///< Username of remote endpoint
    );

    /**A call back function when a connection indicates it is to be forwarded.
       An H323 application may handle this call back so it can make
       complicated decisions on if the call forward ius to take place. If it
       decides to do so it must call MakeCall() and return TRUE.

       The default behaviour simply returns FALSE and that the automatic
       call forwarding should take place. See ForwardConnection()
      */
    virtual BOOL OnConnectionForwarded(
      H323Connection & connection,    ///< Connection to be forwarded
      const PString & forwardParty,   ///< Remote party to forward to
      const H323SignalPDU & pdu       ///< Full PDU initiating forwarding
    );

    /**Forward the call using the same token as the specified connection.
       Return TRUE if the call is being redirected.

       The default behaviour will replace the current call in the endpoints
       call list using the same token as the call being redirected. Not that
       even though the same token is being used the actual object is
       completely mad anew.
      */
    virtual BOOL ForwardConnection(
      H323Connection & connection,    ///< Connection to be forwarded
      const PString & forwardParty,   ///< Remote party to forward to
      const H323SignalPDU & pdu       ///< Full PDU initiating forwarding
    );

    /**A call back function whenever a connection is established.
       This indicates that a connection to a remote endpoint was established
       with a control channel and zero or more logical channels.

       The default behaviour does nothing.
      */
    virtual void OnConnectionEstablished(
      H323Connection & connection,    ///< Connection that was established
      const PString & token           ///< Token for identifying connection
    );

    /**Determine if a connection is established.
      */
    virtual BOOL IsConnectionEstablished(
      const PString & token   ///< Token for identifying connection
    );

    /**A call back function whenever a connection is broken.
       This indicates that a connection to a remote endpoint is no longer
       available.

       The default behaviour does nothing.
      */
    virtual void OnConnectionCleared(
      H323Connection & connection,    ///< Connection that was established
      const PString & token           ///< Token for identifying connection
    );

    /**Build a unique token for the connection.
       This identifies the call using the Q931 transport host name and the
       Q931 call reference number.
      */
    static PString BuildConnectionToken(
      const H323Transport & transport,  ///< Transport for connection
      unsigned callReference,           ///< Call reference of Q.931 link
      BOOL fromRemote                   ///< Call reference is from remote endpoint
    );

    /**Handle a new incoming connection.
       This will examine the setup PDU and either attach the signalling
       transport to an existing connection that has the same Q.931 call
       reference, or creates a new connection using CreateConnection().
      */
    virtual H323Connection * OnIncomingConnection(
      H323Transport * transport,  ///< Transport for connection
      H323SignalPDU & setupPDU    ///< Setup PDU
    );

    /**Called when an outgoing call connects
       If FALSE is returned the connection is aborted and a Release Complete
       PDU is sent.

       The default behaviour simply returns TRUE.
      */
    virtual BOOL OnOutgoingCall(
        H323Connection & conn, 
        const H323SignalPDU & connectPDU
    );

    /**Create a connection that uses the specified call reference.
      */
    virtual H323Connection * CreateConnection(
      unsigned callReference,     ///< Call reference to use
      void * userData,            ///< user data to pass to CreateConnection
      H323Transport * transport,  ///< Transport for connection
      H323SignalPDU * setupPDU    ///< Setup PDU, NULL if outgoing call
    );
    virtual H323Connection * CreateConnection(
      unsigned callReference,   ///< Call reference to use
      void * userData           ///< user data to pass to CreateConnection
    );
    virtual H323Connection * CreateConnection(
      unsigned callReference    ///< Call reference to use
    );

    /**Clean up connections.
       This function is called from a background thread and checks for closed
       connections to clean up.

       This would not normally be called by an application.
      */
    virtual void CleanUpConnections();
  //@}

#ifndef DISABLE_CALLAUTH
  /**@name Caller Authentication */
  //@{
    /**Create a list of authenticators for Call Authentication.
       To Create a list of Autheniticators the Endpoint MUST have
       set EPSecurityPassword (via SetEPCredentials()) and either
       set CallAuthPolicy (via SetEPSecurityPolicy()) or set 
       isSecureCall to TRUE (via MakeAuthenticatedCall()) 
      */
     virtual H235Authenticators CreateEPAuthenticators();

    /** Retrieve Password and UserName for EPAuthentication
        NOTE: Returns FALSE is EPSecurityPassword.IsEmpty()
       */
    virtual BOOL GetEPCredentials(PString & password,   ///* Password to use for call
                                  PString & username    ///* Username to use for call
                                  );

     /** Set the Password and UserName for EPAuthentication for Connection
       */
     virtual void SetEPCredentials(PString password,   ///* Password to use for call
                                   PString username    ///* Username to use for call
                                   );

     enum EPSecurityPolicy
     {
         SecNone,           ///* Default: Do Not Include Call Authenication
         SecRequest,        ///* Request Authentication but Accept if Missing/Fail 
         SecRequired        ///* Calls are Rejected with EndedBySecurityDenial if Authenitication fails.
     };

     /** Set the EP Security Policy
       */
     virtual void SetEPSecurityPolicy(EPSecurityPolicy policy);

     /** Get the EP Security Policy 
       */
     virtual EPSecurityPolicy GetEPSecurityPolicy();

     /** Retrieve the List of UserNames/Passwords to be used
	 to Authenticate Incoming Calls.
       */
     H235AuthenticatorList GetAuthenticatorList();

     /** Call Authentication Call Back 
	 This fires for all the Authentication Methods created by
	 CreateEPAuthenticators() The Function Supplies the Name of 
	 the Authentication process and the supplied SenderID (Username) 
	 and this is then check against EPAuthList to:
	 1. Check if the username exists and if so
	 2. Return the password in the clear to validate.
	 Returning FALSE indicates that Authentication Failed failed for that Method..
       */
      virtual BOOL OnCallAuthentication(const PString & username,  ///* UserName of Caller
                                        PString & password         ///* Password related to caller
                                        );
 //@}
#endif

  /**@name Logical Channels management */
  //@{
    /**Call back for opening a logical channel.

       The default behaviour simply returns TRUE.
      */
    virtual BOOL OnStartLogicalChannel(
      H323Connection & connection,    ///< Connection for the channel
      H323Channel & channel           ///< Channel being started
    );

    /**Call back for closed a logical channel.

       The default behaviour does nothing.
      */
    virtual void OnClosedLogicalChannel(
      H323Connection & connection,    ///< Connection for the channel
      const H323Channel & channel     ///< Channel being started
    );

#ifndef NO_H323_AUDIO_CODECS
    /**Open a channel for use by an audio codec.
       The H323AudioCodec class will use this function to open the channel to
       read/write PCM data.

       The default function creates a PSoundChannel using the member variables
       soundChannelPlayDevice or soundChannelRecordDevice.
      */
    virtual BOOL OpenAudioChannel(
      H323Connection & connection,  ///< Connection for the channel
      BOOL isEncoding,              ///< Direction of data flow
      unsigned bufferSize,          ///< Size of each sound buffer
      H323AudioCodec & codec        ///< codec that is doing the opening
    );
#endif

#ifndef NO_H323_VIDEO
    /**Open a channel for use by an video codec.
       The H323VideoCodec class will use this function to open the channel to
       read/write image data (which is one frame in a video stream - YUV411 format).

       The default function creates a PVideoChannel using the member variables.
      */
    virtual BOOL OpenVideoChannel(
      H323Connection & connection,  ///< Connection for the channel
      BOOL isEncoding,              ///< Direction of data flow
      H323VideoCodec & codec        ///< codec doing the opening
    );

#ifdef H323_H239
    /**Open a channel for use by an application share application.
       The H323VideoCodec class will use this function to open the channel to
       read/write image data (which is one frame in a video stream - YUV411 format).

       The default function creates a PVideoChannel using the member variables.
      */
    virtual BOOL OpenExtendedVideoChannel(
      H323Connection & connection,  ///< Connection for the channel
      BOOL isEncoding,              ///< Direction of data flow
      H323VideoCodec & codec        ///< codec doing the opening
    );
#endif // H323_H239
#endif // NO_H323_VIDEO

    /**Callback from the RTP session for statistics monitoring.
       This is called every so many packets on the transmitter and receiver
       threads of the RTP session indicating that the statistics have been
       updated.

       The default behaviour does nothing.
      */
    virtual void OnRTPStatistics(
      const H323Connection & connection,  ///< Connection for the channel
      const RTP_Session & session         ///< Session with statistics
    ) const;

   /**Callback from the RTP session for statistics monitoring.
       This is called at the end of the RTP session indicating that the statistics 
       of the call

       The default behaviour does nothing.
      */
    virtual void OnRTPFinalStatistics(
      const H323Connection & connection,  ///< Connection for the channel
      const RTP_Session & session         ///< Session with statistics
    ) const;

  //@}

  /**@name Indications */
  //@{
    /**Call back for remote enpoint has sent user input as a string.

       The default behaviour does nothing.
      */
    virtual void OnUserInputString(
      H323Connection & connection,  ///< Connection for the input
      const PString & value         ///< String value of indication
    );

    /**Call back for remote enpoint has sent user input.

       The default behaviour calls H323Connection::OnUserInputTone().
      */
    virtual void OnUserInputTone(
      H323Connection & connection,  ///< Connection for the input
      char tone,                    ///< DTMF tone code
      unsigned duration,            ///< Duration of tone in milliseconds
      unsigned logicalChannel,      ///< Logical channel number for RTP sync.
      unsigned rtpTimestamp         ///< RTP timestamp in logical channel sync.
    );

#ifdef H323_GNUGK
     /**Call back from GK admission confirm to notify the Endpoint it is behind a NAT
	(GNUGK Gatekeeper) The default does nothing. Override this to notify the user they are behind a NAT.
     */
	virtual void OnGatekeeperNATDetect(
		PIPSocket::Address publicAddr,         ///< Public address as returned by the Gatekeeper
		PString & gkIdentifier,                ///< Identifier at the gatekeeper
		H323TransportAddress & gkRouteAddress  ///< Gatekeeper Route Address
		);
		
     /**Call back from GK admission confirm to notify the Endpoint it is not detected as being NAT
	(GNUGK Gatekeeper) The default does nothing. Override this to notify the user they are not NAT 
	so they can confirm that it is true.
     */
	virtual void OnGatekeeperOpenNATDetect(
		PString & gkIdentifier,                ///< Identifier at the gatekeeper
		H323TransportAddress & gkRouteAddress  ///< Gatekeeper Route Address
		);

	/** Fired with the keep-alive connection to GnuGk fails or is re-established
	    This allows the endpoint to re-register.
	  */
	virtual void NATLostConnection(BOOL lost);
#endif

	/** Call back for GK assigned aliases returned from the gatekeeper in the RCF. 
	    The default returns FALSE which appends the new aliases to the existing alias list. 
		By overriding this function and returning TRUE overrides the default operation
	  */
	virtual BOOL OnGatekeeperAliases(
		const H225_ArrayOf_AliasAddress & aliases  ///< Alias List returned from the gatekeeper
		);
  //@}

#ifdef H323_H248
  /**@name Service Control */
  //@{
    /**Call back for HTTP based Service Control.
       An application may override this to use an HTTP link to display 
	   call information/CDR's or Billing information.

       The default behaviour does nothing.
      */
    virtual void OnHTTPServiceControl(
      unsigned operation,  ///< Control operation
      unsigned sessionId,  ///< Session ID for HTTP page
      const PString & url  ///< URL to use.
    );

    /**Call back for Call Credit Service Control.
       An application may override this to display call credit Information. 

       The default behaviour does nothing.
      */
	virtual void OnCallCreditServiceControl(
      const PString & amount,         ///< UTF-8 string for amount, including currency.
      BOOL mode,                      ///< Flag indicating that calls will debit the account.
	  const unsigned & durationLimit  ///< Duration Limit (used to decrement display)
    );

#ifdef H323_H350
    /**Call back for LDAP based Service Control.
       An application may override this to use an LDAP directory to query
	   White Page searches.

       The default behaviour does nothing.
      */
    virtual void OnH350ServiceControl(
		const PString & url,
		const PString & BaseDN
		);
#endif

    /**Call back for call credit information.
       An application may override this to display call credit information
       on registration, or when a call is started.

       The canDisplayAmountString member variable must also be set to TRUE
       for this to operate.

       The default behaviour does nothing.
      */
    virtual void OnCallCreditServiceControl(
      const PString & amount,  ///< UTF-8 string for amount, including currency.
      BOOL mode          ///< Flag indicating that calls will debit the account.
    );

    /**Handle incoming service control session information.
       Default behaviour calls session.OnChange()
     */
    virtual void OnServiceControlSession(
      unsigned type,
      unsigned sessionid,
      const H323ServiceControlSession & session,
      H323Connection * connection
    );

    /**Create the service control session object.
     */
    virtual H323ServiceControlSession * CreateServiceControlSession(
      const H225_ServiceControlDescriptor & contents
    );
  //@}
#endif // H323_H248

  /**@name Other services */
  //@{
#ifdef H323_T120
    /**Create an instance of the T.120 protocol handler.
       This is called when the OpenLogicalChannel subsystem requires that
       a T.120 channel be established.

       Note that if the application overrides this it should return a pointer to a
       heap variable (using new) as it will be automatically deleted when the
       H323Connection is deleted.

       The default behavour returns NULL.
      */
    virtual OpalT120Protocol * CreateT120ProtocolHandler(
      const H323Connection & connection  ///< Connection for which T.120 handler created
    ) const;
#endif

#ifdef H323_T38
    /**Create an instance of the T.38 protocol handler.
       This is called when the OpenLogicalChannel subsystem requires that
       a T.38 fax channel be established.

       Note that if the application overrides this it should return a pointer to a
       heap variable (using new) as it will be automatically deleted when the
       H323Connection is deleted.

       The default behavour returns NULL.
      */
    virtual OpalT38Protocol * CreateT38ProtocolHandler(
      const H323Connection & connection  ///< Connection for which T.38 handler created
    ) const;
  //@}
#endif

#if H323_H224

	/** Create an instance of the H.224 protocol handler.
        This is called when the subsystem requires that a H.224 channel be established.
		
        Note that if the application overrides this it should return a pointer to a
        heap variable (using new) as it will be automatically deleted when the Connection
        is deleted.
		
        The default behaviour creates a new OpalH224Handler.
      */
    virtual OpalH224Handler * CreateH224ProtocolHandler(
      H323Connection & connection, 
      unsigned sessionID
    ) const;
	
    /** Create an instance of the H.224 protocol handler.
        This is called when the subsystem requires that a H.224 channel be established.
		
        Note that if the application overrides this it should return a pointer to a
        heap variable (using new) as it will be automatically deleted when the Connection
        is deleted.
		
        The default behaviour creates a new OpalH281Handler.
      */
    virtual OpalH281Handler * CreateH281ProtocolHandler(
      OpalH224Handler & h224Handler
    ) const;
#endif

#ifdef H323_FILE
    /** Open File Transfer Session
	    Use this to initiate a file transfer.
	  */
	BOOL OpenFileTransferSession(
		const PString & token,   ///< Connection Token
		H323ChannelNumber & num  ///< Opened Channel number
	);

	/** Open a File Transfer Channel.
        This is called when the subsystem requires that a File Transfer channel be established.

		An implementer should override this function to facilitate file transfer. 
		If transmitting, list of files should be populated to notify the channel which files to read.
		If receiving, the list of files should be altered to include path information for the storage
		of received files.
		
        The default behaviour returns FALSE to indicate File Transfer is not implemented. 
      */
    virtual BOOL OpenFileTransferChannel(H323Connection & connection,         ///< Connection
										   H323Channel::Directions dir,       ///< direction of channel
						                   H323FileTransferList & filelist    ///< Transfer File List
										   ); 
#endif

  /**@name Additional call services */
  //@{
    /** Called when an endpoint receives a SETUP PDU with a
        conference goal of "invite"
      
        The default behaviour is to return FALSE, which will close the connection
     */
    virtual BOOL OnConferenceInvite(
      BOOL sending,                       ///< direction
      const H323Connection * connection,  ///< Connection
      const H323SignalPDU & setupPDU      ///< PDU message
    );

    /** Called when an endpoint receives a SETUP PDU with a
        conference goal of "callIndependentSupplementaryService"
      
        The default behaviour is to return FALSE, which will close the connection
     */
    virtual BOOL OnSendCallIndependentSupplementaryService(
      const H323Connection * connection,  ///< Connection
      H323SignalPDU & pdu                 ///< PDU message
    );

    virtual BOOL OnReceiveCallIndependentSupplementaryService(
      const H323Connection * connection,  ///< Connection
      const H323SignalPDU & pdu                 ///< PDU message
    );

    /** Called when an endpoint receives a SETUP PDU with a
        conference goal of "capability_negotiation"
      
        The default behaviour is to return FALSE, which will close the connection
     */
    virtual BOOL OnNegotiateConferenceCapabilities(
      const H323SignalPDU & setupPDU
    );
  //@}

  /**@name Member variable access */
  //@{
    /**Set the user name to be used for the local end of any connections. This
       defaults to the logged in user as obtained from the
       PProcess::GetUserName() function.

       Note that this name is technically the first alias for the endpoint.
       Additional aliases may be added by the use of the AddAliasName()
       function, however that list will be cleared when this function is used.
     */
    virtual void SetLocalUserName(
      const PString & name  ///< Local name of endpoint (prime alias)
    );

    /**Get the user name to be used for the local end of any connections. This
       defaults to the logged in user as obtained from the
       PProcess::GetUserName() function.
     */
    virtual const PString & GetLocalUserName() const { return localAliasNames[0]; }

    /**Add an alias name to be used for the local end of any connections. If
       the alias name already exists in the list then is is not added again.

       The list defaults to the value set in the SetLocalUserName() function.
       Note that calling SetLocalUserName() will clear the alias list.
     */
    BOOL AddAliasName(
      const PString & name  ///< New alias name to add
    );

    /**Remove an alias name used for the local end of any connections. 
       defaults to an empty list.
     */
    BOOL RemoveAliasName(
      const PString & name  ///< New alias namer to add
    );

    /**Get the user name to be used for the local end of any connections. This
       defaults to the logged in user as obtained from the
       PProcess::GetUserName() function.
     */
    const PStringList & GetAliasNames() const { return localAliasNames; }

#if P_LDAP

    /**Get the default ILS server to use for user lookup.
      */
    const PString & GetDefaultILSServer() const { return ilsServer; }

    /**Set the default ILS server to use for user lookup.
      */
    void SetDefaultILSServer(
      const PString & server
      ) { ilsServer = server; }

#endif

    /**Get the default fast start mode.
      */
    BOOL IsFastStartDisabled() const
      { return disableFastStart; }

    /**Set the default fast start mode.
      */
    void DisableFastStart(
      BOOL mode ///< New default mode
    ) { disableFastStart = mode; } 

    /**Get the default single line mode.
      */
    BOOL IsSingleLine() const
      { return singleLine; }

    /**Set the default single line mode.
      */
    void StrictSingleLine(
      BOOL mode ///< New default mode
    ) { singleLine = mode; } 

    /**Get the default H.245 tunneling mode.
      */
    BOOL IsH245TunnelingDisabled() const
      { return disableH245Tunneling; }

    /**Set the default H.245 tunneling mode.
      */
    void DisableH245Tunneling(
      BOOL mode ///< New default mode
    ) { disableH245Tunneling = mode; } 

    /**Get the default H.245 tunneling mode.
      */
    BOOL IsH245inSetupDisabled() const
      { return disableH245inSetup; }

    /**Set the default H.245 tunneling mode.
      */
    void DisableH245inSetup(
      BOOL mode ///< New default mode
    ) { disableH245inSetup = mode; } 

	/** Get the default H.245 QoS mode.
	  */
	BOOL IsH245QoSDisabled() const
	  { return disableH245QoS; }

    /** Disable H.245 QoS support
	  */
    void DisableH245QoS(
      BOOL mode ///< New default mode
    ) { disableH245QoS = mode; } 

    /**Get the detect in-band DTMF flag.
      */
    BOOL DetectInBandDTMFDisabled() const
      { return disableDetectInBandDTMF; }

    /**Set the detect in-band DTMF flag.
      */
    void DisableDetectInBandDTMF(
      BOOL mode ///< New default mode
    ) { disableDetectInBandDTMF = mode; } 

    /**Get the flag indicating the endpoint can display an amount string.
      */
    BOOL CanDisplayAmountString() const
      { return canDisplayAmountString; }

    /**Set the flag indicating the endpoint can display an amount string.
      */
    void SetCanDisplayAmountString(
      BOOL mode ///< New default mode
    ) { canDisplayAmountString = mode; } 

    /**Get the flag indicating the call will automatically clear after a time.
      */
    BOOL CanEnforceDurationLimit() const
      { return canEnforceDurationLimit; }

    /**Set the flag indicating the call will automatically clear after a time.
      */
    void SetCanEnforceDurationLimit(
      BOOL mode ///< New default mode
    ) { canEnforceDurationLimit = mode; } 

#ifdef H323_RTP_AGGREGATE
    /**Set the RTP aggregation size
      */
    void SetRTPAggregatationSize(
      PINDEX size            ///< max connections per aggregation thread. Value of 1 or zero disables aggregation
    ) { rtpAggregationSize = size; }

    /**Get the RTP aggregation size
      */
    PINDEX GetRTPAggregationSize() const
    { return rtpAggregationSize; }

    /** Get the aggregator used for RTP channels
      */
    PHandleAggregator * GetRTPAggregator();
#endif

#ifdef H323_SIGNAL_AGGREGATE
    /**Set the signalling aggregation size
      */
    void SetSignallingAggregationSize(
      PINDEX size            ///< max connections per aggregation thread. Value of 1 or zero disables aggregation
    ) { signallingAggregationSize = size; }

    /**Get the RTP aggregation size
      */
    PINDEX GetSignallingAggregationSize() const
    { return signallingAggregationSize; }

    /** Get the aggregator used for signalling channels
      */
    PHandleAggregator * GetSignallingAggregator();
#endif

#ifdef H323_H450

    /**Get Call Intrusion Protection Level of the end point.
      */
    unsigned GetCallIntrusionProtectionLevel() const { return callIntrusionProtectionLevel; }

    /**Set Call Intrusion Protection Level of the end point.
      */
    void SetCallIntrusionProtectionLevel(
      unsigned level  ///< New level from 0 to 3
    ) { PAssert(level<=3, PInvalidParameter); callIntrusionProtectionLevel = level; }

    /**Called from H.450 OnReceivedInitiateReturnError
      */
    virtual void OnReceivedInitiateReturnError();

#endif // H323_H450

#ifdef H323_AUDIO_CODECS
#ifdef P_AUDIO

    /**Set the name for the sound channel to be used for output.
       If the name is not suitable for use with the PSoundChannel class then
       the function will return FALSE and not change the device.

       This defaults to the value of the PSoundChannel::GetDefaultDevice()
       function.
     */
    virtual BOOL SetSoundChannelPlayDevice(const PString & name);
    virtual BOOL SetSoundChannelPlayDriver(const PString & name);

    /**Get the name for the sound channel to be used for output.
       This defaults to the value of the PSoundChannel::GetDefaultDevice()
       function.
     */
    const PString & GetSoundChannelPlayDevice() const { return soundChannelPlayDevice; }
    const PString & GetSoundChannelPlayDriver() const { return soundChannelPlayDriver; }

    /**Set the name for the sound channel to be used for input.
       If the name is not suitable for use with the PSoundChannel class then
       the function will return FALSE and not change the device.

       This defaults to the value of the PSoundChannel::GetDefaultDevice()
       function.
     */
    virtual BOOL SetSoundChannelRecordDevice(const PString & name);
    virtual BOOL SetSoundChannelRecordDriver(const PString & name);

    /**Get the name for the sound channel to be used for input.
       This defaults to the value of the PSoundChannel::GetDefaultDevice()
       function.
     */
    const PString & GetSoundChannelRecordDevice() const { return soundChannelRecordDevice; }
    const PString & GetSoundChannelRecordDriver() const { return soundChannelRecordDriver; }

    /**Get default the sound channel buffer depth.
      */
    unsigned GetSoundChannelBufferDepth() const { return soundChannelBuffers; }

    /**Set the default sound channel buffer depth.
      */
    void SetSoundChannelBufferDepth(
      unsigned depth    ///< New depth
    );

#endif  // P_AUDIO

    /**Get the default silence detection mode.
      */
    H323AudioCodec::SilenceDetectionMode GetSilenceDetectionMode() const
      { return defaultSilenceDetection; }

    /**Set the default silence detection mode.
      */
    void SetSilenceDetectionMode(
      H323AudioCodec::SilenceDetectionMode mode ///< New default mode
    ) { defaultSilenceDetection = mode; } 

#endif  // H323_AUDIO_CODECS

    /**Get the default mode for sending User Input Indications.
      */
    H323Connection::SendUserInputModes GetSendUserInputMode() const { return defaultSendUserInputMode; }

    /**Set the default mode for sending User Input Indications.
      */
    void SetSendUserInputMode(H323Connection::SendUserInputModes mode) { defaultSendUserInputMode = mode; }

#ifdef H323_VIDEO

    /**See if should auto-start receive video channels on connection.
     */
    BOOL CanAutoStartReceiveVideo() const { return autoStartReceiveVideo; }

    /**See if should auto-start transmit video channels on connection.
     */
    BOOL CanAutoStartTransmitVideo() const { return autoStartTransmitVideo; }

#ifdef H323_H239
    /**See if should auto-start receive extended Video channels on connection.
     */
    BOOL CanAutoStartReceiveExtVideo() const { return autoStartReceiveExtVideo; }

    /**See if should auto-start transmit extended Video channels on connection.
     */
    BOOL CanAutoStartTransmitExtVideo() const { return autoStartTransmitExtVideo; }

#endif  // H323_H239
#endif  // H323_VIDEO

#ifdef H323_T38

    /**See if should auto-start receive fax channels on connection.
     */
    BOOL CanAutoStartReceiveFax() const { return autoStartReceiveFax; }

    /**See if should auto-start transmit fax channels on connection.
     */
    BOOL CanAutoStartTransmitFax() const { return autoStartTransmitFax; }

#endif // H323_T38

    /**See if should automatically do call forward of connection.
     */
    BOOL CanAutoCallForward() const { return autoCallForward; }

    /**Get the set of listeners (incoming call transports) for this endpoint.
     */
    const H323ListenerList & GetListeners() const { return listeners; }

    /**Get the current capability table for this endpoint.
     */
    const H323Capabilities & GetCapabilities() const { return capabilities; }

    /**Endpoint types.
     */
    enum TerminalTypes {
      e_TerminalOnly = 50,
      e_TerminalAndMC = 70,
      e_GatewayOnly = 60,
      e_GatewayAndMC = 80,
      e_GatewayAndMCWithDataMP = 90,
      e_GatewayAndMCWithAudioMP = 100,
      e_GatewayAndMCWithAVMP = 110,
      e_GatekeeperOnly = 120,
      e_GatekeeperWithDataMP = 130,
      e_GatekeeperWithAudioMP = 140,
      e_GatekeeperWithAVMP = 150,
      e_MCUOnly = 160,
      e_MCUWithDataMP = 170,
      e_MCUWithAudioMP = 180,
      e_MCUWithAVMP = 190
    };

    /**Get the endpoint terminal type.
     */
    TerminalTypes GetTerminalType() const { return terminalType; }

    /**Determine if endpoint is terminal type.
     */
    BOOL IsTerminal() const;

    /**Determine if endpoint is gateway type.
     */
    BOOL IsGateway() const;

    /**Determine if endpoint is gatekeeper type.
     */
    BOOL IsGatekeeper() const;

    /**Determine if endpoint is gatekeeper type.
     */
    BOOL IsMCU() const;

#ifdef H323_AUDIO_CODECS
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
#endif

    /**Get the initial bandwidth parameter.
     */
    unsigned GetInitialBandwidth() const { return initialBandwidth; }

    /**Get the initial bandwidth parameter.
     */
    void SetInitialBandwidth(unsigned bandwidth) { initialBandwidth = bandwidth; }

    /**Called when an outgoing PDU requires a feature set
     */
    virtual BOOL OnSendFeatureSet(unsigned, H225_FeatureSet &);

    /**Called when an incoming PDU contains a feature set
     */
    virtual void OnReceiveFeatureSet(unsigned, const H225_FeatureSet &);

	/**Load the Base FeatureSet usually called when you initialise the endpoint prior to 
	   registering with a gatekeeper.
	  */
	virtual void LoadBaseFeatureSet();

    /**Handle Unsolicited Information PDU received on the signal listening socket not
       associated with a connection.
     */
	virtual BOOL HandleUnsolicitedInformation(const H323SignalPDU & pdu);

#ifdef H323_H460
	/** Get the Endpoint FeatureSet
	    This creates a new instance of the featureSet
	 */
    H460_FeatureSet * GetFeatureSet() {  return features.DeriveNewFeatureSet(); };

    /** Is the FeatureSet disabled
	  */
	BOOL FeatureSetDisabled()  {  return disableH460; }

	/** Disable all FeatureSets. Use this for pre H323v4 interoperability
	  */
	void FeatureSetDisable()  {  disableH460 = TRUE;  }

#endif

#ifdef H323_AEC
	BOOL AECEnabled()   {  return algoAEC; }
	int AECAlgo()   {  return algoAEC; }

	void SetAECAlgo(int algo)  { algoAEC = algo; }
#endif
        int agc;

#ifdef P_STUN

    /**Return the STUN server to use.
       Returns NULL if address is a local address as per IsLocalAddress().
       Always returns the STUN server if address is zero.
       Note, the pointer is NOT to be deleted by the user.
      */
    PSTUNClient * GetSTUN(
      const PIPSocket::Address & address = 0
    ) const;

    /**Set the STUN server address, is of the form host[:port]
      */
    void SetSTUNServer(
      const PString & server
    );

    /**Type of NAT detected (if available) when initialing STUN Client
	  */
	virtual BOOL STUNNatType(int type) { return FALSE; };

    /** Retrieve the first available 
        NAT Traversal Techniques
     */
    PNatMethod * GetPreferedNatMethod(
        const PIPSocket::Address & address = 0 
	);

    /** Get the Nat Methods List
       */
    PNatStrategy GetNatMethods();

#endif // P_NONCORE

    virtual BOOL OnUnsolicitedInformation(const H323SignalPDU & pdu)
    { return FALSE; }

    /**Determine if the address is "local", ie does not need STUN
     */
    virtual BOOL IsLocalAddress(
      const PIPSocket::Address & remoteAddress
    ) const;

    /**Provide TCP address translation hook
     */
    virtual void TranslateTCPAddress(
      PIPSocket::Address & /*localAddr*/,
      const PIPSocket::Address & /*remoteAddr */
    ) { }
    void InternalTranslateTCPAddress(
      PIPSocket::Address & /*localAddr*/,
      const PIPSocket::Address & /*remoteAddr */,
	  const H323Connection * conn = NULL
    );

    /**Provide TCP Port translation hook
     */
    virtual void TranslateTCPPort( 
        WORD & /*ListenPort*/,                     ///* Local listening port 
        const PIPSocket::Address & /*remoteAddr*/  ///* Remote address
    ) { };

    /**Get the TCP port number base for H.245 channels
     */
    WORD GetTCPPortBase() const { return tcpPorts.base; }

    /**Get the TCP port number base for H.245 channels.
     */
    WORD GetTCPPortMax() const { return tcpPorts.max; }

    /**Set the TCP port number base and max for H.245 channels.
     */
    void SetTCPPorts(unsigned tcpBase, unsigned tcpMax);

    /**Get the next TCP port number for H.245 channels
     */
    WORD GetNextTCPPort();

    /**Get the UDP port number base for RAS channels
     */
    WORD GetUDPPortBase() const { return udpPorts.base; }

    /**Get the UDP port number base for RAS channels.
     */
    WORD GetUDPPortMax() const { return udpPorts.max; }

    /**Set the TCP port number base and max for H.245 channels.
     */
    void SetUDPPorts(unsigned udpBase, unsigned udpMax);

    /**Get the next UDP port number for RAS channels
     */
    WORD GetNextUDPPort();

    /**Get the UDP port number base for RTP channels.
     */
    WORD GetRtpIpPortBase() const { return rtpIpPorts.base; }

    /**Get the max UDP port number for RTP channels.
     */
    WORD GetRtpIpPortMax() const { return rtpIpPorts.max; }

    /**Set the UDP port number base and max for RTP channels.
     */
    void SetRtpIpPorts(unsigned udpBase, unsigned udpMax);

    /**Get the UDP port number pair for RTP channels.
     */
    WORD GetRtpIpPortPair();

    /**Get the IP Type Of Service byte for RTP channels.
     */
    BYTE GetRtpIpTypeofService() const { return rtpIpTypeofService; }

    /**Set the IP Type Of Service byte for RTP channels.
     */
    void SetRtpIpTypeofService(unsigned tos) { rtpIpTypeofService = (BYTE)tos; }

    /**Get the IP Type Of Service byte for TCP channels.
     */
    BYTE GetTcpIpTypeofService() const { return tcpIpTypeofService; }

    /**Set the IP Type Of Service byte for TCP channels.
     */
    void SetTcpIpTypeofService(unsigned tos) { tcpIpTypeofService = (BYTE)tos; }

    /** Get the default timeout for connecting via TCP
      */
    const PTimeInterval & GetSignallingChannelConnectTimeout() const { return signallingChannelConnectTimeout; }

    /**Get the default timeout for calling another endpoint.
     */
    const PTimeInterval & GetSignallingChannelCallTimeout() const { return signallingChannelCallTimeout; }

    /**Get the default timeout for incoming H.245 connection.
     */
    const PTimeInterval & GetControlChannelStartTimeout() const { return controlChannelStartTimeout; }

    /**Get the default timeout for waiting on an end session.
     */
    const PTimeInterval & GetEndSessionTimeout() const { return endSessionTimeout; }

    /**Get the default timeout for master slave negotiations.
     */
    const PTimeInterval & GetMasterSlaveDeterminationTimeout() const { return masterSlaveDeterminationTimeout; }

    /**Get the default retries for H245 master slave negotiations.
     */
    unsigned GetMasterSlaveDeterminationRetries() const { return masterSlaveDeterminationRetries; }

    /**Get the default timeout for H245 capability exchange negotiations.
     */
    const PTimeInterval & GetCapabilityExchangeTimeout() const { return capabilityExchangeTimeout; }

    /**Get the default timeout for H245 logical channel negotiations.
     */
    const PTimeInterval & GetLogicalChannelTimeout() const { return logicalChannelTimeout; }

    /**Get the default timeout for H245 request mode negotiations.
     */
    const PTimeInterval & GetRequestModeTimeout() const { return logicalChannelTimeout; }

    /**Get the default timeout for H245 round trip delay negotiations.
     */
    const PTimeInterval & GetRoundTripDelayTimeout() const { return roundTripDelayTimeout; }

    /**Get the default rate H245 round trip delay is calculated by connection.
     */
    const PTimeInterval & GetRoundTripDelayRate() const { return roundTripDelayRate; }

    /**Get the flag for clearing a call if the round trip delay calculation fails.
     */
    BOOL ShouldClearCallOnRoundTripFail() const { return clearCallOnRoundTripFail; }

    /**Get the amount of time with no media that should cause call to clear
     */
    const PTimeInterval & GetNoMediaTimeout() const;

    /**Set the amount of time with no media that should cause call to clear
     */
    BOOL SetNoMediaTimeout(PTimeInterval newInterval);

    /**Get the default timeout for GatekeeperRequest and Gatekeeper discovery.
     */
    const PTimeInterval & GetGatekeeperRequestTimeout() const { return gatekeeperRequestTimeout; }

    /**Get the default retries for GatekeeperRequest and Gatekeeper discovery.
     */
    unsigned GetGatekeeperRequestRetries() const { return gatekeeperRequestRetries; }

    /**Get the default timeout for RAS protocol transactions.
     */
    const PTimeInterval & GetRasRequestTimeout() const { return rasRequestTimeout; }

    /**Get the default retries for RAS protocol transations.
     */
    unsigned GetRasRequestRetries() const { return rasRequestRetries; }

    /**Get the default time for gatekeeper to reregister.
       A value of zero disables the keep alive facility.
     */
    const PTimeInterval & GetGatekeeperTimeToLive() const { return registrationTimeToLive; }

    /**Get the iNow Gatekeeper Access Token OID.
     */
    const PString & GetGkAccessTokenOID() const { return gkAccessTokenOID; }

    /**Set the iNow Gatekeeper Access Token OID.
     */
    void SetGkAccessTokenOID(const PString & token) { gkAccessTokenOID = token; }

    /**Get flag to indicate whether to send GRQ on gatekeeper registration
     */
    BOOL GetSendGRQ() const
    { return sendGRQ; }

    /**Sent flag to indicate whether to send GRQ on gatekeeper registration
     */
    void SetSendGRQ(BOOL v) 
    { sendGRQ = v; }

#ifdef H323_H450

    /**Get the default timeout for Call Transfer Timer CT-T1.
     */
    const PTimeInterval & GetCallTransferT1() const { return callTransferT1; }

    /**Get the default timeout for Call Transfer Timer CT-T2.
     */
    const PTimeInterval & GetCallTransferT2() const { return callTransferT2; }

    /**Get the default timeout for Call Transfer Timer CT-T3.
     */
    const PTimeInterval & GetCallTransferT3() const { return callTransferT3; }

    /**Get the default timeout for Call Transfer Timer CT-T4.
     */
    const PTimeInterval & GetCallTransferT4() const { return callTransferT4; }

    /** Get Call Intrusion timers timeout */
    const PTimeInterval & GetCallIntrusionT1() const { return callIntrusionT1; }
    const PTimeInterval & GetCallIntrusionT2() const { return callIntrusionT2; }
    const PTimeInterval & GetCallIntrusionT3() const { return callIntrusionT3; }
    const PTimeInterval & GetCallIntrusionT4() const { return callIntrusionT4; }
    const PTimeInterval & GetCallIntrusionT5() const { return callIntrusionT5; }
    const PTimeInterval & GetCallIntrusionT6() const { return callIntrusionT6; }

    /**Get the dictionary of <callIdentities, connections>
     */
    H323CallIdentityDict& GetCallIdentityDictionary() { return secondaryConnectionsActive; }

    /**Get the next available invoke Id for H450 operations
      */
    unsigned GetNextH450CallIdentityValue() const { return ++nextH450CallIdentity; }

#endif // H323_H450

    /**Get the default stack size of cleaner threads.
     */
    PINDEX GetCleanerThreadStackSize() const { return cleanerThreadStackSize; }

    /**Get the default stack size of listener threads.
     */
    PINDEX GetListenerThreadStackSize() const { return listenerThreadStackSize; }

    /**Get the default stack size of signalling channel threads.
     */
    PINDEX GetSignallingThreadStackSize() const { return signallingThreadStackSize; }

    /**Get the default stack size of control channel threads.
     */
    PINDEX GetControlThreadStackSize() const { return controlThreadStackSize; }

    /**Get the default stack size of logical channel threads.
     */
    PINDEX GetChannelThreadStackSize() const { return logicalThreadStackSize; }

    /**Get the default stack size of RAS channel threads.
     */
    PINDEX GetRasThreadStackSize() const { return rasThreadStackSize; }

    /**Get the default stack size of jitter buffer threads.
     */
    PINDEX GetJitterThreadStackSize() const { return jitterThreadStackSize; }

    /** Get the priority at which channel threads run
      */
    PThread::Priority GetChannelThreadPriority() const { return channelThreadPriority; }

    H323ConnectionDict GetConnections() { return connectionsActive; };

  //@}

    /**
     * default settings H.221 settings
     */
    static BYTE defaultT35CountryCode;
    static BYTE defaultT35Extension;
    static WORD defaultManufacturerCode;

  protected:
    H323Gatekeeper * InternalCreateGatekeeper(H323Transport * transport);
    BOOL InternalRegisterGatekeeper(H323Gatekeeper * gk, BOOL discovered);
    H323Connection * FindConnectionWithoutLocks(const PString & token);
    virtual H323Connection * InternalMakeCall(
      const PString & existingToken,/// Existing connection to be transferred
      const PString & callIdentity, /// Call identity of the secondary call (if it exists)
      unsigned capabilityLevel,     /// Intrusion capability level
      const PString & remoteParty,  /// Remote party to call
      H323Transport * transport,    /// Transport to use for call.
      PString & token,              /// String to use/receive token for connection
      void * userData               /// user data to pass to CreateConnection
    );

    // Configuration variables, commonly changed
    PStringList localAliasNames;

#ifdef H323_AUDIO_CODECS
    H323AudioCodec::SilenceDetectionMode defaultSilenceDetection;
    unsigned minAudioJitterDelay;
    unsigned maxAudioJitterDelay;
#ifdef P_AUDIO
    PString     soundChannelPlayDevice;
    PString     soundChannelPlayDriver;
    PString     soundChannelRecordDevice;
    PString     soundChannelRecordDriver;
    unsigned    soundChannelBuffers;
#endif // P_AUDIO
#endif // H323_AUDIO_CODECS

#ifdef H323_VIDEO
    PString     videoChannelPlayDevice;
    PString     videoChannelRecordDevice;
    BOOL        autoStartReceiveVideo;
    BOOL        autoStartTransmitVideo;

#ifdef H323_H239
    BOOL        autoStartReceiveExtVideo;
    BOOL        autoStartTransmitExtVideo;
#endif // H323_H239
#endif // H323_VIDEO

#ifdef H323_T38
    BOOL        autoStartReceiveFax;
    BOOL        autoStartTransmitFax;
#endif // H323_T38

    BOOL        autoCallForward;
    BOOL        disableFastStart;
    BOOL        singleLine;
    BOOL        disableH245Tunneling;
    BOOL        disableH245inSetup;
	BOOL        disableH245QoS;
    BOOL        disableDetectInBandDTMF;
    BOOL        canDisplayAmountString;
    BOOL        canEnforceDurationLimit;

#ifdef H323_H450
    unsigned    callIntrusionProtectionLevel;
#endif // H323_H450

    H323Connection::SendUserInputModes defaultSendUserInputMode;

#ifdef P_LDAP
    PString     ilsServer;
#endif // P_LDAP

    // Some more configuration variables, rarely changed.
    BYTE          rtpIpTypeofService;
    BYTE          tcpIpTypeofService;
    PTimeInterval signallingChannelConnectTimeout;
    PTimeInterval signallingChannelCallTimeout;
    PTimeInterval controlChannelStartTimeout;
    PTimeInterval endSessionTimeout;
    PTimeInterval masterSlaveDeterminationTimeout;
    unsigned      masterSlaveDeterminationRetries;
    PTimeInterval capabilityExchangeTimeout;
    PTimeInterval logicalChannelTimeout;
    PTimeInterval requestModeTimeout;
    PTimeInterval roundTripDelayTimeout;
    PTimeInterval roundTripDelayRate;
    PTimeInterval noMediaTimeout;
    PTimeInterval gatekeeperRequestTimeout;
    unsigned      gatekeeperRequestRetries;
    PTimeInterval rasRequestTimeout;
    unsigned      rasRequestRetries;
    PTimeInterval registrationTimeToLive;
    PString       gkAccessTokenOID;
    BOOL          sendGRQ;

    unsigned initialBandwidth;  // in 100s of bits/sev
    BOOL     clearCallOnRoundTripFail;

    struct PortInfo {
      void Set(
        unsigned base,
        unsigned max,
        unsigned range,
        unsigned dflt
      );
      WORD GetNext(
        unsigned increment
      );

      PMutex mutex;
      WORD   base;
      WORD   max;
      WORD   current;
    } tcpPorts, udpPorts, rtpIpPorts;

#ifdef P_STUN
    PSTUNClient * stun;
	BOOL disableSTUNTranslate;
    PNatStrategy natMethods;
#endif

    BYTE t35CountryCode;
    BYTE t35Extension;
    WORD manufacturerCode;

    TerminalTypes terminalType;

#ifdef H323_H450

    /* Protect against absence of a response to the ctIdentify reqest
       (Transferring Endpoint - Call Transfer with a secondary Call) */
    PTimeInterval callTransferT1;
    /* Protect against failure of completion of the call transfer operation
       involving a secondary Call (Transferred-to Endpoint) */
    PTimeInterval callTransferT2;
    /* Protect against failure of the Transferred Endpoint not responding
       within sufficient time to the ctInitiate APDU (Transferring Endpoint) */
    PTimeInterval callTransferT3;
    /* May optionally operate - protects against absence of a response to the
       ctSetup request (Transferred Endpoint) */
    PTimeInterval callTransferT4;

    /** Call Intrusion Timers */
    PTimeInterval callIntrusionT1;
    PTimeInterval callIntrusionT2;
    PTimeInterval callIntrusionT3;
    PTimeInterval callIntrusionT4;
    PTimeInterval callIntrusionT5;
    PTimeInterval callIntrusionT6;

    H323CallIdentityDict secondaryConnectionsActive;

    mutable PAtomicInteger nextH450CallIdentity; 
            /// Next available callIdentity for H450 Transfer operations via consultation.

#endif // H323_H450

    PINDEX cleanerThreadStackSize;
    PINDEX listenerThreadStackSize;
    PINDEX signallingThreadStackSize;
    PINDEX controlThreadStackSize;
    PINDEX logicalThreadStackSize;
    PINDEX rasThreadStackSize;
    PINDEX jitterThreadStackSize;

#ifdef H323_RTP_AGGREGATE
    PINDEX rtpAggregationSize;
    PHandleAggregator * rtpAggregator;
#endif

#ifdef H323_SIGNAL_AGGREGATE
    PINDEX signallingAggregationSize;
    PHandleAggregator * signallingAggregator;
#endif

    PThread::Priority channelThreadPriority;

    // Dynamic variables
    H323ListenerList listeners;
    H323Capabilities capabilities;
    H323Gatekeeper * gatekeeper;
    PString          gatekeeperPassword;

    H323ConnectionDict       connectionsActive;

    PMutex                   connectionsMutex;
    PMutex                   noMediaMutex;
    PStringSet               connectionsToBeCleaned;
    H323ConnectionsCleaner * connectionsCleaner;
    PSyncPoint               connectionsAreCleaned;

#ifndef DISABLE_CALLAUTH
    // Call Authentication
    PString EPSecurityUserName;	   /// Local UserName Authenticated Call 
    PString EPSecurityPassword;	   /// Local Password Authenticated Call		
    BOOL isSecureCall;			   /// Flag to Specify Call to make is Authenticated.
    EPSecurityPolicy CallAuthPolicy;   /// Incoming Call Authentication acceptance level
    H235AuthenticatorList EPAuthList;  /// List of Usernames & Password to check incoming call Against				   
#endif

#ifdef H323_H460
	H460_FeatureSet features;
	BOOL disableH460;
#endif

#ifdef H323_AEC
    int algoAEC;
#endif

#ifdef H323_GNUGK
    void RegInvokeReRegistration();
	PMutex reregmutex;
	GNUGK_Feature * gnugk;
    PThread  *  RegThread;
    PDECLARE_NOTIFIER(PThread, H323EndPoint, RegMethod);
#endif
};


#endif // __OPAL_H323EP_H


/////////////////////////////////////////////////////////////////////////////
