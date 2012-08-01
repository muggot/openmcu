/*
 * socket.h
 *
 * Berkley sockets ancestor class.
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
 * $Log: socket.h,v $
 * Revision 1.19  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.18  2003/02/20 23:32:00  robertj
 * More RTEMS support patches, thanks Sebastian Meyer.
 *
 * Revision 1.17  2002/10/17 12:57:24  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.16  2002/10/17 09:50:10  craigs
 * Added typedef for SOCKET to allow compilation on Linux
 *
 * Revision 1.15  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.14  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.13  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.12  1998/11/30 22:07:11  robertj
 * New directory structure.
 *
 * Revision 1.11  1998/09/24 04:11:56  robertj
 * Added open software license.
 *
 * Revision 1.10  1996/11/03 04:36:25  craigs
 * Added Read override to avoid problem with recv/read
 *
 * Revision 1.9  1996/08/09 12:16:09  craigs
 * *** empty log message ***
 *
 * Revision 1.8  1996/08/03 12:09:51  craigs
 * Changed for new common directories
 *
 * Revision 1.7  1996/05/03 13:12:07  craigs
 * More Sun4 fixes
 *
 * Revision 1.6  1996/05/02 13:34:22  craigs
 * More Sun4 fixes
 *
 * Revision 1.5  1996/05/02 12:28:03  craigs
 * More Sun4 fixes
 *
 * Revision 1.4  1996/01/26 11:06:31  craigs
 * Added destructor
 *
 * Revision 1.3  1995/12/08 13:15:21  craigs
 * Added new header file
 *
 * Revision 1.2  1995/01/23 22:59:51  craigs
 * Changes for HPUX and Sun 4
 *
 * Revision 1.1  1994/04/12  08:31:05  robertj
 * Initial revision
 *
 */

///////////////////////////////////////////////////////////////////////////////
// PSocket

  public:
    BOOL Read(void * ptr, PINDEX len);
    ~PSocket();

// End Of File ////////////////////////////////////////////////////////////////
