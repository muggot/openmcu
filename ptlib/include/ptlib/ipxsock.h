/*
 * ipxsock.h
 *
 * IPX protocol socket I/O channel class.
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
 * $Log: ipxsock.h,v $
 * Revision 1.12  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.11  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.10  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.9  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.8  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.7  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.6  1999/02/16 08:12:00  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.5  1998/11/30 02:50:58  robertj
 * New directory structure
 *
 * Revision 1.4  1998/09/23 06:20:47  robertj
 * Added open source copyright license.
 *
 * Revision 1.3  1996/10/08 13:21:04  robertj
 * More IPX implementation.
 *
 * Revision 1.1  1996/09/14 13:00:56  robertj
 * Initial revision
 *
 */

#ifndef _PIPXSOCKET
#define _PIPXSOCKET

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/socket.h>


/**This class describes a type of socket that will communicate using the
   IPX/SPX protocols.
 */
class PIPXSocket : public PSocket
{
  PCLASSINFO(PIPXSocket, PSocket);

  public:
    /**Create a new IPX datagram socket.
     */
    PIPXSocket(
      WORD port = 0       ///< Port number to use for the connection.
    );


  public:
    /** IPX protocol address specification.
     */
    class Address {
      public:
        union {
          struct {
            BYTE b1,b2,b3,b4;
          } b;
          struct {
            WORD w1,s_w2;
          } w;
          DWORD dw;
        } network;
        BYTE node[6];

        /** Create new, invalid, address. */
        Address();
        /** Create copy of existing address */
        Address(const Address & addr /** Address to copy */);
        /** Create address from string representation. */
        Address(const PString & str /** String representation of address */);
        /** Create address from node and net numbers. */
        Address(
          DWORD netNum, ///< IPX network number.
          const char * nodeNum  ///< IPX node number (MAC address)
        );
        /** Create copy of existing address */
        Address & operator=(const Address & addr /** Address to copy */);
        /** Get string representation of IPX address */
        operator PString() const;
        /** Determine if address is valid. Note that this does not mean that
            the host is online.
            @return TRUE is address is valid.
          */
        BOOL IsValid() const;
      /** Output string representation of IPX address to stream. */
      friend ostream & operator<<(
        ostream & strm, ///< Stream to output to
        Address & addr  ///< Address to output
      ) { return strm << (PString)addr; }
    };

  /**@name Overrides from class PChannel */
  //@{
    /**Get the platform and I/O channel type name of the channel. For an
       IPX/SPX socket this returns the network number, node number of the
       peer the socket is connected to, followed by the socket number it
       is connected to.

       @return
       the name of the channel.
     */
    virtual PString GetName() const;
  //@}


  /**@name Overrides from class PSocket */
  //@{
    /**Connect a socket to a remote host on the port number of the socket.
       This is
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
    /**Connect a socket to a remote host on the port number of the socket.
       This is
       typically used by the client or initiator of a communications channel.
       This connects to a "listening" socket at the other end of the
       communications channel.

       The port number as defined by the object instance construction or the
       #PIPSocket::SetPort()# function.

       @return
       TRUE if the channel was successfully connected to the remote host.
     */
    virtual BOOL Connect(
      const Address & address   ///< Address of remote machine to connect to.
    );

    /**Listen on a socket for a remote host on the specified port number. This
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
  //@}

  /**@name Address and name space look up functions */
  //@{
    /**Get the host name for the host specified server.

       @return
       Name of the host or IPX number of host.
     */
    static PString GetHostName(
      const Address & addr    ///< Hosts IP address to get name for
    );

    /**Get the IPX address for the specified host.

       @return
       TRUE if the IPX number was returned.
     */
    static BOOL GetHostAddress(
      Address & addr    ///< Variable to receive this hosts IP address
    );

    /**Get the IPX address for the specified host.

       @return
       TRUE if the IPX number was returned.
     */
    static BOOL GetHostAddress(
      const PString & hostname,
      /** Name of host to get address for. This may be either a server name or
         an IPX number in "colon" format.
       */
      Address & addr    ///< Variable to receive hosts IPX address
    );

    /**Get the IPX/SPX address for the local host.

       @return
       TRUE if the IPX number was returned.
     */
    BOOL GetLocalAddress(
      Address & addr    ///< Variable to receive hosts IPX address
    );

    /**Get the IPX/SPX address for the local host.

       @return
       TRUE if the IPX number was returned.
     */
    BOOL GetLocalAddress(
      Address & addr,    ///< Variable to receive peer hosts IPX address
      WORD & port        ///< Variable to receive peer hosts port number
    );

    /**Get the IPX/SPX address for the peer host the socket is
       connected to.

       @return
       TRUE if the IPX number was returned.
     */
    BOOL GetPeerAddress(
      Address & addr    ///< Variable to receive hosts IPX address
    );

    /**Get the IPX/SPX address for the peer host the socket is
       connected to.

       @return
       TRUE if the IPX number was returned.
     */
    BOOL GetPeerAddress(
      Address & addr,    ///< Variable to receive peer hosts IPX address
      WORD & port        ///< Variable to receive peer hosts port number
    );
  //@}

  /**@name I/O functions */
  //@{
    /**Sets the packet type for datagrams sent by this socket.

       @return
       TRUE if the type was successfully set.
     */
    BOOL SetPacketType(
      int type    ///< IPX packet type for this socket.
    );

    /**Gets the packet type for datagrams sent by this socket.

       @return
       type of packets or -1 if error.
     */
    int GetPacketType();


    /**Read a datagram from a remote computer.
       
       @return
       TRUE if all the bytes were sucessfully written.
     */
    virtual BOOL ReadFrom(
      void * buf,     ///< Data to be written as URGENT TCP data.
      PINDEX len,     ///< Number of bytes pointed to by #buf#.
      Address & addr, ///< Address from which the datagram was received.
      WORD & port     ///< Port from which the datagram was received.
    );

    /**Write a datagram to a remote computer.

       @return
       TRUE if all the bytes were sucessfully written.
     */
    virtual BOOL WriteTo(
      const void * buf,   ///< Data to be written as URGENT TCP data.
      PINDEX len,         ///< Number of bytes pointed to by #buf#.
      const Address & addr, ///< Address to which the datagram is sent.
      WORD port           ///< Port to which the datagram is sent.
    );
  //@}


  protected:
    virtual BOOL OpenSocket();
    virtual const char * GetProtocolName() const;


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/ipxsock.h"
#else
#include "unix/ptlib/ipxsock.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
