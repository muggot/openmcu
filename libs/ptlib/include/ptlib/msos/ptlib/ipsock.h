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
 * Revision 1.12  2007/10/03 01:18:44  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.11  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.10  2003/02/04 22:28:19  robertj
 * Fixed use of NETDB_SUCCESS symbol in WinCE, thanks Joerg Schoemer
 *
 * Revision 1.9  2002/12/04 00:41:12  robertj
 * Added sockets symbol not in winsock for getXbyY functions.
 *
 * Revision 1.8  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.7  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.6  2001/01/24 06:06:34  yurik
 * Windows CE port-related changes - actually my first change of openh323 code
 *
 * Revision 1.5  1998/11/30 02:55:15  robertj
 * New directory structure
 *
 * Revision 1.4  1998/09/24 03:30:09  robertj
 * Added open software license.
 *
 * Revision 1.3  1997/01/10 13:15:39  robertj
 * Added unix style error codes for WinSock codes compatible with GetErrorNumber().
 *
 * Revision 1.2  1996/08/08 10:09:04  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.1  1994/07/27 06:00:10  robertj
 * Initial revision
 *
 */

#ifdef EINPROGRESS
# undef EINPROGRESS
# undef ENOTSOCK     
# undef EMSGSIZE     
# undef EOPNOTSUPP   
# undef EAFNOSUPPORT 
# undef EADDRINUSE   
# undef EADDRNOTAVAIL
# undef ENETDOWN     
# undef ENETUNREACH  
# undef ENETRESET    
# undef ECONNABORTED 
# undef ECONNRESET   
# undef ENOBUFS      
# undef EISCONN      
# undef ENOTCONN     
# undef ETIMEDOUT    
# undef ECONNREFUSED 
# undef EHOSTUNREACH 
#endif

#define EINPROGRESS             (WSAEINPROGRESS|PWIN32ErrorFlag)
#define ENOTSOCK                (WSAENOTSOCK|PWIN32ErrorFlag)
#define EMSGSIZE                (WSAEMSGSIZE|PWIN32ErrorFlag)
#define ESOCKTNOSUPPORT         (WSAESOCKTNOSUPPORT|PWIN32ErrorFlag)
#define EOPNOTSUPP              (WSAEOPNOTSUPP|PWIN32ErrorFlag)
#define EPFNOSUPPORT            (WSAEPFNOSUPPORT|PWIN32ErrorFlag)
#define EAFNOSUPPORT            (WSAEAFNOSUPPORT|PWIN32ErrorFlag)
#define EADDRINUSE              (WSAEADDRINUSE|PWIN32ErrorFlag)
#define EADDRNOTAVAIL           (WSAEADDRNOTAVAIL|PWIN32ErrorFlag)
#define ENETDOWN                (WSAENETDOWN|PWIN32ErrorFlag)
#define ENETUNREACH             (WSAENETUNREACH|PWIN32ErrorFlag)
#define ENETRESET               (WSAENETRESET|PWIN32ErrorFlag)
#define ECONNABORTED            (WSAECONNABORTED|PWIN32ErrorFlag)
#define ECONNRESET              (WSAECONNRESET|PWIN32ErrorFlag)
#define ENOBUFS                 (WSAENOBUFS|PWIN32ErrorFlag)
#define EISCONN                 (WSAEISCONN|PWIN32ErrorFlag)
#define ENOTCONN                (WSAENOTCONN|PWIN32ErrorFlag)
#define ESHUTDOWN               (WSAESHUTDOWN|PWIN32ErrorFlag)
#define ETOOMANYREFS            (WSAETOOMANYREFS|PWIN32ErrorFlag)
#define ETIMEDOUT               (WSAETIMEDOUT|PWIN32ErrorFlag)
#define ECONNREFUSED            (WSAECONNREFUSED|PWIN32ErrorFlag)
#define EHOSTDOWN               (WSAEHOSTDOWN|PWIN32ErrorFlag)
#define EHOSTUNREACH            (WSAEHOSTUNREACH|PWIN32ErrorFlag)


#define NETDB_SUCCESS 0


///////////////////////////////////////////////////////////////////////////////
// PIPSocket

// nothing to do


// End Of File ///////////////////////////////////////////////////////////////
