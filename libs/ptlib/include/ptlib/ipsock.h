/*
 * ipsock.h
 *
 * Internet Protocol socket I/O channel class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: ipsock.h,v $
 * Revision 1.90  2007/08/22 05:00:02  rjongbloed
 * Added function to return local and peer address as string in "addr:port" format.
 *
 * Revision 1.89  2007/06/27 03:15:21  rjongbloed
 * Added ability to select filtering of down network interfaces.
 *
 * Revision 1.88  2007/05/21 06:09:25  csoutheren
 * Add paramaterless constructor for PIPSocket::InterfaceEntry
 *
 * Revision 1.87  2007/02/19 04:37:19  csoutheren
 * Fixed parsing in PIPSocketAddressAndPort
 *
 * Revision 1.86  2007/02/01 03:18:42  csoutheren
 * Add extra features to PIPSocketAddressAndPort
 *
 * Revision 1.85  2007/01/31 00:44:34  csoutheren
 * Fix warning on Linux
 *
 * Revision 1.84  2007/01/30 02:26:21  csoutheren
 * Fix minor problem with PIPSocketAddressAndPort
 *
 * Revision 1.83  2006/10/03 06:29:38  csoutheren
 * Add PIPSocketAndPort::AsString
 *
 * Revision 1.82  2006/07/05 03:58:09  csoutheren
 * Additional implementation of PIPSocketAddressAndPort
 *
 * Revision 1.81  2006/04/09 07:05:40  rjongbloed
 * Moved output stream operator for PString from sockets code to string code and fixed
 *   its implemetation to continue to use PrintOn. Why it was added is unknown, probably
 *   a compiler issue, but it should not be in a random source file!
 *
 * Revision 1.80  2006/02/26 11:51:20  csoutheren
 * Extended DNS test program to include URL based SRV lookups
 * Re-arranged SRV lookup code to allow access to internal routine
 * Reformatted code
 *
 * Revision 1.79  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.78  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.77  2005/07/13 11:48:41  csoutheren
 * Backported QOS changes from isvo branch
 *
 * Revision 1.76.2.1  2005/04/25 13:30:41  shorne
 * Extra support for DHCP Environment (Win32)
 *
 * Revision 1.76  2005/03/22 07:29:29  csoutheren
 * Fixed problem where PStrings sometimes get case into
 * PIPSocket::Address when outputting to an ostream
 *
 * Revision 1.75  2005/02/13 23:01:35  csoutheren
 * Fixed problem with not detecting mapped IPV6 addresses within the RFC1918
 * address range as RFC1918
 *
 * Revision 1.74  2005/02/07 12:12:33  csoutheren
 * Expanded interface list routines to include IPV6 addresses
 * Added IPV6 to GetLocalAddress
 *
 * Revision 1.73  2005/02/07 00:47:17  csoutheren
 * Changed IPV6 code to use standard IPV6 macros
 *
 * Revision 1.72  2005/02/04 05:50:27  csoutheren
 * Extended IsRFC1918 to handle IPV6
 *
 * Revision 1.71  2005/01/16 21:27:01  csoutheren
 * Changed PIPSocket::IsAny to be const
 *
 * Revision 1.70  2004/12/20 07:59:33  csoutheren
 * Fixed operator *= for IPV6
 *
 * Revision 1.69  2004/12/14 14:24:19  csoutheren
 * Added PIPSocket::Address::operator*= to compare IPV4 addresses
 * to IPV4-compatible IPV6 addresses. More documentation needed
 * once this is tested as working
 *
 * Revision 1.68  2004/12/14 06:20:29  csoutheren
 * Added function to get address of network interface
 *
 * Revision 1.67  2004/08/24 07:08:13  csoutheren
 * Added use of recvmsg to determine which interface UDP packets arrive on
 *
 * Revision 1.66  2004/07/11 07:56:35  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.65  2004/04/18 04:33:36  rjongbloed
 * Changed all operators that return BOOL to return standard type bool. This is primarily
 *   for improved compatibility with std STL usage removing many warnings.
 *
 * Revision 1.64  2004/04/07 05:29:50  csoutheren
 * Added function to detect RFC 1918 addresses
 *
 * Revision 1.63  2004/02/23 17:27:19  ykiryanov
 * Added == and != operators for in_addr_t on BeOS as suggested by Craig Southeren to please compiler
 *
 * Revision 1.62  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.61  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.60  2003/05/21 09:34:43  rjongbloed
 * Name lookup support for IPv6, thanks again Sébastien Josset
 *
 * Revision 1.59  2003/04/28 02:55:08  robertj
 * Added function to see at run time if IPv6 available, thanks Sebastien Josset
 *
 * Revision 1.58  2003/04/03 08:43:23  robertj
 * Added IPv4 mapping into IPv6, thanks Sebastien Josset
 *
 * Revision 1.57  2003/03/26 05:36:37  robertj
 * More IPv6 support (INADDR_ANY handling), thanks Sébastien Josset
 *
 * Revision 1.56  2003/02/03 11:23:32  robertj
 * Added function to get pointer to IP address data.
 *
 * Revision 1.55  2003/02/03 08:51:44  robertj
 * Fixed compatibility with old code so taking address of PIPSocket::Address
 *   gets address of 4 or 16 byte IP address.
 *
 * Revision 1.54  2002/12/02 03:57:18  robertj
 * More RTEMS support patches, thank you Vladimir Nesic.
 *
 * Revision 1.53  2002/11/02 00:32:21  robertj
 * Further fixes to VxWorks (Tornado) port, thanks Andreas Sikkema.
 *
 * Revision 1.52  2002/10/29 07:59:45  robertj
 * Changed in_addr6 to more universally used in6_addr.
 *
 * Revision 1.51  2002/10/08 14:31:43  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.50  2002/10/08 12:41:51  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.49  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.48  2001/12/13 09:17:01  robertj
 * Added function to convert PString to IP address with error checking that can
 *   distinguish between 0.0.0.0 or 255.255.255.255 and illegal address.
 *
 * Revision 1.47  2001/09/14 08:00:38  robertj
 * Added new versions of Conenct() to allow binding to a specific local interface.
 *
 * Revision 1.46  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.45  2001/03/05 04:18:27  robertj
 * Added net mask to interface info returned by GetInterfaceTable()
 *
 * Revision 1.44  2001/01/29 06:41:18  robertj
 * Added printing of entry of interface table.
 *
 * Revision 1.43  2000/06/26 11:17:19  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.42  1999/09/10 04:35:42  robertj
 * Added Windows version of PIPSocket::GetInterfaceTable() function.
 *
 * Revision 1.41  1999/09/10 02:31:42  craigs
 * Added interface table routines
 *
 * Revision 1.40  1999/08/30 02:21:03  robertj
 * Added ability to listen to specific interfaces for IP sockets.
 *
 * Revision 1.39  1999/08/08 09:04:01  robertj
 * Added operator>> for PIPSocket::Address class.
 *
 * Revision 1.38  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.37  1999/02/23 07:19:22  robertj
 * Added [] operator PIPSocket::Address to get the bytes out of an IP address.
 *
 * Revision 1.36  1999/02/16 08:12:00  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.35  1998/12/21 07:22:50  robertj
 * Virtualised functions for SOCKS support.
 *
 * Revision 1.34  1998/12/18 04:34:14  robertj
 * PPC Linux GNU C compatibility.
 *
 * Revision 1.33  1998/11/30 08:57:32  robertj
 * New directory structure
 *
 * Revision 1.32  1998/11/22 11:30:08  robertj
 * Check route table function to get a list
 *
 * Revision 1.31  1998/11/19 05:18:22  robertj
 * Added route table manipulation functions to PIPSocket class.
 *
 * Revision 1.30  1998/09/23 06:20:45  robertj
 * Added open source copyright license.
 *
 * Revision 1.29  1997/12/11 10:28:57  robertj
 * Added operators for IP address to DWORD conversions.
 *
 * Revision 1.28  1996/12/17 11:08:05  robertj
 * Added DNS name cache clear command.
 *
 * Revision 1.27  1996/11/30 12:10:00  robertj
 * Added Connect() variant so can set the local port number on link.
 *
 * Revision 1.26  1996/11/16 10:48:49  robertj
 * Fixed missing const in PIPSocket::Address stream output operator..
 *
 * Revision 1.25  1996/11/04 03:40:54  robertj
 * Moved address printer from inline to source.
 *
 * Revision 1.24  1996/09/14 13:09:21  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.23  1996/08/25 09:33:55  robertj
 * Added function to detect "local" host name.
 *
 * Revision 1.22  1996/03/26 00:51:13  robertj
 * Added GetLocalAddress() variant that returns port number as well.
 *
 * Revision 1.21  1996/03/16 04:41:30  robertj
 * Changed all the get host name and get host address functions to be more consistent.
 *
 * Revision 1.20  1996/03/03 07:37:56  robertj
 * Added Reusability clause to the Listen() function on sockets.
 *
 * Revision 1.19  1996/02/25 03:00:31  robertj
 * Added operator<< to PIPSocket::Address.
 * Moved some socket functions to platform dependent code.
 *
 * Revision 1.18  1996/02/08 12:11:19  robertj
 * Added GetPeerAddress that returns a port.
 *
 * Revision 1.17  1996/01/28 14:07:31  robertj
 * Changed service parameter to PString for ease of use in GetPortByService function
 * Fixed up comments.
 *
 * Revision 1.16  1995/12/23 03:44:59  robertj
 * Fixed unix portability issues.
 *
 * Revision 1.15  1995/12/10 11:32:11  robertj
 * Numerous fixes for sockets.
 *
 * Revision 1.14  1995/10/14 14:57:26  robertj
 * Added internet address to string conversion functionality.
 *
 * Revision 1.13  1995/07/02 01:18:19  robertj
 * Added static functions to get the current host name/address.
 *
 * Revision 1.12  1995/06/17 00:41:40  robertj
 * More logical design of port numbers and service names.
 *
 * Revision 1.11  1995/03/18 06:26:44  robertj
 * Changed IP address variable for GNU compatibility.
 *
 * Revision 1.10  1995/03/14  12:41:38  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.9  1995/03/12  04:38:41  robertj
 * Added more functionality.
 *
 * Revision 1.8  1995/01/02  12:28:24  robertj
 * Documentation.
 * Added more socket functions.
 *
 * Revision 1.7  1995/01/01  01:07:33  robertj
 * More implementation.
 *
 * Revision 1.6  1994/12/15  12:47:14  robertj
 * Documentation.
 *
 * Revision 1.5  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.4  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.3  1994/08/21  23:43:02  robertj
 * Spelt Berkeley correctly.
 *
 * Revision 1.2  1994/07/25  03:36:03  robertj
 * Added sockets to common, normalising to same comment standard.
 *
 */

#ifndef _PIPSOCKET
#define _PIPSOCKET

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/socket.h>

#ifdef P_HAS_QOS
#ifdef _WIN32
#ifdef P_KNOCKOUT_WINSOCK2 
   #include "IPExport.h"
#endif // KNOCKOUT_WINSOCK2
#endif // _WIN32
#endif // P_HAS_QOS

/** This class describes a type of socket that will communicate using the
   Internet Protocol.
   If P_HAS_IPV6 is not set, IPv4 only is supported.
   If P_HAS_IPV6 is set, both IPv4 and IPv6 adresses are supported, with 
   IPv4 as default. This allows to transparently use IPv4, IPv6 or Dual 
   stack operating systems.
 */
class PIPSocket : public PSocket
{
  PCLASSINFO(PIPSocket, PSocket);
  protected:
    /* Create a new Internet Protocol socket based on the port number
       specified.
     */
    PIPSocket();

  public:
    /**
      A class describing an IP address
     */
    class Address : public PObject {
      public:

        /**@name Address constructors */
        //@{
        /// Create an IPv4 address with the default address: 127.0.0.1 (loopback)
        Address();

        /** Create an IP address from string notation.
            eg dot notation x.x.x.x. for IPv4, or colon notation x:x:x::xxx for IPv6
          */
        Address(const PString & dotNotation);

        /// Create an IPv4 or IPv6 address from 4 or 16 byte values
        Address(PINDEX len, const BYTE * bytes);

        /// Create an IP address from four byte values
        Address(BYTE b1, BYTE b2, BYTE b3, BYTE b4);

        /// Create an IPv4 address from a four byte value in network byte order
        Address(DWORD dw);

        /// Create an IPv4 address from an in_addr structure
        Address(const in_addr & addr);

#if P_HAS_IPV6
        /// Create an IPv6 address from an in_addr structure
        Address(const in6_addr & addr);

        /// Create an IP (v4 or v6) address from a sockaddr (sockaddr_in,
        /// sockaddr_in6 or sockaddr_in6_old) structure
        Address(const int ai_family, const int ai_addrlen,struct sockaddr *ai_addr);
#endif

#ifdef __NUCLEUS_NET__
        Address(const struct id_struct & addr);
        Address & operator=(const struct id_struct & addr);
#endif

        /// Copy an address from another IP v4 address
        Address & operator=(const in_addr & addr);

#if P_HAS_IPV6
        /// Copy an address from another IPv6 address
        Address & operator=(const in6_addr & addr);
#endif

        /// Copy an address from a string
        Address & operator=(const PString & dotNotation);

        /// Copy an address from a four byte value in network order
        Address & operator=(DWORD dw);
        //@}

        /// Compare two adresses for absolute (in)equality
        Comparison Compare(const PObject & obj) const;
        bool operator==(const Address & addr) const { return Compare(addr) == EqualTo; }
        bool operator!=(const Address & addr) const { return Compare(addr) != EqualTo; }
#if P_HAS_IPV6
        bool operator==(in6_addr & addr) const;
        bool operator!=(in6_addr & addr) const { return !operator==(addr); }
#endif
        bool operator==(in_addr & addr) const;
        bool operator!=(in_addr & addr) const { return !operator==(addr); }
        bool operator==(DWORD dw) const;
        bool operator!=(DWORD dw) const   { return !operator==(dw); }
#ifdef P_VXWORKS 
        bool operator==(long unsigned int u) const { return  operator==((DWORD)u); }
        bool operator!=(long unsigned int u) const { return !operator==((DWORD)u); }
#endif
#ifdef _WIN32
        bool operator==(unsigned u) const { return  operator==((DWORD)u); }
        bool operator!=(unsigned u) const { return !operator==((DWORD)u); }
#endif
#ifdef P_RTEMS
        bool operator==(u_long u) const { return  operator==((DWORD)u); }
        bool operator!=(u_long u) const { return !operator==((DWORD)u); }
#endif
#ifdef __BEOS__
        bool operator==(in_addr_t a) const { return  operator==((DWORD)a); }
        bool operator!=(in_addr_t a) const { return !operator==((DWORD)a); }
#endif
        bool operator==(int i) const      { return  operator==((DWORD)i); }
        bool operator!=(int i) const      { return !operator==((DWORD)i); }

        /// Compare two addresses for equivalence. This will return TRUE
        /// if the two addresses are equivalent even if they are IPV6 and IPV4
#if P_HAS_IPV6
        bool operator*=(const Address & addr) const;
#else
        bool operator*=(const Address & addr) const { return operator==(addr); }
#endif

        /// Format an address as a string
        PString AsString() const;

        /// Convert string to IP address. Returns TRUE if was a valid address.
        BOOL FromString(
          const PString & str
        );

        /// Format an address as a string
        operator PString() const;

        /// Return IPv4 address in network order
        operator in_addr() const;

#if P_HAS_IPV6
        /// Return IPv4 address in network order
        operator in6_addr() const;
#endif

        /// Return IPv4 address in network order
        operator DWORD() const;

        /// Return first byte of IPv4 address
        BYTE Byte1() const;

        /// Return second byte of IPv4 address
        BYTE Byte2() const;

        /// Return third byte of IPv4 address
        BYTE Byte3() const;

        /// Return fourth byte of IPv4 address
        BYTE Byte4() const;

        /// return specified byte of IPv4 or IPv6 address
        BYTE operator[](PINDEX idx) const;

        /// Get the address length (will be either 4 or 16)
        PINDEX GetSize() const;

        /// Get the pointer to IP address data
        const char * GetPointer() const { return (const char *)&v; }

        /// Get the version of the IP address being used
        unsigned GetVersion() const { return version; }

        /// Check address 0.0.0.0 or :: 
        BOOL IsValid() const;
        BOOL IsAny() const;

        /// Check address 127.0.0.1 or ::1
        BOOL IsLoopback() const;

        /// Check for Broadcast address 255.255.255.255
        BOOL IsBroadcast() const;

        // Check if the remote address is a private address.
        // For IPV4 this is specified RFC 1918 as the following ranges:
        //    10.0.0.0    - 10.255.255.255.255
        //    172.16.0.0  - 172.31.255.255
        //    192.168.0.0 - 192.168.255.255
        // For IPV6 this is specified as any address having "1111 1110 1” for the first nine bits
        BOOL IsRFC1918() const ;

#if P_HAS_IPV6
        /// Check for v4 mapped i nv6 address ::ffff:a.b.c.d
        BOOL IsV4Mapped() const;
#endif
        
        static const Address & GetLoopback();
#if P_HAS_IPV6
        static const Address & GetLoopback6();
        static const Address & GetAny6();
#endif
        static const Address & GetBroadcast();

      protected:
        /// Runtime test of IP addresse type
        union {
          in_addr four;
#if P_HAS_IPV6
          in6_addr six;
#endif
        } v;
        unsigned version;

      /// output IPv6 & IPv4 address as a string to the specified string
      friend ostream & operator<<(ostream & s, const Address & a);

      /// input IPv4 (not IPv6 yet!) address as a string from the specified string
      friend istream & operator>>(istream & s, Address & a);
    };

  // Overrides from class PChannel
    /** Get the platform and I/O channel type name of the channel. For an IP
       socket this returns the host name of the peer the socket is connected
       to, followed by the socket number it is connected to.

       @return
       the name of the channel.
     */
    virtual PString GetName() const;

    // Set the default IP address familly.
    // Needed as lot of IPv6 stack are not able to receive IPv4 packets in IPv6 sockets
    // They are not RFC 2553, chapter 7.3, compliant.
    // As a concequence, when opening a socket to listen to port 1720 (for exemple) from any remot host
    // one must decide whether this an IPv4 or an IPv6 socket...
    static int GetDefaultIpAddressFamily();
    static void SetDefaultIpAddressFamily(int ipAdressFamily); // PF_INET, PF_INET6
    static void SetDefaultIpAddressFamilyV4(); // PF_INET
#if P_HAS_IPV6
    static void SetDefaultIpAddressFamilyV6(); // PF_INET6
    static BOOL IsIpAddressFamilyV6Supported();
#endif
    static PIPSocket::Address GetDefaultIpAny();

    // Open an IPv4 or IPv6 socket
    virtual BOOL OpenSocket(
      int ipAdressFamily=PF_INET
    ) = 0;


  // Overrides from class PSocket.
    /** Connect a socket to a remote host on the specified port number. This is
       typically used by the client or initiator of a communications channel.
       This connects to a "listening" socket at the other end of the
       communications channel.

       The port number as defined by the object instance construction or the
       #PIPSocket::SetPort()# function.

       @return
       TRUE if the channel was successfully connected to the remote host.
     */
    virtual BOOL Connect(
      const PString & address   ///< Address of remote machine to connect to.
    );
    virtual BOOL Connect(
      const Address & addr      ///< Address of remote machine to connect to.
    );
    virtual BOOL Connect(
      WORD localPort,           ///< Local port number for connection
      const Address & addr      ///< Address of remote machine to connect to.
    );
    virtual BOOL Connect(
      const Address & iface,    ///< Address of local interface to us.
      const Address & addr      ///< Address of remote machine to connect to.
    );
    virtual BOOL Connect(
      const Address & iface,    ///< Address of local interface to us.
      WORD localPort,           ///< Local port number for connection
      const Address & addr      ///< Address of remote machine to connect to.
    );

    /** Listen on a socket for a remote host on the specified port number. This
       may be used for server based applications. A "connecting" socket begins
       a connection by initiating a connection to this socket. An active socket
       of this type is then used to generate other "accepting" sockets which
       establish a two way communications channel with the "connecting" socket.

       If the #port# parameter is zero then the port number as
       defined by the object instance construction or the
       #PIPSocket::SetPort()# function.

       For the UDP protocol, the #queueSize# parameter is ignored.

       @return
       TRUE if the channel was successfully opened.
     */
    virtual BOOL Listen(
      unsigned queueSize = 5,  ///< Number of pending accepts that may be queued.
      WORD port = 0,           ///< Port number to use for the connection.
      Reusability reuse = AddressIsExclusive ///< Can/Cant listen more than once.
    );
    virtual BOOL Listen(
      const Address & bind,     ///< Local interface address to bind to.
      unsigned queueSize = 5,   ///< Number of pending accepts that may be queued.
      WORD port = 0,            ///< Port number to use for the connection.
      Reusability reuse = AddressIsExclusive ///< Can/Can't listen more than once.
    );


  // New functions for class
    /** Get the "official" host name for the host specified or if none, the host
       this process is running on. The host may be specified as an IP number
       or a hostname alias and is resolved to the canonical form.

       @return
       Name of the host or IP number of host.
     */
    static PString GetHostName();
    static PString GetHostName(
      const PString & hostname  ///< Hosts IP address to get name for
    );
    static PString GetHostName(
      const Address & addr    ///< Hosts IP address to get name for
    );

    /** Get the Internet Protocol address for the specified host, or if none
       specified, for the host this process is running on.

       @return
       TRUE if the IP number was returned.
     */
    static BOOL GetHostAddress(
      Address & addr    ///< Variable to receive hosts IP address
    );
    static BOOL GetHostAddress(
      const PString & hostname,
      /* Name of host to get address for. This may be either a domain name or
         an IP number in "dot" format.
       */
      Address & addr    ///< Variable to receive hosts IP address
    );

    /** Get the alias host names for the specified host. This includes all DNS
       names, CNAMEs, names in the local hosts file and IP numbers (as "dot"
       format strings) for the host.

       @return
       array of strings for each alias for the host.
     */
    static PStringArray GetHostAliases(
      const PString & hostname
      /* Name of host to get address for. This may be either a domain name or
         an IP number in "dot" format.
       */
    );
    static PStringArray GetHostAliases(
      const Address & addr    ///< Hosts IP address
      /* Name of host to get address for. This may be either a domain name or
         an IP number in "dot" format.
       */
    );

    /** Determine if the specified host is actually the local machine. This
       can be any of the host aliases or multi-homed IP numbers or even
       the special number 127.0.0.1 for the loopback device.

       @return
       TRUE if the host is the local machine.
     */
    static BOOL IsLocalHost(
      const PString & hostname
      /* Name of host to get address for. This may be either a domain name or
         an IP number in "dot" format.
       */
    );

    /** Get the Internet Protocol address and port for the local host.

       @return
       FALSE (or empty string) if the IP number was not available.
     */
    virtual PString GetLocalAddress();
    virtual BOOL GetLocalAddress(
      Address & addr    ///< Variable to receive hosts IP address
    );
    virtual BOOL GetLocalAddress(
      Address & addr,    ///< Variable to receive peer hosts IP address
      WORD & port        ///< Variable to receive peer hosts port number
    );

    /** Get the Internet Protocol address for the peer host and port the
       socket is connected to.

       @return
       FALSE (or empty string) if the IP number was not available.
     */
    virtual PString GetPeerAddress();
    virtual BOOL GetPeerAddress(
      Address & addr    ///< Variable to receive hosts IP address
    );
    virtual BOOL GetPeerAddress(
      Address & addr,    ///< Variable to receive peer hosts IP address
      WORD & port        ///< Variable to receive peer hosts port number
    );

    /** Get the host name for the local host.

       @return
       Name of the host, or an empty string if an error occurs.
     */
    PString GetLocalHostName();

    /** Get the host name for the peer host the socket is connected to.

       @return
       Name of the host, or an empty string if an error occurs.
     */
    PString GetPeerHostName();

    /** Clear the name (DNS) cache.
     */
    static void ClearNameCache();

    /** Get the IP address that is being used as the gateway, that is, the
       computer that packets on the default route will be sent.

       The string returned may be used in the Connect() function to open that
       interface.

       Note that the driver does not need to be open for this function to work.

       @return
       TRUE if there was a gateway.
     */
    static BOOL GetGatewayAddress(
      Address & addr     ///< Variable to receive the IP address.
    );

    /** Get the name for the interface that is being used as the gateway,
       that is, the interface that packets on the default route will be sent.

       The string returned may be used in the Connect() function to open that
       interface.

       Note that the driver does not need to be open for this function to work.

       @return

       String name of the gateway device, or empty string if there is none.
     */
    static PString GetGatewayInterface();

 #ifdef _WIN32
    /** Get the IP address for the interface that is being used as the gateway,
       that is, the interface that packets on the default route will be sent.

       This Function can be used to Bind the Listener to only the default Packet
       route in DHCP Environs.

       Note that the driver does not need to be open for this function to work.

       @return
       The Local Interface IP Address for Gatway Access
     */
    static PIPSocket::Address GetGatewayInterfaceAddress();

    /** Retrieve the Local IP Address for which packets would have be routed to the to reach the remote Address.
       @return Local Address
    */
    static PIPSocket::Address GetRouteAddress(PIPSocket::Address RemoteAddress);

    /** IP Address to a Numerical Representation
    */
    static unsigned AsNumeric(Address addr);

    /** Check if packets on Interface Address can reach the remote IP Address.
     */
    static BOOL IsAddressReachable(PIPSocket::Address LocalIP,
                                   PIPSocket::Address LocalMask, 
                                   PIPSocket::Address RemoteIP);

    /** Get the Interface Name for a given local Interface Address
     */
    static PString GetInterface(PIPSocket::Address addr);
 #endif
    /**
       Describes a route table entry
    */
    class RouteEntry : public PObject
    {
      PCLASSINFO(RouteEntry, PObject);
      public:
        /// create a route table entry from an IP address
        RouteEntry(const Address & addr) : network(addr) { }

        /// Get the network address associated with the route table entry
        Address GetNetwork() const { return network; }

        /// Get the network address mask associated with the route table entry
        Address GetNetMask() const { return net_mask; }

        /// Get the default gateway address associated with the route table entry
        Address GetDestination() const { return destination; }

        /// Get the network address name associated with the route table entry
        const PString & GetInterface() const { return interfaceName; }

        /// Get the network metric associated with the route table entry
        long GetMetric() const { return metric; }

      protected:
        Address network;
        Address net_mask;
        Address destination;
        PString interfaceName;
        long    metric;

      friend class PIPSocket;
    };

    PLIST(RouteTable, RouteEntry);

    /** Get the systems route table.

       @return
       TRUE if the route table is returned, FALSE if an error occurs.
     */
    static BOOL GetRouteTable(
      RouteTable & table      ///< Route table
    );


    /**
      Describes an interface table entry
     */
    class InterfaceEntry : public PObject
    {
      PCLASSINFO(InterfaceEntry, PObject)

      public:
        /// create an interface entry from a name, IP addr and MAC addr
        InterfaceEntry();
        InterfaceEntry(
          const PString & _name,
          const Address & _addr,
          const Address & _mask,
          const PString & _macAddr
#if P_HAS_IPV6
          , const PString & _ip6Addr = PString::Empty()
#endif
        );

        /// Print to specified stream
        virtual void PrintOn(
          ostream &strm   // Stream to print the object into.
        ) const;

        /// Get the name of the interface
        const PString & GetName() const { return name; }

        /// Get the address associated with the interface
        Address GetAddress() const { return ipAddr; }

        BOOL HasIP6Address() const
#if ! P_HAS_IPV6
        { return FALSE;}
#else
        { return !ip6Addr.IsEmpty();}

        /// Get the address associated with the interface
        Address GetIP6Address() const { return ip6Addr; }
#endif

        /// Get the net mask associated with the interface
        Address GetNetMask() const { return netMask; }

        /// Get the MAC address associate with the interface
        const PString & GetMACAddress() const { return macAddr; }

      protected:
        PString name;
        Address ipAddr;
        Address netMask;
        PString macAddr;
#if P_HAS_IPV6
        PString ip6Addr;
#endif
    };

    PLIST(InterfaceTable, InterfaceEntry);

    /** Get a list of all interfaces
       @return
       TRUE if the interface table is returned, FALSE if an error occurs.
     */
    static BOOL GetInterfaceTable(
      InterfaceTable & table,      ///< interface table
      BOOL includeDown = FALSE     ///< Include interfaces that are down
    );

    /** Get the address of an interface that corresponds to a real network
       @return
       FALSE if only loopback interfaces could be found, else TRUE
     */
    static BOOL GetNetworkInterface(PIPSocket::Address & addr);

#if P_HAS_RECVMSG

    /**
      * Set flag to capture destination address for incoming packets
      *
      * @return TRUE if host is able to capture incoming address, else FALSE
      */
    BOOL SetCaptureReceiveToAddress()
    { if (!SetOption(IP_PKTINFO, 1, SOL_IP)) return FALSE; catchReceiveToAddr = TRUE; return TRUE; }

    /**
      * return the interface address of the last incoming packet
      */
    PIPSocket::Address GetLastReceiveToAddress() const
    { return lastReceiveToAddr; }

  protected:
    void SetLastReceiveAddr(void * addr, int addrLen)
    { if (addrLen == sizeof(in_addr)) lastReceiveToAddr = *(in_addr *)addr; }

    PIPSocket::Address lastReceiveToAddr;

#else

    /**
      * Set flag to capture interface address for incoming packets
      *
      * @return TRUE if host is able to capture incoming address, else FALSE
      */
    BOOL SetCaptureReceiveToAddress()
    { return FALSE; }

    /**
      * return the interface address of the last incoming packet
      */
    PIPSocket::Address GetLastReceiveToAddress() const
    { return PIPSocket::Address(); }

#endif

// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/ipsock.h"
#else
#include "unix/ptlib/ipsock.h"
#endif
};

class PIPSocketAddressAndPort
{
  public:
    PIPSocketAddressAndPort()
      : port(0), sep(':')
    { }

    PIPSocketAddressAndPort(char _sep)
      : port(0), sep(_sep)
    { }

    PIPSocketAddressAndPort(const PString & str, WORD defaultPort = 0, char _sep = ':')
      : sep(_sep)
    { Parse(str, defaultPort, sep); }

    BOOL Parse(const PString & str, WORD defaultPort = 0, char sep = ':');

    PString AsString(char _sep = 0) const
    { return address.AsString() + (_sep ? _sep : sep) + PString(PString::Unsigned, port); }

    PIPSocket::Address address;
    WORD port;
    char sep;
};

typedef std::vector<PIPSocketAddressAndPort> PIPSocketAddressAndPortVector;

#endif


// End Of File ///////////////////////////////////////////////////////////////
