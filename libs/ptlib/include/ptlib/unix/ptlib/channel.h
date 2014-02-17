/*
 * channel.h
 *
 * I/O channel ancestor class.
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
 * $Log: channel.h,v $
 * Revision 1.25  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.24  2004/04/27 04:37:50  rjongbloed
 * Fixed ability to break of a PSocket::Select call under linux when a socket
 *   is closed by another thread.
 *
 * Revision 1.23  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.22  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.21  2001/11/27 02:20:20  robertj
 * Fixed problem with a read ro write blocking until connect completed, it
 *   really should return an error as the caller is doing a bad thing.
 *
 * Revision 1.20  2001/09/10 03:03:36  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.19  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.18  2001/03/20 06:44:25  robertj
 * Lots of changes to fix the problems with terminating threads that are I/O
 *   blocked, especially when doing orderly shutdown of service via SIGTERM.
 *
 * Revision 1.17  1999/10/24 01:32:11  craigs
 * Removed definition of BREAK_SIGNAL and moved to src file
 *
 * Revision 1.16  1999/09/03 02:26:25  robertj
 * Changes to aid in breaking I/O locks on thread termination. Still needs more work esp in BSD!
 *
 * Revision 1.15  1999/01/07 03:37:12  robertj
 * dded default for pthreads, shortens command line in compile.
 *
 * Revision 1.14  1998/11/30 22:06:30  robertj
 * New directory structure.
 *
 * Revision 1.13  1998/09/24 04:11:20  robertj
 * Added open software license.
 *
 * Revision 1.12  1998/05/25 09:57:15  robertj
 * Fixed problem with socket/channel close with active thread block.
 *
 * Revision 1.11  1998/03/26 04:55:53  robertj
 * Added PMutex and PSyncPoint
 *
 * Revision 1.10  1998/01/03 22:58:25  craigs
 * Added PThread support
 *
 * Revision 1.9  1996/08/03 12:08:19  craigs
 * Changed for new common directories
 *
 * Revision 1.8  1996/05/03 13:12:07  craigs
 * More Sun4 fixes
 *
 * Revision 1.7  1996/05/02 12:01:47  craigs
 * More Sun4 fixed
 *
 * Revision 1.6  1996/05/02 11:55:28  craigs
 * Added ioctl definition for Sun4
 *
 * Revision 1.5  1996/04/15 10:50:48  craigs
 * Last revision prior to release of MibMaster
 *
 * Revision 1.4  1996/01/26 11:06:31  craigs
 * Fixed problem with blocking Accept calls
 *
 * Revision 1.3  1995/07/09 00:34:58  craigs
 * Latest and greatest omnibus change
 *
 * Revision 1.2  1995/01/23 22:59:47  craigs
 * Changes for HPUX and Sun 4
 *
 */
#ifndef __PCHANNEL_UNIX__
#define __PCHANNEL_UNIX__

  public:
    enum PXBlockType {
      PXReadBlock,
      PXWriteBlock,
      PXAcceptBlock,
      PXConnectBlock
    };

  protected:
    BOOL PXSetIOBlock(PXBlockType type, const PTimeInterval & timeout);
    int  PXClose();

    PString     channelName;
    PMutex      px_threadMutex;
    PXBlockType px_lastBlockType;
    PThread   * px_readThread;
    PThread   * px_writeThread;
    PMutex      px_writeMutex;
    PThread   * px_selectThread;
    PMutex      px_selectMutex;

#endif //#ifndef __PCHANNEL_UNIX__ 

// End Of File ////////////////////////////////////////////////////////////////
