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
 * Revision 1.23  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.22  2003/01/11 05:10:51  robertj
 * Fixed Win CE compatibility issues, thanks Joerg Schoemer
 *
 * Revision 1.21  2002/10/11 08:24:10  robertj
 * Added test for not needing tpipv6.h  with later winsock2.h, thanks Ted Szoczei
 *
 * Revision 1.20  2002/10/08 12:41:52  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.19  2002/09/23 07:17:23  robertj
 * Changes to allow winsock2 to be included.
 *
 * Revision 1.18  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.17  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.16  1999/02/16 08:10:33  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.15  1998/11/30 02:55:31  robertj
 * New directory structure
 *
 * Revision 1.14  1998/09/24 03:30:25  robertj
 * Added open software license.
 *
 * Revision 1.13  1998/08/27 02:06:42  robertj
 * GNU C library v6 compatibility
 *
 * Revision 1.12  1996/10/08 13:05:01  robertj
 * More IPX support.
 *
 * Revision 1.11  1996/09/14 13:09:46  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.10  1996/08/08 10:09:14  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.9  1996/07/27 04:08:58  robertj
 * Created static version of ConvertOSError().
 *
 * Revision 1.8  1996/03/31 09:11:40  robertj
 * Fixed major performance problem in timeout read/write to sockets.
 *
 * Revision 1.7  1995/12/10 11:49:43  robertj
 * Numerous fixes for sockets.
 *
 * Revision 1.6  1995/03/12 05:00:01  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.5  1995/01/02  12:16:22  robertj
 * Moved constructor to platform dependent code.
 *
 * Revision 1.4  1994/12/12  10:10:17  robertj
 * Changed so can compile if no winsock available.
 *
 * Revision 1.3  1994/10/30  11:24:22  robertj
 * Fixed DOS version of header.
 *
 * Revision 1.2  1994/10/23  05:36:51  robertj
 * Sockets implementation.
 *
 * Revision 1.1  1994/08/22  00:18:02  robertj
 * Initial revision
 *
 * Revision 1.1  1994/07/27  06:00:10  robertj
 * Initial revision
 *
 */

///////////////////////////////////////////////////////////////////////////////
// PSocket

  public:
    ~PSocket();
      // close a socket

    virtual BOOL Read(void * buf, PINDEX len);
    virtual BOOL Write(const void * buf, PINDEX len);
    virtual BOOL Close();

  protected:
    BOOL ConvertOSError(int status, ErrorGroup group = LastGeneralError);
    static BOOL ConvertOSError(int error, Errors & lastError, int & osError);

  private:
#ifdef PHAS_WINSOCK
    static BOOL WinSockStarted;
#endif


// End Of File ///////////////////////////////////////////////////////////////
