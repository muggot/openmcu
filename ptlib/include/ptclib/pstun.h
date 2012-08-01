/*
 * pstun.h
 *
 * STUN client
 *
 * Portable Windows Library
 *
 * Copyright (c) 2003 Equivalence Pty. Ltd.
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
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: pstun.h,v $
 * Revision 1.18  2007/09/25 14:25:32  hfriederich
 * Allow to get STUN server address as PIPSocket::Address instance
 *
 * Revision 1.17  2007/08/22 05:04:39  rjongbloed
 * Added ability to set a specific local port for STUN created sockets.
 *
 * Revision 1.16  2007/07/22 03:07:31  rjongbloed
 * Added parameter so can bind STUN socket to specific interface.
 *
 * Revision 1.15  2007/04/19 04:33:53  csoutheren
 * Fixed problems with pre-compiled headers
 *
 * Revision 1.14  2007/02/11 13:13:18  shorne
 * Added GetName function
 *
 * Revision 1.13  2006/12/23 15:08:00  shorne
 * Now Factory loaded for ease of addition of new NAT Methods
 *
 * Revision 1.12  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.11  2005/07/13 11:15:15  csoutheren
 * Backported NAT abstraction files from isvo branch
 *
 * Revision 1.10  2005/06/20 10:55:16  rjongbloed
 * Changed the timeout and retries so if there is a blocking firewall it does not take 15 seconds to find out!
 * Added access functions so timeout and retries are application configurable.
 * Added function (and << operator) to get NAT type enum as string.
 *
 * Revision 1.9.4.1  2005/04/25 13:21:36  shorne
 * Add Support for other NAT methods
 *
 * Revision 1.9  2004/11/25 07:23:46  csoutheren
 * Added IsSupportingRTP function to simplify detecting when STUN supports RTP
 *
 * Revision 1.8  2004/03/14 05:47:52  rjongbloed
 * Fixed incorrect detection of symmetric NAT (eg Linux masquerading) and also
 *   some NAT systems which are partially blocked due to firewall rules.
 *
 * Revision 1.7  2004/02/24 11:15:48  rjongbloed
 * Added function to get external router address, also did a bunch of documentation.
 *
 * Revision 1.6  2004/01/17 17:54:02  rjongbloed
 * Added function to get server name from STUN client.
 *
 * Revision 1.5  2003/10/05 00:56:25  rjongbloed
 * Rewrite of STUN to not to use imported code with undesirable license.
 *
 * Revision 1.4  2003/02/05 06:26:49  robertj
 * More work in making the STUN usable for Symmetric NAT systems.
 *
 * Revision 1.3  2003/02/04 07:01:02  robertj
 * Added ip/port version of constructor.
 *
 * Revision 1.2  2003/02/04 05:05:55  craigs
 * Added new functions
 *
 * Revision 1.1  2003/02/04 03:31:04  robertj
 * Added STUN
 *
 */

#ifndef _PSTUN_H
#define _PSTUN_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#include <ptclib/pnat.h>
#include <ptlib/sockets.h>


/**UDP socket that has been created by the STUN client.
  */
class PSTUNUDPSocket : public PUDPSocket
{
  PCLASSINFO(PSTUNUDPSocket, PUDPSocket);
  public:
    PSTUNUDPSocket();

    virtual BOOL GetLocalAddress(
      Address & addr    ///< Variable to receive hosts IP address
    );
    virtual BOOL GetLocalAddress(
      Address & addr,    ///< Variable to receive peer hosts IP address
      WORD & port        ///< Variable to receive peer hosts port number
    );

  protected:
    PIPSocket::Address externalIP;

  friend class PSTUNClient;
};


/**STUN client.
  */
class PSTUNClient : public PNatMethod
{
  PCLASSINFO(PSTUNClient, PNatMethod);
  public:
    enum {
      DefaultPort = 3478
    };

    PSTUNClient();

    PSTUNClient(
      const PString & server,
      WORD portBase = 0,
      WORD portMax = 0,
      WORD portPairBase = 0,
      WORD portPairMax = 0
    );
    PSTUNClient(
      const PIPSocket::Address & serverAddress,
      WORD serverPort = DefaultPort,
      WORD portBase = 0,
      WORD portMax = 0,
      WORD portPairBase = 0,
      WORD portPairMax = 0
    );


    void Initialise(
      const PString & server,
      WORD portBase = 0, 
      WORD portMax = 0,
      WORD portPairBase = 0, 
      WORD portPairMax = 0
    );

    /**Get the NAT Method Name
     */
    static PStringList GetNatMethodName() { return PStringList("STUN"); }

    virtual PStringList GetName() const
      { return GetNatMethodName(); }

    /**Get the current STUN server address and port being used.
      */
    PString GetServer() const;
    
    void GetServer(PIPSocket::Address & address, WORD & port) const;

    /**Set the STUN server to use.
       The server string may be of the form host:port. If :port is absent
       then the default port 3478 is used. The substring port can also be
       a service name as found in /etc/services. The host substring may be
       a DNS name or explicit IP address.
      */
    BOOL SetServer(
      const PString & server
    );

    /**Set the STUN server to use by IP address and port.
       If serverPort is zero then the default port of 3478 is used.
      */
    BOOL SetServer(
      const PIPSocket::Address & serverAddress,
      WORD serverPort = 0
    );

    enum NatTypes {
      UnknownNat,
      OpenNat,
      ConeNat,
      RestrictedNat,
      PortRestrictedNat,
      SymmetricNat,
      SymmetricFirewall,
      BlockedNat,
      PartialBlockedNat,
      NumNatTypes
    };

    /**Determine via the STUN protocol the NAT type for the router.
       This will cache the last determine NAT type. Use the force variable to
       guarantee an up to date value.
      */
    NatTypes GetNatType(
      BOOL force = FALSE    ///< Force a new check
    );

    /**Determine via the STUN protocol the NAT type for the router.
       As for GetNatType() but returns an English string for the type.
      */
    PString GetNatTypeName(
      BOOL force = FALSE    ///< Force a new check
    ) { return GetNatTypeString(GetNatType(force)); }

    /**Get NatTypes enumeration as an English string for the type.
      */
    static PString GetNatTypeString(
      NatTypes type   ///< NAT Type to get name of
    );

    enum RTPSupportTypes {
      RTPOK,
      RTPUnknown,
      RTPUnsupported,
      RTPIfSendMedia
    };

    /**Return an indication if the current STUN type supports RTP
      Use the force variable to guarantee an up to date test
      */
    RTPSupportTypes IsSupportingRTP(
      BOOL force = FALSE    ///< Force a new check
    );

    /**Determine the external router address.
       This will send UDP packets out using the STUN protocol to determine
       the intervening routers external IP address.

       A cached address is returned provided it is no older than the time
       specified.
      */
    virtual BOOL GetExternalAddress(
      PIPSocket::Address & externalAddress, ///< External address of router
      const PTimeInterval & maxAge = 1000   ///< Maximum age for caching
    );

    /**Create a single socket.
       The STUN protocol is used to create a socket for which the external IP
       address and port numbers are known. A PUDPSocket descendant is returned
       which will, in response to GetLocalAddress() return the externally
       visible IP and port rather than the local machines IP and socket.

       The will create a new socket pointer. It is up to the caller to make
       sure the socket is deleted to avoid memory leaks.

       The socket pointer is set to NULL if the function fails and returns
       FALSE.
      */
    BOOL CreateSocket(
      PUDPSocket * & socket,
      const PIPSocket::Address & binding = PIPSocket::GetDefaultIpAny(),
      WORD localPort = 0
    );

    /**Create a socket pair.
       The STUN protocol is used to create a pair of sockets with adjacent
       port numbers for which the external IP address and port numbers are
       known. PUDPSocket descendants are returned which will, in response
       to GetLocalAddress() return the externally visible IP and port rather
       than the local machines IP and socket.

       The will create new socket pointers. It is up to the caller to make
       sure the sockets are deleted to avoid memory leaks.

       The socket pointers are set to NULL if the function fails and returns
       FALSE.
      */
    virtual BOOL CreateSocketPair(
      PUDPSocket * & socket1,
      PUDPSocket * & socket2,
      const PIPSocket::Address & binding = PIPSocket::GetDefaultIpAny()
    );

    /**Get the timeout for responses from STUN server.
      */
    const PTimeInterval GetTimeout() const { return replyTimeout; }

    /**Set the timeout for responses from STUN server.
      */
    void SetTimeout(
      const PTimeInterval & timeout   ///< New timeout in milliseconds
    ) { replyTimeout = timeout; }

    /**Get the number of retries for responses from STUN server.
      */
    PINDEX GetRetries() const { return pollRetries; }

    /**Set the number of retries for responses from STUN server.
      */
    void SetRetries(
      PINDEX retries    ///< Number of retries
    ) { pollRetries = retries; }

    /**Get the number of sockets to create in attempt to get a port pair.
       RTP requires a pair of consecutive ports. To get this several sockets
       must be opened and fired through the NAT firewall to get a pair. The
       busier the firewall the more sockets will be required.
      */
    PINDEX GetSocketsForPairing() const { return numSocketsForPairing; }

    /**Set the number of sockets to create in attempt to get a port pair.
       RTP requires a pair of consecutive ports. To get this several sockets
       must be opened and fired through the NAT firewall to get a pair. The
       busier the firewall the more sockets will be required.
      */
    void SetSocketsForPairing(
      PINDEX numSockets   ///< Number opf sockets to create
    ) { numSocketsForPairing = numSockets; }

    /**Returns whether the Nat Method is ready and available in
       assisting in NAT Traversal. The principal is this function is
       to allow the EP to detect various methods and if a method
       is detected then this method is available for NAT traversal.
       The availablity of the STUN Method is dependant on the Type
       of NAT being used.
     */
    virtual BOOL IsAvailable();

  protected:
    PIPSocket::Address serverAddress;
    WORD               serverPort;
    PTimeInterval      replyTimeout;
    PINDEX             pollRetries;
    PINDEX             numSocketsForPairing;

    bool OpenSocket(PUDPSocket & socket, PortInfo & portInfo, const PIPSocket::Address & binding) const;

    NatTypes           natType;
    PIPSocket::Address cachedExternalAddress;
    PTime              timeAddressObtained;
};


inline ostream & operator<<(ostream & strm, PSTUNClient::NatTypes type) { return strm << PSTUNClient::GetNatTypeString(type); }

/// Factory loader
typedef PSTUNClient PNatMethod_STUN;
PWLIB_STATIC_LOAD_PLUGIN(STUN, PNatMethod);


#endif // _PSTUN_H


// End of file ////////////////////////////////////////////////////////////////
