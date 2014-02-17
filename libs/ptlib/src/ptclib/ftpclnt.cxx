/*
 * ftpclnt.cxx
 *
 * FTP client class.
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
 * $Log: ftpclnt.cxx,v $
 * Revision 1.12  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.11  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.10  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.9  2000/06/21 01:14:23  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.8  2000/04/07 06:29:46  rogerh
 * Add a short term workaround for an Internal Compiler Error on MAC OS X when
 * returning certain types of PString. Submitted by Kevin Packard.
 *
 * Revision 1.7  1998/12/23 00:34:55  robertj
 * Fixed normal TCP socket support after adding SOCKS support.
 *
 * Revision 1.6  1998/12/22 10:29:42  robertj
 * Added support for SOCKS based channels.
 *
 * Revision 1.5  1998/12/18 03:48:32  robertj
 * Fixed wanring on PPC linux compile
 *
 * Revision 1.4  1998/11/30 04:50:47  robertj
 * New directory structure
 *
 * Revision 1.3  1998/09/23 06:22:00  robertj
 * Added open source copyright license.
 *
 * Revision 1.2  1997/03/28 13:06:58  robertj
 * made STAT command more robust for getting file info from weird FTP servers.
 *
 * Revision 1.1  1996/09/14 13:02:18  robertj
 * Initial revision
 *
 */

#include <ptlib.h>
#include <ptlib/sockets.h>
#include <ptclib/ftp.h>


#define new PNEW


/////////////////////////////////////////////////////////
//  FTP Client

PFTPClient::PFTPClient()
{
}


PFTPClient::~PFTPClient()
{
  Close();
}


BOOL PFTPClient::Close()
{
  if (!IsOpen())
    return FALSE;
  BOOL ok = ExecuteCommand(QUIT)/100 == 2;
  return PFTP::Close() && ok;
}

BOOL PFTPClient::OnOpen()
{
  if (!ReadResponse() || lastResponseCode != 220)
    return FALSE;

  // the default data port for a server is the adjacent port
  PIPSocket::Address remoteHost;
  PIPSocket * socket = GetSocket();
  if (socket == NULL)
    return FALSE;

  socket->GetPeerAddress(remoteHost, remotePort);
  remotePort--;
  return TRUE;
}


BOOL PFTPClient::LogIn(const PString & username, const PString & password)
{
  if (ExecuteCommand(USER, username)/100 != 3)
    return FALSE;
  return ExecuteCommand(PASS, password)/100 == 2;
}


PString PFTPClient::GetSystemType()
{
  if (ExecuteCommand(SYST)/100 != 2)
    return PString();

  return lastResponseInfo.Left(lastResponseInfo.Find(' '));
}


BOOL PFTPClient::SetType(RepresentationType type)
{
  static const char * const typeCode[] = { "A", "E", "I" };
  PAssert((PINDEX)type < PARRAYSIZE(typeCode), PInvalidParameter);
  return ExecuteCommand(TYPE, typeCode[type])/100 == 2;
}


BOOL PFTPClient::ChangeDirectory(const PString & dirPath)
{
  return ExecuteCommand(CWD, dirPath)/100 == 2;
}


PString PFTPClient::GetCurrentDirectory()
{
  if (ExecuteCommand(PWD) != 257)
    return PString();

  PINDEX quote1 = lastResponseInfo.Find('"');
  if (quote1 == P_MAX_INDEX)
    return PString();

  PINDEX quote2 = quote1 + 1;
  do {
    quote2 = lastResponseInfo.Find('"', quote2);
    if (quote2 == P_MAX_INDEX)
      return PString();

    while (lastResponseInfo[quote2]=='"' && lastResponseInfo[quote2+1]=='"')
      quote2 += 2;

  } while (lastResponseInfo[quote2] != '"');

  // make Apple's and Tornado's gnu compiler happy
  PString retval = lastResponseInfo(quote1+1, quote2-1);
  return retval;
}


PStringArray PFTPClient::GetDirectoryNames(NameTypes type,
                                           DataChannelType ctype)
{
  return GetDirectoryNames(PString(), type, ctype);
}


PStringArray PFTPClient::GetDirectoryNames(const PString & path,
                                          NameTypes type,
                                          DataChannelType ctype)
{
  SetType(PFTP::ASCII);

  Commands lcmd = type == DetailedNames ? LIST : NLST;
  PTCPSocket * socket = ctype != Passive ? NormalClientTransfer(lcmd, path)
                                         : PassiveClientTransfer(lcmd, path);
  if (socket == NULL)
    return PStringArray();

  PString response = lastResponseInfo;
  PString str;
  int count = 0;
  while(socket->Read(str.GetPointer(count+1000)+count, 1000))
    count += socket->GetLastReadCount();
  str.SetSize(count+1);

  delete socket;
  ReadResponse();
  lastResponseInfo = response + '\n' + lastResponseInfo;
  return str.Lines();
}


PString PFTPClient::GetFileStatus(const PString & path, DataChannelType ctype)
{
  if (ExecuteCommand(STATcmd, path)/100 == 2 && lastResponseInfo.Find(path) != P_MAX_INDEX) {
    PINDEX start = lastResponseInfo.Find('\n');
    if (start != P_MAX_INDEX) {
      PINDEX end = lastResponseInfo.Find('\n', ++start);
      if (end != P_MAX_INDEX)
        return lastResponseInfo(start, end-1);
    }
  }

  PTCPSocket * socket = ctype != Passive ? NormalClientTransfer(LIST, path)
                                         : PassiveClientTransfer(LIST, path);
  if (socket == NULL)
    return PString();

  PString str;
  socket->Read(str.GetPointer(200), 199);
  str[socket->GetLastReadCount()] = '\0';
  delete socket;
  ReadResponse();

  PINDEX end = str.FindOneOf("\r\n");
  if (end != P_MAX_INDEX)
    str[end] = '\0';
  return str;
}


PTCPSocket * PFTPClient::NormalClientTransfer(Commands cmd,
                                              const PString & args)
{
  PIPSocket * socket = GetSocket();
  if (socket == NULL)
    return NULL;

  // setup a socket so we can tell the host where to connect to
  PTCPSocket * listenSocket = (PTCPSocket *)socket->Clone();
  listenSocket->SetPort(0);  // Want new random port number
  listenSocket->Listen();

  // The following is just used to automatically delete listenSocket
  PIndirectChannel autoDeleteSocket;
  autoDeleteSocket.Open(listenSocket);

  // get host address and port to send to other end
  WORD localPort = listenSocket->GetPort();
  PIPSocket::Address localAddr;
  socket->GetLocalAddress(localAddr);

  // send PORT command to host
  if (!SendPORT(localAddr, localPort))
    return NULL;

  if (ExecuteCommand(cmd, args)/100 != 1)
    return NULL;

  PTCPSocket * dataSocket = (PTCPSocket *)socket->Clone();
  if (dataSocket->Accept(*listenSocket))
    return dataSocket;

  delete dataSocket;
  return NULL;
}


PTCPSocket * PFTPClient::PassiveClientTransfer(Commands cmd,
                                               const PString & args)
{
  PIPSocket::Address passiveAddress;
  WORD passivePort;

  if (ExecuteCommand(PASV) != 227)
    return NULL;

  PINDEX start = lastResponseInfo.FindOneOf("0123456789");
  if (start == P_MAX_INDEX)
    return NULL;

  PStringArray bytes = lastResponseInfo(start, P_MAX_INDEX).Tokenise(',');
  if (bytes.GetSize() != 6)
    return NULL;

  passiveAddress = PIPSocket::Address((BYTE)bytes[0].AsInteger(),
                                      (BYTE)bytes[1].AsInteger(),
                                      (BYTE)bytes[2].AsInteger(),
                                      (BYTE)bytes[3].AsInteger());
  passivePort = (WORD)(bytes[4].AsInteger()*256 + bytes[5].AsInteger());

  PTCPSocket * socket = new PTCPSocket(passiveAddress, passivePort);
  if (socket->IsOpen())
    if (ExecuteCommand(cmd, args)/100 == 1)
      return socket;

  delete socket;
  return NULL;
}


PTCPSocket * PFTPClient::GetFile(const PString & filename,
                                 DataChannelType channel)
{
  return channel != Passive ? NormalClientTransfer(RETR, filename)
                            : PassiveClientTransfer(RETR, filename);
}


PTCPSocket * PFTPClient::PutFile(const PString & filename,
                                 DataChannelType channel)
{
  return channel != Passive ? NormalClientTransfer(STOR, filename)
                            : PassiveClientTransfer(STOR, filename);
}



// End of File ///////////////////////////////////////////////////////////////
