/*
 * psockbun.h
 *
 * Socket and interface bundle code
 *
 * Portable Windows Library
 *
 * Copyright (C) 2007 Post Increment
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
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: psockbun.h,v $
 * Revision 1.14  2007/10/12 03:52:15  rjongbloed
 * Fixed broken virtual by someone changing base class function signature,
 *   and the override is silently not called. pet hate #1 about C++!
 *
 * Revision 1.13  2007/10/07 07:35:30  rjongbloed
 * Changed bundled sockets so does not return error if interface goes away it just
 *   blocks reads till the interface comes back, or is explicitly closed.
 * Also return error codes, rather than just a BOOL.
 *
 * Revision 1.12  2007/09/28 09:59:16  hfriederich
 * Allow to use PInterfaceMonitor without running monitor thread
 *
 * Revision 1.11  2007/09/25 14:27:51  hfriederich
 * Don't use STUN if interface filter is in use and STUN server is not
 * reachable through local binding. This avoids unnecessary timeouts.
 *
 * Revision 1.10  2007/09/22 04:32:03  rjongbloed
 * Fixed lock up on exit whena  gatekeeper is used.
 * Also fixed fatal "read error" (ECONNRESET) when send packet to a machine which
 *   is not listening on the specified port. No error is lgged but does not stop listener.
 *
 * Revision 1.9  2007/08/26 20:01:58  hfriederich
 * Allow to filter interfaces based on remote address
 *
 * Revision 1.8  2007/07/22 04:03:32  rjongbloed
 * Fixed issues with STUN usage in socket bundling, now OpalTransport indicates
 *   if it wants local or NAT address/port for inclusion to outgoing PDUs.
 *
 * Revision 1.7  2007/07/03 08:55:17  rjongbloed
 * Fixed various issues with handling interfaces going up, eg not being added
 *   to currently active ReadFrom().
 * Added more logging.
 *
 * Revision 1.6  2007/06/25 05:44:01  rjongbloed
 * Fixed numerous issues with "bound" managed socket, ie associating
 *   listeners to a specific named interface.
 *
 * Revision 1.5  2007/06/14 00:43:04  csoutheren
 * Removed warnings on Linux
 * Fixed Makefiles for new socket bundle code
 *
 * Revision 1.4  2007/06/10 06:26:50  rjongbloed
 * Major enhancements to the "socket bundling" feature:
 *   singleton thread for monitoring network interfaces
 *   a generic API for anything to be informed of interface changes
 *   PChannel derived class for reading/writing to bundled sockets
 *   many new API functions
 *
 * Revision 1.3  2007/05/22 11:50:47  csoutheren
 * Further implementation of socket bundle
 *
 * Revision 1.2  2007/05/21 06:35:37  csoutheren
 * Changed to be descended off PSafeObject
 *
 * Revision 1.1  2007/05/21 06:06:56  csoutheren
 * Add new socket bundle code to be used to OpalUDPListener
 *
 */

#ifndef _PSOCKBUN_H
#define _PSOCKBUN_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include <ptlib.h>
#include <ptlib/ipsock.h>
#include <ptlib/sockets.h>
#include <ptlib/safecoll.h>
#include <list>


class PSTUNClient;
class PInterfaceMonitorClient;
class PInterfaceFilter;


//////////////////////////////////////////////////

/** This class is a singleton that will monitor the network interfaces on a
    machine and update a list aof clients on any changes to the number or
    addresses of the interfaces.

    A user may override this singleton by creating a derived class and making
    a static instance of it before any monitor client classes are created.
    This would typically be done in the users main program.
  */
class PInterfaceMonitor : public PObject
{
  PCLASSINFO(PInterfaceMonitor, PObject);
  public: 
    enum {
      DefaultRefreshInterval = 5000
    };

    PInterfaceMonitor(
      unsigned refreshInterval = DefaultRefreshInterval,
      BOOL runMonitorThread = TRUE
    );
    virtual ~PInterfaceMonitor();

    /// Return the singleton interface for the network monitor
    static PInterfaceMonitor & GetInstance();

    /// Start monitoring network interfaces
    BOOL Start();

    /// Stop monitoring network interfaces.
    void Stop();

    typedef PIPSocket::InterfaceEntry InterfaceEntry;

    /** Get an array of all current interface descriptors, possibly including
        the loopback (127.0.0.1) interface. Note the names are of the form
        ip%name, eg "10.0.1.11%3Com 3C90x Ethernet Adapter" or "192.168.0.10%eth0"
      */
    PStringArray GetInterfaces(
      BOOL includeLoopBack = FALSE,  /// Flag for if loopback is to included in list
      const PIPSocket::Address & destination = PIPSocket::GetDefaultIpAny()
    );

    /** Returns whether destination is reachable through binding or not.
        The default behaviour returns TRUE unless there is an interface
        filter installed an the filter does not return 'binding' among
        it's interfaces.
      */
    BOOL IsValidBindingForDestination(
      const PIPSocket::Address & binding,
      const PIPSocket::Address & destination
    );

    /** Return information about an active interface given the descriptor
       string. Note that when searchin the descriptor may be a partial match
       e.g. "10.0.1.11" or "%eth0" may be used.
      */
    BOOL GetInterfaceInfo(
      const PString & iface,  /// Interface desciptor name
      InterfaceEntry & info   /// Information on the interface
    );
    
    /** Sets the monitor's interface filter. Note that the monitor instance
        handles deletion of the filter.
      */
    void SetInterfaceFilter(PInterfaceFilter * filter);
    
    virtual void RefreshInterfaceList();

  protected:
    void UpdateThreadMain();

    void AddClient(PInterfaceMonitorClient *);
    void RemoveClient(PInterfaceMonitorClient *);

    virtual void OnAddInterface(const InterfaceEntry & entry);
    virtual void OnRemoveInterface(const InterfaceEntry & entry);


    typedef PSmartPtr<PInterfaceMonitorClient> ClientPtr;

    typedef std::list<PInterfaceMonitorClient *> ClientList_T;
    ClientList_T              currentClients;
    PIPSocket::InterfaceTable currentInterfaces;

    BOOL runMonitorThread;
    PTimeInterval  refreshInterval;
    PMutex         mutex;
    PThread      * updateThread;
    PSyncPoint     threadRunning;
    
    PInterfaceFilter * interfaceFilter;

  friend class PInterfaceMonitorClient;
};


//////////////////////////////////////////////////

/** This is a base class for clients of the PInterfaceMonitor singleton object.
    The OnAddInterface() and OnRemoveInterface() functions are called in the
    context of a thread that is monitoring interfaces. The client object is
    locked for Read/Write before these functions are called.
  */
class PInterfaceMonitorClient : public PSafeObject
{
  PCLASSINFO(PInterfaceMonitorClient, PSafeObject);
  public:
    PInterfaceMonitorClient();
    ~PInterfaceMonitorClient();

    typedef PIPSocket::InterfaceEntry InterfaceEntry;

    /** Get an array of all current interface descriptors, possibly including
        the loopback (127.0.0.1) interface. Note the names are of the form
        ip%name, eg "10.0.1.11%3Com 3C90x Ethernet Adapter" or "192.168.0.10%eth0".
        If destination is not 'any' and a filter is set, filters the interface list
        before returning it.
      */
    virtual PStringArray GetInterfaces(
      BOOL includeLoopBack = FALSE,  /// Flag for if loopback is to included in list
      const PIPSocket::Address & destination = PIPSocket::GetDefaultIpAny() /// destination
    );

    /** Return information about an active interface given the descriptor
       string. Note that when searchin the descriptor may be a partial match
       e.g. "10.0.1.11" or "%eth0" may be used.
      */
    virtual BOOL GetInterfaceInfo(
      const PString & iface,  /// Interface desciptor name
      InterfaceEntry & info   /// Information on the interface
    );

  protected:
    /// Call back function for when an interface has been added to the system
    virtual void OnAddInterface(const InterfaceEntry & entry) = 0;

    /// Call back function for when an interface has been removed from the system
    virtual void OnRemoveInterface(const InterfaceEntry & entry) = 0;

  friend class PInterfaceMonitor;
};


//////////////////////////////////////////////////

class PInterfaceFilter : public PObject {
  PCLASSINFO(PInterfaceFilter, PObject);
  
  public:
    virtual PIPSocket::InterfaceTable FilterInterfaces(const PIPSocket::Address & destination,
                                                       PIPSocket::InterfaceTable & interfaces) const = 0;
};


//////////////////////////////////////////////////

/** This is a base class for UDP socket(s) that are monitored for interface
    changes. Two derived classes are available, one that is permanently
    bound to an IP address and/or interface name. The second will dynamically
    open/close ports as interfaces are added and removed from the system.
  */
class PMonitoredSockets : public PInterfaceMonitorClient
{
  PCLASSINFO(PMonitoredSockets, PInterfaceMonitorClient);
  protected:
    PMonitoredSockets(
      BOOL reuseAddr,
      PSTUNClient * stunClient
    );

  public:
    /** Open the socket(s) using the specified port. If port is zero then a
        system allocated port is used. In this case and when multiple
        interfaces are supported, all sockets use the same dynamic port value.

        Returns TRUE if all sockets are opened.
     */
    virtual BOOL Open(
      WORD port
    ) = 0;

    /// Indicate if the socket(s) are open and ready for reads/writes.
    BOOL IsOpen() const { return opened; }

    /// Close all socket(s)
    virtual BOOL Close() = 0;

    /// Return the local port number being used by the socket(s)
    WORD GetPort() const { return localPort; }

    /// Get the local address for the given interface.
    virtual BOOL GetAddress(
      const PString & iface,        /// Interface to get address for
      PIPSocket::Address & address, /// Address of interface
      WORD & port,                  /// Port listening on
      BOOL usingNAT                 /// Require NAT address/port
    ) const = 0;

    /** Write to the remote address/port using the socket(s) available. If the
        iface parameter is empty, then the data is written to all socket(s).
        Otherwise the iface parameter indicates the specific interface socket
        to write the data to.
      */
    virtual PChannel::Errors WriteToBundle(
      const void * buffer,              /// Data to write
      PINDEX length,                    /// Length of data
      const PIPSocket::Address & addr,  /// Remote IP address to write to
      WORD port,                        /// Remote port to write to
      const PString & iface,            /// Interface to use for writing
      PINDEX & lastWriteCount
    ) = 0;

    /** Read fram a remote address/port using the socket(s) available. If the
        iface parameter is empty, then the first data received on any socket(s)
        is used, and the iface parameter is set to the name of that interface.
        Otherwise the iface parameter indicates the specific interface socket
        to read the data from.
      */
    virtual PChannel::Errors ReadFromBundle(
      void * buffer,                /// Data to read
      PINDEX length,                /// Maximum length of data
      PIPSocket::Address & addr,    /// Remote IP address data came from
      WORD & port,                  /// Remote port data came from
      PString & iface,              /// Interface to use for read, also one data was read on
      PINDEX & lastReadCount,       /// Actual length of data read
      const PTimeInterval & timeout /// Time to wait for data
    ) = 0;

    /// Set the STUN server
    void SetSTUN(
      PSTUNClient * stunClient
    ) { stun = stunClient; }

    // Get the current STUN server
    PSTUNClient * GetSTUN() const { return stun; }

    /** Create a new monitored socket instance based on the interface
        descriptor. This will create a multiple or single socket derived class
        of PMonitoredSockets depending on teh iface parameter.
      */
    static PMonitoredSockets * Create(
      const PString & iface,            /// Interface name to create socket for
      BOOL reuseAddr = FALSE,           /// Re-use or exclusive port number
      PSTUNClient * stunClient = NULL   /// STUN client code
    );

  protected:
    struct SocketInfo {
      SocketInfo()
        : socket(NULL)
        , inUse(false)
      { }
      PUDPSocket * socket;
      bool         inUse;
    };

    BOOL CreateSocket(
      SocketInfo & info,
      const PIPSocket::Address & binding
    );
    BOOL DestroySocket(SocketInfo & info);
    BOOL GetSocketAddress(
      const SocketInfo & info,
      PIPSocket::Address & address,
      WORD & port,
      BOOL usingNAT
    ) const;

    PChannel::Errors WriteToSocket(
      const void * buf,
      PINDEX len,
      const PIPSocket::Address & addr,
      WORD port,
      const SocketInfo & info,
      PINDEX & lastWriteCount
    );
    PChannel::Errors ReadFromSocket(
      SocketInfo & info,
      void * buf,
      PINDEX len,
      PIPSocket::Address & addr,
      WORD & port,
      PINDEX & lastReadCount,
      const PTimeInterval & timeout
    );

    WORD          localPort;
    BOOL          reuseAddress;
    PSTUNClient * stun;

    bool          opened;
    PUDPSocket    interfaceAddedSignal;
};

typedef PSafePtr<PMonitoredSockets> PMonitoredSocketsPtr;


//////////////////////////////////////////////////

/** This class can be used to access the bundled/monitored UDP sockets using
    the PChannel API.
  */
class PMonitoredSocketChannel : public PChannel
{
  PCLASSINFO(PMonitoredSocketChannel, PChannel);
  public:
  /**@name Construction */
  //@{
    /// Construct a monitored socket bundle channel
    PMonitoredSocketChannel(
      const PMonitoredSocketsPtr & sockets,  /// Monitored socket bundle to use in channel
      BOOL shared                            /// Monitored socket is shared by other channels
    );
  //@}

  /**@name Overrides from class PSocket */
  //@{
    virtual BOOL IsOpen() const;
    virtual BOOL Close();

    /** Override of PChannel functions to allow connectionless reads
     */
    virtual BOOL Read(
      void * buffer,
      PINDEX length
    );

    virtual BOOL Write(
    /** Override of PChannel functions to allow connectionless writes
     */
      const void * buffer,
      PINDEX length
    );
  //@}

  /**@name New functions for class */
  //@{
    /** Set the interface descriptor to be used for all reads/writes to this channel.
        The iface parameter can be a partial descriptor eg "%eth0".
      */
    void SetInterface(
      const PString & iface   /// Interface descriptor
    );

    /// Get the current interface descriptor being used/
    const PString & GetInterface();

    /** Get the local IP address and port for the currently selected interface.
      */
    BOOL GetLocal(
      PIPSocket::Address & address, /// IP address of local interface
      WORD & port,                  /// Port listening on
      BOOL usingNAT                 /// Require NAT address/port
    );

    /// Set the remote address/port for all Write() functions
    void SetRemote(
      const PIPSocket::Address & address, /// Remote IP address
      WORD port                           /// Remote port number
    );

    /// Set the remote address/port for all Write() functions
    void SetRemote(
      const PString & hostAndPort /// String of the form host[:port]
    );

    /// Get the current remote address/port for all Write() functions
    void GetRemote(
      PIPSocket::Address & addr,  /// Remote IP address
      WORD & port                 /// Remote port number
    ) const { addr = remoteAddress; port = remotePort; }

    /** Set flag for receiving UDP data from any remote address. If the flag
        is FALSE then data received from anything other than the configured
        remote address and port is ignored.
      */
    void SetPromiscuous(
      BOOL flag   /// New flag
    ) { promiscuousReads = flag; }

    /// Get flag for receiving UDP data from any remote address
    bool GetPromiscuous() { return promiscuousReads; }

    // Get the IP address and port of the last received UDP data.
    void GetLastReceived(
      PIPSocket::Address & addr,  /// Remote IP address
      WORD & port                 /// Remote port number
    ) const { addr = lastReceivedAddress; port = lastReceivedPort; }

    /// Get the monitored socket bundle being used by this channel.
    const PMonitoredSocketsPtr & GetMonitoredSockets() const { return socketBundle; }
  //@}

  protected:
    PMonitoredSocketsPtr socketBundle;
    BOOL                 sharedBundle;
    PString              currentInterface;
    BOOL                 promiscuousReads;
    PIPSocket::Address   remoteAddress;
    BOOL                 closing;
    WORD                 remotePort;
    PIPSocket::Address   lastReceivedAddress;
    WORD                 lastReceivedPort;
};


//////////////////////////////////////////////////

/** This concrete class bundles a set of UDP sockets which are dynamically
    adjusted as interfaces are added and removed from the system.
  */
class PMonitoredSocketBundle : public PMonitoredSockets
{
  PCLASSINFO(PMonitoredSocketBundle, PMonitoredSockets);
  public:
    PMonitoredSocketBundle(
      BOOL reuseAddr = FALSE,
      PSTUNClient * stunClient = NULL
    );
    ~PMonitoredSocketBundle();

    /** Open the socket(s) using the specified port. If port is zero then a
        system allocated port is used. In this case and when multiple
        interfaces are supported, all sockets use the same dynamic port value.

        Returns TRUE if all sockets are opened.
     */
    virtual BOOL Open(
      WORD port
    );

    /// Close all socket(s)
    virtual BOOL Close();

    /// Get the local address for the given interface.
    virtual BOOL GetAddress(
      const PString & iface,        /// Interface to get address for
      PIPSocket::Address & address, /// Address of interface
      WORD & port,                  /// Port listening on
      BOOL usingNAT                 /// Require NAT address/port
    ) const;

    /** Write to the remote address/port using the socket(s) available. If the
        iface parameter is empty, then the data is written to all socket(s).
        Otherwise the iface parameter indicates the specific interface socket
        to write the data to.
      */
    virtual PChannel::Errors WriteToBundle(
      const void * buf,
      PINDEX len,
      const PIPSocket::Address & addr,
      WORD port,
      const PString & iface,
      PINDEX & lastWriteCount
    );

    /** Read fram a remote address/port using the socket(s) available. If the
        iface parameter is empty, then the first data received on any socket(s)
        is used, and the iface parameter is set to the name of that interface.
        Otherwise the iface parameter indicates the specific interface socket
        to read the data from.
      */
    virtual PChannel::Errors ReadFromBundle(
      void * buf,
      PINDEX len,
      PIPSocket::Address & addr,
      WORD & port,
      PString & iface,
      PINDEX & lastReadCount,
      const PTimeInterval & timeout
    );

  protected:
    /// Call back function for when an interface has been added to the system
    virtual void OnAddInterface(const InterfaceEntry & entry);

    /// Call back function for when an interface has been removed from the system
    virtual void OnRemoveInterface(const InterfaceEntry & entry);

    typedef std::map<std::string, SocketInfo> SocketInfoMap_T;

    void OpenSocket(const PString & iface);
    void CloseSocket(const SocketInfoMap_T::iterator & iterSocket);

    SocketInfoMap_T socketInfoMap;
};


//////////////////////////////////////////////////

/** This concrete class monitors a single scoket bound to a specific interface
   or address. The interface name may be a partial descriptor such as
   "%eth0".
  */
class PSingleMonitoredSocket : public PMonitoredSocketBundle
{
  PCLASSINFO(PSingleMonitoredSocket, PMonitoredSocketBundle);
  public:
    PSingleMonitoredSocket(
      const PString & theInterface,
      BOOL reuseAddr = FALSE,
      PSTUNClient * stunClient = NULL
    );
    ~PSingleMonitoredSocket();

    /** Get an array of all current interface descriptors, possibly including
        the loopback (127.0.0.1) interface. Note the names are of the form
        ip%name, eg "10.0.1.11%3Com 3C90x Ethernet Adapter" or "192.168.0.10%eth0"
      */
    virtual PStringArray GetInterfaces(
      BOOL includeLoopBack = FALSE,  /// Flag for if loopback is to included in list
      const PIPSocket::Address & destination = PIPSocket::GetDefaultIpAny()
    );

    /** Open the socket(s) using the specified port. If port is zero then a
        system allocated port is used. In this case and when multiple
        interfaces are supported, all sockets use the same dynamic port value.

        Returns TRUE if all sockets are opened.
     */
    virtual BOOL Open(
      WORD port
    );

    /// Close all socket(s)
    virtual BOOL Close();

    /// Get the local address for the given interface.
    virtual BOOL GetAddress(
      const PString & iface,        /// Interface to get address for
      PIPSocket::Address & address, /// Address of interface
      WORD & port,                  /// Port listening on
      BOOL usingNAT                 /// Require NAT address/port
    ) const;

    /** Write to the remote address/port using the socket(s) available. If the
        iface parameter is empty, then the data is written to all socket(s).
        Otherwise the iface parameter indicates the specific interface socket
        to write the data to.
      */
    virtual PChannel::Errors WriteToBundle(
      const void * buf,
      PINDEX len,
      const PIPSocket::Address & addr,
      WORD port,
      const PString & iface,
      PINDEX & lastWriteCount
    );

    /** Read fram a remote address/port using the socket(s) available. If the
        iface parameter is empty, then the first data received on any socket(s)
        is used, and the iface parameter is set to the name of that interface.
        Otherwise the iface parameter indicates the specific interface socket
        to read the data from.
      */
    virtual PChannel::Errors ReadFromBundle(
      void * buf,
      PINDEX len,
      PIPSocket::Address & addr,
      WORD & port,
      PString & iface,
      PINDEX & lastReadCount,
      const PTimeInterval & timeout
    );


  protected:
    /// Call back function for when an interface has been added to the system
    virtual void OnAddInterface(const InterfaceEntry & entry);

    /// Call back function for when an interface has been removed from the system
    virtual void OnRemoveInterface(const InterfaceEntry & entry);

    BOOL IsInterface(const PString & iface) const;

    PString        theInterface;
    InterfaceEntry theEntry;
    SocketInfo     theInfo;
};

#endif
