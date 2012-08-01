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
 * Revision 1.1  2000/06/26 11:17:19  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.7  1999/02/22 13:26:53  robertj
 * BeOS port changes.
 *
 * Revision 1.6  1998/11/30 22:06:49  robertj
 * New directory structure.
 *
 * Revision 1.5  1998/09/24 04:11:40  robertj
 * Added open software license.
 *
 * Revision 1.4  1997/10/03 14:47:07  craigs
 * Fixed ifdef guard
 *
 * Revision 1.3  1996/08/03 12:08:19  craigs
 * Changed for new common directories
 *
 * Revision 1.2  1996/04/15 10:50:48  craigs
 * Last revision prior to release of MibMaster
 *
 * Revision 1.1  1994/04/12  08:31:05  robertj
 * Initial revision
 *
 */

#ifndef _PIPSOCKET

#ifdef __GNUC__
#pragma interface
#endif

#if !defined(__BEOS__) && !defined(__NUCLEUS_PLUS__)
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

// Crib from msos
#if defined(ENETUNREACH) || defined(EHOSTUNREACH) || defined(WSAENETUNREACH) || defined (WSAENETEUNREACH)
#pragma message ("ENET errors actually defined!!!")
#else
// I'm sure this is dodgy, but I'm just copying 'doze for the moment.

#define WSABASEERR              10000

#define WSAENETUNREACH          (WSABASEERR+51)
#define WSAEHOSTUNREACH         (WSABASEERR+65)

#define ENETUNREACH             (WSAENETUNREACH|0x40000000)
#define EHOSTUNREACH            (WSAEHOSTUNREACH|0x40000000)
#endif



///////////////////////////////////////////////////////////////////////////////
// PIPSocket

#include "../../ipsock.h"
};


ostream & operator << (ostream & strm, const PIPSocket::Address & addr);

#endif
