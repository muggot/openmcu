/*
 * inetmail.cxx
 *
 * Internet Mail classes.
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
 * Contributor(s): Federico Pinna and Reitek S.p.A. (SASL authentication)
 *
 * $Log: inetmail.cxx,v $
 * Revision 1.32  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.31  2007/04/10 05:08:48  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.30  2004/05/09 07:23:49  rjongbloed
 * More work on XMPP, thanks Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.29  2004/05/02 08:58:15  csoutheren
 * Removed warnings when compling without SASL
 *
 * Revision 1.28  2004/04/28 11:26:43  csoutheren
 * Hopefully fixed SASL and SASL2 problems
 *
 * Revision 1.27  2004/04/26 01:33:20  rjongbloed
 * Fixed minor problem with SASL authentication, thanks Federico Pinna, Reitek S.p.A.
 *
 * Revision 1.26  2004/04/21 00:29:56  csoutheren
 * Added SASL authentication to PPOP3Client and PSMTPClient
 * Thanks to Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.25  2004/04/03 06:54:25  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.24  2003/02/20 00:16:06  craigs
 * Changed MIME_Version to MIME-Version
 *
 * Revision 1.23  2002/12/19 01:35:24  robertj
 * Fixed problem with returning incorrect lastWriteLength on translated output.
 *
 * Revision 1.22  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.21  2002/01/07 05:26:47  robertj
 * Fixed getting scan list of messages, thanks xradish
 *
 * Revision 1.20  2001/09/28 00:45:27  robertj
 * Removed HasKey() as is confusing due to ancestor Contains().
 *
 * Revision 1.19  2000/11/21 01:49:25  robertj
 * Fixed warning on GNU compiler.
 *
 * Revision 1.18  2000/11/16 07:15:15  robertj
 * Fixed problem with not closing off base64 encoding at next MIME part.
 *
 * Revision 1.17  2000/11/14 08:30:03  robertj
 * Fixed bug in closing SMTP client, conditional around wrong way.
 *
 * Revision 1.16  2000/11/10 01:08:11  robertj
 * Added content transfer encoding and automatic base64 translation.
 *
 * Revision 1.15  2000/11/09 06:01:58  robertj
 * Added MIME version and content disposition to RFC822 class.
 *
 * Revision 1.14  2000/11/09 05:50:23  robertj
 * Added RFC822 aware channel class for doing internet mail.
 *
 * Revision 1.13  2000/06/21 01:01:22  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.12  1998/11/30 04:52:01  robertj
 * New directory structure
 *
 * Revision 1.11  1998/09/23 06:22:18  robertj
 * Added open source copyright license.
 *
 * Revision 1.10  1998/01/26 02:49:20  robertj
 * GNU support.
 *
 * Revision 1.9  1997/07/14 11:47:14  robertj
 * Added "const" to numerous variables.
 *
 * Revision 1.8  1996/12/21 01:24:39  robertj
 * Added missing open message to smtp server.
 *
 * Revision 1.7  1996/09/14 13:18:03  robertj
 * Renamed file and changed to be a protocol off new indirect channel to separate
 *   the protocol from the low level byte transport channel.
 *
 * Revision 1.6  1996/07/27 04:12:45  robertj
 * Redesign and reimplement of mail sockets.
 *
 * Revision 1.5  1996/06/28 13:22:09  robertj
 * Changed SMTP incoming message handler so can tell when started, processing or ended message.
 *
 * Revision 1.4  1996/05/26 03:46:51  robertj
 * Compatibility to GNU 2.7.x
 *
 * Revision 1.3  1996/03/18 13:33:16  robertj
 * Fixed incompatibilities to GNU compiler where PINDEX != int.
 *
 * Revision 1.2  1996/03/16 04:51:28  robertj
 * Changed lastResponseCode to an integer.
 * Added ParseReponse() for splitting reponse line into code and info.
 *
 * Revision 1.1  1996/03/04 12:12:51  robertj
 * Initial revision
 *
 */

#ifdef __GNUC__
#pragma implementation "inetmail.h"
#endif

#include <ptlib.h>
#include <ptlib/sockets.h>
#include <ptclib/inetmail.h>
#if P_SASL2
#include <ptclib/psasl.h>
#endif

static const PString CRLF = "\r\n";
static const PString CRLFdotCRLF = "\r\n.\r\n";


#define new PNEW


//////////////////////////////////////////////////////////////////////////////
// PSMTP

static char const * const SMTPCommands[PSMTP::NumCommands] = {
  "HELO", "EHLO", "QUIT", "HELP", "NOOP",
  "TURN", "RSET", "VRFY", "EXPN", "RCPT",
  "MAIL", "SEND", "SAML", "SOML", "DATA",
  "AUTH"
};


PSMTP::PSMTP()
  : PInternetProtocol("smtp 25", NumCommands, SMTPCommands)
{
}


//////////////////////////////////////////////////////////////////////////////
// PSMTPClient

PSMTPClient::PSMTPClient()
{
  haveHello = FALSE;
  extendedHello = FALSE;
  eightBitMIME = FALSE;
}


PSMTPClient::~PSMTPClient()
{
  Close();
}


BOOL PSMTPClient::OnOpen()
{
  return ReadResponse() && lastResponseCode/100 == 2;
}


BOOL PSMTPClient::Close()
{
  BOOL ok = TRUE;

  if (sendingData)
    ok = EndMessage();

  if (IsOpen() && haveHello) {
    SetReadTimeout(60000);
    ok = ExecuteCommand(QUIT, "")/100 == 2 && ok;
  }
  return PInternetProtocol::Close() && ok;
}


#if P_SASL2
BOOL PSMTPClient::LogIn(const PString & username,
                        const PString & password)
{

  PString localHost;
  PIPSocket * socket = GetSocket();
  if (socket != NULL) {
    localHost = socket->GetLocalHostName();
  }

  if (haveHello)
    return FALSE; // Wrong state

  if (ExecuteCommand(EHLO, localHost)/100 != 2)
    return TRUE; // EHLO not supported, therefore AUTH not supported

  haveHello = extendedHello = TRUE;

  PStringArray caps = lastResponseInfo.Lines();
  PStringArray serverMechs;
  PINDEX i, max;

  for (i = 0, max = caps.GetSize() ; i < max ; i++)
    if (caps[i].Left(5) == "AUTH ") {
      serverMechs = caps[i].Mid(5).Tokenise(" ", FALSE);
      break;
    }

  if (serverMechs.GetSize() == 0)
    return TRUE; // No mechanisms, no login

  PSASLClient auth("smtp", username, username, password);
  PStringSet ourMechs;

  if (!auth.Init("", ourMechs))
    return FALSE;

  PString mech;

  for (i = 0, max = serverMechs.GetSize() ; i < max ; i++)
    if (ourMechs.Contains(serverMechs[i])) {
      mech = serverMechs[i];
      break;
    }

  if (mech.IsEmpty())
    return TRUE;  // No mechanism in common

  PString output;

  // Ok, let's go...
  if (!auth.Start(mech, output))
    return FALSE;

  if (!output.IsEmpty())
    mech = mech + " " + output;

  if (ExecuteCommand(AUTH, mech) <= 0)
    return FALSE;

  PSASLClient::PSASLResult result;
  int response;

  do {
    response = lastResponseCode/100;

    if (response == 2)
      break;
    else if (response != 3)
      return FALSE;

    result = auth.Negotiate(lastResponseInfo, output);
      
    if (result == PSASLClient::Fail)
      return FALSE;

    if (!output.IsEmpty()) {
      WriteLine(output);
      if (!ReadResponse())
        return FALSE;
    }
  } while (result == PSASLClient::Continue);
  auth.End();

  return TRUE;
}

#else

BOOL PSMTPClient::LogIn(const PString &,
                        const PString &)
{
  return TRUE;
}

#endif


BOOL PSMTPClient::BeginMessage(const PString & from,
                               const PString & to,
                               BOOL useEightBitMIME)
{
  fromAddress = from;
  toNames.RemoveAll();
  toNames.AppendString(to);
  eightBitMIME = useEightBitMIME;
  return _BeginMessage();
}


BOOL PSMTPClient::BeginMessage(const PString & from,
                               const PStringList & toList,
                               BOOL useEightBitMIME)
{
  fromAddress = from;
  toNames = toList;
  eightBitMIME = useEightBitMIME;
  return _BeginMessage();
}


BOOL PSMTPClient::_BeginMessage()
{
  PString localHost;
  PString peerHost;
  PIPSocket * socket = GetSocket();
  if (socket != NULL) {
    localHost = socket->GetLocalHostName();
    peerHost = socket->GetPeerHostName();
  }

  if (!haveHello) {
    if (ExecuteCommand(EHLO, localHost)/100 == 2)
      haveHello = extendedHello = TRUE;
  }

  if (!haveHello) {
    extendedHello = FALSE;
    if (eightBitMIME)
      return FALSE;
    if (ExecuteCommand(HELO, localHost)/100 != 2)
      return FALSE;
    haveHello = TRUE;
  }

  if (fromAddress[0] != '"' && fromAddress.Find(' ') != P_MAX_INDEX)
    fromAddress = '"' + fromAddress + '"';
  if (!localHost && fromAddress.Find('@') == P_MAX_INDEX)
    fromAddress += '@' + localHost;
  if (ExecuteCommand(MAIL, "FROM:<" + fromAddress + '>')/100 != 2)
    return FALSE;

  for (PINDEX i = 0; i < toNames.GetSize(); i++) {
    if (!peerHost && toNames[i].Find('@') == P_MAX_INDEX)
      toNames[i] += '@' + peerHost;
    if (ExecuteCommand(RCPT, "TO:<" + toNames[i] + '>')/100 != 2)
      return FALSE;
  }

  if (ExecuteCommand(DATA, PString())/100 != 3)
    return FALSE;

  stuffingState = StuffIdle;
  sendingData = TRUE;
  return TRUE;
}


BOOL PSMTPClient::EndMessage()
{
  flush();

  stuffingState = DontStuff;
  sendingData = FALSE;

  if (!WriteString(CRLFdotCRLF))
    return FALSE;

  return ReadResponse() && lastResponseCode/100 == 2;
}


//////////////////////////////////////////////////////////////////////////////
// PSMTPServer

PSMTPServer::PSMTPServer()
{
  extendedHello = FALSE;
  eightBitMIME = FALSE;
  messageBufferSize = 30000;
  ServerReset();
}


void PSMTPServer::ServerReset()
{
  eightBitMIME = FALSE;
  sendCommand = WasMAIL;
  fromAddress = PString();
  toNames.RemoveAll();
}


BOOL PSMTPServer::OnOpen()
{
  return WriteResponse(220, PIPSocket::GetHostName() + "ESMTP server ready");
}


BOOL PSMTPServer::ProcessCommand()
{
  PString args;
  PINDEX num;
  if (!ReadCommand(num, args))
    return FALSE;

  switch (num) {
    case HELO :
      OnHELO(args);
      break;
    case EHLO :
      OnEHLO(args);
      break;
    case QUIT :
      OnQUIT();
      return FALSE;
    case NOOP :
      OnNOOP();
      break;
    case TURN :
      OnTURN();
      break;
    case RSET :
      OnRSET();
      break;
    case VRFY :
      OnVRFY(args);
      break;
    case EXPN :
      OnEXPN(args);
      break;
    case RCPT :
      OnRCPT(args);
      break;
    case MAIL :
      OnMAIL(args);
      break;
    case SEND :
      OnSEND(args);
      break;
    case SAML :
      OnSAML(args);
      break;
    case SOML :
      OnSOML(args);
      break;
    case DATA :
      OnDATA();
      break;
    default :
      return OnUnknown(args);
  }

  return TRUE;
}


void PSMTPServer::OnHELO(const PCaselessString & remoteHost)
{
  extendedHello = FALSE;
  ServerReset();

  PCaselessString peerHost;
  PIPSocket * socket = GetSocket();
  if (socket != NULL)
    peerHost = socket->GetPeerHostName();

  PString response = PIPSocket::GetHostName() & "Hello" & (peerHost + ", ");

  if (remoteHost == peerHost)
    response += "pleased to meet you.";
  else if (remoteHost.IsEmpty())
    response += "why do you wish to remain anonymous?";
  else
    response += "why do you wish to call yourself \"" + remoteHost + "\"?";

  WriteResponse(250, response);
}


void PSMTPServer::OnEHLO(const PCaselessString & remoteHost)
{
  extendedHello = TRUE;
  ServerReset();

  PCaselessString peerHost;
  PIPSocket * socket = GetSocket();
  if (socket != NULL)
    peerHost = socket->GetPeerHostName();

  PString response = PIPSocket::GetHostName() & "Hello" & (peerHost + ", ");

  if (remoteHost == peerHost)
    response += ", pleased to meet you.";
  else if (remoteHost.IsEmpty())
    response += "why do you wish to remain anonymous?";
  else
    response += "why do you wish to call yourself \"" + remoteHost + "\"?";

  response += "\nHELP\nVERB\nONEX\nMULT\nEXPN\nTICK\n8BITMIME\n";
  WriteResponse(250, response);
}


void PSMTPServer::OnQUIT()
{
  WriteResponse(221, PIPSocket::GetHostName() + " closing connection, goodbye.");
  Close();
}


void PSMTPServer::OnHELP()
{
  WriteResponse(214, "No help here.");
}


void PSMTPServer::OnNOOP()
{
  WriteResponse(250, "Ok");
}


void PSMTPServer::OnTURN()
{
  WriteResponse(502, "I don't do that yet. Sorry.");
}


void PSMTPServer::OnRSET()
{
  ServerReset();
  WriteResponse(250, "Reset state.");
}


void PSMTPServer::OnVRFY(const PCaselessString & name)
{
  PString expandedName;
  switch (LookUpName(name, expandedName)) {
    case AmbiguousUser :
      WriteResponse(553, "User \"" + name + "\" ambiguous.");
      break;

    case ValidUser :
      WriteResponse(250, expandedName);
      break;

    case UnknownUser :
      WriteResponse(550, "Name \"" + name + "\" does not match anything.");
      break;

    default :
      WriteResponse(550, "Error verifying user \"" + name + "\".");
  }
}


void PSMTPServer::OnEXPN(const PCaselessString &)
{
  WriteResponse(502, "I don't do that. Sorry.");
}


static PINDEX ParseMailPath(const PCaselessString & args,
                            const PCaselessString & subCmd,
                            PString & name,
                            PString & domain,
                            PString & forwardList)
{
  PINDEX colon = args.Find(':');
  if (colon == P_MAX_INDEX)
    return 0;

  PCaselessString word = args.Left(colon).Trim();
  if (subCmd != word)
    return 0;

  PINDEX leftAngle = args.Find('<', colon);
  if (leftAngle == P_MAX_INDEX)
    return 0;

  PINDEX finishQuote;
  PINDEX startQuote = args.Find('"', leftAngle);
  if (startQuote == P_MAX_INDEX) {
    colon = args.Find(':', leftAngle);
    if (colon == P_MAX_INDEX)
      colon = leftAngle;
    finishQuote = startQuote = colon+1;
  }
  else {
    finishQuote = args.Find('"', startQuote+1);
    if (finishQuote == P_MAX_INDEX)
      finishQuote = startQuote;
    colon = args.Find(':', leftAngle);
    if (colon > startQuote)
      colon = leftAngle;
  }

  PINDEX rightAngle = args.Find('>', finishQuote);
  if (rightAngle == P_MAX_INDEX)
    return 0;

  PINDEX at = args.Find('@', finishQuote);
  if (at > rightAngle)
    at = rightAngle;

  if (startQuote == finishQuote)
    finishQuote = at;

  name = args(startQuote, finishQuote-1);
  domain = args(at+1, rightAngle-1);
  forwardList = args(leftAngle+1, colon-1);

  return rightAngle+1;
}


void PSMTPServer::OnRCPT(const PCaselessString & recipient)
{
  PCaselessString toName;
  PCaselessString toDomain;
  PCaselessString forwardList;
  if (ParseMailPath(recipient, "to", toName, toDomain, forwardList) == 0)
    WriteResponse(501, "Syntax error.");
  else {
    switch (ForwardDomain(toDomain, forwardList)) {
      case CannotForward :
        WriteResponse(550, "Cannot do forwarding.");
        break;

      case WillForward :
        if (!forwardList)
          forwardList += ":";
        forwardList += toName;
        if (!toDomain)
          forwardList += "@" + toDomain;
        toNames.AppendString(toName);
        toDomains.AppendString(forwardList);
        break;

      case LocalDomain :
      {
        PString expandedName;
        switch (LookUpName(toName, expandedName)) {
          case ValidUser :
            WriteResponse(250, "Recipient " + toName + " Ok");
            toNames.AppendString(toName);
            toDomains.AppendString("");
            break;

          case AmbiguousUser :
            WriteResponse(553, "User ambiguous.");
            break;

          case UnknownUser :
            WriteResponse(550, "User unknown.");
            break;

          default :
            WriteResponse(550, "Error verifying user.");
        }
      }
    }
  }
}


void PSMTPServer::OnMAIL(const PCaselessString & sender)
{
  sendCommand = WasMAIL;
  OnSendMail(sender);
}


void PSMTPServer::OnSEND(const PCaselessString & sender)
{
  sendCommand = WasSEND;
  OnSendMail(sender);
}


void PSMTPServer::OnSAML(const PCaselessString & sender)
{
  sendCommand = WasSAML;
  OnSendMail(sender);
}


void PSMTPServer::OnSOML(const PCaselessString & sender)
{
  sendCommand = WasSOML;
  OnSendMail(sender);
}


void PSMTPServer::OnSendMail(const PCaselessString & sender)
{
  if (!fromAddress) {
    WriteResponse(503, "Sender already specified.");
    return;
  }

  PString fromDomain;
  PINDEX extendedArgPos = ParseMailPath(sender, "from", fromAddress, fromDomain, fromPath);
  if (extendedArgPos == 0 || fromAddress.IsEmpty()) {
    WriteResponse(501, "Syntax error.");
    return;
  }
  fromAddress += fromDomain;

  if (extendedHello) {
    PINDEX equalPos = sender.Find('=', extendedArgPos);
    PCaselessString body = sender(extendedArgPos, equalPos).Trim();
    PCaselessString mime = sender.Mid(equalPos+1).Trim();
    eightBitMIME = (body == "BODY" && mime == "8BITMIME");
  }

  PString response = "Sender " + fromAddress;
  if (eightBitMIME)
    response += " and 8BITMIME";
  WriteResponse(250, response + " Ok");
}


void PSMTPServer::OnDATA()
{
  if (fromAddress.IsEmpty()) {
    WriteResponse(503, "Need a valid MAIL command.");
    return;
  }

  if (toNames.GetSize() == 0) {
    WriteResponse(503, "Need a valid RCPT command.");
    return;
  }

  // Ok, everything is ready to start the message.
  if (!WriteResponse(354,
        eightBitMIME ? "Enter 8BITMIME message, terminate with '<CR><LF>.<CR><LF>'."
                     : "Enter mail, terminate with '.' alone on a line."))
    return;

  endMIMEDetectState = eightBitMIME ? StuffIdle : DontStuff;

  BOOL ok = TRUE;
  BOOL completed = FALSE;
  BOOL starting = TRUE;

  while (ok && !completed) {
    PCharArray buffer;
    if (eightBitMIME)
      ok = OnMIMEData(buffer, completed);
    else
      ok = OnTextData(buffer, completed);
    if (ok) {
      ok = HandleMessage(buffer, starting, completed);
      starting = FALSE;
    }
  }

  if (ok)
    WriteResponse(250, "Message received Ok.");
  else
    WriteResponse(554, "Message storage failed.");
}


BOOL PSMTPServer::OnUnknown(const PCaselessString & command)
{
  WriteResponse(500, "Command \"" + command + "\" unrecognised.");
  return TRUE;
}


BOOL PSMTPServer::OnTextData(PCharArray & buffer, BOOL & completed)
{
  PString line;
  while (ReadLine(line)) {
    PINDEX len = line.GetLength();
    if (len == 1 && line[0] == '.') {
      completed = TRUE;
      return TRUE;
    }

    PINDEX start = (len > 1 && line[0] == '.' && line[1] == '.') ? 1 : 0;
    PINDEX size = buffer.GetSize();
    len -= start;
    memcpy(buffer.GetPointer(size + len + 2) + size,
           ((const char *)line)+start, len);
    size += len;
    buffer[size++] = '\r';
    buffer[size++] = '\n';
    if (size > messageBufferSize)
      return TRUE;
  }
  return FALSE;
}


BOOL PSMTPServer::OnMIMEData(PCharArray & buffer, BOOL & completed)
{
  PINDEX count = 0;
  int c;
  while ((c = ReadChar()) >= 0) {
    if (count >= buffer.GetSize())
      buffer.SetSize(count + 100);
    switch (endMIMEDetectState) {
      case StuffIdle :
        buffer[count++] = (char)c;
        break;

      case StuffCR :
        endMIMEDetectState = c != '\n' ? StuffIdle : StuffCRLF;
        buffer[count++] = (char)c;
        break;

      case StuffCRLF :
        if (c == '.')
          endMIMEDetectState = StuffCRLFdot;
        else {
          endMIMEDetectState = StuffIdle;
          buffer[count++] = (char)c;
        }
        break;

      case StuffCRLFdot :
        switch (c) {
          case '\r' :
            endMIMEDetectState = StuffCRLFdotCR;
            break;

          case '.' :
            endMIMEDetectState = StuffIdle;
            buffer[count++] = (char)c;
            break;

          default :
            endMIMEDetectState = StuffIdle;
            buffer[count++] = '.';
            buffer[count++] = (char)c;
        }
        break;

      case StuffCRLFdotCR :
        if (c == '\n') {
          completed = TRUE;
          return TRUE;
        }
        buffer[count++] = '.';
        buffer[count++] = '\r';
        buffer[count++] = (char)c;
        endMIMEDetectState = StuffIdle;

      default :
        PAssertAlways("Illegal SMTP state");
    }
    if (count > messageBufferSize) {
      buffer.SetSize(messageBufferSize);
      return TRUE;
    }
  }

  return FALSE;
}


PSMTPServer::ForwardResult PSMTPServer::ForwardDomain(PCaselessString & userDomain,
                                                      PCaselessString & forwardDomainList)
{
  return userDomain.IsEmpty() && forwardDomainList.IsEmpty() ? LocalDomain : CannotForward;
}


PSMTPServer::LookUpResult PSMTPServer::LookUpName(const PCaselessString &,
                                                  PString & expandedName)
{
  expandedName = PString();
  return LookUpError;
}


BOOL PSMTPServer::HandleMessage(PCharArray &, BOOL, BOOL)
{
  return FALSE;
}


//////////////////////////////////////////////////////////////////////////////
// PPOP3

static char const * const POP3Commands[PPOP3::NumCommands] = {
  "USER", "PASS", "QUIT", "RSET", "NOOP", "STAT",
  "LIST", "RETR", "DELE", "APOP", "TOP",  "UIDL",
  "AUTH"
};


PString PPOP3::okResponse = "+OK";
PString PPOP3::errResponse = "-ERR";


PPOP3::PPOP3()
  : PInternetProtocol("pop3 110", NumCommands, POP3Commands)
{
}


PINDEX PPOP3::ParseResponse(const PString & line)
{
  lastResponseCode = line[0] == '+';
  PINDEX endCode = line.Find(' ');
  if (endCode != P_MAX_INDEX)
    lastResponseInfo = line.Mid(endCode+1);
  else
    lastResponseInfo = PString();
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// PPOP3Client

PPOP3Client::PPOP3Client()
{
  loggedIn = FALSE;
}


PPOP3Client::~PPOP3Client()
{
  Close();
}

BOOL PPOP3Client::OnOpen()
{
  if (!ReadResponse() || lastResponseCode <= 0)
    return FALSE;

  // APOP login command supported?
  PINDEX i = lastResponseInfo.FindRegEx("<.*@.*>");

  if (i != P_MAX_INDEX)
    apopBanner = lastResponseInfo.Mid(i);

  return TRUE;
}


BOOL PPOP3Client::Close()
{
  BOOL ok = TRUE;
  if (IsOpen() && loggedIn) {
    SetReadTimeout(60000);
    ok = ExecuteCommand(QUIT, "") > 0;
  }
  return PInternetProtocol::Close() && ok;
}


BOOL PPOP3Client::LogIn(const PString & username, const PString & password, int options)
{
#if P_SASL2
  PString mech;
  PSASLClient auth("pop", username, username, password);

  if ((options & UseSASL) && ExecuteCommand(AUTH, "") > 0) {
    PStringSet serverMechs;
    while (ReadLine(mech) && mech[0] != '.')
      serverMechs.Include(mech);

    mech = PString::Empty();
    PStringSet ourMechs;

    if (auth.Init("", ourMechs)) {

      if (!(options & AllowClearTextSASL)) {
        ourMechs.Exclude("PLAIN");
        ourMechs.Exclude("LOGIN");
      }

      for (PINDEX i = 0, max = serverMechs.GetSize() ; i < max ; i++)
        if (ourMechs.Contains(serverMechs.GetKeyAt(i))) {
          mech = serverMechs.GetKeyAt(i);
          break;
        }
    }
  }

  PString output;

  if ((options & UseSASL) && !mech.IsEmpty() && auth.Start(mech, output)) {

    if (ExecuteCommand(AUTH, mech) <= 0)
      return FALSE;

    PSASLClient::PSASLResult result;

    do {
      result = auth.Negotiate(lastResponseInfo, output);
      
      if (result == PSASLClient::Fail)
        return FALSE;

      if (!output.IsEmpty()) {
        WriteLine(output);
        if (!ReadResponse() || !lastResponseCode)
          return FALSE;
      }
    } while (result == PSASLClient::Continue);
    auth.End();
  }
  else {
#endif

    if (!apopBanner.IsEmpty()) { // let's try with APOP

      PMessageDigest::Result bin_digest;
      PMessageDigest5::Encode(apopBanner + password, bin_digest);
      PString digest;

      const BYTE * data = bin_digest.GetPointer();

      for (PINDEX i = 0, max = bin_digest.GetSize(); i < max ; i++)
        digest.sprintf("%02x", (unsigned)data[i]);

      if (ExecuteCommand(APOP, username + " " + digest) > 0)
        return loggedIn = TRUE;
    }

    // No SASL and APOP didn't work for us
    // If we really have to, we'll go with the plain old USER/PASS scheme

    if (!(options & AllowUserPass))
      return FALSE;

    if (ExecuteCommand(USER, username) <= 0)
      return FALSE;

    if (ExecuteCommand(PASS, password) <= 0)
      return FALSE;

#if P_SASL2
  }
#endif

  loggedIn = TRUE;
  return TRUE;
}


int PPOP3Client::GetMessageCount()
{
  if (ExecuteCommand(STATcmd, "") <= 0)
    return -1;

  return (int)lastResponseInfo.AsInteger();
}


PUnsignedArray PPOP3Client::GetMessageSizes()
{
  PUnsignedArray sizes;

  if (ExecuteCommand(LIST, "") > 0) {
    PString msgInfo;
    while (ReadLine(msgInfo) && isdigit(msgInfo[0]))
      sizes.SetAt((PINDEX)msgInfo.AsInteger()-1,
                  (unsigned)msgInfo.Mid(msgInfo.Find(' ')).AsInteger());
  }

  return sizes;
}


PStringArray PPOP3Client::GetMessageHeaders()
{
  PStringArray headers;

  int count = GetMessageCount();
  for (int msgNum = 1; msgNum <= count; msgNum++) {
    if (ExecuteCommand(TOP, PString(PString::Unsigned,msgNum) + " 0") > 0) {
      PString headerLine;
      while (ReadLine(headerLine, TRUE))
        headers[msgNum-1] += headerLine;
    }
  }
  return headers;
}


BOOL PPOP3Client::BeginMessage(PINDEX messageNumber)
{
  return ExecuteCommand(RETR, PString(PString::Unsigned, messageNumber)) > 0;
}


BOOL PPOP3Client::DeleteMessage(PINDEX messageNumber)
{
  return ExecuteCommand(DELE, PString(PString::Unsigned, messageNumber)) > 0;
}


//////////////////////////////////////////////////////////////////////////////
// PPOP3Server

PPOP3Server::PPOP3Server()
{
}


BOOL PPOP3Server::OnOpen()
{
  return WriteResponse(okResponse, PIPSocket::GetHostName() +
                     " POP3 server ready at " +
                      PTime(PTime::MediumDateTime).AsString());
}


BOOL PPOP3Server::ProcessCommand()
{
  PString args;
  PINDEX num;
  if (!ReadCommand(num, args))
    return FALSE;

  switch (num) {
    case USER :
      OnUSER(args);
      break;
    case PASS :
      OnPASS(args);
      break;
    case QUIT :
      OnQUIT();
      return FALSE;
    case RSET :
      OnRSET();
      break;
    case NOOP :
      OnNOOP();
      break;
    case STATcmd :
      OnSTAT();
      break;
    case LIST :
      OnLIST((PINDEX)args.AsInteger());
      break;
    case RETR :
      OnRETR((PINDEX)args.AsInteger());
      break;
    case DELE :
      OnDELE((PINDEX)args.AsInteger());
      break;
    case TOP :
      if (args.Find(' ') == P_MAX_INDEX)
        WriteResponse(errResponse, "Syntax error");
      else
        OnTOP((PINDEX)args.AsInteger(),
              (PINDEX)args.Mid(args.Find(' ')).AsInteger());
      break;
    case UIDL :
      OnUIDL((PINDEX)args.AsInteger());
      break;
    default :
      return OnUnknown(args);
  }

  return TRUE;
}


void PPOP3Server::OnUSER(const PString & name)
{
  messageSizes.SetSize(0);
  messageIDs.SetSize(0);
  username = name;
  WriteResponse(okResponse, "User name accepted.");
}


void PPOP3Server::OnPASS(const PString & password)
{
  if (username.IsEmpty())
    WriteResponse(errResponse, "No user name specified.");
  else if (HandleOpenMailbox(username, password))
    WriteResponse(okResponse, username + " mail is available.");
  else
    WriteResponse(errResponse, "No access to " + username + " mail.");
  messageDeletions.SetSize(messageIDs.GetSize());
}


void PPOP3Server::OnQUIT()
{
  for (PINDEX i = 0; i < messageDeletions.GetSize(); i++)
    if (messageDeletions[i])
      HandleDeleteMessage(i+1, messageIDs[i]);

  WriteResponse(okResponse, PIPSocket::GetHostName() +
                     " POP3 server signing off at " +
                      PTime(PTime::MediumDateTime).AsString());

  Close();
}


void PPOP3Server::OnRSET()
{
  for (PINDEX i = 0; i < messageDeletions.GetSize(); i++)
    messageDeletions[i] = FALSE;
  WriteResponse(okResponse, "Resetting state.");
}


void PPOP3Server::OnNOOP()
{
  WriteResponse(okResponse, "Doing nothing.");
}


void PPOP3Server::OnSTAT()
{
  DWORD total = 0;
  for (PINDEX i = 0; i < messageSizes.GetSize(); i++)
    total += messageSizes[i];
  WriteResponse(okResponse, psprintf("%u %u", messageSizes.GetSize(), total));
}


void PPOP3Server::OnLIST(PINDEX msg)
{
  if (msg == 0) {
    WriteResponse(okResponse, psprintf("%u messages.", messageSizes.GetSize()));
    for (PINDEX i = 0; i < messageSizes.GetSize(); i++)
      if (!messageDeletions[i])
        WriteLine(psprintf("%u %u", i+1, messageSizes[i]));
    WriteLine(".");
  }
  else if (msg < 1 || msg > messageSizes.GetSize())
    WriteResponse(errResponse, "No such message.");
  else
    WriteResponse(okResponse, psprintf("%u %u", msg, messageSizes[msg-1]));
}


void PPOP3Server::OnRETR(PINDEX msg)
{
  if (msg < 1 || msg > messageDeletions.GetSize())
    WriteResponse(errResponse, "No such message.");
  else {
    WriteResponse(okResponse,
                 PString(PString::Unsigned, messageSizes[msg-1]) + " octets.");
    stuffingState = StuffIdle;
    HandleSendMessage(msg, messageIDs[msg-1], P_MAX_INDEX);
    stuffingState = DontStuff;
    WriteString(CRLFdotCRLF);
  }
}


void PPOP3Server::OnDELE(PINDEX msg)
{
  if (msg < 1 || msg > messageDeletions.GetSize())
    WriteResponse(errResponse, "No such message.");
  else {
    messageDeletions[msg-1] = TRUE;
    WriteResponse(okResponse, "Message marked for deletion.");
  }
}


void PPOP3Server::OnTOP(PINDEX msg, PINDEX count)
{
  if (msg < 1 || msg > messageDeletions.GetSize())
    WriteResponse(errResponse, "No such message.");
  else {
    WriteResponse(okResponse, "Top of message");
    stuffingState = StuffIdle;
    HandleSendMessage(msg, messageIDs[msg-1], count);
    stuffingState = DontStuff;
    WriteString(CRLFdotCRLF);
  }
}


void PPOP3Server::OnUIDL(PINDEX msg)
{
  if (msg == 0) {
    WriteResponse(okResponse,
              PString(PString::Unsigned, messageIDs.GetSize()) + " messages.");
    for (PINDEX i = 0; i < messageIDs.GetSize(); i++)
      if (!messageDeletions[i])
        WriteLine(PString(PString::Unsigned, i+1) & messageIDs[i]);
    WriteLine(".");
  }
  else if (msg < 1 || msg > messageSizes.GetSize())
    WriteResponse(errResponse, "No such message.");
  else
    WriteLine(PString(PString::Unsigned, msg) & messageIDs[msg-1]);
}


BOOL PPOP3Server::OnUnknown(const PCaselessString & command)
{
  WriteResponse(errResponse, "Command \"" + command + "\" unrecognised.");
  return TRUE;
}


BOOL PPOP3Server::HandleOpenMailbox(const PString &, const PString &)
{
  return FALSE;
}


void PPOP3Server::HandleSendMessage(PINDEX, const PString &, PINDEX)
{
}


void PPOP3Server::HandleDeleteMessage(PINDEX, const PString &)
{
}


//////////////////////////////////////////////////////////////////////////////
// PRFC822Channel

const PString & PRFC822Channel::MimeVersionTag() { static PString s = "MIME-version"; return s; }
const PString & PRFC822Channel::FromTag() { static PString s = "From"; return s; }
const PString & PRFC822Channel::ToTag() { static PString s = "To"; return s; }
const PString & PRFC822Channel::CCTag() { static PString s = "cc"; return s; }
const PString & PRFC822Channel::BCCTag() { static PString s = "bcc"; return s; }
const PString & PRFC822Channel::SubjectTag() { static PString s = "Subject"; return s; }
const PString & PRFC822Channel::DateTag() { static PString s = "Date"; return s; }
const PString & PRFC822Channel::ReturnPathTag() { static PString s = "Return-Path"; return s; }
const PString & PRFC822Channel::ReceivedTag() { static PString s = "Received"; return s; }
const PString & PRFC822Channel::MessageIDTag() { static PString s = "Message-ID"; return s; }
const PString & PRFC822Channel::MailerTag() { static PString s = "X-Mailer"; return s; }
const PString & PRFC822Channel::ContentTypeTag() { static PString s = "Content-Type"; return s; }
const PString & PRFC822Channel::ContentDispositionTag() { static PString s = "Content-Disposition"; return s; }
const PString & PRFC822Channel::ContentTransferEncodingTag() { static PString s = "Content-Transfer-Encoding"; return s; }



PRFC822Channel::PRFC822Channel(Direction direction)
{
  writeHeaders = direction == Sending;
  writePartHeaders = FALSE;
  base64 = NULL;
}


PRFC822Channel::~PRFC822Channel()
{
  Close();
  delete base64;
}


BOOL PRFC822Channel::Close()
{
  flush();
  NextPart(""); // Flush out all the parts
  return PIndirectChannel::Close();
}


BOOL PRFC822Channel::Write(const void * buf, PINDEX len)
{
  flush();

  if (writeHeaders) {
    if (!headers.Contains(FromTag()) || !headers.Contains(ToTag()))
      return FALSE;

    if (!headers.Contains(MimeVersionTag()))
      headers.SetAt(MimeVersionTag(), "1.0");

    if (!headers.Contains(DateTag()))
      headers.SetAt(DateTag(), PTime().AsString());

    if (writePartHeaders)
      headers.SetAt(ContentTypeTag(), "multipart/mixed; boundary=\""+boundaries[0]+'"');
    else if (!headers.Contains(ContentTypeTag()))
      headers.SetAt(ContentTypeTag(), "text/plain");

    PStringStream hdr;
    hdr << ::setfill('\r') << headers;
    if (!PIndirectChannel::Write(hdr.GetPointer(), hdr.GetLength()))
      return FALSE;

    if (base64 != NULL)
      base64->StartEncoding();

    writeHeaders = FALSE;
  }

  if (writePartHeaders) {
    if (!partHeaders.Contains(ContentTypeTag()))
      partHeaders.SetAt(ContentTypeTag(), "text/plain");

    PStringStream hdr;
    hdr << "\n--"  << boundaries[0] << '\n'
        << ::setfill('\r') << partHeaders;
    if (!PIndirectChannel::Write(hdr.GetPointer(), hdr.GetLength()))
      return FALSE;

    if (base64 != NULL)
      base64->StartEncoding();

    writePartHeaders = FALSE;
  }

  BOOL ok;
  if (base64 == NULL)
    ok = PIndirectChannel::Write(buf, len);
  else {
    base64->ProcessEncoding(buf, len);
    PString str = base64->GetEncodedString();
    ok = PIndirectChannel::Write(str.GetPointer(), str.GetLength());
  }

  // Always return the lastWriteCount as the number of bytes expected to be
  // written, not teh actual number which with base64 encoding etc may be
  // significantly more.
  if (ok)
    lastWriteCount = len;
  return ok;
}


BOOL PRFC822Channel::OnOpen()
{
  if (writeHeaders)
    return TRUE;

  istream & this_stream = *this;
  this_stream >> headers;
  return !bad();
}


void PRFC822Channel::NewMessage(Direction direction)
{

  NextPart(""); // Flush out all the parts

  boundaries.RemoveAll();
  headers.RemoveAll();
  partHeaders.RemoveAll();
  writeHeaders = direction == Sending;
  writePartHeaders = FALSE;
}


PString PRFC822Channel::MultipartMessage()
{
  PString boundary;

  do {
    boundary.sprintf("PWLib.%lu.%u", time(NULL), rand());
  } while (!MultipartMessage(boundary));

  return boundary;
}


BOOL PRFC822Channel::MultipartMessage(const PString & boundary)
{
  writePartHeaders = TRUE;
  for (PINDEX i = 0; i < boundaries.GetSize(); i++) {
    if (boundaries[i] == boundary)
      return FALSE;
  }

  if (boundaries.GetSize() > 0) {
    partHeaders.SetAt(ContentTypeTag(), "multipart/mixed; boundary=\""+boundary+'"');
    flush();
    writePartHeaders = TRUE;
  }

  boundaries.InsertAt(0, new PString(boundary));
  return TRUE;
}


void PRFC822Channel::NextPart(const PString & boundary)
{
  if (base64 != NULL) {
    PBase64 * oldBase64 = base64;
    base64 = NULL;
    *this << oldBase64->CompleteEncoding() << '\n';
    delete oldBase64;
  }

  while (boundaries.GetSize() > 0) {
    if (boundaries[0] == boundary)
      break;
    *this << "\n--" << boundaries[0] << "--\n";
    boundaries.RemoveAt(0);
  }

  flush();

  writePartHeaders = boundaries.GetSize() > 0;
  partHeaders.RemoveAll();
}


void PRFC822Channel::SetFromAddress(const PString & fromAddress)
{
  SetHeaderField(FromTag(), fromAddress);
}


void PRFC822Channel::SetToAddress(const PString & toAddress)
{
  SetHeaderField(ToTag(), toAddress);
}


void PRFC822Channel::SetCC(const PString & ccAddress)
{
  SetHeaderField(CCTag(), ccAddress);
}


void PRFC822Channel::SetBCC(const PString & bccAddress)
{
  SetHeaderField(BCCTag(), bccAddress);
}


void PRFC822Channel::SetSubject(const PString & subject)
{
  SetHeaderField(SubjectTag(), subject);
}


void PRFC822Channel::SetContentType(const PString & contentType)
{
  SetHeaderField(ContentTypeTag(), contentType);
}


void PRFC822Channel::SetContentAttachment(const PFilePath & file)
{
  PString name = file.GetFileName();
  SetHeaderField(ContentDispositionTag(), "attachment; filename=\"" + name + '"');
  SetHeaderField(ContentTypeTag(),
                 PMIMEInfo::GetContentType(file.GetType())+"; name=\"" + name + '"');
}


void PRFC822Channel::SetTransferEncoding(const PString & encoding, BOOL autoTranslate)
{
  SetHeaderField(ContentTransferEncodingTag(), encoding);
  if ((encoding *= "base64") && autoTranslate)
    base64 = new PBase64;
  else {
    delete base64;
    base64 = NULL;
  }
}


void PRFC822Channel::SetHeaderField(const PString & name, const PString & value)
{
  if (writePartHeaders)
    partHeaders.SetAt(name, value);
  else if (writeHeaders)
    headers.SetAt(name, value);
  else
    PAssertAlways(PLogicError);
}


BOOL PRFC822Channel::SendWithSMTP(const PString & hostname)
{
  PSMTPClient * smtp = new PSMTPClient;
  smtp->Connect(hostname);
  return SendWithSMTP(smtp);
}


BOOL PRFC822Channel::SendWithSMTP(PSMTPClient * smtp)
{
  if (!Open(smtp))
    return FALSE;

  if (!headers.Contains(FromTag()) || !headers.Contains(ToTag()))
    return FALSE;

  return smtp->BeginMessage(headers[FromTag()], headers[ToTag()]);
}


// End Of File ///////////////////////////////////////////////////////////////


