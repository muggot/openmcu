/*
 * ftp.cxx
 *
 * FTP ancestor class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2002 Equivalence Pty. Ltd.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: ftp.cxx,v $
 * Revision 1.14  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.13  1998/11/30 04:50:46  robertj
 * New directory structure
 *
 * Revision 1.12  1998/09/23 06:21:59  robertj
 * Added open source copyright license.
 *
 * Revision 1.11  1998/01/26 05:20:30  robertj
 * GNU Support.
 *
 * Revision 1.10  1997/07/14 11:47:09  robertj
 * Added "const" to numerous variables.
 *
 * Revision 1.9  1996/09/14 13:09:26  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.8  1996/05/30 10:04:46  robertj
 * Fixed bug in breaking accept within FTP constructor returning wrong error code.
 *
 * Revision 1.7  1996/05/26 03:46:36  robertj
 * Compatibility to GNU 2.7.x
 *
 * Revision 1.6  1996/05/23 09:56:27  robertj
 * Changed FTP so can do passive/active mode on all data transfers.
 *
 * Revision 1.5  1996/03/31 09:01:20  robertj
 * More FTP client implementation.
 *
 * Revision 1.4  1996/03/26 00:50:30  robertj
 * FTP Client Implementation.
 *
 * Revision 1.3  1996/03/18 13:33:15  robertj
 * Fixed incompatibilities to GNU compiler where PINDEX != int.
 *
 * Revision 1.2  1996/03/16 04:51:12  robertj
 * Changed lastResponseCode to an integer.
 *
 * Revision 1.1  1996/03/04 12:12:51  robertj
 * Initial revision
 *
 */

#ifdef __GNUC__
#pragma implementation "ftp.h"
#endif

#include <ptlib.h>
#include <ptlib/sockets.h>
#include <ptclib/ftp.h>


/////////////////////////////////////////////////////////
//  File Transfer Protocol

static const char * const FTPCommands[PFTP::NumCommands] = 
{
  "USER", "PASS", "ACCT", "CWD", "CDUP", "SMNT", "QUIT", "REIN", "PORT", "PASV",
  "TYPE", "STRU", "MODE", "RETR", "STOR", "STOU", "APPE", "ALLO", "REST", "RNFR",
  "RNTO", "ABOR", "DELE", "RMD", "MKD", "PWD", "LIST", "NLST", "SITE", "SYST",
  "STAT", "HELP", "NOOP"
};

PFTP::PFTP()
  : PInternetProtocol("ftp 21", NumCommands, FTPCommands)
{
}


BOOL PFTP::SendPORT(const PIPSocket::Address & addr, WORD port)
{
  PString str(PString::Printf,
              "%i,%i,%i,%i,%i,%i",
              addr.Byte1(),
              addr.Byte2(),
              addr.Byte3(),
              addr.Byte4(),
              port/256,
              port%256);
  return ExecuteCommand(PORT, str)/100 == 2;
}


// End of File ///////////////////////////////////////////////////////////////
