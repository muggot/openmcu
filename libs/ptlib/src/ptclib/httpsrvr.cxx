/*
 * httpsrvr.cxx
 *
 * HTTP server classes.
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
 * $Log: httpsrvr.cxx,v $
 * Revision 1.50  2007/04/10 05:08:48  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.49  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.48  2005/01/15 07:53:55  csoutheren
 * Fixed problem when disabling http
 *
 * Revision 1.47  2004/02/03 09:37:20  rjongbloed
 * Added check to text files via the type extension, thanks David Parr
 *
 * Revision 1.46  2003/03/19 01:55:26  robertj
 * Fixed bugs in deleteing HTTP resources from server, thanks Diego Tártara
 *
 * Revision 1.45  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.44  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.43  2002/10/02 08:54:01  craigs
 * Added support for XMLRPC server
 *
 * Revision 1.42  2002/08/27 23:49:08  robertj
 * Fixed security hole where possible to get any file on disk when using
 *   PHTTPDirectory HTTP resource.
 *
 * Revision 1.41  2002/07/17 08:43:52  robertj
 * Fixed closing of html msg on generated post output.
 *
 * Revision 1.40  2002/05/08 05:38:54  robertj
 * Added PHTTPTailFile resource to do a unix 'tail -f' of a file.
 *
 * Revision 1.39  2002/04/12 08:15:23  robertj
 * Fixed warning on older GNU compilers, also guarantees numeric output.
 *
 * Revision 1.38  2001/10/31 01:37:13  robertj
 * Fixed deleting of object added to http name space if add fails.
 * Changes to support HTTP v1.1 chunked transfer encoding.
 *
 * Revision 1.37  2001/09/28 00:45:27  robertj
 * Removed HasKey() as is confusing due to ancestor Contains().
 *
 * Revision 1.36  2001/06/01 07:28:23  craigs
 * Added handling for binary data in multi-part MIME fields
 *
 * Revision 1.35  2001/03/14 01:49:54  craigs
 * Added ability to handle multi-part form POST commands
 *
 * Revision 1.34  2001/01/15 06:17:56  robertj
 * Set HTTP resource members to private to assure are not modified by
 *   dscendents in non-threadsafe manner.
 *
 * Revision 1.33  2000/09/04 03:57:58  robertj
 * Added ability to change the persistent connection parameters (timeout etc).
 *
 * Revision 1.32  2000/05/02 07:55:22  craigs
 * Changed static PString to static const char * to avoid "memory leak"
 *
 * Revision 1.31  1999/05/13 04:04:04  robertj
 * Fixed problem of initialised commandName in ConnectionInfo.
 *
 * Revision 1.30  1999/05/12 01:40:47  robertj
 * Fixed "unknown" response codes being passed on when used with an "unknown" command.
 *
 * Revision 1.29  1999/05/11 12:23:22  robertj
 * Fixed search for persistent connection to accept kee-alive on multile MIME fields.
 *
 * Revision 1.28  1999/05/04 15:26:01  robertj
 * Improved HTTP/1.1 compatibility (pass through user commands).
 * Fixed problems with quicktime installer.
 *
 * Revision 1.27  1999/04/24 11:50:11  robertj
 * Changed HTTP command parser so will work if some idiot puts spaces in a URL.
 *
 * Revision 1.26  1999/04/21 01:58:08  robertj
 * Fixed problem with reading data for request using second form of PHTTPRequestInfo constructor.
 *
 * Revision 1.25  1998/11/30 04:51:59  robertj
 * New directory structure
 *
 * Revision 1.24  1998/11/14 01:11:38  robertj
 * PPC linux GNU compatibility.
 *
 * Revision 1.23  1998/10/31 12:49:23  robertj
 * Added read/write mutex to the HTTP space variable to avoid thread crashes.
 *
 * Revision 1.22  1998/10/25 01:02:41  craigs
 * Added ability to specify per-directory authorisation for PHTTPDirectory
 *
 * Revision 1.21  1998/10/13 14:06:23  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.20  1998/09/23 06:22:13  robertj
 * Added open source copyright license.
 *
 * Revision 1.19  1998/08/06 00:54:22  robertj
 * Fixed bug in sending empty files, caused endless wait in Netscape.
 *
 * Revision 1.18  1998/06/16 03:32:14  robertj
 * Propagated persistence and proxy flags in new connection info instances.
 *
 * Revision 1.17  1998/04/01 01:55:16  robertj
 * Fixed bug when serving HTTPFile that has zero bytes in it.
 *
 * Revision 1.16  1998/02/03 06:24:10  robertj
 * Added local address and port to PHTTPRequest.
 * Fixed bug in default entity length. should be read to EOF.
 * Fixed OnError() so can detec HTML bosy tag with parameters.
 *
 * Revision 1.14  1998/01/26 00:42:19  robertj
 * Added more information to PHTTPConnectionInfo.
 * Made SetDefaultMIMEFields in HTTP Server not set those fields if already set.
 *
 * Revision 1.13  1997/10/30 10:22:04  robertj
 * Added multiple user basic authorisation scheme.
 *
 * Revision 1.12  1997/10/03 13:39:25  robertj
 * Fixed race condition on socket close in Select() function.
 *
 * Revision 1.12  1997/10/03 13:31:12  craigs
 * Added ability to access client socket from within HTTP resources
 *
 * Revision 1.11  1997/08/04 10:44:36  robertj
 * Improved receiving of a POST on a non-persistant connection, do not wait for EOF if have CRLF.
 *
 * Revision 1.10  1997/07/14 11:47:13  robertj
 * Added "const" to numerous variables.
 *
 * Revision 1.9  1997/07/08 13:10:26  robertj
 * Fixed bug in HTTP server where standard error text is not sent to remote client.
 *
 * Revision 1.8  1997/04/15 14:32:19  robertj
 * Fixed case problem for HTTP version string.
 *
 * Revision 1.7  1997/03/20 13:01:32  robertj
 * Fixed bug in proxy POST having unexpexted reset of connection.
 *
 * Revision 1.6  1997/02/09 04:09:30  robertj
 * Fixed GCC warning
 *
 * Revision 1.5  1997/01/12 04:15:23  robertj
 * Globalised MIME tag strings.
 *
 * Revision 1.4  1996/12/12 09:24:16  robertj
 * Persistent proxy connection support (work in progress).
 *
 * Revision 1.3  1996/11/10 21:09:33  robertj
 * Removed redundent GetSocket() call.
 * Added flush of stream after processing request, important on persistent connections.
 *
 * Revision 1.2  1996/10/26 03:31:05  robertj
 * Changed OnError so can pass in full HTML page as parameter.
 *
 * Revision 1.1  1996/09/14 13:02:18  robertj
 * Initial revision
 *
 */

#include <ptlib.h>

#ifdef P_HTTP

#include <ptlib/sockets.h>
#include <ptclib/http.h>
#include <ctype.h>

#define new PNEW


// define to enable work-around for Netscape persistant connection bug
// set to lifetime of suspect sockets (in seconds)
#define STRANGE_NETSCAPE_BUG  3

// maximum delay between characters whilst reading a line of text
#define READLINE_TIMEOUT  30

#define DEFAULT_PERSIST_TIMEOUT 30
#define DEFAULT_PERSIST_TRANSATIONS 10

//  filename to use for directory access directives
static const char * accessFilename = "_access";


//////////////////////////////////////////////////////////////////////////////
// PHTTPSpace

PHTTPSpace::PHTTPSpace()
{
  mutex = new PReadWriteMutex;
  root = new Node(PString(), NULL);
}


void PHTTPSpace::DestroyContents()
{
  delete mutex;
  delete root;
}


void PHTTPSpace::CloneContents(const PHTTPSpace * c)
{
  mutex = new PReadWriteMutex;
  root = new Node(*c->root);
}


void PHTTPSpace::CopyContents(const PHTTPSpace & c)
{
  mutex = c.mutex;
  root = c.root;
}


PHTTPSpace::Node::Node(const PString & nam, Node * parentNode)
  : PString(nam)
{
  parent = parentNode;
  resource = NULL;
}


PHTTPSpace::Node::~Node()
{
  delete resource;
}


BOOL PHTTPSpace::AddResource(PHTTPResource * res, AddOptions overwrite)
{
  PAssert(res != NULL, PInvalidParameter);
  const PStringArray & path = res->GetURL().GetPath();
  Node * node = root;
  for (PINDEX i = 0; i < path.GetSize(); i++) {
    if (path[i].IsEmpty())
      break;

    if (node->resource != NULL) {
      delete res;
      return FALSE;   // Already a resource in tree in partial path
    }

    PINDEX pos = node->children.GetValuesIndex(path[i]);
    if (pos == P_MAX_INDEX)
      pos = node->children.Append(new Node(path[i], node));

    node = &node->children[pos];
  }

  if (!node->children.IsEmpty()) {
    delete res;
    return FALSE;   // Already a resource in tree further down path.
  }

  if (overwrite == ErrorOnExist && node->resource != NULL) {
    delete res;
    return FALSE;   // Already a resource in tree at leaf
  }

  delete node->resource;
  node->resource = res;

  return TRUE;
}


BOOL PHTTPSpace::DelResource(const PURL & url)
{
  const PStringArray & path = url.GetPath();
  Node * node = root;
  for (PINDEX i = 0; i < path.GetSize(); i++) {
    if (path[i].IsEmpty())
      break;

    PINDEX pos = node->children.GetValuesIndex(path[i]);
    if (pos == P_MAX_INDEX)
      return FALSE;

    node = &node->children[pos];

    // If have resource and not last node, then trying to remove something
    // further down the tree than a leaf node.
    if (node->resource != NULL && i < (path.GetSize()-1))
      return FALSE;
  }

  if (!node->children.IsEmpty())
    return FALSE;   // Still a resource in tree further down path.

  if (node->parent != NULL) {
    do {
      Node * par = node->parent;
      par->children.Remove(node);
      node = par;
    } while (node != NULL && node->children.IsEmpty());
  }

  return TRUE;
}


static const char * const HTMLIndexFiles[] = {
  "Welcome.html", "welcome.html", "index.html",
  "Welcome.htm",  "welcome.htm",  "index.htm"
};

PHTTPResource * PHTTPSpace::FindResource(const PURL & url)
{
  const PStringArray & path = url.GetPath();

  Node * node = root;
  PINDEX i;
  for (i = 0; i < path.GetSize(); i++) {
    if (path[i].IsEmpty())
      break;

    PINDEX pos = node->children.GetValuesIndex(path[i]);
    if (pos == P_MAX_INDEX)
      return NULL;

    node = &node->children[pos];

    if (node->resource != NULL)
      return node->resource;
  }

  for (i = 0; i < PARRAYSIZE(HTMLIndexFiles); i++) {
    PINDEX pos = node->children.GetValuesIndex(PString(HTMLIndexFiles[i]));
    if (pos != P_MAX_INDEX)
      return node->children[pos].resource;
  }

  return NULL;
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPServer

PHTTPServer::PHTTPServer()
{
  Construct();
}


PHTTPServer::PHTTPServer(const PHTTPSpace & space)
  : urlSpace(space)
{
  Construct();
}


void PHTTPServer::Construct()
{
  transactionCount = 0;
  SetReadLineTimeout(PTimeInterval(0, READLINE_TIMEOUT));
}

void PHTTPConnectionInfo::DecodeMultipartFormInfo(const PString & type, const PString & entityBody)
{
  // remove trailing ","
  PINDEX pos = type.Find(",");
  if (pos == P_MAX_INDEX) {
    pos = type.Find(";");
    if (pos == P_MAX_INDEX) 
      return;
  }
  PString seperator = type.Mid(pos+1).Trim();

  // remove "boundary"
  pos = seperator.Find("boundary");
  if (pos == P_MAX_INDEX)
    return;
  seperator = seperator.Mid(8).Trim();

  // remove "="
  pos = seperator.Find("=");
  if (pos == P_MAX_INDEX)
    return;
  seperator = seperator.Mid(1).Trim();

  // seperators have a "--" according to RFC 1521
  seperator = PString("--") + seperator;

  PINDEX sepLen = seperator.GetLength();
  const char * sep = (const char *)seperator;

  // split body into parts, assuming binary data
  const char * body = (const char *)entityBody;
  PINDEX entityOffs = 0;
  PINDEX entityLen = entityBody.GetSize()-1;

  BOOL ignore = TRUE;
  BOOL last = FALSE;

  PMultipartFormInfo * info = NULL;

  while (!last && (entityOffs < entityLen)) {

    // find end of part
    PINDEX partStart = entityOffs;
    PINDEX partLen;
    BOOL foundSep = FALSE;

    // collect length of part until seperator
    for (partLen = 0; (partStart + partLen) < entityLen; partLen++) {
      if ((partLen >= sepLen) && (memcmp(body + partStart + partLen - sepLen, sep, sepLen) == 0)) {
        foundSep = TRUE;
        break;
      }
    }

    // move entity ptr to the end of the part
    entityOffs = partStart + partLen;

    // if no seperator found, then this is the last part
    // otherwise, look for "--" trailer on seperator and remove CRLF
    if (!foundSep)
      last = TRUE;
    else {
      partLen -= sepLen;

      // determine if this is the last block
      if (((entityOffs + 2) <= entityLen) && (body[entityOffs] == '-') && (body[entityOffs+1] == '-')) {
        last = TRUE;
        entityOffs += 2;
      }

      // remove crlf
      if (((entityOffs + 2) <= entityLen) && (body[entityOffs] == '\r') && (body[entityOffs+1] == '\n')) 
        entityOffs += 2;
    }

    // ignore everything up to the first seperator, 
    // then adjust seperator to include leading CRLF
    if (ignore) {
      ignore = FALSE;
      seperator = PString("\r\n") + seperator;
      sepLen = seperator.GetLength();
      sep = (const char *)seperator;
      continue;
    }

    // extract the MIME header, by looking for a double CRLF
    PINDEX ptr;
    PINDEX nlCount = 0;
    for (ptr = partStart;(ptr < (partStart + partLen)) && (nlCount < 2); ptr++) {
      if (body[ptr] == '\r') {
        nlCount++;
        if ((ptr < entityLen-1) && (body[ptr+1] == '\n'))
          ptr++;
      } else
        nlCount = 0;
    }

    // create the new part info
    info = new PMultipartFormInfo;

    // read MIME information
    PStringStream strm(PString(body + partStart, ptr - partStart));
    info->mime.ReadFrom(strm);

    // save the entity body, being careful of binary files
    int savedLen = partStart + partLen - ptr;
    char * saved = info->body.GetPointer(savedLen + 1);
    memcpy(saved, body + ptr, savedLen);
    saved[savedLen] = '\0';

    // add the data to the array
    multipartFormInfoArray.Append(info);
    info = NULL;
  }
  
#if 0
  // ignore until first separator
  do {
    data >> line;
    if (line.IsEmpty())
      return;
  } while (line.Find(sep) != 0);

  PMultipartFormInfo * info = NULL;

  // read form parts
  while (data.good() && (line.Right(2) != "--")) {

    info = new PMultipartFormInfo;

    // read MIME information
    info->mime.ReadFrom(data);

    // get the content type
    PString type = info->mime(PHTTP::ContentTypeTag);

    // check the encoding
    PString encoding = info->mime("Content-Transfer-Encoding");

    // accumulate text until another seperator or end of data
    PString & buf = info->body;
    PINDEX len = 0;
    buf.SetSize(len+1);
    buf[0] = '\0';
    PINDEX sepLen = sep.GetLength();
    const char * sepPtr = (const char *)sep;
    while (data.good()) {
      buf.SetSize(len);
      data >> buf[len++];
      if ((len >= sepLen) && (memcmp(((const char *)buf) + len - sepLen, sepPtr, sepLen) == 0)) {
        char ch;
        data >> ch;
        if (ch != 0x0d)
          data.putback(ch); 
        else {
          data >> ch;
          if (ch != 0x0a)
            data.putback(ch); 
        }
        len -= sepLen;
        break;
      }
    }
    buf.SetSize(len+1);
    buf[len] = '\0';

    /*
    while (data.good()) {
      data >> line;
      if (line.Find(sep) == 0)
        break;
      info->body += line + "\n";
    } 
    */

    multipartFormInfoArray.Append(info);
    info = NULL;
  }
#endif
}

BOOL PHTTPServer::ProcessCommand()
{
  PString args;
  PINDEX cmd;

  // if this is not the first command received by this socket, then set
  // the read timeout appropriately.
  if (transactionCount > 0) 
    SetReadTimeout(nextTimeout);

  // this will only return false upon timeout or completely invalid command
  if (!ReadCommand(cmd, args))
    return FALSE;

  connectInfo.commandCode = (Commands)cmd;
  if (cmd < NumCommands)
    connectInfo.commandName = commandNames[cmd];
  else {
    PINDEX spacePos = args.Find(' ');
    connectInfo.commandName = args.Left(spacePos);
    args = args.Mid(spacePos);
  }

  // if no tokens, error
  if (args.IsEmpty()) {
    OnError(BadRequest, args, connectInfo);
    return FALSE;
  }

  if (!connectInfo.Initialise(*this, args))
      return FALSE;

  // now that we've decided we did receive a HTTP request, increment the
  // count of transactions
  transactionCount++;
  nextTimeout = connectInfo.GetPersistenceTimeout();

  PIPSocket * socket = GetSocket();
  WORD myPort = (WORD)(socket != NULL ? socket->GetPort() : 80);

  // the URL that comes with Connect requests is not quite kosher, so 
  // mangle it into a proper URL and do NOT close the connection.
  // for all other commands, close the read connection if not persistant
  if (cmd == CONNECT) 
    connectInfo.url = "https://" + args;
  else {
    connectInfo.url = args;
    if (connectInfo.url.GetPort() == 0)
      connectInfo.url.SetPort(myPort);
  }

  BOOL persist;
  
  // make sure the form info is reset for each new operation
  connectInfo.ResetMultipartFormInfo();

  // If the incoming URL is of a proxy type then call OnProxy() which will
  // probably just go OnError(). Even if a full URL is provided in the
  // command we should check to see if it is a local server request and process
  // it anyway even though we are not a proxy. The usage of GetHostName()
  // below are to catch every way of specifying the host (name, alias, any of
  // several IP numbers etc).
  const PURL & url = connectInfo.GetURL();
  if (url.GetScheme() != "http" ||
      (url.GetPort() != 0 && url.GetPort() != myPort) ||
      (!url.GetHostName() && !PIPSocket::IsLocalHost(url.GetHostName())))
    persist = OnProxy(connectInfo);
  else {
    connectInfo.entityBody = ReadEntityBody();

    // Handle the local request
    PStringToString postData;
    switch (cmd) {
      case GET :
        persist = OnGET(url, connectInfo.GetMIME(), connectInfo);
        break;

      case HEAD :
        persist = OnHEAD(url, connectInfo.GetMIME(), connectInfo);
        break;

      case POST :
        {
          // check for multi-part form POSTs
          PString postType = (connectInfo.GetMIME())(ContentTypeTag());
          if (postType.Find("multipart/form-data") == 0)
            connectInfo.DecodeMultipartFormInfo(postType, connectInfo.entityBody);
          else  // if (postType *= "x-www-form-urlencoded)
            PURL::SplitQueryVars(connectInfo.entityBody, postData);
        }
        persist = OnPOST(url, connectInfo.GetMIME(), postData, connectInfo);
        break;

      case P_MAX_INDEX:
      default:
        persist = OnUnknown(args, connectInfo);
    }
  }

  flush();

  // if the function just indicated that the connection is to persist,
  // and so did the client, then return TRUE. Note that all of the OnXXXX
  // routines above must make sure that their return value is FALSE if
  // if there was no ContentLength field in the response. This ensures that
  // we always close the socket so the client will get the correct end of file
  if (persist && connectInfo.IsPersistant()) {
    unsigned max = connectInfo.GetPersistenceMaximumTransations();
    if (max == 0 || transactionCount < max)
      return TRUE;
  }

  PTRACE(5, "HTTPServer\tConnection end: " << connectInfo.IsPersistant());

  // close the output stream now and return FALSE
  Shutdown(ShutdownWrite);
  return FALSE;
}


PString PHTTPServer::ReadEntityBody()
{
  if (connectInfo.GetMajorVersion() < 1)
    return PString();

  PString entityBody;
  long contentLength = connectInfo.GetEntityBodyLength();
  // a content length of > 0 means read explicit length
  // a content length of < 0 means read until EOF
  // a content length of 0 means read nothing
  int count = 0;
  if (contentLength > 0) {
    entityBody = ReadString((PINDEX)contentLength);
  } else if (contentLength == -2) {
    ReadLine(entityBody, FALSE);
  } else if (contentLength < 0) {
    while (Read(entityBody.GetPointer(count+1000)+count, 1000))
      count += GetLastReadCount();
    entityBody.SetSize(count+1);
  }

  // close the connection, if not persistant
  if (!connectInfo.IsPersistant()) {
    PIPSocket * socket = GetSocket();
    if (socket != NULL)
      socket->Shutdown(PIPSocket::ShutdownRead);
  }

  return entityBody;
}


PString PHTTPServer::GetServerName() const
{
  return "PWLib-HTTP-Server/1.0 PWLib/1.0";
}


void PHTTPServer::SetURLSpace(const PHTTPSpace & space)
{
  urlSpace = space;
}


BOOL PHTTPServer::OnGET(const PURL & url,
                   const PMIMEInfo & info,
         const PHTTPConnectionInfo & connectInfo)
{
  urlSpace.StartRead();
  PHTTPResource * resource = urlSpace.FindResource(url);
  if (resource == NULL) {
    urlSpace.EndRead();
    return OnError(NotFound, url.AsString(), connectInfo);
  }

  BOOL retval = resource->OnGET(*this, url, info, connectInfo);
  urlSpace.EndRead();
  return retval;
}


BOOL PHTTPServer::OnHEAD(const PURL & url,
                    const PMIMEInfo & info,
          const PHTTPConnectionInfo & connectInfo)
{
  urlSpace.StartRead();
  PHTTPResource * resource = urlSpace.FindResource(url);
  if (resource == NULL) {
    urlSpace.EndRead();
    return OnError(NotFound, url.AsString(), connectInfo);
  }

  BOOL retval = resource->OnHEAD(*this, url, info, connectInfo);
  urlSpace.EndRead();
  return retval;
}


BOOL PHTTPServer::OnPOST(const PURL & url,
                    const PMIMEInfo & info,
              const PStringToString & data,
          const PHTTPConnectionInfo & connectInfo)
{
  urlSpace.StartRead();
  PHTTPResource * resource = urlSpace.FindResource(url);
  if (resource == NULL) {
    urlSpace.EndRead();
    return OnError(NotFound, url.AsString(), connectInfo);
  }

  BOOL retval = resource->OnPOST(*this, url, info, data, connectInfo);
  urlSpace.EndRead();
  return retval;
}


BOOL PHTTPServer::OnProxy(const PHTTPConnectionInfo & connectInfo)
{
  return OnError(BadGateway, "Proxy not implemented.", connectInfo) &&
         connectInfo.GetCommandCode() != CONNECT;
}


struct httpStatusCodeStruct {
  const char * text;
  int  code;
  BOOL allowedBody;
  int  majorVersion;
  int  minorVersion;
};

static const httpStatusCodeStruct * GetStatusCodeStruct(int code)
{
  static const httpStatusCodeStruct httpStatusDefn[] = {
    // First entry MUST be InternalServerError
    { "Internal Server Error",         PHTTP::InternalServerError, 1 },
    { "OK",                            PHTTP::RequestOK, 1 },
    { "Unauthorised",                  PHTTP::UnAuthorised, 1 },
    { "Forbidden",                     PHTTP::Forbidden, 1 },
    { "Not Found",                     PHTTP::NotFound, 1 },
    { "Not Modified",                  PHTTP::NotModified },
    { "No Content",                    PHTTP::NoContent },
    { "Bad Gateway",                   PHTTP::BadGateway, 1 },
    { "Bad Request",                   PHTTP::BadRequest, 1 },
    { "Continue",                      PHTTP::Continue, 1, 1, 1 },
    { "Switching Protocols",           PHTTP::SwitchingProtocols, 1, 1, 1 },
    { "Created",                       PHTTP::Created, 1 },
    { "Accepted",                      PHTTP::Accepted, 1 },
    { "Non-Authoritative Information", PHTTP::NonAuthoritativeInformation, 1, 1, 1 },
    { "Reset Content",                 PHTTP::ResetContent, 0, 1, 1 },
    { "Partial Content",               PHTTP::PartialContent, 1, 1, 1 },
    { "Multiple Choices",              PHTTP::MultipleChoices, 1, 1, 1 },
    { "Moved Permanently",             PHTTP::MovedPermanently, 1 },
    { "Moved Temporarily",             PHTTP::MovedTemporarily, 1 },
    { "See Other",                     PHTTP::SeeOther, 1, 1, 1 },
    { "Use Proxy",                     PHTTP::UseProxy, 1, 1, 1 },
    { "Payment Required",              PHTTP::PaymentRequired, 1, 1, 1 },
    { "Method Not Allowed",            PHTTP::MethodNotAllowed, 1, 1, 1 },
    { "None Acceptable",               PHTTP::NoneAcceptable, 1, 1, 1 },
    { "Proxy Authetication Required",  PHTTP::ProxyAuthenticationRequired, 1, 1, 1 },
    { "Request Timeout",               PHTTP::RequestTimeout, 1, 1, 1 },
    { "Conflict",                      PHTTP::Conflict, 1, 1, 1 },
    { "Gone",                          PHTTP::Gone, 1, 1, 1 },
    { "Length Required",               PHTTP::LengthRequired, 1, 1, 1 },
    { "Unless True",                   PHTTP::UnlessTrue, 1, 1, 1 },
    { "Not Implemented",               PHTTP::NotImplemented, 1 },
    { "Service Unavailable",           PHTTP::ServiceUnavailable, 1, 1, 1 },
    { "Gateway Timeout",               PHTTP::GatewayTimeout, 1, 1, 1 }
  };

  // make sure the error code is valid
  for (PINDEX i = 0; i < PARRAYSIZE(httpStatusDefn); i++)
    if (code == httpStatusDefn[i].code)
      return &httpStatusDefn[i];

  return httpStatusDefn;
}


BOOL PHTTPServer::StartResponse(StatusCode code,
                                PMIMEInfo & headers,
                                long bodySize)
{
  if (connectInfo.majorVersion < 1) 
    return FALSE;

  httpStatusCodeStruct dummyInfo;
  const httpStatusCodeStruct * statusInfo;
  if (connectInfo.commandCode < NumCommands)
    statusInfo = GetStatusCodeStruct(code);
  else {
    dummyInfo.text = "";
    dummyInfo.code = code;
    dummyInfo.allowedBody = TRUE;
    dummyInfo.majorVersion = connectInfo.majorVersion;
    dummyInfo.minorVersion = connectInfo.minorVersion;
    statusInfo = &dummyInfo;
  }

  // output the command line
  *this << "HTTP/" << connectInfo.majorVersion << '.' << connectInfo.minorVersion
        << ' ' << statusInfo->code << ' ' << statusInfo->text << "\r\n";

  BOOL chunked = FALSE;

  // If do not have user set content length, decide if we should add one
  if (!headers.Contains(ContentLengthTag())) {
    if (connectInfo.minorVersion < 1) {
      // v1.0 client, don't put in ContentLength if the bodySize is zero because
      // that can be confused by some browsers as meaning there is no body length.
      if (bodySize > 0)
        headers.SetAt(ContentLengthTag(), bodySize);
    }
    else {
      // v1.1 or later, see if will use chunked output
      chunked = bodySize == P_MAX_INDEX;
      if (chunked)
        headers.SetAt(TransferEncodingTag(), ChunkedTag());
      else if (bodySize >= 0 && bodySize < P_MAX_INDEX)
        headers.SetAt(ContentLengthTag(), bodySize);
    }
  }

  *this << setfill('\r') << headers;

#ifdef STRANGE_NETSCAPE_BUG
  // The following is a work around for a strange bug in Netscape where it
  // locks up when a persistent connection is made and data less than 1k
  // (including MIME headers) is sent. Go figure....
  if (bodySize < 1024 && connectInfo.GetMIME()(UserAgentTag()).Find("Mozilla/2.0") != P_MAX_INDEX)
    nextTimeout.SetInterval(STRANGE_NETSCAPE_BUG*1000);
#endif

  return chunked;
}


void PHTTPServer::SetDefaultMIMEInfo(PMIMEInfo & info,
                     const PHTTPConnectionInfo & connectInfo)
{
  PTime now;
  if (!info.Contains(DateTag()))
    info.SetAt(DateTag(), now.AsString(PTime::RFC1123, PTime::GMT));
  if (!info.Contains(MIMEVersionTag()))
    info.SetAt(MIMEVersionTag(), "1.0");
  if (!info.Contains(ServerTag()))
    info.SetAt(ServerTag(), GetServerName());

  if (connectInfo.IsPersistant()) {
    if (connectInfo.IsProxyConnection()) {
      PTRACE(5, "HTTPServer\tSetting proxy persistant response");
      info.SetAt(ProxyConnectionTag(), KeepAliveTag());
    }
    else {
      PTRACE(5, "HTTPServer\tSetting direct persistant response");
      info.SetAt(ConnectionTag(), KeepAliveTag());
    }
  }
}



BOOL PHTTPServer::OnUnknown(const PCaselessString & cmd, 
                        const PHTTPConnectionInfo & connectInfo)
{
  return OnError(NotImplemented, cmd, connectInfo);
}


BOOL PHTTPServer::OnError(StatusCode code,
             const PCaselessString & extra,
         const PHTTPConnectionInfo & connectInfo)
{
  const httpStatusCodeStruct * statusInfo = GetStatusCodeStruct(code);

  if (!connectInfo.IsCompatible(statusInfo->majorVersion, statusInfo->minorVersion))
    statusInfo = GetStatusCodeStruct((code/100)*100);

  PMIMEInfo headers;
  SetDefaultMIMEInfo(headers, connectInfo);

  if (!statusInfo->allowedBody) {
    StartResponse(code, headers, 0);
    return statusInfo->code == RequestOK;
  }

  PString reply;
  if (extra.Find("<body") != P_MAX_INDEX)
    reply = extra;
  else {
    PHTML html;
    html << PHTML::Title()
         << statusInfo->code
         << ' '
         << statusInfo->text
         << PHTML::Body()
         << PHTML::Heading(1)
         << statusInfo->code
         << ' '
         << statusInfo->text
         << PHTML::Heading(1)
         << extra
         << PHTML::Body();
    reply = html;
  }

  headers.SetAt(ContentTypeTag(), "text/html");
  StartResponse(code, headers, reply.GetLength());
  WriteString(reply);
  return statusInfo->code == RequestOK;
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPSimpleAuth

void PHTTPAuthority::DecodeBasicAuthority(const PString & authInfo,
                                          PString & username,
                                          PString & password)
{
  PString decoded;
  if (authInfo(0, 5) *= "Basic ")
    decoded = PBase64::Decode(authInfo(6, P_MAX_INDEX));
  else
    decoded = PBase64::Decode(authInfo);

  PINDEX colonPos = decoded.Find(':');
  if (colonPos == P_MAX_INDEX) {
    username = decoded;
    password = PString();
  }
  else {
    username = decoded.Left(colonPos).Trim();
    password = decoded.Mid(colonPos+1).Trim();
  }
}


BOOL PHTTPAuthority::IsActive() const
{
  return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPSimpleAuth

PHTTPSimpleAuth::PHTTPSimpleAuth(const PString & realm_,
                                 const PString & username_,
                                 const PString & password_)
  : realm(realm_), username(username_), password(password_)
{
  PAssert(!realm, "Must have a realm!");
}


PObject * PHTTPSimpleAuth::Clone() const
{
  return new PHTTPSimpleAuth(realm, username, password);
}


BOOL PHTTPSimpleAuth::IsActive() const
{
  return !username || !password;
}


PString PHTTPSimpleAuth::GetRealm(const PHTTPRequest &) const
{
  return realm;
}


BOOL PHTTPSimpleAuth::Validate(const PHTTPRequest &,
                               const PString & authInfo) const
{
  PString user, pass;
  DecodeBasicAuthority(authInfo, user, pass);
  return username == user && password == pass;
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPMultiSimpAuth

PHTTPMultiSimpAuth::PHTTPMultiSimpAuth(const PString & realm_)
  : realm(realm_)
{
  PAssert(!realm, "Must have a realm!");
}


PHTTPMultiSimpAuth::PHTTPMultiSimpAuth(const PString & realm_,
                                       const PStringToString & users_)
  : realm(realm_), users(users_)
{
  PAssert(!realm, "Must have a realm!");
}


PObject * PHTTPMultiSimpAuth::Clone() const
{
  return new PHTTPMultiSimpAuth(realm, users);
}


BOOL PHTTPMultiSimpAuth::IsActive() const
{
  return !users.IsEmpty();
}


PString PHTTPMultiSimpAuth::GetRealm(const PHTTPRequest &) const
{
  return realm;
}


BOOL PHTTPMultiSimpAuth::Validate(const PHTTPRequest &,
                                  const PString & authInfo) const
{
  PString user, pass;
  DecodeBasicAuthority(authInfo, user, pass);
  return users.Contains(user) && users[user] == pass;
}


void PHTTPMultiSimpAuth::AddUser(const PString & username, const PString & password)
{
  users.SetAt(username, password);
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPRequest

PHTTPRequest::PHTTPRequest(const PURL & _url,
                      const PMIMEInfo & _mime,
        const PMultipartFormInfoArray & _multipartFormInfo,
                          PHTTPServer & _server)
  : server(_server),
    url(_url),
    inMIME(_mime),
    multipartFormInfo(_multipartFormInfo),
    origin(0),
    localAddr(0),
    localPort(0)
{
  code        = PHTTP::RequestOK;
  contentSize = P_MAX_INDEX;

  PIPSocket * socket = server.GetSocket();
  if (socket != NULL) {
    socket->GetPeerAddress(origin);
    socket->GetLocalAddress(localAddr, localPort);
  }
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPConnectionInfo

PHTTPConnectionInfo::PHTTPConnectionInfo()
  : persistenceTimeout(0, DEFAULT_PERSIST_TIMEOUT) // maximum lifetime (in seconds) of persistant connections
{
  // maximum lifetime (in transactions) of persistant connections
  persistenceMaximum = DEFAULT_PERSIST_TRANSATIONS;

  commandCode       = PHTTP::NumCommands;

  majorVersion      = 0;
  minorVersion      = 9;

  isPersistant      = FALSE;
  wasPersistant     = FALSE;
  isProxyConnection = FALSE;

  entityBodyLength  = -1;

  multipartFormInfoArray.AllowDeleteObjects();
}


BOOL PHTTPConnectionInfo::Initialise(PHTTPServer & server, PString & args)
{
  // if only one argument, then it must be a version 0.9 simple request
  PINDEX lastSpacePos = args.FindLast(' ');
  static const PCaselessString httpId = "HTTP/";
  if (lastSpacePos == P_MAX_INDEX || httpId != args(lastSpacePos+1, lastSpacePos+5)) {
    majorVersion = 0;
    minorVersion = 9;
    return TRUE;
  }

  // otherwise, attempt to extract a version number
  PCaselessString verStr = args.Mid(lastSpacePos + 6);
  PINDEX dotPos = verStr.Find('.');
  if (dotPos == 0 || dotPos >= verStr.GetLength()) {
    server.OnError(PHTTP::BadRequest, "Malformed version number: " + verStr, *this);
    return FALSE;
  }

  // should actually check if the text contains only digits, but the
  // chances of matching everything else and it not being a valid number
  // are pretty small, so don't bother
  majorVersion = (int)verStr.Left(dotPos).AsInteger();
  minorVersion = (int)verStr.Mid(dotPos+1).AsInteger();
  args.Delete(lastSpacePos, P_MAX_INDEX);

  // build our connection info reading MIME info until an empty line is
  // received, or EOF
  if (!mimeInfo.Read(server))
    return FALSE;

  wasPersistant = isPersistant;
  isPersistant = FALSE;

  PString str;

  // check for Proxy-Connection and Connection strings
  isProxyConnection = mimeInfo.Contains(PHTTP::ProxyConnectionTag());
  if (isProxyConnection)
    str = mimeInfo[PHTTP::ProxyConnectionTag()];
  else if (mimeInfo.Contains(PHTTP::ConnectionTag()))
    str = mimeInfo[PHTTP::ConnectionTag()];

  // get any connection options
  if (!str) {
    PStringArray tokens = str.Tokenise(", \r\n", FALSE);
    for (PINDEX z = 0; !isPersistant && z < tokens.GetSize(); z++)
      isPersistant = isPersistant || (tokens[z] *= PHTTP::KeepAliveTag());
  }

  // If the protocol is version 1.0 or greater, there is MIME info, and the
  // prescence of a an entity body is signalled by the inclusion of
  // Content-Length header. If the protocol is less than version 1.0, then 
  // there is no entity body!

  // if the client specified a persistant connection, then use the
  // ContentLength field. If there is no content length field, then
  // assume a ContentLength of zero and close the connection.
  // The spec actually says to read until end of file in this case,
  // but Netscape hangs if this is done.
  // If the client didn't specify a persistant connection, then use the
  // ContentLength if there is one or read until end of file if there isn't
  if (!isPersistant)
    entityBodyLength = mimeInfo.GetInteger(PHTTP::ContentLengthTag(),
                                           (commandCode == PHTTP::POST) ? -2 : 0);
  else {
    entityBodyLength = mimeInfo.GetInteger(PHTTP::ContentLengthTag(), -1);
    if (entityBodyLength < 0) {
      PTRACE(5, "HTTPServer\tPersistant connection has no content length");
      entityBodyLength = 0;
      mimeInfo.SetAt(PHTTP::ContentLengthTag(), "0");
    }
  }

  return TRUE;
}


void PHTTPConnectionInfo::SetMIME(const PString & tag, const PString & value)
{
  mimeInfo.MakeUnique();
  mimeInfo.SetAt(tag, value);
}


BOOL PHTTPConnectionInfo::IsCompatible(int major, int minor) const
{
  if (minor == 0 && major == 0)
    return TRUE;
  else
    return (majorVersion > major) ||
           ((majorVersion == major) && (minorVersion >= minor));
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPResource

PHTTPResource::PHTTPResource(const PURL & url)
  : baseURL(url)
{
  authority = NULL;
  hitCount = 0;
}


PHTTPResource::PHTTPResource(const PURL & url, const PHTTPAuthority & auth)
  : baseURL(url)
{
  authority = (PHTTPAuthority *)auth.Clone();
  hitCount = 0;
}


PHTTPResource::PHTTPResource(const PURL & url, const PString & type)
  : baseURL(url), contentType(type)
{
  authority = NULL;
  hitCount = 0;
}


PHTTPResource::PHTTPResource(const PURL & url,
                             const PString & type,
                             const PHTTPAuthority & auth)
  : baseURL(url), contentType(type)
{
  authority = (PHTTPAuthority *)auth.Clone();
  hitCount = 0;
}


PHTTPResource::~PHTTPResource()
{
  delete authority;
}


BOOL PHTTPResource::OnGET(PHTTPServer & server,
                           const PURL & url,
                      const PMIMEInfo & info,
            const PHTTPConnectionInfo & connectInfo)
{
  return OnGETOrHEAD(server, url, info, connectInfo, TRUE);
}


BOOL PHTTPResource::OnHEAD(PHTTPServer & server,
                           const PURL & url,
                      const PMIMEInfo & info,
            const PHTTPConnectionInfo & connectInfo)
{
  return OnGETOrHEAD(server, url, info, connectInfo, FALSE);
}


BOOL PHTTPResource::OnGETOrHEAD(PHTTPServer & server,
                           const PURL & url,
                      const PMIMEInfo & info,
            const PHTTPConnectionInfo & connectInfo,
                                   BOOL isGET)
{
  // Nede to split songle if into 2 so the Tornado compiler won't end with
  // 'internal compiler error'
  if (isGET && info.Contains(PHTTP::IfModifiedSinceTag()))
    if (!IsModifiedSince(PTime(info[PHTTP::IfModifiedSinceTag()])))
      return server.OnError(PHTTP::NotModified, url.AsString(), connectInfo);

  PHTTPRequest * request = CreateRequest(url,
                                         info,
                                         connectInfo.GetMultipartFormInfo(),
                                         server);

  BOOL retVal = TRUE;
  if (CheckAuthority(server, *request, connectInfo)) {
    retVal = FALSE;
    server.SetDefaultMIMEInfo(request->outMIME, connectInfo);

    PTime expiryDate;
    if (GetExpirationDate(expiryDate))
      request->outMIME.SetAt(PHTTP::ExpiresTag(),
                              expiryDate.AsString(PTime::RFC1123, PTime::GMT));

    if (!LoadHeaders(*request)) 
      retVal = server.OnError(request->code, url.AsString(), connectInfo);
    else if (!isGET)
      retVal = request->outMIME.Contains(PHTTP::ContentLengthTag());
    else {
      hitCount++;
      retVal = OnGETData(server, url, connectInfo, *request);
    }
  }

  delete request;
  return retVal;
}


BOOL PHTTPResource::OnGETData(PHTTPServer & /*server*/,
                               const PURL & /*url*/,
                const PHTTPConnectionInfo & /*connectInfo*/,
                             PHTTPRequest & request)
{
  SendData(request);
  return request.outMIME.Contains(PHTTP::ContentLengthTag()) ||
         request.outMIME.Contains(PHTTP::TransferEncodingTag());
}


BOOL PHTTPResource::OnPOST(PHTTPServer & server,
                            const PURL & url,
                       const PMIMEInfo & info,
                 const PStringToString & data,
             const PHTTPConnectionInfo & connectInfo)
{
  PHTTPRequest * request = CreateRequest(url,
                                         info,
                                         connectInfo.GetMultipartFormInfo(),
                                         server);

  request->entityBody = connectInfo.GetEntityBody();

  BOOL persist = TRUE;
  if (CheckAuthority(server, *request, connectInfo)) {
    server.SetDefaultMIMEInfo(request->outMIME, connectInfo);
    persist = OnPOSTData(*request, data);
    if (request->code != PHTTP::RequestOK)
      persist = server.OnError(request->code, "", connectInfo) && persist;
  }

  delete request;
  return persist;
}


BOOL PHTTPResource::OnPOSTData(PHTTPRequest & request,
                               const PStringToString & data)
{
  PHTML msg;
  BOOL persist = Post(request, data, msg);

  if (msg.Is(PHTML::InBody))
    msg << PHTML::Body();

  if (request.code != PHTTP::RequestOK)
    return persist;

  if (msg.IsEmpty())
    msg << PHTML::Title()    << (unsigned)PHTTP::RequestOK << " OK" << PHTML::Body()
        << PHTML::Heading(1) << (unsigned)PHTTP::RequestOK << " OK" << PHTML::Heading(1)
        << PHTML::Body();

  request.outMIME.SetAt(PHTTP::ContentTypeTag(), "text/html");

  PINDEX len = msg.GetLength();
  request.server.StartResponse(request.code, request.outMIME, len);
  return request.server.Write((const char *)msg, len) && persist;
}


BOOL PHTTPResource::CheckAuthority(PHTTPServer & server,
                            const PHTTPRequest & request,
                     const PHTTPConnectionInfo & connectInfo)
{
  if (authority == NULL)
    return TRUE;

  return CheckAuthority(*authority, server, request, connectInfo);
}
    
    
BOOL PHTTPResource::CheckAuthority(PHTTPAuthority & authority,
                                      PHTTPServer & server,
                               const PHTTPRequest & request,
                        const PHTTPConnectionInfo & connectInfo)
{
  if (!authority.IsActive())
    return TRUE;


  // if this is an authorisation request...
  if (request.inMIME.Contains(PHTTP::AuthorizationTag()) &&
      authority.Validate(request, request.inMIME[PHTTP::AuthorizationTag()]))
    return TRUE;

  // it must be a request for authorisation
  PMIMEInfo headers;
  server.SetDefaultMIMEInfo(headers, connectInfo);
  headers.SetAt(PHTTP::WWWAuthenticateTag(),
                       "Basic realm=\"" + authority.GetRealm(request) + "\"");
  headers.SetAt(PHTTP::ContentTypeTag(), "text/html");

  const httpStatusCodeStruct * statusInfo =
                               GetStatusCodeStruct(PHTTP::UnAuthorised);

  PHTML reply;
  reply << PHTML::Title()
        << statusInfo->code
        << ' '
        << statusInfo->text
        << PHTML::Body()
        << PHTML::Heading(1)
        << statusInfo->code
        << ' '
        << statusInfo->text
        << PHTML::Heading(1)
        << "Your request cannot be authorised because it requires authentication."
        << PHTML::Paragraph()
        << "This may be because you entered an incorrect username or password, "
        << "or because your browser is not performing Basic authentication."
        << PHTML::Body();

  server.StartResponse(PHTTP::UnAuthorised, headers, reply.GetLength());
  server.WriteString(reply);

  return FALSE;
}


void PHTTPResource::SetAuthority(const PHTTPAuthority & auth)
{
  delete authority;
  authority = (PHTTPAuthority *)auth.Clone();
}


void PHTTPResource::ClearAuthority()
{
  delete authority;
  authority = NULL;
}


BOOL PHTTPResource::IsModifiedSince(const PTime &)
{
  return TRUE;
}


BOOL PHTTPResource::GetExpirationDate(PTime &)
{
  return FALSE;
}


PHTTPRequest * PHTTPResource::CreateRequest(const PURL & url,
                                            const PMIMEInfo & inMIME,
                                            const PMultipartFormInfoArray & multipartFormInfo,
                                            PHTTPServer & socket)
{
  return new PHTTPRequest(url, inMIME, multipartFormInfo, socket);
}


static void WriteChunkedDataToServer(PHTTPServer & server, PCharArray & data)
{
  if (data.GetSize() == 0)
    return;

  server << data.GetSize() << "\r\n";
  server.Write(data, data.GetSize());
  server << "\r\n";
  data.SetSize(0);
}


void PHTTPResource::SendData(PHTTPRequest & request)
{
  if (!request.outMIME.Contains(PHTTP::ContentTypeTag()) && !contentType)
    request.outMIME.SetAt(PHTTP::ContentTypeTag(), contentType);

  PCharArray data;
  if (LoadData(request, data)) {
    if (request.server.StartResponse(request.code, request.outMIME, request.contentSize)) {
      // Chunked transfer encoding
      request.outMIME.RemoveAll();
      do {
        WriteChunkedDataToServer(request.server, data);
      } while (LoadData(request, data));
      WriteChunkedDataToServer(request.server, data);
      request.server << "0\r\n" << request.outMIME;
    }
    else {
      do {
        request.server.Write(data, data.GetSize());
        data.SetSize(0);
      } while (LoadData(request, data));
      request.server.Write(data, data.GetSize());
    }
  }
  else {
    request.server.StartResponse(request.code, request.outMIME, data.GetSize());
    request.server.Write(data, data.GetSize());
  }
}


BOOL PHTTPResource::LoadData(PHTTPRequest & request, PCharArray & data)
{
  PString text = LoadText(request);
  OnLoadedText(request, text);
  text.SetSize(text.GetLength());  // Lose the trailing '\0'
  data = text;
  return FALSE;
}


PString PHTTPResource::LoadText(PHTTPRequest &)
{
  PAssertAlways(PUnimplementedFunction);
  return PString();
}


void PHTTPResource::OnLoadedText(PHTTPRequest &, PString &)
{
  // Do nothing
}


BOOL PHTTPResource::Post(PHTTPRequest & request,
                         const PStringToString &,
                         PHTML & msg)
{
  request.code = PHTTP::MethodNotAllowed;
  msg = "Error in POST";
  msg << "Post to this resource is not allowed" << PHTML::Body();
  return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPString

PHTTPString::PHTTPString(const PURL & url)
  : PHTTPResource(url, "text/html")
{
}


PHTTPString::PHTTPString(const PURL & url,
                         const PHTTPAuthority & auth)
  : PHTTPResource(url, "text/html", auth)
{
}


PHTTPString::PHTTPString(const PURL & url, const PString & str)
  : PHTTPResource(url, "text/html"), string(str)
{
}


PHTTPString::PHTTPString(const PURL & url,
                         const PString & str,
                         const PString & type)
  : PHTTPResource(url, type), string(str)
{
}


PHTTPString::PHTTPString(const PURL & url,
                         const PString & str,
                         const PHTTPAuthority & auth)
  : PHTTPResource(url, "text/html", auth), string(str)
{
}


PHTTPString::PHTTPString(const PURL & url,
                         const PString & str,
                         const PString & type,
                         const PHTTPAuthority & auth)
  : PHTTPResource(url, type, auth), string(str)
{
}


BOOL PHTTPString::LoadHeaders(PHTTPRequest & request)
{
  request.contentSize = string.GetLength();
  return TRUE;
}


PString PHTTPString::LoadText(PHTTPRequest &)
{
  return string;
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPFile

PHTTPFile::PHTTPFile(const PURL & url, int)
  : PHTTPResource(url)
{
}


PHTTPFile::PHTTPFile(const PString & filename)
  : PHTTPResource(filename, PMIMEInfo::GetContentType(PFilePath(filename).GetType())),
    filePath(filename)
{
}


PHTTPFile::PHTTPFile(const PString & filename, const PHTTPAuthority & auth)
  : PHTTPResource(filename, auth), filePath(filename)
{
}


PHTTPFile::PHTTPFile(const PURL & url, const PFilePath & path)
  : PHTTPResource(url, PMIMEInfo::GetContentType(path.GetType())),
    filePath(path)
{
}


PHTTPFile::PHTTPFile(const PURL & url,
                     const PFilePath & path,
                     const PString & type)
  : PHTTPResource(url, type), filePath(path)
{
}


PHTTPFile::PHTTPFile(const PURL & url,
                     const PFilePath & path,
                     const PHTTPAuthority & auth)
  : PHTTPResource(url, PMIMEInfo::GetContentType(path.GetType()), auth),
    filePath(path)
{
}


PHTTPFile::PHTTPFile(const PURL & url,
                     const PFilePath & path,
                     const PString & type,
                     const PHTTPAuthority & auth)
  : PHTTPResource(url, type, auth), filePath(path)
{
}


PHTTPFileRequest::PHTTPFileRequest(const PURL & url,
                                   const PMIMEInfo & inMIME,
                                   const PMultipartFormInfoArray & multipartFormInfo,
                                   PHTTPServer & server)
  : PHTTPRequest(url, inMIME, multipartFormInfo, server)
{
}


PHTTPRequest * PHTTPFile::CreateRequest(const PURL & url,
                                        const PMIMEInfo & inMIME,
                          const PMultipartFormInfoArray & multipartFormInfo,
                PHTTPServer & server)
{
  return new PHTTPFileRequest(url, inMIME, multipartFormInfo, server);
}


BOOL PHTTPFile::LoadHeaders(PHTTPRequest & request)
{
  PFile & file = ((PHTTPFileRequest&)request).file;

  if (!file.Open(filePath, PFile::ReadOnly)) {
    request.code = PHTTP::NotFound;
    return FALSE;
  }

  request.contentSize = file.GetLength();
  return TRUE;
}


BOOL PHTTPFile::LoadData(PHTTPRequest & request, PCharArray & data)
{
  PFile & file = ((PHTTPFileRequest&)request).file;

  PString contentType = GetContentType();
  if (contentType.IsEmpty())
    contentType = PMIMEInfo::GetContentType(file.GetFilePath().GetType());

  if (contentType(0, 4) *= "text/")
    return PHTTPResource::LoadData(request, data);

  PAssert(file.IsOpen(), PLogicError);

  PINDEX count = file.GetLength() - file.GetPosition();
  if (count > 10000)
    count = 10000;

  if (count > 0)
    PAssert(file.Read(data.GetPointer(count), count), PLogicError);

  if (!file.IsEndOfFile())
    return TRUE;

  file.Close();
  return FALSE;
}


PString PHTTPFile::LoadText(PHTTPRequest & request)
{
  PFile & file = ((PHTTPFileRequest&)request).file;
  PAssert(file.IsOpen(), PLogicError);
  PINDEX count = file.GetLength();
  PString text;
  if (count > 0)
    PAssert(file.Read(text.GetPointer(count+1), count), PLogicError);
  PAssert(file.Close(), PLogicError);
  return text;
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPTailFile

PHTTPTailFile::PHTTPTailFile(const PString & filename)
  : PHTTPFile(filename)
{
}


PHTTPTailFile::PHTTPTailFile(const PString & filename,
                             const PHTTPAuthority & auth)
  : PHTTPFile(filename, auth)
{
}


PHTTPTailFile::PHTTPTailFile(const PURL & url,
                             const PFilePath & file)
  : PHTTPFile(url, file)
{
}


PHTTPTailFile::PHTTPTailFile(const PURL & url,
                             const PFilePath & file,
                             const PString & contentType)
  : PHTTPFile(url, file, contentType)
{
}


PHTTPTailFile::PHTTPTailFile(const PURL & url,
                             const PFilePath & file,
                             const PHTTPAuthority & auth)
  : PHTTPFile(url, file, auth)
{
}


PHTTPTailFile::PHTTPTailFile(const PURL & url,
                             const PFilePath & file,
                             const PString & contentType,
                             const PHTTPAuthority & auth)
  : PHTTPFile(url, file, contentType, auth)
{
}


BOOL PHTTPTailFile::LoadHeaders(PHTTPRequest & request)
{
  if (!PHTTPFile::LoadHeaders(request))
    return FALSE;

  request.contentSize = P_MAX_INDEX;
  return TRUE;
}


BOOL PHTTPTailFile::LoadData(PHTTPRequest & request, PCharArray & data)
{
  PFile & file = ((PHTTPFileRequest&)request).file;

  if (file.GetPosition() == 0)
    file.SetPosition(file.GetLength()-request.url.GetQueryVars()("offset", "10000").AsUnsigned());

  while (file.GetPosition() >= file.GetLength()) {
    if (!request.server.Write(NULL, 0))
      return FALSE;
    PThread::Sleep(200);
  }

  PINDEX count = file.GetLength() - file.GetPosition();
  return file.Read(data.GetPointer(count), count);
}


//////////////////////////////////////////////////////////////////////////////
// PHTTPDirectory

PHTTPDirectory::PHTTPDirectory(const PURL & url, const PDirectory & dir)
  : PHTTPFile(url, 0), basePath(dir), allowDirectoryListing(TRUE)
{
}


PHTTPDirectory::PHTTPDirectory(const PURL & url,
                               const PDirectory & dir,
                               const PHTTPAuthority & auth)
  : PHTTPFile(url, PString(), auth), basePath(dir), allowDirectoryListing(TRUE)
{
}


PHTTPDirRequest::PHTTPDirRequest(const PURL & url,
                                 const PMIMEInfo & inMIME,
                                 const PMultipartFormInfoArray & multipartFormInfo,
                                 PHTTPServer & server)
  : PHTTPFileRequest(url, inMIME, multipartFormInfo, server)
{
}


PHTTPRequest * PHTTPDirectory::CreateRequest(const PURL & url,
                                        const PMIMEInfo & inMIME,
                          const PMultipartFormInfoArray & multipartFormInfo,
                          PHTTPServer & socket)
{
  PHTTPDirRequest * request = new PHTTPDirRequest(url, inMIME, multipartFormInfo, socket);

  const PStringArray & path = url.GetPath();
  request->realPath = basePath;
  PINDEX i;
  for (i = GetURL().GetPath().GetSize(); i < path.GetSize()-1; i++)
    request->realPath += path[i] + PDIR_SEPARATOR;

  // append the last path element
  if (i < path.GetSize())
    request->realPath += path[i];

  if (request->realPath.Find(basePath) != 0)
    request->realPath = basePath;

  return request;
}


void PHTTPDirectory::EnableAuthorisation(const PString & realm)
{
  authorisationRealm = realm;
}


BOOL PHTTPDirectory::FindAuthorisations(const PDirectory & dir, PString & realm, PStringToString & authorisations)
{
  PFilePath fn = dir + accessFilename;
  PTextFile file;
  BOOL first = TRUE;
  if (file.Open(fn, PFile::ReadOnly)) {
    PString line;
    while (file.ReadLine(line)) {
      if (first) {
        realm = line.Trim();
        first = FALSE;
      } else {
        PStringArray tokens = line.Tokenise(':');
        if (tokens.GetSize() > 1)
          authorisations.SetAt(tokens[0].Trim(), tokens[1].Trim());
      }
    }
    return TRUE;
  }
    
  if (dir.IsRoot() || (dir == basePath))
    return FALSE;

  return FindAuthorisations(dir.GetParent(), realm, authorisations);
}

BOOL PHTTPDirectory::CheckAuthority(PHTTPServer & server,
                             const PHTTPRequest & request,
                      const PHTTPConnectionInfo & conInfo)
{
  // if access control is enabled, then search parent directories for password files
  PStringToString authorisations;
  PString newRealm;
  if (authorisationRealm.IsEmpty() ||
      !FindAuthorisations(((PHTTPDirRequest&)request).realPath.GetDirectory(), newRealm, authorisations) ||
      authorisations.GetSize() == 0)
    return TRUE;

  PHTTPMultiSimpAuth authority(newRealm, authorisations);
  return PHTTPResource::CheckAuthority(authority, server, request, conInfo);
}

BOOL PHTTPDirectory::LoadHeaders(PHTTPRequest & request)
{
  PFilePath & realPath = ((PHTTPDirRequest&)request).realPath;
    
  // if not able to obtain resource information, then consider the resource "not found"
  PFileInfo info;
  if (!PFile::GetInfo(realPath, info)) {
    request.code = PHTTP::NotFound;
    return FALSE;
  }

  // if the resource is a file, and the file can't be opened, then return "not found"
  PFile & file = ((PHTTPDirRequest&)request).file;
  if (info.type != PFileInfo::SubDirectory) {
    if (!file.Open(realPath, PFile::ReadOnly) ||
        (!authorisationRealm.IsEmpty() && realPath.GetFileName() == accessFilename)) {
      request.code = PHTTP::NotFound;
      return FALSE;
    }
  } 

  // resource is a directory - if index files disabled, then return "not found"
  else if (!allowDirectoryListing) {
    request.code = PHTTP::NotFound;
    return FALSE;
  }

  // else look for index files
  else {
    PINDEX i;
    for (i = 0; i < PARRAYSIZE(HTMLIndexFiles); i++)
      if (file.Open(realPath +
                          PDIR_SEPARATOR + HTMLIndexFiles[i], PFile::ReadOnly))
        break;
  }

  // open the file and return information
  PString & fakeIndex = ((PHTTPDirRequest&)request).fakeIndex;
  if (file.IsOpen()) {
    request.outMIME.SetAt(PHTTP::ContentTypeTag(),
                          PMIMEInfo::GetContentType(file.GetFilePath().GetType()));
    request.contentSize = file.GetLength();
    fakeIndex = PString();
    return TRUE;
  }

  // construct a directory listing
  request.outMIME.SetAt(PHTTP::ContentTypeTag(), "text/html");
  PHTML reply("Directory of " + request.url.AsString());
  PDirectory dir = realPath;
  if (dir.Open()) {
    do {
      const char * imgName;
      if (dir.IsSubDir())
        imgName = "internal-gopher-menu";
      else if (PMIMEInfo::GetContentType(
                    PFilePath(dir.GetEntryName()).GetType())(0,4) == "text/")
        imgName = "internal-gopher-text";
      else
        imgName = "internal-gopher-unknown";
      reply << PHTML::Image(imgName) << ' '
            << PHTML::HotLink(realPath.GetFileName()+'/'+dir.GetEntryName())
            << dir.GetEntryName()
            << PHTML::HotLink()
            << PHTML::BreakLine();
    } while (dir.Next());
  }
  reply << PHTML::Body();
  fakeIndex = reply;

  return TRUE;
}


PString PHTTPDirectory::LoadText(PHTTPRequest & request)
{
  PString & fakeIndex = ((PHTTPDirRequest&)request).fakeIndex;
  if (fakeIndex.IsEmpty())
    return PHTTPFile::LoadText(request);

  return fakeIndex;
}

#endif // P_HTTP


// End Of File ///////////////////////////////////////////////////////////////
