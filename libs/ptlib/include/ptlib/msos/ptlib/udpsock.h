/*
 * udpsock.h
 *
 * User Datagram Protocol socket I/O channel class
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
 * $Log: udpsock.h,v $
 * Revision 1.8  2005/11/21 11:49:35  shorne
 * Changed disableQos to disableGQoS to better reflect what it does
 *
 * Revision 1.7  2005/07/13 12:08:09  csoutheren
 * Fixed QoS patches to be more consistent with PWLib style and to allow Unix compatibility
 *
 * Revision 1.6  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.5  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.4  1998/11/30 02:55:45  robertj
 * New directory structure
 *
 * Revision 1.3  1998/09/24 03:30:38  robertj
 * Added open software license.
 *
 * Revision 1.2  1996/08/08 10:09:22  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.1  1994/07/27 06:00:10  robertj
 * Initial revision
 *
 */

///////////////////////////////////////////////////////////////////////////////
// PUDPSocket
  protected:
    static BOOL disableGQoS;

// End Of File ///////////////////////////////////////////////////////////////
