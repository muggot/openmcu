/*
 * transports.h
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
 * $Log: transports.h,v $
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.51.2.1  2007/02/11 00:45:20  shorne
 * Added ability to disable NAT method on a call by call basis
 *
 * Revision 1.51  2006/07/05 04:37:44  csoutheren
 * Applied 1488904 - SetPromiscuous(AcceptFromLastReceivedOnly) for T.38
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.50  2006/06/23 03:21:03  shorne
 * Added unsolicited Information support
 *
 * Revision 1.49  2006/01/20 00:32:24  csoutheren
 * First check-in of signalling aggregation code - incomplete and disabled by default
 *
 * Revision 1.48  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.47  2005/11/21 21:07:41  shorne
 * Added GnuGK Nat support
 *
 * Revision 1.46  2005/01/16 20:39:43  csoutheren
 * Fixed problem with IPv6 INADDR_ANY
 *
 * Revision 1.45  2004/08/24 08:11:25  csoutheren
 * Added initial support for receiving broadcasts on Linux
 *
 * Revision 1.44  2004/05/13 02:26:13  dereksmithies
 * Fixes so make docs does not generate warning messages about brackets.
 *
 * Revision 1.43  2003/12/29 13:28:45  dominance
 * fixed docbook syntax trying to generate LaTeX formula with ip$10.x.x.x.
 *
 * Revision 1.42  2003/04/10 09:44:55  robertj
 * Added associated transport to new GetInterfaceAddresses() function so
 *   interfaces can be ordered according to active transport links. Improves
 *   interoperability.
 * Replaced old listener GetTransportPDU() with GetInterfaceAddresses()
 *   and H323SetTransportAddresses() functions.
 *
 * Revision 1.41  2003/04/10 01:03:25  craigs
 * Added functions to access to lists of interfaces
 *
 * Revision 1.40  2003/03/21 05:24:02  robertj
 * Added setting of remote port in UDP transport constructor.
 *
 * Revision 1.39  2003/02/06 04:29:23  robertj
 * Added more support for adding things to H323TransportAddressArrays
 *
 * Revision 1.38  2002/11/21 06:39:56  robertj
 * Changed promiscuous mode to be three way. Fixes race condition in gkserver
 *   which can cause crashes or more PDUs to be sent to the wrong place.
 *
 * Revision 1.37  2002/11/10 08:10:43  robertj
 * Moved constants for "well known" ports to better place (OPAL change).
 *
 * Revision 1.36  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.35  2002/07/22 09:40:15  robertj
 * Added ability to automatically convert string arrays, lists sorted lists
 *   directly to H323TransportAddressArray.
 *
 * Revision 1.34  2002/07/02 10:02:29  robertj
 * Added H323TransportAddress::GetIpAddress() so don't have to provide port
 *   when you don't need it as in GetIpAndPort(),.
 *
 * Revision 1.33  2002/06/28 03:34:25  robertj
 * Fixed issues with address translation on gatekeeper RAS channel.
 *
 * Revision 1.32  2002/06/12 03:51:59  robertj
 * Added function to compare two transport addresses in a more intelligent
 *   way that strict string comparison. Takes into account wildcarding.
 *
 * Revision 1.31  2002/02/11 04:20:25  robertj
 * Fixed documentation errors, thanks Horacio J. Peña
 *
 * Revision 1.30  2002/02/05 23:29:23  robertj
 * Changed default for H.323 listener to reuse addresses.
 *
 * Revision 1.29  2001/12/22 01:48:12  robertj
 * Added ability to use local and remote port from transport channel as well
 *   as explicit port in H.245 address PDU setting routine.
 * Added PrintOn() to listener and transport for tracing purposes.
 *
 * Revision 1.28  2001/07/17 04:44:29  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 * Revision 1.27  2001/06/25 02:30:46  robertj
 * Allowed TCP listener socket to be opened in non-exclusive mode
 *   (ie SO_REUSEADDR) to avoid daemon restart problems.
 *
 * Revision 1.26  2001/06/22 00:14:14  robertj
 * Added ConnectTo() function to conencto specific address.
 * Added promiscuous mode for UDP channel.
 *
 * Revision 1.25  2001/05/17 06:37:02  robertj
 * Added multicast gatekeeper discovery support.
 *
 * Revision 1.24  2001/04/09 08:43:39  robertj
 * Added ability to get transport address for a listener.
 *
 * Revision 1.23  2001/03/02 06:59:57  robertj
 * Enhanced the globally unique identifier class.
 *
 * Revision 1.22  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.21  2001/01/23 05:08:04  robertj
 * Fixed bug when trying to clear call while waiting on H.245 channel connect, thanks Yura Aksyonov.
 *
 * Revision 1.20  2000/10/20 06:18:58  robertj
 * Fixed very small race condition on creating new connectionon incoming call.
 * Fixed memory/socket leak if do TCP connect and don't send valid setup PDU.
 *
 * Revision 1.19  2000/10/04 05:59:09  robertj
 * Minor reorganisation of the H.245 secondary channel start up to make it simpler
 *    to override its behaviour.
 *
 * Revision 1.18  2000/09/25 12:59:16  robertj
 * Added StartListener() function that takes a H323TransportAddress to start
 *     listeners bound to specific interfaces.
 *
 * Revision 1.17  2000/09/22 01:35:03  robertj
 * Added support for handling LID's that only do symmetric codecs.
 *
 * Revision 1.16  2000/06/07 05:47:55  robertj
 * Added call forwarding.
 *
 * Revision 1.15  2000/05/22 05:21:36  robertj
 * Fixed race condition where controlChannel variable could be used before set.
 *
 * Revision 1.14  2000/05/18 11:53:34  robertj
 * Changes to support doc++ documentation generation.
 *
 * Revision 1.13  2000/05/08 14:07:26  robertj
 * Improved the provision and detection of calling and caller numbers, aliases and hostnames.
 *
 * Revision 1.12  2000/05/02 04:32:25  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.11  1999/11/06 05:37:44  robertj
 * Complete rewrite of termination of connection to avoid numerous race conditions.
 *
 * Revision 1.10  1999/10/16 03:47:57  robertj
 * Fixed termination of gatekeeper RAS thread problem
 *
 * Revision 1.9  1999/10/10 08:59:47  robertj
 * Fixed race condition in connection shutdown
 *
 * Revision 1.8  1999/09/14 06:52:54  robertj
 * Added better support for multi-homed client hosts.
 *
 * Revision 1.7  1999/09/10 09:43:59  robertj
 * Removed attempt at determining local interface for gatekeeper, so still has problem on multi-homed hosts.
 *
 * Revision 1.6  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 * Revision 1.5  1999/08/25 05:14:21  robertj
 * Fixed problem with calling modal dialog from a background thread.
 *
 * Revision 1.4  1999/07/14 06:06:14  robertj
 * Fixed termination problems (race conditions) with deleting connection object.
 *
 * Revision 1.3  1999/07/09 06:09:49  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.2  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.1  1999/06/09 05:26:20  robertj
 * Major restructuring of classes.
 *
 */

#ifndef __TRANSPORTS_H
#define __TRANSPORTS_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include <ptlib/sockets.h>


class H225_Setup_UUIE;
class H225_TransportAddress;
class H225_ArrayOf_TransportAddress;
class H225_TransportAddress_ipAddress;

class H245_TransportAddress;

class H323SignalPDU;
class H323RasPDU;
class H323EndPoint;
class H323Connection;
class H323Listener;
class H323Transport;
class H323Gatekeeper;



///////////////////////////////////////////////////////////////////////////////

/**String representation of a transport address.
 */

class H323TransportAddress : public PString
{
  PCLASSINFO(H323TransportAddress, PString);
  public:
    H323TransportAddress() { }
    H323TransportAddress(const char *);
    H323TransportAddress(const PString &);
    H323TransportAddress(const H225_TransportAddress &);
    H323TransportAddress(const H245_TransportAddress &);
    H323TransportAddress(const PIPSocket::Address &, WORD);

    BOOL SetPDU(H225_TransportAddress & pdu) const;
    BOOL SetPDU(H245_TransportAddress & pdu) const;

    /**Determine if the two transport addresses are equivalent.
      */
    BOOL IsEquivalent(
      const H323TransportAddress & address
    );

    /**Extract the ip address from transport address.
       Returns FALSE, if the address is not an IP transport address.
      */
    BOOL GetIpAddress(
      PIPSocket::Address & ip
    ) const;

    /**Extract the ip address and port number from transport address.
       Returns FALSE, if the address is not an IP transport address.
      */
    BOOL GetIpAndPort(
      PIPSocket::Address & ip,
      WORD & port,
      const char * proto = "tcp"
    ) const;

    /**Translate the transport address to a more human readable form.
       Returns the hostname if using IP.
      */
    PString GetHostName() const;

    /**Create a listener based on this transport address.

       For example an address of "#ip$10.0.0.1:1720#" would create a TCP
       listening socket that would be bound to the specific interface
       10.0.0.1 and listens on port 1720. Note that the address
       "#ip$*:1720#" can be used to bind to INADDR_ANY.

       Also note that if the address has a trailing '+' character then the
       socket will be bound using the REUSEADDR option.
      */
    H323Listener * CreateListener(
      H323EndPoint & endpoint   ///<  Endpoint object for transport creation.
    ) const;

    /**Create a listener compatible for this address type.
       This is similar to CreateListener() but does not use the TSAP specified
       in the H323Transport. For example an address of "#ip$10.0.0.1:1720#"
       would create a TCP listening socket that would be bound to the specific
       interface 10.0.0.1 but listens on a random OS allocated port number.
      */
    H323Listener * CreateCompatibleListener(
      H323EndPoint & endpoint   ///<  Endpoint object for transport creation.
    ) const;

    /**Create a transport suitable for this address type.
      */
    H323Transport * CreateTransport(
      H323EndPoint & endpoint   ///<  Endpoint object for transport creation.
    ) const;

  protected:
    void Validate();
};


PDECLARE_ARRAY(H323TransportAddressArray, H323TransportAddress)
#ifdef DOC_PLUS_PLUS
{
#endif
  public:
    H323TransportAddressArray(
      const H323TransportAddress & address
    ) { AppendAddress(address); }
    H323TransportAddressArray(
      const H225_ArrayOf_TransportAddress & addresses
    );
    H323TransportAddressArray(
      const PStringArray & array
    ) { AppendStringCollection(array); }
    H323TransportAddressArray(
      const PStringList & list
    ) { AppendStringCollection(list); }
    H323TransportAddressArray(
      const PSortedStringList & list
    ) { AppendStringCollection(list); }

    void AppendString(
      const char * address
    );
    void AppendString(
      const PString & address
    );
    void AppendAddress(
      const H323TransportAddress & address
    );

  protected:
    void AppendStringCollection(
      const PCollection & coll
    );
};


/**This class describes a "listener" on a transport protocol.
   A "listener" is an object that listens for incoming connections on the
   particular transport. It is executed as a separate thread.

   The Main() function is used to handle incoming H.323 connections and
   dispatch them in new threads based on the actual H323Transport class. This
   is defined in the descendent class that knows what the low level transport
   is, eg H323ListenerIP for the TCP/IP protocol.

   An application may create a descendent off this class and override
   functions as required for operating the channel protocol.
 */
class H323Listener : public PThread
{
  PCLASSINFO(H323Listener, PThread);

  public:
  /**@name Construction */
  //@{
    /**Create a new listener.
     */
    H323Listener(
      H323EndPoint & endpoint      ///<  Endpoint instance for channel
    );
  //@}

  /**@name Overrides from PObject */
  //@{
    virtual void PrintOn(
      ostream & strm
    ) const;
  //@}

  /**@name Operations */
  //@{
    /** Open the listener.
      */
    virtual BOOL Open() = 0;

    /**Stop the listener thread and no longer accept incoming connections.
     */
    virtual BOOL Close() = 0;

    /**Accept a new incoming transport.
      */
    virtual H323Transport * Accept(
      const PTimeInterval & timeout  ///<  Time to wait for incoming connection
    ) = 0;

    /**Get the local transport address on which this listener may be accessed.
      */
    virtual H323TransportAddress GetTransportAddress() const = 0;

    /**Set up a transport address PDU for bidirectional logical channels.
      */
    virtual BOOL SetUpTransportPDU(
      H245_TransportAddress & pdu,         ///<  Transport addresses listening on
      const H323Transport & associatedTransport ///<  Associated transport for precendence and translation
    ) = 0;
  //@}

  protected:
    H323EndPoint & endpoint;  /// Endpoint that owns the listener.
};


PLIST(H323ListenerList, H323Listener);


/** Return a list of transport addresses corresponding to a listener list
  */
H323TransportAddressArray H323GetInterfaceAddresses(
  const H323ListenerList & listeners, ///<  List of listeners
  BOOL excludeLocalHost = TRUE,       ///<  Flag to exclude 127.0.0.1
  H323Transport * associatedTransport = NULL
                          ///<  Associated transport for precedence and translation
);

H323TransportAddressArray H323GetInterfaceAddresses(
  const H323TransportAddress & addr,  ///<  Possible INADDR_ANY address
  BOOL excludeLocalHost = TRUE,       ///<  Flag to exclude 127.0.0.1
  H323Transport * associatedTransport = NULL
                          ///<  Associated transport for precedence and translation
);

/**Set the PDU field for the list of transport addresses
  */
void H323SetTransportAddresses(
  const H323Transport & associatedTransport,   ///<  Transport for NAT address translation
  const H323TransportAddressArray & addresses, ///<  Addresses to set
  H225_ArrayOf_TransportAddress & pdu          ///<  List of PDU transport addresses
);


/**This class describes a I/O transport protocol..
   A "transport" is an object that listens for incoming connections on the
   particular transport.
 */
class H323Transport : public PIndirectChannel
{
  PCLASSINFO(H323Transport, PIndirectChannel);

  public:
  /**@name Construction */
  //@{
    /**Create a new transport channel.
     */
    H323Transport(H323EndPoint & endpoint);
    ~H323Transport();
  //@}

  /**@name Overrides from PObject */
  //@{
    virtual void PrintOn(
      ostream & strm
    ) const;
  //@}

  /**@name Operations */
  //@{
    /**Get the transport address of the local endpoint.
      */
    virtual H323TransportAddress GetLocalAddress() const = 0;

    /**Get the transport address of the remote endpoint.
      */
    virtual H323TransportAddress GetRemoteAddress() const = 0;

    /**Set remote address to connect to.
       Note that this does not necessarily initiate a transport level
       connection, but only indicates where to connect to. The actual
       connection is made by the Connect() function.
      */
    virtual BOOL SetRemoteAddress(
      const H323TransportAddress & address
    ) = 0;

    /**Connect to the remote address.
      */
    virtual BOOL Connect() = 0;

    /**Connect to the specified address.
      */
    BOOL ConnectTo(
      const H323TransportAddress & address
    ) { return SetRemoteAddress(address) && Connect(); }

    /**Close the channel.
      */
    virtual BOOL Close();

    /**Check that the transport address PDU is compatible with transport.
      */
    virtual BOOL IsCompatibleTransport(
      const H225_TransportAddress & pdu
    ) const;

    /**Set up a transport address PDU for RAS channel.
      */
    virtual void SetUpTransportPDU(
      H225_TransportAddress & pdu,
      BOOL localTsap,
	  H323Connection * connection = NULL
    ) const;

    enum {
      UseLocalTSAP = 0x10001,
      UseRemoteTSAP
    };

    /**Set up a transport address PDU for logical channel.
       If tsap is UseLocalTSAP or UseRemoteTSAP then the local or remote port
       of the transport is used, otherwise the explicit port number is used.
      */
    virtual void SetUpTransportPDU(
      H245_TransportAddress & pdu,
      unsigned tsap
    ) const;

    /// Promiscious modes for transport
    enum PromisciousModes {
      AcceptFromRemoteOnly,
      AcceptFromAnyAutoSet,
      AcceptFromAny,
      AcceptFromLastReceivedOnly,
      NumPromisciousModes
    };

    /**Set read to promiscuous mode.
       Normally only reads from the specifed remote address are accepted. This
       flag allows packets to be accepted from any remote, provided the
       underlying protocol can do so. For example TCP will do nothing.

       The Read() call may optionally set the remote address automatically to
       whatever the sender host of the last received message was.

       Default behaviour does nothing.
      */
    virtual void SetPromiscuous(
      PromisciousModes promiscuous
    );

    /**Get the transport address of the last received PDU.

       Default behaviour returns GetRemoteAddress().
      */
    virtual H323TransportAddress GetLastReceivedAddress() const;

    /**Read a protocol data unit from the transport.
       This will read using the transports mechanism for PDU boundaries, for
       example UDP is a single Read() call, while for TCP there is a TPKT
       header that indicates the size of the PDU.
      */
    virtual BOOL ReadPDU(
      PBYTEArray & pdu   ///<  PDU read from transport
    ) = 0;

    /**Extract a protocol data unit from the transport
       This is used by the aggregator to deblock the incoming data stream
       into valid PDUs.
      */
    virtual BOOL ExtractPDU(
      const PBYTEArray & pdu, 
      PINDEX & len
    ) = 0;

    /**Write a protocol data unit from the transport.
       This will write using the transports mechanism for PDU boundaries, for
       example UDP is a single Write() call, while for TCP there is a TPKT
       header that indicates the size of the PDU.
      */
    virtual BOOL WritePDU(
      const PBYTEArray & pdu  ///<  PDU to write
    ) = 0;
  //@}

  /**@name Signalling Channel */
  //@{
	/** Handle the PDU Reading for the first connection object
	  */
    BOOL HandleSignallingSocket(H323SignalPDU & pdu);

    /**Wait for first PDU and find/create connection object.
       If returns FALSE, then the transport is deleted by the calling thread.
      */
    BOOL HandleFirstSignallingChannelPDU();
  //@}

  /**@name Control Channel */
  //@{
    /**Begin the opening of a control channel.
       This sets up the channel so that the remote endpoint can connect back
       to this endpoint. This would be called on the signalling channel
       instance of a H323Transport.
      */
    virtual H323Transport * CreateControlChannel(
      H323Connection & connection
    );

    /**Finish the opening of a control channel.
       This waits for the connect backfrom the remote endpoint, completing the
       control channel open sequence.
      */
    virtual BOOL AcceptControlChannel(
      H323Connection & connection
    );

    /**Connect the control channel.
      */
    virtual void StartControlChannel(
      H323Connection & connection
    );
  //@}

  /**@name RAS Channel */
  //@{
    /**Discover a Gatekeeper on the network.
       This locates a gatekeeper on the network and associates this transport
       object with packet exchange with that gatekeeper.
      */
    virtual BOOL DiscoverGatekeeper(
      H323Gatekeeper & gk,                  ///<  Gatekeeper to set on discovery.
      H323RasPDU & pdu,                     ///<  GatekeeperRequest PDU
      const H323TransportAddress & address  ///<  Address of gatekeeper (if present)
    );
  //@}


  /**@name Member variable access */
  //@{
    /**Get the associated endpoint to this transport.
      */
    H323EndPoint & GetEndPoint() const { return endpoint; }

    /**Attach a thread to the transport.
      */
    void AttachThread(
      PThread * thread
    );

    /**Wait for associated thread to terminate.
      */
    void CleanUpOnTermination();
  //@}

  protected:
    H323EndPoint & endpoint;    /// Endpoint that owns the listener.
    PThread      * thread;      /// Thread handling the transport
    BOOL canGetInterface;
};



///////////////////////////////////////////////////////////////////////////////
// Transport classes for IP

/**This class represents a particular H323 transport using IP.
   It is used by the TCP and UDP transports.
 */
class H323TransportIP : public H323Transport
{
  PCLASSINFO(H323TransportIP, H323Transport);

  public:
    /**Create a new transport channel.
     */
    H323TransportIP(
      H323EndPoint & endpoint,    ///<  H323 End Point object
      PIPSocket::Address binding, ///<  Local interface to use
      WORD remPort                ///<  Remote port to use
    );

    /**Get the transport dependent name of the local endpoint.
      */
    virtual H323TransportAddress GetLocalAddress() const;

    /**Get the transport dependent name of the remote endpoint.
      */
    virtual H323TransportAddress GetRemoteAddress() const;

    /**Check that the transport address PDU is compatible with transport.
      */
    virtual BOOL IsCompatibleTransport(
      const H225_TransportAddress & pdu
    ) const;

    /**Set up a transport address PDU for RAS channel.
      */
    virtual void SetUpTransportPDU(
      H225_TransportAddress & pdu,
      BOOL localTsap,
	  H323Connection * connection = NULL
    ) const;

    /**Set up a transport address PDU for logical channel.
      */
    virtual void SetUpTransportPDU(
      H245_TransportAddress & pdu,
      unsigned tsap
    ) const;


  protected:
    PIPSocket::Address localAddress;  // Address of the local interface
    WORD               localPort;
    PIPSocket::Address remoteAddress; // Address of the remote host
    WORD               remotePort;
};


///////////////////////////////////////////////////////////////////////////////
// Transport classes for TCP/IP

/**This class manages H323 connections using TCP/IP transport.
 */
class H323ListenerTCP : public H323Listener
{
  PCLASSINFO(H323ListenerTCP, H323Listener);

  public:
    /**Create a new listener for the TCP/IP protocol.
     */
    H323ListenerTCP(
      H323EndPoint & endpoint,    ///<  Endpoint instance for channel
      PIPSocket::Address binding, ///<  Local interface to listen on
      WORD port,                  ///<  TCP port to listen for connections
      BOOL exclusive = FALSE      ///<  Fail if listener port in use
    );

    /** Destroy the listener thread.
      */
    ~H323ListenerTCP();
    
  // Overrides from H323Listener
    /** Open the listener.
      */
    virtual BOOL Open();

    /**Stop the listener thread and no longer accept incoming connections.
     */
    virtual BOOL Close();

    /**Accept a new incoming transport.
      */
    virtual H323Transport * Accept(
      const PTimeInterval & timeout  ///<  Time to wait for incoming connection
    );

    /**Get the local transport address on which this listener may be accessed.
      */
    virtual H323TransportAddress GetTransportAddress() const;

    /**Set up a transport address PDU for bidirectional logical channels.
      */
    virtual BOOL SetUpTransportPDU(
      H245_TransportAddress & pdu,        ///<  Transport addresses listening on
      const H323Transport & associatedTransport ///<  Associated transport for precendence and translation
    );

    WORD GetListenerPort() const { return listener.GetPort(); }


  protected:
    /**Handle incoming H.323 connections and dispatch them in new threads
       based on the H323Transport class. This is defined in the descendent
       class that knows what the low level transport is, eg H323ListenerIP
       for the TCP/IP protocol.

       Note this function does not return until the Close() function is called
       or there is some other error.
     */
    virtual void Main();


    PTCPSocket listener;
    PIPSocket::Address localAddress;
    BOOL exclusiveListener;
};


/**This class represents a particular H323 transport using TCP/IP.
 */
class H323TransportTCP : public H323TransportIP
{
  PCLASSINFO(H323TransportTCP, H323TransportIP);

  public:
    /**Create a new transport channel.
     */
    H323TransportTCP(
      H323EndPoint & endpoint,    ///<  H323 End Point object
      PIPSocket::Address binding = PIPSocket::GetDefaultIpAny(), ///<  Local interface to use
      BOOL listen = FALSE         ///<  Flag for need to wait for remote to connect
    );

    /**Destroy transport channel.
     */
    ~H323TransportTCP();

    /**Set default remote address to connect to.
       Note that this does not necessarily initiate a transport level
       connection, but only indicates where to connect to. The actual
       connection is made by the Connect() function.
      */
    virtual BOOL SetRemoteAddress(
      const H323TransportAddress & address
    );

    /**Connect to the remote party.
      */
    virtual BOOL Connect();

    /**Close the channel.
      */
    virtual BOOL Close();

    /**Read a protocol data unit from the transport.
       This will read using the transports mechanism for PDU boundaries, for
       example UDP is a single Read() call, while for TCP there is a TPKT
       header that indicates the size of the PDU.
      */
    BOOL ReadPDU(
      PBYTEArray & pdu   ///<  PDU read from transport
    );

    /**Extract a protocol data unit from the transport
      */
    BOOL ExtractPDU(
      const PBYTEArray & pdu, 
      PINDEX & len
    );
 
    /**Write a protocol data unit from the transport.
       This will write using the transports mechanism for PDU boundaries, for
       example UDP is a single Write() call, while for TCP there is a TPKT
       header that indicates the size of the PDU.
      */
    BOOL WritePDU(
      const PBYTEArray & pdu  ///<  PDU to write
    );

    /**Begin the opening of a control channel.
       This sets up the channel so that the remote endpoint can connect back
       to this endpoint.
      */
    virtual H323Transport * CreateControlChannel(
      H323Connection & connection
    );

    /**Finish the opening of a control channel.
       This waits for the connect backfrom the remote endpoint, completing the
       control channel open sequence.
      */
    virtual BOOL AcceptControlChannel(
      H323Connection & connection
    );

    /**Indicate we are waiting from remote to connect back to us.
      */
    virtual BOOL IsListening() const;


  protected:
    /**This callback is executed when the Open() function is called with
       open channels. It may be used by descendent channels to do any
       handshaking required by the protocol that channel embodies.

       The default behaviour is to simply return TRUE.

       @return
       Returns TRUE if the protocol handshaking is successful.
     */
    virtual BOOL OnOpen();


    PTCPSocket * h245listener;
};


///////////////////////////////////////////////////////////////////////////////
// Transport classes for UDP/IP

/**This class represents a particular H323 transport using UDP/IP.
 */
class H323TransportUDP : public H323TransportIP
{
  PCLASSINFO(H323TransportUDP, H323TransportIP);

  public:
    /**Create a new transport channel.
     */
    H323TransportUDP(
      H323EndPoint & endpoint,                  ///<  H323 End Point object
      PIPSocket::Address binding = PIPSocket::GetDefaultIpAny(),  ///<  Local interface to listen on
      WORD localPort = 0,                       ///<  Local port to listen on
      WORD remotePort = 0                       ///<  Remote port to connect on
    );
    ~H323TransportUDP();

    /**Set default remote address to connect to.
       Note that this does not necessarily initiate a transport level
       connection, but only indicates where to connect to. The actual
       connection is made by the Connect() function.
      */
    virtual BOOL SetRemoteAddress(
      const H323TransportAddress & address
    );

    /**Connect to the remote party.
      */
    virtual BOOL Connect();

    /**Set read to promiscuous mode.
       Normally only reads from the specifed remote address are accepted. This
       flag allows packets to be accepted from any remote, provided the
       underlying protocol can do so.

       The Read() call may optionally set the remote address automatically to
       whatever the sender host of the last received message was.

       Default behaviour sets the internal flag, so that Read() operates as
       described.
      */
    virtual void SetPromiscuous(
      PromisciousModes promiscuous
    );

    /**Get the transport address of the last received PDU.

       Default behaviour returns the lastReceivedAddress member variable.
      */
    virtual H323TransportAddress GetLastReceivedAddress() const;

    /**Read a protocol data unit from the transport.
       This will read using the transports mechanism for PDU boundaries, for
       example UDP is a single Read() call, while for TCP there is a TPKT
       header that indicates the size of the PDU.
      */
    virtual BOOL ReadPDU(
      PBYTEArray & pdu   ///<  PDU read from transport
    );

    /**Extract a protocol data unit from the transport
      */
    BOOL ExtractPDU(
      const PBYTEArray & pdu, 
      PINDEX & len
    );
 
    /**Write a protocol data unit from the transport.
       This will write using the transports mechanism for PDU boundaries, for
       example UDP is a single Write() call, while for TCP there is a TPKT
       header that indicates the size of the PDU.
      */
    virtual BOOL WritePDU(
      const PBYTEArray & pdu  ///<  PDU to write
    );

    /**Discover a Gatekeeper on the local network.
       This locates a gatekeeper on the network and associates this transport
       object with packet exchange with that gatekeeper. This broadcasts a UDP
       packet on the local network to find the gatekeeper's IP address.
      */
    virtual BOOL DiscoverGatekeeper(
      H323Gatekeeper & gk,                  ///<  Gatekeeper to set on discovery.
      H323RasPDU & pdu,                     ///<  GatekeeperRequest PDU
      const H323TransportAddress & address  ///<  Address of gatekeeper (if present)
    );

    /**Get the transport address of the local endpoint.
      */
    virtual H323TransportAddress GetLocalAddress() const;

  protected:
    PromisciousModes     promiscuousReads;
    H323TransportAddress lastReceivedAddress;
    PIPSocket::Address   lastReceivedInterface;
    WORD interfacePort;
};


#endif // __TRANSPORTS_H


/////////////////////////////////////////////////////////////////////////////
