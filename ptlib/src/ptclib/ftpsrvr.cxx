/*
 * ftpsrvr.cxx
 *
 * FTP server class.
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
 * $Log: ftpsrvr.cxx,v $
 * Revision 1.10  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.9  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.8  2000/06/21 01:10:18  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.7  2000/06/21 01:01:22  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.6  1999/02/16 06:04:41  robertj
 * Fixed bug in FTP server for PASV mode, may return incorrect IP address.
 *
 * Revision 1.5  1998/11/30 04:50:48  robertj
 * New directory structure
 *
 * Revision 1.4  1998/10/13 14:06:21  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.3  1998/09/23 06:22:02  robertj
 * Added open source copyright license.
 *
 * Revision 1.2  1996/10/26 01:39:49  robertj
 * Added check for security breach using 3 way FTP transfer or use of privileged PORT.
 *
 * Revision 1.1  1996/09/14 13:02:35  robertj
 * Initial revision
 *
 */

#include <ptlib.h>
#include <ptlib/sockets.h>
#include <ptclib/ftp.h>

#define new PNEW


#define READY_STRING  "PWLib FTP Server v1.0 ready"
#define GOOBYE_STRING "Goodbye"


/////////////////////////////////////////////////////////
//  FTPServer

PFTPServer::PFTPServer()
  : readyString(PIPSocket::GetHostName() & READY_STRING)
{
  Construct();
}


PFTPServer::PFTPServer(const PString & readyStr)
  : readyString(readyStr)
{
  Construct();
}


void PFTPServer::Construct()
{
  thirdPartyPort = FALSE;
  illegalPasswordCount = 0;
  state     = NotConnected;
  type      = 'A';
  structure = 'F';
  mode      = 'S';
  passiveSocket = NULL;
}


PFTPServer::~PFTPServer()
{
  delete passiveSocket;
}


BOOL PFTPServer::OnOpen()
{
  // the default data port for a client is the same port
  PIPSocket * socket = GetSocket();
  if (socket == NULL)
    return FALSE;

  state = NeedUser;
  if (!WriteResponse(220, readyString))
    return FALSE;

  socket->GetPeerAddress(remoteHost, remotePort);
  return TRUE;
}


PString PFTPServer::GetHelloString(const PString & user) const
{
  return PString("User") & user & "logged in.";
}


PString PFTPServer::GetGoodbyeString(const PString &) const
{
  return PString(GOOBYE_STRING);
}


PString PFTPServer::GetSystemTypeString() const
{
  return PProcess::GetOSClass() + " " + PProcess::GetOSName() + " " + PProcess::GetOSVersion();
}


BOOL PFTPServer::AuthoriseUser(const PString &, const PString &, BOOL &)
{
  return TRUE;
}


BOOL PFTPServer::ProcessCommand()
{
  PString args;
  PINDEX code;
  if (!ReadCommand(code, args))
    return FALSE;

  if (code == P_MAX_INDEX)
    return OnUnknown(args);

  //  handle commands that require login
  if (state == Connected || !CheckLoginRequired(code)) 
    return DispatchCommand(code, args);
  
  // otherwise enforce login
  WriteResponse(530, "Please login with USER and PASS.");
  return TRUE;
}


BOOL PFTPServer::DispatchCommand(PINDEX code, const PString & args)
{
  switch (code) {

    // mandatory commands
    case USER:
      return OnUSER(args);
    case PASS:
      return OnPASS(args);
    case QUIT:
      return OnQUIT(args);
    case PORT:
      return OnPORT(args);
    case STRU:
      return OnSTRU(args);
    case MODE:
      return OnMODE(args);
    case NOOP:
      return OnNOOP(args);
    case TYPE:
      return OnTYPE(args);
    case RETR:
      return OnRETR(args);
    case STOR:
      return OnSTOR(args);
    case SYST:
      return OnSYST(args);
    case STATcmd:
      return OnSTAT(args);
    case ACCT:
      return OnACCT(args);
    case CWD:
      return OnCWD(args);
    case CDUP:
      return OnCDUP(args);
    case PASV:
      return OnPASV(args);
    case APPE:
      return OnAPPE(args);
    case RNFR:
      return OnRNFR(args);
    case RNTO:
      return OnRNTO(args);
    case DELE:
      return OnDELE(args);
    case RMD:
      return OnRMD(args);
    case MKD:
      return OnMKD(args);
    case PWD:
      return OnPWD(args);
    case LIST:
      return OnLIST(args);
    case NLST:
      return OnNLST(args);

    // optional commands
    case HELP:
      return OnHELP(args);
    case SITE:
      return OnSITE(args);
    case ABOR:
      return OnABOR(args);
    case SMNT:
      return OnSMNT(args);
    case REIN:
      return OnREIN(args);
    case STOU:
      return OnSTOU(args);
    case ALLO:
      return OnALLO(args);
    case REST:
      return OnREST(args);
    default:
      PAssertAlways("Registered FTP command not handled");
      return FALSE;
  }
  return TRUE;
}


BOOL PFTPServer::CheckLoginRequired(PINDEX cmd)
{
  static const BYTE RequiresLogin[NumCommands] = {
    1, // USER
    1, // PASS
    0, // ACCT
    0, // CWD
    0, // CDUP
    0, // SMNT
    1, // QUIT
    0, // REIN
    1, // PORT
    0, // PASV
    1, // TYPE
    1, // STRU
    1, // MODE
    0, // RETR
    0, // STOR
    0, // STOU
    0, // APPE
    0, // ALLO
    0, // REST
    0, // RNFR
    0, // RNTO
    1, // ABOR
    0, // DELE
    0, // RMD
    0, // MKD
    0, // PWD
    0, // LIST
    0, // NLST
    1, // SITE
    1, // SYST
    1, // STAT
    1, // HELP
    1, // NOOP
  };
  if (cmd < NumCommands)
    return RequiresLogin[cmd] == 0;
  else
    return TRUE;
}


BOOL PFTPServer::OnUnknown(const PCaselessString & command)
{
  WriteResponse(500, "\"" + command + "\" command unrecognised.");
  return TRUE;
}


void PFTPServer::OnError(PINDEX errorCode, PINDEX cmdNum, const char * msg)
{
  if (cmdNum < commandNames.GetSize())
    WriteResponse(errorCode, "Command \"" + commandNames[cmdNum] + "\":" + msg);
  else
    WriteResponse(errorCode, msg);
}


void PFTPServer::OnNotImplemented(PINDEX cmdNum)
{
  OnError(502, cmdNum, "not implemented");
}


void PFTPServer::OnSyntaxError(PINDEX cmdNum)
{
  OnError(501, cmdNum, "syntax error in parameters or arguments.");
}


void PFTPServer::OnCommandSuccessful(PINDEX cmdNum)
{
  if (cmdNum < commandNames.GetSize())
    WriteResponse(200, "\"" + commandNames[cmdNum] + "\" command successful.");
}


// mandatory commands that can be performed without loggin in

BOOL PFTPServer::OnUSER(const PCaselessString & args)
{
  userName = args;
  state    = NeedPassword;
  WriteResponse(331, "Password required for " + args + ".");
  return TRUE;
}


BOOL PFTPServer::OnPASS(const PCaselessString & args)
{
  BOOL replied = FALSE;
  if (state != NeedPassword) 
    WriteResponse(503, "Login with USER first.");
  else if (!AuthoriseUser(userName, args, replied)) {
    if (!replied)
      WriteResponse(530, "Login incorrect.");
    if (illegalPasswordCount++ == MaxIllegalPasswords)
      return FALSE;
  } else {
    if (!replied)
      WriteResponse(230, GetHelloString(userName));
    illegalPasswordCount = 0;
    state = Connected;
  }
  return TRUE;
}


BOOL PFTPServer::OnQUIT(const PCaselessString & userName)
{
  WriteResponse(221, GetGoodbyeString(userName));
  return FALSE;
}


BOOL PFTPServer::OnPORT(const PCaselessString & args)
{
  PStringArray tokens = args.Tokenise(",");

  long values[6];
  PINDEX len = PMIN(args.GetSize(), 6);

  PINDEX i;
  for (i = 0; i < len; i++) {
    values[i] = tokens[i].AsInteger();
    if (values[i] < 0 || values[i] > 255)
      break;
  }
  if (i < 6) 
    OnSyntaxError(PORT);
  else {
    PIPSocket * socket = GetSocket();
    if (socket == NULL)
      OnError(590, PORT, "not available on non-TCP transport.");
    else {
      remoteHost = PIPSocket::Address((BYTE)values[0],
                              (BYTE)values[1], (BYTE)values[2], (BYTE)values[3]);
      remotePort = (WORD)(values[4]*256 + values[5]);
      if (remotePort < 1024 && remotePort != socket->GetPort()-1)
        OnError(590, PORT, "cannot access privileged port number.");
      else {
        PIPSocket::Address controlHost;
        GetSocket()->GetPeerAddress(controlHost);
        if (thirdPartyPort || remoteHost == controlHost)
          OnCommandSuccessful(PORT);
        else
          OnError(591, PORT, "three way transfer not allowed.");
      }
    }
  }
  return TRUE;
}


BOOL PFTPServer::OnPASV(const PCaselessString &)
{
  if (passiveSocket != NULL)
    delete passiveSocket;

  passiveSocket = new PTCPSocket;
  passiveSocket->Listen();

  WORD portNo = passiveSocket->GetPort();
  PIPSocket::Address ourAddr;
  PIPSocket * socket = GetSocket();
  if (socket != NULL)
    socket->GetLocalAddress(ourAddr);
  PString str(PString::Printf,
              "Entering Passive Mode (%i,%i,%i,%i,%i,%i)",
              ourAddr.Byte1(),
              ourAddr.Byte2(),
              ourAddr.Byte3(),
              ourAddr.Byte4(),
              portNo/256, portNo%256);

  return WriteResponse(227, str);
}


BOOL PFTPServer::OnTYPE(const PCaselessString & args)
{
  if (args.IsEmpty())
    OnSyntaxError(TYPE);
  else {
    switch (toupper(args[0])) {
      case 'A':
        type = 'A';
        break;
      case 'I':
        type = 'I';
        break;
      case 'E':
      case 'L':
        WriteResponse(504, PString("TYPE not implemented for parameter ") + args);
        return TRUE;

      default:
        OnSyntaxError(TYPE);
        return TRUE;
    }
  }
  OnCommandSuccessful(TYPE);
  return TRUE;
}


BOOL PFTPServer::OnMODE(const PCaselessString & args)
{
  if (args.IsEmpty())
    OnSyntaxError(MODE);
  else {
    switch (toupper(args[0])) {
      case 'S':
        structure = 'S';
        break;
      case 'B':
      case 'C':
        WriteResponse(504, PString("MODE not implemented for parameter ") + args);
        return TRUE;
      default:
        OnSyntaxError(MODE);
        return TRUE;
    }
  }
  OnCommandSuccessful(MODE);
  return TRUE;
}


BOOL PFTPServer::OnSTRU(const PCaselessString & args)
{
  if (args.IsEmpty())
    OnSyntaxError(STRU);
  else {
    switch (toupper(args[0])) {
      case 'F':
        structure = 'F';
        break;
      case 'R':
      case 'P':
        WriteResponse(504, PString("STRU not implemented for parameter ") + args);
        return TRUE;
      default:
        OnSyntaxError(STRU);
        return TRUE;
    }
  }
  OnCommandSuccessful(STRU);
  return TRUE;
}


BOOL PFTPServer::OnNOOP(const PCaselessString &)
{
  OnCommandSuccessful(NOOP);
  return TRUE;
}


// mandatory commands that cannot be performed without logging in

BOOL PFTPServer::OnRETR(const PCaselessString &)
{
  OnNotImplemented(RETR);
  return TRUE;
}


BOOL PFTPServer::OnSTOR(const PCaselessString &)
{
  OnNotImplemented(STOR);
  return TRUE;
}


BOOL PFTPServer::OnACCT(const PCaselessString &)
{
  WriteResponse(532, "Need account for storing files");
  return TRUE;
}


BOOL PFTPServer::OnCWD(const PCaselessString &)
{
  OnNotImplemented(CWD);
  return TRUE;
}


BOOL PFTPServer::OnCDUP(const PCaselessString &)
{
  OnNotImplemented(CDUP);
  return TRUE;
}


BOOL PFTPServer::OnSMNT(const PCaselessString &)
{
  OnNotImplemented(SMNT);
  return TRUE;
}


BOOL PFTPServer::OnREIN(const PCaselessString &)
{
  OnNotImplemented(REIN);
  return TRUE;
}


BOOL PFTPServer::OnSTOU(const PCaselessString &)
{
  OnNotImplemented(STOU);
  return TRUE;
}


BOOL PFTPServer::OnAPPE(const PCaselessString &)
{
  OnNotImplemented(APPE);
  return TRUE;
}


BOOL PFTPServer::OnALLO(const PCaselessString &)
{
  OnNotImplemented(ALLO);
  return TRUE;
}


BOOL PFTPServer::OnREST(const PCaselessString &)
{
  OnNotImplemented(REST);
  return TRUE;
}


BOOL PFTPServer::OnRNFR(const PCaselessString &)
{
  OnNotImplemented(RNFR);
  return TRUE;
}


BOOL PFTPServer::OnRNTO(const PCaselessString &)
{
  OnNotImplemented(RNTO);
  return TRUE;
}


BOOL PFTPServer::OnABOR(const PCaselessString &)
{
  OnNotImplemented(ABOR);
  return TRUE;
}


BOOL PFTPServer::OnDELE(const PCaselessString &)
{
  OnNotImplemented(DELE);
  return TRUE;
}


BOOL PFTPServer::OnRMD(const PCaselessString &)
{
  OnNotImplemented(RMD);
  return TRUE;
}


BOOL PFTPServer::OnMKD(const PCaselessString &)
{
  OnNotImplemented(MKD);
  return TRUE;
}


BOOL PFTPServer::OnPWD(const PCaselessString &)
{
  OnNotImplemented(PWD);
  return TRUE;
}


BOOL PFTPServer::OnLIST(const PCaselessString &)
{
  OnNotImplemented(LIST);
  return TRUE;
}


BOOL PFTPServer::OnNLST(const PCaselessString &)
{
  OnNotImplemented(NLST);
  return TRUE;
}


BOOL PFTPServer::OnSITE(const PCaselessString &)
{
  OnNotImplemented(SITE);
  return TRUE;
}


BOOL PFTPServer::OnSYST(const PCaselessString &)
{
  WriteResponse(215, GetSystemTypeString());
  return TRUE;
}


BOOL PFTPServer::OnSTAT(const PCaselessString &)
{
  OnNotImplemented(STATcmd);
  return TRUE;
}


BOOL PFTPServer::OnHELP(const PCaselessString &)
{
  OnNotImplemented(HELP);
  return TRUE;
}


void PFTPServer::SendToClient(const PFilePath & filename)
{
  if (!PFile::Exists(filename)) 
    WriteResponse(450, filename + ": file not found");
  else {
    PTCPSocket * dataSocket;
    if (passiveSocket != NULL) {
      dataSocket = new PTCPSocket(*passiveSocket);
      delete passiveSocket;
      passiveSocket = NULL;
    } else
      dataSocket = new PTCPSocket(remoteHost, remotePort);
    if (!dataSocket->IsOpen())
      WriteResponse(425, "Cannot open data connection");
    else {
      if (type == 'A') {
        PTextFile file(filename, PFile::ReadOnly);
        if (!file.IsOpen())
          WriteResponse(450, filename + ": cannot open file");
        else {
          PString fileSize(PString::Unsigned, file.GetLength());
          WriteResponse(150, PString("Opening ASCII data connection for " + filename.GetFileName() + "(" + fileSize + " bytes)"));
          PString line;
          BOOL ok = TRUE;
          while (ok && file.ReadLine(line)) {
            if (!dataSocket->Write((const char *)line, line.GetLength())) {
              WriteResponse(426, "Connection closed - transfer aborted");
              ok = FALSE;
            }
          }
          file.Close();
        }
      } else {
        PFile file(filename, PFile::ReadOnly);
        if (!file.IsOpen())
          WriteResponse(450, filename + ": cannot open file");
        else {
          PString fileSize(PString::Unsigned, file.GetLength());
          WriteResponse(150, PString("Opening BINARY data connection for " + filename.GetFileName() + "(" + fileSize + " bytes)"));
          BYTE buffer[2048];
          BOOL ok = TRUE;
          while (ok && file.Read(buffer, 2048)) {
            if (!dataSocket->Write(buffer, file.GetLastReadCount())) {
              WriteResponse(426, "Connection closed - transfer aborted");
              ok = FALSE;
            }
          }
          file.Close();
        }
      }
      delete dataSocket;
      WriteResponse(226, "Transfer complete");
    }
  }
}


// End of File ///////////////////////////////////////////////////////////////
