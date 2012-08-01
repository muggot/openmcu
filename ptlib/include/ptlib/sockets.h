/*
 * sockets.h
 *
 * Berkley Sockets classes.
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
 * $Log: sockets.h,v $
 * Revision 1.14  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.13  1998/11/30 02:51:59  robertj
 * New directory structure
 *
 * Revision 1.12  1998/09/23 06:19:58  robertj
 * Added open source copyright license.
 *
 * Revision 1.11  1998/08/21 05:25:21  robertj
 * Added ethernet socket in right place this time.
 *
 * Revision 1.10  1998/08/20 06:02:35  robertj
 * Added direct ethernet socket type.
 *
 * Revision 1.9  1996/10/08 13:05:38  robertj
 * More IPX support.
 *
 * Revision 1.8  1996/09/14 13:09:44  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.7  1996/08/08 10:08:53  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.6  1996/05/15 10:13:15  robertj
 * Added ICMP protocol socket, getting common ancestor to UDP.
 *
 * Revision 1.5  1995/06/04 12:36:55  robertj
 * Added application layer protocol sockets.
 *
 * Revision 1.4  1994/08/23 11:32:52  robertj
 * Oops
 *
 * Revision 1.3  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.2  1994/08/21  23:43:02  robertj
 * Added telnet.
 *
 * Revision 1.1  1994/07/25  03:36:03  robertj
 * Initial revision
 *
 * Revision 1.3  1994/07/21  12:17:41  robertj
 * Sockets.
 *
 * Revision 1.2  1994/06/25  12:27:39  robertj
 * *** empty log message ***
 *
 * Revision 1.1  1994/04/01  14:38:42  robertj
 * Initial revision
 *
 */

#ifndef _SOCKETS_H
#define _SOCKETS_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


///////////////////////////////////////////////////////////////////////////////
// PSocket

#include <ptlib/socket.h>


///////////////////////////////////////////////////////////////////////////////
// PIPSocket

#include <ptlib/ipsock.h>


///////////////////////////////////////////////////////////////////////////////
// PIPDatagramSocket

#include <ptlib/ipdsock.h>


///////////////////////////////////////////////////////////////////////////////
// PUDPSocket

#include <ptlib/udpsock.h>


///////////////////////////////////////////////////////////////////////////////
// PICMPSocket

#include <ptlib/icmpsock.h>


///////////////////////////////////////////////////////////////////////////////
// PTCPSocket

#include <ptlib/tcpsock.h>


#ifdef PIPX

///////////////////////////////////////////////////////////////////////////////
// PIPXSocket

#include <ptlib/ipxsock.h>


///////////////////////////////////////////////////////////////////////////////
// PSPXSocket

#include <ptlib/spxsock.h>

#endif // PIPX


///////////////////////////////////////////////////////////////////////////////
// PEthSocket

#include <ptlib/ethsock.h>


#endif // _SOCKETS_H


// End Of File ///////////////////////////////////////////////////////////////
