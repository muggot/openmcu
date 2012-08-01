/*
 * ipdsock.h
 *
 * IP Datagram socket I/O channel class.
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
 * $Log: ipdsock.h,v $
 * Revision 1.11  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.10  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.9  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.8  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.7  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.6  1999/03/09 02:59:49  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.5  1999/02/16 08:12:00  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.4  1998/11/14 06:28:09  robertj
 * Fixed error in documentation
 *
 * Revision 1.3  1998/09/23 06:20:43  robertj
 * Added open source copyright license.
 *
 * Revision 1.2  1996/09/14 13:09:20  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.1  1996/05/15 21:11:16  robertj
 * Initial revision
 *
 */

#ifndef _PIPDATAGRAMSOCKET
#define _PIPDATAGRAMSOCKET

#ifdef P_USE_PRAGMA
#pragma interface
#endif

/** Internet Protocol Datagram Socket class.
*/
class PIPDatagramSocket : public PIPSocket
{
  PCLASSINFO(PIPDatagramSocket, PIPSocket);
  protected:
    /**Create a TCP/IP protocol socket channel. If a remote machine address or
       a "listening" socket is specified then the channel is also opened.
     */
    PIPDatagramSocket();


  public:
  // New functions for class
    /**Read a datagram from a remote computer.
       
       @return TRUE if any bytes were sucessfully read.
     */
    virtual BOOL ReadFrom(
      void * buf,     ///< Data to be written as URGENT TCP data.
      PINDEX len,     ///< Number of bytes pointed to by #buf#.
      Address & addr, ///< Address from which the datagram was received.
      WORD & port     ///< Port from which the datagram was received.
    );

    /**Write a datagram to a remote computer.

       @return TRUE if all the bytes were sucessfully written.
     */
    virtual BOOL WriteTo(
      const void * buf,   ///< Data to be written as URGENT TCP data.
      PINDEX len,         ///< Number of bytes pointed to by #buf#.
      const Address & addr, ///< Address to which the datagram is sent.
      WORD port           ///< Port to which the datagram is sent.
    );


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/ipdsock.h"
#else
#include "unix/ptlib/ipdsock.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
