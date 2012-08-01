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
 * Revision 1.12  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.11  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.10  2002/10/08 14:31:17  robertj
 * Fixed GNU compiler compatibility
 *
 * Revision 1.9  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.8  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
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

#ifdef P_VXWORKS
#define NETDB_SUCCESS 0
#endif // P_VXWORKS

///////////////////////////////////////////////////////////////////////////////
// PIPSocket

// nothing to do

// End Of File ////////////////////////////////////////////////////////////////
