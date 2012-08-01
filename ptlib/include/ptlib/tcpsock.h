/*
 * tcpsock.h
 *
 * Transmission Control Protocol socket channel class.
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
 * $Log: tcpsock.h,v $
 * Revision 1.31  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.30  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.29  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.28  2002/10/08 12:41:51  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.27  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.26  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.25  1999/09/28 00:08:59  robertj
 * Fixed some comment typoes
 *
 * Revision 1.24  1999/09/28 00:07:54  robertj
 * Fixed some comment typoes
 *
 * Revision 1.23  1999/08/30 02:21:03  robertj
 * Added ability to listen to specific interfaces for IP sockets.
 *
 * Revision 1.22  1999/03/09 02:59:51  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.21  1999/02/16 08:11:17  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.20  1998/12/22 10:23:08  robertj
 * Added clone() function to support SOCKS in FTP style protocols.
 *
 * Revision 1.19  1998/09/23 06:21:37  robertj
 * Added open source copyright license.
 *
 * Revision 1.18  1998/08/21 05:24:46  robertj
 * Fixed bug where write streams out to non-stream socket.
 *
 * Revision 1.17  1996/09/14 13:09:42  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.16  1996/03/26 00:57:15  robertj
 * Added contructor that takes PTCPSocket so avoid copy constructor being used instead of accept.
 *
 * Revision 1.15  1996/03/03 07:37:59  robertj
 * Added Reusability clause to the Listen() function on sockets.
 *
 * Revision 1.14  1996/02/25 03:01:27  robertj
 * Moved some socket functions to platform dependent code.
 *
 * Revision 1.13  1995/12/10 11:43:34  robertj
 * Numerous fixes for sockets.
 *
 * Revision 1.12  1995/06/17 11:13:31  robertj
 * Documentation update.
 *
 * Revision 1.11  1995/06/17 00:47:31  robertj
 * Changed overloaded Open() calls to 3 separate function names.
 * More logical design of port numbers and service names.
 *
 * Revision 1.10  1995/06/04 12:46:25  robertj
 * Slight redesign of port numbers on sockets.
 *
 * Revision 1.9  1995/03/14 12:42:46  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.8  1995/03/12  04:46:40  robertj
 * Added more functionality.
 *
 * Revision 1.7  1995/01/03  09:36:22  robertj
 * Documentation.
 *
 * Revision 1.6  1995/01/01  01:07:33  robertj
 * More implementation.
 *
 * Revision 1.5  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.4  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.3  1994/08/21  23:43:02  robertj
 * Changed type of socket port number for better portability.
 * Added Out of Band data functions.
 *
 * Revision 1.2  1994/07/25  03:36:03  robertj
 * Added sockets to common, normalising to same comment standard.
 *
 */

#ifndef _PTCPSOCKET
#define _PTCPSOCKET

#ifdef P_USE_PRAGMA
#pragma interface
#endif


/** A socket that uses the TCP transport on the Internet Protocol.
 */
class PTCPSocket : public PIPSocket
{
  PCLASSINFO(PTCPSocket, PIPSocket);
  public:
  /**@name Construction. */
  //@{
    /**Create a TCP/IP protocol socket channel. If a remote machine address or
       a "listening" socket is specified then the channel is also opened.

       Note that what looks like a "copy" constructor here is really a
       the accept of a "listening" socket the same as the PSocket & parameter
       version constructor.
     */
    PTCPSocket(
      WORD port = 0             ///< Port number to use for the connection.
    );
    PTCPSocket(
      const PString & service   ///< Service name to use for the connection.
    );
    PTCPSocket(
      const PString & address,  ///< Address of remote machine to connect to.
      WORD port                 ///< Port number to use for the connection.
    );
    PTCPSocket(
      const PString & address,  ///< Address of remote machine to connect to.
      const PString & service   ///< Service name to use for the connection.
    );
    PTCPSocket(
      PSocket & socket          ///< Listening socket making the connection.
    );
    PTCPSocket(
      PTCPSocket & tcpSocket    ///< Listening socket making the connection.
    );
  //@}

  /**@name Overrides from class PObject. */
  //@{
    /** Create a copy of the class on the heap. The exact semantics of the
       descendent class determine what is required to make a duplicate of the
       instance. Not all classes can even {\bf do} a clone operation.
       
       The main user of the clone function is the #PDictionary# class as
       it requires copies of the dictionary keys.

       The default behaviour is for this function to assert.

       @return
       pointer to new copy of the class instance.
     */
    virtual PObject * Clone() const;
  //@}

  /**@name Overrides from class PChannel. */
  //@{
    /** Low level write to the channel. This function will block until the
       requested number of characters are written or the write timeout is
       reached. The GetLastWriteCount() function returns the actual number
       of bytes written.

       The GetErrorCode() function should be consulted after Write() returns
       FALSE to determine what caused the failure.

       This override repeatedly writes if there is no error until all of the
       requested bytes have been written.

       @return
       TRUE if at least len bytes were written to the channel.
     */
    virtual BOOL Write(
      const void * buf, ///< Pointer to a block of memory to write.
      PINDEX len        ///< Number of bytes to write.
    );
  //@}

  /**@name Overrides from class PSocket. */
  //@{
    /** Listen on a socket for a remote host on the specified port number. This
       may be used for server based applications. A "connecting" socket begins
       a connection by initiating a connection to this socket. An active socket
       of this type is then used to generate other "accepting" sockets which
       establish a two way communications channel with the "connecting" socket.

       If the #port# parameter is zero then the port number as
       defined by the object instance construction or the
       #PIPSocket::SetPort()# function.

       @return
       TRUE if the channel was successfully opened.
     */
    virtual BOOL Listen(
      unsigned queueSize = 5,  ///< Number of pending accepts that may be queued.
      WORD port = 0,           ///< Port number to use for the connection.
      Reusability reuse = AddressIsExclusive ///< Can/Can't listen more than once.
    );
    virtual BOOL Listen(
      const Address & bind,     ///< Local interface address to bind to.
      unsigned queueSize = 5,   ///< Number of pending accepts that may be queued.
      WORD port = 0,            ///< Port number to use for the connection.
      Reusability reuse = AddressIsExclusive ///< Can/Can't listen more than once.
    );

    /** Open a socket to a remote host on the specified port number. This is an
       "accepting" socket. When a "listening" socket has a pending connection
       to make, this will accept a connection made by the "connecting" socket
       created to establish a link.

       The port that the socket uses is the one used in the #Listen()#
       command of the #socket# parameter.

       Note that this function will block until a remote system connects to the
       port number specified in the "listening" socket.

       @return
       TRUE if the channel was successfully opened.
     */
    virtual BOOL Accept(
      PSocket & socket          ///< Listening socket making the connection.
    );
  //@}

  /**@name New functions for class. */
  //@{
    /** Write out of band data from the TCP/IP stream. This data is sent as TCP
       URGENT data which does not follow the usual stream sequencing of the
       normal channel data.

       This is subject to the write timeout and sets the
       #lastWriteCount# member variable in the same way as usual
       #PChannel::Write()# function.
       
       @return
       TRUE if all the bytes were sucessfully written.
     */
    virtual BOOL WriteOutOfBand(
      const void * buf,   ///< Data to be written as URGENT TCP data.
      PINDEX len          ///< Number of bytes pointed to by #buf#.
    );

    /** This is callback function called by the system whenever out of band data
       from the TCP/IP stream is received. A descendent class may interpret
       this data according to the semantics of the high level protocol.

       The default behaviour is for the out of band data to be ignored.
     */
    virtual void OnOutOfBand(
      const void * buf,   ///< Data to be received as URGENT TCP data.
      PINDEX len          ///< Number of bytes pointed to by #buf#.
    );
  //@}


  protected:
    // Open an IPv4 socket (for backward compatibility)
    virtual BOOL OpenSocket();

    // Open an IPv4 or IPv6 socket
    virtual BOOL OpenSocket(
      int ipAdressFamily
    );

    virtual const char * GetProtocolName() const;


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/tcpsock.h"
#else
#include "unix/ptlib/tcpsock.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
