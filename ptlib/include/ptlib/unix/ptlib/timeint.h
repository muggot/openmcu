/*
 * timeint.h
 *
 * Time interval (64 bit milliseconds)
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
 * $Log: timeint.h,v $
 * Revision 1.12  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.11  2002/10/17 12:57:24  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.10  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.9  2001/09/18 05:56:03  robertj
 * Fixed numerous problems with thread suspend/resume and signals handling.
 *
 * Revision 1.8  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.7  1998/11/30 22:07:25  robertj
 * New directory structure.
 *
 * Revision 1.6  1998/09/24 04:12:04  robertj
 * Added open software license.
 *
 * Revision 1.5  1996/08/03 12:09:51  craigs
 * Changed for new common directories
 *
 * Revision 1.4  1996/06/10 11:03:33  craigs
 * Remove unneeded function declarations
 *
 * Revision 1.3  1995/07/09 00:35:00  craigs
 * Latest and greatest omnibus change
 *
 * Revision 1.2  1995/03/25 20:58:24  craigs
 * Removed unnecessary prefix on declaration of PTimerInterval::operator =
 *
 * Revision 1.1  1995/01/23  18:43:27  craigs
 * Initial revision
 *
 * Revision 1.1  1994/04/12  08:31:05  robertj
 * Initial revision
 *
 */

#include <time.h>

#define PMaxTimeInterval PTimeInterval((long)0x7fffffff)

///////////////////////////////////////////////////////////////////////////////
// PTimeInterval

// nothing to do

// End Of File ////////////////////////////////////////////////////////////////
