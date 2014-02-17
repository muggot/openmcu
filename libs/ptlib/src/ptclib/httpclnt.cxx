/*
 * httpclnt.cxx
 *
 * HTTP Client class.
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
 * $Log: httpclnt.cxx,v $
 * Revision 1.39  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.38  2007/04/10 05:08:48  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.37  2004/10/26 18:25:54  ykiryanov
 * Added (const char*) qualifier to url parameter, similar to one below
 *
 * Revision 1.36  2004/10/21 09:20:33  csoutheren
 * Fixed compile problems on gcc 2.95.x
 *
 * Revision 1.35  2004/08/17 15:18:30  csoutheren
 * Added support for MovedTemp/MovedPerm to GetDocument
 * Fixed problem with empty URL passed to ExecuteCommand
 *
 * Revision 1.34  2004/04/19 12:53:06  csoutheren
 * Fix for iostream changes thanks to David Parr
 *
 * Revision 1.33  2003/04/23 07:00:15  rogerh
 * Fix the encoding checking. the find_ip sample program now works again
 *
 * Revision 1.32  2003/01/28 06:48:35  robertj
 * Added https support to PHTTPClient (if #define P_SSL availbel).
 *
 * Revision 1.31  2002/12/03 22:38:35  robertj
 * Removed get document that just returns a content length as the chunked
 *   transfer encoding makes this very dangerous.
 * Added GetTextDocument() to get a URL content into a PString.
 * Added a version pf PostData() that gets the reponse content into a PString.
 * Added ReadContentBody() that takes a PString, not just PBYTEArray.
 * Fixed bug where conten-encoding must be checked even if there is a
 *   full content length MIME field.
 *
 * Revision 1.30  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.29  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.28  2002/05/28 01:41:50  robertj
 * Fixed bug in reading chunked data, thanks David Iodice
 *
 * Revision 1.27  2001/10/30 07:02:28  robertj
 * Fixed problem with bad servers causing endless loops in client.
 *
 * Revision 1.26  2001/10/03 00:26:34  robertj
 * Upgraded client to HTTP/1.1 and for chunked mode entity bodies.
 *
 * Revision 1.25  2001/09/28 08:55:15  robertj
 * More changes to support restartable PHTTPClient
 *
 * Revision 1.24  2001/09/28 00:43:47  robertj
 * Added automatic setting of some outward MIME fields.
 * Added "user agent" string field for automatic inclusion.
 * Added function to read the contents of the HTTP request.
 * Added "restarting" of connection if lost persistence.
 *
 * Revision 1.23  2001/09/11 03:27:46  robertj
 * Improved error processing on high level protocol failures, usually
 *   caused by unexpected shut down of a socket.
 *
 * Revision 1.22  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.21  2001/02/22 05:27:14  robertj
 * Added "nicer" version of GetDocument in HTTP client class.
 *
 * Revision 1.20  1999/05/13 04:59:24  robertj
 * Increased amount of buffering on output request write.
 *
 * Revision 1.19  1999/05/11 12:23:52  robertj
 * Fixed bug introduced in last revision to have arbitrary HTTP commands, missing CRLF.
 *
 * Revision 1.18  1999/05/04 15:26:01  robertj
 * Improved HTTP/1.1 compatibility (pass through user commands).
 * Fixed problems with quicktime installer.
 *
 * Revision 1.17  1998/11/30 04:51:55  robertj
 * New directory structure
 *
 * Revision 1.16  1998/09/23 06:22:09  robertj
 * Added open source copyright license.
 *
 * Revision 1.15  1998/07/24 06:57:21  robertj
 * Fixed error returned on illegal URL passed to unopened socket.
 * Changed PostData function so just has string for data instead of dictionary.
 *
 * Revision 1.14  1998/06/16 03:32:56  robertj
 * Changed TCP connection shutdown to be parameterised.
 *
 * Revision 1.13  1998/06/13 15:03:58  robertj
 * More conditions for NOT shutting down write.
 *
 * Revision 1.12  1998/06/13 12:28:04  robertj
 * Added shutdown to client command if no content length specified.
 *
 * Revision 1.11  1998/04/14 03:42:41  robertj
 * Fixed error code propagation in HTTP client.
 *
 * Revision 1.10  1998/02/03 06:27:10  robertj
 * Fixed propagation of error codes, especially EOF.
 * Fixed writing to some CGI scripts that require CRLF outside of byte count.
 *
 * Revision 1.9  1998/01/26 00:39:00  robertj
 * Added function to allow HTTPClient to automatically connect if URL has hostname.
 * Fixed incorrect return values on HTTPClient GetDocument(), Post etc functions.
 *
 * Revision 1.8  1997/06/12 12:33:35  robertj
 * Fixed bug where mising MIME fields is regarded as an eror.
 *
 * Revision 1.7  1997/03/31 08:26:58  robertj
 * GNU compiler compatibilty.
 *
 * Revision 1.6  1997/03/28 04:40:46  robertj
 * Fixed bug in Post function doing wrong command.
 *
 * Revision 1.5  1997/03/18 22:04:03  robertj
 * Fix bug for binary POST commands.
 *
 * Revision 1.4  1996/12/21 01:26:21  robertj
 * Fixed bug in persistent connections when server closes socket during command.
 *
 * Revision 1.3  1996/12/12 09:24:44  robertj
 * Persistent connection support.
 *
 * Revision 1.2  1996/10/08 13:12:03  robertj
 * Fixed bug in HTTP/0.9 response, first 5 character not put back properly.
 *
 * Revision 1.1  1996/09/14 13:02:18  robertj
 * Initial revision
 *
 * Revision 1.37  1996/08/25 09:37:41  robertj
 * Added function to detect "local" host name.
 * Fixed printing of trailing '/' in empty URL, is distinction between with and without.
 *
 * Revision 1.36  1996/08/22 13:22:26  robertj
 * Fixed bug in print of URLs, extra @ signs.
 *
 * Revision 1.35  1996/08/19 13:42:40  robertj
 * Fixed errors in URL parsing and display.
 * Fixed "Forbidden" problem out of HTTP authorisation system.
 * Fixed authorisation so if have no user/password on basic authentication, does not require it.
 *
 * Revision 1.34  1996/07/27 04:13:47  robertj
 * Fixed use of HTTP proxy on non-persistent connections.
 *
 * Revision 1.33  1996/07/15 10:37:20  robertj
 * Improved proxy "self" detection (especially localhost).
 *
 * Revision 1.32  1996/06/28 13:20:24  robertj
 * Modified HTTPAuthority so gets PHTTPReqest (mainly for URL) passed in.
 * Moved HTTP form resource to another compilation module.
 * Fixed memory leak in POST command.
 *
 * Revision 1.31  1996/06/10 10:00:00  robertj
 * Added global function for query parameters parsing.
 *
 * Revision 1.30  1996/06/07 13:52:23  robertj
 * Added PUT to HTTP proxy FTP. Necessitating redisign of entity body processing.
 *
 * Revision 1.29  1996/06/05 12:33:04  robertj
 * Fixed bug in parsing URL with no path, is NOT absolute!
 *
 * Revision 1.28  1996/05/30 10:07:26  robertj
 * Fixed bug in version number checking of return code compatibility.
 *
 * Revision 1.27  1996/05/26 03:46:42  robertj
 * Compatibility to GNU 2.7.x
 *
 * Revision 1.26  1996/05/23 10:02:13  robertj
 * Added common function for GET and HEAD commands.
 * Fixed status codes to be the actual status code instead of sequential enum.
 * This fixed some problems with proxy pass through of status codes.
 * Fixed bug in URL parsing of username and passwords.
 *
 * Revision 1.19.1.1  1996/04/17 11:08:22  craigs
 * New version by craig pending confirmation by robert
 *
 * Revision 1.19  1996/04/05 01:46:30  robertj
 * Assured PSocket::Write always writes the number of bytes specified, no longer need write loops.
 * Added workaraound for NT Netscape Navigator bug with persistent connections.
 *
 * Revision 1.18  1996/03/31 09:05:07  robertj
 * HTTP 1.1 upgrade.
 *
 * Revision 1.17  1996/03/17 05:48:07  robertj
 * Fixed host name print out of URLs.
 * Added hit count to PHTTPResource.
 *
 * Revision 1.16  1996/03/16 05:00:26  robertj
 * Added ParseReponse() for splitting reponse line into code and info.
 * Added client side support for HTTP socket.
 * Added hooks for proxy support in HTTP socket.
 * Added translation type to TranslateString() to accommodate query variables.
 * Defaulted scheme field in URL to "http".
 * Inhibited output of port field on string conversion of URL according to scheme.
 *
 * Revision 1.15  1996/03/11 10:29:50  robertj
 * Fixed bug in help image HTML.
 *
 * Revision 1.14  1996/03/10 13:15:24  robertj
 * Redesign to make resources thread safe.
 *
 * Revision 1.13  1996/03/02 03:27:37  robertj
 * Added function to translate a string to a form suitable for inclusion in a URL.
 * Added radio button and selection boxes to HTTP form resource.
 * Fixed bug in URL parsing, losing first / if hostname specified.
 *
 * Revision 1.12  1996/02/25 11:14:24  robertj
 * Radio button support for forms.
 *
 * Revision 1.11  1996/02/25 03:10:34  robertj
 * Removed pass through HTTP resource.
 * Fixed PHTTPConfig resource to use correct name for config key.
 *
 * Revision 1.10  1996/02/19 13:48:28  robertj
 * Put multiple uses of literal strings into const variables.
 * Fixed URL parsing so that the unmangling of strings occurs correctly.
 * Moved nested classes from PHTTPForm.
 * Added overwrite option to AddResource().
 * Added get/set string to PHTTPString resource.
 *
 * Revision 1.9  1996/02/13 13:09:17  robertj
 * Added extra parameters to callback function in PHTTPResources, required
 *   by descendants to make informed decisions on data being loaded.
 *
 * Revision 1.8  1996/02/08 12:26:29  robertj
 * Redesign of resource callback mechanism.
 * Added new resource types for HTML data entry forms.
 *
 * Revision 1.7  1996/02/03 11:33:19  robertj
 * Changed RadCmd() so can distinguish between I/O error and unknown command.
 *
 * Revision 1.6  1996/02/03 11:11:49  robertj
 * Numerous bug fixes.
 * Added expiry date and ismodifiedsince support.
 *
 * Revision 1.5  1996/01/30 23:32:40  robertj
 * Added single .
 *
 * Revision 1.4  1996/01/28 14:19:09  robertj
 * Split HTML into separate source file.
 * Beginning of pass through resource type.
 * Changed PCharArray in OnLoadData to PString for convenience in mangling data.
 * Made PHTTPSpace return standard page on selection of partial path.
 *
 * Revision 1.3  1996/01/28 02:49:16  robertj
 * Further implementation.
 *
 * Revision 1.2  1996/01/26 02:24:30  robertj
 * Further implemetation.
 *
 * Revision 1.1  1996/01/23 13:04:32  robertj
 * Initial revision
 *
 */

#include <ptlib.h>
#include <ptlib/sockets.h>
#include <ptclib/http.h>

#if P_SSL
#include <ptclib/pssl.h>
#endif

#include <ctype.h>


#define new PNEW


//////////////////////////////////////////////////////////////////////////////
// PHTTPClient

PHTTPClient::PHTTPClient()
{
}


PHTTPClient::PHTTPClient(const PString & userAgent)
  : userAgentName(userAgent)
{
}


int PHTTPClient::ExecuteCommand(Commands cmd,
                                const PURL & url,
                                PMIMEInfo & outMIME,
                                const PString & dataBody,
                                PMIMEInfo & replyMime,
                                BOOL persist)
{
  return ExecuteCommand(commandNames[cmd], url, outMIME, dataBody, replyMime, persist);
}


int PHTTPClient::ExecuteCommand(const PString & cmdName,
                                const PURL & url,
                                PMIMEInfo & outMIME,
                                const PString & dataBody,
                                PMIMEInfo & replyMime,
                                BOOL persist)
{
  if (!outMIME.Contains(DateTag()))
    outMIME.SetAt(DateTag(), PTime().AsString());

  if (!userAgentName && !outMIME.Contains(UserAgentTag()))
    outMIME.SetAt(UserAgentTag(), userAgentName);

  if (persist)
    outMIME.SetAt(ConnectionTag(), KeepAliveTag());

  for (PINDEX retry = 0; retry < 3; retry++) {
    if (!AssureConnect(url, outMIME))
      break;

    if (!WriteCommand(cmdName, url.AsString(PURL::URIOnly), outMIME, dataBody)) {
      lastResponseCode = -1;
      lastResponseInfo = GetErrorText(LastWriteError);
      break;
    }

    // If not persisting need to shut down write so other end stops reading
    if (!persist)
      Shutdown(ShutdownWrite);

    // Await a response, if all OK exit loop
    if (ReadResponse(replyMime))
      break;

    // If not persisting, we have no oppurtunity to write again, just error out
    if (!persist)
      break;

    // If have had a failure to read a response but there was no error then
    // we have a shutdown socket probably due to a lack of persistence so ...
    if (GetErrorCode(LastReadError) != NoError)
      break;

    // ... we close the channel and allow AssureConnet() to reopen it.
    Close();
  }

  return lastResponseCode;
}


BOOL PHTTPClient::WriteCommand(Commands cmd,
                               const PString & url,
                               PMIMEInfo & outMIME,
                               const PString & dataBody)
{
  return WriteCommand(commandNames[cmd], url, outMIME, dataBody);
}


BOOL PHTTPClient::WriteCommand(const PString & cmdName,
                               const PString & url,
                               PMIMEInfo & outMIME,
                               const PString & dataBody)
{
  ostream & this_stream = *this;
  PINDEX len = dataBody.GetSize()-1;
  if (!outMIME.Contains(ContentLengthTag()))
    outMIME.SetInteger(ContentLengthTag(), len);

  if (cmdName.IsEmpty())
    this_stream << "GET";
  else
    this_stream << cmdName;

  this_stream << ' ' << (url.IsEmpty() ? "/" :  (const char*) url) << " HTTP/1.1\r\n"
              << setfill('\r') << outMIME;

  return Write((const char *)dataBody, len);
}


BOOL PHTTPClient::ReadResponse(PMIMEInfo & replyMIME)
{
  PString http = ReadString(7);
  if (!http) {
    UnRead(http);

    if (http.Find("HTTP/") == P_MAX_INDEX) {
      lastResponseCode = PHTTP::RequestOK;
      lastResponseInfo = "HTTP/0.9";
      return TRUE;
    }

    if (http[0] == '\n')
      ReadString(1);
    else if (http[0] == '\r' &&  http[1] == '\n')
      ReadString(2);

    if (PHTTP::ReadResponse())
      if (replyMIME.Read(*this))
        return TRUE;
  }
 
  lastResponseCode = -1;
  if (GetErrorCode(LastReadError) != NoError)
    lastResponseInfo = GetErrorText(LastReadError);
  else {
    lastResponseInfo = "Premature shutdown";
    SetErrorValues(ProtocolFailure, 0, LastReadError);
  }

  return FALSE;
}


BOOL PHTTPClient::ReadContentBody(PMIMEInfo & replyMIME, PString & body)
{
  BOOL ok = InternalReadContentBody(replyMIME, body);
  body.SetSize(body.GetSize()+1);
  return ok;
}


BOOL PHTTPClient::ReadContentBody(PMIMEInfo & replyMIME, PBYTEArray & body)
{
  return InternalReadContentBody(replyMIME, body);
}


BOOL PHTTPClient::InternalReadContentBody(PMIMEInfo & replyMIME, PAbstractArray & body)
{
  PCaselessString encoding = replyMIME(TransferEncodingTag());

  if (encoding != ChunkedTag()) {
    if (replyMIME.Contains(ContentLengthTag())) {
      PINDEX length = replyMIME.GetInteger(ContentLengthTag());
      body.SetSize(length);
      return ReadBlock(body.GetPointer(), length);
    }

    if (!(encoding.IsEmpty())) {
      lastResponseCode = -1;
      lastResponseInfo = "Unknown Transfer-Encoding extension";
      return FALSE;
    }

    // Must be raw, read to end file variety
    static const PINDEX ChunkSize = 2048;
    PINDEX bytesRead = 0;
    while (ReadBlock((char *)body.GetPointer(bytesRead+ChunkSize)+bytesRead, ChunkSize))
      bytesRead += GetLastReadCount();

    body.SetSize(bytesRead + GetLastReadCount());
    return GetErrorCode(LastReadError) == NoError;
  }

  // HTTP1.1 chunked format
  PINDEX bytesRead = 0;
  for (;;) {
    // Read chunk length line
    PString chunkLengthLine;
    if (!ReadLine(chunkLengthLine))
      return FALSE;

    // A zero length chunk is end of output
    PINDEX chunkLength = chunkLengthLine.AsUnsigned(16);
    if (chunkLength == 0)
      break;

    // Read the chunk
    if (!ReadBlock((char *)body.GetPointer(bytesRead+chunkLength)+bytesRead, chunkLength))
      return FALSE;
    bytesRead+= chunkLength;

    // Read the trailing CRLF
    if (!ReadLine(chunkLengthLine))
      return FALSE;
  }

  // Read the footer
  PString footer;
  do {
    if (!ReadLine(footer))
      return FALSE;
  } while (replyMIME.AddMIME(footer));

  return TRUE;
}


BOOL PHTTPClient::GetTextDocument(const PURL & url,
                                  PString & document,
                                  BOOL persist)
{
  PMIMEInfo outMIME, replyMIME;
  if (!GetDocument(url, outMIME, replyMIME, persist))
    return FALSE;

  return ReadContentBody(replyMIME, document);
}


BOOL PHTTPClient::GetDocument(const PURL & _url,
                              PMIMEInfo & _outMIME,
                              PMIMEInfo & replyMIME,
                              BOOL persist)
{
  int count = 0;
  static const char locationTag[] = "Location";
  PURL url = _url;
  for (;;) {
    PMIMEInfo outMIME = _outMIME;
    replyMIME.RemoveAll();
    PString u = url.AsString();
    int code = ExecuteCommand(GET, url, outMIME, PString(), replyMIME, persist);
    switch (code) {
      case RequestOK:
        return TRUE;
      case MovedPermanently:
      case MovedTemporarily:
        {
          if (count > 10)
            return FALSE;
          PString str = replyMIME(locationTag);
          if (str.IsEmpty())
            return FALSE;
          PString doc;
          if (!ReadContentBody(replyMIME, doc))
            return FALSE;
          url = str;
          count++;
        }
        break;
      default:
        return FALSE;
    }
  }
}


BOOL PHTTPClient::GetHeader(const PURL & url,
                            PMIMEInfo & outMIME,
                            PMIMEInfo & replyMIME,
                            BOOL persist)
{
  return ExecuteCommand(HEAD, url, outMIME, PString(), replyMIME, persist) == RequestOK;
}


BOOL PHTTPClient::PostData(const PURL & url,
                           PMIMEInfo & outMIME,
                           const PString & data,
                           PMIMEInfo & replyMIME,
                           BOOL persist)
{
  PString dataBody = data;
  if (!outMIME.Contains(ContentTypeTag())) {
    outMIME.SetAt(ContentTypeTag(), "application/x-www-form-urlencoded");
    dataBody += "\r\n"; // Add CRLF for compatibility with some CGI servers.
  }

  return ExecuteCommand(POST, url, outMIME, data, replyMIME, persist) == RequestOK;
}


BOOL PHTTPClient::PostData(const PURL & url,
                           PMIMEInfo & outMIME,
                           const PString & data,
                           PMIMEInfo & replyMIME,
                           PString & body,
                           BOOL persist)
{
  if (!PostData(url, outMIME, data, replyMIME, persist))
    return FALSE;

  return ReadContentBody(replyMIME, body);
}


BOOL PHTTPClient::AssureConnect(const PURL & url, PMIMEInfo & outMIME)
{
  PString host = url.GetHostName();

  // Is not open or other end shut down, restablish connection
  if (!IsOpen()) {
    if (host.IsEmpty()) {
      lastResponseCode = BadRequest;
      lastResponseInfo = "No host specified";
      return SetErrorValues(ProtocolFailure, 0, LastReadError);
    }

#if P_SSL
    if (url.GetScheme() == "https") {
      PTCPSocket * tcp = new PTCPSocket(url.GetPort());
      tcp->SetReadTimeout(readTimeout);
      if (!tcp->Connect(host)) {
        lastResponseCode = -2;
        lastResponseInfo = tcp->GetErrorText();
        delete tcp;
        return FALSE;
      }

      PSSLChannel * ssl = new PSSLChannel;
      if (!ssl->Connect(tcp)) {
        lastResponseCode = -2;
        lastResponseInfo = ssl->GetErrorText();
        delete ssl;
        return FALSE;
      }

      if (!Open(ssl)) {
        lastResponseCode = -2;
        lastResponseInfo = GetErrorText();
        return FALSE;
      }
    }
    else
#endif

    if (!Connect(host, url.GetPort())) {
      lastResponseCode = -2;
      lastResponseInfo = GetErrorText();
      return FALSE;
    }
  }

  // Have connection, so fill in the required MIME fields
  static char HostTag[] = "Host";
  if (!outMIME.Contains(HostTag)) {
    if (!host)
      outMIME.SetAt(HostTag, host);
    else {
      PIPSocket * sock = GetSocket();
      if (sock != NULL)
        outMIME.SetAt(HostTag, sock->GetHostName());
    }
  }

  return TRUE;
}


// End Of File ///////////////////////////////////////////////////////////////
