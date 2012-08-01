/*
 * spxsock.h
 *
 * SPX socket channel class
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
 * $Log: spxsock.h,v $
 * Revision 1.10  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.9  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.8  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.7  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.6  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.5  1999/03/09 02:59:51  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.4  1999/02/16 08:11:17  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.3  1998/11/30 02:51:59  robertj
 * New directory structure
 *
 * Revision 1.2  1998/09/23 06:21:29  robertj
 * Added open source copyright license.
 *
 * Revision 1.1  1996/09/14 13:00:56  robertj
 * Initial revision
 *
 */

#ifndef _PSPXSOCKET
#define _PSPXSOCKET

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/ipxsock.h>


/** Create a socket channel that uses the SPX transport over the IPX
   Protocol.
 */
class PSPXSocket : public PIPXSocket
{
  PCLASSINFO(PSPXSocket, PIPXSocket);

  public:
  /**@name Construction. */
  //@{
    /** Create an SPX protocol socket channel. If a remote machine address or
       a "listening" socket is specified then the channel is also opened.

       Note that the "copy" constructor here is really a "listening" socket
       the same as the PSocket & parameter version.
     */
    PSPXSocket(
      WORD port = 0             ///< Port number to use for the connection.
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
      Reusability reuse = AddressIsExclusive ///< Can/Cant listen more than once.
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

  protected:
    virtual BOOL OpenSocket();
    virtual const char * GetProtocolName() const;


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/spxsock.h"
#else
#include "unix/ptlib/spxsock.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
