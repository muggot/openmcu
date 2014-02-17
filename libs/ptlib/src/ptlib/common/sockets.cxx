/*
 * sockets.cxx
 *
 * Berkley sockets classes.
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
 * $Log: sockets.cxx,v $
 * Revision 1.218  2007/10/03 01:18:46  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.217  2007/08/22 05:00:02  rjongbloed
 * Added function to return local and peer address as string in "addr:port" format.
 *
 * Revision 1.216  2007/07/22 03:17:53  rjongbloed
 * Added check for legal hostname characters as system function does not
 *   always return "no host" when some illegal characters are used.
 *
 * Revision 1.215  2007/06/29 02:47:28  rjongbloed
 * Added PString::FindSpan() function (strspn equivalent) with slightly nicer semantics.
 *
 * Revision 1.214  2007/06/25 03:46:16  rjongbloed
 * Added ability to specify an IP address as an interface using the %ifname form, anywhere
 *   that the "dotted decimal" could be used before. Useful in "demand dial" evironments
 *   or short lease DHCP where and interfaces IP address may change frequently.
 *
 * Revision 1.213  2007/05/21 06:10:58  csoutheren
 * Add paramaterless constructor for PIPSocket::InterfaceEntry
 *
 * Revision 1.212  2007/04/20 07:39:52  csoutheren
 * Applied 1703666 - PIPSocket::Connect change to avoid DNS access
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.211  2007/04/04 01:51:38  rjongbloed
 * Reviewed and adjusted PTRACE log levels
 *   Now follows 1=error,2=warn,3=info,4+=debug
 *
 * Revision 1.210  2007/04/03 16:04:02  csoutheren
 * Fixed problem with IPV6 detection causing error on Windows
 *
 * Revision 1.209  2007/02/19 04:36:35  csoutheren
 * Fixed parsing in PIPSocketAddressAndPort
 *
 * Revision 1.208  2007/01/30 02:26:22  csoutheren
 * Fix minor problem with PIPSocketAddressAndPort
 *
 * Revision 1.207  2007/01/03 22:28:48  dsandras
 * Fixed possible race condition in PHostByName and PHostByAddr. Thanks Robert!
 * Hopefully fixing Ekiga report #364480.
 *
 * Revision 1.206  2006/10/05 05:43:32  csoutheren
 * Fix uninitialised variable
 *
 * Revision 1.205  2006/07/05 03:58:09  csoutheren
 * Additional implementation of PIPSocketAddressAndPort
 *
 * Revision 1.204  2006/06/27 12:03:29  csoutheren
 * Applied 1494931 - fixed memory leak
 * Thanks to Frederich Heem
 *
 * Revision 1.203  2006/04/09 11:03:59  csoutheren
 * Remove warnings on VS.net 2005
 *
 * Revision 1.202  2006/04/09 07:05:40  rjongbloed
 * Moved output stream operator for PString from sockets code to string code and fixed
 *   its implemetation to continue to use PrintOn. Why it was added is unknown, probably
 *   a compiler issue, but it should not be in a random source file!
 *
 * Revision 1.201  2006/02/13 06:56:26  csoutheren
 * Fixed problem on Windows
 *
 * Revision 1.200  2006/02/10 23:56:58  csoutheren
 * Fixed compile problems on Debian and Windows
 *
 * Revision 1.199  2006/02/10 22:50:11  csoutheren
 * Fixed error in last commit
 *
 * Revision 1.198  2006/02/10 22:47:01  csoutheren
 * Ensure IPV6 addresses are not returned by IPSocket::GetHostAddress when
 * IPV4 has been forced using PIPSocket::SetDefaultIpAddressFamilyV4
 * This is intended to address the following Ekiga bug:
 * Bug 330388 . Cannot make calls to host with IPv6 address
 *
 * Revision 1.197  2005/11/21 11:49:36  shorne
 * Changed disableQos to disableGQoS to better reflect what it does
 *
 * Revision 1.196  2005/10/21 06:01:30  csoutheren
 * Fixed warning on VS.NET 2005
 *
 * Revision 1.195  2005/09/25 10:51:23  dominance
 * almost complete the mingw support. We'll be there soon. ;)
 *
 * Revision 1.194  2005/08/18 06:24:30  shorne
 * Reversed Last Patch
 *
 * Revision 1.193  2005/08/18 03:42:53  shorne
 * Chaeck for RSVP QoS Thx Zdenek
 *
 * Revision 1.192  2005/08/09 12:46:00  rjongbloed
 * Fixed some platforms where in6addr_any does not exist.
 *
 * Revision 1.191  2005/07/17 09:25:40  csoutheren
 * Fixed problem in IPV6 variant of PIPSocket::Address::IsLoopback
 * Thanks to Roger Hardiman
 *
 * Revision 1.190  2005/07/13 12:08:09  csoutheren
 * Fixed QoS patches to be more consistent with PWLib style and to allow Unix compatibility
 *
 * Revision 1.189  2005/07/13 11:48:54  csoutheren
 * Backported QOS changes from isvo branch
 *
 * Revision 1.188  2005/06/21 22:28:32  rjongbloed
 * Assured IP is set to zero, so if parse of dotted decimal fails is not random IP address.
 *
 * Revision 1.187.2.1  2005/04/25 13:42:28  shorne
 * Extended QoS support for per-call negotiation
 *
 * Revision 1.187  2005/03/22 07:29:30  csoutheren
 * Fixed problem where PStrings sometimes get case into
 * PIPSocket::Address when outputting to an ostream
 *
 * Revision 1.186  2005/02/13 23:01:36  csoutheren
 * Fixed problem with not detecting mapped IPV6 addresses within the RFC1918
 * address range as RFC1918
 *
 * Revision 1.185  2005/02/07 12:12:30  csoutheren
 * Expanded interface list routines to include IPV6 addresses
 * Added IPV6 to GetLocalAddress
 *
 * Revision 1.184  2005/02/07 00:47:18  csoutheren
 * Changed IPV6 code to use standard IPV6 macros
 *
 * Revision 1.183  2005/01/26 05:38:01  csoutheren
 * Added ability to remove config file support
 *
 * Revision 1.182  2005/01/16 21:27:07  csoutheren
 * Changed PIPSocket::IsAny to be const
 *
 * Revision 1.181  2005/01/16 20:35:41  csoutheren
 * Fixed problem with IPv6 INADDR_ANY
 *
 * Revision 1.180  2005/01/15 19:23:39  csoutheren
 * Fixed problem in operator *= for IP V6
 *
 * Revision 1.179  2004/12/14 14:24:20  csoutheren
 * Added PIPSocket::Address::operator*= to compare IPV4 addresses
 * to IPV4-compatible IPV6 addresses. More documentation needed
 * once this is tested as working
 *
 * Revision 1.178  2004/12/14 06:20:29  csoutheren
 * Added function to get address of network interface
 *
 * Revision 1.177  2004/11/16 00:31:44  csoutheren
 * Added Cygwin support (needs to have gethostbyname_r fixed)
 *
 * Revision 1.176  2004/10/26 18:27:28  ykiryanov
 * Added another for of SetOption to set SO_REUSEADDR for BeOS
 *
 * Revision 1.175  2004/08/24 07:08:11  csoutheren
 * Added use of recvmsg to determine which interface UDP packets arrive on
 *
 * Revision 1.174  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.173  2004/04/27 04:37:51  rjongbloed
 * Fixed ability to break of a PSocket::Select call under linux when a socket
 *   is closed by another thread.
 *
 * Revision 1.172  2004/04/22 12:27:24  rjongbloed
 * Fixed selection of QoS to use more flexible #if rather than #ifdef
 *
 * Revision 1.171  2004/04/18 04:33:38  rjongbloed
 * Changed all operators that return BOOL to return standard type bool. This is primarily
 *   for improved compatibility with std STL usage removing many warnings.
 *
 * Revision 1.170  2004/04/18 00:50:14  ykiryanov
 * Removed BE_BONELESS ifdefs. Be is boned now. More funcitonality
 *
 * Revision 1.169  2004/04/03 08:22:22  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.168  2004/02/04 02:32:31  csoutheren
 * Changed #ifdef to #if to ensure flags are tested correctly
 *
 * Revision 1.167  2004/01/23 07:07:35  csoutheren
 * Fixed compile warning under Linux
 *
 * Revision 1.166  2003/11/12 05:16:48  csoutheren
 * Fixed compiling problem on systems without QoS or IPV6
 *
 * Revision 1.165  2003/10/30 11:32:57  rjongbloed
 * Added bullet proofing for converting from inaddr
 *
 * Revision 1.164  2003/10/27 09:48:47  csoutheren
 * Changed use of P_HAS_QOS to ensure that setsockopt is still used
 * for diffserv if available. Thanks to Henry Harrison
 *
 * Revision 1.163  2003/10/27 03:46:15  csoutheren
 * Added ifdef to disable QoS code on systems that do not support it
 *
 * Revision 1.162  2003/10/27 03:22:44  csoutheren
 * Added handling for QoS
 *   Thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.161  2003/05/27 08:53:11  dsandras
 *
 * Added test error case when the host lookup fails for IPv6.
 *
 * Revision 1.160  2003/05/21 09:34:44  rjongbloed
 * Name lookup support for IPv6, thanks again Sébastien Josset
 *
 * Revision 1.159  2003/04/28 02:55:50  robertj
 * Added function to see at run time if IPv6 available, thanks Sebastien Josset
 *
 * Revision 1.158  2003/04/15 07:40:08  robertj
 * Removed redundent variable.
 * Fixed IPv6 support for multiple IP address DNS lookups.
 *
 * Revision 1.157  2003/04/08 01:12:35  robertj
 * Latest patch for IPv6 operation, thanks Sebastien Josset
 *
 * Revision 1.156  2003/04/07 23:31:33  robertj
 * Fixed incorrect host to network byte order function, should be long!
 *
 * Revision 1.155  2003/04/07 23:22:08  robertj
 * Fixed GNU compatibility issue.
 *
 * Revision 1.154  2003/04/07 11:57:56  robertj
 * Allowed for full integer numeric form of IP address read from a stream.
 *
 * Revision 1.153  2003/04/03 08:43:23  robertj
 * Added IPv4 mapping into IPv6, thanks Sebastien Josset
 *
 * Revision 1.152  2003/03/26 05:36:38  robertj
 * More IPv6 support (INADDR_ANY handling), thanks Sébastien Josset
 *
 * Revision 1.151  2003/02/11 06:49:12  craigs
 * Added missing OpenSocket function
 *
 * Revision 1.150  2003/02/03 11:23:32  robertj
 * Added function to get pointer to IP address data.
 *
 * Revision 1.149  2003/02/03 10:27:55  robertj
 * Cleaned up the gethostbyX functions for various platforms
 *
 * Revision 1.148  2003/02/03 08:43:01  robertj
 * Fixed correct scoping of local variables in gethostbyX functions.
 *
 * Revision 1.147  2003/01/14 04:36:08  robertj
 * Fixed v6 conversion of numeric string to binary so does not internally
 *   doa DNS lookup, confuses other parts of the system.
 *
 * Revision 1.146  2003/01/11 05:10:51  robertj
 * Fixed Win CE compatibility issues, thanks Joerg Schoemer
 *
 * Revision 1.145  2002/12/16 08:04:46  robertj
 * Fixed correct error check for gethostbyname_r, thanks Vladimir Toncar
 *
 * Revision 1.144  2002/12/13 04:01:46  robertj
 * Initialised error code in gethostbyname usage.
 *
 * Revision 1.143  2002/12/04 00:52:59  robertj
 * Fixed GNU warning
 *
 * Revision 1.142  2002/12/02 12:25:08  craigs
 * Fixed problem with error code from gethostbyname_r not being checked correctly
 *
 * Revision 1.141  2002/11/24 23:47:01  robertj
 * Fixed MSVC v5 compatibility
 *
 * Revision 1.140  2002/11/13 02:15:25  craigs
 * Fixed problem with GetLocalHostName
 *
 * Revision 1.139  2002/11/12 11:47:53  rogerh
 * Add a missing memset in the Psockaddr constructor
 *
 * Revision 1.138  2002/11/02 00:32:21  robertj
 * Further fixes to VxWorks (Tornado) port, thanks Andreas Sikkema.
 *
 * Revision 1.137  2002/11/01 23:56:11  robertj
 * Fixed GNu compatibility isse with IPv6
 *
 * Revision 1.136  2002/11/01 03:32:08  robertj
 * More IPv6 fixes, thanks Sébastien Josset.
 *
 * Revision 1.135  2002/10/31 07:55:33  robertj
 * Put sizeof ipv6 structure back as magic number 28 is explained by
 *   mismatched header file and running implementation.
 *
 * Revision 1.134  2002/10/29 08:04:44  robertj
 * Changed in_addr6 to more universally used in6_addr.
 * Changed size of ipv6 address to be 28 under win32, Why? I don't know!
 *
 * Revision 1.133  2002/10/19 06:12:20  robertj
 * Moved P_fd_set::Zero() from platform independent to platform dependent
 *   code as Win32 implementation is completely different from Unix.
 *
 * Revision 1.132  2002/10/18 08:07:41  robertj
 * Fixed use of FD_ZERO as (strangely) crashes on some paltforms and would
 *   not have cleared enough of an enlarges fd_set anyway.
 *
 * Revision 1.131  2002/10/17 13:44:27  robertj
 * Port to RTEMS, thanks Vladimir Nesic.
 *
 * Revision 1.130  2002/10/17 08:17:28  robertj
 * Fixed incomplete changes for expandable fd_set
 *
 * Revision 1.129  2002/10/17 07:17:43  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.128  2002/10/17 01:24:11  robertj
 * Fixed so internal sockaddr classes GetSize() returns correct size for
 *   particular sockaddr it represents, thanks Sébastien Josset.
 *
 * Revision 1.127  2002/10/16 06:19:36  robertj
 * Rewrite of IPv6 sockaddr code to use intelligent class to automatically
 *   know if it is sockaddr_in or aockaddr_in6.
 * Fixed Connect() function to work correctly on unopened socket.
 *
 * Revision 1.126  2002/10/10 11:38:56  robertj
 * Added close of socket if not open in correct ip version, thanks Sébastien Josset
 *
 * Revision 1.125  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.124  2002/10/09 05:37:52  robertj
 * Fixed IPv6 version of ReadFrom() and WriteTo().
 *
 * Revision 1.123  2002/10/08 23:31:44  robertj
 * Added missing GetSize() implementation in ip address.
 *
 * Revision 1.122  2002/10/08 12:41:52  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.121  2002/09/23 07:17:24  robertj
 * Changes to allow winsock2 to be included.
 *
 * Revision 1.120  2002/05/22 07:18:46  robertj
 * Fixed bug where SO_RESUSEADDR wsa being turned ON instead of OFF when
 *   making an outgoing connection, should only be ON for listener sockets.
 *
 * Revision 1.119  2002/04/12 01:42:41  robertj
 * Changed return value on os_connect() and os_accept() to make sure
 *   get the correct error codes propagated up under unix.
 *
 * Revision 1.118  2002/01/28 01:27:03  robertj
 * Removed previous change that actually has nothing to do with GCC 3 compatibility,
 *   setting default timeout for all sockets to 10 seconds is NOT a sensible thing to do!
 *
 * Revision 1.117  2002/01/26 23:57:45  craigs
 * Changed for GCC 3.0 compatibility, thanks to manty@manty.net
 *
 * Revision 1.116  2002/01/07 05:37:32  robertj
 * Changed to allow for a service name that starts with a number.
 *
 * Revision 1.115  2002/01/02 04:55:31  craigs
 * Fixed problem when PSocket::GetPortByService called with a number
 * that is a substring of a valid service name
 *
 * Revision 1.114  2001/12/13 09:18:07  robertj
 * Added function to convert PString to IP address with error checking that can
 *   distinguish between 0.0.0.0 or 255.255.255.255 and illegal address.
 * Added ability to decode bracketed IP addresss [10.1.2.3] as host name.
 *
 * Revision 1.113  2001/09/14 08:00:38  robertj
 * Added new versions of Conenct() to allow binding to a specific local interface.
 *
 * Revision 1.112  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.111  2001/06/30 06:59:07  yurik
 * Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
 *
 * Revision 1.110  2001/05/24 02:07:31  yurik
 * ::setsockopt on WinCE is now not called if option is not supported
 *
 * Revision 1.109  2001/05/23 19:48:55  yurik
 * Fix submitted by Dave Cassel, dcassel@cyberfone.com,
 * allowing a connection between a client and a gatekeeper.
 *
 * Revision 1.108  2001/03/20 06:44:25  robertj
 * Lots of changes to fix the problems with terminating threads that are I/O
 *   blocked, especially when doing orderly shutdown of service via SIGTERM.
 *
 * Revision 1.107  2001/03/05 04:18:27  robertj
 * Added net mask to interface info returned by GetInterfaceTable()
 *
 * Revision 1.106  2001/01/29 06:41:32  robertj
 * Added printing of entry of interface table.
 *
 * Revision 1.105  2001/01/28 01:15:01  yurik
 * WinCE port-related
 *
 * Revision 1.104  2001/01/24 06:32:17  yurik
 * Windows CE port-related changes
 *
 * Revision 1.103  2000/06/26 11:17:21  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.102  2000/06/21 01:01:22  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.101  2000/05/02 08:14:40  craigs
 * Fixed problem with "memory leak" reporting under Unix
 *
 * Revision 1.100  2000/04/27 02:43:45  robertj
 * Fixed warning about signedness mismatch.
 *
 * Revision 1.99  2000/04/19 00:13:52  robertj
 * BeOS port changes.
 *
 * Revision 1.98  2000/02/18 09:55:21  robertj
 * Added parameter so get/setsockopt can have other levels to SOL_SOCKET.
 *
 * Revision 1.97  1999/10/27 01:21:44  robertj
 * Improved portability of copy from host_info struct to IP address.
 *
 * Revision 1.96  1999/08/30 02:21:03  robertj
 * Added ability to listen to specific interfaces for IP sockets.
 *
 * Revision 1.95  1999/08/27 08:18:52  robertj
 * Added ability to get the host/port of the the last packet read/written to UDP socket.
 *
 * Revision 1.94  1999/08/08 09:04:01  robertj
 * Added operator>> for PIPSocket::Address class.
 *
 * Revision 1.93  1999/07/11 13:42:13  craigs
 * pthreads support for Linux
 *
 * Revision 1.92  1999/06/01 08:04:35  robertj
 * Fixed mistake from previous fix.
 *
 * Revision 1.91  1999/06/01 07:39:23  robertj
 * Added retries to DNS lookup if get temporary error.
 *
 * Revision 1.90  1999/03/09 08:13:52  robertj
 * Fixed race condition in doing Select() on closed sockets. Could go into infinite wait.
 *
 * Revision 1.89  1999/03/02 05:41:58  robertj
 * More BeOS changes
 *
 * Revision 1.88  1999/02/26 04:10:39  robertj
 * More BeOS port changes
 *
 * Revision 1.87  1999/02/25 03:43:35  robertj
 * Fixed warning when PINDEX is unsigned.
 *
 * Revision 1.86  1999/02/23 07:19:22  robertj
 * Added [] operator PIPSocket::Address to get the bytes out of an IP address.
 *
 * Revision 1.85  1999/02/16 08:08:06  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.84  1999/01/08 01:29:47  robertj
 * Support for pthreads under FreeBSD
 *
 * Revision 1.83  1999/01/06 10:58:01  robertj
 * Fixed subtle mutex bug in returning string hostname from DNS cache.
 *
 * Revision 1.82  1998/12/22 10:25:01  robertj
 * Added clone() function to support SOCKS in FTP style protocols.
 * Fixed internal use of new operator in IP cache.
 *
 * Revision 1.81  1998/12/18 04:34:37  robertj
 * PPC Linux GNU C compatibility.
 *
 * Revision 1.80  1998/11/30 04:47:52  robertj
 * New directory structure
 *
 * Revision 1.79  1998/11/14 06:28:36  robertj
 * Changed senatics of os_sendto to return TRUE if ANY bytes are sent.
 *
 * Revision 1.78  1998/11/08 12:05:04  robertj
 * Fixed multiple thread access problem with DNS aliases array.
 *
 * Revision 1.77  1998/10/01 09:05:35  robertj
 * Added check that port number is between 1 and 65535.
 *
 * Revision 1.76  1998/09/23 06:22:44  robertj
 * Added open source copyright license.
 *
 * Revision 1.75  1998/08/31 13:00:34  robertj
 * Prevented dependency on snmpapi.dll for all ptlib apps.
 *
 * Revision 1.74  1998/08/27 00:58:42  robertj
 * Resolved signedness problems with various GNU libraries.
 *
 * Revision 1.73  1998/08/25 14:07:43  robertj
 * Added getprotobyxxx wrapper functions.
 *
 * Revision 1.72  1998/08/25 11:09:20  robertj
 * Fixed parsing of 802.x header on ethernet frames.
 * Changed DNS cache to not cache temporary lookup failures, only an authoratative 'no such host'.
 *
 * Revision 1.71  1998/08/21 05:26:10  robertj
 * Fixed bug where write streams out to non-stream socket.
 * Added ethernet socket.
 *
 * Revision 1.70  1998/05/07 05:20:25  robertj
 * Fixed DNS lookup so only works around bug in old Win95 and not OSR2
 *
 * Revision 1.69  1998/03/20 03:18:21  robertj
 * Added special classes for specific sepahores, PMutex and PSyncPoint.
 *
 * Revision 1.68  1998/03/05 12:45:48  robertj
 * DNS cache and NT bug fix attempts.
 *
 * Revision 1.67  1998/01/26 02:49:22  robertj
 * GNU support.
 *
 * Revision 1.66  1998/01/26 00:49:28  robertj
 * Fixed bug in detecting local host on NT, 95 bug kludge was interfering with it.
 *
 * Revision 1.65  1998/01/06 12:43:23  craigs
 * Added definition of REENTRANT_BUFFER_LEN
 *
 * Revision 1.64  1998/01/04 07:25:09  robertj
 * Added pthreads compatible calls for gethostbyx functions.
 *
 * Revision 1.63  1997/12/18 05:06:13  robertj
 * Moved IsLocalHost() to platform dependent code.
 *
 * Revision 1.62  1997/12/11 10:30:35  robertj
 * Added operators for IP address to DWORD conversions.
 *
 * Revision 1.61  1997/10/03 13:33:22  robertj
 * Added workaround for NT winsock bug with RAS and DNS lookups.
 *
 * Revision 1.60  1997/09/27 00:58:39  robertj
 * Fixed race condition on socket close in Select() function.
 *
 * Revision 1.59  1997/06/06 10:56:36  craigs
 * Added new functions for connectionless UDP writes
 *
 * Revision 1.58  1997/01/04 07:42:18  robertj
 * Fixed GCC Warnings.
 *
 * Revision 1.57  1997/01/04 06:54:38  robertj
 * Added missing canonical name to alias list.
 *
 * Revision 1.56  1996/12/17 11:07:05  robertj
 * Added clear of name cache.
 *
 * Revision 1.55  1996/12/12 09:23:27  robertj
 * Fixed name cache to cache missing names as well.
 * Fixed new connect with specific local port so can be re-used (simultaneous FTP session bug)
 *
 * Revision 1.54  1996/12/05 11:46:39  craigs
 * Fixed problem with Win95 recvfrom not having timeouts
 *
 * Revision 1.53  1996/11/30 12:08:17  robertj
 * Added Connect() variant so can set the local port number on link.
 *
 * Revision 1.52  1996/11/16 10:49:03  robertj
 * Fixed missing const in PIPSocket::Address stream output operator..
 *
 * Revision 1.51  1996/11/16 01:43:49  craigs
 * Fixed problem with ambiguous DNS cache keys
 *
 * Revision 1.50  1996/11/10 21:08:31  robertj
 * Added host name caching.
 *
 * Revision 1.49  1996/11/04 03:40:22  robertj
 * Moved address printer from inline to source.
 *
 * Revision 1.48  1996/10/26 01:41:09  robertj
 * Compensated for Win'95 gethostbyaddr bug.
 *
 * Revision 1.47  1996/09/14 13:09:40  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.46  1996/08/25 09:33:32  robertj
 * Added function to detect "local" host name.
 *
 * Revision 1.45  1996/07/30 12:24:53  robertj
 * Fixed incorrect conditional stopping Select() from working.
 *
 * Revision 1.44  1996/07/27 04:10:35  robertj
 * Changed Select() calls to return error codes.
 *
 * Revision 1.43  1996/06/10 09:58:21  robertj
 * Fixed win95 compatibility with looking up zero address (got a response and shouldn't).
 *
 * Revision 1.42  1996/05/26 03:47:03  robertj
 * Compatibility to GNU 2.7.x
 *
 * Revision 1.39  1996/04/29 12:20:01  robertj
 * Fixed GetHostAliases() so doesn't overwrite names with IP numbers.
 *
 * Revision 1.38  1996/04/15 10:59:41  robertj
 * Opened socket on UDP sockets so ReadFrom/WriteTo work when no Connect/Listen.
 *
 * Revision 1.37  1996/03/31 09:06:41  robertj
 * Added socket shutdown function.
 *
 * Revision 1.35  1996/03/18 13:33:18  robertj
 * Fixed incompatibilities to GNU compiler where PINDEX != int.
 *
 * Revision 1.34  1996/03/17 05:51:18  robertj
 * Fixed strange bug in accept cant have NULL address.
 *
 * Revision 1.33  1996/03/16 04:52:20  robertj
 * Changed all the get host name and get host address functions to be more consistent.
 *
 * Revision 1.32  1996/03/04 12:21:00  robertj
 * Split file into telnet.cxx
 *
 * Revision 1.31  1996/03/03 07:38:45  robertj
 * Added Reusability clause to the Listen() function on sockets.
 *
 * Revision 1.30  1996/03/02 03:25:13  robertj
 * Added Capability to get and set Berkeley socket options.
 *
 * Revision 1.29  1996/02/25 11:30:08  robertj
 * Changed Listen so can do a listen on a socket that is connected.
 *
 * Revision 1.28  1996/02/25 03:10:55  robertj
 * Moved some socket functions to platform dependent code.
 *
 * Revision 1.27  1996/02/19 13:30:15  robertj
 * Fixed bug in getting port by service name when specifying service by string number.
 * Added SO_LINGER option to socket to stop data loss on close.
 *
 * Revision 1.26  1996/02/15 14:46:44  robertj
 * Added Select() function to PSocket.
 *
 * Revision 1.25  1996/02/13 13:08:09  robertj
 * Fixed usage of sock_addr structure, not being cleared correctly.
 *
 * Revision 1.24  1996/02/08 12:27:22  robertj
 * Added function to get peer port as well as IP number..
 *
 * Revision 1.23  1996/02/03 11:07:37  robertj
 * Fixed buf in assuring error when converting string to IP number and string is empty.
 *
 * Revision 1.22  1996/01/28 14:08:13  robertj
 * Changed service parameter to PString for ease of use in GetPortByService function
 * Fixed up comments.
 * Added default value in string for service name.
 *
 * Revision 1.21  1996/01/23 13:19:13  robertj
 * Moved Accept() function to platform dependent code.
 *
 * Revision 1.20  1995/12/23 03:42:53  robertj
 * Unix portability issues.
 *
 * Revision 1.19  1995/12/10 11:42:23  robertj
 * Numerous fixes for sockets.
 *
 * Revision 1.18  1995/10/14 15:11:31  robertj
 * Added internet address to string conversion functionality.
 *
 * Revision 1.17  1995/07/02 01:21:23  robertj
 * Added static functions to get the current host name/address.
 *
 * Revision 1.16  1995/06/17 00:47:01  robertj
 * Changed overloaded Open() calls to 3 separate function names.
 * More logical design of port numbers and service names.
 *
 * Revision 1.15  1995/06/04 12:45:33  robertj
 * Added application layer protocol sockets.
 * Slight redesign of port numbers on sockets.
 *
 * Revision 1.14  1995/04/25 11:12:44  robertj
 * Fixed functions hiding ancestor virtuals.
 *
 * Revision 1.13  1995/04/01 08:31:54  robertj
 * Finally got a working TELNET.
 *
 * Revision 1.12  1995/03/18 06:27:49  robertj
 * Rewrite of telnet socket protocol according to RFC1143.
 *
 * Revision 1.11  1995/03/12  04:46:29  robertj
 * Added more functionality.
 *
 * Revision 1.10  1995/02/21  11:25:29  robertj
 * Further implementation of telnet socket, feature complete now.
 *
 * Revision 1.9  1995/01/27  11:16:16  robertj
 * Fixed missing cast in function, required by some platforms.
 *
 * Revision 1.8  1995/01/15  04:55:47  robertj
 * Moved all Berkley socket functions inside #ifdef.
 *
 * Revision 1.7  1995/01/04  10:57:08  robertj
 * Changed for HPUX and GNU2.6.x
 *
 * Revision 1.6  1995/01/03  09:37:52  robertj
 * Added constructor to open TCP socket.
 *
 * Revision 1.5  1995/01/02  12:28:25  robertj
 * Documentation.
 * Added more socket functions.
 *
 * Revision 1.4  1995/01/01  01:06:58  robertj
 * More implementation.
 *
 * Revision 1.3  1994/11/28  12:38:49  robertj
 * Added DONT and WONT states.
 *
 * Revision 1.2  1994/08/21  23:43:02  robertj
 * Some implementation.
 *
 * Revision 1.1  1994/08/01  03:39:05  robertj
 * Initial revision
 *
 */

#ifdef __NUCLEUS_PLUS__
#include <ConfigurationClass.h>
#endif


#include <ptlib.h>

#include <ptlib/sockets.h>

#include <ctype.h>

#ifdef P_VXWORKS
// VxWorks variant of inet_ntoa() allocates INET_ADDR_LEN bytes via malloc
// BUT DOES NOT FREE IT !!!  Use inet_ntoa_b() instead.
#define INET_ADDR_LEN      18
extern "C" void inet_ntoa_b(struct in_addr inetAddress, char *pString);
#endif // P_VXWORKS

#if P_HAS_QOS

#ifdef _WIN32
#include <winbase.h>
#include <winreg.h>

#ifndef _WIN32_WCE

void CALLBACK CompletionRoutine(DWORD dwError,
                                DWORD cbTransferred,
                                LPWSAOVERLAPPED lpOverlapped,
                                DWORD dwFlags);
                                

#endif  // _WIN32_WCE
#endif  // _WIN32
#endif // P_HAS_QOS

///////////////////////////////////////////////////////////////////////////////
// PIPSocket::Address

static int defaultIpAddressFamily = PF_INET;  // PF_UNSPEC;   // default to IPV4

static PIPSocket::Address loopback4(127,0,0,1);
static PIPSocket::Address broadcast4(INADDR_BROADCAST);
static PIPSocket::Address any4(INADDR_ANY);
static in_addr inaddr_empty;
#if P_HAS_IPV6
static PIPSocket::Address loopback6(16,(const BYTE *)"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\001");
static PIPSocket::Address any6(16,(const BYTE *)"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"); 
#endif


int PIPSocket::GetDefaultIpAddressFamily()
{
  return defaultIpAddressFamily;
}


void PIPSocket::SetDefaultIpAddressFamily(int ipAdressFamily)
{
  defaultIpAddressFamily = ipAdressFamily;
}


void PIPSocket::SetDefaultIpAddressFamilyV4()
{
  SetDefaultIpAddressFamily(PF_INET);
}


#if P_HAS_IPV6

void PIPSocket::SetDefaultIpAddressFamilyV6()
{
  SetDefaultIpAddressFamily(PF_INET6);
}


BOOL PIPSocket::IsIpAddressFamilyV6Supported()
{
  int s = ::socket(PF_INET6, SOCK_DGRAM, 0);
  if (s < 0)
    return FALSE;

#if _WIN32
  closesocket(s);
#else
  _close(s);
#endif
  return TRUE;
}

#endif


PIPSocket::Address PIPSocket::GetDefaultIpAny()
{
#if P_HAS_IPV6
  if (defaultIpAddressFamily != PF_INET)
    return any6;
#endif

  return any4;
}


#if P_HAS_IPV6

class Psockaddr
{
  public:
    Psockaddr() { memset(&storage, 0, sizeof(storage)); }
    Psockaddr(const PIPSocket::Address & ip, WORD port);
    sockaddr* operator->() const { return (sockaddr *)&storage; }
    operator sockaddr*()   const { return (sockaddr *)&storage; }
    socklen_t GetSize() const;
    PIPSocket::Address GetIP() const;
    WORD GetPort() const;
  private:
    sockaddr_storage storage;
};


Psockaddr::Psockaddr(const PIPSocket::Address & ip, WORD port)
{
  memset(&storage, 0, sizeof(storage));

  if (ip.GetVersion() == 6) {
    sockaddr_in6 * addr6 = (sockaddr_in6 *)&storage;
    addr6->sin6_family = AF_INET6;
    addr6->sin6_addr = ip;
    addr6->sin6_port = htons(port);
    addr6->sin6_flowinfo = 0;
    addr6->sin6_scope_id = 0; // Should be set to the right interface....
  }
  else {
    sockaddr_in * addr4 = (sockaddr_in *)&storage;
    addr4->sin_family = AF_INET;
    addr4->sin_addr = ip;
    addr4->sin_port = htons(port);
  }
}


socklen_t Psockaddr::GetSize() const
{
  switch (((sockaddr *)&storage)->sa_family) {
    case AF_INET :
      return sizeof(sockaddr_in);
    case AF_INET6 :
      // RFC 2133 (Old IPv6 spec) size is 24
      // RFC 2553 (New IPv6 spec) size is 28
      return sizeof(sockaddr_in6);
    default :
      return sizeof(storage);
  }
}


PIPSocket::Address Psockaddr::GetIP() const
{
  switch (((sockaddr *)&storage)->sa_family) {
    case AF_INET :
      return ((sockaddr_in *)&storage)->sin_addr;
    case AF_INET6 :
      return ((sockaddr_in6 *)&storage)->sin6_addr;
    default :
      return 0;
  }
}


WORD Psockaddr::GetPort() const
{
  switch (((sockaddr *)&storage)->sa_family) {
    case AF_INET :
      return ntohs(((sockaddr_in *)&storage)->sin_port);
    case AF_INET6 :
      return ntohs(((sockaddr_in6 *)&storage)->sin6_port);
    default :
      return 0;
  }
}

#endif


#if (defined(_WIN32) || defined(WINDOWS)) && !defined(__NUCLEUS_MNT__)
static PWinSock dummyForWinSock; // Assure winsock is initialised
#endif

#if (defined(P_PTHREADS) && !defined(P_THREAD_SAFE_CLIB)) || defined(__NUCLEUS_PLUS__)
#define REENTRANT_BUFFER_LEN 1024
#endif


class PIPCacheData : public PObject
{
  PCLASSINFO(PIPCacheData, PObject)
  public:
    PIPCacheData(struct hostent * ent, const char * original);
#if P_HAS_IPV6
    PIPCacheData(struct addrinfo  * addr_info, const char * original);
    void AddEntry(struct addrinfo  * addr_info);
#endif
    const PString & GetHostName() const { return hostname; }
    const PIPSocket::Address & GetHostAddress() const { return address; }
    const PStringList & GetHostAliases() const { return aliases; }
    BOOL HasAged() const;
  private:
    PString            hostname;
    PIPSocket::Address address;
    PStringList        aliases;
    PTime              birthDate;
};



PDICTIONARY(PHostByName_private, PCaselessString, PIPCacheData);

class PHostByName : PHostByName_private
{
  public:
    BOOL GetHostName(const PString & name, PString & hostname);
    BOOL GetHostAddress(const PString & name, PIPSocket::Address & address);
    BOOL GetHostAliases(const PString & name, PStringArray & aliases);
  private:
    PIPCacheData * GetHost(const PString & name);
    PMutex mutex;
  friend void PIPSocket::ClearNameCache();
};

PMutex creationMutex;
static PHostByName & pHostByName()
{
  PWaitAndSignal m(creationMutex);
  static PHostByName t;
  return t;
}

class PIPCacheKey : public PObject
{
  PCLASSINFO(PIPCacheKey, PObject)
  public:
    PIPCacheKey(const PIPSocket::Address & a)
      { addr = a; }

    PObject * Clone() const
      { return new PIPCacheKey(*this); }

    PINDEX HashFunction() const
      { return (addr[1] + addr[2] + addr[3])%41; }

  private:
    PIPSocket::Address addr;
};

PDICTIONARY(PHostByAddr_private, PIPCacheKey, PIPCacheData);

class PHostByAddr : PHostByAddr_private
{
  public:
    BOOL GetHostName(const PIPSocket::Address & addr, PString & hostname);
    BOOL GetHostAddress(const PIPSocket::Address & addr, PIPSocket::Address & address);
    BOOL GetHostAliases(const PIPSocket::Address & addr, PStringArray & aliases);
  private:
    PIPCacheData * GetHost(const PIPSocket::Address & addr);
    PMutex mutex;
  friend void PIPSocket::ClearNameCache();
};

static PHostByAddr & pHostByAddr()
{
  PWaitAndSignal m(creationMutex);
  static PHostByAddr t;
  return t;
}

#define new PNEW


//////////////////////////////////////////////////////////////////////////////
// IP Caching

PIPCacheData::PIPCacheData(struct hostent * host_info, const char * original)
{
  if (host_info == NULL) {
    address = 0;
    return;
  }

  hostname = host_info->h_name;
  if (host_info->h_addr != NULL)
#ifndef _WIN32_WCE
    address = *(DWORD *)host_info->h_addr;
#else
    address = PIPSocket::Address(host_info->h_length, (const BYTE *)host_info->h_addr);
#endif
  aliases.AppendString(host_info->h_name);

  PINDEX i;
  for (i = 0; host_info->h_aliases[i] != NULL; i++)
    aliases.AppendString(host_info->h_aliases[i]);

  for (i = 0; host_info->h_addr_list[i] != NULL; i++) {
#ifndef _WIN32_WCE
    PIPSocket::Address ip(*(DWORD *)host_info->h_addr_list[i]);
#else
    PIPSocket::Address ip(host_info->h_length, (const BYTE *)host_info->h_addr_list[i]);
#endif
    aliases.AppendString(ip.AsString());
  }

  for (i = 0; i < aliases.GetSize(); i++)
    if (aliases[i] *= original)
      return;

  aliases.AppendString(original);
}


#if P_HAS_IPV6

PIPCacheData::PIPCacheData(struct addrinfo * addr_info, const char * original)
{
  PINDEX i;
  if (addr_info == NULL) {
    address = 0;
    return;
  }

  // Fill Host primary informations
  hostname = addr_info->ai_canonname; // Fully Qualified Domain Name (FQDN)
  if (addr_info->ai_addr != NULL)
    address = PIPSocket::Address(addr_info->ai_family, addr_info->ai_addrlen, addr_info->ai_addr);

  // Next entries
  while (addr_info != NULL) {
    AddEntry(addr_info);
    addr_info = addr_info->ai_next;
  }

  // Add original as alias or allready added ?
  for (i = 0; i < aliases.GetSize(); i++) {
    if (aliases[i] *= original)
      return;
  }

  aliases.AppendString(original);
}


void PIPCacheData::AddEntry(struct addrinfo * addr_info)
{
  PINDEX i;

  if (addr_info == NULL)
    return;

  // Add canonical name
  BOOL add_it = TRUE;
  for (i = 0; i < aliases.GetSize(); i++) {
    if (addr_info->ai_canonname != NULL && (aliases[i] *= addr_info->ai_canonname)) {
      add_it = FALSE;
      break;
    }
  }

  if (add_it && addr_info->ai_canonname != NULL)
    aliases.AppendString(addr_info->ai_canonname);

  // Add IP address
  PIPSocket::Address ip(addr_info->ai_family, addr_info->ai_addrlen, addr_info->ai_addr);
  add_it = TRUE;
  for (i = 0; i < aliases.GetSize(); i++) {
    if (aliases[i] *= ip.AsString()) {
      add_it = FALSE;
      break;
    }
  }

  if (add_it)
    aliases.AppendString(ip.AsString());
}

#endif


static PTimeInterval GetConfigTime(const char * /*key*/, DWORD dflt)
{
  //PConfig cfg("DNS Cache");
  //return cfg.GetInteger(key, dflt);
  return dflt;
}


BOOL PIPCacheData::HasAged() const
{
  static PTimeInterval retirement = GetConfigTime("Age Limit", 300000); // 5 minutes
  PTime now;
  PTimeInterval age = now - birthDate;
  return age > retirement;
}


BOOL PHostByName::GetHostName(const PString & name, PString & hostname)
{
  PIPCacheData * host = GetHost(name);

  if (host != NULL) {
    hostname = host->GetHostName();
    hostname.MakeUnique();
  }

  mutex.Signal();

  return host != NULL;
}


BOOL PHostByName::GetHostAddress(const PString & name, PIPSocket::Address & address)
{
  PIPCacheData * host = GetHost(name);

  if (host != NULL)
    address = host->GetHostAddress();

  mutex.Signal();

  return host != NULL;
}


BOOL PHostByName::GetHostAliases(const PString & name, PStringArray & aliases)
{
  PIPCacheData * host = GetHost(name);

  if (host != NULL) {
    const PStringList & a = host->GetHostAliases();
    aliases.SetSize(a.GetSize());
    for (PINDEX i = 0; i < a.GetSize(); i++)
      aliases[i] = a[i];
  }

  mutex.Signal();
  return host != NULL;
}


PIPCacheData * PHostByName::GetHost(const PString & name)
{
  mutex.Wait();

  PCaselessString key = name;

  // Check for a legal hostname as per RFC952
  if (key.IsEmpty() ||
      key.FindSpan("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.") != P_MAX_INDEX ||
      key[key.GetLength()-1] == '-' ||
      !isalpha(key[0]))
    return NULL;

  PIPCacheData * host = GetAt(key);
  int localErrNo = NETDB_SUCCESS;

  if (host != NULL && host->HasAged()) {
    SetAt(key, NULL);
    host = NULL;
  }

  if (host == NULL) {
    mutex.Signal();

#if P_HAS_IPV6
    struct addrinfo *res = NULL;
    struct addrinfo hints = { AI_CANONNAME, PF_UNSPEC };
    hints.ai_family = defaultIpAddressFamily;

    localErrNo = getaddrinfo((const char *)name, NULL , &hints, &res);
    mutex.Wait();

    if (localErrNo != NETDB_SUCCESS) {
      freeaddrinfo(res);
      return NULL;
    }
    host = new PIPCacheData(res, name);
    freeaddrinfo(res);
#else // P_HAS_IPV6

    int retry = 3;
    struct hostent * host_info;

#ifdef P_AIX

    struct hostent_data ht_data;
    memset(&ht_data, 0, sizeof(ht_data));
    struct hostent hostEnt;
    do {
      host_info = &hostEnt;
      ::gethostbyname_r(name,
                        host_info,
                        &ht_data);
      localErrNo = h_errno;
    } while (localErrNo == TRY_AGAIN && --retry > 0);

#elif defined(P_RTEMS) || defined(P_CYGWIN) || defined(P_MINGW)

    host_info = ::gethostbyname(name);
    localErrNo = h_errno;

#elif defined P_VXWORKS

    struct hostent hostEnt;
    host_info = Vx_gethostbyname((char *)name, &hostEnt);
    localErrNo = h_errno;

#elif defined P_LINUX

    char buffer[REENTRANT_BUFFER_LEN];
    struct hostent hostEnt;
    do {
      if (::gethostbyname_r(name,
                            &hostEnt,
                            buffer, REENTRANT_BUFFER_LEN,
                            &host_info,
                            &localErrNo) == 0)
        localErrNo = NETDB_SUCCESS;
    } while (localErrNo == TRY_AGAIN && --retry > 0);

#elif (defined(P_PTHREADS) && !defined(P_THREAD_SAFE_CLIB)) || defined(__NUCLEUS_PLUS__)

    char buffer[REENTRANT_BUFFER_LEN];
    struct hostent hostEnt;
    do {
      host_info = ::gethostbyname_r(name,
                                    &hostEnt,
                                    buffer, REENTRANT_BUFFER_LEN,
                                    &localErrNo);
    } while (localErrNo == TRY_AGAIN && --retry > 0);

#else

    host_info = ::gethostbyname(name);
    localErrNo = h_errno;

#endif

    mutex.Wait();

    if (localErrNo != NETDB_SUCCESS || retry == 0)
      return NULL;
    host = new PIPCacheData(host_info, name);

#endif //P_HAS_IPV6

    SetAt(key, host);
  }

  if (host->GetHostAddress() == 0)
    return NULL;

  return host;
}


BOOL PHostByAddr::GetHostName(const PIPSocket::Address & addr, PString & hostname)
{
  PIPCacheData * host = GetHost(addr);

  if (host != NULL) {
    hostname = host->GetHostName();
    hostname.MakeUnique();
  }

  mutex.Signal();
  return host != NULL;
}


BOOL PHostByAddr::GetHostAddress(const PIPSocket::Address & addr, PIPSocket::Address & address)
{
  PIPCacheData * host = GetHost(addr);

  if (host != NULL)
    address = host->GetHostAddress();

  mutex.Signal();
  return host != NULL;
}


BOOL PHostByAddr::GetHostAliases(const PIPSocket::Address & addr, PStringArray & aliases)
{
  PIPCacheData * host = GetHost(addr);

  if (host != NULL) {
    const PStringList & a = host->GetHostAliases();
    aliases.SetSize(a.GetSize());
    for (PINDEX i = 0; i < a.GetSize(); i++)
      aliases[i] = a[i];
  }

  mutex.Signal();
  return host != NULL;
}

PIPCacheData * PHostByAddr::GetHost(const PIPSocket::Address & addr)
{
  mutex.Wait();

  PIPCacheKey key = addr;
  PIPCacheData * host = GetAt(key);

  if (host != NULL && host->HasAged()) {
    SetAt(key, NULL);
    host = NULL;
  }

  if (host == NULL) {
    mutex.Signal();

    int retry = 3;
    int localErrNo = NETDB_SUCCESS;
    struct hostent * host_info;

#ifdef P_AIX

    struct hostent_data ht_data;
    struct hostent hostEnt;
    do {
      host_info = &hostEnt;
      ::gethostbyaddr_r((char *)addr.GetPointer(), addr.GetSize(),
                        PF_INET, 
                        host_info,
                        &ht_data);
      localErrNo = h_errno;
    } while (localErrNo == TRY_AGAIN && --retry > 0);

#elif defined P_RTEMS || defined P_CYGWIN || defined P_MINGW

    host_info = ::gethostbyaddr(addr.GetPointer(), addr.GetSize(), PF_INET);
    localErrNo = h_errno;

#elif defined P_VXWORKS

    struct hostent hostEnt;
    host_info = Vx_gethostbyaddr(addr.GetPointer(), &hostEnt);

#elif defined P_LINUX

    char buffer[REENTRANT_BUFFER_LEN];
    struct hostent hostEnt;
    do {
      ::gethostbyaddr_r(addr.GetPointer(), addr.GetSize(),
                        PF_INET, 
                        &hostEnt,
                        buffer, REENTRANT_BUFFER_LEN,
                        &host_info,
                        &localErrNo);
    } while (localErrNo == TRY_AGAIN && --retry > 0);

#elif (defined(P_PTHREADS) && !defined(P_THREAD_SAFE_CLIB)) || defined(__NUCLEUS_PLUS__)

    char buffer[REENTRANT_BUFFER_LEN];
    struct hostent hostEnt;
    do {
      host_info = ::gethostbyaddr_r(addr.GetPointer(), addr.GetSize(),
                                    PF_INET, 
                                    &hostEnt,
                                    buffer, REENTRANT_BUFFER_LEN,
                                    &localErrNo);
    } while (localErrNo == TRY_AGAIN && --retry > 0);

#else

    host_info = ::gethostbyaddr(addr.GetPointer(), addr.GetSize(), PF_INET);
    localErrNo = h_errno;

#if defined(_WIN32) || defined(WINDOWS)  // Kludge to avoid strange 95 bug
    extern int P_IsOldWin95();
    if (P_IsOldWin95() && host_info != NULL && host_info->h_addr_list[0] != NULL)
      host_info->h_addr_list[1] = NULL;
#endif

#endif

    mutex.Wait();

    if (localErrNo != NETDB_SUCCESS || retry == 0)
      return NULL;

    host = new PIPCacheData(host_info, addr.AsString());

    SetAt(key, host);
  }

  if (host->GetHostAddress() == 0)
    return NULL;

  return host;
}


//////////////////////////////////////////////////////////////////////////////
// P_fd_set

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)
#endif

P_fd_set::P_fd_set()
{
  Construct();
  Zero();
}


P_fd_set::P_fd_set(SOCKET fd)
{
  Construct();
  Zero();
  FD_SET(fd, set);
}


P_fd_set & P_fd_set::operator=(SOCKET fd)
{
  PAssert(fd < max_fd, PInvalidParameter);
  Zero();
  FD_SET(fd, set);
  return *this;
}


P_fd_set & P_fd_set::operator+=(SOCKET fd)
{
  PAssert(fd < max_fd, PInvalidParameter);
  FD_SET(fd, set);
  return *this;
}


P_fd_set & P_fd_set::operator-=(SOCKET fd)
{
  PAssert(fd < max_fd, PInvalidParameter);
  FD_CLR(fd, set);
  return *this;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif


//////////////////////////////////////////////////////////////////////////////
// P_timeval

P_timeval::P_timeval()
{
  tval.tv_usec = 0;
  tval.tv_sec = 0;
  infinite = FALSE;
}


P_timeval & P_timeval::operator=(const PTimeInterval & time)
{
  infinite = time == PMaxTimeInterval;
  tval.tv_usec = (long)(time.GetMilliSeconds()%1000)*1000;
  tval.tv_sec = time.GetSeconds();
  return *this;
}


//////////////////////////////////////////////////////////////////////////////
// PSocket

PSocket::PSocket()
{
  port = 0;
#if P_HAS_RECVMSG
  catchReceiveToAddr = FALSE;
#endif
}


BOOL PSocket::Connect(const PString &)
{
  PAssertAlways("Illegal operation.");
  return FALSE;
}


BOOL PSocket::Listen(unsigned, WORD, Reusability)
{
  PAssertAlways("Illegal operation.");
  return FALSE;
}


BOOL PSocket::Accept(PSocket &)
{
  PAssertAlways("Illegal operation.");
  return FALSE;
}


BOOL PSocket::SetOption(int option, int value, int level)
{
#ifdef _WIN32_WCE
  if(option == SO_RCVBUF || option == SO_SNDBUF || option == IP_TOS)
    return TRUE;
#endif

  return ConvertOSError(::setsockopt(os_handle, level, option,
                                     (char *)&value, sizeof(value)));
}


BOOL PSocket::SetOption(int option, const void * valuePtr, PINDEX valueSize, int level)
{
  return ConvertOSError(::setsockopt(os_handle, level, option,
                                     (char *)valuePtr, valueSize));
}


BOOL PSocket::GetOption(int option, int & value, int level)
{
  socklen_t valSize = sizeof(value);
  return ConvertOSError(::getsockopt(os_handle, level, option,
                                     (char *)&value, &valSize));
}


BOOL PSocket::GetOption(int option, void * valuePtr, PINDEX valueSize, int level)
{
  return ConvertOSError(::getsockopt(os_handle, level, option,
                                     (char *)valuePtr, (socklen_t *)&valueSize));
}


BOOL PSocket::Shutdown(ShutdownValue value)
{
  return ConvertOSError(::shutdown(os_handle, value));
}


WORD PSocket::GetProtocolByName(const PString & name)
{
#if !defined(__NUCLEUS_PLUS__) && !defined(P_VXWORKS)
  struct protoent * ent = getprotobyname(name);
  if (ent != NULL)
    return ent->p_proto;
#endif

  return 0;
}


PString PSocket::GetNameByProtocol(WORD proto)
{
#if !defined(__NUCLEUS_PLUS__) && !defined(P_VXWORKS)
  struct protoent * ent = getprotobynumber(proto);
  if (ent != NULL)
    return ent->p_name;
#endif

  return psprintf("%u", proto);
}


WORD PSocket::GetPortByService(const PString & serviceName) const
{
  return GetPortByService(GetProtocolName(), serviceName);
}


WORD PSocket::GetPortByService(const char * protocol, const PString & service)
{
  // if the string is a valid integer, then use integer value
  // this avoids stupid problems like operating systems that match service
  // names to substrings (like "2000" to "taskmaster2000")
  if (service.FindSpan("0123456789") == P_MAX_INDEX)
    return (WORD)service.AsUnsigned();

#if defined( __NUCLEUS_PLUS__ )
  PAssertAlways("PSocket::GetPortByService: problem as no ::getservbyname in Nucleus NET");
  return 0;
#elif defined(P_VXWORKS)
  PAssertAlways("PSocket::GetPortByService: problem as no ::getservbyname in VxWorks");
  return 0;
#else
  PINDEX space = service.FindOneOf(" \t\r\n");
  struct servent * serv = ::getservbyname(service(0, space-1), protocol);
  if (serv != NULL)
    return ntohs(serv->s_port);

  long portNum;
  if (space != P_MAX_INDEX)
    portNum = atol(service(space+1, P_MAX_INDEX));
  else if (isdigit(service[0]))
    portNum = atoi(service);
  else
    portNum = -1;

  if (portNum < 0 || portNum > 65535)
    return 0;

  return (WORD)portNum;
#endif
}


PString PSocket::GetServiceByPort(WORD port) const
{
  return GetServiceByPort(GetProtocolName(), port);
}


PString PSocket::GetServiceByPort(const char * protocol, WORD port)
{
#if !defined(__NUCLEUS_PLUS__) && !defined(P_VXWORKS)
  struct servent * serv = ::getservbyport(htons(port), protocol);
  if (serv != NULL)
    return PString(serv->s_name);
  else
#endif
    return PString(PString::Unsigned, port);
}


void PSocket::SetPort(WORD newPort)
{
  PAssert(!IsOpen(), "Cannot change port number of opened socket");
  port = newPort;
}


void PSocket::SetPort(const PString & service)
{
  PAssert(!IsOpen(), "Cannot change port number of opened socket");
  port = GetPortByService(service);
}


WORD PSocket::GetPort() const
{
  return port;
}


PString PSocket::GetService() const
{
  return GetServiceByPort(port);
}


int PSocket::Select(PSocket & sock1, PSocket & sock2)
{
  return Select(sock1, sock2, PMaxTimeInterval);
}


int PSocket::Select(PSocket & sock1,
                    PSocket & sock2,
                    const PTimeInterval & timeout)
{
  SelectList read, dummy1, dummy2;
  read += sock1;
  read += sock2;

  Errors lastError;
  int osError;
  if (!ConvertOSError(Select(read, dummy1, dummy2, timeout), lastError, osError))
    return lastError;

  switch (read.GetSize()) {
    case 0 :
      return 0;
    case 2 :
      return -3;
    default :
      return &read[0] == &sock1 ? -1 : -2;
  }
}


PChannel::Errors PSocket::Select(SelectList & read)
{
  SelectList dummy1, dummy2;
  return Select(read, dummy1, dummy2, PMaxTimeInterval);
}


PChannel::Errors PSocket::Select(SelectList & read, const PTimeInterval & timeout)
{
  SelectList dummy1, dummy2;
  return Select(read, dummy1, dummy2, timeout);
}


PChannel::Errors PSocket::Select(SelectList & read, SelectList & write)
{
  SelectList dummy1;
  return Select(read, write, dummy1, PMaxTimeInterval);
}


PChannel::Errors PSocket::Select(SelectList & read,
                                 SelectList & write,
                                 const PTimeInterval & timeout)
{
  SelectList dummy1;
  return Select(read, write, dummy1, timeout);
}


PChannel::Errors PSocket::Select(SelectList & read,
                                 SelectList & write,
                                 SelectList & except)
{
  return Select(read, write, except, PMaxTimeInterval);
}


//////////////////////////////////////////////////////////////////////////////
// PIPSocket

PIPSocket::PIPSocket()
{
}


void PIPSocket::ClearNameCache()
{
  pHostByName().mutex.Wait();
  pHostByAddr().mutex.Wait();
  pHostByName().RemoveAll();
  pHostByAddr().RemoveAll();
#if (defined(_WIN32) || defined(WINDOWS)) && !defined(__NUCLEUS_MNT__) // Kludge to avoid strange NT bug
  static PTimeInterval delay = GetConfigTime("NT Bug Delay", 0);
  if (delay != 0) {
    ::Sleep(delay.GetInterval());
    ::gethostbyname("www.microsoft.com");
  }
#endif
  pHostByName().mutex.Signal();
  pHostByAddr().mutex.Signal();
}


PString PIPSocket::GetName() const
{
#if P_HAS_IPV6

  Psockaddr sa;
  socklen_t size = sa.GetSize();
  if (getpeername(os_handle, sa, &size) == 0)
    return GetHostName(sa.GetIP()) + psprintf(":%u", sa.GetPort());

#else

  sockaddr_in address;
  socklen_t size = sizeof(address);
  if (getpeername(os_handle, (struct sockaddr *)&address, &size) == 0)
    return GetHostName(address.sin_addr) + psprintf(":%u", ntohs(address.sin_port));

#endif

  return PString::Empty();
}


PString PIPSocket::GetHostName()
{
  char name[100];
  if (gethostname(name, sizeof(name)-1) != 0)
    return "localhost";
  name[sizeof(name)-1] = '\0';
  return name;
}


PString PIPSocket::GetHostName(const PString & hostname)
{
  // lookup the host address using inet_addr, assuming it is a "." address
  Address temp = hostname;
  if (temp != 0)
    return GetHostName(temp);

  PString canonicalname;
  if (pHostByName().GetHostName(hostname, canonicalname))
    return canonicalname;

  return hostname;
}


PString PIPSocket::GetHostName(const Address & addr)
{
  if (addr == 0)
    return addr.AsString();

  PString hostname;
  if (pHostByAddr().GetHostName(addr, hostname))
    return hostname;

  return addr.AsString();
}


BOOL PIPSocket::GetHostAddress(Address & addr)
{
  return pHostByName().GetHostAddress(GetHostName(), addr);
}


BOOL PIPSocket::GetHostAddress(const PString & hostname, Address & addr)
{
  if (hostname.IsEmpty())
    return FALSE;

  // Check for special case of "[ipaddr]"
  if (hostname[0] == '[') {
    PINDEX end = hostname.Find(']');
    if (end != P_MAX_INDEX) {
      if (addr.FromString(hostname(1, end-1)))
        return TRUE;
    }
  }

  // Assuming it is a "." address and return if so
  if (addr.FromString(hostname))
    return TRUE;

  // otherwise lookup the name as a host name
  return pHostByName().GetHostAddress(hostname, addr);
}


PStringArray PIPSocket::GetHostAliases(const PString & hostname)
{
  PStringArray aliases;

  // lookup the host address using inet_addr, assuming it is a "." address
  Address addr = hostname;
  if (addr != 0)
    pHostByAddr().GetHostAliases(addr, aliases);
  else
    pHostByName().GetHostAliases(hostname, aliases);

  return aliases;
}


PStringArray PIPSocket::GetHostAliases(const Address & addr)
{
  PStringArray aliases;

  pHostByAddr().GetHostAliases(addr, aliases);

  return aliases;
}


PString PIPSocket::GetLocalAddress()
{
  PStringStream str;
  Address addr;
  WORD port;
  if (GetLocalAddress(addr, port))
    str << addr << ':' << port;
  return str;
}


BOOL PIPSocket::GetLocalAddress(Address & addr)
{
  WORD dummy;
  return GetLocalAddress(addr, dummy);
}


BOOL PIPSocket::GetLocalAddress(Address & addr, WORD & portNum)
{
#if P_HAS_IPV6
  Address   addrv4;
  Address   peerv4;
  Psockaddr sa;
  socklen_t size = sa.GetSize();
  if (!ConvertOSError(::getsockname(os_handle, sa, &size)))
    return FALSE;

  addr = sa.GetIP();
  portNum = sa.GetPort();

  // If the remote host is an IPv4 only host and our interface if an IPv4/IPv6 mapped
  // Then return an IPv4 address instead of an IPv6
  if (GetPeerAddress(peerv4)) {
    if ((peerv4.GetVersion()==4)||(peerv4.IsV4Mapped())) {
      if (addr.IsV4Mapped()) {
        addr = Address(addr[12], addr[13], addr[14], addr[15]);
      }
    }
  }
  
#else

  sockaddr_in address;
  socklen_t size = sizeof(address);
  if (!ConvertOSError(::getsockname(os_handle,(struct sockaddr*)&address,&size)))
    return FALSE;

  addr = address.sin_addr;
  portNum = ntohs(address.sin_port);

#endif

  return TRUE;
}


PString PIPSocket::GetPeerAddress()
{
  PStringStream str;
  Address addr;
  WORD port;
  if (GetPeerAddress(addr, port))
    str << addr << ':' << port;
  return str;
}


BOOL PIPSocket::GetPeerAddress(Address & addr)
{
  WORD portNum;
  return GetPeerAddress(addr, portNum);
}

BOOL PIPSocket::GetPeerAddress(Address & addr, WORD & portNum)
{
#if P_HAS_IPV6

  Psockaddr sa;
  socklen_t size = sa.GetSize();
  if (!ConvertOSError(::getpeername(os_handle, sa, &size)))
    return FALSE;

  addr = sa.GetIP();
  portNum = sa.GetPort();

#else

  sockaddr_in address;
  socklen_t size = sizeof(address);
  if (!ConvertOSError(::getpeername(os_handle,(struct sockaddr*)&address,&size)))
    return FALSE;

  addr = address.sin_addr;
  portNum = ntohs(address.sin_port);

#endif

  return TRUE;
}


PString PIPSocket::GetLocalHostName()
{
  Address addr;

  if (GetLocalAddress(addr))
    return GetHostName(addr);

  return PString::Empty();
}


PString PIPSocket::GetPeerHostName()
{
  Address addr;

  if (GetPeerAddress(addr))
    return GetHostName(addr);

  return PString::Empty();
}


BOOL PIPSocket::Connect(const PString & host)
{
  Address ipnum(host);
#if P_HAS_IPV6
  if (ipnum.IsValid() || GetHostAddress(host, ipnum))
    return Connect(GetDefaultIpAny(), 0, ipnum);
#else
  if (ipnum.IsValid() || GetHostAddress(host, ipnum))
    return Connect(INADDR_ANY, 0, ipnum);
#endif  
  return FALSE;
}


BOOL PIPSocket::Connect(const Address & addr)
{
#if P_HAS_IPV6
  return Connect(GetDefaultIpAny(), 0, addr);
#else
  return Connect(INADDR_ANY, 0, addr);
#endif
}


BOOL PIPSocket::Connect(WORD localPort, const Address & addr)
{
#if P_HAS_IPV6
  return Connect(GetDefaultIpAny(), localPort, addr);
#else
  return Connect(INADDR_ANY, localPort, addr);
#endif
}


BOOL PIPSocket::Connect(const Address & iface, const Address & addr)
{
  return Connect(iface, 0, addr);
}


BOOL PIPSocket::Connect(const Address & iface, WORD localPort, const Address & addr)
{
  // close the port if it is already open
  if (IsOpen())
    Close();

  // make sure we have a port
  PAssert(port != 0, "Cannot connect socket without setting port");

#if P_HAS_IPV6

  Psockaddr sa(addr, port);

  // attempt to create a socket with the right family
  if (!OpenSocket(sa->sa_family))
    return FALSE;

  if (localPort != 0 || iface.IsValid()) {
    Psockaddr bind_sa(iface, localPort);

    if (!SetOption(SO_REUSEADDR, 0)) {
      os_close();
      return FALSE;
    }
    
    if (!ConvertOSError(::bind(os_handle, bind_sa, bind_sa.GetSize()))) {
      os_close();
      return FALSE;
    }
  }
  
  // attempt to connect
  if (os_connect(sa, sa.GetSize()))
    return TRUE;
  
#else

  // attempt to create a socket
  if (!OpenSocket())
    return FALSE;

  // attempt to connect
  sockaddr_in sin;
  if (localPort != 0 || iface.IsValid()) {
    if (!SetOption(SO_REUSEADDR, 0)) {
      os_close();
      return FALSE;
    }
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = iface;
    sin.sin_port        = htons(localPort);       // set the port
    if (!ConvertOSError(::bind(os_handle, (struct sockaddr*)&sin, sizeof(sin)))) {
      os_close();
      return FALSE;
    }
  }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port   = htons(port);  // set the port
  sin.sin_addr   = addr;
  if (os_connect((struct sockaddr *)&sin, sizeof(sin)))
    return TRUE;

#endif

  os_close();
  return FALSE;
}


BOOL PIPSocket::Listen(unsigned queueSize, WORD newPort, Reusability reuse)
{
#if P_HAS_IPV6
  return Listen(GetDefaultIpAny(), queueSize, newPort, reuse);
#else
  return Listen(INADDR_ANY, queueSize, newPort, reuse);
#endif
}


BOOL PIPSocket::Listen(const Address & bindAddr,
                       unsigned,
                       WORD newPort,
                       Reusability reuse)
{
  // make sure we have a port
  if (newPort != 0)
    port = newPort;

#if P_HAS_IPV6
  Psockaddr bind_sa(bindAddr, port); 

  if (IsOpen()) {
    int socketType;
    if (!GetOption(SO_TYPE, socketType, SOL_SOCKET) || bind_sa->sa_family != socketType)
      Close();
  }
#endif

  if (!IsOpen()) {
    // attempt to create a socket
#if P_HAS_IPV6
    if (!OpenSocket(bind_sa->sa_family))
      return FALSE;
#else
    if (!OpenSocket())
      return FALSE;
#endif
  }
  
#ifndef __BEOS__
  // attempt to listen
  if (!SetOption(SO_REUSEADDR, reuse == CanReuseAddress ? 1 : 0)) {
    os_close();
    return FALSE;
  }
#else
  // attempt to listen
  int value = reuse == CanReuseAddress ? 1 : 0;
  if (!SetOption(SO_REUSEADDR, &value, sizeof(int))) {
    os_close();
    return FALSE;
  }
#endif // BEOS

#if P_HAS_IPV6

  if (ConvertOSError(::bind(os_handle, bind_sa, bind_sa.GetSize()))) {
    Psockaddr sa;
    socklen_t size = sa.GetSize();
    if (!ConvertOSError(::getsockname(os_handle, sa, &size)))
      return FALSE;

    port = sa.GetPort();
    return TRUE;
  }

#else

  // attempt to listen
  sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family      = AF_INET;
  sin.sin_addr.s_addr = bindAddr;
  sin.sin_port        = htons(port);       // set the port

#ifdef __NUCLEUS_NET__
  int bind_result;
  if (port == 0)
    bind_result = ::bindzero(os_handle, (struct sockaddr*)&sin, sizeof(sin));
  else
    bind_result = ::bind(os_handle, (struct sockaddr*)&sin, sizeof(sin));
  if (ConvertOSError(bind_result))
#else
  if (ConvertOSError(::bind(os_handle, (struct sockaddr*)&sin, sizeof(sin))))
#endif
  {
    socklen_t size = sizeof(sin);
    if (ConvertOSError(::getsockname(os_handle, (struct sockaddr*)&sin, &size))) {
      port = ntohs(sin.sin_port);
      return TRUE;
    }
  }

#endif

  os_close();
  return FALSE;
}


const PIPSocket::Address & PIPSocket::Address::GetLoopback()
{
  return loopback4;
}


#if P_HAS_IPV6

/// Check for v4 mapped i nv6 address ::ffff:a.b.c.d
BOOL PIPSocket::Address::IsV4Mapped() const
{
  if (version != 6)
    return FALSE;
  return IN6_IS_ADDR_V4MAPPED(&v.six) || IN6_IS_ADDR_V4COMPAT(&v.six);
}


const PIPSocket::Address & PIPSocket::Address::GetLoopback6()
{
  return loopback6;
}


const PIPSocket::Address & PIPSocket::Address::GetAny6()
{
  return any6;
}

#endif


BOOL PIPSocket::Address::IsAny() const
{
  return (!IsValid());
}


const PIPSocket::Address & PIPSocket::Address::GetBroadcast()
{
  return broadcast4;
}


PIPSocket::Address::Address()
{
  *this = loopback4;
}


PIPSocket::Address::Address(const PString & dotNotation)
{
  operator=(dotNotation);
}


PIPSocket::Address::Address(PINDEX len, const BYTE * bytes)
{
  switch (len) {
#if P_HAS_IPV6
    case 16 :
      version = 6;
      memcpy(&v.six, bytes, len);
      break;
#endif
    case 4 :
      version = 4;
      memcpy(&v.four, bytes, len);
      break;

    default :
      version = 0;
  }
}


PIPSocket::Address::Address(const in_addr & addr)
{
  version = 4;
  v.four = addr;
}


#if P_HAS_IPV6
PIPSocket::Address::Address(const in6_addr & addr)
{
  version = 6;
  v.six = addr;
}

// Create an IP (v4 or v6) address from a sockaddr (sockaddr_in, sockaddr_in6 or sockaddr_in6_old) structure
PIPSocket::Address::Address(const int ai_family, const int ai_addrlen, struct sockaddr *ai_addr)
{
  switch (ai_family) {
#if P_HAS_IPV6
    case AF_INET6:
      if (ai_addrlen < (int)sizeof(sockaddr_in6))
        break;

      version = 6;
      v.six = ((struct sockaddr_in6 *)ai_addr)->sin6_addr;
      //sin6_scope_id, should be taken into account for link local addresses
      return;
#endif
    case AF_INET: 
      if (ai_addrlen < (int)sizeof(sockaddr_in))
        break;

      version = 4;
      v.four = ((struct sockaddr_in  *)ai_addr)->sin_addr;
      return;
  }
  version = 0;
}

#endif


#ifdef __NUCLEUS_NET__
PIPSocket::Address::Address(const struct id_struct & addr)
{
  operator=(addr);
}


PIPSocket::Address & PIPSocket::Address::operator=(const struct id_struct & addr)
{
  s_addr = (((unsigned long)addr.is_ip_addrs[0])<<24) +
           (((unsigned long)addr.is_ip_addrs[1])<<16) +
           (((unsigned long)addr.is_ip_addrs[2])<<8) +
           (((unsigned long)addr.is_ip_addrs[3]));
  return *this;
}
#endif
 

PIPSocket::Address & PIPSocket::Address::operator=(const in_addr & addr)
{
  version = 4;
  v.four = addr;
  return *this;
}

#if P_HAS_IPV6
PIPSocket::Address & PIPSocket::Address::operator=(const in6_addr & addr)
{
  version = 6;
  v.six = addr;
  return *this;
}
#endif


PObject::Comparison PIPSocket::Address::Compare(const PObject & obj) const
{
  const PIPSocket::Address & other = (const PIPSocket::Address &)obj;

  if (version < other.version)
    return LessThan;
  if (version > other.version)
    return GreaterThan;

#if P_HAS_IPV6
  if (version == 6) {
    int result = memcmp(&v.six, &other.v.six, sizeof(v.six));
    if (result < 0)
      return LessThan;
    if (result > 0)
      return GreaterThan;
    return EqualTo;
  }
#endif

  if ((DWORD)*this < other)
    return LessThan;
  if ((DWORD)*this > other)
    return GreaterThan;
  return EqualTo;
}

#if P_HAS_IPV6
bool PIPSocket::Address::operator*=(const PIPSocket::Address & addr) const
{
  if (version == addr.version)
    return operator==(addr);

  if (this->GetVersion() == 6 && this->IsV4Mapped()) 
    return PIPSocket::Address((*this)[12], (*this)[13], (*this)[14], (*this)[15]) == addr;
  else if (addr.GetVersion() == 6 && addr.IsV4Mapped()) 
    return *this == PIPSocket::Address(addr[12], addr[13], addr[14], addr[15]);
  return FALSE;
}

bool PIPSocket::Address::operator==(in6_addr & addr) const
{
  PIPSocket::Address a(addr);
  return Compare(a) == EqualTo;
}
#endif


bool PIPSocket::Address::operator==(in_addr & addr) const
{
  PIPSocket::Address a(addr);
  return Compare(a) == EqualTo;
}


bool PIPSocket::Address::operator==(DWORD dw) const
{
  if (dw != 0)
    return (DWORD)*this == dw;

  return !IsValid();
}


PIPSocket::Address & PIPSocket::Address::operator=(const PString & dotNotation)
{
  version = 0;
  memset(&v, 0, sizeof(v));

#if P_HAS_IPV6

  struct addrinfo *res = NULL;
  struct addrinfo hints = { AI_NUMERICHOST, PF_UNSPEC }; // Could be IPv4: x.x.x.x or IPv6: x:x:x:x::x

  if (getaddrinfo((const char *)dotNotation, NULL , &hints, &res) == 0) {
    if (res->ai_family == PF_INET6) {
      // IPv6 addr
      version = 6;
      struct sockaddr_in6 * addr_in6 = (struct sockaddr_in6 *)res->ai_addr;
      v.six = addr_in6->sin6_addr;
    } else {
      // IPv4 addr
      version = 4;
      struct sockaddr_in * addr_in = (struct sockaddr_in *)res->ai_addr;
      v.four = addr_in->sin_addr;
    }
    freeaddrinfo(res);
  }

#else //P_HAS_IPV6

  DWORD iaddr;
  if (dotNotation.FindSpan("0123456789.") == P_MAX_INDEX &&
                    (iaddr = ::inet_addr((const char *)dotNotation)) != (DWORD)INADDR_NONE) {
    version = 4;
    v.four.s_addr = iaddr;
  }

#endif

  else {
    PINDEX percent = dotNotation.Find('%');
    if (percent != P_MAX_INDEX) {
      PString iface = dotNotation.Mid(percent+1);
      if (!iface.IsEmpty()) {
        PIPSocket::InterfaceTable interfaceTable;
        if (PIPSocket::GetInterfaceTable(interfaceTable)) {
          for (PINDEX i = 0; i < interfaceTable.GetSize(); i++) {
            if (interfaceTable[i].GetName().NumCompare(iface) == EqualTo) {
              *this = interfaceTable[i].GetAddress();
              break;
            }
          }
        }
      }
    }
  }

  return *this;
}


PString PIPSocket::Address::AsString() const
{
#if P_HAS_IPV6
  if (version == 6) {
    PString str;
    Psockaddr sa(*this, 0);
    PAssertOS(getnameinfo(sa, sa.GetSize(), str.GetPointer(1024), 1024, NULL, 0, NI_NUMERICHOST) == 0);
    PINDEX percent = str.Find('%'); // used for scoped address e.g. fe80::1%ne0, (ne0=network interface 0)
    if (percent != P_MAX_INDEX)
      str[percent] = '\0';
    str.MakeMinimumSize();
    return str;
  }
#endif
#ifdef P_VXWORKS
  char ipStorage[INET_ADDR_LEN];
  inet_ntoa_b(v.four, ipStorage);
  return ipStorage;    
#else // P_VXWORKS
  return inet_ntoa(v.four);
#endif // P_VXWORKS
}


BOOL PIPSocket::Address::FromString(const PString & dotNotation)
{
  (*this) = dotNotation;
  return IsValid();

}


PIPSocket::Address::operator PString() const
{
  return AsString();
}


PIPSocket::Address::operator in_addr() const
{
  if (version != 4)
    return inaddr_empty;

  return v.four;
}


#if P_HAS_IPV6
PIPSocket::Address::operator in6_addr() const
{
  if (version != 6)
    return any6.v.six;

  return v.six;
}
#endif


BYTE PIPSocket::Address::operator[](PINDEX idx) const
{
  PASSERTINDEX(idx);
#if P_HAS_IPV6
  if (version == 6) {
    PAssert(idx <= 15, PInvalidParameter);
    return v.six.s6_addr[idx];
  }
#endif

  PAssert(idx <= 3, PInvalidParameter);
  return ((BYTE *)&v.four)[idx];
}


ostream & operator<<(ostream & s, const PIPSocket::Address & a)
{
  return s << a.AsString();
}

istream & operator>>(istream & s, PIPSocket::Address & a)
{
/// Not IPv6 ready !!!!!!!!!!!!!
  char dot1, dot2, dot3;
  unsigned b1, b2, b3, b4;
  s >> b1;
  if (!s.fail()) {
    if (s.peek() != '.')
      a = htonl(b1);
    else {
      s >> dot1 >> b2 >> dot2 >> b3 >> dot3 >> b4;
      if (!s.fail() && dot1 == '.' && dot2 == '.' && dot3 == '.')
        a = PIPSocket::Address((BYTE)b1, (BYTE)b2, (BYTE)b3, (BYTE)b4);
    }
  }
  return s;
}


PINDEX PIPSocket::Address::GetSize() const
{
  switch (version) {
#if P_HAS_IPV6
    case 6 :
      return 16;
#endif

    case 4 :
      return 4;
  }

  return 0;
}


BOOL PIPSocket::Address::IsValid() const
{
  switch (version) {
#if P_HAS_IPV6
    case 6 :
      return memcmp(&v.six, &any6.v.six, sizeof(v.six)) != 0;
#endif

    case 4 :
      return (DWORD)*this != INADDR_ANY;
  }
  return FALSE;
}


BOOL PIPSocket::Address::IsLoopback() const
{
#if P_HAS_IPV6
  if (version == 6)
    return IN6_IS_ADDR_LOOPBACK(&v.six);
#endif
  return *this == loopback4;
}


BOOL PIPSocket::Address::IsBroadcast() const
{
#if P_HAS_IPV6
  if (version == 6) // In IPv6, no broadcast exist. Only multicast
    return FALSE;
#endif

  return *this == broadcast4;
}

BOOL PIPSocket::Address::IsRFC1918() const 
{ 
#if P_HAS_IPV6
  if (version == 6) {
    if (IN6_IS_ADDR_LINKLOCAL(&v.six) || IN6_IS_ADDR_SITELOCAL(&v.six))
      return TRUE;
    if (IsV4Mapped())
      return PIPSocket::Address((*this)[12], (*this)[13], (*this)[14], (*this)[15]).IsRFC1918();
  }
#endif
  return (Byte1() == 10)
          ||
          (
            (Byte1() == 172)
            &&
            (Byte2() >= 16) && (Byte2() <= 31)
          )
          ||
          (
            (Byte1() == 192) 
            &&
            (Byte2() == 168)
          );
}

PIPSocket::InterfaceEntry::InterfaceEntry()
  : ipAddr(GetDefaultIpAny())
  , netMask(GetDefaultIpAny())
{
}

PIPSocket::InterfaceEntry::InterfaceEntry(const PString & _name,
                                          const Address & _addr,
                                          const Address & _mask,
                                          const PString & _macAddr
#if P_HAS_IPV6
                                         ,const PString & _ip6Addr
#endif
                                         )
  : name(_name.Trim()),
    ipAddr(_addr),
    netMask(_mask),
    macAddr(_macAddr)
#if P_HAS_IPV6
    , ip6Addr(_ip6Addr)
#endif
{
}


void PIPSocket::InterfaceEntry::PrintOn(ostream & strm) const
{
  strm << ipAddr;
#if P_HAS_IPV6
  if (!ip6Addr)
    strm << " [" << ip6Addr << ']';
#endif
  if (!macAddr)
    strm << " <" << macAddr << '>';
  if (!name)
    strm << " (" << name << ')';
}


#ifdef __NUCLEUS_NET__
BOOL PIPSocket::GetInterfaceTable(InterfaceTable & table)
{
    InterfaceEntry *IE;
    list<IPInterface>::iterator i;
    for(i=Route4Configuration->Getm_IPInterfaceList().begin();
            i!=Route4Configuration->Getm_IPInterfaceList().end();
            i++)
    {
        char ma[6];
        for(int j=0; j<6; j++) ma[j]=(*i).Getm_macaddr(j);
        IE = new InterfaceEntry((*i).Getm_name().c_str(), (*i).Getm_ipaddr(), ma );
        if(!IE) return false;
        table.Append(IE);
    }
    return true;
}
#endif

BOOL PIPSocket::GetNetworkInterface(PIPSocket::Address & addr)
{
  PIPSocket::InterfaceTable interfaceTable;
  if (PIPSocket::GetInterfaceTable(interfaceTable)) {
    PINDEX i;
    for (i = 0; i < interfaceTable.GetSize(); ++i) {
      PIPSocket::Address localAddr = interfaceTable[i].GetAddress();
      if (!localAddr.IsLoopback() && (!localAddr.IsRFC1918() || !addr.IsRFC1918()))
        addr = localAddr;
    }
  }
  return addr.IsValid();
}

//////////////////////////////////////////////////////////////////////////////
// PTCPSocket

PTCPSocket::PTCPSocket(WORD newPort)
{
  SetPort(newPort);
}


PTCPSocket::PTCPSocket(const PString & service)
{
  SetPort(service);
}


PTCPSocket::PTCPSocket(const PString & address, WORD newPort)
{
  SetPort(newPort);
  Connect(address);
}


PTCPSocket::PTCPSocket(const PString & address, const PString & service)
{
  SetPort(service);
  Connect(address);
}


PTCPSocket::PTCPSocket(PSocket & socket)
{
  Accept(socket);
}


PTCPSocket::PTCPSocket(PTCPSocket & tcpSocket)
{
  Accept(tcpSocket);
}


PObject * PTCPSocket::Clone() const
{
  return new PTCPSocket(port);
}


// By default IPv4 only adresses
BOOL PTCPSocket::OpenSocket()
{
  return ConvertOSError(os_handle = os_socket(AF_INET, SOCK_STREAM, 0));
}


// ipAdressFamily should be AF_INET or AF_INET6
BOOL PTCPSocket::OpenSocket(int ipAdressFamily) 
{
  return ConvertOSError(os_handle = os_socket(ipAdressFamily, SOCK_STREAM, 0));
}


const char * PTCPSocket::GetProtocolName() const
{
  return "tcp";
}


BOOL PTCPSocket::Write(const void * buf, PINDEX len)
{
  flush();
  PINDEX writeCount = 0;

  while (len > 0) {
    if (!os_sendto(((char *)buf)+writeCount, len, 0, NULL, 0))
      return FALSE;
    writeCount += lastWriteCount;
    len -= lastWriteCount;
  }

  lastWriteCount = writeCount;
  return TRUE;
}


BOOL PTCPSocket::Listen(unsigned queueSize, WORD newPort, Reusability reuse)
{
#if P_HAS_IPV6
  return Listen(GetDefaultIpAny(), queueSize, newPort, reuse);
#else
  return Listen(INADDR_ANY, queueSize, newPort, reuse);
#endif
}


BOOL PTCPSocket::Listen(const Address & bindAddr,
                        unsigned queueSize,
                        WORD newPort,
                        Reusability reuse)
{
  if (PIPSocket::Listen(bindAddr, queueSize, newPort, reuse) &&
      ConvertOSError(::listen(os_handle, queueSize)))
    return TRUE;

  os_close();
  return FALSE;
}


BOOL PTCPSocket::Accept(PSocket & socket)
{
  PAssert(PIsDescendant(&socket, PIPSocket), "Invalid listener socket");

#if P_HAS_IPV6

  Psockaddr sa;
  PINDEX size = sa.GetSize();
  if (!os_accept(socket, sa, &size))
    return FALSE;
    
#else

  sockaddr_in address;
  address.sin_family = AF_INET;
  PINDEX size = sizeof(address);
  if (!os_accept(socket, (struct sockaddr *)&address, &size))
    return FALSE;

#endif

  port = ((PIPSocket &)socket).GetPort();
  
  return TRUE;
}


BOOL PTCPSocket::WriteOutOfBand(void const * buf, PINDEX len)
{
#ifdef __NUCLEUS_NET__
  PAssertAlways("WriteOutOfBand unavailable on Nucleus Plus");
  //int count = NU_Send(os_handle, (char *)buf, len, 0);
  int count = ::send(os_handle, (const char *)buf, len, 0);
#elif defined(P_VXWORKS)
  int count = ::send(os_handle, (char *)buf, len, MSG_OOB);
#else
  int count = ::send(os_handle, (const char *)buf, len, MSG_OOB);
#endif
  if (count < 0) {
    lastWriteCount = 0;
    return ConvertOSError(count, LastWriteError);
  }
  else {
    lastWriteCount = count;
    return TRUE;
  }
}


void PTCPSocket::OnOutOfBand(const void *, PINDEX)
{
}


//////////////////////////////////////////////////////////////////////////////
// PIPDatagramSocket

PIPDatagramSocket::PIPDatagramSocket()
{
}


BOOL PIPDatagramSocket::ReadFrom(void * buf, PINDEX len,
                                 Address & addr, WORD & port)
{
  lastReadCount = 0;

#if P_HAS_IPV6

  Psockaddr sa;
  PINDEX size = sa.GetSize();
  if (os_recvfrom(buf, len, 0, sa, &size)) {
    addr = sa.GetIP();
    port = sa.GetPort();
  }

#else

  sockaddr_in sockAddr;
  PINDEX addrLen = sizeof(sockAddr);
  if (os_recvfrom(buf, len, 0, (struct sockaddr *)&sockAddr, &addrLen)) {
    addr = sockAddr.sin_addr;
    port = ntohs(sockAddr.sin_port);
  }

#endif

  return lastReadCount > 0;
}


BOOL PIPDatagramSocket::WriteTo(const void * buf, PINDEX len,
                                const Address & addr, WORD port)
{
  lastWriteCount = 0;

  BOOL broadcast = addr.IsAny() || addr.IsBroadcast();
  if (broadcast) {
#ifdef __BEOS__
    PAssertAlways("Broadcast option under BeOS is not implemented yet");
    return FALSE;
#else
    if (!SetOption(SO_BROADCAST, 1))
      return FALSE;
#endif
  }

#if P_HAS_IPV6

  Psockaddr sa(broadcast ? Address::GetBroadcast() : addr, port);
  BOOL ok = os_sendto(buf, len, 0, sa, sa.GetSize());

#else

  sockaddr_in sockAddr;
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_addr = (broadcast ? Address::GetBroadcast() : addr);
  sockAddr.sin_port = htons(port);
  BOOL ok = os_sendto(buf, len, 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr));

#endif

#ifndef __BEOS__
  if (broadcast)
    SetOption(SO_BROADCAST, 0);
#endif

  return ok && lastWriteCount >= len;
}


//////////////////////////////////////////////////////////////////////////////
// PUDPSocket

PUDPSocket::PUDPSocket(WORD newPort)
{
  sendPort = 0;
  SetPort(newPort);
  OpenSocket();
}

PUDPSocket::PUDPSocket(PQoS * qos, WORD newPort)
{
  if (qos != NULL)
      qosSpec = *qos;
  sendPort = 0;
  SetPort(newPort);
  OpenSocket();
}


PUDPSocket::PUDPSocket(const PString & service, PQoS * qos)
{
  if (qos != NULL)
      qosSpec = *qos;
  sendPort = 0;
  SetPort(service);
  OpenSocket();
}


PUDPSocket::PUDPSocket(const PString & address, WORD newPort)
{
  sendPort = 0;
  SetPort(newPort);
  Connect(address);
}


PUDPSocket::PUDPSocket(const PString & address, const PString & service)
{
  sendPort = 0;
  SetPort(service);
  Connect(address);
}


BOOL PUDPSocket::ModifyQoSSpec(PQoS * qos)
{
  if (qos==NULL)
    return FALSE;

  qosSpec = *qos;
  return TRUE;
}

#if P_HAS_QOS
PQoS & PUDPSocket::GetQoSSpec()
{
  return qosSpec;
}
#endif

BOOL PUDPSocket::ApplyQoS()
{
#ifdef _WIN32_WCE
  return FALSE;   //QoS not supported
#endif

  char DSCPval = 0;
  if (qosSpec.GetDSCP() < 0 ||
      qosSpec.GetDSCP() > 63) {
    if (qosSpec.GetServiceType() == SERVICETYPE_PNOTDEFINED)
      return TRUE;
    else {
      switch (qosSpec.GetServiceType()) {
        case SERVICETYPE_GUARANTEED:
          DSCPval = PQoS::guaranteedDSCP;
          break;
        case SERVICETYPE_CONTROLLEDLOAD:
          DSCPval = PQoS::controlledLoadDSCP;
          break;
        case SERVICETYPE_BESTEFFORT:
        default:
          DSCPval = PQoS::bestEffortDSCP;
          break;
      }
    }
  }
  else
    DSCPval = (char)qosSpec.GetDSCP();

#ifdef _WIN32
#if P_HAS_QOS
  if (disableGQoS)
    return FALSE;

  BOOL usesetsockopt = FALSE;

  OSVERSIONINFO versInfo;
  ZeroMemory(&versInfo,sizeof(OSVERSIONINFO));
  versInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  if (!(GetVersionEx(&versInfo)))
    usesetsockopt = TRUE;
  else {
    if (versInfo.dwMajorVersion < 5)
      usesetsockopt = TRUE;

    if (disableGQoS)
          return FALSE;

    BOOL usesetsockopt = FALSE;

    if (versInfo.dwMajorVersion == 5 &&
        versInfo.dwMinorVersion == 0)
      usesetsockopt = TRUE;         //Windows 2000 does not always support QOS_DESTADDR
  }

  BOOL retval = FALSE;
  if (!usesetsockopt && sendAddress.IsValid() && sendPort != 0) {
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(sendPort);
    sa.sin_addr = sendAddress;
    memset(sa.sin_zero,0,8);

    char * inBuf = new char[2048];
    memset(inBuf,0,2048);
    DWORD bufLen = 0;
    PWinQoS wqos(qosSpec, (struct sockaddr *)(&sa), inBuf, bufLen);

    DWORD dummy = 0;
    int irval = WSAIoctl(os_handle, SIO_SET_QOS, inBuf, bufLen, NULL, 0, &dummy, NULL, NULL);

    delete[] inBuf;

    return irval == 0;
  }

  if (!usesetsockopt)
    return retval;

#endif  // P_HAS_QOS
#endif  // _WIN32

  unsigned int setDSCP = DSCPval<<2;

  int rv = 0;
  unsigned int curval = 0;
  socklen_t cursize = sizeof(curval);
  rv = ::getsockopt(os_handle,IPPROTO_IP, IP_TOS, (char *)(&curval), &cursize);
  if (curval == setDSCP)
    return TRUE;    //Required DSCP already set


  rv = ::setsockopt(os_handle, IPPROTO_IP, IP_TOS, (char *)&setDSCP, sizeof(setDSCP));

  if (rv != 0) {
    int err;
#ifdef _WIN32
    err = WSAGetLastError();
#else
    err = errno;
#endif
    PTRACE(1,"QOS\tsetsockopt failed with code " << err);
    return FALSE;
  }
    
  return TRUE;
}

BOOL PUDPSocket::OpenSocketGQOS(int af, int type, int proto)
{
#ifdef _WIN32_WCE   //QOS not supported
  return ConvertOSError(os_handle = os_socket(af, type, proto));
#endif

#if defined(_WIN32) && defined(P_HAS_QOS)
    
  DWORD bufferSize = 0;
  DWORD numProtocols, i;
  LPWSAPROTOCOL_INFO installedProtocols, qosProtocol;

  //Try to find a QOS-enabled protocol
 
  BOOL retval = ConvertOSError(numProtocols = WSAEnumProtocols(((proto==0) ? NULL : &proto),
                                                            NULL,
                                                            &bufferSize));
    
  if (numProtocols == SOCKET_ERROR && WSAGetLastError()!=WSAENOBUFS) 
    return retval;

  installedProtocols = (LPWSAPROTOCOL_INFO)(new BYTE[bufferSize]);
  retval = ConvertOSError(numProtocols = WSAEnumProtocols(((proto==0) ? NULL : &proto),
                                                            installedProtocols,
                                                            &bufferSize));
  if (numProtocols == SOCKET_ERROR) {
    delete[] installedProtocols;
    return retval;
  }

  qosProtocol = installedProtocols;
  BOOL haveQoSproto = FALSE;

  for (i=0; i<numProtocols; qosProtocol++, i++) {
    if ((qosProtocol->dwServiceFlags1 & XP1_QOS_SUPPORTED) &&
        (qosProtocol->iSocketType == type) &&
        (qosProtocol->iAddressFamily == af)) {
      haveQoSproto = TRUE;
      break;
    }
  }

  if (haveQoSproto) {
    retval =  ConvertOSError(os_handle = WSASocket(af,
                                                   type,
                                                   proto,
                                                   qosProtocol,
                                                   0,
                                                   WSA_FLAG_OVERLAPPED));
  }
  else
  {    
    retval = ConvertOSError(os_handle = WSASocket (af,
                                                   type,
                                                   proto,
                                                   NULL,
                                                   0,
                                                   WSA_FLAG_OVERLAPPED));
  }

  delete[] installedProtocols;

  if (os_handle == INVALID_SOCKET)
    return retval;
#else
  BOOL retval = ConvertOSError(os_handle = os_socket(af, type, proto));
#endif

  return retval;
}

#ifdef _WIN32
#ifndef _WIN32_WCE
#ifdef P_HAS_QOS

#define COULD_HAVE_QOS

static BOOL CheckOSVersion()
{
    OSVERSIONINFO versInfo;
    ZeroMemory(&versInfo,sizeof(OSVERSIONINFO));
    versInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (GetVersionEx(&versInfo))
    {
        if (versInfo.dwMajorVersion > 5 ||
           (versInfo.dwMajorVersion == 5 &&
            versInfo.dwMinorVersion > 0))
          return TRUE;
    }
  return FALSE;
}

#endif
#endif
#endif

BOOL PUDPSocket::OpenSocket()
{
#ifdef COULD_HAVE_QOS
  if (CheckOSVersion()) 
    return OpenSocketGQOS(AF_INET, SOCK_DGRAM, 0);
#endif

  return ConvertOSError(os_handle = os_socket(AF_INET,SOCK_DGRAM, 0));
}

BOOL PUDPSocket::OpenSocket(int ipAdressFamily)
{
#ifdef COULD_HAVE_QOS
  if (CheckOSVersion()) 
    return OpenSocketGQOS(ipAdressFamily, SOCK_DGRAM, 0);
#endif

  return ConvertOSError(os_handle = os_socket(ipAdressFamily,SOCK_DGRAM, 0));
}

const char * PUDPSocket::GetProtocolName() const
{
  return "udp";
}


BOOL PUDPSocket::Connect(const PString & address)
{
  sendPort = 0;
  return PIPDatagramSocket::Connect(address);
}


BOOL PUDPSocket::Read(void * buf, PINDEX len)
{
  return PIPDatagramSocket::ReadFrom(buf, len, lastReceiveAddress, lastReceivePort);
}


BOOL PUDPSocket::Write(const void * buf, PINDEX len)
{
  if (sendPort == 0)
    return PIPDatagramSocket::Write(buf, len);
  else
    return PIPDatagramSocket::WriteTo(buf, len, sendAddress, sendPort);
}


void PUDPSocket::SetSendAddress(const Address & newAddress, WORD newPort)
{
  sendAddress = newAddress;
  sendPort    = newPort;
  ApplyQoS();
}


void PUDPSocket::GetSendAddress(Address & address, WORD & port)
{
  address = sendAddress;
  port    = sendPort;
}


void PUDPSocket::GetLastReceiveAddress(Address & address, WORD & port)
{
  address = lastReceiveAddress;
  port    = lastReceivePort;
}

//////////////////////////////////////////////////////////////////////////////

BOOL PICMPSocket::OpenSocket(int)
{
  return FALSE;
}

//////////////////////////////////////////////////////////////////////////////

BOOL PIPSocketAddressAndPort::Parse(const PString & str, WORD defaultPort, char _sep)
{
  sep = _sep;
  PINDEX pos = str.Find(sep);
  if (pos != P_MAX_INDEX) {
    port    = (WORD)str.Mid(pos+1).AsInteger();
    if (!PIPSocket::GetHostAddress(str.Left(pos), address))
      return FALSE;
  }
  else if (port == 0)
    return FALSE;
  else {
    port = defaultPort;
    if (!PIPSocket::GetHostAddress(str, address))
      return FALSE;
  }
  return TRUE;
}


// End Of File ///////////////////////////////////////////////////////////////
