/*
 * serchan.h
 *
 * Asynchronous serial I/O channel class.
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
 * $Log: serchan.h,v $
 * Revision 1.13  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.12  2002/11/02 00:32:21  robertj
 * Further fixes to VxWorks (Tornado) port, thanks Andreas Sikkema.
 *
 * Revision 1.11  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.10  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.9  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.8  1998/11/30 22:07:08  robertj
 * New directory structure.
 *
 * Revision 1.7  1998/09/24 04:11:53  robertj
 * Added open software license.
 *
 * Revision 1.6  1996/08/03 12:08:19  craigs
 * Changed for new common directories
 *
 * Revision 1.5  1996/05/03 13:12:07  craigs
 * More Sun4 fixes
 *
 * Revision 1.4  1996/05/02 12:01:47  craigs
 * More Sun4 fixed
 *
 * Revision 1.3  1996/05/02 11:55:01  craigs
 * Fixed problem with compiling on Sun4
 *
 * Revision 1.2  1995/12/08 13:14:52  craigs
 * Added new function
 *
 * Revision 1.1  1995/01/23 18:43:27  craigs
 * Initial revision
 *
 */

  public:
    BOOL Close();

  private:
#ifndef P_VXWORKS
    struct termios oldTermio;
    struct termios Termio;
#endif // !P_VXWORKS

    DWORD  baudRate;
    BYTE   dataBits;
    Parity parityBits;
    BYTE   stopBits;

// End Of File ////////////////////////////////////////////////////////////////
