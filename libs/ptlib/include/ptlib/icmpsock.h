/*
 * icmpsock.h
 *
 * Internet Control Message Protocol socket I/O channel class.
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
 * $Log: icmpsock.h,v $
 * Revision 1.17  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.16  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.15  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.14  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.13  2003/02/11 06:47:19  craigs
 * Added missing OpenSocket function
 *
 * Revision 1.12  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.11  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.10  1999/08/07 15:22:20  craigs
 * Changed Success to PingSuccess to avoid namespace collision with X define of the same name
 *
 * Revision 1.9  1999/03/09 02:59:49  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.8  1999/02/16 08:20:48  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.7  1998/09/23 06:20:39  robertj
 * Added open source copyright license.
 *
 * Revision 1.6  1998/01/26 00:30:41  robertj
 * Added error codes, TTL and data buffer to Ping.
 *
 * Revision 1.5  1997/02/05 11:52:07  robertj
 * Changed current process function to return reference and validate objects descendancy.
 *
 * Revision 1.4  1996/11/04 03:57:16  robertj
 * Rewrite of ping for Win32 support.
 *
 * Revision 1.3  1996/09/14 13:09:19  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.2  1996/06/03 10:03:22  robertj
 * Changed ping to return more parameters.
 *
 * Revision 1.1  1996/05/15 21:11:16  robertj
 * Initial revision
 *
 */

#ifndef _PICMPSOCKET
#define _PICMPSOCKET

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/pprocess.h>

/**Create a socket channel that uses allows ICMP commands in the Internal
   Protocol.
 */
class PICMPSocket : public PIPDatagramSocket
{
  PCLASSINFO(PICMPSocket, PIPDatagramSocket);

  public:
  /**@name Construction */
  //@{
    /**Create a TCP/IP protocol socket channel. If a remote machine address or
       a "listening" socket is specified then the channel is also opened.
     */
    PICMPSocket();
  //@}

  /**@name Status & Information */
  //@{
    /// Results of ICMP operation.
    enum PingStatus {
      PingSuccess,         // don't use Success - X11 defines this!
      NetworkUnreachable,
      HostUnreachable,
      PacketTooBig,
      RequestTimedOut,
      BadRoute,
      TtlExpiredTransmit,
      TtlExpiredReassembly,
      SourceQuench,
      MtuChange,
      GeneralError,
      NumStatuses
    };

    /// Information used by and obtained by the ping operation.
    class PingInfo {
      public:
        /// Create Ping information structure.
        PingInfo(WORD id = (WORD)PProcess::Current().GetProcessID());

        /**@name Supplied data */
        //@{
        /// Arbitrary identifier for the ping.
        WORD identifier;         
        /// Sequence number for ping packet.
        WORD sequenceNum;        
        /// Time To Live for packet.
        BYTE ttl;                
        /// Send buffer (if NULL, defaults to 32 bytes).
        const BYTE * buffer;     
        /// Size of buffer (< 64k).
        PINDEX bufferSize;       
        //@}

        /**@name Returned data */
        //@{
        /// Time for packet to make trip.
        PTimeInterval delay;     
        /// Source address of reply packet.
        Address remoteAddr;      
        /// Destination address of reply packet.
        Address localAddr;       
        /// Status of the last ping operation
        PingStatus status;       
        //@}
    };
  //@}

  /**@name Ping */
  //@{
    /**Send an ECHO_REPLY message to the specified host and wait for a reply
       to be sent back.

       @return
       FALSE if host not found or no response.
     */
    BOOL Ping(
      const PString & host   ///< Host to send ping.
    );
    /**Send an ECHO_REPLY message to the specified host and wait for a reply
       to be sent back.

       @return
       FALSE if host not found or no response.
     */
    BOOL Ping(
      const PString & host,   ///< Host to send ping.
      PingInfo & info         ///< Information on the ping and reply.
    );
  //@}

  protected:
    const char * GetProtocolName() const;
    virtual BOOL OpenSocket();
    virtual BOOL OpenSocket(int ipAdressFamily);


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/icmpsock.h"
#else
#include "unix/ptlib/icmpsock.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
